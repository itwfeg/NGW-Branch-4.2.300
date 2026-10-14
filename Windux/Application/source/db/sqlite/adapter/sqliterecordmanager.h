#ifndef SQLITERECORDMANAGER_H
#define SQLITERECORDMANAGER_H


#include <string>
#include <vector>


#include "edb.h"
#include "sqliteview.h"
#include "sqliterecord.h"

#include PROPERTIES_INC
#include DIRECTORY_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteRecordManager : public SqliteView
	{
		public:

			// --- Constructors and destructor ---
			SqliteRecordManager(const SqliteDatabase& sdb, const util_env::Properties& props, const std::string& filename, bool openTable = true);
			virtual ~SqliteRecordManager();


			// --- Processing methods ---

			// Methods to find a record based on primary keys, use
			SqliteRecord* findKey(const SqliteField& key, const std::string& queryStart = std::string()); // Single key
			SqliteRecord* findKey(const std::vector<SqliteField>& keys, const std::string& queryStart = std::string()); // Compound keys

			// Method to get the single record of a single-record table
			SqliteRecord* findSingleRecord();

			SqliteRecord* create();
			bool save(const SqliteRecord* rec);
			bool markDeleted(const SqliteRecord* rec);
			virtual int deleteAll();
			virtual int deleteAll(std::string whereClause);
			virtual int countAll();
			virtual int countAll(std::string whereClause);
			virtual int calculateTotalSpace();
			virtual int calculateAverageSpace();
			virtual bool execute(const std::string sql);

			virtual void show();

			virtual bool copy(bool from, const util_filesystem::Directory& path);
			virtual bool backup(const std::string& dir);
			virtual bool exists();
			virtual bool compress();
			virtual bool remove();
			virtual bool check();
			virtual bool checkIndex();
			virtual bool reindex();


		protected:

			std::string autoIncrementField_;  // Up to one field per table may be autoincremented
			std::string autoIncrementTag_;    // To verify that an auto-increment value is not used

			std::string getSemiautoIncrementedValue(const std::string& field);
			void ensureAutoIncrementValue(const std::string& field, int value);


			// Methods that are to be implemented by individual record managers in order to
			// be able to process the backup command.
			//
			// LAP - I've changed these two from pure virtual to virtual, providing a default implementation, for
			// the database that doesn't have the ability to backup an individual table, such as SQLite, since all
			// of the tables are contained within a single operating system file.
			virtual bool needsBackup() { return false; }
			virtual void modified() { }


		private:

			const SqliteDatabase& sdb_;
			bool doNothing_;
			bool doReads_;
			bool doSaves_;
			bool doDeletes_;
			bool checkBeforeBackup_;

			void performAutoIncrements(SqliteRecord* rec);             // For create method
			bool checkIndexFiles();

			SqliteRecord* findKeyPrivate(const std::vector<SqliteField>& keys, const std::string& queryStart);


			// Do not allow the default copy constructor.
			SqliteRecordManager(const SqliteRecordManager& rm);
			// Do not allow the default assignment operator.
//			SqliteRecordManager operator=(const SqliteRecordManager& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITERECORDMANAGER_H
