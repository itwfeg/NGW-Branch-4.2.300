#include <string>


#include "sqlitedatabase.h"
#include "sqliteexpirationlabelsrm.h"
#include "sqliteexpirationlabel.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteExpirationLabel>::_dataType_ = 0;
template <> string SqliteQuery<SqliteExpirationLabel>::_dataTypeName_("SqliteQuery<SqliteExpirationLabel>");

int SqliteExpirationLabelsRM::_dataType_ = 0;
string SqliteExpirationLabelsRM::_dataTypeName_("SqliteExpirationLabelsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteExpirationLabelsRM::DATA_FILENAME("ExpirationLabels");



SqliteExpirationLabelsRM::SqliteExpirationLabelsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBExpirationLabelsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteExpirationLabel::SEQUENCENUMBER_FIELD);
	columnNames_.insert(SqliteExpirationLabel::DEPTNUM_FIELD);
	columnNames_.insert(SqliteExpirationLabel::PLU_FIELD);
	columnNames_.insert(SqliteExpirationLabel::PRODUCTDESCRIPTION_FIELD);
	columnNames_.insert(SqliteExpirationLabel::EXPIRATION_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteExpirationLabel::SEQUENCENUMBER_FIELD,     SqliteField(SqliteExpirationLabel::SEQUENCENUMBER_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteExpirationLabel::DEPTNUM_FIELD,            SqliteField(SqliteExpirationLabel::DEPTNUM_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteExpirationLabel::PLU_FIELD,                SqliteField(SqliteExpirationLabel::PLU_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteExpirationLabel::PRODUCTDESCRIPTION_FIELD, SqliteField(SqliteExpirationLabel::PRODUCTDESCRIPTION_FIELD, SqliteField::STRING)));
	columns_.insert(make_pair(SqliteExpirationLabel::EXPIRATION_FIELD,         SqliteField(SqliteExpirationLabel::EXPIRATION_FIELD,         SqliteField::DATETIME)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteExpirationLabelsRM::~SqliteExpirationLabelsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBExpirationLabelI>* SqliteExpirationLabelsRM::retrieveAll()
{
	SqliteQuery<SqliteExpirationLabel>* query = new SqliteQuery<SqliteExpirationLabel>(*this);

	// Sort the expiration labels by Date Time
	query->selectAll(SqliteExpirationLabel::EXPIRATION_FIELD);

	return query;
}


DBQueryI<DBExpirationLabelI>* SqliteExpirationLabelsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteExpirationLabel>* query = new SqliteQuery<SqliteExpirationLabel>(*this);

	string filter = SqliteExpirationLabel::DEPTNUM_FIELD + " = " + deptNum;
	// SOF-1525 Expiration History no longer sorts by number of days remaining.
	//query->selectSome(filter, SqliteExpirationLabel::SEQUENCENUMBER_FIELD);
	query->selectSome(filter, SqliteExpirationLabel::EXPIRATION_FIELD);

	return query;
}

DBQueryI<DBExpirationLabelI>* SqliteExpirationLabelsRM::retrieveLinkedToPLU(const std::string& plu, const std::string& deptNum)
{
	SqliteQuery<SqliteExpirationLabel>* query = new SqliteQuery<SqliteExpirationLabel>(*this);

	string filter = SqliteExpirationLabel::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteExpirationLabel::PLU_FIELD + " = " + plu;
	query->selectSome(filter, SqliteExpirationLabel::SEQUENCENUMBER_FIELD);   // lap - Why by sequence number field (ID field)?  Why not by date/time?
	
	return query;
}


DBExpirationLabelI* SqliteExpirationLabelsRM::find(const string& seq, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteExpirationLabel::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteExpirationLabel::SEQUENCENUMBER_FIELD, SqliteField::INTEGER, seq));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);

	if ( findrec )
		return new SqliteExpirationLabel(findrec);

	return 0;
}


DBExpirationLabelI* SqliteExpirationLabelsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteExpirationLabel* createrec = new SqliteExpirationLabel(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteExpirationLabelsRM::saveRecord(const DBExpirationLabelI* rec)
{
	const SqliteExpirationLabel* inrec = dynamic_cast<const SqliteExpirationLabel*>(rec);

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


int SqliteExpirationLabelsRM::deleteRecord(const DBExpirationLabelI* rec)
{
	const SqliteExpirationLabel* inrec = dynamic_cast<const SqliteExpirationLabel*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteExpirationLabelsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteExpirationLabel::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif






#ifdef SQLITEEXPIRATIONLABELSRM_DRIVER

#include <iostream>

int main(int argc, char * argv[])
{
	unsigned long int tot;
	SqliteExpirationLabelsRM prods;

	tot = prods.countAll();
	if ( tot > 0 )
	{
		SqliteExpirationLabelsRM::iterator it = prods.begin();
		SqliteExpirationLabelsRM::iterator end = prods.end();

		while ( it != end )
			cout << *it;
	}
}

#endif // SQLITEEXPIRATIONLABELSRM_DRIVER
