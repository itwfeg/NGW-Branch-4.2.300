#include <string>


#include "edb.h"
#include DBCOOLCATEGORYCOUNTRIESRM_INC
#include DBCOOLCATEGORYCOUNTRY_INC
#include DBCOOLCOUNTRY_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolCategoryCountriesRM::DBCoolCategoryCountriesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolCategoryCountriesRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCoolCategoryCountriesRM::~DBCoolCategoryCountriesRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable category (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCoolCategoryCountry>* DBCoolCategoryCountriesRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryCountry>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}

DBQuery<DBCoolCategoryCountry>* DBCoolCategoryCountriesRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryCountry>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}

DBQuery<DBCoolCategoryCountry>* DBCoolCategoryCountriesRM::retrieveLinkedtoCoolCategory(const std::string& category, const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryCountry>(dbm_, decorated_->retrieveLinkedtoCoolCategory(category, deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBCoolCategoryCountry>* DBCoolCategoryCountriesRM::retrieveLinkedtoCoolCountry(const DBCoolCountry* country)
{
	if ( enabled_ && country )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryCountry>(dbm_, decorated_->retrieveLinkedtoCoolCountry(country->decorated_), *this);
	}
	else
		return 0;
}


DBCoolCategoryCountry* DBCoolCategoryCountriesRM::find(const std::string& category, const DBCoolCountry* country)
{
	if ( enabled_ && country )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCoolCategoryCountryI* rec = decorated_->find(category, country->decorated_);

		if ( rec )
			return new DBCoolCategoryCountry(rec);
	}

	return 0;
}


DBCoolCategoryCountry* DBCoolCategoryCountriesRM::createRecord(const std::string& category, const DBCoolCountry* country) const
{
	if ( enabled_ && country && category.size() )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBCoolCategoryCountryI* rec = decorated_->createRecord(category, country->decorated_);

		if ( rec )
		{
			DBCoolCategoryCountry* r = new DBCoolCategoryCountry(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBCoolCategoryCountriesRM::saveRecord(const DBCoolCategoryCountry* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCoolCategoryCountriesRM::deleteRecord(const DBCoolCategoryCountry* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCoolCategoryCountriesRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBCoolCategoryCountriesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolCategoryCountriesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolCategoryCountriesRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBCoolCategoryCountriesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolCategoryCountriesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolCategoryCountriesRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCoolCategoryCountriesRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCoolCategoryCountriesRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCoolCategoryCountriesRM::show()
{
	Debug::trace("database.show", 20, "DBCoolCategoryCountriesRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCoolCategoryCountriesRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolCategoryCountry>::show);
}


#ifndef EVOL_NDEBUG
void DBCoolCategoryCountriesRM::showAll(const std::string& dc)
#else
void DBCoolCategoryCountriesRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCoolCategoryCountriesRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCoolCategoryCountriesRM
		_registry_->process_mem(&DBCoolCategoryCountriesRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCoolCategoryCountriesRM INSTANCES: None");
#endif
}


void DBCoolCategoryCountriesRM::disable()
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


bool DBCoolCategoryCountriesRM::registerQ(DBQuery<DBCoolCategoryCountry>* query)
{
	return qsRegistry_.add(query);
}


bool DBCoolCategoryCountriesRM::deregisterQ(DBQuery<DBCoolCategoryCountry>* query)
{
	return qsRegistry_.remove(query);
}


void DBCoolCategoryCountriesRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolCategoryCountry>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCoolCategoryCountriesRM::DESCRIPTOR = "COOLCATEGORYCOUNTRY";



bool DBCoolCategoryCountriesRM::registerRM(DBCoolCategoryCountriesRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCoolCategoryCountriesRM>;
	}

	return _registry_->add(&view);
*/
}


void DBCoolCategoryCountriesRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCoolCategoryCountriesRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCoolCategoryCountriesRM>* DBCoolCategoryCountriesRM::_registry_ = 0;
util_thread::Mutex<> DBCoolCategoryCountriesRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
