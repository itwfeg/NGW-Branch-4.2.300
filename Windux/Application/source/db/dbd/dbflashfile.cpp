#include "edb.h"
#include DBFLASHFILE_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBFlashFile::DBFlashFile(edb_adapter::DBFlashFileI* d)
		: decorated_(d)
{
}


DBFlashFile::DBFlashFile(const DBFlashFile& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBFlashFile::~DBFlashFile()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBFlashFile& rec)
{
	os << "DBFlashFile " << rec.getName() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isDateNull() )
		os << ", Date: " << rec.getDate();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
