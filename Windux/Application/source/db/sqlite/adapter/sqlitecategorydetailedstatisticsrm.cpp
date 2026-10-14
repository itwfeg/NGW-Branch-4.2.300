#include <string>


#include "sqlitedatabase.h"
#include "sqlitecategorydetailedstatisticsrm.h"
#include "sqlitecategorydetailedstatistic.h"
#include "sqlitecategorystatistic.h"
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
template<> int SqliteQuery<SqliteCategoryDetailedStatistic>::_dataType_ = 0;
template<> string SqliteQuery<SqliteCategoryDetailedStatistic>::_dataTypeName_("SqliteQuery<SqliteCategoryDetailedStatistic>");

int SqliteCategoryDetailedStatisticsRM::_dataType_ = 0;
string SqliteCategoryDetailedStatisticsRM::_dataTypeName_("SqliteCategoryDetailedStatisticsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCategoryDetailedStatisticsRM::DATA_FILENAME("CategoryDetailedStatistics");



SqliteCategoryDetailedStatisticsRM::SqliteCategoryDetailedStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCategoryDetailedStatisticsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCategoryDetailedStatistic::CATEGORYNUMBER_FIELD);
	columnNames_.insert(SqliteCategoryDetailedStatistic::CATEGORYSTATISTICLANGUAGE_FIELD);
	columnNames_.insert(SqliteCategoryDetailedStatistic::CATEGORYSTATISTICHOURNUMBER_FIELD);
	columnNames_.insert(SqliteCategoryDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD);
	columnNames_.insert(SqliteCategoryDetailedStatistic::SELECTIONS_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCategoryDetailedStatistic::CATEGORYNUMBER_FIELD,              SqliteField(SqliteCategoryDetailedStatistic::CATEGORYNUMBER_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCategoryDetailedStatistic::CATEGORYSTATISTICLANGUAGE_FIELD,   SqliteField(SqliteCategoryDetailedStatistic::CATEGORYSTATISTICLANGUAGE_FIELD,   SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCategoryDetailedStatistic::CATEGORYSTATISTICHOURNUMBER_FIELD, SqliteField(SqliteCategoryDetailedStatistic::CATEGORYSTATISTICHOURNUMBER_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCategoryDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD,      SqliteField(SqliteCategoryDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD,      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCategoryDetailedStatistic::SELECTIONS_FIELD,                  SqliteField(SqliteCategoryDetailedStatistic::SELECTIONS_FIELD,                  SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCategoryDetailedStatisticsRM::~SqliteCategoryDetailedStatisticsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCategoryDetailedStatisticI>* SqliteCategoryDetailedStatisticsRM::retrieveAll()
{
	SqliteQuery<SqliteCategoryDetailedStatistic>* query = new SqliteQuery<SqliteCategoryDetailedStatistic>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBCategoryDetailedStatisticI>* SqliteCategoryDetailedStatisticsRM::retrieveLinkedtoCategoryStatistic(const DBCategoryStatisticI* stat)
{
	const SqliteCategoryStatistic* inrec = dynamic_cast<const SqliteCategoryStatistic*>(stat);

	if ( inrec )
	{
		SqliteQuery<SqliteCategoryDetailedStatistic>* query = new SqliteQuery<SqliteCategoryDetailedStatistic>(*this);

		string filter = SqliteCategoryStatistic::LANGUAGE_FIELD + "=\'" + inrec->getLanguage() +
		                "\' and " + SqliteCategoryStatistic::HOURNUMBER_FIELD + "=" + inrec->getHourNumber() +
		                " and " + SqliteCategoryStatistic::CATEGORYNUMBER_FIELD + "=" + inrec->getCategoryNumber();
		query->selectSome(filter, SqliteCategoryDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD);

		return query;
	}

	return 0;
}


DBCategoryDetailedStatisticI* SqliteCategoryDetailedStatisticsRM::find(const DBPresentationMethodI* method, const DBCategoryStatisticI* stat)
{
	const SqlitePresentationMethod* inmethod = dynamic_cast<const SqlitePresentationMethod*>(method);
	const SqliteCategoryStatistic* instat = dynamic_cast<const SqliteCategoryStatistic*>(stat);

	if ( inmethod && instat )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteCategoryDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD, SqliteField::INTEGER, inmethod->getType()));
		keys.push_back(SqliteField(SqliteCategoryDetailedStatistic::CATEGORYSTATISTICLANGUAGE_FIELD, SqliteField::STRING, instat->getLanguage()));
		keys.push_back(SqliteField(SqliteCategoryDetailedStatistic::CATEGORYSTATISTICHOURNUMBER_FIELD, SqliteField::INTEGER, instat->getHourNumber()));
		keys.push_back(SqliteField(SqliteCategoryDetailedStatistic::CATEGORYNUMBER_FIELD, SqliteField::INTEGER, instat->getCategoryNumber()));
		
		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);

		if ( findrec )
			return new SqliteCategoryDetailedStatistic(findrec);
	}

	return 0;
}


DBCategoryDetailedStatisticI* SqliteCategoryDetailedStatisticsRM::createRecord(const DBPresentationMethodI* method, const DBCategoryStatisticI* stat)
{
	const SqlitePresentationMethod* inmethod = dynamic_cast<const SqlitePresentationMethod*>(method);
	const SqliteCategoryStatistic* instat = dynamic_cast<const SqliteCategoryStatistic*>(stat);

	if ( inmethod && instat )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteCategoryDetailedStatistic* createrec = new SqliteCategoryDetailedStatistic(dbrec);

			if ( createrec )
			{
				createrec->setPresentationMethodType(inmethod->getType());
				createrec->setCategoryNumber(instat->getCategoryNumber());
				createrec->setCategoryStatisticLanguage(instat->getLanguage());
				createrec->setCategoryStatisticHourNumber(instat->getHourNumber());
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqliteCategoryDetailedStatisticsRM::saveRecord(const DBCategoryDetailedStatisticI* rec)
{
	const SqliteCategoryDetailedStatistic* inrec = dynamic_cast<const SqliteCategoryDetailedStatistic*>(rec);

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


int SqliteCategoryDetailedStatisticsRM::deleteRecord(const DBCategoryDetailedStatisticI* rec)
{
	const SqliteCategoryDetailedStatistic* inrec = dynamic_cast<const SqliteCategoryDetailedStatistic*>(rec);

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
