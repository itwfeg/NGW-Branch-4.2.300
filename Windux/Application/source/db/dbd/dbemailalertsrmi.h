#ifndef DBEMAILALERTSRMI_H
#define DBEMAILALERTSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBEmailAlertI;
	template <class DBEntityI> class DBQueryI;


	class DBEmailAlertsRMI
	{
		public:

			virtual ~DBEmailAlertsRMI() {}


			virtual DBQueryI<DBEmailAlertI>* retrieveAll() = 0;
			virtual DBQueryI<DBEmailAlertI>* retrieveRangeNumber(const std::string& first, const std::string& last) = 0;

			virtual DBEmailAlertI* findNumber(const std::string& number) = 0;

			virtual DBEmailAlertI* createRecord() = 0;
			virtual void saveRecord(const DBEmailAlertI* rec) = 0;
			virtual int deleteRecord(const DBEmailAlertI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBEMAILALERTSRMI_H
