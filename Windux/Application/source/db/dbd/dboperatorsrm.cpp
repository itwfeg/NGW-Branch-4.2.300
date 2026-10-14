#include <string>


#include "edb.h"
#include DBOPERATORSRM_INC
#include DBOPERATOR_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBOperatorsRM::DBOperatorsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBOperatorsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBOperatorsRM::~DBOperatorsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBOperator>* DBOperatorsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBOperator>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBOperator>* DBOperatorsRM::retrieveRangeNumber(const string& first, const string& last)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBOperator>(dbm_, decorated_->retrieveRangeNumber(first, last), *this);
	}
	else
		return 0;
}


DBOperator* DBOperatorsRM::findNumber(const string& number) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBOperatorI* rec = decorated_->findNumber(number);

		if ( rec )
			return new DBOperator(rec);
	}

	return 0;
}


DBOperator* DBOperatorsRM::findRFID(const std::string& id) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBOperatorI* rec = decorated_->findRFID(id);

		if ( rec )
			return new DBOperator(rec);
	}

	return 0;
}


DBOperator* DBOperatorsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBOperatorI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBOperator* r = new DBOperator(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBOperatorsRM::saveRecord(const DBOperator* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBOperatorsRM::deleteRecord(const DBOperator* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBOperatorsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBOperatorsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBOperatorsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBOperatorsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBOperatorsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBOperatorsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBOperatorsRM::show()
{
	Debug::trace("database.show", 20, "DBOperatorsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	if ( enabled_ )
		decorated_->show();
}


void DBOperatorsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBOperator>::show);
}


#ifndef EVOL_NDEBUG
void DBOperatorsRM::showAll(const std::string& dc)
#else
void DBOperatorsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBOperatorsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBOperatorsRM
		_registry_->process_mem(&DBOperatorsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBOperatorsRM INSTANCES: None");
#endif
}


void DBOperatorsRM::disable()
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


bool DBOperatorsRM::registerQ(DBQuery<DBOperator>* query)
{
	return qsRegistry_.add(query);
}


bool DBOperatorsRM::deregisterQ(DBQuery<DBOperator>* query)
{
	return qsRegistry_.remove(query);
}


void DBOperatorsRM::disableAllQs()
{
	// Make all the queries disabled and removed them from the registry
	//qsRegistry_.process_mem_safe(&DBQuery<DBOperator>::disable);
	qsRegistry_.process_mem(&DBQuery<DBOperator>::disable);

	// There should not be any more queries in the registry since they are supposed to remove
	// themselves during the disabling process
	qsRegistry_.removeAll();
}



const std::string DBOperatorsRM::DESCRIPTOR = "OPERATOR";



bool DBOperatorsRM::registerRM(DBOperatorsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBOperatorsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBOperatorsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBOperatorsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBOperatorsRM>* DBOperatorsRM::_registry_ = 0;
util_thread::Mutex<> DBOperatorsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
