#include "edb.h"
#include DBSOFTKEYFEATURE_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSoftKeyFeature::DBSoftKeyFeature(edb_adapter::DBSoftKeyFeatureI* d)
		: decorated_(d)
{
}


DBSoftKeyFeature::DBSoftKeyFeature(const DBSoftKeyFeature& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBSoftKeyFeature::~DBSoftKeyFeature()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBSoftKeyFeature& rec)
{
	os << "DBSoftKeyFeature Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isDefaultGroupNumberNull() )
		os << ", DefaultGroupNumber: " << rec.getDefaultGroupNumber();
	if ( !rec.isDefaultHorizontalGridSizeNull() )
		os << ", DefaultHorizontalGridSize: " << rec.getDefaultHorizontalGridSize();
	if ( !rec.isDefaultVerticalGridSizeNull() )
		os << ", DefaultVerticalGridSize: " << rec.getDefaultVerticalGridSize();
	if ( !rec.isDefaultHorizontalSpacingNull() )
		os << ", DefaultHorizontalSpacing: " << rec.getDefaultHorizontalSpacing();
	if ( !rec.isDefaultVerticalSpacingNull() )
		os << ", DefaultVerticalSpacing: " << rec.getDefaultVerticalSpacing();
	if ( !rec.isSoftKeyStyleNumberNull() )
		os << ", SoftKeyStyleNumber: " << rec.getSoftKeyStyleNumber();
	if ( !rec.isFixedRowColMethodNull() )
		os << ", FixedRowColMethod: " << rec.getFixedRowColMethod();
	if ( !rec.isDefaultButtonNull() )
		os << ", DefaultButton: " << rec.getDefaultButton();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
