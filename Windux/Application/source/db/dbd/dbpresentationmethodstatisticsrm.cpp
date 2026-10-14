#include <string>


#include "edb.h"
#include DBPRESENTATIONMETHODSTATISTICSRM_INC
#include DBPRESENTATIONMETHODSTATISTIC_INC
#include DBPRESENTATIONMETHOD_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBPresentationMethodStatisticsRM::DBPresentationMethodStatisticsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBPresentationMethodStatisticsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBPresentationMethodStatisticsRM::~DBPresentationMethodStatisticsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBPresentationMethodStatistic>* DBPresentationMethodStatisticsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBPresentationMethodStatistic>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBPresentationMethodStatistic>* DBPresentationMethodStatisticsRM::retrieveLinkedtoPresentationMethod(const DBPresentationMethod* method)
{
	if ( enabled_ && method )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBPresentationMethodStatistic>(dbm_, decorated_->retrieveLinkedtoPresentationMethod(method->decorated_), *this);
	}
	else
		return 0;
}


DBPresentationMethodStatistic* DBPresentationMethodStatisticsRM::find(const DBPresentationMethod* method, const std::string& lang, const std::string& hour) const
{
	if ( enabled_ && method )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBPresentationMethodStatisticI* rec = decorated_->find(method->decorated_, lang, hour);

		if ( rec )
			return new DBPresentationMethodStatistic(rec);
	}

	return 0;
}


DBPresentationMethodStatistic* DBPresentationMethodStatisticsRM::createRecord(const DBPresentationMethod* method) const
{
	if ( enabled_ && method )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBPresentationMethodStatisticI* rec = decorated_->createRecord(method->decorated_);

		if ( rec )
		{
			DBPresentationMethodStatistic* r = new DBPresentationMethodStatistic(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBPresentationMethodStatisticsRM::saveRecord(const DBPresentationMethodStatistic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBPresentationMethodStatisticsRM::deleteRecord(const DBPresentationMethodStatistic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBPresentationMethodStatisticsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBPresentationMethodStatisticsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBPresentationMethodStatisticsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBPresentationMethodStatisticsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBPresentationMethodStatisticsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBPresentationMethodStatisticsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBPresentationMethodStatisticsRM::show()
{
	Debug::trace("database.show", 20, "DBPresentationMethodStatisticsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBPresentationMethodStatisticsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBPresentationMethodStatistic>::show);
}


#ifndef EVOL_NDEBUG
void DBPresentationMethodStatisticsRM::showAll(const std::string& dc)
#else
void DBPresentationMethodStatisticsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBPresentationMethodStatisticsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBPresentationMethodStatisticsRM
		_registry_->process_mem(&DBPresentationMethodStatisticsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBPresentationMethodStatisticsRM INSTANCES: None");
#endif
}


void DBPresentationMethodStatisticsRM::disable()
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


bool DBPresentationMethodStatisticsRM::registerQ(DBQuery<DBPresentationMethodStatistic>* query)
{
	return qsRegistry_.add(query);
}


bool DBPresentationMethodStatisticsRM::deregisterQ(DBQuery<DBPresentationMethodStatistic>* query)
{
	return qsRegistry_.remove(query);
}


void DBPresentationMethodStatisticsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBPresentationMethodStatistic>::disable);
	qsRegistry_.removeAll();
}



const std::string DBPresentationMethodStatisticsRM::DESCRIPTOR = "PRESENTATIONMETHODSTATISTIC";



bool DBPresentationMethodStatisticsRM::registerRM(DBPresentationMethodStatisticsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBPresentationMethodStatisticsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBPresentationMethodStatisticsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBPresentationMethodStatisticsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBPresentationMethodStatisticsRM>* DBPresentationMethodStatisticsRM::_registry_ = 0;
util_thread::Mutex<> DBPresentationMethodStatisticsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
