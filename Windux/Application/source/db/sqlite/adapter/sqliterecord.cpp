#include "sqliterecord.h"

#include ACCOUNTANTI_INC
#include DEBUG_INC
#include CHECKSUMS_INC
#include UTILSTRING_INC

#include <algorithm>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


#ifdef USE_ACCOUNTANT
int SqliteRecord::_dataType_ = 0;
string SqliteRecord::_dataTypeName_("SqliteRecord");
#endif

SqliteField SqliteRecord::_missingField_( "MissingField" );


SqliteRecord::SqliteRecord(sqlite3_stmt *stmt, const set<string>& columnNames, const std::vector<SqliteField>& keys, bool)
		: columns_(), valid_(true), usedKeyColumns_()
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);
	util_acc::addInstance(_dataType_);
#endif

	if (keys.size())
	{
		for (int i = 0; i < (int)keys.size(); i++)
			usedKeyColumns_.push_back(keys[i]);
	}

	//keyFields_ = keys;

	if (!stmt)
	{
		if (columnNames.size())
		{
			set<string>::const_iterator iter = columnNames.begin();
			while ( iter != columnNames.end() )
			{
				columns_.insert(make_pair(*iter, SqliteField(*iter, SqliteField::STRING, "")));
				iter++;
			}
		}
	}
	else
	{
		int numFields = sqlite3_data_count(stmt);

		for ( short i = 0; i < numFields; ++i )
		{
			std::string colName = sqlite3_column_name(stmt, i);
			set<string>::const_iterator iter = columnNames.find(colName);

			if ( iter != columnNames.end() )
			{
				int colType = sqlite3_column_type(stmt, i);
				string declaredType = sqlite3_column_decltype(stmt, i);

				if (colType == SQLITE_NULL)
				{
					// The field is set to null.  Use the declaredType to set up the column, and set it to null.
					if (declaredType == SqliteField::SqliteINTEGER)
						colType = SqliteField::INTEGER;
					else if (declaredType == SqliteField::SqliteBOOLEAN)
						colType = SqliteField::INTBOOLEAN;
					else if (declaredType == SqliteField::SqliteDATETIME)
						colType = SqliteField::DATETIME;
					else  // consider everything else to be Text/String
						colType = SqliteField::STRING;

					// a SqliteField with only two parameters in the constructor, no value field as the third parameter,
					// will initialize the SqliteField as 'null'.
					columns_.insert(make_pair(colName, SqliteField(colName, (SqliteField::Type)colType)));
				}
				else if (colType == SQLITE_INTEGER)
				{
					if (declaredType == SqliteField::SqliteBOOLEAN)
						colType = SqliteField::INTBOOLEAN;
					else	
						colType = SqliteField::INTEGER;

					int data = sqlite3_column_int(stmt, i);
					columns_.insert(make_pair(colName, SqliteField(colName, (SqliteField::Type)colType, data)));
				}
				else if (colType == SQLITE_TEXT)
				{
					if (declaredType == SqliteField::SqliteDATETIME)
						colType = SqliteField::DATETIME;
					else	
						colType = SqliteField::STRING;

					char *charData = (char *)sqlite3_column_text(stmt, i);
					if (charData)  // shouldn't be null, but check anyway to be safe.
						columns_.insert(make_pair(colName, SqliteField(colName, (SqliteField::Type)colType, charData)));
					else
						columns_.insert(make_pair(colName, SqliteField(colName, (SqliteField::Type)colType)));
				}
				else
				{
					// I wasn't expecting to get anything of this type, but accept it anyway as a Text field.

					if (Debug::isTracing("database.sql"))
						Debug::trace("database.sql", "SqliteRecord::constructor - Unexpected column type - columnName: " + colName + ",  SQLite datatype: " + util_string::inttostring(colType) + ",  declaredType: " + declaredType);

					char *charData = (char *)sqlite3_column_text(stmt, i);
					if (charData)  // shouldn't be null, but check anyway to be safe.
						columns_.insert(make_pair(colName, SqliteField(colName, SqliteField::STRING, charData)));
					else
						columns_.insert(make_pair(colName, SqliteField(colName, SqliteField::STRING)));
#ifndef EVOL_NDEBUG
					Debug::trace("database.record", 20, "Invalid column type detected!!");
#endif
				}
			}
		}
	}
}


SqliteRecord::SqliteRecord(const std::map<std::string, SqliteField>& columns)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);
	util_acc::addInstance(_dataType_);
#endif

	if (columns.size())
	{
		map<std::string, SqliteField>::const_iterator iter = columns.begin();
  		while ( iter != columns.end() )
		{
			SqliteField field(iter->second);
			string colName(iter->first);
			columns_.insert(make_pair(colName, field));
			iter++;
		}
	}
}


SqliteRecord::SqliteRecord(const SqliteRecord& rec)
		: columns_(rec.columns_), valid_(rec.valid_), usedKeyColumns_(rec.usedKeyColumns_)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);
	util_acc::addInstance(_dataType_);
#endif
}


SqliteRecord::~SqliteRecord()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
	
	if( usedKeyColumns_.size() > 0 )
		usedKeyColumns_.clear();
}


const SqliteField& SqliteRecord::getField(const std::string& columnName) const
{
	container_type::const_iterator i( columns_.find(columnName) );

	if ( columns_.end() == i )
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database.record", 20, std::string("Database missing field: ") + columnName);
#endif
		return _missingField_;
	}

	// The above checks the result of the find to see if positioned at .end(), thus column not found.
	// If we don't do this check, and the column is not found, application crashes on the access of .second
	return (*i).second;
}


SqliteField& SqliteRecord::setField(const string& columnName, int value)
{
	container_type::iterator i( columns_.find(columnName) );

	if ( columns_.end() == i )
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database.record", 20, std::string("Database missing field: ") + columnName);
#endif
		return _missingField_;
	}

	SqliteField& field = (*i).second;

	field.setValue(value);

	valid_ = true;

	return field;
}


SqliteField& SqliteRecord::setField(const string& columnName, const string& value)
{
	container_type::iterator i( columns_.find(columnName) );

	if ( columns_.end() == i )
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database.record", 20, std::string("Database missing field: ") + columnName);
#endif
		return _missingField_;
	}

	SqliteField& field = (*i).second;

	field.setValue(value);

	valid_ = true;

	return field;
}


SqliteField& SqliteRecord::setFieldNull(const string& columnName)
{
	container_type::iterator i( columns_.find(columnName) );

	if ( columns_.end() == i )
	{
#ifndef EVOL_NDEBUG
		Debug::trace("database.record", 20, std::string("Database missing field: ") + columnName);
#endif
		return _missingField_;
	}

	SqliteField& field = (*i).second;

	field.setFieldNull();

	valid_ = true;
	
	return field;
}


const std::vector<SqliteField> SqliteRecord::getColumns() const
{
	std::vector<SqliteField> columns;

	container_type::const_iterator iter = columns_.begin();
	while ( iter != columns_.end() )
	{
		columns.push_back( iter->second );
		iter++;
	}

	return columns;
}


const std::vector<SqliteField> SqliteRecord::getColumns(std::vector<std::string> desiredColumnNames) const
{
	std::vector<SqliteField> columns;

	if ( desiredColumnNames.size() )
	{
		std::vector<std::string>::const_iterator iter = desiredColumnNames.begin();
		while ( iter != desiredColumnNames.end() )
			columns.push_back(getField(*iter++));
	}

	return columns;
}


const std::vector<SqliteField> SqliteRecord::getColumnsExcluding(std::vector<std::string> undesiredColumnNames) const
{
	std::vector<SqliteField> columns;

	if ( undesiredColumnNames.size() == 0 )
		return getColumns();
	else
	{
		container_type::const_iterator iter = columns_.begin();
		while ( iter != columns_.end() )
		{
			std::vector<std::string>::iterator undesiredIter = find(undesiredColumnNames.begin(), undesiredColumnNames.end(), iter->first);
			if ( undesiredIter == undesiredColumnNames.end() )
				columns.push_back( iter->second );
			iter++;
		}
	}

	return columns;
}


const std::vector<std::string> SqliteRecord::getColumnNames() const
{
	std::vector<std::string> columnNames;

	container_type::const_iterator iter = columns_.begin();
	while ( iter != columns_.end() )
	{
		columnNames.push_back( iter->first );
		iter++;
	}

	return columnNames;
}


unsigned short SqliteRecord::calculateChecksum()
{
	unsigned short check = 1;
	return check;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
