#include "sqlitecoollisttext.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCoolListText::COOLLISTKEY_FIELD     ("LLID");
const string SqliteCoolListText::DEPTNUM_FIELD         ("DeptNum");
const string SqliteCoolListText::COOLTEXTNUMBER_FIELD  ("LXNum");
const string SqliteCoolListText::ORDER_FIELD           ("XLOrder");
const string SqliteCoolListText::LINKNUMBER_FIELD      ("XLLink");
const string SqliteCoolListText::NEXTLINKNUMBER_FIELD  ("XLNLink");


#ifdef USE_ACCOUNTANT
int SqliteCoolListText::_dataType_ = 0;
string SqliteCoolListText::_dataTypeName_("SqliteCoolListText");
#endif



SqliteCoolListText::SqliteCoolListText(SqliteRecord* r, DBCoolListTextI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCoolListText::~SqliteCoolListText()
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
