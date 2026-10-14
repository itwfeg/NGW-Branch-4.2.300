#ifndef DBRECALLSRMI_H
#define DBRECALLSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBRecallI;
	template <class DBEntityI> class DBQueryI;


	class DBRecallsRMI
	{
		public:

			virtual ~DBRecallsRMI() {}


			virtual DBQueryI<DBRecallI>* retrieveAll() = 0;
			virtual DBQueryI<DBRecallI>* retrieveAcknowledged() = 0;
			virtual DBQueryI<DBRecallI>* retrieveUnacknowledged() = 0;
			//virtual DBQueryI<DBRecallI>* retrieveRangeNumber(const std::string& first, const std::string& last) = 0;

			virtual DBRecallI* findNumber(const std::string& number) = 0;
			virtual DBRecallI* findName(const std::string& name) = 0;

			virtual DBRecallI* createRecord() = 0;
			virtual void saveRecord(const DBRecallI* rec) = 0;
			virtual int deleteRecord(const DBRecallI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBRECALLSRMI_H
