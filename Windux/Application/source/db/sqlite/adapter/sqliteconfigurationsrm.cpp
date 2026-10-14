#include <string>


#include "sqlitedatabase.h"
#include "sqliteconfigurationsrm.h"
#include "sqliteconfiguration.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template<> int SqliteQuery<SqliteConfiguration>::_dataType_ = 0;
template<> string SqliteQuery<SqliteConfiguration>::_dataTypeName_("SqliteQuery<SqliteConfiguration>");

int SqliteConfigurationsRM::_dataType_ = 0;
string SqliteConfigurationsRM::_dataTypeName_("SqliteConfigurationsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteConfigurationsRM::DATA_FILENAME("Configurations");



SqliteConfigurationsRM::SqliteConfigurationsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBConfigurationsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteConfiguration::NAME_FIELD);
	columnNames_.insert(SqliteConfiguration::VALUE_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteConfiguration::NAME_FIELD,  SqliteField(SqliteConfiguration::NAME_FIELD,  SqliteField::STRING)));
	columns_.insert(make_pair(SqliteConfiguration::VALUE_FIELD, SqliteField(SqliteConfiguration::VALUE_FIELD, SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteConfigurationsRM::~SqliteConfigurationsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBConfigurationI>* SqliteConfigurationsRM::retrieveAll()
{
	SqliteQuery<SqliteConfiguration>* query = new SqliteQuery<SqliteConfiguration>(*this);

	query->selectAll();

	return query;
}


DBConfigurationI* SqliteConfigurationsRM::findName(const string& nameToFind)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteConfiguration::NAME_FIELD, SqliteField::STRING, nameToFind));
	if ( findrec )
		return new SqliteConfiguration(findrec);

	return 0;
}


DBConfigurationI* SqliteConfigurationsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteConfiguration* createrec = new SqliteConfiguration(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteConfigurationsRM::saveRecord(const DBConfigurationI* rec)
{
	const SqliteConfiguration* inrec = dynamic_cast<const SqliteConfiguration*>(rec);

	if ( inrec )
	{
		if ( SqliteRecordManager::save(inrec->getRecord()) )
			modified();
		else
			throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "Unable to save Configuration record.");
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteConfigurationsRM::deleteRecord(const DBConfigurationI* rec)
{
	const SqliteConfiguration* inrec = dynamic_cast<const SqliteConfiguration*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif





#ifdef SQLITECONFIGURATIONSRM_DRIVER

#include <iostream>

int main(int argc, char * argv[])
{
	unsigned long int tot;
	SqliteConfigurationsRM prods;

	tot = prods.countAll();
	if ( tot > 0 )
	{
		SqliteConfigurationsRM::iterator it = prods.begin();
		SqliteConfigurationsRM::iterator end = prods.end();

		while ( it != end )
			cout << *it;
	}
}

#endif // SQLITECONFIGURATIONSRM_DRIVER
