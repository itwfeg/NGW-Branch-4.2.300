#include <string>


#include "sqliteview.h"
#include "sqlitefield.h"

#include ACCOUNTANTI_INC
#include DBEXCEPTION_INC
#include UTILSTRING_INC
#include DEBUG_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
	using namespace util_filesystem;
	using namespace util_string;
#endif


#ifdef USE_ACCOUNTANT
int SqliteView::_dataType_ = 0;
string SqliteView::_dataTypeName_("SqliteView");
#endif

Util::CSmartCriticalSection SqliteView::sqliteViewCS;



SqliteView::SqliteView(sqlite3* s3, const util_env::Properties& props, const string& aliasName, bool openTable)
		: db_(s3), columnNames_(), dbTransactionsEnabled_(false), tableFilename_(getTableFilename(props, aliasName)),
		  primaryKey_(), primaryKeyColumnNames_(), strictTagMatching_(false), checkField_(), columns_(),
		  config_(props), tableAlias_(aliasName), singleRecord_(false), readOnly_(false), compoundPrimaryKey_(false),  primaryIndex_(), throwOnUnlockFailure_(true)
{
	const string* property;

#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

#ifndef EVOL_NDEBUG
	Debug::trace("database.table.create", 28, "SqliteView: Creating table for " + tableFilename_ + " (Usual Constructor)");
#endif


	// Determine if database transactions are being used
	property = getDbProperty("dbTransactionsEnabled");
	if ( property )
		dbTransactionsEnabled_ = stringtobool(*property);

	// Determine table primary key
	property = getTableProperty("primaryKey");
	if ( property )
	{
		primaryKey_.assign(*property);

		// 'getTableMultiProperty' should not return null since 'getTableProperty' didn't.
		primaryKeyColumnNames_ = *getTableMultiProperty("primaryKey");
		if ( primaryKeyColumnNames_.size() > 1 )
			compoundPrimaryKey_ = true;
	}

	// Determine table primary index
	property = getTableProperty("primaryIndex");
	if ( property )
		primaryIndex_.assign(*property);

	// Determine, through properties, whether the primary index uses strict matching
	property = getIndexProperty(getPrimaryIndex(), "strictMatching");
	if ( property )
	{
		strictTagMatching_ = stringtobool(*property);

#ifndef EVOL_NDEBUG
		Debug::trace("database.table", 24, (strictTagMatching_ ? "Using" : "Not using") +
				string(" strict matching for primary index in table ") + getTableAlias());
#endif
	}

	// Determine table check field
	property = getTableProperty("checkField");
	if ( property )
		checkField_.assign(*property);

	// Determine table single record condition
	property = getTableProperty("singleRecord");
	if ( property )
		singleRecord_ = util_string::stringtobool(*property);

	// Determine table readOnly
	property = getTableProperty("readOnly");
	if ( property )
		readOnly_ = util_string::stringtobool(*property);

	// Determine throw on unlock failure behavior
	property = getTableProperty("throwOnUnlockFailure");
	if ( property )
		throwOnUnlockFailure_ = util_string::stringtobool(*property);

	if ( openTable )
	{
		open();
#ifndef EVOL_NDEBUG
		Debug::trace("database.table.create", 28, "SqliteView: Created table for " + tableFilename_ + " (Usual Constructor)");
#endif
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace("database.table.create", 28, "SqliteView: Created table for " + tableFilename_ + " (Usual Constructor). Did not open.");
#endif
}


SqliteView::SqliteView(const SqliteView& view)
		: db_(view.db_), columnNames_(view.columnNames_), dbTransactionsEnabled_(view.dbTransactionsEnabled_), tableFilename_(view.tableFilename_),
		  primaryKey_(view.primaryKey_), primaryKeyColumnNames_(view.primaryKeyColumnNames_), strictTagMatching_(view.strictTagMatching_), checkField_(view.checkField_), columns_(view.columns_),
		  config_(view.config_), tableAlias_(view.tableAlias_), singleRecord_(view.singleRecord_), readOnly_(view.readOnly_), compoundPrimaryKey_(view.compoundPrimaryKey_),
		  primaryIndex_(view.primaryIndex_), throwOnUnlockFailure_(view.throwOnUnlockFailure_)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

#ifndef EVOL_NDEBUG
	Debug::trace("database.table.create", 28, "SqliteView: Creating table for " + tableFilename_ + " (Copy Constructor)");
#endif

	// Open the database file
	open(false);

#ifndef EVOL_NDEBUG
	Debug::trace("database.table.create", 28, "SqliteView: Created table for " + tableFilename_ + " (Copy Constructor)");
#endif
}


SqliteView::~SqliteView()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	columnNames_.clear();
}


string SqliteView::generateWhereClause(const vector<SqliteField>& keys) const
{
	if ( keys.size() == 0 )
		return "";
	
	string where(" where ");
	for (int i = 0; i < (int)keys.size(); i++)
	{
		if (i>0)
			where.append(" and ");
		where.append(keys[i].getName()).append(" = ");
		if (keys[i].getValueType() == SqliteField::STRING)
			where.append("'").append(keys[i]).append("'");
		else
			where.append(inttostring(keys[i]));
	}
	where.append(" ");
	return where;
}


string SqliteView::generateColumnNamesList(const SqliteRecord* rec) const
{
	if ( !rec )
		return "";
	
	vector<string> columnNames = rec->getColumnNames();
	if (columnNames.size() == 0)
		return "";

	string colNameList(" ( ");
	for (int i = 0; i < (int)columnNames.size(); i++)
	{
		if (i>0)
			colNameList.append(", ");
		colNameList.append(columnNames[i]);
	}
	colNameList.append(") ");
	return colNameList;
}


string SqliteView::generateColumnValuesList(const SqliteRecord* rec) const
{
	if ( !rec )
		return "";
	
	vector<string> columnNames = rec->getColumnNames();
	if (columnNames.size() == 0)
		return "";

	string colValueList(" ( ");
	for (int i = 0; i < (int)columnNames.size(); i++)
	{
		if (i>0)
			colValueList.append(", ");
		const SqliteField col = rec->getField(columnNames[i]);
		if (col.isFieldNull())
			colValueList.append("NULL");
		else if (col.getValueType() == SqliteField::STRING || col.getValueType() == SqliteField::DATETIME)
		{
			string duped = col;
			if (duped.find_first_of('\'') != string::npos)
				util_string::duplicateCharInString(duped, '\'');
			colValueList.append("'").append(duped).append("'");
		}
		else
			colValueList.append(inttostring(col));
	}
	colValueList.append(") ");
	return colValueList;
}

string SqliteView::generateSetClauseList(const SqliteRecord* rec) const
{
	vector<SqliteField> nonKeyColumns = rec->getColumnsExcluding(primaryKeyColumnNames_);

	if ( nonKeyColumns.size() == 0 )
		return "";

	string setClause;
	for (int i = 0; i < (int)nonKeyColumns.size(); i++)
	{
		if (i>0)
			setClause.append(", ");
		setClause.append(nonKeyColumns[i].getName()).append(" = ");
		if (nonKeyColumns[i].isFieldNull())
			setClause.append("NULL");
		else if (nonKeyColumns[i].getValueType() == SqliteField::STRING || nonKeyColumns[i].getValueType() == SqliteField::DATETIME)
		{
			string duped = nonKeyColumns[i];
			if (duped.find_first_of('\'') != string::npos)
				util_string::duplicateCharInString(duped, '\'');
			setClause.append("'").append(duped).append("'");
		}
		else
			setClause.append(inttostring(nonKeyColumns[i]));
	}
	setClause.append(" ");
	return setClause;
}


const string SqliteView::getPrimaryKeyString() const
{
	if ( isCompoundPrimaryKey() )
	{
		string pkString;
		for (size_t i = 0; i < primaryKeyColumnNames_.size(); i++)
		{
			if (i)
				pkString.append(", ");
			pkString.append(primaryKeyColumnNames_[i]);
		}
		return pkString;
	}
	else
		return primaryKey_;
}

const string SqliteView::getPrimaryKeyValue(const SqliteRecord* rec) const
{
	vector<SqliteField> keys;

	if ( isCompoundPrimaryKey() )
	{
		// The assumption is that the compound primary key is defined as appending the
		// individual keys separated by "|".
		vector<string>::const_iterator iter = primaryKeyColumnNames_.begin();
		while ( iter != primaryKeyColumnNames_.end() )
			keys.push_back(rec->getField(*iter++));
	}
	else
		keys.push_back(rec->getField(primaryKey_));

	string pkWhereClause = generateWhereClause(keys);
	keys.clear();
	return pkWhereClause;
}


void SqliteView::open(bool lookupFields)
{
	if ( tableFilename_.size() )
	{
		if ( lookupFields )
		{
#ifndef EVOL_NDEBUG
			Debug::trace("database.table.open", 28, "SqliteView: going to read table fields");
#endif

			// Determine the table's fields
/* lap todo
			int numFields = table_->numFields();
			for ( short i = 1; i <= numFields; ++i )
			{
				Field4 f(*table_, i);
				columnNames_.insert(f.name());
			}
*/

#ifndef EVOL_NDEBUG
			Debug::trace("database.table.open", 28, "SqliteView: read table fields");
#endif
		}
	}
}


void SqliteView::close()
{
}


void SqliteView::show()
{
#ifndef EVOL_NDEBUG
	Debug::trace("database.show", 20, " SQLite instance: " + ptrtostring(db_));
	Debug::trace("database.show", 20, "  Table filename: (name is " + getTableName() + " and alias is " + getTableAlias() + ")");
	Debug::trace("database.show", 20, string("  It ") + (isSingleRecord() ? "is" : "is not") + " a single-record table.");
	Debug::trace("database.show", 20, string("  It ") + (isReadOnly() ? "is" : "is not") + " a read-only table.");
	Debug::trace("database.show", 20, "  Primary Key: " + getPrimaryKey() + " (it " + (isCompoundPrimaryKey() ? "is" : "is not") + " compound).");
	Debug::trace("database.show", 20, "  Check Field: " + getCheckField() + ".");
#endif
}


string SqliteView::getTableFilename(const util_env::Properties& props, const string& aliasName)
{
	const string* property = props.getProperty("db.SQLite.Tables." + aliasName);
	if ( property )
		return *property;

	return aliasName;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
