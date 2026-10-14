#include <string>


#include "sqlitedatabase.h"
#include "sqlitepresentationmethoddetailedstatisticsrm.h"
#include "sqlitepresentationmethoddetailedstatistic.h"
#include "sqlitepresentationmethod.h"
#include "sqlitepresentationmethodstatistic.h"
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
template <> int SqliteQuery<SqlitePresentationMethodDetailedStatistic>::_dataType_ = 0;
template <> string SqliteQuery<SqlitePresentationMethodDetailedStatistic>::_dataTypeName_("SqliteQuery<SqlitePresentationMethodDetailedStatistic>");

int SqlitePresentationMethodDetailedStatisticsRM::_dataType_ = 0;
string SqlitePresentationMethodDetailedStatisticsRM::_dataTypeName_("SqlitePresentationMethodDetailedStatisticsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqlitePresentationMethodDetailedStatisticsRM::DATA_FILENAME("PresentationMethodDetailedStatistics");



SqlitePresentationMethodDetailedStatisticsRM::SqlitePresentationMethodDetailedStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBPresentationMethodDetailedStatisticsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD);
	columnNames_.insert(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICLANGUAGE_FIELD);
	columnNames_.insert(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICHOURNUMBER_FIELD);
	columnNames_.insert(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICMETHODTYPE_FIELD);
	columnNames_.insert(SqlitePresentationMethodDetailedStatistic::SELECTIONS_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);
	
	columns_.insert(make_pair(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD,                SqliteField(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICLANGUAGE_FIELD,   SqliteField(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICLANGUAGE_FIELD,   SqliteField::STRING)));
	columns_.insert(make_pair(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICHOURNUMBER_FIELD, SqliteField(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICHOURNUMBER_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICMETHODTYPE_FIELD, SqliteField(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICMETHODTYPE_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqlitePresentationMethodDetailedStatistic::SELECTIONS_FIELD,                            SqliteField(SqlitePresentationMethodDetailedStatistic::SELECTIONS_FIELD,                            SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_,                                                                            SqliteField(checkField_,                                                                            SqliteField::INTEGER, 1)));
}


SqlitePresentationMethodDetailedStatisticsRM::~SqlitePresentationMethodDetailedStatisticsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBPresentationMethodDetailedStatisticI>* SqlitePresentationMethodDetailedStatisticsRM::retrieveAll()
{
	SqliteQuery<SqlitePresentationMethodDetailedStatistic>* query = new SqliteQuery<SqlitePresentationMethodDetailedStatistic>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBPresentationMethodDetailedStatisticI>* SqlitePresentationMethodDetailedStatisticsRM::retrieveLinkedtoPresentationMethodStatistic(const DBPresentationMethodStatisticI* stat)
{
	const SqlitePresentationMethodStatistic* inrec = dynamic_cast<const SqlitePresentationMethodStatistic*>(stat);

	if ( inrec )
	{
		SqliteQuery<SqlitePresentationMethodDetailedStatistic>* query = new SqliteQuery<SqlitePresentationMethodDetailedStatistic>(*this);

		string filter = SqlitePresentationMethodStatistic::PRESENTATIONMETHODTYPE_FIELD + " = " + inrec->getPresentationMethodType() +
		                " and " + SqlitePresentationMethodStatistic::LANGUAGE_FIELD + " = \'" + inrec->getLanguage() +
		                "\' and " + SqlitePresentationMethodStatistic::HOURNUMBER_FIELD + " = " + inrec->getHourNumber();
		query->selectSome(filter, SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD);

		return query;
	}

	return 0;
}


DBPresentationMethodDetailedStatisticI* SqlitePresentationMethodDetailedStatisticsRM::find(const DBPresentationMethodI* method, const DBPresentationMethodStatisticI* stat)
{
	const SqlitePresentationMethod* inmethod = dynamic_cast<const SqlitePresentationMethod*>(method);
	const SqlitePresentationMethodStatistic* instat = dynamic_cast<const SqlitePresentationMethodStatistic*>(stat);

	if ( inmethod && instat )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD, SqliteField::INTEGER, inmethod->getType()));
		keys.push_back(SqliteField(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICMETHODTYPE_FIELD, SqliteField::INTEGER, instat->getPresentationMethodType()));
		keys.push_back(SqliteField(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICLANGUAGE_FIELD, SqliteField::STRING, instat->getLanguage()));
		keys.push_back(SqliteField(SqlitePresentationMethodDetailedStatistic::PRESENTATIONMETHODSTATISTICHOURNUMBER_FIELD, SqliteField::INTEGER, instat->getHourNumber()));
		
		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);

		if ( findrec )
			return new SqlitePresentationMethodDetailedStatistic(findrec);
	}

	return 0;
}


DBPresentationMethodDetailedStatisticI* SqlitePresentationMethodDetailedStatisticsRM::createRecord(const DBPresentationMethodI* method, const DBPresentationMethodStatisticI* stat)
{
	const SqlitePresentationMethod* inmethod = dynamic_cast<const SqlitePresentationMethod*>(method);
	const SqlitePresentationMethodStatistic* instat = dynamic_cast<const SqlitePresentationMethodStatistic*>(stat);

	if ( inmethod && instat )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqlitePresentationMethodDetailedStatistic* createrec = new SqlitePresentationMethodDetailedStatistic(dbrec);

			if ( createrec )
			{
				createrec->setPresentationMethodType(inmethod->getType());
				createrec->setPresentationMethodStatisticMethodType(instat->getPresentationMethodType());
				createrec->setPresentationMethodStatisticHourNumber(instat->getHourNumber());
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqlitePresentationMethodDetailedStatisticsRM::saveRecord(const DBPresentationMethodDetailedStatisticI* rec)
{
	const SqlitePresentationMethodDetailedStatistic* inrec = dynamic_cast<const SqlitePresentationMethodDetailedStatistic*>(rec);

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


int SqlitePresentationMethodDetailedStatisticsRM::deleteRecord(const DBPresentationMethodDetailedStatisticI* rec)
{
	const SqlitePresentationMethodDetailedStatistic* inrec = dynamic_cast<const SqlitePresentationMethodDetailedStatistic*>(rec);

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
