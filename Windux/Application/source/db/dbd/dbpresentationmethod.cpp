#include "edb.h"
#include DBPRESENTATIONMETHOD_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBPresentationMethod::DBPresentationMethod(edb_adapter::DBPresentationMethodI* d)
		: decorated_(d)
{
}


DBPresentationMethod::DBPresentationMethod(const DBPresentationMethod& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBPresentationMethod::~DBPresentationMethod()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBPresentationMethod& rec)
{
	os << "DBPresentationMethod Type " << rec.getType() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isDescriptionNull() )
		os << ", Description: " << rec.getDescription();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
