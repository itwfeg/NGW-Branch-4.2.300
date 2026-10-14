#include "edb.h"
#include DBOPERATOR_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBOperator::DBOperator(edb_adapter::DBOperatorI* d)
		: decorated_(d)
{
}


DBOperator::DBOperator(const DBOperator& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBOperator::~DBOperator()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBOperator& rec)
{
	os << "DBOperator # " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isNameNull() )
		os << "Name: " << rec.getName();
	if ( !rec.isRFIDNull() )
		os << "RFID: " << rec.getRFID();
	if ( !rec.isClassificationNull() )
		os << ", Classification: " << rec.getClassification();
	if ( !rec.isSawNoteNull() )
		os << ", SawNote: " << rec.getSawNote();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
