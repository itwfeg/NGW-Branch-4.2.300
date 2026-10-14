#ifndef DBSOFTKEYGROUPSRMI_H
#define DBSOFTKEYGROUPSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBSoftKeyGroupI;
	template <class DBEntityI> class DBQueryI;


	class DBSoftKeyGroupsRMI
	{
		public:

			virtual ~DBSoftKeyGroupsRMI() {}


			virtual DBQueryI<DBSoftKeyGroupI>* retrieveAll() = 0;
			virtual DBQueryI<DBSoftKeyGroupI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBSoftKeyGroupI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum) = 0;

			virtual DBSoftKeyGroupI* findNumber(const std::string& number, const std::string& deptNum) = 0;

			virtual DBSoftKeyGroupI* createRecord(const std::string& number) = 0;
			virtual void saveRecord(const DBSoftKeyGroupI* rec) = 0;
			virtual int deleteRecord(const DBSoftKeyGroupI* rec) = 0;
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


#endif // DBSOFTKEYGROUPSRMI_H
