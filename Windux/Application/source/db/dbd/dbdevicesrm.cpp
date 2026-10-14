#include <string>


#include "edb.h"
#include DBDEVICESRM_INC
#include DBDEVICE_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBDevicesRM::DBDevicesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBDevicesRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBDevicesRM::~DBDevicesRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBDevice>* DBDevicesRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBDevice>(dbm_, decorated_->retrieveAll(), *this);
	}

	return 0;
}


DBQuery<DBDevice>* DBDevicesRM::retrieveType(const std::string& type)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBDevice>(dbm_, decorated_->retrieveType(type), *this);
	}

	return 0;
}


DBQuery<DBDevice>* DBDevicesRM::retrieveCheckPresence(bool check)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBDevice>(dbm_, decorated_->retrieveCheckPresence(check), *this);
	}

	return 0;
}


DBDevice* DBDevicesRM::findTypeSubtype(const string& type, const string& subtype) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBDeviceI* rec = decorated_->findTypeSubtype(type, subtype);

		if ( rec )
			return new DBDevice(rec);
	}

	return 0;
}


DBDevice* DBDevicesRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBDeviceI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBDevice* r = new DBDevice(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBDevicesRM::saveRecord(const DBDevice* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBDevicesRM::deleteRecord(const DBDevice* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}

	return 0;
}


int DBDevicesRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBDevicesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBDevicesRM*>(this));

		return result;
	}

	return 0;
}


int DBDevicesRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}

	return 0;
}


int DBDevicesRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}

	return 0;
}


bool DBDevicesRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBDevicesRM::show()
{
	Debug::trace("database.show", 20, "DBDevicesRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBDevicesRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBDevice>::show);
}


#ifndef EVOL_NDEBUG
void DBDevicesRM::showAll(const std::string& dc)
#else
void DBDevicesRM::showAll(const std::string& UNUSED1(dc))
#endif
{
#ifndef EVOL_NDEBUG
	std::string debuggingCategory(dc);

	if ( debuggingCategory.empty() )
		debuggingCategory.assign("database.show");
#else
	UNUSED1(dc);
#endif

	if ( _registry_ && _registry_->size() )
	{
#ifndef EVOL_NDEBUG
		Debug::trace(debuggingCategory, 20, "DBDevicesRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBDevicesRM
		_registry_->process_mem(&DBDevicesRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBDevicesRM INSTANCES: None");
#endif
}


void DBDevicesRM::disable()
{
	// Mark this record manager as not enabled so that it cannot be used
	enabled_ = false;

	// Now disable all queries associated with this record manager
	disableAllQs();

	// Now dispose of the internal decorated_ element so we do not have problems
	// later on when deleting the record manager (the session currently deletes
	// the DBManager before it deletes some of the associated record managers).
	if ( decorated_ )
	{
		delete decorated_;
		decorated_ = 0;
	}
}


bool DBDevicesRM::registerQ(DBQuery<DBDevice>* query)
{
	return qsRegistry_.add(query);
}


bool DBDevicesRM::deregisterQ(DBQuery<DBDevice>* query)
{
	return qsRegistry_.remove(query);
}


void DBDevicesRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBDevice>::disable);
	qsRegistry_.removeAll();
}



const std::string DBDevicesRM::DESCRIPTOR = "DEVICE";



bool DBDevicesRM::registerRM(DBDevicesRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBDevicesRM>;
	}

	return _registry_->add(&view);
*/
}


void DBDevicesRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBDevicesRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBDevicesRM>* DBDevicesRM::_registry_ = 0;
util_thread::Mutex<> DBDevicesRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
