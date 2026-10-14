/* Evolution Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "DBSession.h"

#include "SynchronizationManager.h"
#include "HobList.h"

#include "SessionManager.h"
#include "ProductManager.h"
#include "ClassManager.h"
#include "ExpirationLabelManager.h"
#include "NutrifactsManager.h"
#include "ProductTextManager.h"
#include "LabelTypeManager.h"
#include "GraphicManager.h"

#include "CategoryManager.h"
#include "SpecialsManager.h"
#include "COOLManager.h"
#include "ProductDefaultManager.h"

#include "SoftkeyManager.h"
#include "SoftkeyFeatureManager.h"
#include "SoftkeyGroupManager.h"
#include "SoftkeyStyleManager.h"

#include "OperatorManager.h"
#include "OperatorChangeManager.h"
#include "OperatorNoteManager.h"
#include "StreamingGraphicManager.h"
#include "GenericLabelTextManager.h"
#include "ConfigurationManager.h"
#include "HourlyTotalManager.h"
#include "OperatorTotalManager.h"
#include "ClassTotalManager.h"
#include "ProductTotalManager.h"
#include "TransactionTypeManager.h"
#include "TransactionManager.h"
#include "EmailAlertsManager.h"
#include "PeripheralManager.h"
#include "FlashFileManager.h"

#include "LabelGraphicManager.h"
#include "CategoryGraphicManager.h"
#include "ProductGraphicManager.h"
#include "SoftkeyGraphicManager.h"
#include "SpecialsGraphicManager.h"
#include "PlanogramGraphicManager.h"

#include "EvolWrapperKnifeHistory.h"
#include "EvolWrapperErrorHistoryManager.h"
#include "CutTestManager.h"
#include "CutTestTotalManager.h"

#include "RecallManager.h"

#include "DeviceManager.h"
#include "ProductTextCfgManager.h"
#include "BarcodeScannerManager.h"

#include "DeptManager.h"

#include "DBException.h"
#include "DBManager.h"
#include "DBCategoriesRM.h"
#include "DBClassesRM.h"
#include "DBCoolCategoriesRM.h"
#include "DBCoolCategoryCountriesRM.h"
#include "DBCoolCategoryPreTextsRM.h"
#include "DBCoolCategoryProductionStepsRM.h"
#include "DBCoolCountriesRM.h"
#include "DBCoolListTextsRM.h"
#include "DBCoolPreTextsRM.h"
#include "DBCoolProductionStepsRM.h"
#include "DBCoolTextsRM.h"
#include "DBDevicesRM.h"
#include "DBExpirationLabelsRM.h"
#include "DBLabelTextsRM.h"
#include "DBNutritionsRM.h"
#include "DBProductsRM.h"
#include "DBProductDefaultsRM.h"
#include "DBProductTextsRM.h"
#include "DBSoftKeysRM.h"
#include "DBSoftKeyFeaturesRM.h"
#include "DBSoftKeyGroupsRM.h"
#include "DBSoftKeyStylesRM.h"
#include "DBSpecialProgramsRM.h"
#include "DBStreamingGraphicsRM.h"
#include "DBEmailAlertsRM.h"
#include "DBLabelsRM.h"
#include "DBProductStatisticsRM.h"
#include "DBROMLabelsRM.h"
#include "DBRecallsRM.h"
#include "DBDevicesRM.h"
#include "DBProductTextCfgsRM.h"
#include "DBBarcodeScannersRM.h"
#include "DBDeptsRM.h"

#include "Integer.h"
#include "UInteger.h"
#include "Long.h"
#include "Hex.h"
#include "ULong.h"
#include "Bool.h"

#include "util/thread/lock.h"
#include "util/thread/mutex.h"
#include "util/time/dbtime.h"
#include "util/e_string/utilstring.h"

#include "Trace.h"

#include "HandsFreeDeviceEvent.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DBSession::_dataType_ = 0;
std::string DBSession::_dataTypeName_("DBSession"); 
#endif


DBSession::DBSession(CommMechanism *uiClient, const char* name) :
    AbstractSession(uiClient, name, true, true, false),
    hfc::HFCManagerContainer()
{
#ifdef USE_ACCOUNTANT
    if (_dataType_ == 0)
        _dataType_ = util_acc::registerDataType(_dataTypeName_);

    util_acc::addInstance(_dataType_);
#endif

	CommSessionSoftkeyReload = false;
	CommSessionOperatorReload = false;


    productManager          = getProductManager();
    classManager            = getClassManager();
    expirationLabelManager  = getExpirationLabelManager();
    nutrifactsManager       = getNutrifactsManager();
    expandedTextManager     = getProductTextManager(psttExpandedText);
    specialMessageManager   = getProductTextManager(psttSpecialMessage);
    productNoteManager      = getProductTextManager(psttProductNote);
    marqueeTextManager      = getProductTextManager(psttMarqueeText);
	productText5Manager     = getProductTextManager(psttProductText5);
	productText6Manager     = getProductTextManager(psttProductText6);
	productText7Manager     = getProductTextManager(psttProductText7);
	productText8Manager     = getProductTextManager(psttProductText8);
	productText9Manager     = getProductTextManager(psttProductText9);
    labelTypeManager        = getLabelTypeManager();
    graphicManager          = getGraphicManager();
    categoryManager         = getCategoryManager();
    specialsManager         = getSpecialsManager();
    coolManager             = getCOOLManager();
    transactionTypeManager  = getTransactionTypeManager();
    transactionManager      = getTransactionManager();
    hourlyTotalManager      = getHourlyTotalManager();
    operatorTotalManager    = getOperatorTotalManager();
    classTotalManager       = getClassTotalManager();
    productTotalManager     = getProductTotalManager();
    coolTotalManager        = getCOOLTotalManager();
    productDefaultManager   = getProductDefaultManager();

    softkeyManager          = getSoftkeyManager();
    softkeyFeatureManager   = getSoftkeyFeatureManager();
    softkeyGroupManager     = getSoftkeyGroupManager();
    softkeyStyleManager     = getSoftkeyStyleManager();

    operatorManager         = getOperatorManager();
    operatorChangeManager   = getOperatorChangeManager();
    operatorNoteManager     = getOperatorNoteManager();
    streamingGraphicManager = getStreamingGraphicManager();
    genericLabelTextManager = getGenericLabelTextManager();
    configurationManager    = getConfigurationManager();
    emailAlertsManager      = getEmailAlertsManager();
    peripheralManager       = getPeripheralManager();
    flashFileManager        = getFlashFileManager();

    labelGraphicManager     = getLabelGraphicManager();
    categoryGraphicManager  = getCategoryGraphicManager();
    productGraphicManager   = getProductGraphicManager();
    softkeyGraphicManager   = getSoftkeyGraphicManager();
    specialsGraphicManager  = getSpecialsGraphicManager();
	planogramGraphicManager = getPlanogramGraphicManager();


	evolWrapperKnifeHistoryManager = getEvolWrapperKnifeHistoryManager();
	evolWrapperErrorHistoryManager = getEvolWrapperErrorHistoryManager();
	
	cutTestManager = getCutTestManager();
	cutTestTotalManager = getCutTestTotalManager();

    recallManager            = getRecallManager();

	deviceManager            = getDeviceManager();
	productTextCfgManager    = getProductTextCfgManager();
	barcodeScannerManager    = getBarcodeScannerManager();

	deptManager = getDeptManager();


	if (productManager)
        productManager->setMySession(this);
    if (classManager)
        classManager->setMySession(this);
    if (expirationLabelManager)
        expirationLabelManager->setMySession(this);
    if (nutrifactsManager)
        nutrifactsManager->setMySession(this);
    if (expandedTextManager)
        expandedTextManager->setMySession(this);
    if (specialMessageManager)
        specialMessageManager->setMySession(this);
    if (productNoteManager)
        productNoteManager->setMySession(this);
    if (marqueeTextManager)
        marqueeTextManager->setMySession(this);
    if (productText5Manager)
        productText5Manager->setMySession(this);
    if (productText6Manager)
        productText6Manager->setMySession(this);
    if (productText7Manager)
        productText7Manager->setMySession(this);
    if (productText8Manager)
        productText8Manager->setMySession(this);
    if (productText9Manager)
        productText9Manager->setMySession(this);
    if (labelTypeManager)
        labelTypeManager->setMySession(this);
    if (graphicManager)
        graphicManager->setMySession(this);
    if (categoryManager)
        categoryManager->setMySession(this);
    if (specialsManager)
        specialsManager->setMySession(this);
    if (coolManager)
        coolManager->setMySession(this);
    if (transactionTypeManager)
        transactionTypeManager->setMySession(this);
    if (transactionManager)
        transactionManager->setMySession(this);
    if (hourlyTotalManager)
        hourlyTotalManager->setMySession(this);
    if (operatorTotalManager)
        operatorTotalManager->setMySession(this);
    if (classTotalManager)
        classTotalManager->setMySession(this);
    if (productTotalManager)
        productTotalManager->setMySession(this);
    if (productDefaultManager)
        productDefaultManager->setMySession(this);

    if (softkeyManager)
        softkeyManager->setMySession(this);
    if (softkeyFeatureManager)
        softkeyFeatureManager->setMySession(this);
    if (softkeyGroupManager)
        softkeyGroupManager->setMySession(this);
    if (softkeyStyleManager)
        softkeyStyleManager->setMySession(this);

    if (operatorManager)
        operatorManager->setMySession(this);
    if (operatorChangeManager)
        operatorChangeManager->setMySession(this);
    if (operatorNoteManager)
        operatorNoteManager->setMySession(this);
    if (streamingGraphicManager)
        streamingGraphicManager->setMySession(this);
    if (genericLabelTextManager)
        genericLabelTextManager->setMySession(this);
    if (configurationManager)
        configurationManager->setMySession(this);
    if (emailAlertsManager)
        emailAlertsManager->setMySession(this);
    if (peripheralManager)
        peripheralManager->setMySession(this);
    if (flashFileManager)
        flashFileManager->setMySession(this);

    if (labelGraphicManager)
        labelGraphicManager->setMySession(this);
    if (categoryGraphicManager)
        categoryGraphicManager->setMySession(this);
    if (productGraphicManager)
        productGraphicManager->setMySession(this);
    if (softkeyGraphicManager)
        softkeyGraphicManager->setMySession(this);
    if (specialsGraphicManager)
        specialsGraphicManager->setMySession(this);
    if (planogramGraphicManager)
        planogramGraphicManager->setMySession(this);

	if (cutTestManager)
		cutTestManager->setMySession(this);
	if (cutTestTotalManager)
		cutTestTotalManager->setMySession(this);
	if (evolWrapperErrorHistoryManager)
		evolWrapperErrorHistoryManager->setMySession(this);

    if (recallManager)
        recallManager->setMySession(this);

	if (deviceManager)
		deviceManager->setMySession(this);
	if (productTextCfgManager)
		productTextCfgManager->setMySession(this);
	if (barcodeScannerManager)
		barcodeScannerManager->setMySession(this);

	if (deptManager)
		deptManager->setMySession(this);

	resetBatchId();

    Debug::trace("DBSession", util_string::ptrtostring(this) + " - DBSession(CommMechanism *uiClient)");
}

DBSession::~DBSession()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(_dataType_);
#endif

    Debug::trace("DBSession",
            util_string::ptrtostring(this) + " id = " + getName() + " - ~DBSession destructor");

	//check whether softkeys and operator information needs reloaded following a COMM Session
	ProcessReloadSoftkeyUpdates();	
	ProcessReloadOperatorUpdates();	

}


// No implementation here; intended to be overridden by Session class.
//virtual
bool DBSession::addDialogMessage(ErrorEvent UNUSED(errorDevice), int UNUSED(errorCode), short UNUSED(responseStyle), const HobList *UNUSED(msgData), bool UNUSED(allowDuplicates))
{
    return false;
}

// No implementation here; intended to be overridden by Session class.
//virtual
bool DBSession::removeDialogMessage(ErrorEvent UNUSED(errorDevice), int UNUSED(errorCode), short UNUSED(responseStyle))
{
    return false;
}


bool DBSession::isBatching() const
{
    return (currentBatchId == hfc::Batch::_invalidBatchNumber_) ? false : true;
}

int DBSession::getBatchId() const
{
    return currentBatchId;
}

void DBSession::resetBatchId()
{
    currentBatchId = hfc::Batch::_invalidBatchNumber_;
}


void DBSession::dispatchMessage(EvolMessage *message)
{
//	IMPORTANT:  (HTA-11/FT-1709) Be aware that this message will get sent to the sending Session 
//	because dispatchEvent does not set the CEvolMsgReceiver pointer in the message.
    if (message)
        dispatchEvent(message);

    //		msgManager_->dispatchEvent(message);
    /*
       SessionMessage msg(ConfigurationDev, CfgSaved);
       msg.setFormatType(InternalFormat);
       mySession->send(message, Messenger::DEST_RTC);
       */
}


// override from AbstractSession
//virtual
void DBSession::initializationBeforeRun()
{
    AbstractSession::initializationBeforeRun();

    // All Sessions need to be notified of the following messages.
    registerEvent(SessionDev, SnDBCompressStarting);
    registerEvent(SessionDev, SnDBCompressDone);
    registerEvent(SessionDev, SnDBBackupStarting);
    registerEvent(SessionDev, SnDBBackupDone);

    initData();
}

// override from AbstractSession
//virtual
void DBSession::finalizationAfterRun()
{
    uninitData();

    AbstractSession::finalizationAfterRun();
}

// override from AbstractSession
//virtual
void DBSession::quitGracefully()
{
}

// override from AbstractSession
//virtual
void DBSession::handleResultOfRTC(UINT32 result)
{
    switch (result & (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED))
    {
        case (SessionManager::COMMON_BLOCKED | SessionManager::SUPER_BLOCKED):
            {
                if ( !reinitDB_ )
                {
                    util_thread::Lock< util_thread::Mutex<> > lock (reinitializingDBMutex_);
                    if ( !reinitDB_ )
                    {
                        util_time::Time t0;

                        hfc::HFCManagerContainer::uninitData();
                        hfc::HFCManagerContainer::initData();

                        util_time::Time t1;
                        Debug::trace("AbstractSession", "After compressing DB, its reinitialization took " +
                                util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " msecs.");

                        reinitDB_ = true;
                    }

                }
            }
            break;
        case SessionManager::COMMON_BLOCKED:
            {
                reinitDB_ = false;

                // FT-828 fix for GCD db access getting yanked
                if (!(isPrimarySession() || isRemoteSession() || isGCDSession()))
                {
                    quitGracefully();

                    return;
                }
            }
            break;
        default:
            {

                reinitDB_ = false;
            }
            break;
    }
}

void DBSession::ProcessReloadSoftkeyUpdates()
{
	if (CommSessionSoftkeyReload)
	{
		// Dispatch a message that all UI Sessions will be listening for in order to update the
		// soft keys on their displays.
		
		EvolMessage msg(SoftkeyDev, LoadAllSoftkeyGroups);
		dispatchMessage(&msg);
		CommSessionSoftkeyReload = false;
	}
}

void DBSession::ProcessReloadOperatorUpdates()
{
	if (CommSessionOperatorReload)
	{
		// Dispatch a message that all UI Sessions will be listening for in order to update the configuration in the browser (madp).		
		EvolMessage msg(ConfigurationDev, CfgSaved);
		dispatchMessage(&msg);

		EvolMessage hfmsg(HandsFreeDevice, HFDSendOpNameList);
		dispatchMessage(&hfmsg);

		CommSessionOperatorReload = false;	
	}	
}

