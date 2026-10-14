#include <string>


#include "edb.h"
#include DBCUTTINGTESTSRM_INC
#include DBCUTTINGTEST_INC
#include DBOPERATOR_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCuttingTestsRM::DBCuttingTestsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCuttingTestsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCuttingTestsRM::~DBCuttingTestsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);
	
	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCuttingTest>* DBCuttingTestsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);
		
		return new DBQuery<DBCuttingTest>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBCuttingTest>* DBCuttingTestsRM::retrieveByOperator(const string& opNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCuttingTest>(dbm_, decorated_->retrieveByOperator(opNum), *this);
	}

	return 0;
}


DBCuttingTest* DBCuttingTestsRM::find(const string& id) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);
		
		edb_adapter::DBCuttingTestI* rec = decorated_->find(id);

		if ( rec )
			return new DBCuttingTest(rec);
	}

	return 0;
}


DBCuttingTest* DBCuttingTestsRM::createRecord(const std::string& testNumber) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);
		
		edb_adapter::DBCuttingTestI* rec = decorated_->createRecord(testNumber);

		if ( rec )
		{
			DBCuttingTest* r = new DBCuttingTest(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBCuttingTestsRM::saveRecord(const DBCuttingTest* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);
		
		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCuttingTestsRM::deleteRecord(const DBCuttingTest* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);
		
		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCuttingTestsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);
		
		return decorated_->deleteAll();
	}
	else
		return 0;
}


int DBCuttingTestsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);
		
		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCuttingTestsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);
		
		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCuttingTestsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCuttingTestsRM::show()
{
	Debug::trace("database.show", 20, "DBCuttingTestsRM instance address: " + 
			 util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " +  util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCuttingTestsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCuttingTest>::show);
}


#ifndef EVOL_NDEBUG
void DBCuttingTestsRM::showAll(const std::string& dc)
#else
void DBCuttingTestsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCuttingTestsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCuttingTestsRM
		_registry_->process_mem(&DBCuttingTestsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCuttingTestsRM INSTANCES: None");
#endif
}


void DBCuttingTestsRM::disable()
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


bool DBCuttingTestsRM::registerQ(DBQuery<DBCuttingTest>* query)
{
	return qsRegistry_.add(query);
}


bool DBCuttingTestsRM::deregisterQ(DBQuery<DBCuttingTest>* query)
{
	return qsRegistry_.remove(query);
}


void DBCuttingTestsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCuttingTest>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCuttingTestsRM::DESCRIPTOR = "CUTTINGTEST";



bool DBCuttingTestsRM::registerRM(DBCuttingTestsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCuttingTestsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBCuttingTestsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCuttingTestsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCuttingTestsRM>* DBCuttingTestsRM::_registry_ = 0;
util_thread::Mutex<> DBCuttingTestsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
