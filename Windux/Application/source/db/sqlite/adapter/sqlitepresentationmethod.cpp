#include "sqlitepresentationmethod.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqlitePresentationMethod::TYPE_FIELD          ("PMType");
const string SqlitePresentationMethod::DESCRIPTION1_FIELD  ("PMName1");
const string SqlitePresentationMethod::DESCRIPTION2_FIELD  ("PMName2");


#ifdef USE_ACCOUNTANT
int SqlitePresentationMethod::_dataType_ = 0;
string SqlitePresentationMethod::_dataTypeName_("SqlitePresentationMethod");
#endif



SqlitePresentationMethod::SqlitePresentationMethod(SqliteRecord* r, DBPresentationMethodI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqlitePresentationMethod::~SqlitePresentationMethod()
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
