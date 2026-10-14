#include "edb.h"
#include DBCONFIGURATION_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBConfiguration::DBConfiguration(edb_adapter::DBConfigurationI* d)
		: decorated_(d)
{
}


DBConfiguration::DBConfiguration(const DBConfiguration& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBConfiguration::~DBConfiguration()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBConfiguration& rec)
{
	os << "DBConfiguration " << rec.getName() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isValueNull() )
		os << ", Value: " << rec.getValue();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
