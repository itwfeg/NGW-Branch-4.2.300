#ifndef DBWRAPPERERRORHISTORYRMI_H
#define DBWRAPPERERRORHISTORYRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBWrapperErrorHistoryI;
	template <class DBEntityI> class DBQueryI;


	class DBWrapperErrorHistoryRMI
	{
		public:

			virtual ~DBWrapperErrorHistoryRMI() {}


			virtual DBQueryI<DBWrapperErrorHistoryI>* retrieveAll() = 0;

			virtual DBWrapperErrorHistoryI* find(const std::string& number) = 0;
			
			virtual DBWrapperErrorHistoryI* createRecord() = 0;
			virtual void saveRecord(const DBWrapperErrorHistoryI* rec) = 0;
			virtual int deleteRecord(const DBWrapperErrorHistoryI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBWRAPPERERRORHISTORYRMI_H
