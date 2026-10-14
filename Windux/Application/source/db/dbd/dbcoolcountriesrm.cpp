#include <string>


#include "edb.h"
#include DBCOOLCOUNTRIESRM_INC
#include DBCOOLCOUNTRY_INC
#include DBCOOLCATEGORYCOUNTRY_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolCountriesRM::DBCoolCountriesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolCountriesRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCoolCountriesRM::~DBCoolCountriesRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCoolCountry>* DBCoolCountriesRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCountry>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}

DBQuery<DBCoolCountry>* DBCoolCountriesRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCountry>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}

DBQuery<DBCoolCountry>* DBCoolCountriesRM::retrieveAllCategories(const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCountry>(dbm_, decorated_->retrieveAllCategories(deptNum), *this );
	}
	else
		return 0;
}


DBCoolCountry* DBCoolCountriesRM::findCountry(const string& name, const string& deptNum) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCoolCountryI* rec = decorated_->findCountry(name, deptNum);

		if ( rec )
			return new DBCoolCountry(rec);
	}

	return 0;
}


DBCoolCountry* DBCoolCountriesRM::find(const DBCoolCategoryCountry* country)
{
	if ( enabled_ && country )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCoolCountryI* rec = decorated_->find(country->decorated_);

		if ( rec )
			return new DBCoolCountry(rec);
	}

	return 0;
}


DBCoolCountry* DBCoolCountriesRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBCoolCountryI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBCoolCountry* r = new DBCoolCountry(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBCoolCountriesRM::saveRecord(const DBCoolCountry* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCoolCountriesRM::deleteRecord(const DBCoolCountry* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCoolCountriesRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBCoolCountriesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolCountriesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolCountriesRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBCoolCountriesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolCountriesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolCountriesRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCoolCountriesRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCoolCountriesRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCoolCountriesRM::show()
{
	Debug::trace("database.show", 20, "DBCoolCountriesRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCoolCountriesRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolCountry>::show);
}


#ifndef EVOL_NDEBUG
void DBCoolCountriesRM::showAll(const std::string& dc)
#else
void DBCoolCountriesRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCoolCountriesRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCoolCountriesRM
		_registry_->process_mem(&DBCoolCountriesRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCoolCountriesRM INSTANCES: None");
#endif
}


void DBCoolCountriesRM::disable()
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


bool DBCoolCountriesRM::registerQ(DBQuery<DBCoolCountry>* query)
{
	return qsRegistry_.add(query);
}


bool DBCoolCountriesRM::deregisterQ(DBQuery<DBCoolCountry>* query)
{
	return qsRegistry_.remove(query);
}


void DBCoolCountriesRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolCountry>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCoolCountriesRM::DESCRIPTOR = "COOLCOUNTRY";



bool DBCoolCountriesRM::registerRM(DBCoolCountriesRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCoolCountriesRM>;
	}

	return _registry_->add(&view);
*/
}


void DBCoolCountriesRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCoolCountriesRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCoolCountriesRM>* DBCoolCountriesRM::_registry_ = 0;
util_thread::Mutex<> DBCoolCountriesRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
