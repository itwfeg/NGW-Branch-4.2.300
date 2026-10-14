#ifndef DBDEFAULTPRODUCTSRMI_H
#define DBDEFAULTPRODUCTSRMI_H



#include <string>


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif

	class DBDefaultProductI;
	class DBPresentationMethodI;
	class DBProductI;
	template <class DBEntityI> class DBQueryI;


	class DBDefaultProductsRMI
	{
		public:
			virtual ~DBDefaultProductsRMI() {}

			virtual DBDefaultProductI* find(const DBPresentationMethodI* method, const DBProductI* prod, const std::string& lang) = 0;
			virtual DBQueryI<DBDefaultProductI>* retrieveAll() = 0;
			virtual DBQueryI<DBDefaultProductI>* retrieveLinkedtoPresentationMethod(const DBPresentationMethodI* method, const std::string& lang) = 0;

			virtual DBDefaultProductI* createRecord(const DBPresentationMethodI* method, const DBProductI* prod) = 0;
			virtual void saveRecord(const DBDefaultProductI* pds) = 0;
			virtual int deleteRecord(const DBDefaultProductI* pds) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};

#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif



#endif // DBDEFAULTPRODUCTSRMI_H

