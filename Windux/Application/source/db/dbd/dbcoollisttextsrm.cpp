#include <string>


#include "edb.h"
#include DBCOOLLISTTEXTSRM_INC
#include DBCOOLLISTTEXT_INC
#include DBCOOLLIST_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolListTextsRM::DBCoolListTextsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolListTextsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCoolListTextsRM::~DBCoolListTextsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable category (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCoolListText>* DBCoolListTextsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolListText>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBCoolListText>* DBCoolListTextsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolListText>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBCoolListText>* DBCoolListTextsRM::retrieveLink(const std::string& link)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolListText>(dbm_, decorated_->retrieveLink(link), *this);
	}
	else
		return 0;
}


DBQuery<DBCoolListText>* DBCoolListTextsRM::retrieveLinkedtoCoolList(const DBCoolList* list)
{
	if ( enabled_ && list )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolListText>(dbm_, decorated_->retrieveLinkedtoCoolList(list->decorated_), *this);
	}
	else
		return 0;
}


DBQuery<DBCoolListText>* DBCoolListTextsRM::retrievePreviousCoolListTexts(const std::string& link)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolListText>(dbm_, decorated_->retrievePreviousCoolListTexts(link), *this);
	}
	else
		return 0;
}


DBCoolListText* DBCoolListTextsRM::find(const std::string& text, const string& deptNum, const DBCoolList* list)
{
	if ( enabled_ && list )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCoolListTextI* rec = decorated_->find(text, deptNum, list->decorated_);

		if ( rec )
			return new DBCoolListText(rec);
	}

	return 0;
}


DBCoolListText* DBCoolListTextsRM::createRecord(const std::string& text, const DBCoolList* list, HobRecordNum deptNum) const
{
	if ( enabled_ && list )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBCoolListTextI* rec = decorated_->createRecord(text, list->decorated_, deptNum);

		if ( rec )
			return new DBCoolListText(rec);
	}

	return 0;
}


DBCoolListText* DBCoolListTextsRM::createRecordInList(const std::string& text, const DBCoolListText* listText) const
{
	if ( enabled_ && listText)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBCoolListTextI* rec = decorated_->createRecordInList(text, listText->decorated_);

		if ( rec )
		{
			DBCoolListText* r = new DBCoolListText(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBCoolListTextsRM::saveRecord(const DBCoolListText* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCoolListTextsRM::deleteRecord(const DBCoolListText* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCoolListTextsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBCoolListTextsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolListTextsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolListTextsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBCoolListTextsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolListTextsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolListTextsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCoolListTextsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCoolListTextsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCoolListTextsRM::show()
{
	Debug::trace("database.show", 20, "DBCoolListTextsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCoolListTextsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolListText>::show);
}


#ifndef EVOL_NDEBUG
void DBCoolListTextsRM::showAll(const std::string& dc)
#else
void DBCoolListTextsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCoolListTextsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCoolListTextsRM
		_registry_->process_mem(&DBCoolListTextsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCoolListTextsRM INSTANCES: None");
#endif
}


void DBCoolListTextsRM::disable()
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


bool DBCoolListTextsRM::registerQ(DBQuery<DBCoolListText>* query)
{
	return qsRegistry_.add(query);
}


bool DBCoolListTextsRM::deregisterQ(DBQuery<DBCoolListText>* query)
{
	return qsRegistry_.remove(query);
}


void DBCoolListTextsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolListText>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCoolListTextsRM::DESCRIPTOR = "COOLLISTTEXT";



bool DBCoolListTextsRM::registerRM(DBCoolListTextsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCoolListTextsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBCoolListTextsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCoolListTextsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCoolListTextsRM>* DBCoolListTextsRM::_registry_ = 0;
util_thread::Mutex<> DBCoolListTextsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
