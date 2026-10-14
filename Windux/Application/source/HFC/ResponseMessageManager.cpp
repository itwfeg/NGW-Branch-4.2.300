/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ResponseMessageManager.h"
#include "Configuration.h"
#include "Session.h"
#include "Messenger.h"
#include "SessionMessage.h"
#include "HfcResponse.h"
#include "ProductManager.h"
#include "Product.h"
#include "OperatorEvent.h"
#include "SessionManager.h"
#include "FpmManager.h"

#include "WeigherEvent.h"
#include "LabelApplierEvent.h"

#include "HfcWeigherStatus.h"
#include "HfcPrinterStatus.h"
#include "HfcLabelApplierStatus.h"
#include "HfcWrapperStatus.h"
#include "HfcEvolWrapperStatus.h"
#include "HfcOperationModeStatus.h"

#include "HobList.h"

#include "DBProductsRM.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#include "ServiceSpeaker.h"

#include "MemShow.h"
#include "MessageLogger.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ResponseMessageManager::dataType = 0;
std::string ResponseMessageManager::dataTypeName("ResponseMessageManager"); 
#endif


ResponseMessageManager::ResponseMessageManager(Session *session)
	: mySession(session), displayingDialogs(true)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	sentNegWeightMessNotRemovedYet = false;
	sentOutOfTiltMessNotRemovedYet = false;
	canopyError	= false;

	// SOF-3475
	// prevent spamming of certain messages
	messageWaitTimes[ ErrorDeviceCode(ErrOperatorDev, OperatorErrRFIDOperatorNotFound) ] = DurationMillisecondsType( 60 * 1000 );		// RFID tag !assigned to operator spam
	messageWaitTimes[ErrorDeviceCode(ErrWeigher, WeigherErrValMaxTiltReached)] = DurationMillisecondsType(30 * 1000);					// 3 degrees out of tilt spam

}

ResponseMessageManager::~ResponseMessageManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void ResponseMessageManager::manageSelfServeMessages()
{
	if (/*!Configuration::getInstance()->isSelfServiceScale() ||*/ !mySession)
	{
		Debug::trace("ResponseMessageManager.ss", "manageSelfServeMessages - don't have session");
		removeMessage(ErrProductDev, ProductErrPrintNoWeight, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	ProductManager *currentProductManager = mySession->getProductManager();
	if (!currentProductManager)
	{
		Debug::trace("ResponseMessageManager.ss", "manageSelfServeMessages - don't have a current product manager");
		removeMessage(ErrProductDev, ProductErrPrintNoWeight, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	Product *currentProduct = currentProductManager->getCurrentProduct();
	if (!currentProduct)
	{
		Debug::trace("ResponseMessageManager.ss", "manageSelfServeMessages - don't have a current product");
		removeMessage(ErrProductDev, ProductErrPrintNoWeight, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	if (currentProduct->isReadyToPrint())
	{
		Debug::trace("ResponseMessageManager.ss", "manageSelfServeMessages - current product is ready to print");
		removeMessage(ErrProductDev, ProductErrPrintNoWeight, HfcResponse::RESPONSESTYLE_OK);
	}
	else
	{
		//	Is it a label problem?
		if (currentProduct->hasLabelTypeError())
			Debug::trace("ResponseMessageManager.ss", "manageSelfServeMessages - current product is not ready to print due to LabelType error.");
	}
}


bool ResponseMessageManager::manageResponseAnswer(HfcResponse *answer)
{
	if (!answer)
		return false;

	return removeMessage(answer->getErrorDevice(), answer->getCode(), answer->getStyle());
}

/* Check if any printer status needs to be displayed, and tell the Session to display it.
 * Returns true if a status message gets displayed, else return false
 */
bool ResponseMessageManager::checkPrinterStatuses()
{
	Session* s = dynamic_cast<Session*> (mySession);
	if (!s)
		return false;

	for (int i = (int)FirstPrinter; i < (int)MaxPrinterTypes; i++)
	{
		HfcPrinterStatus* status = s->getCurrentPrinterStatus(i);

		if (status )
		{
			Debug::trace("ResponseMessageManager.EvolWrapper", "RMM::checkPrinterStatuses() - printer status, notification Reason Exists for printer: " + inttostring(i));
			manageCurrentPrinterStatus(status, i);
			return true;
		}
	}

	return false;
}

void ResponseMessageManager::manageStatus(HfcWeigherStatus *status)
{
	if (!status)
		return;

//	if (mySession && !mySession->isInOperateMode())
//		return;

	if (!SessionManager::getInstance()->isWeigherAttached())
		return;


#ifndef EVOL_NDEBUG
if (Debug::isTracing("Memory"))
	memShow(0);

	if (Debug::isTracing("ResponseMessageManager.weigher"))
	{
		Debug::trace("ResponseMessageManager.weigher","manageStatus");
		status->show();
	}
#endif

	if (!status->isPeripheralComm() || status->isManagerDeleted())
	{
		removeAllMessages(ErrWeigher);
		if (addMessage(ErrWeigher, WeigherErrCommLoss, HfcResponse::RESPONSESTYLE_OK))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Weigher stopped communicating ");
	}
	else
	{
		if (removeMessage(ErrWeigher, WeigherErrCommLoss, HfcResponse::RESPONSESTYLE_OK))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, " Weigher started communicating ");
	}

	if (status->isScaleWarmup() && !status->isNewZero())
	{
		addMessage(ErrWeigher, WeigherErrScaleWarmup, HfcResponse::RESPONSESTYLE_OK);
	}
	else
	{
		removeMessage(ErrWeigher, WeigherErrScaleWarmup, HfcResponse::RESPONSESTYLE_OK);
	}

	if (status->isZeroStableWeight())
	{
		removeMessage(ErrWeigher, WeigherErrScaleNotZero, HfcResponse::RESPONSESTYLE_OK);
	}

	if (status->isNewZero() && !status->isWeigherInError())
	{
		if ((Configuration::getInstance()->isPrepackScale() == true))
		{
			removeMessage(ErrWeigher, WeigherErrScaleWarmup, HfcResponse::RESPONSESTYLE_OK);			
		}
		
		removeMessage(ErrWeigher, WeigherErrRezeroing, HfcResponse::RESPONSESTYLE_OK);
	}


	if (mySession && !mySession->isInOperateMode())
		return;


	if (status->isWeigherConfigOutOfRange())
	{
		addMessage(ErrWeigher, WeigherErrConfigOutOfRange, HfcResponse::RESPONSESTYLE_OK);
	}

	if ((Configuration::getInstance()->isPrepackScale() == false) && (status->isNegativeGrossWeight()))
	{
		//Only send negative weight message if we haven't sent in once (and hasn't been removed)
		if (!sentNegWeightMessNotRemovedYet)
		{
			addMessage(ErrWeigher, WeigherErrNegativeGrossWeight, HfcResponse::RESPONSESTYLE_REZERO);
			sentNegWeightMessNotRemovedYet = true;
		}
	}
	else
	{
		if (sentNegWeightMessNotRemovedYet)
		{
			//determine ahead of time what we are going to send to the UI for the weigher status			
			status->determineDisplayValues();			
			//get the weight value going to be sent to the UI			
			HobString weightGoingToBrowser = status->getWeightDisplay();

			if (strcmp(weightGoingToBrowser.c_str(), ""))
			{
				removeMessage(ErrWeigher, WeigherErrNegativeGrossWeight, HfcResponse::RESPONSESTYLE_OK);
				removeMessage(ErrWeigher, WeigherErrCantRezeroNegativeGrossWeight, HfcResponse::RESPONSESTYLE_OK);
				sentNegWeightMessNotRemovedYet = false;
			}
			else
			{
				Debug::trace("ResponseMessageManager", "We are going to send a blank weight to the UI, don't remove negative message box just yet..");	
			}		
		}
	}

	if (status->isOverGrossWeightLimit())
	{
		addMessage(ErrWeigher, WeigherErrOverGrossWeightLimit, HfcResponse::RESPONSESTYLE_OK);
	}
	else
	{
		removeMessage(ErrWeigher, WeigherErrOverGrossWeightLimit, HfcResponse::RESPONSESTYLE_OK);
	}

	if (status->isZeroOutOfMaintZone())
	{
		if (status->isScaleWarmup())
		{
			addMessage(ErrWeigher, WeigherErrZeroOutOfMaintZone, HfcResponse::RESPONSESTYLE_OK);
		}
		else
		{
			removeMessage(ErrWeigher, WeigherErrRezeroing, HfcResponse::RESPONSESTYLE_OK);
			if (addMessage(ErrWeigher, WeigherErrZeroOutOfMaintZoneStatus, HfcResponse::RESPONSESTYLE_OK))
				MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Weigher is Outside zero maintenance zone ");
		}
	}
	else
	{
		removeMessage(ErrWeigher, WeigherErrZeroOutOfMaintZone, HfcResponse::RESPONSESTYLE_OK);
		if (removeMessage(ErrWeigher, WeigherErrZeroOutOfMaintZoneStatus, HfcResponse::RESPONSESTYLE_OK))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Weigher is Inside zero maintenance zone ");
	}

	// VAL_MAX
	if (status->isValMaxEepCommFailure())
	{
		addMessage(ErrWeigher, WeigherErrValMaxEepCommFailure, HfcResponse::RESPONSESTYLE_REBOOT_CANCEL);
	}
	else
	{
		//SOF-3958
		//removeMessage(ErrWeigher, WeigherErrValMaxEepCommFailure, HfcResponse::RESPONSESTYLE_REBOOT_CANCEL);
	}

	if (status->isValMaxEepChecksumFailure())
	{
		addMessage(ErrWeigher, WeigherErrValMaxEepChecksumFailure, HfcResponse::RESPONSESTYLE_OK);
	}
	else
	{
		//SOF-3958
		//removeMessage(ErrWeigher, WeigherErrValMaxEepChecksumFailure, HfcResponse::RESPONSESTYLE_OK);
	}

	if (status->isValMaxEepAcclCommFailure())
	{
		addMessage(ErrWeigher, WeigherErrValMaxEepAcclCommFailure, HfcResponse::RESPONSESTYLE_REBOOT_CANCEL);
	}
	else
	{
		//SOF-3958
		//removeMessage(ErrWeigher, WeigherErrValMaxEepAcclCommFailure, HfcResponse::RESPONSESTYLE_REBOOT_CANCEL);
	}

	if (status->isValMaxTiltReached())
	{
		Debug::trace("ResponseMessageManager.tilt", "isValMaxTiltReached() IS TRUE so DISPLAY message");
		//if (!sentOutOfTiltMessNotRemovedYet)
		//{
		addMessage(ErrWeigher, WeigherErrValMaxTiltReached, HfcResponse::RESPONSESTYLE_OK);
		sentOutOfTiltMessNotRemovedYet = true;
		// }
	}
	else
	{
		Debug::trace("ResponseMessageManager.tilt", "isValMaxTiltReached() IS FALSE so REMOVE message");
		//if (sentOutOfTiltMessNotRemovedYet)
		//{
		removeMessage(ErrWeigher, WeigherErrValMaxTiltReached, HfcResponse::RESPONSESTYLE_OK);
		sentOutOfTiltMessNotRemovedYet = false;
		//}
	}

	manageSelfServeMessages();

#ifndef EVOL_NDEBUG
if (Debug::isTracing("Memory"))
	memShow(0);
#endif
}

void ResponseMessageManager::manageStatus(HfcPrinterStatus *newStatus, HfcPrinterStatus *currentStatus)
{
	if (!newStatus)
		return;

	newPrinterStatus = newStatus;
	currentPrinterStatus = currentStatus;

	if (mySession && !mySession->isInOperateMode())
		return;

	if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale())
	{
		if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeSearchKeypadFlashkeys) && (Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors) == SelfServOption_PrintToScreenAndLabel))
			Debug::trace("ResponseMessageManager.TestScreenLabel", "ResponseMessageManager Don't skip manageStatus if BigY");
		else
		{
			Debug::trace("ResponseMessageManager.TestScreenLabel", "ResponseMessageManager SKIP manageStatus");
			return;
		}
	}
	
#ifndef EVOL_NDEBUG
	if (Debug::isTracing("ResponseMessageManager.printer"))
	{
		Debug::trace("ResponseMessageManager.printer","manageStatus");
		newStatus->show();
	}
#endif

	PrinterType prType = newStatus->getPrinterType();

	if (newStatus->isHeadDisengaged())
	{
		if (!currentStatus || !currentStatus->isHeadDisengaged())
		{
			if (Configuration::getInstance()->isPrepackScale())
				addPrinterMessage(PrinterErrHeadUpForEpp, HfcResponse::RESPONSESTYLE_OK, prType);
			else
				addPrinterMessage(PrinterErrHeadUp, HfcResponse::RESPONSESTYLE_OK, prType);
		}

		//	PTR 679 & 520 - Remove the Product error LabelNotSized and NoLabelTypesSelected messages.
		switch (prType)
		{
			case FirstPrinter:
				removeMessage(ErrProductDev, ProductErrPriLabelNotSized, HfcResponse::RESPONSESTYLE_OK);
				break;
			case SecondPrinter:
				removeMessage(ErrProductDev, ProductErrSecLabelNotSized, HfcResponse::RESPONSESTYLE_OK);
				break;
			case ThirdPrinter:
				removeMessage(ErrProductDev, ProductErrThirdLabelNotSized, HfcResponse::RESPONSESTYLE_OK);
				break;
	        default:
	            // Nothing (handle the other cases)
	            break;
		}
		removeMessage(ErrProductDev, ProductErrNoLabelTypesSelected, HfcResponse::RESPONSESTYLE_OK);
	}
	else
	{
		if (Configuration::getInstance()->isPrepackScale() == true)
			removePrinterMessage(PrinterErrHeadUpForEpp, HfcResponse::RESPONSESTYLE_OK, prType);
		else
			removePrinterMessage(PrinterErrHeadUp, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	if (newStatus->isLabelSizing() && !newStatus->isTakeLabelError())
	{
		if (!currentStatus || !(currentStatus->isLabelSizing() && !currentStatus->isTakeLabelError()))
			addPrinterMessage(PrinterErrLabelSizing, HfcResponse::RESPONSESTYLE_OK, prType);
	}
	else
	{
		removePrinterMessage(PrinterErrLabelSizing, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	if (newStatus->isLabelSizing())
	{
		switch (newStatus->getPrinterType())
		{
			case FirstPrinter:
				removeMessage(ErrProductDev, ProductErrPriLabelNotSized, HfcResponse::RESPONSESTYLE_OK);
				break;
			case SecondPrinter:
				removeMessage(ErrProductDev, ProductErrSecLabelNotSized, HfcResponse::RESPONSESTYLE_OK);
				break;
			case ThirdPrinter:
				removeMessage(ErrProductDev, ProductErrThirdLabelNotSized, HfcResponse::RESPONSESTYLE_OK);
				break;
	        default:
	            // Nothing (handle the other cases)
	            break;
		}
	}

	// SOF-4404
	// timing issue where we add error message, then self service kicks in, and message is never removed
	// fix is to remove it all the time, but only suppress adding it in self service mode
	const bool isSelfServiceOperateMode( Configuration::getInstance()->isSelfServiceScale() && mySession && mySession->isInOperateMode() );

	if (newStatus->isLabelTakenError())
	{
		if (( !isSelfServiceOperateMode ) && (!currentStatus || !currentStatus->isLabelTakenError()))
			addPrinterMessage(PrinterErrTakeLabelBeep, HfcResponse::RESPONSESTYLE_OK, prType);
		else if ((isSelfServiceOperateMode) && ((Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeEnableFreshServe))) && (!currentStatus || !currentStatus->isTakeLabelError()))
			addPrinterMessage(PrinterErrTakeLabel, HfcResponse::RESPONSESTYLE_NOINPUT, prType);
	}
	else
	{
		removePrinterMessage(PrinterErrTakeLabelBeep, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	if (newStatus->isTakeLabelError())
	{
		if ( (!isSelfServiceOperateMode) && (!currentStatus || !currentStatus->isTakeLabelError())) 
			addPrinterMessage(PrinterErrTakeLabel, HfcResponse::RESPONSESTYLE_OK, prType);                                     
		else if ((isSelfServiceOperateMode) && ( (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeEnableFreshServe) ) ) && (!currentStatus || !currentStatus->isTakeLabelError()))
			addPrinterMessage(PrinterErrTakeLabel, HfcResponse::RESPONSESTYLE_NOINPUT, prType);
	}
	else
	{
		// For Squish auto-testing of Fastrack, System Test wanted to be notified that the label has been taken,
		// irregardless of whether or not that error dialog is displayed.  This will accomplish that.
		// If the printer status message indicates that the label has been taken, which I was told that they
		// would fake this out with some kind of hardware gizmo on the label cassette, or some other way,
		// whatever they come up with, doesn't really matter to me, ... then I need to notify the UI that the
		// "label has been taken"; and I have to do this even if the ResponseMessageManager never displayed the
		// "take label" dialog, so here you go (with the following two lines ...
		// We are triggering this with an internal configuration parameter (being set in config.dat),
		// as you can see with the following line.
		if (Configuration::getInstance()->toBoolCfgI("AutoTesterNeedsLabelTaken"))
			sendMessage(ErrRemoveAnyModalDialog, PrinterErrTakeLabel, HfcResponse::RESPONSESTYLE_OK);

		removePrinterMessage(PrinterErrTakeLabel, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	if (newStatus->isMediaJammed())
	{
		if (!currentStatus || !currentStatus->isMediaJammed())
			addPrinterMessage(PrinterErrLabelJammed, HfcResponse::RESPONSESTYLE_OK, prType);
	}
	else
	{
		removePrinterMessage(PrinterErrLabelJammed, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	if (newStatus->isLabelMissing())
	{
		Debug::trace("ResponseMessageManager.PrinterStatus", "Label is missing!"); 
		if ( !currentStatus || !currentStatus->isLabelMissing() ) {
			Debug::trace("ResponseMessageManager.PrinterStatus", "Label is missing: sent to UI");
			addPrinterMessage(PrinterErrLabelMissing, HfcResponse::RESPONSESTYLE_OK, prType);
		} else {
			if( FpmManager::getInstance()->isFreeStandingSelfService() ) {
				addPrinterMessage(PrinterErrLabelMissing, HfcResponse::RESPONSESTYLE_OK, prType);
			} else {
				Debug::trace("ResponseMessageManager.PrinterStatus", "Label is missing: not sent to UI"); 
			}
		}
	}
	else
	{
        //HA-74: We would dismiss this message when the new status would be received.
        //Instead, we should make them press Ok on the screen so they know something is wrong
        if( !Configuration::getInstance()->isPrepackScale() )
        {
        	Debug::trace("ResponseMessageManager.PrinterStatus", "Remove Label is missing: sent to UI");
		    removePrinterMessage(PrinterErrLabelMissing, HfcResponse::RESPONSESTYLE_OK, prType);
        }
	}

	if (newStatus->isOutOfMedia())
	{
		if (!currentStatus || !currentStatus->isOutOfMedia())
		{
			Debug::trace("PrinterManager.Error", "ResponseMessageManager: Out of Labels");
			if (addPrinterMessage(PrinterErrOutOfMedia, HfcResponse::RESPONSESTYLE_OK, prType))
			{
				HobString prStr("Unknown");
				if (prType == FirstPrinter)
					prStr = "Primary";
				else if (prType == SecondPrinter)
					prStr = "Second";
				else if (prType == ThirdPrinter)
					prStr = "Third";
				prStr.append(" Printer out of labels.");
				MessageLogger::logMessage(MessageLogger::ScaleEventsLog, prStr);
			}
		}
	}
	else
	{
		if (removePrinterMessage(PrinterErrOutOfMedia, HfcResponse::RESPONSESTYLE_OK, prType))
		{
			HobString prStr("Unknown");
			if (prType == FirstPrinter)
				prStr = "Primary";
			else if (prType == SecondPrinter)
				prStr = "Second";
			else if (prType == ThirdPrinter)
				prStr = "Third";
			prStr.append(" Printer labels loaded.");
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, prStr);
		}
	}

	if ( newStatus->isStockLow() )	//SOF-4608
	{
		addPrinterMessage(PrinterErrLowStock, HfcResponse::RESPONSESTYLE_OK, prType);	
	}
	else 
	{
		removePrinterMessage(PrinterErrLowStock, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	if ( newStatus->isOutOfStock() )	//SOF-4608
	{
		//SOF-5190
		if( mySession ) {
			if( mySession->getCurrentMode() == OpModeSelfService ) 
				addPrinterMessage(PrinterErrOutOfStock, HfcResponse::RESPONSESTYLE_NOINPUT, prType);	
			else 
				addPrinterMessage(PrinterErrOutOfStock, HfcResponse::RESPONSESTYLE_OK, prType);		
		} else {
			addPrinterMessage(PrinterErrOutOfStock, HfcResponse::RESPONSESTYLE_OK, prType);		
		}
	}
	else 
	{
		removePrinterMessage(PrinterErrOutOfStock, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	if (newStatus->isCassetteMissing())
	{
		if (!currentStatus || !currentStatus->isCassetteMissing())
			addPrinterMessage(PrinterErrCassetteMissing, HfcResponse::RESPONSESTYLE_OK, prType);
	}
	else
	{
		removePrinterMessage(PrinterErrCassetteMissing, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	if (newStatus->isThermalOverload())
	{
		if (!currentStatus || !currentStatus->isThermalOverload())
			addPrinterMessage(PrinterErrThermalOverload, HfcResponse::RESPONSESTYLE_OK, prType);
	}
	else
	{
		removePrinterMessage(PrinterErrThermalOverload, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	if (newStatus->isThermalShutdown())
	{
		if (!currentStatus || !currentStatus->isThermalShutdown())
			addPrinterMessage(PrinterErrThermalShutdown, HfcResponse::RESPONSESTYLE_OK, prType);
	}
	else
	{
		removePrinterMessage(PrinterErrThermalShutdown, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	if (newStatus->isHardwareError() || newStatus->isMediaShutdown())
	{
		if (!currentStatus || !(currentStatus->isHardwareError() || currentStatus->isMediaShutdown()))
			addPrinterMessage(PrinterErrHardwareError, HfcResponse::RESPONSESTYLE_OK, prType);
	}
	else
	{
		removePrinterMessage(PrinterErrHardwareError, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	if (!newStatus->isPeripheralComm())
	{
		if (!currentStatus || currentStatus->isPeripheralComm())
		{
			if (addPrinterMessage(PrinterErrCommLoss, HfcResponse::RESPONSESTYLE_OK, prType))
			{
				MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Printer communication lost.");
			}
		}
	}
	else
	{
		if (removePrinterMessage(PrinterErrCommLoss, HfcResponse::RESPONSESTYLE_OK, prType))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, " Printer communications established");
	}

	if (newStatus->isManagerDeleted())
	{
		if (!currentStatus || !currentStatus->isManagerDeleted())
			addPrinterMessage(PrinterErrManagerDeleted, HfcResponse::RESPONSESTYLE_OK, prType);
	}
	else
	{
		removePrinterMessage(PrinterErrManagerDeleted, HfcResponse::RESPONSESTYLE_OK, prType);
	}

	manageSelfServeMessages();
}

void ResponseMessageManager::manageCurrentPrinterStatus(HfcPrinterStatus* currentStatus, int printerNum)
{
	if (!currentStatus)
		return;

	if (mySession && !mySession->isInOperateMode())
		return;

	if (currentStatus && currentStatus->isHeadDisengaged() )
	{
		Debug::trace("ResponseMessageManager.PrinterStatus", "ResponseMesRMM:: manageCurrentPrinterStatus(Printer[" + HobString(Integer::toString(printerNum)) + "]) is disengaged so pop dialog" );
		if (Configuration::getInstance()->isPrepackScale())
			addPrinterMessage(PrinterErrHeadUpForEpp, HfcResponse::RESPONSESTYLE_OK, (PrinterType)printerNum);
		else
			addPrinterMessage(PrinterErrHeadUp, HfcResponse::RESPONSESTYLE_OK, (PrinterType)printerNum);
	}

	// Could add more conditions here
	////////////////////////////////////////////////////
}

bool ResponseMessageManager::addPrinterMessage(int errorCode, short responseStyle, PrinterType prType)
{
	if (Configuration::getInstance()->isSelfServiceScale() == false)
	{
		errorCode = translatePrinterMessage(errorCode, prType);
	}

	bool ret = addMessage(ErrPrinter, errorCode, responseStyle);
	return ret;
}

bool ResponseMessageManager::removePrinterMessage(int errorCode, short responseStyle, PrinterType prType)
{
	if (Configuration::getInstance()->isSelfServiceScale() == false)
	{
		errorCode = translatePrinterMessage(errorCode, prType);
	}

	bool ret = removeMessage(ErrPrinter, errorCode, responseStyle);
	return ret;
}

void ResponseMessageManager::manageStatus(HfcLabelApplierStatus *status)
{
	if (!status)
		return;

	if (mySession && !mySession->isInOperateMode())
		return;


if (Debug::isTracing("Memory"))
	memShow(0);

	if (Debug::isTracing("ResponseMessageManager.labelApplier"))
		status->show();

	// add label applier error windows here!!
	if (status->isPrinter1NoWakeup() == true)
	{
		if (addMessage(ErrLabelApplier, LaErrPrinter1NoWakeupError, HfcResponse::RESPONSESTYLE_OK))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "The First Printer Station is not responding");
	}
	else
	{
		if (removeMessage(ErrLabelApplier, LaErrPrinter1NoWakeupError, HfcResponse::RESPONSESTYLE_OK))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "The First Printer Station has responded");
	}
	if (status->isPrinter2NoWakeup() == true)
	{
		if (addMessage(ErrLabelApplier, LaErrPrinter2NoWakeupError, HfcResponse::RESPONSESTYLE_OK))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "The Second Printer Station is not responding");
	}
	else
	{
		if (removeMessage(ErrLabelApplier, LaErrPrinter2NoWakeupError, HfcResponse::RESPONSESTYLE_OK))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "The Second Printer Station has responded");
	}	
	// This message should not go away automatically when the status for it is cleared.
	// The user must press the OK button to clear it.
	if (status->isPackageOverrun() == true)
		addMessage(ErrLabelApplier, LaErrPackageOverrunError, HfcResponse::RESPONSESTYLE_OK);

	if (status->isHighVoltage() == true)
		addMessage(ErrLabelApplier, LaErrHighVoltageError, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrHighVoltageError, HfcResponse::RESPONSESTYLE_OK);

	if (status->isPrinter1ArmOut() == true)
		addMessage(ErrLabelApplier, LaErrPrinter1ArmOutError, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrPrinter1ArmOutError, HfcResponse::RESPONSESTYLE_OK);

	if (status->isPrinter2ArmOut() == true)
		addMessage(ErrLabelApplier, LaErrPrinter2ArmOutError, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrPrinter2ArmOutError, HfcResponse::RESPONSESTYLE_OK);

	if (status->isWeigherHardware() == true)
		addMessage(ErrLabelApplier, LaErrWeigherHardwareError, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrWeigherHardwareError, HfcResponse::RESPONSESTYLE_OK);

	if (status->isModeError() == true)
		addMessage(ErrLabelApplier, LaErrModeError, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrModeError, HfcResponse::RESPONSESTYLE_OK);

	if (status->isPrinter1Hardware() == true)
		addMessage(ErrLabelApplier, LaErrPrinter1HardwareError, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrPrinter1HardwareError, HfcResponse::RESPONSESTYLE_OK);

	if (status->isPrinter2Hardware() == true)
		addMessage(ErrLabelApplier, LaErrPrinter2HardwareError, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrPrinter2HardwareError, HfcResponse::RESPONSESTYLE_OK);
	/* 10/10/2013 Alex Zavala's new errors CLA	*/
	if (status->isWeigherConveyorBadHall() == true)
		addMessage(ErrLabelApplier, LaErrWeigherConveyorBadHallReading, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrWeigherConveyorBadHallReading, HfcResponse::RESPONSESTYLE_OK);

	if (status->isWeigherConveyorJammed() == true)
		addMessage(ErrLabelApplier, LaErrWeigherConveyorJammed, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrWeigherConveyorJammed, HfcResponse::RESPONSESTYLE_OK);

	if (status->isWeigherConveyorStall() == true)
		addMessage(ErrLabelApplier, LaErrWeigherConveyorStall, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrWeigherConveyorStall, HfcResponse::RESPONSESTYLE_OK);

	if (status->isWeigherConveyorPkgSensor() == true)
		addMessage(ErrLabelApplier, LaErrWeigherConveyorPkgSensor, HfcResponse::RESPONSESTYLE_NOINPUT);			//	Was RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrWeigherConveyorPkgSensor, HfcResponse::RESPONSESTYLE_NOINPUT);		//	Was RESPONSESTYLE_OK);

	if (status->isConveyorBadHall() == true)
		addMessage(ErrLabelApplier, LaErrConveyorBadHallReading, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrConveyorBadHallReading, HfcResponse::RESPONSESTYLE_OK);

	if (status->isConveyorJammed() == true)
		addMessage(ErrLabelApplier, LaErrConveyorJammed, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrConveyorJammed, HfcResponse::RESPONSESTYLE_OK);

	if (status->isConveyorStall() == true)
		addMessage(ErrLabelApplier, LaErrConveyorStall, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrConveyorStall, HfcResponse::RESPONSESTYLE_OK);

	if (status->isConveyorPkgSensor() == true)
		addMessage(ErrLabelApplier, LaErrConveyorPkgSensor, HfcResponse::RESPONSESTYLE_NOINPUT);			//	Was RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrConveyorPkgSensor, HfcResponse::RESPONSESTYLE_NOINPUT);			//	Was RESPONSESTYLE_OK);

	if (status->isConveyorPkgEdgeSensor() == true)
		addMessage(ErrLabelApplier, LaErrConveyorPkgEdgeSensor, HfcResponse::RESPONSESTYLE_NOINPUT);		//	Was RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrConveyorPkgEdgeSensor, HfcResponse::RESPONSESTYLE_NOINPUT);		//	Was RESPONSESTYLE_OK);

	if (status->isMotor1FuseBlown() == true)
		addMessage(ErrLabelApplier, LaErrMotor1FuseBlown, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrMotor1FuseBlown, HfcResponse::RESPONSESTYLE_OK);

	if (status->isMotor2FuseBlown() == true)
		addMessage(ErrLabelApplier, LaErrMotor2FuseBlown, HfcResponse::RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrMotor2FuseBlown, HfcResponse::RESPONSESTYLE_OK);

	if (status->isPkgWakeUpSensor() == true)
		addMessage(ErrLabelApplier, LaErrPkgWakeUpSensor, HfcResponse::RESPONSESTYLE_NOINPUT);				//	Was RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrPkgWakeUpSensor, HfcResponse::RESPONSESTYLE_NOINPUT);			//	Was RESPONSESTYLE_OK);

	if (status->isPkgGateSensor() == true)
		addMessage(ErrLabelApplier, LaErrPkgGateSensor, HfcResponse::RESPONSESTYLE_NOINPUT);				//	Was RESPONSESTYLE_OK);
	else
		removeMessage(ErrLabelApplier, LaErrPkgGateSensor, HfcResponse::RESPONSESTYLE_NOINPUT);			//	Was RESPONSESTYLE_OK);

	/*	END OF NEW ERRORS	*/
	if (!status->isPeripheralComm() || status->isManagerDeleted())
	{
		removeAllMessages(ErrLabelApplier);
		if (addMessage(ErrLabelApplier, LabelApplierErrCommLoss, HfcResponse::RESPONSESTYLE_OK))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Label Applier stopped communicating");
	}
	else
		if (removeMessage(ErrLabelApplier, LabelApplierErrCommLoss, HfcResponse::RESPONSESTYLE_OK))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Label Applier has started communicating");
}

void ResponseMessageManager::manageStatus(HfcWrapperStatus *status)
{
	if (!status)
		return;

/*
	if (mySession && !mySession->isInOperateMode())
		return;
*/

	if (Debug::isTracing("ResponseMessageManager.wrapper"))
		status->show();

	if (status->getInvalidPackSize() == true)
	{
		addMessage(ErrWrapper, ErrWrapper * DEVICE_ERROR_OFFSET + WrapperErrorInfo::InvalidPackSize, HfcResponse::RESPONSESTYLE_CANCEL);
	}

	if (!status->isPeripheralComm() || status->isManagerDeleted())
	{
		removeAllMessages(ErrWrapper);
		if (addMessage(ErrWrapper, ErrWrapper * DEVICE_ERROR_OFFSET + WrapperErrorInfo::WrapperCommLost, HfcResponse::RESPONSESTYLE_CANCEL))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Wrapper has lost communication ");
	}
	else
	{
		if (removeMessage(ErrWrapper, ErrWrapper * DEVICE_ERROR_OFFSET + WrapperErrorInfo::WrapperCommLost, HfcResponse::RESPONSESTYLE_CANCEL))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Wrapper has established communication ");
	}
}

void ResponseMessageManager::manageStatus(HfcEvolWrapperStatus *status)
{
	if (!status)
		return;

	if (!status->isPeripheralComm())
	{
		removeAllMessages(ErrWrapper);
		if (addMessage((ErrorEvent)(10), 10017 /*ErrWrapper * DEVICE_ERROR_OFFSET + EvolWrapperErrorInfo::EvolWrapperCommLost*/, HfcResponse::RESPONSESTYLE_CANCEL))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, " The Wrapper did not respond (10017).");
	}
	else if (status->isManagerDeleted())
	{
		removeAllMessages(ErrWrapper);
		if (addMessage((ErrorEvent)(10), 10018 /*ErrWrapper * DEVICE_ERROR_OFFSET + EvolWrapperErrorInfo::EvolWrapperCommLost*/, HfcResponse::RESPONSESTYLE_CANCEL))
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "The Wrapper did not respond (10018).");
	}
	else
	{
		int gen = status->getCurrErrorInfo().getGeneralWrapper() - 501; //EvolWrapperErrorOffset;
		Debug::trace("ResponseMessageManager.EvolWrapper", HobString(" general wrapper error is ") + HobString(Integer::toString(gen)));
		if (gen == 201)
		{
			if (addMessage((ErrorEvent)(10), 10014, HfcResponse::RESPONSESTYLE_OK))
				MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "The Wrapper did not respond (10014).");
		}
		else if (gen == 202)
		{
			removeMessage((ErrorEvent)(10), 10014, HfcResponse::RESPONSESTYLE_OK);
			if (addMessage((ErrorEvent)(10), 10015, HfcResponse::RESPONSESTYLE_OK))
				MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "The Wrapper did not respond (10015).");
		}
		else if (gen == 203)
		{
			removeMessage((ErrorEvent)(10), 10015, HfcResponse::RESPONSESTYLE_OK);
			if (addMessage((ErrorEvent)(10), 10016, HfcResponse::RESPONSESTYLE_OK))
				MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "The Wrapper did not respond (10016).");
		}
		else
		{
			removeMessage((ErrorEvent)(10), 10017 /*ErrWrapper * DEVICE_ERROR_OFFSET + EvolWrapperErrorInfo::EvolWrapperCommLost*/, HfcResponse::RESPONSESTYLE_CANCEL);
		}
	}
}

void ResponseMessageManager::manageStatus(HfcOperationModeStatus *status)
{
	if (status && mySession && (mySession->isInOperateMode() || mySession->isInService()))
	{
		bool wrapperError = false;
		
		// This is a status message!
		if (status->isEvolWrapperHoming() == true)
		{
			//removeAllMessages(ErrWrapper);
			if (!wrapperError)
			{
				//	Only put this message up if there are no other wrapper errors.
				addMessage(ErrOperationMode,
						   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::EvolWrapperHoming,
						   HfcResponse::RESPONSESTYLE_OK);
			}
		}
		else
		{
			removeMessage(ErrOperationMode,
						  ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::EvolWrapperHoming,
						  HfcResponse::RESPONSESTYLE_OK);
		}

		if(SessionManager::getInstance()->isEvolutionWrapperAttached()==false)
		{
			//Alex Zavala 1/3/14
			if(status->isPkgDischargeError(FirstPrinter) == true)
			{
				addMessage(	ErrOperationMode,
							ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::PkgDischargeErrorFirstPrinter,
							HfcResponse::RESPONSESTYLE_OK);
			}
			else
			{
				removeMessage(ErrOperationMode,
							ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::PkgDischargeErrorFirstPrinter,
							HfcResponse::RESPONSESTYLE_OK);
			}

			if(status->isPkgDischargeError(SecondPrinter) == true)
			{
				addMessage(	ErrOperationMode,
							ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::PkgDischargeErrorSecondPrinter,
							HfcResponse::RESPONSESTYLE_OK);
			}
			else
			{
				removeMessage(ErrOperationMode,
							ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::PkgDischargeErrorSecondPrinter,
							HfcResponse::RESPONSESTYLE_OK);
			}

			if(status->isPkgDischargeError(ThirdPrinter) == true)
			{
				addMessage(	ErrOperationMode,
							ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::PkgDischargeErrorThirdPrinter,
							HfcResponse::RESPONSESTYLE_OK);
			}
			else
			{
				removeMessage(ErrOperationMode,
							ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::PkgDischargeErrorThirdPrinter,
							HfcResponse::RESPONSESTYLE_OK);
			}
		}
		if (status->isWrapperPressStart() == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperPressStart,
					   HfcResponse::RESPONSESTYLE_OK);
		}
		else
		{
			removeMessage(ErrOperationMode,
						  ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperPressStart,
						  HfcResponse::RESPONSESTYLE_OK);
		}

		// add the other op mode (non-wrapper) errors here
		if (status->isWrapperIdleError() == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperIdleError,
					   HfcResponse::RESPONSESTYLE_OK);
		}
		else
		{
			removeMessage(ErrOperationMode,
						  ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperIdleError,
						  HfcResponse::RESPONSESTYLE_OK);
		}

		if (status->isWrapperOverWeight() == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperOverWeight,
					   HfcResponse::RESPONSESTYLE_OK);
		}
		else
		{
			removeMessage(ErrOperationMode,
						  ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperOverWeight,
						  HfcResponse::RESPONSESTYLE_OK);
		}

		if (status->isWrapperGhostPackage() == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperGhostPackage,
					   HfcResponse::RESPONSESTYLE_OK);
		}
		else
		{
			removeMessage(ErrOperationMode,
						  ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperGhostPackage,
						  HfcResponse::RESPONSESTYLE_OK);
		}

		if (status->isEvolWrapperCalibratePlacePkg() == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::EvolWrapperCalPlacePackage,
					   HfcResponse::RESPONSESTYLE_OK);
		}
		else
		{
			removeMessage(ErrOperationMode,
						  ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::EvolWrapperCalPlacePackage,
						  HfcResponse::RESPONSESTYLE_OK);
		}

		if (status->isEvolWrapperBuildingAirPressure() == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::BuildingAirPressure,
					   HfcResponse::RESPONSESTYLE_OK);
		}
		else
		{
			removeMessage(ErrOperationMode,
						  ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::BuildingAirPressure,
						  HfcResponse::RESPONSESTYLE_OK);
		}

		if (status->isPrinterOverflow(FirstPrinter) == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::PrinterOverflowFirstPrinter,
					   HfcResponse::RESPONSESTYLE_CANCEL);
		}

		if (status->isPrinterOverflow(SecondPrinter) == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::PrinterOverflowSecondPrinter,
					   HfcResponse::RESPONSESTYLE_CANCEL);
		}

		if (status->isPrinterOverflow(ThirdPrinter) == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::PrinterOverflowThirdPrinter,
					   HfcResponse::RESPONSESTYLE_CANCEL);
		}

		if (status->isLabelApplierPackageOverrun() == true)
			addMessage(ErrLabelApplier, LaErrPackageOverrunError, HfcResponse::RESPONSESTYLE_OK);
		else
			removeMessage(ErrLabelApplier, LaErrPackageOverrunError, HfcResponse::RESPONSESTYLE_OK);

		if (status->isEncoderWindow_1() == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperTestEncoderWindow_1_Failed,
					   HfcResponse::RESPONSESTYLE_OK);
		}

		if (status->isEncoderWindow_2() == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperTestEncoderWindow_2_Failed,
					   HfcResponse::RESPONSESTYLE_OK);
		}

		if (status->isEncoderWindow_3() == true)
		{
			addMessage(ErrOperationMode,
					   ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperTestEncoderWindow_3_Failed,
					   HfcResponse::RESPONSESTYLE_OK);
		}

		//	Prepack: Costco doesn't want to have to toggle the print button for a Fixed weight or By count PLU.
	    //	So, we are allowing the print key to be accepted regardless of the currentMode on non Random weight PLUs.
		if ((status->isProductRequiredToWrap() == true) && mySession->isInOperateMode())		// HA-65: Only in Operate mode
		{
			if (!checkPrinterStatuses())
			{
				// cant have a printer status error when we pop this error
				ProductManager* currentProductManager = mySession->getProductManager();
				if (!currentProductManager)
				{
					addMessage(ErrOperationMode,
						ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::ProductRequiredToWrap,
						HfcResponse::RESPONSESTYLE_OK);
					return;
				}

				Product* currentProduct = currentProductManager->getCurrentProduct();
				if (!currentProduct)
				{

					addMessage(ErrOperationMode,
						ErrOperationMode* DEVICE_ERROR_OFFSET + OperationModeStatus::ProductRequiredToWrap,
						HfcResponse::RESPONSESTYLE_OK);
					return;
				}

				if ((Product::PRODUCTTYPE_FIXEDWEIGHT == currentProduct->getProductType()) || (Product::PRODUCTTYPE_BYCOUNT == currentProduct->getProductType()))
				{
					addMessage(ErrOperationMode,
						ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::ProductRequiredToWrap,
						HfcResponse::RESPONSESTYLE_OK);
				}
            }
		}

		// This is a dialogue box!
		if (status->isNewWrapperError() == true)
		{
			// if there was an old error message, remove it!
			if (status->getPrevWrapperError() != WrapperErrorInfo::NoErrorSource)
			{
				removeMessage(ErrWrapper, 
							  ErrWrapper * DEVICE_ERROR_OFFSET + status->getPrevWrapperError(),
							  HfcResponse::RESPONSESTYLE_OK);
			}

			if (status->getWrapperError() != WrapperErrorInfo::NoErrorSource)
			{

				addMessage(ErrWrapper, 
						   ErrWrapper * DEVICE_ERROR_OFFSET + status->getWrapperError(),
						   HfcResponse::RESPONSESTYLE_OK);
			}
		}
		else
		{
			//	I don't know why we are doing this here. This may erase all error messages that the
			//	Access wrapper is indicating. So, to avoid that and hopefully not affect the UWS,
			//	we will check to make sure there are no Access wrapper errors.
			if (status->isNewEvolWrapperError() == false)
			{
				if ( checkPrinterStatuses() )
					Debug::trace("ResponseMessageManager.EvolWrapper", "manageStatus(HfcOperationModeStatus) - checkPrinterStatuses == true.   Pop the error!!");

				removeAllMessages(ErrWrapper);
			}
		}

		//	This is a dialogue box!
		//	isNewEvolWrapperError() seems to remain true, even after it is no longer a new error.
		//	So, we will check that the current and previous errors are different so we don't remove the message and put it back up.
		if ((status->isNewEvolWrapperError() == true) && (status->getEvolWrapperError() != status->getPrevEvolWrapperError()))
		{
			// if there was an old error message, remove it!
			if (status->getPrevEvolWrapperError() != EvolWrapperErrorInfo::NoWrapperError)
			{
				Debug::trace("ResponseMessageManager.EvolWrapper", "Remove PrevEvolWrapperError Message: " + HobString(Integer::toString(ErrWrapper * DEVICE_ERROR_OFFSET + status->getPrevEvolWrapperError())));
				removeMessage(ErrWrapper, 
							  ErrWrapper * DEVICE_ERROR_OFFSET + status->getPrevEvolWrapperError(),
							  HfcResponse::RESPONSESTYLE_OK);
				wrapperError = false;
			}

			// if there is a new error message, post it!
			if (status->getEvolWrapperError() != EvolWrapperErrorInfo::NoWrapperError)
			{
				Debug::trace("ResponseMessageManager.EvolWrapper", "Add EvolWrapperError Message: " + HobString(Integer::toString(ErrWrapper * DEVICE_ERROR_OFFSET + status->getEvolWrapperError())));
				HobString msgDebug = "ResponseMessageManager:  New Evol Wrapper error!  ErrorSource = ";
				msgDebug.append(Integer::toString(status->getEvolWrapperErrorSource()));
				msgDebug.append("  Error = ").append(Integer::toString(status->getEvolWrapperError()));
				Debug::trace("ResponseMessageManager.EvolWrapper", msgDebug);
				//SOF-4476
				if( ( FpmManager::getInstance()->isWrapperTwoPrinter() ) && ( ( status->getEvolWrapperError() 
																			  == EvolWrapperErrorInfo::RightCanopyInterlock ) ||
																			  (  status->getEvolWrapperError() 
																			  == EvolWrapperErrorInfo::LeftCanopyInterlock ) ) )																			  
				{
					addMessage(ErrWrapper, 9745, HfcResponse::RESPONSESTYLE_OK);
					canopyError = true;
				} else {

				    addMessage(ErrWrapper, 
						   ErrWrapper * DEVICE_ERROR_OFFSET + status->getEvolWrapperError(),
						   HfcResponse::RESPONSESTYLE_OK);
				}				
				wrapperError = true;
			} else {
				//SOF-4616
				if( canopyError ) {
					removeMessage(ErrWrapper, 9745, HfcResponse::RESPONSESTYLE_OK);
					canopyError = false;
				}
			}
		}
	}
}

bool ResponseMessageManager::addMessage(ErrorEvent errorDevice, int errorCode, short responseStyle, const HobList *msgData, bool allowDuplicates)
{
	// Returns true if the message was added to the queue (not necessarily displayed if a message was already in the queue).
	// Returns false if the message wasn't added to the queue (it already existed in the queue).

	/* Determine if the errorcode has already been queued.
	 * If it has and we're turning the error ON, just ignore the new error, the message is
	 * (or will be) displayed.
	 */

	if ( Configuration::getInstance()->isSelfServiceScale() )
	{
		if ( isSelfServeOutOfServiceMessage(errorCode) && ( SelfServOption_PrintToScreen != Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors) ) )
			responseStyle = HfcResponse::RESPONSESTYLE_NOINPUT;
		else if ( isSelfServeSevereWarningMessage(errorCode) )
			responseStyle = HfcResponse::RESPONSESTYLE_OK;

		errorCode = translateSelfServeOutOfServiceMessage(errorCode);
	}

	if (Debug::isTracing("ResponseMessageManager.add"))
	{
		HobString ed = HobString(Integer::toString(errorDevice));
		HobString ec = HobString(Integer::toString(errorCode));
		HobString rs = HobString(Integer::toString(responseStyle));
		Debug::trace("ResponseMessageManager.add", HobString("addMessage  errorDevice=") + ed + 
															HobString("  errorCode=") + ec + HobString("  responseStyle=") + rs +
															HobString("  allowDuplicates=") + Bool::toString(allowDuplicates));
	}

	HobString listData = "";
	if (msgData)
		listData = msgData->serialize(BrowserFormat);
	Response r(errorDevice, errorCode, responseStyle, listData);

	bool ret = false;
	if (responseStyle == HfcResponse::RESPONSESTYLE_STATUS)
		ret = addMessageToQueue(r, &statusLineResponses, allowDuplicates);
	else
		ret = addMessageToQueue(r, &responses, allowDuplicates);

	if (Debug::isTracing("ResponseMessageManager.show"))
		show();
	return ret;
}

bool ResponseMessageManager::addMessageToQueue(Response r, responseContainer_type *queue, bool allowDuplicates)
{
	Debug::trace("ResponseMessageManager.add", HobString("addMessageToQueue  allowDuplicates=") + Bool::toString(allowDuplicates));

	if (queue->size() > 0 && !allowDuplicates)
	{
		// Look to see if the error/message has been queued in the queue.
		responseContainer_type::iterator iter = find(queue->begin(), queue->end(), r);
		if (iter != queue->end())
		{
			// This error is already in the list. Don't add another one.
			Debug::trace("ResponseMessageManager", "addMessageToQueue  ignoring message, already in the queue");
			return false;
		}
	}

	// SOF-3475
	// check we are not spamming the same messages
	const ErrorDeviceCode edc( r.getErrorDevice(), r.getErrorCode() );

	const PreventSpamMessageMap::iterator waitTime( messageWaitTimes.find( edc ) );
	const ResponseTimestampMap::iterator lastResponseTime( responseTimes.find( edc ) );

	if (( waitTime != messageWaitTimes.end() ) && ( lastResponseTime != responseTimes.end() ))
	{
		Debug::trace("ResponseMessageManager", "addMessageToQueue  checking message wait time...");

		if ( (ClockType::now() - lastResponseTime->second ) <= waitTime->second )
		{
			Debug::trace("ResponseMessageManager", "addMessageToQueue  suppressing message due to spam timeouts");
			return false;
		}
	}

	Debug::trace("ResponseMessageManager", "addMessageToQueue  adding " + Long::toString( r.getErrorDevice() ) + " " + Long::toString( r.getErrorCode() ));


	// 2nd Label Applier:
	// On the printer plate positioning screen in the UI, the take label dialogs are being suppressed and remain in the queue, this is causing
	// the 9693 error (slide adjustment timeout) to be suppressed. Gonna go ahead and clear the queue if we get a 9693 error since it is just full of
	// take label errors.
	if( r.getErrorCode() == 9693 )
	{
		queue->clear();
	}

	// queue is a std::list
	queue->push_back(r);

	Debug::trace("ResponseMessageManager", HobString("queue->size() = ") + HobString(Integer::toString(queue->size())));

	if ( ( queue->size() == 1 && displayingDialogs ) /*|| r.getErrorCode() == 9693*/ )
		sendMessage(r.getErrorDevice(), r.getErrorCode(), r.getResponseStyle(), r.getListSerializedData());
	else
		Debug::trace("ResponseMessageManager", "addMessageToQueue  message added, but not displayed; is not the first in the queue or not displayingDialogs");

	return true;
}


bool ResponseMessageManager::removeAllMessages(ErrorEvent errorDevice, short responseStyle, bool keepSelfServeOutOfServiceMessages)
{
	// Look to see if the error/message has been queued in the queue of messages passed in.
	// The key to finding the appropriate responses managed in the queue is the operator== method in
	// the internal Response class found in ResponseMessageManager.h

	bool isTracing = Debug::isTracing("ResponseMessageManager.remove");

	int removed = 0;
	HobString style(Integer::toString(responseStyle));
	if (isTracing)
		Debug::trace("ResponseMessageManager.remove", HobString("removeAllMessages for errorDevice:") + HobString(Integer::toString(errorDevice)) +
																	HobString("  style:") + style);

	if (responseStyle == -1 || responseStyle == HfcResponse::RESPONSESTYLE_STATUS)
	{
		responseContainer_type *queue = &statusLineResponses;
		Response r(errorDevice, -1, -1);
		bool msgRemoved = true;
		while (msgRemoved)
		{
			msgRemoved = false;
			responseContainer_type::reverse_iterator iter = queue->rbegin();
			while (iter != queue->rend())
			{
				removed++;
				if (r == *iter)
				{
					msgRemoved = removeMessage(iter->getErrorDevice(), iter->getErrorCode(), iter->getResponseStyle());
					break;
				}
				++iter;
			}
		}
		if (removed == 0)
		{
			if (isTracing)
				Debug::trace("ResponseMessageManager.remove", "removeMessage  No messages to remove from STATUS LINE.");
		}
		else
			removed = 0;
	}

	if (responseStyle == -1 || responseStyle != HfcResponse::RESPONSESTYLE_STATUS)
	{
		responseContainer_type *queue = &responses;
		Response r(errorDevice, -1, -1);
		bool msgRemoved = true;
		while (msgRemoved)
		{
			msgRemoved = false;
			responseContainer_type::reverse_iterator iter = queue->rbegin();
			while (iter != queue->rend())
			{
				if (r == *iter)
				{
					if (keepSelfServeOutOfServiceMessages &&
					    Configuration::getInstance()->isSelfServiceScale() &&
						 isSelfServeOutOfServiceMessage(iter->getErrorCode()))
						;
					else if ( errorDevice == ErrUnknown || errorDevice == iter->getErrorDevice() )
						msgRemoved = removeMessage(iter->getErrorDevice(), iter->getErrorCode(), iter->getResponseStyle());
					break;
				}
				++iter;
			}
		}
		if (removed == 0)
		{
			if (isTracing)
				Debug::trace("ResponseMessageManager.remove", "removeMessage  No messages to remove from OVERLAY WINDOWS.");
		}
		else
			removed = 0;
	}

	if (Debug::isTracing("ResponseMessageManager.show"))
		show();
	return true;
}

bool ResponseMessageManager::removeMessage(ErrorEvent errorDevice, int errorCode, short responseStyle)
{
	// Returns true if the message was removed from the queue,
	// Retruns false if the queue is empty or the message not found in the queue.

	/* If turning the error off,
	 *    If the message is displayed, send a message to the browser to remove the message.
	 *    else if the message is just queued, then just delete it from the queue
	 *            no need to send a message to the browser since it hasn't yet been displayed.
	 */

	bool isTracing = Debug::isTracing("ResponseMessageManager.remove");
	if (isTracing)
	{
		HobString ed = HobString(Integer::toString(errorDevice));
		HobString ec = HobString(Integer::toString(errorCode));
		HobString rs = HobString(Integer::toString(responseStyle));
		if (isTracing)
			Debug::trace("ResponseMessageManager.remove", HobString("removeMessage  errorDevice=") + ed + 
																		HobString("  errorCode=") + ec + HobString("  responseStyle=") + rs);
	}

	// For optimization, don't create a Response object if there is nothing in the queue to remove.
	responseContainer_type *queue = (responseStyle == HfcResponse::RESPONSESTYLE_STATUS) ? &statusLineResponses : &responses;
	if (queue->size() == 0)
	{
		if (isTracing)
			Debug::trace("ResponseMessageManager.remove", "removeMessage  ignoring message, nothing in the queue to remove");
		return false;
	}

	Response r(errorDevice, errorCode, responseStyle);
	bool ret = removeMessageFromQueue(r, queue);

	if ( Configuration::getInstance()->isSelfServiceScale() )
	{
		// If we're a self-service scale, the error dialog may have been displayed before going into self-serve mode.
		// So just in case, delete the non-self-serve version of the message, and the self-serve-out-of-service versions.
		int ssErrorCode = translateSelfServeOutOfServiceMessage(errorCode);

		if (ssErrorCode != errorCode)
		{
			if ( isSelfServeOutOfServiceMessage(errorCode) )
				responseStyle = HfcResponse::RESPONSESTYLE_NOINPUT;
			else if ( isSelfServeSevereWarningMessage(errorCode) )
				responseStyle = HfcResponse::RESPONSESTYLE_OK;

			bool isTracing = Debug::isTracing("ResponseMessageManager.remove");
			if (isTracing)
			{
				HobString ed = HobString(Integer::toString(errorDevice));
				HobString ec = HobString(Integer::toString(errorCode));
				HobString rs = HobString(Integer::toString(responseStyle));
				if (isTracing)
					Debug::trace("ResponseMessageManager.remove", HobString("removeMessage  errorDevice=") + ed + 
																				HobString("  errorCode=") + ec + HobString("  responseStyle=") + rs);
			}

			Response r(errorDevice, ssErrorCode, responseStyle);
			bool ret = removeMessageFromQueue(r, queue);
		}
	}

	if (Debug::isTracing("ResponseMessageManager.show"))
		show();
	return ret;
}

bool ResponseMessageManager::removeMessageFromQueue(Response r, responseContainer_type *queue)
{
	responseContainer_type::iterator iter;

	if ( r.getErrorCode() == -1 )
	{
		// Removing all messages for the specified ErrorDevice
		Debug::trace("ResponseMessageManager", string("removeMessageFromQueue  removing all messages for device: ") + Integer::toString(r.getErrorDevice()) );
		iter = queue->begin();
	}
	else
	{
		// Look to see if the error/message has been queued in the queue of messages passed in.
		iter = find(queue->begin(), queue->end(), r);
	}

	if ( iter == queue->end() )
	{
		Debug::trace("ResponseMessageManager", "removeMessageFromQueue  ignoring message, not in the queue to remove");
		return false;
	}

	// Found the error/message in the list.

	bool removingFirst = false;

	if ( r.getErrorCode() == -1 )
	{
		// Removing all messages for the specified ErrorDevice
		bool firstTime = true;
		while ( iter != queue->end() )
		{
			if ( iter->getErrorDevice() == r.getErrorDevice() )
			{
				if ( firstTime )
				{
					Debug::trace("ResponseMessageManager", "removeMessageFromQueue  removing the message being displayed");
					removingFirst = true;
					sendMessage(ErrRemoveAnyModalDialog, iter->getErrorCode(), iter->getResponseStyle());

					// SOF-3475
					// record when message was removed from display
					responseTimes[ ErrorDeviceCode(iter->getErrorDevice(), iter->getErrorCode()) ] = ClockType::now();
				}

				queue->erase(iter);
				iter = queue->begin();
			}
			else
				iter++;
			firstTime = false;
		}
	}
	else
	{
		// If it is the first one in the list, then send down a blank to clear the message.
		if (iter == queue->begin())
		{
			Debug::trace("ResponseMessageManager", "removeMessageFromQueue  removing the message being displayed");
			removingFirst = true;
			sendMessage(ErrRemoveAnyModalDialog, iter->getErrorCode(), iter->getResponseStyle());

			// SOF-3475
			// record when message was removed from display
			responseTimes[ ErrorDeviceCode(iter->getErrorDevice(), iter->getErrorCode()) ] = ClockType::now();
		}

		queue->erase(iter);
	}

	// If there are other status line messages queued, send the next one down.
	if (removingFirst && queue->size() > 0 && displayingDialogs)
	{
		Debug::trace("ResponseMessageManager", "removeMessageFromQueue  sending the next message to the browser");

		Response r = queue->front();
		sendMessage(r.getErrorDevice(), r.getErrorCode(), r.getResponseStyle(), r.getListSerializedData());
	}

	return true;
}

void ResponseMessageManager::sendMessage(ErrorEvent errorDevice, int errorCode, short responseStyle, const HobString &msgData)
{
	if (mySession)
	{
		if (errorDevice == ErrOperatorDev && (errorCode == OperatorErrRenumberOperator || errorCode == OperatorErrEnterIDorRFID))
			mySession->popupVirtualKeyboard(0);

		SessionMessage sMsg(Error, errorDevice);
		sMsg.setFormatType(BrowserFormat);

		HfcResponse m(errorCode, responseStyle, msgData);
		sMsg.setHfcObject(&m);
		mySession->send(&sMsg, Messenger::DEST_UI);

		if (shouldBeep(errorDevice, errorCode, responseStyle))
			ServiceSpeaker::badBeep(2);
		//Sleep(100);
	}
}

void ResponseMessageManager::resendMessage(short responseStyle)
{
	if (mySession)
	{
		responseContainer_type::iterator iter, endIter;
		if (responseStyle == HfcResponse::RESPONSESTYLE_STATUS)
		{
			iter = statusLineResponses.begin();
			endIter = statusLineResponses.end();

			if (iter != endIter)
			{
				Debug::trace("ResponseMessageManager", "Resending Message for " + HobString(Integer::toString(responseStyle)));
				sendMessage(iter->getErrorDevice(), iter->getErrorCode(), responseStyle, iter->getListSerializedData());
			}
		}
		else // don't want to resend any messages displayed in a dialog.
		     // the browser will display two dialog boxes, and continue to try to
		     // bring them both to the front, making them flash
		{
		}
	}
}


bool ResponseMessageManager::isSelfServeOutOfServiceMessage(int errorCode)
{
	bool outOfService = false;
	if (Configuration::getInstance()->isSelfServiceScale() && mySession && mySession->isInOperateMode() && !mySession->isAnyAccessKeyPresent())
	{
		switch (errorCode)
		{
			case WeigherErrCommLoss:
			//case WeigherErrNegativeGrossWeight:
			case WeigherErrOverGrossWeightLimit:
			case WeigherErrZeroOutOfMaintZoneStatus:
			case WeigherErrNotAttached:
			case PrinterErrCommLoss:
			case PrinterErrManagerDeleted:
			case PrinterErrHeadUp:
			case PrinterErrTakeLabel:
			case PrinterErrTakeLabelBeep:
			case PrinterErrLabelJammed:
			//case PrinterErrLabelMissing:
			case PrinterErrOutOfMedia:
			case PrinterErrCassetteMissing:
			case PrinterErrHardwareError:
			case PrinterErrThermalOverload:
			case PrinterErrThermalShutdown:
				outOfService = true;
				break;

			default:
				break;
		}
	}
	return outOfService;
}

bool ResponseMessageManager::isSelfServeSevereWarningMessage(int errorCode)
{
      bool outOfService = false;
      if (Configuration::getInstance()->isSelfServiceScale() && !mySession->isAnyAccessKeyPresent()
		  && (!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && (SelfServOption_PrintToScreen != Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors))))
	  {
		  Debug::trace("ResponseMessageManager.TestScreenLabel", "ResponseMessageManager isSelfServeSevereWarningMessage()");

            switch (errorCode)
            {
                  case ProductErrLabelType:
                        outOfService = true;
                        break;

                  default:
                        break;
            }
      }
      return outOfService;
}

int ResponseMessageManager::translateSelfServeOutOfServiceMessage(int errorCode)
{
	int newErrorCode = errorCode;
	if (isSelfServeOutOfServiceMessage(errorCode))
		newErrorCode += SelfServiceErrorOffset;
	return newErrorCode;
}

bool ResponseMessageManager::isPrinterMessage(int errorCode)
{
	bool printerMessage = false;

	if ((PrinterErrUnknownError <= errorCode) && (errorCode < PrinterErrUnknownError + SelfServiceErrorOffset))
	{
		printerMessage = true;
	}

	return printerMessage;
}

int ResponseMessageManager::translatePrinterMessage(int errorCode, PrinterType prType)
{
	int newErrorCode = errorCode;
	if (isPrinterMessage(errorCode))
		newErrorCode += prType * PrinterErrorOffset;
	return newErrorCode;
}

void ResponseMessageManager::setDisplayingDialogs(bool displaying)
{
	if (displayingDialogs && !displaying)
	{
		// Stop displaying dialogs.  If one is currently displayed, remove it from the screen, but leave it in the
		// queue to be displayed when the dialogs start up again.
		Debug::trace("ResponseMessageManager.dialog", HobString("Stopping the dialogs"));

		displayingDialogs = displaying;

		// Look to see if the error/message has been queued in the queue of messages passed in.
		responseContainer_type::iterator iter = responses.begin();
		if (iter != responses.end())
		{
			Debug::trace("ResponseMessageManager.dialog", HobString("first response being removed from browser - queue size: ") + HobString(Integer::toString((int)responses.size())));
			sendMessage(ErrRemoveAnyModalDialog, iter->getErrorCode(), iter->getResponseStyle());
		}

		iter = statusLineResponses.begin();
		if (iter != statusLineResponses.end())
		{
			Debug::trace("ResponseMessageManager.dialog", HobString("first status line message being removed from browser - queue size: ") + HobString(Integer::toString((int)responses.size())));
			sendMessage(ErrRemoveAnyModalDialog, iter->getErrorCode(), iter->getResponseStyle());
		}
	}
	else if (!displayingDialogs && displaying)
	{
		// Start the dialogs back up again.  Send the first in the queue to the browser.
		Debug::trace("ResponseMessageManager.dialog", HobString("Start the dialogs back up again"));

		displayingDialogs = displaying;

		responseContainer_type::iterator iter = responses.begin();
		if (iter != responses.end())
		{
			Debug::trace("ResponseMessageManager.dialog", HobString("first response being sent to browser - queue size: ") + HobString(Integer::toString((int)responses.size())));
			sendMessage(iter->getErrorDevice(), iter->getErrorCode(), iter->getResponseStyle(), iter->getListSerializedData());
		}

		iter = statusLineResponses.begin();
		if (iter != statusLineResponses.end())
		{
			Debug::trace("ResponseMessageManager.dialog", HobString("first status line response being sent to browser - queue size: ") + HobString(Integer::toString((int)statusLineResponses.size())));
			sendMessage(iter->getErrorDevice(), iter->getErrorCode(), iter->getResponseStyle(), iter->getListSerializedData());
		}
	}
}


void ResponseMessageManager::show()
{
	if (statusLineResponses.size() == 0)
		Debug::trace("ResponseMessageManager.show", "No messages in status line responses queue.");
	else
	{
		Debug::trace("ResponseMessageManager.show", HobString("status line responses queue size: ") + HobString(Integer::toString((int)statusLineResponses.size())));
		showQueue(&statusLineResponses);
	}

	if (responses.size() == 0)
		Debug::trace("ResponseMessageManager.show", "No messages in overlay windows responses queue.");
	else
	{
		Debug::trace("ResponseMessageManager.show", HobString("overlay windows responses queue size: ") + HobString(Integer::toString((int)responses.size())));
		showQueue(&responses);
	}
}

void ResponseMessageManager::showQueue(responseContainer_type *queue)
{
	responseContainer_type::iterator iter = queue->begin();
	while (iter != queue->end())
	{
		Debug::trace("ResponseMessageManager.show", 
								HobString("  errordevice:") + HobString(Integer::toString((int)iter->getErrorDevice())) +
								HobString("  errorcode:") + HobString(Integer::toString(iter->getErrorCode())) +
								HobString("  errorstyle:") + HobString(Integer::toString(iter->getResponseStyle())));
		++iter;
	}
}

bool ResponseMessageManager::shouldBeep(ErrorEvent errorDevice, int errorCode, short responseStyle) const
{
	if (errorDevice == ErrRemoveAnyModalDialog)
		return false;

	if (responseStyle == HfcResponse::RESPONSESTYLE_STATUS ||
		 responseStyle == HfcResponse::RESPONSESTYLE_NODIALOG ||
		 responseStyle == HfcResponse::RESPONSESTYLE_NEWPRODUCT ||
		 responseStyle == HfcResponse::RESPONSESTYLE_NEWCLASS)
		return false;

	if (mySession)
	{
		if (!mySession->isPrimarySession() || !mySession->isInOperateMode())
			return false;
	}

	bool answer = true;
	switch (errorDevice)
	{
		case ErrWeigher:
		{
			switch (errorCode)
			{
				case WeigherErrNegativeGrossWeight:
				case WeigherErrScaleWarmup:
				case WeigherErrRezeroing:
				case PrinterErrLabelSizing:
					answer = false;
					break;
				default:
					break;
			}
			break;
		}
		case ErrPrinter:
			if (errorCode == PrinterErrLabelSizing)
				answer = false;
			break;
//		case :
//			break;
		default:
			break;
	}
	return answer;
}


bool ResponseMessageManager::displayingResponse() const
{
	return (responses.size()) ? true : false;
}
bool ResponseMessageManager::displayingStatusLineMessage() const
{
	return (statusLineResponses.size()) ? true : false;
}
