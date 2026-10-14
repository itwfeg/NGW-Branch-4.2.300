#ifndef SQLITECOOLCATEGORYPRODUCTIONSTEPSRM_H
#define SQLITECOOLCATEGORYPRODUCTIONSTEPSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCOOLCATEGORYPRODUCTIONSTEPSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCoolCategoryProductionStepsRM : public DBCoolCategoryProductionStepsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCoolCategoryProductionStepsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolCategoryProductionStepsRMI* d = 0);
			~SqliteCoolCategoryProductionStepsRM();


			// --- Processing methods ---
			// Methods from DBCoolCategoryProductionStepsRMI
			DBQueryI<DBCoolCategoryProductionStepI>* retrieveAll();
			DBQueryI<DBCoolCategoryProductionStepI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBCoolCategoryProductionStepI>* retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum);
			DBQueryI<DBCoolCategoryProductionStepI>* retrieveLinkedtoCoolProductionStep(const DBCoolProductionStepI* step);

			DBCoolCategoryProductionStepI* find(const std::string& category, const std::string& deptNum, const DBCoolProductionStepI* step);

			DBCoolCategoryProductionStepI* createRecord(const std::string& category, const DBCoolProductionStepI* step);
			void saveRecord(const DBCoolCategoryProductionStepI* rec);
			int deleteRecord(const DBCoolCategoryProductionStepI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCoolCategoryProductionStepsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCoolCategoryProductionStepsRM(const SqliteCoolCategoryProductionStepsRM& rm);
			// Do not allow the default assignment operator.
			SqliteCoolCategoryProductionStepsRM operator=(const SqliteCoolCategoryProductionStepsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLCATEGORYPRODUCTIONSTEPSRM_H
