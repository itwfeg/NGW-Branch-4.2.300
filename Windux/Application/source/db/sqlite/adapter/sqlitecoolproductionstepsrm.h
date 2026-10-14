#ifndef SQLITECOOLPRODUCTIONSTEPSRM_H
#define SQLITECOOLPRODUCTIONSTEPSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBCOOLPRODUCTIONSTEPSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteCoolProductionStepsRM : public DBCoolProductionStepsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteCoolProductionStepsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBCoolProductionStepsRMI* d = 0);
			~SqliteCoolProductionStepsRM();


			// --- Processing methods ---
			// Methods from DBCoolProductionStepsRMI
			DBQueryI<DBCoolProductionStepI>* retrieveAll();
			DBQueryI<DBCoolProductionStepI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBCoolProductionStepI>* retrieveAllCategories(const std::string& deptNum);

			DBCoolProductionStepI* findProductionStep(const std::string& name, const std::string& deptNum);
			DBCoolProductionStepI* find(const DBCoolCategoryProductionStepI* step);

			DBCoolProductionStepI* createRecord();
			void saveRecord(const DBCoolProductionStepI* rec);
			int deleteRecord(const DBCoolProductionStepI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBCoolProductionStepsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteCoolProductionStepsRM(const SqliteCoolProductionStepsRM& rm);
			// Do not allow the default assignment operator.
			SqliteCoolProductionStepsRM operator=(const SqliteCoolProductionStepsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITECOOLPRODUCTIONSTEPSRM_H
