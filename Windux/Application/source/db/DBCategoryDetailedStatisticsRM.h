#ifndef DBCATEGORYDETAILEDSTATISTICSRM_H
#define DBCATEGORYDETAILEDSTATISTICSRM_H


#include <string>
       

#include "edb.h"
#include DBCATEGORYDETAILEDSTATISTICSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBCategoryDetailedStatistic;
	class DBPresentationMethod;
	class DBCategory;
	class DBCategoryStatistic;
	template <class DBEntity> class DBQuery;


	class DBCategoryDetailedStatisticsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBCategoryDetailedStatistic>;

		public:

			// Types
			typedef DBCategoryDetailedStatistic record_type;
			typedef DBQuery<DBCategoryDetailedStatistic> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBCategoryDetailedStatisticsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBCategoryDetailedStatistic>* retrieveAll();
			DBQuery<DBCategoryDetailedStatistic>* retrieveLinkedtoCategoryStatistic(const DBCategoryStatistic* stat);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the CategoryDetailedStatistic record".
			DBCategoryDetailedStatistic* find(const DBPresentationMethod* method, const DBCategoryStatistic* stat) const;

			DBCategoryDetailedStatistic* createRecord(const DBPresentationMethod* method, const DBCategoryStatistic* stat) const;
			void saveRecord(const DBCategoryDetailedStatistic* rec) const;
			int deleteRecord(const DBCategoryDetailedStatistic* rec) const;    // returns # of deleted records
			int deleteAll() const;                                             // returns # of deleted records
			int countAll() const;                                              // returns # of existing records
			int calculateAverageSpace() const;                                 // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBCategoryDetailedStatistic> > qsRegistry_;
			edb_adapter::DBCategoryDetailedStatisticsRMI* decorated_;


			// --- Private Constructor ---
			DBCategoryDetailedStatisticsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBCategoryDetailedStatisticsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBCategoryDetailedStatisticsRM(const DBCategoryDetailedStatisticsRM& rm);
			// Do not allow the default assignment operator.
			DBCategoryDetailedStatisticsRM operator=(const DBCategoryDetailedStatisticsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBCategoryDetailedStatisticsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBCategoryDetailedStatisticsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBCategoryDetailedStatisticsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBCategoryDetailedStatistic>* query);
			bool deregisterQ(DBQuery<DBCategoryDetailedStatistic>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBCATEGORYDETAILEDSTATISTICSRM_H
