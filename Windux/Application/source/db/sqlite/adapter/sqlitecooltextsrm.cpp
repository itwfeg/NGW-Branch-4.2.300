#include <string>


#include "sqlitedatabase.h"
#include "sqlitecooltextsrm.h"
#include "sqlitecooltext.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteCoolText>::_dataType_ = 0;
template <> string SqliteQuery<SqliteCoolText>::_dataTypeName_("SqliteQuery<SqliteCoolText>");

int SqliteCoolTextsRM::_dataType_ = 0;
string SqliteCoolTextsRM::_dataTypeName_("SqliteCoolTextsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCoolTextsRM::DATA_FILENAME("CoolTexts");



SqliteCoolTextsRM::SqliteCoolTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolTextsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCoolText::NUMBER_FIELD);
	columnNames_.insert(SqliteCoolText::DEPTNUM_FIELD);
	columnNames_.insert(SqliteCoolText::COOLTEXT_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCoolText::NUMBER_FIELD,   SqliteField(SqliteCoolText::NUMBER_FIELD,   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolText::DEPTNUM_FIELD,  SqliteField(SqliteCoolText::DEPTNUM_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolText::COOLTEXT_FIELD, SqliteField(SqliteCoolText::COOLTEXT_FIELD, SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCoolTextsRM::~SqliteCoolTextsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCoolTextI>* SqliteCoolTextsRM::retrieveAll()
{
	SqliteQuery<SqliteCoolText>* query = new SqliteQuery<SqliteCoolText>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBCoolTextI>* SqliteCoolTextsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolText>* query = new SqliteQuery<SqliteCoolText>(*this);

	string filter = SqliteCoolText::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteCoolText::NUMBER_FIELD);

	return query;
}


DBQueryI<DBCoolTextI>* SqliteCoolTextsRM::retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteCoolText>* query = new SqliteQuery<SqliteCoolText>(*this);

	string filter = SqliteCoolText::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCoolText::NUMBER_FIELD + " >= " + first + " and " + SqliteCoolText::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteCoolText::NUMBER_FIELD);

	return query;
}


DBCoolTextI* SqliteCoolTextsRM::findNumber(const string& number, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteCoolText::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteCoolText::NUMBER_FIELD, SqliteField::INTEGER, number));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteCoolText(findrec);

	return 0;
}


DBCoolTextI* SqliteCoolTextsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteCoolText* createrec = new SqliteCoolText(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteCoolTextsRM::saveRecord(const DBCoolTextI* rec)
{
	const SqliteCoolText* inrec = dynamic_cast<const SqliteCoolText*>(rec);

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


int SqliteCoolTextsRM::deleteRecord(const DBCoolTextI* rec)
{
	const SqliteCoolText* inrec = dynamic_cast<const SqliteCoolText*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolTextsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteCoolText::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
