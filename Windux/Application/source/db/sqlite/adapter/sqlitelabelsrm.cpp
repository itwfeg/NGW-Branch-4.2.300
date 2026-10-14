#include <string>


#include "sqlitedatabase.h"
#include "sqlitelabelsrm.h"
#include "sqlitelabel.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteLabel>::_dataType_ = 0;
template <> string SqliteQuery<SqliteLabel>::_dataTypeName_("SqliteQuery<SqliteLabel>");

int SqliteLabelsRM::_dataType_ = 0;
string SqliteLabelsRM::_dataTypeName_("SqliteLabelsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteLabelsRM::DATA_FILENAME("Labels");



SqliteLabelsRM::SqliteLabelsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBLabelsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteLabel::NUMBER_FIELD);
	columnNames_.insert(SqliteLabel::DEPTNUM_FIELD);
	columnNames_.insert(SqliteLabel::BINARY_FIELD);
	columnNames_.insert(SqliteLabel::ROTATION_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteLabel::NUMBER_FIELD,  SqliteField(SqliteLabel::NUMBER_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteLabel::DEPTNUM_FIELD, SqliteField(SqliteLabel::DEPTNUM_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteLabel::BINARY_FIELD,  SqliteField(SqliteLabel::BINARY_FIELD,  SqliteField::STRING)));
	columns_.insert(make_pair(SqliteLabel::ROTATION_FIELD, SqliteField(SqliteLabel::ROTATION_FIELD, SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteLabelsRM::~SqliteLabelsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBLabelI>* SqliteLabelsRM::retrieveAll()
{
	SqliteQuery<SqliteLabel>* query = new SqliteQuery<SqliteLabel>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBLabelI>* SqliteLabelsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteLabel>* query = new SqliteQuery<SqliteLabel>(*this);

	string filter = SqliteLabel::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteLabel::NUMBER_FIELD);

	return query;
}


DBQueryI<DBLabelI>* SqliteLabelsRM::retrieveLabelNumListForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteLabel>* query = new SqliteQuery<SqliteLabel>(*this);
	if (query)
	{
		// Since deptNum is part of the primary key, it can't be null, so we don't have to worry about checking if it's null in the query.
		string sql = "select " + SqliteLabel::NUMBER_FIELD + " from " + this->getTableName() +
		             " where " + SqliteLabel::DEPTNUM_FIELD + " = " + deptNum +
		             " order by " + SqliteLabel::NUMBER_FIELD;
		query->selectUsingQuery(sql);
	}
	return query;
}


DBQueryI<DBLabelI>* SqliteLabelsRM::retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteLabel>* query = new SqliteQuery<SqliteLabel>(*this);

	string filter = SqliteLabel::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteLabel::NUMBER_FIELD + " >= " + first + " and " + SqliteLabel::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteLabel::NUMBER_FIELD);

	return query;
}


DBLabelI* SqliteLabelsRM::findNumber(const std::string& number, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteLabel::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteLabel::NUMBER_FIELD, SqliteField::INTEGER, number));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteLabel(findrec);

	return 0;
}


DBLabelI* SqliteLabelsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteLabel* createrec = new SqliteLabel(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteLabelsRM::saveRecord(const DBLabelI* rec)
{
	const SqliteLabel* inrec = dynamic_cast<const SqliteLabel*>(rec);

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


int SqliteLabelsRM::deleteRecord(const DBLabelI* rec)
{
	const SqliteLabel* inrec = dynamic_cast<const SqliteLabel*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteLabelsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteLabel::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
