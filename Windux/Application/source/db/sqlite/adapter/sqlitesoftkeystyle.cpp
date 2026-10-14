#include "sqlitesoftkeystyle.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteSoftKeyStyle::NUMBER_FIELD                          ("SSNum");
const string SqliteSoftKeyStyle::DEFAULTTEXTLOCATION_FIELD             ("SSDTextL");
const string SqliteSoftKeyStyle::DEFAULTTEXTCOLOR_FIELD                ("SSDTextC");
const string SqliteSoftKeyStyle::DEFAULTGRAYEDOUTTEXTCOLOR_FIELD       ("SSDGrTxtC");
const string SqliteSoftKeyStyle::DEFAULTFOREGROUNDIMAGELOCATION_FIELD  ("SSDFImgL");
const string SqliteSoftKeyStyle::DEFAULTBACKGROUNDCOLOR_FIELD          ("SSDBgClr");
const string SqliteSoftKeyStyle::DEFAULTBACKGROUNDIMAGE_FIELD          ("SSDBgImg");
const string SqliteSoftKeyStyle::IMAGESDIRECTORY_FIELD                 ("SSImgDir");


#ifdef USE_ACCOUNTANT
int SqliteSoftKeyStyle::_dataType_ = 0;
string SqliteSoftKeyStyle::_dataTypeName_("SqliteSoftKeyStyle");
#endif



SqliteSoftKeyStyle::SqliteSoftKeyStyle(SqliteRecord* r, DBSoftKeyStyleI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteSoftKeyStyle::~SqliteSoftKeyStyle()
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
