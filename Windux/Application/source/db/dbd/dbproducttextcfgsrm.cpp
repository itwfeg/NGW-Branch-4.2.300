#include <string>


#include "edb.h"
#include DBPRODUCTTEXTCFGSRM_INC
#include DBPRODUCTTEXTCFG_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
	using namespace util_string;
#endif



DBProductTextCfgsRM::DBProductTextCfgsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBProductTextCfgsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d), type_(0)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBProductTextCfgsRM::~DBProductTextCfgsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBProductTextCfg>* DBProductTextCfgsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProductTextCfg>(dbm_, decorated_->retrieveAll(), *this);
	}

	return 0;
}


DBProductTextCfg* DBProductTextCfgsRM::findNumber(int type) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBProductTextCfgI* rec = decorated_->findNumber(type);

		if ( rec )
			return new DBProductTextCfg(rec);
	}

	return 0;
}


DBProductTextCfg* DBProductTextCfgsRM::createRecord(int type) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		int usedType = (type_ != 0) ? type_ : type;
		if (usedType != 0)
		{
			edb_adapter::DBProductTextCfgI* rec = decorated_->createRecord(usedType);

			if ( rec )
			{
				DBProductTextCfg* r = new DBProductTextCfg(rec);
				if ( r )
					return r;
				else
					delete rec;
			}	
		}
	}

	return 0;
}


void DBProductTextCfgsRM::saveRecord(const DBProductTextCfg* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBProductTextCfgsRM::deleteRecord(const DBProductTextCfg* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBProductTextCfgsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBProductTextCfgsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBProductTextCfgsRM*>(this));

		return result;
	}

	return 0;
}


int DBProductTextCfgsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}

	return 0;
}


int DBProductTextCfgsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBProductTextCfgsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBProductTextCfgsRM::show()
{
	Debug::trace("database.show", 20, "DBProductTextCfgsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBProductTextCfgsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBProductTextCfg>::show);
}


#ifndef EVOL_NDEBUG
void DBProductTextCfgsRM::showAll(const std::string& dc)
#else
void DBProductTextCfgsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBProductTextCfgsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBProductTextCfgsRM
		_registry_->process_mem(&DBProductTextCfgsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBProductTextCfgsRM INSTANCES: None");
#endif
}


void DBProductTextCfgsRM::disable()
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


bool DBProductTextCfgsRM::registerQ(DBQuery<DBProductTextCfg>* query)
{
	return qsRegistry_.add(query);
}


bool DBProductTextCfgsRM::deregisterQ(DBQuery<DBProductTextCfg>* query)
{
	return qsRegistry_.remove(query);
}


void DBProductTextCfgsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBProductTextCfg>::disable);
	qsRegistry_.removeAll();
}



const std::string DBProductTextCfgsRM::DESCRIPTOR = "PRODUCTTEXTCFG";



bool DBProductTextCfgsRM::registerRM(DBProductTextCfgsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBProductTextCfgsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBProductTextCfgsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBProductTextCfgsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBProductTextCfgsRM>* DBProductTextCfgsRM::_registry_ = 0;
util_thread::Mutex<> DBProductTextCfgsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
