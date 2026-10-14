#include <string>



#include "edb.h"
#include DBDEFAULTPRODUCTSRM_INC

#include DBDEFAULTPRODUCT_INC
#include DBPRESENTATIONMETHOD_INC
#include DBPRODUCT_INC
#include DBQUERY_INC


#include DBSTATUSRM_INC
#include UTILSTRING_INC
#include LOCK_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif


using namespace std;


DBDefaultProductsRM::DBDefaultProductsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBDefaultProductsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBDefaultProductsRM::~DBDefaultProductsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(dbm_, DBAccessTypes::DBAccessCloseTable, recordType_);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBDefaultProduct* DBDefaultProductsRM::find(const DBPresentationMethod* method,
		const DBProduct* prod, const std::string& lang) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(dbm_, DBAccessTypes::DBAccessFindRecord, recordType_);

		edb_adapter::DBDefaultProductI* dp = decorated_->find(method->decorated, prod->decorated, lang);

		if ( dp )
			return new DBDefaultProduct(dp);
	}

	return 0;
}


DBQuery<DBDefaultProduct>* DBDefaultProductsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(dbm_, DBAccessTypes::DBAccessRetrieveRecords, recordType_);

		return new DBQuery<DBDefaultProduct>(decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBDefaultProduct>* DBDefaultProductsRM::retrieveLinkedtoPresentationMethod(const DBPresentationMethod* method, const std::string& lang)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(dbm_, DBAccessTypes::DBAccessRetrieveRecords, recordType_);

		return new DBQuery<DBDefaultProduct>(decorated_->retrieveLinkedtoPresentationMethod(method->decorated, lang), *this);
	}
	else
		return 0;
}


DBDefaultProduct* DBDefaultProductsRM::createRecord(const DBPresentationMethod* method, const DBProduct* prod) const
{
	if ( enabled_ )
	{
		if ( method && prod )
		{
			DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(dbm_, DBAccessTypes::DBAccessCreateRecord, recordType_);

			edb_adapter::DBDefaultProductI* dp = decorated_->createRecord(method->decorated, prod->decorated);

			if ( dp )
				return new DBDefaultProduct(dp);
		}
	}

	return 0;
}


void DBDefaultProductsRM::saveRecord(const DBDefaultProduct* dp) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(dbm_, DBAccessTypes::DBAccessSaveRecord, recordType_);

		decorated_->saveRecord(dp->decorated);
	}
}


int DBDefaultProductsRM::deleteRecord(const DBDefaultProduct* dp) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(dbm_, DBAccessTypes::DBAccessDeleteRecord, recordType_);

		return decorated_->deleteRecord(dp->decorated);
	}
	else
		return 0;
}


int DBDefaultProductsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(dbm_, DBAccessTypes::DBAccessDeleteAllRecords, recordType_);

		return decorated_->deleteAll();
	}
	else
		return 0;
}


int DBDefaultProductsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(dbm_, DBAccessTypes::DBAccessCountAllRecords, recordType_);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBDefaultProductsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(dbm_, DBAccessTypes::DBAccessCalculateAverageSpace, recordType_);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBDefaultProductsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBDefaultProductsRM::show()
{
	Debug::trace("database.show", 20, "DBDefaultProductsRM instance address: " + 
			ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBDefaultProductsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBDefaultProduct>::show);
}


void DBDefaultProductsRM::showAll()
{
	Debug::trace("database.show", 20, "");
	Debug::trace("database.show", 20, "");
	if ( _registry_ && _registry_->size() )
	{
		Debug::trace("database.show", 20, "DBDefaultProductsRM INSTANCES: " + inttostring(_registry_->size()));

		// Show each of the currently registered DBDefaultProductsRM
		_registry_->process_mem(&DBDefaultProductsRM::show);
	}
	else
		Debug::trace("database.show", 20, "DBDefaultProductsRM INSTANCES: None");
}


void DBDefaultProductsRM::disable()
{
	// Mark this record manager as not enabled so that it cannot be used
	enabled_ = false;

	// Now disable all queries associated with this record manager
	disableAllQs();

	// Now dispose of the internal decorated element so we do not have problems
	// later on when deleting the record manager (the session currently deletes
	// the DBManager before it deletes some of the associated record managers).
	if ( decorated_ )
	{
		delete decorated_;
		decorated_ = 0;
	}
}


bool DBDefaultProductsRM::registerQ(DBQuery<DBDefaultProduct>* query)
{
	return qsRegistry_.add(query);
}


bool DBDefaultProductsRM::deregisterQ(DBQuery<DBDefaultProduct>* query)
{
	return qsRegistry_.remove(query);
}


void DBDefaultProductsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBDefaultProduct>::disable);
	qsRegistry_.removeAll();
}



const std::string DBDefaultProductsRM::DESCRIPTOR = "DEFAULTPRODUCT";



bool DBDefaultProductsRM::registerRM(DBDefaultProductsRM& view)
{
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBDefaultProductsRM>;
	}

	return _registry_->add(&view);
}


void DBDefaultProductsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBDefaultProductsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBDefaultProductsRM>* DBDefaultProductsRM::_registry_ = 0;
util_thread::Mutex<> DBDefaultProductsRM::_registryMutex_ = util_thread::Mutex<>();



#ifndef NO_NAMESPACES
} // namespace edb
#endif

