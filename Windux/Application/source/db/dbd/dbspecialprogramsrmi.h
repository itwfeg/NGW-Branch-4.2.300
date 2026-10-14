#ifndef DBSPECIALPROGRAMSRMI_H
#define DBSPECIALPROGRAMSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBSpecialProgramI;
	template <class DBEntityI> class DBQueryI;


	class DBSpecialProgramsRMI
	{
		public:

			virtual ~DBSpecialProgramsRMI() {}


			virtual DBQueryI<DBSpecialProgramI>* retrieveAll() = 0;
			virtual DBQueryI<DBSpecialProgramI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBSpecialProgramI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum) = 0;

			virtual DBSpecialProgramI* findNumber(const std::string& number, const std::string& deptNum) = 0;

			virtual DBSpecialProgramI* createRecord() = 0;
			virtual void saveRecord(const DBSpecialProgramI* rec) = 0;
			virtual int deleteRecord(const DBSpecialProgramI* rec) = 0;
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


#endif // DBSPECIALPROGRAMSRMI_H
