#include <string>


#include "edb.h"
#include DBCOOLCATEGORYPRETEXTSRM_INC
#include DBCOOLCATEGORYPRETEXT_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCoolCategoryPreTextsRM::DBCoolCategoryPreTextsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolCategoryPreTextsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBCoolCategoryPreTextsRM::~DBCoolCategoryPreTextsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable category (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBCoolCategoryPreText>* DBCoolCategoryPreTextsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryPreText>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}

DBQuery<DBCoolCategoryPreText>* DBCoolCategoryPreTextsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryPreText>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}

DBQuery<DBCoolCategoryPreText>* DBCoolCategoryPreTextsRM::retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryPreText>(dbm_, decorated_->retrieveLinkedtoCoolCategory(category, deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBCoolCategoryPreText>* DBCoolCategoryPreTextsRM::retrieveLinkedtoCoolPreText(const std::string& pretext, const std::string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBCoolCategoryPreText>(dbm_, decorated_->retrieveLinkedtoCoolPreText(pretext, deptNum), *this);
	}
	else
		return 0;
}


DBCoolCategoryPreText* DBCoolCategoryPreTextsRM::find(const std::string& category, const std::string& pretext, const std::string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBCoolCategoryPreTextI* rec = decorated_->find(category, pretext, deptNum);

		if ( rec )
			return new DBCoolCategoryPreText(rec);
	}

	return 0;
}


DBCoolCategoryPreText* DBCoolCategoryPreTextsRM::createRecord(const std::string& category, const std::string& pretext, const std::string& deptNum) const
{
	if ( enabled_ )
	{
		if ( category.size() && pretext.size() )
		{
			DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

			edb_adapter::DBCoolCategoryPreTextI* rec = decorated_->createRecord(category, pretext, deptNum);

			if ( rec )
			{
				DBCoolCategoryPreText* r = new DBCoolCategoryPreText(rec);
				if ( r )
					return r;
				else
					delete rec;
			}
		}
	}

	return 0;
}


void DBCoolCategoryPreTextsRM::saveRecord(const DBCoolCategoryPreText* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBCoolCategoryPreTextsRM::deleteRecord(const DBCoolCategoryPreText* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBCoolCategoryPreTextsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBCoolCategoryPreTextsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolCategoryPreTextsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolCategoryPreTextsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBCoolCategoryPreTextsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBCoolCategoryPreTextsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBCoolCategoryPreTextsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBCoolCategoryPreTextsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBCoolCategoryPreTextsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBCoolCategoryPreTextsRM::show()
{
	Debug::trace("database.show", 20, "DBCoolCategoryPreTextsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBCoolCategoryPreTextsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolCategoryPreText>::show);
}


#ifndef EVOL_NDEBUG
void DBCoolCategoryPreTextsRM::showAll(const std::string& dc)
#else
void DBCoolCategoryPreTextsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBCoolCategoryPreTextsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBCoolCategoryPreTextsRM
		_registry_->process_mem(&DBCoolCategoryPreTextsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBCoolCategoryPreTextsRM INSTANCES: None");
#endif
}


void DBCoolCategoryPreTextsRM::disable()
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


bool DBCoolCategoryPreTextsRM::registerQ(DBQuery<DBCoolCategoryPreText>* query)
{
	return qsRegistry_.add(query);
}


bool DBCoolCategoryPreTextsRM::deregisterQ(DBQuery<DBCoolCategoryPreText>* query)
{
	return qsRegistry_.remove(query);
}


void DBCoolCategoryPreTextsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBCoolCategoryPreText>::disable);
	qsRegistry_.removeAll();
}



const std::string DBCoolCategoryPreTextsRM::DESCRIPTOR = "COOLCATEGORYPRETEXT";



bool DBCoolCategoryPreTextsRM::registerRM(DBCoolCategoryPreTextsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBCoolCategoryPreTextsRM>;
	}

	return _registry_->add(&view);
*/
}


void DBCoolCategoryPreTextsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBCoolCategoryPreTextsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBCoolCategoryPreTextsRM>* DBCoolCategoryPreTextsRM::_registry_ = 0;
util_thread::Mutex<> DBCoolCategoryPreTextsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
