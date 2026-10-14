#include "SessionManagerHelper.h"

#include <string>


#include "sqliterecordmanager.h"
#include "sqlitedatabase.h"

#include ACCOUNTANTI_INC
#include DBEXCEPTION_INC
#include UTILSTRING_INC
#include FILENAME_INC
#include FILE_INC
#include DEBUG_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_filesystem;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
int SqliteRecordManager::_dataType_ = 0;
string SqliteRecordManager::_dataTypeName_("SqliteRecordManager");
#endif



SqliteRecordManager::SqliteRecordManager(const SqliteDatabase& sdb, const util_env::Properties& props, const string& aliasName, bool openTable)
		: SqliteView(sdb.getDB(), props, aliasName, openTable), sdb_(sdb), doNothing_(false), doReads_(true), doSaves_(true), doDeletes_(true), checkBeforeBackup_(false)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);
	util_acc::addInstance(_dataType_);
#endif

	const string* property = getTableProperty("checkBeforeBackup");
	if ( property )
	{
		checkBeforeBackup_ = stringtobool(*property);

#ifndef EVOL_NDEBUG
		Debug::trace("database.table", (checkBeforeBackup_ ? "Checking" : "Not checking") + string(" table:") + tableFilename_ + " before backups");
#endif
	}

	property = getTableProperty("doNothing");
	if ( property )
	{
		doNothing_ = stringtobool(*property);
		if ( doNothing_ )
		{
			doReads_ = false;
			doSaves_ = false;
			doDeletes_ = false;
		}
	}

	property = getTableProperty("doReads");
	if ( property )
		doReads_ = stringtobool(*property);

	property = getTableProperty("doSaves");
	if ( property )
		doSaves_ = stringtobool(*property);

	property = getTableProperty("doDeletes");
	if ( property )
		doDeletes_ = stringtobool(*property);

	// Determine, through properties, whether this table has an auto-increment field
	property = getTableProperty("autoIncrementField");
	if ( property )
	{
		autoIncrementField_.assign(*property);

		property = getFieldProperty(autoIncrementField_, "autoIncrementTag");
		if ( property )
			autoIncrementTag_.assign(*property);
	}
}


SqliteRecordManager::~SqliteRecordManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


SqliteRecord* SqliteRecordManager::findSingleRecord()
{
	if ( !db_ || !doReads_ )
		return 0;

	Util::CLockCriticalSection lock( sqliteViewCS );

#ifndef EVOL_NDEBUG
	util_time::Time t0 = util_time::Time();
#endif

	bool isTracingSql = Debug::isTracing("database.sql");
	bool isTracingSqlSuccess = Debug::isTracing("database.sql.success");
	bool isTracingSqlExceptionDialog = Debug::isTracing("database.exception.dialog");

	string dbgHeader = "SRM::findSingleRecord - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	SqliteRecord* rec = 0;

	if ( isSingleRecord() )
	{
		// If this is a single (at most) record table, simply retrieve the first record

		int result;
		
		std::string sql = "select * from ";
		sql.append(tableFilename_);

		if (isTracingSql)
			Debug::trace("database.sql", dbgHeader + sql);

		bool looping = true;
		int numberOfTimeslocked = 0;
		while (looping)
		{
			// Assume we're not going to need to loop
			looping = false;

			sqlite3_stmt *stmt = 0;

			result = sqlite3_prepare_v2(db_, sql.c_str(), (int)sql.length()+1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				if (isTracingSqlSuccess)
					Debug::trace("database.sql.success", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
			}
			else
			{
				if (isTracingSql)
					Debug::trace("database.sql", dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);
				if (isTracingSqlExceptionDialog)
					SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);

				throw edb::DBException(edb::DBException::SQLITE_FIND_SINGLE_RECORD, "SQLite Error preparing select query - result:" + inttostring(result) + " - " + sql);
			}
		
			result = sqlite3_step(stmt);
			if ( result == SQLITE_ROW )
			{
				if (isTracingSqlSuccess)
					Debug::trace("database.sql.success", dbgHeader + "succeeded -" + sql);

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
					Debug::trace("database.sql", dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);
			}

			result = sqlite3_finalize(stmt);
			if (Debug::isTracing("database.sql.finalize"))
				Debug::trace("database.sql.finalize", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql);
		}
	}

#ifndef EVOL_NDEBUG
	util_time::Time t1 = util_time::Time();
	Debug::trace("database.timing.findSingleRecord", dbgHeader + "Find single record in " + tableFilename_ + " took " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return rec;
}


SqliteRecord* SqliteRecordManager::findKey(const SqliteField& key, const std::string& queryStart)
{
	std::vector<SqliteField> keys;
	keys.push_back(key);

	SqliteRecord* rec = findKey(keys, queryStart);
	return rec;
}


SqliteRecord* SqliteRecordManager::findKey(const std::vector<SqliteField>& keys, const std::string& queryStart)
{
	Util::CLockCriticalSection lock( sqliteViewCS );
	return findKeyPrivate(keys, queryStart);
}


SqliteRecord* SqliteRecordManager::findKeyPrivate(const std::vector<SqliteField>& keys, const std::string& queryStart)
{
	// Find with a compound primary key.

	if ( !db_ || !doReads_ || keys.size() == 0 )
		return 0;

#ifndef EVOL_NDEBUG
	util_time::Time t0 = util_time::Time();
#endif

	bool isTracingSql = Debug::isTracing("database.sql");
	bool isTracingSqlSuccess = Debug::isTracing("database.sql.success");
	bool isTracingSqlExceptionDialog = Debug::isTracing("database.exception.dialog");

	string dbgHeader = "SRM::findKey - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	SqliteRecord* rec = 0;

	// findKey is always performed based on the primary index which is always set.

	std::string sql;
	int result;
	
	sql = (queryStart.size() > 0) ? queryStart : "select * from ";
	sql.append(tableFilename_);
	sql.append(generateWhereClause(keys));

	if (isTracingSql)
		Debug::trace("database.sql", dbgHeader + sql);

	bool looping = true;
	int numberOfTimeslocked = 0;
	while (looping)
	{
		// Assume we're not going to need to loop
		looping = false;

		sqlite3_stmt *stmt = 0;

		result = sqlite3_prepare_v2(db_, sql.c_str(), (int)sql.length()+1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			if (isTracingSqlSuccess)
				Debug::trace("database.sql.success", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
		}
		else
		{
			if (isTracingSql)
				Debug::trace("database.sql", dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);
			if (isTracingSqlExceptionDialog)
				SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);

			throw edb::DBException(edb::DBException::SQLITE_FIND_RECORD_KEY, "SQLite Error preparing select query - result:" + inttostring(result) + " - " + sql);
		}

		result = sqlite3_step(stmt);
		if ( result == SQLITE_ROW )
		{
			if (isTracingSqlSuccess)
				Debug::trace("database.sql.success", dbgHeader + "succeeded - " + sql);
				
			rec = new SqliteRecord(stmt, columnNames_, keys);
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
				Debug::trace("database.sql", dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);
		}
	
		result = sqlite3_finalize(stmt);
		if (Debug::isTracing("database.sql.finalize"))
			Debug::trace("database.sql.finalize", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql);
	}
	
#ifndef EVOL_NDEBUG
	util_time::Time t1 = util_time::Time();
	Debug::trace("database.timing.findRecord", dbgHeader + "Find record by key in " + tableFilename_ + " took " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return rec;
}


SqliteRecord* SqliteRecordManager::create()
{
	Util::CLockCriticalSection lock( sqliteViewCS );

#ifndef EVOL_NDEBUG
	util_time::Time t0 = util_time::Time();
#endif

	bool isTracingSql = Debug::isTracing("database.sql");

	string dbgHeader = "SRM::create - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	SqliteRecord* rec = 0;

	if (columns_.size())
		rec = new SqliteRecord(columns_);
	else
		rec = new SqliteRecord(0, columnNames_, vector<SqliteField>());

	// Set the value of the autoincrement field, if any
	if ( !autoIncrementField_.empty() )
	{
		if (isTracingSql)
			Debug::trace("database.sql", dbgHeader + "for table:" + tableFilename_ + " - before calling performAutoIncrements");
		performAutoIncrements(rec);
	}

	if (isTracingSql)
		Debug::trace("database.sql", dbgHeader + "record created for table:" + tableFilename_);

#ifndef EVOL_NDEBUG
	util_time::Time t1 = util_time::Time();
	Debug::trace("database.timing.createRecord", dbgHeader + "Create record for " + tableFilename_ + " took " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return rec;
}


bool SqliteRecordManager::save(const SqliteRecord* rec)
{
	if ( !rec )
		return false;

	if ( !db_ || !doSaves_  || isReadOnly() )
		return true;

	Util::CLockCriticalSection lock( sqliteViewCS );

#ifndef EVOL_NDEBUG
	util_time::Time t0 = util_time::Time();
#endif

	bool isTracingSql = Debug::isTracing("database.sql");
	bool isTracingSqlSuccess = Debug::isTracing("database.sql.success");
	bool isTracingSqlExceptionDialog = Debug::isTracing("database.exception.dialog");

	string dbgHeader = "SRM::save - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	if (isTracingSql)
		Debug::trace("database.sql", dbgHeader + "save initiated for table:" + tableFilename_);

	bool retVal = true;
	
	std::vector<SqliteField> tempPKeys;
	if (!isSingleRecord())
		tempPKeys = rec->getColumns(primaryKeyColumnNames_);

	int result;

	// See if the record already exists in the database ...
	SqliteRecord* tempRec = 0;

	if ( isSingleRecord() )
		tempRec = findSingleRecord();
	else
	{
		// lap - Optimize in the future to just get the primary key columns, or even just one of them
		string keyname = "*";
		if (tempPKeys.size() > 0)
			keyname = tempPKeys[0].getName();
		string tempQueryStart = "select " + keyname + " from ";
		tempRec = findKeyPrivate(tempPKeys, tempQueryStart);
	}

	if ( tempRec )
	{
		// Only needed tempRec to see if it was in the database; will use the rec passed in to save, so don't need tempRec any longer.
		delete tempRec;
		tempRec = 0;

		/*record found, so just update the one in the db*/

		std::string sql = "update ";
		sql.append(tableFilename_);
		sql.append(" set ");
		sql.append(generateSetClauseList(rec));
		if ( !isSingleRecord() )
		{
			sql.append(" ");
			sql.append(generateWhereClause(tempPKeys));
		}

		if (isTracingSql)
			Debug::trace("database.sql", dbgHeader + sql);

#if 0
		// Don't delete this code; I may want to use it someday. I want to remember how I had it working at the beginning development of Fastrack.
		bool execResult = execute(sql);
		if (execResult)
		{
			if (isTracingSqlSuccess)
				Debug::trace("database.sql.success", dbgHeader + "execute of update succeeded - " + sql);
		}
		else
		{
			if (isTracingSql)
				Debug::trace("database.sql", dbgHeader + "execute of update failed - result:" + inttostring(result) + " - " + sql);
			if (isTracingSqlExceptionDialog)
				SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "update failed - result:" + inttostring(result) + " - " + sql);

			retVal = false;
		}
#else

		bool looping = true;
		int numberOfTimeslocked = 0;
		while (looping)
		{
			// Assume we're not going to need to loop
			looping = false;

			sqlite3_stmt *stmt = 0;

			result = sqlite3_prepare_v2(db_, sql.c_str(), (int)sql.length()+1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				if (isTracingSqlSuccess)
					Debug::trace("database.sql.success", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
			}
			else
			{
				if (isTracingSql)
					Debug::trace("database.sql", dbgHeader + "update query failed to prepare - result:" + inttostring(result) + " - " + sql);
				if (isTracingSqlExceptionDialog)
					SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "update query failed to prepare - result:" + inttostring(result) + " - " + sql);

				throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "SQLite Error preparing update query - result:" + inttostring(result) + " - " + sql);
			}

			result = sqlite3_step(stmt);
			if ( result == SQLITE_DONE )
			{
				if (isTracingSqlSuccess)
					Debug::trace("database.sql.success", dbgHeader + "update succeeded - " + sql);
			}
			else if ( result == SQLITE_BUSY || result == SQLITE_IOERR_BLOCKED )
			{
				if (numberOfTimeslocked++ < MAX_LOCK_COUNT)
					looping = true;
				else
					retVal = false;
				Debug::trace("database.busy", dbgHeader + "update busy - lockedCount:" + inttostring(numberOfTimeslocked) + " - " + sql);
				PAL_absclass::PAL_sleep(100);
			}
			else
			{
				if (isTracingSql)
					Debug::trace("database.sql", dbgHeader + "update failed - result:" + inttostring(result) + " - " + sql);
				if (isTracingSqlExceptionDialog)
					SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "update failed - result:" + inttostring(result) + " - " + sql);
				retVal = false;
			}
			
			result = sqlite3_finalize(stmt);
			if (Debug::isTracing("database.sql.finalize"))
				Debug::trace("database.sql.finalize", dbgHeader + "update finalize - result:" + inttostring(result) + " - " + sql);
		}

#endif

	}
	else
	{
		// The record does not exist, so insert this one.

		std::string sql = "insert into ";
		sql.append(tableFilename_);
		sql.append(generateColumnNamesList(rec));
		sql.append(" values ");
		sql.append(generateColumnValuesList(rec));

		if (isTracingSql)
			Debug::trace("database.sql", dbgHeader + sql);

		bool looping = true;
		int numberOfTimeslocked = 0;
		while (looping)
		{
			// Assume we're not going to need to loop
			looping = false;

			sqlite3_stmt *stmt = 0;

			result = sqlite3_prepare_v2(db_, sql.c_str(), (int)sql.length()+1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				if (isTracingSqlSuccess)
					Debug::trace("database.sql.success", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
			}
			else
			{
				if (isTracingSql)
					Debug::trace("database.sql", dbgHeader + "insert query failed to prepare - result:" + inttostring(result) + " - " + sql);
				if (isTracingSqlExceptionDialog)
					SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "insert query failed to prepare - result:" + inttostring(result) + " - " + sql);

				throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "SQLite Error preparing insert query - result:" + inttostring(result) + " - " + sql);
			}
			
			result = sqlite3_step(stmt);
			if ( result == SQLITE_DONE )
			{
				if (isTracingSqlSuccess)
					Debug::trace("database.sql.success", dbgHeader + "insert succeeded - " + sql);
			}
			else if ( result == SQLITE_BUSY || result == SQLITE_IOERR_BLOCKED )
			{
				if (numberOfTimeslocked++ < MAX_LOCK_COUNT)
					looping = true;
				else
					retVal = false;
				Debug::trace("database.busy", dbgHeader + "insert busy - lockedCount:" + inttostring(numberOfTimeslocked) + " - " + sql);
				PAL_absclass::PAL_sleep(100);
			}
			else
			{
				if (isTracingSql)
					Debug::trace("database.sql", dbgHeader + "insert failed - result:" + inttostring(result) + " - " + sql);
				if (isTracingSqlExceptionDialog)
					SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "insert failed - result:" + inttostring(result) + " - " + sql);

				retVal = false;
			}
			
			result = sqlite3_finalize(stmt);
			if (Debug::isTracing("database.sql.finalize"))
				Debug::trace("database.sql.finalize", dbgHeader + "insert finalize - result:" + inttostring(result) + " - " + sql);
		}
	}

#ifndef EVOL_NDEBUG
	util_time::Time t1 = util_time::Time();
	Debug::trace("database.timing.saveRecord", dbgHeader + "Save record for " + tableFilename_ + " took " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return retVal;
}


// Delete the record passed in
bool SqliteRecordManager::markDeleted(const SqliteRecord* rec)
{
	if ( !rec )
		return false;

	if ( !db_ || !doDeletes_ || isReadOnly() )
		return true;

	Util::CLockCriticalSection lock( sqliteViewCS );

#ifndef EVOL_NDEBUG
	util_time::Time t0 = util_time::Time();
#endif

	bool isTracingSql = Debug::isTracing("database.sql");
	bool isTracingSqlSuccess = Debug::isTracing("database.sql.success");
	bool isTracingSqlExceptionDialog = Debug::isTracing("database.exception.dialog");

	string dbgHeader = "SRM::markDeleted - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	bool retVal = true;
	
	std::vector<SqliteField> tempPKeys;
	if (!isSingleRecord())
		tempPKeys = rec->getColumns(primaryKeyColumnNames_);

#if 0
	// For now, I'm commenting out the reading of the record just to see if it's in the database before deleting it.
	// If it's not there by just trying to delete it, then it's not there, which was the goal to begin with.

	// See if the record exists in the database ...
	SqliteRecord* tempRec = 0;

	if ( isSingleRecord() )
		tempRec = findSingleRecord();
	else
	{
		// lap - Optimize in the future to just get the primary key columns, or even just one of them
		string tempQueryStart = "select * from ";
		tempRec = findKeyPrivate(tempPKeys, tempQueryStart);
	}

	if ( !tempRec )
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database.deleteRecord", 3, "Unable to delete record: record not found in database.");
#endif
		return false;
	}

	delete tempRec;
	tempRec = 0;
#endif

	// Delete it.

	int result;
			
	std::string sql = "delete from ";
	sql.append(tableFilename_);
	if ( !isSingleRecord() )
		sql.append(generateWhereClause(tempPKeys));

	if (isTracingSql)
		Debug::trace("database.sql", dbgHeader + sql);

	bool looping = true;
	int numberOfTimeslocked = 0;
	while (looping)
	{
		// Assume we're not going to need to loop
		looping = false;

		sqlite3_stmt *stmt = 0;

		result = sqlite3_prepare_v2(db_, sql.c_str(), (int)sql.length()+1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			if (isTracingSqlSuccess)
				Debug::trace("database.sql.success", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
		}
		else
		{
			if (isTracingSql)
				Debug::trace("database.sql", dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);
			if (isTracingSqlExceptionDialog)
				SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);

			throw edb::DBException(edb::DBException::SQLITE_DELETE_RECORD, "SQLite Error preparing delete query - result:" + inttostring(result) + " - " + sql);
		}

		result = sqlite3_step(stmt);
		if ( result == SQLITE_DONE )
		{
			if (isTracingSqlSuccess)
				Debug::trace("database.sql.success", dbgHeader + "succeeded - " + sql);
		}
		else if ( result == SQLITE_BUSY || result == SQLITE_IOERR_BLOCKED )
		{
			if (numberOfTimeslocked++ < MAX_LOCK_COUNT)
				looping = true;
			else
				retVal = false;
			Debug::trace("database.busy", dbgHeader + "busy - lockedCount:" + inttostring(numberOfTimeslocked) + " - " + sql);
			PAL_absclass::PAL_sleep(100);
		}
		else
		{
			if (isTracingSql)
				Debug::trace("database.sql", dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);
			if (isTracingSqlExceptionDialog)
				SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);

			retVal = false;
		}

		result = sqlite3_finalize(stmt);
		if (Debug::isTracing("database.sql.finalize"))
			Debug::trace("database.sql.finalize", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql);
	}
	
#ifndef EVOL_NDEBUG
		util_time::Time t1 = util_time::Time();
		Debug::trace("database.timing.deleteRecord", dbgHeader + "Delete record for " + tableFilename_ + " took " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return retVal;
}


int SqliteRecordManager::deleteAll()
{
	return deleteAll("");
}


int SqliteRecordManager::deleteAll(string whereClause)
{
	if ( !db_ || !doDeletes_ || isReadOnly() )
		return 0;

	Util::CLockCriticalSection lock( sqliteViewCS );

	// We don't get back the number of records deleted from sqlite, from executing the delete query,
	// so count them righ before deleting them, and return that count.
	int recCount = countAll(whereClause);


#ifndef EVOL_NDEBUG
	util_time::Time t0 = util_time::Time();
#endif

	bool isTracingSql = Debug::isTracing("database.sql");
	bool isTracingSqlSuccess = Debug::isTracing("database.sql.success");
	bool isTracingSqlExceptionDialog = Debug::isTracing("database.exception.dialog");

	string dbgHeader = "SRM::deleteAll - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	int result;
	
	std::string sql = "delete from ";
	sql.append(tableFilename_);
	if (whereClause.size())
		sql.append(" ").append(whereClause);

	if (isTracingSql)
		Debug::trace("database.sql", dbgHeader + sql);

	bool looping = true;
	int numberOfTimeslocked = 0;
	while (looping)
	{
		// Assume we're not going to need to loop
		looping = false;

		sqlite3_stmt *stmt = 0;

		result = sqlite3_prepare_v2(db_, sql.c_str(), (int)sql.length()+1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			if (isTracingSqlSuccess)
				Debug::trace("database.sql.success", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
		}
		else
		{
			if (isTracingSql)
				Debug::trace("database.sql", dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);
			if (isTracingSqlExceptionDialog)
				SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);

			throw edb::DBException(edb::DBException::SQLITE_DELETE_RECORD, "SQLite Error preparing deleteAll query - result:" + inttostring(result) + " - " + sql);
		}
	
		result = sqlite3_step(stmt);
		if ( result == SQLITE_DONE )
		{
			if (isTracingSqlSuccess)
				Debug::trace("database.sql.success", dbgHeader + "succeeded - records counted/deleted:" + inttostring(recCount) + " - " + sql);
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
				Debug::trace("database.sql", dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);
			if (isTracingSqlExceptionDialog)
				SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);

			recCount = 0;
		}

		result = sqlite3_finalize(stmt);
		if (Debug::isTracing("database.sql.finalize"))
			Debug::trace("database.sql.finalize", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql);
	}

#ifndef EVOL_NDEBUG
		util_time::Time t1 = util_time::Time();
		Debug::trace("database.timing.deleteAll", dbgHeader + "Deleting " + tableFilename_ + " took " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return recCount;
}


int SqliteRecordManager::countAll()
{
	return countAll("");
}


int SqliteRecordManager::countAll(string whereClause)
{
	if ( !db_ || !doReads_ )
		return 0;

	Util::CLockCriticalSection lock( sqliteViewCS );

#ifndef EVOL_NDEBUG
	util_time::Time t0 = util_time::Time();
#endif

	int recCount = 0;

	bool isTracingSql = Debug::isTracing("database.sql");
	bool isTracingSqlSuccess = Debug::isTracing("database.sql.success");
	bool isTracingSqlExceptionDialog = Debug::isTracing("database.exception.dialog");

	string dbgHeader = "SRM::countAll - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	int result;
			
	std::string sql = "select count(*) from ";
	sql.append(tableFilename_);
	if (whereClause.size())
		sql.append(" ").append(whereClause);

	if (isTracingSql)
		Debug::trace("database.sql", dbgHeader + sql);

	bool looping = true;
	int numberOfTimeslocked = 0;
	while (looping)
	{
		// Assume we're not going to need to loop
		looping = false;

		sqlite3_stmt *stmt = 0;

		result = sqlite3_prepare_v2(db_, sql.c_str(), (int)sql.length()+1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			if (isTracingSqlSuccess)
				Debug::trace("database.sql.success", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
		}
		else
		{
			if (isTracingSql)
				Debug::trace("database.sql", dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);
			if (isTracingSqlExceptionDialog)
				SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);

			throw edb::DBException(edb::DBException::SQLITE_DELETE_RECORD, "SQLite Error preparing countAll query - result:" + inttostring(result) + " - " + sql);
		}
			
		result = sqlite3_step(stmt);
		if ( result == SQLITE_ROW )
		{
			int data = -1;
			int numFields = sqlite3_data_count(stmt);
			if (numFields > 0)
				data = sqlite3_column_int(stmt, 0);

			recCount = data;

			if (isTracingSqlSuccess)
				Debug::trace("database.sql.success", dbgHeader + "succeeded - records counted/deleted:" + inttostring(recCount) + " - " + sql);
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
				Debug::trace("database.sql", dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);
			if (isTracingSqlExceptionDialog)
				SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);

			recCount = 0;
		}
			
		result = sqlite3_finalize(stmt);
		if (Debug::isTracing("database.sql.finalize"))
			Debug::trace("database.sql.finalize", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql);
	}

#ifndef EVOL_NDEBUG
		util_time::Time t1 = util_time::Time();
		Debug::trace("database.timing.countAll", dbgHeader + "Counting " + tableFilename_ + " took " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return recCount;
}


bool SqliteRecordManager::execute(const string sql)
{
	if ( !db_ || sql.size() == 0 )
		return false;

	Util::CLockCriticalSection lock( sqliteViewCS );

#ifndef EVOL_NDEBUG
	util_time::Time t0 = util_time::Time();
#endif

	bool isTracingSql = Debug::isTracing("database.sql");
	bool isTracingSqlSuccess = Debug::isTracing("database.sql.success");
	bool isTracingSqlExceptionDialog = Debug::isTracing("database.exception.dialog");

	string dbgHeader = "SRM::execute - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	bool retVal = true;
	int result;

	if (isDbTransactionsEnabled())
	{
		if (isTracingSql)
		{
			int i = sqlite3_get_autocommit(db_);
			Debug::trace("database.sql", dbgHeader + "before begin - autocommit:" + inttostring(i));
		}

		int resultexec = sqlite3_exec(db_, "begin", NULL, NULL, NULL);
		Debug::trace("database.sql", dbgHeader + "begin result:" + inttostring(resultexec));

		if (isTracingSql)
		{
			int i = sqlite3_get_autocommit(db_);
			Debug::trace("database.sql", dbgHeader + "after begin, before prepare - autocommit:" + inttostring(i));
		}
	}

	if (isTracingSql)
		Debug::trace("database.sql", dbgHeader + sql);

#if 0
	// Here, I'm commenting out the code that does the prepare/step/finalize, and using exec instead,
	// because I just want to execute an SQL statement, I don't care if any data is returned (currently anyway).

	sqlite3_stmt *stmt = 0;
			
	result = sqlite3_prepare_v2(db_, sql.c_str(), sql.length()+1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		if (isTracingSqlSuccess)
			Debug::trace("database.sql.success", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
	}
	else
	{
		if (isTracingSql)
			Debug::trace("database.sql", dbgHeader + "SQL statement failed to prepare - result:" + inttostring(result) + " - " + sql);
		if (isTracingSqlExceptionDialog)
			SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "SQL statement failed to prepare - result:" + inttostring(result) + " - " + sql);

		throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "SQLite Error preparing SQL statement - result:" + inttostring(result) + " - " + sql);
	}

	result = sqlite3_step(stmt);
	if ( result == SQLITE_DONE )
	{
		if (isTracingSqlSuccess)
			Debug::trace("database.sql.success", dbgHeader + "succeeded - " + sql);
	}
	else if ( result == SQLITE_ROW )
	{
		if (isTracingSqlSuccess)
			Debug::trace("database.sql.success", dbgHeader + "succeeded, found row - " + sql);

		SqliteRecord* rec = 0;
		rec = 0; //new SqliteRecord(stmt, columnNames_, keys);
		if (rec)
		{
			delete rec;
			rec = 0;
		}
	}
	else
	{
		if (isTracingSql)
			Debug::trace("database.sql", dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);
		if (isTracingSqlExceptionDialog)
			SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "failed - result:" + inttostring(result) + " - " + sql);

		retVal = false;
	}

	result = sqlite3_finalize(stmt);
	if (Debug::isTracing("database.sql.finalize"))
		Debug::trace("database.sql.finalize", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql);

#else

	bool looping = true;
	int numberOfTimeslocked = 0;
	while (looping)
	{
		// Assume we're not going to need to loop
		looping = false;

		char *errMsg = 0;
		result = sqlite3_exec(db_, sql.c_str(), NULL, NULL, &errMsg);
		if ( result == SQLITE_OK )
		{
			if (isTracingSqlSuccess)
				Debug::trace("database.sql.success", dbgHeader + "sqlite3_exec succeeded - " + sql);
		}
		else if ( result == SQLITE_ROW )
		{
			// Right now, we're not returning any row(s) returned by the query.
			// The current primary purpose is to execute an SQL statement that doesn't return data,
			// and just let the caller know whether it was successful or not.

			if (isTracingSqlSuccess)
				Debug::trace("database.sql.success", dbgHeader + "sqlite3_exec found row - " + sql);
		}
		else if ( result == SQLITE_BUSY || result == SQLITE_IOERR_BLOCKED )
		{
			if (numberOfTimeslocked++ < MAX_LOCK_COUNT)
				looping = true;
			else
				retVal = false;
			Debug::trace("database.busy", dbgHeader + "busy - lockedCount:" + inttostring(numberOfTimeslocked) + " - " + sql);
			PAL_absclass::PAL_sleep(100);
		}
		else
		{
			retVal = false;
			string eMsg(errMsg);
			if (isTracingSql)
				Debug::trace("database.sql", dbgHeader + "sqlite3_exec failed - result:" + inttostring(result) + " - errMsg:" + eMsg + " - " + sql);
			if (isTracingSqlExceptionDialog)
				SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "sqlite3_exec failed - result:" + inttostring(result) + " - errMsg:" + eMsg + " - " + sql);
		}

	#endif


		if (isDbTransactionsEnabled())
		{
			int resultexec = sqlite3_exec(db_, "commit", NULL, NULL, NULL);
			Debug::trace("database.sql", dbgHeader + "commit result:" + inttostring(resultexec));

			if (isTracingSql)
			{
				int i = sqlite3_get_autocommit(db_);
				Debug::trace("database.sql", dbgHeader + "after commit - autocommit:" + inttostring(i));
			}
		}
	}

	
#ifndef EVOL_NDEBUG
	util_time::Time t1 = util_time::Time();
	Debug::trace("database.timing.execute", dbgHeader + "SQL statement took " + inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");
#endif

	return retVal;
}


int SqliteRecordManager::calculateTotalSpace()
{
	if ( !doReads_ )
		return 0;

//lap todo
	return 1;
}


int SqliteRecordManager::calculateAverageSpace()
{
	if ( !doReads_ )
		return 0;

//lap todo
	return 1;
}


bool SqliteRecordManager::compress()
{
	return true;
}


 bool SqliteRecordManager::reindex()
{
	return true;
}


bool SqliteRecordManager::exists()
{
	return true;
}


bool SqliteRecordManager::remove()
{
	return true;
}


bool SqliteRecordManager::check()
{
	return true;
}


bool SqliteRecordManager::checkIndex()
{
	return true;
}


void SqliteRecordManager::show()
{
#ifndef EVOL_NDEBUG
	if ( !doNothing_ )
	{
		if ( !doReads_ )
			Debug::trace("database.show", "Table " + getTableAlias() + " is set to not read");
		if ( !doSaves_ )
			Debug::trace("database.show", "Table " + getTableAlias() + " is set to not save");
		if ( !doDeletes_ )
			Debug::trace("database.show", "Table " + getTableAlias() + " is set to not delete");
	}
	else
		Debug::trace("database.show", "Table " + getTableAlias() + " is set to do nothing");

	if ( autoIncrementField_.size() )
	{
		Debug::trace("database.show", "Table " + getTableAlias() + " has " + autoIncrementField_ + " as an auto-increment field and " + autoIncrementTag_ + " as an auto-increment tag");
	}
#endif

	SqliteView::show();
}


bool SqliteRecordManager::checkIndexFiles()
{
	return true;
}


bool SqliteRecordManager::copy(bool /*from*/, const util_filesystem::Directory& /*path*/)
{
	return true;
}


bool SqliteRecordManager::backup(const string& /*dir*/)
{
	return true;
}


std::string SqliteRecordManager::getSemiautoIncrementedValue(const std::string& field)
{
	Util::CLockCriticalSection lock( sqliteViewCS );

	bool isTracingSql = Debug::isTracing("database.sql");
	bool isTracingSqlSuccess = Debug::isTracing("database.sql.success");
	bool isTracingSqlExceptionDialog = Debug::isTracing("database.exception.dialog");

	string dbgHeader = "SRM::getSemiautoIncrementedValue - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	if ( sdb_.isCheckAutoIncrementValues() )
	{
		if (isTracingSql)
			Debug::trace("database.sql", dbgHeader + "before calling getAutoIncrementValue, autoIncField:" + field);

		// If isCheckAutoIncrementValues is true, we are supposed to verify that
		// the value supplied is not being used by any other record in the table.
		string nextValue = sdb_.getAutoIncrementValue(field);

		if (isTracingSql)
			Debug::trace("database.sql", dbgHeader + "after calling getAutoIncrementValue, nextValue:" + nextValue);

		if ( doReads_ )
		{
			std::string sql;
			sqlite3_stmt *stmt = 0;
			int result;
				
			sql = "select " + field + " from " + tableFilename_ + " where " + field + " = ?";

			if (isTracingSql)
				Debug::trace("database.sql", dbgHeader + sql);

			result = sqlite3_prepare_v2(db_, sql.c_str(), (int)sql.length()+1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				if (isTracingSqlSuccess)
					Debug::trace("database.sql.success", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
			}
			else
			{
				if (isTracingSql)
					Debug::trace("database.sql", dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);
				if (isTracingSqlExceptionDialog)
					SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);

				throw edb::DBException(edb::DBException::NO_AUTOINCREMENT_TAG,
						"SQLite Error preparing insert query - result:" + inttostring(result) + " - Auto-increment tag not specified for " + tableFilename_ + " table is needed to verify semiauto-increment value.");
			}

			do
			{
				if (isTracingSql)
					Debug::trace("database.sql", dbgHeader + "before bind_text, nextValue:" + nextValue);

				sqlite3_bind_text(stmt, 1, nextValue.c_str(), (int)nextValue.size(), SQLITE_STATIC);
				result = sqlite3_step(stmt);
				if ( result == SQLITE_ROW )
				{
					nextValue.assign(sdb_.getAutoIncrementValue(field));
					sqlite3_reset(stmt);
				}
				else
				{
					if (isTracingSql)
						Debug::trace("database.sql", dbgHeader + "row not found - " + sql);
				}
			} while (result == SQLITE_ROW);
				
			result = sqlite3_finalize(stmt);
			if (Debug::isTracing("database.sql.finalize"))
				Debug::trace("database.sql.finalize", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql);
		}

		return nextValue;
	}
	else
		return sdb_.getAutoIncrementValue(field);
}


void SqliteRecordManager::ensureAutoIncrementValue(const std::string& field, int value)
{
	sdb_.ensureAutoIncrementValue(field, value);
}


void SqliteRecordManager::performAutoIncrements(SqliteRecord* rec)
{
	Util::CLockCriticalSection lock( sqliteViewCS );

	bool isTracingSql = Debug::isTracing("database.sql");
	bool isTracingSqlSuccess = Debug::isTracing("database.sql.success");
	bool isTracingSqlExceptionDialog = Debug::isTracing("database.exception.dialog");

	string dbgHeader = "SRM::performAutoIncrements - this:" + ptrtostring(this) + " sqlite3*:" + ptrtostring(db_) + " - ";

	if ( !isReadOnly() )
	{
		if ( sdb_.isCheckAutoIncrementValues() )
		{
			if (isTracingSql)
				Debug::trace("database.sql", dbgHeader + "before calling getAutoIncrementValue, autoIncField:" + autoIncrementField_);

			// If isCheckAutoIncrementValues is true, we are supposed to verify that
			// the value supplied is not being used by any other record in the table.
			string nextValue = sdb_.getAutoIncrementValue(autoIncrementField_);

			if (isTracingSql)
				Debug::trace("database.sql", dbgHeader + "after calling getAutoIncrementValue, nextValue:" + nextValue);

			if ( doReads_ )
			{
				std::string sql;
				sqlite3_stmt *stmt = 0;
				int result;
					
				sql = "select " + autoIncrementField_ + " from " + tableFilename_ + " where " + autoIncrementField_ + " = ?";

				if (isTracingSql)
					Debug::trace("database.sql", dbgHeader + sql);

				result = sqlite3_prepare_v2(db_, sql.c_str(), (int)sql.length()+1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					if (isTracingSqlSuccess)
						Debug::trace("database.sql.success", dbgHeader + "sqlite3_prepare_v2 succeeded - " + sql);
				}
				else
				{
					if (isTracingSql)
						Debug::trace("database.sql", dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);
					if (isTracingSqlExceptionDialog)
						SessionManagerHelper::getInstance()->addDialog(0, 1, dbgHeader + "query failed to prepare - result:" + inttostring(result) + " - " + sql);

					throw edb::DBException(edb::DBException::NO_AUTOINCREMENT_TAG,
							"SQLite Error preparing insert query - result:" + inttostring(result) + " - Auto-increment tag not specified for " + tableFilename_ + " table is needed to verify semiauto-increment value.");
				}

				do
				{
					if (isTracingSql)
						Debug::trace("database.sql", dbgHeader + "before bind_text, nextValue:" + nextValue);

					sqlite3_bind_text(stmt, 1, nextValue.c_str(), (int)nextValue.size(), SQLITE_STATIC);
					result = sqlite3_step(stmt);
					if ( result == SQLITE_ROW )
					{
						nextValue.assign(sdb_.getAutoIncrementValue(autoIncrementField_));
						sqlite3_reset(stmt);
					}
					else
					{
						if (isTracingSql)
							Debug::trace("database.sql", dbgHeader + "row not found - " + sql);
					}
				} while (result == SQLITE_ROW);
					
				result = sqlite3_finalize(stmt);
				if (Debug::isTracing("database.sql.finalize"))
					Debug::trace("database.sql.finalize", dbgHeader + "finalize - result:" + inttostring(result) + " - " + sql);
			}

			rec->setField(autoIncrementField_, nextValue);
		}
		else
			rec->setField(autoIncrementField_, sdb_.getAutoIncrementValue(autoIncrementField_));
	}
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
