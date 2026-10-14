#include <string>


#include "sqlitedatabase.h"
#include "sqlitecoolcategorypretextsrm.h"
#include "sqlitecoolcategorypretext.h"
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
template <> int SqliteQuery<SqliteCoolCategoryPreText>::_dataType_ = 0;
template <> string SqliteQuery<SqliteCoolCategoryPreText>::_dataTypeName_("SqliteQuery<SqliteCoolCategoryPreText>");

int SqliteCoolCategoryPreTextsRM::_dataType_ = 0;
string SqliteCoolCategoryPreTextsRM::_dataTypeName_("SqliteCoolCategoryPreTextsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCoolCategoryPreTextsRM::DATA_FILENAME("CoolCategoryPreTexts");



SqliteCoolCategoryPreTextsRM::SqliteCoolCategoryPreTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolCategoryPreTextsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCoolCategoryPreText::PRETEXTNUMBER_FIELD);
	columnNames_.insert(SqliteCoolCategoryPreText::CATEGORYNUMBER_FIELD);
	columnNames_.insert(SqliteCoolCategoryPreText::DEPTNUM_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCoolCategoryPreText::PRETEXTNUMBER_FIELD,  SqliteField(SqliteCoolCategoryPreText::PRETEXTNUMBER_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolCategoryPreText::CATEGORYNUMBER_FIELD, SqliteField(SqliteCoolCategoryPreText::CATEGORYNUMBER_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolCategoryPreText::DEPTNUM_FIELD,        SqliteField(SqliteCoolCategoryPreText::DEPTNUM_FIELD,        SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCoolCategoryPreTextsRM::~SqliteCoolCategoryPreTextsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCoolCategoryPreTextI>* SqliteCoolCategoryPreTextsRM::retrieveAll()
{
	SqliteQuery<SqliteCoolCategoryPreText>* query = new SqliteQuery<SqliteCoolCategoryPreText>(*this);

	query->selectAll();

	return query;
}

DBQueryI<DBCoolCategoryPreTextI>* SqliteCoolCategoryPreTextsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolCategoryPreText>* query = new SqliteQuery<SqliteCoolCategoryPreText>(*this);

	string filter = SqliteCoolCategoryPreText::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteCoolCategoryPreText::DEPTNUM_FIELD);

	return query;
}

DBQueryI<DBCoolCategoryPreTextI>* SqliteCoolCategoryPreTextsRM::retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum)
{
	if ( category.size() )
	{
		SqliteQuery<SqliteCoolCategoryPreText>* query = new SqliteQuery<SqliteCoolCategoryPreText>(*this);

		string filter = SqliteCoolCategoryPreText::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCoolCategoryPreText::CATEGORYNUMBER_FIELD + " = " + category;
		query->selectSome(filter, SqliteCoolCategoryPreText::PRETEXTNUMBER_FIELD);

		return query;
	}

	return 0;
}


DBQueryI<DBCoolCategoryPreTextI>* SqliteCoolCategoryPreTextsRM::retrieveLinkedtoCoolPreText(const std::string& pretext, const std::string& deptNum)
{
	if ( pretext.size() )
	{
		SqliteQuery<SqliteCoolCategoryPreText>* query = new SqliteQuery<SqliteCoolCategoryPreText>(*this);

		string filter = SqliteCoolCategoryPreText::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCoolCategoryPreText::PRETEXTNUMBER_FIELD + " = " + pretext;
		query->selectSome(filter, SqliteCoolCategoryPreText::CATEGORYNUMBER_FIELD);

		return query;
	}

	return 0;
}


DBCoolCategoryPreTextI* SqliteCoolCategoryPreTextsRM::find(const std::string& category, const std::string& pretext, const std::string& deptNum)
{
	if ( category.size() && pretext.size() )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteCoolCategoryPreText::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
		keys.push_back(SqliteField(SqliteCoolCategoryPreText::PRETEXTNUMBER_FIELD, SqliteField::INTEGER, pretext));
		keys.push_back(SqliteField(SqliteCoolCategoryPreText::CATEGORYNUMBER_FIELD, SqliteField::INTEGER, category));
		
		SqliteRecord* rec = SqliteRecordManager::findKey(keys);
		if ( rec )
			return new SqliteCoolCategoryPreText(rec);
	}

	return 0;
}


DBCoolCategoryPreTextI* SqliteCoolCategoryPreTextsRM::createRecord(const std::string& category, const std::string& pretext, const std::string& deptNum)
{
	if ( category.size() && pretext.size() )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteCoolCategoryPreText* rec = new SqliteCoolCategoryPreText(dbrec);

			if ( rec )
			{
				rec->setPreTextNumber(pretext);
				rec->setDeptNumber(deptNum);
				rec->setCategoryNumber(category);
				return rec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqliteCoolCategoryPreTextsRM::saveRecord(const DBCoolCategoryPreTextI* rec)
{
	const SqliteCoolCategoryPreText* drec = dynamic_cast<const SqliteCoolCategoryPreText*>(rec);

	if ( drec )
	{
		if ( SqliteRecordManager::save(drec->getRecord()) )
			modified();
		else
			throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "Unable to save record.");
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolCategoryPreTextsRM::deleteRecord(const DBCoolCategoryPreTextI* rec)
{
	const SqliteCoolCategoryPreText* drec = dynamic_cast<const SqliteCoolCategoryPreText*>(rec);

	if ( drec )
	{
		int deleted = SqliteRecordManager::markDeleted(drec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolCategoryPreTextsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteCoolCategoryPreText::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
