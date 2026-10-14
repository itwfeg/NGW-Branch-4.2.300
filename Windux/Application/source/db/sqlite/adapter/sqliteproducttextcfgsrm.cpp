#include <string>


#include "sqlitedatabase.h"
#include "sqliteproducttextcfgsrm.h"
#include "sqliteproducttextcfg.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteProductTextCfg>::_dataType_ = 0;
template <> string SqliteQuery<SqliteProductTextCfg>::_dataTypeName_("SqliteQuery<SqliteProductTextCfg>");

int SqliteProductTextCfgsRM::_dataType_ = 0;
string SqliteProductTextCfgsRM::_dataTypeName_("SqliteProductTextCfgsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteProductTextCfgsRM::DATA_FILENAME("PRODUCTTEXTCFGS");



SqliteProductTextCfgsRM::SqliteProductTextCfgsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductTextCfgsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteProductTextCfg::TEXTTYPE_FIELD);
	columnNames_.insert(SqliteProductTextCfg::LABELNUM_FIELD);
	columnNames_.insert(SqliteProductTextCfg::PRINTERNUM_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteProductTextCfg::TEXTTYPE_FIELD,   SqliteField(SqliteProductTextCfg::TEXTTYPE_FIELD,   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductTextCfg::LABELNUM_FIELD,   SqliteField(SqliteProductTextCfg::LABELNUM_FIELD,   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteProductTextCfg::PRINTERNUM_FIELD, SqliteField(SqliteProductTextCfg::PRINTERNUM_FIELD, SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteProductTextCfgsRM::~SqliteProductTextCfgsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBProductTextCfgI>* SqliteProductTextCfgsRM::retrieveAll()
{
	SqliteQuery<SqliteProductTextCfg>* query = new SqliteQuery<SqliteProductTextCfg>(*this);

	query->selectAll();

	return query;
}


DBProductTextCfgI* SqliteProductTextCfgsRM::findNumber(int textType)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteProductTextCfg::TEXTTYPE_FIELD, SqliteField::INTEGER, textType));
		
	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
	if ( findrec )
		return new SqliteProductTextCfg(findrec);

	return 0;
}


DBProductTextCfgI* SqliteProductTextCfgsRM::createRecord(int textType)
{
	if ( textType )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteProductTextCfg* createrec = new SqliteProductTextCfg(dbrec);

			if ( createrec )
			{
				createrec->setTextType(inttostring(textType));
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqliteProductTextCfgsRM::saveRecord(const DBProductTextCfgI* rec)
{
	const SqliteProductTextCfg* inrec = dynamic_cast<const SqliteProductTextCfg*>(rec);

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


int SqliteProductTextCfgsRM::deleteRecord(const DBProductTextCfgI* rec)
{
	const SqliteProductTextCfg* inrec = dynamic_cast<const SqliteProductTextCfg*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteProductTextCfgsRM::deleteAll()
{
	int deleted = SqliteRecordManager::deleteAll();
	return deleted;
}


int SqliteProductTextCfgsRM::countAll()
{
	int count = SqliteRecordManager::countAll();
	return count;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
