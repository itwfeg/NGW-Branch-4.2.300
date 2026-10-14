#include <string>


#include "sqlitedatabase.h"
#include "sqliteknifetimehistoryrm.h"
#include "sqliteknifetimehistory.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteKnifeTimeHistory>::_dataType_ = 0;
template <> string SqliteQuery<SqliteKnifeTimeHistory>::_dataTypeName_("SqliteQuery<SqliteKnifeTimeHistory>");

int SqliteKnifeTimeHistoryRM::_dataType_ = 0;
string SqliteKnifeTimeHistoryRM::_dataTypeName_("SqliteKnifeTimeHistoryRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteKnifeTimeHistoryRM::DATA_FILENAME("WrapperKnifeTimes");



SqliteKnifeTimeHistoryRM::SqliteKnifeTimeHistoryRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBKnifeTimeHistoryRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteKnifeTimeHistory::TYPE_FIELD);
	columnNames_.insert(SqliteKnifeTimeHistory::KNIFETIME_FIELD);
	columnNames_.insert(SqliteKnifeTimeHistory::KNIFETIMESTAMP_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteKnifeTimeHistory::TYPE_FIELD,           SqliteField(SqliteKnifeTimeHistory::TYPE_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteKnifeTimeHistory::KNIFETIME_FIELD,      SqliteField(SqliteKnifeTimeHistory::KNIFETIME_FIELD,      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteKnifeTimeHistory::KNIFETIMESTAMP_FIELD, SqliteField(SqliteKnifeTimeHistory::KNIFETIMESTAMP_FIELD, SqliteField::DATETIME)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteKnifeTimeHistoryRM::~SqliteKnifeTimeHistoryRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBKnifeTimeHistoryI>* SqliteKnifeTimeHistoryRM::retrieveAll()
{
	SqliteQuery<SqliteKnifeTimeHistory>* query = new SqliteQuery<SqliteKnifeTimeHistory>(*this);

	query->selectAll();

	return query;
}


DBKnifeTimeHistoryI* SqliteKnifeTimeHistoryRM::find(const string& number)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteKnifeTimeHistory::TYPE_FIELD, SqliteField::INTEGER, number));
	if ( findrec )
		return new SqliteKnifeTimeHistory(findrec);

	return 0;
}


DBKnifeTimeHistoryI* SqliteKnifeTimeHistoryRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteKnifeTimeHistory* createrec = new SqliteKnifeTimeHistory(dbrec);

		if ( createrec )
			return createrec;
		else
			delete dbrec;
	}

	return 0;
}


void SqliteKnifeTimeHistoryRM::saveRecord(const DBKnifeTimeHistoryI* rec)
{
	const SqliteKnifeTimeHistory* inrec = dynamic_cast<const SqliteKnifeTimeHistory*>(rec);

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


int SqliteKnifeTimeHistoryRM::deleteRecord(const DBKnifeTimeHistoryI* rec)
{
	const SqliteKnifeTimeHistory* inrec = dynamic_cast<const SqliteKnifeTimeHistory*>(rec);

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
