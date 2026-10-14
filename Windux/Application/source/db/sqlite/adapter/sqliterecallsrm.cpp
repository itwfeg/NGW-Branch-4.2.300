#include <string>


#include "sqlitedatabase.h"
#include "sqliterecallsrm.h"
#include "sqliterecall.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteRecall>::_dataType_ = 0;
template <> string SqliteQuery<SqliteRecall>::_dataTypeName_("SqliteQuery<SqliteRecall>");

int SqliteRecallsRM::_dataType_ = 0;
string SqliteRecallsRM::_dataTypeName_("SqliteRecallsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteRecallsRM::DATA_FILENAME("Recalls");



SqliteRecallsRM::SqliteRecallsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBRecallsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteRecall::NUMBER_FIELD);
	columnNames_.insert(SqliteRecall::NAME_FIELD);
	columnNames_.insert(SqliteRecall::TEXT_FIELD);
	columnNames_.insert(SqliteRecall::RECEIVEDDATETIME_FIELD);
	columnNames_.insert(SqliteRecall::ACKNOWLEDGED_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteRecall::NUMBER_FIELD,            SqliteField(SqliteRecall::NUMBER_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteRecall::NAME_FIELD,              SqliteField(SqliteRecall::NAME_FIELD,              SqliteField::STRING)));
	columns_.insert(make_pair(SqliteRecall::TEXT_FIELD,              SqliteField(SqliteRecall::TEXT_FIELD,              SqliteField::STRING)));
	columns_.insert(make_pair(SqliteRecall::RECEIVEDDATETIME_FIELD,  SqliteField(SqliteRecall::RECEIVEDDATETIME_FIELD,  SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteRecall::ACKNOWLEDGED_FIELD,      SqliteField(SqliteRecall::ACKNOWLEDGED_FIELD,      SqliteField::INTBOOLEAN)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteRecallsRM::~SqliteRecallsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBRecallI>* SqliteRecallsRM::retrieveAll()
{
	SqliteQuery<SqliteRecall>* query = new SqliteQuery<SqliteRecall>(*this);

	query->selectAll();

	return query;
}

DBQueryI<DBRecallI>* SqliteRecallsRM::retrieveAcknowledged()
{
	SqliteQuery<SqliteRecall>* query = new SqliteQuery<SqliteRecall>(*this);

	string filter = SqliteRecall::ACKNOWLEDGED_FIELD + " = 1";
	query->selectSome(filter);

	return query;
}

DBQueryI<DBRecallI>* SqliteRecallsRM::retrieveUnacknowledged()
{
	SqliteQuery<SqliteRecall>* query = new SqliteQuery<SqliteRecall>(*this);

	string filter = SqliteRecall::ACKNOWLEDGED_FIELD + " = 0";
	query->selectSome(filter);

	return query;
}

/*
DBQueryI<DBRecallI>* SqliteRecallsRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteRecall>* query = new SqliteQuery<SqliteRecall>(*this);

	string filter = SqliteRecall::NUMBER_FIELD + " >= " + first + " and " + SqliteRecall::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteRecall::NUMBER_FIELD);

	return query;
}
*/

DBRecallI* SqliteRecallsRM::findNumber(const string& num)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteRecall::NUMBER_FIELD, SqliteField::STRING, num));
	if ( findrec )
		return new SqliteRecall(findrec);

	return 0;
}


DBRecallI* SqliteRecallsRM::findName(const string& name)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteRecall::NAME_FIELD, SqliteField::STRING, name));
	if ( findrec )
		return new SqliteRecall(findrec);

	return 0;
}


DBRecallI* SqliteRecallsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteRecall* createrec = new SqliteRecall(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteRecallsRM::saveRecord(const DBRecallI* rec)
{
	const SqliteRecall* inrec = dynamic_cast<const SqliteRecall*>(rec);

	if ( inrec )
	{
		if ( SqliteRecordManager::save( inrec->getRecord()) )
			modified();
		else
			throw edb::DBException(edb::DBException::SQLITE_SAVE_RECORD, "Unable to save record.");
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteRecallsRM::deleteRecord(const DBRecallI* rec)
{
	const SqliteRecall* inrec = dynamic_cast<const SqliteRecall*>(rec);

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





#ifdef SQLITERECALLSRM_DRIVER

#include <iostream>

int main(int argc, char * argv[])
{
	unsigned long int tot;
	SqliteRecallsRM recalls;

	tot = recalls.countAll();
	if ( tot > 0 )
	{
		SqliteRecallsRM::iterator it = recalls.begin();
		SqliteRecallsRM::iterator end = recalls.end();

		while ( it != end )
			cout << *it;
	}
}

#endif // SQLITERECALLSRM_DRIVER
