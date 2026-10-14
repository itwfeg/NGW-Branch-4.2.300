#ifndef DBBATCHSRM_H
#define DBBATCHSRM_H


#include <string>


#include "edb.h"
#include DBBATCHESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBBatch;
	template <class DBEntity> class DBQuery;


	class DBBatchesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBBatch>;

		public:
			// Types
			typedef DBBatch record_type;
			typedef DBQuery<DBBatch> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBBatchesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBBatch>* retrieveAll();
			DBQuery<DBBatch>* retrieveCompleteBatchesOfOrigin(int origin);
			DBQuery<DBBatch>* retrieveCompleteBatches(int version);
			DBQuery<DBBatch>* retrieveIncompleteBatches();
			DBQuery<DBBatch>* retrievePendingSynchronizationBatches();
			DBQuery<DBBatch>* retrievePendingDeleteBatches();
			DBQuery<DBBatch>* retrieveSameSignatureBatches(const std::string& signature);
			DBQuery<DBBatch>* retrieveRangeNumber(const std::string& first, const std::string& last);


			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the Batch record".
			DBBatch* findNumber(const std::string& number) const;
			DBBatch* findVersion(const std::string& version) const;
			DBBatch* find(const std::string& key) const { return findNumber(key); }

			DBBatch* createRecord() const;
			void saveRecord(const DBBatch* rec) const;
			int deleteRecord(const DBBatch* rec) const;         // returns # of deleted records
			int deleteAll() const;                              // returns # of deleted records
			int countAll() const;                               // returns # of existing records
			int calculateAverageSpace() const;                  // returns average # of bytes
			bool execute(const std::string sql) const;

			void ensureAutoIncrementValue(int value);

			void show();
			void showAllQueries();
			static void showAll(const std::string& debuggingCategory);


			static const std::string DESCRIPTOR;


		private:

			// --- Data members ---
			DBManager* dbm_;
			DBRecordManager::DBRecordType recordType_;
			bool enabled_;
			util_registry::Registry< DBQuery<DBBatch> > qsRegistry_;
			edb_adapter::DBBatchesRMI* decorated_;


			// --- Private Constructor ---
			DBBatchesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBBatchesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBBatchesRM(const DBBatchesRM& rm);
			// Do not allow the default assignment operator.
			DBBatchesRM operator=(const DBBatchesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBBatchesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBBatchesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBBatchesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBBatch>* query);
			bool deregisterQ(DBQuery<DBBatch>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBBATCHSRM_H
