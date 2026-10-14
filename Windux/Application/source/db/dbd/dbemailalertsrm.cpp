#include <string>


#include "edb.h"
#include DBEMAILALERTSRM_INC
#include DBEMAILALERT_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBEmailAlertsRM::DBEmailAlertsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBEmailAlertsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBEmailAlertsRM::~DBEmailAlertsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBEmailAlert>* DBEmailAlertsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBEmailAlert>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBEmailAlert>* DBEmailAlertsRM::retrieveRangeNumber(const string& first, const string& last)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBEmailAlert>(dbm_, decorated_->retrieveRangeNumber(first, last), *this);
	}
	else
		return 0;
}


DBEmailAlert* DBEmailAlertsRM::findNumber(const string& number) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBEmailAlertI* rec = decorated_->findNumber(number);

		if ( rec )
			return new DBEmailAlert(rec);
	}

	return 0;
}


DBEmailAlert* DBEmailAlertsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBEmailAlertI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBEmailAlert* r = new DBEmailAlert(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBEmailAlertsRM::saveRecord(const DBEmailAlert* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBEmailAlertsRM::deleteRecord(const DBEmailAlert* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBEmailAlertsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBEmailAlertsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBEmailAlertsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBEmailAlertsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBEmailAlertsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBEmailAlertsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBEmailAlertsRM::show()
{
	Debug::trace("database.show", 20, "DBEmailAlertsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBEmailAlertsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBEmailAlert>::show);
}


#ifndef EVOL_NDEBUG
void DBEmailAlertsRM::showAll(const std::string& dc)
#else
void DBEmailAlertsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBEmailAlertsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBEmailAlertsRM
		_registry_->process_mem(&DBEmailAlertsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBEmailAlertsRM INSTANCES: None");
#endif
}


void DBEmailAlertsRM::disable()
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


bool DBEmailAlertsRM::registerQ(DBQuery<DBEmailAlert>* query)
{
	return qsRegistry_.add(query);
}


bool DBEmailAlertsRM::deregisterQ(DBQuery<DBEmailAlert>* query)
{
	return qsRegistry_.remove(query);
}


void DBEmailAlertsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBEmailAlert>::disable);
	qsRegistry_.removeAll();
}



const std::string DBEmailAlertsRM::DESCRIPTOR = "EMAILALERT";



bool DBEmailAlertsRM::registerRM(DBEmailAlertsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBEmailAlertsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBEmailAlertsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBEmailAlertsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBEmailAlertsRM>* DBEmailAlertsRM::_registry_ = 0;
util_thread::Mutex<> DBEmailAlertsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
