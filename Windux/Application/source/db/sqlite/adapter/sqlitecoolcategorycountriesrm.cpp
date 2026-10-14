#include <string>


#include "sqlitedatabase.h"
#include "sqlitecoolcategorycountriesrm.h"
#include "sqlitecoolcategorycountry.h"
#include "sqlitecoolcountry.h"
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
template <> int SqliteQuery<SqliteCoolCategoryCountry>::_dataType_ = 0;
template <> string SqliteQuery<SqliteCoolCategoryCountry>::_dataTypeName_("SqliteQuery<SqliteCoolCategoryCountry>");

int SqliteCoolCategoryCountriesRM::_dataType_ = 0;
string SqliteCoolCategoryCountriesRM::_dataTypeName_("SqliteCoolCategoryCountriesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCoolCategoryCountriesRM::DATA_FILENAME("CoolCategoryCountries");



SqliteCoolCategoryCountriesRM::SqliteCoolCategoryCountriesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolCategoryCountriesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCoolCategoryCountry::COUNTRYKEY_FIELD);
	columnNames_.insert(SqliteCoolCategoryCountry::DEPTNUM_FIELD);
	columnNames_.insert(SqliteCoolCategoryCountry::CATEGORYNUMBER_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCoolCategoryCountry::COUNTRYKEY_FIELD,     SqliteField(SqliteCoolCategoryCountry::COUNTRYKEY_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolCategoryCountry::DEPTNUM_FIELD,        SqliteField(SqliteCoolCategoryCountry::DEPTNUM_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolCategoryCountry::CATEGORYNUMBER_FIELD, SqliteField(SqliteCoolCategoryCountry::CATEGORYNUMBER_FIELD, SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCoolCategoryCountriesRM::~SqliteCoolCategoryCountriesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCoolCategoryCountryI>* SqliteCoolCategoryCountriesRM::retrieveAll()
{
	SqliteQuery<SqliteCoolCategoryCountry>* query = new SqliteQuery<SqliteCoolCategoryCountry>(*this);

	query->selectAll();

	return query;
}

DBQueryI<DBCoolCategoryCountryI>* SqliteCoolCategoryCountriesRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolCategoryCountry>* query = new SqliteQuery<SqliteCoolCategoryCountry>(*this);

	string filter = SqliteCoolCategoryCountry::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteCoolCategoryCountry::DEPTNUM_FIELD);

	return query;
}

DBQueryI<DBCoolCategoryCountryI>* SqliteCoolCategoryCountriesRM::retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum)
{
	if ( category.size() )
	{
		SqliteQuery<SqliteCoolCategoryCountry>* query = new SqliteQuery<SqliteCoolCategoryCountry>(*this);

		string filter = SqliteCoolCategoryCountry::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCoolCategoryCountry::CATEGORYNUMBER_FIELD + " = " + category;
		query->selectSome(filter);

		return query;
	}

	return 0;
}


DBQueryI<DBCoolCategoryCountryI>* SqliteCoolCategoryCountriesRM::retrieveLinkedtoCoolCountry(const DBCoolCountryI* country)
{
	const SqliteCoolCountry* inrec = dynamic_cast<const SqliteCoolCountry*>(country);

	if ( inrec )
	{
		SqliteQuery<SqliteCoolCategoryCountry>* query = new SqliteQuery<SqliteCoolCategoryCountry>(*this);

		string filter = SqliteCoolCategoryCountry::DEPTNUM_FIELD + " = " + inrec->getDeptNumber() + " and " + SqliteCoolCategoryCountry::COUNTRYKEY_FIELD + " = " + inrec->getCountryKey();
		query->selectSome(filter);

		return query;
	}

	return 0;
}


DBCoolCategoryCountryI* SqliteCoolCategoryCountriesRM::find(const std::string& category, const DBCoolCountryI* country)
{
	const SqliteCoolCountry* inrec = dynamic_cast<const SqliteCoolCountry*>(country);

	if ( category.size() && inrec )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteCoolCategoryCountry::DEPTNUM_FIELD, SqliteField::INTEGER, inrec->getDeptNumber()));
		keys.push_back(SqliteField(SqliteCoolCategoryCountry::COUNTRYKEY_FIELD, SqliteField::INTEGER, inrec->getCountryKey()));
		keys.push_back(SqliteField(SqliteCoolCategoryCountry::CATEGORYNUMBER_FIELD, SqliteField::INTEGER, category));
		
		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
		if ( findrec )
			return new SqliteCoolCategoryCountry(findrec);
	}

	return 0;
}


DBCoolCategoryCountryI* SqliteCoolCategoryCountriesRM::createRecord(const std::string& category, const DBCoolCountryI* country)
{
	const SqliteCoolCountry* inrec = dynamic_cast<const SqliteCoolCountry*>(country);

	if ( category.size() && inrec )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteCoolCategoryCountry* createrec = new SqliteCoolCategoryCountry(dbrec);

			if ( createrec )
			{
				createrec->setCountryKey(inrec->getCountryKey());
				createrec->setDeptNumber(inrec->getDeptNumber());
				createrec->setCategoryNumber(category);
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqliteCoolCategoryCountriesRM::saveRecord(const DBCoolCategoryCountryI* rec)
{
	const SqliteCoolCategoryCountry* inrec = dynamic_cast<const SqliteCoolCategoryCountry*>(rec);

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


int SqliteCoolCategoryCountriesRM::deleteRecord(const DBCoolCategoryCountryI* rec)
{
	const SqliteCoolCategoryCountry* inrec = dynamic_cast<const SqliteCoolCategoryCountry*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolCategoryCountriesRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteCoolCategoryCountry::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
