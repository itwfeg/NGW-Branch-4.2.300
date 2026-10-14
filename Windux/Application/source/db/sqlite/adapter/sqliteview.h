#ifndef SQLITEVIEW_H
#define SQLITEVIEW_H


#include <string>
#include <set>


#include "edb.h"
#include "sqliterecord.h"
#include <sqlite3.h>

#include PROPERTIES_INC
#include FILENAME_INC

#include "CSmartPtr.h"
#include "CCriticalSection.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



class SqliteView
{
	public:

		// --- Constructors and destructor ---
		SqliteView(sqlite3* s3, const util_env::Properties& props, const std::string& aliasName, bool openTable = true);
		SqliteView(const SqliteView& view);
		virtual ~SqliteView();


		// --- Accessor methods ---
		bool isDbTransactionsEnabled() const { return dbTransactionsEnabled_; }
		bool isSingleRecord() const { return singleRecord_; }
		bool isReadOnly() const { return readOnly_; }
		bool isCompoundPrimaryKey() const { return compoundPrimaryKey_; }
		const std::string& getTableAlias() const { return tableAlias_; }
		const std::string& getTableName() const { return tableFilename_; }
		const std::string& getPrimaryKey() const { return primaryKey_; }
		const std::string& getPrimaryIndex() const { return primaryIndex_; }
		const std::string& getCheckField() const { return checkField_; }
		const std::vector<std::string>& getPrimaryKeyColumnNames() const { return primaryKeyColumnNames_; }


	protected:

		sqlite3* db_;
		std::set<std::string> columnNames_;              // names of columns
		bool dbTransactionsEnabled_;

		std::string tableFilename_;                      // used to query properties file, as well as 
		std::string primaryKey_;
		std::vector<std::string> primaryKeyColumnNames_;       // Name of primary key column(s)
		bool strictTagMatching_;
		std::string checkField_;

		typedef std::map<std::string, SqliteField> container_type;
		container_type columns_;

		// keyValue is used only for the purpose of returning the value of the key field
		// through the 'getPrimaryKeyValue' method. It is mutable because its value does
		// not add anything to the object (i.e., it is ok to modify this value even if the
		// object is const).
		mutable std::string keyValue_;

		static Util::CSmartCriticalSection sqliteViewCS;		


		// --- Processing methods ---
		const std::string* getDbProperty(const std::string& property) const
				{ return config_.getProperty("db.SQLite." + property); }
		const std::string* getTableProperty(const std::string& property) const
				{ return config_.getProperty("db.SQLite." + tableFilename_ + "." + property); }
		const std::vector<std::string>* getTableMultiProperty(const std::string& property) const
				{ return config_.getMultiProperty("db.SQLite." + tableFilename_ + "." + property); }
		const std::string* getIndexProperty(const std::string& index, const std::string& property) const
				{ return config_.getProperty("db.SQLite." + tableFilename_ + "." + index + "." + property); }
		const std::string* getFieldProperty(const std::string& field, const std::string& property) const
				{ return config_.getProperty("db.SQLite." + tableFilename_ + "." + field + "." + property); }

		void open(bool lookupFields = true);
		void close();
		void show();

		std::string generateWhereClause(const std::vector<SqliteField>& keys) const;
		std::string generateColumnNamesList(const SqliteRecord* rec) const;
		std::string generateColumnValuesList(const SqliteRecord* rec) const;
		std::string generateSetClauseList(const SqliteRecord* rec) const;

		const std::string getPrimaryKeyString() const;
		const std::string getPrimaryKeyValue(const SqliteRecord* rec) const;


	private:

		const util_env::Properties& config_;
		std::string tableAlias_;
		bool singleRecord_;
		bool readOnly_;              // for read-only tables (like ROMLabels)
		bool compoundPrimaryKey_;
		std::string primaryIndex_;
		bool throwOnUnlockFailure_;

		static std::string getTableFilename(const util_env::Properties& props, const std::string& name);  /* (SJJC port 6/19/06) */


		// Do not allow the default assignment operator.
		SqliteView operator=(const SqliteView& rhs);


		// static Util::CSmartCriticalSection sqliteViewCS;		


		// --- Class Members ---
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEVIEW_H
