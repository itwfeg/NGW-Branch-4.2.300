#ifndef DBTRANSACTIONTYPESRMI_H
#define DBTRANSACTIONTYPESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBTransactionTypeI;
	class DBTransactionI;
	template <class DBEntityI> class DBQueryI;


	class DBTransactionTypesRMI
	{
		public:

			virtual ~DBTransactionTypesRMI() {}


			virtual DBQueryI<DBTransactionTypeI>* retrieveAll() = 0;
			virtual DBQueryI<DBTransactionTypeI>* retrieveRangeNumber(const std::string& first, const std::string& last) = 0;

			virtual DBTransactionTypeI* findNumber(const std::string& number) = 0;
			virtual DBTransactionTypeI* findName1(const std::string& name) = 0;
			virtual DBTransactionTypeI* findName2(const std::string& name) = 0;
			virtual DBTransactionTypeI* findLinkedRecord(const DBTransactionI* transaction) = 0;

			virtual DBTransactionTypeI* createRecord() = 0;
			virtual void saveRecord(const DBTransactionTypeI* rec) = 0;
			virtual int deleteRecord(const DBTransactionTypeI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBTRANSACTIONTYPESRMI_H
