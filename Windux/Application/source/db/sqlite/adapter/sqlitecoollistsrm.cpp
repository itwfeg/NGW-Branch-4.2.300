#include <string>


#include "sqlitedatabase.h"
#include "sqlitecoollistsrm.h"
#include "sqlitecoollist.h"
#include "sqlitecoollisttext.h"
#include "sqliteproduct.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteCoolList>::_dataType_ = 0;
template <> string SqliteQuery<SqliteCoolList>::_dataTypeName_("SqliteQuery<SqliteCoolList>");

int SqliteCoolListsRM::_dataType_ = 0;
string SqliteCoolListsRM::_dataTypeName_("SqliteCoolListsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCoolListsRM::DATA_FILENAME("CoolLists");



SqliteCoolListsRM::SqliteCoolListsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolListsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCoolList::LISTKEY_FIELD);
	columnNames_.insert(SqliteCoolList::NUMBER_FIELD);
	columnNames_.insert(SqliteCoolList::DEPTNUM_FIELD);
	columnNames_.insert(SqliteCoolList::FIRSTLINKNUMBER_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCoolList::LISTKEY_FIELD,         SqliteField(SqliteCoolList::LISTKEY_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolList::NUMBER_FIELD,          SqliteField(SqliteCoolList::NUMBER_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolList::DEPTNUM_FIELD,         SqliteField(SqliteCoolList::DEPTNUM_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolList::FIRSTLINKNUMBER_FIELD, SqliteField(SqliteCoolList::FIRSTLINKNUMBER_FIELD, SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCoolListsRM::~SqliteCoolListsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCoolListI>* SqliteCoolListsRM::retrieveAll()
{
	SqliteQuery<SqliteCoolList>* query = new SqliteQuery<SqliteCoolList>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBCoolListI>* SqliteCoolListsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolList>* query = new SqliteQuery<SqliteCoolList>(*this);

	string filter = SqliteCoolList::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteCoolList::NUMBER_FIELD);

	return query;
}


DBQueryI<DBCoolListI>* SqliteCoolListsRM::retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteCoolList>* query = new SqliteQuery<SqliteCoolList>(*this);

	string filter = SqliteCoolList::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCoolList::NUMBER_FIELD + " >= " + first + " and " + SqliteCoolList::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteCoolList::NUMBER_FIELD);

	return query;
}


DBCoolListI* SqliteCoolListsRM::findNumber(const string& number, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteCoolList::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteCoolList::NUMBER_FIELD, SqliteField::INTEGER, number));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteCoolList(findrec);

	return 0;
}


DBCoolListI* SqliteCoolListsRM::find(const DBProductI* prod)
{
	const SqliteProduct* inrec = dynamic_cast<const SqliteProduct*>(prod);

	if ( inrec )
	{
		SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteCoolList::LISTKEY_FIELD, SqliteField::INTEGER, inrec->getCoolListKey()));
		if ( findrec )
			return new SqliteCoolList(findrec);
	}

	return 0;
}


DBCoolListI* SqliteCoolListsRM::find(const DBCoolListTextI* text)
{
	const SqliteCoolListText* inrec = dynamic_cast<const SqliteCoolListText*>(text);

	if ( inrec )
	{
		SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteCoolList::LISTKEY_FIELD, SqliteField::INTEGER, inrec->getCoolListKey()));
		if ( findrec )
			return new SqliteCoolList(findrec);
	}

	return 0;
}


DBCoolListI* SqliteCoolListsRM::findFirstLink(const string& link)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteCoolList::FIRSTLINKNUMBER_FIELD, SqliteField::INTEGER, link));
	if ( findrec )
		return new SqliteCoolList(findrec);

	return 0;
}


DBCoolListI* SqliteCoolListsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteCoolList* createrec = new SqliteCoolList(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteCoolListsRM::saveRecord(const DBCoolListI* rec)
{
	const SqliteCoolList* inrec = dynamic_cast<const SqliteCoolList*>(rec);

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


int SqliteCoolListsRM::deleteRecord(const DBCoolListI* rec)
{
	const SqliteCoolList* inrec = dynamic_cast<const SqliteCoolList*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolListsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteCoolList::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
