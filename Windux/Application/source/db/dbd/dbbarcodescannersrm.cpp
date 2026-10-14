#include <string>


#include "edb.h"

#include DBBARCODESCANNERSRM_INC
#include DBBARCODESCANNER_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
	using namespace util_string;
#endif



DBBarcodeScannersRM::DBBarcodeScannersRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBBarcodeScannersRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d), type_(0)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBBarcodeScannersRM::~DBBarcodeScannersRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBBarcodeScanner>* DBBarcodeScannersRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBBarcodeScanner>(dbm_, decorated_->retrieveAll(), *this);
	}

	return 0;
}

DBBarcodeScanner* DBBarcodeScannersRM::findNumber(const string& code) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBBarcodeScannerI* rec = decorated_->findNumber(code);

		if ( rec )
			return new DBBarcodeScanner(rec);
	}

	return 0;
}


DBBarcodeScanner* DBBarcodeScannersRM::createRecord(const std::string& code) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBBarcodeScannerI* rec = decorated_->createRecord(code);

		if ( rec )
		{
			DBBarcodeScanner* r = new DBBarcodeScanner(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBBarcodeScannersRM::saveRecord(const DBBarcodeScanner* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBBarcodeScannersRM::deleteRecord(const DBBarcodeScanner* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBBarcodeScannersRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBBarcodeScannersRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBBarcodeScannersRM*>(this));

		return result;
	}

	return 0;
}


int DBBarcodeScannersRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}

	return 0;
}


int DBBarcodeScannersRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBBarcodeScannersRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBBarcodeScannersRM::show()
{
	Debug::trace("database.show", 20, "DBBarcodeScannersRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBBarcodeScannersRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBBarcodeScanner>::show);
}


#ifndef EVOL_NDEBUG
void DBBarcodeScannersRM::showAll(const std::string& dc)
#else
void DBBarcodeScannersRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBBarcodeScannersRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBBarcodeScannersRM
		_registry_->process_mem(&DBBarcodeScannersRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBBarcodeScannersRM INSTANCES: None");
#endif
}


void DBBarcodeScannersRM::disable()
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


bool DBBarcodeScannersRM::registerQ(DBQuery<DBBarcodeScanner>* query)
{
	return qsRegistry_.add(query);
}


bool DBBarcodeScannersRM::deregisterQ(DBQuery<DBBarcodeScanner>* query)
{
	return qsRegistry_.remove(query);
}


void DBBarcodeScannersRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBBarcodeScanner>::disable);
	qsRegistry_.removeAll();
}



const std::string DBBarcodeScannersRM::DESCRIPTOR = "HfcScannerProductRead";



bool DBBarcodeScannersRM::registerRM(DBBarcodeScannersRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBBarcodeScannersRM>;
	}

	return _registry_->add(&view);
*/
}


void DBBarcodeScannersRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBBarcodeScannersRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBBarcodeScannersRM>* DBBarcodeScannersRM::_registry_ = 0;
util_thread::Mutex<> DBBarcodeScannersRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
