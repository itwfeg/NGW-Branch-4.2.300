#ifndef DBCOOLPRODUCTIONSTEPSRM_H
#define DBCOOLPRODUCTIONSTEPSRM_H


#include <string>


#include "edb.h"
#include DBCOOLPRODUCTIONSTEPSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCoolProductionStep;
	class DBCoolCategoryProductionStep;
	template <class DBEntity> class DBQuery;


	class DBCoolProductionStepsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCoolProductionStep>;

		public:

			// Types
			typedef DBCoolProductionStep record_type;
			typedef DBQuery<DBCoolProductionStep> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCoolProductionStepsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCoolProductionStep>* retrieveAll();
			DBQuery<DBCoolProductionStep>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBCoolProductionStep>* retrieveAllCategories(const std::string& deptNum);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the Category record".
			DBCoolProductionStep* findProductionStep(const std::string& name, const std::string& deptNum) const;
			DBCoolProductionStep* find(const std::string& key, const std::string& deptNum) const { return findProductionStep(key, deptNum); }
			DBCoolProductionStep* find(const DBCoolCategoryProductionStep* step);

			DBCoolProductionStep* createRecord() const;
			void saveRecord(const DBCoolProductionStep* rec) const;
			int deleteRecord(const DBCoolProductionStep* rec) const;   // returns # of deleted records
			int deleteAll() const;                                     // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBCoolProductionStep> > qsRegistry_;
			edb_adapter::DBCoolProductionStepsRMI* decorated_;


			// --- Private Constructor ---
			DBCoolProductionStepsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolProductionStepsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCoolProductionStepsRM(const DBCoolProductionStepsRM& rm);
			// Do not allow the default assignment operator.
			DBCoolProductionStepsRM operator=(const DBCoolProductionStepsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCoolProductionStepsRM>* _registry_;
			static bool registerRM(DBCoolProductionStepsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCoolProductionStepsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();
			// The following mutex is to prevent the creation of two registry objects
			static util_thread::Mutex<> _registryMutex_;   

			void disable();
			bool registerQ(DBQuery<DBCoolProductionStep>* query);
			bool deregisterQ(DBQuery<DBCoolProductionStep>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLPRODUCTIONSTEPSRM_H
