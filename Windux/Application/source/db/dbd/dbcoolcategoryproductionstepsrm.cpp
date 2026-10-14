#include <string>


#include "edb.h"
#include DBCOOLCATEGORYPRODUCTIONSTEPSRM_INC
#include DBCOOLCATEGORYPRODUCTIONSTEP_INC
#include DBCOOLPRODUCTIONSTEP_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolCategoryProductionStepsRM::DBCoolCategoryProductionStepsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolCategoryProductionStepsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCoolCategoryProductionStepsRM::~DBCoolCategoryProductionStepsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable category (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCoolCategoryProductionStep>* DBCoolCategoryProductionStepsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryProductionStep>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}

DBQuery<DBCoolCategoryProductionStep>* DBCoolCategoryProductionStepsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryProductionStep>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBCoolCategoryProductionStep>* DBCoolCategoryProductionStepsRM::retrieveLinkedtoCoolCategory(const std::string& category, const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryProductionStep>(dbm_, decorated_->retrieveLinkedtoCoolCategory(category, deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBCoolCategoryProductionStep>* DBCoolCategoryProductionStepsRM::retrieveLinkedtoCoolProductionStep(const DBCoolProductionStep* step)
{
	if ( enabled_ && step )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryProductionStep>(dbm_, decorated_->retrieveLinkedtoCoolProductionStep(step->decorated_), *this);
	}
	else
		return 0;
}


DBCoolCategoryProductionStep* DBCoolCategoryProductionStepsRM::find(const std::string& category, const std::string& deptNum, const DBCoolProductionStep* step)
{
	if ( enabled_ && step )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCoolCategoryProductionStepI* rec = decorated_->find(category, deptNum, step->decorated_);

		if ( rec )
			return new DBCoolCategoryProductionStep(rec);
	}

	return 0;
}


DBCoolCategoryProductionStep* DBCoolCategoryProductionStepsRM::createRecord(const std::string& category, const DBCoolProductionStep* step) const
{
	if ( enabled_ && step && category.size() )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBCoolCategoryProductionStepI* rec = decorated_->createRecord(category, step->decorated_);

		if ( rec )
		{
			DBCoolCategoryProductionStep* r = new DBCoolCategoryProductionStep(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBCoolCategoryProductionStepsRM::saveRecord(const DBCoolCategoryProductionStep* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCoolCategoryProductionStepsRM::deleteRecord(const DBCoolCategoryProductionStep* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCoolCategoryProductionStepsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBCoolCategoryProductionStepsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolCategoryProductionStepsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolCategoryProductionStepsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBCoolCategoryProductionStepsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolCategoryProductionStepsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolCategoryProductionStepsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCoolCategoryProductionStepsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCoolCategoryProductionStepsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCoolCategoryProductionStepsRM::show()
{
	Debug::trace("database.show", 20, "DBCoolCategoryProductionStepsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCoolCategoryProductionStepsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolCategoryProductionStep>::show);
}


#ifndef EVOL_NDEBUG
void DBCoolCategoryProductionStepsRM::showAll(const std::string& dc)
#else
void DBCoolCategoryProductionStepsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCoolCategoryProductionStepsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCoolCategoryProductionStepsRM
		_registry_->process_mem(&DBCoolCategoryProductionStepsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCoolCategoryProductionStepsRM INSTANCES: None");
#endif
}


void DBCoolCategoryProductionStepsRM::disable()
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


bool DBCoolCategoryProductionStepsRM::registerQ(DBQuery<DBCoolCategoryProductionStep>* query)
{
	return qsRegistry_.add(query);
}


bool DBCoolCategoryProductionStepsRM::deregisterQ(DBQuery<DBCoolCategoryProductionStep>* query)
{
	return qsRegistry_.remove(query);
}


void DBCoolCategoryProductionStepsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolCategoryProductionStep>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCoolCategoryProductionStepsRM::DESCRIPTOR = "COOLCATEGORYPRODUCTIONSTEP";



bool DBCoolCategoryProductionStepsRM::registerRM(DBCoolCategoryProductionStepsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCoolCategoryProductionStepsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBCoolCategoryProductionStepsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCoolCategoryProductionStepsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCoolCategoryProductionStepsRM>* DBCoolCategoryProductionStepsRM::_registry_ = 0;
util_thread::Mutex<> DBCoolCategoryProductionStepsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
