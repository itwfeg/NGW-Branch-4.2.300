#ifndef DBROMLABELSRMI_H
#define DBROMLABELSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBROMLabelI;
	template <class DBEntityI> class DBQueryI;


	class DBROMLabelsRMI
	{
		public:

			virtual ~DBROMLabelsRMI() {}


			virtual DBQueryI<DBROMLabelI>* retrieveAll() = 0;
			virtual DBQueryI<DBROMLabelI>* retrieveRangeNumber(const std::string& first, const std::string& last) = 0;

			virtual DBROMLabelI* findNumber(const std::string& number) = 0;

			virtual DBROMLabelI* createRecord() = 0;
			virtual void saveRecord(const DBROMLabelI* rec) = 0;
			virtual int deleteRecord(const DBROMLabelI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBROMLABELSRMI_H
