#include "sqlitecoolcountry.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCoolCountry::COUNTRYKEY_FIELD     ("LUID");
const string SqliteCoolCountry::DEPTNUM_FIELD        ("DeptNum");
const string SqliteCoolCountry::NAME1_FIELD          ("LUName1");
const string SqliteCoolCountry::NAME2_FIELD          ("LUName2");
const string SqliteCoolCountry::ALLCATEGORIES_FIELD  ("LUAllCa");
const string SqliteCoolCountry::COUNTRYNUM_FIELD     ("LUKey");


#ifdef USE_ACCOUNTANT
int SqliteCoolCountry::_dataType_ = 0;
string SqliteCoolCountry::_dataTypeName_("SqliteCoolCountry");
#endif



SqliteCoolCountry::SqliteCoolCountry(SqliteRecord* r, DBCoolCountryI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCoolCountry::~SqliteCoolCountry()
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
