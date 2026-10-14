#include <string>


#include "sqlitedatabase.h"
#include "sqlitenutritionsrm.h"
#include "sqlitenutrition.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteNutrition>::_dataType_ = 0;
template <> string SqliteQuery<SqliteNutrition>::_dataTypeName_("SqliteQuery<SqliteNutrition>");

int SqliteNutritionsRM::_dataType_ = 0;
string SqliteNutritionsRM::_dataTypeName_("SqliteNutritionsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteNutritionsRM::DATA_FILENAME("NutritionFacts");



SqliteNutritionsRM::SqliteNutritionsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBNutritionsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteNutrition::NUMBER_FIELD);
	columnNames_.insert(SqliteNutrition::DEPTNUM_FIELD);
	columnNames_.insert(SqliteNutrition::NUTRITIONCOLUMN1_FIELD);
	columnNames_.insert(SqliteNutrition::NUTRITIONCOLUMN2_FIELD);
	columnNames_.insert(SqliteNutrition::NUTRITIONCOLUMN3_FIELD);
	columnNames_.insert(SqliteNutrition::NUTRITIONCOLUMN4_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteNutrition::NUMBER_FIELD,           SqliteField(SqliteNutrition::NUMBER_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteNutrition::DEPTNUM_FIELD,          SqliteField(SqliteNutrition::DEPTNUM_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteNutrition::NUTRITIONCOLUMN1_FIELD, SqliteField(SqliteNutrition::NUTRITIONCOLUMN1_FIELD, SqliteField::STRING)));
	columns_.insert(make_pair(SqliteNutrition::NUTRITIONCOLUMN2_FIELD, SqliteField(SqliteNutrition::NUTRITIONCOLUMN2_FIELD, SqliteField::STRING)));
	columns_.insert(make_pair(SqliteNutrition::NUTRITIONCOLUMN3_FIELD, SqliteField(SqliteNutrition::NUTRITIONCOLUMN3_FIELD, SqliteField::STRING)));
	columns_.insert(make_pair(SqliteNutrition::NUTRITIONCOLUMN4_FIELD, SqliteField(SqliteNutrition::NUTRITIONCOLUMN4_FIELD, SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteNutritionsRM::~SqliteNutritionsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBNutritionI>* SqliteNutritionsRM::retrieveAll()
{
	SqliteQuery<SqliteNutrition>* query = new SqliteQuery<SqliteNutrition>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBNutritionI>* SqliteNutritionsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteNutrition>* query = new SqliteQuery<SqliteNutrition>(*this);

	string filter = SqliteNutrition::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteNutrition::NUMBER_FIELD);

	return query;
}


DBQueryI<DBNutritionI>* SqliteNutritionsRM::retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteNutrition>* query = new SqliteQuery<SqliteNutrition>(*this);

	string filter = SqliteNutrition::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteNutrition::NUMBER_FIELD + " >= " + first + " and " + SqliteNutrition::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteNutrition::NUMBER_FIELD);

	return query;
}


DBNutritionI* SqliteNutritionsRM::findNumber(const std::string& number, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteNutrition::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteNutrition::NUMBER_FIELD, SqliteField::INTEGER, number));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteNutrition(findrec);

	return 0;
}


DBNutritionI* SqliteNutritionsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteNutrition* createrec = new SqliteNutrition(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteNutritionsRM::saveRecord(const DBNutritionI* rec)
{
	const SqliteNutrition* inrec = dynamic_cast<const SqliteNutrition*>(rec);

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


int SqliteNutritionsRM::deleteRecord(const DBNutritionI* rec)
{
	const SqliteNutrition* inrec = dynamic_cast<const SqliteNutrition*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteNutritionsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteNutrition::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
