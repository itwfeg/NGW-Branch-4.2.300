#ifndef SQLITEKNIFETIMEHISTORYRM_H
#define SQLITEKNIFETIMEHISTORYRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBKNIFETIMEHISTORYRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteKnifeTimeHistoryRM : public DBKnifeTimeHistoryRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteKnifeTimeHistoryRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBKnifeTimeHistoryRMI* d = 0);
			~SqliteKnifeTimeHistoryRM();
			

			// --- Processing methods ---
			// Methods from DBKnifeTimeHistoryRMI
			DBQueryI<DBKnifeTimeHistoryI>* retrieveAll();
			
			DBKnifeTimeHistoryI* find(const std::string& number);

			DBKnifeTimeHistoryI* createRecord();
			void saveRecord(const DBKnifeTimeHistoryI* rec);
			int deleteRecord(const DBKnifeTimeHistoryI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBKnifeTimeHistoryRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteKnifeTimeHistoryRM(const SqliteKnifeTimeHistoryRM& rm);
			// Do not allow the default assignment operator.
			SqliteKnifeTimeHistoryRM operator=(const SqliteKnifeTimeHistoryRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEKNIFETIMEHISTORYRM_H
