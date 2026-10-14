#ifndef SQLITEPRODUCTTEXTCFGSRM_H
#define SQLITEPRODUCTTEXTCFGSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBPRODUCTTEXTCFGSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteProductTextCfgsRM : public DBProductTextCfgsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteProductTextCfgsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBProductTextCfgsRMI* d = 0);
			~SqliteProductTextCfgsRM();


			// --- Processing methods ---
			// Methods from DBProductTextCfgsRMI
			DBQueryI<DBProductTextCfgI>* retrieveAll();

			DBProductTextCfgI* findNumber(int type = 0);

			DBProductTextCfgI* createRecord(int type = 0);
			void saveRecord(const DBProductTextCfgI* rec);
			int deleteRecord(const DBProductTextCfgI* rec);
			int deleteAll();
			int countAll();
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBProductTextCfgsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteProductTextCfgsRM(const SqliteProductTextCfgsRM& rm);
			// Do not allow the default assignment operator.
			SqliteProductTextCfgsRM operator=(const SqliteProductTextCfgsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEPRODUCTTEXTCFGSRM_H
