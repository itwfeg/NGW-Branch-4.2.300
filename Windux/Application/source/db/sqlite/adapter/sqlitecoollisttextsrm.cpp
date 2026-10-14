#include <string>


#include "sqlitedatabase.h"
#include "sqlitecoollisttextsrm.h"
#include "sqlitecoollisttext.h"
#include "sqlitecoollist.h"
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
template <> int SqliteQuery<SqliteCoolListText>::_dataType_ = 0;
template <> string SqliteQuery<SqliteCoolListText>::_dataTypeName_("SqliteQuery<SqliteCoolListText>");

int SqliteCoolListTextsRM::_dataType_ = 0;
string SqliteCoolListTextsRM::_dataTypeName_("SqliteCoolListTextsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCoolListTextsRM::DATA_FILENAME("CoolListTexts");



SqliteCoolListTextsRM::SqliteCoolListTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolListTextsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCoolListText::COOLLISTKEY_FIELD);
	columnNames_.insert(SqliteCoolListText::DEPTNUM_FIELD);
	columnNames_.insert(SqliteCoolListText::COOLTEXTNUMBER_FIELD);
	columnNames_.insert(SqliteCoolListText::ORDER_FIELD);
	columnNames_.insert(SqliteCoolListText::LINKNUMBER_FIELD);
	columnNames_.insert(SqliteCoolListText::NEXTLINKNUMBER_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCoolListText::COOLLISTKEY_FIELD,    SqliteField(SqliteCoolListText::COOLLISTKEY_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolListText::DEPTNUM_FIELD,        SqliteField(SqliteCoolListText::DEPTNUM_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolListText::COOLTEXTNUMBER_FIELD, SqliteField(SqliteCoolListText::COOLTEXTNUMBER_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolListText::ORDER_FIELD,          SqliteField(SqliteCoolListText::ORDER_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolListText::LINKNUMBER_FIELD,     SqliteField(SqliteCoolListText::LINKNUMBER_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolListText::NEXTLINKNUMBER_FIELD, SqliteField(SqliteCoolListText::NEXTLINKNUMBER_FIELD, SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCoolListTextsRM::~SqliteCoolListTextsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCoolListTextI>* SqliteCoolListTextsRM::retrieveAll()
{
	SqliteQuery<SqliteCoolListText>* query = new SqliteQuery<SqliteCoolListText>(*this);

	query->selectAll(SqliteCoolListText::COOLLISTKEY_FIELD + ", " + SqliteCoolListText::ORDER_FIELD);

	return query;
}


DBQueryI<DBCoolListTextI>* SqliteCoolListTextsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolListText>* query = new SqliteQuery<SqliteCoolListText>(*this);

	string filter = SqliteCoolListText::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteCoolListText::COOLLISTKEY_FIELD + ", " + SqliteCoolListText::ORDER_FIELD);

	return query;
}


DBQueryI<DBCoolListTextI>* SqliteCoolListTextsRM::retrieveLink(const std::string& link)
{
	if ( link.size() )
	{
		SqliteQuery<SqliteCoolListText>* query = new SqliteQuery<SqliteCoolListText>(*this);

		string filter = SqliteCoolListText::LINKNUMBER_FIELD + " = " + link;
		query->selectSome(filter, SqliteCoolListText::COOLLISTKEY_FIELD);

		return query;
	}

	return 0;
}


DBQueryI<DBCoolListTextI>* SqliteCoolListTextsRM::retrieveLinkedtoCoolList(const DBCoolListI* l)
{
	const SqliteCoolList* list = dynamic_cast<const SqliteCoolList*>(l);

	if ( list )
	{
		SqliteQuery<SqliteCoolListText>* query = new SqliteQuery<SqliteCoolListText>(*this);

		string filter = SqliteCoolListText::COOLLISTKEY_FIELD + " = " + list->getListKey();
		query->selectSome(filter, SqliteCoolListText::ORDER_FIELD);

		return query;
	}

	return 0;
}


DBQueryI<DBCoolListTextI>* SqliteCoolListTextsRM::retrievePreviousCoolListTexts(const std::string& link)
{
	if ( link.size() )
	{
		SqliteQuery<SqliteCoolListText>* query = new SqliteQuery<SqliteCoolListText>(*this);

		string filter = SqliteCoolListText::NEXTLINKNUMBER_FIELD +  " = " + link;
		query->selectSome(filter, SqliteCoolListText::COOLLISTKEY_FIELD);

		return query;
	}

	return 0;
}


DBCoolListTextI* SqliteCoolListTextsRM::find(const std::string& text, const std::string& deptNum, const DBCoolListI* list)
{
	const SqliteCoolList* inrec = dynamic_cast<const SqliteCoolList*>(list);

	if ( text.size() && inrec )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteCoolListText::COOLLISTKEY_FIELD, SqliteField::INTEGER, inrec->getListKey()));
		keys.push_back(SqliteField(SqliteCoolListText::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
		keys.push_back(SqliteField(SqliteCoolListText::COOLTEXTNUMBER_FIELD, SqliteField::INTEGER, text));
		
		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
		if ( findrec )
			return new SqliteCoolListText(findrec);
	}

	return 0;
}


DBCoolListTextI* SqliteCoolListTextsRM::createRecord(const std::string& text, const DBCoolListI* list, HobRecordNum deptNum)
{
	const SqliteCoolList* inrec = dynamic_cast<const SqliteCoolList*>(list);

	if ( text.size() && inrec )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteCoolListText* createrec = new SqliteCoolListText(dbrec);

			if ( createrec )
			{
				createrec->setCoolListKey(inrec->getListKey());
				createrec->setCoolTextNumber(text);
				createrec->setDeptNumber(longtostring(deptNum));
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


DBCoolListTextI* SqliteCoolListTextsRM::createRecordInList(const std::string& text, const DBCoolListTextI* listText)
{
	const SqliteCoolListText* inrec = dynamic_cast<const SqliteCoolListText*>(listText);

	if ( text.size() && inrec )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteCoolListText* createrec = new SqliteCoolListText(dbrec);

			if ( createrec )
			{
				createrec->setCoolListKey(inrec->getCoolListKey());
				createrec->setCoolTextNumber(text);
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqliteCoolListTextsRM::saveRecord(const DBCoolListTextI* rec)
{
	const SqliteCoolListText* inrec = dynamic_cast<const SqliteCoolListText*>(rec);

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


int SqliteCoolListTextsRM::deleteRecord(const DBCoolListTextI* rec)
{
	const SqliteCoolListText* inrec = dynamic_cast<const SqliteCoolListText*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolListTextsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteCoolListText::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
