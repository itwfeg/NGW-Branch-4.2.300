#include <string>


#include "sqlitedatabase.h"
#include "sqliteoperatorsrm.h"
#include "sqliteoperator.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteOperator>::_dataType_ = 0;
template <> string SqliteQuery<SqliteOperator>::_dataTypeName_("SqliteQuery<SqliteOperator>");

int SqliteOperatorsRM::_dataType_ = 0;
string SqliteOperatorsRM::_dataTypeName_("SqliteOperatorsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteOperatorsRM::DATA_FILENAME("Operators");



SqliteOperatorsRM::SqliteOperatorsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBOperatorsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteOperator::NUMBER_FIELD);
	columnNames_.insert(SqliteOperator::RFID_FIELD);
	columnNames_.insert(SqliteOperator::CLASSIFICATION_FIELD);
	columnNames_.insert(SqliteOperator::LANGUAGE_FIELD);
	columnNames_.insert(SqliteOperator::SAWNOTE_FIELD);
	columnNames_.insert(SqliteOperator::NAME_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteOperator::NUMBER_FIELD,         SqliteField(SqliteOperator::NUMBER_FIELD,         SqliteField::STRING)));
	columns_.insert(make_pair(SqliteOperator::RFID_FIELD,           SqliteField(SqliteOperator::RFID_FIELD,           SqliteField::STRING)));
	columns_.insert(make_pair(SqliteOperator::CLASSIFICATION_FIELD, SqliteField(SqliteOperator::CLASSIFICATION_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteOperator::LANGUAGE_FIELD,       SqliteField(SqliteOperator::LANGUAGE_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteOperator::SAWNOTE_FIELD,        SqliteField(SqliteOperator::SAWNOTE_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteOperator::NAME_FIELD,           SqliteField(SqliteOperator::NAME_FIELD,           SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteOperatorsRM::~SqliteOperatorsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBOperatorI>* SqliteOperatorsRM::retrieveAll()
{
	SqliteQuery<SqliteOperator>* query = new SqliteQuery<SqliteOperator>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBOperatorI>* SqliteOperatorsRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteOperator>* query = new SqliteQuery<SqliteOperator>(*this);

	string filter = SqliteOperator::NUMBER_FIELD + " >= " + first + " and " + SqliteOperator::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteOperator::NUMBER_FIELD);

	return query;
}


DBOperatorI* SqliteOperatorsRM::findNumber(const string& oper)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteOperator::NUMBER_FIELD, SqliteField::STRING, oper));
	if ( findrec )
		return new SqliteOperator(findrec);

	return 0;
}


DBOperatorI* SqliteOperatorsRM::findRFID(const string& id)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteOperator::RFID_FIELD, SqliteField::STRING, id));
	if ( findrec )
		return new SqliteOperator(findrec);

	return 0;
}


DBOperatorI* SqliteOperatorsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteOperator* createrec = new SqliteOperator(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteOperatorsRM::saveRecord(const DBOperatorI* rec)
{
	const SqliteOperator* inrec = dynamic_cast<const SqliteOperator*>(rec);

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


int SqliteOperatorsRM::deleteRecord(const DBOperatorI* rec)
{
	const SqliteOperator* inrec = dynamic_cast<const SqliteOperator*>(rec);

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





#ifdef SQLITEPRODUCTSRM_DRIVER

#include <iostream>

int main(int argc, char * argv[])
{
	unsigned long int tot;
	SqliteOperatorsRM prods;

	tot = prods.countAll();
	if ( tot > 0 )
	{
		SqliteOperatorsRM::iterator it = prods.begin();
		SqliteOperatorsRM::iterator end = prods.end();

		while ( it != end )
			cout << *it;
	}
}

#endif // SQLITEPRODUCTSRM_DRIVER
