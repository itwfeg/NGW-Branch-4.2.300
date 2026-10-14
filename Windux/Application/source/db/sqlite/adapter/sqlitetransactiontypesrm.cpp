#include <string>


#include "sqlitedatabase.h"
#include "sqlitetransactiontypesrm.h"
#include "sqlitetransactiontype.h"
#include "sqlitetransaction.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteTransactionType>::_dataType_ = 0;
template <> string SqliteQuery<SqliteTransactionType>::_dataTypeName_("edb::SqliteQuery<SqliteTransactionType>");

int SqliteTransactionTypesRM::_dataType_ = 0;
string SqliteTransactionTypesRM::_dataTypeName_("edb::SqliteTransactionTypesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteTransactionTypesRM::DATA_FILENAME("TransactionTypes");



SqliteTransactionTypesRM::SqliteTransactionTypesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBTransactionTypesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteTransactionType::NUMBER_FIELD);
	columnNames_.insert(SqliteTransactionType::NAME1_FIELD);
	columnNames_.insert(SqliteTransactionType::NAME2_FIELD);
	columnNames_.insert(SqliteTransactionType::AFFECTINGPRODUCTION_FIELD);
	columnNames_.insert(SqliteTransactionType::ALLOWINGBACKDATING_FIELD);
	columnNames_.insert(SqliteTransactionType::AUTOMATICALLYCHANGINGUPCDIGIT_FIELD);
	columnNames_.insert(SqliteTransactionType::MANUALLYCHANGINGUPCDIGIT_FIELD);
	columnNames_.insert(SqliteTransactionType::UPCDIGIT_FIELD);
	columnNames_.insert(SqliteTransactionType::USEBACKGROUNDCOLOR_FIELD);
	columnNames_.insert(SqliteTransactionType::BACKGROUNDCOLOR_FIELD);
	columnNames_.insert(SqliteTransactionType::TEXTFORLABEL_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteTransactionType::NUMBER_FIELD,                        SqliteField(SqliteTransactionType::NUMBER_FIELD,                        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransactionType::NAME1_FIELD,                         SqliteField(SqliteTransactionType::NAME1_FIELD,                         SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransactionType::NAME2_FIELD,                         SqliteField(SqliteTransactionType::NAME2_FIELD,                         SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransactionType::AFFECTINGPRODUCTION_FIELD,           SqliteField(SqliteTransactionType::AFFECTINGPRODUCTION_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransactionType::ALLOWINGBACKDATING_FIELD,            SqliteField(SqliteTransactionType::ALLOWINGBACKDATING_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransactionType::AUTOMATICALLYCHANGINGUPCDIGIT_FIELD, SqliteField(SqliteTransactionType::AUTOMATICALLYCHANGINGUPCDIGIT_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransactionType::MANUALLYCHANGINGUPCDIGIT_FIELD,      SqliteField(SqliteTransactionType::MANUALLYCHANGINGUPCDIGIT_FIELD,      SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransactionType::UPCDIGIT_FIELD,                      SqliteField(SqliteTransactionType::UPCDIGIT_FIELD,                      SqliteField::STRING)));
	columns_.insert(make_pair(SqliteTransactionType::USEBACKGROUNDCOLOR_FIELD,            SqliteField(SqliteTransactionType::USEBACKGROUNDCOLOR_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransactionType::BACKGROUNDCOLOR_FIELD,               SqliteField(SqliteTransactionType::BACKGROUNDCOLOR_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteTransactionType::TEXTFORLABEL_FIELD,                  SqliteField(SqliteTransactionType::TEXTFORLABEL_FIELD,                  SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteTransactionTypesRM::~SqliteTransactionTypesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBTransactionTypeI>* SqliteTransactionTypesRM::retrieveAll()
{
	SqliteQuery<SqliteTransactionType>* query = new SqliteQuery<SqliteTransactionType>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBTransactionTypeI>* SqliteTransactionTypesRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteTransactionType>* query = new SqliteQuery<SqliteTransactionType>(*this);

	string filter = SqliteTransactionType::NUMBER_FIELD + " >= " + first + " and " + SqliteTransactionType::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteTransactionType::NUMBER_FIELD);

	return query;
}


DBTransactionTypeI* SqliteTransactionTypesRM::findNumber(const string& number)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteTransactionType::NUMBER_FIELD, SqliteField::INTEGER, number));
	if ( findrec )
		return new SqliteTransactionType(findrec);

	return 0;
}


DBTransactionTypeI* SqliteTransactionTypesRM::findName1(const string& name)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteTransactionType::NAME1_FIELD, SqliteField::STRING, name));
	if ( findrec )
		return new SqliteTransactionType(findrec);

	return 0;
}


DBTransactionTypeI* SqliteTransactionTypesRM::findName2(const string& name)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteTransactionType::NAME2_FIELD, SqliteField::STRING, name));
	if ( findrec )
		return new SqliteTransactionType(findrec);

	return 0;
}


DBTransactionTypeI* SqliteTransactionTypesRM::findLinkedRecord(const DBTransactionI* transaction)
{
	const SqliteTransaction* inrec = dynamic_cast<const SqliteTransaction*>(transaction);

	if ( inrec )
	{
		SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteTransactionType::NUMBER_FIELD, SqliteField::INTEGER, inrec->getTransactionType()));
		if ( findrec )
			return new SqliteTransactionType(findrec);
	}

	return 0;
}


DBTransactionTypeI* SqliteTransactionTypesRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteTransactionType* createrec = new SqliteTransactionType(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteTransactionTypesRM::saveRecord(const DBTransactionTypeI* rec)
{
	const SqliteTransactionType* inrec = dynamic_cast<const SqliteTransactionType*>(rec);

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


int SqliteTransactionTypesRM::deleteRecord(const DBTransactionTypeI* rec)
{
	const SqliteTransactionType* inrec = dynamic_cast<const SqliteTransactionType*>(rec);

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





#ifdef SQLITETRANSACTIONTYPESRM_DRIVER

#include <iostream>

int main(int argc, char * argv[])
{
	unsigned long int tot;
	SqliteTransactionTypesRM prods;

	tot = prods.countAll();
	if ( tot > 0 )
	{
		SqliteTransactionTypesRM::iterator it = prods.begin();
		SqliteTransactionTypesRM::iterator end = prods.end();

		while ( it != end )
			cout << *it;
	}
}

#endif // SQLITETRANSACTIONTYPESRM_DRIVER
