#include "edb.h"
#include DBKNIFETIMEHISTORY_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBKnifeTimeHistory::DBKnifeTimeHistory(edb_adapter::DBKnifeTimeHistoryI* d)
		: decorated_(d)
{
}


DBKnifeTimeHistory::DBKnifeTimeHistory(const DBKnifeTimeHistory& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBKnifeTimeHistory::~DBKnifeTimeHistory()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os,  DBKnifeTimeHistory& rec)
{

	os << "DBKnifeTimeHistory Type " << rec.getType() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isKnifeTimeNull() )
		os << "KnifeTime: " << rec.getKnifeTime();
	if ( !rec.isTimeStampDateTimeNull() )
		os << ", Date Time: " << rec.getTimeStampDateTime();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
