#include <string>


#include "sqlitedatabase.h"
#include "sqlitecoolpretextsrm.h"
#include "sqlitecoolpretext.h"
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
template <> int SqliteQuery<SqliteCoolPreText>::_dataType_ = 0;
template <> string SqliteQuery<SqliteCoolPreText>::_dataTypeName_("SqliteQuery<SqliteCoolPreText>");

int SqliteCoolPreTextsRM::_dataType_ = 0;
string SqliteCoolPreTextsRM::_dataTypeName_("SqliteCoolPreTextsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCoolPreTextsRM::DATA_FILENAME("CoolPreTexts");



SqliteCoolPreTextsRM::SqliteCoolPreTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolPreTextsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCoolPreText::NUMBER_FIELD);
	columnNames_.insert(SqliteCoolPreText::NAME1_FIELD);
	columnNames_.insert(SqliteCoolPreText::NAME2_FIELD);
	columnNames_.insert(SqliteCoolPreText::ALLCATEGORIES_FIELD);
	columnNames_.insert(SqliteCoolPreText::DEPTNUM_FIELD);

	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCoolPreText::NUMBER_FIELD,        SqliteField(SqliteCoolPreText::NUMBER_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolPreText::NAME1_FIELD,         SqliteField(SqliteCoolPreText::NAME1_FIELD,         SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCoolPreText::NAME2_FIELD,         SqliteField(SqliteCoolPreText::NAME2_FIELD,         SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCoolPreText::ALLCATEGORIES_FIELD, SqliteField(SqliteCoolPreText::ALLCATEGORIES_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolPreText::DEPTNUM_FIELD,       SqliteField(SqliteCoolPreText::DEPTNUM_FIELD,        SqliteField::INTEGER)));

	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCoolPreTextsRM::~SqliteCoolPreTextsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCoolPreTextI>* SqliteCoolPreTextsRM::retrieveAll()
{
	SqliteQuery<SqliteCoolPreText>* query = new SqliteQuery<SqliteCoolPreText>(*this);

	query->selectAll();

	return query;
}

DBQueryI<DBCoolPreTextI>* SqliteCoolPreTextsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolPreText>* query = new SqliteQuery<SqliteCoolPreText>(*this);

	string filter = SqliteCoolPreText::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteCoolPreText::NUMBER_FIELD);

	return query;
}

DBQueryI<DBCoolPreTextI>* SqliteCoolPreTextsRM::retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteCoolPreText>* query = new SqliteQuery<SqliteCoolPreText>(*this);

	string filter = SqliteCoolPreText::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCoolPreText::NUMBER_FIELD + " >= " + first + " and " + SqliteCoolPreText::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteCoolPreText::NUMBER_FIELD);

	return query;
}


DBQueryI<DBCoolPreTextI>* SqliteCoolPreTextsRM::retrieveAllCategories(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolPreText>* query = new SqliteQuery<SqliteCoolPreText>(*this);

	string filter = SqliteCoolPreText::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCoolPreText::ALLCATEGORIES_FIELD + " = 1";
	query->selectSome(filter, SqliteCoolPreText::NAME1_FIELD);

	return query;
}


DBCoolPreTextI* SqliteCoolPreTextsRM::findNumber(const string& number, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteCoolPreText::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteCoolPreText::NUMBER_FIELD, SqliteField::INTEGER, number));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteCoolPreText(findrec);

	return 0;
}


DBCoolPreTextI* SqliteCoolPreTextsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteCoolPreText* createrec = new SqliteCoolPreText(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteCoolPreTextsRM::saveRecord(const DBCoolPreTextI* rec)
{
	const SqliteCoolPreText* inrec = dynamic_cast<const SqliteCoolPreText*>(rec);

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


int SqliteCoolPreTextsRM::deleteRecord(const DBCoolPreTextI* rec)
{
	const SqliteCoolPreText* inrec = dynamic_cast<const SqliteCoolPreText*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolPreTextsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteCoolPreText::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
