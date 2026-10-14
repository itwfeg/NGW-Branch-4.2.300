/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include <math.h>

#include "ProductDefault.h"
#include "Product.h"
#include "ProductTags.h"
#include "ProductTransaction.h"
#include "Class.h"
#include "Graphic.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Configuration.h"
#include "AbstractSession.h"
#include "LabelTypeManager.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBProductDefaultsRM.h"
#include "DBProductDefault.h"
#include "DBProduct.h"

#include "DBLabelsRM.h"
#include "DBROMLabelsRM.h"

#include "MultiLanguageText.h"

#include "AllDepts.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ProductDefault::dataType = 0;
std::string ProductDefault::dataTypeName("ProductDefault"); 
#endif

list<Product::ProductFieldOffsets> ProductDefault::evolFieldList;
list<Product::ProductFieldOffsets> ProductDefault::dbindependentFieldList;


ProductDefault::ProductDefault()
	: Product()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

ProductDefault::ProductDefault(const char *data, int len, int formatType)
	: Product()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	switch (formatType)
	{
		case BrowserFormat:
		{
			// The following code is from the Product class constructor with the same parameters.

			HobString temp(data, len);
			std::list<Product::ProductFieldOffsets>::iterator fieldIter = uiFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != uiFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
				if (token.length())
				{
					token.unQuoteForCSV();
					updateField(offset, token);
				}
				fieldIter++;
			}

			// End of the code above from the Product class constructor with the same parameters.

			// Still have one more field to process: fieldOffsetEANprefix
			// which directly follows the Product fields in the message.

			// The fieldOffsetEANprefix is already in the token var at this point.
			setReceived(fieldOffsetEANprefix, (token.length() == 0) ? false : true);
			updateField(fieldOffsetEANprefix, token);

			// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetProductDeptNum))
			{
				setReceived(fieldOffsetProductDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			ProductTags	productTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				HobString tokenData = "";
				while (tok.next(token) != -1)
				{
					temp = token->getTag();
					offset = (temp.size()) ? productTags.getOffset(temp) : 0;
					if (Debug::isTracing("ProductDefault.ctor"))
						Debug::trace("ProductDefault.ctor", string("ProductDefault(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
					if (offset)
					{
						setReceived(offset, true);
						tokenData = token->getData();

						//Changed -- Dual range is only available on the HLX and new USB weigher peripheral with dual-range load cell. -- ats 
						// Fix for PTR#26 made during fix for PTR#119. (haberrc)
						if (offset == fieldOffsetTareAValue && 
							( EvolutionTaggedFormat == formatType  || UltimaTaggedFormat == formatType ) &&
							tokenData.find(".") == std::string::npos &&
							config->getExternalWeightPrecision() == 2)
						{
							// Data is coming in with only 2 implied decimal places.  We need to add another decimal place before updating the tare.
							tokenData.append("0");
						}

						// HT-136 Need to account for TareB
						if (offset == fieldOffsetTareBValue && 
							( EvolutionTaggedFormat == formatType  || UltimaTaggedFormat == formatType ) &&
							tokenData.find(".") == std::string::npos &&
							config->getExternalWeightPrecision() == 2)
						{
							// Data is coming in with only 2 implied decimal places.  We need to add another decimal place before updating the tare.
							tokenData.append("0");
						}

						updateField(offset, tokenData);
					}
				}	
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetProductDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetProductDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			break;
	}
}

ProductDefault::ProductDefault(DBProductDefault *dbRec, DBAccess *access)
	: Product()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("ProductDefault.ctor", string("ProductDefault(DBProductDefault, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		deptNum = Long::valueOf(dbRec->getDeptNumber());
		productType = (Product::ProductType)dbRec->getType();

		productionPlanningType = (Product::ProductionPlanningType)dbRec->getProductionPlanningType();

		description = dbRec->getDescription();

		byCount = dbRec->getByCount();

		byCountForced = dbRec->getForceByCount();

		exceptionPrice = (HobPrice) dbRec->getExceptionPrice();
		unitPrice = (HobPrice) dbRec->getPrice();
		unitPriceForced = dbRec->getForcePrice();

		priceModifier = dbRec->getPriceFactor(); // This is what we used to call PriceModifier

		discountType = dbRec->getDiscountMethod();
		discountValue1 = (HobPrice) dbRec->getDiscountPrice1();
		discountValue2 = (HobPrice) dbRec->getDiscountPrice2();
		discountValue3 = (HobPrice) dbRec->getDiscountPrice3();

		tareA.setValue((HobWeight) dbRec->getTare());
		tareB.setValue((HobWeight) dbRec->getTareB());
		tareA.setForced(dbRec->getForceTare());		// HT-136 There is only one Forced Tare field, shared by TareA and TareB

		proportionalTare = dbRec->getProportionalTare();
		fixedWeight = (HobWeight) dbRec->getNetWeight();

		shelfLifeDays = dbRec->getShelfLifeDays();
		shelfLifeHours = dbRec->getShelfLifeHours();
		shelfLifeMinutes = dbRec->getShelfLifeMinutes();
		productLifeDays = dbRec->getProductLifeDays();
		productLifeHours = dbRec->getProductLifeHours();
		productLifeMinutes = dbRec->getProductLifeMinutes();

		dontPrintPackedOn = dbRec->getDontPrintPackedOn();
		dontPrintSellBy = dbRec->getDontPrintShelfLife();
		dontPrintBestBefore = dbRec->getDontPrintProductLife();

		barCodeNum.setPrefix(Integer::toString(dbRec->getUPCNumberSystem()));

		setLabelRotation(FirstPrinter, dbRec->getLabel1Rotation());
		setLabelRotation(SecondPrinter, dbRec->getLabel2Rotation());
		setLabelRotation(ThirdPrinter, dbRec->getLabel3Rotation());

		setLabelPlacement(FirstPrinter, dbRec->getLabel1Placement());
		setLabelPlacement(SecondPrinter, dbRec->getLabel2Placement());
		setLabelPlacement(ThirdPrinter, dbRec->getLabel3Placement());

		points = dbRec->getBonusPoints();

		intakeMode = static_cast<WrapperIntakeMode::IntakeMode> (dbRec->getIntakeMode());

		applySecurityLabel = dbRec->getApplySecurityLabel();

		setGraphic(Graphic::GRAPHIC1, Graphic::findGraphic(dbRec->getGraphic1()));
		setGraphic(Graphic::GRAPHIC2, Graphic::findGraphic(dbRec->getGraphic2()));
		setGraphic(Graphic::GRAPHIC3, Graphic::findGraphic(dbRec->getGraphic3()));
		setGraphic(Graphic::GRAPHIC4, Graphic::findGraphic(dbRec->getGraphic4()));

		HobRecordNum reportingClassNum = Integer::valueOf(dbRec->getReportingClass());
		Class *newReportingClass = new Class();
		newReportingClass->setClassNum(reportingClassNum) ;
		setReportingClass(newReportingClass);

		// TODO - Larry - done - How are we going to handle multiple labels on one printer? We now keep the label type numbers as separate attributes in a Product object.
		defaultLabelTypeNum1   = Long::valueOf(dbRec->getLabel1());
		defaultLabelTypeNum2   = Long::valueOf(dbRec->getLabel2());
		defaultLabelTypeNum3   = Long::valueOf(dbRec->getLabel3());
		manualLabelTypeNum1    = Long::valueOf(dbRec->getManualLabelTypeNum1());
		manualLabelTypeNum2    = Long::valueOf(dbRec->getManualLabelTypeNum2());
		manualLabelTypeNum3    = Long::valueOf(dbRec->getManualLabelTypeNum3());
		prepackLabelTypeNum1   = Long::valueOf(dbRec->getPrepackLabelTypeNum1());
		prepackLabelTypeNum2   = Long::valueOf(dbRec->getPrepackLabelTypeNum2());
		prepackLabelTypeNum3   = Long::valueOf(dbRec->getPrepackLabelTypeNum3());
		prodEntryLabelTypeNum1 = Long::valueOf(dbRec->getProdEntryLabelTypeNum1());
		prodEntryLabelTypeNum2 = Long::valueOf(dbRec->getProdEntryLabelTypeNum2());
		prodEntryLabelTypeNum3 = Long::valueOf(dbRec->getProdEntryLabelTypeNum3());
		selfServLabelTypeNum1  = Long::valueOf(dbRec->getSelfServLabelTypeNum1());
		selfServLabelTypeNum2  = Long::valueOf(dbRec->getSelfServLabelTypeNum2());
		selfServLabelTypeNum3  = Long::valueOf(dbRec->getSelfServLabelTypeNum3());
		pick5LabelTypeNum1     = Long::valueOf(dbRec->getPick5LabelTypeNum1());
		pick5LabelTypeNum2     = Long::valueOf(dbRec->getPick5LabelTypeNum2());
		pick5LabelTypeNum3     = Long::valueOf(dbRec->getPick5LabelTypeNum3());
		onlineOrderingLabelTypeNum1 = Long::valueOf(dbRec->getOnlineOrderingLabelTypeNum1());
		onlineOrderingLabelTypeNum2 = Long::valueOf(dbRec->getOnlineOrderingLabelTypeNum2());
		onlineOrderingLabelTypeNum3 = Long::valueOf(dbRec->getOnlineOrderingLabelTypeNum3());

		// TODO - Larry - done - auto labels. Do we need to set the others? Numbering?
		// For the 'label types by mode' project, continue to let the label types be set up based on the default label types.
		setLabelType(FirstPrinter,  LabelType::findRecord(defaultLabelTypeNum1, deptNum, dbAccess));
		setLabelType(SecondPrinter, LabelType::findRecord(defaultLabelTypeNum2, deptNum, dbAccess));
		setLabelType(ThirdPrinter,  LabelType::findRecord(defaultLabelTypeNum3, deptNum, dbAccess));


		if(!dbRec->isEANPrefixNull())
			ean13PrefixString = Integer::toString(dbRec->getEANPrefix());

		if(!dbRec->isEANFormatStringNull())
			barCodeNum.setFormatString(EAN13, dbRec->getEANFormatString());
		//Debug::trace("ProductDefault", HobString("ProductDefault(): EAN13 format String = ") + HobString(dbRec->getEANFormatString()) );

		if(!dbRec->isEAN128CFormatStringNull())
			barCodeNum.setFormatString(EAN128C, dbRec->getEAN128CFormatString());
		//Debug::trace("ProductDefault", HobString("ProductDefault(): EAN128C format String = ") + HobString(dbRec->getEAN128CFormatString()) );

		if(!dbRec->isGS1GTINFormatStringNull())
			barCodeNum.setFormatString(GS1GTIN, dbRec->getGS1GTINFormatString());
		//Debug::trace("ProductDefault", HobString("ProductDefault(): GS1 GTIN format String = ") + HobString(dbRec->getGS1GTINFormatString()) );

		if(!dbRec->isGS1AIFormatStringNull())
			barCodeNum.setFormatString(GS1AI, dbRec->getGS1AIFormatString());
		//Debug::trace("ProductDefault", HobString("ProductDefault(): GS1 A1 format String = ") + HobString(dbRec->getGS1AIFormatString()) );

		if (!dbRec->isQRCodeFormatStringNull())
			barCodeNum.setFormatString(QR_CODE, dbRec->getQRCodeFormatString());
		//Debug::trace("ProductDefault", HobString("ProductDefault(): QR format String = ") + HobString(dbRec->getQRCodeFormatString()) );

		if (!dbRec->isDataMatrixFormatStringNull())
			barCodeNum.setFormatString(DATAMATRIX, dbRec->getDataMatrixFormatString());
		//Debug::trace("ProductDefault", HobString("ProductDefault(): Data Matrix format String = ") + HobString(dbRec->getDataMatrixFormatString()) );

		errorCode = RECORD_OK;
	}
}

ProductDefault::~ProductDefault()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("ProductDefault.dtor", "    ProductDefault: " + ptrtostring(this) + "  type " + longtostring(productType) + "  deptNum " + longtostring(deptNum));

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void ProductDefault::init()
{
	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBProductDefault *)NULL;

	initFieldLists();
}

// static
void ProductDefault::initFieldLists()
{
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetProductDeptNum);						// p#
		evolFieldList.push_back(fieldOffsetProductType);						// PT
		evolFieldList.push_back(fieldOffsetDescription);						// dt

		evolFieldList.push_back(fieldOffsetBarcodePrefix);

		evolFieldList.push_back(fieldOffsetByCount);							// bc	
		evolFieldList.push_back(fieldOffsetByCountForced);						// fb

		evolFieldList.push_back(fieldOffsetUnitPrice);							// u$
		evolFieldList.push_back(fieldOffsetUnitPriceForced);					// fp

		evolFieldList.push_back(fieldOffsetPriceModifier);						// pm
		evolFieldList.push_back(fieldOffsetExceptionPrice);						// xp

		evolFieldList.push_back(fieldOffsetDiscountType);						// Dt
		evolFieldList.push_back(fieldOffsetDiscountValue1);						// dp
		evolFieldList.push_back(fieldOffsetDiscountValue2);						// D2
		evolFieldList.push_back(fieldOffsetDiscountValue3);						// D3

		evolFieldList.push_back(fieldOffsetTareAValue);							// ta
		evolFieldList.push_back(fieldOffsetTareAForced);						// fT
		evolFieldList.push_back(fieldOffsetProportionalTare);					// pt
		evolFieldList.push_back(fieldOffsetFixedWeight);						// nw

		evolFieldList.push_back(fieldOffsetShelfLifeDays);						// sl
		evolFieldList.push_back(fieldOffsetShelfLifeHours);						// SL			minutes at the bottom
		evolFieldList.push_back(fieldOffsetProductLifeDays);					// pl
		evolFieldList.push_back(fieldOffsetProductLifeHours);					// PL			minutes at the bottom
		evolFieldList.push_back(fieldOffsetDontPrintSellBy);					// PS
		evolFieldList.push_back(fieldOffsetDontPrintBestBefore);				// Pi
		evolFieldList.push_back(fieldOffsetDontPrintPackedOn);					// PP
		evolFieldList.push_back(fieldOffsetApplySecurity);						// us

		evolFieldList.push_back(fieldOffsetProductReportingClassNum);			// rc

		evolFieldList.push_back(fieldOffsetLabelTypeNum1);						// l1
		evolFieldList.push_back(fieldOffsetLabelRotation1);						// p1
		evolFieldList.push_back(fieldOffsetLabelTypeNum2);						// l2
		evolFieldList.push_back(fieldOffsetLabelRotation2);						// p2
		evolFieldList.push_back(fieldOffsetLabelTypeNum3);						// l3
		evolFieldList.push_back(fieldOffsetLabelRotation3);						// p3
		evolFieldList.push_back(fieldOffsetLabelPlacement1);					// L1
		evolFieldList.push_back(fieldOffsetLabelPlacement2);					// L2
		evolFieldList.push_back(fieldOffsetLabelPlacement3);					// L3

		//evolFieldList.push_back(fieldOffsetPortions);							// P_
		evolFieldList.push_back(fieldOffsetPoints);							    // ps
		evolFieldList.push_back(fieldOffsetIntakeMode);

		evolFieldList.push_back(fieldOffsetGraphicName1);						// G1
		evolFieldList.push_back(fieldOffsetGraphicName2);						// G2
		evolFieldList.push_back(fieldOffsetGraphicName3);						// G3
		evolFieldList.push_back(fieldOffsetGraphicName4);						// G4

		evolFieldList.push_back(fieldOffsetExpandedTextNum);					// r#
		evolFieldList.push_back(fieldOffsetSpecialMessageNum);					// s#
		evolFieldList.push_back(fieldOffsetProdProductNoteNum);					// n#
		evolFieldList.push_back(fieldOffsetProdNutrifactNum);					// N#

		evolFieldList.push_back(fieldOffsetProductionPlanningType);				// Pp					
#ifndef NOTSELFSERVICE
		evolFieldList.push_back(fieldOffsetProductGraphicName);					// i1
		evolFieldList.push_back(fieldOffsetProductName1);						// n1
		evolFieldList.push_back(fieldOffsetProductName2);						// n2
		evolFieldList.push_back(fieldOffsetProdCategoryNum);					// cb
		evolFieldList.push_back(fieldOffsetProdSpecialNum);						// S#
#endif

		evolFieldList.push_back(fieldOffsetEANFormatString);					// ef = G1
		evolFieldList.push_back(fieldOffsetEAN128CFormatString);				// eF = ALV
		evolFieldList.push_back(fieldOffsetGS1DatabarGTIN);						// gs = GS1 GTIN
		evolFieldList.push_back(fieldOffsetGS1DatabarAI);						// gS = GS1 AI
		evolFieldList.push_back(fieldOffsetEANprefix);							// ex = G2

		evolFieldList.push_back(fieldOffsetBuildAMealSideItemsGroupNum);		// Bg
		evolFieldList.push_back(fieldOffsetBuildAMealSideItemsAreFree);			// Bf
		evolFieldList.push_back(fieldOffsetBuildAMealNumberOfSidesIncluded);	// Bn

		evolFieldList.push_back(fieldOffsetCutTestMeatSpecSheetGroupNum);		// cM
		evolFieldList.push_back(fieldOffsetTareBValue);							// ta
		evolFieldList.push_back(fieldOffsetQRCodeFormatString);					// qs = QR Code
		evolFieldList.push_back(fieldOffsetDataMatrixFormatString);				// qS = Data Matrix

		// For automatic label types
		evolFieldList.push_back(fieldOffsetManualLabelTypeNum1);			// l4
		evolFieldList.push_back(fieldOffsetManualLabelTypeNum2);			// l5
		evolFieldList.push_back(fieldOffsetManualLabelTypeNum3);		    // l6
		evolFieldList.push_back(fieldOffsetPrepackLabelTypeNum1);			// l7
		evolFieldList.push_back(fieldOffsetPrepackLabelTypeNum2);			// l8
		evolFieldList.push_back(fieldOffsetPrepackLabelTypeNum3);			// l9
		evolFieldList.push_back(fieldOffsetProdEntryLabelTypeNum1);			// L4
		evolFieldList.push_back(fieldOffsetProdEntryLabelTypeNum2);			// L5
		evolFieldList.push_back(fieldOffsetProdEntryLabelTypeNum3);			// L6
		evolFieldList.push_back(fieldOffsetSelfServLabelTypeNum1);			// L7
		evolFieldList.push_back(fieldOffsetSelfServLabelTypeNum2);			// L8
		evolFieldList.push_back(fieldOffsetSelfServLabelTypeNum3);			// L9
		evolFieldList.push_back(fieldOffsetPick5LabelTypeNum1);				// La
		evolFieldList.push_back(fieldOffsetPick5LabelTypeNum2);				// Lb
		evolFieldList.push_back(fieldOffsetPick5LabelTypeNum3);				// Lc
		evolFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum1);	// Ld
		evolFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum2);	// Le
		evolFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum3);	// Lf
		evolFieldList.push_back(fieldOffsetShelfLifeMinutes);				// SM
		evolFieldList.push_back(fieldOffsetProductLifeMinutes);			    // PM

		ProductTags productTags((FieldOffsetsList *)&evolFieldList);
	}

	if (dbindependentFieldList.empty())
	{
		dbindependentFieldList.push_back(fieldOffsetProductDeptNum);
		dbindependentFieldList.push_back(fieldOffsetProductType);
		dbindependentFieldList.push_back(fieldOffsetDescription);
		dbindependentFieldList.push_back(fieldOffsetBarcodePrefix);
		dbindependentFieldList.push_back(fieldOffsetByCount);
		dbindependentFieldList.push_back(fieldOffsetByCountForced);
		dbindependentFieldList.push_back(fieldOffsetUnitPrice);
		dbindependentFieldList.push_back(fieldOffsetUnitPriceForced);
		dbindependentFieldList.push_back(fieldOffsetPriceModifier);
		dbindependentFieldList.push_back(fieldOffsetExceptionPrice);
		dbindependentFieldList.push_back(fieldOffsetDiscountType);
		dbindependentFieldList.push_back(fieldOffsetDiscountValue1);
		dbindependentFieldList.push_back(fieldOffsetDiscountValue2);
		dbindependentFieldList.push_back(fieldOffsetDiscountValue3);
		dbindependentFieldList.push_back(fieldOffsetTareAValue);
		dbindependentFieldList.push_back(fieldOffsetTareAForced);
		dbindependentFieldList.push_back(fieldOffsetProportionalTare);
		dbindependentFieldList.push_back(fieldOffsetFixedWeight);
				// todo - we don't currently read this from the db record			dbindependentFieldList.push_back(fieldOffsetFluidOunces);
		dbindependentFieldList.push_back(fieldOffsetShelfLifeDays);
		dbindependentFieldList.push_back(fieldOffsetShelfLifeHours);				// minutes at the bottom
		dbindependentFieldList.push_back(fieldOffsetProductLifeDays);
		dbindependentFieldList.push_back(fieldOffsetProductLifeHours);				// minutes at the bottom
		dbindependentFieldList.push_back(fieldOffsetDontPrintSellBy);
		dbindependentFieldList.push_back(fieldOffsetDontPrintBestBefore);
		dbindependentFieldList.push_back(fieldOffsetDontPrintPackedOn);
		dbindependentFieldList.push_back(fieldOffsetApplySecurity);

		dbindependentFieldList.push_back(fieldOffsetProductReportingClassNum);

		dbindependentFieldList.push_back(fieldOffsetLabelTypeNum1);
		dbindependentFieldList.push_back(fieldOffsetLabelRotation1);
		dbindependentFieldList.push_back(fieldOffsetLabelPlacement1);
		dbindependentFieldList.push_back(fieldOffsetLabelTypeNum2);
		dbindependentFieldList.push_back(fieldOffsetLabelRotation2);
		dbindependentFieldList.push_back(fieldOffsetLabelPlacement2);
		dbindependentFieldList.push_back(fieldOffsetLabelTypeNum3);
		dbindependentFieldList.push_back(fieldOffsetLabelRotation3);
		dbindependentFieldList.push_back(fieldOffsetLabelPlacement3);

		dbindependentFieldList.push_back(fieldOffsetPoints);
		dbindependentFieldList.push_back(fieldOffsetIntakeMode);

		dbindependentFieldList.push_back(fieldOffsetGraphicName1);
		dbindependentFieldList.push_back(fieldOffsetGraphicName2);
		dbindependentFieldList.push_back(fieldOffsetGraphicName3);
		dbindependentFieldList.push_back(fieldOffsetGraphicName4);
		dbindependentFieldList.push_back(fieldOffsetProductionPlanningType);
			
		//dbindependentFieldList.push_back(fieldOffsetExpandedTextNum);
		//dbindependentFieldList.push_back(fieldOffsetSpecialMessageNum);
		//dbindependentFieldList.push_back(fieldOffsetProdProductNoteNum);
		//dbindependentFieldList.push_back(fieldOffsetProdNutrifactNum);

		//dbindependentFieldList.push_back(fieldOffsetProductGraphicName);
		//dbindependentFieldList.push_back(fieldOffsetProductName1);
		//dbindependentFieldList.push_back(fieldOffsetProductName2);
		//dbindependentFieldList.push_back(fieldOffsetProdCategoryNum);
		//dbindependentFieldList.push_back(fieldOffsetProdSpecialNum);

		dbindependentFieldList.push_back(fieldOffsetEANFormatString);
		dbindependentFieldList.push_back(fieldOffsetEAN128CFormatString);		// ALV
			
		dbindependentFieldList.push_back(fieldOffsetGS1DatabarGTIN);
		dbindependentFieldList.push_back(fieldOffsetGS1DatabarAI);	

		dbindependentFieldList.push_back(fieldOffsetEANprefix);

		dbindependentFieldList.push_back(fieldOffsetBuildAMealSideItemsGroupNum);
		dbindependentFieldList.push_back(fieldOffsetBuildAMealSideItemsAreFree);
		dbindependentFieldList.push_back(fieldOffsetBuildAMealNumberOfSidesIncluded);

		dbindependentFieldList.push_back(fieldOffsetCutTestMeatSpecSheetGroupNum);

		dbindependentFieldList.push_back(fieldOffsetTareBValue);

		dbindependentFieldList.push_back(fieldOffsetQRCodeFormatString);
		dbindependentFieldList.push_back(fieldOffsetDataMatrixFormatString);

		// For automatic label types
		dbindependentFieldList.push_back(fieldOffsetManualLabelTypeNum1);			// l4
		dbindependentFieldList.push_back(fieldOffsetManualLabelTypeNum2);			// l5
		dbindependentFieldList.push_back(fieldOffsetManualLabelTypeNum3);		    // l6
		dbindependentFieldList.push_back(fieldOffsetPrepackLabelTypeNum1);			// l7
		dbindependentFieldList.push_back(fieldOffsetPrepackLabelTypeNum2);			// l8
		dbindependentFieldList.push_back(fieldOffsetPrepackLabelTypeNum3);			// l9
		dbindependentFieldList.push_back(fieldOffsetProdEntryLabelTypeNum1);		// L4
		dbindependentFieldList.push_back(fieldOffsetProdEntryLabelTypeNum2);		// L5
		dbindependentFieldList.push_back(fieldOffsetProdEntryLabelTypeNum3);		// L6
		dbindependentFieldList.push_back(fieldOffsetSelfServLabelTypeNum1);			// L7
		dbindependentFieldList.push_back(fieldOffsetSelfServLabelTypeNum2);			// L8
		dbindependentFieldList.push_back(fieldOffsetSelfServLabelTypeNum3);			// L9
		dbindependentFieldList.push_back(fieldOffsetPick5LabelTypeNum1);			// La
		dbindependentFieldList.push_back(fieldOffsetPick5LabelTypeNum2);			// Lb
		dbindependentFieldList.push_back(fieldOffsetPick5LabelTypeNum3);			// Lc
		dbindependentFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum1);	// Ld
		dbindependentFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum2);	// Le
		dbindependentFieldList.push_back(fieldOffsetOnlineOrderingLabelTypeNum3);	// Lf
        dbindependentFieldList.push_back(fieldOffsetShelfLifeMinutes);				// SM
		dbindependentFieldList.push_back(fieldOffsetProductLifeMinutes);			// PM

		ProductTags productTags((FieldOffsetsList *)&dbindependentFieldList);
	}
}


HobString ProductDefault::getEAN13PrefixString() const
{
	return ean13PrefixString;
}
void ProductDefault::setEAN13PrefixString(HobString prefixString)
{
	ean13PrefixString = prefixString;
}

HobString ProductDefault::getFormatString(BarcodeType bcType) const
{
	return barCodeNum.getFormatString(bcType);
}
void ProductDefault::setFormatString(BarcodeType bcType, HobString format)
{
	barCodeNum.setFormatString(bcType, format);
}


/*****************************************************************************************
 *
 * updateField: Update an attribute for the field indicated by the fieldCode passed in.
 *              The data for the field is passed in as a string.
 *
 ****************************************************************************************/
bool ProductDefault::updateField(int fieldOffset, const HobString &newValue, bool linksMustExist)
{
	Debug::trace("ProductDefault", HobString("updateField() - fieldCode: ") + HobString(Integer::toString(fieldOffset)) + HobString("  data: ") + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		// ALV
		case fieldOffsetEAN128CFormatString:	//	Only Product Defaults can change these fields
			Debug::trace("ProductDefault", HobString("updateField() - fieldOffsetEAN128CFormatString being set to ") + newValue);
			setFormatString(EAN128C, newValue);
			break;
		case fieldOffsetEANFormatString:		//	Only Product Defaults can change these fields
			Debug::trace("ProductDefault", HobString("updateField() - fieldOffsetEANFormatString being set to ") + newValue);
			setFormatString(EAN13, newValue);
			break;

		case fieldOffsetGS1DatabarGTIN:	//	Only Product Defaults can change these fields
			Debug::trace("ProductDefault", HobString("updateField() - fieldOffsetGS1DatabarGTIN being set to ") + newValue);
			setFormatString(GS1GTIN, newValue);
			break;
		case fieldOffsetGS1DatabarAI:		//	Only Product Defaults can change these fields
			Debug::trace("ProductDefault", HobString("updateField() - fieldOffsetGS1DatabarAI being set to ") + newValue);
			setFormatString(GS1AI, newValue);
			break;
		
		case fieldOffsetQRCodeFormatString:	//	Only Product Defaults can change these fields
			Debug::trace("ProductDefault", HobString("updateField() - fieldOffsetQRCodeFormatString being set to ") + newValue);
			setFormatString(QR_CODE, newValue);
			break;
		case fieldOffsetDataMatrixFormatString:		//	Only Product Defaults can change these fields
			Debug::trace("ProductDefault", HobString("updateField() - fieldOffsetDataMatrixFormatString being set to ") + newValue);
			setFormatString(DATAMATRIX, newValue);
			break;

		case fieldOffsetEANprefix:			//	Product Defaults only
			Debug::trace("ProductDefault", HobString("updateField() - fieldOffsetEANprefix being set to ") + newValue);
			setEAN13PrefixString(newValue);
			break;

		case fieldOffsetProductNum:
		case fieldOffsetBarcodeType:
		case fieldOffsetBarcodeFields:
		case fieldOffsetBarcodeProductNum:
		case fieldOffsetBarcodeUpcNum:
		case fieldOffsetBarcodeVendorNum:
		case fieldOffsetBarcodeEANCode:
		case fieldOffsetSpecialStartDate:
			// These do not exist in the ProductDefaults record, don't update them.
			break;

		default:
			return Product::updateField(fieldOffset, newValue, linksMustExist);
			break;
	}

	return true;
}

void ProductDefault::updateRecord(ProductDefault *newData, AbstractSession *session)
{
	if (!newData)
		return;

	bool isTracing = Debug::isTracing("ProductDefault");
	if (isTracing)
	{
		Debug::trace("ProductDefault", string("updateRecord() - this num ") + longtostring(productNum) + string("  this DeptNum ") + longtostring(deptNum));
		Debug::trace("ProductDefault", string("updateRecord() - passed in num ") + longtostring(newData->getProductNum()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));
	}

	Product::updateRecord(newData, session);

	// ALV
	if (isTracing)
	{
		Debug::trace("ProductDefault", string("updateRecord() - Received fieldOffsetEANFormatString: ") + Bool::toString(newData->isReceived(fieldOffsetEANFormatString)) + string("   data = ") + newData->getFormatString(EAN13));
		Debug::trace("ProductDefault", string("updateRecord() - Received fieldOffset128CEANFormatString: ") + Bool::toString(newData->isReceived(fieldOffsetEAN128CFormatString)) + string("   data = ") + newData->getFormatString(EAN128C));
		Debug::trace("ProductDefault", string("updateRecord() - Received fieldOffsetGS1DatabarGTIN: ") + Bool::toString(newData->isReceived(fieldOffsetGS1DatabarGTIN)) + string("   data = ") + newData->getFormatString(GS1GTIN));
		Debug::trace("ProductDefault", string("updateRecord() - Received fieldOffsetGS1DatabarAI: ") + Bool::toString(newData->isReceived(fieldOffsetGS1DatabarAI)) + string("   data = ") + newData->getFormatString(GS1AI));
		Debug::trace("ProductDefault", string("updateRecord() - Received fieldOffsetQRCodeFormatString: ") + Bool::toString(newData->isReceived(fieldOffsetQRCodeFormatString)) + string("   data = ") + newData->getFormatString(QR_CODE));
		Debug::trace("ProductDefault", string("updateRecord() - Received fieldOffsetDataMatrixFormatString: ") + Bool::toString(newData->isReceived(fieldOffsetDataMatrixFormatString)) + string("   data = ") + newData->getFormatString(DATAMATRIX));
		Debug::trace("ProductDefault", string("updateRecord() - Received fieldOffsetEANprefix: ") + Bool::toString(newData->isReceived(fieldOffsetEANprefix)) + string("   data = ") + newData->getEAN13PrefixString());
	}

	if (newData->isReceived(fieldOffsetEANFormatString))
		setFormatString(EAN13, newData->getFormatString(EAN13));

	if (newData->isReceived(fieldOffsetEAN128CFormatString))
		setFormatString(EAN128C, newData->getFormatString(EAN128C));

	if (newData->isReceived(fieldOffsetGS1DatabarGTIN))
		setFormatString(GS1GTIN, newData->getFormatString(GS1GTIN));

	if (newData->isReceived(fieldOffsetGS1DatabarAI))
		setFormatString(GS1AI, newData->getFormatString(GS1AI));

	if (newData->isReceived(fieldOffsetQRCodeFormatString))
		setFormatString(QR_CODE, newData->getFormatString(QR_CODE));

	if (newData->isReceived(fieldOffsetDataMatrixFormatString))
		setFormatString(DATAMATRIX, newData->getFormatString(DATAMATRIX));

	if (newData->isReceived(fieldOffsetEANprefix))
		setEAN13PrefixString(newData->getEAN13PrefixString());
}


void ProductDefault::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
	case fieldOffsetEANprefix:
		result = ean13PrefixString;
		break;

	case fieldOffsetProductNum:
	case fieldOffsetBarcodeType:
	case fieldOffsetBarcodeFields:
	case fieldOffsetBarcodeProductNum:
	case fieldOffsetBarcodeUpcNum:
	case fieldOffsetBarcodeVendorNum:
	case fieldOffsetBarcodeEANCode:
	case fieldOffsetSpecialStartDate:
		// These do not exist in the ProductDefaults record, don't serialize them.
		break;

	default:
		Product::getFieldAsString(fieldOffset, result, format);
		break;
	}
}

const char *ProductDefault::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<Product::ProductFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
			Product::serialize(format);
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(ean13PrefixString);
			break;

		case EvolutionTaggedFormat:
		case SynchronizationTaggedFormat:
		{
			ProductTags productTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = productTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		case DBIndependentFormatFormat:
		{
			ProductTags productTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = dbindependentFieldList.begin();
			while (fieldIter != dbindependentFieldList.end())
			{
				tag = productTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = string("ProductDefault::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


bool ProductDefault::isRecordValid() const
{
	if (myDbRecord)
		return myDbRecord->isRecordValid();
	else
		return false;
}

int ProductDefault::deleteRecord()
{
	// can't delete these!!
	return 0;
}

void ProductDefault::saveRecord()
{
	if (dbAccess)
	{ 
		DBProductDefaultsRM *dbRM = dbAccess->getDBProductDefaultsRM();
		if (!dbRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("ProductDefault", "saveRecord() - don't have a db record, creating it...");
			myDbRecord = dbRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbProductRecord.
			myDbRecord->setDeptNumber(Long::toString(deptNum));
			myDbRecord->setType(getProductType());
				
			myDbRecord->setDescription(getDescription());

			HobRecordNum reportingClassNum = Class::GRAND_TOTALS_CLASS;
			if (reportingClass != NULL)
				reportingClassNum = reportingClass->getClassNum();

			myDbRecord->setReportingClass(Long::toString(reportingClassNum));

			myDbRecord->setUPCNumberSystem(Integer::valueOf(getBarCodeNum().getUPCNumberSystem()));

			myDbRecord->setByCount(getByCount());
			myDbRecord->setForceByCount(isByCountForced());

			myDbRecord->setExceptionPrice(getExceptionPrice());
			myDbRecord->setPrice(getUnitPrice());
			myDbRecord->setForcePrice(isUnitPriceForced());

			myDbRecord->setDiscountMethod(getDiscountType());
			myDbRecord->setDiscountPrice1(getDiscountValue1());
			myDbRecord->setDiscountPrice2(getDiscountValue2());
			myDbRecord->setDiscountPrice3(getDiscountValue3());

			//	fieldOffsetDiscountLabel1
			//	fieldOffsetDiscountLabel2
			//	fieldOffsetDiscountLabel3

			myDbRecord->setTare(getTareA().getValue());
			myDbRecord->setTareB(getTareB().getValue());
			myDbRecord->setForceTare(getTareA().isForced());
			myDbRecord->setProportionalTare(getProportionalTare());
			myDbRecord->setNetWeight(getFixedWeight());

			myDbRecord->setShelfLifeDays(getShelfLifeDays());
			myDbRecord->setShelfLifeHours(getShelfLifeHours());
			myDbRecord->setShelfLifeMinutes(getShelfLifeMinutes());
			myDbRecord->setProductLifeDays(getProductLifeDays());
			myDbRecord->setProductLifeHours(getProductLifeHours());
			myDbRecord->setProductLifeMinutes(getProductLifeMinutes());

			myDbRecord->setPriceFactor(getPriceModifier());

			Graphic *graphic;
			if ((graphic = getGraphic(Graphic::GRAPHIC1)) == NULL)
				myDbRecord->setGraphic1Null();
			else
				myDbRecord->setGraphic1(graphic->getGraphicName().c_str());

			if ((graphic = getGraphic(Graphic::GRAPHIC2)) == NULL)
				myDbRecord->setGraphic2Null();
			else
				myDbRecord->setGraphic2(graphic->getGraphicName().c_str());

			if ((graphic = getGraphic(Graphic::GRAPHIC3)) == NULL)
				myDbRecord->setGraphic3Null();
			else
				myDbRecord->setGraphic3(graphic->getGraphicName().c_str());

			if ((graphic = getGraphic(Graphic::GRAPHIC4)) == NULL)
				myDbRecord->setGraphic4Null();
			else
				myDbRecord->setGraphic4(graphic->getGraphicName().c_str());

			myDbRecord->setLabel1(Long::toString(defaultLabelTypeNum1));
			myDbRecord->setLabel2(Long::toString(defaultLabelTypeNum2));
			myDbRecord->setLabel3(Long::toString(defaultLabelTypeNum3));
			myDbRecord->setManualLabelTypeNum1(Long::toString(manualLabelTypeNum1));
			myDbRecord->setManualLabelTypeNum2(Long::toString(manualLabelTypeNum2));
			myDbRecord->setManualLabelTypeNum3(Long::toString(manualLabelTypeNum3));
			myDbRecord->setPrepackLabelTypeNum1(Long::toString(prepackLabelTypeNum1));
			myDbRecord->setPrepackLabelTypeNum2(Long::toString(prepackLabelTypeNum2));
			myDbRecord->setPrepackLabelTypeNum3(Long::toString(prepackLabelTypeNum3));
			myDbRecord->setProdEntryLabelTypeNum1(Long::toString(prodEntryLabelTypeNum1));
			myDbRecord->setProdEntryLabelTypeNum2(Long::toString(prodEntryLabelTypeNum2));
			myDbRecord->setProdEntryLabelTypeNum3(Long::toString(prodEntryLabelTypeNum3));
			myDbRecord->setSelfServLabelTypeNum1(Long::toString(selfServLabelTypeNum1));
			myDbRecord->setSelfServLabelTypeNum2(Long::toString(selfServLabelTypeNum2));
			myDbRecord->setSelfServLabelTypeNum3(Long::toString(selfServLabelTypeNum3));
			myDbRecord->setPick5LabelTypeNum1(Long::toString(pick5LabelTypeNum1));
			myDbRecord->setPick5LabelTypeNum2(Long::toString(pick5LabelTypeNum2));
			myDbRecord->setPick5LabelTypeNum3(Long::toString(pick5LabelTypeNum3));
			myDbRecord->setOnlineOrderingLabelTypeNum1(Long::toString(onlineOrderingLabelTypeNum1));
			myDbRecord->setOnlineOrderingLabelTypeNum2(Long::toString(onlineOrderingLabelTypeNum2));
			myDbRecord->setOnlineOrderingLabelTypeNum3(Long::toString(onlineOrderingLabelTypeNum3));

			myDbRecord->setLabel1Rotation(getLabelRotation(FirstPrinter));
			myDbRecord->setLabel2Rotation(getLabelRotation(SecondPrinter));
			myDbRecord->setLabel3Rotation(getLabelRotation(ThirdPrinter));

			myDbRecord->setLabel1Placement(getLabelPlacement(FirstPrinter));
			myDbRecord->setLabel2Placement(getLabelPlacement(SecondPrinter));
			myDbRecord->setLabel3Placement(getLabelPlacement(ThirdPrinter));

			myDbRecord->setBonusPoints(getPoints());
			myDbRecord->setIntakeMode(static_cast<WrapperIntakeMode::IntakeMode> (intakeMode));

			myDbRecord->setApplySecurityLabel(isApplySecurityLabel());

			myDbRecord->setDontPrintPackedOn(isDontPrintPackedOn());
			myDbRecord->setDontPrintShelfLife(isDontPrintSellBy());
			myDbRecord->setDontPrintProductLife(isDontPrintBestBefore());

			myDbRecord->setProductionPlanningType(getProductionPlanningType());

/*=============		NOTE TO WHOEVER - From Clare
			What about the following items in Product:
			Image, Name1, Name2, Category, SpecialNumber, SpecialStartDate
			
			Will we set these up in Product Defaults?????
===============*/

			HobString formatString;

			formatString = barCodeNum.getFormatString(EAN13);
			Debug::trace("ProductDefault", HobString("saveRecord() - ean13FormatString: ") + formatString);
			if (formatString.empty())
				myDbRecord->setEANFormatStringNull();
			else
				myDbRecord->setEANFormatString(formatString);

			// ALV
			formatString = barCodeNum.getFormatString(EAN128C);
			Debug::trace("ProductDefault", HobString("saveRecord() - ean128CFormatString: ") + formatString);
			if (formatString.empty())
				myDbRecord->setEAN128CFormatStringNull();
			else
				myDbRecord->setEAN128CFormatString(formatString);
			
			formatString = barCodeNum.getFormatString(GS1GTIN);
			Debug::trace("ProductDefault", HobString("saveRecord() - gs1GtinFormatString: ") + formatString);
			if (formatString.empty())
				myDbRecord->setGS1GTINFormatStringNull();
			else
				myDbRecord->setGS1GTINFormatString(formatString);

			formatString = barCodeNum.getFormatString(GS1AI);
			Debug::trace("ProductDefault", HobString("saveRecord() - gs1AiFormatString: ") + formatString);
			if (formatString.empty())
				myDbRecord->setGS1AIFormatStringNull();
			else
				myDbRecord->setGS1AIFormatString(formatString);
			
			/////////////////////////////////////////

			formatString = barCodeNum.getFormatString(QR_CODE);
			Debug::trace("ProductDefault", HobString("saveRecord() - qrcodeFormatString: ") + formatString);
			if (formatString.empty())
				myDbRecord->setQRCodeFormatStringNull();
			else
				myDbRecord->setQRCodeFormatString(formatString);

			formatString = barCodeNum.getFormatString(DATAMATRIX);
			Debug::trace("ProductDefault", HobString("saveRecord() - dataMatrixFormatString: ") + formatString);
			if (formatString.empty())
				myDbRecord->setDataMatrixFormatStringNull();
			else
				myDbRecord->setDataMatrixFormatString(formatString);

			/////////////////////////////////////////
			
			Debug::trace("ProductDefault", HobString("saveRecord() - ean13PrefixString: ") + ean13PrefixString);
			if (ean13PrefixString.empty())
				myDbRecord->setEANPrefixNull();
			else
				myDbRecord->setEANPrefix(Integer::valueOf(ean13PrefixString));

			try
			{
				dbRM->saveRecord(myDbRecord);
				Debug::trace("ProductDefault", string("saveRecord() - saved type ") + inttostring(productType) + string("  deptNum ") + longtostring(deptNum));
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "ProductDefault::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}


// static
ProductDefault *ProductDefault::findRecord(short productType, HobRecordNum deptNum, DBAccess *dbAccess)
{
	ProductDefault *rec = (ProductDefault *)NULL;

	if (productType <= PRODUCTTYPE_UNKNOWN || productType >= PRODUCTTYPE_INVALID || deptNum < 0 || !dbAccess)
		return rec;

	DBProductDefaultsRM *dbRM = dbAccess->getDBProductDefaultsRM();
	if (!dbRM)
		return rec;

	try
	{
		DBProductDefault *dbRec = dbRM->findType(inttostring(productType), longtostring(deptNum));
		if (dbRec)
		{
			rec = new ProductDefault(dbRec, dbAccess);
			if (!rec)
				delete dbRec;
			else
			{
				if (Debug::isTracing("ProductDefault"))
					Debug::trace("ProductDefault", string("findRecord() - rec ptr: ") + ptrtostring(rec) + "  type " + inttostring((int)rec->getProductType()) + "  deptNum " + longtostring(rec->getDeptNum()));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("ProductDefault::findRecord() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}


// static
void ProductDefault::verifyReservedRecords(DBAccess *dbAccess)
{
	if (!dbAccess)
	{
		Debug::trace("ProductDefault", "verifyReservedRecords - no dbAccess passed in!");
		return;
	}

	bool traceOn = Debug::isTracing("ProductDefault");
	HobString msgDebug = "verifyReservedRecords() - Created ProductDefault ";
	if (traceOn)
		Debug::trace("ProductDefault", "verifying all reserved ProductDefaults ...");

	AllDepts allDepts;
	allDepts.findAll(dbAccess);
	HobList deptNumList = allDepts.getDeptNumList();

	HobString deptNumStr = "0";
	long deptNum = 0;

	ProductDefault *rec = (ProductDefault *)NULL;
	hfc::MultiLanguageText internationalize;

	while ( !deptNumStr.empty() )
	{
		deptNum = Long::valueOf(deptNumStr);

		rec = ProductDefault::findRecord(Product::PRODUCTTYPE_RANDOMWEIGHT, deptNum, dbAccess);
		if (!rec)
		{
			rec = new ProductDefault();
			if (rec)
			{
				rec->setDeptNum(deptNum);
				rec->setProductType(Product::PRODUCTTYPE_RANDOMWEIGHT);
				rec->setDescription(internationalize("ProductDefault.RWDescription"));
				rec->setUnitPrice((HobPrice)0);
				rec->getBarCodeNum().setUPCNumberSystem("2");
				rec->setFormatString(EAN13, "XXEEEEEPPPPP");
				rec->setFormatString(EAN128C, "XXEEEEEPPPPPWWWWW");
			
				rec->setDBAccess(dbAccess);
				rec->saveRecord();
				if (traceOn)
					Debug::trace("ProductDefault", msgDebug + "PRODUCTTYPE_RANDOMWEIGHT  deptNum " + deptNumStr);
			}
		}
		if (rec)
		{
			delete rec;
			rec = (ProductDefault *)NULL;
		}

		rec = ProductDefault::findRecord(Product::PRODUCTTYPE_FIXEDWEIGHT, deptNum, dbAccess);
		if (!rec)
		{
			rec = new ProductDefault();
			if (rec)
			{
				rec->setDeptNum(deptNum);
				rec->setProductType(Product::PRODUCTTYPE_FIXEDWEIGHT);
				rec->setDescription(internationalize("ProductDefault.FWDescription"));
				rec->setUnitPrice((HobPrice)0);
				rec->setByCount(1);
				rec->setFixedWeight((HobWeight)0);
				rec->getBarCodeNum().setUPCNumberSystem("0");
				rec->setFormatString(EAN13, "XXEEEEEPPPPP");
				rec->setFormatString(EAN128C, "XXEEEEEPPPPPWWWWW");

				rec->setDBAccess(dbAccess);
				rec->saveRecord();
				if (traceOn)
					Debug::trace("ProductDefault", msgDebug + "PRODUCTTYPE_FIXEDWEIGHT  deptNum " + deptNumStr);
			}
		}
		if (rec)
		{
			delete rec;
			rec = (ProductDefault *)NULL;
		}

		rec = ProductDefault::findRecord(Product::PRODUCTTYPE_BYCOUNT, deptNum, dbAccess);
		if (!rec)
		{
			rec = new ProductDefault();
			if (rec)
			{
				rec->setDeptNum(deptNum);
				rec->setProductType(Product::PRODUCTTYPE_BYCOUNT);
				rec->setDescription(internationalize("ProductDefault.BCDescription"));
				rec->setUnitPrice((HobPrice)0);
				rec->setByCount(1);
				rec->getBarCodeNum().setUPCNumberSystem("2");
				rec->setFormatString(EAN13, "XXEEEEEPPPPP");
				rec->setFormatString(EAN128C, "XXEEEEEPPPPPWWWWW");

				rec->setDBAccess(dbAccess);
				rec->saveRecord();
				if (traceOn)
					Debug::trace("ProductDefault", msgDebug + "PRODUCTTYPE_BYCOUNT  deptNum " + deptNumStr);
			}
		}
		if (rec)
		{
			delete rec;
			rec = (ProductDefault *)NULL;
		}

		rec = ProductDefault::findRecord(Product::PRODUCTTYPE_FLUIDOUNCE, deptNum, dbAccess);
		if (!rec)
		{
			rec = new ProductDefault();
			if (rec)
			{
				rec->setDeptNum(deptNum);
				rec->setProductType(Product::PRODUCTTYPE_FLUIDOUNCE);
				rec->setDescription(internationalize("ProductDefault.FODescription"));
				rec->setUnitPrice((HobPrice)0);
				rec->setByCount(1);
				rec->setFixedWeight((HobWeight)0);
				rec->getBarCodeNum().setUPCNumberSystem("0");
				rec->setFormatString(EAN13, "XXEEEEEPPPPP");
				rec->setFormatString(EAN128C, "XXEEEEEPPPPPWWWWW");

				rec->setDBAccess(dbAccess);
				rec->saveRecord();
				if (traceOn)
					Debug::trace("ProductDefault", msgDebug + "PRODUCTTYPE_FLUIDOUNCE  deptNum " + deptNumStr);
			}
		}
		if (rec)
		{
			delete rec;
			rec = (ProductDefault *)NULL;
		}

		// Get the next dept record from the database.  If the next one is the default dept (Dept 0), skip it and get the next one, since we
		// automatically created the reserved records for dept 0 in the first pass through the while loop (since deptNumStr was initialized to "0").
		// If the database doesn't contain the default dept record, we still need the reserved records for the default dept (Dept 0).
		deptNumStr = "";
		if (deptNumList.next(deptNumStr) != -1 && deptNumStr == "0")
		{
			deptNumStr = "";
			deptNumList.next(deptNumStr);
		}
	}
}

