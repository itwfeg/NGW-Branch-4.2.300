#include <string>


#include "edb.h"
#include DBFLASHFILESRM_INC
#include DBFLASHFILE_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBFlashFilesRM::DBFlashFilesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBFlashFilesRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBFlashFilesRM::~DBFlashFilesRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBFlashFile>* DBFlashFilesRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBFlashFile>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBFlashFile* DBFlashFilesRM::findName(const string& name) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBFlashFileI* rec = decorated_->findName(name);

		if ( rec )
			return new DBFlashFile(rec);
	}

	return 0;
}


DBFlashFile* DBFlashFilesRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBFlashFileI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBFlashFile* r = new DBFlashFile(rec);
			if ( r )
				return r;
			else
				delete rec;
		}	
	}

	return 0;
}


void DBFlashFilesRM::saveRecord(const DBFlashFile* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBFlashFilesRM::deleteRecord(const DBFlashFile* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBFlashFilesRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBFlashFilesRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBFlashFilesRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBFlashFilesRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBFlashFilesRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBFlashFilesRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBFlashFilesRM::show()
{
	Debug::trace("database.show", 20, "DBFlashFilesRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBFlashFilesRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBFlashFile>::show);
}


#ifndef EVOL_NDEBUG
void DBFlashFilesRM::showAll(const std::string& dc)
#else
void DBFlashFilesRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBFlashFilesRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBFlashFilesRM
		_registry_->process_mem(&DBFlashFilesRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBFlashFilesRM INSTANCES: None");
#endif
}


void DBFlashFilesRM::disable()
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


bool DBFlashFilesRM::registerQ(DBQuery<DBFlashFile>* query)
{
	return qsRegistry_.add(query);
}


bool DBFlashFilesRM::deregisterQ(DBQuery<DBFlashFile>* query)
{
	return qsRegistry_.remove(query);
}


void DBFlashFilesRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBFlashFile>::disable);
	qsRegistry_.removeAll();
}



const std::string DBFlashFilesRM::DESCRIPTOR = "FLASHFILE";



bool DBFlashFilesRM::registerRM(DBFlashFilesRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBFlashFilesRM>;
	}

	return _registry_->add(&view);
*/
}


void DBFlashFilesRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBFlashFilesRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBFlashFilesRM>* DBFlashFilesRM::_registry_ = 0;
util_thread::Mutex<> DBFlashFilesRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
