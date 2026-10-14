#ifndef SQLITETRANSACTIONSRM_H
#define SQLITETRANSACTIONSRM_H


#include <string>
#include <vector>


#include "edb.h"
#include "sqliterecordmanager.h"
#include "CSmartPtr.h"

#include DBTRANSACTIONSRMI_INC
#include PROPERTIES_INC
#include MUTEX_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class SqliteDatabase;
	class SqliteTransaction;


	class SqliteTransactionsRM : public DBTransactionsRMI, public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteTransactionsRM(const SqliteDatabase& sdb, const util_env::Properties& props, DBTransactionsRMI* d = 0);
			~SqliteTransactionsRM();


			// --- Processing methods ---
			// Methods from DBTransactionsRMI
			DBQueryI<DBTransactionI>* retrieveAll();
			DBQueryI<DBTransactionI>* retrieveAllForDept(const std::string& deptNum);
			DBQueryI<DBTransactionI>* retrieveReadAndClear(bool equal, int cleared, const std::string& deptNum);
			DBQueryI<DBTransactionI>* retrieveOperatorVoidable(const std::string& operatorId, int voidValue, const std::string& deptNum);
			DBQueryI<DBTransactionI>* retrieveSupervisorVoidable(const std::string& operatorId, const std::string& deptNum, const std::string& pluNumber, const std::string& startTime, const std::string& endTime, int voidValue);
			DBQueryI<DBTransactionI>* retrieveFilteredTransactions(const std::string& startTime, const std::string& endTime, const std::string& transType, int voidValue,
																	const std::string& deptNum, const std::string& pluNumber, const std::string& cutTestNumber, const std::string& operatorId, bool defaultOrder);
			DBQueryI<DBTransactionI>* retrieveAllCutTestTransactions();		//	AWL-135, HA-134
			DBQueryI<DBTransactionI>* retrieveCutTestTransactionsForCutTest(const std::string& cutTestNumber);

			DBTransactionI* findTransaction(const std::string& scaleId, const std::string& transId);

			DBTransactionI* createRecord(const std::string& scaleId, const std::string& transId);
			void saveRecord(const DBTransactionI* rec);
			int deleteRecord(const DBTransactionI* rec);
			int deleteAll() { return SqliteRecordManager::deleteAll(); }
			int deleteAllForDept(const std::string& deptNum);
			int countAll() { return SqliteRecordManager::countAll(); }
			int calculateAverageSpace() { return SqliteRecordManager::calculateAverageSpace(); }
			bool execute(const std::string sql) { return SqliteRecordManager::execute(sql); }

			void show() { SqliteRecordManager::show(); }

			// Other methods
			std::string getNextTransactionId();


		private:

			const SqliteDatabase& sdb_;
			Util::CSmartPtr<DBTransactionsRMI> decorated_;

			static const std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteTransactionsRM(const SqliteTransactionsRM& rm);
			// Do not allow the default assignment operator.
			SqliteTransactionsRM operator=(const SqliteTransactionsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};

	
#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITETRANSACTIONSRM_H
