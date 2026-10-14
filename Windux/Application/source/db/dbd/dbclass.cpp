#include "edb.h"
#include DBCLASS_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBClass::DBClass(edb_adapter::DBClassI* d)
		: decorated_(d)
{
}


DBClass::DBClass(const DBClass& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBClass::~DBClass()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBClass& rec)
{
	os << "DBClass Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isReportingClassNull() )
		os << ", ReportingClass: " << rec.getReportingClass();
	if ( !rec.isDescriptionNull() )
		os << ", Description: " << rec.getDescription();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
