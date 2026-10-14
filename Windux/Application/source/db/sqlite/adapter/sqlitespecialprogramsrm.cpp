#include <string>


#include "sqlitedatabase.h"
#include "sqlitespecialprogramsrm.h"
#include "sqlitespecialprogram.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteSpecialProgram>::_dataType_ = 0;
template <> string SqliteQuery<SqliteSpecialProgram>::_dataTypeName_("SqliteQuery<SqliteSpecialProgram>");

int SqliteSpecialProgramsRM::_dataType_ = 0;
string SqliteSpecialProgramsRM::_dataTypeName_("SqliteSpecialProgramsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteSpecialProgramsRM::DATA_FILENAME("Specials");



SqliteSpecialProgramsRM::SqliteSpecialProgramsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSpecialProgramsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteSpecialProgram::NUMBER_FIELD);
	columnNames_.insert(SqliteSpecialProgram::DEPTNUM_FIELD);
	columnNames_.insert(SqliteSpecialProgram::DESCRIPTION1_FIELD);
	//columnNames_.insert(SqliteSpecialProgram::DESCRIPTION2_FIELD);
	columnNames_.insert(SqliteSpecialProgram::DEFAULTLENGTH_FIELD);
	columnNames_.insert(SqliteSpecialProgram::IMAGE1_FIELD);
	columnNames_.insert(SqliteSpecialProgram::IMAGE2_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteSpecialProgram::NUMBER_FIELD,        SqliteField(SqliteSpecialProgram::NUMBER_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSpecialProgram::DEPTNUM_FIELD,       SqliteField(SqliteSpecialProgram::DEPTNUM_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSpecialProgram::DESCRIPTION1_FIELD,  SqliteField(SqliteSpecialProgram::DESCRIPTION1_FIELD,  SqliteField::STRING)));
	//columns_.insert(make_pair(SqliteSpecialProgram::DESCRIPTION2_FIELD,  SqliteField(SqliteSpecialProgram::DESCRIPTION2_FIELD,  SqliteField::STRING)));
	columns_.insert(make_pair(SqliteSpecialProgram::DEFAULTLENGTH_FIELD, SqliteField(SqliteSpecialProgram::DEFAULTLENGTH_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSpecialProgram::IMAGE1_FIELD,        SqliteField(SqliteSpecialProgram::IMAGE1_FIELD,        SqliteField::STRING)));
	columns_.insert(make_pair(SqliteSpecialProgram::IMAGE2_FIELD,        SqliteField(SqliteSpecialProgram::IMAGE2_FIELD,        SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteSpecialProgramsRM::~SqliteSpecialProgramsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBSpecialProgramI>* SqliteSpecialProgramsRM::retrieveAll()
{
	SqliteQuery<SqliteSpecialProgram>* query = new SqliteQuery<SqliteSpecialProgram>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBSpecialProgramI>* SqliteSpecialProgramsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteSpecialProgram>* query = new SqliteQuery<SqliteSpecialProgram>(*this);

	string filter = SqliteSpecialProgram::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteSpecialProgram::NUMBER_FIELD);

	return query;
}


DBQueryI<DBSpecialProgramI>* SqliteSpecialProgramsRM::retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteSpecialProgram>* query = new SqliteQuery<SqliteSpecialProgram>(*this);

	string filter = SqliteSpecialProgram::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteSpecialProgram::NUMBER_FIELD + " >= " + first + " and " + SqliteSpecialProgram::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteSpecialProgram::NUMBER_FIELD);

	return query;
}


DBSpecialProgramI* SqliteSpecialProgramsRM::findNumber(const std::string& number, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteSpecialProgram::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteSpecialProgram::NUMBER_FIELD, SqliteField::INTEGER, number));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteSpecialProgram(findrec);

	return 0;
}


DBSpecialProgramI* SqliteSpecialProgramsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteSpecialProgram* createrec = new SqliteSpecialProgram(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteSpecialProgramsRM::saveRecord(const DBSpecialProgramI* rec)
{
	const SqliteSpecialProgram* inrec = dynamic_cast<const SqliteSpecialProgram*>(rec);

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


int SqliteSpecialProgramsRM::deleteRecord(const DBSpecialProgramI* rec)
{
	const SqliteSpecialProgram* inrec = dynamic_cast<const SqliteSpecialProgram*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteSpecialProgramsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteSpecialProgram::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
