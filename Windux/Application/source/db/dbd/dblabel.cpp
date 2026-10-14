#include "edb.h"
#include DBLABEL_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBLabel::DBLabel(edb_adapter::DBLabelI* d)
		: decorated_(d)
{
}


DBLabel::DBLabel(const DBLabel& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBLabel::~DBLabel()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBLabel& rec)
{
	os << "DBLabel Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if (!rec.isRotationNull())
		os << ", Rotation: " << rec.getRotation();
	if ( !rec.isBinaryNull() )
		os << ", Name: " << rec.getBinary();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
