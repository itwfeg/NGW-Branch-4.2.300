#ifndef SQLITEQUERY_H
#define SQLITEQUERY_H


#include "SessionManagerHelper.h"


#include <string>


#include "edb.h"
#include DBQUERYI_INC
#include "sqliteview.h"

#include DBEXCEPTION_INC
#include UTILSTRING_INC
#include TIME_INC
#include DEBUG_INC

#include <vector>

#define MAX_LOCK_COUNT 100


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	template <class SqliteEntity>
	class SqliteQuery : public DBQueryI<typename SqliteEntity::interface_type>, public SqliteView
	{
		public:

			// --- Types and subtypes ---
			typedef typename SqliteEntity::interface_type DBEntityI;
			typedef typename DBQueryI<DBEntityI>::size_type size_type;

			// --- Constructors and destructor ---
			SqliteQuery(const SqliteView& view, DBQueryI<DBEntityI>* d = 0);
			~SqliteQuery();


			// --- Processing methods ---
			// Methods from DBQueryI
			DBEntityI* first();
			//DBEntityI* next();
			DBEntityI* next(const DBEntityI* current = 0);  // For Fastrack, passing in a current record from which to do next is no longer supported, any 'current' passed in is ignored!
			//DBEntityI* previous();
			DBEntityI* previous(const DBEntityI* current = 0);  // For Fastrack, passing in a current record from which to do previous is no longer supported, any 'current' passed in is ignored!
			DBEntityI* last();
			DBEntityI* find(const DBEntityI* current);

			size_type size();
			bool empty();

			void show();

			// Other methods
			bool selectAll(const std::string& sortOrder = "");
			bool selectSome(const std::string& filter, const std::string& sortOrder = "");
			bool selectSome(const std::string& filter, const std::string& sortOrder, int limit);
			bool selectSomeIndexed(const std::string& filter, const std::string& sortOrder = "", bool defaultOrder = true, bool allowReverse = true);
			bool selectUsingQuery(const std::string& query);

			void setPreloadQueryRecordKeys(bool preload) { preloadQueryRecordKeys_ = preload; }


		private:

			DBQueryI<DBEntityI>* decorated_;
			std::string sortOrder_;			// used for 'order by'?
			int limit_;						// used for 'limit' clause?
			bool defaultOrder_;
			bool doQueries_;

			std::string sql_forward_;
			std::string sql_reverse_;
			std::string sql_count_;

			sqlite3_stmt *stmt_forward_;
			sqlite3_stmt *stmt_reverse_;

			int recordCounter_forward_;
			int recordCounter_reverse_;

			int mostRecentAccessedQuery_;  // -1 = the reverse query, 1 = the forward query, 0 = no query accessed yet


			bool preloadQueryRecordKeys_;  // this will be set to true by default (via the constructor)

			std::string sql_preload_;

			// This vector will hold the keys (that can be used in a where clause to retrieve the record from the database),
			// since we don't have a cursor to traverse the query without holding shared locks on database records for the
			// life of any sqlite3_stmt pointer, in which those shared locks would prevent others from writing to the database.
			std::vector<std::string> queryRecordKeys_;

			void preloadQuery();
			SqliteRecord* findPreloadedRecord(const std::string& recordIdentifier);


			//std::vector<typename SqliteEntity> queryResults;
			//int currentVectorOffset;
			//using std::vector<typename SqliteEntity>::iterator queryIter;


			// Do not allow the default copy constructor.
			SqliteQuery(const SqliteQuery& ps);
			// Do not allow the default assignment operator.
			SqliteQuery operator=(const SqliteQuery& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif





#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace util_time;
	using namespace util_string;
	using namespace std;
#endif



template <class SqliteEntity>
SqliteQuery<SqliteEntity>::SqliteQuery(const SqliteView& view, DBQueryI<DBEntityI>* d)
			: SqliteView(view), decorated_(d), sortOrder_(), limit_(0), defaultOrder_(true), doQueries_(true),
			  sql_forward_(""), sql_reverse_(""), sql_count_(""), stmt_forward_(0), stmt_reverse_(0),
			  recordCounter_forward_(0), recordCounter_reverse_(0), mostRecentAccessedQuery_(0),
			  preloadQueryRecordKeys_(true), queryRecordKeys_()
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

#ifndef EVOL_NDEBUG
	Time t0 = Time();
#endif

	const std::string* property = getTableProperty("doNothing");
	if ( property )
	{
		if ( stringtobool(*property) )
			doQueries_ = false;
	}

	property = getTableProperty("doQueries");
	if ( property )
		doQueries_ = stringtobool(*property);

	string dbgHeader = "SQ::constructor - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";
	Debug::trace("database.query", dbgHeader);

#ifndef EVOL_NDEBUG
	Time t1 = Time();
	Debug::trace("database.timing.query", dbgHeader + "Created query in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif
}


template <class SqliteEntity>
SqliteQuery<SqliteEntity>::~SqliteQuery()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;

	string dbgHeader = "SQ::destructor - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	if (stmt_forward_)
	{
		Debug::trace("database.query", dbgHeader + "finalizing stmt_forward: " + sql_forward_);
		sqlite3_finalize(stmt_forward_);
	}

	if (stmt_reverse_)
	{
		Debug::trace("database.query", dbgHeader + "finalizing stmt_reverse: " + sql_reverse_);
		sqlite3_finalize(stmt_reverse_);
	}

	queryRecordKeys_.clear();

#ifndef EVOL_NDEBUG
	Debug::trace("database.query", dbgHeader + "Destroyed query");
#endif
}


template <class SqliteEntity>
bool SqliteQuery<SqliteEntity>::selectAll(const std::string& sortOrder)
{
	if ( !doQueries_ )
		return true;

	bool isTracingSql = Debug::isTracing("database.query");

	string dbgHeader = "SQ::selectAll - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

#ifndef EVOL_NDEBUG
	Time t0 = Time();
	if (isTracingSql)
		Debug::trace("database.query", dbgHeader + "Preparing query");
#endif

	// If we've already started a query, and the caller wants to start over,
	// finalize the previous one to get rid of it.
	if (stmt_forward_)
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "called again, finalizing forward query before reinitializing - " + sql_forward_);
		sqlite3_finalize(stmt_forward_);
		stmt_forward_ = 0;
	}
	if (stmt_reverse_)
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "called again, finalizing reverse query before reinitializing - " + sql_reverse_);
		sqlite3_finalize(stmt_reverse_);
		stmt_reverse_ = 0;
	}

	mostRecentAccessedQuery_ = 0;

	sortOrder_ = sortOrder;

	string pkString = getPrimaryKeyString();

	sql_forward_ = "select * from ";
	sql_forward_.append(tableFilename_);

	sql_count_ = "select count(*) from ";
	sql_count_.append(tableFilename_);

	sql_preload_ = "select ";
	sql_preload_.append(( isSingleRecord() || pkString.size()==0 ) ? "*" : pkString);
	sql_preload_.append(" from ");
	sql_preload_.append(tableFilename_);

	bool hasOrderBy = true;
	if (sortOrder_.size())
	{
		sql_forward_.append(" order by ");
		sql_forward_.append(sortOrder_);
		sql_preload_.append(" order by ");
		sql_preload_.append(sortOrder_);
	}
	else if (pkString.size())
	{
		sql_forward_.append(" order by ");
		sql_forward_.append(pkString);
		sql_preload_.append(" order by ");
		sql_preload_.append(pkString);
	}
	else
		hasOrderBy = false;

	if (hasOrderBy)
		sql_reverse_ = sql_forward_ + " desc";

	if (isTracingSql)
	{
		Debug::trace("database.query", dbgHeader + "forward - " + sql_forward_);
		Debug::trace("database.query", dbgHeader + "reverse - " + sql_reverse_);
		Debug::trace("database.query", dbgHeader + "count   - " + sql_count_);
		Debug::trace("database.query", dbgHeader + "preload - " + sql_preload_);
	}

	int result = sqlite3_prepare_v2(db_, sql_forward_.c_str(), ((int)sql_forward_.length())+1, &stmt_forward_, NULL);
	if (result != SQLITE_OK)
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "forward query failed to prepare - " + sql_forward_);

		throw edb::DBException(edb::DBException::SQLITE_QUERY_NOT_INITIALIZED, "SQLite Error " +
								util_string::inttostring(result) + " trying to initialize forward query: " + sql_forward_);
	}

	if (sql_reverse_.size())
	{
		result = sqlite3_prepare_v2(db_, sql_reverse_.c_str(), ((int)sql_reverse_.length())+1, &stmt_reverse_, NULL);
		if (result != SQLITE_OK)
		{
			if (isTracingSql)
				Debug::trace("database.query", dbgHeader + "reverse query failed to prepare - " + sql_reverse_);

			throw edb::DBException(edb::DBException::SQLITE_QUERY_NOT_INITIALIZED, "SQLite Error " +
									util_string::inttostring(result) + " trying to initialize reverse query: " + sql_reverse_);
		}
	}
	else
		stmt_reverse_ = 0;

	if (preloadQueryRecordKeys_)
		preloadQuery();

#ifndef EVOL_NDEBUG
	Time t1 = Time();
	Debug::trace("database.timing.query", dbgHeader + "Prepared query in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return true;
}


// Use this method when the filter condition will reduce the number of records
// significantly to only a few compared to the total number of records in the table.
// Otherwise use method 'selectSomeIndexed'.
template <class SqliteEntity>
bool SqliteQuery<SqliteEntity>::selectSome(const std::string& filter, const std::string& sortOrder)
{
	return selectSome(filter, sortOrder, 0);  // No limit phrase
}

template <class SqliteEntity>
bool SqliteQuery<SqliteEntity>::selectSome(const std::string& filter, const std::string& sortOrder, int limit)
{
	if ( !doQueries_ )
		return true;

	bool isTracingSql = Debug::isTracing("database.query");

	string dbgHeader = "SQ::selectSome - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

#ifndef EVOL_NDEBUG
	Time t0 = Time();
	if (isTracingSql)
		Debug::trace("database.query", dbgHeader + "Preparing query");
#endif

	// If we've already started a query, and the caller wants to start over,
	// finalize the previous one to get rid of it.
	if (stmt_forward_)
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "called again, finalizing forward query before reinitializing - " + sql_forward_);
		sqlite3_finalize(stmt_forward_);
		stmt_forward_ = 0;
	}
	if (stmt_reverse_)
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "called again, finalizing reverse query before reinitializing - " + sql_reverse_);
		sqlite3_finalize(stmt_reverse_);
		stmt_reverse_ = 0;
	}

	mostRecentAccessedQuery_ = 0;

	sortOrder_ = sortOrder;
	limit_ = limit;

	string pkString = getPrimaryKeyString();

	sql_forward_ = "select * from ";
	sql_forward_.append(tableFilename_);

	sql_count_ = "select count(*) from ";
	sql_count_.append(tableFilename_);

	sql_preload_ = "select ";
	sql_preload_.append(( isSingleRecord() || pkString.size()==0 ) ? "*" : pkString);
	sql_preload_.append(" from ");
	sql_preload_.append(tableFilename_);

	if (filter.size())
	{
		sql_forward_.append(" where ");
		sql_forward_.append(filter);
		sql_count_.append(" where ");
		sql_count_.append(filter);
		sql_preload_.append(" where ");
		sql_preload_.append(filter);
	}

	bool hasOrderBy = true;
	if (sortOrder_.size())
	{
		sql_forward_.append(" order by ");
		sql_forward_.append(sortOrder_);
		sql_preload_.append(" order by ");
		sql_preload_.append(sortOrder_);
	}
	else if (primaryKey_.size())
	{
		sql_forward_.append(" order by ");
		sql_forward_.append(primaryKey_);
		sql_preload_.append(" order by ");
		sql_preload_.append(pkString);
	}
	else
		hasOrderBy = false;

	if (hasOrderBy)
		sql_reverse_ = sql_forward_ + " desc";

	if (limit_)
	{
		sql_forward_.append(" limit ");
		sql_forward_.append(util_string::inttostring(limit_));
		sql_reverse_.append(" limit ");
		sql_reverse_.append(util_string::inttostring(limit_));
		sql_count_.append(" limit ");
		sql_count_.append(util_string::inttostring(limit_));
		sql_preload_.append(" limit ");
		sql_preload_.append(util_string::inttostring(limit_));
	}

	if (isTracingSql)
	{
		Debug::trace("database.query", dbgHeader + "forward - " + sql_forward_);
		Debug::trace("database.query", dbgHeader + "reverse - " + sql_reverse_);
		Debug::trace("database.query", dbgHeader + "count   - " + sql_count_);
		Debug::trace("database.query", dbgHeader + "preload - " + sql_preload_);
	}

	int result = sqlite3_prepare_v2(db_, sql_forward_.c_str(), ((int)sql_forward_.length())+1, &stmt_forward_, NULL);
	if (result != SQLITE_OK)
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "forward query failed to prepare - " + sql_forward_);

		throw edb::DBException(edb::DBException::SQLITE_QUERY_NOT_INITIALIZED, "SQLite Error " +
								util_string::inttostring(result) + " trying to initialize forward query: " + sql_forward_);
	}

	if (sql_reverse_.size())
	{
		result = sqlite3_prepare_v2(db_, sql_reverse_.c_str(), ((int)sql_reverse_.length())+1, &stmt_reverse_, NULL);
		if (result != SQLITE_OK)
		{
			if (isTracingSql)
				Debug::trace("database.query", dbgHeader + "reverse query failed to prepare - " + sql_reverse_);

			throw edb::DBException(edb::DBException::SQLITE_QUERY_NOT_INITIALIZED, "SQLite Error " +
									util_string::inttostring(result) + " trying to initialize reverse query: " + sql_reverse_);
		}
	}
	else
		stmt_reverse_ = 0;

	if (preloadQueryRecordKeys_)
		preloadQuery();

#ifndef EVOL_NDEBUG
	Time t1 = Time();
	Debug::trace("database.timing.query", dbgHeader + "Prepared query in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return true;
}


// Use this method when the filter condition will not significantly reduce the number
// of records compared to the total number of records in the table.
// Otherwise use method 'selectSome'.
template <class SqliteEntity>
bool SqliteQuery<SqliteEntity>::selectSomeIndexed(const std::string& filter, const std::string& sortOrder, bool defaultOrder, bool allowReverse)
{
	if ( !doQueries_ )
		return true;

	bool isTracingSql = Debug::isTracing("database.query");

	string dbgHeader = "SQ::selectSomeIndexed - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

#ifndef EVOL_NDEBUG
	Time t0 = Time();
	if (isTracingSql)
		Debug::trace("database.query", dbgHeader + "Preparing query");
#endif

	// If we've already started a query, and the caller wants to start over,
	// finalize the previous one to get rid of it.
	if (stmt_forward_)
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "called again, finalizing forward query before reinitializing - " + sql_forward_);
		sqlite3_finalize(stmt_forward_);
		stmt_forward_ = 0;
	}
	if (stmt_reverse_)
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "called again, finalizing reverse query before reinitializing - " + sql_reverse_);
		sqlite3_finalize(stmt_reverse_);
		stmt_reverse_ = 0;
	}

	mostRecentAccessedQuery_ = 0;

	sortOrder_ = sortOrder;
	defaultOrder_ = defaultOrder;
	sql_reverse_ = "";

	string pkString = getPrimaryKeyString();

	sql_forward_ = "select * from ";
	sql_forward_.append(tableFilename_);

	sql_count_ = "select count(*) from ";
	sql_count_.append(tableFilename_);

	sql_preload_ = "select ";
	sql_preload_.append(( isSingleRecord() || pkString.size()==0 ) ? "*" : pkString);
	sql_preload_.append(" from ");
	sql_preload_.append(tableFilename_);

	if (filter.size())
	{
		sql_forward_.append(" where ");
		sql_forward_.append(filter);
		sql_count_.append(" where ");
		sql_count_.append(filter);
		sql_preload_.append(" where ");
		sql_preload_.append(filter);
	}

	bool hasOrderBy = true;
	if (sortOrder_.size())
	{
		sql_forward_.append(" order by ");
		sql_forward_.append(sortOrder_);
		sql_preload_.append(" order by ");
		sql_preload_.append(sortOrder_);
	}
	else if (primaryKey_.size())
	{
		sql_forward_.append(" order by ");
		sql_forward_.append(primaryKey_);
		sql_preload_.append(" order by ");
		sql_preload_.append(pkString);
	}
	else
		hasOrderBy = false;

	if (hasOrderBy)
	{
		if (allowReverse)
			sql_reverse_ = sql_forward_;
		if (!defaultOrder_)
		{
			sql_forward_.append(" desc");   // This only works if the sortOrder_ contains only one field.
			sql_preload_.append(" desc");   // This only works if the sortOrder_ contains only one field.
		}
		else
		{
			if (allowReverse)
				sql_reverse_.append(" desc");   // This only works if the sortOrder_ contains only one field.
		}
	}

	if (isTracingSql)
	{
		Debug::trace("database.query", dbgHeader + "forward - " + sql_forward_);
		Debug::trace("database.query", dbgHeader + "reverse - " + sql_reverse_);
		Debug::trace("database.query", dbgHeader + "count   - " + sql_count_);
		Debug::trace("database.query", dbgHeader + "preload - " + sql_preload_);
	}

	int result = sqlite3_prepare_v2(db_, sql_forward_.c_str(), ((int)sql_forward_.length())+1, &stmt_forward_, NULL);
	if (result != SQLITE_OK)
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "forward query failed to prepare - " + sql_forward_);

		throw edb::DBException(edb::DBException::SQLITE_QUERY_NOT_INITIALIZED, "SQLite Error " +
								util_string::inttostring(result) + " trying to initialize forward query: " + sql_forward_);
	}

	if (sql_reverse_.size())
	{
		result = sqlite3_prepare_v2(db_, sql_reverse_.c_str(), ((int)sql_reverse_.length())+1, &stmt_reverse_, NULL);
		if (result != SQLITE_OK)
		{
			if (isTracingSql)
				Debug::trace("database.query", dbgHeader + "reverse query failed to prepare - " + sql_reverse_);

			throw edb::DBException(edb::DBException::SQLITE_QUERY_NOT_INITIALIZED, "SQLite Error " +
									util_string::inttostring(result) + " trying to initialize reverse query: " + sql_reverse_);
		}
	}
	else
		stmt_reverse_ = 0;

	if (preloadQueryRecordKeys_)
		preloadQuery();

#ifndef EVOL_NDEBUG
	Time t1 = Time();
	Debug::trace("database.query", dbgHeader + "Prepared query in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return true;
}


template <class SqliteEntity>
bool SqliteQuery<SqliteEntity>::selectUsingQuery(const std::string& query)
{
	// NOTE:  If you plan on pasing in the SQL for the query, you CAN NOT do a last on it!!!
	//        I'm not going to figure out whether a 'desc' clause is valid for the query
	//        (need to have 'order by', and if not passed in, well then too bad!)

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Right now, we aren't going to preload this query!!!
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	if ( !doQueries_ )
		return true;

	bool isTracingSql = Debug::isTracing("database.query");

	string dbgHeader = "SQ::selectUsingQuery - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

#ifndef EVOL_NDEBUG
	Time t0 = Time();
	if (isTracingSql)
		Debug::trace("database.query", dbgHeader + "Preparing query");
#endif

	// If we've already started a query, and the caller wants to start over,
	// finalize the previous one to get rid of it.
	if (stmt_forward_)
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "called again, finalizing before reinitializing - " + sql_forward_);
		sqlite3_finalize(stmt_forward_);
		stmt_forward_ = 0;
	}

	preloadQueryRecordKeys_ = false;
	mostRecentAccessedQuery_ = 0;
	sql_forward_ = query;

	if (isTracingSql)
		Debug::trace("database.query", dbgHeader + sql_forward_);

	int result = sqlite3_prepare_v2(db_, sql_forward_.c_str(), ((int)sql_forward_.length())+1, &stmt_forward_, NULL);
	if (result != SQLITE_OK)
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "forward query failed to prepare - " + sql_forward_);

		throw edb::DBException(edb::DBException::SQLITE_QUERY_NOT_INITIALIZED, "SQLite Error " +
								util_string::inttostring(result) + " trying to initialize query: " + sql_forward_);
	}

#ifndef EVOL_NDEBUG
	Time t1 = Time();
	Debug::trace("database.timing.query", dbgHeader + "Prepared query in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return true;
}


/******************************************************************************
 ******************************************************************************
 *
 *  Navigation methods
 *
 ******************************************************************************
 *****************************************************************************/


template <class SqliteEntity>
void SqliteQuery<SqliteEntity>::preloadQuery()
{
	if ( !doQueries_ || sql_preload_.empty() )
		return;

	bool isTracingSql = Debug::isTracing("database.query");
	bool isTracingSqlExceptionDialog = Debug::isTracing("database.exception.dialog");

	string dbgHeader = "SQ::preloadQuery - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";
	Time t0 = Time(); // MSVC2010

#ifndef EVOL_NDEBUG
	if (isTracingSql)
		Debug::trace("database.query", dbgHeader + "Preloading query - " + sql_preload_);
#endif


	int count = (int)size();
	if (isTracingSql)
		Debug::trace("database.query", dbgHeader + "query size - count:" + inttostring(count));

	if (!count)
		return;

	queryRecordKeys_.reserve(count);

	int result;
	sqlite3_stmt *stmt = 0;

	Util::CLockCriticalSection lock( sqliteViewCS );

	result = sqlite3_prepare_v2(db_, sql_preload_.c_str(), (int)sql_preload_.length()+1, &stmt, NULL);
	if ( result == SQLITE_OK )
	{
		if (isTracingSql)
			Debug::trace("database.query", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql_preload_);
	}
	else
	{
		if (isTracingSql)
			Debug::trace("database.sql", dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql_preload_);
		if (isTracingSqlExceptionDialog)
			SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql_preload_);
		return;
	}

	bool moreRecs = true;
	SqliteRecord* rec = 0;
	int recOffset = 0;
	int numberOfTimeslocked = 0;
	while (moreRecs)
	{
		result = sqlite3_step(stmt);
		if ( result == SQLITE_ROW )
		{
			rec = new SqliteRecord(stmt, columnNames_);
			if (rec)
			{
				recOffset++;
				string pkString = getPrimaryKeyValue(rec);
				queryRecordKeys_.push_back(pkString);
				delete rec;
				rec = 0;
				if (isTracingSql)
					Debug::trace("database.query", dbgHeader + "preloaded rec, offset:" + inttostring(recOffset) + " primaryKeyValue:" + pkString);
			}
		}
		else if ( result == SQLITE_BUSY || result == SQLITE_IOERR_BLOCKED )
		{
			if (numberOfTimeslocked++ > MAX_LOCK_COUNT)
				moreRecs = false;
			if (isTracingSql)
				Debug::trace("database.query", dbgHeader + "busy - offset:" + inttostring(recOffset) + " lockedCount:" + inttostring(numberOfTimeslocked) + " - " + sql_preload_);
			PAL_absclass::PAL_sleep(100);
		}
		else
		{
			if (isTracingSql)
				Debug::trace("database.query", dbgHeader + "failed - offset:" + inttostring(recOffset) + " result:" + inttostring(result) + " - " + sql_preload_);
			moreRecs = false;
		}
	}

	result = sqlite3_finalize(stmt);
	if (isTracingSql)
		Debug::trace("database.query", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql_preload_);

	if (isTracingSql)
	{
		Debug::trace("database.query", dbgHeader + "number of records preloaded:" + inttostring(recOffset) + " - " + sql_preload_);
		if (count != recOffset)
			Debug::trace("database.query", dbgHeader + " NOTE: preload query size:" + inttostring(count) + " doesn't equal number of records preloaded:" + inttostring(recOffset) + " - " + sql_preload_);
	}

	if (Debug::isTracing("database.timing.query"))
	{
		Time t1 = Time();
		Debug::trace("database.timing.query", dbgHeader + "took " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds.");
	}

	return;
}


template <class SqliteEntity>
SqliteRecord* SqliteQuery<SqliteEntity>::findPreloadedRecord(const std::string& recordIdentifier)
{
	if ( !db_ || recordIdentifier.empty() )
		return 0;

	Util::CLockCriticalSection lock( sqliteViewCS );

	bool isTracingSql = Debug::isTracing("database.query");
	bool isTracingSqlExceptionDialog = Debug::isTracing("database.exception.dialog");

	string dbgHeader = "SQ::findPreloadedRecord - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	Time t0 = Time();

	// the recordIdentifier passed in contains the where clause for the record to find in the database

	std::string sql = "select * from ";
	sql.append(tableFilename_);
	sql.append(recordIdentifier);

	if (isTracingSql)
		Debug::trace("database.query", dbgHeader + sql);

	SqliteRecord* rec = 0;
	int result;
	bool looping = true;
	int numberOfTimeslocked = 0;
	while (looping)
	{
		// Assume we're not going to need to loop
		looping = false;

		sqlite3_stmt *stmt = 0;

		result = sqlite3_prepare_v2(db_, sql.c_str(), (int)sql.length()+1, &stmt, NULL);
		if ( result == SQLITE_OK )
		{
			if (isTracingSql)
				Debug::trace("database.query", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
		}
		else
		{
			if (isTracingSql)
				Debug::trace("database.query", dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);
			if (isTracingSqlExceptionDialog)
				SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);

			throw edb::DBException(edb::DBException::SQLITE_FIND_RECORD_KEY, "SQLite Error preparing select query - result:" + inttostring(result) + " - " + sql);
		}

		result = sqlite3_step(stmt);
		if ( result == SQLITE_ROW )
		{
			if (isTracingSql)
				Debug::trace("database.query", dbgHeader + "succeeded - " + sql);
				
			rec = new SqliteRecord(stmt, columnNames_);
		}
		else if ( result == SQLITE_BUSY || result == SQLITE_IOERR_BLOCKED )
		{
			if (numberOfTimeslocked++ < MAX_LOCK_COUNT)
				looping = true;
			Debug::trace("database.busy", dbgHeader + "busy - lockedCount:" + inttostring(numberOfTimeslocked) + " - " + sql);
			PAL_absclass::PAL_sleep(100);
		}
		else
		{
			if (isTracingSql)
				Debug::trace("database.query", dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);
		}
	
		result = sqlite3_finalize(stmt);
		if (Debug::isTracing("database.sql.finalize"))
			Debug::trace("database.sql.finalize", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql);
	}
	
	if (Debug::isTracing("database.timing.query"))
	{
		Time t1 = Time();
		Debug::trace("database.timing.query", dbgHeader + "Find query record by key in " + tableFilename_ + " took " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
	}

	return rec;
}


/*************************************************************************
 *  first
 ************************************************************************/

template <class SqliteEntity>
typename SqliteEntity::interface_type* SqliteQuery<SqliteEntity>::first()
{
	if ( !doQueries_ )
		return 0;

	bool isTracingSql = Debug::isTracing("database.query.navigate");

	string dbgHeader = "SQ::first - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	Time t0 = Time();
	if (isTracingSql)
		Debug::trace("database.query.navigate", dbgHeader + "Navigating first record of query");

	if (preloadQueryRecordKeys_)
	{
		// Set the counter to "point" to the offset in the preloaded-records vector of the current record.
		recordCounter_forward_ = 0;

		while ( recordCounter_forward_ < (int)queryRecordKeys_.size() )
		{
			SqliteRecord* rec = findPreloadedRecord(queryRecordKeys_[recordCounter_forward_]);

			if ( !rec )
			{
				if (Debug::isTracing("database.query.navigate"))
					Debug::trace("database.query.navigate", dbgHeader + "record NOT found for " + queryRecordKeys_[recordCounter_forward_]);

				++recordCounter_forward_;
				continue;
			}
			else
			{
				SqliteEntity* e = new SqliteEntity(rec);

				if (Debug::isTracing("database.timing.query"))
				{
					Time t1 = Time();
					Debug::trace("database.timing.query", dbgHeader + "record found in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
				}

				return e;
			}
		}
	}
	else
	{
		if (stmt_forward_)
		{
			SqliteRecord* rec = 0;

			sqlite3_reset(stmt_forward_);

			int result = sqlite3_step(stmt_forward_);
			if ( result == SQLITE_ROW )
			{
				recordCounter_forward_ = 1;
				recordCounter_reverse_ = 0;
				mostRecentAccessedQuery_ = 1;

				if (isTracingSql)
					Debug::trace("database.query.navigate", dbgHeader + "record found - " + sql_forward_);
				
				rec = new SqliteRecord(stmt_forward_, columnNames_);

				SqliteEntity* e = new SqliteEntity(rec);

				if (Debug::isTracing("database.timing.query"))
				{
					Time t1 = Time();
					Debug::trace("database.timing.query", dbgHeader + "record found in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
				}

				return e;
			}
			else
			{
				if (isTracingSql)
					Debug::trace("database.query.navigate", dbgHeader + "record not found - " + sql_forward_);
			}
		}
	}

#ifndef EVOL_NDEBUG
	Time t2 = Time();
	Debug::trace("database.timing.query", dbgHeader + "record not found (took " + inttostring((t2 - t0).getTotalMilliseconds()) + "milliseconds). Query is empty");
#endif

	return 0;
}


/*************************************************************************
 *  next
 ************************************************************************/

//template <class SqliteEntity>
//typename SqliteEntity::interface_type* SqliteQuery<SqliteEntity>::next()
template <class SqliteEntity>
typename SqliteEntity::interface_type* SqliteQuery<SqliteEntity>::next(const DBEntityI* /*current*/)
{
	if ( !doQueries_ )
		return 0;

	bool isTracingSql = Debug::isTracing("database.query.navigate");

	string dbgHeader = "SQ::next - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

#ifndef EVOL_NDEBUG
	Time t0 = Time();
	if (isTracingSql)
		Debug::trace("database.query.navigate", dbgHeader + "Navigating the next record in the query");
#endif

	if (preloadQueryRecordKeys_)
	{
		while ( ++recordCounter_forward_ < (int)queryRecordKeys_.size() )
		{
			SqliteRecord* rec = findPreloadedRecord(queryRecordKeys_[recordCounter_forward_]);

			if ( !rec )
			{
				if (Debug::isTracing("database.query.navigate"))
					Debug::trace("database.query.navigate", dbgHeader + "record NOT found for " + queryRecordKeys_[recordCounter_forward_]);

				continue;
			}
			else
			{
				SqliteEntity* e = new SqliteEntity(rec);
				// Already incremented the counter to "point" to the next record, which is now the current record.
				return e;
			}
		}

		// Went past the end, so go back one to "point" to the last record.
		--recordCounter_forward_;
	}
	else
	{
		// Can't really do a next if haven't done a first or last on the query, so assume they want to use the forward query and perform a first for them.
		if ( mostRecentAccessedQuery_ == 0 || (recordCounter_forward_ == 0 && recordCounter_reverse_ == 0) )
			return first();

		if (mostRecentAccessedQuery_ == 1)
		{
			// We're already using the forward query, so it's easy to do a next on it!!
			if (stmt_forward_)
			{
				SqliteRecord* rec = 0;

				int result = sqlite3_step(stmt_forward_);
				if ( result == SQLITE_ROW )
				{
					if (isTracingSql)
						Debug::trace("database.query.navigate", dbgHeader + "record found for forward query - " + sql_forward_);
					
					rec = new SqliteRecord(stmt_forward_, columnNames_);

					SqliteEntity* e = new SqliteEntity(rec);

#ifndef EVOL_NDEBUG
					Time t1 = Time();
					Debug::trace("database.timing.query", dbgHeader + "record found in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

					return e;
				}
				else
				{
					if (isTracingSql)
						Debug::trace("database.query.navigate", dbgHeader + "record not found for forward query - " + sql_forward_);
				}
			}
		}
		else
		{
			// Wanting to do a next when using the reverse query, which isn't directly/easily supported,
			// but we have a way to do it, it just may be costly, in terms of time, if they've already done a number of 
			// previouses on the reverse query.  We essentially have to start over,
			// and traverse to the record before the current one, using the recordCounter_reverse_ to get there. 
			SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + tableFilename_ + " - method called, but currently using reverse query! - sql_forward - " + sql_forward_);
		}
	}

	return 0;
}


/*************************************************************************
 *  previous
 ************************************************************************/

//template <class SqliteEntity>
//typename SqliteEntity::interface_type* SqliteQuery<SqliteEntity>::previous()
template <class SqliteEntity>
typename SqliteEntity::interface_type* SqliteQuery<SqliteEntity>::previous(const DBEntityI* /*current*/)
{
	if ( !doQueries_ )
		return 0;

	bool isTracingSql = Debug::isTracing("database.query.navigate");

	string dbgHeader = "SQ::previous - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

#ifndef EVOL_NDEBUG
	Time t0 = Time();
	if (isTracingSql)
		Debug::trace("database.query.navigate", dbgHeader + "Navigating the previous record in the query");
#endif

	if (preloadQueryRecordKeys_)
	{
		while ( 0 <= --recordCounter_forward_ )
		{
			SqliteRecord* rec = findPreloadedRecord(queryRecordKeys_[recordCounter_forward_]);

			if ( !rec )
			{
				if (Debug::isTracing("database.query.navigate"))
					Debug::trace("database.query.navigate", dbgHeader + "record NOT found for " + queryRecordKeys_[recordCounter_forward_]);

				continue;
			}
			else
			{
				SqliteEntity* e = new SqliteEntity(rec);
				// Already decremented the counter to "point" to the previous record, which is now the current record.
				return e;
			}
		}

		if ( recordCounter_forward_ < 0 )
			// Went past the beginning, so go and "point" to the first record.
			recordCounter_forward_ = 0;
	}
	else
	{
		// Can't really do a previous if haven't done a first or last on the query, so assume they want to use the reverse query and perform a last for them.
		if ( mostRecentAccessedQuery_ == 0 || (recordCounter_forward_ == 0 && recordCounter_reverse_ == 0) )
			return last();

		if (mostRecentAccessedQuery_ == -1)
		{
			if (stmt_reverse_)
			{
				// We're already using the reverse query, so it's easy to do a previous on it!!
				SqliteRecord* rec = 0;

				int result = sqlite3_step(stmt_reverse_);
				if ( result == SQLITE_ROW )
				{
					if (isTracingSql)
						Debug::trace("database.query.navigate", dbgHeader + "record found for reverse query - " + sql_reverse_);
						
					rec = new SqliteRecord(stmt_reverse_, columnNames_);

					SqliteEntity* e = new SqliteEntity(rec);

#ifndef EVOL_NDEBUG
					Time t1 = Time();
					Debug::trace("database.timing.query", dbgHeader + "record found in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

					return e;
				}
				else
				{
					if (isTracingSql)
						Debug::trace("database.query.navigate", dbgHeader + "record not found for reverse query - " + sql_reverse_);
				}
			}
			else
			{
				SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + tableFilename_ + " - method called, but no reverse query setup! - sql_forward - " + sql_forward_);
				//throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "SqliteQuery::previous - method called, but not implemented, or can't do it!!!");
			}
		}
		else
		{
			// Wanting to do a previous when using the forward query, which isn't directly/easily supported,
			// but we have a way to do it, it just may be costly, in terms of time, if they've already done a number of 
			// nexts on the forward query.  We essentially have to start over,
			// and traverse to the record before the current one, using the recordCounter_forward_ to get there.
			//     SessionManagerHelper::getInstance()->addDialog(0, 1, "SqliteQuery::previous " + tableFilename_ + " - method called, but currently using forward query! -  sql_forward: " + sql_forward_);

			if (isTracingSql)
				Debug::trace("database.query.navigate", dbgHeader + "called previous when using the forward query - starting recordCounter_forward_:" + inttostring(recordCounter_forward_) + " - " + sql_forward_);
						
			if ( recordCounter_forward_ <= 1 )
				// There is no previous if we're on the first record, so just return null.
				return 0;
			else
			{
				int saved_recordCounter_forward = recordCounter_forward_;
				typename SqliteEntity::interface_type* e = first();
				if (e)
				{
					typename SqliteEntity::interface_type* eLoop = e; 
					while ( recordCounter_forward_ < saved_recordCounter_forward - 1 && eLoop )
					{
						eLoop = next();
						if (eLoop)
						{
							delete e;
							e = eLoop;
						}
					}

					if (isTracingSql)
						Debug::trace("database.query.navigate", dbgHeader + "returning when called previous when using the forward query - new recordCounter_forward_:" + inttostring(recordCounter_forward_) + " rec*:" + ptrtostring(e) + " - " + sql_forward_);
						
					return e;
				}
			}
		}
	}

	return 0;
}


/*************************************************************************
 *  last
 ************************************************************************/

template <class SqliteEntity>
typename SqliteEntity::interface_type* SqliteQuery<SqliteEntity>::last()
{
	if ( !doQueries_ )
		return 0;

	bool isTracingSql = Debug::isTracing("database.query.navigate");

	string dbgHeader = "SQ::last - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

#ifndef EVOL_NDEBUG
	Time t0 = Time();
	if (isTracingSql)
		Debug::trace("database.query.navigate", dbgHeader + "Navigating the last record in the query");
#endif

	if (preloadQueryRecordKeys_)
	{
		// Set the counter to "point" to the offset in the preloaded-records vector of the last record.
		recordCounter_forward_ = (int)queryRecordKeys_.size();

		while ( 0 <= --recordCounter_forward_ )
		{
			SqliteRecord* rec = findPreloadedRecord(queryRecordKeys_[recordCounter_forward_]);

			if ( !rec )
			{
				if (Debug::isTracing("database.query.navigate"))
					Debug::trace("database.query.navigate", dbgHeader + "record NOT found for " + queryRecordKeys_[recordCounter_forward_]);

				continue;
			}
			else
			{
				SqliteEntity* e = new SqliteEntity(rec);
				return e;
			}
		}

		if ( recordCounter_forward_ < 0 )
			// Went past the beginning, so go and "point" to the first record.
			recordCounter_forward_ = 0;
	}
	else
	{
		if (stmt_reverse_)
		{
			SqliteRecord* rec = 0;

			sqlite3_reset(stmt_reverse_);

			int result = sqlite3_step(stmt_reverse_);
			if ( result == SQLITE_ROW )
			{
				recordCounter_reverse_ = 1;
				recordCounter_forward_ = 0;
				mostRecentAccessedQuery_ = -1;

				if (isTracingSql)
					Debug::trace("database.query.navigate", dbgHeader + "record found - " + sql_reverse_);
				
				rec = new SqliteRecord(stmt_reverse_, columnNames_);

				SqliteEntity* e = new SqliteEntity(rec);

#ifndef EVOL_NDEBUG
				Time t1 = Time();
				Debug::trace("database.timing.query", dbgHeader + "record found in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

				return e;
			}
			else
			{
				if (isTracingSql)
					Debug::trace("database.query.navigate", dbgHeader + "record not found - " + sql_reverse_);
			}
		}
		else
		{
			SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + tableFilename_ + " - method called, but no reverse query setup! - sql_forward - " + sql_forward_);
			//throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "SqliteQuery::last - method called, but no reverse query setup!");
		}
	}

	return 0;
}


/*************************************************************************
 *  find
 ************************************************************************/

template <class SqliteEntity>
typename SqliteEntity::interface_type* SqliteQuery<SqliteEntity>::find(const DBEntityI* current)
{
	if ( !doQueries_ )
		return 0;

	const SqliteEntity* p = dynamic_cast<const SqliteEntity*>(current);
	if ( p )
		SessionManagerHelper::getInstance()->addDialog(0, 1, "SqliteQuery::find " + tableFilename_ + " - 'current' record passed in!!!");

	SessionManagerHelper::getInstance()->addDialog(0, 1, "SqliteQuery::find " + tableFilename_ + " - method called, but not implemented, or can't do it!! -  sql forward: " + sql_forward_);

	//throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "SqliteQuery::find - method called, but not implemented, or can't do it!!!");
	return 0;
}


/*************************************************************************
 *  size
 ************************************************************************/

template <class SqliteEntity>
typename SqliteQuery<SqliteEntity>::size_type SqliteQuery<SqliteEntity>::size()
{
	if ( !doQueries_ )
		return 0;

	bool isTracingSql = Debug::isTracing("database.query.size");

	string dbgHeader = "SQ::size - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	if ( !sql_count_.size() )
	{
		if (isTracingSql)
			Debug::trace("database.query.size", dbgHeader + "invalid, don't have a query to determine size - sql forward - " + sql_forward_);
		return 0;
	}

#ifndef EVOL_NDEBUG
	Time t0 = Time();
	if (isTracingSql)
		Debug::trace("database.query.size", dbgHeader + "Calculating size of query");
#endif

	if (isTracingSql)
		Debug::trace("database.query.size", dbgHeader + sql_count_);

	int recCount = 0;
	sqlite3_stmt *stmt_count;

	int result = sqlite3_prepare_v2(db_, sql_count_.c_str(), (int)sql_count_.length()+1, &stmt_count, NULL);
	if (result != SQLITE_OK)
	{
		if (isTracingSql)
			Debug::trace("database.query.size", dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql_count_);

		throw edb::DBException(edb::DBException::SQLITE_QUERY_NOT_INITIALIZED, "SQLite Error preparing size query - result:" + inttostring(result) + " - " + sql_count_);
	}
			
	result = sqlite3_step(stmt_count);
	if ( result == SQLITE_ROW )
	{
		int data = -1;
		int numFields = sqlite3_data_count(stmt_count);
		if (numFields > 0)
			data = sqlite3_column_int(stmt_count, 0);

		recCount = data;

		if (isTracingSql)
			Debug::trace("database.query.size", dbgHeader + "succeeded - records counted:" + inttostring(recCount));
	}
	else
	{
		if (isTracingSql)
			Debug::trace("database.query.size", dbgHeader + "failed - result:" + inttostring(result) + " - " + sql_count_);
	}
			
	sqlite3_finalize(stmt_count);
	if (isTracingSql)
		Debug::trace("database.query.size", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql_count_);

#ifndef EVOL_NDEBUG
	Time t1 = Time();
	Debug::trace("database.timing.query", dbgHeader + "Size of query determined in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return recCount;
}


/*************************************************************************
 *  empty
 ************************************************************************/

template <class SqliteEntity>
bool SqliteQuery<SqliteEntity>::empty()
{
	bool isEmpty = true;

	if ( !doQueries_ )
		return isEmpty;

	bool isTracingSql = Debug::isTracing("database.query.size");

	string dbgHeader = "SQ::empty - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

#ifndef EVOL_NDEBUG
	Time t0 = Time();
	if (isTracingSql)
		Debug::trace("database.query.size", dbgHeader + "Figuring out whether query is empty");
#endif

	if (preloadQueryRecordKeys_)
	{
		// Base 'emptiness' on whether there are record keys that have been preloaded in the queryRecordKeys_ vector.
		isEmpty = (queryRecordKeys_.size() == 0) ? true : false;
	}
	else
	{
		// Query is not preloaded.
		// First see if we have navigated to a record already (ie. called first() or last()).  If we have, then it's not empty.
		if ( recordCounter_forward_ > 0 || recordCounter_reverse_ > 0 )
			isEmpty = false;
		else
		{
			// The query has not read any records yet, so do a first() on it and see if there are any records.
			first();
			isEmpty = (recordCounter_forward_ == 0) ? true : false;

			// Then reset the query back to the state as if the query hasn't been executed yet.
			if (stmt_forward_)
			{
				sqlite3_reset(stmt_forward_);
				recordCounter_forward_ = 0;
				mostRecentAccessedQuery_ = 0;
			}
		}
	}

	if (isTracingSql)
		Debug::trace("database.query.size", dbgHeader + ((isEmpty) ? "empty" : "not empty"));

#ifndef EVOL_NDEBUG
	Time t1 = Time();
	Debug::trace("database.timing.query", dbgHeader + "Emptyness determined in " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return isEmpty;
}


template <class SqliteEntity>
void SqliteQuery<SqliteEntity>::show()
{
	Debug::trace("database.show", 20, "SqliteQuery instance address: " + ptrtostring(this));
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEQUERY_H
