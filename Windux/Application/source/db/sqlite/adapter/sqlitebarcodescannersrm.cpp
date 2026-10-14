#include <string>


#include "sqlitedatabase.h"
#include "sqlitebarcodescannersrm.h"
#include "sqlitebarcodescanner.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteBarcodeScanner>::_dataType_ = 0;
template <> string SqliteQuery<SqliteBarcodeScanner>::_dataTypeName_("SqliteQuery<SqliteBarcodeScanner>");

int SqliteBarcodeScannersRM::_dataType_ = 0;
string SqliteBarcodeScannersRM::_dataTypeName_("SqliteBarcodeScannersRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteBarcodeScannersRM::DATA_FILENAME("BarcodeScanData");



SqliteBarcodeScannersRM::SqliteBarcodeScannersRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBBarcodeScannersRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteBarcodeScanner::SCANNERCODE_FIELD);
	columnNames_.insert(SqliteBarcodeScanner::SCANNERPLU_FIELD);

	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteBarcodeScanner::SCANNERCODE_FIELD,  SqliteField(SqliteBarcodeScanner::SCANNERCODE_FIELD,  SqliteField::STRING)));
	columns_.insert(make_pair(SqliteBarcodeScanner::SCANNERPLU_FIELD,   SqliteField(SqliteBarcodeScanner::SCANNERPLU_FIELD,   SqliteField::INTEGER)));
	
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteBarcodeScannersRM::~SqliteBarcodeScannersRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBBarcodeScannerI>* SqliteBarcodeScannersRM::retrieveAll()
{
	SqliteQuery<SqliteBarcodeScanner>* query = new SqliteQuery<SqliteBarcodeScanner>(*this);

	query->selectAll();

	return query;
}


DBBarcodeScannerI* SqliteBarcodeScannersRM::findNumber(const string& scannerCode)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteBarcodeScanner::SCANNERCODE_FIELD, SqliteField::STRING, scannerCode));
		
	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteBarcodeScanner(findrec);

	return 0;
}

// jhc ss elev - I think I am going to need this
//DBBarcodeScannerI* SqliteBarcodeScannersRM::findName(const string& name)
//{
//	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteBarcodeScanner::NAME_FIELD, SqliteField::STRING, name));
//	if ( findrec )
//		return new SqliteBarcodeScanner(findrec);
//
//	return 0;
//}


DBBarcodeScannerI* SqliteBarcodeScannersRM::createRecord(const string& code)
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteBarcodeScanner* createrec = new SqliteBarcodeScanner(dbrec);

		if ( createrec )
		{
			createrec->setScannerCode(code);
			return createrec;
		}
		else
			delete dbrec;
	}

	return 0;
}


void SqliteBarcodeScannersRM::saveRecord(const DBBarcodeScannerI* rec)
{
	const SqliteBarcodeScanner* inrec = dynamic_cast<const SqliteBarcodeScanner*>(rec);

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


int SqliteBarcodeScannersRM::deleteRecord(const DBBarcodeScannerI* rec)
{
	const SqliteBarcodeScanner* inrec = dynamic_cast<const SqliteBarcodeScanner*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteBarcodeScannersRM::deleteAll()
{
	int deleted = SqliteRecordManager::deleteAll();
	return deleted;
}


int SqliteBarcodeScannersRM::countAll()
{
	int count = SqliteRecordManager::countAll();
	return count;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
