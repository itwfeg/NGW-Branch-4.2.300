/* HT Series Scale Project
 * COPYRIGHT (c) 2015.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllBarcodeScanners.h"
#include "HobList.h"
#include "Configuration.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBBarcodeScannersRM.h"
#include "DBBarcodeScanner.h"

#include "Long.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllBarcodeScanners::dataType = 0;
std::string AllBarcodeScanners::dataTypeName("AllBarcodeScanners"); 
#endif

AllBarcodeScanners::AllBarcodeScanners()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

// private
void AllBarcodeScanners::init()
{
	productScannerCodeList        = new HobList();
	productScannerProductNumList  = new HobList();
	productScannerDescriptionList = new HobList();
}

AllBarcodeScanners::~AllBarcodeScanners()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (productScannerCodeList)
		delete productScannerCodeList;
	if (productScannerProductNumList)
		delete productScannerProductNumList;
	if (productScannerDescriptionList)
		delete productScannerDescriptionList;
}

HobList *AllBarcodeScanners::getProductScannerCodeList()
{
	return productScannerCodeList;
}

HobList *AllBarcodeScanners::getProductScannerProductNumList()
{
	return productScannerProductNumList;
}

HobList *AllBarcodeScanners::getProductScannerDescriptionList()
{
	return productScannerDescriptionList;
}

const char *AllBarcodeScanners::serialize(int format) const
{
	serializedData = "";
	switch (format)
	{
		case BrowserFormat:
			{
				serializedData.append(productScannerCodeList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(productScannerProductNumList->serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(productScannerDescriptionList->serialize(format));
			}
			break;

		default:
			break;
	}
	return serializedData.c_str();
}

void AllBarcodeScanners::findAllBarcodeScanners(DBAccess *dbAccess)
{
	bool traceOn = Debug::isTracing("AllBarcodeScanners");
	HobString myDebug("findAllBarcodeScanners():  ");

	if (!dbAccess)
		return;

	DBBarcodeScannersRM *myRM = dbAccess->getDBBarcodeScannersRM();
	if (!myRM)
		return;

	try
	{
		DBQuery<DBBarcodeScanner>* myQuery = myRM->retrieveAll();
		if (!myQuery)
			return;

		DBQuery<DBBarcodeScanner>::const_iterator *myIter;
		DBBarcodeScanner *dbRec = 0;

		DBQuery<DBBarcodeScanner>::const_iterator tempIter = myQuery->begin();
		while (tempIter != myQuery->end())
		{
			myIter = new DBQuery<DBBarcodeScanner>::const_iterator(tempIter);
			dbRec = new DBBarcodeScanner(*(*myIter));

			if (dbRec)
			{
				if (traceOn)
				{
					HobString myDebug2 = myDebug;
					myDebug2.append("  HfcScannerProductRead: scannerCode is ").append(dbRec->getScannerCode()).append("  scannerProductNum is ").append(dbRec->getScannerProductNum());
					Debug::trace("AllBarcodeScanners", myDebug2);
				}

				productScannerCodeList->push_back( dbRec->getScannerCode() );
				productScannerProductNumList->push_back( dbRec->getScannerProductNum() );
				
				/* RIGHT HERE GET THE DESCRIPTION */
				HobRecordNum productNum = Long::valueOf( dbRec->getScannerProductNum() );			// what is the PLU from what we just looked up

				// multidepttodo - is it ok to just get the plu for the currently active dept?
				Util::CSmartPtr<Product> theProduct(Product::findRecord(productNum, Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess));		// go and get the entire plu record with it.

				HobString currentDescription = "";

				if( theProduct.null() )
					currentDescription = "PLU number does not exist.";
				else
					currentDescription = theProduct->getDescription();
				
				currentDescription.stripFontSequences();
				productScannerDescriptionList->push_back( currentDescription );
				/***** END ADDING DESCRIPTION ****/
				
				delete dbRec;
			}

			if (myIter)
				delete myIter;

			++tempIter;
		}

		delete myQuery;
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "AllBarcodeScanners::findAllBarcodeScanners() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}
}
