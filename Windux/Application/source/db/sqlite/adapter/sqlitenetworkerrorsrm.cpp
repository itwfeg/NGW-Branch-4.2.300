#include <string>


#include "sqlitedatabase.h"
#include "sqlitenetworkerrorsrm.h"
#include "sqlitenetworkerror.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteNetworkError>::_dataType_ = 0;
template <> string SqliteQuery<SqliteNetworkError>::_dataTypeName_("SqliteQuery<SqliteNetworkError>");

int SqliteNetworkErrorsRM::_dataType_ = 0;
string SqliteNetworkErrorsRM::_dataTypeName_("SqliteNetworkErrorsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteNetworkErrorsRM::DATA_FILENAME("NetworkErrors");



SqliteNetworkErrorsRM::SqliteNetworkErrorsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBNetworkErrorsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteNetworkError::NETWORKERRORID_FIELD);
	columnNames_.insert(SqliteNetworkError::SCALEID_FIELD);
	columnNames_.insert(SqliteNetworkError::ERRORCODE_FIELD);
	columnNames_.insert(SqliteNetworkError::RECORDTYPE_FIELD);
	columnNames_.insert(SqliteNetworkError::DATETIME_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteNetworkError::NETWORKERRORID_FIELD, SqliteField(SqliteNetworkError::NETWORKERRORID_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteNetworkError::SCALEID_FIELD,        SqliteField(SqliteNetworkError::SCALEID_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteNetworkError::ERRORCODE_FIELD,      SqliteField(SqliteNetworkError::ERRORCODE_FIELD,      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteNetworkError::RECORDTYPE_FIELD,     SqliteField(SqliteNetworkError::RECORDTYPE_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteNetworkError::DATETIME_FIELD,       SqliteField(SqliteNetworkError::DATETIME_FIELD,       SqliteField::DATETIME)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteNetworkErrorsRM::~SqliteNetworkErrorsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBNetworkErrorI>* SqliteNetworkErrorsRM::retrieveAll()
{
	SqliteQuery<SqliteNetworkError>* query = new SqliteQuery<SqliteNetworkError>(*this);

	query->selectAll();

	return query;
}


/*
DBQueryI<DBNetworkErrorI>* SqliteNetworkErrorsRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteNetworkError>* query = new SqliteQuery<SqliteNetworkError>(*this);

	string filter = SqliteNetworkError::NUMBER_FIELD + " >= " + first + " and " + SqliteNetworkError::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteNetworkError::NUMBER_FIELD);

	return query;
}
*/


DBNetworkErrorI* SqliteNetworkErrorsRM::findID(const std::string& id)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteNetworkError::NETWORKERRORID_FIELD, SqliteField::INTEGER, id));
	if ( findrec )
		return new SqliteNetworkError(findrec);

	return 0;
}


DBNetworkErrorI* SqliteNetworkErrorsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteNetworkError* createrec = new SqliteNetworkError(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteNetworkErrorsRM::saveRecord(const DBNetworkErrorI* rec)
{
	const SqliteNetworkError* inrec = dynamic_cast<const SqliteNetworkError*>(rec);

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


int SqliteNetworkErrorsRM::deleteRecord(const DBNetworkErrorI* rec)
{
	const SqliteNetworkError* inrec = dynamic_cast<const SqliteNetworkError*>(rec);

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
