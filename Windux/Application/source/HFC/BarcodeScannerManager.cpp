/* @file BarcodeScannerManager.cpp
 * BarcodeScannerManager tracks all barcodes that have been scanned by the usb scanner.  Currently the
 * Scanner is intended to be attached to an elevated self service device. The customer will have
 * the ability to walk up to a elevated self service scale with their produce item and hold it under
 * the scanner and the barcode sticker attached to the item will be used to look up a PLU which the
 * customer can then print a label for purchase.
 *
 * @copyright Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 */

#include "BarcodeScannerManager.h"
#include "BarcodeScannerEvent.h"
#include "SessionMessage.h"
#include "Session.h"
#include "Configuration.h"
#include "HfcResponse.h"
#include "ResponseMessageManager.h"
#include "HfcError.h"
#include "HfcUpdateStatus.h"
#include "AllBarcodeScanners.h"
#include "GenericHfcObject.h"

#include "SessionManager.h"
#include "CSmartPtr.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBBarcodeScannersRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "HFCManagerContainer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int BarcodeScannerManager::_dataType_ = 0;
std::string BarcodeScannerManager::_dataTypeName_("BarcodeScannerManager"); 
#endif

BarcodeScannerManager::BarcodeScannerManager()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}

BarcodeScannerManager::BarcodeScannerManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

	init();
	dbAccess = container;
	mgrContainer_ = container;
}

BarcodeScannerManager::~BarcodeScannerManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	destroyQueriesAndIters();
}

void BarcodeScannerManager::init()
{
	mySession = 0;

	currentBarcodeScanner = 0;

	allBarcodeScannersQuery = 0;
	allBarcodeScannersIter  = 0;

	receivedBarcodeScannerRestore = false;
}

DBSession *BarcodeScannerManager::getMySession()
{
	return mySession;
}
void BarcodeScannerManager::setMySession(DBSession *session)
{
	mySession = session;
}

/* If the first record was not read, this method will return NULL.
 */
HfcScannerProductRead *BarcodeScannerManager::firstBarcodeScanner()
{
	if (!dbAccess)
		return (HfcScannerProductRead *)NULL;

	DBBarcodeScannersRM *myRM = dbAccess->getDBBarcodeScannersRM();
	if (!myRM)
		return (HfcScannerProductRead *)NULL;

	// If the user calls firstBarcodeScanner() after they have already called it,
	// then delete the old query and create a new one.
	destroyQueriesAndIters();

	allBarcodeScannersQuery = myRM->retrieveAll();

	// Couldn't get the query from which to access the Classes, just return false.
	if (!allBarcodeScannersQuery)
		return (HfcScannerProductRead *)NULL;

	DBQuery<DBBarcodeScanner>::const_iterator tempIter = allBarcodeScannersQuery->begin();
	if (tempIter == allBarcodeScannersQuery->end())
		return (HfcScannerProductRead *)NULL;

	/* Create a copy of the iterator, which will be used to access each record in the query, but maintaining
	 * the iterator for the entire use of the query (which extends beyond this function call) (see nextBarcodeScanner()
	 * method call below).
	 */
	allBarcodeScannersIter = new DBQuery<DBBarcodeScanner>::const_iterator(tempIter);
	DBBarcodeScanner *p = new DBBarcodeScanner(*(*allBarcodeScannersIter));
	HfcScannerProductRead *newBarcodeScanner = new HfcScannerProductRead(p, dbAccess);
	return newBarcodeScanner;
}

/* The return value indicates if the next record was read, or not.
 *
 * If the user has never called the firstBarcodeScanner() method, the first call to this method will
 * result in the first record being returned.
 */
HfcScannerProductRead *BarcodeScannerManager::nextBarcodeScanner()
{
	if (!dbAccess)
		return (HfcScannerProductRead *)NULL;

	if (allBarcodeScannersQuery && !allBarcodeScannersQuery->isEnabled())
		destroyQueriesAndIters();

	if (!allBarcodeScannersQuery || !allBarcodeScannersIter)
		return firstBarcodeScanner();

	/* Per Santos (9/30/02), if we increment the iterator as follows (prepend ++), then we do not get a new iterator.
	 * If we append the ++, we get a new iterator.  Thus, I can continue to use the same iterator throughout
	 * the query, without needing to use the copy constructor on the iterator for each record in the query.
	 */
	++(*allBarcodeScannersIter);
	if (*allBarcodeScannersIter == allBarcodeScannersQuery->end())
	{
		/* reclaim the memory from the query and the iterator used to traverse all BarcodeScanners in the db. */
		destroyQueriesAndIters();
		return (HfcScannerProductRead *)NULL;
	}

	/* Per Santos (9/30/02), we need to use the DBBarcodeScanner copy constructor in order for the Hfc HfcScannerProductRead to
	 * control the destruction of the DBBarcodeScanner record that the iterator contains; otherwise, the DBBarcodeScanner will
	 * be deleted when the iterator goes out of scope.  The DBBarcodeScanner copy will be deleted by the HfcScannerProductRead record
	 * created below.
	 */
	DBBarcodeScanner *p = new DBBarcodeScanner(*(*allBarcodeScannersIter));
	HfcScannerProductRead *newBarcodeScanner = new HfcScannerProductRead(p, dbAccess);
	return newBarcodeScanner;
}

/* Provide a countAll method so the caller doesn't need to know about a dbBarcodeScannersRM.
 */
int BarcodeScannerManager::countAllBarcodeScanners()
{
	if (!dbAccess)
		return -1;

	DBBarcodeScannersRM *myRM = dbAccess->getDBBarcodeScannersRM();
	if (!myRM)
		return -1;

	return myRM->countAll();
}

HfcScannerProductRead *BarcodeScannerManager::getCurrentBarcodeScanner()
{
	return currentBarcodeScanner;
}

void BarcodeScannerManager::setCurrentBarcodeScanner(HfcScannerProductRead *newBarcodeScanner)
{
	if (currentBarcodeScanner)
		delete currentBarcodeScanner;

	currentBarcodeScanner = newBarcodeScanner;
}

void BarcodeScannerManager::handleMessage(SessionMessage *msg)
{
	if (!msg)
		return;

	HfcScannerProductRead *msgBarcodeScanner = dynamic_cast<HfcScannerProductRead *> (msg->getHfcObject());

	UINT8 event = msg->getEventType();

	switch (event)
	{
		case DtRead:
			handleRead(msgBarcodeScanner);
			break;

		case DtReadAll:
			handleReadAll();
			break;

		case DtWrite:
			handleWrite(msgBarcodeScanner);
			break;

		case DtDelete:
			handleDelete(msg);
			break;

		case DtDeleteAll:
			handleDeleteAll();
			break;

		case DtSave:
			handleUpdateAndSave(msgBarcodeScanner);
			break;

		case DtRestore:
			handleRestore(msg);
			break;

		case DtCreateNew:
			handleCreate(msgBarcodeScanner);
			break;

		case BarcodeReadScannerCode:
			handleReadScannedCode(msgBarcodeScanner);
			break;

		default:
			Debug::trace("BarcodeScannerManager", HobString("UNKNOWN EVENT: ") + HobString(Integer::toString(event)));
			break;
	}
}

void BarcodeScannerManager::handleRead(HfcScannerProductRead *msgRec)
{
	if (!msgRec || !dbAccess)
		return;

	HfcScannerProductRead *lookedUpRec = HfcScannerProductRead::findBarcodeScanner(msgRec->getScannerCode(), dbAccess);

	// This is a read for supervisor mode when you do a read upon clicking a row so just send the query results back

	if (!lookedUpRec || lookedUpRec->getErrorCode() != RECORD_OK)		//	This HfcScannerProductRead does not exist
	{
			// Send error message to browser indicating HfcScannerProductRead not found - this likely can never happen bc we clicked on a row we sent to the scane in the first place
		if (mySession) 
			mySession->addDialogMessage(ErrBarcodeScannerDev, BarcodeScannerErrRecordNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

		if (lookedUpRec)
		{
			delete lookedUpRec;
			lookedUpRec = 0;
		}
		return;
	}

	// Get the description to attach it to the looked up scan code
	HobRecordNum productNum = lookedUpRec->getScannerProductNum();						// what is the PLU from what we just looked up
	Util::CSmartPtr<Product> theProduct(Product::findRecord(productNum, Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess));		// go and get the entire plu record with it.

	if (theProduct.null())
	{
		mySession->addDialogMessage(ErrBarcodeScannerDev, BarcodeScannerErrPluDoesNotExist, HfcResponse::RESPONSESTYLE_OKCANCEL);
		return;
	}

	HobString currentDescription = theProduct->getDescription();
	currentDescription.stripFontSequences();
	lookedUpRec->setScannerDescription(currentDescription);

	setCurrentBarcodeScanner(lookedUpRec);			// Yea we have a new current HfcScannerProductRead.
	Debug::trace("BarcodeScannerManager.handleRead", HobString("handleRead(): currentBarcodeScanner = ") + HobString(Hex::toString((long long) currentBarcodeScanner)));

	/////////////////////////////////////////

	if (mySession)
	{
		// Send the HfcScannerProductRead found back to the Browser.
		// Is this the behavior for ALL BarcodeScannerRead messages the Session receives, to send the data back to the Browser?
		// Is there any situation where something else reads a HfcScannerProductRead, but doesn't want the data sent back?
		SessionMessage barcodeScannerData(BarcodeScannerDev, BarcodeReadScannerCode);
		barcodeScannerData.setFormatType(BrowserFormat);
		barcodeScannerData.setHfcObject(currentBarcodeScanner);
		
		mySession->send(&barcodeScannerData, Messenger::DEST_UI);
	}
}

void BarcodeScannerManager::handleReadScannedCode( HfcScannerProductRead *msgRec )
{
	boop();							// we scanned a code, make it boop

	if ( !msgRec || !dbAccess )
		return;

	// See if this record exists in the database for this scan code?

	Debug::trace("BarcodeScannerManager.handleReadScannedCode", "START reading with scannercode: " + msgRec->getScannerCode() );

	HfcScannerProductRead *lookedUpRec = HfcScannerProductRead::findBarcodeScanner(msgRec->getScannerCode(), dbAccess);
	
	Session *s = dynamic_cast<Session *> (mySession);
	bool isInOperateMode = (s && s->isInOperateMode()) ? true : false;

	if( isInOperateMode )
	{
		Debug::trace("BarcodeScannerManager.handleReadScannedCode", "IF isInOperateMode.");

		// This is a read for Operate Mode so we just need to do a PLU lookup
		if (!lookedUpRec || lookedUpRec->getErrorCode() != RECORD_OK)		//	This HfcScannerProductRead does not exist
		{
			if (s->getCurrentMode() == OpModeSelfService)
			{
				Debug::trace("BarcodeScannerManager.handleReadScannedCode", "In Operate and Self Service Mode - might have an operator: " + HobString(Hex::toString((long long)currentBarcodeScanner) ) );

				// SOF - 4771 PJR - 421 Stater Brothers Self Service Login
				// We are in self service mode and we might be trying to log in an operator. Just send the code back to the UI and let them process it
				SessionMessage BarcodeScannerData(BarcodeScannerDev, BarcodeReadScannerCode);
				BarcodeScannerData.setFormatType(BrowserFormat);
				GenericHfcObject gho(msgRec->getScannerCode());
				BarcodeScannerData.setHfcObject(&gho);
				mySession->send(&BarcodeScannerData, Messenger::DEST_UI);

				if (lookedUpRec)
				{
					// dump the record since we are not going to use it
					delete lookedUpRec;
					lookedUpRec = 0;
				}

				return;
			}
			else
			{
				Debug::trace("BarcodeScannerManager.handleReadScannedCode", "IF isInOperateMode where !lookupRec or !RECORD_OK");

			    // Send error message to browser indicating scan code not found in database.
			    mySession->addDialogMessage(ErrBarcodeScannerDev, BarcodeScannerErrScanCodeNotFound, HfcResponse::RESPONSESTYLE_OKCANCEL);

			    if (lookedUpRec)
			    {
				    delete lookedUpRec;
				    lookedUpRec = 0;
			    }
			    return;
			}
		}

		setCurrentBarcodeScanner(lookedUpRec);												// We have a new current HfcScannerProductRead.

		HobRecordNum productNum = lookedUpRec->getScannerProductNum();						// what is the PLU from what we just looked up
		Util::CSmartPtr<Product> theProduct(Product::findRecord(productNum, Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess));		// go and get the entire plu record with it.

		if (theProduct.null())
			return;

		Session *s = dynamic_cast<Session *> (mySession);
		
		if (!s)
			return;

		GenericHfcObject gho(HobString(Long::toString(productNum)));
		SessionMessage pluMsg(BarcodeScannerDev, BarcodeScannerPlu);
		pluMsg.setFormatType(BrowserFormat);
		pluMsg.setHfcObject(&gho);						// Smart pointer here
		mySession->send(&pluMsg, Messenger::DEST_UI);
	}
	else
	{
		Debug::trace("BarcodeScannerManager.handleReadScannedCode", "isInSupervisorMode so create a new item in the db for this scanned item.");

		if (!lookedUpRec || lookedUpRec->getErrorCode() != RECORD_OK)		//	This HfcScannerProductRead does not exist
		{
			// Create a new record here for the scanned item
			handleCreate(msgRec);
		}
		else
		{
			setCurrentBarcodeScanner(lookedUpRec);								// We have a new current HfcScannerProductRead.

			// The scanned item already exists so just return it back to the user.  Also tell them that it exists
			mySession->addDialogMessage(ErrBarcodeScannerDev, BarcodeScannerErrScanItemAlreadyExists, HfcResponse::RESPONSESTYLE_OKCANCEL);
			
			SessionMessage BarcodeScannerData(BarcodeScannerDev, BarcodeReadScannerCode);
			BarcodeScannerData.setFormatType(BrowserFormat);
			BarcodeScannerData.setHfcObject(currentBarcodeScanner);
			mySession->send(&BarcodeScannerData, Messenger::DEST_UI);
		}	
	}
}

void BarcodeScannerManager::handleReadAll()
{
	if (!mySession || !dbAccess)
		return;

	DBBarcodeScannersRM *dbBarcodeScannersRM = dbAccess->getDBBarcodeScannersRM();
	if (!dbBarcodeScannersRM)
		return;

	AllBarcodeScanners scannerList;
	scannerList.findAllBarcodeScanners(mySession);

	SessionMessage scannerMsg(BarcodeScannerDev, DtWriteAll);
	scannerMsg.setFormatType(BrowserFormat);
	scannerMsg.setHfcObject(&scannerList);
	mySession->send(&scannerMsg, Messenger::DEST_UI);
}

void BarcodeScannerManager::handleWrite(HfcScannerProductRead *modifiedRec)
{
	if ((!modifiedRec) || (!currentBarcodeScanner))
		return;

	if (Debug::isTracing("BarcodeScannerManager"))
		Debug::trace("BarcodeScannerManager", HobString("handleWrite(): currentBarcodeScanner = ") + HobString(Hex::toString((long long) currentBarcodeScanner)) +
									  HobString("  , modifiedBarcodeScanner = ") + HobString(Hex::toString((long long) modifiedRec)));

	currentBarcodeScanner->clearErrors();
  	currentBarcodeScanner->updateBarcodeScanner(modifiedRec);

	if (mySession && Configuration::getInstance()->toBoolCfgI(CfgInternal::NOTIFYUIOFUPDATESTATUS))
	{
		SessionMessage updateStatusMsg(BarcodeScannerDev, DtUpdateStatus);
		updateStatusMsg.setFormatType(BrowserFormat);
		HfcUpdateStatus stat(!currentBarcodeScanner->hasErrors());
		updateStatusMsg.setHfcObject(&stat);
		mySession->send(&updateStatusMsg, Messenger::DEST_UI);
	}
}


void BarcodeScannerManager::handleDelete(SessionMessage *msg)
{
	if (!msg)
		return;

	HfcScannerProductRead *msgRec = dynamic_cast<HfcScannerProductRead *> (msg->getHfcObject());
	if (!msgRec || !dbAccess)
		return;

	HfcScannerProductRead *existingRec = HfcScannerProductRead::findBarcodeScanner(msgRec->getScannerCode(), dbAccess);
	if (existingRec)
	{
		existingRec->deleteRecord();
		delete existingRec;
		existingRec = 0;
	}
	else
		Debug::trace("BarcodeScannerManager", HobString("handleDelete - HfcScannerProductRead to be deleted was not in the database to delete - ScannerCode:") + msgRec->getScannerCode() );
}


void BarcodeScannerManager::handleDeleteAll()
{
	if (!dbAccess)
		return;

	DBBarcodeScannersRM *myRM = dbAccess->getDBBarcodeScannersRM();
	if (!myRM)
		return;

	myRM->deleteAll();

	destroyQueriesAndIters();
}


void BarcodeScannerManager::handleUpdateAndSave(HfcScannerProductRead *modifiedRec)
{
	if (currentBarcodeScanner)
	{
		handleWrite(modifiedRec);
		handleSaveRecord(currentBarcodeScanner);
	}
}

void BarcodeScannerManager::handleSaveRecord(HfcScannerProductRead *recordToSave)
{
	if (recordToSave)
		recordToSave->saveRecord();
}


void BarcodeScannerManager::handleRestore(SessionMessage *msg)
{
	if (!msg)
		return;

	HfcScannerProductRead *restoreRec = dynamic_cast<HfcScannerProductRead *> (msg->getHfcObject());
	if (!restoreRec || !dbAccess)
		return;

	receivedBarcodeScannerRestore = true;

	restoreRec->setDBAccess(dbAccess);

	HfcScannerProductRead *usedRec = (HfcScannerProductRead *)NULL;
	HfcScannerProductRead *existingRec = HfcScannerProductRead::findBarcodeScanner(restoreRec->getScannerCode(), dbAccess);
	if (existingRec)
	{
		if (existingRec->isRecordValid())
		{
			existingRec->updateBarcodeScanner(restoreRec);
			existingRec->saveRecord();
			usedRec = existingRec;								
		}
		else
		{
			existingRec->deleteRecord();	   
			restoreRec->saveRecord();
			usedRec = restoreRec;								
		}
	}
	else
	{
		restoreRec->saveRecord();
		usedRec = restoreRec;								
	}

	if (usedRec && usedRec->hasErrors() && mySession && mySession->isCommSession())
	{
		SessionMessage errMsg(Error, DtWrite);
		errMsg.setFormatType(EvolutionTaggedFormat);
		HfcError *err = usedRec->firstError();
		while (err)
		{
			errMsg.setHfcObject(err);
			mySession->send(&errMsg, Messenger::DEST_UI);

			err = usedRec->nextError();
		}
	}

	if (existingRec)
		delete existingRec;
}

void BarcodeScannerManager::handleCreate(HfcScannerProductRead *msgRec)
{
	if (!msgRec)
		return;

	HfcScannerProductRead *newRec = createNewBarcodeScanner(msgRec->getScannerCode());

	if (newRec)
	{
		//	Set this new HfcScannerProductRead as the current HfcScannerProductRead.
		setCurrentBarcodeScanner(newRec);

		if (mySession)
		{
			// Send the new HfcScannerProductRead to the Browser.
			SessionMessage msg(BarcodeScannerDev, BarcodeReadScannerCode);
			msg.setFormatType(BrowserFormat);
			msg.setHfcObject(currentBarcodeScanner);
			mySession->send(&msg, Messenger::DEST_UI);
		}
	}
}

HfcScannerProductRead *BarcodeScannerManager::createNewBarcodeScanner(HobString codeNum)
{
	HfcScannerProductRead *newRec = (HfcScannerProductRead *) NULL;
	if (mySession)
	{
		// Send the current new HfcScannerProductRead to the browser.
		newRec = new HfcScannerProductRead(codeNum);
		if (newRec)
			newRec->setDBAccess(dbAccess);
	}
	return newRec;
}

void BarcodeScannerManager::destroyQueriesAndIters()
{
	if (allBarcodeScannersIter)
	{
		delete allBarcodeScannersIter;
		allBarcodeScannersIter = (DBQuery<DBBarcodeScanner>::const_iterator *)NULL;
	}

	if (allBarcodeScannersQuery)
	{
		delete allBarcodeScannersQuery;
		allBarcodeScannersQuery = (DBQuery<DBBarcodeScanner> *)NULL;
	}
}

void BarcodeScannerManager::boop()
{
	SessionMessage msg(BarcodeScannerDev, BarcodeReadScannerCode);
	msg.setFormatType(BrowserFormat);
	mySession->send(&msg, Messenger::DEST_UI);
}
