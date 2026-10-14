#ifndef SQLITEBATCHSRM_H
#define SQLITEBATCHSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBBATCHESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteBatchesRM : public DBBatchesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteBatchesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBBatchesRMI* d = 0);
			~SqliteBatchesRM();


			// --- Processing methods ---
			// Methods from DBBatchesRMI
			DBQueryI<DBBatchI>* retrieveAll();
			DBQueryI<DBBatchI>* retrieveCompleteBatchesOfOrigin(int origin);
			DBQueryI<DBBatchI>* retrieveCompleteBatches(int version);
			DBQueryI<DBBatchI>* retrieveIncompleteBatches();
			DBQueryI<DBBatchI>* retrievePendingSynchronizationBatches();
			DBQueryI<DBBatchI>* retrievePendingDeleteBatches();
			DBQueryI<DBBatchI>* retrieveSameSignatureBatches(const std::string& signature);
			DBQueryI<DBBatchI>* retrieveRangeNumber(const std::string& first, const std::string& last);

			DBBatchI* findNumber(const std::string& number);
			DBBatchI* findVersion(const std::string& version);

			DBBatchI* createRecord();
			void saveRecord(const DBBatchI* rec);
			int deleteRecord(const DBBatchI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }

			void ensureAutoIncrementValue(int value);


		protected:


		private:

			const SqliteDatabase& sdb_;
			DBBatchesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteBatchesRM(const SqliteBatchesRM& rm);
			// Do not allow the default assignment operator.
			SqliteBatchesRM operator=(const SqliteBatchesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEBATCHSRM_H
