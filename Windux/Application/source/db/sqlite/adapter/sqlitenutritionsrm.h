#ifndef SQLITENUTRITIONSRM_H
#define SQLITENUTRITIONSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBNUTRITIONSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteNutritionsRM : public DBNutritionsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteNutritionsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBNutritionsRMI* d = 0);
			~SqliteNutritionsRM();


			// --- Processing methods ---
			// Methods from DBNutritionsRMI
			DBQueryI<DBNutritionI>* retrieveAll();
			DBQueryI<DBNutritionI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBNutritionI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum);

			DBNutritionI* findNumber(const std::string& number, const std::string& deptNum);

			DBNutritionI* createRecord();
			void saveRecord(const DBNutritionI* rec);
			int deleteRecord(const DBNutritionI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBNutritionsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteNutritionsRM(const SqliteNutritionsRM& rm);
			// Do not allow the default assignment operator.
			SqliteNutritionsRM operator=(const SqliteNutritionsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITENUTRITIONSRM_H
