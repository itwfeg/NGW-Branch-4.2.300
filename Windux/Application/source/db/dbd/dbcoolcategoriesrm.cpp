#include <string>


#include "edb.h"
#include DBCOOLCATEGORIESRM_INC
#include DBCOOLCATEGORY_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolCategoriesRM::DBCoolCategoriesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolCategoriesRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCoolCategoriesRM::~DBCoolCategoriesRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCoolCategory>* DBCoolCategoriesRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategory>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBCoolCategory>* DBCoolCategoriesRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategory>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}

DBQuery<DBCoolCategory>* DBCoolCategoriesRM::retrieveRangeNumber(const string& first, const string& last, const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategory>(dbm_, decorated_->retrieveRangeNumber(first, last, deptNum), *this);
	}
	else
		return 0;
}


DBCoolCategory* DBCoolCategoriesRM::findNumber(const string& number, const string& deptNum) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCoolCategoryI* rec = decorated_->findNumber(number, deptNum);

		if ( rec )
			return new DBCoolCategory(rec);
	}

	return 0;
}


DBCoolCategory* DBCoolCategoriesRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBCoolCategoryI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBCoolCategory* r = new DBCoolCategory(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBCoolCategoriesRM::saveRecord(const DBCoolCategory* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCoolCategoriesRM::deleteRecord(const DBCoolCategory* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCoolCategoriesRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBCoolCategoriesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolCategoriesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolCategoriesRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBCoolCategoriesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolCategoriesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolCategoriesRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCoolCategoriesRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCoolCategoriesRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCoolCategoriesRM::show()
{
	Debug::trace("database.show", 20, "DBCoolCategoriesRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCoolCategoriesRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolCategory>::show);
}


#ifndef EVOL_NDEBUG
void DBCoolCategoriesRM::showAll(const std::string& dc)
#else
void DBCoolCategoriesRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCoolCategoriesRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCoolCategoriesRM
		_registry_->process_mem(&DBCoolCategoriesRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCoolCategoriesRM INSTANCES: None");
#endif
}


void DBCoolCategoriesRM::disable()
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


bool DBCoolCategoriesRM::registerQ(DBQuery<DBCoolCategory>* query)
{
	return qsRegistry_.add(query);
}


bool DBCoolCategoriesRM::deregisterQ(DBQuery<DBCoolCategory>* query)
{
	return qsRegistry_.remove(query);
}


void DBCoolCategoriesRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolCategory>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCoolCategoriesRM::DESCRIPTOR = "COOLCATEGORY";



bool DBCoolCategoriesRM::registerRM(DBCoolCategoriesRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCoolCategoriesRM>;
	}

	return _registry_->add(&view);
	*/
}


void DBCoolCategoriesRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCoolCategoriesRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCoolCategoriesRM>* DBCoolCategoriesRM::_registry_ = 0;
util_thread::Mutex<> DBCoolCategoriesRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
