#include <string>


#include "edb.h"
#include DBCATEGORYSTATISTICSRM_INC
#include DBCATEGORYSTATISTIC_INC
#include DBCATEGORY_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCategoryStatisticsRM::DBCategoryStatisticsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCategoryStatisticsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCategoryStatisticsRM::~DBCategoryStatisticsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCategoryStatistic>* DBCategoryStatisticsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategoryStatistic>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBCategoryStatistic>* DBCategoryStatisticsRM::retrieveLinkedtoCategory(const DBCategory* category)
{
	if ( enabled_ && category )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategoryStatistic>(dbm_, decorated_->retrieveLinkedtoCategory(category->decorated_), *this);
	}

	return 0;
}


DBQuery<DBCategoryStatistic>* DBCategoryStatisticsRM::retrieveMostSelected(const std::string& language, int hour)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCategoryStatistic>(dbm_, decorated_->retrieveMostSelected(language, hour), *this);
	}
	else
		return 0;
}


DBCategoryStatistic* DBCategoryStatisticsRM::find(const DBCategory* category, const std::string& language, const std::string& hour) const
{
	if ( enabled_ && category)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCategoryStatisticI* rec = decorated_->find(category->decorated_, language, hour);

		if ( rec )
			return new DBCategoryStatistic(rec);
	}

	return 0;
}


DBCategoryStatistic* DBCategoryStatisticsRM::createRecord(const DBCategory* category) const
{
	if ( enabled_ && category )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBCategoryStatisticI* rec = decorated_->createRecord(category->decorated_);

		if ( rec )
		{
			DBCategoryStatistic* r = new DBCategoryStatistic(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBCategoryStatisticsRM::saveRecord(const DBCategoryStatistic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCategoryStatisticsRM::deleteRecord(const DBCategoryStatistic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCategoryStatisticsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBCategoryStatisticsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCategoryStatisticsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCategoryStatisticsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCategoryStatisticsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCategoryStatisticsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCategoryStatisticsRM::show()
{
	Debug::trace("database.show", 20, "DBCategoryStatisticsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCategoryStatisticsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCategoryStatistic>::show);
}


#ifndef EVOL_NDEBUG
void DBCategoryStatisticsRM::showAll(const std::string& dc)
#else
void DBCategoryStatisticsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCategoryStatisticsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCategoryStatisticsRM
		_registry_->process_mem(&DBCategoryStatisticsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCategoryStatisticsRM INSTANCES: None");
#endif
}


void DBCategoryStatisticsRM::disable()
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


bool DBCategoryStatisticsRM::registerQ(DBQuery<DBCategoryStatistic>* query)
{
	return qsRegistry_.add(query);
}


bool DBCategoryStatisticsRM::deregisterQ(DBQuery<DBCategoryStatistic>* query)
{
	return qsRegistry_.remove(query);
}


void DBCategoryStatisticsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCategoryStatistic>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCategoryStatisticsRM::DESCRIPTOR = "CATEGORYSTATISTIC";



bool DBCategoryStatisticsRM::registerRM(DBCategoryStatisticsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCategoryStatisticsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBCategoryStatisticsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCategoryStatisticsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCategoryStatisticsRM>* DBCategoryStatisticsRM::_registry_ = 0;
util_thread::Mutex<> DBCategoryStatisticsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
