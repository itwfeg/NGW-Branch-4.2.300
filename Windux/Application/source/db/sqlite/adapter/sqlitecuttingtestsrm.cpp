#include <string>


#include "sqlitedatabase.h"
#include "sqlitecuttingtestsrm.h"
#include "sqlitecuttingtest.h"
#include "sqliteoperator.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template<> int SqliteQuery<SqliteCuttingTest>::_dataType_ = 0;
template<> string SqliteQuery<SqliteCuttingTest>::_dataTypeName_ = std::string("SqliteQuery<SqliteCuttingTest>");

int SqliteCuttingTestsRM::_dataType_ = 0;
string SqliteCuttingTestsRM::_dataTypeName_("SqliteCuttingTestsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCuttingTestsRM::DATA_FILENAME("CuttingTests");



SqliteCuttingTestsRM::SqliteCuttingTestsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCuttingTestsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCuttingTest::CUTTINGTESTID_FIELD);
	columnNames_.insert(SqliteCuttingTest::OPERATORNUMBER_FIELD);
	columnNames_.insert(SqliteCuttingTest::STARTDATE_FIELD);
	columnNames_.insert(SqliteCuttingTest::ENDDATE_FIELD);
	columnNames_.insert(SqliteCuttingTest::PRODUCTNUMBER_FIELD);
	columnNames_.insert(SqliteCuttingTest::VENDORNUMBER_FIELD);
	columnNames_.insert(SqliteCuttingTest::PRIMALWEIGHT_FIELD);
	columnNames_.insert(SqliteCuttingTest::PRIMALCOST_FIELD);
	columnNames_.insert(SqliteCuttingTest::LOTNUMBER_FIELD);
	columnNames_.insert(SqliteCuttingTest::PACKINGDATE_FIELD);
	columnNames_.insert(SqliteCuttingTest::PACKINGHOURS_FIELD);
	columnNames_.insert(SqliteCuttingTest::PACKINGMINUTES_FIELD);
	columnNames_.insert(SqliteCuttingTest::COMMENTS_FIELD);
	columnNames_.insert(SqliteCuttingTest::FINALCOMMENTS_FIELD);
	columnNames_.insert(SqliteCuttingTest::STATUS_FIELD);
	columnNames_.insert(SqliteCuttingTest::PERCENTTOTALSHRINKAGE_FIELD);
	columnNames_.insert(SqliteCuttingTest::TOTALWEIGHTOFCUTPRODUCTS_FIELD);
	columnNames_.insert(SqliteCuttingTest::TOTALPRICEOFCUTPRODUCTS_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);
	
	columns_.insert(make_pair(SqliteCuttingTest::CUTTINGTESTID_FIELD,            SqliteField(SqliteCuttingTest::CUTTINGTESTID_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCuttingTest::OPERATORNUMBER_FIELD,           SqliteField(SqliteCuttingTest::OPERATORNUMBER_FIELD,           SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCuttingTest::STARTDATE_FIELD,                SqliteField(SqliteCuttingTest::STARTDATE_FIELD,                SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteCuttingTest::ENDDATE_FIELD,                  SqliteField(SqliteCuttingTest::ENDDATE_FIELD,                  SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteCuttingTest::PRODUCTNUMBER_FIELD,            SqliteField(SqliteCuttingTest::PRODUCTNUMBER_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCuttingTest::VENDORNUMBER_FIELD,             SqliteField(SqliteCuttingTest::VENDORNUMBER_FIELD,             SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCuttingTest::PRIMALWEIGHT_FIELD,             SqliteField(SqliteCuttingTest::PRIMALWEIGHT_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCuttingTest::PRIMALCOST_FIELD,               SqliteField(SqliteCuttingTest::PRIMALCOST_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCuttingTest::LOTNUMBER_FIELD,                SqliteField(SqliteCuttingTest::LOTNUMBER_FIELD,                SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCuttingTest::PACKINGDATE_FIELD,              SqliteField(SqliteCuttingTest::PACKINGDATE_FIELD,              SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteCuttingTest::PACKINGHOURS_FIELD,             SqliteField(SqliteCuttingTest::PACKINGHOURS_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCuttingTest::PACKINGMINUTES_FIELD,           SqliteField(SqliteCuttingTest::PACKINGMINUTES_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCuttingTest::COMMENTS_FIELD,                 SqliteField(SqliteCuttingTest::COMMENTS_FIELD,                 SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCuttingTest::FINALCOMMENTS_FIELD,            SqliteField(SqliteCuttingTest::FINALCOMMENTS_FIELD,            SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCuttingTest::STATUS_FIELD,                   SqliteField(SqliteCuttingTest::STATUS_FIELD,                   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCuttingTest::PERCENTTOTALSHRINKAGE_FIELD,    SqliteField(SqliteCuttingTest::PERCENTTOTALSHRINKAGE_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCuttingTest::TOTALWEIGHTOFCUTPRODUCTS_FIELD, SqliteField(SqliteCuttingTest::TOTALWEIGHTOFCUTPRODUCTS_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCuttingTest::TOTALPRICEOFCUTPRODUCTS_FIELD,  SqliteField(SqliteCuttingTest::TOTALPRICEOFCUTPRODUCTS_FIELD,  SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCuttingTestsRM::~SqliteCuttingTestsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCuttingTestI>* SqliteCuttingTestsRM::retrieveAll()
{
	SqliteQuery<SqliteCuttingTest>* query = new SqliteQuery<SqliteCuttingTest>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBCuttingTestI>* SqliteCuttingTestsRM::retrieveByOperator(const std::string& opNum)
{
	SqliteQuery<SqliteCuttingTest>* query = new SqliteQuery<SqliteCuttingTest>(*this);

	// Sort the cutting tests by Operator number
	string filter = SqliteCuttingTest::OPERATORNUMBER_FIELD + " = " + opNum;
	query->selectSome(filter, SqliteCuttingTest::CUTTINGTESTID_FIELD);

	return query;
}


DBCuttingTestI* SqliteCuttingTestsRM::find(const std::string& id)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteCuttingTest::CUTTINGTESTID_FIELD, SqliteField::INTEGER, id));
	if ( findrec )
		return new SqliteCuttingTest(findrec);

	return 0;
}


DBCuttingTestI* SqliteCuttingTestsRM::createRecord(const std::string& testNumber)
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteCuttingTest* createrec = new SqliteCuttingTest(dbrec);

		if ( createrec )
		{
			createrec->setTestNumber(testNumber);
			return createrec;
		}
		else
			delete dbrec;
	}
	
	return 0;
}


void SqliteCuttingTestsRM::saveRecord(const DBCuttingTestI* rec)
{
	const SqliteCuttingTest* inrec = dynamic_cast<const SqliteCuttingTest*>(rec);

	if ( inrec )
	{
		if ( SqliteRecordManager::save(inrec->getRecord()) )
			modified();
		else
			throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "Unable to save record: CuttingTest.");
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCuttingTestsRM::deleteRecord(const DBCuttingTestI* rec)
{
	const SqliteCuttingTest* inrec = dynamic_cast<const SqliteCuttingTest*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record (CuttingTest) is not a SQLite layer record.");
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
