#ifndef DBPRESENTATIONMETHODSRMI_H
#define DBPRESENTATIONMETHODSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBPresentationMethodI;
	template <class DBEntityI> class DBQueryI;


	class DBPresentationMethodsRMI
	{
		public:

			virtual ~DBPresentationMethodsRMI() {}


			virtual DBQueryI<DBPresentationMethodI>* retrieveAll() = 0;

			virtual DBPresentationMethodI* find(const std::string& type) = 0;

			virtual DBPresentationMethodI* createRecord() = 0;
			virtual void saveRecord(const DBPresentationMethodI* rec) = 0;
			virtual int deleteRecord(const DBPresentationMethodI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBPRESENTATIONMETHODSRMI_H
