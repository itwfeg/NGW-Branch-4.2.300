#include "edb.h"
#include DBROMLABEL_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBROMLabel::DBROMLabel(edb_adapter::DBROMLabelI* d)
		: decorated_(d)
{
}


DBROMLabel::DBROMLabel(const DBROMLabel& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBROMLabel::~DBROMLabel()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBROMLabel& rec)
{
	os << "DBROMLabel Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isRotationNull())
		os << "Rotation: " << rec.getRotation();
	if ( !rec.isBinaryNull() )
		os << ", Name: " << rec.getBinary();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
