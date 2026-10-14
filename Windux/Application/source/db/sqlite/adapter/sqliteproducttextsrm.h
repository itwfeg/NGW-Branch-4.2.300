#ifndef SQLITEPRODUCTTEXTSRM_H
#define SQLITEPRODUCTTEXTSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBPRODUCTTEXTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteProductTextsRM : public DBProductTextsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteProductTextsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductTextsRMI* d = 0);
			~SqliteProductTextsRM();


			// --- Processing methods ---
			// Methods from DBProductTextsRMI
			DBQueryI<DBProductTextI>* retrieveAll(int type = 0);
			DBQueryI<DBProductTextI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBProductTextI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum, int type = 0);

			DBProductTextI* findNumber(const std::string& number, const std::string& deptNum, int type = 0);

			DBProductTextI* createRecord(int type = 0);
			void saveRecord(const DBProductTextI* rec);
			int deleteRecord(const DBProductTextI* rec);
			int deleteAll(int type = 0);
			int deleteAllForDept(const std::string& deptNum);
			int countAll(int type = 0);
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBProductTextsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteProductTextsRM(const SqliteProductTextsRM& rm);
			// Do not allow the default assignment operator.
			SqliteProductTextsRM operator=(const SqliteProductTextsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTTEXTSRM_H
