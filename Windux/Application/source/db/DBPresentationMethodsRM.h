#ifndef DBPRESENTATIONMETHODSRM_H
#define DBPRESENTATIONMETHODSRM_H


#include <string>


#include "edb.h"
#include DBPRESENTATIONMETHODSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBPresentationMethod;
	template <class DBEntity> class DBQuery;


	class DBPresentationMethodsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBPresentationMethod>;

		public:

			// Types
			typedef DBPresentationMethod record_type;
			typedef DBQuery<DBPresentationMethod> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBPresentationMethodsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBPresentationMethod>* retrieveAll();

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the PresentationMethod record".
			DBPresentationMethod* find(const std::string& type) const;

			DBPresentationMethod* createRecord() const;
			void saveRecord(const DBPresentationMethod* rec) const;
			int deleteRecord(const DBPresentationMethod* rec) const;   // returns # of deleted records
			int deleteAll() const;                                     // returns # of deleted records
			int countAll() const;                                      // returns # of existing records
			int calculateAverageSpace() const;                         // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBPresentationMethod> > qsRegistry_;
			edb_adapter::DBPresentationMethodsRMI* decorated_;


			// --- Private Constructor ---
			DBPresentationMethodsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBPresentationMethodsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBPresentationMethodsRM(const DBPresentationMethodsRM& rm);
			// Do not allow the default assignment operator.
			DBPresentationMethodsRM operator=(const DBPresentationMethodsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBPresentationMethodsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBPresentationMethodsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBPresentationMethodsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBPresentationMethod>* query);
			bool deregisterQ(DBQuery<DBPresentationMethod>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRESENTATIONMETHODSRM_H
