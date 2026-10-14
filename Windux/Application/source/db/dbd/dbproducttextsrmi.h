#ifndef DBPRODUCTTEXTSRMI_H
#define DBPRODUCTTEXTSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductTextI;
	template <class DBEntityI> class DBQueryI;


	class DBProductTextsRMI
	{
		public:

			virtual ~DBProductTextsRMI() {}


			virtual DBQueryI<DBProductTextI>* retrieveAll(int type = 0) = 0;
			virtual DBQueryI<DBProductTextI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBProductTextI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum, int type = 0) = 0;

			virtual DBProductTextI* findNumber(const std::string& number, const std::string& deptNum, int type = 0) = 0;

			virtual DBProductTextI* createRecord(int type = 0) = 0;
			virtual void saveRecord(const DBProductTextI* rec) = 0;
			virtual int deleteRecord(const DBProductTextI* rec) = 0;
			virtual int deleteAll(int type) = 0;
			virtual int deleteAllForDept(const std::string& deptNum) = 0;
			virtual int countAll(int type) = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRODUCTTEXTSRMI_H
