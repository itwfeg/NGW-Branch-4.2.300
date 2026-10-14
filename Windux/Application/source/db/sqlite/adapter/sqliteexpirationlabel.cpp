#include "sqliteexpirationlabel.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteExpirationLabel::SEQUENCENUMBER_FIELD      ("ELID");
const string SqliteExpirationLabel::DEPTNUM_FIELD("DeptNum");
const string SqliteExpirationLabel::PLU_FIELD                 ("ELPrNum");
const string SqliteExpirationLabel::PRODUCTDESCRIPTION_FIELD  ("ELPrDesc");
const string SqliteExpirationLabel::EXPIRATION_FIELD          ("ELExpDT");

														
#ifdef USE_ACCOUNTANT
int SqliteExpirationLabel::_dataType_ = 0;
string SqliteExpirationLabel::_dataTypeName_("SqliteExpirationLabel");
#endif



SqliteExpirationLabel::SqliteExpirationLabel(SqliteRecord* r, DBExpirationLabelI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteExpirationLabel::~SqliteExpirationLabel()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( rec_ )
		delete rec_;

	if ( decorated_ )
		delete decorated_;
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
