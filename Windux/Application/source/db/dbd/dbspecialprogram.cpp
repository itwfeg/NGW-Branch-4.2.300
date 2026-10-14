#include "edb.h"
#include DBSPECIALPROGRAM_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBSpecialProgram::DBSpecialProgram(edb_adapter::DBSpecialProgramI* d)
		: decorated_(d)
{
}


DBSpecialProgram::DBSpecialProgram(const DBSpecialProgram& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBSpecialProgram::~DBSpecialProgram()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBSpecialProgram& rec)
{
	os << "DBSpecialProgram Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isDeptNumberNull() )
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isDescriptionNull() )
		os << ", Description: " << rec.getDescription();
	if ( !rec.isDefaultLengthNull() )
		os << ", DefaultLength: " << rec.getDefaultLength();
	if ( !rec.isImage1Null() )
		os << ", Image1: " << rec.getImage1();
	if ( !rec.isImage2Null() )
		os << ", Image2: " << rec.getImage2();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
