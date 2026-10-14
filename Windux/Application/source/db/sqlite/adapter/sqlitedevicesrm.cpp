#include <string>


#include "sqlitedatabase.h"
#include "sqlitedevicesrm.h"
#include "sqlitedevice.h"
#include "sqlitequery.h"
#include "Bool.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteDevice>::_dataType_ = 0;
template <> string SqliteQuery<SqliteDevice>::_dataTypeName_("SqliteQuery<SqliteDevice>");

int SqliteDevicesRM::_dataType_ = 0;
string SqliteDevicesRM::_dataTypeName_("SqliteDevicesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteDevicesRM::DATA_FILENAME("Devices");



SqliteDevicesRM::SqliteDevicesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBDevicesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteDevice::TYPE_FIELD);
	columnNames_.insert(SqliteDevice::SUBTYPE_FIELD);
	columnNames_.insert(SqliteDevice::KIND_FIELD);
	columnNames_.insert(SqliteDevice::SERIALNUMBER_FIELD);
	columnNames_.insert(SqliteDevice::HARDWAREVERSION_FIELD);
	columnNames_.insert(SqliteDevice::SOFTWAREVERSION_FIELD);
	columnNames_.insert(SqliteDevice::CHECKPRESENCE_FIELD);
	columnNames_.insert(SqliteDevice::INSTALLATIONDATE_FIELD);
	columnNames_.insert(SqliteDevice::SERVICEHISTORY_FIELD);
	columnNames_.insert(SqliteDevice::USERNAME_FIELD);
	columnNames_.insert(SqliteDevice::SYSTEMNAME_FIELD);
	columnNames_.insert(SqliteDevice::IPADDRESS_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteDevice::TYPE_FIELD,             SqliteField(SqliteDevice::TYPE_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteDevice::SUBTYPE_FIELD,          SqliteField(SqliteDevice::SUBTYPE_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteDevice::KIND_FIELD,             SqliteField(SqliteDevice::KIND_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteDevice::SERIALNUMBER_FIELD,     SqliteField(SqliteDevice::SERIALNUMBER_FIELD,     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteDevice::HARDWAREVERSION_FIELD,  SqliteField(SqliteDevice::HARDWAREVERSION_FIELD,  SqliteField::STRING)));
	columns_.insert(make_pair(SqliteDevice::SOFTWAREVERSION_FIELD,  SqliteField(SqliteDevice::SOFTWAREVERSION_FIELD,  SqliteField::STRING)));
	columns_.insert(make_pair(SqliteDevice::CHECKPRESENCE_FIELD,    SqliteField(SqliteDevice::CHECKPRESENCE_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteDevice::INSTALLATIONDATE_FIELD, SqliteField(SqliteDevice::INSTALLATIONDATE_FIELD, SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteDevice::SERVICEHISTORY_FIELD,   SqliteField(SqliteDevice::SERVICEHISTORY_FIELD,   SqliteField::STRING)));
	columns_.insert(make_pair(SqliteDevice::USERNAME_FIELD,         SqliteField(SqliteDevice::USERNAME_FIELD,         SqliteField::STRING)));
	columns_.insert(make_pair(SqliteDevice::SYSTEMNAME_FIELD,       SqliteField(SqliteDevice::SYSTEMNAME_FIELD,       SqliteField::STRING)));
	columns_.insert(make_pair(SqliteDevice::IPADDRESS_FIELD,        SqliteField(SqliteDevice::IPADDRESS_FIELD,        SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteDevicesRM::~SqliteDevicesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBDeviceI>* SqliteDevicesRM::retrieveAll()
{
	SqliteQuery<SqliteDevice>* query = new SqliteQuery<SqliteDevice>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBDeviceI>* SqliteDevicesRM::retrieveType(const std::string& type)
{
	if ( type.size() )
	{
		SqliteQuery<SqliteDevice>* query = new SqliteQuery<SqliteDevice>(*this);

		string filter = SqliteDevice::TYPE_FIELD + " = " + type;
		query->selectSome(filter, SqliteDevice::SUBTYPE_FIELD);

		return query;
	}

	return 0;
}


DBQueryI<DBDeviceI>* SqliteDevicesRM::retrieveCheckPresence(bool check)
{
	SqliteQuery<SqliteDevice>* query = new SqliteQuery<SqliteDevice>(*this);

	string filter = SqliteDevice::CHECKPRESENCE_FIELD + " = " + Bool::toString(check);
	query->selectSomeIndexed(filter);

	return query;
}


DBDeviceI* SqliteDevicesRM::findTypeSubtype(const string& type, const string& subtype)
{
	if ( type.size() && subtype.size() )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteDevice::TYPE_FIELD, SqliteField::INTEGER, type));
		keys.push_back(SqliteField(SqliteDevice::SUBTYPE_FIELD, SqliteField::INTEGER, subtype));
		
		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
		if ( findrec )
			return new SqliteDevice(findrec);
	}

	return 0;
}


DBDeviceI* SqliteDevicesRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteDevice* createrec = new SqliteDevice(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteDevicesRM::saveRecord(const DBDeviceI* rec)
{
	const SqliteDevice* inrec = dynamic_cast<const SqliteDevice*>(rec);

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


int SqliteDevicesRM::deleteRecord(const DBDeviceI* rec)
{
	const SqliteDevice* inrec = dynamic_cast<const SqliteDevice*>(rec);

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
