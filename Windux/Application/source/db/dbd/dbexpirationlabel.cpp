#include "edb.h"
#include DBEXPIRATIONLABEL_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBExpirationLabel::DBExpirationLabel(edb_adapter::DBExpirationLabelI* d)
		: decorated_(d)
{
}


DBExpirationLabel::DBExpirationLabel(const DBExpirationLabel& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBExpirationLabel::~DBExpirationLabel()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBExpirationLabel& rec)
{
	os << "DBExpirationLabel Sequence Number " << rec.getSequenceNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if (!rec.isDeptNumberNull())
		os << "Department: " << rec.getDeptNumber();
	if ( !rec.isPLUNull() )
		os << "PLU: " << rec.getPLU();
	if ( !rec.isProductDescriptionNull() )
		os << ", ProductDescription: " << rec.getProductDescription();
	if ( !rec.isExpirationNull() )
		os << ", Expiration: " << rec.getExpiration();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
