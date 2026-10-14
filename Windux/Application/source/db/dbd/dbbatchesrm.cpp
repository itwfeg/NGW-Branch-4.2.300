#include <string>


#include "edb.h"
#include DBBATCHESRM_INC
#include DBBATCH_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBBatchesRM::DBBatchesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBBatchesRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBBatchesRM::~DBBatchesRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBBatch>* DBBatchesRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBBatch>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBBatch>* DBBatchesRM::retrieveCompleteBatchesOfOrigin(int origin)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBBatch>(dbm_, decorated_->retrieveCompleteBatchesOfOrigin(origin), *this);
	}
	else
		return 0;
}


DBQuery<DBBatch>* DBBatchesRM::retrieveCompleteBatches(int version)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBBatch>(dbm_, decorated_->retrieveCompleteBatches(version), *this);
	}
	else
		return 0;
}


DBQuery<DBBatch>* DBBatchesRM::retrieveIncompleteBatches()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBBatch>(dbm_, decorated_->retrieveIncompleteBatches(), *this);
	}
	else
		return 0;
}


DBQuery<DBBatch>* DBBatchesRM::retrievePendingSynchronizationBatches()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBBatch>(dbm_, decorated_->retrievePendingSynchronizationBatches(), *this);
	}
	else
		return 0;
}


DBQuery<DBBatch>* DBBatchesRM::retrievePendingDeleteBatches()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBBatch>(dbm_, decorated_->retrievePendingDeleteBatches(), *this);
	}
	else
		return 0;
}


DBQuery<DBBatch>* DBBatchesRM::retrieveSameSignatureBatches(const std::string& signature)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBBatch>(dbm_, decorated_->retrieveSameSignatureBatches(signature), *this);
	}
	else
		return 0;
}


DBQuery<DBBatch>* DBBatchesRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBBatch>(dbm_, decorated_->retrieveRangeNumber(first, last), *this);
	}
	else
		return 0;
}


DBBatch* DBBatchesRM::findNumber(const string& number) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBBatchI* rec = decorated_->findNumber(number);

		if ( rec )
			return new DBBatch(rec);
	}

	return 0;
}


DBBatch* DBBatchesRM::findVersion(const std::string& version) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBBatchI* rec = decorated_->findVersion(version);

		if ( rec )
			return new DBBatch(rec);
	}

	return 0;
}


DBBatch* DBBatchesRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBBatchI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBBatch* r = new DBBatch(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBBatchesRM::saveRecord(const DBBatch* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBBatchesRM::deleteRecord(const DBBatch* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBBatchesRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBBatchesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBBatchesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBBatchesRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBBatchesRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBBatchesRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBBatchesRM::ensureAutoIncrementValue(int value)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSemiautoIncrement);

		decorated_->ensureAutoIncrementValue(value);
	}
}


void DBBatchesRM::show()
{
	Debug::trace("database.show", 20, "DBBatchesRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBBatchesRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBBatch>::show);
}


#ifndef EVOL_NDEBUG
void DBBatchesRM::showAll(const std::string& dc)
#else
void DBBatchesRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBBatchesRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBBatchesRM
		_registry_->process_mem(&DBBatchesRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBBatchesRM INSTANCES: None");
#endif
}


void DBBatchesRM::disable()
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


bool DBBatchesRM::registerQ(DBQuery<DBBatch>* query)
{
	return qsRegistry_.add(query);
}


bool DBBatchesRM::deregisterQ(DBQuery<DBBatch>* query)
{
	return qsRegistry_.remove(query);
}


void DBBatchesRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBBatch>::disable);
	qsRegistry_.removeAll();
}



const std::string DBBatchesRM::DESCRIPTOR = "BATCH";



bool DBBatchesRM::registerRM(DBBatchesRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBBatchesRM>;
	}

	return _registry_->add(&view);
	*/
}


void DBBatchesRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBBatchesRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBBatchesRM>* DBBatchesRM::_registry_ = 0;
util_thread::Mutex<> DBBatchesRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
