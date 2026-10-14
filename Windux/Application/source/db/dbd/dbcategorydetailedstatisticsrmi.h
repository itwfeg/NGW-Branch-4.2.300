#ifndef DBCATEGORYDETAILEDSTATISTICSRMI_H
#define DBCATEGORYDETAILEDSTATISTICSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCategoryDetailedStatisticI;
	class DBPresentationMethodI;
	class DBCategoryStatisticI;
	template <class DBEntityI> class DBQueryI;


	class DBCategoryDetailedStatisticsRMI
	{
		public:

			virtual ~DBCategoryDetailedStatisticsRMI() {}


			virtual DBQueryI<DBCategoryDetailedStatisticI>* retrieveAll() = 0;
			virtual DBQueryI<DBCategoryDetailedStatisticI>* retrieveLinkedtoCategoryStatistic(const DBCategoryStatisticI* stat) = 0;

			virtual DBCategoryDetailedStatisticI* find(const DBPresentationMethodI* method, const DBCategoryStatisticI* stat) = 0;

			virtual DBCategoryDetailedStatisticI* createRecord(const DBPresentationMethodI* method, const DBCategoryStatisticI* stat) = 0;
			virtual void saveRecord(const DBCategoryDetailedStatisticI* rec) = 0;
			virtual int deleteRecord(const DBCategoryDetailedStatisticI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCATEGORYDETAILEDSTATISTICSRMI_H
