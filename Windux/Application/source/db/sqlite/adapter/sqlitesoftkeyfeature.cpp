#include "sqlitesoftkeyfeature.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteSoftKeyFeature::NUMBER_FIELD                     ("SFNum");
const string SqliteSoftKeyFeature::DEPTNUM_FIELD                    ("DeptNum");
const string SqliteSoftKeyFeature::SOFTKEYSTYLENUMBER_FIELD         ("SSNum");
const string SqliteSoftKeyFeature::DEFAULTGROUPNUMBER_FIELD         ("SGNum");
const string SqliteSoftKeyFeature::DEFAULTHORIZONTALGRIDSIZE_FIELD  ("SFDHGrid");
const string SqliteSoftKeyFeature::DEFAULTVERTICALGRIDSIZE_FIELD    ("SFDVGrid");
const string SqliteSoftKeyFeature::DEFAULTHORIZONTALSPACING_FIELD   ("SFDHSpc");
const string SqliteSoftKeyFeature::DEFAULTVERTICALSPACING_FIELD     ("SFDVSpc");
const string SqliteSoftKeyFeature::FIXEDROWCOLMETHOD_FIELD          ("SFFixedRowColMethod");
const string SqliteSoftKeyFeature::DEFAULTBUTTON_FIELD              ("SFDefaultButton");

#ifdef USE_ACCOUNTANT
int SqliteSoftKeyFeature::_dataType_ = 0;
string SqliteSoftKeyFeature::_dataTypeName_("SqliteSoftKeyFeature");
#endif



SqliteSoftKeyFeature::SqliteSoftKeyFeature(SqliteRecord* r, DBSoftKeyFeatureI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteSoftKeyFeature::~SqliteSoftKeyFeature()
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
