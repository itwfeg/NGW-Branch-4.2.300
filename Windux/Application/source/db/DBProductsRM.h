#ifndef DBPRODUCTSRM_H
#define DBPRODUCTSRM_H


#include <string>


#include "edb.h"
#include DBPRODUCTSRMI_INC
#include DBRECORDMANAGER_INC
#include REGISTRY_INC
#include MUTEX_INC
#include DBQUERY_INC
#include DBQUERYI_INC


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;
	class DBProduct;
	class DBClass;
	class DBProductText;
	class DBNutrition;
	class DBCategory;
	class DBCoolCategory;
	class DBCoolPreText;
	class DBCoolText;
	class DBCoolList;
	class DBProductStatistic;
	class DBSpecialProgram;
	class DBSoftKey;
	template <class DBEntity> class DBQuery;


	class DBProductsRM : public DBRecordManager
	{
			friend class DBFactory;
			friend class DBQuery<DBProduct>;

		public:

			// Types
			typedef DBProduct record_type;
			typedef DBQuery<DBProduct> query_type;


			// --- Constructors and destructor ---
			// The constructors are private so that only the DBFactory class (a
			// friend class) will be able to instanciate this class.
			~DBProductsRM();


			// --- Accessor methods ---
			DBRecordManager::DBRecordType getRecordType() const { return recordType_; }
			bool isEnabled() const { return enabled_; }
			DBManager* getDBManager() const { return dbm_; }


			// --- Processing methods for querying the database ---
			// These methods always return a query result. If no records were found,
			// the query will be empty (i.e., query.begin() == query.end() is true).
			DBQuery<DBProduct>* retrieveAll();
			DBQuery<DBProduct>* retrieveAllForDept(const std::string& deptNum);
			DBQuery<DBProduct>* retrieveRangePLU(const std::string& first, const std::string& last, const std::string& deptNum);
			DBQuery<DBProduct>* retrieveNearestPLU(const DBProduct* product, int count);
			DBQuery<DBProduct>* retrieveNearestPLU(const std::string& plu, const std::string& deptNum, int count);
			DBQuery<DBProduct>* retrieveKeywordInName(const std::string& keyword);
			DBQuery<DBProduct>* retrieveKeywordInDescription(const std::string& keyword, const std::string& deptNum);
			DBQuery<DBProduct>* retrieveNameStartsWith(const std::string& keyword, LanguageSelection langNum, const std::string& deptNum);
			DBQuery<DBProduct>* retrieveDescriptionStartsWith(const std::string& keyword, const std::string& deptNum);
			DBQuery<DBProduct>* retrieveDescriptionOrNameStartsWith(const std::string& keyword, LanguageSelection langNum, const std::string& deptNum);
			DBQuery<DBProduct>* retrieveLinkedtoProduct(const DBProduct* prod);
			DBQuery<DBProduct>* retrieveLinkedtoClass(const DBClass* clas);
			DBQuery<DBProduct>* retrieveLinkedtoNutrition(const DBNutrition* nutrition);
			DBQuery<DBProduct>* retrieveLinkedtoText(const DBProductText* text);
			DBQuery<DBProduct>* retrieveLinkedtoMessage(const DBProductText* message);
			DBQuery<DBProduct>* retrieveLinkedtoNote(const DBProductText* note);
			DBQuery<DBProduct>* retrieveLinkedtoMarquee(const DBProductText* marquee);
			DBQuery<DBProduct>* retrieveLinkedtoProductText5(const DBProductText* prodText);
			DBQuery<DBProduct>* retrieveLinkedtoProductText6(const DBProductText* prodText);
			DBQuery<DBProduct>* retrieveLinkedtoProductText7(const DBProductText* prodText);
			DBQuery<DBProduct>* retrieveLinkedtoProductText8(const DBProductText* prodText);
			DBQuery<DBProduct>* retrieveLinkedtoProductText9(const DBProductText* prodText);
			DBQuery<DBProduct>* retrieveLinkedtoCategory(const DBCategory* category);
			DBQuery<DBProduct>* retrieveLinkedtoCoolCategory(const DBCoolCategory* coolcategory);
			DBQuery<DBProduct>* retrieveLinkedtoCoolPreText(const DBCoolPreText* coolpretext);
			DBQuery<DBProduct>* retrieveLinkedtoDefaultCoolText(const DBCoolText* cooltext);
			DBQuery<DBProduct>* retrieveLinkedtoCoolList(const DBCoolList* coollist);
			DBQuery<DBProduct>* retrieveSpecials(const DBSpecialProgram* special);
			DBQuery<DBProduct>* retrieveAllSpecials(const std::string& deptNum);
			DBQuery<DBProduct>* retrieveLinkedtoRec(const std::string& column1Name, const std::string column1Value, const std::string& column2Name = "", const std::string& column2Value = "");

			bool isProductShortNameUsed(LanguageSelection langNum);

			// --- Processing methods for other database operations ---
			// findPLU: a null return means "unable to find the Product record".
			DBProduct* findPLU(const std::string& plu, const std::string& deptNum, bool inOperateMode = false) const;
			DBProduct* find(const std::string& key, const std::string& deptNum) const { return findPLU(key, deptNum); }
			DBProduct* findLinkedRecord(const DBProductStatistic* stat) const;
			DBProduct* findLinkedRecord(const DBSoftKey* skey) const;

			DBProduct* createRecord() const;
			void saveRecord(const DBProduct* rec) const;
			int deleteRecord(const DBProduct* rec) const;       // returns # of deleted records
			int deleteAll() const;                              // returns # of deleted records
			int deleteAllForDept(const std::string& deptNum);   // returns # of deleted records
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
			util_registry::Registry<DBQuery<DBProduct> > qsRegistry_;
			edb_adapter::DBProductsRMI* decorated_;


			// --- Private Constructor ---
			DBProductsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBProductsRMI* d);

			// --- Not implemented methods (linker will complain if called) ---
			// Do not allow the default copy constructor.
			DBProductsRM(const DBProductsRM& rm);
			// Do not allow the default assignment operator.
			DBProductsRM operator=(const DBProductsRM& rhs);


			// Registry methods
			static util_registry::Registry<DBProductsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(DBProductsRM& view);

			// We don't need to keep this registry for SQLite, so we're bypassing this.
			static bool deregisterRM(DBProductsRM& /*view*/) { return true; } // _registry_->remove(&view); }

			static void disableAllRMs();

			void disable();
			bool registerQ(DBQuery<DBProduct>* query);
			bool deregisterQ(DBQuery<DBProduct>* query);
			void disableAllQs();
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBPRODUCTSRM_H
