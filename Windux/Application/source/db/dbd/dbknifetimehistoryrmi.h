#ifndef DBKNIFETIMEHISTORYRMI_H
#define DBKNIFETIMEHISTORYRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBKnifeTimeHistoryI;
	template <class DBEntityI> class DBQueryI;


	class DBKnifeTimeHistoryRMI
	{
		public:

			virtual ~DBKnifeTimeHistoryRMI() {}


			virtual DBQueryI<DBKnifeTimeHistoryI>* retrieveAll() = 0;

			virtual DBKnifeTimeHistoryI* find(const std::string& number) = 0;
			
			virtual DBKnifeTimeHistoryI* createRecord() = 0;
			virtual void saveRecord(const DBKnifeTimeHistoryI* rec) = 0;
			virtual int deleteRecord(const DBKnifeTimeHistoryI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBKNIFETIMEHISTORYRMI_H
