#include <string>


#include "edb.h"
#include DBLABELSRM_INC
#include DBLABEL_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBLabelsRM::DBLabelsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBLabelsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBLabelsRM::~DBLabelsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBLabel>* DBLabelsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBLabel>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBLabel>* DBLabelsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBLabel>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBLabel>* DBLabelsRM::retrieveLabelNumListForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBLabel>(dbm_, decorated_->retrieveLabelNumListForDept(deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBLabel>* DBLabelsRM::retrieveRangeNumber(const string& first, const string& last, const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBLabel>(dbm_, decorated_->retrieveRangeNumber(first, last, deptNum), *this);
	}
	else
		return 0;
}


DBLabel* DBLabelsRM::findNumber(const string& number, const string& deptNum) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBLabelI* rec = decorated_->findNumber(number, deptNum);

		if ( rec )
			return new DBLabel(rec);
	}

	return 0;
}


DBLabel* DBLabelsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBLabelI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBLabel* r = new DBLabel(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBLabelsRM::saveRecord(const DBLabel* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBLabelsRM::deleteRecord(const DBLabel* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBLabelsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBLabelsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBLabelsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBLabelsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBLabelsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBLabelsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBLabelsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBLabelsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBLabelsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBLabelsRM::show()
{
	Debug::trace("database.show", 20, "DBLabelsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBLabelsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBLabel>::show);
}


#ifndef EVOL_NDEBUG
void DBLabelsRM::showAll(const std::string& dc)
#else
void DBLabelsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBLabelsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBLabelsRM
		_registry_->process_mem(&DBLabelsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBLabelsRM INSTANCES: None");
#endif
}


void DBLabelsRM::disable()
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


bool DBLabelsRM::registerQ(DBQuery<DBLabel>* query)
{
	return qsRegistry_.add(query);
}


bool DBLabelsRM::deregisterQ(DBQuery<DBLabel>* query)
{
	return qsRegistry_.remove(query);
}


void DBLabelsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBLabel>::disable);
	qsRegistry_.removeAll();
}



const std::string DBLabelsRM::DESCRIPTOR = "LABEL";



bool DBLabelsRM::registerRM(DBLabelsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBLabelsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBLabelsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBLabelsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBLabelsRM>* DBLabelsRM::_registry_ = 0;
util_thread::Mutex<> DBLabelsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
