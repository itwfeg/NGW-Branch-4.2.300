#include <string>


#include "sqlitedatabase.h"
#include "sqlitesoftkeyfeaturesrm.h"
#include "sqlitesoftkeyfeature.h"
#include "sqlitequery.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
template <> int SqliteQuery<SqliteSoftKeyFeature>::_dataType_ = 0;
template <> string SqliteQuery<SqliteSoftKeyFeature>::_dataTypeName_("SqliteQuery<SqliteSoftKeyFeature>");

int SqliteSoftKeyFeaturesRM::_dataType_ = 0;
string SqliteSoftKeyFeaturesRM::_dataTypeName_("SqliteSoftKeyFeaturesRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteSoftKeyFeaturesRM::DATA_FILENAME("SoftKeyFeatures");



SqliteSoftKeyFeaturesRM::SqliteSoftKeyFeaturesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSoftKeyFeaturesRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteSoftKeyFeature::NUMBER_FIELD);
	columnNames_.insert(SqliteSoftKeyFeature::DEPTNUM_FIELD);
	columnNames_.insert(SqliteSoftKeyFeature::SOFTKEYSTYLENUMBER_FIELD);
	columnNames_.insert(SqliteSoftKeyFeature::DEFAULTGROUPNUMBER_FIELD);
	columnNames_.insert(SqliteSoftKeyFeature::DEFAULTHORIZONTALGRIDSIZE_FIELD);
	columnNames_.insert(SqliteSoftKeyFeature::DEFAULTVERTICALGRIDSIZE_FIELD);
	columnNames_.insert(SqliteSoftKeyFeature::DEFAULTHORIZONTALSPACING_FIELD);
	columnNames_.insert(SqliteSoftKeyFeature::DEFAULTVERTICALSPACING_FIELD);
	columnNames_.insert(SqliteSoftKeyFeature::FIXEDROWCOLMETHOD_FIELD);
	columnNames_.insert(SqliteSoftKeyFeature::DEFAULTBUTTON_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteSoftKeyFeature::NUMBER_FIELD,                    SqliteField(SqliteSoftKeyFeature::NUMBER_FIELD,                    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyFeature::DEPTNUM_FIELD,                   SqliteField(SqliteSoftKeyFeature::DEPTNUM_FIELD,                   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyFeature::SOFTKEYSTYLENUMBER_FIELD,        SqliteField(SqliteSoftKeyFeature::SOFTKEYSTYLENUMBER_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyFeature::DEFAULTGROUPNUMBER_FIELD,        SqliteField(SqliteSoftKeyFeature::DEFAULTGROUPNUMBER_FIELD,        SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyFeature::DEFAULTHORIZONTALGRIDSIZE_FIELD, SqliteField(SqliteSoftKeyFeature::DEFAULTHORIZONTALGRIDSIZE_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyFeature::DEFAULTVERTICALGRIDSIZE_FIELD,   SqliteField(SqliteSoftKeyFeature::DEFAULTVERTICALGRIDSIZE_FIELD,   SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyFeature::DEFAULTHORIZONTALSPACING_FIELD,  SqliteField(SqliteSoftKeyFeature::DEFAULTHORIZONTALSPACING_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyFeature::DEFAULTVERTICALSPACING_FIELD,    SqliteField(SqliteSoftKeyFeature::DEFAULTVERTICALSPACING_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyFeature::FIXEDROWCOLMETHOD_FIELD,         SqliteField(SqliteSoftKeyFeature::FIXEDROWCOLMETHOD_FIELD,         SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteSoftKeyFeature::DEFAULTBUTTON_FIELD,             SqliteField(SqliteSoftKeyFeature::DEFAULTBUTTON_FIELD,             SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteSoftKeyFeaturesRM::~SqliteSoftKeyFeaturesRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBSoftKeyFeatureI>* SqliteSoftKeyFeaturesRM::retrieveAll()
{
	SqliteQuery<SqliteSoftKeyFeature>* query = new SqliteQuery<SqliteSoftKeyFeature>(*this);

	query->selectAll();

	return query;
}

DBQueryI<DBSoftKeyFeatureI>* SqliteSoftKeyFeaturesRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteSoftKeyFeature>* query = new SqliteQuery<SqliteSoftKeyFeature>(*this);

	string filter = SqliteSoftKeyFeature::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteSoftKeyFeature::NUMBER_FIELD);

	return query;
}

DBQueryI<DBSoftKeyFeatureI>* SqliteSoftKeyFeaturesRM::retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum)
{
	SqliteQuery<SqliteSoftKeyFeature>* query = new SqliteQuery<SqliteSoftKeyFeature>(*this);

	string filter = SqliteSoftKeyFeature::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteSoftKeyFeature::NUMBER_FIELD + " >= " + first + " and " + SqliteSoftKeyFeature::NUMBER_FIELD + " <= " + last;
	query->selectSome(filter, SqliteSoftKeyFeature::NUMBER_FIELD);

	return query;
}


DBSoftKeyFeatureI* SqliteSoftKeyFeaturesRM::findNumber(const string& number, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteSoftKeyFeature::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteSoftKeyFeature::NUMBER_FIELD, SqliteField::INTEGER, number));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);

	if ( findrec )
		return new SqliteSoftKeyFeature(findrec);

	return 0;
}


DBSoftKeyFeatureI* SqliteSoftKeyFeaturesRM::createRecord(const std::string& number)
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteSoftKeyFeature* createrec = new SqliteSoftKeyFeature(dbrec);

		if ( createrec )
		{
			createrec->setNumber(number);
			return createrec;
		}
		else
			delete dbrec;
	}

	return 0;
}


void SqliteSoftKeyFeaturesRM::saveRecord(const DBSoftKeyFeatureI* rec)
{
	const SqliteSoftKeyFeature* inrec = dynamic_cast<const SqliteSoftKeyFeature*>(rec);

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


int SqliteSoftKeyFeaturesRM::deleteRecord(const DBSoftKeyFeatureI* rec)
{
	const SqliteSoftKeyFeature* inrec = dynamic_cast<const SqliteSoftKeyFeature*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteSoftKeyFeaturesRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteSoftKeyFeature::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
