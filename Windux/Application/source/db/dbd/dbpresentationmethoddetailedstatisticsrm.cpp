#include <string>


#include "edb.h"
#include DBPRESENTATIONMETHODDETAILEDSTATISTICSRM_INC
#include DBPRESENTATIONMETHODDETAILEDSTATISTIC_INC
#include DBPRESENTATIONMETHOD_INC
#include DBPRESENTATIONMETHODSTATISTIC_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBPresentationMethodDetailedStatisticsRM::DBPresentationMethodDetailedStatisticsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBPresentationMethodDetailedStatisticsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBPresentationMethodDetailedStatisticsRM::~DBPresentationMethodDetailedStatisticsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBPresentationMethodDetailedStatistic>* DBPresentationMethodDetailedStatisticsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBPresentationMethodDetailedStatistic>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBPresentationMethodDetailedStatistic>* DBPresentationMethodDetailedStatisticsRM::retrieveLinkedtoPresentationMethodStatistic(const DBPresentationMethodStatistic* stat)
{
	if ( enabled_ && stat )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBPresentationMethodDetailedStatistic>(dbm_, decorated_->retrieveLinkedtoPresentationMethodStatistic(stat->decorated_), *this);
	}
	else
		return 0;
}


DBPresentationMethodDetailedStatistic* DBPresentationMethodDetailedStatisticsRM::find(const DBPresentationMethod* method, const DBPresentationMethodStatistic* stat) const
{
	if ( enabled_ && stat )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBPresentationMethodDetailedStatisticI* rec = decorated_->find(method->decorated_, stat->decorated_);

		if ( rec )
			return new DBPresentationMethodDetailedStatistic(rec);
	}

	return 0;
}


DBPresentationMethodDetailedStatistic* DBPresentationMethodDetailedStatisticsRM::createRecord(const DBPresentationMethod* method, const DBPresentationMethodStatistic* stat) const
{
	if ( enabled_ && method && stat )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBPresentationMethodDetailedStatisticI* rec = decorated_->createRecord(method->decorated_, stat->decorated_);

		if ( rec )
		{
			DBPresentationMethodDetailedStatistic* r = new DBPresentationMethodDetailedStatistic(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBPresentationMethodDetailedStatisticsRM::saveRecord(const DBPresentationMethodDetailedStatistic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBPresentationMethodDetailedStatisticsRM::deleteRecord(const DBPresentationMethodDetailedStatistic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBPresentationMethodDetailedStatisticsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBPresentationMethodDetailedStatisticsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBPresentationMethodDetailedStatisticsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBPresentationMethodDetailedStatisticsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBPresentationMethodDetailedStatisticsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBPresentationMethodDetailedStatisticsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBPresentationMethodDetailedStatisticsRM::show()
{
	Debug::trace("database.show", 20, "DBPresentationMethodDetailedStatisticsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBPresentationMethodDetailedStatisticsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBPresentationMethodDetailedStatistic>::show);
}


#ifndef EVOL_NDEBUG
void DBPresentationMethodDetailedStatisticsRM::showAll(const std::string& dc)
#else
void DBPresentationMethodDetailedStatisticsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBPresentationMethodDetailedStatisticsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBPresentationMethodDetailedStatisticsRM
		_registry_->process_mem(&DBPresentationMethodDetailedStatisticsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBPresentationMethodDetailedStatisticsRM INSTANCES: None");
#endif
}


void DBPresentationMethodDetailedStatisticsRM::disable()
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


bool DBPresentationMethodDetailedStatisticsRM::registerQ(DBQuery<DBPresentationMethodDetailedStatistic>* query)
{
	return qsRegistry_.add(query);
}


bool DBPresentationMethodDetailedStatisticsRM::deregisterQ(DBQuery<DBPresentationMethodDetailedStatistic>* query)
{
	return qsRegistry_.remove(query);
}


void DBPresentationMethodDetailedStatisticsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBPresentationMethodDetailedStatistic>::disable);
	qsRegistry_.removeAll();
}



const std::string DBPresentationMethodDetailedStatisticsRM::DESCRIPTOR = "PRESENTATIONMETHODDETAILEDSTATISTIC";



bool DBPresentationMethodDetailedStatisticsRM::registerRM(DBPresentationMethodDetailedStatisticsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBPresentationMethodDetailedStatisticsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBPresentationMethodDetailedStatisticsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBPresentationMethodDetailedStatisticsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBPresentationMethodDetailedStatisticsRM>* DBPresentationMethodDetailedStatisticsRM::_registry_ = 0;
util_thread::Mutex<> DBPresentationMethodDetailedStatisticsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
