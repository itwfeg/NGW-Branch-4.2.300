#ifndef DBPRODUCTDEFAULTSRMI_H
#define DBPRODUCTDEFAULTSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductDefaultI;
	template <class DBEntityI> class DBQueryI;


	class DBProductDefaultsRMI
	{
		public:

			virtual ~DBProductDefaultsRMI() {}


			virtual DBQueryI<DBProductDefaultI>* retrieveAll() = 0;
			virtual DBQueryI<DBProductDefaultI>* retrieveAllForDept(const std::string& deptNum) = 0;

			virtual DBProductDefaultI* findType(const std::string& type, const std::string& deptNum) = 0;

			virtual DBProductDefaultI* createRecord() = 0;
			virtual void saveRecord(const DBProductDefaultI* rec) = 0;
			virtual int deleteRecord(const DBProductDefaultI* rec) = 0;
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


#endif // DBPRODUCTDEFAULTSRMI_H
