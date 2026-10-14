#include <string>


#include "edb.h"
#include DBSCALESRM_INC
#include DBSCALE_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBScalesRM::DBScalesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBScalesRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBScalesRM::~DBScalesRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBScale>* DBScalesRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBScale>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBScale>* DBScalesRM::retrieveType(int type)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBScale>(dbm_, decorated_->retrieveType(type), *this);
	}
	else
		return 0;
}


DBScale* DBScalesRM::findIdentifier(const string& identifier) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBScaleI* rec = decorated_->findIdentifier(identifier);

		if ( rec )
			return new DBScale(rec);
	}

	return 0;
}


DBScale* DBScalesRM::findHostName(const string& hostname) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBScaleI* rec = decorated_->findHostName(hostname);

		if ( rec )
			return new DBScale(rec);
	}

	return 0;
}


DBScale* DBScalesRM::findIPAddress(const string& ipAddress) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBScaleI* rec = decorated_->findIPAddress(ipAddress);

		if ( rec )
			return new DBScale(rec);
	}

	return 0;
}


DBScale* DBScalesRM::findType(int type) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBScaleI* rec = decorated_->findType(type);

		if ( rec )
			return new DBScale(rec);
	}

	return 0;
}


DBScale* DBScalesRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBScaleI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBScale* r = new DBScale(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBScalesRM::saveRecord(const DBScale* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBScalesRM::deleteRecord(const DBScale* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBScalesRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBScalesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBScalesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBScalesRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBScalesRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBScalesRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBScalesRM::ensureAutoIncrementValue(int value)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSemiautoIncrement);

		decorated_->ensureAutoIncrementValue(value);
	}
}


void DBScalesRM::show()
{
	Debug::trace("database.show", 20, "DBScalesRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBScalesRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBScale>::show);
}


#ifndef EVOL_NDEBUG
void DBScalesRM::showAll(const std::string& dc)
#else
void DBScalesRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBScalesRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBScalesRM
		_registry_->process_mem(&DBScalesRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBScalesRM INSTANCES: None");
#endif
}


void DBScalesRM::disable()
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


bool DBScalesRM::registerQ(DBQuery<DBScale>* query)
{
	return qsRegistry_.add(query);
}


bool DBScalesRM::deregisterQ(DBQuery<DBScale>* query)
{
	return qsRegistry_.remove(query);
}


void DBScalesRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBScale>::disable);
	qsRegistry_.removeAll();
}



const std::string DBScalesRM::DESCRIPTOR = "SCALE";



bool DBScalesRM::registerRM(DBScalesRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBScalesRM>;
	}

	return _registry_->add(&view);
	*/
}


void DBScalesRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBScalesRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBScalesRM>* DBScalesRM::_registry_ = 0;
util_thread::Mutex<> DBScalesRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
