/* HT-Series Scale Project
 * COPYRIGHT (c) 2016.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#include "CutTestHistoryListing.h"
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

#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"


int CutTestHistoryListing::dataType = 0;
std::string CutTestHistoryListing::dataTypeName("CutTestHistoryListing"); 
#endif



CutTestHistoryListing::CutTestHistoryListing()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

// private
void CutTestHistoryListing::init()
{
	cutTestNumList   = new HobList();
	productNumList   = new HobList();
	operatorNameList = new HobList();
	statusList       = new HobList();
	vendorNumList    = new HobList();
	primalCostList   = new HobList();
	primalWeightList = new HobList();
	lotNumList       = new HobList();
	packingDateList  = new HobList();
	
	if (operatorNameList)
		operatorNameList->setQuotingElements(true);
	if (vendorNumList)
		vendorNumList->setQuotingElements(true);
	if (lotNumList)
		lotNumList->setQuotingElements(true);
}

CutTestHistoryListing::~CutTestHistoryListing()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (cutTestNumList)
		delete cutTestNumList;
	if (productNumList)
		delete productNumList;
	if (operatorNameList)
		delete operatorNameList;
	if (statusList)
		delete statusList;
	if (vendorNumList)
		delete vendorNumList;
	if (primalCostList)
		delete primalCostList;
	if (primalWeightList)
		delete primalWeightList;
	if (lotNumList)
		delete lotNumList;
	if (packingDateList)
		delete packingDateList;
}


const char *CutTestHistoryListing::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			if ( cutTestNumList && productNumList && operatorNameList && statusList && 
			     vendorNumList && primalCostList && primalWeightList && lotNumList && packingDateList )
			{
				serializedData.append(cutTestNumList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(productNumList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(operatorNameList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(statusList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(vendorNumList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(primalCostList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(primalWeightList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(lotNumList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(packingDateList->serialize(format));
			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}

// If operator number is passed in, then limit the listed cut tests to just that operator
void CutTestHistoryListing::createHistoryListing(DBAccess *dbAccess, const HobString &operNum)
{
	bool traceOn = Debug::isTracing("CutTestHistoryListing");
	HobString myDebug("createHistoryListing():  ");

	if (!dbAccess)
		return;

	HobOperator *theOperator = 0;
	if (CutTestOption_Option1 == Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled))
	{
		if (!operNum.empty())
		{
			theOperator = HobOperator::find(operNum, dbAccess);
			if (!theOperator)
			    return;		// If an operator number is passed in, it is assumed that the operator exists in the database.  Don't go further if not.
		}
	}

	DBCuttingTestsRM *dbCuttingTestsRM = dbAccess->getDBCuttingTestsRM();
	if (!dbCuttingTestsRM)
		return;

	try
	{
		DBQuery<DBCuttingTest> *allCutTestsQuery = dbCuttingTestsRM->retrieveAll();
		if (allCutTestsQuery)
		{
			DBQuery<DBCuttingTest>::const_iterator *allCutTestsIter;

			DBQuery<DBCuttingTest>::const_iterator tempIter = allCutTestsQuery->begin();
			while ( tempIter != allCutTestsQuery->end() )
			{
				allCutTestsIter = new DBQuery<DBCuttingTest>::const_iterator(tempIter);
				if (allCutTestsIter)
				{
					DBCuttingTest *dbCutTest = new DBCuttingTest(*(*allCutTestsIter));
					if (dbCutTest)
					{
						HobString dbOperNum = dbCutTest->getOperatorNumber();
						// Per requirements, Operators with access level 5 or greater than 5 can view and print all cut tests, not just their own
						if (operNum.empty() || (theOperator && theOperator->getAccessLevel() >= 5) || dbOperNum.compare(operNum) == 0
					                        || (CutTestOption_Classic == Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled)) )
						{
							CuttingTest::CutTestStatus status = (CuttingTest::CutTestStatus)(dbCutTest->getStatus());

							if ( ( status != CuttingTest::Status_Initial && status != CuttingTest::Status_InProcess ) 
								          || (CutTestOption_Classic == Configuration::getInstance()->toIntCfg(fieldOffsetCfgCutTestOptionEnabled)) )
							{
								if (traceOn)
								{
									HobString myDebug2 = myDebug;
									myDebug2.append("  CutTest: CutTest num is ").append(dbCutTest->getTestNumber()).append("  oper is ").append(dbCutTest->getOperatorNumber());
									Debug::trace("CutTestHistoryListing", myDebug2);
								}

								cutTestNumList->push_back( (dbCutTest->isTestNumberNull()) ? " " : dbCutTest->getTestNumber());  
								productNumList->push_back( (dbCutTest->isProductNumberNull()) ? " " : dbCutTest->getProductNumber());  
								statusList->push_back( (dbCutTest->isStatusNull()) ? " " : Integer::toString(dbCutTest->getStatus()));      
								vendorNumList->push_back( (dbCutTest->isVendorNumberNull()) ? " " : dbCutTest->getVendorNumber());
								lotNumList->push_back( (dbCutTest->isLotNumberNull()) ? " " : dbCutTest->getLotNumber());  

								HobString weightDisplay;
								Configuration::getInstance()->formatWeight(dbCutTest->getPrimalWeight(), weightDisplay, true);
								primalWeightList->push_back(weightDisplay);

								HobString costDisplay;
								Configuration::getInstance()->formatPrice(dbCutTest->getPrimalCost(), costDisplay, true);
								primalCostList->push_back(costDisplay);  

								HobDateTime dt;
								HobString dts( dbCutTest->getPackingDate() );
								dt.convertFromDBFormat(&dts);
								packingDateList->push_back( dts ); 

								HobOperator *ctOperator = 0;
								HobRecordNum ctOpNum = Long::valueOf(dbCutTest->getOperatorNumber());
								if ( ctOpNum != 0 )
								{
									ctOperator = HobOperator::find(longtostring(ctOpNum), dbAccess);
								}
								operatorNameList->push_back( (ctOperator) ? ctOperator->getOpName() : HobString(Long::toString(ctOpNum)) );
								if (ctOperator)
								{
									delete ctOperator;
									ctOperator = 0;
								}
							}
						}
						delete dbCutTest;
					}
					delete allCutTestsIter;
				}
				++tempIter;
			}
			delete allCutTestsQuery;
			allCutTestsQuery = 0;
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "CutTestHistoryListing::createHistoryListing() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
}
