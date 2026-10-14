#include "edb.h"
#include DBSOFTKEYGROUP_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSoftKeyGroup::DBSoftKeyGroup(edb_adapter::DBSoftKeyGroupI* d)
		: decorated_(d)
{
}


DBSoftKeyGroup::DBSoftKeyGroup(const DBSoftKeyGroup& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBSoftKeyGroup::~DBSoftKeyGroup()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBSoftKeyGroup& rec)
{
	os << "DBSoftKeyGroup Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isHorizontalGridSizeNull() )
		os << ", HorizontalGridSize: " << rec.getHorizontalGridSize();
	if ( !rec.isVerticalGridSizeNull() )
		os << ", VerticalGridSize: " << rec.getVerticalGridSize();
	if ( !rec.isHorizontalSpacingNull() )
		os << ", HorizontalSpacing: " << rec.getHorizontalSpacing();
	if ( !rec.isVerticalSpacingNull() )
		os << ", VerticalSpacing: " << rec.getVerticalSpacing();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
