#include <string>


#include "edb.h"
#include DBCONFIGURATIONSRM_INC
#include DBCONFIGURATION_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBConfigurationsRM::DBConfigurationsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBConfigurationsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBConfigurationsRM::~DBConfigurationsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBConfiguration>* DBConfigurationsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBConfiguration>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBConfiguration* DBConfigurationsRM::findName(const string& name) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBConfigurationI* rec = decorated_->findName(name);

		if ( rec )
			return new DBConfiguration(rec);
	}

	return 0;
}


DBConfiguration* DBConfigurationsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBConfigurationI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBConfiguration* r = new DBConfiguration(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBConfigurationsRM::saveRecord(const DBConfiguration* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBConfigurationsRM::deleteRecord(const DBConfiguration* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBConfigurationsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBConfigurationsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBConfigurationsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBConfigurationsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBConfigurationsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBConfigurationsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBConfigurationsRM::show()
{
	Debug::trace("database.show", 20, "DBConfigurationsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBConfigurationsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBConfiguration>::show);
}


#ifndef EVOL_NDEBUG
void DBConfigurationsRM::showAll(const std::string& dc)
#else
void DBConfigurationsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBConfigurationsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBConfigurationsRM
		_registry_->process_mem(&DBConfigurationsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBConfigurationsRM INSTANCES: None");
#endif
}


void DBConfigurationsRM::disable()
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


bool DBConfigurationsRM::registerQ(DBQuery<DBConfiguration>* query)
{
	return qsRegistry_.add(query);
}


bool DBConfigurationsRM::deregisterQ(DBQuery<DBConfiguration>* query)
{
	return qsRegistry_.remove(query);
}


void DBConfigurationsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBConfiguration>::disable);
	qsRegistry_.removeAll();
}



const std::string DBConfigurationsRM::DESCRIPTOR = "CONFIGURATION";



bool DBConfigurationsRM::registerRM(DBConfigurationsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBConfigurationsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBConfigurationsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBConfigurationsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBConfigurationsRM>* DBConfigurationsRM::_registry_ = 0;
util_thread::Mutex<> DBConfigurationsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
