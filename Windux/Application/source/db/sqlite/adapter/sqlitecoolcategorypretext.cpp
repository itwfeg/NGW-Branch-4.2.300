#include "sqlitecoolcategorypretext.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCoolCategoryPreText::PRETEXTNUMBER_FIELD   ("LPNum");
const string SqliteCoolCategoryPreText::CATEGORYNUMBER_FIELD  ("LCNum");
const string SqliteCoolCategoryPreText::DEPTNUM_FIELD         ("DeptNum");


#ifdef USE_ACCOUNTANT
int SqliteCoolCategoryPreText::_dataType_ = 0;
string SqliteCoolCategoryPreText::_dataTypeName_("SqliteCoolCategoryPreText");
#endif



SqliteCoolCategoryPreText::SqliteCoolCategoryPreText(SqliteRecord* r, DBCoolCategoryPreTextI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCoolCategoryPreText::~SqliteCoolCategoryPreText()
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
