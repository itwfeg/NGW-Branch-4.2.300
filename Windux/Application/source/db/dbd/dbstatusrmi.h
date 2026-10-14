#ifndef DBSTATUSRMI_H
#define DBSTATUSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBStatusI;
	template <class DBEntityI> class DBQueryI;


	class DBStatusRMI
	{
		public:

			virtual ~DBStatusRMI() {}


			virtual DBQueryI<DBStatusI>* retrieveAll() = 0;

			virtual DBStatusI* findName(const std::string& name) = 0;

			virtual DBStatusI* createRecord() = 0;
			virtual void saveRecord(const DBStatusI* rec) = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSTATUSRMI_H
