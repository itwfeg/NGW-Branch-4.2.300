#include <string>


#include "sqlitedatabase.h"
#include "sqliteoperatornotesrm.h"
#include "sqliteoperatornote.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteOperatorNote>::_dataType_ = 0;
template <> string SqliteQuery<SqliteOperatorNote>::_dataTypeName_("SqliteQuery<SqliteOperatorNote>");

int SqliteOperatorNotesRM::_dataType_ = 0;
string SqliteOperatorNotesRM::_dataTypeName_("SqliteOperatorNotesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteOperatorNotesRM::DATA_FILENAME("OperatorNotes");



SqliteOperatorNotesRM::SqliteOperatorNotesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBOperatorNotesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteOperatorNote::TEXT_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteOperatorNote::TEXT_FIELD, SqliteField(SqliteOperatorNote::TEXT_FIELD, SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteOperatorNotesRM::~SqliteOperatorNotesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBOperatorNoteI>* SqliteOperatorNotesRM::retrieveAll()
{
	SqliteQuery<SqliteOperatorNote>* query = new SqliteQuery<SqliteOperatorNote>(*this);

	query->selectAll();

	return query;
}


DBOperatorNoteI* SqliteOperatorNotesRM::findRecord()
{
	SqliteRecord* findrec = SqliteRecordManager::findSingleRecord();
	if ( findrec )
		return new SqliteOperatorNote(findrec);

	return 0;
}


DBOperatorNoteI* SqliteOperatorNotesRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteOperatorNote* createrec = new SqliteOperatorNote(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteOperatorNotesRM::saveRecord(const DBOperatorNoteI* rec)
{
	const SqliteOperatorNote* inrec = dynamic_cast<const SqliteOperatorNote*>(rec);

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


int SqliteOperatorNotesRM::deleteRecord(const DBOperatorNoteI* rec)
{
	const SqliteOperatorNote* inrec = dynamic_cast<const SqliteOperatorNote*>(rec);

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
