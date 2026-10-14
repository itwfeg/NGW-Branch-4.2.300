#include "edb.h"
#include DBDEPT_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBDept::DBDept(edb_adapter::DBDeptI* d)
		: decorated_(d)
{
}


DBDept::DBDept(const DBDept& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBDept::~DBDept()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBDept& rec)
{
	os << "DBDept Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isName1Null() )
		os << ", Name1: " << rec.getName1();
	if ( !rec.isName2Null() )
		os << ", Name2: " << rec.getName2();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
