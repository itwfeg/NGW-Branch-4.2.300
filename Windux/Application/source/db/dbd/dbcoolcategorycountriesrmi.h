#ifndef DBCOOLCATEGORYCOUNTRIESRMI_H
#define DBCOOLCATEGORYCOUNTRIESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolCategoryCountryI;
	class DBCoolCountryI;
	template <class DBEntityI> class DBQueryI;


	class DBCoolCategoryCountriesRMI
	{
		public:

			virtual ~DBCoolCategoryCountriesRMI() {}


			virtual DBQueryI<DBCoolCategoryCountryI>* retrieveAll() = 0;
			virtual DBQueryI<DBCoolCategoryCountryI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolCategoryCountryI>* retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolCategoryCountryI>* retrieveLinkedtoCoolCountry(const DBCoolCountryI* country) = 0;

			virtual DBCoolCategoryCountryI* find(const std::string& category, const DBCoolCountryI* country) = 0;

			virtual DBCoolCategoryCountryI* createRecord(const std::string& category, const DBCoolCountryI* country) = 0;
			virtual void saveRecord(const DBCoolCategoryCountryI* rec) = 0;
			virtual int deleteRecord(const DBCoolCategoryCountryI* rec) = 0;
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


#endif // DBCOOLCATEGORYCOUNTRIESRMI_H
