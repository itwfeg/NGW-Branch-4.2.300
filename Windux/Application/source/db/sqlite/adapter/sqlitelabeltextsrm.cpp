#include <string>


#include "sqlitedatabase.h"
#include "sqlitelabeltextsrm.h"

#include "sqlitelabeltext.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteLabelText>::_dataType_ = 0;
template <> string SqliteQuery<SqliteLabelText>::_dataTypeName_("SqliteQuery<SqliteLabelText>");

int SqliteLabelTextsRM::_dataType_ = 0;
string SqliteLabelTextsRM::_dataTypeName_("SqliteLabelTextsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteLabelTextsRM::DATA_FILENAME("LabelTexts");



SqliteLabelTextsRM::SqliteLabelTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBLabelTextsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteLabelText::NUMBER_FIELD);
	columnNames_.insert(SqliteLabelText::TEXT_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteLabelText::NUMBER_FIELD, SqliteField(SqliteLabelText::NUMBER_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteLabelText::TEXT_FIELD,   SqliteField(SqliteLabelText::TEXT_FIELD,   SqliteField::STRING)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteLabelTextsRM::~SqliteLabelTextsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBLabelTextI>* SqliteLabelTextsRM::retrieveAll()
{
	SqliteQuery<SqliteLabelText>* query = new SqliteQuery<SqliteLabelText>(*this);

	query->selectAll();

	return query;
}


DBQueryI<DBLabelTextI>* SqliteLabelTextsRM::retrieveRangeNumber(const std::string& first, const std::string& last)
{
	SqliteQuery<SqliteLabelText>* query = new SqliteQuery<SqliteLabelText>(*this);

	string filter = SqliteLabelText::NUMBER_FIELD + " >= " + first + " and " + SqliteLabelText::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteLabelText::NUMBER_FIELD);

	return query;
}


DBLabelTextI* SqliteLabelTextsRM::findNumber(const string& number)
{
	SqliteRecord* findrec = SqliteRecordManager::findKey(SqliteField(SqliteLabelText::NUMBER_FIELD, SqliteField::INTEGER, number));
	if ( findrec )
		return new SqliteLabelText(findrec);

	return 0;
}


DBLabelTextI* SqliteLabelTextsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteLabelText* createrec = new SqliteLabelText(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteLabelTextsRM::saveRecord(const DBLabelTextI* rec)
{
	const SqliteLabelText* inrec = dynamic_cast<const SqliteLabelText*>(rec);

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


int SqliteLabelTextsRM::deleteRecord(const DBLabelTextI* rec)
{
	const SqliteLabelText* inrec = dynamic_cast<const SqliteLabelText*>(rec);

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





#ifdef SQLITELABELTEXTSRM_DRIVER

#include <iostream>

int main(int argc, char * argv[])
{
	unsigned long int tot;
	SqliteLabelTextsRM prods;

	tot = prods.countAll();
	if ( tot > 0 )
	{
		SqliteLabelTextsRM::iterator it = prods.begin();
		SqliteLabelTextsRM::iterator end = prods.end();

		while ( it != end )
			cout << *it;
	}
}

#endif // SQLITELABELTEXTSRM_DRIVER
