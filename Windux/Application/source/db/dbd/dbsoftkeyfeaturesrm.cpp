#include <string>


#include "edb.h"
#include DBSOFTKEYFEATURESRM_INC
#include DBSOFTKEYFEATURE_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSoftKeyFeaturesRM::DBSoftKeyFeaturesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSoftKeyFeaturesRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBSoftKeyFeaturesRM::~DBSoftKeyFeaturesRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBSoftKeyFeature>* DBSoftKeyFeaturesRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKeyFeature>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}

DBQuery<DBSoftKeyFeature>* DBSoftKeyFeaturesRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKeyFeature>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}

DBQuery<DBSoftKeyFeature>* DBSoftKeyFeaturesRM::retrieveRangeNumber(const std::string& first, const std::string& last, const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKeyFeature>(dbm_, decorated_->retrieveRangeNumber(first, last, deptNum), *this);
	}
	else
		return 0;
}


DBSoftKeyFeature* DBSoftKeyFeaturesRM::findNumber(const string& number, const string& deptNum) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBSoftKeyFeatureI* rec = decorated_->findNumber(number, deptNum);

		if ( rec )
			return new DBSoftKeyFeature(rec);
	}

	return 0;
}


DBSoftKeyFeature* DBSoftKeyFeaturesRM::createRecord(const std::string& number) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBSoftKeyFeatureI* rec = decorated_->createRecord(number);

		if ( rec )
		{
			DBSoftKeyFeature* r = new DBSoftKeyFeature(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBSoftKeyFeaturesRM::saveRecord(const DBSoftKeyFeature* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBSoftKeyFeaturesRM::deleteRecord(const DBSoftKeyFeature* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBSoftKeyFeaturesRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBSoftKeyFeaturesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBSoftKeyFeaturesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBSoftKeyFeaturesRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBSoftKeyFeaturesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBSoftKeyFeaturesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBSoftKeyFeaturesRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBSoftKeyFeaturesRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBSoftKeyFeaturesRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBSoftKeyFeaturesRM::show()
{
	Debug::trace("database.show", 20, "DBSoftKeyFeaturesRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBSoftKeyFeaturesRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBSoftKeyFeature>::show);
}


#ifndef EVOL_NDEBUG
void DBSoftKeyFeaturesRM::showAll(const std::string& dc)
#else
void DBSoftKeyFeaturesRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBSoftKeyFeaturesRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBSoftKeyFeaturesRM
		_registry_->process_mem(&DBSoftKeyFeaturesRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBSoftKeyFeaturesRM INSTANCES: None");
#endif
}


void DBSoftKeyFeaturesRM::disable()
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


bool DBSoftKeyFeaturesRM::registerQ(DBQuery<DBSoftKeyFeature>* query)
{
	return qsRegistry_.add(query);
}


bool DBSoftKeyFeaturesRM::deregisterQ(DBQuery<DBSoftKeyFeature>* query)
{
	return qsRegistry_.remove(query);
}


void DBSoftKeyFeaturesRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBSoftKeyFeature>::disable);
	qsRegistry_.removeAll();
}



const std::string DBSoftKeyFeaturesRM::DESCRIPTOR = "SOFTKEYFEATURE";



bool DBSoftKeyFeaturesRM::registerRM(DBSoftKeyFeaturesRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBSoftKeyFeaturesRM>;
	}
	
	return _registry_->add(&view);
	*/
}


void DBSoftKeyFeaturesRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBSoftKeyFeaturesRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBSoftKeyFeaturesRM>* DBSoftKeyFeaturesRM::_registry_ = 0;
util_thread::Mutex<> DBSoftKeyFeaturesRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
