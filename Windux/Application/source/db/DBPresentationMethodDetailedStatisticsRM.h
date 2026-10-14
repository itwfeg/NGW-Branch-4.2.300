#ifndef DBPRESENTATIONMETHODDETAILEDSTATISTICSRM_H
#define DBPRESENTATIONMETHODDETAILEDSTATISTICSRM_H


#include <string>
       

#include "edb.h"
#include DBPRESENTATIONMETHODDETAILEDSTATISTICSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBPresentationMethodDetailedStatistic;
	class DBPresentationMethod;
	class DBPresentationMethodStatistic;
	template <class DBEntity> class DBQuery;


	class DBPresentationMethodDetailedStatisticsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBPresentationMethodDetailedStatistic>;

		public:

			// Types
			typedef DBPresentationMethodDetailedStatistic record_type;
			typedef DBQuery<DBPresentationMethodDetailedStatistic> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBPresentationMethodDetailedStatisticsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBPresentationMethodDetailedStatistic>* retrieveAll();
			DBQuery<DBPresentationMethodDetailedStatistic>* retrieveLinkedtoPresentationMethodStatistic(const DBPresentationMethodStatistic* stat);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the PresentationMethodDetailedStatistic record".
			DBPresentationMethodDetailedStatistic* find(const DBPresentationMethod* method, const DBPresentationMethodStatistic* stat) const;

			DBPresentationMethodDetailedStatistic* createRecord(const DBPresentationMethod* method, const DBPresentationMethodStatistic* stat) const;
			void saveRecord(const DBPresentationMethodDetailedStatistic* rec) const;
			int deleteRecord(const DBPresentationMethodDetailedStatistic* rec) const;    // returns # of deleted records
			int deleteAll() const;                                                       // returns # of deleted records
			int countAll() const;                                                        // returns # of existing records
			int calculateAverageSpace() const;                                           // returns average # of bytes
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
			util_registry::Registry<DBQuery<DBPresentationMethodDetailedStatistic> > qsRegistry_;
			edb_adapter::DBPresentationMethodDetailedStatisticsRMI* decorated_;


			// --- Private Constructor ---
			DBPresentationMethodDetailedStatisticsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBPresentationMethodDetailedStatisticsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBPresentationMethodDetailedStatisticsRM(const DBPresentationMethodDetailedStatisticsRM& rm);
			// Do not allow the default assignment operator.
			DBPresentationMethodDetailedStatisticsRM operator=(const DBPresentationMethodDetailedStatisticsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBPresentationMethodDetailedStatisticsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBPresentationMethodDetailedStatisticsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBPresentationMethodDetailedStatisticsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBPresentationMethodDetailedStatistic>* query);
			bool deregisterQ(DBQuery<DBPresentationMethodDetailedStatistic>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRESENTATIONMETHODDETAILEDSTATISTICSRM_H
