#include "Transaction.h"


#include "tags/TransactionTags.h"

#include "HobOperator.h"
#include "LabelType.h"

#include "Configuration.h"

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int Transaction::_dataType_ = 0;
	string Transaction::_dataTypeName_("hfc::Transaction");
#endif

const std::string Transaction::_VoidInvalidName_                       ("Invalid");
const std::string Transaction::_VoidNotVoidedName_                     ("Not Voided");
const std::string Transaction::_VoidVoidedName_                        ("Voided");
const std::string Transaction::_VoidVoidingName_                       ("Voiding");

const std::string Transaction::_ProductionInvalidName_                 ("Invalid");
const std::string Transaction::_ProductionNegativelyName_              ("Negatively");
const std::string Transaction::_ProductionNoImpactName_                ("No Impact");
const std::string Transaction::_ProductionPositivelyName_              ("Positively");

const std::string Transaction::_ReadClearInvalidName_                  ("Invalid");
const std::string Transaction::_ReadClearNotReadName_                  ("Not Read");
const std::string Transaction::_ReadClearReadName_                     ("Read");
const std::string Transaction::_ReadClearClearedName_                  ("Cleared");

const std::string Transaction::_OperatorChangeTypeNoChange_            ("No Change");
const std::string Transaction::_OperatorChangeUnitPriceName_           ("Unit Price");
const std::string Transaction::_OperatorChangeTareName_                ("Tare");
const std::string Transaction::_OperatorChangeByCountName_             ("By Count");
const std::string Transaction::_OperatorChangeShelfLifeName_           ("Shelf Life Days");
const std::string Transaction::_OperatorChangeProductLifeName_         ("Product Life Days");
const std::string Transaction::_OperatorChangeLabelType1Name_          ("Label Type 1");
const std::string Transaction::_OperatorChangeLabelType2Name_          ("Label Type 2");
const std::string Transaction::_OperatorChangeFixedWeightName_         ("Fixed Weight");
const std::string Transaction::_OperatorChangeUPCDigitName_            ("UPC Digit");
const std::string Transaction::_OperatorChangeFinalPriceName_          ("Final Price");
const std::string Transaction::_OperatorChangeGraphic1Name_            ("Graphic 1");
const std::string Transaction::_OperatorChangeShelfLifeHoursName_      ("Shelf Life Hours");
const std::string Transaction::_OperatorChangeShelfLifeMinutesName_    ("Shelf Life Minutes");        // already had this
const std::string Transaction::_OperatorChangeProductLifeHoursName_    ("Product Life Hours");
const std::string Transaction::_OperatorChangeProductLifeMinutesName_  ("Product Life Minutes");      // already had this
const std::string Transaction::_OperatorChangeLabelType3Name_          ("Label Type 3");
const std::string Transaction::_OperatorChangeLabelType4Name_          ("Label Type 4");
const std::string Transaction::_OperatorChangeGraphic2Name_            ("Graphic 2");
const std::string Transaction::_OperatorChangeGraphic3Name_            ("Graphic 3");
const std::string Transaction::_OperatorChangeGraphic4Name_            ("Graphic 4");
const std::string Transaction::_OperatorChangeTareBName_               ("Tare B");

const std::string Transaction::_scaleIdName_                           = "Scale Id";
const std::string Transaction::_transactionIdName_                     = "Transactions Id";
const std::string Transaction::_transactionGroupName_                  = "Transaction Group";
const std::string Transaction::_transactionTypeName_                   = "Transaction Type";
const std::string Transaction::_transactionCommittedDateTimeName_      = "Transaction Committed";
const std::string Transaction::_transactionVoidName_                   = "Transaction Void";
const std::string Transaction::_transactionAffectsProductionName_      = "Transaction Affects Production";
const std::string Transaction::_transactionReadAndClearName_           = "Transaction Read and Clear";
const std::string Transaction::_transactionPackagesNumberName_         = "Transaction Packages";
const std::string Transaction::_transactionPrintedSecurityLabelName_   = "Transaction Printed Security Label";
const std::string Transaction::_transactionCOOLFlagName_               = "Transaction COOL Flag";
const std::string Transaction::_transactionTotalValueName_             = "Transaction Total Value";
const std::string Transaction::_transactionOldTotalValueName_          = "Transaction Old Total Value";
const std::string Transaction::_transactionCutTestNumber_			   = "Transaction Cut Test Number";
const std::string Transaction::_transactionPercentOfPrimalWeight_	   = "Transaction Cut Test Percent Of Primal Weight";
const std::string Transaction::_transactionPercentOfPrimalCost_		   = "Transaction Cut Test Percent Of Primal Cost";
const std::string Transaction::_deptNumName_							= "Dept Num";
const std::string Transaction::_productNumberName_                     = "Product Number";
const std::string Transaction::_productReportingClassName_             = "Product Reporting Class";
const std::string Transaction::_productTypeName_                       = "Product Type";
const std::string Transaction::_productionPlanningTypeName_            = "Production Planning Type";
const std::string Transaction::_productDescriptionName_                = "Product Description";
const std::string Transaction::_productPriceModifierName_              = "Product Price Modifier";
const std::string Transaction::_productBarcodePrefixName_              = "Product Barcode Prefix";
const std::string Transaction::_productBarcodeNumberName_              = "Product Barcode Number";
const std::string Transaction::_productUnitPriceName_                  = "Product Unit Price";
const std::string Transaction::_productExceptionPriceName_             = "Product Exception Price";
const std::string Transaction::_productFinalPriceName_                 = "Product Final Price";
const std::string Transaction::_productTareName_                       = "Product Tare";
const std::string Transaction::_productOldTareName_                    = "Product Old Tare";
const std::string Transaction::_productTareBName_                      = "Product Tare B";
const std::string Transaction::_productOldTareBName_                   = "Product Old Tare B";
const std::string Transaction::_productProportionalTareName_           = "Product Proportional Tare";
const std::string Transaction::_productFinalTareName_                  = "Product Final Tare";
const std::string Transaction::_productByCountName_                    = "Product By Count";
const std::string Transaction::_productFinalNetWeightName_             = "Product Final Net Weight";
const std::string Transaction::_productFixedWeightName_                = "Product Fixed Weight";
const std::string Transaction::_productShelfLifeDaysName_              = "Product Shelf Life Days";
const std::string Transaction::_productShelfLifeHoursName_             = "Product Shelf Life Hours";
const std::string Transaction::_productShelfLifeMinutesName_           = "Product Shelf Life Minutes";
const std::string Transaction::_productLifeDaysName_                   = "Product Life Days";
const std::string Transaction::_productLifeHoursName_                  = "Product Life Hours";
const std::string Transaction::_productLifeMinutesName_                = "Product Life Minutes";
const std::string Transaction::_productLabel1Name_                     = "Product Label 1";
const std::string Transaction::_productLabel2Name_                     = "Product Label 2";
const std::string Transaction::_productLabel3Name_                     = "Product Label 3";
const std::string Transaction::_productLabel4Name_                     = "Product Label 4";
const std::string Transaction::_productGraphicName1Name_               = "Product Graphic Name 1";
const std::string Transaction::_productGraphicName2Name_               = "Product Graphic Name 2";
const std::string Transaction::_productGraphicName3Name_               = "Product Graphic Name 3";
const std::string Transaction::_productGraphicName4Name_               = "Product Graphic Name 4";
const std::string Transaction::_productCoolTextName_                   = "Product COOL Text";
const std::string Transaction::_productCoolTrackingTextName_           = "Product COOL Tracking Text";
const std::string Transaction::_productOldBarcodeDigitName_            = "Product Old Barcode Digit";
const std::string Transaction::_productOldUnitPriceName_               = "Product Old Unit Price";
const std::string Transaction::_productOldFinalPriceName_              = "Product Old Final Price";
const std::string Transaction::_productOldByCountName_                 = "Product Old By Count";
const std::string Transaction::_productOldFixedWeightName_             = "Product Old Fixed Weight";
const std::string Transaction::_productOldShelfLifeDaysName_           = "Product Old Shelf Life Days";
const std::string Transaction::_productOldShelfLifeHoursName_          = "Product Old Shelf Life Hours";
const std::string Transaction::_productOldShelfLifeMinutesName_        = "Product Old Shelf Life Minutes";
const std::string Transaction::_productOldProductLifeDaysName_         = "Product Old Product Life Days";
const std::string Transaction::_productOldProductLifeHoursName_        = "Product Old Product Life Hours";
const std::string Transaction::_productOldProductLifeMinutesName_      = "Product Old Product Life Minutes";
const std::string Transaction::_productOldLabel1Name_                  = "Product Old Label 1";
const std::string Transaction::_productOldLabel2Name_                  = "Product Old Label 2";
const std::string Transaction::_productOldLabel3Name_                  = "Product Old Label 3";
const std::string Transaction::_productOldLabel4Name_                  = "Product Old Label 4";
const std::string Transaction::_productOldGraphicName1Name_            = "Product Old Graphic Name 1";
const std::string Transaction::_productOldGraphicName2Name_            = "Product Old Graphic Name 2";
const std::string Transaction::_productOldGraphicName3Name_            = "Product Old Graphic Name 3";
const std::string Transaction::_productOldGraphicName4Name_            = "Product Old Graphic Name 4";

const std::string Transaction::_operatorIdName_                        = "Operator Id";
const std::string Transaction::_operatorNameName_                      = "Operator Name";
const std::string Transaction::_operatorChangeTypeName_                = "Operator Change Type";
const std::string Transaction::_modeChangeName_                        = "Operator Mode Change";
const std::string Transaction::_sweetheartWeightName_                  = "Sweetheart Weight";

const std::string Transaction::_unknownFieldOffsetName_                = "Unknown field"; 

FieldOffsetsList Transaction::_dbIndependentFieldList_;
FieldOffsetsList Transaction::_evolTaggedFieldList_;
FieldOffsetsList Transaction::_readCompatibilityFieldList_;


Transaction::Transaction()
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("Transaction", "Transaction(): default constructor");

	init();
}


Transaction::Transaction(int transactionType, int scaleId)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();

	scaleId_ = scaleId;
	transactionTypeNumber_ = transactionType;
	Debug::trace("Transaction", "Transaction(" + inttostring(transactionType) + ", " + inttostring(scaleId) + ")");
}


Transaction::Transaction(const char * data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("Transaction", "Transaction(const char* data, int len): reconstructing Transaction");

	init();

	switch ( formatType )
	{
		case DBIndependentFormatFormat:
		{
			TransactionTags transactionTags;
			HobString evolData(data, len);                    // convert data to HobString
			HobTokenizerUS tok(&evolData, UnitSeparator);     // use the UnitSeparator to parse the string
			HobString temp;
			int offset = 0;
			HobTokenTagged * token = dynamic_cast<HobTokenTagged *>(HobToken::create(temp, HobTaggedToken));
			if ( token )
			{
				while ( tok.next(token) != -1 )
				{
					//get the tag and find out what offset goes with the tag
		  	 		temp = token->getTag();
					offset = temp.size() ? transactionTags.getOffset(temp) : 0;
					if ( offset )
					{
						setReceived(offset, true);
						switch ( offset )
						{
							case fieldOffsetScaleId:
								setScaleId(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetTransactionId:
								setTransactionId(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetTransactionGroup:
							{
								int number = util_string::stringtolong(token->getData());
								
								setTransactionGroupNumber( number == 0 ? RECORD_NOT_FOUND : number );
								
								break;
							}
							case fieldOffsetTransactionCommittedDateTime:
							{
								const std::string s(token->getData());
								
								HobDateTime hdt;
								hdt.convertFromDBFormat(s);

								setCommittedDateTime(hdt);
								break;
							}
							case fieldOffsetTransactionVoid:
								setVoid((VoidFlag)util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionType:
								setTransactionTypeNumber(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionAffectsProduction:
								setAffectsProductionFlag((TransactionType::AffectsProductionFlag)util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionReadAndClear:
								setReadClearFlag((ReadClearFlag)util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionPackagesNumber:
								setNumberPackages(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionPrintedSecurityLabel:
								setPrintedSecurityLabel(util_string::stringtobool(token->getData()));
								break;
							case fieldOffsetTransactionCOOLFlag:
								setCOOLTransaction(util_string::stringtobool(token->getData()));
								break;
							case fieldOffsetTransactionTotalValue:
								setTotalValue(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetTransactionOldTotalValue:
								setOldTotalValue(util_string::stringtolong(token->getData()));
								break;

							case fieldOffsetProductNumber:
								setProductNumber(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductReportingClass:
								setReportingClass(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductType:
								setProductType((Product::ProductType)util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductionPlanningType:
								setProductionPlanningType((Product::ProductionPlanningType)util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductDescription:
								setProductDescription(token->getData());
								break;
							case fieldOffsetProductPriceModifier:
								setPriceModifier(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductBarcodePrefix:
								setBarcodePrefix(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductBarcodeNumber:
								setBarcodeNumber(token->getData());
								break;
							case fieldOffsetProductUnitPrice:
								setUnitPrice(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductExceptionPrice:
								setExceptionPrice(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductFinalPrice:
								setFinalPrice(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductTare:
								setTare(util_string::stringtolong(token->getData()));
								break;
                            case fieldOffsetProductTareB:
                                setTareB(util_string::stringtolong(token->getData()));
                                break;
							case fieldOffsetProductProportionalTare:
								setProportionalTare(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductFinalTare:
								setFinalTare(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductByCount:
								setByCount(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductFinalNetWeight:
								setFinalNetWeight(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductFixedWeight:
								setFixedWeight(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductShelfLifeDays:
								setShelfLifeDays(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductShelfLifeHours:
								setShelfLifeHours(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductShelfLifeMinutes:
								setShelfLifeMinutes(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductLifeDays:
								setProductLifeDays(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductLifeHours:
								setProductLifeHours(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductLifeMinutes:
								setProductLifeMinutes(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductLabel1:
								setLabel1(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductLabel2:
								setLabel2(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductLabel3:
								setLabel3(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductLabel4:
								setLabel4(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductGraphicName1:
								setGraphicName1(token->getData());
								break;
							case fieldOffsetProductGraphicName2:
								setGraphicName2(token->getData());
								break;
							case fieldOffsetProductGraphicName3:
								setGraphicName3(token->getData());
								break;
							case fieldOffsetProductGraphicName4:
								setGraphicName4(token->getData());
								break;
							case fieldOffsetProductCoolTrackingText:
								setCOOLTrackingText(token->getData());
								break;
							case fieldOffsetProductCoolText:
								setCOOLText(token->getData());
								break;
							case fieldOffsetProductOldBarcodeDigit:
								setOldBarcodeDigit(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldUnitPrice:
								setOldUnitPrice(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldFinalPrice:
								setOldFinalPrice(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldTare:
								setOldTare(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldTareB:
								setOldTareB(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldByCount:
								setOldByCount(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldFixedWeight:
								setOldFixedWeight(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldShelfLifeDays:
								setOldShelfLifeDays(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldShelfLifeHours:
								setOldShelfLifeHours(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldShelfLifeMinutes:
								setOldShelfLifeMinutes(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldProductLifeDays:
								setOldProductLifeDays(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldProductLifeHours:
								setOldProductLifeHours(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldProductLifeMinutes:
								setOldProductLifeMinutes(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldLabel1:
								setLabel1(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldLabel2:
								setLabel2(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldLabel3:
								setLabel3(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldLabel4:
								setLabel4(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldGraphicName1:
								setOldGraphicName1(token->getData());
								break;
							case fieldOffsetProductOldGraphicName2:
								setOldGraphicName2(token->getData());
								break;
							case fieldOffsetProductOldGraphicName3:
								setOldGraphicName3(token->getData());
								break;
							case fieldOffsetProductOldGraphicName4:
								setOldGraphicName4(token->getData());
								break;

							case fieldOffsetOperatorId:
								setOperatorId(token->getData());
								break;
							case fieldOffsetOperatorName:
								setOperatorName(token->getData());
								break;
							case fieldOffsetOperatorChangeType:
								setOperatorChangeType(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetOperatorAccessLevel:
								setOperatorAccessLevel(token->getData());
								break;

							case fieldOffsetTransactionCutTestNumber:
								this->setCutTestNumber(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetTransactionPercentOfPrimalWeight:
								this->setPercentOfPrimalWeight(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionPercentOfPrimalCost:
								this->setPercentOfPrimalCost(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionModeChange:
								this->setModeChange(util_string::stringtoint(token->getData()));
								break;

							case fieldOffsetSweetheartWeight:
								setSweetheartWeight(util_string::stringtolong(token->getData()));
								break;

							default:
								break;
						}
					}
				}	

				delete token;
			}

			break;
		}

		case SynchronizationTaggedFormat:
		{
			TransactionTags transactionTags;
			HobString evolData(data, len);                    // convert data to HobString
			HobTokenizerUS tok(&evolData, UnitSeparator);     // use the UnitSeparator to parse the string
			HobString temp;
			int offset = 0;
			HobTokenTagged * token = dynamic_cast<HobTokenTagged *>(HobToken::create(temp, HobTaggedToken));
			if ( token )
			{
				while ( tok.next(token) != -1 )
				{
					//get the tag and find out what offset goes with the tag
		  	 		temp = token->getTag();
					offset = temp.size() ? transactionTags.getOffset(temp) : 0;
					if ( offset )
					{
						setReceived(offset, true);
						switch ( offset )
						{

							case fieldOffsetTransactionId:
								setTransactionId(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetTransactionGroup:
								setTransactionGroupNumber(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetTransactionCommittedDateTime:
							{
								const std::string s(token->getData());

								HobDateTime hdt;
								hdt.convertFromDBFormat(s);

								setCommittedDateTime(hdt);
								break;
							}
							case fieldOffsetTransactionCommittedDate:
								break;
							case fieldOffsetTransactionCommittedTime:
								break;
							case fieldOffsetTransactionCommittedDayWeek:
								break;
							case fieldOffsetTransactionVoid:
								setVoid((VoidFlag)util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionType:
								setTransactionTypeNumber(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionAffectsProduction:
								setAffectsProductionFlag((TransactionType::AffectsProductionFlag)util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionPackagesNumber:
								setNumberPackages(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionPrintedSecurityLabel:
								setPrintedSecurityLabel(util_string::stringtobool(token->getData()));
								break;
							case fieldOffsetTransactionCOOLFlag:
								setCOOLTransaction(util_string::stringtobool(token->getData()));
								break;
							case fieldOffsetTransactionTotalValue:
								setTotalValue(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetTransactionOldTotalValue:
								setOldTotalValue(util_string::stringtolong(token->getData()));
								break;

							case fieldOffsetProductNumber:
								setProductNumber(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductReportingClass:
								setReportingClass(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductType:
								setProductType((Product::ProductType)util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductionPlanningType:
								setProductionPlanningType((Product::ProductionPlanningType)util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductDescription:
								setProductDescription(token->getData());
								break;
							case fieldOffsetProductPriceModifier:
								setPriceModifier(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductBarcodePrefix:
								setBarcodePrefix(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductBarcodeNumber:
								setBarcodeNumber(token->getData());
								break;
							case fieldOffsetProductUnitPrice:
								setUnitPrice(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductExceptionPrice:
								setExceptionPrice(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductFinalPrice:
								setFinalPrice(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductTare:
								setTare(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductTareB:
								setTareB(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductProportionalTare:
								setProportionalTare(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductFinalTare:
								setFinalTare(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductByCount:
								setByCount(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductFinalNetWeight:
								setFinalNetWeight(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductFixedWeight:
								setFixedWeight(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductShelfLifeDays:
								setShelfLifeDays(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductShelfLifeHours:
								setShelfLifeHours(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductShelfLifeMinutes:
								setShelfLifeMinutes(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductLifeDays:
								setProductLifeDays(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductLifeHours:
								setProductLifeHours(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductLifeMinutes:
								setProductLifeMinutes(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductLabel1:
								setLabel1(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductLabel2:
								setLabel2(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductLabel3:
								setLabel3(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductLabel4:
								setLabel4(util_string::stringtolong(token->getData()));
								break;

							case fieldOffsetProductGraphicName1:
								setGraphicName1(token->getData());
								break;
							case fieldOffsetProductGraphicName2:
								setGraphicName2(token->getData());
								break;
							case fieldOffsetProductGraphicName3:
								setGraphicName3(token->getData());
								break;
							case fieldOffsetProductGraphicName4:
								setGraphicName4(token->getData());
								break;
							case fieldOffsetProductCoolTrackingText:
								setCOOLTrackingText(token->getData());
								break;
							case fieldOffsetProductCoolText:
								setCOOLText(token->getData());
								break;
							case fieldOffsetProductPLUFlags:
								{
									int flags = util_string::stringtoint(token->getData());
									
									setPrintedSecurityLabel((flags & PRPrintSecurityLabel) == PRPrintSecurityLabel);
									if (flags & PRFixedWeight)
										setProductType(Product::PRODUCTTYPE_FIXEDWEIGHT);
									if (flags & PRFluidOunce)
										setProductType(Product::PRODUCTTYPE_FLUIDOUNCE);

								}
								break;


							case fieldOffsetProductOldBarcodeDigit:
								setOldBarcodeDigit(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldUnitPrice:
								setOldUnitPrice(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldFinalPrice:
								setOldFinalPrice(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldTare:
								setOldTare(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldTareB:
								setOldTareB(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldByCount:
								setOldByCount(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldFixedWeight:
								setOldFixedWeight(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldShelfLifeDays:
								setOldShelfLifeDays(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldShelfLifeHours:
								setOldShelfLifeHours(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldShelfLifeMinutes:
								setOldShelfLifeMinutes(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldProductLifeDays:
								setOldProductLifeDays(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldProductLifeHours:
								setOldProductLifeHours(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldProductLifeMinutes:
								setOldProductLifeMinutes(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetProductOldLabel1:
								setLabel1(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldLabel2:
								setLabel2(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldLabel3:
								setLabel3(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetProductOldLabel4:
								setLabel4(util_string::stringtolong(token->getData()));
								break;
							
							case fieldOffsetProductOldGraphicName1:
								setOldGraphicName1(token->getData());
								break;
							case fieldOffsetProductOldGraphicName2:
								setOldGraphicName2(token->getData());
								break;
							case fieldOffsetProductOldGraphicName3:
								setOldGraphicName3(token->getData());
								break;
							case fieldOffsetProductOldGraphicName4:
								setOldGraphicName4(token->getData());
								break;

							case fieldOffsetOperatorId:
								setOperatorId(token->getData());
								break;
							case fieldOffsetOperatorRFID:
								setOperatorRFID(token->getData());
								break;
							case fieldOffsetOperatorName:
								setOperatorName(token->getData());
								break;
							case fieldOffsetOperatorChangeType:
								setOperatorChangeType(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetOperatorAccessLevel:
								setOperatorAccessLevel(token->getData());
								break;

							case fieldOffsetTransactionCutTestNumber:
								this->setCutTestNumber(util_string::stringtolong(token->getData()));
								break;
							case fieldOffsetTransactionPercentOfPrimalWeight:
								this->setPercentOfPrimalWeight(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionPercentOfPrimalCost:
								this->setPercentOfPrimalCost(util_string::stringtoint(token->getData()));
								break;
							case fieldOffsetTransactionModeChange:
								this->setModeChange(util_string::stringtoint(token->getData()));
								break;

							case fieldOffsetSweetheartWeight:
								this->setSweetheartWeight(util_string::stringtolong(token->getData()));
								break;

							default:
								break;
						}
					}
				}	

				delete token;
			}

			break;
		}
		default:
			break;
	}
}


Transaction::Transaction(const Transaction& rhs)
    : HfcObject()
	{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("Transaction", "Transaction(const Transaction& rhs):  Copy constructor");
	errorCode_                = rhs.errorCode_;

	scaleId_                  = rhs.scaleId_; /* = This scale */
	transactionId_            = rhs.transactionId_;
	groupNumber_              = rhs.groupNumber_;
	transactionTypeNumber_    = rhs.transactionTypeNumber_;
	transactionTypeName_      = rhs.transactionTypeName_;
	committed_                = rhs.committed_;
	committedDateTime_        = rhs.committedDateTime_;
	void_                     = rhs.void_;
	affectsProduction_        = rhs.affectsProduction_;
	readClear_                = rhs.readClear_;
	numberPackages_           = rhs.numberPackages_;
	printedSecurityLabel_     = rhs.printedSecurityLabel_;
	coolTransaction_          = rhs.coolTransaction_;
	totalValue_               = rhs.totalValue_;
	oldTotalValue_            = rhs.oldTotalValue_;

	deptNum_                  = rhs.deptNum_;
	productNumber_            = rhs.productNumber_;
	reportingClass_           = rhs.reportingClass_;
	productType_              = rhs.productType_;
	productionPlanningType_   = rhs.productionPlanningType_;
	productDescription_       = rhs.productDescription_;
	priceModifier_            = rhs.priceModifier_;
	barcodePrefix_            = rhs.barcodePrefix_;
	barcodeNumber_            = rhs.barcodeNumber_;
	unitPrice_                = rhs.unitPrice_;
	exceptionPrice_           = rhs.exceptionPrice_;
	finalPrice_               = rhs.finalPrice_;
	tare_                     = rhs.tare_;
//    tareB_                    = rhs.tareB_;
	proportionalTare_         = rhs.proportionalTare_;
	finalTare_                = rhs.finalTare_;
	byCount_                  = rhs.byCount_;
	byCountForced_            = rhs.byCountForced_;
	finalNetWeight_           = rhs.finalNetWeight_;
	fixedWeight_              = rhs.fixedWeight_;
	shelfLifeDays_            = rhs.shelfLifeDays_;
	shelfLifeHours_           = rhs.shelfLifeHours_;
	shelfLifeMinutes_         = rhs.shelfLifeMinutes_;
	productLifeDays_          = rhs.productLifeDays_;
	productLifeHours_         = rhs.productLifeHours_;
	productLifeMinutes_       = rhs.productLifeMinutes_;

	label1_                   = rhs.label1_;
	label2_                   = rhs.label2_;
	label3_                   = rhs.label3_;
	label4_                   = rhs.label4_;

	graphicName1_             = rhs.graphicName1_;
	graphicName2_             = rhs.graphicName2_;
	graphicName3_             = rhs.graphicName3_;
	graphicName4_             = rhs.graphicName4_;
	coolText_                 = rhs.coolText_;
	coolTrackingText_         = rhs.coolTrackingText_;

	oldBarcodeDigit_          = rhs.oldBarcodeDigit_;
	oldUnitPrice_             = rhs.oldUnitPrice_;
	oldFinalPrice_            = rhs.oldFinalPrice_;
	oldTare_                  = rhs.oldTare_;
    oldTareB_                 = rhs.oldTareB_;
	oldByCount_               = rhs.oldByCount_;
	oldFixedWeight_           = rhs.oldFixedWeight_;
	oldShelfLifeDays_         = rhs.oldShelfLifeDays_;
	oldShelfLifeHours_        = rhs.oldShelfLifeHours_;
	oldShelfLifeMinutes_      = rhs.oldShelfLifeMinutes_;
	oldProductLifeDays_       = rhs.oldProductLifeDays_;
	oldProductLifeHours_      = rhs.oldProductLifeHours_;
	oldProductLifeMinutes_    = rhs.oldProductLifeMinutes_;
	oldLabel1_                = rhs.oldLabel1_;
	oldLabel2_                = rhs.oldLabel2_;
	oldLabel3_                = rhs.oldLabel3_;
	oldLabel4_                = rhs.oldLabel4_;
	oldGraphicName1_          = rhs.oldGraphicName1_;
	oldGraphicName2_          = rhs.oldGraphicName2_;
	oldGraphicName3_          = rhs.oldGraphicName3_;
	oldGraphicName4_          = rhs.oldGraphicName4_;

	operatorId_               = rhs.operatorId_;
	operatorRFID_             = rhs.operatorRFID_;
	operatorName_             = rhs.operatorName_;
	operatorChangeType_       = rhs.operatorChangeType_;

	cutTestNumber_            = rhs.cutTestNumber_;
	percentOfPrimalWeight_    = rhs.percentOfPrimalWeight_;
	percentOfPrimalCost_      = rhs.percentOfPrimalCost_;
    modeChange_               = rhs.modeChange_;
    sweetheartWeight_         = rhs.sweetheartWeight_;

    shelfLifeMinutes_         = rhs.shelfLifeMinutes_;
    productLifeMinutes_       = rhs.productLifeMinutes_;
	oldShelfLifeMinutes_      = rhs.oldShelfLifeMinutes_;
	oldProductLifeMinutes_    = rhs.oldProductLifeMinutes_;
     
	serializedField_.clear();
}


Transaction::~Transaction()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


void Transaction::init()
{
	errorCode_ = RECORD_NOT_FOUND;

	scaleId_ = 0; /* = This scale */
	transactionId_ = RECORD_NOT_FOUND;
	groupNumber_ = RECORD_NOT_FOUND;
	transactionTypeNumber_ = RECORD_NOT_FOUND;
	transactionTypeName_.clear();
	committed_ = false;
	committedDateTime_.clear();
	void_ = VoidNotVoided;
	affectsProduction_ = TransactionType::AffectsProductionNone;
	readClear_ = ReadClearNotRead;
	numberPackages_ = 0;
	printedSecurityLabel_ = false;
	coolTransaction_ = false;
	totalValue_ = 0;
	oldTotalValue_ = 0;

	deptNum_ = 0;
	productNumber_ = RECORD_NOT_FOUND;
	reportingClass_ = RECORD_NOT_FOUND;
	productType_ = Product::PRODUCTTYPE_UNKNOWN;
	productionPlanningType_ = Product::PRODUCTIONPLANNING_NO;
	productDescription_.clear();
	priceModifier_ = UNIT_PRICING;
	barcodePrefix_ = 2;
	barcodeNumber_.clear();
	unitPrice_ = 0;
	exceptionPrice_ = 0;
	finalPrice_ = 0;
	tare_ = 0;
    tareB_ = 0;
	proportionalTare_ = 0;
	finalTare_ = 0;
	byCount_ = 0;
	byCountForced_ = false;
	byCountOldZero_ = false;
	finalNetWeight_ = 0;
	fixedWeight_ = 0;
	shelfLifeDays_ = 0;
	shelfLifeHours_ = 0;
	shelfLifeMinutes_ = 0;
	productLifeDays_ = 0;
	productLifeHours_ = 0;
	productLifeMinutes_ = 0;

	label1_          = RECORD_NOT_FOUND;
	label2_          = RECORD_NOT_FOUND;
	label3_          = RECORD_NOT_FOUND;
	label4_          = RECORD_NOT_FOUND;

	graphicName1_.clear();
	graphicName2_.clear();
	graphicName3_.clear();
	graphicName4_.clear();
	coolText_.clear();
	coolTrackingText_.clear();

	oldBarcodeDigit_ = -1;
	oldUnitPrice_ = 0;
	oldFinalPrice_ = 0;
	oldTare_ = 0;
    oldTareB_ = 0;
	oldByCount_ = 0;
	oldFixedWeight_ = 0;
	oldShelfLifeDays_ = 0;
	oldShelfLifeHours_ = 0;
	oldShelfLifeMinutes_ = 0;
	oldProductLifeDays_ = 0;
	oldProductLifeHours_ = 0;
	oldProductLifeMinutes_ = 0;
	oldLabel1_ = RECORD_NOT_FOUND;
	oldLabel2_ = RECORD_NOT_FOUND;
	oldLabel3_ = RECORD_NOT_FOUND;
	oldLabel4_ = RECORD_NOT_FOUND;
	oldGraphicName1_.clear();
	oldGraphicName2_.clear();
	oldGraphicName3_.clear();
	oldGraphicName4_.clear();

	operatorId_.clear();
	operatorRFID_.clear();
	operatorName_.clear();
	operatorChangeType_ = 0;
	operatorAccessLevel_.clear();

	cutTestNumber_ = 0;
	percentOfPrimalWeight_ = 0;
	percentOfPrimalCost_ = 0;
    modeChange_ = 0;

    sweetheartWeight_ = 0;
    
	labelOutputFilename = "";	// Used for printing labels to Zebra printer.

	initFieldLists();
}


void Transaction::initFieldLists()
{
	if ( _dbIndependentFieldList_.empty() )
	{
		_dbIndependentFieldList_.push_back(fieldOffsetScaleId);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionId);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionGroup);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionType);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionCommittedDateTime);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionVoid);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionAffectsProduction);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionReadAndClear);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionPackagesNumber);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionPrintedSecurityLabel);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionCOOLFlag);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionTotalValue);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionOldTotalValue);

		_dbIndependentFieldList_.push_back(fieldOffsetTransactionDeptNum);
		_dbIndependentFieldList_.push_back(fieldOffsetProductNumber);
		_dbIndependentFieldList_.push_back(fieldOffsetProductReportingClass);
		_dbIndependentFieldList_.push_back(fieldOffsetProductType);
		_dbIndependentFieldList_.push_back(fieldOffsetProductionPlanningType);
		_dbIndependentFieldList_.push_back(fieldOffsetProductDescription);
		_dbIndependentFieldList_.push_back(fieldOffsetProductPriceModifier);
		_dbIndependentFieldList_.push_back(fieldOffsetProductBarcodePrefix);
		_dbIndependentFieldList_.push_back(fieldOffsetProductBarcodeNumber);
		_dbIndependentFieldList_.push_back(fieldOffsetProductUnitPrice);
		_dbIndependentFieldList_.push_back(fieldOffsetProductExceptionPrice);
		_dbIndependentFieldList_.push_back(fieldOffsetProductFinalPrice);
		_dbIndependentFieldList_.push_back(fieldOffsetProductTare);
		_dbIndependentFieldList_.push_back(fieldOffsetProductProportionalTare);
		_dbIndependentFieldList_.push_back(fieldOffsetProductFinalTare);
		_dbIndependentFieldList_.push_back(fieldOffsetProductByCount);
		_dbIndependentFieldList_.push_back(fieldOffsetProductFinalNetWeight);
		_dbIndependentFieldList_.push_back(fieldOffsetProductFixedWeight);
		_dbIndependentFieldList_.push_back(fieldOffsetProductShelfLifeDays);
		_dbIndependentFieldList_.push_back(fieldOffsetProductShelfLifeHours);
		_dbIndependentFieldList_.push_back(fieldOffsetProductShelfLifeMinutes);
		_dbIndependentFieldList_.push_back(fieldOffsetProductLifeDays);
		_dbIndependentFieldList_.push_back(fieldOffsetProductLifeHours);
		_dbIndependentFieldList_.push_back(fieldOffsetProductLifeMinutes);
		_dbIndependentFieldList_.push_back(fieldOffsetProductLabel1);
		_dbIndependentFieldList_.push_back(fieldOffsetProductLabel2);
		_dbIndependentFieldList_.push_back(fieldOffsetProductLabel3);
		_dbIndependentFieldList_.push_back(fieldOffsetProductLabel4);
		_dbIndependentFieldList_.push_back(fieldOffsetProductGraphicName1);
		_dbIndependentFieldList_.push_back(fieldOffsetProductGraphicName2);
		_dbIndependentFieldList_.push_back(fieldOffsetProductGraphicName3);
		_dbIndependentFieldList_.push_back(fieldOffsetProductGraphicName4);
		_dbIndependentFieldList_.push_back(fieldOffsetProductCoolTrackingText);
		_dbIndependentFieldList_.push_back(fieldOffsetProductCoolText);

		_dbIndependentFieldList_.push_back(fieldOffsetProductOldBarcodeDigit);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldUnitPrice);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldFinalPrice);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldTare);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldByCount);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldFixedWeight);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldShelfLifeDays);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldShelfLifeHours);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldShelfLifeMinutes);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldProductLifeDays);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldProductLifeHours);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldProductLifeMinutes);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldLabel1);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldLabel2);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldLabel3);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldLabel4);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldGraphicName1);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldGraphicName2);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldGraphicName3);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldGraphicName4);

		_dbIndependentFieldList_.push_back(fieldOffsetOperatorId);
		_dbIndependentFieldList_.push_back(fieldOffsetOperatorRFID);
		_dbIndependentFieldList_.push_back(fieldOffsetOperatorName);
		_dbIndependentFieldList_.push_back(fieldOffsetOperatorChangeType);
		_dbIndependentFieldList_.push_back(fieldOffsetOperatorAccessLevel);

		_dbIndependentFieldList_.push_back(fieldOffsetProductTareB);
		_dbIndependentFieldList_.push_back(fieldOffsetProductOldTareB);

		_dbIndependentFieldList_.push_back(fieldOffsetTransactionCutTestNumber);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionPercentOfPrimalWeight);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionPercentOfPrimalCost);
		_dbIndependentFieldList_.push_back(fieldOffsetTransactionModeChange);

		_dbIndependentFieldList_.push_back(fieldOffsetSweetheartWeight);

		TransactionTags transactionTags(&_dbIndependentFieldList_);
	}

	if ( _evolTaggedFieldList_.empty() )
	{
//			_evolTaggedFieldList_.push_back(fieldOffsetScaleId);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionId);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionGroup);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionCommittedDateTime);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionCommittedDate);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionCommittedTime);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionCommittedDayWeek);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionVoid);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionType);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionAffectsProduction);
//			_evolTaggedFieldList_.push_back(fieldOffsetTransactionReadAndClear);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionPackagesNumber);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionPrintedSecurityLabel);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionCOOLFlag);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionTotalValue);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionOldTotalValue);

		_evolTaggedFieldList_.push_back(fieldOffsetTransactionDeptNum);
		_evolTaggedFieldList_.push_back(fieldOffsetProductNumber);
		_evolTaggedFieldList_.push_back(fieldOffsetProductReportingClass);
		_evolTaggedFieldList_.push_back(fieldOffsetProductType);
		_evolTaggedFieldList_.push_back(fieldOffsetProductionPlanningType);
		_evolTaggedFieldList_.push_back(fieldOffsetProductDescription);
		_evolTaggedFieldList_.push_back(fieldOffsetProductPriceModifier);
		_evolTaggedFieldList_.push_back(fieldOffsetProductBarcodePrefix);
		_evolTaggedFieldList_.push_back(fieldOffsetProductBarcodeNumber);
		_evolTaggedFieldList_.push_back(fieldOffsetProductUnitPrice);
		_evolTaggedFieldList_.push_back(fieldOffsetProductExceptionPrice);
		_evolTaggedFieldList_.push_back(fieldOffsetProductFinalPrice);
		_evolTaggedFieldList_.push_back(fieldOffsetProductTare);
		_evolTaggedFieldList_.push_back(fieldOffsetProductProportionalTare);
		_evolTaggedFieldList_.push_back(fieldOffsetProductFinalTare);
		_evolTaggedFieldList_.push_back(fieldOffsetProductByCount);
		_evolTaggedFieldList_.push_back(fieldOffsetProductFinalNetWeight);
		_evolTaggedFieldList_.push_back(fieldOffsetProductFixedWeight);
		_evolTaggedFieldList_.push_back(fieldOffsetProductShelfLifeDays);
		_evolTaggedFieldList_.push_back(fieldOffsetProductShelfLifeHours);
		_evolTaggedFieldList_.push_back(fieldOffsetProductShelfLifeMinutes);
		_evolTaggedFieldList_.push_back(fieldOffsetProductLifeDays);
		_evolTaggedFieldList_.push_back(fieldOffsetProductLifeHours);
		_evolTaggedFieldList_.push_back(fieldOffsetProductLifeMinutes);
		_evolTaggedFieldList_.push_back(fieldOffsetProductLabel1);
		_evolTaggedFieldList_.push_back(fieldOffsetProductLabel2);
		_evolTaggedFieldList_.push_back(fieldOffsetProductLabel3);
		_evolTaggedFieldList_.push_back(fieldOffsetProductLabel4);
		_evolTaggedFieldList_.push_back(fieldOffsetProductGraphicName1);
		_evolTaggedFieldList_.push_back(fieldOffsetProductGraphicName2);
		_evolTaggedFieldList_.push_back(fieldOffsetProductGraphicName3);
		_evolTaggedFieldList_.push_back(fieldOffsetProductGraphicName4);
		_evolTaggedFieldList_.push_back(fieldOffsetProductCoolTrackingText);
		_evolTaggedFieldList_.push_back(fieldOffsetProductCoolText);
		_evolTaggedFieldList_.push_back(fieldOffsetProductPLUFlags);

		_evolTaggedFieldList_.push_back(fieldOffsetProductOldBarcodeDigit);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldUnitPrice);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldFinalPrice);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldTare);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldByCount);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldFixedWeight);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldShelfLifeDays);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldShelfLifeHours);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldShelfLifeMinutes);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldProductLifeDays);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldProductLifeHours);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldProductLifeMinutes);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldLabel1);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldLabel2);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldLabel3);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldLabel4);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldGraphicName1);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldGraphicName2);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldGraphicName3);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldGraphicName4);
		_evolTaggedFieldList_.push_back(fieldOffsetOperatorId);
		_evolTaggedFieldList_.push_back(fieldOffsetOperatorRFID);
		_evolTaggedFieldList_.push_back(fieldOffsetOperatorName);
		_evolTaggedFieldList_.push_back(fieldOffsetOperatorChangeType);
		_evolTaggedFieldList_.push_back(fieldOffsetOperatorAccessLevel);
		_evolTaggedFieldList_.push_back(fieldOffsetProductTareB);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldTareB);

		_evolTaggedFieldList_.push_back(fieldOffsetTransactionCutTestNumber);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionPercentOfPrimalWeight);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionPercentOfPrimalCost);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionModeChange);

		_evolTaggedFieldList_.push_back(fieldOffsetSweetheartWeight);

		TransactionTags transactionTags(&_evolTaggedFieldList_);
	}

	if ( _readCompatibilityFieldList_.empty() )
	{
		_readCompatibilityFieldList_.push_back(fieldOffsetTransactionDeptNum);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductNumber);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductByCount);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductDescription);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductFinalNetWeight);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductPLUFlags);
		_readCompatibilityFieldList_.push_back(fieldOffsetTransactionVoid);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductPriceModifier);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductReportingClass);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductShelfLifeDays);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductUnitPrice);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductBarcodePrefix);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductBarcodeNumber);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductProportionalTare);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductTare);
		_readCompatibilityFieldList_.push_back(fieldOffsetOperatorId);
		_readCompatibilityFieldList_.push_back(fieldOffsetOperatorName);
		_readCompatibilityFieldList_.push_back(fieldOffsetTransactionCommittedDate);
		_readCompatibilityFieldList_.push_back(fieldOffsetTransactionCommittedTime);
		_readCompatibilityFieldList_.push_back(fieldOffsetTransactionCommittedDayWeek);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductFinalPrice);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductLifeDays);
		_readCompatibilityFieldList_.push_back(fieldOffsetTransactionId);
		_readCompatibilityFieldList_.push_back(fieldOffsetTransactionPackagesNumber);
		_readCompatibilityFieldList_.push_back(fieldOffsetTransactionTotalValue);
		// "ud"
		// "is"
		// "mi"
		_readCompatibilityFieldList_.push_back(fieldOffsetProductShelfLifeHours);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductLifeHours);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductShelfLifeMinutes);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductLifeMinutes);
		_readCompatibilityFieldList_.push_back(fieldOffsetTransactionType);
		_readCompatibilityFieldList_.push_back(fieldOffsetScaleId);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductTareB);
		_readCompatibilityFieldList_.push_back(fieldOffsetProductOldTareB);
		_readCompatibilityFieldList_.push_back(fieldOffsetOperatorChangeType);
		// ov
		// nv

		/*  These, from the _evolTaggedFieldList_, weren't used for the _readCompatibilityFieldList_

		_evolTaggedFieldList_.push_back(fieldOffsetTransactionGroup);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionCommittedDateTime);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionAffectsProduction);
//			_evolTaggedFieldList_.push_back(fieldOffsetTransactionReadAndClear);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionPrintedSecurityLabel);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionCOOLFlag);
		_evolTaggedFieldList_.push_back(fieldOffsetTransactionOldTotalValue);

		_evolTaggedFieldList_.push_back(fieldOffsetProductType);
		_evolTaggedFieldList_.push_back(fieldOffsetProductionPlanningType);
		_evolTaggedFieldList_.push_back(fieldOffsetProductExceptionPrice);
		_evolTaggedFieldList_.push_back(fieldOffsetProductFinalTare);
		_evolTaggedFieldList_.push_back(fieldOffsetProductFixedWeight);
		_evolTaggedFieldList_.push_back(fieldOffsetProductLabel1);
		_evolTaggedFieldList_.push_back(fieldOffsetProductLabel2);
		_evolTaggedFieldList_.push_back(fieldOffsetProductLabel3);
		_evolTaggedFieldList_.push_back(fieldOffsetProductLabel4);
		_evolTaggedFieldList_.push_back(fieldOffsetProductGraphicName1);
		_evolTaggedFieldList_.push_back(fieldOffsetProductGraphicName2);
		_evolTaggedFieldList_.push_back(fieldOffsetProductGraphicName3);
		_evolTaggedFieldList_.push_back(fieldOffsetProductGraphicName4);
		_evolTaggedFieldList_.push_back(fieldOffsetProductCoolTrackingText);
		_evolTaggedFieldList_.push_back(fieldOffsetProductCoolText);

		_evolTaggedFieldList_.push_back(fieldOffsetProductOldBarcodeDigit);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldUnitPrice);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldFinalPrice);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldTare);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldByCount);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldFixedWeight);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldShelfLifeDays);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldShelfLifeHours);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldShelfLifeMinutes);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldProductLifeDays);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldProductLifeHours);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldProductLifeMinutes);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldLabel1);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldLabel2);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldLabel3);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldLabel4);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldGraphicName1);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldGraphicName2);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldGraphicName3);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldGraphicName4);
		_evolTaggedFieldList_.push_back(fieldOffsetOperatorRFID);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldShelfLifeMinutes);
		_evolTaggedFieldList_.push_back(fieldOffsetProductOldProductLifeMinutes);

		*/
		TransactionTags transactionTags(&_evolTaggedFieldList_);
	}
}

/* Add methods from the header file that are considered inline functions that nobody can step into or debug */
unsigned int Transaction::getOperatorChangeType() const
{
	return operatorChangeType_;
}

void Transaction::setOperatorChangeType(unsigned int change)
{
	operatorChangeType_ = change;
	debugSetter(fieldOffsetOperatorChangeType, operatorChangeType_);
}
			
bool Transaction::isOperatorChangeType(OperatorChange::OperatorChangeableField field) const
{
	unsigned int bit = 0x00000001 << (field - 1);
	return (operatorChangeType_ & bit) == bit;
}

void Transaction::addOperatorChangeType(OperatorChange::OperatorChangeableField field)
{
	operatorChangeType_ |= (0x00000001 << (field - 1));
	debugSetter(fieldOffsetOperatorChangeType, operatorChangeType_);
}

void Transaction::clearOperatorChangeType(OperatorChange::OperatorChangeableField field)
{
	operatorChangeType_ &= ~(0x00000001 << (field - 1));
}

void Transaction::setErrorCode(HobErrorCode errorCode)
{
	errorCode_ = errorCode;

	Debug::trace("Transaction", "setErrorCode():  errorCode = " + util_string::inttostring(errorCode_));
}


void Transaction::updateTransactionTotalValues()
{
	setTotalValue(finalPrice_ * numberPackages_);
	setOldTotalValue(oldFinalPrice_ * numberPackages_);
}

void Transaction::updateSupervisorChangesValues(bool origBCF, int bc, bool fbc, HobWeight tareA, HobWeight tareB, HobWeight fw, HobPrice price, int modeChange)
{
	// I only care about this subset, superset, gameset and match
	
	int currentBC = getByCount();
	bool isForcedNow = getByCountForced();
	if ((origBCF == true) && (isForcedNow == false) && (bc == 1))
	{
		setOldByCount(0);				// might not have to do this
		setByCountOldZero(true);        // might not have to do this
		if (getOperatorChangeType() == OperatorChange::OC_Unknown)
			setOperatorChangeType(4);	// because I am doing this
	}

	if( bc != currentBC)
		setByCount(bc);
	
	if ( fbc != isForcedNow )
		setByCountForced( fbc );

	if( tareA != getTare() )
		setTare(tareA);
	
	if( tareB != getTareB() )
		setTareB(tareB);
	
	if( fw != getFixedWeight() )
		setFixedWeight(fw);
	
	if( price != getUnitPrice() )
		setUnitPrice(price);

	// dont need to do this in Operate Mode because an OperateMode change is 0 (Supervisor Mode Change is 1)
	// and 0 is the default.  So in Operate I dont go out and do this save.
	if( modeChange != getModeChange() )
		setModeChange(modeChange);

	// These fields I just need to purposely set no matter what because we are in Supervisor Mode
	setTransactionTypeNumber(-1);
	setAffectsProductionFlag(TransactionType::AffectsProductionNone);
}

void Transaction::associateTransactionType(Util::CSmartPtr<TransactionType> transType)
{
	if ( transType.null() ) return;

	setTransactionTypeNumber(transType->getNumber());
	setTransactionTypeName(transType->getName1());
	setTransactionTypeTextForLabel(transType->getTextForLabel());
	setAffectsProductionFlag(transType->getAffectsProduction());
}


void Transaction::associateOperator(const HobOperator* oper)
{
	if ( oper )
	{
		setOperatorId(oper->getOpNum());
		setOperatorRFID(oper->getOpRFID());
		setOperatorName(oper->getOpName());
		setOperatorAccessLevel(util_string::inttostring(oper->getAccessLevel()));
	}
	else
	{
		setOperatorId("");
		setOperatorRFID("");
		setOperatorName("");
		setOperatorAccessLevel("");
	}
}


std::string Transaction::getOperatorChangeOldValue(OperatorChange::OperatorChangeableField field, int format)
{
	return serializeField(convertOperatorChangeableField(field, true), format);
}


std::string Transaction::getOperatorChangeNewValue(OperatorChange::OperatorChangeableField field, int format)
{
	return serializeField(convertOperatorChangeableField(field), format);
}


const char * Transaction::serialize(int format) const
{
	serializedData.clear();

	if ( getFormatType() != UnknownFormat )
		format = getFormatType();

	switch ( format )
	{
		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		{
			std::string fieldTag;
			TransactionTags transTags;
			FieldOffsetsList::iterator fieldIter = _evolTaggedFieldList_.begin();

			while ( fieldIter != _evolTaggedFieldList_.end() )
			{
				fieldTag = transTags.getTag(*fieldIter);
				if ( fieldTag.size() )
				{
					serializedData.append(fieldTag);
					serializedData.append(serializeField((FieldOffsets)*fieldIter, format));
					serializedData.append(UnitSeparator);
				}

				++fieldIter;
			}

			break;
		}
		case DBIndependentFormatFormat:
		{
			std::string fieldTag;
			TransactionTags transTags;
			FieldOffsetsList::iterator fieldIter = _dbIndependentFieldList_.begin();

			while ( fieldIter != _dbIndependentFieldList_.end() )
			{
				fieldTag = transTags.getTag(*fieldIter);
				if ( fieldTag.size() )
				{
					serializedData.append(fieldTag);
					serializedData.append(serializeField((FieldOffsets)*fieldIter, format));
					serializedData.append(UnitSeparator);
				}

				++fieldIter;
			}

			break;	
		}
		case ReadTransactionCompatibility:
		{
			std::string fieldTag;
			TransactionTags transTags;
			FieldOffsetsList::iterator fieldIter = _readCompatibilityFieldList_.begin();

			while ( fieldIter != _readCompatibilityFieldList_.end() )
			{
				fieldTag = transTags.getTag(*fieldIter);
				if ( fieldTag.size() )
				{
					serializedData.append(fieldTag);
					serializedData.append(serializeField((FieldOffsets)*fieldIter, format));
					serializedData.append(UnitSeparator);
				}

				++fieldIter;
			}

			break;
		}
		
		default:
			serializedData = "Transaction::serialize - unknown format:" + inttostring(format);
			break;
	}
	return serializedData.data();
}


const std::string& Transaction::serializeField(FieldOffsets offset, int format) const
{
	serializedField_.clear();

	switch ( offset )
	{
		case fieldOffsetScaleId:
			serializedField_ = util_string::longtostring(getScaleId());
			break;
		case fieldOffsetTransactionId:
			if (format == ReadTransactionCompatibility)
				serializedField_ = util_string::longtostring(getTransactionGroupNumber());
			else
				serializedField_ = util_string::longtostring(getTransactionId());
			break;
		case fieldOffsetTransactionGroup:
			if ( getTransactionGroupNumber() != RECORD_NOT_FOUND )
				serializedField_ = util_string::longtostring(getTransactionGroupNumber());
			break;
		case fieldOffsetTransactionCommittedDateTime:
		{
			HobDateTime hdt = getCommittedDateTime();
			serializedField_ =	hdt.convertToDBFormat( (format == EvolutionTaggedFormat || format == DBIndependentFormatFormat || format == SynchronizationTaggedFormat) ? FormatDateTime_Windux : FormatDateTime_HTSeries );
			break;
		}
		case fieldOffsetTransactionCommittedDate:
			serializedField_ = getCommittedDateTime().formatDate(FormatDate_MMDDYY_NoDelimiter);
			break;
		case fieldOffsetTransactionCommittedTime:
			serializedField_ = getCommittedDateTime().formatTime(FormatTime_HHMMSS_NoDelimiter, HourFormat_MILITARY);
			break;
		case fieldOffsetTransactionCommittedDayWeek:
			serializedField_ = util_string::inttostring(getCommittedDateTime().getDayOfWeek() + 1);
			break;
		case fieldOffsetTransactionVoid:
			serializedField_ = util_string::inttostring(getVoid());
			break;
		case fieldOffsetTransactionType:
			if ( getTransactionTypeNumber() != RECORD_NOT_FOUND )
				serializedField_ = util_string::longtostring(getTransactionTypeNumber());
			break;
		case fieldOffsetTransactionAffectsProduction:
			serializedField_ = util_string::inttostring(getAffectsProductionFlag());
			break;
		case fieldOffsetTransactionReadAndClear:
			serializedField_ = util_string::inttostring(getReadClearFlag());
			break;
		case fieldOffsetTransactionPackagesNumber:
			serializedField_ = util_string::inttostring(getNumberPackages());
			break;
		case fieldOffsetTransactionPrintedSecurityLabel:
			serializedField_ = util_string::inttostring(isPrintedSecurityLabel());
			break;
		case fieldOffsetTransactionCOOLFlag:
			serializedField_ = util_string::inttostring(isCOOLTransaction());
			break;
		case fieldOffsetTransactionTotalValue:
			serializedField_ = util_string::longtostring(getTotalValue());
			break;
		case fieldOffsetTransactionOldTotalValue:
			serializedField_ = util_string::longtostring(getOldTotalValue());
			break;

		case fieldOffsetTransactionDeptNum:
			serializedField_ = util_string::longtostring(getDeptNum());
			break;
		case fieldOffsetProductNumber:
			serializedField_ = util_string::longtostring(getProductNumber());
			break;
		case fieldOffsetProductReportingClass:
			serializedField_ = util_string::longtostring(getReportingClass());
			break;
		case fieldOffsetProductType:
			serializedField_ = util_string::inttostring(getProductType());
			break;
		case fieldOffsetProductionPlanningType:
			serializedField_ = util_string::inttostring(getProductionPlanningType());
			break;
		case fieldOffsetProductDescription:
			return getProductDescription();
		case fieldOffsetProductPriceModifier:
		{
			if ( format == EvolutionTaggedFormat )
			{
				switch ( getPriceModifier() )
				{
					case 25:  serializedField_.assign("14"); break;
					case 50:  serializedField_.assign("12"); break;
					case 100: serializedField_.assign("1:"); break;
					default:
					{
						int pm = getPriceModifier();

						if ( 1 < pm && pm <= 9 )
							serializedField_ = util_string::inttostring(pm) + "1";
						else
							serializedField_.assign("11");

						break;
					}
				}
			}
			else
				serializedField_ = util_string::inttostring(getPriceModifier());

			break;
		}
		case fieldOffsetProductBarcodePrefix:
			serializedField_ = util_string::inttostring(getBarcodePrefix());
			break;
		case fieldOffsetProductBarcodeNumber:
			return getBarcodeNumber();
		case fieldOffsetProductBarcodeDigit:
			serializedField_ = getBarcodeNumber().substr(0, 1);
			break;
		case fieldOffsetProductUnitPrice:
			if ( format == BrowserFormat )
				serializedField_ = Configuration::getInstance()->formatHobPrice(getUnitPrice());
			else
				serializedField_ = util_string::longtostring(getUnitPrice());
			break;
		case fieldOffsetProductExceptionPrice:
			if ( format == BrowserFormat )
				serializedField_ = Configuration::getInstance()->formatHobPrice(getExceptionPrice());
			else
				serializedField_ = util_string::longtostring(getExceptionPrice());
			break;
		case fieldOffsetProductFinalPrice:
			if ( format == BrowserFormat )
				serializedField_ = Configuration::getInstance()->formatHobPrice(getFinalPrice());
			else
				serializedField_ = util_string::longtostring(getFinalPrice());
			break;
		case fieldOffsetProductTare:
			if ( format == BrowserFormat )
				Configuration::getInstance()->formatWeight(getTare(), serializedField_);
			else
				serializedField_ = util_string::longtostring(getTare());
			break;
		case fieldOffsetProductTareB:
			if ( format == BrowserFormat )
				Configuration::getInstance()->formatWeight(getTareB(), serializedField_);
			else
				serializedField_ = util_string::longtostring(getTareB());
			break;
        		case fieldOffsetProductProportionalTare:
			serializedField_ = util_string::inttostring(getProportionalTare());
			break;
		case fieldOffsetProductFinalTare:
			if ( format == BrowserFormat )
				Configuration::getInstance()->formatWeight(getFinalTare(), serializedField_);
			else
				serializedField_ = util_string::longtostring(getFinalTare());
			break;
		case fieldOffsetProductByCount:
			serializedField_ = util_string::inttostring(getByCount());
			break;
		case fieldOffsetProductFinalNetWeight:
			if ( format == BrowserFormat )
				Configuration::getInstance()->formatWeight(getFinalNetWeight(), serializedField_);
			else
				serializedField_ = util_string::longtostring(getFinalNetWeight());
			break;
		case fieldOffsetProductFixedWeight:
//				if ( format == BrowserFormat )
//					Configuration::getInstance()->formatWeight(getFixedWeight(), serializedField_);
//				else
				serializedField_ = util_string::longtostring(getFixedWeight());
			break;
		case fieldOffsetProductShelfLifeDays:
			serializedField_ = util_string::inttostring(getShelfLifeDays());
			break;
		case fieldOffsetProductShelfLifeHours:
			serializedField_ = util_string::inttostring(getShelfLifeHours());
			break;
		case fieldOffsetProductShelfLifeMinutes:
			serializedField_ = util_string::inttostring(getShelfLifeMinutes());
			break;
		case fieldOffsetProductLifeDays:
			serializedField_ = util_string::inttostring(getProductLifeDays());
			break;
		case fieldOffsetProductLifeHours:
			serializedField_ = util_string::inttostring(getProductLifeHours());
			break;
		case fieldOffsetProductLifeMinutes:
			serializedField_ = util_string::inttostring(getProductLifeMinutes());
			break;
		case fieldOffsetProductLabel1:
			if ( getLabel1() != RECORD_NOT_FOUND )
				serializedField_ = util_string::longtostring(getLabel1());
			break;
		case fieldOffsetProductLabel2:
			if ( getLabel2() != RECORD_NOT_FOUND )
				serializedField_ = util_string::longtostring(getLabel2());
			break;
		case fieldOffsetProductLabel3:
			if ( getLabel3() != RECORD_NOT_FOUND )
				serializedField_ = util_string::longtostring(getLabel3());
			break;
		case fieldOffsetProductLabel4:
			if ( getLabel4() != RECORD_NOT_FOUND )
				serializedField_ = util_string::longtostring(getLabel4());
			break;
		case fieldOffsetProductGraphicName1:
			return getGraphicName1();
		case fieldOffsetProductGraphicName2:
			return getGraphicName2();
		case fieldOffsetProductGraphicName3:
			return getGraphicName3();
		case fieldOffsetProductGraphicName4:
			return getGraphicName4();
		case fieldOffsetProductCoolTrackingText:
			return getCOOLTrackingText();
		case fieldOffsetProductCoolText:
			return getCOOLText();
		case fieldOffsetProductPLUFlags:
		{
			int flags = 0;

			if ( isPrintedSecurityLabel() )
				flags |= PRPrintSecurityLabel;
			if ( getProductType() == Product::PRODUCTTYPE_FIXEDWEIGHT )
				flags |= PRFixedWeight;
			if ( getProductType() == Product::PRODUCTTYPE_FLUIDOUNCE )
				flags |= PRFluidOunce;

			serializedField_ = util_string::inttostring(flags);

			break;
		}

		case fieldOffsetProductOldUnitPrice:
		{
			if ( isOperatorChangeType(OperatorChange::OC_UnitPrice) )
			{
				if ( format == BrowserFormat )
					serializedField_ = Configuration::getInstance()->formatHobPrice(getOldUnitPrice());
				else
					serializedField_ = util_string::longtostring(getOldUnitPrice());
			}

			break;
		}
		case fieldOffsetProductOldTare:
		{
			if ( isOperatorChangeType(OperatorChange::OC_Tare) )
			{
				if ( format == BrowserFormat )
					Configuration::getInstance()->formatWeight(getOldTare(), serializedField_);
				else
					serializedField_ = util_string::longtostring(getOldTare());
			}

			break;
		}

		case fieldOffsetProductOldTareB:
		{
			if ( isOperatorChangeType(OperatorChange::OC_TareB) )
			{
				if ( format == BrowserFormat )
					Configuration::getInstance()->formatWeight(getOldTareB(), serializedField_);
				else
					serializedField_ = util_string::longtostring(getOldTare());
			}

			break;
		}

		case fieldOffsetProductOldByCount:
			if ( isOperatorChangeType(OperatorChange::OC_ByCount) )
				serializedField_ = util_string::inttostring(getOldByCount());
			break;
		case fieldOffsetProductOldFixedWeight:
		{
			if ( isOperatorChangeType(OperatorChange::OC_FixedWeight) )
			{
//					if ( format == BrowserFormat )
//						Configuration::getInstance()->formatWeight(getOldFixedWeight(), serializedField_);
//					else
					serializedField_ = util_string::longtostring(getOldFixedWeight());
			}

			break;
		}
		case fieldOffsetProductOldBarcodeDigit:
			if ( isOperatorChangeType(OperatorChange::OC_UpcDigit) )
				serializedField_ = util_string::inttostring(getOldBarcodeDigit());
			break;
		case fieldOffsetProductOldFinalPrice:
		{
			if ( isOperatorChangeType(OperatorChange::OC_TotalPrice) )
			{
				if ( format == BrowserFormat )
					serializedField_ = Configuration::getInstance()->formatHobPrice(getOldFinalPrice());
				else
					serializedField_ = util_string::longtostring(getOldFinalPrice());
			}

			break;
		}
		case fieldOffsetProductOldShelfLifeDays:
			if ( isOperatorChangeType(OperatorChange::OC_ShelfLifeDays) )
				serializedField_ = util_string::inttostring(getOldShelfLifeDays());
			break;
		case fieldOffsetProductOldShelfLifeHours:
			if ( isOperatorChangeType(OperatorChange::OC_ShelfLifeHours) )
				serializedField_ = util_string::inttostring(getOldShelfLifeHours());
			break;
		case fieldOffsetProductOldShelfLifeMinutes:
			// SOF-4904 Add shelfLifeMinutes to product and transactions - not supporting operator changes at this time, yet, so set to 0
			//if (isOperatorChangeType(OperatorChange::OC_ShelfLifeMinutes))
			//	serializedField_ = util_string::inttostring(getOldShelfLifeMinutes());
			serializedField_ = "0";
			break;
		case fieldOffsetProductOldProductLifeDays:
			if ( isOperatorChangeType(OperatorChange::OC_ProductLifeDays) )
				serializedField_ = util_string::inttostring(getOldProductLifeDays());
			break;
		case fieldOffsetProductOldProductLifeHours:
			if ( isOperatorChangeType(OperatorChange::OC_ProductLifeHours) )
				serializedField_ = util_string::inttostring(getOldProductLifeHours());
			break;
		case fieldOffsetProductOldProductLifeMinutes:
			// SOF-4904 Add shelfLifeMinutes to product and transactions - not supporting oeprator changes at this time, yet, so set to 0
			//if (isOperatorChangeType(OperatorChange::OC_ProductLifeMinutes))
			//	serializedField_ = util_string::inttostring(getOldProductLifeMinutes());
			serializedField_ = "0";
			break;
		case fieldOffsetProductOldLabel1:
			if ( isOperatorChangeType(OperatorChange::OC_PrimaryLabel) )
				if ( getOldLabel1() != RECORD_NOT_FOUND )
					serializedField_ = util_string::longtostring(getOldLabel1());
			break;
		case fieldOffsetProductOldLabel2:
			if ( isOperatorChangeType(OperatorChange::OC_SecondLabel) )
				if ( getOldLabel2() != RECORD_NOT_FOUND )
					serializedField_ = util_string::longtostring(getOldLabel2());
			break;
		case fieldOffsetProductOldLabel3:
			if ( isOperatorChangeType(OperatorChange::OC_ThirdLabel) )
				if ( getOldLabel3() != RECORD_NOT_FOUND )
					serializedField_ = util_string::longtostring(getOldLabel3());
			break;
		case fieldOffsetProductOldLabel4:
			if ( getOldLabel4() != RECORD_NOT_FOUND )
				serializedField_ = util_string::longtostring(getOldLabel4());
			break;
		case fieldOffsetProductOldGraphicName1:
			if ( isOperatorChangeType(OperatorChange::OC_Graphic1) )
				return getOldGraphicName1();
			break;
		case fieldOffsetProductOldGraphicName2:
			if ( isOperatorChangeType(OperatorChange::OC_Graphic2) )
				return getOldGraphicName2();
			break;
		case fieldOffsetProductOldGraphicName3:
			if ( isOperatorChangeType(OperatorChange::OC_Graphic3) )
				return getOldGraphicName3();
			break;
		case fieldOffsetProductOldGraphicName4:
			if ( isOperatorChangeType(OperatorChange::OC_Graphic4) )
				return getOldGraphicName4();
			break;

		case fieldOffsetOperatorId:
			return getOperatorId();
		case fieldOffsetOperatorRFID:
			return getOperatorRFID();
		case fieldOffsetOperatorName:
			return getOperatorName();
		case fieldOffsetOperatorChangeType:
			if ( getOperatorChangeType() )
				serializedField_ = util_string::uinttostring(getOperatorChangeType());
			break;

		case fieldOffsetTransactionCutTestNumber:
			serializedField_ = util_string::longtostring(getCutTestNumber());
			break;
		case fieldOffsetTransactionPercentOfPrimalWeight:
			serializedField_ = util_string::inttostring(getPercentOfPrimalWeight());
			break;
		case fieldOffsetTransactionPercentOfPrimalCost:
			serializedField_ = util_string::inttostring(getPercentOfPrimalCost());
			break;
		case fieldOffsetTransactionModeChange:
			serializedField_ = util_string::inttostring(getModeChange());
			break;

		case fieldOffsetSweetheartWeight:
			if (format == BrowserFormat)
				Configuration::getInstance()->formatWeight(getSweetheartWeight(), serializedField_);
			else
				serializedField_ = util_string::longtostring(getSweetheartWeight());
			break;

		default:
			break;
	}

	return serializedField_;
}


Transaction::FieldOffsets Transaction::convertOperatorChangeableField(OperatorChange::OperatorChangeableField field, bool oldValue)
{
	if ( oldValue )
	{
		switch ( field )
		{
			case OperatorChange::OC_UnitPrice:          return fieldOffsetProductOldUnitPrice;
			case OperatorChange::OC_Tare:               return fieldOffsetProductOldTare;
            case OperatorChange::OC_TareB:              return fieldOffsetProductOldTareB;
			case OperatorChange::OC_ByCount:            return fieldOffsetProductOldByCount;
			case OperatorChange::OC_ShelfLifeDays:      return fieldOffsetProductOldShelfLifeDays;
			case OperatorChange::OC_ProductLifeDays:    return fieldOffsetProductOldProductLifeDays;
			case OperatorChange::OC_PrimaryLabel:       return fieldOffsetProductOldLabel1;
			case OperatorChange::OC_SecondLabel:        return fieldOffsetProductOldLabel2;
			case OperatorChange::OC_FixedWeight:        return fieldOffsetProductOldFixedWeight;
			case OperatorChange::OC_UpcDigit:           return fieldOffsetProductOldBarcodeDigit;
			case OperatorChange::OC_TotalPrice:         return fieldOffsetProductOldFinalPrice;
			case OperatorChange::OC_Graphic1:           return fieldOffsetProductOldGraphicName1;
			case OperatorChange::OC_Graphic2:           return fieldOffsetProductOldGraphicName2;
			case OperatorChange::OC_Graphic3:           return fieldOffsetProductOldGraphicName3;
			case OperatorChange::OC_Graphic4:           return fieldOffsetProductOldGraphicName4;
			case OperatorChange::OC_ThirdLabel:         return fieldOffsetProductOldLabel3;
			case OperatorChange::OC_ShelfLifeHours:     return fieldOffsetProductOldShelfLifeHours;
			case OperatorChange::OC_ProductLifeHours:   return fieldOffsetProductOldProductLifeHours;
			// SOF-4904 Add shelfLifeMinutes to product and transactions - Not supporting Operator Changes right now
			//case OperatorChange::OC_ShelfLifeMinutes:     return fieldOffsetProductOldShelfLifeMinutes;
			//case OperatorChange::OC_ProductLifeMinutes:   return fieldOffsetProductOldProductLifeMinutes;
			default:                                    return endTransactionFieldOffsets;
		}
	}
	else
	{
		switch ( field )
		{
			case OperatorChange::OC_UnitPrice:          return fieldOffsetProductUnitPrice;
			case OperatorChange::OC_Tare:               return fieldOffsetProductTare;
            case OperatorChange::OC_TareB:              return fieldOffsetProductTareB;
			case OperatorChange::OC_ByCount:            return fieldOffsetProductByCount;
			case OperatorChange::OC_ShelfLifeDays:      return fieldOffsetProductShelfLifeDays;
			case OperatorChange::OC_ProductLifeDays:    return fieldOffsetProductLifeDays;
			case OperatorChange::OC_PrimaryLabel:       return fieldOffsetProductLabel1;
			case OperatorChange::OC_SecondLabel:        return fieldOffsetProductLabel2;
			case OperatorChange::OC_FixedWeight:        return fieldOffsetProductFixedWeight;
			case OperatorChange::OC_UpcDigit:           return fieldOffsetProductBarcodeDigit;
			case OperatorChange::OC_TotalPrice:         return fieldOffsetProductFinalPrice;
			case OperatorChange::OC_Graphic1:           return fieldOffsetProductGraphicName1;
			case OperatorChange::OC_Graphic2:           return fieldOffsetProductGraphicName2;
			case OperatorChange::OC_Graphic3:           return fieldOffsetProductGraphicName3;
			case OperatorChange::OC_Graphic4:           return fieldOffsetProductGraphicName4;
			case OperatorChange::OC_ThirdLabel:         return fieldOffsetProductLabel3;
			case OperatorChange::OC_ShelfLifeHours:     return fieldOffsetProductShelfLifeHours;
			case OperatorChange::OC_ProductLifeHours:   return fieldOffsetProductLifeHours;
			// SOF-4904 Add shelfLifeMinutes to product and transactions - not supporting oeprator changes at this time, yet
			//case OperatorChange::OC_ShelfLifeMinutes:   return fieldOffsetProductShelfLifeMinutes;
			//case OperatorChange::OC_ProductLifeMinutes: return fieldOffsetProductLifeMinutes;
			default:                                    return endTransactionFieldOffsets;
		}
	}
}


const std::string& Transaction::getVoidAsString() const
{
	switch ( void_ )
	{
		case VoidNotVoided:    return _VoidNotVoidedName_;
		case VoidVoided:       return _VoidVoidedName_;
		case VoidVoiding:      return _VoidVoidingName_;
		default:               return _VoidInvalidName_;
	}
}


const std::string& Transaction::getAffectsProductionFlagAsString() const
{
	switch ( affectsProduction_ )
	{
		case TransactionType::AffectsProductionNegatively:     return _ProductionNegativelyName_;
		case TransactionType::AffectsProductionNone:           return _ProductionNoImpactName_;
		case TransactionType::AffectsProductionPositively:     return _ProductionPositivelyName_;
		default:                                               return _ProductionInvalidName_;
	}
}


const std::string& Transaction::getReadClearFlagAsString() const
{
	switch ( readClear_ )
	{
		case ReadClearNotRead:         return _ReadClearNotReadName_;
		case ReadClearRead:            return _ReadClearReadName_;
		case ReadClearCleared:         return _ReadClearClearedName_;
		default:                       return _ReadClearInvalidName_;
	}
}


std::string Transaction::getOperatorChangeTypeAsString() const
{
	if ( operatorChangeType_ )
	{
		bool addedField = false;
		std::string result;
		OperatorChange::OperatorChangeableField field = (OperatorChange::OperatorChangeableField)(OperatorChange::OC_Unknown + 1);

		while ( field < OperatorChange::end_OpChangeFieldType )
		{
			if ( isOperatorChangeType(field) )
			{

				if ( addedField )
					result.append(" & ");
				else
					addedField = true;

				switch ( field )
				{
					case OperatorChange::OC_UnitPrice:         result.append(_OperatorChangeUnitPriceName_); break;
					case OperatorChange::OC_Tare:              result.append(_OperatorChangeTareName_); break;
					case OperatorChange::OC_ByCount:           result.append(_OperatorChangeByCountName_); break;
					case OperatorChange::OC_ShelfLifeDays:     result.append(_OperatorChangeShelfLifeName_); break;
					case OperatorChange::OC_ProductLifeDays:   result.append(_OperatorChangeProductLifeName_); break;
					case OperatorChange::OC_PrimaryLabel:      result.append(_OperatorChangeLabelType1Name_); break;
					case OperatorChange::OC_SecondLabel:       result.append(_OperatorChangeLabelType2Name_); break;
					case OperatorChange::OC_FixedWeight:       result.append(_OperatorChangeFixedWeightName_); break;
					case OperatorChange::OC_UpcDigit:          result.append(_OperatorChangeUPCDigitName_); break;
					case OperatorChange::OC_TotalPrice:        result.append(_OperatorChangeFinalPriceName_); break;
					case OperatorChange::OC_Graphic1:          result.append(_OperatorChangeGraphic1Name_); break;
					case OperatorChange::OC_Graphic2:          result.append(_OperatorChangeGraphic2Name_); break;
					case OperatorChange::OC_Graphic3:          result.append(_OperatorChangeGraphic3Name_); break;
					case OperatorChange::OC_Graphic4:          result.append(_OperatorChangeGraphic4Name_); break;
					case OperatorChange::OC_ThirdLabel:        result.append(_OperatorChangeLabelType3Name_); break;
					case OperatorChange::OC_ShelfLifeHours:    result.append(_OperatorChangeShelfLifeHoursName_); break;
					case OperatorChange::OC_ProductLifeHours:  result.append(_OperatorChangeProductLifeHoursName_); break;
					case OperatorChange::OC_TareB:             result.append(_OperatorChangeTareBName_); break;
					// SOF-4904 Add shelfLifeMinutes to product and transactions - not supporting oeprator changes at this time, yet
					// case OperatorChange::OC_ShelfLifeMinutes:    result.append(_OperatorChangeShelfLifeMinutesName_); break;
					// case OperatorChange::OC_ProductLifeMinutes:  result.append(_OperatorChangeProductLifeMinutesName_); break;
					default: addedField = false;
				}
			}

			field = (OperatorChange::OperatorChangeableField)(field + 1);
		}

		return result;
	}
	else
		return _OperatorChangeTypeNoChange_;
}


const std::string& Transaction::getFieldOffsetAsString(FieldOffsets offset)
{
	switch ( offset )
	{
		case fieldOffsetScaleId:                             return _scaleIdName_;
		case fieldOffsetTransactionId:                       return _transactionIdName_;
		case fieldOffsetTransactionGroup:                    return _transactionGroupName_;
		case fieldOffsetTransactionType:                     return _transactionTypeName_;
		case fieldOffsetTransactionCommittedDateTime:        return _transactionCommittedDateTimeName_;
		case fieldOffsetTransactionVoid:                     return _transactionVoidName_;
		case fieldOffsetTransactionAffectsProduction:        return _transactionAffectsProductionName_;
		case fieldOffsetTransactionReadAndClear:             return _transactionReadAndClearName_;
		case fieldOffsetTransactionPackagesNumber:           return _transactionPackagesNumberName_;
		case fieldOffsetTransactionPrintedSecurityLabel:     return _transactionPrintedSecurityLabelName_;
		case fieldOffsetTransactionCOOLFlag:                 return _transactionCOOLFlagName_;
		case fieldOffsetTransactionTotalValue:               return _transactionTotalValueName_;
		case fieldOffsetTransactionOldTotalValue:            return _transactionOldTotalValueName_;
		case fieldOffsetTransactionDeptNum:                  return _deptNumName_;
		case fieldOffsetProductNumber:                       return _productNumberName_;
		case fieldOffsetProductReportingClass:               return _productReportingClassName_;
		case fieldOffsetProductType:                         return _productTypeName_;
		case fieldOffsetProductionPlanningType:              return _productionPlanningTypeName_;
		case fieldOffsetProductDescription:                  return _productDescriptionName_;
		case fieldOffsetProductPriceModifier:                return _productPriceModifierName_;
		case fieldOffsetProductBarcodePrefix:                return _productBarcodePrefixName_;
		case fieldOffsetProductBarcodeNumber:                return _productBarcodeNumberName_;
		case fieldOffsetProductUnitPrice:                    return _productUnitPriceName_;
		case fieldOffsetProductExceptionPrice:               return _productExceptionPriceName_;
		case fieldOffsetProductFinalPrice:                   return _productFinalPriceName_;
		case fieldOffsetProductTare:                         return _productTareName_;
		case fieldOffsetProductTareB:                        return _productTareBName_;
		case fieldOffsetProductProportionalTare:             return _productProportionalTareName_;
		case fieldOffsetProductFinalTare:                    return _productFinalTareName_;
		case fieldOffsetProductByCount:                      return _productByCountName_;
		case fieldOffsetProductFinalNetWeight:               return _productFinalNetWeightName_;
		case fieldOffsetProductFixedWeight:                  return _productFixedWeightName_;
		case fieldOffsetProductShelfLifeDays:                return _productShelfLifeDaysName_;
		case fieldOffsetProductShelfLifeHours:               return _productShelfLifeHoursName_;
		case fieldOffsetProductShelfLifeMinutes:             return _productShelfLifeMinutesName_;
		case fieldOffsetProductLifeDays:                     return _productLifeDaysName_;
		case fieldOffsetProductLifeHours:                    return _productLifeHoursName_;
		case fieldOffsetProductLifeMinutes:                  return _productLifeMinutesName_;
		case fieldOffsetProductLabel1:                       return _productLabel1Name_;
		case fieldOffsetProductLabel2:                       return _productLabel2Name_;
		case fieldOffsetProductLabel3:                       return _productLabel3Name_;
		case fieldOffsetProductLabel4:                       return _productLabel4Name_;
		case fieldOffsetProductGraphicName1:                 return _productGraphicName1Name_;
		case fieldOffsetProductGraphicName2:                 return _productGraphicName2Name_;
		case fieldOffsetProductGraphicName3:                 return _productGraphicName3Name_;
		case fieldOffsetProductGraphicName4:                 return _productGraphicName4Name_;
		case fieldOffsetProductCoolText:                     return _productCoolTextName_;
		case fieldOffsetProductCoolTrackingText:             return _productCoolTrackingTextName_;
		case fieldOffsetProductOldBarcodeDigit:              return _productOldBarcodeDigitName_;
		case fieldOffsetProductOldUnitPrice:                 return _productOldUnitPriceName_;
		case fieldOffsetProductOldFinalPrice:                return _productOldFinalPriceName_;
		case fieldOffsetProductOldTare:                      return _productOldTareName_;
		case fieldOffsetProductOldTareB:                     return _productOldTareBName_;
		case fieldOffsetProductOldByCount:                   return _productOldByCountName_;
		case fieldOffsetProductOldFixedWeight:               return _productOldFixedWeightName_;
		case fieldOffsetProductOldShelfLifeDays:             return _productOldShelfLifeDaysName_;
		case fieldOffsetProductOldShelfLifeHours:            return _productOldShelfLifeHoursName_;
		case fieldOffsetProductOldShelfLifeMinutes:          return _productOldShelfLifeMinutesName_;
		case fieldOffsetProductOldProductLifeDays:           return _productOldProductLifeDaysName_;
		case fieldOffsetProductOldProductLifeHours:          return _productOldProductLifeHoursName_;
		case fieldOffsetProductOldProductLifeMinutes:        return _productOldProductLifeMinutesName_;
		case fieldOffsetProductOldLabel1:                    return _productOldLabel1Name_;
		case fieldOffsetProductOldLabel2:                    return _productOldLabel2Name_;
		case fieldOffsetProductOldLabel3:                    return _productOldLabel3Name_;
		case fieldOffsetProductOldLabel4:                    return _productOldLabel4Name_;
		case fieldOffsetProductOldGraphicName1:              return _productOldGraphicName1Name_;
		case fieldOffsetProductOldGraphicName2:              return _productOldGraphicName2Name_;
		case fieldOffsetProductOldGraphicName3:              return _productOldGraphicName3Name_;
		case fieldOffsetProductOldGraphicName4:              return _productOldGraphicName4Name_;
		case fieldOffsetOperatorId:                          return _operatorIdName_;
		case fieldOffsetOperatorName:                        return _operatorNameName_;
		case fieldOffsetOperatorChangeType:                  return _operatorChangeTypeName_;
        case fieldOffsetTransactionModeChange:               return _modeChangeName_;
		case fieldOffsetSweetheartWeight:                    return _sweetheartWeightName_;
		default:                                             return _unknownFieldOffsetName_;
	}
}


void Transaction::show() const
{
	if ( Debug::isTracing("Transaction") )
	{
		Debug::trace("Transaction", "------------------------------------------------------");
		Debug::trace("Transaction", "Transaction Attributes for (scaleId, transId) = (" +
			util_string::longtostring(scaleId_) + ", " + util_string::longtostring(transactionId_) + ")");
		Debug::trace("Transaction", "Group Number: " + util_string::inttostring(groupNumber_));
		Debug::trace("Transaction", "Type Number: " + util_string::longtostring(transactionTypeNumber_));
		Debug::trace("Transaction", "Committed: " + (isTransactionCommitted() ? getCommittedDateTimeAsString() : "not committed"));
		Debug::trace("Transaction", "Void Flag: " + getVoidAsString());
		Debug::trace("Transaction", "Affects Production Flag: " + getAffectsProductionFlagAsString());
		Debug::trace("Transaction", "Read&Clear Flag: " + getReadClearFlagAsString());
		Debug::trace("Transaction", "Number of Packages: " + util_string::inttostring(numberPackages_));
		Debug::trace("Transaction", "Printed security label: " + util_string::booltostring(printedSecurityLabel_));
		Debug::trace("Transaction", "COOL Required: " + util_string::booltostring(coolTransaction_));
		Debug::trace("Transaction", "Total Value: " + util_string::longtostring(totalValue_));
		Debug::trace("Transaction", "Old Total Value: " + util_string::longtostring(oldTotalValue_));

		if ( productNumber_ != RECORD_NOT_FOUND )
		{
			Debug::trace("Transaction", "Dept Num: " + util_string::longtostring(deptNum_));
			Debug::trace("Transaction", "Product Number: " + util_string::longtostring(productNumber_));
			Debug::trace("Transaction", "Reporting Class Number: " + util_string::longtostring(reportingClass_));
			Debug::trace("Transaction", "Product Type: " + Product::getProductTypeAsString(productType_));
			Debug::trace("Transaction", "Production Planning Type: " + Product::getProductionPlanningTypeAsString(productionPlanningType_));
			Debug::trace("Transaction", "Product Description: " + productDescription_);
			Debug::trace("Transaction", "Price Modifier: " + util_string::inttostring(getPriceModifier()));
			Debug::trace("Transaction", "Barcode Prefix: " + util_string::inttostring(getBarcodePrefix()));
			Debug::trace("Transaction", "Barcode Number: " + barcodeNumber_);
			Debug::trace("Transaction", "Unit Price: " + util_string::longtostring(unitPrice_));
			Debug::trace("Transaction", "Exception Price: " + util_string::longtostring(exceptionPrice_));
			Debug::trace("Transaction", "Final Price: " + util_string::longtostring(finalPrice_));
			Debug::trace("Transaction", "Tare: " + util_string::longtostring(tare_));
			Debug::trace("Transaction", "TareB: " + util_string::longtostring(tareB_));
			Debug::trace("Transaction", "Proportional Tare: " + util_string::inttostring(proportionalTare_));
			Debug::trace("Transaction", "Final Tare: " + util_string::longtostring(finalTare_));
			Debug::trace("Transaction", "By Count: " + util_string::inttostring(byCount_));
			Debug::trace("Transaction", "By Count Forced: " + util_string::booltostring(byCountForced_));
			Debug::trace("Transaction", "By Count Old Set 0: " + util_string::booltostring(byCountOldZero_));
			Debug::trace("Transaction", "Final Net Weight: " + util_string::longtostring(finalNetWeight_));
			Debug::trace("Transaction", "Fixed Weight: " + util_string::longtostring(fixedWeight_));
			Debug::trace("Transaction", "Shelf Life Days: " + util_string::inttostring(shelfLifeDays_));
			Debug::trace("Transaction", "Shelf Life Hours: " + util_string::inttostring(shelfLifeHours_));
			Debug::trace("Transaction", "Shelf Life Minutes: " + util_string::inttostring(shelfLifeMinutes_));
			Debug::trace("Transaction", "Product Life Days: " + util_string::inttostring(productLifeDays_));
			Debug::trace("Transaction", "Product Life Hours: " + util_string::inttostring(productLifeHours_));
			Debug::trace("Transaction", "Product Life Minutes: " + util_string::inttostring(productLifeMinutes_));
			
			Debug::trace("Transaction", "Label 1: " + util_string::longtostring(label1_));
			Debug::trace("Transaction", "Label 2: " + util_string::longtostring(label2_));
			Debug::trace("Transaction", "Label 3: " + util_string::longtostring(label3_));
			Debug::trace("Transaction", "Label 4: " + util_string::longtostring(label4_));

			Debug::trace("Transaction", "Graphic Name 1: " + graphicName1_);
			Debug::trace("Transaction", "Graphic Name 2: " + graphicName2_);
			Debug::trace("Transaction", "Graphic Name 3: " + graphicName3_);
			Debug::trace("Transaction", "Graphic Name 4: " + graphicName4_);
			Debug::trace("Transaction", "COOL Text: " + coolText_);
			Debug::trace("Transaction", "COOL Tracking Text: " + coolTrackingText_);

			Debug::trace("Transaction", "Old Barcode Digit: " + util_string::inttostring(oldBarcodeDigit_));
			Debug::trace("Transaction", "Old Unit Price: " + util_string::longtostring(oldUnitPrice_));
			Debug::trace("Transaction", "Old Final Price: " + util_string::longtostring(oldFinalPrice_));
			Debug::trace("Transaction", "Old Tare: " + util_string::longtostring(oldTare_));
			Debug::trace("Transaction", "Old Tare:B " + util_string::longtostring(oldTareB_));
			Debug::trace("Transaction", "Old By Count: " + util_string::inttostring(oldByCount_));
			Debug::trace("Transaction", "Old Fixed Weight: " + util_string::longtostring(oldFixedWeight_));
			Debug::trace("Transaction", "Old Shelf Life Days: " + util_string::inttostring(oldShelfLifeDays_));
			Debug::trace("Transaction", "Old Shelf Life Hours: " + util_string::inttostring(oldShelfLifeHours_));
			Debug::trace("Transaction", "Old Shelf Life Minutes: " + util_string::inttostring(oldShelfLifeMinutes_));
			Debug::trace("Transaction", "Old Product Life Days: " + util_string::inttostring(oldProductLifeDays_));
			Debug::trace("Transaction", "Old Product Life Hours: " + util_string::inttostring(oldProductLifeHours_));
			Debug::trace("Transaction", "Old Product Life Minutes: " + util_string::inttostring(oldProductLifeMinutes_));
			Debug::trace("Transaction", "Old Label 1: " + util_string::longtostring(oldLabel1_));
			Debug::trace("Transaction", "Old Label 2: " + util_string::longtostring(oldLabel2_));
			Debug::trace("Transaction", "Old Label 3: " + util_string::longtostring(oldLabel3_));
			Debug::trace("Transaction", "Old Label 4: " + util_string::longtostring(oldLabel4_));
			Debug::trace("Transaction", "Old Graphic Name 1: " + oldGraphicName1_);
			Debug::trace("Transaction", "Old Graphic Name 2: " + oldGraphicName2_);
			Debug::trace("Transaction", "Old Graphic Name 3: " + oldGraphicName3_);
			Debug::trace("Transaction", "Old Graphic Name 4: " + oldGraphicName4_);

		}

		if ( operatorId_.size() )
		{
			Debug::trace("Transaction", "Operator Id: " + operatorId_);
			Debug::trace("Transaction", "Operator RFID: " + operatorRFID_);
			Debug::trace("Transaction", "Operator Name: " + operatorName_);
			Debug::trace("Transaction", "Operator Access Level: " + operatorAccessLevel_);
			Debug::trace("Transaction", "Operator Change Type: " + getOperatorChangeTypeAsString());
		}

		Debug::trace("Transaction", "------------------------------------------------------");
	}
}
}
