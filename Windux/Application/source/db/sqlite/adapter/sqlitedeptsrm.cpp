#include <string>


#include "sqlitedatabase.h"
#include "sqlitedeptsrm.h"
#include "sqlitedept.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteDept>::_dataType_ = 0;
template <> string SqliteQuery<SqliteDept>::_dataTypeName_("SqliteQuery<SqliteDept>");

int SqliteDeptsRM::_dataType_ = 0;
string SqliteDeptsRM::_dataTypeName_("SqliteDeptsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteDeptsRM::DATA_FILENAME("Depts");



SqliteDeptsRM::SqliteDeptsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBDeptsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteDept::NUMBER_FIELD);
	columnNames_.insert(SqliteDept::NAME1_FIELD);
	columnNames_.insert(SqliteDept::NAME2_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteDept::NUMBER_FIELD,       SqliteField(SqliteDept::NUMBER_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteDept::NAME1_FIELD,        SqliteField(SqliteDept::NAME1_FIELD,        SqliteField::STRING)));
	columns_.insert(make_pair(SqliteDept::NAME2_FIELD,        SqliteField(SqliteDept::NAME2_FIELD,        SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteDeptsRM::~SqliteDeptsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBDeptI>* SqliteDeptsRM::retrieveAll()
{
	SqliteQuery<SqliteDept>* query = new SqliteQuery<SqliteDept>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBDeptI>* SqliteDeptsRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteDept>* query = new SqliteQuery<SqliteDept>(*this);

	string filter = SqliteDept::NUMBER_FIELD + " >= " + first + " and " + SqliteDept::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteDept::NUMBER_FIELD);

	return query;
}


DBDeptI* SqliteDeptsRM::findNumber(const string& number)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteDept::NUMBER_FIELD, SqliteField::INTEGER, number));
	if ( findrec )
		return new SqliteDept(findrec);

	return 0;
}


DBDeptI* SqliteDeptsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteDept* createrec = new SqliteDept(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteDeptsRM::saveRecord(const DBDeptI* rec)
{
	const SqliteDept* inrec = dynamic_cast<const SqliteDept*>(rec);

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


int SqliteDeptsRM::deleteRecord(const DBDeptI* rec)
{
	const SqliteDept* inrec = dynamic_cast<const SqliteDept*>(rec);

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
