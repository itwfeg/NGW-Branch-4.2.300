#include <string>


#include "sqlitedatabase.h"
#include "sqlitecategoriesrm.h"
#include "sqlitecategory.h"
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
template<> int SqliteQuery<SqliteCategory>::_dataType_ = 0;
template<> string SqliteQuery<SqliteCategory>::_dataTypeName_("SqliteQuery<SqliteCategory>");

int SqliteCategoriesRM::_dataType_ = 0;
string SqliteCategoriesRM::_dataTypeName_("SqliteCategoriesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCategoriesRM::DATA_FILENAME("Categories");



SqliteCategoriesRM::SqliteCategoriesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCategoriesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d), localeTransformedLength_(0)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	retrieveProperties();

	columnNames_.insert(SqliteCategory::NUMBER_FIELD);
	columnNames_.insert(SqliteCategory::DEPTNUM_FIELD);
	columnNames_.insert(SqliteCategory::PARENTCATEGORY_FIELD);
	columnNames_.insert(SqliteCategory::NAME1_FIELD);
	columnNames_.insert(SqliteCategory::NAME2_FIELD);
	columnNames_.insert(SqliteCategory::CODEDNAME1_FIELD);
	columnNames_.insert(SqliteCategory::CODEDNAME2_FIELD);
	columnNames_.insert(SqliteCategory::BACKGROUNDCOLOR_FIELD);
	columnNames_.insert(SqliteCategory::TEXTCOLOR_FIELD);
	columnNames_.insert(SqliteCategory::IMAGE_FIELD);
	columnNames_.insert(SqliteCategory::PLAYLIST_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCategory::NUMBER_FIELD,          SqliteField(SqliteCategory::NUMBER_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCategory::DEPTNUM_FIELD,         SqliteField(SqliteCategory::DEPTNUM_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCategory::PARENTCATEGORY_FIELD,  SqliteField(SqliteCategory::PARENTCATEGORY_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCategory::NAME1_FIELD,           SqliteField(SqliteCategory::NAME1_FIELD,           SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCategory::NAME2_FIELD,           SqliteField(SqliteCategory::NAME2_FIELD,           SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCategory::CODEDNAME1_FIELD,      SqliteField(SqliteCategory::CODEDNAME1_FIELD,      SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCategory::CODEDNAME2_FIELD,      SqliteField(SqliteCategory::CODEDNAME2_FIELD,      SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCategory::BACKGROUNDCOLOR_FIELD, SqliteField(SqliteCategory::BACKGROUNDCOLOR_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCategory::TEXTCOLOR_FIELD,       SqliteField(SqliteCategory::TEXTCOLOR_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCategory::IMAGE_FIELD,           SqliteField(SqliteCategory::IMAGE_FIELD,           SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCategory::PLAYLIST_FIELD,        SqliteField(SqliteCategory::PLAYLIST_FIELD,        SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCategoriesRM::~SqliteCategoriesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCategoryI>* SqliteCategoriesRM::retrieveAll()
{
	SqliteQuery<SqliteCategory>* query = new SqliteQuery<SqliteCategory>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBCategoryI>* SqliteCategoriesRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteCategory>* query = new SqliteQuery<SqliteCategory>(*this);

	string filter = SqliteCategory::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteCategory::NUMBER_FIELD);

	return query;
}


DBQueryI<DBCategoryI>* SqliteCategoriesRM::retrieveAllByName()
{
	SqliteQuery<SqliteCategory>* query = new SqliteQuery<SqliteCategory>(*this);

	if ( sdb_.getCurrentLocaleLanguageNumber() == 2 )
		query->selectAll(SqliteCategory::CODEDNAME2_FIELD);
	else
		query->selectAll(SqliteCategory::CODEDNAME1_FIELD);

	return query;
}


DBQueryI<DBCategoryI>* SqliteCategoriesRM::retrieveAllByName(const std::string& deptNum)
{
	SqliteQuery<SqliteCategory>* query = new SqliteQuery<SqliteCategory>(*this);

	string filter = SqliteCategory::DEPTNUM_FIELD + " = " + deptNum;
	if (sdb_.getCurrentLocaleLanguageNumber() == 2)
		query->selectSome(filter, SqliteCategory::CODEDNAME2_FIELD);
	else
		query->selectSome(filter, SqliteCategory::CODEDNAME1_FIELD);

	return query;
}


DBQueryI<DBCategoryI>* SqliteCategoriesRM::retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteCategory>* query = new SqliteQuery<SqliteCategory>(*this);

	string filter = SqliteCategory::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCategory::NUMBER_FIELD + ">=" + first + " and " + SqliteCategory::NUMBER_FIELD + "<=" + last;
	query->selectSome(filter, SqliteCategory::NUMBER_FIELD);

	return query;
}


DBQueryI<DBCategoryI>* SqliteCategoriesRM::retrieveChildCategories(const std::string& ca, const std::string& deptNum)
{
	SqliteQuery<SqliteCategory>* query = new SqliteQuery<SqliteCategory>(*this);

	string filter = SqliteCategory::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCategory::PARENTCATEGORY_FIELD + "=" + ca;
	if ( sdb_.getCurrentLocaleLanguageNumber() == 2 ) // By default use NAME1
		query->selectSomeIndexed(filter, SqliteCategory::CODEDNAME2_FIELD);
	else
		query->selectSomeIndexed(filter, SqliteCategory::CODEDNAME1_FIELD);

	return query;
}


DBQueryI<DBCategoryI>* SqliteCategoriesRM::retrieveAllSubcategories(const std::string& deptNum)
{
	SqliteQuery<SqliteCategory>* query = new SqliteQuery<SqliteCategory>(*this);

	string filter = "(" + SqliteCategory::DEPTNUM_FIELD + " = " + deptNum + " and (" + SqliteCategory::PARENTCATEGORY_FIELD + " != 999999) and (" + SqliteCategory::PARENTCATEGORY_FIELD + " != 0)";
	if ( sdb_.getCurrentLocaleLanguageNumber() == 2 ) // By default use NAME1
		query->selectSomeIndexed(filter, SqliteCategory::CODEDNAME2_FIELD);
	else
		query->selectSomeIndexed(filter, SqliteCategory::CODEDNAME1_FIELD);

	return query;
}


DBCategoryI* SqliteCategoriesRM::findNumber(const std::string& number, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteCategory::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteCategory::NUMBER_FIELD, SqliteField::INTEGER, number));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteCategory(findrec);

	return 0;
}


DBCategoryI* SqliteCategoriesRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteCategory* createrec = new SqliteCategory(dbrec);

		if ( createrec )
			return createrec;
		else
			delete dbrec;
	}

	return 0;
}


void SqliteCategoriesRM::saveRecord(const DBCategoryI* rec)
{
	const SqliteCategory* inrec = dynamic_cast<const SqliteCategory*>(rec);

	if ( inrec )
	{
		string transformedName;

		// Updated the coded name 1 field
		string name1 = inrec->getName1();
		int truncSize = (int)name1.size() * 2;
		if (truncSize > localeTransformedLength_)
			truncSize = localeTransformedLength_;

		util_string::localeTransform(transformedName, name1, sdb_.getLanguageCode(1));
		const_cast<SqliteCategory*>(inrec)->setCodedName1(transformedName.substr(0, truncSize));

		// Updated the coded name 2 field
		string name2 = inrec->getName2();
		truncSize = (int)name2.size() * 2;
		if (truncSize > localeTransformedLength_)
			truncSize = localeTransformedLength_;

		util_string::localeTransform(transformedName, name2, sdb_.getLanguageCode(2));
		const_cast<SqliteCategory*>(inrec)->setCodedName2(transformedName.substr(0, truncSize));

		// Now save the record
		if ( SqliteRecordManager::save(inrec->getRecord()) )
			modified();
		else
			throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "Unable to save record.");
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCategoriesRM::deleteRecord(const DBCategoryI* rec)
{
	const SqliteCategory* inrec = dynamic_cast<const SqliteCategory*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCategoriesRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteCategory::DEPTNUM_FIELD + " = " + deptNum);
}


void SqliteCategoriesRM::retrieveProperties()
{
	const string* property = getTableProperty("localeTransformedLength");
	if ( property )
		localeTransformedLength_ = util_string::stringtoint(*property);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
