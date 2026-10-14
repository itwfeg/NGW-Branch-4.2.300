#ifndef DBNETWORKERRORSRMI_H
#define DBNETWORKERRORSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBNetworkErrorI;
	template <class DBEntityI> class DBQueryI;


	class DBNetworkErrorsRMI
	{
		public:

			virtual ~DBNetworkErrorsRMI() {}


			virtual DBQueryI<DBNetworkErrorI>* retrieveAll() = 0;
//			virtual DBQueryI<DBNetworkErrorI>* retrieveRangeNumber(const std::string& first, const std::string& last) = 0;

			virtual DBNetworkErrorI* findID(const std::string& id) = 0;

			virtual DBNetworkErrorI* createRecord() = 0;
			virtual void saveRecord(const DBNetworkErrorI* rec) = 0;
			virtual int deleteRecord(const DBNetworkErrorI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBNETWORKERRORSRMI_H
