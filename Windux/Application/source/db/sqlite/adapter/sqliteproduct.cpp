#include "sqliteproduct.h"
#include "sqlitecoollist.h"

#include ACCOUNTANTI_INC
//#include DEBUG_INC

#ifndef NO_NAMESPACES
namespace edb_adapter
{
	using namespace std;
#endif


const string SqliteProduct::PRODUCTID_FIELD               ("PrID");
const string SqliteProduct::DEPTNUM_FIELD                 ("DeptNum");
const string SqliteProduct::REPORTINGCLASS_FIELD          ("ClNum");
const string SqliteProduct::TEXT_FIELD                    ("TxNum");
const string SqliteProduct::SPECIALMESSAGE_FIELD          ("SMNum");
const string SqliteProduct::NUTRITION_FIELD               ("NFNum");
const string SqliteProduct::NOTE_FIELD                    ("NtNum");
const string SqliteProduct::LABEL1_FIELD                  ("LbNum1");
const string SqliteProduct::LABEL2_FIELD                  ("LbNum2");
const string SqliteProduct::LABEL3_FIELD                  ("LbNum3");
const string SqliteProduct::CATEGORY_FIELD                ("CaNum");
const string SqliteProduct::SPECIALNUMBER_FIELD           ("SPNum");
const string SqliteProduct::COOLCATEGORY_FIELD            ("LCNum");
const string SqliteProduct::COOLPRETEXT_FIELD             ("LPNum");
const string SqliteProduct::CURRENTCOOLTEXT_FIELD         ("LXNum");
const string SqliteProduct::DEFAULTCOOLTEXT_FIELD         ("DLXNum");
const string SqliteProduct::COOLLISTKEY_FIELD             ("LLID");
const string SqliteProduct::MARQUEE_FIELD                 ("MaNum");
const string SqliteProduct::PRODUCTTEXT5_FIELD            ("ProdTxt5Num");
const string SqliteProduct::PRODUCTTEXT6_FIELD            ("ProdTxt6Num");
const string SqliteProduct::PRODUCTTEXT7_FIELD            ("ProdTxt7Num");
const string SqliteProduct::PRODUCTTEXT8_FIELD            ("ProdTxt8Num");
const string SqliteProduct::PRODUCTTEXT9_FIELD            ("ProdTxt9Num");
const string SqliteProduct::PRICE_FIELD                   ("PrPrice");
const string SqliteProduct::PRICEFACTOR_FIELD             ("PrPrMod");
const string SqliteProduct::FORCEPRICE_FIELD              ("PrIsFrcPr");
const string SqliteProduct::EXCEPTIONPRICE_FIELD          ("PrExcPr");
const string SqliteProduct::DISCOUNTMETHOD_FIELD          ("PrDscMe");
const string SqliteProduct::DISCOUNTPRICE1_FIELD          ("PrDscPr1");
const string SqliteProduct::DISCOUNTPRICE2_FIELD          ("PrDscPr2");
const string SqliteProduct::DISCOUNTPRICE3_FIELD          ("PrDscPr3");
const string SqliteProduct::TARE_FIELD                    ("PrTare");
const string SqliteProduct::FORCETARE_FIELD               ("PrIsFrcTr");
const string SqliteProduct::PROPORTIONALTARE_FIELD        ("PrPrpTare");
const string SqliteProduct::NETWEIGHT_FIELD               ("PrNetWt");
const string SqliteProduct::BYCOUNT_FIELD                 ("PrByCnt");
const string SqliteProduct::FORCEBYCOUNT_FIELD            ("PrIsFrcBC");
const string SqliteProduct::SHELFLIFEDAYS_FIELD           ("PrShlfLfD");
const string SqliteProduct::SHELFLIFEHOURS_FIELD          ("PrShlfLfH");
const string SqliteProduct::SHELFLIFEMINUTES_FIELD        ("PrShlfLfM");
const string SqliteProduct::PRODUCTLIFEDAYS_FIELD         ("PrProdLfD");
const string SqliteProduct::PRODUCTLIFEHOURS_FIELD        ("PrProdLfH");
const string SqliteProduct::PRODUCTLIFEMINUTES_FIELD      ("PrProdLfM");
const string SqliteProduct::DONTPRINTSHELFLIFE_FIELD      ("PrIsPrnSL");
const string SqliteProduct::DONTPRINTPRODUCTLIFE_FIELD    ("PrIsPrnPL");
const string SqliteProduct::DONTPRINTPACKEDON_FIELD       ("PrIsPrnPO");
const string SqliteProduct::BONUSPOINTS_FIELD             ("PrBnsPts");
const string SqliteProduct::PORTIONS_FIELD                ("PrPrtns");
const string SqliteProduct::APPLYSECURITYLABEL_FIELD      ("PrIsUsSL");
const string SqliteProduct::INTAKEMODE_FIELD              ("PrInMode");
const string SqliteProduct::LABEL1ROTATION_FIELD          ("PrLblRot1");
const string SqliteProduct::LABEL1PLACEMENT_FIELD         ("PrLblPlc1");
const string SqliteProduct::LABEL2ROTATION_FIELD          ("PrLblRot2");
const string SqliteProduct::LABEL2PLACEMENT_FIELD         ("PrLblPlc2");
const string SqliteProduct::LABEL3ROTATION_FIELD          ("PrLblRot3");
const string SqliteProduct::LABEL3PLACEMENT_FIELD         ("PrLblPlc3");
const string SqliteProduct::NAME1_FIELD                   ("PrName1");
const string SqliteProduct::NAME2_FIELD                   ("PrName2");
const string SqliteProduct::FORCECOOLREVIEW_FIELD         ("PrVCOLTxt");
const string SqliteProduct::COOLREQUIRED_FIELD            ("PrCOLReq");
const string SqliteProduct::PRODUCTIONPLANNINGTYPE_FIELD  ("PrPrPlT");
const string SqliteProduct::PLU_FIELD                     ("PrNum");
const string SqliteProduct::TYPE_FIELD                    ("PrType");
const string SqliteProduct::BARCODEPREFIX_FIELD           ("PrBarPref");
const string SqliteProduct::BARCODEPRODUCTNUMBER_FIELD    ("PrBarProN");
const string SqliteProduct::BARCODEVENDORNUMBER_FIELD     ("PrBarVenN");
const string SqliteProduct::BARCODEEANCODE_FIELD          ("PrEANCd");
const string SqliteProduct::RAWBARCODENUMBER_FIELD        ("PrRawBarC");
const string SqliteProduct::CODEDNAME1_FIELD              ("PrCdNm1");
const string SqliteProduct::CODEDNAME2_FIELD              ("PrCdNm2");
const string SqliteProduct::SPECIALSTARTDATE_FIELD        ("PrSpcSD");
const string SqliteProduct::DESCRIPTION_FIELD             ("PrDesc");
const string SqliteProduct::GRAPHIC1_FIELD                ("PrGrNm1");
const string SqliteProduct::GRAPHIC2_FIELD                ("PrGrNm2");
const string SqliteProduct::GRAPHIC3_FIELD                ("PrGrNm3");
const string SqliteProduct::GRAPHIC4_FIELD                ("PrGrNm4");
const string SqliteProduct::IMAGE_FIELD                   ("PrImage");
const string SqliteProduct::COOLTRACKINGTEXT_FIELD        ("PrCOLTN");
//const string SqliteProduct::FORCEPORTIONS_FIELD           ("PrIsFrcPt");
const string SqliteProduct::MEALSIDEITEMSGROUPNUM_FIELD   ("PrMealSideItemsGroupNum");
const string SqliteProduct::MEALSIDEITEMSAREFREE_FIELD    ("PrMealSideItemsAreFree");
const string SqliteProduct::MEALNUMSIDESINCLUDED_FIELD    ("PrMealNumSidesIncluded");
const string SqliteProduct::CUTTESTMEATSPECSHEETGROUPNUM_FIELD   ("PrCutTestMeatSpecSheetGroupNum");
const string SqliteProduct::TAREB_FIELD                   ("PrTareB");

// For Pick-5 project
const string SqliteProduct::WEIGHTMIN_FIELD               ("PrWeightMin");
const string SqliteProduct::WEIGHTMAX_FIELD               ("PrWeightMax");
const string SqliteProduct::TOTALPRICEMIN_FIELD           ("PrTotalPriceMin");
const string SqliteProduct::TOTALPRICEMAX_FIELD           ("PrTotalPriceMax");

// for automatic mode labels
const string SqliteProduct::MANUALLABELTYPENUM1_FIELD     ("PrManualLabelTypeNum1");
const string SqliteProduct::MANUALLABELTYPENUM2_FIELD     ("PrManualLabelTypeNum2");
const string SqliteProduct::MANUALLABELTYPENUM3_FIELD     ("PrManualLabelTypeNum3");
const string SqliteProduct::PREPACKLABELTYPENUM1_FIELD    ("PrPrepackLabelTypeNum1");
const string SqliteProduct::PREPACKLABELTYPENUM2_FIELD    ("PrPrepackLabelTypeNum2");
const string SqliteProduct::PREPACKLABELTYPENUM3_FIELD    ("PrPrepackLabelTypeNum3");
const string SqliteProduct::PRODENTRYLABELTYPENUM1_FIELD  ("PrProdEntryLabelTypeNum1");
const string SqliteProduct::PRODENTRYLABELTYPENUM2_FIELD  ("PrProdEntryLabelTypeNum2");
const string SqliteProduct::PRODENTRYLABELTYPENUM3_FIELD  ("PrProdEntryLabelTypeNum3");
const string SqliteProduct::SELFSERVLABELTYPENUM1_FIELD   ("PrSelfServLabelTypeNum1");
const string SqliteProduct::SELFSERVLABELTYPENUM2_FIELD   ("PrSelfServLabelTypeNum2");
const string SqliteProduct::SELFSERVLABELTYPENUM3_FIELD   ("PrSelfServLabelTypeNum3");
const string SqliteProduct::PICK5LABELTYPENUM1_FIELD      ("PrPick5LabelTypeNum1");
const string SqliteProduct::PICK5LABELTYPENUM2_FIELD      ("PrPick5LabelTypeNum2");
const string SqliteProduct::PICK5LABELTYPENUM3_FIELD      ("PrPick5LabelTypeNum3");
const string SqliteProduct::ONLINEORDERINGLABELTYPENUM1_FIELD("PrOnlineOrderingLabelTypeNum1");
const string SqliteProduct::ONLINEORDERINGLABELTYPENUM2_FIELD("PrOnlineOrderingLabelTypeNum2");
const string SqliteProduct::ONLINEORDERINGLABELTYPENUM3_FIELD("PrOnlineOrderingLabelTypeNum3");

#ifdef USE_ACCOUNTANT
int SqliteProduct::_dataType_ = 0;
string SqliteProduct::_dataTypeName_("SqliteProduct");
#endif



SqliteProduct::SqliteProduct(SqliteRecord* r, DBProductI* d)
		: rec_(r), decorated_(d)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}


SqliteProduct::~SqliteProduct()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
	
	if ( rec_ )
	{
		delete rec_;
		rec_ = NULL;
	}

	if ( decorated_ )
	{
		delete decorated_;
		decorated_ = NULL;
	}
}


void SqliteProduct::setCoolList(const DBCoolListI* l)
{
	const SqliteCoolList* rec = dynamic_cast<const SqliteCoolList*>(l);

	if ( rec )
		rec_->setField(COOLLISTKEY_FIELD, rec->getListKey());
}


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif
