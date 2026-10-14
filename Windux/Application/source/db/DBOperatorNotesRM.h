#ifndef DBOPERATORNOTESRM_H
#define DBOPERATORNOTESRM_H


#include <string>


#include "edb.h"
#include DBOPERATORNOTESRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBOperatorNote;
	template <class DBEntity> class DBQuery;


	class DBOperatorNotesRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBOperatorNote>;

		public:

			// Types
			typedef DBOperatorNote record_type;
			typedef DBQuery<DBOperatorNote> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBOperatorNotesRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBOperatorNote>* retrieveAll();

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the OperatorNote record".
			DBOperatorNote* findRecord() const;
			DBOperatorNote* find() const { return findRecord(); }

			DBOperatorNote* createRecord() const;
			void saveRecord(const DBOperatorNote* rec) const;
			int deleteRecord(const DBOperatorNote* rec) const;  // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBOperatorNote> > qsRegistry_;
			edb_adapter::DBOperatorNotesRMI* decorated_;


			// --- Private Constructor ---
			DBOperatorNotesRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBOperatorNotesRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBOperatorNotesRM(const DBOperatorNotesRM& rm);
			// Do not allow the default assignment operator.
			DBOperatorNotesRM operator=(const DBOperatorNotesRM& rhs);


			// Registry methods
			static util_registry::Registry<DBOperatorNotesRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBOperatorNotesRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBOperatorNotesRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBOperatorNote>* query);
			bool deregisterQ(DBQuery<DBOperatorNote>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBOPERATORNOTESRM_H
