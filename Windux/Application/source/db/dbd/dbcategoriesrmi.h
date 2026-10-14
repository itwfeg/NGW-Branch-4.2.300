#ifndef DBCATEGORIESRMI_H
#define DBCATEGORIESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCategoryI;
	template <class DBEntityI> class DBQueryI;


	class DBCategoriesRMI
	{
		public:

			virtual ~DBCategoriesRMI() {}


			virtual DBQueryI<DBCategoryI>* retrieveAll() = 0;
			virtual DBQueryI<DBCategoryI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCategoryI>* retrieveAllByName() = 0;
			virtual DBQueryI<DBCategoryI>* retrieveAllByName(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCategoryI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum) = 0;
			virtual DBQueryI<DBCategoryI>* retrieveChildCategories(const std::string& ca, const std::string& deptNum) = 0;
			virtual DBQueryI<DBCategoryI>* retrieveAllSubcategories(const std::string& deptNum) = 0;

			virtual DBCategoryI* findNumber(const std::string& number, const std::string& deptNum) = 0;

			virtual DBCategoryI* createRecord() = 0;
			virtual void saveRecord(const DBCategoryI* rec) = 0;
			virtual int deleteRecord(const DBCategoryI* rec) = 0;
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


#endif // DBCATEGORIESRMI_H
