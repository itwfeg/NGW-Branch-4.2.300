#include <string>


#include "sqlitedatabase.h"
#include "sqliteproductdetailedstatisticsrm.h"
#include "sqliteproductdetailedstatistic.h"
#include "sqlitepresentationmethod.h"
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
template <> int SqliteQuery<SqliteProductDetailedStatistic>::_dataType_ = 0;
template <> string SqliteQuery<SqliteProductDetailedStatistic>::_dataTypeName_("SqliteQuery<SqliteProductDetailedStatistic>");

int SqliteProductDetailedStatisticsRM::_dataType_ = 0;
string SqliteProductDetailedStatisticsRM::_dataTypeName_("SqliteProductDetailedStatisticsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteProductDetailedStatisticsRM::DATA_FILENAME("ProductDetailedStatistics");



SqliteProductDetailedStatisticsRM::SqliteProductDetailedStatisticsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductDetailedStatisticsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteProductDetailedStatistic::PRODUCTKEY_FIELD);
	columnNames_.insert(SqliteProductDetailedStatistic::PRODUCTSTATISTICLANGUAGE_FIELD);
	columnNames_.insert(SqliteProductDetailedStatistic::PRODUCTSTATISTICHOURNUMBER_FIELD);
	columnNames_.insert(SqliteProductDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD);
	columnNames_.insert(SqliteProductDetailedStatistic::SELECTIONS_FIELD);
	columnNames_.insert(SqliteProductDetailedStatistic::LABELSPRINTED_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteProductDetailedStatistic::PRODUCTKEY_FIELD,                 SqliteField(SqliteProductDetailedStatistic::PRODUCTKEY_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDetailedStatistic::PRODUCTSTATISTICLANGUAGE_FIELD,   SqliteField(SqliteProductDetailedStatistic::PRODUCTSTATISTICLANGUAGE_FIELD,   SqliteField::STRING)));
	columns_.insert(make_pair(SqliteProductDetailedStatistic::PRODUCTSTATISTICHOURNUMBER_FIELD, SqliteField(SqliteProductDetailedStatistic::PRODUCTSTATISTICHOURNUMBER_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD,     SqliteField(SqliteProductDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDetailedStatistic::SELECTIONS_FIELD,                 SqliteField(SqliteProductDetailedStatistic::SELECTIONS_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductDetailedStatistic::LABELSPRINTED_FIELD,              SqliteField(SqliteProductDetailedStatistic::LABELSPRINTED_FIELD,              SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteProductDetailedStatisticsRM::~SqliteProductDetailedStatisticsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBProductDetailedStatisticI>* SqliteProductDetailedStatisticsRM::retrieveAll()
{
	SqliteQuery<SqliteProductDetailedStatistic>* query = new SqliteQuery<SqliteProductDetailedStatistic>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBProductDetailedStatisticI>* SqliteProductDetailedStatisticsRM::retrieveLinkedtoProductStatistic(const DBProductStatisticI* stat)
{
	const SqliteProductStatistic* inrec = dynamic_cast<const SqliteProductStatistic*>(stat);

	if ( inrec )
	{
		SqliteQuery<SqliteProductDetailedStatistic>* query = new SqliteQuery<SqliteProductDetailedStatistic>(*this);

		string filter = SqliteProductStatistic::LANGUAGE_FIELD + " = \'" + inrec->getLanguage()
		                + "\' and " + SqliteProductStatistic::HOURNUMBER_FIELD + " = " + inrec->getHourNumber()
		                + " and " + SqliteProduct::PRODUCTID_FIELD + " = " + inrec->getProductKey();
		query->selectSome(filter, SqlitePresentationMethod::TYPE_FIELD);

		return query;
	}

	return 0;
}


DBProductDetailedStatisticI* SqliteProductDetailedStatisticsRM::find(const DBPresentationMethodI* method, const DBProductStatisticI* stat)
{
	const SqlitePresentationMethod* inmethod = dynamic_cast<const SqlitePresentationMethod*>(method);
	const SqliteProductStatistic* instat = dynamic_cast<const SqliteProductStatistic*>(stat);

	if ( inmethod && instat )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteProductDetailedStatistic::PRESENTATIONMETHODTYPE_FIELD, SqliteField::INTEGER, inmethod->getType()));
		keys.push_back(SqliteField(SqliteProductDetailedStatistic::PRODUCTKEY_FIELD, SqliteField::INTEGER, instat->getProductKey()));
		keys.push_back(SqliteField(SqliteProductDetailedStatistic::PRODUCTSTATISTICLANGUAGE_FIELD, SqliteField::STRING, instat->getLanguage()));
		keys.push_back(SqliteField(SqliteProductDetailedStatistic::PRODUCTSTATISTICHOURNUMBER_FIELD, SqliteField::INTEGER, instat->getHourNumber()));
		
		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
		if ( findrec )
			return new SqliteProductDetailedStatistic(findrec);
	}

	return 0;
}


DBProductDetailedStatisticI* SqliteProductDetailedStatisticsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteProductDetailedStatistic* createrec = new SqliteProductDetailedStatistic(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteProductDetailedStatisticsRM::saveRecord(const DBProductDetailedStatisticI* rec)
{
	const SqliteProductDetailedStatistic* inrec = dynamic_cast<const SqliteProductDetailedStatistic*>(rec);

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


int SqliteProductDetailedStatisticsRM::deleteRecord(const DBProductDetailedStatisticI* rec)
{
	const SqliteProductDetailedStatistic* inrec = dynamic_cast<const SqliteProductDetailedStatistic*>(rec);

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
