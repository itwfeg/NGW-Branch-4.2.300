#ifndef SQLITESYNCHRONIZATIONCHANGESRM_H
#define SQLITESYNCHRONIZATIONCHANGESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBSYNCHRONIZATIONCHANGESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;

	
	class SqliteSynchronizationChangesRM : public DBSynchronizationChangesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteSynchronizationChangesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBSynchronizationChangesRMI* d = 0);
			~SqliteSynchronizationChangesRM();


			// --- Processing methods ---
			// Methods from DBSynchronizationChangesRMI
			DBQueryI<DBSynchronizationChangeI>* retrieveAll();
			//DBQueryI<DBSynchronizationChangeI>* retrieveRangeNumber(const std::string& first, const std::string& last);
			DBQueryI<DBSynchronizationChangeI>* retrieveLinkedToBatch(const DBBatchI* batch);
			DBQueryI<DBSynchronizationChangeI>* retrieveLinkedToBatch(const std::string& batchNumber);

			DBSynchronizationChangeI* findIdentifier(const std::string& id);

			DBSynchronizationChangeI* createRecord(const DBBatchI* batch);
			DBSynchronizationChangeI* createRecord(const std::string& batchNumber);
			void saveRecord(const DBSynchronizationChangeI* rec);
			int deleteRecord(const DBSynchronizationChangeI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBSynchronizationChangesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteSynchronizationChangesRM(const SqliteSynchronizationChangesRM& rm);
			// Do not allow the default assignment batch.
			SqliteSynchronizationChangesRM operator=(const SqliteSynchronizationChangesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITESYNCHRONIZATIONCHANGESRM_H
