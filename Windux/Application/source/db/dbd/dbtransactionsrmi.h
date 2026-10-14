#ifndef DBTRANSACTIONSRMI_H
#define DBTRANSACTIONSRMI_H


#include <string>

#include DATETIME_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBTransactionI;
	template <class DBEntityI> class DBQueryI;


	class DBTransactionsRMI
	{
		public:

			virtual ~DBTransactionsRMI() {}


			virtual std::string getNextTransactionId() = 0;


			virtual DBQueryI<DBTransactionI>* retrieveAll() = 0;
			virtual DBQueryI<DBTransactionI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBTransactionI>* retrieveReadAndClear(bool equal, int cleared, const std::string& deptNum) = 0;
			virtual DBQueryI<DBTransactionI>* retrieveOperatorVoidable(const std::string& operatorId, int voidValue, const std::string& deptNum) = 0;
			virtual DBQueryI<DBTransactionI>* retrieveSupervisorVoidable(const std::string& operatorId, const std::string& deptNum, const std::string& pluNumber, const std::string& startTime, const std::string& endTime, int voidValue) = 0;
			virtual DBQueryI<DBTransactionI>* retrieveFilteredTransactions(const std::string& startTime, const std::string& endTime, const std::string& transType, int voidValue,
																			const std::string& deptNum, const std::string& pluNumber, const std::string& cutTestNumber, const std::string& operatorId, bool defaultOrder) = 0;
			virtual DBQueryI<DBTransactionI>* retrieveAllCutTestTransactions() = 0;	//	AWL-135, HA-134
			virtual DBQueryI<DBTransactionI>* retrieveCutTestTransactionsForCutTest(const std::string& cutTestNumber) = 0;

			virtual DBTransactionI* findTransaction(const std::string& scaleId, const std::string& transId) = 0;

			virtual DBTransactionI* createRecord(const std::string& scaleId, const std::string& transId) = 0;
			virtual void saveRecord(const DBTransactionI* rec) = 0;
			virtual int deleteRecord(const DBTransactionI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int deleteAllForDept(const std::string& deptNum) = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBTRANSACTIONSRMI_H
