#include <string>


#include "sqlitedatabase.h"
#include "sqlitesoftkeygroupsrm.h"
#include "sqlitesoftkeygroup.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteSoftKeyGroup>::_dataType_ = 0;
template <> string SqliteQuery<SqliteSoftKeyGroup>::_dataTypeName_("SqliteQuery<SqliteSoftKeyGroup>");

int SqliteSoftKeyGroupsRM::_dataType_ = 0;
string SqliteSoftKeyGroupsRM::_dataTypeName_("SqliteSoftKeyGroupsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteSoftKeyGroupsRM::DATA_FILENAME("SoftKeyGroups");



SqliteSoftKeyGroupsRM::SqliteSoftKeyGroupsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSoftKeyGroupsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteSoftKeyGroup::NUMBER_FIELD);
	columnNames_.insert(SqliteSoftKeyGroup::DEPTNUM_FIELD);
	columnNames_.insert(SqliteSoftKeyGroup::HORIZONTALGRIDSIZE_FIELD);
	columnNames_.insert(SqliteSoftKeyGroup::VERTICALGRIDSIZE_FIELD);
	columnNames_.insert(SqliteSoftKeyGroup::HORIZONTALSPACING_FIELD);
	columnNames_.insert(SqliteSoftKeyGroup::VERTICALSPACING_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteSoftKeyGroup::NUMBER_FIELD,             SqliteField(SqliteSoftKeyGroup::NUMBER_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyGroup::DEPTNUM_FIELD,            SqliteField(SqliteSoftKeyGroup::DEPTNUM_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyGroup::HORIZONTALGRIDSIZE_FIELD, SqliteField(SqliteSoftKeyGroup::HORIZONTALGRIDSIZE_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyGroup::VERTICALGRIDSIZE_FIELD,   SqliteField(SqliteSoftKeyGroup::VERTICALGRIDSIZE_FIELD,   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyGroup::HORIZONTALSPACING_FIELD,  SqliteField(SqliteSoftKeyGroup::HORIZONTALSPACING_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyGroup::VERTICALSPACING_FIELD,    SqliteField(SqliteSoftKeyGroup::VERTICALSPACING_FIELD,    SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteSoftKeyGroupsRM::~SqliteSoftKeyGroupsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBSoftKeyGroupI>* SqliteSoftKeyGroupsRM::retrieveAll()
{
	SqliteQuery<SqliteSoftKeyGroup>* query = new SqliteQuery<SqliteSoftKeyGroup>(*this);

	query->selectAll();

	return query;
}

DBQueryI<DBSoftKeyGroupI>* SqliteSoftKeyGroupsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteSoftKeyGroup>* query = new SqliteQuery<SqliteSoftKeyGroup>(*this);

	string filter = SqliteSoftKeyGroup::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteSoftKeyGroup::NUMBER_FIELD);

	return query;
}


DBQueryI<DBSoftKeyGroupI>* SqliteSoftKeyGroupsRM::retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteSoftKeyGroup>* query = new SqliteQuery<SqliteSoftKeyGroup>(*this);

	string filter = SqliteSoftKeyGroup::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteSoftKeyGroup::NUMBER_FIELD + " >= " + first + " and " + SqliteSoftKeyGroup::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteSoftKeyGroup::NUMBER_FIELD);

	return query;
}


DBSoftKeyGroupI* SqliteSoftKeyGroupsRM::findNumber(const string& number, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteSoftKeyGroup::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteSoftKeyGroup::NUMBER_FIELD, SqliteField::INTEGER, number));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);

	if ( findrec )
		return new SqliteSoftKeyGroup(findrec);

	return 0;
}


DBSoftKeyGroupI* SqliteSoftKeyGroupsRM::createRecord(const std::string& number)
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteSoftKeyGroup* createrec = new SqliteSoftKeyGroup(dbrec);

		if ( createrec )
		{
			createrec->setNumber(number);
			return createrec;
		}
		else
			delete dbrec;
	}

	return 0;
}


void SqliteSoftKeyGroupsRM::saveRecord(const DBSoftKeyGroupI* rec)
{
	const SqliteSoftKeyGroup* inrec = dynamic_cast<const SqliteSoftKeyGroup*>(rec);

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


int SqliteSoftKeyGroupsRM::deleteRecord(const DBSoftKeyGroupI* rec)
{
	const SqliteSoftKeyGroup* inrec = dynamic_cast<const SqliteSoftKeyGroup*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteSoftKeyGroupsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteSoftKeyGroup::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
