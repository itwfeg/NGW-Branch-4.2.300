#ifndef SQLITENETWORKERRORSRM_H
#define SQLITENETWORKERRORSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBNETWORKERRORSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;

	
	class SqliteNetworkErrorsRM : public DBNetworkErrorsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteNetworkErrorsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBNetworkErrorsRMI* d = 0);
			~SqliteNetworkErrorsRM();


			// --- Processing methods ---
			// Methods from DBNetworkErrorsRMI
			DBQueryI<DBNetworkErrorI>* retrieveAll();
			//DBQueryI<DBNetworkErrorI>* retrieveRangeNumber(const std::string& first, const std::string& last);

			DBNetworkErrorI* findID(const std::string& id);

			DBNetworkErrorI* createRecord();
			void saveRecord(const DBNetworkErrorI* rec);
			int deleteRecord(const DBNetworkErrorI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBNetworkErrorsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteNetworkErrorsRM(const SqliteNetworkErrorsRM& rm);
			// Do not allow the default assignment operator.
			SqliteNetworkErrorsRM operator=(const SqliteNetworkErrorsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITENETWORKERRORSRM_H
