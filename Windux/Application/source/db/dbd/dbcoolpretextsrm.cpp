#include <string>


#include "edb.h"
#include DBCOOLPRETEXTSRM_INC
#include DBCOOLPRETEXT_INC
#include DBCOOLCATEGORY_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolPreTextsRM::DBCoolPreTextsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolPreTextsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCoolPreTextsRM::~DBCoolPreTextsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCoolPreText>* DBCoolPreTextsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolPreText>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}

DBQuery<DBCoolPreText>* DBCoolPreTextsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolPreText>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBCoolPreText>* DBCoolPreTextsRM::retrieveRangeNumber(const string& first, const string& last, const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolPreText>(dbm_, decorated_->retrieveRangeNumber(first, last,deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBCoolPreText>* DBCoolPreTextsRM::retrieveAllCategories(const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolPreText>(dbm_, decorated_->retrieveAllCategories(deptNum), *this);
	}
	else
		return 0;
}


DBCoolPreText* DBCoolPreTextsRM::findNumber(const string& number, const string& deptNum) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCoolPreTextI* rec = decorated_->findNumber(number, deptNum);

		if ( rec )
			return new DBCoolPreText(rec);
	}

	return 0;
}


DBCoolPreText* DBCoolPreTextsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBCoolPreTextI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBCoolPreText* r = new DBCoolPreText(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBCoolPreTextsRM::saveRecord(const DBCoolPreText* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCoolPreTextsRM::deleteRecord(const DBCoolPreText* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCoolPreTextsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBCoolPreTextsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolPreTextsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolPreTextsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBCoolPreTextsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolPreTextsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolPreTextsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCoolPreTextsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCoolPreTextsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCoolPreTextsRM::show()
{
	Debug::trace("database.show", 20, "DBCoolPreTextsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCoolPreTextsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolPreText>::show);
}


#ifndef EVOL_NDEBUG
void DBCoolPreTextsRM::showAll(const std::string& dc)
#else
void DBCoolPreTextsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCoolPreTextsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCoolPreTextsRM
		_registry_->process_mem(&DBCoolPreTextsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCoolPreTextsRM INSTANCES: None");
#endif
}


void DBCoolPreTextsRM::disable()
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


bool DBCoolPreTextsRM::registerQ(DBQuery<DBCoolPreText>* query)
{
	return qsRegistry_.add(query);
}


bool DBCoolPreTextsRM::deregisterQ(DBQuery<DBCoolPreText>* query)
{
	return qsRegistry_.remove(query);
}


void DBCoolPreTextsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolPreText>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCoolPreTextsRM::DESCRIPTOR = "COOLPRETEXT";



bool DBCoolPreTextsRM::registerRM(DBCoolPreTextsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCoolPreTextsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBCoolPreTextsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCoolPreTextsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCoolPreTextsRM>* DBCoolPreTextsRM::_registry_ = 0;
util_thread::Mutex<> DBCoolPreTextsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
