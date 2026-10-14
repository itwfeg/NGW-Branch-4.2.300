#include <string>


#include "edb.h"
#include DBCOOLPRODUCTIONSTEPSRM_INC
#include DBCOOLPRODUCTIONSTEP_INC
#include DBCOOLCATEGORYPRODUCTIONSTEP_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolProductionStepsRM::DBCoolProductionStepsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolProductionStepsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCoolProductionStepsRM::~DBCoolProductionStepsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCoolProductionStep>* DBCoolProductionStepsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolProductionStep>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}

DBQuery<DBCoolProductionStep>* DBCoolProductionStepsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolProductionStep>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}

DBQuery<DBCoolProductionStep>* DBCoolProductionStepsRM::retrieveAllCategories(const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolProductionStep>(dbm_, decorated_->retrieveAllCategories(deptNum), *this);
	}
	else
		return 0;
}


DBCoolProductionStep* DBCoolProductionStepsRM::findProductionStep(const string& name, const string& deptNum) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCoolProductionStepI* rec = decorated_->findProductionStep(name, deptNum);

		if ( rec )
			return new DBCoolProductionStep(rec);
	}

	return 0;
}


DBCoolProductionStep* DBCoolProductionStepsRM::find(const DBCoolCategoryProductionStep* step)
{
	if ( enabled_ && step )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCoolProductionStepI* rec = decorated_->find(step->decorated_);

		if ( rec )
			return new DBCoolProductionStep(rec);
	}

	return 0;
}


DBCoolProductionStep* DBCoolProductionStepsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBCoolProductionStepI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBCoolProductionStep* r = new DBCoolProductionStep(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBCoolProductionStepsRM::saveRecord(const DBCoolProductionStep* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCoolProductionStepsRM::deleteRecord(const DBCoolProductionStep* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCoolProductionStepsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBCoolProductionStepsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolProductionStepsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolProductionStepsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBCoolProductionStepsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolProductionStepsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolProductionStepsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCoolProductionStepsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCoolProductionStepsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCoolProductionStepsRM::show()
{
	Debug::trace("database.show", 20, "DBCoolProductionStepsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCoolProductionStepsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolProductionStep>::show);
}


#ifndef EVOL_NDEBUG
void DBCoolProductionStepsRM::showAll(const std::string& dc)
#else
void DBCoolProductionStepsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCoolProductionStepsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCoolProductionStepsRM
		_registry_->process_mem(&DBCoolProductionStepsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCoolProductionStepsRM INSTANCES: None");
#endif
}


void DBCoolProductionStepsRM::disable()
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


bool DBCoolProductionStepsRM::registerQ(DBQuery<DBCoolProductionStep>* query)
{
	return qsRegistry_.add(query);
}


bool DBCoolProductionStepsRM::deregisterQ(DBQuery<DBCoolProductionStep>* query)
{
	return qsRegistry_.remove(query);
}


void DBCoolProductionStepsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolProductionStep>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCoolProductionStepsRM::DESCRIPTOR = "COOLPRODUCTIONSTEP";



bool DBCoolProductionStepsRM::registerRM(DBCoolProductionStepsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCoolProductionStepsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBCoolProductionStepsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCoolProductionStepsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCoolProductionStepsRM>* DBCoolProductionStepsRM::_registry_ = 0;
util_thread::Mutex<> DBCoolProductionStepsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
