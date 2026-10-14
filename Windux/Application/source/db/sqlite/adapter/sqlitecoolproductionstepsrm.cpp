#include <string>


#include "sqlitedatabase.h"
#include "sqlitecoolproductionstepsrm.h"
#include "sqlitecoolproductionstep.h"
#include "sqlitecoolcategoryproductionstep.h"
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
template <> int SqliteQuery<SqliteCoolProductionStep>::_dataType_ = 0;
template <> string SqliteQuery<SqliteCoolProductionStep>::_dataTypeName_("SqliteQuery<SqliteCoolProductionStep>");

int SqliteCoolProductionStepsRM::_dataType_ = 0;
string SqliteCoolProductionStepsRM::_dataTypeName_("SqliteCoolProductionStepsRM");
#endif


// The filename needs to match the way it is spelled, including case, in the defaults.ini file.
const string SqliteCoolProductionStepsRM::DATA_FILENAME("CoolProductionSteps");



SqliteCoolProductionStepsRM::SqliteCoolProductionStepsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolProductionStepsRMI* d)
		: SqliteRecordManager(sdb, props, DATA_FILENAME), sdb_(sdb), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	columnNames_.insert(SqliteCoolProductionStep::PRODUCTIONSTEPID_FIELD);
	columnNames_.insert(SqliteCoolProductionStep::DEPTNUM_FIELD);
	columnNames_.insert(SqliteCoolProductionStep::NAME1_FIELD);
	columnNames_.insert(SqliteCoolProductionStep::NAME2_FIELD);
	columnNames_.insert(SqliteCoolProductionStep::ALLCATEGORIES_FIELD);
	columnNames_.insert(SqliteCoolProductionStep::PRODUCTIONSTEPNUM_FIELD);
	if (checkField_.size())
		columnNames_.insert(checkField_);

	columns_.insert(make_pair(SqliteCoolProductionStep::PRODUCTIONSTEPID_FIELD,  SqliteField(SqliteCoolProductionStep::PRODUCTIONSTEPID_FIELD,  SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolProductionStep::DEPTNUM_FIELD,           SqliteField(SqliteCoolProductionStep::DEPTNUM_FIELD,           SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolProductionStep::NAME1_FIELD,             SqliteField(SqliteCoolProductionStep::NAME1_FIELD,             SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCoolProductionStep::NAME2_FIELD,             SqliteField(SqliteCoolProductionStep::NAME2_FIELD,             SqliteField::STRING)));
	columns_.insert(make_pair(SqliteCoolProductionStep::ALLCATEGORIES_FIELD,     SqliteField(SqliteCoolProductionStep::ALLCATEGORIES_FIELD,     SqliteField::INTEGER)));
	columns_.insert(make_pair(SqliteCoolProductionStep::PRODUCTIONSTEPNUM_FIELD, SqliteField(SqliteCoolProductionStep::PRODUCTIONSTEPNUM_FIELD, SqliteField::INTEGER)));
	if (checkField_.size())
		columns_.insert(make_pair(checkField_, SqliteField(checkField_, SqliteField::INTEGER, 1)));
}


SqliteCoolProductionStepsRM::~SqliteCoolProductionStepsRM()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( decorated_ )
		delete decorated_;
}


DBQueryI<DBCoolProductionStepI>* SqliteCoolProductionStepsRM::retrieveAll()
{
	SqliteQuery<SqliteCoolProductionStep>* query = new SqliteQuery<SqliteCoolProductionStep>(*this);

	query->selectAll();

	return query;
}

DBQueryI<DBCoolProductionStepI>* SqliteCoolProductionStepsRM::retrieveAllForDept(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolProductionStep>* query = new SqliteQuery<SqliteCoolProductionStep>(*this);

	string filter = SqliteCoolProductionStep::DEPTNUM_FIELD + " = " + deptNum;
	query->selectSome(filter, SqliteCoolProductionStep::DEPTNUM_FIELD);

	return query;
}

DBQueryI<DBCoolProductionStepI>* SqliteCoolProductionStepsRM::retrieveAllCategories(const std::string& deptNum)
{
	SqliteQuery<SqliteCoolProductionStep>* query = new SqliteQuery<SqliteCoolProductionStep>(*this);

	string filter = SqliteCoolProductionStep::DEPTNUM_FIELD + " = " + deptNum + " and " + SqliteCoolProductionStep::ALLCATEGORIES_FIELD + " = 1";
	query->selectSome(filter, SqliteCoolProductionStep::NAME1_FIELD);

	return query;
}


DBCoolProductionStepI* SqliteCoolProductionStepsRM::findProductionStep(const string& name, const std::string& deptNum)
{
	std::vector<SqliteField> keys;
	keys.push_back(SqliteField(SqliteCoolProductionStep::DEPTNUM_FIELD, SqliteField::INTEGER, deptNum));
	keys.push_back(SqliteField(SqliteCoolProductionStep::NAME1_FIELD, SqliteField::STRING, name));

	SqliteRecord* findrec = SqliteRecordManager::findKey(keys);

	if ( findrec )
		return new SqliteCoolProductionStep(findrec);

	return 0;
}


DBCoolProductionStepI* SqliteCoolProductionStepsRM::find(const DBCoolCategoryProductionStepI* step)
{
	const SqliteCoolCategoryProductionStep* inrec = dynamic_cast<const SqliteCoolCategoryProductionStep*>(step);

	if ( inrec )
	{
		std::vector<SqliteField> keys;
		keys.push_back(SqliteField(SqliteCoolProductionStep::DEPTNUM_FIELD, SqliteField::INTEGER, inrec->getDeptNumber()));
		keys.push_back(SqliteField(SqliteCoolProductionStep::PRODUCTIONSTEPID_FIELD, SqliteField::INTEGER, inrec->getProductionStepKey()));

		SqliteRecord* findrec = SqliteRecordManager::findKey(keys);
		if ( findrec )
			return new SqliteCoolProductionStep(findrec);
	}

	return 0;
}


DBCoolProductionStepI* SqliteCoolProductionStepsRM::createRecord()
{
	SqliteRecord* dbrec = SqliteRecordManager::create();

	if ( dbrec )
	{
		SqliteCoolProductionStep* createrec = new SqliteCoolProductionStep(dbrec);

		if ( createrec )
			return createrec;
		else	
			delete dbrec;
	}

	return 0;
}


void SqliteCoolProductionStepsRM::saveRecord(const DBCoolProductionStepI* rec)
{
	const SqliteCoolProductionStep* inrec = dynamic_cast<const SqliteCoolProductionStep*>(rec);

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


int SqliteCoolProductionStepsRM::deleteRecord(const DBCoolProductionStepI* rec)
{
	const SqliteCoolProductionStep* inrec = dynamic_cast<const SqliteCoolProductionStep*>(rec);

	if ( inrec )
	{
		int deleted = SqliteRecordManager::markDeleted(inrec->getRecord());
		return deleted;
	}
	else
		throw edb::DBException(edb::DBException::SQLITE_LAYERING_ERROR, "Current record is not a SQLite layer record.");
}


int SqliteCoolProductionStepsRM::deleteAllForDept(const std::string& deptNum)
{
	return SqliteRecordManager::deleteAll(std::string("where ") + SqliteCoolProductionStep::DEPTNUM_FIELD + " = " + deptNum);
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
