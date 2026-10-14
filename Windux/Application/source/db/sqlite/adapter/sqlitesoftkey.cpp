#include "sqlitesoftkey.h"
#include "sqliteproduct.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteSoftKey::GROUPNUMBER_FIELD                 ("SGNum");
const string SqliteSoftKey::DEPTNUM_FIELD                     ("DeptNum");
const string SqliteSoftKey::HORIZONTALPOSITION_FIELD          ("SKHPos");
const string SqliteSoftKey::VERTICALPOSITION_FIELD            ("SKVPos");
const string SqliteSoftKey::ACTIONCATEGORYNUMBER_FIELD        ("CaNum");
const string SqliteSoftKey::ACTIONOPERATORNUMBER_FIELD        ("OpNum");
const string SqliteSoftKey::ACTIONFEATURENUMBER_FIELD         ("SFNum");
const string SqliteSoftKey::HORIZONTALSIZE_FIELD              ("SKHSize");
const string SqliteSoftKey::VERTICALSIZE_FIELD                ("SKVSize");
const string SqliteSoftKey::BACKGROUNDCOLOR_FIELD             ("SKBgClr");
const string SqliteSoftKey::TOGGLEDBACKGROUNDCOLOR_FIELD      ("SKToggledBgClr");
const string SqliteSoftKey::TEXTCOLOR_FIELD                   ("SKTxClr");
const string SqliteSoftKey::NAME1_FIELD                       ("SKName1");
const string SqliteSoftKey::NAME2_FIELD                       ("SKName2");
const string SqliteSoftKey::FORCENOBACKGROUNDCOLOR_FIELD      ("SKFNBCl");
const string SqliteSoftKey::FORCENOBACKGROUNDIMAGE_FIELD      ("SKFNBIm");
const string SqliteSoftKey::FORCENOTEXT_FIELD                 ("SKFNTxt");
const string SqliteSoftKey::LINKEDTOPRODUCTIONPLANNING_FIELD  ("SKLkPPl");
const string SqliteSoftKey::ACTION_FIELD                      ("SKActn");
const string SqliteSoftKey::ACTIONSUBTYPE_FIELD               ("SKActSb");
const string SqliteSoftKey::ACTIONPRODUCTNUMBER_FIELD         ("SKPrNum");
const string SqliteSoftKey::ACTIONINTEGERPARAMETER_FIELD      ("SKActPI");
const string SqliteSoftKey::ACTIONSTRINGPARAMETER_FIELD       ("SKActPS");
const string SqliteSoftKey::BACKGROUNDIMAGE_FIELD             ("SKBgImage");
const string SqliteSoftKey::SCRIPT_NAME_FIELD                 ("SKScript");
const string SqliteSoftKey::SCRIPT_INTERVAL_FIELD             ("SKScriptInterval");
const string SqliteSoftKey::TOGGLEDBACKGROUNDIMAGE_FIELD      ("SKToggledBgImage");


#ifdef USE_ACCOUNTANT
int SqliteSoftKey::_dataType_ = 0;
string SqliteSoftKey::_dataTypeName_("SqliteSoftKey");
#endif



SqliteSoftKey::SqliteSoftKey(SqliteRecord* r, DBSoftKeyI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteSoftKey::~SqliteSoftKey()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if ( rec_ )
		delete rec_;

	if ( decorated_ )
		delete decorated_;
}


void SqliteSoftKey::setActionProduct(const DBProductI* p)
{
	const SqliteProduct* product = dynamic_cast<const SqliteProduct*>(p);

	if ( product )
		rec_->setField(ACTIONPRODUCTNUMBER_FIELD, product->getPLU());
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
