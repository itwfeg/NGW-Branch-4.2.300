/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ProductTags.h"
#include "DeptTags.h"
#include "ClassTags.h"
#include "ExpandedTextTags.h"
#include "SpecialMessageTags.h"
#include "ProductNotesTags.h"
#include "NutrifactsDataTags.h"
#include "MarqueeTextTags.h"
#include "CategoryTags.h"
#include "SpecialsTags.h"
#include "COOLTags.h"
#include "Product.h"
#include "ProductText5Tags.h"
#include "ProductText6Tags.h"
#include "ProductText7Tags.h"
#include "ProductText8Tags.h"
#include "ProductText9Tags.h"
#include "Debug.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductTags::dataType = 0;
std::string ProductTags::dataTypeName("ProductTags"); 
#endif

const std::string ProductTags::prodNumber			= "p#";
const std::string ProductTags::prodType				= "PT";  // new for Evolution (but related to pf)
const std::string ProductTags::prodDescription		= "dt";

//const std::string ProductTags::prodBarType		= "bt";  // new for Evolution
const std::string ProductTags::prodBarPrefix		= "u#";
const std::string ProductTags::prodUpcNum 			= "up";
const std::string ProductTags::prodProductNumber	= "pn";  // new for Evolution
const std::string ProductTags::prodVendorNumber		= "vn";  // new for Evolution
const std::string ProductTags::prodBarEanCode		= "Ec";  // new for Evolution

const std::string ProductTags::prodByCount 			= "bc";
const std::string ProductTags::prodByCountForced	= "fb";
const std::string ProductTags::prodExceptPrice 		= "xp";
const std::string ProductTags::prodExceptPriceTare	= "xt";
const std::string ProductTags::prodUnitPrice 		= "u$";
const std::string ProductTags::prodUnitPriceForced	= "fp";

const std::string ProductTags::prodDiscountType		= "Dt";
const std::string ProductTags::prodDiscount1 		= "dp";
const std::string ProductTags::prodDiscount2 		= "D2";
const std::string ProductTags::prodDiscount3 		= "D3";

const std::string ProductTags::prodTareA 			= "ta";
const std::string ProductTags::prodTareB 			= "T2";
const std::string ProductTags::prodTareAForced		= "fT";
const std::string ProductTags::prodProportionalTare = "pt";
const std::string ProductTags::prodFixedWeight		= "nw";

const std::string ProductTags::prodShelfLife 		= "sl";
const std::string ProductTags::prodShelfLifeHours	= "SL";  // new for Evolution
const std::string ProductTags::prodShelfLifeMinutes = "SM";		// SOF-4904 Add shelfLifeMinutes to product and transactions
const std::string ProductTags::prodDontPrintSellBy	= "PS";
const std::string ProductTags::prodProdLife 		= "pl";
const std::string ProductTags::prodProdLifeHours	= "PL";  // new for Evolution
const std::string ProductTags::prodProdLifeMinutes  = "PM";		// SOF-4904 Add shelfLifeMinutes to product and transactions
const std::string ProductTags::prodDontPrintBestBefore	= "Pi";
const std::string ProductTags::prodDontPrintPackedOn	= "PP";

const std::string ProductTags::prodApplySecurityLabel	= "us";

const std::string ProductTags::prodPriceModifier	= "pm";

const std::string ProductTags::prodLabelType1 		= "l1";
const std::string ProductTags::prodLabelRotate1		= "p1";
const std::string ProductTags::prodLabelPlacement1	= "L1";
const std::string ProductTags::prodLabelType2 		= "l2";
const std::string ProductTags::prodLabelRotate2		= "p2";
const std::string ProductTags::prodLabelPlacement2	= "L2";
const std::string ProductTags::prodLabelType3 		= "l3";  // new for Evolution
const std::string ProductTags::prodLabelRotate3		= "p3";  // new for Evolution
const std::string ProductTags::prodLabelPlacement3	= "L3";

//const std::string ProductTags::prodPortions 		= "p_";  // new for Evolution
const std::string ProductTags::prodPoints			= "ps";

const std::string ProductTags::prodIntakeMode		= "im";

const std::string ProductTags::graphicNumber		= "g#";  // Evolution doesn't store as a number, but will use this as the graphic name if G1 not specified.
const std::string ProductTags::prodGraphicName1		= "G1";  // new for Evolution
const std::string ProductTags::prodGraphicName2		= "G2";  // new for Evolution
const std::string ProductTags::prodGraphicName3		= "G3";  // new for Evolution
const std::string ProductTags::prodGraphicName4		= "G4";  // new for Evolution

const std::string ProductTags::pluFlags				= "pf";
const std::string ProductTags::totalsFlags			= "tf";

#ifndef NOTSELFSERVICE
const std::string ProductTags::productGraphicName	= "i1";
const std::string ProductTags::prodName1			= "n1";
const std::string ProductTags::prodName2			= "n2";
const std::string ProductTags::specialStartDate		= "sS";
#endif

const std::string ProductTags::eANFormatString		= "ef";		// Product Defaults only
const std::string ProductTags::eAN128CFormatString	= "eF";		// Product Defaults only

const std::string ProductTags::gS1GTINFormatString	= "gs";		// Product Defaults only
const std::string ProductTags::gS1AIFormatString	= "gS";		// Product Defaults only

const std::string ProductTags::qrCodeFormatString     = "qs";		// Product Defaults only
const std::string ProductTags::dataMatrixFormatString = "qS";		// Product Defaults only

const std::string ProductTags::eANprefix			= "ex";		// Product Defaults only

// COOL related tags
const std::string ProductTags::defaultCOOLTextNum = "c7";
const std::string ProductTags::currentCOOLTextNum = "c8";
const std::string ProductTags::verifyCOOLText     = "cf";
const std::string ProductTags::coolInfoRequired   = "co";
const std::string ProductTags::coolTrackingText   = "cc";

const std::string ProductTags::productionPlanningType   = "Pp";

const std::string ProductTags::buildAMealSideItemsGroupNum      = "Bg";
const std::string ProductTags::buildAMealSideItemsAreFree       = "Bf";
const std::string ProductTags::buildAMealNumberOfSidesIncluded  = "Bn";

const std::string ProductTags::cutTestMeatSpecSheetGroupNum     = "cM";

// For Pick-5 project
const std::string ProductTags::weightMin      = "mw";
const std::string ProductTags::weightMax      = "Mw";
const std::string ProductTags::totalPriceMin  = "mp";
const std::string ProductTags::totalPriceMax  = "Mp";

// automatic mode labels
const std::string ProductTags::manualLabelTypeNum1    = "l4";
const std::string ProductTags::manualLabelTypeNum2    = "l5";
const std::string ProductTags::manualLabelTypeNum3    = "l6";
const std::string ProductTags::prepackLabelTypeNum1   = "l7";
const std::string ProductTags::prepackLabelTypeNum2   = "l8";
const std::string ProductTags::prepackLabelTypeNum3   = "l9";
const std::string ProductTags::prodEntryLabelTypeNum1 = "L4";
const std::string ProductTags::prodEntryLabelTypeNum2 = "L5";
const std::string ProductTags::prodEntryLabelTypeNum3 = "L6";
const std::string ProductTags::selfServLabelTypeNum1  = "L7";
const std::string ProductTags::selfServLabelTypeNum2  = "L8";
const std::string ProductTags::selfServLabelTypeNum3  = "L9";
const std::string ProductTags::pick5LabelTypeNum1     = "La";
const std::string ProductTags::pick5LabelTypeNum2     = "Lb";
const std::string ProductTags::pick5LabelTypeNum3     = "Lc";
const std::string ProductTags::onlineOrderingLabelTypeNum1 = "Ld";
const std::string ProductTags::onlineOrderingLabelTypeNum2 = "Le";
const std::string ProductTags::onlineOrderingLabelTypeNum3 = "Lf";

bool     ProductTags::_tagsInitialized_ = false;
TagsList ProductTags::productTags(Product::endProductFieldOffsets);
TagsMap  ProductTags::fieldMap;


ProductTags::ProductTags(FieldOffsetsList *pFieldList)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( !_tagsInitialized_ )
	{
		//initialize the table of tags for the product class
		productTags[0] 									    = emptyTag;
		productTags[Product::fieldOffsetProductNum] 		= prodNumber;
		productTags[Product::fieldOffsetProductType] 		= prodType;
		productTags[Product::fieldOffsetDescription]		= prodDescription;

		productTags[Product::fieldOffsetBarcodeType] 		= emptyTag; //prodBarType;
		productTags[Product::fieldOffsetBarcodeFields] 		= emptyTag;
		productTags[Product::fieldOffsetBarcodePrefix] 		= prodBarPrefix;
		productTags[Product::fieldOffsetBarcodeProductNum]	= prodProductNumber;
		productTags[Product::fieldOffsetBarcodeUpcNum]		= prodUpcNum;
		productTags[Product::fieldOffsetBarcodeVendorNum] 	= prodVendorNumber;
		productTags[Product::fieldOffsetBarcodeEANCode] 	= prodBarEanCode;

		productTags[Product::fieldOffsetByCount] 			= prodByCount;
		productTags[Product::fieldOffsetExceptionPrice] 	= prodExceptPrice;
		productTags[Product::fieldOffsetExceptionPriceTare]	= prodExceptPriceTare;
		productTags[Product::fieldOffsetUnitPrice] 			= prodUnitPrice;

		productTags[Product::fieldOffsetDiscountType] 		= prodDiscountType;
		productTags[Product::fieldOffsetDiscountValue1] 	= prodDiscount1;
		productTags[Product::fieldOffsetDiscountValue2] 	= prodDiscount2;
		productTags[Product::fieldOffsetDiscountValue3] 	= prodDiscount3;

		productTags[Product::fieldOffsetTareAValue] 		= prodTareA;
		productTags[Product::fieldOffsetProportionalTare]	= prodProportionalTare;
		productTags[Product::fieldOffsetFixedWeight] 		= prodFixedWeight;

		productTags[Product::fieldOffsetShelfLifeDays] 		= prodShelfLife;
		productTags[Product::fieldOffsetShelfLifeHours] 	= prodShelfLifeHours;			// Minutes at the bottom

		productTags[Product::fieldOffsetProductLifeDays] 	= prodProdLife;
		productTags[Product::fieldOffsetProductLifeHours]	= prodProdLifeHours;			// Minutes at the bottom

		productTags[Product::fieldOffsetPriceModifier] 		= prodPriceModifier;

		productTags[Product::fieldOffsetProductReportingClassNum]= ClassTags::reportingClassNumber;

		productTags[Product::fieldOffsetLabelTypeNum1] 		= prodLabelType1;
		productTags[Product::fieldOffsetLabelRotation1] 	= prodLabelRotate1;
		productTags[Product::fieldOffsetLabelTypeNum2] 		= prodLabelType2;
		productTags[Product::fieldOffsetLabelRotation2] 	= prodLabelRotate2;
		productTags[Product::fieldOffsetLabelTypeNum3] 		= prodLabelType3;
		productTags[Product::fieldOffsetLabelRotation3] 	= prodLabelRotate3;

		//productTags[Product::fieldOffsetPortions]			= prodPortions;
		productTags[Product::fieldOffsetPoints] 			= prodPoints;

		productTags[Product::fieldOffsetGraphicNum] 		= graphicNumber;
		productTags[Product::fieldOffsetGraphicName1] 		= prodGraphicName1;
		productTags[Product::fieldOffsetGraphicName2] 		= prodGraphicName2;
		productTags[Product::fieldOffsetGraphicName3] 		= prodGraphicName3;
		productTags[Product::fieldOffsetGraphicName4]	 	= prodGraphicName4;

		productTags[Product::fieldOffsetByCountForced] 			= prodByCountForced;
		productTags[Product::fieldOffsetTareAForced] 			= prodTareAForced;
		productTags[Product::fieldOffsetFractionalPriceUnit]	= emptyTag;
		productTags[Product::fieldOffsetPoundsForAmount] 		= emptyTag;
		productTags[Product::fieldOffsetUnitPriceForced] 		= prodUnitPriceForced;
		productTags[Product::fieldOffsetTareType] 				= emptyTag;
		productTags[Product::fieldOffsetApplySecurity] 			= prodApplySecurityLabel;
		productTags[Product::fieldOffsetDontPrintPackedOn] 		= prodDontPrintPackedOn;
		productTags[Product::fieldOffsetDontPrintSellBy] 		= prodDontPrintSellBy;
		productTags[Product::fieldOffsetDontPrintBestBefore]	= prodDontPrintBestBefore;
		productTags[Product::fieldOffsetLabelPlacement1]		= prodLabelPlacement1;
		productTags[Product::fieldOffsetLabelPlacement2]		= prodLabelPlacement2;
		productTags[Product::fieldOffsetLabelPlacement3]		= prodLabelPlacement3;
		productTags[Product::fieldOffsetFractionalPrice] 		= emptyTag;
		productTags[Product::fieldOffsetForcedFractionalPrice]	= emptyTag;
		productTags[Product::fieldOffsetPoundsForPrice] 		= emptyTag;
		productTags[Product::fieldOffsetForcedPoundsFor] 		= emptyTag;

		productTags[Product::fieldOffsetIntakeMode]				= prodIntakeMode;

#ifndef NOTSELFSERVICE
		productTags[Product::fieldOffsetProductGraphicName]		= productGraphicName;
		productTags[Product::fieldOffsetProductName1]			= prodName1;
		productTags[Product::fieldOffsetProductName2]			= prodName2;
		productTags[Product::fieldOffsetProdCategoryNum]		= CategoryTags::categoryNum;
		productTags[Product::fieldOffsetProdSpecialNum]			= SpecialsTags::specialsNum;
		productTags[Product::fieldOffsetSpecialStartDate]		= specialStartDate;
#endif

		productTags[Product::fieldOffsetEANFormatString]		= eANFormatString;	// Product Defaults only
		productTags[Product::fieldOffsetEAN128CFormatString]	= eAN128CFormatString;	// Product Defaults only
		
		productTags[Product::fieldOffsetGS1DatabarGTIN]			= gS1GTINFormatString;	// Product Defaults only
		productTags[Product::fieldOffsetGS1DatabarAI]			= gS1AIFormatString;	// Product Defaults only

		productTags[Product::fieldOffsetQRCodeFormatString]     = qrCodeFormatString;	    // Product Defaults only
		productTags[Product::fieldOffsetDataMatrixFormatString] = dataMatrixFormatString;	// Product Defaults only



		productTags[Product::fieldOffsetEANprefix]				= eANprefix;			// Product Defaults only

		productTags[Product::fieldOffsetPluFlags]				= pluFlags;
		productTags[Product::fieldOffsetTotalsFlags]			= totalsFlags;

		productTags[Product::fieldOffsetExpandedTextNum]		= ExpandedTextTags::expandedTextNumber;
		productTags[Product::fieldOffsetSpecialMessageNum]		= SpecialMessageTags::specialTextNumber;
		productTags[Product::fieldOffsetProdProductNoteNum]		= ProductNotesTags::productNotesTextNumber;
		productTags[Product::fieldOffsetProdNutrifactNum]		= NutrifactsDataTags::nutriFactNum;
		productTags[Product::fieldOffsetMarqueeTextNum]			= MarqueeTextTags::marqueeNumber;

		productTags[Product::fieldOffsetProductText5Num]		= ProductText5Tags::productText5Number;
		productTags[Product::fieldOffsetProductText6Num]		= ProductText6Tags::productText6Number;
		productTags[Product::fieldOffsetProductText7Num]		= ProductText7Tags::productText7Number;
		productTags[Product::fieldOffsetProductText8Num]		= ProductText8Tags::productText8Number;
		productTags[Product::fieldOffsetProductText9Num]		= ProductText9Tags::productText9Number;


		// COOL related tags
		productTags[Product::fieldOffsetProdCoolCategoryNum]     = COOLTags::coolCategoryNum;
		productTags[Product::fieldOffsetProdCoolPretextNum]      = COOLTags::coolPretextNum;
		productTags[Product::fieldOffsetProdCoolDefaultTextNum]  = defaultCOOLTextNum;
		productTags[Product::fieldOffsetProdCoolTextNum]         = currentCOOLTextNum;
		productTags[Product::fieldOffsetProdCoolShortlistNum]    = COOLTags::coolShortlistNum;
		productTags[Product::fieldOffsetProdCoolShortlistFirstLink]    = COOLTags::coolShortlistFirstLink;
		productTags[Product::fieldOffsetProdCoolVerifyForced]	= verifyCOOLText;
		productTags[Product::fieldOffsetProdCOOLTrackingText]	= coolTrackingText;
		productTags[Product::fieldOffsetProdCoolRequired]		= coolInfoRequired;

		productTags[Product::fieldOffsetProductionPlanningType]	= productionPlanningType;

		productTags[Product::fieldOffsetBuildAMealSideItemsGroupNum]     = buildAMealSideItemsGroupNum;
		productTags[Product::fieldOffsetBuildAMealSideItemsAreFree]      = buildAMealSideItemsAreFree;
		productTags[Product::fieldOffsetBuildAMealNumberOfSidesIncluded] = buildAMealNumberOfSidesIncluded;

		productTags[Product::fieldOffsetCutTestMeatSpecSheetGroupNum] = cutTestMeatSpecSheetGroupNum;

		productTags[Product::fieldOffsetTareBValue] 			= prodTareB;

		productTags[Product::fieldOffsetProductDeptNum]			= DeptTags::deptNum;

		// For Pick-5 project
		productTags[Product::fieldOffsetWeightMin]      = weightMin;
		productTags[Product::fieldOffsetWeightMax]      = weightMax;
		productTags[Product::fieldOffsetTotalPriceMin]  = totalPriceMin;
		productTags[Product::fieldOffsetTotalPriceMax]  = totalPriceMax;

		// Automate labels based on mode
		productTags[Product::fieldOffsetManualLabelTypeNum1] = manualLabelTypeNum1;
		productTags[Product::fieldOffsetManualLabelTypeNum2] = manualLabelTypeNum2;
		productTags[Product::fieldOffsetManualLabelTypeNum3] = manualLabelTypeNum3;
		productTags[Product::fieldOffsetPrepackLabelTypeNum1] = prepackLabelTypeNum1;
		productTags[Product::fieldOffsetPrepackLabelTypeNum2] = prepackLabelTypeNum2;
		productTags[Product::fieldOffsetPrepackLabelTypeNum3] = prepackLabelTypeNum3;
		productTags[Product::fieldOffsetProdEntryLabelTypeNum1] = prodEntryLabelTypeNum1;
		productTags[Product::fieldOffsetProdEntryLabelTypeNum2] = prodEntryLabelTypeNum2;
		productTags[Product::fieldOffsetProdEntryLabelTypeNum3] = prodEntryLabelTypeNum3;
		productTags[Product::fieldOffsetSelfServLabelTypeNum1] = selfServLabelTypeNum1;
		productTags[Product::fieldOffsetSelfServLabelTypeNum2] = selfServLabelTypeNum2;
		productTags[Product::fieldOffsetSelfServLabelTypeNum3] = selfServLabelTypeNum3;
		productTags[Product::fieldOffsetPick5LabelTypeNum1] = pick5LabelTypeNum1;
		productTags[Product::fieldOffsetPick5LabelTypeNum2] = pick5LabelTypeNum2;
		productTags[Product::fieldOffsetPick5LabelTypeNum3] = pick5LabelTypeNum3;
		productTags[Product::fieldOffsetOnlineOrderingLabelTypeNum1] = onlineOrderingLabelTypeNum1;
		productTags[Product::fieldOffsetOnlineOrderingLabelTypeNum2] = onlineOrderingLabelTypeNum2;
		productTags[Product::fieldOffsetOnlineOrderingLabelTypeNum3] = onlineOrderingLabelTypeNum3;
        productTags[Product::fieldOffsetShelfLifeMinutes]   = prodShelfLifeMinutes;
		productTags[Product::fieldOffsetProductLifeMinutes] = prodProdLifeMinutes;


		_tagsInitialized_ = true;
	}
	if (pFieldList)
		putTagsInMap(pFieldList);
}

ProductTags::~ProductTags()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}