#include "edb.h"
#include DBTRANSACTIONTYPE_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBTransactionType::DBTransactionType(edb_adapter::DBTransactionTypeI* d)
		: decorated_(d)
{
}


DBTransactionType::DBTransactionType(const DBTransactionType& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBTransactionType::~DBTransactionType()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBTransactionType& rec)
{
	os << "DBTransactionType # " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isName1Null() )
		os << ", Name1: " << rec.getName1();
	if ( !rec.isName2Null() )
		os << ", Name2: " << rec.getName2();
	if ( !rec.isAffectingProductionNull() )
		os << ", AffectingProduction: " << rec.getAffectingProduction();
	if ( !rec.isAllowingBackDatingNull() )
		os << ", AllowingBackDating: " << (rec.getAllowingBackDating()?'Y':'N');
	if ( !rec.isManuallyChangingUPCDigitNull() )
		os << ", ManuallyChangingUPCDigit: " << (rec.getManuallyChangingUPCDigit()?'Y':'N');
	if ( !rec.isAutomaticallyChangingUPCDigitNull() )
		os << ", AutomaticallyChangingUPCDigit: " << (rec.getAutomaticallyChangingUPCDigit()?'Y':'N');
	if ( !rec.isUPCDigitNull() )
		os << ", UPCDigit: " << rec.getUPCDigit();
	if (!rec.isUseBackgroundColorNull())
		os << ", UseBackgroundColor: " << (rec.getUseBackgroundColor() ? 'Y' : 'N');
	if (!rec.isBackgroundColorNull())
		os << ", BackgroundColor: " << rec.getBackgroundColor();
	if (!rec.isTextForLabelNull())
		os << ", TextForLabel: " << rec.getTextForLabel();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
