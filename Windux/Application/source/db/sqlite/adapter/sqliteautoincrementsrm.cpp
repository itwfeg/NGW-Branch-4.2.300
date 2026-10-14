#include <string>


#include "sqliteautoincrementsrm.h"

#include ACCOUNTANTI_INC
#include UTILSTRING_INC
#include DBEXCEPTION_INC
#include DEBUG_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using edb::DBException;
	using namespace util_string;
	using namespace util_thread;
#endif


const string SqliteAutoIncrementsRM::PRODUCTID_FIELD                ("AIPrID");
const string SqliteAutoIncrementsRM::OPERATORCHANGEID_FIELD         ("AIOCID");
const string SqliteAutoIncrementsRM::SYNCHRONIZATIONCHANGEID_FIELD  ("AIYCID");
const string SqliteAutoIncrementsRM::NETWORKERRORID_FIELD           ("AINEID");
const string SqliteAutoIncrementsRM::SCALEID_FIELD                  ("AIScID");
const string SqliteAutoIncrementsRM::COOLTOTALID_FIELD              ("AILOID");
const string SqliteAutoIncrementsRM::COOLPRODUCTIONSTEPID_FIELD     ("AILSID");
const string SqliteAutoIncrementsRM::COOLCOUNTRYID_FIELD            ("AILUID");
const string SqliteAutoIncrementsRM::COOLLISTID_FIELD               ("AILLID");
const string SqliteAutoIncrementsRM::EXPIRATIONLABELID_FIELD        ("AIELID");
const string SqliteAutoIncrementsRM::TRANSACTIONID_FIELD            ("AITrID");
const string SqliteAutoIncrementsRM::WRAPPERERRORID_FIELD           ("AIWEID");


#ifdef USE_ACCOUNTANT
int SqliteAutoIncrementsRM::_dataType_ = 0;
string SqliteAutoIncrementsRM::_dataTypeName_("SqliteAutoIncrementsRM");
#endif


util_registry::Registry<SqliteAutoIncrementsRM>* SqliteAutoIncrementsRM::_registry_ = 0;
util_thread::Mutex<> SqliteAutoIncrementsRM::_registryMutex_ = util_thread::Mutex<>();

Mutex<> SqliteAutoIncrementsRM::_incrementMutex_ = Mutex<>();

const string SqliteAutoIncrementsRM::DATA_FILENAME("AutoIncrement");



SqliteAutoIncrementsRM::SqliteAutoIncrementsRM(const SqliteDatabase& sdb, const util_env::Properties& props)
		: SqliteRecordManager(sdb, props, DATA_FILENAME, false), opened_(false), firstAutoIncrement_(1), maximumIncrement_(99999999)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	const string* property = getTableProperty("MaximumAutoIncrement");
	if ( property )
		maximumIncrement_ = stringtoint(*property);

	property = getTableProperty("FirstAutoIncrement");
	if ( property )
		firstAutoIncrement_ = stringtoint(*property);


	columnNames_.insert(SqliteAutoIncrementsRM::PRODUCTID_FIELD);
	columnNames_.insert(SqliteAutoIncrementsRM::OPERATORCHANGEID_FIELD);
	columnNames_.insert(SqliteAutoIncrementsRM::SYNCHRONIZATIONCHANGEID_FIELD);
	columnNames_.insert(SqliteAutoIncrementsRM::NETWORKERRORID_FIELD);
	columnNames_.insert(SqliteAutoIncrementsRM::SCALEID_FIELD);
	columnNames_.insert(SqliteAutoIncrementsRM::COOLTOTALID_FIELD);
	columnNames_.insert(SqliteAutoIncrementsRM::COOLPRODUCTIONSTEPID_FIELD);
	columnNames_.insert(SqliteAutoIncrementsRM::COOLCOUNTRYID_FIELD);
	columnNames_.insert(SqliteAutoIncrementsRM::COOLLISTID_FIELD);
	columnNames_.insert(SqliteAutoIncrementsRM::EXPIRATIONLABELID_FIELD);
	columnNames_.insert(SqliteAutoIncrementsRM::TRANSACTIONID_FIELD);
	columnNames_.insert(SqliteAutoIncrementsRM::WRAPPERERRORID_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteAutoIncrementsRM::PRODUCTID_FIELD,               SqliteField(SqliteAutoIncrementsRM::PRODUCTID_FIELD,               SqliteField::INTEGER, 0)));
	columns_.insert(make_pair(SqliteAutoIncrementsRM::OPERATORCHANGEID_FIELD,        SqliteField(SqliteAutoIncrementsRM::OPERATORCHANGEID_FIELD,        SqliteField::INTEGER, 0)));
	columns_.insert(make_pair(SqliteAutoIncrementsRM::SYNCHRONIZATIONCHANGEID_FIELD, SqliteField(SqliteAutoIncrementsRM::SYNCHRONIZATIONCHANGEID_FIELD, SqliteField::INTEGER, 0)));
	columns_.insert(make_pair(SqliteAutoIncrementsRM::NETWORKERRORID_FIELD,          SqliteField(SqliteAutoIncrementsRM::NETWORKERRORID_FIELD,          SqliteField::INTEGER, 0)));
	columns_.insert(make_pair(SqliteAutoIncrementsRM::SCALEID_FIELD,                 SqliteField(SqliteAutoIncrementsRM::SCALEID_FIELD,                 SqliteField::INTEGER, 0)));
	columns_.insert(make_pair(SqliteAutoIncrementsRM::COOLTOTALID_FIELD,             SqliteField(SqliteAutoIncrementsRM::COOLTOTALID_FIELD,             SqliteField::INTEGER, 0)));
	columns_.insert(make_pair(SqliteAutoIncrementsRM::COOLPRODUCTIONSTEPID_FIELD,    SqliteField(SqliteAutoIncrementsRM::COOLPRODUCTIONSTEPID_FIELD,    SqliteField::INTEGER, 0)));
	columns_.insert(make_pair(SqliteAutoIncrementsRM::COOLCOUNTRYID_FIELD,           SqliteField(SqliteAutoIncrementsRM::COOLCOUNTRYID_FIELD,           SqliteField::INTEGER, 0)));
	columns_.insert(make_pair(SqliteAutoIncrementsRM::COOLLISTID_FIELD,              SqliteField(SqliteAutoIncrementsRM::COOLLISTID_FIELD,              SqliteField::INTEGER, 0)));
	columns_.insert(make_pair(SqliteAutoIncrementsRM::EXPIRATIONLABELID_FIELD,       SqliteField(SqliteAutoIncrementsRM::EXPIRATIONLABELID_FIELD,       SqliteField::INTEGER, 0)));
	columns_.insert(make_pair(SqliteAutoIncrementsRM::TRANSACTIONID_FIELD,           SqliteField(SqliteAutoIncrementsRM::TRANSACTIONID_FIELD,           SqliteField::INTEGER, 0)));
	columns_.insert(make_pair(SqliteAutoIncrementsRM::WRAPPERERRORID_FIELD,          SqliteField(SqliteAutoIncrementsRM::WRAPPERERRORID_FIELD,          SqliteField::INTEGER, 0)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));


	if ( !registerRM(this) )
		throw DBException(DBException::SQLITE_AUTOINC_REGISTRY, "Failed to register an AutoIncrementer");
}


SqliteAutoIncrementsRM::~SqliteAutoIncrementsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	deregisterRM(this);
}


void SqliteAutoIncrementsRM::open()
{
}

void SqliteAutoIncrementsRM::close()
{
}

void SqliteAutoIncrementsRM::openAll()
{
	if ( _registry_ )
		_registry_->process_mem(&SqliteAutoIncrementsRM::open);
}

void SqliteAutoIncrementsRM::closeAll()
{
	if ( _registry_ )
		_registry_->process_mem(&SqliteAutoIncrementsRM::close);
}


const string SqliteAutoIncrementsRM::operator()(const string& key)
{
	string fieldName("AI");
	fieldName.append(key);

	bool isTracing = Debug::isTracing("database.autoIncrement");
	if (isTracing)
		Debug::trace("database.autoIncrement", 3, "operator() key=" + key + "  fieldName=" + fieldName);

	// We need a critical section here so that two threads don't run into
	// each other when retrieving and assigning a new value to the auto key
	// (i.e., the retrieve-assign operations needs to be atomic).
	Lock< Mutex<> > lock(_incrementMutex_);

	SqliteRecord* record = findSingleRecord();
	if ( !record )
		throw DBException(DBException::SQLITE_NO_AUTOINC_REC, "The AutoInc table is empty.");

	if ( !record->isValid() && isTracing )
		Debug::trace("database.autoIncrement", 3, "The AutoIncrement record was not valid (invalid checksum).");

	int number = record->getField(fieldName);
	if (isTracing)
		Debug::trace("database.autoIncrement", 3, "number returned from getField = " + util_string::inttostring(number));

	// ALERT: it is possible, albeight unlikely, to overflow the storage
	// space if the counter goes beyond 99,999,999 iterations. If it
	// rolls over, there is also the potential problem of calculating a
	// currently used identifier.
	if ( ++number > maximumIncrement_ ) 
		number = firstAutoIncrement_;

	string incremented = util_string::inttostring(number);

	if (isTracing)
		Debug::trace("database.autoIncrement", 3, "before calling setfield, number = " + incremented);
	record->setField(fieldName, incremented);
	if ( save(record) )
		modified();
	else
	{
		delete record;

		throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "Unable to save AutoIncrement record.");
	}

#ifndef EVOL_NDEBUG
	Debug::trace("database.autoIncrement", 24, "Auto incremented field " + key + " to value " + incremented);
#endif

	delete record;

	return incremented;
}


void SqliteAutoIncrementsRM::ensureAutoIncrementValue(const std::string& key, int value)
{
	string fieldName("AI");
	fieldName.append(key);

	// We need a critical section here so that two threads don't run into
	// each other when retrieving and assigning a new value to the auto key
	// (i.e., the retrieve-assign operations needs to be atomic).
	Lock< Mutex<> > lock(_incrementMutex_);

	SqliteRecord* record = findSingleRecord();
	if ( !record )
		throw DBException(DBException::SQLITE_NO_AUTOINC_REC, "The AutoInc table is empty (2).");

	if ( !record->isValid() )
		Debug::trace("database.ensureAutoIncrement", 3, "The AutoIncrement record was not valid (invalid checksum).");

	int number = record->getField(fieldName);

	if ( number < value )
	{
		string newValue = util_string::inttostring(value);

		record->setField(fieldName, newValue);

		if ( save(record) )
			modified();
		else
		{
			delete record;

			throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "Unable to save AutoIncrement record.");
		}

#ifndef EVOL_NDEBUG
		Debug::trace("database.autoIncrement", 24, "Updated auto-increment field " + key + " to value " + newValue);
#endif
	}

	delete record;
}


bool SqliteAutoIncrementsRM::registerRM(SqliteAutoIncrementsRM* kai)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<SqliteAutoIncrementsRM>;
	}

	return _registry_->add(kai);
	*/
}


bool SqliteAutoIncrementsRM::deregisterRM(SqliteAutoIncrementsRM* kai)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true; // _registry_->remove(kai);
}


void SqliteAutoIncrementsRM::disableAll()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&SqliteAutoIncrementsRM::close);
		_registry_->removeAll();
	}
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
