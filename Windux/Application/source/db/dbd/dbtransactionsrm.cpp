#include <string>


#include "edb.h"
#include DBTRANSACTIONSRM_INC
#include DBTRANSACTION_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBTransactionsRM::DBTransactionsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBTransactionsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBTransactionsRM::~DBTransactionsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


std::string DBTransactionsRM::getNextTransactionId()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSemiautoIncrement);

		return decorated_->getNextTransactionId();
	}

	return 0;
}


DBQuery<DBTransaction>* DBTransactionsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBTransaction>(dbm_, decorated_->retrieveAll(), *this);
	}

	return 0;
}


DBQuery<DBTransaction>* DBTransactionsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBTransaction>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBTransaction>* DBTransactionsRM::retrieveReadAndClear(bool equal, int cleared, const std::string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBTransaction>(dbm_, decorated_->retrieveReadAndClear(equal, cleared, deptNum), *this);
	}

	return 0;
}


DBQuery<DBTransaction>* DBTransactionsRM::retrieveOperatorVoidable(const std::string& operatorId, int voidValue, const std::string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBTransaction>(dbm_, decorated_->retrieveOperatorVoidable(operatorId, voidValue, deptNum), *this);
	}

	return 0;
}


DBQuery<DBTransaction>* DBTransactionsRM::retrieveSupervisorVoidable(const std::string& operatorId, const std::string& deptNum, const std::string& pluNumber, const std::string& startTime, const std::string& endTime, int voidValue)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBTransaction>(dbm_, decorated_->retrieveSupervisorVoidable(operatorId, deptNum, pluNumber, startTime, endTime, voidValue), *this);
	}

	return 0;
}


DBQuery<DBTransaction>* DBTransactionsRM::retrieveFilteredTransactions(const std::string& startTime, const std::string& endTime, const std::string& transType, int voidValue,
																		const std::string& deptNum, const std::string& pluNumber, const std::string& cutTestNumber, const std::string& operatorId, bool defaultOrder)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBTransaction>(dbm_, decorated_->retrieveFilteredTransactions(startTime, endTime, transType, voidValue, deptNum, pluNumber, cutTestNumber, operatorId, defaultOrder), *this);
	}

	return 0;
}


DBQuery<DBTransaction>* DBTransactionsRM::retrieveAllCutTestTransactions()		//	AWL-135, HA-134
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBTransaction>(dbm_, decorated_->retrieveAllCutTestTransactions(), *this);
	}

	return 0;
}


DBQuery<DBTransaction>* DBTransactionsRM::retrieveCutTestTransactionsForCutTest(const std::string& cutTestNumber)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBTransaction>(dbm_, decorated_->retrieveCutTestTransactionsForCutTest(cutTestNumber), *this);
	}

	return 0;
}


DBTransaction* DBTransactionsRM::findTransaction(const std::string& scaleId, const std::string& transId) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBTransactionI* rec = decorated_->findTransaction(scaleId, transId);

		if ( rec )
			return new DBTransaction(rec);
	}

	return 0;
}


DBTransaction* DBTransactionsRM::createRecord(const std::string& scaleId, const std::string& transId) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBTransactionI* rec = decorated_->createRecord(scaleId, transId);

		if ( rec )
		{
			DBTransaction* r = new DBTransaction(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBTransactionsRM::saveRecord(const DBTransaction* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBTransactionsRM::deleteRecord(const DBTransaction* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}

	return 0;
}


int DBTransactionsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBTransactionsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBTransactionsRM*>(this));

		return result;
	}

	return 0;
}


int DBTransactionsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBTransactionsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBTransactionsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBTransactionsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}

	return 0;
}


int DBTransactionsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}

	return 0;
}


bool DBTransactionsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBTransactionsRM::show()
{
	Debug::trace("database.show", 20, "DBTransactionsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBTransactionsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBTransaction>::show);
}


#ifndef EVOL_NDEBUG
void DBTransactionsRM::showAll(const std::string& dc)
#else
void DBTransactionsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBTransactionsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBTransactionsRM
		_registry_->process_mem(&DBTransactionsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBTransactionsRM INSTANCES: None");
#endif
}


void DBTransactionsRM::disable()
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


bool DBTransactionsRM::registerQ(DBQuery<DBTransaction>* query)
{
	return qsRegistry_.add(query);
}


bool DBTransactionsRM::deregisterQ(DBQuery<DBTransaction>* query)
{
	return qsRegistry_.remove(query);
}


void DBTransactionsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBTransaction>::disable);
	qsRegistry_.removeAll();
}



const std::string DBTransactionsRM::DESCRIPTOR = "TRANSACTION";



bool DBTransactionsRM::registerRM(DBTransactionsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;
	
	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBTransactionsRM>;
	}
	
	return _registry_->add(&view);
	*/
}


void DBTransactionsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBTransactionsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBTransactionsRM>* DBTransactionsRM::_registry_ = 0;
util_thread::Mutex<> DBTransactionsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
