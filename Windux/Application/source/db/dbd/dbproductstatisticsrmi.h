#ifndef DBPRODUCTSTATISTICSRMI_H
#define DBPRODUCTSTATISTICSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductStatisticI;
	class DBPresentationMethodI;
	class DBProductI;
	template <class DBEntityI> class DBQueryI;


	class DBProductStatisticsRMI
	{
		public:

			virtual ~DBProductStatisticsRMI() {}


			virtual DBQueryI<DBProductStatisticI>* retrieveAll() = 0;
			virtual DBQueryI<DBProductStatisticI>* retrieveLinkedtoProduct(const DBProductI* prod) = 0;
			virtual DBQueryI<DBProductStatisticI>* retrieveMostSelected(const std::string& language, int hour) = 0;
			virtual DBQueryI<DBProductStatisticI>* retrieveMostPrinted(const std::string& language, int hour) = 0;
			virtual DBQueryI<DBProductStatisticI>* retrieveDateTimeOrder() = 0;

			virtual DBProductStatisticI* find(const DBProductI* prod, const std::string& language, const std::string& hour) = 0;

			virtual DBProductStatisticI* createRecord(const DBProductI* prod) = 0;
			virtual void saveRecord(const DBProductStatisticI* rec) = 0;
			virtual int deleteRecord(const DBProductStatisticI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRODUCTSTATISTICSRMI_H
