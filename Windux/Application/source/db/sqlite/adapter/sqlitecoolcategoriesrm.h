#ifndef SQLITECOOLCATEGORIESRM_H
#define SQLITECOOLCATEGORIESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCOOLCATEGORIESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCoolCategoriesRM : public DBCoolCategoriesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCoolCategoriesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolCategoriesRMI* d = 0);
			~SqliteCoolCategoriesRM();


			// --- Processing methods ---
			// Methods from DBCoolCategoriesRMI
			DBQueryI<DBCoolCategoryI>* retrieveAll();
			DBQueryI<DBCoolCategoryI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBCoolCategoryI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			DBCoolCategoryI* findNumber(const std::string& number, const std::string& deptNum);

			DBCoolCategoryI* createRecord();
			void saveRecord(const DBCoolCategoryI* rec);
			int deleteRecord(const DBCoolCategoryI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCoolCategoriesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCoolCategoriesRM(const SqliteCoolCategoriesRM& rm);
			// Do not allow the default assignment operator.
			SqliteCoolCategoriesRM operator=(const SqliteCoolCategoriesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLCATEGORIESRM_H
