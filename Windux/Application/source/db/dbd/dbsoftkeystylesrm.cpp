#include <string>


#include "edb.h"
#include DBSOFTKEYSTYLESRM_INC
#include DBSOFTKEYSTYLE_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSoftKeyStylesRM::DBSoftKeyStylesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBSoftKeyStylesRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBSoftKeyStylesRM::~DBSoftKeyStylesRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBSoftKeyStyle>* DBSoftKeyStylesRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKeyStyle>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBSoftKeyStyle>* DBSoftKeyStylesRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBSoftKeyStyle>(dbm_, decorated_->retrieveRangeNumber(first, last), *this);
	}
	else
		return 0;
}


DBSoftKeyStyle* DBSoftKeyStylesRM::findNumber(const string& number) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBSoftKeyStyleI* rec = decorated_->findNumber(number);

		if ( rec )
			return new DBSoftKeyStyle(rec);
	}

	return 0;
}


DBSoftKeyStyle* DBSoftKeyStylesRM::createRecord(const std::string& number) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBSoftKeyStyleI* rec = decorated_->createRecord(number);

		if ( rec )
		{
			DBSoftKeyStyle* r = new DBSoftKeyStyle(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBSoftKeyStylesRM::saveRecord(const DBSoftKeyStyle* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBSoftKeyStylesRM::deleteRecord(const DBSoftKeyStyle* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBSoftKeyStylesRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBSoftKeyStylesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBSoftKeyStylesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBSoftKeyStylesRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBSoftKeyStylesRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBSoftKeyStylesRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBSoftKeyStylesRM::show()
{
	Debug::trace("database.show", 20, "DBSoftKeyStylesRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBSoftKeyStylesRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBSoftKeyStyle>::show);
}


#ifndef EVOL_NDEBUG
void DBSoftKeyStylesRM::showAll(const std::string& dc)
#else
void DBSoftKeyStylesRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBSoftKeyStylesRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBSoftKeyStylesRM
		_registry_->process_mem(&DBSoftKeyStylesRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBSoftKeyStylesRM INSTANCES: None");
#endif
}


void DBSoftKeyStylesRM::disable()
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


bool DBSoftKeyStylesRM::registerQ(DBQuery<DBSoftKeyStyle>* query)
{
	return qsRegistry_.add(query);
}


bool DBSoftKeyStylesRM::deregisterQ(DBQuery<DBSoftKeyStyle>* query)
{
	return qsRegistry_.remove(query);
}


void DBSoftKeyStylesRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBSoftKeyStyle>::disable);
	qsRegistry_.removeAll();
}



const std::string DBSoftKeyStylesRM::DESCRIPTOR = "SOFTKEYSTYLE";



bool DBSoftKeyStylesRM::registerRM(DBSoftKeyStylesRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBSoftKeyStylesRM>;
	}

	return _registry_->add(&view);
*/
}


void DBSoftKeyStylesRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBSoftKeyStylesRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBSoftKeyStylesRM>* DBSoftKeyStylesRM::_registry_ = 0;
util_thread::Mutex<> DBSoftKeyStylesRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
