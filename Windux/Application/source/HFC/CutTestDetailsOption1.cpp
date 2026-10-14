/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#include "CutTestDetailsOption1.h"
#include "CuttingTest.h"
#include "HobList.h"
#include "HobDateTime.h"
#include "HobOperator.h"
#include "Configuration.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBCuttingTestsRM.h"
#include "DBCuttingTest.h"
#include "DBTransactionsRM.h"
#include "DBTransaction.h"

#include "MultiLanguageText.h"
#include "filesys/file.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"

#include <iostream>
#include <fstream>

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"


int CutTestDetailsOption1::dataType = 0;
std::string CutTestDetailsOption1::dataTypeName("CutTestDetailsOption1"); 
#endif



CutTestDetailsOption1::CutTestDetailsOption1()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

// private
void CutTestDetailsOption1::init()
{
	cutTestRec_ = 0;

	cutTestNumber_ = "";

	totalWaste_            = 0;
	totalWeightOfCutItems_ = 0;
	totalPriceOfCutItems_  = 0;

	totalPrimalWeight_ = 0;
	shrinkYield_       = 0;
	productYield_      = 0;
		
	pluNumList      = new HobList();
	descriptionList = new HobList();
	packagesList    = new HobList();
	netWeightList   = new HobList();
	unitPriceList   = new HobList();
	totalPriceList  = new HobList();
	yieldList       = new HobList();
	
	if (descriptionList)
		descriptionList->setQuotingElements(true);
}

CutTestDetailsOption1::~CutTestDetailsOption1()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (cutTestRec_)
		delete cutTestRec_;
	if (pluNumList)
		delete pluNumList;
	if (descriptionList)
		delete descriptionList;
	if (packagesList)
		delete packagesList;
	if (netWeightList)
		delete netWeightList;
	if (unitPriceList)
		delete unitPriceList;
	if (totalPriceList)
		delete totalPriceList;
	if (yieldList)
		delete yieldList;
}


const char *CutTestDetailsOption1::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			if ( pluNumList && descriptionList && packagesList && netWeightList && unitPriceList && totalPriceList && yieldList )
			{
				HobString formattedValue;

				serializedData.append(cutTestNumber_);
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(productNum_);
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(vendorNum_);
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(operatorID_);
				serializedData.append(BrowserFormatSeparator);

				serializedData.append("111");		// QString& sellWeight() {return _Mybase::operator [] (6);}
				serializedData.append(BrowserFormatSeparator);

				long shrinkWeight = totalPrimalWeight_ - totalWeightOfCutItems_;
				Configuration::getInstance()->formatWeight( shrinkWeight, formattedValue, true);
 				serializedData.append(formattedValue);		// QString& shrinkWeight() {return _Mybase::operator [] (7);}
				serializedData.append(BrowserFormatSeparator);

				serializedData.append(primalWeight_);
				serializedData.append(BrowserFormatSeparator);

				serializedData.append("333");		// QString& sellYield() {return _Mybase::operator [] (9);}
				serializedData.append(BrowserFormatSeparator);

				Configuration::getInstance()->formatDecimalValue(shrinkYield_, formattedValue, 2, true);
				serializedData.append(formattedValue);	// QString& shrinkYield() {return _Mybase::operator [] (10);}
				serializedData.append(BrowserFormatSeparator);

				Configuration::getInstance()->formatDecimalValue(productYield_, formattedValue, 2, true);
				serializedData.append(formattedValue);	// QString& itemYield() {return _Mybase::operator [] (11);}
				serializedData.append(BrowserFormatSeparator);

				serializedData.append("666");		// QString& avgPrice() {return _Mybase::operator [] (12);}
				serializedData.append(BrowserFormatSeparator);
				Configuration::getInstance()->formatPrice( totalPriceOfCutItems_, formattedValue, true );
				serializedData.append(formattedValue);	// QString& sellPrice() {return _Mybase::operator [] (13);}
				serializedData.append(BrowserFormatSeparator);

				serializedData.append(primalCost_);
				serializedData.append(BrowserFormatSeparator);

				serializedData.append("888");		//     QString& imu() {return _Mybase::operator [] (15);}
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(startDate_);		//     QString& startDate() {return _Mybase::operator [] (16);}
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(endDate_);		//     QString& endDate() {return _Mybase::operator [] (17);}
				serializedData.append(BrowserFormatSeparator);

				serializedData.append(pluNumList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(descriptionList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(netWeightList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(yieldList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(unitPriceList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(totalPriceList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(lotNumber_);
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(packingDate_);
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(operatorName_);
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(inttostring(status_));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(packagesList->serialize(format));

			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}

// If operator number is passed in, then limit the listed cut tests to just that operator
void CutTestDetailsOption1::createDetailsOption1(DBAccess *dbAccess, const HobString &cutTestNum)
{
	bool traceOn = Debug::isTracing("CutTestDetailsOption1");
	HobString myDebug("createDetailsOption1():  ");

	if (!dbAccess)
		return;

	DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
	if (!dbCuttingTestsRM)
		return;

	cutTestRec_ = CuttingTest::findCuttingTest(stringtolong(cutTestNum), dbAccess);
	if (!cutTestRec_)
		return;

	HobString formattedValue;
	
	cutTestNumber_ = cutTestNum;
	if( cutTestNumber_.length() <= 0 )
		return;

	productNum_ = cutTestRec_->getProductNumber();
	vendorNum_ = cutTestRec_->getVendorNumber();

	Configuration::getInstance()->formatPrice(cutTestRec_->getPrimalCost(), formattedValue, true);
	primalCost_ = formattedValue;  

	Configuration::getInstance()->formatWeight(cutTestRec_->getPrimalWeight(), formattedValue, true);
	primalWeight_ = formattedValue;
	totalPrimalWeight_ = cutTestRec_->getPrimalWeight();

	lotNumber_ = cutTestRec_->getLotNumber();

	comments_      = cutTestRec_->getComments();
	finalComments_ = cutTestRec_->getFinalComments();

	// You will get the next two from the cut test record, only when the cut test is completed, not when it is suspended.
	totalPriceOfCutItems_  = cutTestRec_->getTotalPriceOfCutProducts();
	totalWeightOfCutItems_ = cutTestRec_->getTotalWeightOfCutProducts();
	shrinkYield_ = cutTestRec_->getPercentTotalShrinkage();

	HobDateTime dt = cutTestRec_->getPackingDate();
	packingDate_ = dt.formatDateTime();

	dt = cutTestRec_->getStartDate();
	startDate_ = dt.formatDateTime();

	dt = cutTestRec_->getEndDate();
	endDate_ = dt.formatDateTime();

	status_ = cutTestRec_->getStatus();

	operatorID_ = cutTestRec_->getOperatorId();
	HobOperator *ctOperator = 0;
	HobRecordNum ctOpNum = Long::valueOf(cutTestRec_->getOperatorId());
	if ( ctOpNum != 0 )
		ctOperator = HobOperator::find(longtostring(ctOpNum), dbAccess);
	if (ctOperator)
	{
		operatorName_ = ctOperator->getOpName();
		delete ctOperator;
		ctOperator = 0;
	}
	else
		operatorName_ = Long::toString(ctOpNum);

	if ( totalPrimalWeight_ > 0 && totalWeightOfCutItems_ > 0 )
		totalWaste_ = totalPrimalWeight_ - totalWeightOfCutItems_;


	try
	{
		DBTransactionsRM* dbTransactionsRM = dbAccess->getDBTransactionsRM();
		if ( dbTransactionsRM )
		{
			DBQuery<DBTransaction> *cutTestTotalsQuery = dbTransactionsRM->retrieveCutTestTransactionsForCutTest(cutTestNum);

			// If couldn't get the query from which to access the CuttingTests, just assume there aren't any transaction records for this cut test.
			if (cutTestTotalsQuery)
			{
				DBQuery<DBTransaction>::const_iterator *transIter;

				DBQuery<DBTransaction>::const_iterator tempIter = cutTestTotalsQuery->begin();
				while ( tempIter != cutTestTotalsQuery->end() )
				{
					transIter = new DBQuery<DBTransaction>::const_iterator(tempIter);
					if (transIter)
					{
						DBTransaction *trans = new DBTransaction(*(*transIter));
						if (trans)
						{
							pluNumList->push_back( (trans->isProductNumberNull()) ? "0" : trans->getProductNumber());  
							descriptionList->push_back( (trans->isProductDescriptionNull()) ? " " : trans->getProductDescription());  

							if ( !trans->isTransactionPackagesNull() )
								packagesList->push_back( (trans->getTransactionPackages() == 0) ? "1" : inttostring(trans->getTransactionPackages()));
							else
								packagesList->push_back( " " );

							if ( !trans->isFinalNetWeightNull() )
							{
								Configuration::getInstance()->formatWeight( trans->getFinalNetWeight(), formattedValue, true);
								netWeightList->push_back( formattedValue );
							}
							else
								netWeightList->push_back( " " );

							if ( !trans->isUnitPriceNull() )
							{
								Configuration::getInstance()->formatPrice( trans->getUnitPrice(), formattedValue, true );
								unitPriceList->push_back( formattedValue );
							}
							else
								unitPriceList->push_back( " " );

							if ( !trans->isFinalPriceNull() )
							{
								Configuration::getInstance()->formatPrice( trans->getFinalPrice(), formattedValue, true );
								totalPriceList->push_back( formattedValue );
							}
							else
								totalPriceList->push_back( " " );

							if ( cutTestRec_->getPrimalWeight() > 0 )
							{
								long yield = (long) ((double)(trans->getFinalNetWeight()) / cutTestRec_->getPrimalWeight() * 10000.0);
								Configuration::getInstance()->formatDecimalValue(yield, formattedValue, 2, true);
								yieldList->push_back( formattedValue );
							}
							else
								yieldList->push_back( " " );

							delete trans;
						}
						delete transIter;
					}
					++tempIter;
				}
				delete cutTestTotalsQuery;
				cutTestTotalsQuery = 0;
			}
		}
	}

	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "CutTestDetailsOption1::createDetailsOption1() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}

	if ( totalPrimalWeight_ > 0 )
		productYield_ = (long) ((double)(totalWeightOfCutItems_ + totalWaste_) / totalPrimalWeight_ * 10000.0);

}


void CutTestDetailsOption1::exportToFile(std::string outputFilename, LanguageSelection whichLang)
{
	Configuration *cfg = Configuration::getInstance();
	if (!cfg)
		return;

	hfc::MultiLanguageText internationalize;

	string x="";
	HobString formattedValue;
    const string weightUnits( MetSig::isScaleMetric() ? " kg" : " lb" );
    const string currSymbol( cfg->getPrimaryMonetarySymbol() );
	CfgCurrency::MonetaryRule placement = cfg->getMonetaryRule();

	// Note:  We will append to the file, in case this method is used to export multiple Cut Tests
	//File output(outputFilename);
	std::ofstream output(outputFilename.c_str(), std::ios::app);

	string cutTestHeader = internationalize("cutTestExportHeader1", whichLang);
	output << cutTestHeader << endl;

	string cutTestOutput;
	cutTestOutput += cutTestNumber_ + ",";
	cutTestOutput += productNum_ + ",";
	cutTestOutput += vendorNum_ + ",";

	if ( placement == CfgCurrency::MonetaryRuleLeading ) cutTestOutput += currSymbol;
	cutTestOutput += primalCost_;
	if ( placement == CfgCurrency::MonetaryRuleTrailing ) cutTestOutput += currSymbol;
	cutTestOutput += ",";

	cutTestOutput += primalWeight_ + weightUnits + ",";
	cutTestOutput += lotNumber_ + ",";

	CuttingTest::CutTestStatus status = cutTestRec_->getStatus();
	string cutTestStatus = ( status == CuttingTest::Status_Initial )   ? internationalize("cutTestStatusInitial", whichLang) : 
	                       ( status == CuttingTest::Status_InProcess ) ? internationalize("cutTestStatusInProcess", whichLang) : 
	                       ( status == CuttingTest::Status_Completed ) ? internationalize("cutTestStatusCompleted", whichLang) : 
	                       ( status == CuttingTest::Status_Suspended ) ? internationalize("cutTestStatusSuspended", whichLang) : 
	                       ( status == CuttingTest::Status_Deleted )   ? internationalize("cutTestStatusDeleted", whichLang) : internationalize("cutTestStatusUnknown", whichLang);
	cutTestOutput += cutTestStatus + ",";
	cutTestOutput += operatorName_ + ",";
	cutTestOutput += packingDate_ + ",";
	cutTestOutput += startDate_ + ",";
	cutTestOutput += endDate_ + ",";

	Configuration::getInstance()->formatWeight( totalWaste_, formattedValue, true);
	cutTestOutput += formattedValue + weightUnits + ",";

	Configuration::getInstance()->formatWeight( totalWeightOfCutItems_, formattedValue, true);
	cutTestOutput += formattedValue + weightUnits + ",";

	Configuration::getInstance()->formatPrice( totalPriceOfCutItems_, formattedValue, true );
	if ( placement == CfgCurrency::MonetaryRuleLeading ) cutTestOutput += currSymbol;
	cutTestOutput += formattedValue;
	if ( placement == CfgCurrency::MonetaryRuleTrailing ) cutTestOutput += currSymbol;
	cutTestOutput += ",";

	Configuration::getInstance()->formatDecimalValue(shrinkYield_, formattedValue, 2, true);
	cutTestOutput += formattedValue + "%,";

	Configuration::getInstance()->formatDecimalValue(productYield_, formattedValue, 2, true);
	cutTestOutput += formattedValue + "%";

	output << cutTestOutput << endl;


	string transHeader = internationalize("cutTestExportHeader2", whichLang);
	output << transHeader << endl;  

	if ( pluNumList->size() )
	{
		HobString pluNum, desc, pkgs, wgt, unit, total, yield;
		int gotEntry = pluNumList->first(pluNum);
		gotEntry = descriptionList->first(desc);
		gotEntry = packagesList->first(pkgs);
		gotEntry = netWeightList->first(wgt);
		gotEntry = unitPriceList->first(unit);
		gotEntry = totalPriceList->first(total);
		gotEntry = yieldList->first(yield);
		while ( 0 == gotEntry )
		{
			cutTestOutput = pluNum + ",";
			cutTestOutput += "'" + desc + "',";
			cutTestOutput += pkgs + ",";
			cutTestOutput += wgt + weightUnits + ",";

			if ( placement == CfgCurrency::MonetaryRuleLeading ) cutTestOutput += currSymbol;
			cutTestOutput += unit + ",";
			if ( placement == CfgCurrency::MonetaryRuleTrailing ) cutTestOutput += currSymbol;

			if ( placement == CfgCurrency::MonetaryRuleLeading ) cutTestOutput += currSymbol;
			cutTestOutput += total + ",";
			if ( placement == CfgCurrency::MonetaryRuleTrailing ) cutTestOutput += currSymbol;

			cutTestOutput += yield + "%";

			output << cutTestOutput << endl;

			gotEntry = pluNumList->next(pluNum);
			gotEntry = descriptionList->next(desc);
			gotEntry = packagesList->next(pkgs);
			gotEntry = netWeightList->next(wgt);
			gotEntry = unitPriceList->next(unit);
			gotEntry = totalPriceList->next(total);
			gotEntry = yieldList->next(yield);
		}
	}

	output.close();
}
