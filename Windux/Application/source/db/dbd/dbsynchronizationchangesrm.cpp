#include <string>


#include "edb.h"
#include DBSYNCHRONIZATIONCHANGESRM_INC
#include DBSYNCHRONIZATIONCHANGE_INC
#include DBBATCH_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSynchronizationChangesRM::DBSynchronizationChangesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSynchronizationChangesRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBSynchronizationChangesRM::~DBSynchronizationChangesRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBSynchronizationChange>* DBSynchronizationChangesRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSynchronizationChange>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBSynchronizationChange>* DBSynchronizationChangesRM::retrieveLinkedToBatch(const DBBatch* batch)
{
	if ( enabled_ && batch )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSynchronizationChange>(dbm_, decorated_->retrieveLinkedToBatch(batch->decorated_), *this);
	}
	else
		return 0;
}


DBQuery<DBSynchronizationChange>* DBSynchronizationChangesRM::retrieveLinkedToBatch(const std::string& batchNumber)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSynchronizationChange>(dbm_, decorated_->retrieveLinkedToBatch(batchNumber), *this);
	}
	else
		return 0;
}


DBSynchronizationChange* DBSynchronizationChangesRM::findIdentifier(const string& id) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBSynchronizationChangeI* rec = decorated_->findIdentifier(id);

		if ( rec )
			return new DBSynchronizationChange(rec);
	}

	return 0;
}


DBSynchronizationChange* DBSynchronizationChangesRM::createRecord(const DBBatch* batch) const
{
	if ( enabled_ && batch )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBSynchronizationChangeI* rec = decorated_->createRecord(batch->decorated_);

		if ( rec )
			return new DBSynchronizationChange(rec);
	}

	return 0;
}


DBSynchronizationChange* DBSynchronizationChangesRM::createRecord(const std::string& batchNumber) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBSynchronizationChangeI* rec = decorated_->createRecord(batchNumber);

		if ( rec )
		{
			DBSynchronizationChange* r = new DBSynchronizationChange(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBSynchronizationChangesRM::saveRecord(const DBSynchronizationChange* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBSynchronizationChangesRM::deleteRecord(const DBSynchronizationChange* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBSynchronizationChangesRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBSynchronizationChangesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBSynchronizationChangesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBSynchronizationChangesRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBSynchronizationChangesRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBSynchronizationChangesRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBSynchronizationChangesRM::show()
{
	Debug::trace("database.show", 20, "DBSynchronizationChangesRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBSynchronizationChangesRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBSynchronizationChange>::show);
}


#ifndef EVOL_NDEBUG
void DBSynchronizationChangesRM::showAll(const std::string& dc)
#else
void DBSynchronizationChangesRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBSynchronizationChangesRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBSynchronizationChangesRM
		_registry_->process_mem(&DBSynchronizationChangesRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBSynchronizationChangesRM INSTANCES: None");
#endif
}


void DBSynchronizationChangesRM::disable()
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


bool DBSynchronizationChangesRM::registerQ(DBQuery<DBSynchronizationChange>* query)
{
	return qsRegistry_.add(query);
}


bool DBSynchronizationChangesRM::deregisterQ(DBQuery<DBSynchronizationChange>* query)
{
	return qsRegistry_.remove(query);
}


void DBSynchronizationChangesRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBSynchronizationChange>::disable);
	qsRegistry_.removeAll();
}



const std::string DBSynchronizationChangesRM::DESCRIPTOR = "SYNCHRONIZATIONCHANGE";



bool DBSynchronizationChangesRM::registerRM(DBSynchronizationChangesRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBSynchronizationChangesRM>;
	}

	return _registry_->add(&view);
	*/
}


void DBSynchronizationChangesRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBSynchronizationChangesRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBSynchronizationChangesRM>* DBSynchronizationChangesRM::_registry_ = 0;
util_thread::Mutex<> DBSynchronizationChangesRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
