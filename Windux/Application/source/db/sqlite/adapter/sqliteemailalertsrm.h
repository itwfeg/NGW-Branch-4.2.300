#ifndef SQLITEEMAILALERTSRM_H
#define SQLITEEMAILALERTSRM_H


#include <string>
#include <vector>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBEMAILALERTSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteEmailAlertsRM : public DBEmailAlertsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteEmailAlertsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBEmailAlertsRMI* d = 0);
			~SqliteEmailAlertsRM();


			// --- Processing methods ---
			// Methods from DBEmailAlertsRMI
			DBQueryI<DBEmailAlertI>* retrieveAll();
			DBQueryI<DBEmailAlertI>* retrieveRangeNumber(const std::string& first, const std::string& last);

			DBEmailAlertI* findNumber(const std::string& number);

			DBEmailAlertI* createRecord();
			void saveRecord(const DBEmailAlertI* rec);
			int deleteRecord(const DBEmailAlertI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBEmailAlertsRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteEmailAlertsRM(const SqliteEmailAlertsRM& rm);
			// Do not allow the default assignment operator.
			SqliteEmailAlertsRM operator=(const SqliteEmailAlertsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEEMAILALERTSRM_H
