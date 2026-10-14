#include <string>


#include "edb.h"
#include DBSTREAMINGGRAPHICSRM_INC
#include DBSTREAMINGGRAPHIC_INC
#include DBQUERY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBStreamingGraphicsRM::DBStreamingGraphicsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBStreamingGraphicsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBStreamingGraphicsRM::~DBStreamingGraphicsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBStreamingGraphic>* DBStreamingGraphicsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBStreamingGraphic>(dbm_, decorated_->retrieveAll(), *this);
	}
	else
		return 0;
}


DBQuery<DBStreamingGraphic>* DBStreamingGraphicsRM::retrieveRangeNumber(const string& first, const string& last)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBStreamingGraphic>(dbm_, decorated_->retrieveRangeNumber(first, last), *this);
	}
	else
		return 0;
}


DBStreamingGraphic* DBStreamingGraphicsRM::findNumber(const string& number) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBStreamingGraphicI* rec = decorated_->findNumber(number);

		if ( rec )
			return new DBStreamingGraphic(rec);
	}

	return 0;
}


DBStreamingGraphic* DBStreamingGraphicsRM::findNext(const DBStreamingGraphic* current) const
{
	if ( enabled_ && current )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBStreamingGraphicI* rec = decorated_->findNext(current->decorated_);

		if ( rec )
			return new DBStreamingGraphic(rec);
	}

	return 0;
}


DBStreamingGraphic* DBStreamingGraphicsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBStreamingGraphicI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBStreamingGraphic* r = new DBStreamingGraphic(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBStreamingGraphicsRM::saveRecord(const DBStreamingGraphic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBStreamingGraphicsRM::deleteRecord(const DBStreamingGraphic* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}
	else
		return 0;
}


int DBStreamingGraphicsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBStreamingGraphicsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBStreamingGraphicsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBStreamingGraphicsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}
	else
		return 0;
}


int DBStreamingGraphicsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}
	else
		return 0;
}


bool DBStreamingGraphicsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBStreamingGraphicsRM::show()
{
	Debug::trace("database.show", 20, "DBStreamingGraphicsRM instance address: " + 
		util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBStreamingGraphicsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBStreamingGraphic>::show);
}


#ifndef EVOL_NDEBUG
void DBStreamingGraphicsRM::showAll(const std::string& dc)
#else
void DBStreamingGraphicsRM::showAll(const std::string& UNUSED1(dc))
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
		Debug::trace(debuggingCategory, 20, "DBStreamingGraphicsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBStreamingGraphicsRM
		_registry_->process_mem(&DBStreamingGraphicsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBStreamingGraphicsRM INSTANCES: None");
#endif
}


void DBStreamingGraphicsRM::disable()
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


bool DBStreamingGraphicsRM::registerQ(DBQuery<DBStreamingGraphic>* query)
{
	return qsRegistry_.add(query);
}


bool DBStreamingGraphicsRM::deregisterQ(DBQuery<DBStreamingGraphic>* query)
{
	return qsRegistry_.remove(query);
}


void DBStreamingGraphicsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBStreamingGraphic>::disable);
	qsRegistry_.removeAll();
}



const std::string DBStreamingGraphicsRM::DESCRIPTOR = "STREAMINGGRAPHIC";



bool DBStreamingGraphicsRM::registerRM(DBStreamingGraphicsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

    /* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBStreamingGraphicsRM>;
	}

	return _registry_->add(&view);
	*/
}


void DBStreamingGraphicsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBStreamingGraphicsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBStreamingGraphicsRM>* DBStreamingGraphicsRM::_registry_ = 0;
util_thread::Mutex<> DBStreamingGraphicsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
