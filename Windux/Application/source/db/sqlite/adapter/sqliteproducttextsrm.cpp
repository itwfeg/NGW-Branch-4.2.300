#include <string>


#include "sqlitedatabase.h"
#include "sqliteproducttextsrm.h"
#include "sqliteproducttext.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteProductText>::_dataType_ = 0;
template <> string SqliteQuery<SqliteProductText>::_dataTypeName_("SqliteQuery<SqliteProductText>");

int SqliteProductTextsRM::_dataType_ = 0;
string SqliteProductTextsRM::_dataTypeName_("SqliteProductTextsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteProductTextsRM::DATA_FILENAME("ProductTexts");



SqliteProductTextsRM::SqliteProductTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductTextsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteProductText::TYPE_FIELD);
	columnNames_.insert(SqliteProductText::NUMBER_FIELD);
	columnNames_.insert(SqliteProductText::DEPTNUM_FIELD);
	columnNames_.insert(SqliteProductText::TEXT_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteProductText::TYPE_FIELD,   SqliteField(SqliteProductText::TYPE_FIELD,   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductText::NUMBER_FIELD, SqliteField(SqliteProductText::NUMBER_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductText::DEPTNUM_FIELD, SqliteField(SqliteProductText::DEPTNUM_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductText::TEXT_FIELD,   SqliteField(SqliteProductText::TEXT_FIELD,   SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteProductTextsRM::~SqliteProductTextsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBProductTextI>* SqliteProductTextsRM::retrieveAll(int type)
{
	SqliteQuery<SqliteProductText>* query = new SqliteQuery<SqliteProductText>(*this);

	string filter = SqliteProductText::TYPE_FIELD + " = " + inttostring(type);
	query->selectSome(filter, SqliteProductText::NUMBER_FIELD);

	return query;
}


DBQueryI<DBProductTextI>* SqliteProductTextsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteProductText>* query = new SqliteQuery<SqliteProductText>(*this);

	string filter = SqliteProductText::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteProductText::NUMBER_FIELD);

	return query;
}


DBQueryI<DBProductTextI>* SqliteProductTextsRM::retrieveRangeNumber(const string& first, const string& last, const string& deptNum, int type)
{
	SqliteQuery<SqliteProductText>* query = new SqliteQuery<SqliteProductText>(*this);

	string filter = SqliteProductText::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteProductText::TYPE_FIELD + " = " + inttostring(type) + " and " + SqliteProductText::NUMBER_FIELD + " >= " + first + " and " + SqliteProductText::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteProductText::NUMBER_FIELD);

	return query;
}


DBProductTextI* SqliteProductTextsRM::findNumber(const string& number, const string& deptNum, int type)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteProductText::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteProductText::TYPE_FIELD, SqliteField::INTEGER, type));
	keys.push_back(SqliteField(SqliteProductText::NUMBER_FIELD, SqliteField::INTEGER, stringtoint(number)));
		
	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteProductText(findrec);

	return 0;
}


DBProductTextI* SqliteProductTextsRM::createRecord(int type)
{
	if ( type )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteProductText* createrec = new SqliteProductText(dbrec);

			if ( createrec )
			{
				createrec->setType(inttostring(type));
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqliteProductTextsRM::saveRecord(const DBProductTextI* rec)
{
	const SqliteProductText* inrec = dynamic_cast<const SqliteProductText*>(rec);

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


int SqliteProductTextsRM::deleteRecord(const DBProductTextI* rec)
{
	const SqliteProductText* inrec = dynamic_cast<const SqliteProductText*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteProductTextsRM::deleteAll(int type)
{
	std::string whereClause = "where " + SqliteProductText::TYPE_FIELD + " = " + inttostring(type); 
	int deleted = SqliteRecordManager::deleteAll(whereClause);
	return deleted;
}


int SqliteProductTextsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteProductText::DEPTNUM_FIELD + " = " + deptNum);
}


int SqliteProductTextsRM::countAll(int type)
{
	std::string whereClause = "where " + SqliteProductText::TYPE_FIELD + " = " + inttostring(type); 
	int count = SqliteRecordManager::countAll(whereClause);
	return count;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
