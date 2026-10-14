#ifndef DBCOOLPRETEXTSRMI_H
#define DBCOOLPRETEXTSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolPreTextI;
	class DBCoolCategoryI;
	template <class DBEntityI> class DBQueryI;


	class DBCoolPreTextsRMI
	{
		public:

			virtual ~DBCoolPreTextsRMI() {}


			virtual DBQueryI<DBCoolPreTextI>* retrieveAll() = 0;
			virtual DBQueryI<DBCoolPreTextI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolPreTextI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolPreTextI>* retrieveAllCategories(const std::string& deptNum) = 0;

			virtual DBCoolPreTextI* findNumber(const std::string& number, const std::string& deptNum) = 0;

			virtual DBCoolPreTextI* createRecord() = 0;
			virtual void saveRecord(const DBCoolPreTextI* rec) = 0;
			virtual int deleteRecord(const DBCoolPreTextI* rec) = 0;
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


#endif // DBCOOLPRETEXTSRMI_H
