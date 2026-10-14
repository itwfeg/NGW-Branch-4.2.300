#ifndef DBLABELTEXTSRMI_H
#define DBLABELTEXTSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBLabelTextI;
	template <class DBEntityI> class DBQueryI;


	class DBLabelTextsRMI
	{
		public:

			virtual ~DBLabelTextsRMI() {}


			virtual DBQueryI<DBLabelTextI>* retrieveAll() = 0;
			virtual DBQueryI<DBLabelTextI>* retrieveRangeNumber(const std::string& first, const std::string& last) = 0;

			virtual DBLabelTextI* findNumber(const std::string& number) = 0;

			virtual DBLabelTextI* createRecord() = 0;
			virtual void saveRecord(const DBLabelTextI* rec) = 0;
			virtual int deleteRecord(const DBLabelTextI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBLABELTEXTSRMI_H
