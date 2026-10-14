#ifndef DBCOOLCATEGORYPRODUCTIONSTEPSRMI_H
#define DBCOOLCATEGORYPRODUCTIONSTEPSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolCategoryProductionStepI;
	class DBCoolProductionStepI;
	template <class DBEntityI> class DBQueryI;


	class DBCoolCategoryProductionStepsRMI
	{
		public:

			virtual ~DBCoolCategoryProductionStepsRMI() {}


			virtual DBQueryI<DBCoolCategoryProductionStepI>* retrieveAll() = 0;
			virtual DBQueryI<DBCoolCategoryProductionStepI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolCategoryProductionStepI>* retrieveLinkedtoCoolCategory(const std::string& category, const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolCategoryProductionStepI>* retrieveLinkedtoCoolProductionStep(const DBCoolProductionStepI* step) = 0;

			virtual DBCoolCategoryProductionStepI* find(const std::string& category, const std::string& deptNum, const DBCoolProductionStepI* step) = 0;

			virtual DBCoolCategoryProductionStepI* createRecord(const std::string& category, const DBCoolProductionStepI* step) = 0;
			virtual void saveRecord(const DBCoolCategoryProductionStepI* rec) = 0;
			virtual int deleteRecord(const DBCoolCategoryProductionStepI* rec) = 0;
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


#endif // DBCOOLCATEGORYPRODUCTIONSTEPSRMI_H
