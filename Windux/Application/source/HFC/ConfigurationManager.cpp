/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "ConfigurationManager.h"
#include "SessionMessage.h"
#include "SessionManager.h"
#include "Session.h"
#include "HfcObject.h"
#include "GenericHfcObject.h"
#include "WriteSingleField.h"
#include "HobTokenizerCSV.h"

#include "SynchronizationManagerProxy.h"
#include "SynchronizationChange.h"
#include "WeigherEvent.h"
#include "Weigher.h"
#include "WeigherInfo.h"
#include "WeigherMode.h"

#include "WeigherSystemRequestInquire.h"
#include "HfcWeigherInquire.h"
#include "HfcWeigherSystemInquire.h"
#include "WeigherRequestInquire.h"
#include "WeigherSystemRequestValueMax.h"
#include "HfcWeigherValueMaxFitParams.h"

#include "HfcWeigherStatus.h"
#include "WeigherRezero.h"

#include "HfcWeigherDefaultsComplete.h"

#include "PrinterSystemRequestInquire.h"
#include "HfcPrinterInquire.h"
#include "HfcPrinterSystemInquire.h"

#include "HfcLabelApplierInquire.h"

#include "ProductTextManager.h"
#include "ProductText.h"
#include "Product.h"
#include "MarqueeUpdate.h"

#include "HfcWrapperErrorTotals.h"
#include "HfcWrapperInquire.h"
#include "HfcWrapperPackageTotals.h"
#include "HfcWrapperPackCodeInfo.h"
#include "HfcWrapperSettings.h"
#include "HfcWrapperStatus.h"
#include "HfcWrapperControl.h"

//#include "HfcEvolWrapperConfig.h"
#include "HfcEvolWrapperInquire.h"
#include "HfcEvolWrapperSettings.h"
#include "HfcEvolWrapperControl.h"
#include "ProductDefault.h"

#include "CfgScale.h"
#include "CfgFrequentShopper.h"
#include "CfgOperatorAccess.h"
#include "CfgPrintedFields.h"
#include "CfgStoreInfo.h"
#include "CfgAll.h"
#include "CfgAccumulateItems.h"
#include "CfgSupportedLabelLengths.h"

#include "DBAccess.h"
#include "DBException.h"

#include "SoftkeyDevEvent.h"
#include "BackupRestoreEvent.h"

#include "CMessageCenter.h"

#include "../app/Messages/ChangePrimaryScaleMessage.h"
#include "../app/Messages/ChangeSynchronizationTypeMessage.h"

#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "Long.h"

#include "ResponseMessageManager.h"

#include "HFCManagerContainer.h"

#include "PythonScripter.h"

#include "filesys/directory.h"
#include "filesys/file.h"

#include "MessageLogger.h"
#include "PrinterStockMetrics.h"

#include "TimerManager.h"
//Timer

#include "WeigherCat3DateTime.h"

#include "HfcPrinterUseContinuousStock.h"
#include "HfcPrinterRequestLabelSizing.h"

#include <HobartCustomerHook.h>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ConfigurationManager::dataType = 0;
std::string ConfigurationManager::dataTypeName("ConfigurationManager"); 
#endif


using namespace std;


ConfigurationManager::ConfigurationManager()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

ConfigurationManager::ConfigurationManager(hfc::HFCManagerContainer *container)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	dbAccess = container;
	mgrContainer_ = container;
}

ConfigurationManager::~ConfigurationManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

//	Debug::trace("ConfigurationManager", "*************  In destructor");

	if (commSessionSavedConfig)
	{
		commSessionSavedConfig = false;
		dispatchConfigurationSavedNow();
	}

	if (currentWeigherInfo)
		delete currentWeigherInfo;
	if (currentWeigherConfig)
		delete currentWeigherConfig;
	if (currentLabelApplierConfig)
		delete currentLabelApplierConfig;
//	if (currentPrinterConfig)
//		delete currentPrinterConfig;

	if (currentPrinterConfig.size() > 0)
	{
		vector<HfcPrinterConfig *>::iterator iter = currentPrinterConfig.begin();

		while (iter != currentPrinterConfig.end())
		{
			if (*iter)
				delete *iter;
			++iter;
		}
		currentPrinterConfig.clear();
	}


	if (currentWrapperConfig)
		delete currentWrapperConfig;
	if (currentWrapperControl)
		delete currentWrapperControl;

	if (currentEvolWrapperConfig)
		delete currentEvolWrapperConfig;
	if (currentEvolWrapperControl)
		delete currentEvolWrapperControl;

	if (currentPrinterInfo.size() > 0)
	{
		vector<PrinterInfo *>::iterator iter = currentPrinterInfo.begin();

		while (iter != currentPrinterInfo.end())
		{
			if (*iter)
				delete *iter;
			++iter;
		}
		currentPrinterInfo.clear();
	}

	if (initialServiceWeigherConfig)
		delete initialServiceWeigherConfig;

	if (pendingSynchronizationChange)
		delete pendingSynchronizationChange;
}


void ConfigurationManager::init()
{
	mySession = 0;

	currentWeigherInfo = 0;
	currentWeigherConfig = 0;
	currentLabelApplierConfig = 0;
//	currentPrinterConfig = 0;
	
	currentPrinterConfig.insert(currentPrinterConfig.begin(), MaxPrinterTypes, NULL);
	
	currentWrapperConfig = 0;
	currentWrapperControl = 0;

	currentEvolWrapperConfig = 0;
	currentEvolWrapperControl = 0;

	currentPrinterInfo.insert(currentPrinterInfo.begin(), MaxPrinterTypes, NULL);

	enteringServiceMode = false;
	leavingServiceMode = false;
	savedWeigherConfig = false;
	commSessionSavedConfig = false;
	initialServiceWeigherConfig = (HfcWeigherConfig *)NULL;

	pendingSynchronizationChange = 0;
}


DBSession *ConfigurationManager::getMySession()
{
	return mySession;
}
void ConfigurationManager::setMySession(DBSession *session)
{
	mySession = session;
}

CfgAll* ConfigurationManager::getFirstRecord()
{
	return new CfgAll();
}

void ConfigurationManager::handleMessage(SessionMessage *msg)
{
	switch(msg->getDeviceType())
	{
		case Weigher:
		{
			switch((WgEvent) msg->getEventType())
			{
				case WgReqInquire:	//	To RTC
					handleWeigherRequestInquire(msg);
					break;

				case WgInquire:		//	To UI
					handleWeigherInquire(msg);
					break;

				case WgReqValueMax:	//	To RTC
					handleWeigherRequestValueMax(msg);
					break;

				case WgValueMaxFitParams:	//	To UI
					handleWeigherValueMax(msg);
					break;

				case WgConfig:		//  To RTC
					handleWeigherConfig(msg);
					break;

				case WgRestoreWeigherSettings:
					if ( Configuration::getInstance()->isCAT3AuditTrailLicensed() )
						handleWeigherConfig(msg);
					break;

				case WgReqInfo:
					handleWeigherRequestInfo(msg);
					break;

				case WgSaveInfo:
					handleWeigherSaveInfo(msg);
					break;

				case WgDoneInfo:
					handleWeigherDoneInfo(msg);
					break;

				case WgAttach:
					handleWeigherAttach(msg);
					break;

				// SOF-1667
				case WgResetFactoryDefaults:			// To RTC
					handleWeigherResetFactoryDefaults(msg);
					break;

					// SOF-1667
				case WgResetFactoryDefaultsComplete:	// To UI
					handleWeigherResetFactoryDefaultsComplete(msg);
					break;

				default:
					break;
			}
			break;
		}

		case WeigherSystem:
		{
			switch ((SysEvent)msg->getEventType())
			{
				case SysInquire:
					handleWeigherInfo(msg);
					break;
				default:
					break;
			}
			break;
		}

		case LabelApplier:
		{
			switch ((LaEvent)msg->getEventType())
			{
				case LaReqInquire:
					handleLabelApplierRequestInquire(msg);
					break;

				case LaInquire:
					handleLabelApplierInquire(msg);
					break;

				case LaConfig:
					handleLabelApplierConfig(msg);
					break;

				default:
					break;
			}
			break;
		}

		case Printer:
		{
			switch ((PrEvent)msg->getEventType())
			{
				case PrReqInquire:
					handlePrinterRequestInquire(msg);
					break;

				case PrInquire:
					handlePrinterInquire(msg);
					break;

				case PrConfig:
				case PrRestorePrinterSettings:
					handlePrinterConfig(msg);
					break;

				case PrReqInfo:
					handlePrinterRequestInfo(msg);
					break;

				case PrSaveInfo:
					handlePrinterSaveInfo(msg);
					break;

				default:
					break;
			}
			break;
		}

		case PrinterSystem:
		{
			switch ((SysEvent)msg->getEventType())
			{
				case SysInquire:
					handlePrinterInfo(msg);
					break;
				default:
					break;
			}
			break;
		}

		case Wrapper:
		{
			Debug::trace("ConfigurationManager", "Received a Wrapper message.");
			switch((WrEvent) msg->getEventType())
			{
				case WrInquire:
					handleWrapperInquire(msg);
					break;

				case WrConfig:
					handleWrapperConfig(msg);
					break;

				case WrResetErrorTotals:
					handleWrapperResetErrorTotals();
					break;

				case WrResetPackageTotals:
					handleWrapperResetPackageTotals();
					break;

				case WrResetFactoryDefaults:
					handleWrapperResetConfig();
					break;

				case WrWritePackCode:
					handleWrapperWritePackCode(msg);
					break;

				case WrSavePackCode:
					handleWrapperSavePackCode(msg);
					break;

				case WrEvolSettings:
					handleEvolWrapperSettings(msg);
					break;

				case WrSettings:
					handleWrapperSettings(msg);
					break;

				case WrStatus:
					handleWrapperStatus(msg);
					break;

				case WrControl:
					handleWrapperControl(msg);
					break;

				case WrProductLoaded:
					handleProductReloadWrapperControls(msg);
					break;

				default:
					Debug::trace("ConfigurationManager", "Invalid Wrapper message!!!");
					break;
			}
			break;
		}

		case ConfigurationDev:
		{
			Debug::trace("ConfigurationManager", "Received a ConfigurationDev message.");
			switch ((ConfigurationEvent) msg->getEventType())
			{
				case CfgBatchApplyDeleteAllFiles:
				case CfgDeleteAllFiles:
					Debug::trace("ConfigurationManager", "handling SnDeleteAllFiles or CfgBatchApplyDeleteAllFiles Event");
					handleDeleteAllFiles(msg->getEventType());
					break;

				case CfgSaved:
					// Some Session has made changes to, and saved, the configuration in the database.
					// Reread it and sent it to the UI.
					handleReadAllConfigMsgs();
					break;

				default:
					handleConfigurationDevMsg(msg);
			}
			break;
		}

		default:
			break;
	}
}

WeigherInfo *ConfigurationManager::getCurrentWeigherInfo() const
{
	return currentWeigherInfo;
}
void ConfigurationManager::setCurrentWeigherInfo(WeigherInfo *weigherInfo)
{
	//	Save the currentWeigherInfo values, but release any current data first.
	if (currentWeigherInfo)
		delete currentWeigherInfo;
	currentWeigherInfo = weigherInfo;
}

HfcWeigherConfig *ConfigurationManager::getCurrentWeigherConfig() const
{
	return currentWeigherConfig;
}
void ConfigurationManager::setCurrentWeigherConfig(HfcWeigherConfig *weigherConfig)
{
	//	Save the currentWeigherConfig values, but release any current data first.
	if (currentWeigherConfig)
		delete currentWeigherConfig;
	currentWeigherConfig = weigherConfig;
}

HfcLabelApplierConfig *ConfigurationManager::getCurrentLabelApplierConfig() const
{
	return currentLabelApplierConfig;
}
void ConfigurationManager::setCurrentLabelApplierConfig(HfcLabelApplierConfig *labelApplierConfig)
{
	//	Save the current values, but release any current data first.
	if (currentLabelApplierConfig)
		delete currentLabelApplierConfig;
	currentLabelApplierConfig = labelApplierConfig;
}

HfcPrinterConfig *ConfigurationManager::getCurrentPrinterConfig(PrinterType printerType) const
{
	if (printerType < MaxPrinterTypes)
	{
		HfcPrinterConfig *pPrConfig = currentPrinterConfig[printerType];
		Debug::trace("ConfigurationManager.PrinterConfig", "getCurrentPrinterConfig(" + Integer::toString(printerType) + "):  Returning " + util_string::ptrtostring(pPrConfig));
		return pPrConfig;
	}
	else
	{
		Debug::trace("ConfigurationManager.PrinterConfig", "getCurrentPrinterConfig(" + Integer::toString(printerType) + "):  Invalid Printer number!");
		return 0;
	}
}
void ConfigurationManager::setCurrentPrinterConfig(PrinterType printerType, HfcPrinterConfig *printerConfig)
{
	if (printerType < MaxPrinterTypes)
	{
		//	Save the currentPrinterConfig values, but release any current data first.
		Debug::trace("ConfigurationManager.PrinterConfig", "setCurrentPrinterConfig(" + Integer::toString(printerType) + ", " + util_string::ptrtostring(printerConfig) + ")");
		if (currentPrinterConfig[printerType])
			delete currentPrinterConfig[printerType];
		currentPrinterConfig[printerType] = printerConfig;
		printerConfig->show();
	}
	else
	{
		Debug::trace("ConfigurationManager.PrinterConfig", "setCurrentPrinterConfig(" + Integer::toString(printerType) + "):  Invalid Printer number!");
	}
}

PrinterInfo *ConfigurationManager::getCurrentPrinterInfo(PrinterType printerType) const
{
	if (printerType < MaxPrinterTypes)
		return currentPrinterInfo[printerType];
	else
	{
		Debug::trace("ConfigurationManager.PrinterConfig", "getCurrentPrinterInfo(" + Integer::toString(printerType) + "):  Invalid Printer number!");
		return 0;
	}
}
void ConfigurationManager::setCurrentPrinterInfo(PrinterInfo *printerInfo, PrinterType printerType)
{
	if (printerType < MaxPrinterTypes)
	{
		//	Save the currentPrinterInfo values, but release any current data first.
		if (currentPrinterInfo[printerType])
			delete currentPrinterInfo[printerType];
		currentPrinterInfo[printerType] = printerInfo;
	}
	else
	{
		Debug::trace("ConfigurationManager.PrinterConfig", "setCurrentPrinterInfo(" + Integer::toString(printerType) + "):  Invalid Printer number!");
	}
}

HfcWrapperConfig *ConfigurationManager::getCurrentWrapperConfig() const
{
	return currentWrapperConfig;
}
void ConfigurationManager::setCurrentWrapperConfig(HfcWrapperConfig *wrapperConfig)
{
	//	Save the current values, but release any current data first.
	if (currentWrapperConfig)
	{
		Debug::trace("ConfigurationManager", "deleting currentWrapperConfig = " + HobString(Hex::toString((long long) currentWrapperConfig)));
		delete currentWrapperConfig;
	}
	currentWrapperConfig = wrapperConfig;
}

HfcWrapperControl *ConfigurationManager::getCurrentWrapperControl() const
{
	return currentWrapperControl;
}
void ConfigurationManager::setCurrentWrapperControl(HfcWrapperControl *wrapperControl)
{
	//	Save the current values, but release any current data first.
	if (currentWrapperControl)
	{
		Debug::trace("ConfigurationManager", "deleting currentWrapperControl = " + HobString(Hex::toString((long long) currentWrapperControl)));
		delete currentWrapperControl;
	}
	currentWrapperControl = wrapperControl;
	Debug::trace("ConfigurationManager", "setCurrentWrapperControl(): currentWrapperControl = " + HobString(Hex::toString((long long) currentWrapperControl)));
}

HfcEvolWrapperConfig *ConfigurationManager::getCurrentEvolWrapperConfig() const
{
	Debug::trace("ConfigurationManager", "getCurrentEvolWrapperConfig(): currentEvolWrapperConfig = " + HobString(Hex::toString((long long) currentEvolWrapperConfig)));
	return currentEvolWrapperConfig;
}
void ConfigurationManager::setCurrentEvolWrapperConfig(HfcEvolWrapperConfig *wrapperConfig)
{
	//	Save the current values, but release any current data first.
	if (currentEvolWrapperConfig)
	{
		Debug::trace("ConfigurationManager", "setCurrentEvolWrapperConfig():  Deleting currentEvolWrapperConfig = " + HobString(Hex::toString((long long) currentEvolWrapperConfig)));
		delete currentEvolWrapperConfig;
	}
	currentEvolWrapperConfig = wrapperConfig;
}

HfcEvolWrapperControl *ConfigurationManager::getCurrentEvolWrapperControl() const
{
	Debug::trace("ConfigurationManager", "getCurrentEvolWrapperControl(): currentEvolWrapperControl = " + HobString(Hex::toString((long long) currentEvolWrapperControl)));
	return currentEvolWrapperControl;
}
void ConfigurationManager::setCurrentEvolWrapperControl(HfcEvolWrapperControl *wrapperControl)
{
	//	Save the current values, but release any current data first.
	if (currentEvolWrapperControl)
	{
		Debug::trace("ConfigurationManager", "setCurrentEvolWrapperControl():  Deleting currentEvolWrapperControl = " + HobString(Hex::toString((long long) currentEvolWrapperControl)));
		delete currentEvolWrapperControl;
	}
	currentEvolWrapperControl = wrapperControl;
}



//********************************************************
//						WEIGHER
//********************************************************

void ConfigurationManager::handleWeigherConfig(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Weigher Configuration Message");

	Debug::trace("ConfigurationManager.weigherConfig", "handleWeigherConfig() msg->device: " + inttostring(msg->getDeviceType()) +
		"  msg->event: " + inttostring(msg->getEventType()));

	if (!msg || !currentWeigherConfig)
		return;

	Configuration *myConfig = Configuration::getInstance();
	if (!myConfig)
		return;

 	HfcWeigherConfig *weigherConfig	= dynamic_cast<HfcWeigherConfig *> (msg->getHfcObject());

	if (!weigherConfig)
		return;

	Session *s = dynamic_cast<Session *>(mySession);
	if (!s)
		return;

	if (!currentWeigherConfig)
		return;

	/* WE HAVE EVERYTHING - FIX HERE */
	EvolDeviceType localDev = msg->getDeviceType();
	int localEvt            = msg->getEventType();

	if ( localDev == Weigher && localEvt == WgRestoreWeigherSettings )
	{
		Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "Weigher with WgRestoreWeigherSettings");

		// This is where are are going to manipulate data when we receive individual or combined
		// CAT3 COM messages dropped in downloads Folder
		// Now we need to find out if it contains a WP tag (minwttoprint)
		HobString buff(msg->getOriginalBuff());
			
		std::size_t pos = buff.find("WP");
		if (pos != std::string::npos)
		{
			// need to also find out if the scaleFactor came into play
			bool scaleFactorHasChanged = weigherConfig->isReceived(fieldOffsetScaleFactor);
			Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "scaleFactorHasChanged: " + booltostring(scaleFactorHasChanged));

			// now keep in mind, we ONLY do this if the scaleFactor is 5, unless this cascades into something more when I fix this
			UINT32 scaleFactor = 1;
			if ( scaleFactorHasChanged )
			{
				scaleFactor = weigherConfig->getScaleFactor();
				Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherConfig() scaleFactorHasChanged is TRUE scaleFactor from weigherConfig: " + uinttostring(scaleFactor));
			}
			else
			{
				scaleFactor = myConfig->getWeigherScaleFactor();
				Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherConfig() scaleFactorHasChanged is FALSE scaleFactor from scaleConfig: " + uinttostring(scaleFactor));
			}

			if ( scaleFactor == 5 )
			{
				// game on
				
				// wont work because the getMinWeightToPrint is already 0 on a 0.004 type number so nothing to shift.
				//UINT32 minwt = weigherConfig->getMinWeightToPrint();
				//minwt *= 10;
				//weigherConfig->setMinWeightToPrint(minwt);
				
				HobString token = buff.substr(pos + 1, 5);
				HobWeight minwt = MetSig::determineImplicitDecimalWeight(token);
				weigherConfig->setMinWeightToPrint(minwt);

				//THIS IS SCALE CONFIG not weigherConfig  DO NOT USE myConfig->setMinWeightToPrint((HobWeight)minwt);
				Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherConfig() update weigherConfig with minwt: " + longtostring(minwt));
			}
		}
	}

 	HfcWeigherConfig copyCurrentWeigherConfig(currentWeigherConfig);

	Session *primarySession = SessionManager::getInstance()->getPrimaryConsoleSession();

	if ( Debug::isTracing("WeigherConfiguration") )
	{
		Debug::trace("WeigherConfiguration", "handling Weigher Configuration Message - existing weigher config:");
		currentWeigherConfig->show();
		Debug::trace("WeigherConfiguration", "handling Weigher Configuration Message - new weigher config:");
		weigherConfig->show();
	}
	
	bool weigherCurrentlyZero = false;
	if (primarySession)
	{
		HfcWeigherStatus *currWeigherStatus = primarySession->getCurrentWeight();
		if (currWeigherStatus)
			weigherCurrentlyZero = currWeigherStatus->isZeroStableWeight();
	}

	if ( msg->getEventType() == WgRestoreWeigherSettings && mySession )
	{
		// Need to display a message on the UI that configuration is being updated remotely, so that the operator cannot use the scale while the update is in-process.
		if ( mySession->isPrimarySession() )
		{
			mySession->addDialogMessage(ErrWeigher, WeigherErrRemoteConfigurationStarted, HfcResponse::RESPONSESTYLE_NOINPUT);
			PAL_absclass::PAL_sleep(2000);
		}
		else
		{
			SessionMessage msg(Weigher, WgBeginRemoteWgConfig);
			msg.setFormatType(InternalFormat);
			mySession->send(&msg, Messenger::DEST_RTC);
		}
	}

	bool hasChanged = copyCurrentWeigherConfig.update(weigherConfig);
	Debug::trace("ConfigurationManager.weigherConfig", string("handling Weigher Configuration Message - updated current weigher config - result:") + Bool::toString(hasChanged));

	if ( !hasChanged )
	{
		Debug::trace("ConfigurationManager.weigherConfig", "Nothing has changed, not saving weigherConfig");

		// Well this is nice, we popped the dialog saying remote update, and then if nothing has changed, we exit.
		// Well WHAT ABOUT THE DIALOG WE JUST POPPED UP?  THE SCALE IS LOCKED UP NOW - REQUIRES HARD REBOOT
		if (msg->getEventType() == WgRestoreWeigherSettings && mySession)
		{
			// Need to remove the message on the UI that configuration is being updated remotely, so that the operator can now use the scale since the update is complete.
			if (mySession->isPrimarySession())
				mySession->removeDialogMessage(ErrWeigher, WeigherErrRemoteConfigurationStarted, HfcResponse::RESPONSESTYLE_NOINPUT);
			else
			{
				SessionMessage msg(Weigher, WgEndRemoteWgConfig);
				msg.setFormatType(InternalFormat);
				mySession->send(&msg, Messenger::DEST_RTC);
			}
		}
		return;
	}
	else if ( !copyCurrentWeigherConfig.isValid() )
	{
		Debug::trace("ConfigurationManager.weigherConfig", "Changes to WeigherConfig result in invalid configuration, not saving weigherConfig");
		
		// Well this is nice, we popped the dialog saying remote update, and then if nothing has changed, we exit.
		// Well WHAT ABOUT THE DIALOG WE JUST POPPED UP?  THE SCALE IS LOCKED UP NOW - REQUIRES HARD REBOOT
		if (msg->getEventType() == WgRestoreWeigherSettings && mySession)
		{
			// Need to remove the message on the UI that configuration is being updated remotely, so that the operator can now use the scale since the update is complete.
			if (mySession->isPrimarySession())
				mySession->removeDialogMessage(ErrWeigher, WeigherErrRemoteConfigurationStarted, HfcResponse::RESPONSESTYLE_NOINPUT);
			else
			{
				SessionMessage msg(Weigher, WgEndRemoteWgConfig);
				msg.setFormatType(InternalFormat);
				mySession->send(&msg, Messenger::DEST_RTC);
			}
		}
		return;
	}
	else
	{
		hasChanged = currentWeigherConfig->update(weigherConfig);

		bool scaleFactorHasChanged = weigherConfig->isReceived(fieldOffsetScaleFactor);
		Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "hasChanged: " + booltostring(hasChanged) );
		Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "scaleFactorHasChanged: " + booltostring(scaleFactorHasChanged));

		Debug::trace("ConfigurationManager.weigherConfig", string("handling Weigher Configuration Message - updated current weigher config - result:") + Bool::toString(hasChanged));
		if (hasChanged)
		{
			// Don't do this here, we only want to update this when we leave Service Mode, per Weights & Measures.  lap
			//currentWeigherConfig->updateConfigurationDateAndTimes();

			//	Since something here has changed, save the Configuration.
			Debug::trace("ConfigurationManager.weigherConfig", "saving weigherConfig");

			// Save the fields duplicated in Configuration from the weigherInquire data to Configuration.
			Debug::trace("ConfigurationManager.weigherConfig", "handleWeigherConfig() - setting duplicated weigher config fields into Configuration singleton");
			myConfig->setScaleWeighMode(weigherConfig->getScaleWeighMode());

			myConfig->setNetWeightDualRangeLimitCheck(weigherConfig->isNetWeightDualRangeLimitCheck());

			// Configuration stores the min weight to print with 3-decimal places, where the WeigherConfiguration object stores it based on single-range vs. dual-range moded,
			// so multiply the WeigherConfiguration min weight to print by 10 if it's in single-range mode.
			UINT32 scaleFactor = 0;
			if (scaleFactorHasChanged)
			{
				scaleFactor = weigherConfig->getScaleFactor();
				Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherConfig() scaleFactorHasChanged is TRUE scaleFactor from weigherConfig: " + uinttostring(scaleFactor));
			}
			else
			{
				scaleFactor = myConfig->getWeigherScaleFactor();
				Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherConfig() scaleFactorHasChanged is FALSE scaleFactor from scaleConfig: " + uinttostring(scaleFactor));

			}

			long minWeightForConfiguration = weigherConfig->getMinWeightToPrint();
			Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherConfig minWeightForConfiguration BEFORE any change " + longtostring(minWeightForConfiguration));

			
			if ( scaleFactor == 1 )
			{
				Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherConfig scaleFactor=1 so " + longtostring(minWeightForConfiguration) + "*=10");
				minWeightForConfiguration *= 10;
				Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherConfig minWeightForConfiguration after " + longtostring(minWeightForConfiguration));
			}
			else if ( scaleFactor == 5 )
			{
				Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherConfig scaleFactor=5 SO DO NOTHING NO *=10");
			}

			Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherConfig() call setMinWeightToPrint() with: " + longtostring(minWeightForConfiguration));
			myConfig->setMinWeightToPrint(minWeightForConfiguration);

			myConfig->setWeigherModel(weigherConfig->getWeigherModel());

			myConfig->validateTareEntryPrecision();

			Debug::trace("ConfigurationManager.valueMax", "handleWeigherConfig() value max enabled: " + longtostring(weigherConfig->getValueMaxEnabled()));
			myConfig->setValueMaxEnabled(weigherConfig->getValueMaxEnabled());

			myConfig->save(dbAccess);
			dispatchConfigurationSaved();

			// Added log message when remote scale management feature added, FHA 11/13
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, "Weigher configuration has changed.");

			if (mySession)
			{
				// For Category III, have to send the current date/time to the Weigher Manager before permanently setting any configuration or calibration values.
				if ( myConfig->isCAT3AuditTrailLicensed() )
				{
					time_t now;
					time(&now);
					WeigherCat3DateTime wCat3DateTime( mySession, (UINT32) now );
					//wCat3DateTime.setEpochTime(0x12345678);
					mySession->send( &wCat3DateTime, Messenger::DEST_RTC );
				}

				currentWeigherConfig->setEventType(WgConfig);
				currentWeigherConfig->setConfigType(WgPermanent);

				SessionMessage response(Weigher, WgConfig);
				response.setFormatType(InternalFormat);
				response.setHfcObject(currentWeigherConfig);

				const char *msgdata = response.serialize();
				int msglen = response.getSerializedLength();

				Debug::trace("ConfigurationManager.response",	HobString("weigher configuration response: ") +
																HobString(Hex::toString(msgdata, msglen)));

				Debug::trace("ConfigurationManager.weigherConfig", "Sending message to RTC to write weigherConfig");
				mySession->send(&response, Messenger::DEST_RTC);

				Debug::trace("ConfigurationManager.weigherConfig", "Setting savedWeigherConfig to true");
				savedWeigherConfig = true;

				WeigherRequestInquire wri;
				mySession->send(&wri, Messenger::DEST_RTC);
			}
		}

		if ( weigherCurrentlyZero && mySession )
		{
			WeigherRezero rezero;
			mySession->dispatchEvent(&rezero);
		}

		if ( msg->getEventType() == WgRestoreWeigherSettings && mySession )
		{
			// Need to remove the message on the UI that configuration is being updated remotely, so that the operator can now use the scale since the update is complete.
			if ( mySession->isPrimarySession() )
				mySession->removeDialogMessage(ErrWeigher, WeigherErrRemoteConfigurationStarted, HfcResponse::RESPONSESTYLE_NOINPUT);
			else
			{
				SessionMessage msg(Weigher, WgEndRemoteWgConfig);
				msg.setFormatType(InternalFormat);
				mySession->send(&msg, Messenger::DEST_RTC);
			}
		}
	}
}

void ConfigurationManager::handleWeigherInquire(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Weigher Inquire Message");

	if (!msg)
		return;

	Configuration *myConfig = Configuration::getInstance();
	if (!myConfig)
		return;

 	HfcWeigherInquire *weigherInquire = dynamic_cast<HfcWeigherInquire *> (msg->getHfcObject());
	if (!weigherInquire)
		return;

	if (enteringServiceMode)
	{
		Debug::trace("ConfigurationManager.weigherConfig", "saving initialServiceWeigherConfig since enteringServiceMode=true");
		initialServiceWeigherConfig = new HfcWeigherConfig(weigherInquire);
		enteringServiceMode = false;
	}

	//	Save the current values, but release any current data first.
	msg->setHfcObjectCreatedInternally(false);
	setCurrentWeigherConfig(weigherInquire);

#if defined( __linux__ )
	// send weigher info to hook
    if ( Configuration::getInstance()->getCustomerHooksEnabled() )
	{
		std::string calibDateTimeStr;
		std::string configDateTimeStr;

		if ( 0 < currentWeigherConfig->getNumberOfCalibrations() )
		{
			HobDateTime dt;
			
			dt = currentWeigherConfig->getLastCalibrationDate();
			calibDateTimeStr = dt.formatDate( Configuration::getInstance()->getDateDisplayFormat() );

			dt = currentWeigherConfig->getLastConfigurationDate();
			configDateTimeStr = dt.formatDate( Configuration::getInstance()->getDateDisplayFormat() );
		}

		hobart_cust_hook_update_scale_state( HOOK_SS_SCALE_WEIGHER_CALIB_STAMP, calibDateTimeStr.c_str() );
		hobart_cust_hook_update_scale_state( HOOK_SS_SCALE_WEIGHER_CONF_STAMP, configDateTimeStr.c_str() );

		// ---- //

		int limit;
		int weightDivisor = (int) Configuration::getInstance()->getWeightDivisor();

		std::string unit;
		std::string div;

		std::string divDR;

		if ( currentWeigherConfig->isScaleMetric() )
		{
			limit = Configuration::getInstance()->toIntCfgI( CfgInternal::DUALRANGELIMITMETRIC );

			unit = "kg";
			div = "0.005";

			if ( 2 == currentWeigherConfig->getScaleFactor() )
				divDR = "0.002";
		}
		else
		{
			limit = Configuration::getInstance()->toIntCfgI( CfgInternal::DUALRANGELIMITAVOIR );

			unit = "lb";
			div = "0.01";

			if ( 5 == currentWeigherConfig->getScaleFactor() )
				divDR = "0.005";
		}

		std::string maxWeight = Long::toString( currentWeigherConfig->getMaxScaleWeight() );
		std::string maxWeightDR = Long::toString( limit / weightDivisor );

		// ---- //

		hookWeigherPrecision = maxWeight + " x " + div + " " + unit;

		if ( divDR.length() )
			hookWeigherPrecisionDR = maxWeightDR + " x " + divDR + " " + unit;
		else
			hookWeigherPrecisionDR.clear();

		Debug::trace("ConfigurationManager.weigherConfig", "hook precision " + hookWeigherPrecision);
		Debug::trace("ConfigurationManager.weigherConfig", "hook precision dual range " + hookWeigherPrecisionDR);
	}
#endif

	if (leavingServiceMode)
	{
		Debug::trace("ConfigurationManager.weigherConfig", "We can call leavingServiceMode() since we have an updated currentWeigherConfig.");
		leaveServiceMode();
		leavingServiceMode = false;
		return;
	}

	Session * s = dynamic_cast<Session *>(mySession);
	if (s && s->isPrimarySession())
	{
		// try and read the APC weigher configuration parameters (one of them) that were previously stored in the database, but
		// are now stored in the weigher's eep.  This change happened with version 3.0 of the application, and version 2.0 of
		// the APC firmware.  If they exist in the db, then assume they have never been written to the eep, so write them now.
		// Then delete them from the database, so they won't be 'setup' (initially written) in the eep again in the future.

		if ( (myConfig->weigherConfigExistsInDB(dbAccess)) && (!SessionManager::getInstance()->isEzUsbWeigherAttached()) )
		{
			// For Category III, have to send the current date/time to the Weigher Manager before permanently setting any configuration or calibration values.
			time_t now;
			time(&now);
			WeigherCat3DateTime wCat3DateTime( mySession, (UINT32) now );
			mySession->send( &wCat3DateTime, Messenger::DEST_RTC );

			currentWeigherConfig->setScaleWeighMode(Configuration::getInstance()->getScaleWeighMode());
			currentWeigherConfig->setNetWeightDualRangeLimitCheck(Configuration::getInstance()->isNetWeightDualRangeLimitCheck());
			currentWeigherConfig->setMinWeightToPrint((UINT32) Configuration::getInstance()->getMinWeightToPrint());
			currentWeigherConfig->setWeigherModel(Configuration::getInstance()->getWeigherModel());
			currentWeigherConfig->setConfigType(WgPermanent);

			currentWeigherConfig->setEventType(WgConfig);

			currentWeigherConfig->show();

			SessionMessage writeWeigherCfg(Weigher, WgConfig);
			writeWeigherCfg.setFormatType(InternalFormat);
			writeWeigherCfg.setHfcObject(currentWeigherConfig);
			mySession->send(&writeWeigherCfg, Messenger::DEST_RTC);
			Debug::trace("ConfigurationManager.weigherConfig", "sending message to RTC to write weigherConfig (initial setup after APC 2.0 upgrade)");

			myConfig->deleteWeigherConfigFromDB(dbAccess);
/*
			SessionMessage shutdownMsg(SessionDev, SnShutdownRequest);
			shutdownMsg.setFormatType(BrowserFormat);
			GenericHfcObject gho(inttostring(ShutdownRequest_TurnOff));
			shutdownMsg.setHfcObject(&gho);
			s->handleMessage(&shutdownMsg);
*/
		}
	}

	//	The HT series does not support the EzUsbWeigher.
	if (!SessionManager::getInstance()->isEzUsbWeigherAttached())   //  Only this weigher saves this info in the configuration database.
	{
		// Save the fields duplicated in Configuration from the weigherInquire data to Configuration.
		//  Apparently, they do not save these fields to the database for the APC weigher, nor will we for the Atmel-based SAM-BA device.
		//  We will need to save these for the EzUSB weigher, but the CfgScale methods will handle that. [CCD June 2010]
		Debug::trace("ConfigurationManager.weigherConfig", "handleWeigherInquire() - setting duplicated weigher config fields into Configuration singleton");
		myConfig->setScaleWeighMode(weigherInquire->getScaleWeighMode());
		myConfig->setNetWeightDualRangeLimitCheck(weigherInquire->isNetWeightDualRangeLimitCheck());

		// Configuration stores the min weight to print with 3-decimal places, where the WeigherConfiguration object stores it based on single-range vs. dual-range moded,
		// so multiply the WeigherConfiguration min weight to print by 10 if it's in single-range mode.
		long minWeightForConfiguration = weigherInquire->getMinWeightToPrint();
		if ( weigherInquire->getScaleFactor() == 1 )
		{
			Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherInquire() scaleFactor=1 so " + longtostring(minWeightForConfiguration) + "*=10" );
			minWeightForConfiguration *= 10;
			Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherInquire() minWeightForConfiguration after " + longtostring(minWeightForConfiguration) );
		}
		else if ( weigherInquire->getScaleFactor() == 5 )
		{
			Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherInquire() scaleFactor=5 SO DO NOTHING NO *=10");
		}

		Debug::trace("ConfigurationManager.checkScaleFactorForMinWt", "handleWeigherInquire() call setMinWeightToPrint() with: " + longtostring(minWeightForConfiguration));

		myConfig->setMinWeightToPrint(minWeightForConfiguration);

		Debug::trace("ConfigurationManager.valueMax", "handleWeigherInquire() value max enabled: " + longtostring(weigherInquire->getValueMaxEnabled()));
		myConfig->setValueMaxEnabled(weigherInquire->getValueMaxEnabled());

		myConfig->setWeigherModel(weigherInquire->getWeigherModel());
	}

	// The following exists to make the scaleWeighMode available to HTe-ScaleManagement
	myConfig->updateField(fieldOffsetCfgWeighMode, inttostring((int)weigherInquire->getScaleWeighMode()));
	myConfig->saveField(fieldOffsetCfgWeighMode, dbAccess);

//	Just because we are saving the WeighMode to config for HTe, we don't need to dispatch a CfgSaved msg.
//	HTe will cause a new Session which would then have the current configuration data. [CCD Aug 2015]
        dispatchConfigurationSaved();

	if (mySession)
	{
		SessionMessage response(Weigher, WgInquire);
		response.setFormatType(BrowserFormat);
		response.setHfcObject(currentWeigherConfig);

		if (Debug::isTracing("ConfigurationManager.response"))
		{
			const char *msgdata = response.toString();
			int msglen = response.getSerializedLength();
			Debug::trace("ConfigurationManager.response",	HobString("weigher inquire response: ") + HobString(Hex::toString(msgdata, msglen)));
		}

		mySession->send(&response, Messenger::DEST_UI);

		// Any prior request for weigher status would be ignored because the weigherModel in Configuration is initialized to WmNone,
		// and Weigher Status messages are ignored if the weigher model is 'None'.
		// So I'm requesting a weigher status now.
		if (s)
			s->requestAWeigherStatus(false);
	}

	if (s)
		s->requestAWeigherStatus(false);
}

void ConfigurationManager::handleWeigherRequestInquire(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Weigher Request Inquire Message");

	if (!msg || !mySession)
		return;

	//	Register for the WgInquire message
	//  These have been moved to Session so that we always listen for a WgInquire and SysInquire messages coming in.
	//  They won't be coming in constantly, plus the RealTimeController may ask for them (if the FPC enumerates after the UI is up and running),
	//  so we always process any WgInquire/SysInquire that come in; they can be unsolicited.
	//mySession->registerEvent(Weigher, WgInquire, false );

	//	Register for the Weigher system inquire message
	//mySession->registerEvent(WeigherSystem, SysInquire, false);

	WeigherSystemRequestInquire wsri;
	mySession->send(&wsri, Messenger::DEST_RTC);

	//	Not much to do here with a Weigher Request Inquire message except pass it on.
	//	But first, change the format from Browser to Internal.
	msg->setFormatType(InternalFormat);

	Debug::trace("ConfigurationManager.response", HobString("weigher request inquire response: "));

	mySession->send(msg, Messenger::DEST_RTC);
}

void ConfigurationManager::handleWeigherRequestValueMax(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Weigher Request ValueMax Message");

	if (!msg || !mySession)
		return;

	// Just pass the request on to the RTC
	WeigherSystemRequestValueMax wsri;
	mySession->send(&wsri, Messenger::DEST_RTC);

	// But first, change the format from Browser to Internal.
	msg->setFormatType(InternalFormat);
	Debug::trace("ConfigurationManager.response", HobString("weigher request ValueMax response: "));
	mySession->send(msg, Messenger::DEST_RTC);
}

void ConfigurationManager::handleWeigherValueMax(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Weigher Value Max Message");

	HfcWeigherValueMaxFitParams *valuemax = dynamic_cast<HfcWeigherValueMaxFitParams *> (msg->getHfcObject());
	if (!valuemax)
	{
		Debug::trace("Session", "HfcWeigherValueMaxFitParams is NULL!");
		return;
	}

	Debug::trace("ConfigurationManager", "Weigher Reply to Value Max received from RTC was Successful");

	SessionMessage response(Weigher, WgValueMaxFitParams);
	response.setFormatType(BrowserFormat);
	response.setHfcObject(valuemax);

	mySession->send(&response, Messenger::DEST_UI);
}

void ConfigurationManager::handleWeigherRequestInfo(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Weigher Request Info Message");

	if (!msg || !mySession)
		return;
#if 0
	WeigherMode wgMode(mySession, (mySession->isPrimarySession()) ? ScaleWeightTrackingMode : ScaleManualMode);
	mySession->send(&wgMode, Messenger::DEST_RTC);
#endif
	Session * s = dynamic_cast<Session *>(mySession);
	if (s)
		s->requestAWeigherStatus();

#if 0
	mySession->setCurrentModeOp(Session::OP_WEIGHERINFO);
#endif
}

void ConfigurationManager::handleWeigherInfo(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Weigher Info Message");

	if (!msg)
		return;

 	HfcWeigherSystemInquire *weigherSystemInquire = dynamic_cast<HfcWeigherSystemInquire *> (msg->getHfcObject());

	if (!weigherSystemInquire)
		return;

	if (mySession)
	{
		SessionMessage response(Weigher, WgInfo);

		WeigherInfo *weigherInfo = new WeigherInfo();
		if (weigherInfo)
		{
			// Take ownership of the HfcWeigherSystemInquire from the message, so we can use it later.
			msg->setHfcObjectCreatedInternally(false);
			weigherInfo->setWeigherSystemConfig(weigherSystemInquire);
			weigherInfo->setStationAddress(weigherSystemInquire->getStationAddress());

			response.setFormatType(BrowserFormat);
			response.setHfcObject(weigherInfo);

			if (Debug::isTracing("ConfigurationManager.response"))
			{
				const char *msgdata = response.toString();
				int msglen = response.getSerializedLength();
				Debug::trace("ConfigurationManager.response",	HobString("weigher info response: ") +
																HobString(Hex::toString(msgdata, msglen)));
			}
			mySession->send(&response, Messenger::DEST_UI);

			//	Save the currentWeigherInfo values.
			setCurrentWeigherInfo(weigherInfo);
		}
	}
}

void ConfigurationManager::handleWeigherSaveInfo(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Weigher Save Info Message");

	if (!msg || !currentWeigherInfo || !mySession)
		return;

 	WeigherInfo *weigherInfo = dynamic_cast<WeigherInfo *> (msg->getHfcObject());

	if (!weigherInfo)
		return;

	if (weigherInfo->getStationAddress() == 0)
	{
		mySession->addDialogMessage(ErrWeigher, WeigherErrInvalidAddress, HfcResponse::RESPONSESTYLE_OK);

		SessionMessage response(Weigher, WgInfo);

		response.setFormatType(BrowserFormat);
		response.setHfcObject(weigherInfo);
		weigherInfo->setStationAddress(currentWeigherInfo->getStationAddress());

		if (Debug::isTracing("ConfigurationManager.WgSaveInfo"))
		{
			const char *msgdata = response.toString();
			int msglen = response.getSerializedLength();
			Debug::trace("ConfigurationManager.WgSaveInfo",	HobString("weigher info response: ") +
														 	HobString(Hex::toString(msgdata, msglen)));
		}

		mySession->send(&response, Messenger::DEST_UI);
	}
	else
	{
		currentWeigherInfo->update(weigherInfo);

		HfcWeigherSystemConfig *weigherSystemConfig = currentWeigherInfo->getWeigherSystemConfig();
		if (weigherSystemConfig)
		{
			//	If the old address is not equal to the new address, set the new address
			if (weigherSystemConfig->getStationAddress() != currentWeigherInfo->getStationAddress())
			{
				//	Send the WeigherSystemConfig back to the RTC

				if (mySession)
				{
					weigherSystemConfig->setEventType(SysConfig);
					weigherSystemConfig->setStationAddress(currentWeigherInfo->getStationAddress());
					mySession->send(weigherSystemConfig, Messenger::DEST_RTC);
				}
			}
		}
	}
}

void ConfigurationManager::handleWeigherDoneInfo(SessionMessage * /* msg */)
{
	Debug::trace("ConfigurationManager", "handling Weigher Done Info Message");

	if (mySession)
	{
		WeigherMode wgMode(mySession, ScaleManualMode);
		mySession->send(&wgMode, Messenger::DEST_RTC);

		Session *s = dynamic_cast<Session *> (mySession);
		if (s)
			s->setCurrentModeOp(Session::OP_UNKNOWN);
	}
}


void ConfigurationManager::handleWeigherAttach(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Weigher Attach Message");

	if (!msg)
		return;

	GenericHfcObject *gho = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
	if (!gho)
		return;

	bool attached = Bool::valueOf(gho->getData());

	// Don't need to do anything if the message doesn't actually flip the flag to the opposite value.
	if (Configuration::getInstance()->isWeigherAttached() == attached)
		return;

	Debug::trace("ConfigurationManager", HobString("Setting weigher attached flag to ") + HobString(Bool::toString(attached)));

	Configuration::getInstance()->setWeigherAttached(attached);
	Configuration::getInstance()->save(dbAccess);
	dispatchConfigurationSaved();

	if (mySession)
	{
		if (attached)
		{
			WeigherMode wgMode(mySession, ScaleManualMode);
			mySession->send(&wgMode, Messenger::DEST_RTC);
		}
		else
		{
			Session *s = dynamic_cast<Session *> (mySession);
			if (s)
				s->deleteCurrentWeight();

			WeigherMode wgMode(mySession, ScaleDumbMode);
			mySession->send(&wgMode, Messenger::DEST_RTC);
		}
	}
}

// SOF-1667
void ConfigurationManager::handleWeigherResetFactoryDefaults(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager.handleWeigherResetFactoryDefaults", "handling Weigher Reset Factory Defaults Message send to RTC");

	msg->setFormatType(InternalFormat);
	mySession->send(msg, Messenger::DEST_RTC);

	mySession->registerEvent(Weigher, WgResetFactoryDefaultsComplete);
}

// SOF-1667
void ConfigurationManager::handleWeigherResetFactoryDefaultsComplete(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager.handleWeigherResetFactoryDefaultsComplete", "handling Weigher Reset Factory Defaults Complete Message");

	HfcWeigherDefaultsComplete *complete = dynamic_cast<HfcWeigherDefaultsComplete *> (msg->getHfcObject());
	if (!complete) {

		Debug::trace("ConfigurationManager.handleWeigherResetFactoryDefaultsComplete", "HfcWeigherDefaultsComplete is NULL!");
		return;
	}

	Debug::trace("ConfigurationManager", "Weigher Reset Factory Defaults Complete was Successful");

	SessionMessage response(Weigher, WgResetFactoryDefaultsComplete);
	response.setFormatType(BrowserFormat);
	response.setHfcObject(complete);

	mySession->send(&response, Messenger::DEST_UI);

	// Set so that CAT-I dates and counts are updated when exiting Service mode
	savedWeigherConfig = true;
}


//********************************************************
//						LABEL APPLIER
//********************************************************

void ConfigurationManager::handleLabelApplierConfig(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("ConfigurationManager");
	if (traceOn)
		Debug::trace("ConfigurationManager", "handling Label Applier Configuration Message");

	if (!msg || !currentLabelApplierConfig || !mySession)
		return;

	HfcLabelApplierConfig labelApplierConfig(dynamic_cast<HfcLabelApplierConfig *> (msg->getHfcObject()));

	bool hasChanged = currentLabelApplierConfig->update(labelApplierConfig);

	if (traceOn)
	{
		HobString msgDebug = "handleLabelApplierConfig():  ";
		msgDebug.append("isLeftToRight = ").append(Bool::toString(currentLabelApplierConfig->isLeftToRight()));
		msgDebug.append("  defaultLabelPlacement = ").append(Integer::toString(Configuration::getInstance()->getDefaultLabelPlacement()));
		msgDebug.append("  configChanged = ").append(Bool::toString(hasChanged));
		Debug::trace("ConfigurationManager", msgDebug);
	}

	if (hasChanged)
	{
		currentLabelApplierConfig->setEventType(LaConfig);
		currentLabelApplierConfig->setConfigType((LaConfigType) LaPermanent);

		SessionMessage response(LabelApplier, LaConfig);
		response.setFormatType(InternalFormat);
		response.setHfcObject(currentLabelApplierConfig);

		const char *msgdata = response.serialize();
		int msglen = response.getSerializedLength();
		Debug::trace("ConfigurationManager.response",	HobString("label applier configuration response: ") +
														HobString(Hex::toString(msgdata, msglen)));

		int bytesSent = mySession->send(&response, Messenger::DEST_RTC);
		if (bytesSent < 0)
		{
			// todo - error handling?
			Debug::trace("ConfigurationManager", "LaConfig RESPONSE MESSAGE COULD NOT BE SENT");
		}
	}
}

void ConfigurationManager::handleLabelApplierInquire(SessionMessage *msg)
{
	bool traceOn = true;

	Debug::trace("ConfigurationManager", "handling Label Applier Inquire Message");

	if (!msg)
		return;

	bool configChanged = false;
	HfcLabelApplierInquire *labelApplierInquire = dynamic_cast<HfcLabelApplierInquire *> (msg->getHfcObject());

	if (!labelApplierInquire)
		return;

	//	Save the current values.
	msg->setHfcObjectCreatedInternally(false);

	if (!currentLabelApplierConfig)
	{
		setCurrentLabelApplierConfig(labelApplierInquire);		//	Set this in case there is no currentLabelApplierConfig
		configChanged = true;			//	Do I really need to do this?
		//Debug::trace("ConfigurationManager", msgDebug + "setCurrentLabelApplierConfig(labelApplierInquire)");
	}
	else
	{
		HfcLabelApplierConfig tempConfig(labelApplierInquire);
		configChanged = currentLabelApplierConfig->update(tempConfig);
		//Debug::trace("ConfigurationManager", msgDebug + "currentLabelApplierConfig->update(tempConfig)");
	}
	currentLabelApplierConfig->setEventType(LaConfig);
	//Debug::trace("ConfigurationManager", msgDebug + "currentLabelApplierConfig->getEventType = " + inttostring(labelApplierInquire->getEventType()));

	if (traceOn)
	{
		HobString msgDebug = "handleLabelApplierInquire():  isTrailingEdge = ";
		//msgDebug.append(Bool::toString(currentLabelApplierConfig->isTrailingEdge()));
		msgDebug.append("  isLeftToRight = ").append(Bool::toString(currentLabelApplierConfig->isLeftToRight()));
		msgDebug.append("  defaultLabelPlacement = ").append(Integer::toString(Configuration::getInstance()->getDefaultLabelPlacement()));
		Debug::trace("ConfigurationManager", msgDebug);
	}
	if (configChanged)
	{
		Configuration *myConfig = Configuration::getInstance();
		if(myConfig && currentLabelApplierConfig->getLabelPlacement() != myConfig->getDefaultLabelPlacement())
		{
			myConfig->setDefaultLabelPlacement(currentLabelApplierConfig->getLabelPlacement());
			if (traceOn)
				//Debug::trace("ConfigurationManager", msgDebug + "defaultLabelPlacement NOW = " + inttostring(myConfig->getDefaultLabelPlacement()) + ".  Saving Configuration.");
			//	Save the Configuration.
			myConfig->save(dbAccess);
			dispatchConfigurationSaved();
		}
	}
	//	The Session will determine if we want to send the Label Applier Inquire message to the Browser.
}

void ConfigurationManager::handleLabelApplierRequestInquire(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Label Applier Request Inquire Message");

	if (!msg || !mySession)
		return;

	//	Register for the LaInquire message
	mySession->registerEvent(LabelApplier, LaInquire, false ); 

	//	Not much to do here with a Label Applier Request Inquire message except pass it on.
	//	But first, change the format from Browser to Internal.
	msg->setFormatType(InternalFormat);

	Debug::trace("ConfigurationManager.response", HobString("label applier request inquire response: "));

	mySession->send(msg, Messenger::DEST_RTC);
}

//********************************************************
//						PRINTER
//********************************************************

void ConfigurationManager::handlePrinterConfig(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("ConfigurationManager.PrinterConfig");
	if (traceOn)
		Debug::trace("ConfigurationManager.PrinterConfig", "handling Printer Configuration Message");

	if (!msg)
	{
		if (traceOn)
			Debug::trace("ConfigurationManager.PrinterConfig", "msg is NULL - exiting!");
		return;
	}

	HfcPrinterConfig *printerConfig = dynamic_cast<HfcPrinterConfig *> (msg->getHfcObject());
	if (!printerConfig)
	{
		if (traceOn)
			Debug::trace("ConfigurationManager.PrinterConfig", "printerConfig is NULL - exiting!");
		return;
	}
	
	bool recordPrinterUpdated = false;
	PrinterType printerType = printerConfig->getPrinterType();
	if(printerType < MaxPrinterTypes)
	{
		if (currentPrinterConfig[printerType])
		{
			if (traceOn)
				Debug::trace("ConfigurationManager.PrinterConfig", "Updating printer " + inttostring(printerType) + " configuration.");

			//Debug::trace("ConfigurationManager.PrinterConfig", "Just BEFORE currrentPrinterConfig->update() which sets all of the HfcFields.");
			currentPrinterConfig[printerType]->update(printerConfig);
			//Debug::trace("ConfigurationManager.PrinterConfig", "Just AFTER currrentPrinterConfig->update() which sets all of the HfcFields.");

			if (traceOn)
			{
				Debug::trace("ConfigurationManager.PrinterConfig", "****  Show updated printer configuration.  ****");
				currentPrinterConfig[printerType]->show();
				Debug::trace("ConfigurationManager.PrinterConfig", "***********************************************");
			}
		}
		else
		{
			if (traceOn)
				Debug::trace("ConfigurationManager.PrinterConfig", "Creating a new printer " + inttostring(printerType) + " configuration.");
			HfcPrinterConfig *pNewPrinterConfig = new HfcPrinterConfig();
			pNewPrinterConfig->update(printerConfig);
			currentPrinterConfig[printerType] = pNewPrinterConfig;
		}

//		if (currentPrinterConfig->isDbSaveNeeded() && mySession)
		if (currentPrinterConfig[printerType]->isDbSaveNeeded() && mySession)
		{
			if (traceOn)
				Debug::trace("ConfigurationManager.PrinterConfig", "dispatchConfigurationSaved");
			Configuration::getInstance()->save(dbAccess);
			dispatchConfigurationSaved();
			recordPrinterUpdated = true;
		}

		if (currentPrinterConfig[printerType]->isEepConfigNeeded() && mySession)
		{
			if (traceOn)
				Debug::trace("ConfigurationManager.PrinterConfig", "EepConfigNeeded");
			currentPrinterConfig[printerType]->setEventType(PrConfig);	//	Wait until printerConfiguration is created.

			SessionMessage response(Printer, PrConfig);
			response.setFormatType(InternalFormat);
			response.setHfcObject(currentPrinterConfig[printerType]);

			const char *msgdata = response.serialize();
			int msglen = response.getSerializedLength();
			if (traceOn)
			{
				Debug::trace("ConfigurationManager.PrinterConfig",	HobString("printer configuration response: ") +
																	HobString(Hex::toString(msgdata, msglen)));
			}

			mySession->send(&response, Messenger::DEST_RTC);				// WHO CATCHES THIS? I think PrinterCommManager.
			recordPrinterUpdated = true;
		}
		if (recordPrinterUpdated)
		{
			// Added log message when remote scale maintenance feature added, FHA 11/13
			MessageLogger::logMessage(MessageLogger::ScaleEventsLog, HobString(printerType == FirstPrinter ? "FirstPrinter" : (printerType == SecondPrinter ? "SecondPrinter" : (printerType == ThirdPrinter ? "ThirdPrinter" : "Invalid Printer"))) + ":  Printer configuration was changed.");
		}
	}
	if (traceOn)
		Debug::trace("ConfigurationManager.PrinterConfig", "handlePrinterConfig():  Done.");
}

void ConfigurationManager::handlePrinterInquire(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager.PrinterConfig", "handling Printer Inquire Message");

	if (!msg)
		return;

	HfcPrinterInquire *printerInquire = dynamic_cast<HfcPrinterInquire *> (msg->getHfcObject());

	if (!printerInquire)
		return;

	//	Save the current values.
	msg->setHfcObjectCreatedInternally(false);
	setCurrentPrinterConfig(printerInquire->getPrinterType(), printerInquire);

	if (mySession)
	{
		SessionMessage response(Printer, PrInquire);
		response.setFormatType(BrowserFormat);
		response.setHfcObject(printerInquire);

		const char *msgdata = response.toString();
		int msglen = response.getSerializedLength();
		Debug::trace("ConfigurationManager.PrinterConfig",	HobString("printer inquire response: ") +
															HobString(Hex::toString(msgdata, msglen)));

		mySession->send(&response, Messenger::DEST_UI);
	}
}

void ConfigurationManager::handlePrinterRequestInquire(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager.PrinterConfig", "handling Printer Request Inquire Message");

	if (!msg || !mySession)
		return;

	//	Register for the PrInquire message
	mySession->registerEvent(Printer, PrInquire, false ); 

	//	Not much to do here with a Printer Request Inquire message except pass it on.
	//	But first, change the format from Browser to Internal.
	msg->setFormatType(InternalFormat);

	Debug::trace("ConfigurationManager.PrinterConfig", HobString("printer request inquire response: "));

	mySession->send(msg, Messenger::DEST_RTC);
}


void ConfigurationManager::handlePrinterRequestInfo(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Printer Request Info Message");

	if (!msg || !mySession)
		return;

	//	Register for the Printer system inquire message
	//	Stay registered until we get all the Printer system inquire messages back.
	mySession->registerEvent(PrinterSystem, SysInquire ); 

	int printerNum;
	for(printerNum = FirstPrinter; printerNum < MaxPrinterTypes; printerNum++)
	{
		PrinterSystemRequestInquire *psri = new PrinterSystemRequestInquire(NULL, (PrinterType) printerNum);
		const char *msgdata;
		msgdata = psri->serialize();
		int msglen;
		msglen = psri->getSerializedLength();

		mySession->send(psri, Messenger::DEST_RTC);
		delete psri;
	}
}

void ConfigurationManager::handlePrinterInfo(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Printer Info Message");

	if (!msg)
		return;

	HfcPrinterSystemConfig *printerSystemConfig = dynamic_cast<HfcPrinterSystemConfig *> (msg->getHfcObject());

	if (!printerSystemConfig)
		return;

	//deregisterEvent(PrinterSystem, SysInquire, mySession->getMsgQId());		//	Deregister until Aaron fixes the 'Temporary' crash.

	short myPrinterType = printerSystemConfig->getPrinterType();
	if (myPrinterType >= FirstPrinter && myPrinterType < MaxPrinterTypes)
	{
		SessionMessage response(Printer, PrInfo);

		PrinterInfo *printerInfo = new PrinterInfo();
		if (printerInfo)
		{
			msg->setHfcObjectCreatedInternally(false);
			printerInfo->setPrinterSystemConfig(printerSystemConfig);
			printerInfo->setStationAddress(printerSystemConfig->PrinterSystemConfiguration::getStationAddress());
			printerInfo->setPrinterType((PrinterType) myPrinterType);

			if (mySession)
			{
				response.setFormatType(BrowserFormat);
				response.setHfcObject(printerInfo);

				const char *msgdata = response.toString();
				int msglen = response.getSerializedLength();
				Debug::trace("ConfigurationManager.response",	HobString("printer info # ") + HobString(Integer::toString(myPrinterType)) +
																	HobString(" response: ") + HobString(Hex::toString(msgdata, msglen)));

				mySession->send(&response, Messenger::DEST_UI);
			}
			//	Save the currentPrinterInfo values.
			setCurrentPrinterInfo(printerInfo, (PrinterType) myPrinterType);
		}
	}
}

void ConfigurationManager::handlePrinterSaveInfo(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Printer Save Info Message");

	if (!msg)
		return;

	PrinterInfo *printerInfo = dynamic_cast<PrinterInfo *> (msg->getHfcObject());

	if (!printerInfo)
		return;

	PrinterType myPrinterType = printerInfo->getPrinterType();
	PrinterInfo *currentPrinterSaveInfo = currentPrinterInfo[myPrinterType];

	if (currentPrinterSaveInfo)
	{
		currentPrinterSaveInfo->update(printerInfo);

		HfcPrinterSystemConfig *printerSystemConfig = currentPrinterSaveInfo->getPrinterSystemConfig();
		if (printerSystemConfig)
		{
			if (printerSystemConfig->PrinterSystemConfiguration::getStationAddress() != currentPrinterSaveInfo->getStationAddress())
			{
				//	Send the PrinterSystemConfig back to the RTC

				if (mySession)
				{
					printerSystemConfig->PrinterSystemConfiguration::setStationAddress(currentPrinterSaveInfo->getStationAddress());
					printerSystemConfig->PrinterSystemConfiguration::setEventType(SysConfig);

					SessionMessage response(PrinterSystem, SysConfig);
					response.setFormatType(InternalFormat);
					response.setHfcObject((HfcObject*) printerSystemConfig);

					const char *msgdata = response.serialize();
					int msglen = response.getSerializedLength();
					Debug::trace("ConfigurationManager.response",	HobString("printer save info response: ") +
																	HobString(Hex::toString(msgdata, msglen)));

					mySession->send(&response, Messenger::DEST_RTC);
				}
			}
		}
	}
}



//********************************************************
//						WRAPPER
//********************************************************

void ConfigurationManager::handleWrapperInquire(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("ConfigurationManager");
	bool traceOnNames = Debug::isTracing("WrapperPackCodeNames");
	HobString msgDebug = "";

	if (traceOn || traceOnNames)
		msgDebug = "handleWrapperInquire():  ";

	if (traceOn)
		Debug::trace("ConfigurationManager", "handling Wrapper Inquire Message");

	if (!msg)
		return;

	HfcWrapperInquire *wrapperInquire = dynamic_cast<HfcWrapperInquire *> (msg->getHfcObject());

	if (!wrapperInquire)
		return;

	if (traceOnNames && currentWrapperConfig)
	{
		Debug::trace("WrapperPackCodeNames", msgDebug + "BEFORE setCurrentWrapperConfig(): ");
		currentWrapperConfig->show();
	}
	//	Save the current values.
	msg->setHfcObjectCreatedInternally(false);
	setCurrentWrapperConfig(wrapperInquire);
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("currentWrapperConfig = ").append(Hex::toString((long long) currentWrapperConfig));
		Debug::trace("ConfigurationManager", msgDebug2);
	}

	if (traceOnNames && currentWrapperConfig)
	{
		Debug::trace("WrapperPackCodeNames", msgDebug + "AFTER setCurrentWrapperConfig(): ");
		currentWrapperConfig->show();
	}
}

void ConfigurationManager::saveConfigToWrapper(WrConfigType configType)
{
	if (!currentWrapperConfig)
		return;

	if (currentWrapperConfig->itHasChanged() && mySession)
	{
		currentWrapperConfig->setEventType(WrConfig);
		currentWrapperConfig->setConfigType(configType);

		SessionMessage response(Wrapper, WrConfig);
		response.setFormatType(InternalFormat);
		response.setHfcObject(currentWrapperConfig);

		const char *msgdata = response.serialize();
		int msglen = response.getSerializedLength();
		Debug::trace("ConfigurationManager.response",	HobString("Sending wrapper configuration from Wrapper Settings response: ") +
														HobString(Hex::toString(msgdata, msglen)));

		if (mySession->send(&response, Messenger::DEST_RTC) >= 0)
			currentWrapperConfig->clearHasChanged();
	}
}

void ConfigurationManager::handleWrapperConfig(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("ConfigurationManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperConfig():  ";
		Debug::trace("ConfigurationManager", "handling a Wrapper Config Message.");
	}

	if (!msg)
		return;

	if (!currentWrapperConfig)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("- currentWrapperConfig is 0. Exiting!");
			Debug::trace("ConfigurationManager", msgDebug2);
		}
	}

	HfcWrapperConfig *wrapperConfig = dynamic_cast<HfcWrapperConfig *> (msg->getHfcObject());
	if (!wrapperConfig)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("- wrapperConfig is 0. Exiting!");
			Debug::trace("ConfigurationManager", msgDebug2);
		}
		return;
	}

	if (currentWrapperConfig->update(*wrapperConfig) == true)
	{
		currentWrapperConfig->setEventType(WrConfig);

		SessionMessage response(Wrapper, WrConfig);
		response.setFormatType(InternalFormat);
		response.setHfcObject(currentWrapperConfig);

		const char *msgdata = response.serialize();
		int msglen = response.getSerializedLength();
		Debug::trace("ConfigurationManager.response",	HobString("wrapper configuration response: ") +
														HobString(Hex::toString(msgdata, msglen)));

		mySession->send(&response, Messenger::DEST_RTC);
	}
}

void ConfigurationManager::handleWrapperResetErrorTotals()
{
	Debug::trace("ConfigurationManager", "handling Wrapper Reset Error Totals Message");

	if (!currentWrapperConfig)
		return;

	currentWrapperConfig->resetErrorTotals();

	if (mySession)
	{
	//	Send the Configuration to the Wrapper.
		currentWrapperConfig->setEventType(WrConfig);

		SessionMessage response(Wrapper, WrConfig);
		response.setFormatType(InternalFormat);
		response.setHfcObject(currentWrapperConfig);

		const char *msgdata = response.serialize();
		int msglen = response.getSerializedLength();
		Debug::trace("ConfigurationManager.response",	HobString("wrapper configuration response: ") +
														HobString(Hex::toString(msgdata, msglen)));

		mySession->send(&response, Messenger::DEST_RTC);

	//	Now send the HfcWrapperErrorTotals back to the Browser.
		HfcWrapperErrorTotals *wrapperErrorTotals = new HfcWrapperErrorTotals(currentWrapperConfig);
		if (wrapperErrorTotals)
		{
			SessionMessage settings(Wrapper, WrErrorTotals);
			settings.setFormatType(BrowserFormat);
			settings.setHfcObject(wrapperErrorTotals);

			mySession->send(&settings, Messenger::DEST_UI);
			delete wrapperErrorTotals;
		}
	}
}

void ConfigurationManager::handleWrapperResetPackageTotals()
{
	Debug::trace("ConfigurationManager", "handling Wrapper Reset Package Totals Message");

	if (!currentWrapperConfig)
		return;

	currentWrapperConfig->resetPackageTotals();

	if (mySession)
	{
	//	Send the Configuration to the Wrapper.
		currentWrapperConfig->setEventType(WrConfig);

		SessionMessage response(Wrapper, WrConfig);
		response.setFormatType(InternalFormat);
		response.setHfcObject(currentWrapperConfig);

		const char *msgdata = response.serialize();
		int msglen = response.getSerializedLength();
		Debug::trace("ConfigurationManager.response",	HobString("wrapper configuration response: ") +
														HobString(Hex::toString(msgdata, msglen)));

		mySession->send(&response, Messenger::DEST_RTC);

	//	Now send the HfcWrapperPackageTotals back to the Browser.
		HfcWrapperPackageTotals *wrapperPackageTotals = new HfcWrapperPackageTotals(currentWrapperConfig);
		if (wrapperPackageTotals)
		{
			SessionMessage settings(Wrapper, WrPackageTotals);
			settings.setFormatType(BrowserFormat);
			settings.setHfcObject(wrapperPackageTotals);

			mySession->send(&settings, Messenger::DEST_UI);
			delete wrapperPackageTotals;
		}
	}
}


void ConfigurationManager::handleWrapperWritePackCode(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Wrapper Write Pack Code Message");

	if (!msg || !currentWrapperConfig)
		return;

	HfcWrapperPackCodeInfo *newPackCode = dynamic_cast<HfcWrapperPackCodeInfo *> (msg->getHfcObject());
	if (!newPackCode)
		return;

	currentWrapperConfig->update(newPackCode);
	saveConfigToWrapper(WrTemporary);
}

void ConfigurationManager::handleWrapperSavePackCode(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling Wrapper Save Pack Code Message");

	if (!msg || !currentWrapperConfig)
		return;

	HfcWrapperPackCodeInfo *newPackCode = dynamic_cast<HfcWrapperPackCodeInfo *> (msg->getHfcObject());

	if (newPackCode)
		currentWrapperConfig->update(newPackCode);
	saveConfigToWrapper(WrPermanent);
}

void ConfigurationManager::handleWrapperResetConfig()
{
	Debug::trace("ConfigurationManager", "handling Wrapper Configuration - Reset to Factory Defaults Message");

 	HfcWrapperConfig *wrapperConfig =  new HfcWrapperConfig();

	if (!wrapperConfig)
		return;

	if (mySession)
	{
		wrapperConfig->setEventType(WrConfig);
		wrapperConfig->setConfigType(WrDefault);

		SessionMessage resetConfig(Wrapper, WrConfig);
		resetConfig.setFormatType(InternalFormat);
		resetConfig.setHfcObject(wrapperConfig);

		const char *msgdata = resetConfig.serialize();
		int msglen = resetConfig.getSerializedLength();
		Debug::trace("ConfigurationManager.response",	HobString("wrapper configuration - Reset to Factory Defaults response: ") +
														HobString(Hex::toString(msgdata, msglen)));

		mySession->send(&resetConfig, Messenger::DEST_RTC);
		delete wrapperConfig;
		setCurrentWrapperConfig(0);		//	This pointer is no longer valid after the reset so delete it.
	}
}

void ConfigurationManager::handleWrapperSettings(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("ConfigurationManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperSettings():  ";
		Debug::trace("ConfigurationManager", "handling a Wrapper Settings Message.");
	}

	if (!msg)
		return;

	HfcWrapperSettings *wrapperSettings = dynamic_cast<HfcWrapperSettings *> (msg->getHfcObject());

	if (!wrapperSettings)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("- wrapperSettings is 0. Exiting!");
			Debug::trace("ConfigurationManager", msgDebug2);
		}
		return;
	}
	if (Debug::isTracing("ConfigurationManager.WrapperSettings"))
	{
		HobString msgDebug2 = "Show wrapperSettings --------------  ";
		Debug::trace("ConfigurationManager.WrapperSettings", msgDebug + msgDebug2 + "START");
		wrapperSettings->show();
		Debug::trace("ConfigurationManager.WrapperSettings", msgDebug + msgDebug2 + " END");
	}

	//	Copy the HfcWrapperSettings into an HfcWrapperControl object that we can keep track of.
	HfcWrapperControl *myWrControl = new HfcWrapperControl(wrapperSettings);
	if (Debug::isTracing("ConfigurationManager.WrapperControl"))
	{
		HobString msgDebug2 = "Show wrapperControl --------------  ";
		Debug::trace("ConfigurationManager.WrapperControl", msgDebug + msgDebug2 + "START");
		myWrControl->show();
		Debug::trace("ConfigurationManager.WrapperControl", msgDebug + msgDebug2 + " END");
	}

	//	Save the current values.
	setCurrentWrapperControl(myWrControl);

	//	Let the Session determine what messages to send to the UI.
}

void ConfigurationManager::handleWrapperStatus(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("ConfigurationManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperStatus():  ";
		Debug::trace("ConfigurationManager", "handling a Wrapper Status Message.");
	}

	if (!msg)
		return;

	HfcWrapperStatus *wrapperStatus = dynamic_cast<HfcWrapperStatus *> (msg->getHfcObject());

	if (!wrapperStatus)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("- wrapperStatus is 0. Exiting!");
			Debug::trace("ConfigurationManager", msgDebug2);
		}
		return;
	}
	if (Debug::isTracing("ConfigurationManager.WrapperStatus"))
	{
		HobString msgDebug2 = "Show wrapperStatus --------------  ";
		Debug::trace("ConfigurationManager.WrapperStatus", msgDebug + msgDebug2 + "START");
		wrapperStatus->show();
		Debug::trace("ConfigurationManager.WrapperStatus", msgDebug + msgDebug2 + " END");
	}

	//	Update the currentWrapperControl with the latest settings.
	if (currentWrapperControl)
	{
		currentWrapperControl->update(wrapperStatus);
		if (Debug::isTracing("ConfigurationManager.WrapperControl"))
		{
			HobString msgDebug2 = "Show updated currentWrapperControl --------------  ";
			Debug::trace("ConfigurationManager.WrapperControl", msgDebug + msgDebug2 + "START");
			currentWrapperControl->show();
			Debug::trace("ConfigurationManager.WrapperControl", msgDebug + msgDebug2 + " END");
		}
	}
}

void ConfigurationManager::handleWrapperControl(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("ConfigurationManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperControl():  ";
		Debug::trace("ConfigurationManager", "handling a Wrapper Control Message.");
	}

	if (!msg || !mySession)
		return;

	if (!currentWrapperControl)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("- currentWrapperControl is 0. Exiting!");
			Debug::trace("ConfigurationManager", msgDebug2);
		}
		return;
	}

	HfcWrapperControl *wrControl = dynamic_cast<HfcWrapperControl *> (msg->getHfcObject());
	if (!wrControl)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("- wrControl is 0. Exiting!");
			Debug::trace("ConfigurationManager", msgDebug2);
		}
		return;
	}

	bool settingChanged, updateUI = false;
	bool controlChanged = currentWrapperControl->update(wrControl, settingChanged);
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("controlChanged = ").append(Bool::toString(controlChanged));
		msgDebug2.append("   settingChanged = ").append(Bool::toString(settingChanged));
		Debug::trace("ConfigurationManager", msgDebug2);
	}
	if (settingChanged && currentWrapperConfig)
	{
		//	Validate the max value of the belt setting.
		int maxValue;
		if (currentWrapperConfig->isCelsiusEnable())
			maxValue = 190;
		else
			maxValue = 375;

		if (currentWrapperControl->getHeaterSetting() > maxValue)
		{
			currentWrapperControl->setHeaterSetting(maxValue);
			updateUI = true;
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Reducing the beltSetting to ").append(Integer::toString(currentWrapperControl->getHeaterSetting()));
				msgDebug2.append(". Don't forget to update the UI!");
				Debug::trace("ConfigurationManager", msgDebug2);
			}
		}
	}
	if (controlChanged)
	{
		if (Debug::isTracing("ConfigurationManager.WrapperControl"))
		{
			Debug::trace("ConfigurationManager.WrapperControl", "-----#####  currentWrapperControl  #####-----");
			currentWrapperControl->show();
			Debug::trace("ConfigurationManager.WrapperControl", "-----#####           end           #####-----");
		}
	}

	if (controlChanged)
	{
		SessionMessage response(Wrapper, WrControl);
		response.setFormatType(InternalFormat);
		response.setHfcObject(currentWrapperControl);

		if (traceOn)
		{
			const char *msgdata = response.serialize();
			int msglen = response.getSerializedLength();

			HobString msgDebug2 = msgDebug;
			msgDebug2.append("WrControl response to DEST_RTC: ").append(Hex::toString(msgdata, msglen));
			Debug::trace("ConfigurationManager", msgDebug2);
		}

		mySession->send(&response, Messenger::DEST_RTC);
	}

	if (updateUI)
	{
		SessionMessage response(Wrapper, WrReqSettings);
		response.setFormatType(InternalFormat);

		mySession->send(&response, Messenger::DEST_RTC);
	}
}


void ConfigurationManager::handleReloadUltimaWrapperControls(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("ConfigurationManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleReloadUltimaWrapperControls()";
		Debug::trace("ConfigurationManager", msgDebug);
		msgDebug.append(":  ");
	}

	if (!msg || !mySession)
		return;

	if (!currentWrapperControl)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("currentWrapperControl is 0. Exiting!");
			Debug::trace("ConfigurationManager", msgDebug2);
		}
		return;
	}

	GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
	if (!msgData)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("msgData is 0. Exiting!");
			Debug::trace("ConfigurationManager", msgDebug2);
		}
		return;
	}

	HobString temp(msgData->getData().c_str(), msgData->getData().length());
	HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
	HobString token;
	int wrKind = -1, intakeMode = -1;
	bool intakeChanged = false;

	if (tok.next(&token) != -1)
		wrKind = Integer::valueOf(token);
	if (tok.next(&token) != -1)
	{
		intakeMode = Integer::valueOf(token.c_str());
		if (intakeMode == WrapperIntakeMode::DefaultIntake)
		{
			intakeMode = (WrapperIntakeMode::IntakeMode) Configuration::getInstance()->getDefaultIntakeMode();
		}
		if (intakeMode != currentWrapperControl->getIntakeMode())
		{
			intakeChanged = true;
			currentWrapperControl->setIntakeMode((WrapperIntakeMode::IntakeMode) intakeMode);
		}
	}
	if (traceOn)
	{
		HobString msgDebug3 = msgDebug;
		msgDebug3.append("Original data = [").append(msgData->getData()).append("]");
		Debug::trace("ConfigurationManager", msgDebug3);

		HobString msgDebug2 = msgDebug;
		msgDebug2.append("wrKind = ").append(Integer::toString(wrKind));
		msgDebug2.append("  intakeMode = ").append(Integer::toString(intakeMode));
		msgDebug2.append("  intakeChanged = ").append(Bool::toString(intakeChanged));
		Debug::trace("ConfigurationManager", msgDebug2);
	}

	if (Debug::isTracing("ConfigurationManager.WrapperControl"))
	{
		Debug::trace("ConfigurationManager.WrapperControl", "-----#####  currentWrapperControl  #####-----");
		currentWrapperControl->show();
		Debug::trace("ConfigurationManager.WrapperControl", "-----#####           end           #####-----");
	}

	if (intakeChanged)
	{
		SessionMessage response(Wrapper, WrControl);
		response.setFormatType(InternalFormat);
		response.setHfcObject(currentWrapperControl);

		if (traceOn)
		{
			const char *msgdata = response.serialize();
			int msglen = response.getSerializedLength();

			HobString msgDebug2 = msgDebug;
			msgDebug2.append("WrControl sent to DEST_RTC: ").append(Hex::toString(msgdata, msglen));
			Debug::trace("ConfigurationManager", msgDebug2);
		}

		mySession->send(&response, Messenger::DEST_RTC);
	}
}

//********************************************************
//						EVOLUTION WRAPPER
//********************************************************

void ConfigurationManager::handleEvolWrapperSettings(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("ConfigurationManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleEvolWrapperSettings():  ";
		Debug::trace("ConfigurationManager", "handling an Evol Wrapper Settings Message.");
	}

	if (!msg)
		return;

	HfcEvolWrapperSettings *wrapperSettings = dynamic_cast<HfcEvolWrapperSettings *> (msg->getHfcObject());

	if (!wrapperSettings)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("- wrapperSettings is 0. Exiting!");
			Debug::trace("ConfigurationManager", msgDebug2);
		}
		return;
	}
	if (Debug::isTracing("ConfigurationManager.EvolWrapperSettings"))
	{
		HobString msgDebug2 = "Show wrapperSettings --------------  ";
		Debug::trace("ConfigurationManager.EvolWrapperSettings", msgDebug + msgDebug2 + "START");
		wrapperSettings->show();
		Debug::trace("ConfigurationManager.EvolWrapperSettings", msgDebug + msgDebug2 + " END");
	}
	//	Let the Session determine what messages to send to the UI.
}

void ConfigurationManager::handleReloadEvolWrapperControls(SessionMessage *msg)
{
	bool traceOn = Debug::isTracing("ConfigurationManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleReloadEvolWrapperControls()";
		Debug::trace("ConfigurationManager", msgDebug);
		msgDebug.append(":  ");
	}

	if (!msg || !mySession)
		return;

	if (!currentEvolWrapperControl)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("currentEvolWrapperControl is 0. Exiting!");
			Debug::trace("ConfigurationManager", msgDebug2);
		}
		return;
	}

	GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
	if (!msgData)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("msgData is 0. Exiting!");
			Debug::trace("ConfigurationManager", msgDebug2);
		}
		return;
	}

	HobString temp(msgData->getData().c_str(), msgData->getData().length());
	HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
	HobString token;

	if (tok.next(&token) != -1)
		currentEvolWrapperControl->setIntakeMode((WrapperIntakeMode::IntakeMode) (Integer::valueOf(token.c_str())));
	else
		currentEvolWrapperControl->setIntakeMode((WrapperIntakeMode::IntakeMode) Configuration::getInstance()->getDefaultIntakeMode());

	currentEvolWrapperControl->setFilmTension((WrFilmTension) Configuration::getInstance()->getDefaultFilmTension());
	currentEvolWrapperControl->setFilmTensionFrontToBack(Configuration::getInstance()->getDefaultFilmTensionFrontToBack());
	currentEvolWrapperControl->setFilmTensionRightToLeft(Configuration::getInstance()->getDefaultFilmTensionLeftToRight());

	if (Debug::isTracing("ConfigurationManager.EvolWrapperControl"))
	{
		Debug::trace("ConfigurationManager.EvolWrapperControl", "-----88888  currentEvolWrapperControl  88888-----");
		currentEvolWrapperControl->show();
		Debug::trace("ConfigurationManager.EvolWrapperControl", "-----88888            end              88888-----");
	}

	SessionMessage response(Wrapper, WrEvolControl);
	response.setFormatType(InternalFormat);
	response.setHfcObject(currentEvolWrapperControl);

	if (traceOn)
	{
		const char *msgdata = response.serialize();
		int msglen = response.getSerializedLength();

		HobString msgDebug2 = msgDebug;
		msgDebug2.append("WrEvolControl sent to DEST_RTC: ").append(Hex::toString(msgdata, msglen));
		Debug::trace("ConfigurationManager", msgDebug2);
	}

	mySession->send(&response, Messenger::DEST_RTC);
}


//********************************************************
//				Ultima or Evolution wrapper
//********************************************************

void ConfigurationManager::handleProductReloadWrapperControls(SessionMessage *msg)
{
	//	This is called as a result of a new Product in Operate mode.
	bool traceOn = Debug::isTracing("ConfigurationManager");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleProductReloadWrapperControls()";
		Debug::trace("ConfigurationManager", msgDebug);
	}

	if (!msg || !mySession)
		return;

	GenericHfcObject *msgData = dynamic_cast<GenericHfcObject *> (msg->getHfcObject());
	if (msgData)
	{
		HobString temp(msgData->getData().c_str(), msgData->getData().length());
		HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
		HobString token;
		if (tok.next(&token) != -1)
		{
			if (SessionManager::getInstance()->isEvolutionWrapperAttached())
				handleReloadEvolWrapperControls(msg);
		}
	}
}


//********************************************************
//						CONFIGURATION DEV
//********************************************************
void ConfigurationManager::handleReadAllConfigMsgs()
{
	/* We probably want to prevent this code from being executed for non user interface sessions. Thus: */

	Session * s = dynamic_cast<Session*> (mySession);
	Configuration *cfg = Configuration::getInstance();

	if ( s && cfg && (s->isPrimarySession() || s->isRemoteSession() || s->isIntranetSession()) )
	{
		SessionMessage tempMsg(ConfigurationDev, CfgScaleRead);
		tempMsg.setFormatType(BrowserFormat);
		handleConfigurationDevMsg(&tempMsg);

		tempMsg.setEventType(CfgCurrencyRead);
		handleConfigurationDevMsg(&tempMsg);

		tempMsg.setEventType(CfgOperatorAccessRead);
		handleConfigurationDevMsg(&tempMsg);

		tempMsg.setEventType(CfgFrequentShopperRead);
		handleConfigurationDevMsg(&tempMsg);

		if ( s->isPrimarySession() )
			cfg->loadStoreGraphic();

		if ( cfg->isPrepackScale() || cfg->isServiceCounterScale() )
		{
			tempMsg.setEventType(CfgPrepackRead);
			handleConfigurationDevMsg(&tempMsg);
		}

/*
		tempMsg.setDeviceType(Printer);
		tempMsg.setEventType(PrReadAllExternalPrinters);
		s->handleMessage(&tempMsg);

		tempMsg.setDeviceType(Printer);
		tempMsg.setEventType(PrReadAllProductTextCfgs);
		s->handleMessage(&tempMsg);
*/
		// Just assume the InactivityAutoLogoutTime has changed; reset the Session's autoLogout timer.
		s->manageAutoLogoutTimer();

		// Configuration has changed; write the database back to flash NOW.
		if ( s->isPrimarySession() )
		{
			s->backupDatabase("CfgSaved");

			// Assume the Permanent Marquee has changed, so send it to the Marquee Device.
			HobString mqDesc = "";
			if ( cfg->isPermanentMarqueeDisplayed() )
			{
				// Permanent Marquee would be in dept 0, not the currently active Dept
				Util::CSmartPtr<ProductText> mqText(ProductText::findRecord(psttMarqueeText, cfg->getPermanentMarqueeNumber(), 0, dbAccess));
				if ( mqText.notNull() )
					mqDesc = mqText->getText();
			}
			MarqueeUpdate mqUpdate(MqPermanent, mqDesc.c_str());
			s->send(&mqUpdate, Messenger::DEST_RTC);
		}
		if ( cfg->isWeigherAttached() )
		{
			s->registerEvent(Weigher, WgStatus);
			// any others we need to register for?
		}
		else
		{
			s->deleteCurrentWeight();
			s->deregisterEvent(Weigher, WgStatus);
			// any others we need to deregister for?
		}

		if ( s->isPrimarySession() )
		{
			// Check if the offline beep timer interval changed.  If the timer exists, then delete the timer if the interval time was changed to 0, or reset
			// the timer with the configured interval.
			TimerManager *timerManager = TimerManager::getInstance();
			if (timerManager)
			{
				Timer *timer = timerManager->getTimer(s->getId(), timedFuncOfflineBeep);
				if (timer)
				{
					int beepTime = cfg->toIntCfg(fieldOffsetCfgPeriodicOfflineBeep);
					if ( 0 == beepTime )
						timerManager->removeTimer(timer->getTimerId());
					else if ( beepTime != timer->getSignalTime() )
						timerManager->resetTimer(timer->getTimerId(), beepTime);
				}
			}
		}
	}
}

void ConfigurationManager::handleConfigurationDevMsg(SessionMessage *msg)
{
	Debug::trace("ConfigurationManager", "handling ConfigurationDev Message");

	if (!msg)
		return;

	switch ((ConfigurationEvent) msg->getEventType())
	{
		// Some Session has made changes to, and saved, the configuration in the database.
		// Reread it and sent it to the UI.
		case CfgScaleRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgScaleWrite);
				response.setFormatType(BrowserFormat);
				CfgScale myConfig;
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgScaleWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgScaleWrite Message");
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgScaleSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgScaleSave Message");
			bool synchronizationNeeded = false;
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgScale newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case DtReadSingleField:
		{
			Debug::trace("ConfigurationManager", "handling DtReadSingleField Message");
			WriteSingleField *configUpdate = dynamic_cast<WriteSingleField *> (msg->getHfcObject());
			if (configUpdate)
				handleCfgReadSingleFieldMessage(configUpdate);
			break;
		}

		case DtWriteSingleField:
		{
			Debug::trace("ConfigurationManager", "handling DtWriteSingleField Message");
			WriteSingleField *configUpdate = dynamic_cast<WriteSingleField *> (msg->getHfcObject());
			if (configUpdate)
				handleCfgWriteSingleFieldMessage(configUpdate);
			break;
		}

		case DtSaveSingleField:
		{
			Debug::trace("ConfigurationManager", "handling DtSaveSingleField Message");
			WriteSingleField *configUpdate = dynamic_cast<WriteSingleField *> (msg->getHfcObject());
			if (configUpdate)
				handleCfgSaveSingleFieldMessage(configUpdate);
			break;
		}

		case CfgFrequentShopperRead:
		{
			Debug::trace("ConfigurationManager", "handling CfgFrequentShopperRead Message");

			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgFrequentShopperWrite);
				response.setFormatType(BrowserFormat);
				CfgFrequentShopper fsConfig;
				response.setHfcObject(&fsConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgFrequentShopperWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgFrequentShopperWrite Message");
			CfgFrequentShopper *cfg = dynamic_cast<CfgFrequentShopper *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgFrequentShopperSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgFrequentShopperSave Message");
			bool synchronizationNeeded = false;
			CfgFrequentShopper *cfg = dynamic_cast<CfgFrequentShopper *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgFrequentShopper newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case CfgGCDRead:
		{
			Debug::trace("ConfigurationManager", "handling CfgGCDRead Message");

			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgGCDWrite);
				response.setFormatType(BrowserFormat);
				CfgGCD gcdConfig;
				response.setHfcObject(&gcdConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgGCDWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgGCDWrite Message");
			CfgGCD *cfg = dynamic_cast<CfgGCD *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgGCDSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgGCDSave Message");
			bool synchronizationNeeded = false;
			CfgGCD *cfg = dynamic_cast<CfgGCD *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgGCD newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case CfgScaleOperationRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgScaleOperationWrite);
				response.setFormatType(BrowserFormat);
				CfgScale myConfig;
				myConfig.setFormatType(NormalScaleOpFormat);
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgScaleOperationWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgScaleOperationWrite Message");
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgScaleOperationSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgScaleOperationSave Message");
			bool synchronizationNeeded = false;
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgScale newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
					
					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();

			break;
		}

		case CfgOperatorAccessRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgOperatorAccessWrite);
				response.setFormatType(BrowserFormat);
				CfgOperatorAccess myConfig;
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgOperatorAccessWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgOperatorAccessWrite Message");
			CfgOperatorAccess *cfg = dynamic_cast<CfgOperatorAccess *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgOperatorAccessSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgOperatorAccessSave Message");
			bool synchronizationNeeded = false;
			CfgOperatorAccess *cfg = dynamic_cast<CfgOperatorAccess *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgOperatorAccess newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
					
					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case CfgPrintedFieldsRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgPrintedFieldsWrite);
				response.setFormatType(BrowserFormat);
				CfgPrintedFields myConfig;
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgPrintedFieldsWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgPrintedFieldsWrite Message");
			CfgPrintedFields *cfg = dynamic_cast<CfgPrintedFields *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgPrintedFieldsSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgPrintedFieldsSave Message");
			bool synchronizationNeeded = false;
			CfgPrintedFields *cfg = dynamic_cast<CfgPrintedFields *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgPrintedFields newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case CfgSupportedLabelLengthsRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgSupportedLabelLengthsWrite);
				response.setFormatType(BrowserFormat);
				CfgSupportedLabelLengths myConfig;
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}


		case CfgSupportedLabelLengthsWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgSupportedLabelLengthsWrite Message");
			CfgSupportedLabelLengths *cfg = dynamic_cast<CfgSupportedLabelLengths *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgSupportedLabelLengthsSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgSupportedLabelLengthsSave Message");
			bool saveNeeded = false;
			CfgSupportedLabelLengths *cfg = dynamic_cast<CfgSupportedLabelLengths *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				saveNeeded = cfg->isSaveNeeded();
			}
			if (saveNeeded)
			{
				Configuration::getInstance()->save(dbAccess);

				dispatchConfigurationSaved();
				PrinterStockMetrics::initStatics();
			}
			break;
		}

		case CfgGenericTextReadAll:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgGenericTextReadAll);
				response.setFormatType(BrowserFormat);
				CfgGenericLabelText *cfgGenericText = Configuration::getInstance()->getGenericLabelText();
				cfgGenericText->setFormatType(AllGenericTextFormat);
				response.setHfcObject(cfgGenericText);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgGenericTextUpdated:
		{
			Debug::trace("ConfigurationManager", "handling CfgGenericTextUpdated Message");
			CfgGenericLabelText *cfgGenericText = Configuration::getInstance()->getGenericLabelText();
			cfgGenericText->reloadLabelText(dbAccess);

			dispatchConfigurationSaved();
			break;
		}

		case CfgStoreInfoRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgStoreInfoWrite);
				response.setFormatType(BrowserFormat);
				CfgStoreInfo myConfig;
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgStoreInfoWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgStoreInfoWrite Message");
			CfgStoreInfo *cfg = dynamic_cast<CfgStoreInfo *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgStoreInfoSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgStoreInfoSave Message");
			bool synchronizationNeeded = false;
			CfgStoreInfo *cfg = dynamic_cast<CfgStoreInfo *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgStoreInfo newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg, CfgStoreInfoWrite);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
					
					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), CfgStoreInfoWrite, synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case CfgStoreInfoRestore:
		{
			Debug::trace("ConfigurationManager", "handling CfgStoreInfoRestore Message");
			CfgStoreInfo *cfg = dynamic_cast<CfgStoreInfo *> (msg->getHfcObject());
			if (cfg)
			{
				if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					// batch the record being restored; don't actually write it to the database right now.
					// This record does not contain any fields that affect synchronization type, or fields that are not synchronized;
					// so it's ok to just batch the entire record, like other datatypes do.

					Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

					if ( msg->getFormatType() == UltimaTaggedFormat )
					{
						std::string reserialization;
						serializeForSynchronization(reserialization, cfg);

						receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), reserialization);
					}
					else
						receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
					if ( proxy )
						proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);
				}
				else
				{
					cfg->updateConfiguration();
					Configuration::getInstance()->save(dbAccess);
					dispatchConfigurationSaved();
				}
			}
			break;
		}

		case CfgCurrencyRead:
		{
			if ( mySession )
			{
				SessionMessage response(ConfigurationDev, CfgCurrencyWrite);
				CfgCurrency myConfig;
				myConfig = Configuration::getInstance()->getCfgCurrency();

				response.setFormatType(BrowserFormat);
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgCurrencyWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgCurrencyWrite Message");
			CfgCurrency *cfg = dynamic_cast<CfgCurrency *>(msg->getHfcObject());
			if (cfg)
				Configuration::getInstance()->update(cfg);
			break;
		}

		case CfgCurrencySave:
		{
			Debug::trace("ConfigurationManager", "handling CfgCurrencySave Message");
			CfgCurrency *cfg = dynamic_cast<CfgCurrency *>(msg->getHfcObject());
			if ( cfg )
				Configuration::getInstance()->update(cfg);

			bool synchronizationNeeded = Configuration::getInstance()->getCfgCurrency().isSaveNeeded();

			Configuration::getInstance()->save(dbAccess, (CfgCurrency *)0);

			CfgCurrency newCfg;
			newCfg = Configuration::getInstance()->getCfgCurrency();
			if ( synchronizationNeeded )
			{
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), CfgCurrencyWrite, synchSer)));
				}
			}

			dispatchConfigurationSaved();

			break;
		}

		case CfgSelfServiceRead:
		{
			Debug::trace("ConfigurationManager", "handling CfgSelfServiceRead Message");
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgSelfServiceWrite);
				response.setFormatType(BrowserFormat);
				CfgSelfService myConfig;
				myConfig = Configuration::getInstance()->getCfgSelfService();
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgSelfServiceWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgSelfServiceWrite Message");
			CfgSelfService *cfg = dynamic_cast<CfgSelfService *> (msg->getHfcObject());
			if (cfg)
				Configuration::getInstance()->update(cfg);
			break;
		}

		case CfgSelfServiceSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgSelfServiceSave Message");
			CfgSelfService *cfg = dynamic_cast<CfgSelfService *> (msg->getHfcObject());
			if (cfg)
				Configuration::getInstance()->update(cfg);

			bool synchronizationNeeded = Configuration::getInstance()->getCfgSelfService().isSaveNeeded();

			Configuration::getInstance()->save(dbAccess, (CfgSelfService *)0);

			CfgSelfService newCfg;
			newCfg = Configuration::getInstance()->getCfgSelfService();
			if (synchronizationNeeded)
			{
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
					
					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			//dispatchConfigurationSaved();
			break;
		}

		case CfgMarqueeRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgMarqueeWrite);
				response.setFormatType(BrowserFormat);
				CfgMarquee myConfig;
				myConfig = Configuration::getInstance()->getCfgMarquee();
				std::string permanentMarqueeText;
				// Permanent Marquee would be in dept 0, not the currently active Dept
				Util::CSmartPtr<ProductText> permanentMarquee(ProductText::findRecord(psttMarqueeText, myConfig.getPermanentMarqueeNumber(), 0, dbAccess));
				if (( permanentMarquee.notNull() ) && ( RECORD_NOT_FOUND != permanentMarquee->getTextNum() ) && ( RECORD_OK == permanentMarquee->getErrorCode() ))
					permanentMarqueeText = permanentMarquee->getText();
				myConfig.setPermanentMarquee(permanentMarqueeText);
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgMarqueeWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgMarqueeWrite Message");
			CfgMarquee *cfg = dynamic_cast<CfgMarquee *> (msg->getHfcObject());
			if (cfg)
				Configuration::getInstance()->update(cfg);
			break;
		}

		case CfgMarqueeSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgMarqueeSave Message");
			CfgMarquee *cfg = dynamic_cast<CfgMarquee *> (msg->getHfcObject());
			if ( cfg )
			{
				Configuration::getInstance()->update(cfg);

				// Check if the permanent marquee message changed
				// Permanent Marquee would be in dept 0, not the currently active Dept
				Util::CSmartPtr<ProductText> permanentMarquee(ProductText::findRecord(psttMarqueeText, Configuration::getInstance()->getPermanentMarqueeNumber(), 0, dbAccess));

				if (( permanentMarquee.null() ) && ( cfg->getPermanentMarquee().size() ))
					permanentMarquee = new ProductText( psttMarqueeText );

				if ( permanentMarquee.notNull() )
				{
					if ( permanentMarquee->getText() != cfg->getPermanentMarquee() )
					{
						permanentMarquee->setText(cfg->getPermanentMarquee());
						permanentMarquee->setTextNum(Configuration::getInstance()->getPermanentMarqueeNumber());
						permanentMarquee->setErrorCode(RECORD_OK);
						permanentMarquee->setDBAccess(dbAccess);

						SessionMessage message(MarqueeTextDev, DtSave);
						message.setHfcObject(permanentMarquee.get());
						mgrContainer_->getProductTextManager(psttMarqueeText)->handleMessage(&message);
					}
				}
			}

			bool synchronizationNeeded = Configuration::getInstance()->getCfgMarquee().isSaveNeeded();

			Configuration::getInstance()->save(dbAccess, (CfgMarquee *)0);

			CfgMarquee newCfg;
			newCfg = Configuration::getInstance()->getCfgMarquee();
			if (synchronizationNeeded)
			{
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
					
					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case DtRestore:
		{
			Debug::trace("ConfigurationManager", "handling DtRestore Message");
			CfgAll *cfg = dynamic_cast<CfgAll *> (msg->getHfcObject());
			if (cfg)
			{
				// First resynchronize the record received with the SynchronizationTaggedFormat format and save to the synchronization changes db file.
				// The same record can then be synchronized across scales (for those config attributes that get synchronized.)
				// The ones that aren't synchronized, those will be processed immediately after writing this record to the synch changes file.

				if (mySession && mySession->isBatching() && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					Util::CSmartPtr<hfc::SynchronizationChange> receivedChange;

//					if ( msg->getFormatType() == UltimaTaggedFormat )
					{
						std::string reserialization;
						serializeForSynchronization(reserialization, cfg);

						receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), reserialization);
					}
//					else
//						receivedChange = new hfc::SynchronizationChange(getDevice(), getWriteEvent(), msg->getOriginalBuff());

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
					if ( proxy )
						proxy->addRecordToBatch(mySession->getBatchId(), receivedChange);

					// Now we also need to immediately write any configuration attributes we received that are scale specific (aren't synchronized).

					Debug::trace("ConfigurationManager", "processing CfgScale ...");
					//const_cast<CfgScale *>(cfg->getCfgScalePtr())->show();
// todo lap					Configuration::getInstance()->save(dbAccess, cfg->getCfgScalePtr(), true);
				}
				else
				{
					// We aren't batching, so save the config attributes to the database. This would happen if we are a standalone,
					// or if we received this configuration restore record via synchronization.  Make sure that when we save these values,
					// they don't get re-batched.  That is why I'm now calling restore on each Cfg class.

					Configuration *config = Configuration::getInstance();
					if (!config)
						break;

					if (cfg)
						cfg->updateConfiguration();

					// This saves the changes received from CfgScale, CfgFrequentShopper, CfgOperatorAccess, CfgPrintedFields, CfgPrepack, and CfgSynchronization
					config->save(dbAccess);

					// SOF-5414 addendum - Need to reload synchronization configuration flags after restoring a CfgAll (RTB9), in case it contained synchronization flags,
					//                     as is the case when restoring the database after an upgrade (verIndData).
					hfc::HFCManagerContainer::updateSyncList();

					Debug::trace("ConfigurationManager", "processing CfgSelfService ...");
					//const_cast<CfgSelfService *>(cfg->getCfgSelfServicePtr())->show();
					config->save(dbAccess, cfg->getCfgSelfServicePtr());

					Debug::trace("ConfigurationManager", "processing CfgEmail ...");
					//const_cast<CfgEmail *>(cfg->getCfgEmailPtr())->show();
					config->save(dbAccess, cfg->getCfgEmailPtr());

					Debug::trace("ConfigurationManager", "processing CfgCommInterface ...");
					config->save(dbAccess, cfg->getCfgCommInterfacePtr());

					Debug::trace("ConfigurationManager", "processing CfgCurrency ...");
					config->save(dbAccess, cfg->getCfgCurrencyPtr());

					Debug::trace("ConfigurationManager", "processing CfgNetwork ...");
					config->save(dbAccess, cfg->getCfgNetworkPtr());

					Debug::trace("ConfigurationManager", "processing CfgMarquee ...");
					//const_cast<CfgMarquee *>(cfg->getCfgMarqueePtr())->show();
					config->save(dbAccess, cfg->getCfgMarqueePtr());

					bool traceOn = Debug::isTracing("ConfigurationManager.EAN");
					if (traceOn)
					{
						HobString msgDebug = "cfg->isReceived(fieldOffsetCfgRwEanFormat) = ";
						msgDebug.append(Bool::toString(cfg->isReceived(fieldOffsetCfgRwEanFormat)));
						msgDebug.append("  cfg->getRwEanFormat().empty() = ").append(Bool::toString(cfg->getRwEanFormat().empty()));
						msgDebug.append("  cfg->getRwEan128cFormat().empty() = ").append(Bool::toString(cfg->getRwEan128cFormat().empty() ));
						msgDebug.append("  cfg->isReceived(fieldOffsetCfgRwEanPrefix) = ").append(Bool::toString(cfg->isReceived(fieldOffsetCfgRwEanPrefix)));
						msgDebug.append("  cfg->getRwEanPrefix().empty() = ").append(Bool::toString(cfg->getRwEanPrefix().empty()));
						Debug::trace("ConfigurationManager.EAN", msgDebug);
					}
					if ((cfg->isReceived(fieldOffsetCfgRwEanFormat) && !cfg->getRwEanFormat().empty()) ||
						(cfg->isReceived(fieldOffsetCfgRwEan128cFormat) && !cfg->getRwEan128cFormat().empty()) ||		// ALV
						(cfg->isReceived(fieldOffsetCfgRwEanPrefix) && !cfg->getRwEanPrefix().empty()))
					{
						ProductDefault *productDefault = ProductDefault::findRecord(Product::PRODUCTTYPE_RANDOMWEIGHT, Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess);
						if (productDefault)
						{
							if (!cfg->getRwEanFormat().empty())
							{
								productDefault->setFormatString(EAN13, cfg->getRwEanFormat());
								if (traceOn)
									Debug::trace("ConfigurationManager.EAN", "Updated the RandomWeight EAN13 Format String in ProductDefaults.");
							}
							// ALV
							if (!cfg->getRwEan128cFormat().empty())
							{
								productDefault->setFormatString(EAN128C, cfg->getRwEan128cFormat());
								if (traceOn)
									Debug::trace("ConfigurationManager.EAN", "Updated the RandomWeight EAN128C Format String in ProductDefaults.");
							}
							if (!cfg->getRwEanPrefix().empty())
							{
								productDefault->setEAN13PrefixString(cfg->getRwEanPrefix());
								if (traceOn)
									Debug::trace("ConfigurationManager.EAN", "Updated the RandomWeight EAN13 Prefix String in ProductDefaults.");
							}
							productDefault->saveRecord();
							delete productDefault;
						}
					}

					if ((cfg->isReceived(fieldOffsetCfgFwEanFormat) && !cfg->getFwEanFormat().empty()) ||
						(cfg->isReceived(fieldOffsetCfgFwEan128cFormat) && !cfg->getFwEan128cFormat().empty()) ||
						(cfg->isReceived(fieldOffsetCfgFwEanPrefix) && !cfg->getFwEanPrefix().empty()))
					{
						ProductDefault *productDefault = ProductDefault::findRecord(Product::PRODUCTTYPE_FIXEDWEIGHT, config->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess);
						if (productDefault)
						{
							if (!cfg->getFwEanFormat().empty())
							{
								productDefault->setFormatString(EAN13, cfg->getFwEanFormat());
								if (traceOn)
									Debug::trace("ConfigurationManager.EAN", "Updated the FixedWeight EAN13 Format String in ProductDefaults.");
							}
							// ALV
							if (!cfg->getFwEan128cFormat().empty())
							{
								productDefault->setFormatString(EAN128C, cfg->getFwEan128cFormat());

								if (traceOn)
									Debug::trace("ConfigurationManager.EAN", "Updated the FixedWeight EAN128C Format String in ProductDefaults.");
							}
							if (!cfg->getFwEanPrefix().empty())
							{
								productDefault->setEAN13PrefixString(cfg->getFwEanPrefix());
								if (traceOn)
									Debug::trace("ConfigurationManager.EAN", "Updated the FixedWeight EAN13 Prefix String in ProductDefaults.");
							}
							productDefault->saveRecord();
							delete productDefault;
						}
					}

					if ((cfg->isReceived(fieldOffsetCfgBcEanFormat) && !cfg->getBcEanFormat().empty()) ||
						(cfg->isReceived(fieldOffsetCfgBcEan128cFormat) && !cfg->getBcEan128cFormat().empty()) ||
						(cfg->isReceived(fieldOffsetCfgBcEanPrefix) && !cfg->getBcEanPrefix().empty()))
					{
						ProductDefault *productDefault = ProductDefault::findRecord(Product::PRODUCTTYPE_BYCOUNT, config->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess);
						if (productDefault)
						{
							if (!cfg->getBcEanFormat().empty())
							{
								productDefault->setFormatString(EAN13, cfg->getBcEanFormat());
								if (traceOn)
									Debug::trace("ConfigurationManager.EAN", "Updated the ByCount EAN13 Format String in ProductDefaults.");
							}
							// ALV
							if (!cfg->getBcEan128cFormat().empty())
							{
								productDefault->setFormatString(EAN128C, cfg->getBcEan128cFormat());
								if (traceOn)
									Debug::trace("ConfigurationManager.EAN", "Updated the ByCount EAN128C Format String in ProductDefaults.");
							}
							if (!cfg->getBcEanPrefix().empty())
							{
								productDefault->setEAN13PrefixString(cfg->getBcEanPrefix());
								if (traceOn)
									Debug::trace("ConfigurationManager.EAN", "Updated the ByCount EAN13 Prefix String in ProductDefaults.");
							}
							productDefault->saveRecord();
							delete productDefault;
						}
					}

					if ( cfg->isReceived(fieldOffsetCfgChoseToUseContinuousStock) )
					{
						Debug::trace("ConfigurationManager", "fieldOffsetCfgChoseToUseContinuousStock received, so setting up ...");
						Session *primarySession = SessionManager::getInstance()->getPrimaryConsoleSession();
						if ( primarySession && mySession )
						{
							if (config->toBoolCfg(fieldOffsetCfgChoseToUseContinuousStock))
							{
								PrinterGeneric prReqUCS(PrUseContinuousStock, mySession, FirstPrinter);
								mySession->dispatchEvent(&prReqUCS, primarySession);
								Debug::trace("ConfigurationManager", "Dispatched PrUseContinuousStock to primary session to handle");
							}
							else
							{
								PrinterGeneric prReqSizing(PrReqLabelSizing, mySession, FirstPrinter);
								mySession->dispatchEvent(&prReqSizing, primarySession);
								Debug::trace("ConfigurationManager", "Dispatched PrReqLabelSizing to primary session to handle");
							}
						}
					}

					if ( ( cfg->isReceived(fieldOffsetCfgSNMPTrapsIp) || cfg->isReceived(fieldOffsetCfgSNMPTrapsPort) || cfg->isReceived(fieldOffsetCfgSNMPVariablesPort) || cfg->isReceived(fieldOffsetCfgSNMPEnabled) ) && config->toBoolCfgI(CfgInternal::PYTHONENABLED) )
					{
						string command;

						if (config->toStringCfg(fieldOffsetCfgSNMPEnabled) == "1")
						{
							// SNMP is enabled, and since one of the snmp parameters were received, assume snmp needs to be set up again.
							command.assign(PythonScripter::buildPythonCall("snmpReload.py", -1, ""));
#ifdef __linux__
							// SOF-2718: popen(), via PAL_executeWithResults(), was waiting for the background Python process to complete.
							//           Changed to system() call instead.
							command.append(" &");
							const char * myCommand = command.c_str();
							system(myCommand);
#else
							string pyOutput;
							Debug::trace("ConfigurationManager", "snmp script, command: " + command);
							PAL_absclass::PAL_executeWithResults(command, pyOutput);
							Debug::trace("ConfigurationManager", "snmp script, result: " + pyOutput);
							// We don't care about any output from the command.
#endif
						}
						else
						{
							// SNMP is not enabled, so assume snmp agent needs to be stopped.
							command.assign(PythonScripter::buildPythonCall("snmpKill.py", -1, ""));

							string pyOutput;
							Debug::trace("ConfigurationManager", "snmp script, command: " + command);
							PAL_absclass::PAL_executeWithResults(command, pyOutput);
							Debug::trace("ConfigurationManager", "snmp script, result: " + pyOutput);
							// We don't care about any output from the command.
						}
					}
				}

				Debug::trace("ConfigurationManager", "done processing, dispatching CfgSaved ...");
				dispatchConfigurationSaved();
			}
			break;
		}

		case CfgScaleUpgradeRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgScaleUpgradeWrite);
				response.setFormatType(BrowserFormat);
				CfgScale myConfig;
				myConfig.setFormatType(UpgradeScaleFormat);
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgScaleUpgradeWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgScaleUpgradeWrite Message");
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgScaleUpgradeSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgScaleUpgradeSave Message");
			bool synchronizationNeeded = false;
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgScale newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case CfgEmailRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgEmailWrite);
				response.setFormatType(BrowserFormat);
				CfgEmail myConfig;
				myConfig = Configuration::getInstance()->getCfgEmail();
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgEmailWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgEmailWrite Message");
			CfgEmail *cfg = dynamic_cast<CfgEmail *> (msg->getHfcObject());
			if (cfg)
				Configuration::getInstance()->update(cfg);
			break;
		}

		case CfgEmailSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgEmailSave Message");
			CfgEmail *cfg = dynamic_cast<CfgEmail *> (msg->getHfcObject());
			if (cfg)
				Configuration::getInstance()->update(cfg);

			bool synchronizationNeeded = Configuration::getInstance()->getCfgEmail().isSaveNeeded();

			Configuration::getInstance()->save(dbAccess, (CfgEmail *)0);

			CfgEmail newCfg;
			newCfg = Configuration::getInstance()->getCfgEmail();
			if (synchronizationNeeded)
			{
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case CfgSynchronizationRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgSynchronizationWrite);
				response.setFormatType(BrowserFormat);
				CfgSynchronization myConfig;
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgSynchronizationWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgSynchronizationWrite Message");
			CfgSynchronization *cfg = dynamic_cast<CfgSynchronization *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgSynchronizationSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgSynchronizationSave Message");
			bool synchronizationNeeded = false;
			CfgSynchronization *cfg = dynamic_cast<CfgSynchronization *> (msg->getHfcObject());

			/* For console sessions perform use case 2.20. */
			if ( mySession && (mySession->isPrimarySession() || mySession->isRemoteSession()) && cfg )
			{
				cfg->updateConfiguration();

				Debug::trace("ConfigurationManager", "handling CfgSynchronizationSave IF");
				bool syncTypeChanged = false, primaryScaleChanged = false;

				ScaleSynchronizationType newSyncType = cfg->getReceivedScaleSynchronizationType();
				std::string newPrimaryScale = cfg->getReceivedPrimaryScale();

				Debug::trace("ConfigurationManager", "handling CfgSynchronizationSave - checking if synchronization type is changing ...");
				if (isSynchronizationTypeChanging(cfg, &syncTypeChanged, &primaryScaleChanged, newPrimaryScale))
				{
					Debug::trace("ConfigurationManager", "handling CfgSynchronizationSave - synchronization type IS changing");
					processChangeSynchronizationType(syncTypeChanged, primaryScaleChanged, newSyncType, newPrimaryScale);
					Debug::trace("ConfigurationManager", "handling CfgSynchronizationSave - after processChangeSynchronizationType()");
				}
			}
			else
			{
				Debug::trace("ConfigurationManager", "handling CfgSynchronizationSave ELSE");
				Configuration::getInstance()->save(dbAccess);
				// dispatchConfigurationSaved();    // This is not needed as per Larry
			}

			Debug::trace("ConfigurationManager", "handling CfgSynchronizationSave Message COMPLETE");
			break;
		}

		case CfgSynchronizationRestore:
		{
			Debug::trace("ConfigurationManager", "handling CfgSynchronizationRestore Message");
			bool synchronizationNeeded = false;
			CfgSynchronization *cfg = dynamic_cast<CfgSynchronization *> (msg->getHfcObject());
			if (cfg)
			{
				Debug::trace("ConfigurationManager", "processing CfgSynchronization");

				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
				
				bool syncTypeChanged = false, primaryScaleChanged = false;
				ScaleSynchronizationType newSyncType = cfg->getReceivedScaleSynchronizationType();
				std::string newPrimaryScale = cfg->getReceivedPrimaryScale();
				if (isSynchronizationTypeChanging(cfg, &syncTypeChanged, &primaryScaleChanged, newPrimaryScale))
				{
					Debug::trace("ConfigurationManager", "CfgSynchronization is changing, saving for later application");

					// make a fresh copy of the cfgSynchronization object, since the isSynchronizationTypeChanging method could have changed it.
					CfgSynchronization *cfgSync = new CfgSynchronization();
					if (cfgSync)
					{
						cfgSync->setSavedSyncTypeChanged(syncTypeChanged);
						cfgSync->setSavedPrimaryScaleChanged(primaryScaleChanged);
						cfgSync->setSavedNewSyncType(newSyncType);
						cfgSync->setSavedNewPrimaryScale(newPrimaryScale);

						if (pendingSynchronizationChange)
							delete pendingSynchronizationChange;
						pendingSynchronizationChange = cfgSync;
					}

					if (syncTypeChanged)
						Configuration::getInstance()->updateField( fieldOffsetCfgScaleSynchronizationType, inttostring( (int)newSyncType ) );
					if (primaryScaleChanged)
						Configuration::getInstance()->updateField( fieldOffsetCfgPrimaryScale, newPrimaryScale );
				}

				Configuration::getInstance()->save(dbAccess);

				if (synchronizationNeeded)
				{
					Debug::trace("ConfigurationManager", "Updating sync list in HFCManagerContainer.");

					hfc::HFCManagerContainer::updateSyncList();

					CfgSynchronization newCfg;
					if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone)
					{
						std::string synchSer = "";
						serializeForSynchronization(synchSer, &newCfg);

						synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
						
						if (proxy)
							proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
					}
				}

				Debug::trace("ConfigurationManager", "done processing, dispatching CfgSaved ...");
			}
			break;
		}

		case CfgSyncListRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgSyncListWrite);
				response.setFormatType(BrowserFormat);
				CfgSynchronization myConfig;
				myConfig.setFormatType(SyncListFormat);
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgSyncListWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgSyncListWrite Message");
			CfgSynchronization *cfg = dynamic_cast<CfgSynchronization *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgSyncListSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgSyncListSave Message");
			bool synchronizationNeeded = false;
			CfgSynchronization *cfg = dynamic_cast<CfgSynchronization *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				Debug::trace("ConfigurationManager", "Updating sync list in HFCManagerContainer.");

				hfc::HFCManagerContainer::updateSyncList();

				CfgSynchronization newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone )
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}
			break;
		}

		case CfgFTPRead:
		{
			Debug::trace("ConfigurationManager", "handling CfgFTPRead Message");

			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgFTPWrite);
				response.setFormatType(BrowserFormat);
				CfgFTP myConfig;
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgFTPWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgFTPWrite Message");
			CfgFTP *cfg = dynamic_cast<CfgFTP *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgFTPSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgFTPSave Message");
			bool synchronizationNeeded = false;
			CfgFTP *cfg = dynamic_cast<CfgFTP *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgFTP newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
					
					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case CfgCommInterfaceRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgCommInterfaceWrite);
				response.setFormatType(BrowserFormat);
				CfgCommInterface myConfig;
				myConfig = Configuration::getInstance()->getCfgCommInterface();
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgCommInterfaceWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgCommInterfaceWrite Message");
			CfgCommInterface *cfg = dynamic_cast<CfgCommInterface *> (msg->getHfcObject());
			if (cfg)
				Configuration::getInstance()->update(cfg);
			break;
		}

		case CfgCommInterfaceSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgCommInterface Message");
			CfgCommInterface *cfg = dynamic_cast<CfgCommInterface *> (msg->getHfcObject());
			if (cfg)
				Configuration::getInstance()->update(cfg);

			bool synchronizationNeeded = Configuration::getInstance()->getCfgCommInterface().isSaveNeeded();

			Configuration::getInstance()->save(dbAccess, (CfgCommInterface *)0);

			CfgCommInterface newCfg;
			newCfg = Configuration::getInstance()->getCfgCommInterface();
			if (synchronizationNeeded)
			{
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
					
					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

//SOF-4769:ScheduledMerge
		case CfgCommIntfaceScheduledRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgCommIntfaceScheduledWrite);
				response.setFormatType(BrowserFormat);
				CfgCommScheduled myConfig;
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgCommIntfaceScheduledWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgCommIntfaceScheduledWrite Message");
			CfgCommScheduled *cfg = dynamic_cast<CfgCommScheduled *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgCommIntfaceScheduledSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgCommIntfaceScheduledSave Message");
			bool synchronizationNeeded = false;
			CfgCommScheduled *cfg = dynamic_cast<CfgCommScheduled *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);
			
			CfgCommScheduled newCfg;
			if (synchronizationNeeded)
			{
				if (Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}


		case CfgPrepackRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgPrepackWrite);
				response.setFormatType(BrowserFormat);
				CfgPrepack myConfig;
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgPrepackWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgPrepackWrite Message");
			CfgPrepack *cfg = dynamic_cast<CfgPrepack *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgPrepackSave:
		{
			Debug::trace("ConfigurationManager.Prepack", "handling CfgPrepackSave Message");
			bool synchronizationNeeded = false;
			CfgPrepack *cfg = dynamic_cast<CfgPrepack *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgPrepack newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();
					
					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case CfgExpirationLabelNoteRead:
		{
			Debug::trace("ConfigurationManager", "handling CfgExpirationLabelNoteRead Message");
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgExpirationLabelNoteWrite);
				response.setFormatType(BrowserFormat);
				CfgScale myConfig;
				myConfig.setFormatType(ExpirationLabelMessageFormat);
				response.setHfcObject(&myConfig);
				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgExpirationLabelNoteWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgExpirationLabelNoteWrite Message");
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgExpirationLabelNoteSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgExpirationLabelNoteSave Message");
			bool synchronizationNeeded = false;
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgScale newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

					if (proxy)
						proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), getWriteEvent(), synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		case CfgScaleManagementRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgScaleManagementWrite);
				response.setFormatType(BrowserFormat);
				CfgScale myConfig;
				myConfig.setFormatType(ScaleManagementFormat);
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgScaleManagementWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgScaleManagementWrite Message");
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgScaleManagementSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgScaleManagementSave Message");
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			Configuration::getInstance()->save(dbAccess);
			dispatchConfigurationSaved();
			break;
		}
		case CfgRotationOverrideRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgRotationOverrideWrite);
				response.setFormatType(BrowserFormat);
				CfgPrepack myConfig;
				myConfig.setFormatType(RotationOverrideFormat);
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgRotationOverrideWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgRotationOverrideWrite Message");
			CfgPrepack *cfg = dynamic_cast<CfgPrepack *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgRotationOverrideSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgRotationOverrideSave Message");
			CfgPrepack *cfg = dynamic_cast<CfgPrepack *> (msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				Configuration::getInstance()->save(dbAccess);
				dispatchConfigurationSaved();
			}
			break;
		}

		case CfgReportsSetupRead:
		{
			if (mySession)
			{
				SessionMessage response(ConfigurationDev, CfgReportsSetupWrite);
				response.setFormatType(BrowserFormat);
				CfgScale myConfig;
				myConfig.setFormatType(ReportsSetupFormat);
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgReportsSetupWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgReportsSetupWrite Message");
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			break;
		}

		case CfgReportsSetupSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgReportsSetupSave Message");
			CfgScale *cfg = dynamic_cast<CfgScale *> (msg->getHfcObject());
			if (cfg)
				cfg->updateConfiguration();
			Configuration::getInstance()->save(dbAccess);
			dispatchConfigurationSaved();
			break;
		}

		case CfgAccumulateItemsRead:
		{
			if ( mySession )
			{
				SessionMessage response(ConfigurationDev, CfgAccumulateItemsWrite);
				response.setFormatType(BrowserFormat);
				CfgAccumulateItems myConfig;
				response.setHfcObject(&myConfig);

				mySession->send(&response, Messenger::DEST_UI);
			}
			break;
		}

		case CfgAccumulateItemsWrite:
		{
			Debug::trace("ConfigurationManager", "handling CfgAccumulateItemsWrite Message");
			CfgAccumulateItems *cfg = dynamic_cast<CfgAccumulateItems *>(msg->getHfcObject());
			if ( cfg )
				cfg->updateConfiguration();
			break;
		}

		case CfgAccumulateItemsSave:
		{
			Debug::trace("ConfigurationManager", "handling CfgAccumulateItemsSave Message");
			bool synchronizationNeeded = false;
			CfgAccumulateItems *cfg = dynamic_cast<CfgAccumulateItems *>(msg->getHfcObject());
			if (cfg)
			{
				cfg->updateConfiguration();
				synchronizationNeeded = cfg->isSaveNeeded();
			}

			Configuration::getInstance()->save(dbAccess);

			if (synchronizationNeeded)
			{
				CfgAccumulateItems newCfg;
				if ( Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone && mgrContainer_->isSynchronized(hfc::HFCManagerContainer::Configurations))
				{
					std::string synchSer = "";
					serializeForSynchronization(synchSer, &newCfg);

					SessionManager::getInstance()->getSyncManagerProxy()->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
							hfc::SynchronizationChange(getDevice(), CfgAccumulateItemsWrite, synchSer)));
				}
			}

			dispatchConfigurationSaved();
			break;
		}

		default:
			break;
	}
}


bool ConfigurationManager::isSynchronizationTypeChanging(CfgSynchronization *cfg, bool *syncTypeChanged, bool *primaryScaleChanged, std::string &newPrimaryScale)
{
	if (!cfg)
		return false;

	*syncTypeChanged = false;
	*primaryScaleChanged = false;

	ScaleSynchronizationType currentSyncType = Configuration::getInstance()->getScaleSynchronizationType();
	ScaleSynchronizationType newSyncType = cfg->getReceivedScaleSynchronizationType();

	/* (uc2.20) If the user changes contain a change to the Synchronization Type
	configuration element, then mark this condition and temporarily store the new
	value, removing it from the user changes. [For the Synchronization Type to change
	to Secondary, the Primary Scale configuration element needs to either have a pre-
	existing value or receive a new value at the same time; otherwise, the Synchronization
	Type will not change. If the Primary Scale changed at the same time, then also mark
	this condition and temporarily store that new value removing it from the user changes.] */
	if ( newSyncType != currentSyncType )
	{
		if ( newSyncType == ScaleSynchronizationTypeSecondary )
		{
			if ( Configuration::getInstance()->toStringCfg(fieldOffsetCfgPrimaryScale).size() || newPrimaryScale.size() )
				*syncTypeChanged = true;
		}
		else
			*syncTypeChanged = true;

		// This is the way to remove the change to Synchronization Type
		cfg->setReceivedScaleSynchronizationType(currentSyncType);

		if ( *syncTypeChanged )
		{
			switch ( newSyncType )
			{
				case ScaleSynchronizationTypePrimary:
				{
					*primaryScaleChanged = true;
					cfg->setReceivedPrimaryScale("");
					cfg->setReceived(fieldOffsetCfgPrimaryScale, true);

					break;
				}
				case ScaleSynchronizationTypeSecondary:
				{
					if ( newPrimaryScale.size() && cfg->getReceivedPrimaryScale() != Configuration::getInstance()->toStringCfg(fieldOffsetCfgPrimaryScale) )
					{
						*primaryScaleChanged = true;
						// This is the way to remove the change to Primary scale
						cfg->setReceivedPrimaryScale(Configuration::getInstance()->toStringCfg(fieldOffsetCfgPrimaryScale));
					}

					break;
				}
            default:
               // Nothing
               break;
			}   // End of switch ( newSyncType )
		}
	}

	if ( !*syncTypeChanged )
	{
		/* (uc2.20) If this scale is configured as a Secondary scale and the user changes
		do not contain a change to the Synchronization Type, but contain a change to the
		Primary Scale, then mark this condition and temporarily store the new value
		removing it from the user changes. */
		if ( currentSyncType == ScaleSynchronizationTypeSecondary )
		{
			if ( newPrimaryScale.size() && cfg->getReceivedPrimaryScale() != Configuration::getInstance()->toStringCfg(fieldOffsetCfgPrimaryScale) )
			{
				*primaryScaleChanged = true;
				// This is the way to remove the change to Primary scale
				cfg->setReceivedPrimaryScale(Configuration::getInstance()->toStringCfg(fieldOffsetCfgPrimaryScale));
			}
		}
	}

	return (*syncTypeChanged | *primaryScaleChanged);
}


void ConfigurationManager::processChangeSynchronizationType(bool syncTypeChanged, bool primaryScaleChanged,
		ScaleSynchronizationType newSyncType, std::string newPrimaryScale, bool bootingUp)
{
	/**********************************************************************************************************************************
	 **********************************************************************************************************************************
	 **********************************************************************************************************************************
	 * The following is performed to actually change the scale's synch type, after we have determined that the synch type is changing.
	 **********************************************************************************************************************************
	 **********************************************************************************************************************************
	 **********************************************************************************************************************************/

	if ( bootingUp )
	{
		if (syncTypeChanged)
			Configuration::getInstance()->updateField(fieldOffsetCfgScaleSynchronizationType, inttostring(newSyncType));
		if (primaryScaleChanged)
			Configuration::getInstance()->updateField(fieldOffsetCfgPrimaryScale, newPrimaryScale);

		Debug::trace("ConfigurationManager", "processChangeSynchronizationType during bootup (after db independent data): "
			"scale synchronization type = " + inttostring(newSyncType) + " (" + string(syncTypeChanged ? "changed" : "not changed") +
			"), primary scale = '" + newPrimaryScale + "' (" + string(primaryScaleChanged ? "changed" : "not changed") + ")");

		Configuration::getInstance()->save(dbAccess);
		return;
	}


	ScaleSynchronizationType currentSyncType = Configuration::getInstance()->getScaleSynchronizationType();

	/* (uc2.20) If the Synchronization Type is set to Stand Alone, then perform the following steps: */
	if ( currentSyncType == ScaleSynchronizationTypeStandAlone )
	{
		/* (uc2.20) If the Synchronization Type changed, then trigger the execution of
		"Use Case 1.10: Changing Scale Type", passing the new Synchronization Type value
		and, if received, the new Primary Scale's hostname/IP address information. */
		if ( syncTypeChanged )
		{
			ChangeSynchronizationTypeMessage message(newSyncType, newPrimaryScale);
			Hobart::CMessageCenter::instance() << &message;
		}


		/* (uc2.20) End the execution of this use case. */
	}
	else
	{
		/* (uc2.20) Remove any changes for which the scale does not synchronize. If
		nothing is left and neither the Synchronization Type nor the Primary Scale
		configuration elements changed, then end the execution of this use case. */
		if ( syncTypeChanged || primaryScaleChanged )
		{
			/* (uc2.20) Generate a change record from the remaining user changes. */
			// noting to do since all data elements are non-synchonized


			/* (uc2.20) Trigger the execution of "Use Case 5.20: Processing user changes",
			passing the change record. */
			// nothing to do since all data elements are non-synchonized


			/* (uc2.20) If the Synchronization Type changed, then triger the execution of
			"Use Case 1.10: Changing Synchronization Type", passing the new Synchronization
			Type and, if received, the new Primary Scale information. End the execution of
			this use case. */
			if ( syncTypeChanged )
			{
				ChangeSynchronizationTypeMessage message(newSyncType, newPrimaryScale);
				Hobart::CMessageCenter::instance() << &message;
			}
			else
			{
				/* (uc2.20) If the Primary Scale changed, then trigger the execution of
				"Use Case 1.11: Changing Primary Scale", passing the new Primary Scale
				information. End the execution of this use case. */
				ChangePrimaryScaleMessage message(newPrimaryScale);
				Hobart::CMessageCenter::instance() << &message;
			}
		}
	}
}


bool ConfigurationManager::hasPendingSynchronizationChange()
{
	return (pendingSynchronizationChange) ? true : false;
}

void ConfigurationManager::applyPendingSynchronizationChange(bool bootingUp)
{
	if (pendingSynchronizationChange)
	{
		processChangeSynchronizationType(
			pendingSynchronizationChange->getSavedSyncTypeChanged(),
			pendingSynchronizationChange->getSavedPrimaryScaleChanged(),
			pendingSynchronizationChange->getSavedNewSyncType(),
			pendingSynchronizationChange->getSavedNewPrimaryScale(), bootingUp);

		delete pendingSynchronizationChange;
		pendingSynchronizationChange = 0;
	}
}


void ConfigurationManager::dispatchConfigurationSaved()
{
	if (mySession)
	{
		short mySessionType = mySession->getSessionType();
		Debug::trace("ConfigurationManager", "checking if CfgSaved should be dispatched later, mySession type is: " +  inttostring(mySessionType));
		if ( mySessionType == AbstractSession::SESSIONTYPE_COMM ||
		     mySessionType == AbstractSession::SESSIONTYPE_HELPER ||
		     mySessionType == AbstractSession::SESSIONTYPE_HELPER_PROCESSDOWNLOADFILES )
		{
			Debug::trace("ConfigurationManager", "remembering that CfgSaved needs to be dispatched later.");
			commSessionSavedConfig = true;
			return;
		}
	}
	else
		Debug::trace("ConfigurationManager", "mySession is NULL for checking if CfgSaved should be dispatched later.");

	dispatchConfigurationSavedNow();
}

void ConfigurationManager::dispatchConfigurationSavedNow()
{
	Debug::trace("ConfigurationManager", "sending ConfigurationDev|CfgSaved message to all Sessions");

	Configuration *config = Configuration::getInstance();

	if (mySession)
	{
		EvolMessage msg(ConfigurationDev, CfgSaved);
		mySession->dispatchEvent(&msg);

		// HT-286-Sending eS tag through comm doesn't take affect till either reboot or entering supervisor takes place 
		// mgrContainer_->dispatchMessage(&msg); - This was the orignal code, and in dbSession::dispatchMessage(), all it
		// did was call dispatchEvent() anyway.  But for COMM objects, it called HFCManagerContainer.dispatchMessage() and
		// all that was a virtual method in the dot h and no real definition.  
	}

	if (config && config->toBoolCfgI(CfgInternal::PYTHONENABLED))
	{
		const string command( PythonScripter::buildPythonCall(config->toStringCfgI(CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME), config->toIntCfgI(CfgInternal::PYTHONSFTPUPDATEPORT), "sftpUpgradeConfigChanged") );

		string connectionStatusOutput;

		Debug::trace( "ConfigurationManager", "notifyExternalViaPythonScript, command: " + command );
		PAL_absclass::PAL_executeWithResults( command, connectionStatusOutput );

		// We don't care about any output from the command.
	}

/* The old way of sending this message.
	if (mySession)
	{
		SessionMessage msg(ConfigurationDev, CfgSaved);
		msg.setFormatType(InternalFormat);
		mySession->send(&msg, Messenger::DEST_RTC);
	}
	// FT-1709 & HTA-11:  Next statement now known as false:  Have to call this directly, since messages are not being sent to yourself.
	handleReadAllConfigMsgs();
*/
}


void ConfigurationManager::handleCfgReadSingleFieldMessage(WriteSingleField *configUpdate)
{
	if (configUpdate && mySession)
	{
		SessionMessage fieldMsg(ConfigurationDev, DtWriteSingleField);
		fieldMsg.setFormatType(BrowserFormat);
		HobString fieldData;
		if (configUpdate->getFieldCode() >= INTERNAL_CONFIGURATION_FIRST_FIELDCODE)
			fieldData = Configuration::getInstance()->toStringCfgI(configUpdate->getFieldCode());
		else
			fieldData = Configuration::getInstance()->toStringCfg(configUpdate->getFieldCode());
		configUpdate->setFieldNewValue(fieldData);
		fieldMsg.setHfcObject(configUpdate);
		mySession->send(&fieldMsg, Messenger::DEST_UI);
	}
}

void ConfigurationManager::handleCfgWriteSingleFieldMessage(WriteSingleField *configUpdate)
{
	if (configUpdate && configUpdate->getFieldCode() < INTERNAL_CONFIGURATION_FIRST_FIELDCODE)
		Configuration::getInstance()->updateField(configUpdate->getFieldCode(), configUpdate->getFieldNewValue());
}

void ConfigurationManager::handleCfgSaveSingleFieldMessage(WriteSingleField *configUpdate)
{
	if (configUpdate && configUpdate->getFieldCode() < INTERNAL_CONFIGURATION_FIRST_FIELDCODE)
	{
		Configuration::getInstance()->updateField(configUpdate->getFieldCode(), configUpdate->getFieldNewValue());

		HobString verifyField = Configuration::getInstance()->toStringCfg(configUpdate->getFieldCode());

		if (configUpdate->getFieldNewValue() == verifyField)
		{
			Configuration::getInstance()->save(dbAccess);
			dispatchConfigurationSaved();
		}
		else if (mySession)
		{
			// Send a message back to the browser indicating the field was not updated.
			SessionMessage fieldNotUpdatedMsg(ConfigurationDev, DtWriteSingleField);
			fieldNotUpdatedMsg.setFormatType(BrowserFormat);
			configUpdate->setFieldNewValue(configUpdate->getFieldOrigValue());	//	Return the original value.
			fieldNotUpdatedMsg.setHfcObject(configUpdate);
			mySession->send(&fieldNotUpdatedMsg, Messenger::DEST_UI);
		}
	}
}


void ConfigurationManager::enterSeviceMode()
{
	Debug::trace("ConfigurationManager.weigherConfig", "enterSeviceMode()");
	enteringServiceMode = true;
	leavingServiceMode = false;
	savedWeigherConfig = false;
	if (initialServiceWeigherConfig)
		delete initialServiceWeigherConfig;
	initialServiceWeigherConfig = (HfcWeigherConfig *)NULL;
}

void ConfigurationManager::prepareToLeaveServiceMode()
{
	Debug::trace("ConfigurationManager.weigherConfig", "prepareToLeaveServiceMode()");
	if (savedWeigherConfig && mySession && currentWeigherConfig && initialServiceWeigherConfig && currentWeigherConfig->configChanged(initialServiceWeigherConfig))
	{
		Debug::trace("ConfigurationManager.weigherConfig", "prepareToLeaveServiceMode():  Config was changed, so wait until we get a new weigherConfig.");
		//	Send a WgReqInquire in case the weigher was calibrated.
		//	We need an up-to-date weigher config to update the config date and times
		//	and not potentially lose the calibration date and times. PTR # 509 [CCD APR 2011].

		// Commented out the register for the message, we are now always registered for the WgInquire.
		//	Register for the WgInquire message
		//mySession->registerEvent(Weigher, WgInquire, false );

		WeigherRequestInquire wri;
		mySession->send(&wri, Messenger::DEST_RTC);

		leavingServiceMode = true;
	}
	else
	{
		leaveServiceMode();
	}
}

void ConfigurationManager::leaveServiceMode()
{
	Debug::trace("ConfigurationManager.weigherConfig", "leaveServiceMode()");
	Debug::trace("ConfigurationManager.weigherConfig", HobString("leaveServiceMode()  savedWeigherConfig: ") + HobString(Bool::toString(savedWeigherConfig)));
	Debug::trace("ConfigurationManager.weigherConfig", HobString("leaveServiceMode()  currentWeigherConfig: ") + HobString(Hex::toString((long long)currentWeigherConfig)));
	Debug::trace("ConfigurationManager.weigherConfig", HobString("leaveServiceMode()  initialServiceWeigherConfig: ") + HobString(Hex::toString((long long)initialServiceWeigherConfig)));

	if (savedWeigherConfig && mySession && currentWeigherConfig && initialServiceWeigherConfig && currentWeigherConfig->configChanged(initialServiceWeigherConfig))
	{
		Debug::trace("ConfigurationManager.weigherConfig", "leaveServiceMode() - calling updateConfigurationDateAndTimes()");

		// For Category III, have to send the current date/time to the Weigher Manager before permanently setting any configuration or calibration values.
		time_t now;
		time(&now);
		WeigherCat3DateTime wCat3DateTime( mySession, (UINT32) now );
		mySession->send( &wCat3DateTime, Messenger::DEST_RTC );

		// Now go ahead and update the config date and time, and the counter. lap
		currentWeigherConfig->updateConfigurationDateAndTimes();
		currentWeigherConfig->setConfigType(WgPermanent);
		currentWeigherConfig->setEventType(WgConfig);

		SessionMessage response(Weigher, WgConfig);
		response.setFormatType(InternalFormat);
		response.setHfcObject(currentWeigherConfig);
		mySession->send(&response, Messenger::DEST_RTC);
	}

	// reset here just for cleanup purposes
	if (initialServiceWeigherConfig)
	{
		delete initialServiceWeigherConfig;
		initialServiceWeigherConfig = (HfcWeigherConfig *)NULL;
	}
	enteringServiceMode = false;
	leavingServiceMode = false;
	savedWeigherConfig = false;
}

void ConfigurationManager::handleDeleteAllFiles(UINT8 event)
{
	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	if (SessionManager::getInstance()->isDeleteAllFilesInProgress())
	{
		Debug::trace("ConfigurationManager", "handleDeleteAllFiles() already in progress; this request ignored.");
		if (mySession)
			mySession->addDialogMessage(ErrSessionDev, SessionDevDeleteAllFilesAlreadyInProgress, HfcResponse::RESPONSESTYLE_OK);
		return;
	}

	if (mgrContainer_)
	{
		EvolMessage msg(ConfigurationDev, CfgDeleteAllFilesInProgress);
		mgrContainer_->dispatchMessage(&msg);
	}

	if (mySession)
		mySession->addDialogMessage(ErrSessionDev, SessionDevDeleteAllFilesInProgress, HfcResponse::RESPONSESTYLE_NOINPUT);
    PAL_absclass::PAL_sleep(50);    //	To make sure the message above makes it to the display before we take over the processor.

	Debug::trace("ConfigurationManager", "handleDeleteAllFiles() started");

	bool dbDeleteAllSucceeded = true;

	try
	{
		DataManager* deleteAllDM = 0;
		// we may or not be in deptartment 0, or not, and we area deleting all departments, so set it back to dept 0.
		if (config->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum) > 0 )
		{
			config->updateField( fieldOffsetCfgMostRecentActiveDeptNum, "0");
			config->saveField(fieldOffsetCfgMostRecentActiveDeptNum, dbAccess);
		}
		hfc::HFCManagerContainer::recordTypeContainerIterator_type deleteAllDMIterator = mgrContainer_->beginDeleteAllFilesDataManager();
		while ( deleteAllDMIterator != mgrContainer_->endDeleteAllFilesDataManager() )
		{
			deleteAllDM = mgrContainer_->getDataManager(deleteAllDMIterator);
			if ( deleteAllDM )
			{
				int dmDevice = (int)(deleteAllDM->getDevice());
				Debug::trace("ConfigurationManager.deleteAll", HobString("calling handleDeleteAll for DataManager device:") + HobString(Integer::toString(dmDevice)));
				deleteAllDM->handleDeleteAll();
			}
			else
				Debug::trace("ConfigurationManager.deleteAll", HobString("did not find the HFCManagerContainer manager for ") + HobString(Integer::toString(*deleteAllDMIterator)));


			++deleteAllDMIterator;
		}
	}
	catch (DBException &dbe)
	{
		dbDeleteAllSucceeded = false;
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Session::handleDeleteAllFiles() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}


	{
		string istr, dafNameProp, dafRecursiveProp, dafExclusionsProp;
		string dafName, dafExclusions;
		bool dafRecursive;

		char dirSep = '/';
		if ( !config->toStringCfgI(CfgInternal::DIRECTORYSEPARATOR).empty() )
			dirSep = config->toStringCfgI(CfgInternal::DIRECTORYSEPARATOR).at(0);

		int i = 0;
		while (true)
		{
			istr = inttostring(i++);
			dafNameProp = "DeleteAllFiles_name" + istr;
			try
			{
				dafName = config->toStringCfgI(dafNameProp);
			}
			catch (std::exception e)
			{
				string es = e.what();
				Debug::trace("ConfigurationManager.deleteAll", dafNameProp + " not found in CfgI: " + es);
				break;
			}

			if (dafName.empty())
			{
				Debug::trace("ConfigurationManager.deleteAll", dafNameProp + " not found in CfgI:");
				break;
			}

			if ( dafName.at(dafName.size()-1) == dirSep )
			{
				// We're deleting a directory.

				util_filesystem::Directory dirToDelete("");
				dirToDelete.appendPath(dafName);
				if ( dirToDelete.exists() )
				{
					dafRecursive = false;
					dafRecursiveProp = "DeleteAllFiles_recursive" + istr;
					try
					{
						dafRecursive = config->toBoolCfgI(dafRecursiveProp);
					}
					catch (std::exception eRecursive)
					{
						string es = eRecursive.what();
						Debug::trace("ConfigurationManager.deleteAll", dafRecursiveProp + " not found in CfgI: " + es);
					}

					dafExclusions = "";
					dafExclusionsProp = "DeleteAllFiles_exclusions" + istr;
					try
					{
						dafExclusions = config->toStringCfgI(dafExclusionsProp);
					}
					catch (std::exception eExclusions)
					{
						string es = eExclusions.what();
						Debug::trace("ConfigurationManager.deleteAll", dafExclusionsProp + " not found in CfgI: " + es);
					}

					Debug::trace("ConfigurationManager.deleteAll", dafName + " recursive: " + booltostring(dafRecursive) + "  exclusions: " + dafExclusions);
					bool result = dirToDelete.removeContents(dafRecursive, dafExclusions);
					if (result)
						Debug::trace("ConfigurationManager.deleteAll", dafName + " deleted all successfully.");
					else
						Debug::trace("ConfigurationManager.deleteAll", dafName + " deleted all failed.");

				}
			}
			else
			{
				// If it's not a directory, then it must be a file.

				util_filesystem::File fileToDelete(dafName);
				string resultDebugString;

				if ( fileToDelete.exists() )
				{
					if ( fileToDelete.remove() )
						resultDebugString = "Successfully removed file: ";
					else
						resultDebugString = "Could not remove file: ";
				}
				else
					resultDebugString = "File to be removed does not exist: ";

				Debug::trace("ConfigurationManager.deleteAll", resultDebugString + dafName);
			}
		}
	}


	/*if (dbDeleteAllSucceeded)
	{
		if ( event != CfgBatchApplyDeleteAllFiles && Configuration::getInstance()->getScaleSynchronizationType() != ScaleSynchronizationTypeStandAlone )
		{
			SessionMessage message(ConfigurationDev, CfgDeleteAllFiles);
			message.setFormatType(SynchronizationTaggedFormat);
			const char * buffer = message.serialize();

			synchronization::SynchronizationManagerProxy* proxy = SessionManager::getInstance()->getSyncManagerProxy();

			if (proxy)
				proxy->processUserChanges(Util::CSmartPtr<hfc::SynchronizationChange>(new
					hfc::SynchronizationChange(ConfigurationDev, CfgDeleteAllFiles, std::string(buffer, message.getSerializedLength()))));
		}
	}
	*/
	//	Update config label text.
	CfgGenericLabelText *cfgGenericText = Configuration::getInstance()->getGenericLabelText();
	cfgGenericText->reloadLabelText(dbAccess);

	if (mgrContainer_)
	{
		EvolMessage msg(ConfigurationDev, CfgDeleteAllFilesFinished);
		mgrContainer_->dispatchMessage(&msg);
	}

	if (mySession)
	{
		mySession->removeDialogMessage(ErrSessionDev, SessionDevDeleteAllFilesInProgress, HfcResponse::RESPONSESTYLE_NOINPUT);
		if (dbDeleteAllSucceeded)
		{
			mySession->addDialogMessage(ErrSessionDev, SessionDevAllFilesDeleted, HfcResponse::RESPONSESTYLE_OK);

#if defined( __linux__ )
			if (Configuration::getInstance()->getCustomerHooksEnabled())
				hobart_cust_hook_update_scale_state(HOOK_SS_SCALE_ALL_FILES_DELETED, "");
#endif

		}
		else
			mySession->addDialogMessage(ErrSessionDev, SessionDevDeleteAllFilesFailed, HfcResponse::RESPONSESTYLE_OK);
	}

	//	Force a Config message to all Sessions (no super login required for one thing, others?).
	dispatchConfigurationSaved();

	// Dispatch a message that all UI Sessions will be listening for in order to update the
	// soft keys on their displays.
	if ( mgrContainer_ )
	{
		EvolMessage msg(SoftkeyDev, LoadAllSoftkeyGroups);
		mgrContainer_->dispatchMessage(&msg);
	}
}

void ConfigurationManager::updateHookWeigherPrecision( bool dualRange ) const
{	
#if defined( __linux__ )
	// send weigher info to hook
    if ( Configuration::getInstance()->getCustomerHooksEnabled() )
	{
		static std::string oldValue;

		std::string newValue = hookWeigherPrecision;

		if ( dualRange )
			if ( hookWeigherPrecisionDR.length() )
				newValue = hookWeigherPrecisionDR;

		if ( newValue != oldValue )
		{
			oldValue = newValue;

			hobart_cust_hook_update_scale_state( HOOK_SS_SCALE_WEIGHER_PRECISION, oldValue.c_str() );
		}
	}
#endif
}
