#ifndef DBPRESENTATIONMETHODDETAILEDSTATISTICSRMI_H
#define DBPRESENTATIONMETHODDETAILEDSTATISTICSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBPresentationMethodDetailedStatisticI;
	class DBPresentationMethodI;
	class DBPresentationMethodStatisticI;
	template <class DBEntityI> class DBQueryI;


	class DBPresentationMethodDetailedStatisticsRMI
	{
		public:

			virtual ~DBPresentationMethodDetailedStatisticsRMI() {}


			virtual DBQueryI<DBPresentationMethodDetailedStatisticI>* retrieveAll() = 0;
			virtual DBQueryI<DBPresentationMethodDetailedStatisticI>* retrieveLinkedtoPresentationMethodStatistic(const DBPresentationMethodStatisticI* stat) = 0;

			virtual DBPresentationMethodDetailedStatisticI* find(const DBPresentationMethodI* method, const DBPresentationMethodStatisticI* stat) = 0;

			virtual DBPresentationMethodDetailedStatisticI* createRecord(const DBPresentationMethodI* method, const DBPresentationMethodStatisticI* stat) = 0;
			virtual void saveRecord(const DBPresentationMethodDetailedStatisticI* rec) = 0;
			virtual int deleteRecord(const DBPresentationMethodDetailedStatisticI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRESENTATIONMETHODDETAILEDSTATISTICSRMI_H
