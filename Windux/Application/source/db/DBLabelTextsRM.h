#ifndef DBLABELTEXTSRM_H
#define DBLABELTEXTSRM_H


#include <string>


#include "edb.h"
#include DBLABELTEXTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBLabelText;
	template <class DBEntity> class DBQuery;


	class DBLabelTextsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBLabelText>;

		public:

			// Types
			typedef DBLabelText record_type;
			typedef DBQuery<DBLabelText> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBLabelTextsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBLabelText>* retrieveAll();
			DBQuery<DBLabelText>* retrieveRangeNumber(const std::string& first, const std::string& last);

			// --- Processing methods for other database operations ---
			// find: a null return means "unable to find the LabelText record".
			DBLabelText* findNumber(const std::string& number) const;
			DBLabelText* find(const std::string& key) const { return findNumber(key); }

			DBLabelText* createRecord() const;
			void saveRecord(const DBLabelText* rec) const;
			int deleteRecord(const DBLabelText* rec) const;     // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBLabelText> > qsRegistry_;
			edb_adapter::DBLabelTextsRMI* decorated_;


			// --- Private Constructor ---
			DBLabelTextsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBLabelTextsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBLabelTextsRM(const DBLabelTextsRM& rm);
			// Do not allow the default assignment operator.
			DBLabelTextsRM operator=(const DBLabelTextsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBLabelTextsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBLabelTextsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBLabelTextsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBLabelText>* query);
			bool deregisterQ(DBQuery<DBLabelText>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBLABELTEXTSRM_H
