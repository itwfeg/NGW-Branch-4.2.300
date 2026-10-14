#include <string>


#include "edb.h"
#include DBPRESENTATIONMETHODSRM_INC
#include DBPRESENTATIONMETHOD_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBPresentationMethodsRM::DBPresentationMethodsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBPresentationMethodsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBPresentationMethodsRM::~DBPresentationMethodsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBPresentationMethod>* DBPresentationMethodsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBPresentationMethod>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBPresentationMethod* DBPresentationMethodsRM::find(const std::string& type) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBPresentationMethodI* rec = decorated_->find(type);

		if ( rec )
			return new DBPresentationMethod(rec);
	}

	return 0;
}


DBPresentationMethod* DBPresentationMethodsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBPresentationMethodI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBPresentationMethod* r = new DBPresentationMethod(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBPresentationMethodsRM::saveRecord(const DBPresentationMethod* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBPresentationMethodsRM::deleteRecord(const DBPresentationMethod* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBPresentationMethodsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBPresentationMethodsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBPresentationMethodsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBPresentationMethodsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBPresentationMethodsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBPresentationMethodsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBPresentationMethodsRM::show()
{
	Debug::trace("database.show", 20, "DBPresentationMethodsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBPresentationMethodsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBPresentationMethod>::show);
}


#ifndef EVOL_NDEBUG
void DBPresentationMethodsRM::showAll(const std::string& dc)
#else
void DBPresentationMethodsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBPresentationMethodsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBPresentationMethodsRM
		_registry_->process_mem(&DBPresentationMethodsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBPresentationMethodsRM INSTANCES: None");
#endif
}


void DBPresentationMethodsRM::disable()
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


bool DBPresentationMethodsRM::registerQ(DBQuery<DBPresentationMethod>* query)
{
	return qsRegistry_.add(query);
}


bool DBPresentationMethodsRM::deregisterQ(DBQuery<DBPresentationMethod>* query)
{
	return qsRegistry_.remove(query);
}


void DBPresentationMethodsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBPresentationMethod>::disable);
	qsRegistry_.removeAll();
}



const std::string DBPresentationMethodsRM::DESCRIPTOR = "PRESENTATIONMETHOD";



bool DBPresentationMethodsRM::registerRM(DBPresentationMethodsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBPresentationMethodsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBPresentationMethodsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBPresentationMethodsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBPresentationMethodsRM>* DBPresentationMethodsRM::_registry_ = 0;
util_thread::Mutex<> DBPresentationMethodsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
