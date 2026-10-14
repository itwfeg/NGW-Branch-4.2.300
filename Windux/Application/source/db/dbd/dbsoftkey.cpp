#include "edb.h"
#include DBSOFTKEY_INC
#include DBPRODUCT_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSoftKey::DBSoftKey(edb_adapter::DBSoftKeyI* d)
		: decorated_(d)
{
}


DBSoftKey::DBSoftKey(const DBSoftKey& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBSoftKey::~DBSoftKey()
{
	if ( decorated_ )
		delete decorated_;
}


void DBSoftKey::setActionProduct(const DBProduct* p)
{
	if ( decorated_ && p )
		decorated_->setActionProduct(p->decorated_);
}


std::ostream& operator<<(std::ostream& os, const DBSoftKey& rec)
{
	os << "DBSoftKey GroupNumber " << rec.getGroupNumber() << 
			", HorizontalPosition " << rec.getHorizontalPosition() << 
			", VerticalPosition " << rec.getVerticalPosition() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << ", Department: " << rec.getDeptNumber();
	if ( !rec.isHorizontalSizeNull() )
		os << ", HorizontalSize: " << rec.getHorizontalSize();
	if ( !rec.isVerticalSizeNull() )
		os << ", VerticalSize: " << rec.getVerticalSize();
	if ( !rec.isTextColorNull() )
		os << ", TextColor: " << rec.getTextColor();
	if ( !rec.isBackgroundColorNull() )
		os << ", BackgroundColor: " << rec.getBackgroundColor();
	if (!rec.isToggledBackgroundColorNull())
		os << ", ToggledBackgroundColor: " << rec.getToggledBackgroundColor();
	if ( !rec.isBackgroundImageNull() )
		os << ", BackgroundImage: " << rec.getBackgroundImage();
	if (!rec.isToggledBackgroundImageNull())
		os << ", ToggledBackgroundImage: " << rec.getToggledBackgroundImage();
	if ( !rec.isName1Null() )
		os << ", Name1: " << rec.getName1();
	if ( !rec.isName2Null() )
		os << ", Name2: " << rec.getName2();
	if ( !rec.isForceNoBackgroundColorNull() )
		os << ", ForceNoBackgroundColor: " << rec.getForceNoBackgroundColor();
	if ( !rec.isForceNoBackgroundImageNull() )
		os << ", ForceNoBackgroundImage: " << rec.getForceNoBackgroundImage();
	if ( !rec.isForceNoTextNull() )
		os << ", ForceNoText: " << rec.getForceNoText();
	if ( !rec.isLinkedToProductionPlanningNull() )
		os << ", LinkedToProductionPlanning: " << rec.getLinkedToProductionPlanning();
	if ( !rec.isActionNull() )
		os << ", Action: " << rec.getAction();
	if ( !rec.isActionSubtypeNull() )
		os << ", ActionSubtype: " << rec.getActionSubtype();
	if ( !rec.isActionIntegerParameterNull() )
		os << ", ActionIntegerParameter: " << rec.getActionIntegerParameter();
	if ( !rec.isActionStringParameterNull() )
		os << ", ActionStringParameter: " << rec.getActionStringParameter();
	if ( !rec.isActionProductNumberNull() )
		os << ", ActionProductNumber: " << rec.getActionProductNumber();
	if ( !rec.isActionCategoryNumberNull() )
		os << ", ActionCategoryNumber: " << rec.getActionCategoryNumber();
	if ( !rec.isActionOperatorNumberNull() )
		os << ", ActionOperatorNumber: " << rec.getActionOperatorNumber();
	if ( !rec.isActionFeatureNumberNull() )
		os << ", ActionFeatureNumber: " << rec.getActionFeatureNumber();
	if (!rec.isActionFeatureNumberNull())
		os << ", ActionFeatureNumber: " << rec.getActionFeatureNumber();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
