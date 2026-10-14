#include <string>


#include "edb.h"
#include DBSOFTKEYGROUPSRM_INC
#include DBSOFTKEYGROUP_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSoftKeyGroupsRM::DBSoftKeyGroupsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSoftKeyGroupsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBSoftKeyGroupsRM::~DBSoftKeyGroupsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBSoftKeyGroup>* DBSoftKeyGroupsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKeyGroup>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}

DBQuery<DBSoftKeyGroup>* DBSoftKeyGroupsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKeyGroup>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}

DBQuery<DBSoftKeyGroup>* DBSoftKeyGroupsRM::retrieveRangeNumber(const std::string& first, const std::string& last, const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKeyGroup>(dbm_, decorated_->retrieveRangeNumber(first, last, deptNum), *this);
	}
	else
		return 0;
}


DBSoftKeyGroup* DBSoftKeyGroupsRM::findNumber(const string& number, const string& deptNum) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBSoftKeyGroupI* rec = decorated_->findNumber(number, deptNum);

		if ( rec )
			return new DBSoftKeyGroup(rec);
	}

	return 0;
}


DBSoftKeyGroup* DBSoftKeyGroupsRM::createRecord(const std::string& number) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBSoftKeyGroupI* rec = decorated_->createRecord(number);

		if ( rec )
		{
			DBSoftKeyGroup* r = new DBSoftKeyGroup(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBSoftKeyGroupsRM::saveRecord(const DBSoftKeyGroup* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBSoftKeyGroupsRM::deleteRecord(const DBSoftKeyGroup* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBSoftKeyGroupsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBSoftKeyGroupsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBSoftKeyGroupsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBSoftKeyGroupsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBSoftKeyGroupsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBSoftKeyGroupsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBSoftKeyGroupsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBSoftKeyGroupsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBSoftKeyGroupsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBSoftKeyGroupsRM::show()
{
	Debug::trace("database.show", 20, "DBSoftKeyGroupsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBSoftKeyGroupsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBSoftKeyGroup>::show);
}


#ifndef EVOL_NDEBUG
void DBSoftKeyGroupsRM::showAll(const std::string& dc)
#else
void DBSoftKeyGroupsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBSoftKeyGroupsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBSoftKeyGroupsRM
		_registry_->process_mem(&DBSoftKeyGroupsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBSoftKeyGroupsRM INSTANCES: None");
#endif
}


void DBSoftKeyGroupsRM::disable()
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


bool DBSoftKeyGroupsRM::registerQ(DBQuery<DBSoftKeyGroup>* query)
{
	return qsRegistry_.add(query);
}


bool DBSoftKeyGroupsRM::deregisterQ(DBQuery<DBSoftKeyGroup>* query)
{
	return qsRegistry_.remove(query);
}


void DBSoftKeyGroupsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBSoftKeyGroup>::disable);
	qsRegistry_.removeAll();
}



const std::string DBSoftKeyGroupsRM::DESCRIPTOR = "SOFTKEYGROUP";



bool DBSoftKeyGroupsRM::registerRM(DBSoftKeyGroupsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBSoftKeyGroupsRM>;
	}

	return _registry_->add(&view);
	*/
}


void DBSoftKeyGroupsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBSoftKeyGroupsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBSoftKeyGroupsRM>* DBSoftKeyGroupsRM::_registry_ = 0;
util_thread::Mutex<> DBSoftKeyGroupsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
