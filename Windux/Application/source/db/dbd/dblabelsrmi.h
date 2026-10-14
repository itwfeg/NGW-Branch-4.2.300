#ifndef DBLABELSRMI_H
#define DBLABELSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBLabelI;
	template <class DBEntityI> class DBQueryI;


	class DBLabelsRMI
	{
		public:

			virtual ~DBLabelsRMI() {}


			virtual DBQueryI<DBLabelI>* retrieveAll() = 0;
			virtual DBQueryI<DBLabelI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBLabelI>* retrieveLabelNumListForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBLabelI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum) = 0;

			virtual DBLabelI* findNumber(const std::string& number, const std::string& deptNum) = 0;

			virtual DBLabelI* createRecord() = 0;
			virtual void saveRecord(const DBLabelI* rec) = 0;
			virtual int deleteRecord(const DBLabelI* rec) = 0;
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


#endif // DBLABELSRMI_H
