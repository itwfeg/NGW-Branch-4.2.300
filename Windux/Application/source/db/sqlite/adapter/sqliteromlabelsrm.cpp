#include <string>


#include "sqlitedatabase.h"
#include "sqliteromlabelsrm.h"
#include "sqliteromlabel.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteROMLabel>::_dataType_ = 0;
template <> string SqliteQuery<SqliteROMLabel>::_dataTypeName_("SqliteQuery<SqliteROMLabel>");

int SqliteROMLabelsRM::_dataType_ = 0;
string SqliteROMLabelsRM::_dataTypeName_("SqliteROMLabelsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteROMLabelsRM::DATA_FILENAME("ROMLabels");



SqliteROMLabelsRM::SqliteROMLabelsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBROMLabelsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteROMLabel::NUMBER_FIELD);
	columnNames_.insert(SqliteROMLabel::BINARY_FIELD);
	columnNames_.insert(SqliteROMLabel::ROTATION_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteROMLabel::NUMBER_FIELD, SqliteField(SqliteROMLabel::NUMBER_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteROMLabel::BINARY_FIELD, SqliteField(SqliteROMLabel::BINARY_FIELD, SqliteField::STRING)));
	columns_.insert(make_pair(SqliteROMLabel::ROTATION_FIELD, SqliteField(SqliteROMLabel::ROTATION_FIELD, SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteROMLabelsRM::~SqliteROMLabelsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBROMLabelI>* SqliteROMLabelsRM::retrieveAll()
{
	SqliteQuery<SqliteROMLabel>* query = new SqliteQuery<SqliteROMLabel>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBROMLabelI>* SqliteROMLabelsRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteROMLabel>* query = new SqliteQuery<SqliteROMLabel>(*this);

	string filter = SqliteROMLabel::NUMBER_FIELD + " >= " + first + " and " + SqliteROMLabel::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteROMLabel::NUMBER_FIELD);

	return query;
}


DBROMLabelI* SqliteROMLabelsRM::findNumber(const string& number)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteROMLabel::NUMBER_FIELD, SqliteField::INTEGER, number));
	if ( findrec )
		return new SqliteROMLabel(findrec);

	return 0;
}


DBROMLabelI* SqliteROMLabelsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteROMLabel* createrec = new SqliteROMLabel(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteROMLabelsRM::saveRecord(const DBROMLabelI* rec)
{
	const SqliteROMLabel* inrec = dynamic_cast<const SqliteROMLabel*>(rec);

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


int SqliteROMLabelsRM::deleteRecord(const DBROMLabelI* rec)
{
	const SqliteROMLabel* inrec = dynamic_cast<const SqliteROMLabel*>(rec);

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
