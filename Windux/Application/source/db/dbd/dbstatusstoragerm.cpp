#include <string>


#include "edb.h"
#include DBSTATUSSTORAGERM_INC
#include DBSTATUSSTORAGE_INC
#include DBEXCEPTION_INC
#include DBQUERYI_INC

#include UTILSTRING_INC
#include LOCK_INC
#include DEBUG_INC


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
	using namespace util_string;
	using namespace util_thread;
#endif



class DBStatusQuery : public edb_adapter::DBQueryI<edb_adapter::DBStatusI>
{
	edb_adapter::DBStatusI* first() { return DBStatusStorageRM::first(); }
	edb_adapter::DBStatusI* next(const edb_adapter::DBStatusI* current)
			{ return DBStatusStorageRM::next(current); }
	edb_adapter::DBStatusI* previous(const edb_adapter::DBStatusI* current)
			{ return DBStatusStorageRM::previous(current); }
	edb_adapter::DBStatusI* last() { return DBStatusStorageRM::last(); }
	edb_adapter::DBStatusI* find(const edb_adapter::DBStatusI* current) 
			{ return DBStatusStorageRM::find(current); }

	size_type size() { return DBStatusStorageRM::size(); }
	bool empty() { return false; }

	void show() {}
};


DBStatusStorageRM::DBStatusStorageRM()
{
}


DBStatusStorageRM::~DBStatusStorageRM()
{
}


edb_adapter::DBQueryI<edb_adapter::DBStatusI>* DBStatusStorageRM::retrieveAll()
{
	return new DBStatusQuery();
}


edb_adapter::DBStatusI* DBStatusStorageRM::findName(const string& name)
{
	return new DBStatusStorage(name, getStatus(name));
}


edb_adapter::DBStatusI* DBStatusStorageRM::createRecord()
{
	return new DBStatusStorage();
}


void DBStatusStorageRM::saveRecord(const edb_adapter::DBStatusI* current)
{
	const DBStatusStorage* rec = dynamic_cast<const DBStatusStorage*>(current);

	if ( rec )
		setStatus(rec->getName(), rec->getValue());
	else
		throw edb::DBException(edb::DBException::DB_LAYERING_ERROR, "Current record is not a DBStatusStorage record.");
}


int DBStatusStorageRM::countAll()
{
	return (int)size();
}


bool DBStatusStorageRM::execute(const std::string sql)
{
	return false;
}


DBStatusStorageRM::statusContainerType DBStatusStorageRM::_statusContainer_ = statusContainerType();
util_thread::Mutex<> DBStatusStorageRM::_statusContainerMutex_ = util_thread::Mutex<>();


edb_adapter::DBStatusI* DBStatusStorageRM::first()
{
	Lock< Mutex<> > lock(_statusContainerMutex_); 
	statusContainerType::const_iterator iter = _statusContainer_.begin();

	if ( iter != _statusContainer_.end() )
		return new DBStatusStorage(iter->first, new string(iter->second));
	else
		return 0;
}


edb_adapter::DBStatusI* DBStatusStorageRM::next(const edb_adapter::DBStatusI* current)
{
	const DBStatusStorage* status = dynamic_cast<const DBStatusStorage*>(current);

	if ( status )
	{
		Lock< Mutex<> > lock(_statusContainerMutex_); 
		statusContainerType::const_iterator iter = _statusContainer_.find(status->getName());

		if ( iter != _statusContainer_.end() )
		{
			++iter;
			if ( iter != _statusContainer_.end() )
				return new DBStatusStorage(iter->first, new string(iter->second));
		}

		return 0;
	}
	else
		throw edb::DBException(edb::DBException::DB_LAYERING_ERROR, "Current record is not a DBStatusStorage record.");
}


edb_adapter::DBStatusI* DBStatusStorageRM::previous(const edb_adapter::DBStatusI* current)
{
	const DBStatusStorage* status = dynamic_cast<const DBStatusStorage*>(current);

	if ( status )
	{
		Lock< Mutex<> > lock(_statusContainerMutex_); 
		statusContainerType::const_iterator iter = _statusContainer_.find(status->getName());

		if ( iter != _statusContainer_.begin() )
		{
			--iter;

			return new DBStatusStorage(iter->first, new string(iter->second));
		}

		return 0;
	}
	else
		throw edb::DBException(edb::DBException::DB_LAYERING_ERROR, "Current record is not a DBStatusStorage record.");
}


edb_adapter::DBStatusI* DBStatusStorageRM::last()
{
	Lock< Mutex<> > lock(_statusContainerMutex_); 
	statusContainerType::const_iterator iter = _statusContainer_.end();

	--iter;
	if ( iter != _statusContainer_.end() )
		return new DBStatusStorage(iter->first, new string(iter->second));
	else
		return 0;
}


edb_adapter::DBStatusI* DBStatusStorageRM::find(const edb_adapter::DBStatusI* current)
{
	const DBStatusStorage* status = dynamic_cast<const DBStatusStorage*>(current);

	if ( status )
	{
		Lock< Mutex<> > lock(_statusContainerMutex_); 
		statusContainerType::const_iterator iter = _statusContainer_.find(status->getName());

		if ( iter != _statusContainer_.end() )
			return new DBStatusStorage(iter->first, new string(iter->second));

		return 0;
	}
	else
		throw edb::DBException(edb::DBException::DB_LAYERING_ERROR, "Current record is not a DBStatusStorage record.");
}


DBStatusStorageRM::size_type DBStatusStorageRM::size()
{
	Lock< Mutex<> > lock(_statusContainerMutex_); 

	return _statusContainer_.size();
}



string* DBStatusStorageRM::getStatus(const std::string& field)
{
	bool found = false;
	string* result = new string();

	{
		Lock< Mutex<> > lock(_statusContainerMutex_);

		statusContainerType::const_iterator iter = _statusContainer_.find(field);
		if ( iter != _statusContainer_.end() )
		{
			result->assign(iter->second);
			found = true;
		}
	}

	if ( !found )
	{
		delete result;
		result = 0;
	}

	return result;
}


void DBStatusStorageRM::setStatus(const std::string& field, const std::string& value)
{
	Lock< Mutex<> > lock(_statusContainerMutex_); 

	_statusContainer_[field] = value;

	Debug::trace("database.status", 24, "Set status element \'" + field + "\' to the value \'" + value + "\'");
}


void DBStatusStorageRM::setStatusNull(const std::string& field)
{
	Lock< Mutex<> > lock(_statusContainerMutex_); 

	_statusContainer_.erase(field);

	Debug::trace("database.status", 24, "Removed status element \'" + field + "\' (i.e., set it to null)");
}


void DBStatusStorageRM::copyStatus(const std::string& destField, const std::string& srcField)
{
	Lock< Mutex<> > lock(_statusContainerMutex_); 

	_statusContainer_[destField] = _statusContainer_[srcField];

	Debug::trace("database.status", 24, "Copied status from element '" + srcField + "' to element '" + destField + "'");
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
