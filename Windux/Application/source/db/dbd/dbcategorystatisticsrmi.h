#ifndef DBCATEGORYSTATISTICSRMI_H
#define DBCATEGORYSTATISTICSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBCategoryStatisticI;
	class DBPresentationMethodI;
	class DBCategoryI;
	template <class DBEntityI> class DBQueryI;


	class DBCategoryStatisticsRMI
	{
		public:

			virtual ~DBCategoryStatisticsRMI() {}


			virtual DBQueryI<DBCategoryStatisticI>* retrieveAll() = 0;
			virtual DBQueryI<DBCategoryStatisticI>* retrieveLinkedtoCategory(const DBCategoryI* category) = 0;
			virtual DBQueryI<DBCategoryStatisticI>* retrieveMostSelected(const std::string& language, int hour) = 0;

			virtual DBCategoryStatisticI* find(const DBCategoryI* category, const std::string& language, const std::string& hour) = 0;

			virtual DBCategoryStatisticI* createRecord(const DBCategoryI* cat) = 0;
			virtual void saveRecord(const DBCategoryStatisticI* rec) = 0;
			virtual int deleteRecord(const DBCategoryStatisticI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBCATEGORYSTATISTICSRMI_H
