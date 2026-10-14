#include <string>


#include "sqlitedatabase.h"
#include "sqlitescalesrm.h"
#include "sqlitescale.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteScale>::_dataType_ = 0;
template <> string SqliteQuery<SqliteScale>::_dataTypeName_("SqliteQuery<SqliteScale>");

int SqliteScalesRM::_dataType_ = 0;
string SqliteScalesRM::_dataTypeName_("SqliteScalesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteScalesRM::DATA_FILENAME("Scales");



SqliteScalesRM::SqliteScalesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBScalesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteScale::IDENTIFIER_FIELD);
	columnNames_.insert(SqliteScale::HOSTNAME_FIELD);
	columnNames_.insert(SqliteScale::IPADDRESS_FIELD);
	columnNames_.insert(SqliteScale::TYPE_FIELD);
	columnNames_.insert(SqliteScale::CREATIONDATETIME_FIELD);
	columnNames_.insert(SqliteScale::STATE_FIELD);
	columnNames_.insert(SqliteScale::STATECHANGEDDATETIME_FIELD);
	columnNames_.insert(SqliteScale::REGISTERING_FIELD);
	columnNames_.insert(SqliteScale::DEREGISTERING_FIELD);
	columnNames_.insert(SqliteScale::NOTIFIEDBATCHES_FIELD);
	columnNames_.insert(SqliteScale::NOTIFIEDBATCHESDATETIME_FIELD);
	columnNames_.insert(SqliteScale::NOTIFIEDONLINE_FIELD);
	columnNames_.insert(SqliteScale::NOTIFIEDONLINEDATETIME_FIELD);
	columnNames_.insert(SqliteScale::RETRYCOUNT_FIELD);
	columnNames_.insert(SqliteScale::VERSION_FIELD);
	columnNames_.insert(SqliteScale::VERSIONRECEIVED_FIELD);
	columnNames_.insert(SqliteScale::VERSIONRECEIVEDDATETIME_FIELD);
	columnNames_.insert(SqliteScale::LASTSUCCESSFULCOMMDATETIME_FIELD);
	columnNames_.insert(SqliteScale::FIRSTFAILEDCOMMDATETIME_FIELD);
	columnNames_.insert(SqliteScale::TOTALSUCCESSFULCOMMUNICATIONS_FIELD);
	columnNames_.insert(SqliteScale::TOTALFAILEDCOMMUNICATIONS_FIELD);
	columnNames_.insert(SqliteScale::TOTALBUSYCOMMUNICATIONS_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteScale::IDENTIFIER_FIELD,                    SqliteField(SqliteScale::IDENTIFIER_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::HOSTNAME_FIELD,                      SqliteField(SqliteScale::HOSTNAME_FIELD,                      SqliteField::STRING)));
	columns_.insert(make_pair(SqliteScale::IPADDRESS_FIELD,                     SqliteField(SqliteScale::IPADDRESS_FIELD,                     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteScale::TYPE_FIELD,                          SqliteField(SqliteScale::TYPE_FIELD,                          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::CREATIONDATETIME_FIELD,              SqliteField(SqliteScale::CREATIONDATETIME_FIELD,              SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteScale::STATE_FIELD,                         SqliteField(SqliteScale::STATE_FIELD,                         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::STATECHANGEDDATETIME_FIELD,          SqliteField(SqliteScale::STATECHANGEDDATETIME_FIELD,          SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteScale::REGISTERING_FIELD,                   SqliteField(SqliteScale::REGISTERING_FIELD,                   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::DEREGISTERING_FIELD,                 SqliteField(SqliteScale::DEREGISTERING_FIELD,                 SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::NOTIFIEDBATCHES_FIELD,               SqliteField(SqliteScale::NOTIFIEDBATCHES_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::NOTIFIEDBATCHESDATETIME_FIELD,       SqliteField(SqliteScale::NOTIFIEDBATCHESDATETIME_FIELD,       SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteScale::NOTIFIEDONLINE_FIELD,                SqliteField(SqliteScale::NOTIFIEDONLINE_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::NOTIFIEDONLINEDATETIME_FIELD,        SqliteField(SqliteScale::NOTIFIEDONLINEDATETIME_FIELD,        SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteScale::RETRYCOUNT_FIELD,                    SqliteField(SqliteScale::RETRYCOUNT_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::VERSION_FIELD,                       SqliteField(SqliteScale::VERSION_FIELD,                       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::VERSIONRECEIVED_FIELD,               SqliteField(SqliteScale::VERSIONRECEIVED_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::VERSIONRECEIVEDDATETIME_FIELD,       SqliteField(SqliteScale::VERSIONRECEIVEDDATETIME_FIELD,       SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteScale::LASTSUCCESSFULCOMMDATETIME_FIELD,    SqliteField(SqliteScale::LASTSUCCESSFULCOMMDATETIME_FIELD,    SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteScale::FIRSTFAILEDCOMMDATETIME_FIELD,       SqliteField(SqliteScale::FIRSTFAILEDCOMMDATETIME_FIELD,       SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteScale::TOTALSUCCESSFULCOMMUNICATIONS_FIELD, SqliteField(SqliteScale::TOTALSUCCESSFULCOMMUNICATIONS_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::TOTALFAILEDCOMMUNICATIONS_FIELD,     SqliteField(SqliteScale::TOTALFAILEDCOMMUNICATIONS_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteScale::TOTALBUSYCOMMUNICATIONS_FIELD,       SqliteField(SqliteScale::TOTALBUSYCOMMUNICATIONS_FIELD,       SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteScalesRM::~SqliteScalesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBScaleI>* SqliteScalesRM::retrieveAll()
{
	SqliteQuery<SqliteScale>* query = new SqliteQuery<SqliteScale>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBScaleI>* SqliteScalesRM::retrieveType(int type)
{
	SqliteQuery<SqliteScale>* query = new SqliteQuery<SqliteScale>(*this);

	string filter = SqliteScale::TYPE_FIELD + " = " + util_string::inttostring(type);
	query->selectSome(filter, SqliteScale::IPADDRESS_FIELD);

	return query;
}


DBScaleI* SqliteScalesRM::findIdentifier(const string& identifier)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteScale::IDENTIFIER_FIELD, SqliteField::INTEGER, identifier));
	if ( findrec )
		return new SqliteScale(findrec);

	return 0;
}


DBScaleI* SqliteScalesRM::findHostName(const string& hostname)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteScale::HOSTNAME_FIELD, SqliteField::STRING, hostname));
	if ( findrec )
		return new SqliteScale(findrec);

	return 0;
}


DBScaleI* SqliteScalesRM::findIPAddress(const string& ipAddress)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteScale::IPADDRESS_FIELD, SqliteField::STRING, ipAddress));
	if ( findrec )
		return new SqliteScale(findrec);

	return 0;
}


DBScaleI* SqliteScalesRM::findType(int type)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteScale::TYPE_FIELD, SqliteField::INTEGER, type));
	if ( findrec )
		return new SqliteScale(findrec);

	return 0;
}


DBScaleI* SqliteScalesRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteScale* createrec = new SqliteScale(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteScalesRM::saveRecord(const DBScaleI* rec)
{
	const SqliteScale* inrec = dynamic_cast<const SqliteScale*>(rec);

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


int SqliteScalesRM::deleteRecord(const DBScaleI* rec)
{
	const SqliteScale* inrec = dynamic_cast<const SqliteScale*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


void SqliteScalesRM::ensureAutoIncrementValue(int value)
{
	SqliteRecordManager::ensureAutoIncrementValue(SqliteScale::IDENTIFIER_FIELD, value);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
