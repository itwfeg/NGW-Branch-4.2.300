#include "edb.h"
#include DBCATEGORY_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBCategory::DBCategory(edb_adapter::DBCategoryI* d)
		: decorated_(d)
{
}


DBCategory::DBCategory(const DBCategory& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBCategory::~DBCategory()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBCategory& rec)
{
	os << "DBCategory Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isParentCategoryNull() )
		os << ", ParentCategory: " << rec.getParentCategory();
	if ( !rec.isName1Null() )
		os << ", Name1: " << rec.getName1();
	if ( !rec.isName2Null() )
		os << ", Name2: " << rec.getName2();
	if ( !rec.isBackgroundColorNull() )
		os << ", BackgroundColor: " << rec.getBackgroundColor();
	if ( !rec.isTextColorNull() )
		os << ", TextColor: " << rec.getTextColor();
	if ( !rec.isImageNull() )
		os << ", Image: " << rec.getImage();
	if ( !rec.isPlaylistNull() )
		os << ", Playlist: " << rec.getPlaylist();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
