#include "sqliteromlabel.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteROMLabel::NUMBER_FIELD  ("RLbNum");
const string SqliteROMLabel::BINARY_FIELD  ("RLbBin");
const string SqliteROMLabel::ROTATION_FIELD("RLbRotation");


#ifdef USE_ACCOUNTANT
int SqliteROMLabel::_dataType_ = 0;
string SqliteROMLabel::_dataTypeName_("SqliteROMLabel");
#endif



SqliteROMLabel::SqliteROMLabel(SqliteRecord* r, DBROMLabelI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteROMLabel::~SqliteROMLabel()
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
