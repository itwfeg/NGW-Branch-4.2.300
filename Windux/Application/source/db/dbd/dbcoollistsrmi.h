#ifndef DBCOOLLISTSRMI_H
#define DBCOOLLISTSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolListI;
	class DBCoolListTextI;
	class DBProductI;
	template <class DBEntityI> class DBQueryI;


	class DBCoolListsRMI
	{
		public:

			virtual ~DBCoolListsRMI() {}


			virtual DBQueryI<DBCoolListI>* retrieveAll() = 0;
			virtual DBQueryI<DBCoolListI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolListI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum) = 0;

			virtual DBCoolListI* findNumber(const std::string& number, const std::string& deptNum) = 0;
			virtual DBCoolListI* find(const DBProductI* prod) = 0;
			virtual DBCoolListI* find(const DBCoolListTextI* text) = 0;
			virtual DBCoolListI* findFirstLink(const std::string& link) = 0;

			virtual DBCoolListI* createRecord() = 0;
			virtual void saveRecord(const DBCoolListI* rec) = 0;
			virtual int deleteRecord(const DBCoolListI* rec) = 0;
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


#endif // DBCOOLLISTSRMI_H
