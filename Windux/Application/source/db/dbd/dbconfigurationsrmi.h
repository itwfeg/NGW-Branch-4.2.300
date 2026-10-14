#ifndef DBCONFIGURATIONSRMI_H
#define DBCONFIGURATIONSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBConfigurationI;
	template <class DBEntityI> class DBQueryI;


	class DBConfigurationsRMI
	{
		public:

			virtual ~DBConfigurationsRMI() {}


			virtual DBQueryI<DBConfigurationI>* retrieveAll() = 0;

			virtual DBConfigurationI* findName(const std::string& name) = 0;

			virtual DBConfigurationI* createRecord() = 0;
			virtual void saveRecord(const DBConfigurationI* rec) = 0;
			virtual int deleteRecord(const DBConfigurationI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCONFIGURATIONSRMI_H
