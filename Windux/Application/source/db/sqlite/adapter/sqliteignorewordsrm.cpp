#include <string>


#include "sqlitedatabase.h"
#include "sqliteignorewordsrm.h"
#include "sqliteignoreword.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteIgnoreWord>::_dataType_ = 0;
template <> string SqliteQuery<SqliteIgnoreWord>::_dataTypeName_("SqliteQuery<SqliteIgnoreWord>");

int SqliteIgnoreWordsRM::_dataType_ = 0;
string SqliteIgnoreWordsRM::_dataTypeName_("SqliteIgnoreWordsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteIgnoreWordsRM::DATA_FILENAME("IgnoreWords");



SqliteIgnoreWordsRM::SqliteIgnoreWordsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBIgnoreWordsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteIgnoreWord::LANGUAGE_FIELD);
	columnNames_.insert(SqliteIgnoreWord::NAME_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteIgnoreWord::LANGUAGE_FIELD, SqliteField(SqliteIgnoreWord::LANGUAGE_FIELD, SqliteField::STRING)));
	columns_.insert(make_pair(SqliteIgnoreWord::NAME_FIELD,     SqliteField(SqliteIgnoreWord::NAME_FIELD,     SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteIgnoreWordsRM::~SqliteIgnoreWordsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBIgnoreWordI>* SqliteIgnoreWordsRM::retrieveAll()
{
	SqliteQuery<SqliteIgnoreWord>* query = new SqliteQuery<SqliteIgnoreWord>(*this);

	query->selectAll();

	return query;
}


DBIgnoreWordI* SqliteIgnoreWordsRM::find(const string& language, const string& name)
{
#ifndef EVOL_NDEBUG
	Debug::trace("database.ignoreWords", 28, "Searching for word '" + name + "' in language '" + language + "'");
#endif

	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteIgnoreWord::LANGUAGE_FIELD, SqliteField::STRING, language));
	keys.push_back(SqliteField(SqliteIgnoreWord::NAME_FIELD, SqliteField::STRING, name));
		
	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
	{
#ifndef EVOL_NDEBUG
	Debug::trace("database.ignoreWords", 28, "Found word '" + name + "' in language '" + language + "'");
#endif

		return new SqliteIgnoreWord(findrec);
	}

	return 0;
}


DBIgnoreWordI* SqliteIgnoreWordsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteIgnoreWord* createrec = new SqliteIgnoreWord(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteIgnoreWordsRM::saveRecord(const DBIgnoreWordI* rec)
{
	const SqliteIgnoreWord* inrec = dynamic_cast<const SqliteIgnoreWord*>(rec);

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


int SqliteIgnoreWordsRM::deleteRecord(const DBIgnoreWordI* rec)
{
	const SqliteIgnoreWord* inrec = dynamic_cast<const SqliteIgnoreWord*>(rec);

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





#ifdef SQLITEIGNOREWORDSRM_DRIVER

#include <iostream>

int main(int argc, char * argv[])
{
	unsigned long int tot;
	SqliteIgnoreWordsRM prods;

	tot = prods.countAll();
	if ( tot > 0 )
	{
		SqliteIgnoreWordsRM::iterator it = prods.begin();
		SqliteIgnoreWordsRM::iterator end = prods.end();

		while ( it != end )
			cout << *it;
	}
}

#endif // SQLITEIGNOREWORDSRM_DRIVER
