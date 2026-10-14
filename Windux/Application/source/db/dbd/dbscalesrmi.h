#ifndef DBSCALESRMI_H
#define DBSCALESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBScaleI;
	template <class DBEntityI> class DBQueryI;


	class DBScalesRMI
	{
		public:

			virtual ~DBScalesRMI() {}


			virtual DBQueryI<DBScaleI>* retrieveAll() = 0;
			virtual DBQueryI<DBScaleI>* retrieveType(int type) = 0;

			virtual DBScaleI* findIdentifier(const std::string& identifier) = 0;
			virtual DBScaleI* findHostName(const std::string& hostname) = 0;
			virtual DBScaleI* findIPAddress(const std::string& ipAddress) = 0;
			virtual DBScaleI* findType(int type) = 0;

			virtual DBScaleI* createRecord() = 0;
			virtual void saveRecord(const DBScaleI* rec) = 0;
			virtual int deleteRecord(const DBScaleI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;

			virtual void ensureAutoIncrementValue(int value) = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSCALESRMI_H
