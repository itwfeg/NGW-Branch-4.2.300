#include "edb.h"
#include DBSOFTKEYSTYLE_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSoftKeyStyle::DBSoftKeyStyle(edb_adapter::DBSoftKeyStyleI* d)
		: decorated_(d)
{
}


DBSoftKeyStyle::DBSoftKeyStyle(const DBSoftKeyStyle& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBSoftKeyStyle::~DBSoftKeyStyle()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBSoftKeyStyle& rec)
{
	os << "DBSoftKeyStyle Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isDefaultTextLocationNull() )
		os << ", DefaultTextLocation: " << rec.getDefaultTextLocation();
	if ( !rec.isDefaultTextColorNull() )
		os << ", DefaultTextColor: " << rec.getDefaultTextColor();
	if ( !rec.isDefaultGrayedOutTextColorNull() )
		os << ", DefaultGrayedOutTextColor: " << rec.getDefaultGrayedOutTextColor();
	if ( !rec.isDefaultForegroundImageLocationNull() )
		os << ", DefaultForegroundImageLocation: " << rec.getDefaultForegroundImageLocation();
	if ( !rec.isDefaultBackgroundColorNull() )
		os << ", DefaultBackgroundColor: " << rec.getDefaultBackgroundColor();
	if ( !rec.isDefaultBackgroundImageNull() )
		os << ", DefaultBackgroundImage: " << rec.getDefaultBackgroundImage();
	if ( !rec.isImagesDirectoryNull() )
		os << ", ImagesDirectory: " << rec.getImagesDirectory();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
