#include <string>


#include "sqlitedatabase.h"
#include "sqliteclassesrm.h"
#include "sqliteclass.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template<> int SqliteQuery<SqliteClass>::_dataType_ = 0;
template<> string SqliteQuery<SqliteClass>::_dataTypeName_("SqliteQuery<SqliteClass>");

int SqliteClassesRM::_dataType_ = 0;
string SqliteClassesRM::_dataTypeName_("SqliteClassesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteClassesRM::DATA_FILENAME("Classes");



SqliteClassesRM::SqliteClassesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBClassesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteClass::NUMBER_FIELD);
	columnNames_.insert(SqliteClass::DEPTNUM_FIELD);
	columnNames_.insert(SqliteClass::REPORTINGCLASS_FIELD);
	columnNames_.insert(SqliteClass::DESCRIPTION1_FIELD);
	//columnNames_.insert(SqliteClass::DESCRIPTION2_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteClass::NUMBER_FIELD,         SqliteField(SqliteClass::NUMBER_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteClass::DEPTNUM_FIELD,        SqliteField(SqliteClass::DEPTNUM_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteClass::REPORTINGCLASS_FIELD, SqliteField(SqliteClass::REPORTINGCLASS_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteClass::DESCRIPTION1_FIELD,   SqliteField(SqliteClass::DESCRIPTION1_FIELD,   SqliteField::STRING)));
	//columns_.insert(make_pair(SqliteClass::DESCRIPTION2_FIELD,   SqliteField(SqliteClass::DESCRIPTION2_FIELD,   SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteClassesRM::~SqliteClassesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBClassI>* SqliteClassesRM::retrieveAll()
{
	SqliteQuery<SqliteClass>* query = new SqliteQuery<SqliteClass>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBClassI>* SqliteClassesRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteClass>* query = new SqliteQuery<SqliteClass>(*this);

	string filter = SqliteClass::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteClass::NUMBER_FIELD);

	return query;
}


DBQueryI<DBClassI>* SqliteClassesRM::retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteClass>* query = new SqliteQuery<SqliteClass>(*this);

	string filter = SqliteClass::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteClass::NUMBER_FIELD + " >= " + first + " and " + SqliteClass::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteClass::NUMBER_FIELD);

	return query;
}


DBQueryI<DBClassI>* SqliteClassesRM::retrieveChildClasses(const std::string& number, const std::string& deptNum)
{
	SqliteQuery<SqliteClass>* query = new SqliteQuery<SqliteClass>(*this);

	string filter = SqliteClass::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteClass::REPORTINGCLASS_FIELD + " = " + number;
	query->selectSome(filter, SqliteClass::NUMBER_FIELD);

	return query;
}


DBClassI* SqliteClassesRM::findNumber(const std::string& number, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteClass::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteClass::NUMBER_FIELD, SqliteField::INTEGER, number));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if (findrec)
		return new SqliteClass(findrec);

	return 0;
}


DBClassI* SqliteClassesRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteClass* createrec = new SqliteClass(dbrec);
		
		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteClassesRM::saveRecord(const DBClassI* rec)
{
	const SqliteClass* inrec = dynamic_cast<const SqliteClass*>(rec);

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


int SqliteClassesRM::deleteRecord(const DBClassI* rec)
{
	const SqliteClass* inrec = dynamic_cast<const SqliteClass*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteClassesRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteClass::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
