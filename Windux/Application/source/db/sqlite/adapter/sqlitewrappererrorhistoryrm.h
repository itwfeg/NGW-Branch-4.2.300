#ifndef SQLITEWRAPPERERRORHISTORYRM_H
#define SQLITEWRAPPERERRORHISTORYRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBWRAPPERERRORHISTORYRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteWrapperErrorHistoryRM : public DBWrapperErrorHistoryRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteWrapperErrorHistoryRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBWrapperErrorHistoryRMI* d = 0);
			~SqliteWrapperErrorHistoryRM();


			// --- Processing methods ---
			// Methods from DBWrapperErrorHistoryRMI
			DBQueryI<DBWrapperErrorHistoryI>* retrieveAll();
			
			DBWrapperErrorHistoryI* find(const std::string& number);

			DBWrapperErrorHistoryI* createRecord();
			void saveRecord(const DBWrapperErrorHistoryI* rec);
			int deleteRecord(const DBWrapperErrorHistoryI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBWrapperErrorHistoryRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteWrapperErrorHistoryRM(const SqliteWrapperErrorHistoryRM& rm);
			// Do not allow the default assignment operator.
			SqliteWrapperErrorHistoryRM operator=(const SqliteWrapperErrorHistoryRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEWRAPPERERRORHISTORYRM_H
