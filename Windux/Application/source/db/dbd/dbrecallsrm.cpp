#include <string>


#include "edb.h"
#include DBRECALLSRM_INC
#include DBRECALL_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBRecallsRM::DBRecallsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBRecallsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBRecallsRM::~DBRecallsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBRecall>* DBRecallsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBRecall>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}

DBQuery<DBRecall>* DBRecallsRM::retrieveAcknowledged()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBRecall>(dbm_, decorated_->retrieveAcknowledged(), *this);
	}
	else
		return 0;
}

DBQuery<DBRecall>* DBRecallsRM::retrieveUnacknowledged()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBRecall>(dbm_, decorated_->retrieveUnacknowledged(), *this);
	}
	else
		return 0;
}

/*
DBQuery<DBRecall>* DBRecallsRM::retrieveRangeNumber(const string& first, const string& last)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBRecall>(dbm_, decorated_->retrieveRangeNumber(first, last), *this);
	}
	else
		return 0;
}
*/

DBRecall* DBRecallsRM::findNumber(const string& number) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBRecallI* rec = decorated_->findNumber(number);

		if ( rec )
			return new DBRecall(rec);
	}

	return 0;
}


DBRecall* DBRecallsRM::findName(const string& name) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBRecallI* rec = decorated_->findName(name);

		if ( rec )
			return new DBRecall(rec);
	}

	return 0;
}


DBRecall* DBRecallsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBRecallI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBRecall* r = new DBRecall(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBRecallsRM::saveRecord(const DBRecall* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBRecallsRM::deleteRecord(const DBRecall* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBRecallsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBRecallsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBRecallsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBRecallsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBRecallsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBRecallsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBRecallsRM::show()
{
	Debug::trace("database.show", 20, "DBRecallsRM instance address: " + util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	if ( enabled_ )
		decorated_->show();
}


void DBRecallsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBRecall>::show);
}


#ifndef EVOL_NDEBUG
void DBRecallsRM::showAll(const std::string& dc)
#else
void DBRecallsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBRecallsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBRecallsRM
		_registry_->process_mem(&DBRecallsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBRecallsRM INSTANCES: None");
#endif
}


void DBRecallsRM::disable()
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


bool DBRecallsRM::registerQ(DBQuery<DBRecall>* query)
{
	return qsRegistry_.add(query);
}


bool DBRecallsRM::deregisterQ(DBQuery<DBRecall>* query)
{
	return qsRegistry_.remove(query);
}


void DBRecallsRM::disableAllQs()
{
	// Make all the queries disabled and removed them from the registry
	//qsRegistry_.process_mem_safe(&DBQuery<DBRecall>::disable);
	qsRegistry_.process_mem(&DBQuery<DBRecall>::disable);

	// There should not be any more queries in the registry since they are supposed to remove
	// themselves during the disabling process
	qsRegistry_.removeAll();
}



const std::string DBRecallsRM::DESCRIPTOR = "RECALLS";



bool DBRecallsRM::registerRM(DBRecallsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

    /* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBRecallsRM>;
	}

	return _registry_->add(&view);
	*/
}


void DBRecallsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBRecallsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBRecallsRM>* DBRecallsRM::_registry_ = 0;
util_thread::Mutex<> DBRecallsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
