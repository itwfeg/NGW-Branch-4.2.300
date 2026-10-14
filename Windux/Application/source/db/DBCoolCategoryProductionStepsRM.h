#ifndef DBCOOLCATEGORYPRODUCTIONSTEPSRM_H
#define DBCOOLCATEGORYPRODUCTIONSTEPSRM_H


#include <string>
       

#include "edb.h"
#include DBCOOLCATEGORYPRODUCTIONSTEPSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCoolCategoryProductionStep;
	class DBCoolCategory;
	class DBCoolProductionStep;
	template <class DBEntity> class DBQuery;


	class DBCoolCategoryProductionStepsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCoolCategoryProductionStep>;

		public:

			// Types
			typedef DBCoolCategoryProductionStep record_type;
			typedef DBQuery<DBCoolCategoryProductionStep> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCoolCategoryProductionStepsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCoolCategoryProductionStep>* retrieveAll();
			DBQuery<DBCoolCategoryProductionStep>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBCoolCategoryProductionStep>* retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum);
			DBQuery<DBCoolCategoryProductionStep>* retrieveLinkedtoCoolProductionStep(const DBCoolProductionStep* step);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the CoolCategoryProductionStep record".
			DBCoolCategoryProductionStep* find(const std::string& category, const std::string& deptNum, const DBCoolProductionStep* step);

			DBCoolCategoryProductionStep* createRecord(const std::string& category, const DBCoolProductionStep* step) const;
			void saveRecord(const DBCoolCategoryProductionStep* rec) const;
			int deleteRecord(const DBCoolCategoryProductionStep* rec) const;    // returns # of deleted records
			int deleteAll() const;                                              // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
			int countAll() const;                                               // returns # of existing records
			int calculateAverageSpace() const;                                  // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBCoolCategoryProductionStep> > qsRegistry_;
			edb_adapter::DBCoolCategoryProductionStepsRMI* decorated_;


			// --- Private Constructor ---
			DBCoolCategoryProductionStepsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCoolCategoryProductionStepsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCoolCategoryProductionStepsRM(const DBCoolCategoryProductionStepsRM& rm);
			// Do not allow the default assignment operator.
			DBCoolCategoryProductionStepsRM operator=(const DBCoolCategoryProductionStepsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCoolCategoryProductionStepsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBCoolCategoryProductionStepsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCoolCategoryProductionStepsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBCoolCategoryProductionStep>* query);
			bool deregisterQ(DBQuery<DBCoolCategoryProductionStep>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCOOLCATEGORYPRODUCTIONSTEPSRM_H
