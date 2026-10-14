#ifndef SQLITEPRODUCTDEFAULTSRM_H
#define SQLITEPRODUCTDEFAULTSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBPRODUCTDEFAULTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteProductDefaultsRM : public DBProductDefaultsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteProductDefaultsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductDefaultsRMI* d = 0);
			~SqliteProductDefaultsRM();


			// --- Processing methods ---
			// Methods from DBProductDefaultsRMI
			DBQueryI<DBProductDefaultI>* retrieveAll();
			DBQueryI<DBProductDefaultI>* retrieveAllForDept(const std::string& deptNum);

			DBProductDefaultI* findType(const std::string& type, const std::string& deptNum);

			DBProductDefaultI* createRecord();
			void saveRecord(const DBProductDefaultI* rec);
			int deleteRecord(const DBProductDefaultI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBProductDefaultsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteProductDefaultsRM(const SqliteProductDefaultsRM& rm);
			// Do not allow the default assignment operator.
			SqliteProductDefaultsRM operator=(const SqliteProductDefaultsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTDEFAULTSRM_H
