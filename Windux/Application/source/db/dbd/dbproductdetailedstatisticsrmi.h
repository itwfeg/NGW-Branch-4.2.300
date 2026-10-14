#ifndef DBPRODUCTDETAILEDSTATISTICSRMI_H
#define DBPRODUCTDETAILEDSTATISTICSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBProductDetailedStatisticI;
	class DBPresentationMethodI;
	class DBProductStatisticI;
	template <class DBEntityI> class DBQueryI;


	class DBProductDetailedStatisticsRMI
	{
		public:

			virtual ~DBProductDetailedStatisticsRMI() {}


			virtual DBQueryI<DBProductDetailedStatisticI>* retrieveAll() = 0;
			virtual DBQueryI<DBProductDetailedStatisticI>* retrieveLinkedtoProductStatistic(const DBProductStatisticI* stat) = 0;

			virtual DBProductDetailedStatisticI* find(const DBPresentationMethodI* method, const DBProductStatisticI* stat) = 0;

			virtual DBProductDetailedStatisticI* createRecord() = 0;
			virtual void saveRecord(const DBProductDetailedStatisticI* rec) = 0;
			virtual int deleteRecord(const DBProductDetailedStatisticI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRODUCTDETAILEDSTATISTICSRMI_H
