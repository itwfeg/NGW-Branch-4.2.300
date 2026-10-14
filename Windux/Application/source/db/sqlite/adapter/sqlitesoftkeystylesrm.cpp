#include <string>


#include "sqlitedatabase.h"
#include "sqlitesoftkeystylesrm.h"
#include "sqlitesoftkeystyle.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteSoftKeyStyle>::_dataType_ = 0;
template <> string SqliteQuery<SqliteSoftKeyStyle>::_dataTypeName_("SqliteQuery<SqliteSoftKeyStyle>");

int SqliteSoftKeyStylesRM::_dataType_ = 0;
string SqliteSoftKeyStylesRM::_dataTypeName_("SqliteSoftKeyStylesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteSoftKeyStylesRM::DATA_FILENAME("SoftKeyStyles");


SqliteSoftKeyStylesRM::SqliteSoftKeyStylesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSoftKeyStylesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteSoftKeyStyle::NUMBER_FIELD);
	columnNames_.insert(SqliteSoftKeyStyle::DEFAULTTEXTLOCATION_FIELD);
	columnNames_.insert(SqliteSoftKeyStyle::DEFAULTTEXTCOLOR_FIELD);
	columnNames_.insert(SqliteSoftKeyStyle::DEFAULTGRAYEDOUTTEXTCOLOR_FIELD);
	columnNames_.insert(SqliteSoftKeyStyle::DEFAULTFOREGROUNDIMAGELOCATION_FIELD);
	columnNames_.insert(SqliteSoftKeyStyle::DEFAULTBACKGROUNDCOLOR_FIELD);
	columnNames_.insert(SqliteSoftKeyStyle::DEFAULTBACKGROUNDIMAGE_FIELD);
	columnNames_.insert(SqliteSoftKeyStyle::IMAGESDIRECTORY_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteSoftKeyStyle::NUMBER_FIELD,                         SqliteField(SqliteSoftKeyStyle::NUMBER_FIELD,                         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyStyle::DEFAULTTEXTLOCATION_FIELD,            SqliteField(SqliteSoftKeyStyle::DEFAULTTEXTLOCATION_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyStyle::DEFAULTTEXTCOLOR_FIELD,               SqliteField(SqliteSoftKeyStyle::DEFAULTTEXTCOLOR_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyStyle::DEFAULTGRAYEDOUTTEXTCOLOR_FIELD,      SqliteField(SqliteSoftKeyStyle::DEFAULTGRAYEDOUTTEXTCOLOR_FIELD,      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyStyle::DEFAULTFOREGROUNDIMAGELOCATION_FIELD, SqliteField(SqliteSoftKeyStyle::DEFAULTFOREGROUNDIMAGELOCATION_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyStyle::DEFAULTBACKGROUNDCOLOR_FIELD,         SqliteField(SqliteSoftKeyStyle::DEFAULTBACKGROUNDCOLOR_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyStyle::DEFAULTBACKGROUNDIMAGE_FIELD,         SqliteField(SqliteSoftKeyStyle::DEFAULTBACKGROUNDIMAGE_FIELD,         SqliteField::STRING)));
	columns_.insert(make_pair(SqliteSoftKeyStyle::IMAGESDIRECTORY_FIELD,                SqliteField(SqliteSoftKeyStyle::IMAGESDIRECTORY_FIELD,                SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteSoftKeyStylesRM::~SqliteSoftKeyStylesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBSoftKeyStyleI>* SqliteSoftKeyStylesRM::retrieveAll()
{
	SqliteQuery<SqliteSoftKeyStyle>* query = new SqliteQuery<SqliteSoftKeyStyle>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBSoftKeyStyleI>* SqliteSoftKeyStylesRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteSoftKeyStyle>* query = new SqliteQuery<SqliteSoftKeyStyle>(*this);

	string filter = SqliteSoftKeyStyle::NUMBER_FIELD + " >= " + first + " and " + SqliteSoftKeyStyle::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteSoftKeyStyle::NUMBER_FIELD);

	return query;
}


DBSoftKeyStyleI* SqliteSoftKeyStylesRM::findNumber(const string& number)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteSoftKeyStyle::NUMBER_FIELD, SqliteField::INTEGER, number));
	if ( findrec )
		return new SqliteSoftKeyStyle(findrec);

	return 0;
}


DBSoftKeyStyleI* SqliteSoftKeyStylesRM::createRecord(const std::string& number)
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteSoftKeyStyle* createrec = new SqliteSoftKeyStyle(dbrec);

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


void SqliteSoftKeyStylesRM::saveRecord(const DBSoftKeyStyleI* rec)
{
	const SqliteSoftKeyStyle* inrec = dynamic_cast<const SqliteSoftKeyStyle*>(rec);

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


int SqliteSoftKeyStylesRM::deleteRecord(const DBSoftKeyStyleI* rec)
{
	const SqliteSoftKeyStyle* inrec = dynamic_cast<const SqliteSoftKeyStyle*>(rec);

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
