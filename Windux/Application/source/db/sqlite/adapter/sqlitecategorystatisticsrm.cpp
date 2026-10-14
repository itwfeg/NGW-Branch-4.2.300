#include <string>


#include "sqlitedatabase.h"
#include "sqlitecategorystatisticsrm.h"
#include "sqlitecategorystatistic.h"
#include "sqlitecategory.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template<> int SqliteQuery<SqliteCategoryStatistic>::_dataType_ = 0;
template<> string SqliteQuery<SqliteCategoryStatistic>::_dataTypeName_("SqliteQuery<SqliteCategoryStatistic>");

int SqliteCategoryStatisticsRM::_dataType_ = 0;
string SqliteCategoryStatisticsRM::_dataTypeName_("SqliteCategoryStatisticsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCategoryStatisticsRM::DATA_FILENAME("CategoryStatistics");



SqliteCategoryStatisticsRM::SqliteCategoryStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCategoryStatisticsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCategoryStatistic::CATEGORYNUMBER_FIELD);
	columnNames_.insert(SqliteCategoryStatistic::LANGUAGE_FIELD);
	columnNames_.insert(SqliteCategoryStatistic::HOURNUMBER_FIELD);
	columnNames_.insert(SqliteCategoryStatistic::TOTALSELECTIONS_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCategoryStatistic::CATEGORYNUMBER_FIELD,  SqliteField(SqliteCategoryStatistic::CATEGORYNUMBER_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCategoryStatistic::LANGUAGE_FIELD,        SqliteField(SqliteCategoryStatistic::LANGUAGE_FIELD,        SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCategoryStatistic::HOURNUMBER_FIELD,      SqliteField(SqliteCategoryStatistic::HOURNUMBER_FIELD,      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCategoryStatistic::TOTALSELECTIONS_FIELD, SqliteField(SqliteCategoryStatistic::TOTALSELECTIONS_FIELD, SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCategoryStatisticsRM::~SqliteCategoryStatisticsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCategoryStatisticI>* SqliteCategoryStatisticsRM::retrieveAll()
{
	SqliteQuery<SqliteCategoryStatistic>* query = new SqliteQuery<SqliteCategoryStatistic>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBCategoryStatisticI>* SqliteCategoryStatisticsRM::retrieveLinkedtoCategory(const DBCategoryI* cat)
{
	const SqliteCategory* inrec = dynamic_cast<const SqliteCategory*>(cat);

	if ( inrec )
	{
		SqliteQuery<SqliteCategoryStatistic>* query = new SqliteQuery<SqliteCategoryStatistic>(*this);

		string filter = SqliteCategoryStatistic::CATEGORYNUMBER_FIELD + "=" + inrec->getNumber();
		query->selectSome(filter, SqliteCategoryStatistic::HOURNUMBER_FIELD);

		return query;
	}

	return 0;
}


DBQueryI<DBCategoryStatisticI>* SqliteCategoryStatisticsRM::retrieveMostSelected(const std::string& language, int hour)
{
	SqliteQuery<SqliteCategoryStatistic>* query = new SqliteQuery<SqliteCategoryStatistic>(*this);

	string filter = SqliteCategoryStatistic::LANGUAGE_FIELD + " = \'" + language + "\' and " + SqliteCategoryStatistic::HOURNUMBER_FIELD + " = " + inttostring(hour);
	query->selectSome(filter, SqliteCategoryStatistic::TOTALSELECTIONS_FIELD);

	return query;
}


DBCategoryStatisticI* SqliteCategoryStatisticsRM::find(const DBCategoryI* cat, const std::string& language, const std::string& hour)
{
	const SqliteCategory* inrec = dynamic_cast<const SqliteCategory*>(cat);

	if ( inrec )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteCategoryStatistic::LANGUAGE_FIELD, SqliteField::STRING, language));
		keys.push_back(SqliteField(SqliteCategoryStatistic::HOURNUMBER_FIELD, SqliteField::INTEGER, hour));
		keys.push_back(SqliteField(SqliteCategoryStatistic::CATEGORYNUMBER_FIELD, SqliteField::INTEGER, inrec->getNumber()));
		
		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);

		if ( findrec )
			return new SqliteCategoryStatistic(findrec);
	}

	return 0;
}


DBCategoryStatisticI* SqliteCategoryStatisticsRM::createRecord(const DBCategoryI* cat)
{
	const SqliteCategory* inrec = dynamic_cast<const SqliteCategory*>(cat);

	if ( inrec )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteCategoryStatistic* createrec = new SqliteCategoryStatistic(dbrec);

			if ( createrec )
			{
				createrec->setCategoryNumber(inrec->getNumber());
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqliteCategoryStatisticsRM::saveRecord(const DBCategoryStatisticI* rec)
{
	const SqliteCategoryStatistic* inrec = dynamic_cast<const SqliteCategoryStatistic*>(rec);

	if ( inrec )
	{
		if ( SqliteRecordManager::save(inrec->getRecord()) )
			modified();
		else
			throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "Unable to save record.");
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCategoryStatisticsRM::deleteRecord(const DBCategoryStatisticI* rec)
{
	const SqliteCategoryStatistic* inrec = dynamic_cast<const SqliteCategoryStatistic*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
