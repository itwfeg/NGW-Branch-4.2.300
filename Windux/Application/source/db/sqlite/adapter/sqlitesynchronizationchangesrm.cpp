#include <string>


#include "sqlitedatabase.h"
#include "sqlitesynchronizationchangesrm.h"
#include "sqlitesynchronizationchange.h"
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
template <> int SqliteQuery<SqliteSynchronizationChange>::_dataType_ = 0;
template <> string SqliteQuery<SqliteSynchronizationChange>::_dataTypeName_("SqliteQuery<SqliteSynchronizationChange>");

int SqliteSynchronizationChangesRM::_dataType_ = 0;
string SqliteSynchronizationChangesRM::_dataTypeName_("SqliteSynchronizationChangesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteSynchronizationChangesRM::DATA_FILENAME("SynchronizationChanges");



SqliteSynchronizationChangesRM::SqliteSynchronizationChangesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSynchronizationChangesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteSynchronizationChange::IDENTIFIER_FIELD);
	columnNames_.insert(SqliteSynchronizationChange::BATCHNUMBER_FIELD);
	columnNames_.insert(SqliteSynchronizationChange::APPLICATIONDEVICE_FIELD);
	columnNames_.insert(SqliteSynchronizationChange::APPLICATIONEVENT_FIELD);
	columnNames_.insert(SqliteSynchronizationChange::APPLICATIONFORMAT_FIELD);
	columnNames_.insert(SqliteSynchronizationChange::CHANGE_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteSynchronizationChange::IDENTIFIER_FIELD,        SqliteField(SqliteSynchronizationChange::IDENTIFIER_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSynchronizationChange::BATCHNUMBER_FIELD,       SqliteField(SqliteSynchronizationChange::BATCHNUMBER_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSynchronizationChange::APPLICATIONDEVICE_FIELD, SqliteField(SqliteSynchronizationChange::APPLICATIONDEVICE_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSynchronizationChange::APPLICATIONEVENT_FIELD,  SqliteField(SqliteSynchronizationChange::APPLICATIONEVENT_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSynchronizationChange::APPLICATIONFORMAT_FIELD, SqliteField(SqliteSynchronizationChange::APPLICATIONFORMAT_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSynchronizationChange::CHANGE_FIELD,            SqliteField(SqliteSynchronizationChange::CHANGE_FIELD,            SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteSynchronizationChangesRM::~SqliteSynchronizationChangesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBSynchronizationChangeI>* SqliteSynchronizationChangesRM::retrieveAll()
{
	SqliteQuery<SqliteSynchronizationChange>* query = new SqliteQuery<SqliteSynchronizationChange>(*this);

	query->selectAll();

	return query;
}


/*
DBQueryI<DBSynchronizationChangeI>* SqliteSynchronizationChangesRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteSynchronizationChange>* query = new SqliteQuery<SqliteSynchronizationChange>(*this);

	string filter = SqliteSynchronizationChange::NUMBER_FIELD + " >= " + first + " and " + SqliteSynchronizationChange::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteSynchronizationChange::NUMBER_FIELD);

	return query;
}
*/


DBQueryI<DBSynchronizationChangeI>* SqliteSynchronizationChangesRM::retrieveLinkedToBatch(const DBBatchI* batch)
{
	const SqliteBatch* inrec = dynamic_cast<const SqliteBatch*>(batch);

	if ( inrec )
	{
		SqliteQuery<SqliteSynchronizationChange>* query = new SqliteQuery<SqliteSynchronizationChange>(*this);

		string filter = SqliteSynchronizationChange::BATCHNUMBER_FIELD + " = " + inrec->getNumber();
		query->selectSome(filter, SqliteSynchronizationChange::IDENTIFIER_FIELD);

		return query;
	}

	return 0;
}


DBQueryI<DBSynchronizationChangeI>* SqliteSynchronizationChangesRM::retrieveLinkedToBatch(const std::string& batchNumber)
{
	SqliteQuery<SqliteSynchronizationChange>* query = new SqliteQuery<SqliteSynchronizationChange>(*this);

	string filter = SqliteSynchronizationChange::BATCHNUMBER_FIELD + " = " + batchNumber;
	query->selectSome(filter, SqliteSynchronizationChange::IDENTIFIER_FIELD);

	return query;
}


DBSynchronizationChangeI* SqliteSynchronizationChangesRM::findIdentifier(const std::string& id)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteSynchronizationChange::IDENTIFIER_FIELD, SqliteField::INTEGER, id));
	if ( findrec )
		return new SqliteSynchronizationChange(findrec);

	return 0;
}


DBSynchronizationChangeI* SqliteSynchronizationChangesRM::createRecord(const DBBatchI* batch)
{
	const SqliteBatch* inrec = dynamic_cast<const SqliteBatch*>(batch);

	if ( inrec )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteSynchronizationChange* createrec = new SqliteSynchronizationChange(dbrec);

			if ( createrec )
			{
				createrec->setBatchNumber(inrec->getNumber());
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


DBSynchronizationChangeI* SqliteSynchronizationChangesRM::createRecord(const std::string& batchNumber)
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteSynchronizationChange* createrec = new SqliteSynchronizationChange(dbrec);

		if ( createrec )
		{
			createrec->setBatchNumber(batchNumber);
			return createrec;
		}
		else
			delete dbrec;
	}

	return 0;
}


void SqliteSynchronizationChangesRM::saveRecord(const DBSynchronizationChangeI* rec)
{
	const SqliteSynchronizationChange* inrec = dynamic_cast<const SqliteSynchronizationChange*>(rec);

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


int SqliteSynchronizationChangesRM::deleteRecord(const DBSynchronizationChangeI* rec)
{
	const SqliteSynchronizationChange* inrec = dynamic_cast<const SqliteSynchronizationChange*>(rec);

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
