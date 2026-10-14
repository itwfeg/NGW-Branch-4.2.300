#ifndef DBPRESENTATIONMETHODSTATISTICSRMI_H
#define DBPRESENTATIONMETHODSTATISTICSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBPresentationMethodStatisticI;
	class DBPresentationMethodI;
	class DBPresentationMethodI;
	template <class DBEntityI> class DBQueryI;


	class DBPresentationMethodStatisticsRMI
	{
		public:

			virtual ~DBPresentationMethodStatisticsRMI() {}


			virtual DBQueryI<DBPresentationMethodStatisticI>* retrieveAll() = 0;
			virtual DBQueryI<DBPresentationMethodStatisticI>* retrieveLinkedtoPresentationMethod(const DBPresentationMethodI* method) = 0;

			virtual DBPresentationMethodStatisticI* find(const DBPresentationMethodI* method, const std::string& lang, const std::string& hour) = 0;

			virtual DBPresentationMethodStatisticI* createRecord(const DBPresentationMethodI* method) = 0;
			virtual void saveRecord(const DBPresentationMethodStatisticI* rec) = 0;
			virtual int deleteRecord(const DBPresentationMethodStatisticI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRESENTATIONMETHODSTATISTICSRMI_H
