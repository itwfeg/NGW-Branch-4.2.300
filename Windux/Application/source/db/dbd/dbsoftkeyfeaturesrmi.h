#ifndef DBSOFTKEYFEATURESRMI_H
#define DBSOFTKEYFEATURESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBSoftKeyFeatureI;
	template <class DBEntityI> class DBQueryI;


	class DBSoftKeyFeaturesRMI
	{
		public:

			virtual ~DBSoftKeyFeaturesRMI() {}


			virtual DBQueryI<DBSoftKeyFeatureI>* retrieveAll() = 0;
			virtual DBQueryI<DBSoftKeyFeatureI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBSoftKeyFeatureI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum) = 0;

			virtual DBSoftKeyFeatureI* findNumber(const std::string& number, const std::string& deptNum) = 0;

			virtual DBSoftKeyFeatureI* createRecord(const std::string& number) = 0;
			virtual void saveRecord(const DBSoftKeyFeatureI* rec) = 0;
			virtual int deleteRecord(const DBSoftKeyFeatureI* rec) = 0;
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


#endif // DBSOFTKEYFEATURESRMI_H
