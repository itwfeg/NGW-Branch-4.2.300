#ifndef DBPRODUCTTEXTSRM_H
#define DBPRODUCTTEXTSRM_H


#include <string>


#include "edb.h"
#include DBPRODUCTTEXTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC
#include UTILSTRING_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBProductText;
	template <class DBEntity> class DBQuery;


	class DBProductTextsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBProductText>;

		public:

			// Types
			typedef DBProductText record_type;
			typedef DBQuery<DBProductText> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBProductTextsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBProductText>* retrieveAll(int type = 0);
			DBQuery<DBProductText>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBProductText>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum, int type = 0);

			// --- Processing methods for other database operations ---
			// findNumber: a null return means "unable to find the ProductText record".
			DBProductText* findNumber(const std::string& number, const std::string& deptNum, int type = 0) const;
			//DBProductText* find(const std::string& key, const std::string& deptNum) const { return findNumber(key, deptNum); }

			DBProductText* createRecord(int type = 0) const;
			void saveRecord(const DBProductText* rec) const;
			int deleteRecord(const DBProductText* rec) const;      // returns # of deleted records
			int deleteAll(int type) const;                         // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
			int countAll(int type) const;                          // returns # of existing records
			int calculateAverageSpace() const;                     // returns average # of bytes
			bool execute(const std::string sql) const;

			void show();
			void showAllQueries();
			static void showAll(const std::string& debuggingCategory);


			int deleteAll() const { return deleteAll(type_); }     // returns # of deleted records
			int countAll() const { return countAll(type_); }       // returns # of existing records


			int getType() const { return type_; }
			void setType(int type) { type_ = type; }


			static const std::string DESCRIPTOR;


		private:

			// --- Data members ---
			DBManager* dbm_;
			DBRecordManager::DBRecordType recordType_;
			bool enabled_;
			util_registry::Registry<DBQuery<DBProductText> > qsRegistry_;
			edb_adapter::DBProductTextsRMI* decorated_;

			int type_;


			// --- Private Constructor ---
			DBProductTextsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBProductTextsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBProductTextsRM(const DBProductTextsRM& rm);
			// Do not allow the default assignment operator.
			DBProductTextsRM operator=(const DBProductTextsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBProductTextsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBProductTextsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBProductTextsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBProductText>* query);
			bool deregisterQ(DBQuery<DBProductText>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRODUCTTEXTSRM_H
