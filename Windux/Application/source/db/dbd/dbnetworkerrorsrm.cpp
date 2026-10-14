#include <string>


#include "edb.h"
#include DBNETWORKERRORSRM_INC
#include DBNETWORKERROR_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBNetworkErrorsRM::DBNetworkErrorsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBNetworkErrorsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBNetworkErrorsRM::~DBNetworkErrorsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBNetworkError>* DBNetworkErrorsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBNetworkError>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


/*
DBQuery<DBNetworkError>* DBNetworkErrorsRM::retrieveRangeNumber(const string& first, const string& last)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBNetworkError>(dbm_, decorated_->retrieveRangeNumber(first, last), *this);
	}
	else
		return 0;
}
*/


DBNetworkError* DBNetworkErrorsRM::findID(const string& id) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBNetworkErrorI* rec = decorated_->findID(id);

		if ( rec )
			return new DBNetworkError(rec);
	}

	return 0;
}


DBNetworkError* DBNetworkErrorsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBNetworkErrorI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBNetworkError* r = new DBNetworkError(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBNetworkErrorsRM::saveRecord(const DBNetworkError* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBNetworkErrorsRM::deleteRecord(const DBNetworkError* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBNetworkErrorsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBNetworkErrorsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBNetworkErrorsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBNetworkErrorsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBNetworkErrorsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBNetworkErrorsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBNetworkErrorsRM::show()
{
	Debug::trace("database.show", 20, "DBNetworkErrorsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBNetworkErrorsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBNetworkError>::show);
}


#ifndef EVOL_NDEBUG
void DBNetworkErrorsRM::showAll(const std::string& dc)
#else
void DBNetworkErrorsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBNetworkErrorsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBNetworkErrorsRM
		_registry_->process_mem(&DBNetworkErrorsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBNetworkErrorsRM INSTANCES: None");
#endif
}


void DBNetworkErrorsRM::disable()
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


bool DBNetworkErrorsRM::registerQ(DBQuery<DBNetworkError>* query)
{
	return qsRegistry_.add(query);
}


bool DBNetworkErrorsRM::deregisterQ(DBQuery<DBNetworkError>* query)
{
	return qsRegistry_.remove(query);
}


void DBNetworkErrorsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBNetworkError>::disable);
	qsRegistry_.removeAll();
}



const std::string DBNetworkErrorsRM::DESCRIPTOR = "NETWORKERROR";



bool DBNetworkErrorsRM::registerRM(DBNetworkErrorsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBNetworkErrorsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBNetworkErrorsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBNetworkErrorsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBNetworkErrorsRM>* DBNetworkErrorsRM::_registry_ = 0;
util_thread::Mutex<> DBNetworkErrorsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
