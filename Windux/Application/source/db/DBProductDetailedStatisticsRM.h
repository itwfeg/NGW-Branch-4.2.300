#ifndef DBPRODUCTDETAILEDSTATISTICSRM_H
#define DBPRODUCTDETAILEDSTATISTICSRM_H


#include <string>
       

#include "edb.h"
#include DBPRODUCTDETAILEDSTATISTICSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBProductDetailedStatistic;
	class DBPresentationMethod;
	class DBProductStatistic;
	template <class DBEntity> class DBQuery;


	class DBProductDetailedStatisticsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBProductDetailedStatistic>;

		public:

			// Types
			typedef DBProductDetailedStatistic record_type;
			typedef DBQuery<DBProductDetailedStatistic> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBProductDetailedStatisticsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBProductDetailedStatistic>* retrieveAll();
			DBQuery<DBProductDetailedStatistic>* retrieveLinkedtoProductStatistic(const DBProductStatistic* stat);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the ProductDetailedStatistic record".
			DBProductDetailedStatistic* find(const DBPresentationMethod* method, const DBProductStatistic* stat) const;

			DBProductDetailedStatistic* createRecord() const;
			void saveRecord(const DBProductDetailedStatistic* rec) const;
			int deleteRecord(const DBProductDetailedStatistic* rec) const;    // returns # of deleted records
			int deleteAll() const;                                            // returns # of deleted records
			int countAll() const;                                             // returns # of existing records
			int calculateAverageSpace() const;                                // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBProductDetailedStatistic> > qsRegistry_;
			edb_adapter::DBProductDetailedStatisticsRMI* decorated_;


			// --- Private Constructor ---
			DBProductDetailedStatisticsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBProductDetailedStatisticsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBProductDetailedStatisticsRM(const DBProductDetailedStatisticsRM& rm);
			// Do not allow the default assignment operator.
			DBProductDetailedStatisticsRM operator=(const DBProductDetailedStatisticsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBProductDetailedStatisticsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBProductDetailedStatisticsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBProductDetailedStatisticsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBProductDetailedStatistic>* query);
			bool deregisterQ(DBQuery<DBProductDetailedStatistic>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRODUCTDETAILEDSTATISTICSRM_H
