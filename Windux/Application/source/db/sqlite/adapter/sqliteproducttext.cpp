#include "sqliteproducttext.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteProductText::TYPE_FIELD    ("ProdTextType");
const string SqliteProductText::NUMBER_FIELD  ("ProdTextNum");
const string SqliteProductText::DEPTNUM_FIELD ("DeptNum");
const string SqliteProductText::TEXT_FIELD    ("ProdTextDesc");


#ifdef USE_ACCOUNTANT
int SqliteProductText::_dataType_ = 0;
string SqliteProductText::_dataTypeName_("SqliteProductText");
#endif



SqliteProductText::SqliteProductText(SqliteRecord* r, DBProductTextI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteProductText::~SqliteProductText()
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
