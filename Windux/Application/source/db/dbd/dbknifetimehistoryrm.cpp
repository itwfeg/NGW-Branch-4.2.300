#include <string>


#include "edb.h"
#include DBKNIFETIMEHISTORYRM_INC
#include DBKNIFETIMEHISTORY_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif


																												  
DBKnifeTimeHistoryRM::DBKnifeTimeHistoryRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBKnifeTimeHistoryRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBKnifeTimeHistoryRM::~DBKnifeTimeHistoryRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBKnifeTimeHistory>* DBKnifeTimeHistoryRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBKnifeTimeHistory>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBKnifeTimeHistory* DBKnifeTimeHistoryRM::find(const string& number) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBKnifeTimeHistoryI* rec = decorated_->find(number);

		if ( rec )
			return new DBKnifeTimeHistory(rec);
	}

	return 0;
}


DBKnifeTimeHistory* DBKnifeTimeHistoryRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBKnifeTimeHistoryI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBKnifeTimeHistory* r = new DBKnifeTimeHistory(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBKnifeTimeHistoryRM::saveRecord(const DBKnifeTimeHistory* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBKnifeTimeHistoryRM::deleteRecord(const DBKnifeTimeHistory* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBKnifeTimeHistoryRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBKnifeTimeHistoryRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBKnifeTimeHistoryRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBKnifeTimeHistoryRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBKnifeTimeHistoryRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBKnifeTimeHistoryRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBKnifeTimeHistoryRM::show()
{
	Debug::trace("database.show", 20, "DBKnifeTimeHistoryRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBKnifeTimeHistoryRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBKnifeTimeHistory>::show);
}


#ifndef EVOL_NDEBUG
void DBKnifeTimeHistoryRM::showAll(const std::string& dc)
#else
void DBKnifeTimeHistoryRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBKnifeTimeHistoryRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBKnifeTimeHistoryRM
		_registry_->process_mem(&DBKnifeTimeHistoryRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBKnifeTimeHistoryRM INSTANCES: None");
#endif
}


void DBKnifeTimeHistoryRM::disable()
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


bool DBKnifeTimeHistoryRM::registerQ(DBQuery<DBKnifeTimeHistory>* query)
{
	return qsRegistry_.add(query);
}


bool DBKnifeTimeHistoryRM::deregisterQ(DBQuery<DBKnifeTimeHistory>* query)
{
	return qsRegistry_.remove(query);
}


void DBKnifeTimeHistoryRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBKnifeTimeHistory>::disable);
	qsRegistry_.removeAll();
}



const std::string DBKnifeTimeHistoryRM::DESCRIPTOR = "WRAPPERKNIFETIMES";



bool DBKnifeTimeHistoryRM::registerRM(DBKnifeTimeHistoryRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBKnifeTimeHistoryRM>;
	}

	return _registry_->add(&view);
*/
}


void DBKnifeTimeHistoryRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBKnifeTimeHistoryRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBKnifeTimeHistoryRM>* DBKnifeTimeHistoryRM::_registry_ = 0;
util_thread::Mutex<> DBKnifeTimeHistoryRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
