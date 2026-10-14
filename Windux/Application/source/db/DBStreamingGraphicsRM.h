#ifndef DBSTREAMINGGRAPHICSRM_H
#define DBSTREAMINGGRAPHICSRM_H


#include <string>


#include "edb.h"
#include DBSTREAMINGGRAPHICSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBStreamingGraphic;
	template <class DBEntity> class DBQuery;


	class DBStreamingGraphicsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBStreamingGraphic>;

		public:

			// Types
			typedef DBStreamingGraphic record_type;
			typedef DBQuery<DBStreamingGraphic> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBStreamingGraphicsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBStreamingGraphic>* retrieveAll();
			DBQuery<DBStreamingGraphic>* retrieveRangeNumber(const std::string& first, const std::string& last);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the StreamingGraphic record".
			DBStreamingGraphic* findNumber(const std::string& number) const;
			DBStreamingGraphic* findNext(const DBStreamingGraphic* rec) const;
			DBStreamingGraphic* find(const std::string& key) const { return findNumber(key); }

			DBStreamingGraphic* createRecord() const;
			void saveRecord(const DBStreamingGraphic* rec) const;
			int deleteRecord(const DBStreamingGraphic* rec) const;       // returns # of deleted records
			int deleteAll() const;                                       // returns # of deleted records
			int countAll() const;                                        // returns # of existing records
			int calculateAverageSpace() const;                           // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBStreamingGraphic> > qsRegistry_;
			edb_adapter::DBStreamingGraphicsRMI* decorated_;


			// --- Private Constructor ---
			DBStreamingGraphicsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBStreamingGraphicsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBStreamingGraphicsRM(const DBStreamingGraphicsRM& rm);
			// Do not allow the default assignment operator.
			DBStreamingGraphicsRM operator=(const DBStreamingGraphicsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBStreamingGraphicsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBStreamingGraphicsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBStreamingGraphicsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBStreamingGraphic>* query);
			bool deregisterQ(DBQuery<DBStreamingGraphic>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBSTREAMINGGRAPHICSRM_H
