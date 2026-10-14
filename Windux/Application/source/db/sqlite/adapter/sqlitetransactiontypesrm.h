#ifndef SQLITETRANSACTIONTYPESRM_H
#define SQLITETRANSACTIONTYPESRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include DBTRANSACTIONTYPESRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;


	class SqliteTransactionTypesRM : public DBTransactionTypesRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteTransactionTypesRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBTransactionTypesRMI* d = 0);
			~SqliteTransactionTypesRM();


			// --- Processing methods ---
			// Methods from DBTransactionTypesRMI
			DBQueryI<DBTransactionTypeI>* retrieveAll();
			DBQueryI<DBTransactionTypeI>* retrieveRangeNumber(const std::string& first, const std::string& last);

			DBTransactionTypeI* findNumber(const std::string& number);
			DBTransactionTypeI* findName1(const std::string& name);
			DBTransactionTypeI* findName2(const std::string& name);
			DBTransactionTypeI* findLinkedRecord(const DBTransactionI* transaction);

			DBTransactionTypeI* createRecord();
			void saveRecord(const DBTransactionTypeI* rec);
			int deleteRecord(const DBTransactionTypeI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }


		private:

			const SqliteDatabase& sdb_;
			DBTransactionTypesRMI* decorated_;

			const static std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteTransactionTypesRM(const SqliteTransactionTypesRM& rm);
			// Do not allow the default assignment operator.
			SqliteTransactionTypesRM operator=(const SqliteTransactionTypesRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITETRANSACTIONTYPESRM_H
