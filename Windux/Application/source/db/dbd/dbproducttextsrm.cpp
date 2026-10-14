#include <string>


#include "edb.h"
#include DBPRODUCTTEXTSRM_INC
#include DBPRODUCTTEXT_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC

#include <Configuration.h>
#include <HobartCustomerHook.h>

#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
	using namespace util_string;
#endif



DBProductTextsRM::DBProductTextsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBProductTextsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d), type_(0)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBProductTextsRM::~DBProductTextsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBProductText>* DBProductTextsRM::retrieveAll(int type)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		int usedType = (type_ != 0) ? type_ : type;
		if (usedType != 0)
			return new DBQuery<DBProductText>(dbm_, decorated_->retrieveAll(usedType), *this);
	}

	return 0;
}


DBQuery<DBProductText>* DBProductTextsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProductText>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBProductText>* DBProductTextsRM::retrieveRangeNumber(const string& first, const string& last, const string& deptNum, int type)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		int usedType = (type_ != 0) ? type_ : type;
		if (usedType != 0)
			return new DBQuery<DBProductText>(dbm_, decorated_->retrieveRangeNumber(first, last, deptNum, usedType), *this);
	}

	return 0;
}


DBProductText* DBProductTextsRM::findNumber(const string& number, const string& deptNum, int type) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		int usedType = (type_ != 0) ? type_ : type;
		if (usedType != 0)
		{
			edb_adapter::DBProductTextI* rec = NULL;

#if defined( __linux__ )
		    if ( !Configuration::getInstance()->getCustomerHooksEnabled() || hobart_cust_hook_before_plu_text_read( number.c_str(), deptNum.c_str(), usedType ) )
				rec = decorated_->findNumber(number, deptNum, usedType);
#endif
			if ( rec )
				return new DBProductText(rec);
		}
	}

	return 0;
}


DBProductText* DBProductTextsRM::createRecord(int type) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		int usedType = (type_ != 0) ? type_ : type;
		if (usedType != 0)
		{
			edb_adapter::DBProductTextI* rec = decorated_->createRecord(usedType);

			if ( rec )
			{
				DBProductText* r = new DBProductText(rec);
				if ( r )
					return r;
				else
					delete rec;
			}	
		}
	}

	return 0;
}


void DBProductTextsRM::saveRecord(const DBProductText* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBProductTextsRM::deleteRecord(const DBProductText* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBProductTextsRM::deleteAll(int type) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int usedType = (type_ != 0) ? type_ : type;
		if (usedType != 0)
		{
			int result = decorated_->deleteAll(usedType);

			deregisterRM(*const_cast<DBProductTextsRM*>(this));
			disableAllRMs();
			registerRM(*const_cast<DBProductTextsRM*>(this));

			return result;
		}
	}

	return 0;
}


int DBProductTextsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBProductTextsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBProductTextsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBProductTextsRM::countAll(int type) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		int usedType = (type_ != 0) ? type_ : type;
		if (usedType != 0)
			return decorated_->countAll(usedType);
	}

	return 0;
}


int DBProductTextsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBProductTextsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBProductTextsRM::show()
{
	Debug::trace("database.show", 20, "DBProductTextsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBProductTextsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBProductText>::show);
}


#ifndef EVOL_NDEBUG
void DBProductTextsRM::showAll(const std::string& dc)
#else
void DBProductTextsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBProductTextsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBProductTextsRM
		_registry_->process_mem(&DBProductTextsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBProductTextsRM INSTANCES: None");
#endif
}


void DBProductTextsRM::disable()
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


bool DBProductTextsRM::registerQ(DBQuery<DBProductText>* query)
{
	return qsRegistry_.add(query);
}


bool DBProductTextsRM::deregisterQ(DBQuery<DBProductText>* query)
{
	return qsRegistry_.remove(query);
}


void DBProductTextsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBProductText>::disable);
	qsRegistry_.removeAll();
}



const std::string DBProductTextsRM::DESCRIPTOR = "PRODUCTTEXT";



bool DBProductTextsRM::registerRM(DBProductTextsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBProductTextsRM>;
	}

	return _registry_->add(&view);
	*/
}


void DBProductTextsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBProductTextsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBProductTextsRM>* DBProductTextsRM::_registry_ = 0;
util_thread::Mutex<> DBProductTextsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
