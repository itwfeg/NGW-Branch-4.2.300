#ifndef DBCOOLCATEGORIESRMI_H
#define DBCOOLCATEGORIESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolCategoryI;
	template <class DBEntityI> class DBQueryI;


	class DBCoolCategoriesRMI
	{
		public:

			virtual ~DBCoolCategoriesRMI() {}


			virtual DBQueryI<DBCoolCategoryI>* retrieveAll() = 0;
			virtual DBQueryI<DBCoolCategoryI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolCategoryI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum) = 0;

			virtual DBCoolCategoryI* findNumber(const std::string& number, const std::string& deptNum) = 0;

			virtual DBCoolCategoryI* createRecord() = 0;
			virtual void saveRecord(const DBCoolCategoryI* rec) = 0;
			virtual int deleteRecord(const DBCoolCategoryI* rec) = 0;
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


#endif // DBCOOLCATEGORIESRMI_H
