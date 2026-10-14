#ifndef DBPRODUCTSTATISTICSRM_H
#define DBPRODUCTSTATISTICSRM_H


#include <string>
       

#include "edb.h"
#include DBPRODUCTSTATISTICSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBProductStatistic;
	class DBPresentationMethod;
	class DBProduct;
	template <class DBEntity> class DBQuery;


	class DBProductStatisticsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBProductStatistic>;

		public:

			// Types
			typedef DBProductStatistic record_type;
			typedef DBQuery<DBProductStatistic> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBProductStatisticsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBProductStatistic>* retrieveAll();
			DBQuery<DBProductStatistic>* retrieveLinkedtoProduct(const DBProduct* prod);
			DBQuery<DBProductStatistic>* retrieveMostSelected(const std::string& language, int hour);
			DBQuery<DBProductStatistic>* retrieveMostPrinted(const std::string& language, int hour);
			DBQuery<DBProductStatistic>* retrieveDateTimeOrder();

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the ProductStatistic record".
			DBProductStatistic* find(const DBProduct* prod, const std::string& language, const std::string& hour) const;

			DBProductStatistic* createRecord(const DBProduct* prod) const;
			void saveRecord(const DBProductStatistic* rec) const;
			int deleteRecord(const DBProductStatistic* rec) const;   // returns # of deleted records
			int deleteAll() const;                                   // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBProductStatistic> > qsRegistry_;
			edb_adapter::DBProductStatisticsRMI* decorated_;


			// --- Private Constructor ---
			DBProductStatisticsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBProductStatisticsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBProductStatisticsRM(const DBProductStatisticsRM& rm);
			// Do not allow the default assignment operator.
			DBProductStatisticsRM operator=(const DBProductStatisticsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBProductStatisticsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBProductStatisticsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBProductStatisticsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBProductStatistic>* query);
			bool deregisterQ(DBQuery<DBProductStatistic>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRODUCTSTATISTICSRM_H
