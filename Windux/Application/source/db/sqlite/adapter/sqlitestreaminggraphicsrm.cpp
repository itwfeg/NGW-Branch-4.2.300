#include <string>


#include "sqlitedatabase.h"
#include "sqlitestreaminggraphicsrm.h"
#include "sqlitestreaminggraphic.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteStreamingGraphic>::_dataType_ = 0;
template <> string SqliteQuery<SqliteStreamingGraphic>::_dataTypeName_("SqliteQuery<SqliteStreamingGraphic>");

int SqliteStreamingGraphicsRM::_dataType_ = 0;
string SqliteStreamingGraphicsRM::_dataTypeName_("SqliteStreamingGraphicsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteStreamingGraphicsRM::DATA_FILENAME("StreamingGraphics");



SqliteStreamingGraphicsRM::SqliteStreamingGraphicsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBStreamingGraphicsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteStreamingGraphic::NUMBER_FIELD);
	columnNames_.insert(SqliteStreamingGraphic::NEXT_FIELD);
	columnNames_.insert(SqliteStreamingGraphic::TIME_FIELD);
	columnNames_.insert(SqliteStreamingGraphic::ACTIONTYPE_FIELD);
	columnNames_.insert(SqliteStreamingGraphic::ACTIONPARAMETER_FIELD);
	columnNames_.insert(SqliteStreamingGraphic::IMAGE_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteStreamingGraphic::NUMBER_FIELD,          SqliteField(SqliteStreamingGraphic::NUMBER_FIELD,          SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteStreamingGraphic::NEXT_FIELD,            SqliteField(SqliteStreamingGraphic::NEXT_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteStreamingGraphic::TIME_FIELD,            SqliteField(SqliteStreamingGraphic::TIME_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteStreamingGraphic::ACTIONTYPE_FIELD,      SqliteField(SqliteStreamingGraphic::ACTIONTYPE_FIELD,      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteStreamingGraphic::ACTIONPARAMETER_FIELD, SqliteField(SqliteStreamingGraphic::ACTIONPARAMETER_FIELD, SqliteField::STRING)));
	columns_.insert(make_pair(SqliteStreamingGraphic::IMAGE_FIELD,           SqliteField(SqliteStreamingGraphic::IMAGE_FIELD,           SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteStreamingGraphicsRM::~SqliteStreamingGraphicsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBStreamingGraphicI>* SqliteStreamingGraphicsRM::retrieveAll()
{
	SqliteQuery<SqliteStreamingGraphic>* query = new SqliteQuery<SqliteStreamingGraphic>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBStreamingGraphicI>* SqliteStreamingGraphicsRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteStreamingGraphic>* query = new SqliteQuery<SqliteStreamingGraphic>(*this);

	string filter = SqliteStreamingGraphic::NUMBER_FIELD + " >= " + first + " and " + SqliteStreamingGraphic::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteStreamingGraphic::NUMBER_FIELD);

	return query;
}


DBStreamingGraphicI* SqliteStreamingGraphicsRM::findNumber(const string& number)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteStreamingGraphic::NUMBER_FIELD, SqliteField::INTEGER, number));
	if ( findrec )
		return new SqliteStreamingGraphic(findrec);

	return 0;
}


DBStreamingGraphicI* SqliteStreamingGraphicsRM::findNext(const DBStreamingGraphicI* sg)
{
	const SqliteStreamingGraphic* inrec = dynamic_cast<const SqliteStreamingGraphic*>(sg);
	if ( inrec )
	{
		SqliteRecord* findrec = 0;

		if ( !inrec->isNextNull() )
			findrec = SqliteRecordManager::findKey(SqliteField(SqliteStreamingGraphic::NUMBER_FIELD, SqliteField::INTEGER, inrec->getNext()));
		else // This 'else' clause is application logic and should not be here. Remove later.
			findrec = SqliteRecordManager::findKey(SqliteField(SqliteStreamingGraphic::NUMBER_FIELD, SqliteField::INTEGER, stringtoint(inrec->getNumber())+1));

		if ( findrec )
			return new SqliteStreamingGraphic(findrec);
	}

	return 0;
}


DBStreamingGraphicI* SqliteStreamingGraphicsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteStreamingGraphic* createrec = new SqliteStreamingGraphic(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteStreamingGraphicsRM::saveRecord(const DBStreamingGraphicI* rec)
{
	const SqliteStreamingGraphic* inrec = dynamic_cast<const SqliteStreamingGraphic*>(rec);

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


int SqliteStreamingGraphicsRM::deleteRecord(const DBStreamingGraphicI* rec)
{
	const SqliteStreamingGraphic* inrec = dynamic_cast<const SqliteStreamingGraphic*>(rec);

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
