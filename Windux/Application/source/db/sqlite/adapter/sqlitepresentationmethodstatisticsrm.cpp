#include <string>


#include "sqlitedatabase.h"
#include "sqlitepresentationmethodstatisticsrm.h"
#include "sqlitepresentationmethodstatistic.h"
#include "sqlitepresentationmethod.h"
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
template <> int SqliteQuery<SqlitePresentationMethodStatistic>::_dataType_ = 0;
template <> string SqliteQuery<SqlitePresentationMethodStatistic>::_dataTypeName_("SqliteQuery<SqlitePresentationMethodStatistic>");

int SqlitePresentationMethodStatisticsRM::_dataType_ = 0;
string SqlitePresentationMethodStatisticsRM::_dataTypeName_("SqlitePresentationMethodStatisticsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqlitePresentationMethodStatisticsRM::DATA_FILENAME("PresentationMethodStatistics");



SqlitePresentationMethodStatisticsRM::SqlitePresentationMethodStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBPresentationMethodStatisticsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqlitePresentationMethodStatistic::PRESENTATIONMETHODTYPE_FIELD);
	columnNames_.insert(SqlitePresentationMethodStatistic::LANGUAGE_FIELD);
	columnNames_.insert(SqlitePresentationMethodStatistic::HOURNUMBER_FIELD);
	columnNames_.insert(SqlitePresentationMethodStatistic::TOTALMETHODSELECTIONS_FIELD);
	columnNames_.insert(SqlitePresentationMethodStatistic::TOTALCATEGORYSELECTIONS_FIELD);
	columnNames_.insert(SqlitePresentationMethodStatistic::TOTALPRODUCTSELECTIONS_FIELD);
	columnNames_.insert(SqlitePresentationMethodStatistic::TOTALLABELSPRINTED_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);
		
	columns_.insert(make_pair(SqlitePresentationMethodStatistic::PRESENTATIONMETHODTYPE_FIELD,  SqliteField(SqlitePresentationMethodStatistic::PRESENTATIONMETHODTYPE_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqlitePresentationMethodStatistic::LANGUAGE_FIELD,                SqliteField(SqlitePresentationMethodStatistic::LANGUAGE_FIELD,                SqliteField::STRING)));
	columns_.insert(make_pair(SqlitePresentationMethodStatistic::HOURNUMBER_FIELD,              SqliteField(SqlitePresentationMethodStatistic::HOURNUMBER_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqlitePresentationMethodStatistic::TOTALMETHODSELECTIONS_FIELD,   SqliteField(SqlitePresentationMethodStatistic::TOTALMETHODSELECTIONS_FIELD,   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqlitePresentationMethodStatistic::TOTALCATEGORYSELECTIONS_FIELD, SqliteField(SqlitePresentationMethodStatistic::TOTALCATEGORYSELECTIONS_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqlitePresentationMethodStatistic::TOTALPRODUCTSELECTIONS_FIELD,  SqliteField(SqlitePresentationMethodStatistic::TOTALPRODUCTSELECTIONS_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqlitePresentationMethodStatistic::TOTALLABELSPRINTED_FIELD,      SqliteField(SqlitePresentationMethodStatistic::TOTALLABELSPRINTED_FIELD,      SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqlitePresentationMethodStatisticsRM::~SqlitePresentationMethodStatisticsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBPresentationMethodStatisticI>* SqlitePresentationMethodStatisticsRM::retrieveAll()
{
	SqliteQuery<SqlitePresentationMethodStatistic>* query = new SqliteQuery<SqlitePresentationMethodStatistic>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBPresentationMethodStatisticI>* SqlitePresentationMethodStatisticsRM::retrieveLinkedtoPresentationMethod(const DBPresentationMethodI* method)
{
	const SqlitePresentationMethod* inrec = dynamic_cast<const SqlitePresentationMethod*>(method);

	if ( inrec )
	{
		SqliteQuery<SqlitePresentationMethodStatistic>* query = new SqliteQuery<SqlitePresentationMethodStatistic>(*this);

		string filter = SqlitePresentationMethodStatistic::PRESENTATIONMETHODTYPE_FIELD + " = \'" + inrec->getType() + "\'";
		query->selectSome(filter, SqlitePresentationMethodStatistic::HOURNUMBER_FIELD);

		return query;
	}

	return 0;
}


DBPresentationMethodStatisticI* SqlitePresentationMethodStatisticsRM::find(const DBPresentationMethodI* method, const std::string& lang, const std::string& hour)
{
	const SqlitePresentationMethod* inrec = dynamic_cast<const SqlitePresentationMethod*>(method);

	if ( inrec )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqlitePresentationMethodStatistic::PRESENTATIONMETHODTYPE_FIELD, SqliteField::INTEGER, inrec->getType()));
		keys.push_back(SqliteField(SqlitePresentationMethodStatistic::LANGUAGE_FIELD, SqliteField::STRING, lang));
		keys.push_back(SqliteField(SqlitePresentationMethodStatistic::HOURNUMBER_FIELD, SqliteField::INTEGER, hour));
		
		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);

		if ( findrec )
			return new SqlitePresentationMethodStatistic(findrec);
	}

	return 0;
}


DBPresentationMethodStatisticI* SqlitePresentationMethodStatisticsRM::createRecord(const DBPresentationMethodI* method)
{
	const SqlitePresentationMethod* inrec = dynamic_cast<const SqlitePresentationMethod*>(method);

	if ( inrec )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqlitePresentationMethodStatistic* createrec = new SqlitePresentationMethodStatistic(dbrec);

			if ( createrec )
			{
				createrec->setPresentationMethodType(inrec->getType());
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqlitePresentationMethodStatisticsRM::saveRecord(const DBPresentationMethodStatisticI* rec)
{
	const SqlitePresentationMethodStatistic* inrec = dynamic_cast<const SqlitePresentationMethodStatistic*>(rec);

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


int SqlitePresentationMethodStatisticsRM::deleteRecord(const DBPresentationMethodStatisticI* rec)
{
	const SqlitePresentationMethodStatistic* inrec = dynamic_cast<const SqlitePresentationMethodStatistic*>(rec);

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
