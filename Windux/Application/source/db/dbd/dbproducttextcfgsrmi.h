#ifndef DBPRODUCTTEXTCFGSRMI_H
#define DBPRODUCTTEXTCFGSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductTextCfgI;
	template <class DBEntityI> class DBQueryI;


	class DBProductTextCfgsRMI
	{
		public:

			virtual ~DBProductTextCfgsRMI() {}


			virtual DBQueryI<DBProductTextCfgI>* retrieveAll() = 0;

			virtual DBProductTextCfgI* findNumber(int type = 0) = 0;

			virtual DBProductTextCfgI* createRecord(int type = 0) = 0;
			virtual void saveRecord(const DBProductTextCfgI* rec) = 0;
			virtual int deleteRecord(const DBProductTextCfgI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRODUCTTEXTCFGSRMI_H
