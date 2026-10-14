#include <string>


#include "edb.h"
#include DBCATEGORIESRM_INC
#include DBCATEGORY_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCategoriesRM::DBCategoriesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCategoriesRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCategoriesRM::~DBCategoriesRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCategory>* DBCategoriesRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategory>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBCategory>* DBCategoriesRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategory>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBCategory>* DBCategoriesRM::retrieveAllByName()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategory>(dbm_, decorated_->retrieveAllByName(), *this);
	}
	else
		return 0;
}


DBQuery<DBCategory>* DBCategoriesRM::retrieveAllByName(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategory>(dbm_, decorated_->retrieveAllByName(deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBCategory>* DBCategoriesRM::retrieveRangeNumber(const string& first, const string& last, const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategory>(dbm_, decorated_->retrieveRangeNumber(first, last, deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBCategory>* DBCategoriesRM::retrieveChildCategories(const DBCategory* ca)
{
	if ( enabled_ && ca )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategory>(dbm_, decorated_->retrieveChildCategories(ca->getNumber(), ca->getDeptNumber()), *this);
	}
	else
		return 0;
}


DBQuery<DBCategory>* DBCategoriesRM::retrieveAllSubcategories(const std::string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategory>(dbm_, decorated_->retrieveAllSubcategories(deptNum), *this);
	}
	else
		return 0;
}


DBCategory* DBCategoriesRM::findNumber(const string& number, const string& deptNum) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCategoryI* rec = decorated_->findNumber(number, deptNum);

		if ( rec )
			return new DBCategory(rec);
	}

	return 0;
}


DBCategory* DBCategoriesRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBCategoryI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBCategory* r = new DBCategory(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBCategoriesRM::saveRecord(const DBCategory* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCategoriesRM::deleteRecord(const DBCategory* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCategoriesRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBCategoriesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCategoriesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCategoriesRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBCategoriesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCategoriesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCategoriesRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCategoriesRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCategoriesRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCategoriesRM::show()
{
	Debug::trace("database.show", 20, "DBCategoriesRM instance address: " + 
		util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCategoriesRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCategory>::show);
}


#ifndef EVOL_NDEBUG
void DBCategoriesRM::showAll(const std::string& dc)
#else
void DBCategoriesRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCategoriesRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCategoriesRM
		_registry_->process_mem(&DBCategoriesRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCategoriesRM INSTANCES: None");
#endif
}


void DBCategoriesRM::disable()
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


bool DBCategoriesRM::registerQ(DBQuery<DBCategory>* query)
{
	return qsRegistry_.add(query);
}


bool DBCategoriesRM::deregisterQ(DBQuery<DBCategory>* query)
{
	return qsRegistry_.remove(query);
}


void DBCategoriesRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCategory>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCategoriesRM::DESCRIPTOR = "CATEGORY";



bool DBCategoriesRM::registerRM(DBCategoriesRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCategoriesRM>;
	}

	return _registry_->add(&view);
*/
}


void DBCategoriesRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCategoriesRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCategoriesRM>* DBCategoriesRM::_registry_ = 0;
util_thread::Mutex<> DBCategoriesRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
