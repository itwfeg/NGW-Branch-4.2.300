#include "ProductRunTotalsReport.h"
#include "Session.h"
#include "ReportManager.h"
#include "ProductManager.h"
#include "HfcStartReport.h"
#include "ProductTotalManager.h"
#include "Product.h"
#include "ProductRunTotal.h"
#include "Debug.h"
#include "Configuration.h"
#include "CutTestManager.h"
#include "CutTestDetailsOption1.h"

#include "util/time/date.h"
#include "util/time/dbtime.h"
#include "util/e_string/utilstring.h"


using namespace std;


const string ProductRunTotalsReport::_contentFontFamily_("Gravilex");
const int ProductRunTotalsReport::_contentFontSize_         = 7;

ProductRunTotalsReport::ProductRunTotalsReport(ReportManager* reportManager,
	   PrinterType type,
	   Session* session,
	   int cutTestNumber,
	   int cutTestReportType)
		: AbstractReport(reportManager, type), name_("Run Totals Report"),
		  session_(session), product_(0), productRunTotal_(0), cutTestManager_(0), ctdo1_(0), cutTestCode_(cutTestNumber), cutTestReportType_(cutTestReportType)
		  
{
	Debug::trace("ProductRunTotalsReport","Constructing the ProductRunTotalsReport for printer, type: " + inttostring(type));

	Debug::trace("ProductRunTotalsReport", "doInitialize() cutTestCode_: " + inttostring(cutTestCode_));
	Debug::trace("ProductRunTotalsReport", "doInitialize() cutTestReportType: " + inttostring(cutTestReportType_));
}


ProductRunTotalsReport::~ProductRunTotalsReport()
{
	Debug::trace("ProductRunTotalsReport", "Destructor");
}


void ProductRunTotalsReport::doInitialize()
{
	Debug::trace("ProductRunTotalsReport","doInitialize()");

	if (session_)
	{
		ProductManager *myProductMgr = session_->getProductManager();

		if (myProductMgr)
		{
			if ( Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled) == CutTestOption_Option1 )
			{
				Debug::trace("ProductRunTotalsReport", "doInitialize() cutTestCode_: " + inttostring(cutTestCode_));
				Debug::trace("ProductRunTotalsReport", "doInitialize() cutTestReportType: " + inttostring(cutTestReportType_));

				if( cutTestCode_ > 0 && cutTestReportType_ > 0)
				{
					Debug::trace("ProductRunTotalsReport", "doInitialize() get myProductMgr->cutTestManager()");
					cutTestManager_ = myProductMgr->getCutTestManager();

					if( cutTestManager_ )
					{
						Debug::trace("ProductRunTotalsReport", "doInitialize() call readCuttingTestDetailsForReport() ");
						ctdo1_ = cutTestManager_->readCuttingTestDetailsForReport( cutTestCode_ );
						
						// now that we have ctdo1 we can print the report
						if( ctdo1_ )
						{
							Debug::trace("ProductRunTotalsReport", "doInitialize() we have a ctdo1 so print a report");
							composeOption1Report();
						}
						else
						{
                            Debug::trace("ProductRunTotalsReport", "doInitialize() we dont have a ctdo1 so we cant print a report ");
							composeErrorReport(1);
						}
					}
					else
					{
						Debug::trace("ProductRunTotalsReport", "doInitialize() didnt have a cutTestManager so do nothing ");
						composeErrorReport(2);
					}
				}
				else
				{
					Debug::trace("ProductRunTotalsReport", "Had Option 1 but cutTestCode_ <= 0 && cutTestReportType_ <= 0 so do run totals");
					// original code but I dont think I do this. I think I need to default back to the run totals report here
					//productRunTotal_ = session_->getProductTotalManager()->getCurrentProductRunTotal();
					//composeErrorReport(3);
					// new code
					product_ = myProductMgr->getCurrentProduct();
					productRunTotal_ = session_->getProductTotalManager()->getCurrentProductRunTotal();
					composeReport();
				}
			}
			else
			{
				Debug::trace("ProductRunTotalsReport", "Had Default Option so do a Default Report on Run Totals");
				product_ = myProductMgr->getCurrentProduct();
				productRunTotal_ = session_->getProductTotalManager()->getCurrentProductRunTotal();
				composeReport();
			}
		}
	}
}


void ProductRunTotalsReport::composeReport()
{
	Debug::trace("ProductRunTotalsReport","composeReport()");

	// Date and Time at top of Report
	string dateTimeContent;
	util_time::Date dnow;
	util_time::Time tnow;

	// SOF-2809: Remove comma from year
	std::string dateStr = dnow.standardLook();
	dateStr.erase(std::remove(dateStr.begin(), dateStr.end(), ','), dateStr.end());

	dateTimeContent.assign("Date:").append(2, '\t').append(dateStr).append(1, '\n');
	dateTimeContent.append("Time:").append(2, '\t').append(tnow.standardLook()).append(2, '\n');
	setContent(dateTimeContent);

	// Run Totals section is the next one
	string runTotalsContent;
	serializeRunTotals(runTotalsContent);
	appendContent(runTotalsContent);

	// Plu Information is next
	string pluInformationContent;
	serializePluInformation(pluInformationContent);
	appendContent(pluInformationContent);

	// Report is now complete
	setContentReady();
}

void ProductRunTotalsReport::composeOption1Report()
{
	Debug::trace("ProductRunTotalsReport","composeOption1Report()");
	Debug::trace("ProductRunTotalsReport", "composeOption1Report cutTestCode_: " + inttostring(cutTestCode_) );

	if( cutTestCode_ > 0)
		Debug::trace("ProductRunTotalsReport", "composeOption1Report cutTestCode_ is> 0 " );
	else
		Debug::trace("ProductRunTotalsReport", "composeOption1Report cutTestCode_ is < than 0 and it should not be 0." );

	switch ( cutTestReportType_ )
	{
		case fieldOffsetCutTestDetails:
		{
			// Cutting Test Details Report is now complete
			string detailsContent;
			serializeOption1Details(detailsContent);
			appendContent(detailsContent);
			Debug::trace("ProductRunTotalsReport", "Finished serializeOption1Details" );
			
			// Report is now complete
			setContentReady();

			break;
		}
		case fieldOffsetCutTestInitialContents:
		{
			// Initial Comments Report is now complete
			string commentsContent;
			serializeOption1Comments(commentsContent);
			appendContent(commentsContent);
			Debug::trace("ProductRunTotalsReport", "Finished serializeOption1Comments" );
			
			// Initial Comments Report is now complete
			setContentReady();
			break;
		}
		case fieldOffsetCutTestFinalContents:
		{
			// Final Comments Report is now complete
			string finalCommentsContent;
			serializeOption1FinalComments(finalCommentsContent);
			appendContent(finalCommentsContent);
			Debug::trace("ProductRunTotalsReport", "Finished serializeOption1FinalComments" );

			// Final Comments Report is now complete
			setContentReady();

			break;
		}
		default:
			break;
	}
}

void ProductRunTotalsReport::composeErrorReport(int errorType)
{
	Debug::trace("ProductRunTotalsReport","composeErrorReport()");

	string errorContent;
	serializeErrorInformation(errorContent, errorType);
	appendContent(errorContent);

	// Report is now complete
	setContentReady();
}

void ProductRunTotalsReport::serializeRunTotals(std::string& runTotalsContent)
{
	Debug::trace("ProductRunTotalsReport","serializeRunTotals()");

	// Run Totals header
#if 1
	runTotalsContent.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B");
	runTotalsContent.append("Run Totals");
	runTotalsContent.append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');

	if ( !productRunTotal_ )		//	HA-192
	{
		runTotalsContent.append("Data not available.").append(2,'\n');
	}
	else
	{
		// Total Weight
		runTotalsContent.append("Weight:").append(2,'\t');
		long totalWeight = productRunTotal_->getWeight();
		HobString totalWeightString;
		Configuration::getInstance()->formatWeight(totalWeight,totalWeightString,true,DefaultPrecision);
		if (MetSig::isScaleMetric())
			totalWeightString.append(" kg");
		else
			totalWeightString.append(" lb");
		runTotalsContent.append(totalWeightString).append(1, '\n');

		// Total Price
		runTotalsContent.append("Price:").append(2,'\t');
		long totalPrice = productRunTotal_->getPrice();
		HobString monetarySymbol = Configuration::getInstance()->getMonetarySymbol();
		HobString totalPriceString;
		Configuration::getInstance()->formatPrice(totalPrice,totalPriceString,true,DefaultPrecision);
		runTotalsContent.append(monetarySymbol).append(totalPriceString).append(1, '\n');
		
		// Total Packages
		runTotalsContent.append("Packages:").append(1, '\t');
		runTotalsContent.append(uinttostring(productRunTotal_->getPackages())).append(2, '\n');
	}
#else
	runTotalsContent.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B").append("Run Totals").append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');

	if (productRunTotal_)
	{
		// Total Weight
		runTotalsContent.append("Weight:").append(2,'\t');
		long totalWeight = productRunTotal_->getWeight();
		HobString totalWeightString;
		Configuration::getInstance()->formatWeight(totalWeight,totalWeightString,true,DefaultPrecision);
		if (Configuration::getInstance()->isScaleMetric())
			totalWeightString.append(" kg");
		else
			totalWeightString.append(" lb");
		runTotalsContent.append(totalWeightString);
		runTotalsContent.append(1, '\n');

		// Total Price
		runTotalsContent.append("Price:").append(2, '\t');
		long totalPrice = productRunTotal_->getPrice();
		HobString monetarySymbol = Configuration::getInstance()->getMonetarySymbol();
		HobString totalPriceString;
		Configuration::getInstance()->formatPrice(totalPrice,totalPriceString,true,DefaultPrecision);
		runTotalsContent.append(monetarySymbol).append(totalPriceString);
		runTotalsContent.append(1, '\n');
		
		// Total Packages
		runTotalsContent.append("Packages:").append(1, '\t');
		runTotalsContent.append(uinttostring(productRunTotal_->getPackages()));
		runTotalsContent.append(2, '\n');
	}
#endif
}

void ProductRunTotalsReport::serializePluInformation(std::string& pluInformationContent)
{
	Debug::trace("ProductRunTotalsReport","serializePluInformation()");

	// PLU Information header
	pluInformationContent.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B");
	pluInformationContent.append("Product Information");
	pluInformationContent.append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');

	if ( !product_ )		//	HA-192
	{
		pluInformationContent.append("No active Product.").append(2,'\n');
	}
	else
	{
		// Description
		pluInformationContent.append(product_->getDescription()).append(2, '\n');
		
		// PLU Number
		pluInformationContent.append("PLU Number:");
		pluInformationContent.append(2, '\t');
		pluInformationContent.append(uinttostring(product_->getProductNum())).append(1, '\n');
		
		// Item Number
		pluInformationContent.append("Item Number:");
		pluInformationContent.append(1, '\t');

		BarCodeNumber barCodeNumber = product_->getBarCodeNum();
		HobString upcNumber = barCodeNumber.getUPCNumber();
		pluInformationContent.append(upcNumber).append(1, '\n');
	}
}

void ProductRunTotalsReport::serializeOption1Details(std::string& detailsContent)
{
	Debug::trace("ProductRunTotalsReport","serializeOption1Details()");

	detailsContent.append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');

	// Product Name ( Product Description )
	detailsContent.append("Product Name:").append(1, '\t');
	HobString plunum = ctdo1_->productNum_;
	HobString descriptionString = cutTestManager_->readCuttingTestDetailsDescription(plunum);
	detailsContent.append(descriptionString).append(1, '\n');

	// Cutting Test Number
	detailsContent.append("Cutting Test:").append(2,'\t');
	HobString cutTestNumString = ctdo1_->cutTestNumber_;
	detailsContent.append(cutTestNumString).append(1, '\n');

	// Supplier Number
	detailsContent.append("Supplier #:").append(3,'\t');
	HobString supplierNumberString = ctdo1_->vendorNum_;
	detailsContent.append(supplierNumberString).append(1, '\n');

	// Lot Number
	detailsContent.append("Lot #:").append(4,'\t');
	HobString lotNumberString = ctdo1_->lotNumber_;
	detailsContent.append(lotNumberString).append(1, '\n');

	// Product (Primal) Cost
	detailsContent.append("Product Cost:").append(2,'\t');
	HobString monetarySymbol = Configuration::getInstance()->getMonetarySymbol();
	detailsContent.append(monetarySymbol).append(ctdo1_->primalCost_).append(1, '\n');

	// Product (Primal) Number
	detailsContent.append("Product #:").append(3,'\t');
	detailsContent.append(plunum).append(1, '\n');

	// Product (Primal) Weight
	detailsContent.append("Product Weight:").append(2,'\t');
	HobString totalWeightString = ctdo1_->primalWeight_;
	if (MetSig::isScaleMetric())
		totalWeightString.append(" kg");
	else
		totalWeightString.append(" lb");
	detailsContent.append(totalWeightString).append(1, '\n');

	// Waste
	detailsContent.append("Waste:").append(4,'\t');
	long totalWaste = ctdo1_->totalWaste_;
	HobString totalWasteString;
	Configuration::getInstance()->formatWeight(totalWaste, totalWasteString, true);
	if (MetSig::isScaleMetric())
		totalWasteString.append(" kg");
	else
		totalWasteString.append(" lb");
	detailsContent.append(totalWasteString).append(1, '\n');

	// Packing Date
	detailsContent.append("Packing Date:").append(2,'\t');
	HobString packingString = ctdo1_->packingDate_;
	detailsContent.append(packingString).append(1, '\n');

	// Sale Amount
	detailsContent.append("Sale Amount:").append(2,'\t');
	long saleAmount = ctdo1_->totalPriceOfCutItems_;
	monetarySymbol = Configuration::getInstance()->getMonetarySymbol();
	HobString totalSaleAmountString;
	Configuration::getInstance()->formatPrice(saleAmount, totalSaleAmountString, true, DefaultPrecision);
	detailsContent.append(monetarySymbol).append(totalSaleAmountString).append(1, '\n');

	// Operator Name
	detailsContent.append("Operator Name:").append(2,'\t');
	HobString operatorString = ctdo1_->operatorName_;
	detailsContent.append(operatorString).append(1, '\n');
}

void ProductRunTotalsReport::serializeOption1Comments(std::string& commentsContent)
{
	Debug::trace("ProductRunTotalsReport","serializeOption1Comments()");

	commentsContent.append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');

	commentsContent.append("Operator Name:").append(2,'\t');
	HobString operatorString = ctdo1_->operatorName_;
	commentsContent.append(operatorString).append(1, '\n');

	commentsContent.append("Cutting Test:").append(2,'\t');
	HobString cutTestNumString = ctdo1_->cutTestNumber_;
	commentsContent.append(cutTestNumString).append(1, '\n');

	commentsContent.append("Initial Comments:").append(1,'\t');
	HobString initCommentsString = ctdo1_->comments_;
	commentsContent.append(initCommentsString).append(1, '\n');
}

void ProductRunTotalsReport::serializeOption1FinalComments(std::string& finalCommentsContent)
{
	Debug::trace("ProductRunTotalsReport","serializeOption1FinalComments()");

	finalCommentsContent.append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(2, '\n');

	finalCommentsContent.append("Operator Name:").append(2,'\t');
	HobString operatorString = ctdo1_->operatorName_;
	finalCommentsContent.append(operatorString).append(1, '\n');

	finalCommentsContent.append("Cutting Test:").append(2,'\t');
	HobString cutTestNumString = ctdo1_->cutTestNumber_;
	finalCommentsContent.append(cutTestNumString).append(1, '\n');

	finalCommentsContent.append("Final Comments:").append(1,'\n');
	HobString finalCommentsString = ctdo1_->finalComments_;
	finalCommentsContent.append(finalCommentsString).append(1, '\n');
}

void ProductRunTotalsReport::serializeErrorInformation(std::string& errorContent, int errorType)
{
	Debug::trace("ProductRunTotalsReport","serializeErrorInformation()");

	errorContent.assign("\t\t\x1B\x1B""fn=Gravilex;ps=9\x1B");
	errorContent.assign("Cut Test Details");
	errorContent.append("\x1B\x1B""fn=Gravilex;ps=7\x1B").append(1, '\n');

	if( errorType == 1 )
	{
		errorContent.append("Could not find Manager");
	}
	else if( errorType == 2 )
	{
		errorContent.append("Could not find details");
	}
	else if( errorType == 3 )
	{
		errorContent.append("Cut test out of range");
	}
	else
	{
		errorContent.append("Unknown Report Error.").append(2,'\n');
	}
}