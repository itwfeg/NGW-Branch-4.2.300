#include <string>


#include "edb.h"
#include DBSPECIALPROGRAMSRM_INC
#include DBSPECIALPROGRAM_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSpecialProgramsRM::DBSpecialProgramsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSpecialProgramsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBSpecialProgramsRM::~DBSpecialProgramsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBSpecialProgram>* DBSpecialProgramsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSpecialProgram>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBSpecialProgram>* DBSpecialProgramsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSpecialProgram>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBSpecialProgram>* DBSpecialProgramsRM::retrieveRangeNumber(const string& first, const string& last, const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSpecialProgram>(dbm_, decorated_->retrieveRangeNumber(first, last, deptNum), *this);
	}
	else
		return 0;
}


DBSpecialProgram* DBSpecialProgramsRM::findNumber(const string& number, const string& deptNum) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBSpecialProgramI* rec = decorated_->findNumber(number, deptNum);

		if ( rec )
			return new DBSpecialProgram(rec);
	}

	return 0;
}


DBSpecialProgram* DBSpecialProgramsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBSpecialProgramI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBSpecialProgram* r = new DBSpecialProgram(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBSpecialProgramsRM::saveRecord(const DBSpecialProgram* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBSpecialProgramsRM::deleteRecord(const DBSpecialProgram* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBSpecialProgramsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBSpecialProgramsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBSpecialProgramsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBSpecialProgramsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBSpecialProgramsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBSpecialProgramsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBSpecialProgramsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBSpecialProgramsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBSpecialProgramsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBSpecialProgramsRM::show()
{
	Debug::trace("database.show", 20, "DBSpecialProgramsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBSpecialProgramsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBSpecialProgram>::show);
}


#ifndef EVOL_NDEBUG
void DBSpecialProgramsRM::showAll(const std::string& dc)
#else
void DBSpecialProgramsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBSpecialProgramsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBSpecialProgramsRM
		_registry_->process_mem(&DBSpecialProgramsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBSpecialProgramsRM INSTANCES: None");
#endif
}


void DBSpecialProgramsRM::disable()
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


bool DBSpecialProgramsRM::registerQ(DBQuery<DBSpecialProgram>* query)
{
	return qsRegistry_.add(query);
}


bool DBSpecialProgramsRM::deregisterQ(DBQuery<DBSpecialProgram>* query)
{
	return qsRegistry_.remove(query);
}


void DBSpecialProgramsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBSpecialProgram>::disable);
	qsRegistry_.removeAll();
}



const std::string DBSpecialProgramsRM::DESCRIPTOR = "SPECIALPROGRAM";



bool DBSpecialProgramsRM::registerRM(DBSpecialProgramsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBSpecialProgramsRM>;
	}

	return _registry_->add(&view);
	*/
}


void DBSpecialProgramsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBSpecialProgramsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBSpecialProgramsRM>* DBSpecialProgramsRM::_registry_ = 0;
util_thread::Mutex<> DBSpecialProgramsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
