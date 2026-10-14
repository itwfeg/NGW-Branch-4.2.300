#include <string>


#include "edb.h"
#include DBSOFTKEYSRM_INC
#include DBSOFTKEY_INC
#include DBSOFTKEYGROUP_INC
#include DBPRODUCT_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSoftKeysRM::DBSoftKeysRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSoftKeysRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBSoftKeysRM::~DBSoftKeysRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBSoftKey>* DBSoftKeysRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKey>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}

DBQuery<DBSoftKey>* DBSoftKeysRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKey>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}

DBQuery<DBSoftKey>* DBSoftKeysRM::retrieveGroup(const string& groupNumber, const string& deptNumber)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKey>(dbm_, decorated_->retrieveGroup(groupNumber, deptNumber), *this);
	}
	else
		return 0;
}


DBQuery<DBSoftKey>* DBSoftKeysRM::retrieveGroup(const DBSoftKeyGroup* skg)
{
	if ( enabled_ && skg )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKey>(dbm_, decorated_->retrieveGroup(skg->decorated_), *this);
	}
	else
		return 0;
}


DBSoftKey* DBSoftKeysRM::find(const string& gn, const string& dn, const string& hp, const string& vp) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBSoftKeyI* rec = decorated_->findSoftKey(gn, dn, hp, vp);

		if ( rec )
			return new DBSoftKey(rec);
	}

	return 0;
}


DBSoftKey* DBSoftKeysRM::createRecord(int gn, int dn, int hp, int vp) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBSoftKeyI* rec = decorated_->createRecord(util_string::inttostring(gn), util_string::inttostring(dn), util_string::inttostring(hp), util_string::inttostring(vp));

		if ( rec )
		{
			DBSoftKey* r = new DBSoftKey(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBSoftKeysRM::saveRecord(const DBSoftKey* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBSoftKeysRM::deleteRecord(const DBSoftKey* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBSoftKeysRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBSoftKeysRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBSoftKeysRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBSoftKeysRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBSoftKeysRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBSoftKeysRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBSoftKeysRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBSoftKeysRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBSoftKeysRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBSoftKeysRM::show()
{
	Debug::trace("database.show", 20, "DBSoftKeysRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBSoftKeysRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBSoftKey>::show);
}


#ifndef EVOL_NDEBUG
void DBSoftKeysRM::showAll(const std::string& dc)
#else
void DBSoftKeysRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBSoftKeysRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBSoftKeysRM
		_registry_->process_mem(&DBSoftKeysRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBSoftKeysRM INSTANCES: None");
#endif
}


void DBSoftKeysRM::disable()
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


bool DBSoftKeysRM::registerQ(DBQuery<DBSoftKey>* query)
{
	return qsRegistry_.add(query);
}


bool DBSoftKeysRM::deregisterQ(DBQuery<DBSoftKey>* query)
{
	return qsRegistry_.remove(query);
}


void DBSoftKeysRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBSoftKey>::disable);
	qsRegistry_.removeAll();
}



const std::string DBSoftKeysRM::DESCRIPTOR = "SOFTKEY";



bool DBSoftKeysRM::registerRM(DBSoftKeysRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBSoftKeysRM>;
	}

	return _registry_->add(&view);
	*/
}


void DBSoftKeysRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBSoftKeysRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBSoftKeysRM>* DBSoftKeysRM::_registry_ = 0;
util_thread::Mutex<> DBSoftKeysRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
