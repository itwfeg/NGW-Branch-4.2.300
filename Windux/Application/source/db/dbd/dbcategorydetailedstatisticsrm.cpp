#include <string>


#include "edb.h"
#include DBCATEGORYDETAILEDSTATISTICSRM_INC
#include DBCATEGORYDETAILEDSTATISTIC_INC
#include DBPRESENTATIONMETHOD_INC
#include DBCATEGORY_INC
#include DBCATEGORYSTATISTIC_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCategoryDetailedStatisticsRM::DBCategoryDetailedStatisticsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCategoryDetailedStatisticsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCategoryDetailedStatisticsRM::~DBCategoryDetailedStatisticsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCategoryDetailedStatistic>* DBCategoryDetailedStatisticsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategoryDetailedStatistic>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBCategoryDetailedStatistic>* DBCategoryDetailedStatisticsRM::retrieveLinkedtoCategoryStatistic(const DBCategoryStatistic* stat)
{
	if ( enabled_ && stat )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategoryDetailedStatistic>(dbm_, decorated_->retrieveLinkedtoCategoryStatistic(stat->decorated_), *this);
	}

	return 0;
}


DBCategoryDetailedStatistic* DBCategoryDetailedStatisticsRM::find(const DBPresentationMethod* method, const DBCategoryStatistic* stat) const
{
	if ( enabled_ && stat )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCategoryDetailedStatisticI* rec = decorated_->find(method->decorated_, stat->decorated_);

		if ( rec )
			return new DBCategoryDetailedStatistic(rec);
	}

	return 0;
}


DBCategoryDetailedStatistic* DBCategoryDetailedStatisticsRM::createRecord(const DBPresentationMethod* method, const DBCategoryStatistic* stat) const
{
	if ( enabled_ && method && stat )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBCategoryDetailedStatisticI* rec = decorated_->createRecord(method->decorated_, stat->decorated_);

		if ( rec )
		{
			DBCategoryDetailedStatistic* r = new DBCategoryDetailedStatistic(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBCategoryDetailedStatisticsRM::saveRecord(const DBCategoryDetailedStatistic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCategoryDetailedStatisticsRM::deleteRecord(const DBCategoryDetailedStatistic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCategoryDetailedStatisticsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBCategoryDetailedStatisticsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCategoryDetailedStatisticsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCategoryDetailedStatisticsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCategoryDetailedStatisticsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCategoryDetailedStatisticsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCategoryDetailedStatisticsRM::show()
{
	Debug::trace("database.show", 20, "DBCategoryDetailedStatisticsRM instance address: " + 
		util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCategoryDetailedStatisticsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCategoryDetailedStatistic>::show);
}


#ifndef EVOL_NDEBUG
void DBCategoryDetailedStatisticsRM::showAll(const std::string& dc)
#else
void DBCategoryDetailedStatisticsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCategoryDetailedStatisticsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCategoryDetailedStatisticsRM
		_registry_->process_mem(&DBCategoryDetailedStatisticsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCategoryDetailedStatisticsRM INSTANCES: None");
#endif
}


void DBCategoryDetailedStatisticsRM::disable()
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


bool DBCategoryDetailedStatisticsRM::registerQ(DBQuery<DBCategoryDetailedStatistic>* query)
{
	return qsRegistry_.add(query);
}


bool DBCategoryDetailedStatisticsRM::deregisterQ(DBQuery<DBCategoryDetailedStatistic>* query)
{
	return qsRegistry_.remove(query);
}


void DBCategoryDetailedStatisticsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCategoryDetailedStatistic>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCategoryDetailedStatisticsRM::DESCRIPTOR = "CATEGORYDETAILEDSTATISTIC";



bool DBCategoryDetailedStatisticsRM::registerRM(DBCategoryDetailedStatisticsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCategoryDetailedStatisticsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBCategoryDetailedStatisticsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCategoryDetailedStatisticsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCategoryDetailedStatisticsRM>* DBCategoryDetailedStatisticsRM::_registry_ = 0;
util_thread::Mutex<> DBCategoryDetailedStatisticsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
