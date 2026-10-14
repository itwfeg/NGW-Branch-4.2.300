#include "sqliteclass.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteClass::NUMBER_FIELD          ("ClNum");
const string SqliteClass::DEPTNUM_FIELD         ("DeptNum");
const string SqliteClass::REPORTINGCLASS_FIELD  ("Cla_ClNum");
const string SqliteClass::DESCRIPTION1_FIELD    ("ClDesc1");
const string SqliteClass::DESCRIPTION2_FIELD    ("ClDesc2");


#ifdef USE_ACCOUNTANT
int SqliteClass::_dataType_ = 0;
string SqliteClass::_dataTypeName_("SqliteClass");
#endif



SqliteClass::SqliteClass(SqliteRecord* r, DBClassI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteClass::~SqliteClass()
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
