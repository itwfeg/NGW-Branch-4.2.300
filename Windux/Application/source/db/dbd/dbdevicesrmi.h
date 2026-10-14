#ifndef DBDEVICESRMI_H
#define DBDEVICESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBDeviceI;
	template <class DBEntityI> class DBQueryI;


	class DBDevicesRMI
	{
		public:

			virtual ~DBDevicesRMI() {}


			virtual DBQueryI<DBDeviceI>* retrieveAll() = 0;
			virtual DBQueryI<DBDeviceI>* retrieveType(const std::string& type) = 0;
			virtual DBQueryI<DBDeviceI>* retrieveCheckPresence(bool check) = 0;

			virtual DBDeviceI* findTypeSubtype(const std::string& type, const std::string& subtype) = 0;

			virtual DBDeviceI* createRecord() = 0;
			virtual void saveRecord(const DBDeviceI* rec) = 0;
			virtual int deleteRecord(const DBDeviceI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBDEVICESRMI_H
