#ifndef DBCOOLPRODUCTIONSTEPSRMI_H
#define DBCOOLPRODUCTIONSTEPSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolProductionStepI;
	class DBCoolCategoryProductionStepI;
	template <class DBEntityI> class DBQueryI;


	class DBCoolProductionStepsRMI
	{
		public:

			virtual ~DBCoolProductionStepsRMI() {}


			virtual DBQueryI<DBCoolProductionStepI>* retrieveAll() = 0;
			virtual DBQueryI<DBCoolProductionStepI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolProductionStepI>* retrieveAllCategories(const std::string& deptNum) = 0;

			virtual DBCoolProductionStepI* findProductionStep(const std::string& name, const std::string& deptNum) = 0;
			virtual DBCoolProductionStepI* find(const DBCoolCategoryProductionStepI* step) = 0;

			virtual DBCoolProductionStepI* createRecord() = 0;
			virtual void saveRecord(const DBCoolProductionStepI* rec) = 0;
			virtual int deleteRecord(const DBCoolProductionStepI* rec) = 0;
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


#endif // DBCOOLPRODUCTIONSTEPSRMI_H
