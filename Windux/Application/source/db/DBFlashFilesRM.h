#ifndef DBFLASHFILESRM_H
#define DBFLASHFILESRM_H


#include <string>


#include "edb.h"
#include DBFLASHFILESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBFlashFile;
	template <class DBEntity> class DBQuery;


	class DBFlashFilesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBFlashFile>;

		public:

			// Types
			typedef DBFlashFile record_type;
			typedef DBQuery<DBFlashFile> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBFlashFilesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBFlashFile>* retrieveAll();

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the FlashFile record".
			DBFlashFile* findName(const std::string& name) const;
			DBFlashFile* find(const std::string& key) const { return findName(key); }

			DBFlashFile* createRecord() const;
			void saveRecord(const DBFlashFile* rec) const;
			int deleteRecord(const DBFlashFile* rec) const;     // returns # of deleted records
			int deleteAll() const;                              // returns # of deleted records
			int countAll() const;                               // returns # of existing records
			int calculateAverageSpace() const;                  // returns average # of bytes
			bool execute(const std::string sql) const;

			void show();
			void showAllQueries();
			static void showAll(const std::string& debuggingCategory);


			static const std::string DESCRIPTOR;


		private:

			// --- Data members ---
			DBManager* dbm_;
			DBRecordManager::DBRecordType recordType_;
			bool enabled_;
			util_registry::Registry<DBQuery<DBFlashFile> > qsRegistry_;
			edb_adapter::DBFlashFilesRMI* decorated_;


			// --- Private Constructor ---
			DBFlashFilesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBFlashFilesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBFlashFilesRM(const DBFlashFilesRM& rm);
			// Do not allow the default assignment operator.
			DBFlashFilesRM operator=(const DBFlashFilesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBFlashFilesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBFlashFilesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBFlashFilesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBFlashFile>* query);
			bool deregisterQ(DBQuery<DBFlashFile>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBFLASHFILESRM_H
