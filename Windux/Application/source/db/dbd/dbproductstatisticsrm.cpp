#include <string>


#include "edb.h"
#include DBPRODUCTSTATISTICSRM_INC
#include DBPRODUCTSTATISTIC_INC
#include DBPRODUCT_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBProductStatisticsRM::DBProductStatisticsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBProductStatisticsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBProductStatisticsRM::~DBProductStatisticsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBProductStatistic>* DBProductStatisticsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProductStatistic>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBProductStatistic>* DBProductStatisticsRM::retrieveLinkedtoProduct(const DBProduct* prod)
{
	if ( enabled_ && prod )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProductStatistic>(dbm_, decorated_->retrieveLinkedtoProduct(prod->decorated_), *this);
	}
	else
		return 0;
}


DBQuery<DBProductStatistic>* DBProductStatisticsRM::retrieveMostSelected(const std::string& language, int hour)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProductStatistic>(dbm_, decorated_->retrieveMostSelected(language, hour), *this);
	}
	else
		return 0;
}


DBQuery<DBProductStatistic>* DBProductStatisticsRM::retrieveMostPrinted(const std::string& language, int hour)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProductStatistic>(dbm_, decorated_->retrieveMostPrinted(language, hour), *this);
	}
	else
		return 0;
}


DBQuery<DBProductStatistic>* DBProductStatisticsRM::retrieveDateTimeOrder()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProductStatistic>(dbm_, decorated_->retrieveDateTimeOrder(), *this);
	}
	else
		return 0;
}


DBProductStatistic* DBProductStatisticsRM::find(const DBProduct* prod, const std::string& language, const std::string& hour) const
{
	if ( enabled_ && prod )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBProductStatisticI* rec = decorated_->find(prod->decorated_, language, hour);

		if ( rec )
			return new DBProductStatistic(rec);
	}

	return 0;
}


DBProductStatistic* DBProductStatisticsRM::createRecord(const DBProduct* prod) const
{
	if ( enabled_ && prod )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBProductStatisticI* rec = decorated_->createRecord(prod->decorated_);

		if ( rec )
		{
			DBProductStatistic* r = new DBProductStatistic(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBProductStatisticsRM::saveRecord(const DBProductStatistic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBProductStatisticsRM::deleteRecord(const DBProductStatistic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBProductStatisticsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBProductStatisticsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBProductStatisticsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBProductStatisticsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBProductStatisticsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBProductStatisticsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBProductStatisticsRM::show()
{
	Debug::trace("database.show", 20, "DBProductStatisticsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBProductStatisticsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBProductStatistic>::show);
}


#ifndef EVOL_NDEBUG
void DBProductStatisticsRM::showAll(const std::string& dc)
#else
void DBProductStatisticsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBProductStatisticsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBProductStatisticsRM
		_registry_->process_mem(&DBProductStatisticsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBProductStatisticsRM INSTANCES: None");
#endif
}


void DBProductStatisticsRM::disable()
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


bool DBProductStatisticsRM::registerQ(DBQuery<DBProductStatistic>* query)
{
	return qsRegistry_.add(query);
}


bool DBProductStatisticsRM::deregisterQ(DBQuery<DBProductStatistic>* query)
{
	return qsRegistry_.remove(query);
}


void DBProductStatisticsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBProductStatistic>::disable);
	qsRegistry_.removeAll();
}



const std::string DBProductStatisticsRM::DESCRIPTOR = "PRODUCTSTATISTIC";



bool DBProductStatisticsRM::registerRM(DBProductStatisticsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBProductStatisticsRM>;
	}
	return _registry_->add(&view);
	*/
}


void DBProductStatisticsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBProductStatisticsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBProductStatisticsRM>* DBProductStatisticsRM::_registry_ = 0;
util_thread::Mutex<> DBProductStatisticsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
