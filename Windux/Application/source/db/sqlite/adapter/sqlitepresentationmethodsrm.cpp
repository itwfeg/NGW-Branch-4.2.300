#include <string>


#include "sqlitedatabase.h"
#include "sqlitepresentationmethodsrm.h"
#include "sqlitepresentationmethod.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqlitePresentationMethod>::_dataType_ = 0;
template <> string SqliteQuery<SqlitePresentationMethod>::_dataTypeName_("SqliteQuery<SqlitePresentationMethod>");

int SqlitePresentationMethodsRM::_dataType_ = 0;
string SqlitePresentationMethodsRM::_dataTypeName_("SqlitePresentationMethodsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqlitePresentationMethodsRM::DATA_FILENAME("PresentationMethods");



SqlitePresentationMethodsRM::SqlitePresentationMethodsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBPresentationMethodsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqlitePresentationMethod::TYPE_FIELD);
	columnNames_.insert(SqlitePresentationMethod::DESCRIPTION1_FIELD);
	//columnNames_.insert(SqlitePresentationMethod::DESCRIPTION2_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);
	
	columns_.insert(make_pair(SqlitePresentationMethod::TYPE_FIELD,         SqliteField(SqlitePresentationMethod::TYPE_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqlitePresentationMethod::DESCRIPTION1_FIELD, SqliteField(SqlitePresentationMethod::DESCRIPTION1_FIELD, SqliteField::STRING)));
	//columns_.insert(make_pair(SqlitePresentationMethod::DESCRIPTION2_FIELD, SqliteField(SqlitePresentationMethod::DESCRIPTION2_FIELD, SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqlitePresentationMethodsRM::~SqlitePresentationMethodsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBPresentationMethodI>* SqlitePresentationMethodsRM::retrieveAll()
{
	SqliteQuery<SqlitePresentationMethod>* query = new SqliteQuery<SqlitePresentationMethod>(*this);

	query->selectAll();

	return query;
}


DBPresentationMethodI* SqlitePresentationMethodsRM::find(const string& type)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqlitePresentationMethod::TYPE_FIELD, SqliteField::INTEGER, type));
	if ( findrec )
		return new SqlitePresentationMethod(findrec);

	return 0;
}


DBPresentationMethodI* SqlitePresentationMethodsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqlitePresentationMethod* createrec = new SqlitePresentationMethod(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqlitePresentationMethodsRM::saveRecord(const DBPresentationMethodI* rec)
{
	const SqlitePresentationMethod* inrec = dynamic_cast<const SqlitePresentationMethod*>(rec);

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


int SqlitePresentationMethodsRM::deleteRecord(const DBPresentationMethodI* rec)
{
	const SqlitePresentationMethod* inrec = dynamic_cast<const SqlitePresentationMethod*>(rec);

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
