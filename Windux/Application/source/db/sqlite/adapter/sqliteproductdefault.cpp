#include "sqliteproductdefault.h"

#include ACCOUNTANTI_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteProductDefault::DEPTNUM_FIELD                 ("DeptNum");
const string SqliteProductDefault::TYPE_FIELD                    ("PDType");
const string SqliteProductDefault::PRICE_FIELD                   ("PrPrice");
const string SqliteProductDefault::PRICEFACTOR_FIELD             ("PrPrMod");
const string SqliteProductDefault::FORCEPRICE_FIELD              ("PrIsFrcPr");
const string SqliteProductDefault::EXCEPTIONPRICE_FIELD          ("PrExcPr");
const string SqliteProductDefault::DISCOUNTMETHOD_FIELD          ("PrDscMe");
const string SqliteProductDefault::DISCOUNTPRICE1_FIELD          ("PrDscPr1");
const string SqliteProductDefault::DISCOUNTPRICE2_FIELD          ("PrDscPr2");
const string SqliteProductDefault::DISCOUNTPRICE3_FIELD          ("PrDscPr3");
const string SqliteProductDefault::TARE_FIELD                    ("PrTare");
const string SqliteProductDefault::FORCETARE_FIELD               ("PrIsFrcTr");
const string SqliteProductDefault::PROPORTIONALTARE_FIELD        ("PrPrpTare");
const string SqliteProductDefault::NETWEIGHT_FIELD               ("PrNetWt");
const string SqliteProductDefault::BYCOUNT_FIELD                 ("PrByCnt");
const string SqliteProductDefault::FORCEBYCOUNT_FIELD            ("PrIsFrcBC");
const string SqliteProductDefault::SHELFLIFEDAYS_FIELD           ("PrShlfLfD");
const string SqliteProductDefault::SHELFLIFEHOURS_FIELD          ("PrShlfLfH");
const string SqliteProductDefault::SHELFLIFEMINUTES_FIELD        ("PrShlfLfM");
const string SqliteProductDefault::PRODUCTLIFEDAYS_FIELD         ("PrProdLfD");
const string SqliteProductDefault::PRODUCTLIFEHOURS_FIELD        ("PrProdLfH");
const string SqliteProductDefault::PRODUCTLIFEMINUTES_FIELD      ("PrProdLfM");
const string SqliteProductDefault::DONTPRINTSHELFLIFE_FIELD      ("PrIsPrnSL");
const string SqliteProductDefault::DONTPRINTPRODUCTLIFE_FIELD    ("PrIsPrnPL");
const string SqliteProductDefault::DONTPRINTPACKEDON_FIELD       ("PrIsPrnPO");
const string SqliteProductDefault::BONUSPOINTS_FIELD             ("PrBnsPts");
const string SqliteProductDefault::PORTIONS_FIELD                ("PrPrtns");
const string SqliteProductDefault::APPLYSECURITYLABEL_FIELD      ("PrIsUsSL");
const string SqliteProductDefault::INTAKEMODE_FIELD              ("PrInMode");
const string SqliteProductDefault::LABEL1ROTATION_FIELD          ("PrLblRot1");
const string SqliteProductDefault::LABEL1PLACEMENT_FIELD         ("PrLblPlc1");
const string SqliteProductDefault::LABEL2ROTATION_FIELD          ("PrLblRot2");
const string SqliteProductDefault::LABEL2PLACEMENT_FIELD         ("PrLblPlc2");
const string SqliteProductDefault::LABEL3ROTATION_FIELD          ("PrLblRot3");
const string SqliteProductDefault::LABEL3PLACEMENT_FIELD         ("PrLblPlc3");
const string SqliteProductDefault::NAME1_FIELD                   ("PrName1");
const string SqliteProductDefault::NAME2_FIELD                   ("PrName2");
const string SqliteProductDefault::VERIFYCOOL_FIELD              ("PrVCOLTxt");
const string SqliteProductDefault::COOLREQUIRED_FIELD            ("PrCOLReq");
const string SqliteProductDefault::PRODUCTIONPLANNINGTYPE_FIELD  ("PrPrPlT");
const string SqliteProductDefault::REPORTINGCLASS_FIELD          ("PDClNum");
const string SqliteProductDefault::TEXT_FIELD                    ("PDTxNum");
const string SqliteProductDefault::SPECIALMESSAGE_FIELD          ("PDSMNum");
const string SqliteProductDefault::NUTRITION_FIELD               ("PDNFNum");
const string SqliteProductDefault::MARQUEE_FIELD                 ("PDMaNum");
const string SqliteProductDefault::NOTE_FIELD                    ("PDNtNum");
const string SqliteProductDefault::CATEGORY_FIELD                ("PDCaNum");
const string SqliteProductDefault::LABEL1_FIELD                  ("PDLbNum1");
const string SqliteProductDefault::LABEL2_FIELD                  ("PDLbNum2");
const string SqliteProductDefault::LABEL3_FIELD                  ("PDLbNum3");
const string SqliteProductDefault::SPECIALTYPE_FIELD             ("PDSPNum");
const string SqliteProductDefault::COOLCATEGORYNUMBER_FIELD      ("PDLCNum");
const string SqliteProductDefault::DEFAULTCOOLTEXTNUMBER_FIELD   ("PDDXNum");
const string SqliteProductDefault::COOLPRETEXTNUMBER_FIELD       ("PDLPNum");
const string SqliteProductDefault::COOLLISTNUMBER_FIELD          ("PDLLNum");
const string SqliteProductDefault::UPCNUMBERSYSTEM_FIELD         ("PDUPCNS");
const string SqliteProductDefault::EANFORMATSTRING_FIELD         ("PDEANFS");
const string SqliteProductDefault::EAN128CFORMATSTRING_FIELD     ("PDEAN128FS");
const string SqliteProductDefault::GS1GTINFORMATSTRING_FIELD     ("PDGS1GTINFS");
const string SqliteProductDefault::GS1AIFORMATSTRING_FIELD       ("PDGS1AIFS");
const string SqliteProductDefault::QRCODEFORMATSTRING_FIELD      ("PDQRCODEFS");
const string SqliteProductDefault::DATAMATRIXFORMATSTRING_FIELD  ("PDDMATRXFS");
const string SqliteProductDefault::EANPREFIX_FIELD               ("PDEANPr");
const string SqliteProductDefault::DESCRIPTION_FIELD             ("PDDesc");
const string SqliteProductDefault::GRAPHIC1_FIELD                ("PDGrNm1");
const string SqliteProductDefault::GRAPHIC2_FIELD                ("PDGrNm2");
const string SqliteProductDefault::GRAPHIC3_FIELD                ("PDGrNm3");
const string SqliteProductDefault::GRAPHIC4_FIELD                ("PDGrNm4");
const string SqliteProductDefault::IMAGE_FIELD                   ("PDImage");
const string SqliteProductDefault::COOLTRACKINGNUMBER_FIELD      ("PDCOLTN");
//const string SqliteProductDefault::FORCEPORTIONS_FIELD           ("PrIsFrcPt");
const string SqliteProductDefault::MEALSIDEITEMSGROUPNUM_FIELD   ("PrMealSideItemsGroupNum");
const string SqliteProductDefault::MEALSIDEITEMSAREFREE_FIELD    ("PrMealSideItemsAreFree");
const string SqliteProductDefault::MEALNUMSIDESINCLUDED_FIELD    ("PrMealNumSidesIncluded");
const string SqliteProductDefault::CUTTESTMEATSPECSHEETGROUPNUM_FIELD   ("PrCutTestMeatSpecSheetGroupNum");
const string SqliteProductDefault::TAREB_FIELD                   ("PrTareB");
// for automatic mode labels
const string SqliteProductDefault::MANUALLABELTYPENUM1_FIELD           ("PDManualLabelTypeNum1");
const string SqliteProductDefault::MANUALLABELTYPENUM2_FIELD           ("PDManualLabelTypeNum2");
const string SqliteProductDefault::MANUALLABELTYPENUM3_FIELD           ("PDManualLabelTypeNum3");
const string SqliteProductDefault::PREPACKLABELTYPENUM1_FIELD          ("PDPrepackLabelTypeNum1");
const string SqliteProductDefault::PREPACKLABELTYPENUM2_FIELD          ("PDPrepackLabelTypeNum2");
const string SqliteProductDefault::PREPACKLABELTYPENUM3_FIELD          ("PDPrepackLabelTypeNum3");
const string SqliteProductDefault::PRODENTRYLABELTYPENUM1_FIELD        ("PDProdEntryLabelTypeNum1");
const string SqliteProductDefault::PRODENTRYLABELTYPENUM2_FIELD        ("PDProdEntryLabelTypeNum2");
const string SqliteProductDefault::PRODENTRYLABELTYPENUM3_FIELD        ("PDProdEntryLabelTypeNum3");
const string SqliteProductDefault::SELFSERVLABELTYPENUM1_FIELD         ("PDSelfServLabelTypeNum1");
const string SqliteProductDefault::SELFSERVLABELTYPENUM2_FIELD         ("PDSelfServLabelTypeNum2");
const string SqliteProductDefault::SELFSERVLABELTYPENUM3_FIELD         ("PDSelfServLabelTypeNum3");
const string SqliteProductDefault::PICK5LABELTYPENUM1_FIELD            ("PDPick5LabelTypeNum1");
const string SqliteProductDefault::PICK5LABELTYPENUM2_FIELD            ("PDPick5LabelTypeNum2");
const string SqliteProductDefault::PICK5LABELTYPENUM3_FIELD            ("PDPick5LabelTypeNum3");
const string SqliteProductDefault::ONLINEORDERINGLABELTYPENUM1_FIELD   ("PDOnlineOrderingLabelTypeNum1");
const string SqliteProductDefault::ONLINEORDERINGLABELTYPENUM2_FIELD   ("PDOnlineOrderingLabelTypeNum2");
const string SqliteProductDefault::ONLINEORDERINGLABELTYPENUM3_FIELD   ("PDOnlineOrderingLabelTypeNum3");

#ifdef USE_ACCOUNTANT
int SqliteProductDefault::_dataType_ = 0;
string SqliteProductDefault::_dataTypeName_("SqliteProductDefault");
#endif



SqliteProductDefault::SqliteProductDefault(SqliteRecord* r, DBProductDefaultI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteProductDefault::~SqliteProductDefault()
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
