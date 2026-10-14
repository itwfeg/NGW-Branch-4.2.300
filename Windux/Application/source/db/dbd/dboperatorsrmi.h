#ifndef DBOPERATORSRMI_H
#define DBOPERATORSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBOperatorI;
	template <class DBEntityI> class DBQueryI;


	class DBOperatorsRMI
	{
		public:

			virtual ~DBOperatorsRMI() {}


			virtual DBQueryI<DBOperatorI>* retrieveAll() = 0;
			virtual DBQueryI<DBOperatorI>* retrieveRangeNumber(const std::string& first, const std::string& last) = 0;

			virtual DBOperatorI* findNumber(const std::string& number) = 0;
			virtual DBOperatorI* findRFID(const std::string& id) = 0;

			virtual DBOperatorI* createRecord() = 0;
			virtual void saveRecord(const DBOperatorI* rec) = 0;
			virtual int deleteRecord(const DBOperatorI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBOPERATORSRMI_H
