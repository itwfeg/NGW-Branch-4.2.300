#include <string>


#include "sqlitedatabase.h"
#include "sqlitesoftkeysrm.h"
#include "sqlitesoftkey.h"
#include "sqlitesoftkeygroup.h"
#include "sqliteproduct.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteSoftKey>::_dataType_ = 0;
template <> string SqliteQuery<SqliteSoftKey>::_dataTypeName_("SqliteQuery<SqliteSoftKey>");

int SqliteSoftKeysRM::_dataType_ = 0;
string SqliteSoftKeysRM::_dataTypeName_("SqliteSoftKeysRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteSoftKeysRM::DATA_FILENAME("SoftKeys");



SqliteSoftKeysRM::SqliteSoftKeysRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSoftKeysRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteSoftKey::GROUPNUMBER_FIELD);
	columnNames_.insert(SqliteSoftKey::HORIZONTALPOSITION_FIELD);
	columnNames_.insert(SqliteSoftKey::VERTICALPOSITION_FIELD);
	columnNames_.insert(SqliteSoftKey::HORIZONTALSIZE_FIELD);
	columnNames_.insert(SqliteSoftKey::VERTICALSIZE_FIELD);
	columnNames_.insert(SqliteSoftKey::BACKGROUNDCOLOR_FIELD);
	columnNames_.insert(SqliteSoftKey::TOGGLEDBACKGROUNDCOLOR_FIELD);
	columnNames_.insert(SqliteSoftKey::TEXTCOLOR_FIELD);
	columnNames_.insert(SqliteSoftKey::NAME1_FIELD);
	columnNames_.insert(SqliteSoftKey::NAME2_FIELD);
	columnNames_.insert(SqliteSoftKey::FORCENOBACKGROUNDCOLOR_FIELD);
	columnNames_.insert(SqliteSoftKey::FORCENOBACKGROUNDIMAGE_FIELD);
	columnNames_.insert(SqliteSoftKey::FORCENOTEXT_FIELD);
	columnNames_.insert(SqliteSoftKey::LINKEDTOPRODUCTIONPLANNING_FIELD);
	columnNames_.insert(SqliteSoftKey::ACTION_FIELD);
	columnNames_.insert(SqliteSoftKey::ACTIONSUBTYPE_FIELD);
	columnNames_.insert(SqliteSoftKey::ACTIONPRODUCTNUMBER_FIELD);
	columnNames_.insert(SqliteSoftKey::ACTIONINTEGERPARAMETER_FIELD);
	columnNames_.insert(SqliteSoftKey::ACTIONSTRINGPARAMETER_FIELD);
	columnNames_.insert(SqliteSoftKey::BACKGROUNDIMAGE_FIELD);
	columnNames_.insert(SqliteSoftKey::TOGGLEDBACKGROUNDIMAGE_FIELD);
	columnNames_.insert(SqliteSoftKey::ACTIONCATEGORYNUMBER_FIELD);
	columnNames_.insert(SqliteSoftKey::ACTIONOPERATORNUMBER_FIELD);
	columnNames_.insert(SqliteSoftKey::ACTIONFEATURENUMBER_FIELD);
	columnNames_.insert(SqliteSoftKey::DEPTNUM_FIELD);
	columnNames_.insert(SqliteSoftKey::SCRIPT_NAME_FIELD);
	columnNames_.insert(SqliteSoftKey::SCRIPT_INTERVAL_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteSoftKey::GROUPNUMBER_FIELD,                SqliteField(SqliteSoftKey::GROUPNUMBER_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::HORIZONTALPOSITION_FIELD,         SqliteField(SqliteSoftKey::HORIZONTALPOSITION_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::VERTICALPOSITION_FIELD,           SqliteField(SqliteSoftKey::VERTICALPOSITION_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::ACTIONCATEGORYNUMBER_FIELD,       SqliteField(SqliteSoftKey::ACTIONCATEGORYNUMBER_FIELD,       SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::ACTIONOPERATORNUMBER_FIELD,       SqliteField(SqliteSoftKey::ACTIONOPERATORNUMBER_FIELD,       SqliteField::STRING)));
	columns_.insert(make_pair(SqliteSoftKey::ACTIONFEATURENUMBER_FIELD,        SqliteField(SqliteSoftKey::ACTIONFEATURENUMBER_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::HORIZONTALSIZE_FIELD,             SqliteField(SqliteSoftKey::HORIZONTALSIZE_FIELD,             SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::VERTICALSIZE_FIELD,               SqliteField(SqliteSoftKey::VERTICALSIZE_FIELD,               SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::BACKGROUNDCOLOR_FIELD,            SqliteField(SqliteSoftKey::BACKGROUNDCOLOR_FIELD,            SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::TOGGLEDBACKGROUNDCOLOR_FIELD,     SqliteField(SqliteSoftKey::TOGGLEDBACKGROUNDCOLOR_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::TEXTCOLOR_FIELD,                  SqliteField(SqliteSoftKey::TEXTCOLOR_FIELD,                  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::NAME1_FIELD,                      SqliteField(SqliteSoftKey::NAME1_FIELD,                      SqliteField::STRING)));
	columns_.insert(make_pair(SqliteSoftKey::NAME2_FIELD,                      SqliteField(SqliteSoftKey::NAME2_FIELD,                      SqliteField::STRING)));
	columns_.insert(make_pair(SqliteSoftKey::FORCENOBACKGROUNDCOLOR_FIELD,     SqliteField(SqliteSoftKey::FORCENOBACKGROUNDCOLOR_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::FORCENOBACKGROUNDIMAGE_FIELD,     SqliteField(SqliteSoftKey::FORCENOBACKGROUNDIMAGE_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::FORCENOTEXT_FIELD,                SqliteField(SqliteSoftKey::FORCENOTEXT_FIELD,                SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::LINKEDTOPRODUCTIONPLANNING_FIELD, SqliteField(SqliteSoftKey::LINKEDTOPRODUCTIONPLANNING_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::ACTION_FIELD,                     SqliteField(SqliteSoftKey::ACTION_FIELD,                     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::ACTIONSUBTYPE_FIELD,              SqliteField(SqliteSoftKey::ACTIONSUBTYPE_FIELD,              SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::ACTIONPRODUCTNUMBER_FIELD,        SqliteField(SqliteSoftKey::ACTIONPRODUCTNUMBER_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::ACTIONINTEGERPARAMETER_FIELD,     SqliteField(SqliteSoftKey::ACTIONINTEGERPARAMETER_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::ACTIONSTRINGPARAMETER_FIELD,      SqliteField(SqliteSoftKey::ACTIONSTRINGPARAMETER_FIELD,      SqliteField::STRING)));
	columns_.insert(make_pair(SqliteSoftKey::BACKGROUNDIMAGE_FIELD,            SqliteField(SqliteSoftKey::BACKGROUNDIMAGE_FIELD,            SqliteField::STRING)));
	columns_.insert(make_pair(SqliteSoftKey::TOGGLEDBACKGROUNDIMAGE_FIELD,     SqliteField(SqliteSoftKey::TOGGLEDBACKGROUNDIMAGE_FIELD,     SqliteField::STRING)));
	columns_.insert(make_pair(SqliteSoftKey::DEPTNUM_FIELD,                    SqliteField(SqliteSoftKey::DEPTNUM_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKey::SCRIPT_NAME_FIELD,                SqliteField(SqliteSoftKey::SCRIPT_NAME_FIELD,                SqliteField::STRING)));
	columns_.insert(make_pair(SqliteSoftKey::SCRIPT_INTERVAL_FIELD,            SqliteField(SqliteSoftKey::SCRIPT_INTERVAL_FIELD,            SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteSoftKeysRM::~SqliteSoftKeysRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBSoftKeyI>* SqliteSoftKeysRM::retrieveAll()
{
	SqliteQuery<SqliteSoftKey>* query = new SqliteQuery<SqliteSoftKey>(*this);

	query->selectAll();

	return query;
}

DBQueryI<DBSoftKeyI>* SqliteSoftKeysRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteSoftKey>* query = new SqliteQuery<SqliteSoftKey>(*this);

	string filter = SqliteSoftKey::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteSoftKey::GROUPNUMBER_FIELD);

	return query;
}


DBQueryI<DBSoftKeyI>* SqliteSoftKeysRM::retrieveGroup(const std::string& groupNumber, const std::string& deptNum)
{
	SqliteQuery<SqliteSoftKey>* query = new SqliteQuery<SqliteSoftKey>(*this);

	string filter = SqliteSoftKey::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteSoftKey::GROUPNUMBER_FIELD + " = " + groupNumber;
	query->selectSome(filter, SqliteSoftKey::GROUPNUMBER_FIELD);

	return query;
}


DBQueryI<DBSoftKeyI>* SqliteSoftKeysRM::retrieveGroup(const DBSoftKeyGroupI* skg)
{
	const SqliteSoftKeyGroup* inrec = dynamic_cast<const SqliteSoftKeyGroup*>(skg);

	if ( inrec )
		return retrieveGroup(inrec->getNumber(), inrec->getDeptNumber());

	return 0;
}


DBSoftKeyI* SqliteSoftKeysRM::findSoftKey(const string& gn, const string& dn, const string& hp, const string& vp)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteSoftKey::DEPTNUM_FIELD, SqliteField::INTEGER, dn));
	keys.push_back(SqliteField(SqliteSoftKey::GROUPNUMBER_FIELD, SqliteField::INTEGER, gn));
	keys.push_back(SqliteField(SqliteSoftKey::HORIZONTALPOSITION_FIELD, SqliteField::INTEGER, hp));
	keys.push_back(SqliteField(SqliteSoftKey::VERTICALPOSITION_FIELD, SqliteField::INTEGER, vp));
		
	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteSoftKey(findrec);

	return 0;
}


DBSoftKeyI* SqliteSoftKeysRM::createRecord(const std::string& gn, const std::string& dn, const std::string& hp, const std::string& vp)
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteSoftKey* createrec = new SqliteSoftKey(dbrec);

		if ( createrec )
		{
			createrec->setGroupNumber(gn);
			createrec->setDeptNumber(dn);
			createrec->setHorizontalPosition(hp);
			createrec->setVerticalPosition(vp);
			return createrec;
		}
		else
			delete dbrec;
	}

	return 0;
}


void SqliteSoftKeysRM::saveRecord(const DBSoftKeyI* rec)
{
	const SqliteSoftKey* inrec = dynamic_cast<const SqliteSoftKey*>(rec);

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


int SqliteSoftKeysRM::deleteRecord(const DBSoftKeyI* rec)
{
	const SqliteSoftKey* inrec = dynamic_cast<const SqliteSoftKey*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteSoftKeysRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteSoftKey::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
