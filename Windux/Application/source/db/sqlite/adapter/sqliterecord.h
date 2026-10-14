#ifndef SQLITERECORD_H
#define SQLITERECORD_H


#include <string>
#include <map>
#include <set>
#include <vector>


#include "edb.h"
#include "sqlitefield.h"

#include <sqlite3.h>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteRecord
	{
		public:

			// --- Constructors and destructor ---
			SqliteRecord(sqlite3_stmt *stmt, const std::set<std::string>& columnNames, const std::vector<SqliteField>& keys = std::vector<SqliteField> (), bool loadData = true);
			SqliteRecord(const std::map<std::string, SqliteField>& columns);
			SqliteRecord(const SqliteRecord& rec);
			virtual ~SqliteRecord();

			// --- Accessor methods ---
			bool isValid() const { return valid_; }

			// --- Processing methods ---
			const SqliteField& getField(const std::string& columnName) const ;
			SqliteField& setField(const std::string& columnName, int value);
			SqliteField& setField(const std::string& columnName, const std::string& value);
			bool isFieldNull(const std::string& columnName) const { return (*columns_.find(columnName)).second.isFieldNull(); }
			SqliteField& setFieldNull(const std::string& columnName);

			const std::vector<SqliteField> getColumns() const;
			const std::vector<SqliteField> getColumns(std::vector<std::string> desiredColumnNames) const;
			const std::vector<SqliteField> getColumnsExcluding(std::vector<std::string> undesiredColumnNames) const;
			const std::vector<std::string> getColumnNames() const;

			bool save(const std::string& table, const std::string& checkField, bool newRecord = false) const;

			static SqliteField _missingField_;


		private:

			// --- Types and Subtypes ---
			typedef std::map<std::string, SqliteField> container_type;

			// --- Instance members ---
			container_type columns_;
			bool valid_;
			std::vector<SqliteField> usedKeyColumns_;

			// --- Processing methods ---
			//lap Deprecated!
			static unsigned short calculateChecksum();

			// Do not allow the default assignment operator.
			SqliteRecord operator=(const SqliteRecord& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif

	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITERECORD_H
