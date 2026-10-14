#include <string>


#include "sqlitedatabase.h"
#include "sqlitebatchesrm.h"
#include "sqlitebatch.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template<> int SqliteQuery<SqliteBatch>::_dataType_ = 0;
template<> string SqliteQuery<SqliteBatch>::_dataTypeName_ = std::string("SqliteQuery<SqliteBatch>");

int SqliteBatchesRM::_dataType_ = 0;
string SqliteBatchesRM::_dataTypeName_("SqliteBatchesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteBatchesRM::DATA_FILENAME("Batches");



SqliteBatchesRM::SqliteBatchesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBBatchesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteBatch::NUMBER_FIELD);
	columnNames_.insert(SqliteBatch::SCALEID_FIELD);
	columnNames_.insert(SqliteBatch::VERSION_FIELD);
	columnNames_.insert(SqliteBatch::REMOTEVERSION_FIELD);
	columnNames_.insert(SqliteBatch::ORIGIN_FIELD);
	columnNames_.insert(SqliteBatch::TYPE_FIELD);
	columnNames_.insert(SqliteBatch::CREATEDDATETIME_FIELD);
	columnNames_.insert(SqliteBatch::COMPLETED_FIELD);
	columnNames_.insert(SqliteBatch::COMPLETEDDATETIME_FIELD);
	columnNames_.insert(SqliteBatch::SIGNATURE_FIELD);
	columnNames_.insert(SqliteBatch::PENDINGSYNCHRONIZATION_FIELD);
	columnNames_.insert(SqliteBatch::SYNCHRONIZEDDATETIME_FIELD);
	columnNames_.insert(SqliteBatch::PENDINGAPPLICATION_FIELD);
	columnNames_.insert(SqliteBatch::APPLIEDDATETIME_FIELD);
	columnNames_.insert(SqliteBatch::PENDINGDELETION_FIELD);
	columnNames_.insert(SqliteBatch::DELETEDDATETIME_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);
		
	columns_.insert(make_pair(SqliteBatch::NUMBER_FIELD,                 SqliteField(SqliteBatch::NUMBER_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteBatch::SCALEID_FIELD,                SqliteField(SqliteBatch::SCALEID_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteBatch::VERSION_FIELD,                SqliteField(SqliteBatch::VERSION_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteBatch::REMOTEVERSION_FIELD,          SqliteField(SqliteBatch::REMOTEVERSION_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteBatch::ORIGIN_FIELD,                 SqliteField(SqliteBatch::ORIGIN_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteBatch::TYPE_FIELD,                   SqliteField(SqliteBatch::TYPE_FIELD,                   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteBatch::CREATEDDATETIME_FIELD,        SqliteField(SqliteBatch::CREATEDDATETIME_FIELD,        SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteBatch::COMPLETED_FIELD,              SqliteField(SqliteBatch::COMPLETED_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteBatch::COMPLETEDDATETIME_FIELD,      SqliteField(SqliteBatch::COMPLETEDDATETIME_FIELD,      SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteBatch::SIGNATURE_FIELD,              SqliteField(SqliteBatch::SIGNATURE_FIELD,              SqliteField::STRING)));
	columns_.insert(make_pair(SqliteBatch::PENDINGSYNCHRONIZATION_FIELD, SqliteField(SqliteBatch::PENDINGSYNCHRONIZATION_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteBatch::SYNCHRONIZEDDATETIME_FIELD,   SqliteField(SqliteBatch::SYNCHRONIZEDDATETIME_FIELD,   SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteBatch::PENDINGAPPLICATION_FIELD,     SqliteField(SqliteBatch::PENDINGAPPLICATION_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteBatch::APPLIEDDATETIME_FIELD,        SqliteField(SqliteBatch::APPLIEDDATETIME_FIELD,        SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteBatch::PENDINGDELETION_FIELD,        SqliteField(SqliteBatch::PENDINGDELETION_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteBatch::DELETEDDATETIME_FIELD,        SqliteField(SqliteBatch::DELETEDDATETIME_FIELD,        SqliteField::DATETIME)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteBatchesRM::~SqliteBatchesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBBatchI>* SqliteBatchesRM::retrieveAll()
{
	SqliteQuery<SqliteBatch>* query = new SqliteQuery<SqliteBatch>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBBatchI>* SqliteBatchesRM::retrieveCompleteBatchesOfOrigin(int origin)
{
	SqliteQuery<SqliteBatch>* query = new SqliteQuery<SqliteBatch>(*this);

	// Sort the records by version number
	string filter = SqliteBatch::ORIGIN_FIELD + " = " + inttostring(origin) + " and (" + SqliteBatch::COMPLETED_FIELD + " = 1)";
	query->selectSome(filter, SqliteBatch::VERSION_FIELD);

	return query;
}


DBQueryI<DBBatchI>* SqliteBatchesRM::retrieveCompleteBatches(int version)
{
	SqliteQuery<SqliteBatch>* query = new SqliteQuery<SqliteBatch>(*this);

	// Sort the records by version number
	string filter = "(" + SqliteBatch::COMPLETED_FIELD + " = 1) and (" + SqliteBatch::VERSION_FIELD + " > " + inttostring(version) + ")";
	query->selectSome(filter, SqliteBatch::VERSION_FIELD);

	return query;
}


DBQueryI<DBBatchI>* SqliteBatchesRM::retrieveIncompleteBatches()
{
	SqliteQuery<SqliteBatch>* query = new SqliteQuery<SqliteBatch>(*this);

	// Sort the records by version number
	string filter = "(" + SqliteBatch::COMPLETED_FIELD + " = 0)";
	query->selectSome(filter, SqliteBatch::VERSION_FIELD);

	return query;
}


DBQueryI<DBBatchI>* SqliteBatchesRM::retrievePendingSynchronizationBatches()
{
	SqliteQuery<SqliteBatch>* query = new SqliteQuery<SqliteBatch>(*this);

	// Sort the records by version number
	string filter = "(" + SqliteBatch::PENDINGSYNCHRONIZATION_FIELD + " = 1)";
	query->selectSome(filter, SqliteBatch::VERSION_FIELD);

	return query;
}


DBQueryI<DBBatchI>* SqliteBatchesRM::retrievePendingDeleteBatches()
{
	SqliteQuery<SqliteBatch>* query = new SqliteQuery<SqliteBatch>(*this);

	// Sort the records by version number
	string filter = "(" + SqliteBatch::PENDINGDELETION_FIELD + " = 1)";
	query->selectSome(filter, SqliteBatch::VERSION_FIELD);

	return query;
}


DBQueryI<DBBatchI>* SqliteBatchesRM::retrieveSameSignatureBatches(const std::string& signature)
{
	SqliteQuery<SqliteBatch>* query = new SqliteQuery<SqliteBatch>(*this);

	// Sort the records by version number
	string filter = SqliteBatch::SIGNATURE_FIELD + " = '" + signature + "' and (" + SqliteBatch::COMPLETED_FIELD + " = 1)";
	query->selectSome(filter, SqliteBatch::VERSION_FIELD);

	return query;
}


DBQueryI<DBBatchI>* SqliteBatchesRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteBatch>* query = new SqliteQuery<SqliteBatch>(*this);

	string filter = SqliteBatch::NUMBER_FIELD + ">=" + first + " and " + SqliteBatch::NUMBER_FIELD + "<=" + last;
	query->selectSome(filter);

	return query;
}


DBBatchI* SqliteBatchesRM::findNumber(const string& number)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteBatch::NUMBER_FIELD, SqliteField::INTEGER, number));
	if ( findrec )
		return new SqliteBatch(findrec);

	return 0;
}


DBBatchI* SqliteBatchesRM::findVersion(const std::string& version)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteBatch::VERSION_FIELD, SqliteField::INTEGER, version));
	if ( findrec )
		return new SqliteBatch(findrec);

	return 0;
}


DBBatchI* SqliteBatchesRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteBatch* createrec = new SqliteBatch(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteBatchesRM::saveRecord(const DBBatchI* rec)
{
	const SqliteBatch* inrec = dynamic_cast<const SqliteBatch*>(rec);

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


int SqliteBatchesRM::deleteRecord(const DBBatchI* rec)
{
	const SqliteBatch* inrec = dynamic_cast<const SqliteBatch*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


void SqliteBatchesRM::ensureAutoIncrementValue(int value)
{
	SqliteRecordManager::ensureAutoIncrementValue(SqliteBatch::NUMBER_FIELD, value);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
