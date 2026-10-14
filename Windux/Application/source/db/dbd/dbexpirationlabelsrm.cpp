#include <string>


#include "edb.h"
#include DBEXPIRATIONLABELSRM_INC
#include DBEXPIRATIONLABEL_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBExpirationLabelsRM::DBExpirationLabelsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBExpirationLabelsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBExpirationLabelsRM::~DBExpirationLabelsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBExpirationLabel>* DBExpirationLabelsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBExpirationLabel>(dbm_, decorated_->retrieveAll(), *this);
	}

	return 0;
}

DBQuery<DBExpirationLabel>* DBExpirationLabelsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBExpirationLabel>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}

DBQuery<DBExpirationLabel>* DBExpirationLabelsRM::retrieveLinkedToPLU(const std::string& plu, const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBExpirationLabel>(dbm_, decorated_->retrieveLinkedToPLU(plu, deptNum), *this);
	}

	return 0;
}


DBExpirationLabel* DBExpirationLabelsRM::find(const string& key, const string& deptNum) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBExpirationLabelI* el = decorated_->find(key, deptNum);

		if ( el )
			return new DBExpirationLabel(el);
	}

	return 0;
}


DBExpirationLabel* DBExpirationLabelsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBExpirationLabelI* el = decorated_->createRecord();

		if ( el )
			return new DBExpirationLabel(el);
	}

	return 0;
}


void DBExpirationLabelsRM::saveRecord(const DBExpirationLabel* el) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(el->decorated_);
	}
}


int DBExpirationLabelsRM::deleteRecord(const DBExpirationLabel* el) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(el->decorated_);
	}

	return 0;
}


int DBExpirationLabelsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBExpirationLabelsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBExpirationLabelsRM*>(this));

		return result;
	}

	return 0;
}


int DBExpirationLabelsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBExpirationLabelsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBExpirationLabelsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBExpirationLabelsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}

	return 0;
}


int DBExpirationLabelsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}

	return 0;
}


bool DBExpirationLabelsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBExpirationLabelsRM::show()
{
	Debug::trace("database.show", 20, "DBExpirationLabelsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBExpirationLabelsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBExpirationLabel>::show);
}


#ifndef EVOL_NDEBUG
void DBExpirationLabelsRM::showAll(const std::string& dc)
#else
void DBExpirationLabelsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBExpirationLabelsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBExpirationLabelsRM
		_registry_->process_mem(&DBExpirationLabelsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBExpirationLabelsRM INSTANCES: None");
#endif
}


void DBExpirationLabelsRM::disable()
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


bool DBExpirationLabelsRM::registerQ(DBQuery<DBExpirationLabel>* query)
{
	return qsRegistry_.add(query);
}


bool DBExpirationLabelsRM::deregisterQ(DBQuery<DBExpirationLabel>* query)
{
	return qsRegistry_.remove(query);
}


void DBExpirationLabelsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBExpirationLabel>::disable);
	qsRegistry_.removeAll();
}



const std::string DBExpirationLabelsRM::DESCRIPTOR = "EXPIRATIONLABEL";



bool DBExpirationLabelsRM::registerRM(DBExpirationLabelsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBExpirationLabelsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBExpirationLabelsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBExpirationLabelsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBExpirationLabelsRM>* DBExpirationLabelsRM::_registry_ = 0;
util_thread::Mutex<> DBExpirationLabelsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
