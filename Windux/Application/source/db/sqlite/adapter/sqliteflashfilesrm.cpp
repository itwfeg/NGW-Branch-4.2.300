#include <string>


#include "sqlitedatabase.h"
#include "sqliteflashfilesrm.h"
#include "sqliteflashfile.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteFlashFile>::_dataType_ = 0;
template <> string SqliteQuery<SqliteFlashFile>::_dataTypeName_("SqliteQuery<SqliteFlashFile>");

int SqliteFlashFilesRM::_dataType_ = 0;
string SqliteFlashFilesRM::_dataTypeName_("SqliteFlashFilesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteFlashFilesRM::DATA_FILENAME("FlashFiles");



SqliteFlashFilesRM::SqliteFlashFilesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBFlashFilesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteFlashFile::NAME_FIELD);
	columnNames_.insert(SqliteFlashFile::DATE_FIELD);
	columnNames_.insert(SqliteFlashFile::COMPLETE_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteFlashFile::NAME_FIELD,     SqliteField(SqliteFlashFile::NAME_FIELD,     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteFlashFile::DATE_FIELD,     SqliteField(SqliteFlashFile::DATE_FIELD,     SqliteField::DATETIME)));
	columns_.insert(make_pair(SqliteFlashFile::COMPLETE_FIELD, SqliteField(SqliteFlashFile::COMPLETE_FIELD, SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteFlashFilesRM::~SqliteFlashFilesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBFlashFileI>* SqliteFlashFilesRM::retrieveAll()
{
	SqliteQuery<SqliteFlashFile>* query = new SqliteQuery<SqliteFlashFile>(*this);

	query->selectAll();

	return query;
}


DBFlashFileI* SqliteFlashFilesRM::findName(const string& name)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteFlashFile::NAME_FIELD, SqliteField::STRING, name));
	if ( findrec )
		return new SqliteFlashFile(findrec);

	return 0;
}


DBFlashFileI* SqliteFlashFilesRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteFlashFile* createrec = new SqliteFlashFile(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteFlashFilesRM::saveRecord(const DBFlashFileI* rec)
{
	const SqliteFlashFile* inrec = dynamic_cast<const SqliteFlashFile*>(rec);

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


int SqliteFlashFilesRM::deleteRecord(const DBFlashFileI* rec)
{
	const SqliteFlashFile* inrec = dynamic_cast<const SqliteFlashFile*>(rec);

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





#ifdef SQLITEFLASHFILESRM_DRIVER

#include <iostream>

int main(int argc, char * argv[])
{
	unsigned long int tot;
	SqliteFlashFilesRM prods;

	tot = prods.countAll();
	if ( tot > 0 )
	{
		SqliteFlashFilesRM::iterator it = prods.begin();
		SqliteFlashFilesRM::iterator end = prods.end();

		while ( it != end )
			cout << *it;
	}
}

#endif // SQLITEFLASHFILESRM_DRIVER
