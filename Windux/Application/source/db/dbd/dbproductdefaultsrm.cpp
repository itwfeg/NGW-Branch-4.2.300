#include <string>


#include "edb.h"
#include DBPRODUCTDEFAULTSRM_INC
#include DBPRODUCTDEFAULT_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBProductDefaultsRM::DBProductDefaultsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBProductDefaultsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBProductDefaultsRM::~DBProductDefaultsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBProductDefault>* DBProductDefaultsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProductDefault>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBProductDefault>* DBProductDefaultsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProductDefault>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}


DBProductDefault* DBProductDefaultsRM::findType(const string& type, const string& deptNum) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBProductDefaultI* rec = decorated_->findType(type, deptNum);

		if ( rec )
			return new DBProductDefault(rec);
	}

	return 0;
}


DBProductDefault* DBProductDefaultsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBProductDefaultI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBProductDefault* r = new DBProductDefault(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBProductDefaultsRM::saveRecord(const DBProductDefault* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBProductDefaultsRM::deleteRecord(const DBProductDefault* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBProductDefaultsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBProductDefaultsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBProductDefaultsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBProductDefaultsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBProductDefaultsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBProductDefaultsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBProductDefaultsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBProductDefaultsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBProductDefaultsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBProductDefaultsRM::show()
{
	Debug::trace("database.show", 20, "DBProductDefaultsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBProductDefaultsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBProductDefault>::show);
}


#ifndef EVOL_NDEBUG
void DBProductDefaultsRM::showAll(const std::string& dc)
#else
void DBProductDefaultsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBProductDefaultsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBProductDefaultsRM
		_registry_->process_mem(&DBProductDefaultsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBProductDefaultsRM INSTANCES: None");
#endif
}


void DBProductDefaultsRM::disable()
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


bool DBProductDefaultsRM::registerQ(DBQuery<DBProductDefault>* query)
{
	return qsRegistry_.add(query);
}


bool DBProductDefaultsRM::deregisterQ(DBQuery<DBProductDefault>* query)
{
	return qsRegistry_.remove(query);
}


void DBProductDefaultsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBProductDefault>::disable);
	qsRegistry_.removeAll();
}



const std::string DBProductDefaultsRM::DESCRIPTOR = "PRODUCTDEFAULT";



bool DBProductDefaultsRM::registerRM(DBProductDefaultsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBProductDefaultsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBProductDefaultsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBProductDefaultsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBProductDefaultsRM>* DBProductDefaultsRM::_registry_ = 0;
util_thread::Mutex<> DBProductDefaultsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
