#include <string>


#include "sqlitedatabase.h"
#include "sqliteproductstatisticsrm.h"
#include "sqliteproductstatistic.h"
#include "sqliteproduct.h"
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
template <> int SqliteQuery<SqliteProductStatistic>::_dataType_ = 0;
template <> string SqliteQuery<SqliteProductStatistic>::_dataTypeName_("SqliteQuery<SqliteProductStatistic>");

int SqliteProductStatisticsRM::_dataType_ = 0;
string SqliteProductStatisticsRM::_dataTypeName_("SqliteProductStatisticsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteProductStatisticsRM::DATA_FILENAME("ProductStatistics");



SqliteProductStatisticsRM::SqliteProductStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductStatisticsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteProductStatistic::PRODUCTKEY_FIELD);
	columnNames_.insert(SqliteProductStatistic::LANGUAGE_FIELD);
	columnNames_.insert(SqliteProductStatistic::HOURNUMBER_FIELD);
	columnNames_.insert(SqliteProductStatistic::TOTALSELECTIONS_FIELD);
	columnNames_.insert(SqliteProductStatistic::TIMETOSELECT_FIELD);
	columnNames_.insert(SqliteProductStatistic::TOTALLABELSPRINTED_FIELD);
	columnNames_.insert(SqliteProductStatistic::RECORDEDDATETIME_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteProductStatistic::PRODUCTKEY_FIELD,         SqliteField(SqliteProductStatistic::PRODUCTKEY_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductStatistic::LANGUAGE_FIELD,           SqliteField(SqliteProductStatistic::LANGUAGE_FIELD,           SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductStatistic::HOURNUMBER_FIELD,         SqliteField(SqliteProductStatistic::HOURNUMBER_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductStatistic::TOTALSELECTIONS_FIELD,    SqliteField(SqliteProductStatistic::TOTALSELECTIONS_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductStatistic::TIMETOSELECT_FIELD,       SqliteField(SqliteProductStatistic::TIMETOSELECT_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductStatistic::TOTALLABELSPRINTED_FIELD, SqliteField(SqliteProductStatistic::TOTALLABELSPRINTED_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductStatistic::RECORDEDDATETIME_FIELD,   SqliteField(SqliteProductStatistic::RECORDEDDATETIME_FIELD,   SqliteField::DATETIME)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteProductStatisticsRM::~SqliteProductStatisticsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBProductStatisticI>* SqliteProductStatisticsRM::retrieveAll()
{
	SqliteQuery<SqliteProductStatistic>* query = new SqliteQuery<SqliteProductStatistic>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBProductStatisticI>* SqliteProductStatisticsRM::retrieveLinkedtoProduct(const DBProductI* prod)
{
	const SqliteProduct* inrec = dynamic_cast<const SqliteProduct*>(prod);

	if ( inrec )
	{
		SqliteQuery<SqliteProductStatistic>* query = new SqliteQuery<SqliteProductStatistic>(*this);

		string filter = SqliteProductStatistic::PRODUCTKEY_FIELD + " = " + inrec->getProductKey();
		query->selectSome(filter, SqliteProductStatistic::HOURNUMBER_FIELD);

		return query;
	}

	return 0;
}


DBQueryI<DBProductStatisticI>* SqliteProductStatisticsRM::retrieveMostSelected(const std::string& language, int hour)
{
	SqliteQuery<SqliteProductStatistic>* query = new SqliteQuery<SqliteProductStatistic>(*this);

	string filter = SqliteProductStatistic::LANGUAGE_FIELD + " = \'" + language + "\' and " + SqliteProductStatistic::HOURNUMBER_FIELD + " = " + inttostring(hour);
	string orderBy = SqliteProductStatistic::TOTALSELECTIONS_FIELD + " desc, " + SqliteProductStatistic::RECORDEDDATETIME_FIELD + " desc ";
	query->selectSomeIndexed(filter, orderBy, true);

	return query;
}


DBQueryI<DBProductStatisticI>* SqliteProductStatisticsRM::retrieveMostPrinted(const std::string& language, int hour)
{
	SqliteQuery<SqliteProductStatistic>* query = new SqliteQuery<SqliteProductStatistic>(*this);

	string filter = SqliteProductStatistic::LANGUAGE_FIELD + " = \'" + language + "\' and " + SqliteProductStatistic::HOURNUMBER_FIELD + " = " + inttostring(hour);
	string orderBy = SqliteProductStatistic::TOTALLABELSPRINTED_FIELD + " desc, " + SqliteProductStatistic::RECORDEDDATETIME_FIELD + " desc ";
	query->selectSomeIndexed(filter, orderBy, true, false);

	return query;
}


DBQueryI<DBProductStatisticI>* SqliteProductStatisticsRM::retrieveDateTimeOrder()
{
	SqliteQuery<SqliteProductStatistic>* query = new SqliteQuery<SqliteProductStatistic>(*this);

	query->selectAll(SqliteProductStatistic::RECORDEDDATETIME_FIELD);

	return query;
}


DBProductStatisticI* SqliteProductStatisticsRM::find(const DBProductI* prod, const std::string& language, const std::string& hour)
{
	const SqliteProduct* inrec = dynamic_cast<const SqliteProduct*>(prod);

	if ( inrec )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteProductStatistic::PRODUCTKEY_FIELD, SqliteField::INTEGER, inrec->getProductKey()));
		keys.push_back(SqliteField(SqliteProductStatistic::LANGUAGE_FIELD, SqliteField::STRING, language));
		keys.push_back(SqliteField(SqliteProductStatistic::HOURNUMBER_FIELD, SqliteField::INTEGER, hour));
		
		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
		if ( findrec )
			return new SqliteProductStatistic(findrec);
	}

	return 0;
}


DBProductStatisticI* SqliteProductStatisticsRM::createRecord(const DBProductI* prod)
{
	const SqliteProduct* inrec = dynamic_cast<const SqliteProduct*>(prod);

	if ( inrec )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteProductStatistic* createrec = new SqliteProductStatistic(dbrec);

			if ( createrec )
			{
				createrec->setProductKey(inrec->getProductKey());
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqliteProductStatisticsRM::saveRecord(const DBProductStatisticI* rec)
{
	const SqliteProductStatistic* inrec = dynamic_cast<const SqliteProductStatistic*>(rec);

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


int SqliteProductStatisticsRM::deleteRecord(const DBProductStatisticI* rec)
{
	const SqliteProductStatistic* inrec = dynamic_cast<const SqliteProductStatistic*>(rec);

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
