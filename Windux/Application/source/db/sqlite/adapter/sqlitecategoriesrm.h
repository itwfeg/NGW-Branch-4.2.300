#ifndef SQLITECATEGORIESRM_H
#define SQLITECATEGORIESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCATEGORIESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCategoriesRM : public DBCategoriesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCategoriesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCategoriesRMI* d = 0);
			~SqliteCategoriesRM();


			// --- Processing methods ---
			// Methods from DBCategoriesRMI
			DBQueryI<DBCategoryI>* retrieveAll();
			DBQueryI<DBCategoryI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBCategoryI>* retrieveAllByName();
			DBQueryI<DBCategoryI>* retrieveAllByName(const std::string& deptNum);
			DBQueryI<DBCategoryI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);
			DBQueryI<DBCategoryI>* retrieveChildCategories(const std::string& ca, const std::string& deptNum);
			DBQueryI<DBCategoryI>* retrieveAllSubcategories(const std::string& deptNum);

			DBCategoryI* findNumber(const std::string& number, const std::string& deptNum);

			DBCategoryI* createRecord();
			void saveRecord(const DBCategoryI* rec);
			int deleteRecord(const DBCategoryI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		protected:


		private:

			const SqliteDatabase& sdb_;
			DBCategoriesRMI* decorated_;

			const static std::string DATA_FILENAME;
         
			// Do not allow the default copy constructor.
			SqliteCategoriesRM(const SqliteCategoriesRM& rm);
			// Do not allow the default assignment operator.
			SqliteCategoriesRM operator=(const SqliteCategoriesRM& rhs);


			int localeTransformedLength_;

			void retrieveProperties();


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECATEGORIESRM_H
