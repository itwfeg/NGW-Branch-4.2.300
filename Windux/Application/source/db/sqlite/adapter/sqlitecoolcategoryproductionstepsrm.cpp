#include <string>


#include "sqlitedatabase.h"
#include "sqlitecoolcategoryproductionstepsrm.h"
#include "sqlitecoolcategoryproductionstep.h"
#include "sqlitecoolproductionstep.h"
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
template <> int SqliteQuery<SqliteCoolCategoryProductionStep>::_dataType_ = 0;
template <> string SqliteQuery<SqliteCoolCategoryProductionStep>::_dataTypeName_("SqliteQuery<SqliteCoolCategoryProductionStep>");

int SqliteCoolCategoryProductionStepsRM::_dataType_ = 0;
string SqliteCoolCategoryProductionStepsRM::_dataTypeName_("SqliteCoolCategoryProductionStepsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCoolCategoryProductionStepsRM::DATA_FILENAME("CoolCategoryProductionSteps");



SqliteCoolCategoryProductionStepsRM::SqliteCoolCategoryProductionStepsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolCategoryProductionStepsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCoolCategoryProductionStep::PRODUCTIONSTEPKEY_FIELD);
	columnNames_.insert(SqliteCoolCategoryProductionStep::CATEGORYNUMBER_FIELD);
	columnNames_.insert(SqliteCoolCategoryProductionStep::DEPTNUM_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCoolCategoryProductionStep::PRODUCTIONSTEPKEY_FIELD, SqliteField(SqliteCoolCategoryProductionStep::PRODUCTIONSTEPKEY_FIELD, SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolCategoryProductionStep::CATEGORYNUMBER_FIELD,    SqliteField(SqliteCoolCategoryProductionStep::CATEGORYNUMBER_FIELD,    SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolCategoryProductionStep::DEPTNUM_FIELD,           SqliteField(SqliteCoolCategoryProductionStep::DEPTNUM_FIELD,           SqliteField::INTEGER)));

	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCoolCategoryProductionStepsRM::~SqliteCoolCategoryProductionStepsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCoolCategoryProductionStepI>* SqliteCoolCategoryProductionStepsRM::retrieveAll()
{
	SqliteQuery<SqliteCoolCategoryProductionStep>* query = new SqliteQuery<SqliteCoolCategoryProductionStep>(*this);

	query->selectAll();

	return query;
}

DBQueryI<DBCoolCategoryProductionStepI>* SqliteCoolCategoryProductionStepsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolCategoryProductionStep>* query = new SqliteQuery<SqliteCoolCategoryProductionStep>(*this);

	string filter = SqliteCoolCategoryProductionStep::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteCoolCategoryProductionStep::DEPTNUM_FIELD);

	return query;
}

DBQueryI<DBCoolCategoryProductionStepI>* SqliteCoolCategoryProductionStepsRM::retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum)
{
	if ( category.size() )
	{
		SqliteQuery<SqliteCoolCategoryProductionStep>* query = new SqliteQuery<SqliteCoolCategoryProductionStep>(*this);

		string filter = SqliteCoolCategoryProductionStep::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCoolCategoryProductionStep::CATEGORYNUMBER_FIELD + " = " + category;
		query->selectSome(filter);

		return query;
	}

	return 0;
}


DBQueryI<DBCoolCategoryProductionStepI>* SqliteCoolCategoryProductionStepsRM::retrieveLinkedtoCoolProductionStep(const DBCoolProductionStepI* step)
{
	const SqliteCoolProductionStep* inrec = dynamic_cast<const SqliteCoolProductionStep*>(step);

	if ( inrec )
	{
		SqliteQuery<SqliteCoolCategoryProductionStep>* query = new SqliteQuery<SqliteCoolCategoryProductionStep>(*this);

		string filter = SqliteCoolCategoryProductionStep::DEPTNUM_FIELD + " = " + inrec->getDeptNumber() + " and " + SqliteCoolCategoryProductionStep::PRODUCTIONSTEPKEY_FIELD + " = " + inrec->getProductionStepKey();
		query->selectSome(filter);

		return query;
	}

	return 0;
}


DBCoolCategoryProductionStepI* SqliteCoolCategoryProductionStepsRM::find(const std::string& category, const std::string& deptNum,  const DBCoolProductionStepI* step)
{
	const SqliteCoolProductionStep* scps = dynamic_cast<const SqliteCoolProductionStep*>(step);

	if ( category.size() && scps )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteCoolCategoryProductionStep::PRODUCTIONSTEPKEY_FIELD, SqliteField::INTEGER, scps->getProductionStepKey()));
		keys.push_back(SqliteField(SqliteCoolCategoryProductionStep::CATEGORYNUMBER_FIELD, SqliteField::INTEGER, category));
		keys.push_back(SqliteField(SqliteCoolCategoryProductionStep::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));

		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
		if ( findrec )
			return new SqliteCoolCategoryProductionStep(findrec);
	}

	return 0;
}


DBCoolCategoryProductionStepI* SqliteCoolCategoryProductionStepsRM::createRecord(const std::string& category, const DBCoolProductionStepI* step)
{
	const SqliteCoolProductionStep* inrec = dynamic_cast<const SqliteCoolProductionStep*>(step);

	if ( category.size() && inrec )
	{
		SqliteRecord* dbrec = SqliteRecordManager::create();

		if ( dbrec )
		{
			SqliteCoolCategoryProductionStep* createrec = new SqliteCoolCategoryProductionStep(dbrec);

			if ( createrec )
			{
				createrec->setProductionStepKey(inrec->getProductionStepKey());
				createrec->setDeptNumber(inrec->getDeptNumber());
				createrec->setCategoryNumber(category);
				return createrec;
			}
			else
				delete dbrec;
		}
	}

	return 0;
}


void SqliteCoolCategoryProductionStepsRM::saveRecord(const DBCoolCategoryProductionStepI* rec)
{
	const SqliteCoolCategoryProductionStep* inrec = dynamic_cast<const SqliteCoolCategoryProductionStep*>(rec);

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


int SqliteCoolCategoryProductionStepsRM::deleteRecord(const DBCoolCategoryProductionStepI* rec)
{
	const SqliteCoolCategoryProductionStep* inrec = dynamic_cast<const SqliteCoolCategoryProductionStep*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolCategoryProductionStepsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteCoolCategoryProductionStep::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
