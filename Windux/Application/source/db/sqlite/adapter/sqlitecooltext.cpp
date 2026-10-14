#include "sqlitecooltext.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCoolText::NUMBER_FIELD    ("LXNum");
const string SqliteCoolText::DEPTNUM_FIELD   ("DeptNum");
const string SqliteCoolText::COOLTEXT_FIELD  ("LXDesc");


#ifdef USE_ACCOUNTANT
int SqliteCoolText::_dataType_ = 0;
string SqliteCoolText::_dataTypeName_("SqliteCoolText");
#endif



SqliteCoolText::SqliteCoolText(SqliteRecord* r, DBCoolTextI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCoolText::~SqliteCoolText()
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
