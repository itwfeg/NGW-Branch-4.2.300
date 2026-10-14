#ifndef DBEXPIRATIONLABELSRM_H
#define DBEXPIRATIONLABELSRM_H


#include <string>


#include "edb.h"
#include DBEXPIRATIONLABELSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBExpirationLabel;
	template <class DBEntity> class DBQuery;


	class DBExpirationLabelsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBExpirationLabel>;

		public:

			// Types
			typedef DBExpirationLabel record_type;
			typedef DBQuery<DBExpirationLabel> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBExpirationLabelsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBExpirationLabel>* retrieveAll();
			DBQuery<DBExpirationLabel>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBExpirationLabel>* retrieveLinkedToPLU(const std::string& plu, const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the ExpirationLabel record".
			DBExpirationLabel* find(const std::string& key, const std::string& deptNum) const;

			DBExpirationLabel* createRecord() const;
			void saveRecord(const DBExpirationLabel* rec) const;
			int deleteRecord(const DBExpirationLabel* rec) const;    // returns # of deleted records
			int deleteAll() const;                                   // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
			int countAll() const;                                    // returns # of existing records
			int calculateAverageSpace() const;                       // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBExpirationLabel> > qsRegistry_;
			edb_adapter::DBExpirationLabelsRMI* decorated_;


			// --- Private Constructor ---
			DBExpirationLabelsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBExpirationLabelsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBExpirationLabelsRM(const DBExpirationLabelsRM& rm);
			// Do not allow the default assignment operator.
			DBExpirationLabelsRM operator=(const DBExpirationLabelsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBExpirationLabelsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBExpirationLabelsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBExpirationLabelsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBExpirationLabel>* query);
			bool deregisterQ(DBQuery<DBExpirationLabel>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBEXPIRATIONLABELSRM_H
