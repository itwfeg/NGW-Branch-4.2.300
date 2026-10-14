#ifndef DBCUTTINGTESTSRMI_H
#define DBCUTTINGTESTSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCuttingTestI;
	class DBOperatorI;
	template <class DBEntityI> class DBQueryI;


	class DBCuttingTestsRMI
	{
		public:

			virtual ~DBCuttingTestsRMI() {}


			virtual DBQueryI<DBCuttingTestI>* retrieveAll() = 0;
			virtual DBQueryI<DBCuttingTestI>* retrieveByOperator(const std::string& opNum) = 0;

			virtual DBCuttingTestI* find(const std::string& id) = 0;

			virtual DBCuttingTestI* createRecord(const std::string& operatorNumber) = 0;
			virtual void saveRecord(const DBCuttingTestI* rec) = 0;
			virtual int deleteRecord(const DBCuttingTestI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCUTTINGTESTSRMI_H
