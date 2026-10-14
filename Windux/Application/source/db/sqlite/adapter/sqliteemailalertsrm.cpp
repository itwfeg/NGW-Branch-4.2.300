#include <string>
#ifdef WIN32_OS
	#include <locale>
#endif


#include "sqlitedatabase.h"
#include "sqliteemailalertsrm.h"
#include "sqliteemailalert.h"
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
template <> int SqliteQuery<SqliteEmailAlert>::_dataType_ = 0;
template <> string SqliteQuery<SqliteEmailAlert>::_dataTypeName_("SqliteQuery<SqliteEmailAlert>");

int SqliteEmailAlertsRM::_dataType_ = 0;
string SqliteEmailAlertsRM::_dataTypeName_("SqliteEmailAlertsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteEmailAlertsRM::DATA_FILENAME("EmailAlerts");



SqliteEmailAlertsRM::SqliteEmailAlertsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBEmailAlertsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteEmailAlert::NUMBER_FIELD);
	columnNames_.insert(SqliteEmailAlert::NOTIFY_ON_FAILURE_FIELD);
	columnNames_.insert(SqliteEmailAlert::NOTIFY_ON_RESOLUTION_FIELD);
	columnNames_.insert(SqliteEmailAlert::DELAY_FIELD);
	columnNames_.insert(SqliteEmailAlert::FAILURESENT_FIELD);
	columnNames_.insert(SqliteEmailAlert::SUBJECT1_FIELD);
	columnNames_.insert(SqliteEmailAlert::SUBJECT2_FIELD);
	columnNames_.insert(SqliteEmailAlert::NOTIFY_HANDS_FREE_DEVICE_ON_FAILURE_FIELD);
	columnNames_.insert(SqliteEmailAlert::NOTIFY_HANDS_FREE_DEVICE_ON_RESOLUTION_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteEmailAlert::NUMBER_FIELD,               SqliteField(SqliteEmailAlert::NUMBER_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteEmailAlert::NOTIFY_ON_FAILURE_FIELD,    SqliteField(SqliteEmailAlert::NOTIFY_ON_FAILURE_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteEmailAlert::NOTIFY_ON_RESOLUTION_FIELD, SqliteField(SqliteEmailAlert::NOTIFY_ON_RESOLUTION_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteEmailAlert::DELAY_FIELD,                SqliteField(SqliteEmailAlert::DELAY_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteEmailAlert::FAILURESENT_FIELD,          SqliteField(SqliteEmailAlert::FAILURESENT_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteEmailAlert::SUBJECT1_FIELD,             SqliteField(SqliteEmailAlert::SUBJECT1_FIELD,             SqliteField::STRING)));
	columns_.insert(make_pair(SqliteEmailAlert::SUBJECT2_FIELD,             SqliteField(SqliteEmailAlert::SUBJECT2_FIELD,             SqliteField::STRING)));
	columns_.insert(make_pair(SqliteEmailAlert::NOTIFY_HANDS_FREE_DEVICE_ON_FAILURE_FIELD,    SqliteField(SqliteEmailAlert::NOTIFY_HANDS_FREE_DEVICE_ON_FAILURE_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteEmailAlert::NOTIFY_HANDS_FREE_DEVICE_ON_RESOLUTION_FIELD, SqliteField(SqliteEmailAlert::NOTIFY_HANDS_FREE_DEVICE_ON_RESOLUTION_FIELD, SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteEmailAlertsRM::~SqliteEmailAlertsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBEmailAlertI>* SqliteEmailAlertsRM::retrieveAll()
{
	SqliteQuery<SqliteEmailAlert>* query = new SqliteQuery<SqliteEmailAlert>(*this);

	// Sort the EmailAlerts by email alert number
	query->selectAll();

	return query;
}


DBQueryI<DBEmailAlertI>* SqliteEmailAlertsRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteEmailAlert>* query = new SqliteQuery<SqliteEmailAlert>(*this);

	// Sort the EmailAlerts by email alert number
	string filter = SqliteEmailAlert::NUMBER_FIELD + " >= " + first + " and " + SqliteEmailAlert::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteEmailAlert::NUMBER_FIELD);

	return query;
}


DBEmailAlertI* SqliteEmailAlertsRM::findNumber(const string& number)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteEmailAlert::NUMBER_FIELD, SqliteField::INTEGER, number));
	if ( findrec )
		return new SqliteEmailAlert(findrec);

	return 0;
}


DBEmailAlertI* SqliteEmailAlertsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteEmailAlert* createrec = new SqliteEmailAlert(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteEmailAlertsRM::saveRecord(const DBEmailAlertI* rec)
{
	const SqliteEmailAlert* inrec = dynamic_cast<const SqliteEmailAlert*>(rec);

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


int SqliteEmailAlertsRM::deleteRecord(const DBEmailAlertI* rec)
{
	const SqliteEmailAlert* inrec = dynamic_cast<const SqliteEmailAlert*>(rec);

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
