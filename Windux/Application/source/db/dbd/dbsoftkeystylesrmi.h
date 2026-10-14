#ifndef DBSOFTKEYSTYLESRMI_H
#define DBSOFTKEYSTYLESRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBSoftKeyStyleI;
	template <class DBEntityI> class DBQueryI;


	class DBSoftKeyStylesRMI
	{
		public:

			virtual ~DBSoftKeyStylesRMI() {}


			virtual DBQueryI<DBSoftKeyStyleI>* retrieveAll() = 0;
			virtual DBQueryI<DBSoftKeyStyleI>* retrieveRangeNumber(const std::string& first, const std::string& last) = 0;

			virtual DBSoftKeyStyleI* findNumber(const std::string& number) = 0;

			virtual DBSoftKeyStyleI* createRecord(const std::string& number) = 0;
			virtual void saveRecord(const DBSoftKeyStyleI* rec) = 0;
			virtual int deleteRecord(const DBSoftKeyStyleI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBSOFTKEYSTYLESRMI_H
