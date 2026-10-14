#include "edb.h"
#include "dbdefaultproduct.h"



#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
#endif


using namespace std;


DBDefaultProduct::DBDefaultProduct(edb_adapter::DBDefaultProductI* d) : decorated(d)
{
}


DBDefaultProduct::DBDefaultProduct(const DBDefaultProduct& p) : decorated(p.decorated->clone())
{
}


DBDefaultProduct::~DBDefaultProduct()
{
	if ( decorated )
		delete decorated;
}


std::ostream& operator<<(std::ostream& os, const DBDefaultProduct& rec)
{
	os << "DBDefaultProduct PresentationMethodType " << rec.getPresentationMethodType() <<
			", ProductKey " << rec.getProductKey() <<
			", LanguageType " << rec.getLanguageType() <<
			" (" << (rec.isRecordValid()?"valid":"not valid") << ") [";

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif

