#include "sqlitecategory.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteCategory::NUMBER_FIELD           ("CaNum");
const string SqliteCategory::DEPTNUM_FIELD          ("DeptNum");
const string SqliteCategory::PARENTCATEGORY_FIELD   ("Cat_CaNum");
const string SqliteCategory::NAME1_FIELD            ("CaName1");
const string SqliteCategory::NAME2_FIELD            ("CaName2");
const string SqliteCategory::CODEDNAME1_FIELD       ("CaCdNm1");
const string SqliteCategory::CODEDNAME2_FIELD       ("CaCdNm2");
const string SqliteCategory::BACKGROUNDCOLOR_FIELD  ("CaBgClr");
const string SqliteCategory::TEXTCOLOR_FIELD        ("CaTxClr");
const string SqliteCategory::IMAGE_FIELD            ("CaImage");
const string SqliteCategory::PLAYLIST_FIELD         ("CaPList");


#ifdef USE_ACCOUNTANT
int SqliteCategory::_dataType_ = 0;
string SqliteCategory::_dataTypeName_("SqliteCategory");
#endif



SqliteCategory::SqliteCategory(SqliteRecord* r, DBCategoryI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteCategory::~SqliteCategory()
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
