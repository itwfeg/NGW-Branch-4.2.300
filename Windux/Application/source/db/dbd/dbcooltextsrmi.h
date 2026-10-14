#ifndef DBCOOLTEXTSRMI_H
#define DBCOOLTEXTSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCoolTextI;
	template <class DBEntityI> class DBQueryI;


	class DBCoolTextsRMI
	{
		public:

			virtual ~DBCoolTextsRMI() {}


			virtual DBQueryI<DBCoolTextI>* retrieveAll() = 0;
			virtual DBQueryI<DBCoolTextI>* retrieveAllForDept(const std::string& deptNum) = 0;
			virtual DBQueryI<DBCoolTextI>* retrieveRangeNumber(const std::string& first, const std::string& last, const std::string& deptNum) = 0;

			virtual DBCoolTextI* findNumber(const std::string& number, const std::string& deptNum) = 0;

			virtual DBCoolTextI* createRecord() = 0;
			virtual void saveRecord(const DBCoolTextI* rec) = 0;
			virtual int deleteRecord(const DBCoolTextI* rec) = 0;
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


#endif // DBCOOLTEXTSRMI_H
