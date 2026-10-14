#include <string>


#include "sqlitedatabase.h"
#include "sqlitecoolcountriesrm.h"
#include "sqlitecoolcountry.h"
#include "sqlitecoolcategorycountry.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteCoolCountry>::_dataType_ = 0;
template <> string SqliteQuery<SqliteCoolCountry>::_dataTypeName_("SqliteQuery<SqliteCoolCountry>");

int SqliteCoolCountriesRM::_dataType_ = 0;
string SqliteCoolCountriesRM::_dataTypeName_("SqliteCoolCountriesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCoolCountriesRM::DATA_FILENAME("CoolCountries");



SqliteCoolCountriesRM::SqliteCoolCountriesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolCountriesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCoolCountry::COUNTRYKEY_FIELD);
	columnNames_.insert(SqliteCoolCountry::DEPTNUM_FIELD);
	columnNames_.insert(SqliteCoolCountry::NAME1_FIELD);
	columnNames_.insert(SqliteCoolCountry::NAME2_FIELD);
	columnNames_.insert(SqliteCoolCountry::ALLCATEGORIES_FIELD);
	columnNames_.insert(SqliteCoolCountry::COUNTRYNUM_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCoolCountry::COUNTRYKEY_FIELD,    SqliteField(SqliteCoolCountry::COUNTRYKEY_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolCountry::DEPTNUM_FIELD,       SqliteField(SqliteCoolCountry::DEPTNUM_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolCountry::NAME1_FIELD,         SqliteField(SqliteCoolCountry::NAME1_FIELD,         SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCoolCountry::NAME2_FIELD,         SqliteField(SqliteCoolCountry::NAME2_FIELD,         SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCoolCountry::ALLCATEGORIES_FIELD, SqliteField(SqliteCoolCountry::ALLCATEGORIES_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolCountry::COUNTRYNUM_FIELD,    SqliteField(SqliteCoolCountry::COUNTRYNUM_FIELD,    SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCoolCountriesRM::~SqliteCoolCountriesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCoolCountryI>* SqliteCoolCountriesRM::retrieveAll()
{
	SqliteQuery<SqliteCoolCountry>* query = new SqliteQuery<SqliteCoolCountry>(*this);

	query->selectAll();

	return query;
}

DBQueryI<DBCoolCountryI>* SqliteCoolCountriesRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolCountry>* query = new SqliteQuery<SqliteCoolCountry>(*this);

	string filter = SqliteCoolCountry::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteCoolCountry::DEPTNUM_FIELD);

	return query;
}

DBQueryI<DBCoolCountryI>* SqliteCoolCountriesRM::retrieveAllCategories(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolCountry>* query = new SqliteQuery<SqliteCoolCountry>(*this);

	string filter = SqliteCoolCountry::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCoolCountry::ALLCATEGORIES_FIELD + " = 1";
	query->selectSome(filter, SqliteCoolCountry::NAME1_FIELD);

	return query;
}


DBCoolCountryI* SqliteCoolCountriesRM::findCountry(const string& name, const string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteCoolCountry::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteCoolCountry::NAME1_FIELD, SqliteField::STRING, name));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	
	if ( findrec )
		return new SqliteCoolCountry(findrec);

	return 0;
}


DBCoolCountryI* SqliteCoolCountriesRM::find(const DBCoolCategoryCountryI* country)
{
	const SqliteCoolCategoryCountry* inrec = dynamic_cast<const SqliteCoolCategoryCountry*>(country);

	if ( inrec )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteCoolCountry::DEPTNUM_FIELD, SqliteField::INTEGER, inrec->getDeptNumber()));
		keys.push_back(SqliteField(SqliteCoolCountry::COUNTRYKEY_FIELD, SqliteField::INTEGER, inrec->getCountryKey()));

		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
		if ( findrec )
			return new SqliteCoolCountry(findrec);
	}

	return 0;
}


DBCoolCountryI* SqliteCoolCountriesRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteCoolCountry* createrec = new SqliteCoolCountry(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteCoolCountriesRM::saveRecord(const DBCoolCountryI* rec)
{
	const SqliteCoolCountry* inrec = dynamic_cast<const SqliteCoolCountry*>(rec);

	if ( inrec )
	{
		if ( SqliteRecordManager::save(inrec->getRecord()) )
			modified();
		else
			throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "Unable to save record.");
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolCountriesRM::deleteRecord(const DBCoolCountryI* rec)
{
	const SqliteCoolCountry* inrec = dynamic_cast<const SqliteCoolCountry*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolCountriesRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteCoolCountry::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
