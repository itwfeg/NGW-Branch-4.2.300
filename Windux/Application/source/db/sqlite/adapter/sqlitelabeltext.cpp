#include "sqlitelabeltext.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteLabelText::NUMBER_FIELD  ("LTNum");
const string SqliteLabelText::TEXT_FIELD    ("LTText");


#ifdef USE_ACCOUNTANT
int SqliteLabelText::_dataType_ = 0;
string SqliteLabelText::_dataTypeName_("SqliteLabelText");
#endif



SqliteLabelText::SqliteLabelText(SqliteRecord* r, DBLabelTextI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

}


SqliteLabelText::~SqliteLabelText()
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
