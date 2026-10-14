#ifndef SQLITEDEVICESRM_H
#define SQLITEDEVICESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBDEVICESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteDevicesRM : public DBDevicesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteDevicesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBDevicesRMI* d = 0);
			~SqliteDevicesRM();


			// --- Processing methods ---
			// Methods from DBDevicesRMI
			DBQueryI<DBDeviceI>* retrieveAll();
			DBQueryI<DBDeviceI>* retrieveType(const std::string& type);
			DBQueryI<DBDeviceI>* retrieveCheckPresence(bool check);

			DBDeviceI* findTypeSubtype(const std::string& type, const std::string& subtype);

			DBDeviceI* createRecord();
			void saveRecord(const DBDeviceI* rec);
			int deleteRecord(const DBDeviceI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBDevicesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteDevicesRM(const SqliteDevicesRM& rm);
			// Do not allow the default assignment operator.
			SqliteDevicesRM operator=(const SqliteDevicesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEDEVICESRM_H
