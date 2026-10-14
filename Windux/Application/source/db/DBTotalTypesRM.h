#ifndef DBTOTALTYPESRM_H
#define DBTOTALTYPESRM_H


#include <string>


#include "edb.h"
#include DBTOTALTYPESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBTotalType;
	class DBProductTotal;
	template <class DBEntity> class DBQuery;


	class DBTotalTypesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBTotalType>;

		public:

			// Types
			typedef DBTotalType record_type;
			typedef DBQuery<DBTotalType> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBTotalTypesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBTotalType>* retrieveAll();
			DBQuery<DBTotalType>* retrieveRangeNumber(const std::string& first, const std::string& last);

			// --- Processing methods for other database operations ---
			// findPLU: a null return means "unable to find the TotalType record".
			DBTotalType* findNumber(const std::string& number) const;
			DBTotalType* find(const std::string& key) const { return findNumber(key); }
			DBTotalType* findName(const std::string& name) const;
			DBTotalType* findLinkedRecord(const DBProductTotal* total) const;

			DBTotalType* createRecord() const;
			void saveRecord(const DBTotalType* rec) const;
			int deleteRecord(const DBTotalType* rec) const;     // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBTotalType> > qsRegistry_;
			edb_adapter::DBTotalTypesRMI* decorated_;


			// --- Private Constructor ---
			DBTotalTypesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBTotalTypesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBTotalTypesRM(const DBTotalTypesRM& rm);
			// Do not allow the default assignment operator.
			DBTotalTypesRM operator=(const DBTotalTypesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBTotalTypesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBTotalTypesRM& view);
			static bool deregisterRM(DBTotalTypesRM& view) { return _registry_->remove(&view); }
			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBTotalType>* query);
			bool deregisterQ(DBQuery<DBTotalType>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBTOTALTYPESRM_H
