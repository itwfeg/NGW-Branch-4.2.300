#include <string>


#include "sqlitedatabase.h"
#include "sqlitecoolcategoriesrm.h"
#include "sqlitecoolcategory.h"
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
template <> int SqliteQuery<SqliteCoolCategory>::_dataType_ = 0;
template <> string SqliteQuery<SqliteCoolCategory>::_dataTypeName_("SqliteQuery<SqliteCoolCategory>");

int SqliteCoolCategoriesRM::_dataType_ = 0;
string SqliteCoolCategoriesRM::_dataTypeName_("SqliteCoolCategoriesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCoolCategoriesRM::DATA_FILENAME("CoolCategories");



SqliteCoolCategoriesRM::SqliteCoolCategoriesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolCategoriesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCoolCategory::NUMBER_FIELD);
	columnNames_.insert(SqliteCoolCategory::DEPTNUM_FIELD);
	columnNames_.insert(SqliteCoolCategory::NAME1_FIELD);
	columnNames_.insert(SqliteCoolCategory::NAME2_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCoolCategory::NUMBER_FIELD, SqliteField(SqliteCoolCategory::NUMBER_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolCategory::DEPTNUM_FIELD, SqliteField(SqliteCoolCategory::DEPTNUM_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolCategory::NAME1_FIELD,  SqliteField(SqliteCoolCategory::NAME1_FIELD,  SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCoolCategory::NAME2_FIELD,  SqliteField(SqliteCoolCategory::NAME2_FIELD,  SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCoolCategoriesRM::~SqliteCoolCategoriesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCoolCategoryI>* SqliteCoolCategoriesRM::retrieveAll()
{
	SqliteQuery<SqliteCoolCategory>* query = new SqliteQuery<SqliteCoolCategory>(*this);

	query->selectAll();

	return query;
}

DBQueryI<DBCoolCategoryI>* SqliteCoolCategoriesRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolCategory>* query = new SqliteQuery<SqliteCoolCategory>(*this);

	string filter = SqliteCoolCategory::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteCoolCategory::NUMBER_FIELD);

	return query;
}


DBQueryI<DBCoolCategoryI>* SqliteCoolCategoriesRM::retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteCoolCategory>* query = new SqliteQuery<SqliteCoolCategory>(*this);

	string filter = SqliteCoolCategory::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCoolCategory::NUMBER_FIELD + " >= " + first + " and " + SqliteCoolCategory::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteCoolCategory::NUMBER_FIELD);

	return query;
}


DBCoolCategoryI* SqliteCoolCategoriesRM::findNumber(const std::string& number, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteCoolCategory::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteCoolCategory::NUMBER_FIELD, SqliteField::INTEGER, number));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	
	if ( findrec )
		return new SqliteCoolCategory(findrec);

	return 0;
}


DBCoolCategoryI* SqliteCoolCategoriesRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteCoolCategory* createrec = new SqliteCoolCategory(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteCoolCategoriesRM::saveRecord(const DBCoolCategoryI* rec)
{
	const SqliteCoolCategory* inrec = dynamic_cast<const SqliteCoolCategory*>(rec);

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


int SqliteCoolCategoriesRM::deleteRecord(const DBCoolCategoryI* rec)
{
	const SqliteCoolCategory* inrec = dynamic_cast<const SqliteCoolCategory*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolCategoriesRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteCoolCategory::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
