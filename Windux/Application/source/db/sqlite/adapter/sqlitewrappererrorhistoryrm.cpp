#include <string>


#include "sqlitedatabase.h"
#include "sqlitewrappererrorhistoryrm.h"
#include "sqlitewrappererrorhistory.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteWrapperErrorHistory>::_dataType_ = 0;
template <> string SqliteQuery<SqliteWrapperErrorHistory>::_dataTypeName_("SqliteQuery<SqliteWrapperErrorHistory>");

int SqliteWrapperErrorHistoryRM::_dataType_ = 0;
string SqliteWrapperErrorHistoryRM::_dataTypeName_("SqliteWrapperErrorHistoryRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteWrapperErrorHistoryRM::DATA_FILENAME("WrapperErrorHistory");



SqliteWrapperErrorHistoryRM::SqliteWrapperErrorHistoryRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBWrapperErrorHistoryRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteWrapperErrorHistory::ERRORID_FIELD);
	columnNames_.insert(SqliteWrapperErrorHistory::ERROR_FIELD);
	columnNames_.insert(SqliteWrapperErrorHistory::DATETIME_FIELD);
	columnNames_.insert(SqliteWrapperErrorHistory::PKGLENGTH_FIELD);
	columnNames_.insert(SqliteWrapperErrorHistory::PKGWIDTH_FIELD);
	columnNames_.insert(SqliteWrapperErrorHistory::PKGHEIGHT_FIELD);
	columnNames_.insert(SqliteWrapperErrorHistory::FILMUSED_FIELD);
	columnNames_.insert(SqliteWrapperErrorHistory::TRAYNAME_FIELD);
	columnNames_.insert(SqliteWrapperErrorHistory::BESTFIT_FIELD);
	columnNames_.insert(SqliteWrapperErrorHistory::PERNET_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteWrapperErrorHistory::ERRORID_FIELD,   SqliteField(SqliteWrapperErrorHistory::ERRORID_FIELD,   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteWrapperErrorHistory::ERROR_FIELD,     SqliteField(SqliteWrapperErrorHistory::ERROR_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteWrapperErrorHistory::DATETIME_FIELD,  SqliteField(SqliteWrapperErrorHistory::DATETIME_FIELD,  SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteWrapperErrorHistory::PKGLENGTH_FIELD, SqliteField(SqliteWrapperErrorHistory::PKGLENGTH_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteWrapperErrorHistory::PKGWIDTH_FIELD,  SqliteField(SqliteWrapperErrorHistory::PKGWIDTH_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteWrapperErrorHistory::PKGHEIGHT_FIELD, SqliteField(SqliteWrapperErrorHistory::PKGHEIGHT_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteWrapperErrorHistory::FILMUSED_FIELD,  SqliteField(SqliteWrapperErrorHistory::FILMUSED_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteWrapperErrorHistory::TRAYNAME_FIELD,  SqliteField(SqliteWrapperErrorHistory::TRAYNAME_FIELD,  SqliteField::STRING)));
	columns_.insert(make_pair(SqliteWrapperErrorHistory::BESTFIT_FIELD,   SqliteField(SqliteWrapperErrorHistory::BESTFIT_FIELD,   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteWrapperErrorHistory::PERNET_FIELD,    SqliteField(SqliteWrapperErrorHistory::PERNET_FIELD,    SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteWrapperErrorHistoryRM::~SqliteWrapperErrorHistoryRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBWrapperErrorHistoryI>* SqliteWrapperErrorHistoryRM::retrieveAll()
{
	SqliteQuery<SqliteWrapperErrorHistory>* query = new SqliteQuery<SqliteWrapperErrorHistory>(*this);

	query->selectAll();

	return query;
}


DBWrapperErrorHistoryI* SqliteWrapperErrorHistoryRM::find(const string& number)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteWrapperErrorHistory::ERROR_FIELD, SqliteField::INTEGER, number));
	if ( findrec )
		return new SqliteWrapperErrorHistory(findrec);

	return 0;
}


DBWrapperErrorHistoryI* SqliteWrapperErrorHistoryRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteWrapperErrorHistory* createrec = new SqliteWrapperErrorHistory(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteWrapperErrorHistoryRM::saveRecord(const DBWrapperErrorHistoryI* rec)
{
	const SqliteWrapperErrorHistory* inrec = dynamic_cast<const SqliteWrapperErrorHistory*>(rec);

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


int SqliteWrapperErrorHistoryRM::deleteRecord(const DBWrapperErrorHistoryI* rec)
{
	const SqliteWrapperErrorHistory* inrec = dynamic_cast<const SqliteWrapperErrorHistory*>(rec);

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
