#ifndef DBCOOLCOUNTRIESRMI_H
#define DBCOOLCOUNTRIESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolCountryI;
	class DBCoolCategoryCountryI;
	template <class DBEntityI> class DBQueryI;


	class DBCoolCountriesRMI
	{
		public:

			virtual ~DBCoolCountriesRMI() {}


			virtual DBQueryI<DBCoolCountryI>* retrieveAll() = 0;
			virtual DBQueryI<DBCoolCountryI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolCountryI>* retrieveAllCategories(const std::string& deptNum) = 0;

			virtual DBCoolCountryI* findCountry(const std::string& name, const std::string& deptNum) = 0;
			virtual DBCoolCountryI* find(const DBCoolCategoryCountryI* country) = 0;

			virtual DBCoolCountryI* createRecord() = 0;
			virtual void saveRecord(const DBCoolCountryI* rec) = 0;
			virtual int deleteRecord(const DBCoolCountryI* rec) = 0;
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


#endif // DBCOOLCOUNTRIESRMI_H
