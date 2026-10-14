#include "TransactionTags.h"


#include "Transaction.h"
#include "DeptTags.h"
#include "ProductTags.h"
#include "ClassTags.h"
#include "COOLTags.h"
#include "HobOperatorTags.h"
#include "CuttingTestTags.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace hfc
{


#ifdef USE_ACCOUNTANT
	int TransactionTags::dataType = 0;
	std::string TransactionTags::dataTypeName("hfc::TransactionTags"); 
#endif



	const std::string TransactionTags::_scaleId_                               = "sd";
	const std::string TransactionTags::_transactionId_                         = "x#";
	const std::string TransactionTags::_transactionGroup_                      = "xG";
	const std::string TransactionTags::_transactionType_                       = "tt";   // This should be moved out of here
	const std::string TransactionTags::_transactionCommittedDateTime_          = "xC";
	const std::string TransactionTags::_transactionCommittedDate_              = "d_";
	const std::string TransactionTags::_transactionCommittedTime_              = "t_";
	const std::string TransactionTags::_transactionCommittedDayWeek_           = "da";
	const std::string TransactionTags::_transactionVoid_                       = "ir";   //??
	const std::string TransactionTags::_transactionAffectsProduction_          = "xA";
	const std::string TransactionTags::_transactionReadAndClear_               = "xR";
	const std::string TransactionTags::_transactionPackagesNumber_             = "xi";
	const std::string TransactionTags::_transactionPrintedSecurityLabel_       = "xS";
	const std::string TransactionTags::_transactionCOOLFlag_                   = "xc";
	const std::string TransactionTags::_transactionTotalValue_                 = "x$";
	const std::string TransactionTags::_transactionOldTotalValue_              = "xO";

	const std::string TransactionTags::_productFinalPrice_                     = "t$";
	const std::string TransactionTags::_productFinalTare_                      = "tT";
	const std::string TransactionTags::_productFinalNetWeight_                 = "nw";
	const std::string TransactionTags::_productFixedWeight_                    = "fw";

	// SOF-4904 Add shelfLifeMinutes to product and transactions - implement like hours
	//const std::string TransactionTags::_productShelfLifeMinutes_               = "SM";
	//const std::string TransactionTags::_productLifeMinutes_                    = "PM";
	
	const std::string TransactionTags::_productLabel4_                         = "O4";   // This was l4, and this should be moved out of here
	const std::string TransactionTags::_productOldBarcodeDigit_                = "OG";
	const std::string TransactionTags::_productOldUnitPrice_                   = "OU";
	const std::string TransactionTags::_productOldFinalPrice_                  = "O$";
	const std::string TransactionTags::_productOldTare_                        = "OT";
	const std::string TransactionTags::_productOldByCount_                     = "OB";
	const std::string TransactionTags::_productOldFixedWeight_                 = "OW";
	const std::string TransactionTags::_productOldShelfLifeDays_               = "OD";
	const std::string TransactionTags::_productOldShelfLifeHours_              = "OH";		// I-O
	const std::string TransactionTags::_productOldShelfLifeMinutes_            = "OM";
	const std::string TransactionTags::_productOldProductLifeDays_             = "Od";
	const std::string TransactionTags::_productOldProductLifeHours_            = "Oh";		// no!
	const std::string TransactionTags::_productOldProductLifeMinutes_          = "Om";
	const std::string TransactionTags::_productOldLabel1_                      = "O1";
	const std::string TransactionTags::_productOldLabel2_                      = "O2";
	const std::string TransactionTags::_productOldLabel3_                      = "O3";
	const std::string TransactionTags::_productOldLabel4_                      = "O4";
	const std::string TransactionTags::_productOldGraphicName1_                = "o1";
	const std::string TransactionTags::_productOldGraphicName2_                = "o2";
	const std::string TransactionTags::_productOldGraphicName3_                = "o3";
	const std::string TransactionTags::_productOldGraphicName4_                = "o4";
	const std::string TransactionTags::_operatorChangeType_                    = "tc";
//	const std::string TransactionTags::_transactionUserDefined_                = "ud";
//	const std::string TransactionTags::_transactionItemSequence_               = "is";
//	const std::string TransactionTags::_transactionMaxItemSequence_            = "mi";
//	const std::string TransactionTags::_operatorChangeOldValue_                = "ov";
//	const std::string TransactionTags::_operatorChangeNewValue_                = "nv";

	const std::string TransactionTags::_cutTestPercentOfPrimalWeight_          = "WP";
	const std::string TransactionTags::_cutTestPercentOfPrimalCost_            = "Cp";
	const std::string TransactionTags::_modeChange_							   = "mC";

	const std::string TransactionTags::_sweetheartWeight_						= "Sw";

	bool     TransactionTags::_initialized_ = false;
	TagsList TransactionTags::_transactionTags_(Transaction::endTransactionFieldOffsets);
	TagsMap  TransactionTags::_fieldMap_;  //example pair: r#,fieldOffsetTextNum


	TransactionTags::TransactionTags(FieldOffsetsList * pFieldList)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif


		if ( !_initialized_ )
		{
			//initialize the the table of tags for the Transaction class
			_transactionTags_[0]                                                         = emptyTag;
			_transactionTags_[Transaction::fieldOffsetScaleId]                           = _scaleId_;
			_transactionTags_[Transaction::fieldOffsetTransactionId]                     = _transactionId_;
			_transactionTags_[Transaction::fieldOffsetTransactionGroup]                  = _transactionGroup_;
			_transactionTags_[Transaction::fieldOffsetTransactionType]                   = _transactionType_;
			_transactionTags_[Transaction::fieldOffsetTransactionCommittedDateTime]      = _transactionCommittedDateTime_;
			_transactionTags_[Transaction::fieldOffsetTransactionCommittedDate]          = _transactionCommittedDate_;
			_transactionTags_[Transaction::fieldOffsetTransactionCommittedTime]          = _transactionCommittedTime_;
			_transactionTags_[Transaction::fieldOffsetTransactionCommittedDayWeek]       = _transactionCommittedDayWeek_;
			_transactionTags_[Transaction::fieldOffsetTransactionVoid]                   = _transactionVoid_;
			_transactionTags_[Transaction::fieldOffsetTransactionAffectsProduction]      = _transactionAffectsProduction_;
			_transactionTags_[Transaction::fieldOffsetTransactionReadAndClear]           = _transactionReadAndClear_;
			_transactionTags_[Transaction::fieldOffsetTransactionPackagesNumber]         = _transactionPackagesNumber_;
			_transactionTags_[Transaction::fieldOffsetTransactionPrintedSecurityLabel]   = _transactionPrintedSecurityLabel_;
			_transactionTags_[Transaction::fieldOffsetTransactionCOOLFlag]               = _transactionCOOLFlag_;
			_transactionTags_[Transaction::fieldOffsetTransactionTotalValue]             = _transactionTotalValue_;
			_transactionTags_[Transaction::fieldOffsetTransactionOldTotalValue]          = _transactionOldTotalValue_;

			_transactionTags_[Transaction::fieldOffsetTransactionDeptNum]                = DeptTags::deptNum;
			_transactionTags_[Transaction::fieldOffsetProductNumber]                     = ProductTags::prodNumber;
			_transactionTags_[Transaction::fieldOffsetProductReportingClass]             = ClassTags::reportingClassNumber;
			_transactionTags_[Transaction::fieldOffsetProductType]                       = ProductTags::prodType;
			_transactionTags_[Transaction::fieldOffsetProductionPlanningType]            = ProductTags::productionPlanningType;
			_transactionTags_[Transaction::fieldOffsetProductDescription]                = ProductTags::prodDescription;
			_transactionTags_[Transaction::fieldOffsetProductPriceModifier]              = ProductTags::prodPriceModifier;
			_transactionTags_[Transaction::fieldOffsetProductBarcodePrefix]              = ProductTags::prodBarPrefix;
			_transactionTags_[Transaction::fieldOffsetProductBarcodeNumber]              = ProductTags::prodUpcNum;
			_transactionTags_[Transaction::fieldOffsetProductUnitPrice]                  = ProductTags::prodUnitPrice;
			_transactionTags_[Transaction::fieldOffsetProductExceptionPrice]             = ProductTags::prodExceptPrice;
			_transactionTags_[Transaction::fieldOffsetProductFinalPrice]                 = _productFinalPrice_;
			_transactionTags_[Transaction::fieldOffsetProductTare]                       = ProductTags::prodTareA;
			_transactionTags_[Transaction::fieldOffsetProductProportionalTare]           = ProductTags::prodProportionalTare;
			_transactionTags_[Transaction::fieldOffsetProductFinalTare]                  = _productFinalTare_;
			_transactionTags_[Transaction::fieldOffsetProductByCount]                    = ProductTags::prodByCount;
			_transactionTags_[Transaction::fieldOffsetProductFinalNetWeight]             = _productFinalNetWeight_;
			_transactionTags_[Transaction::fieldOffsetProductFixedWeight]                = _productFixedWeight_;
			_transactionTags_[Transaction::fieldOffsetProductShelfLifeDays]              = ProductTags::prodShelfLife;
			_transactionTags_[Transaction::fieldOffsetProductShelfLifeHours]             = ProductTags::prodShelfLifeHours;
			_transactionTags_[Transaction::fieldOffsetProductShelfLifeMinutes]           = ProductTags::prodShelfLifeMinutes;
			_transactionTags_[Transaction::fieldOffsetProductLifeDays]                   = ProductTags::prodProdLife;
			_transactionTags_[Transaction::fieldOffsetProductLifeHours]                  = ProductTags::prodProdLifeHours;
			_transactionTags_[Transaction::fieldOffsetProductLifeMinutes]                = ProductTags::prodProdLifeMinutes;
			_transactionTags_[Transaction::fieldOffsetProductLabel1]                     = ProductTags::prodLabelType1;
			_transactionTags_[Transaction::fieldOffsetProductLabel2]                     = ProductTags::prodLabelType2;
			_transactionTags_[Transaction::fieldOffsetProductLabel3]                     = ProductTags::prodLabelType3;
			_transactionTags_[Transaction::fieldOffsetProductLabel4]                     = _productLabel4_;
			_transactionTags_[Transaction::fieldOffsetProductGraphicName1]               = ProductTags::prodGraphicName1;
			_transactionTags_[Transaction::fieldOffsetProductGraphicName2]               = ProductTags::prodGraphicName2;
			_transactionTags_[Transaction::fieldOffsetProductGraphicName3]               = ProductTags::prodGraphicName3;
			_transactionTags_[Transaction::fieldOffsetProductGraphicName4]               = ProductTags::prodGraphicName4;
			_transactionTags_[Transaction::fieldOffsetProductCoolText]                   = COOLTags::coolTextText;
			_transactionTags_[Transaction::fieldOffsetProductCoolTrackingText]           = ProductTags::coolTrackingText;
			_transactionTags_[Transaction::fieldOffsetProductPLUFlags]                   = ProductTags::pluFlags;

			_transactionTags_[Transaction::fieldOffsetProductOldBarcodeDigit]            = _productOldBarcodeDigit_;
			_transactionTags_[Transaction::fieldOffsetProductOldUnitPrice]               = _productOldUnitPrice_;
			_transactionTags_[Transaction::fieldOffsetProductOldFinalPrice]              = _productOldFinalPrice_;
			_transactionTags_[Transaction::fieldOffsetProductOldTare]                    = _productOldTare_;
			_transactionTags_[Transaction::fieldOffsetProductOldByCount]                 = _productOldByCount_;
			_transactionTags_[Transaction::fieldOffsetProductOldFixedWeight]             = _productOldFixedWeight_;
			_transactionTags_[Transaction::fieldOffsetProductOldShelfLifeDays]           = _productOldShelfLifeDays_;
			_transactionTags_[Transaction::fieldOffsetProductOldShelfLifeHours]          = _productOldShelfLifeHours_;
			_transactionTags_[Transaction::fieldOffsetProductOldShelfLifeMinutes]        = _productOldShelfLifeMinutes_;
			_transactionTags_[Transaction::fieldOffsetProductOldProductLifeDays]         = _productOldProductLifeDays_;
			_transactionTags_[Transaction::fieldOffsetProductOldProductLifeHours]        = _productOldProductLifeHours_;
			_transactionTags_[Transaction::fieldOffsetProductOldProductLifeMinutes]      = _productOldProductLifeMinutes_;
			_transactionTags_[Transaction::fieldOffsetProductOldLabel1]                  = _productOldLabel1_;
			_transactionTags_[Transaction::fieldOffsetProductOldLabel2]                  = _productOldLabel2_;
			_transactionTags_[Transaction::fieldOffsetProductOldLabel3]                  = _productOldLabel3_;
			_transactionTags_[Transaction::fieldOffsetProductOldLabel4]                  = _productOldLabel4_;
			_transactionTags_[Transaction::fieldOffsetProductOldGraphicName1]            = _productOldGraphicName1_;
			_transactionTags_[Transaction::fieldOffsetProductOldGraphicName2]            = _productOldGraphicName2_;
			_transactionTags_[Transaction::fieldOffsetProductOldGraphicName3]            = _productOldGraphicName3_;
			_transactionTags_[Transaction::fieldOffsetProductOldGraphicName4]            = _productOldGraphicName4_;
			_transactionTags_[Transaction::fieldOffsetOperatorId]                        = HobOperatorTags::opNumber;
			_transactionTags_[Transaction::fieldOffsetOperatorRFID]                      = HobOperatorTags::opRFID;
			_transactionTags_[Transaction::fieldOffsetOperatorName]                      = HobOperatorTags::opName;
			_transactionTags_[Transaction::fieldOffsetOperatorChangeType]                = _operatorChangeType_;
			_transactionTags_[Transaction::fieldOffsetTransactionCutTestNumber]          = CuttingTestTags::cuttingTestNum;
			_transactionTags_[Transaction::fieldOffsetTransactionPercentOfPrimalWeight]  = _cutTestPercentOfPrimalWeight_;
			_transactionTags_[Transaction::fieldOffsetTransactionPercentOfPrimalCost]    = _cutTestPercentOfPrimalCost_;
			_transactionTags_[Transaction::fieldOffsetTransactionModeChange]             = _modeChange_;

			_transactionTags_[Transaction::fieldOffsetSweetheartWeight]                  = _sweetheartWeight_;
            _transactionTags_[Transaction::fieldOffsetProductShelfLifeMinutes]           = ProductTags::prodShelfLifeMinutes;
			_transactionTags_[Transaction::fieldOffsetProductLifeMinutes]                = ProductTags::prodProdLifeMinutes;

			_initialized_ = true;
		}


		if ( pFieldList )
			putTagsInMap(pFieldList);
	}


	TransactionTags::~TransactionTags()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}



}
