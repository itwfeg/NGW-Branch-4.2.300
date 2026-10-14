#include "sqlitelabel.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteLabel::NUMBER_FIELD  ("LbNum");
const string SqliteLabel::DEPTNUM_FIELD ("DeptNum");
const string SqliteLabel::BINARY_FIELD  ("LbBin");
const string SqliteLabel::ROTATION_FIELD("LbRotation");


#ifdef USE_ACCOUNTANT
int SqliteLabel::_dataType_ = 0;
string SqliteLabel::_dataTypeName_("SqliteLabel");
#endif



SqliteLabel::SqliteLabel(SqliteRecord* r, DBLabelI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteLabel::~SqliteLabel()
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
