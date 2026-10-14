#include "AutoWeighEvolWrapperBakeryMode.h"

#include "LabelApplierManager.h"
#include "LabelApplierRequestOff.h"

#include "PrinterManager.h"
#include "PrinterStatus.h"
#include "PrinterLabelReady.h"
#include "PrinterLabelTaken.h"
#include "PrinterLabelComplete.h"
#include "PrinterPostImage.h"

#include "WrapperManager.h"
#include "EvolWrapperControl.h"
#include "EvolWrapperInquire.h"
#include "EvolWrapperConfiguration.h"
#include "EvolWrapperWrap.h"
#include "WrapperErrorInfo.h"
#include "LabelApplierPrint.h"
#include "LabelApplierLabelReady.h"
#include "LabelApplierLabelTaken.h"

#include "WeigherCachedStatus.h"
#include "HfcWeigherStatus.h"

#include "ProductUpdateMsg.h"
#include "ProductChangeMsg.h"
#include "ProductStatusMsg.h"

#include "ProductTransaction.h"
#include "ServiceSpeaker.h"

#include "Configuration.h"

#include "OpModePackage.h"
#include "LabelType.h"

#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

/*min*/
#include "EvolWrapperHeaterConfig.h"
#include "EvolWrapperGeneralConfig.h"
#include "EvolLabelApplierConfiguration.h"
/*emin*/

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

LOCAL int dataType = 0;
LOCAL string dataTypeName = "AutoWeighEvolWrapperBakeryMode";
#endif

AutoWeighEvolWrapperBakeryMode::AutoWeighEvolWrapperBakeryMode(WrapperSetWrapMode::WrapMode wrapMode,
        WrapperIntakeMode::IntakeMode intakeMode,
        Product *pCurrentProduct,
        //WeigherManager *pWeigherManager,
        //PrinterManager *pFirstPrinter,
        //PrinterManager *pSecondPrinter,
        //PrinterManager *pThirdPrinter,
        //LabelApplierManager *pLabelApplierManager,
        //WrapperManager *pWrapperManager,
        //LabelApplierManager *pBakeryLabelApplierManager)
        Hobart::CEvolMsgReceiver *pWeigherManager,
        Hobart::CEvolMsgReceiver *pFirstPrinter,
        Hobart::CEvolMsgReceiver *pSecondPrinter,
        Hobart::CEvolMsgReceiver *pThirdPrinter,
        Hobart::CEvolMsgReceiver *pLabelApplierManager,
        Hobart::CEvolMsgReceiver *pWrapperManager,
        Hobart::CEvolMsgReceiver *pBakeryLabelApplierManager)
:       OperatingMode("tEvWrBkAW",
        pCurrentProduct,
        pWeigherManager,
        pFirstPrinter,
        pSecondPrinter,
        pThirdPrinter,
        pLabelApplierManager,
        pWrapperManager,
        pBakeryLabelApplierManager),
    AUTO_MaxLabels(7),
    MANUAL_MaxLabels(1),
    TakeLabelTimeout(12)
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = registerDataType(dataTypeName);

    addInstance(dataType);
#endif

    missedProductUpdates = 0;
    wrapOnlyMode = false;
    machineRunning = true;

    wrapStateMachine = awmWrapInitial;
    Debug::trace("WrapperMode.StateMachine", "wrapStateMachine: awmWrapInitial");

    laStateMachine1 = wmLaLabelTaken;
    Debug::trace("WrapperMode.StateMachine", "laStateMachine1: wmLaLabelTaken");

    laStateMachine2 = wmLaLabelTaken;
    Debug::trace("WrapperMode.StateMachine", "laStateMachine2: wmLaLabelTaken");

    laStateMachine3 = wmLaLabelTaken;
    Debug::trace("WrapperMode.StateMachine", "laStateMachine3: wmLaLabelTaken");

    forcePreImage = false;

    modeType = EvolWeighMode;

    wrStateInfo = EvolWrapperStatusInfo::Unknown;
    curWrapState = WrapperSetWrapMode::UnknownMode;

    pChangingProductPendingPackage = NULL;

    if (Debug::isTracing("AutoWeighEvolWrapperBakeryMode"))
    {
        HobString msgDebug = "Constructor: ";
        HobString msgDebug2 = msgDebug;
        msgDebug2.append("wrapMode = ").append(Integer::toString(wrapMode));
        msgDebug2.append("  intakeMode = ").append(Integer::toString(intakeMode));
        msgDebug2.append("  pCurrentProduct = ").append(Hex::toString((long long) pCurrentProduct));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);

        msgDebug2 = msgDebug;
        msgDebug2.append("this = ").append(Hex::toString((long long) this));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
    }
    myInitialWrapMode = wrapMode;

    setIntakeMode((WrapperIntakeMode::IntakeMode) Configuration::getInstance()->getDefaultIntakeMode());
    wrapperSettings.setFilmTension((WrFilmTension) Configuration::getInstance()->getDefaultFilmTension());
    wrapperSettings.setFilmWidth(Automatic);
    wrapperSettings.setFilmTensionFrontToBack((INT16)Configuration::getInstance()->getDefaultFilmTensionFrontToBack());
    wrapperSettings.setFilmTensionRightToLeft((INT16)Configuration::getInstance()->getDefaultFilmTensionLeftToRight());
    settingsInitialized = false;
    reqSettingsPending = false;
    updateApplierSetup = false;

    bakeryPackId = 0;
    waitingForLabelReady = false;
    gotPrintPackageMsg = false;
    //printPkgTimerActive = false;
    applyError = false;
    applyErrorHandled = false;

    resetPackIdData_ = true;            //      Set to true so resetPackIdData() will be called before we run a package.

    labelIsMissing = false;                     //      Set when we get a missing label msg from PrinterStatus
    missingLabelHandled = false;

    registerEvent(ProductDev, ProductChange);
    registerEvent(ProductDev, ProductUpdate);
    registerEvent(ProductDev, ProductStatusEvent);

    registerEvent(Printer, PrStatus);
    registerEvent(Printer, PrLabelReady);
    registerEvent(Printer, PrLabelTaken);
    registerEvent(Printer, PrLabelComplete);
    registerEvent(Printer, PrStockMetrics);
    registerEvent(Printer, PrNomSize);

    registerEvent(Weigher, WgStatus);
    registerEvent(Weigher, WgCachedStatus);

    registerEvent(Wrapper, WrEvolInquire);
    registerEvent(Wrapper, WrOpen);
    registerEvent(Wrapper, WrEvolStatus);
    registerEvent(Wrapper, WrSetWrapMode);
    registerEvent(Wrapper, WrEvolControl);
    registerEvent(Wrapper, WrReqSettings);
    registerEvent(Wrapper, WrTest);
    registerEvent(Wrapper, WrEvolConfig);

    registerEvent(LabelApplier, LaEvolInquire);
    registerEvent(LabelApplier, LaPrintPackage);
    registerEvent(LabelApplier, LaLabelReady);

    handleWrapSetMode(myInitialWrapMode);

    requestWeigherStatus();
    requestWrapperStatus();
    requestWrapperInquire();
    setWrapperMode(WrOperateMode);

    setTakeLabelTime(FirstPrinter, TakeLabelTimeout);
    setTakeLabelTime(SecondPrinter, TakeLabelTimeout);
    setTakeLabelTime(ThirdPrinter, TakeLabelTimeout);

    openWrapWindow();

    requestLabelSize();

    if (pCurrentProduct != NULL)
    {
        OperatingMode::changeProduct();
        preImagePrinters();
    }
}

AutoWeighEvolWrapperBakeryMode::~AutoWeighEvolWrapperBakeryMode()
{
#ifdef USE_ACCOUNTANT
    removeInstance(dataType);
#endif

    //Debug::trace("AutoWeighEvolWrapperBakeryMode", "Destructor...  0x" + Hex::toString((long) this));
    deregisterAllEvents();

    emptyPackageMap();

    sendPrinterReset(FirstPrinter);
    sendPrinterReset(SecondPrinter);
    sendPrinterReset(ThirdPrinter);

    WrapperIntakeMode intakeModeMsg(this, wrapperSettings.getIntakeMode().getIntakeMode());
    dispatchEvent(&intakeModeMsg);
    Debug::trace("OperatingMode", HobString(tName) + HobString(" -- Going away!!!"));
    Debug::trace("AutoWeighEvolWrapperBakeryMode", "Destructor...  We are done!");
}                               


void AutoWeighEvolWrapperBakeryMode::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{       
    switch (msg->getDeviceType())
    {
        case ProductDev:
            {
                handleProductMessage((ProductEvent) msg->getEventType(), msg->Data());
                break;
            }
        case Printer:
            {
                handlePrinterMessage((PrEvent) msg->getEventType(), msg->Data());
                break;
            }
        case Wrapper:
            {
                handleWrapperMessage((WrEvent) msg->getEventType(), msg->Data());
                break;
            }
        case Weigher:
            {
                handleWeigherMessage((WgEvent) msg->getEventType(), msg->Data());
                break;
            }
        case LabelApplier:
            {
                handleLabelApplierMessage((LaEvent) msg->getEventType(), msg->Data());
                break;
            }
        default:
            {
                break;
            }
    }
}

void AutoWeighEvolWrapperBakeryMode::timeoutProcess( __int32 UNUSED(timer_id) )
{
    UNREFERENCED_PARAMETER(timer_id);

    if (applyError && !applyErrorHandled)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", "AFT:  applyError is true and applyErrorHandled is false. Stop the presses!");
        setCLAPackageMissing();
    }
    if (labelIsMissing && !applyErrorHandled)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", "AFT:  labelIsMissing is true and applyErrorHandled is false. Stop the presses!");
        {
            opModeStatus.setBakeryCLALabelMissing();
            dispatchEvent(&opModeStatus);

            applyErrorHandled = true;

            ServiceSpeaker::badBeep(2);
            resetWrapperMode();

            emptyPackageMap();

            sendPrinterReset(FirstPrinter);
            sendPrinterReset(SecondPrinter);
        }
    }
}

/* removed for port -- ats
   int AutoWeighEvolWrapperBakeryMode::activeFunctionTask()
   {    
   Debug::trace("AutoWeighEvolWrapperBakeryMode", "AutoWeighEvolWrapperBakeryMode::activeFunctionTask() -- entered");

   char recvBuffer[1024];
   int msgSize = 0;

   while (!activeFunctionStart)
   {
   Debug::trace("AutoWeighEvolWrapperBakeryMode", "AutoWeighEvolWrapperBakeryMode::activeFunctionTask() -- activeFunctionStart is false, delaying 10 ticks");
   taskDelay(10);
   }


   if (Debug::isTracing("AutoWeighEvolWrapperBakeryMode"))
   {
   HobString msgDebug = "activeFunctionTask: myInitialWrapMode = ";
   msgDebug.append(Integer::toString(myInitialWrapMode));
   msgDebug.append("  pCurrentProduct = ").append(Hex::toString((long) pCurrentProduct));
   Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
   }

   if (pBakeryLabelApplierManager && pBakeryLabelApplierManager->isBakeryCLA())
   {
   pBakeryLabelApplierManager->switchToBakeryCLA();     //      This will set the mode to Auto for us.
   Debug::trace("AutoWeighEvolWrapperBakeryMode", "activeFunctionTask:   Calling pBakeryLabelApplierManager->switchToBakeryCLA()");
   }


   if (applyError && !applyErrorHandled)
   {
   Debug::trace("AutoWeighEvolWrapperBakeryMode", "AFT:  applyError is true and applyErrorHandled is false. Stop the presses!");
   setCLAPackageMissing();
   }
   if (labelIsMissing && !applyErrorHandled)
   {
   Debug::trace("AutoWeighEvolWrapperBakeryMode", "AFT:  labelIsMissing is true and applyErrorHandled is false. Stop the presses!");
   {
   opModeStatus.setBakeryCLALabelMissing();
   dispatchEvent(&opModeStatus, this);

   applyErrorHandled = true;

   ServiceSpeaker::getInstance()->badBeep(2);
   resetWrapperMode();

   emptyPackageMap();

   sendPrinterReset(FirstPrinter);
   sendPrinterReset(SecondPrinter);
   }
   }

   taskDelay(1);
   }
   return 0;
   }
   */
void AutoWeighEvolWrapperBakeryMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
    HobString msgDebug("handleProductMessage() - ");
    bool debuggingOpModeProd = Debug::isTracing("OperatingMode.Product");

    switch (productEvent)
    {
        // Operator has chosen a new product
        case ProductChange:
            {
                updateApplierSetup = false;                     //      A new product nullifies this condition.

                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + HobString("ProductChange"));
                ProductChangeMsg prodChange(recvBuffer);
                //Debug::trace("OperatingMode.ChangeProduct", "ProductChange: 0x" + Hex::toString((UINT32)  prodChange.getProduct()));
                //Debug::trace("OperatingMode.Product", "packageMap.size(): " + HobString(Integer::toString(packageMap.size())));
                //Debug::trace("OperatingMode.Product", "packageMap.empty(): " + HobString(Bool::toString(packageMap.empty())));

                if ((Product *) prodChange.getProduct() == NULL)
                {
                    if (productQ.empty())
                    {
                        Debug::trace("OperatingMode.ChangeProduct", "ProductChange:  Exiting because the new product is 0!.");
                    }
                    return;
                }

                // If changing the plu on the fly more than once before the next package is taken in, remove the previous
                // plu from the product queue (but only if there has been no stable weight for a package placed on the intake).
                // This assumes they can only have one other plu in the queue.
                if (wrapStateMachine == awmWrapChangingProduct)
                {
                    emptyProductQ();
                    if (pChangingProductPendingPackage)
                    {
                        Debug::trace("WrapperMode.ProductChange", "pChangingProductPendingPackage not null; cancelling");
                        sendProductComplete(pChangingProductPendingPackage, false);
                        pChangingProductPendingPackage = NULL;
                    }
                }

                if (pPendingPackage != NULL)
                {
                    Debug::trace("WrapperMode.ProductChange", "pPendingPackage not null; cancelling");
                    sendProductComplete(pPendingPackage, false);
                    pPendingPackage = NULL;
                }

                forcePreImage = true;
                productQ.push((Product *) prodChange.getProduct());
                wrapStateMachine = awmWrapChangingProduct;
                Debug::trace("WrapperMode.StateMachine", "wrapStateMachine: awmWrapChangingProduct");

                if (packageMap.empty() && getNewProduct())
                {
                    Debug::trace("WrapperMode.ProductChange", "changing product");
                    changeProduct();
                    preImagePrinters();
                    openWrapWindow();
                }
                break;
            }

        case ProductUpdate:
            {
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + HobString("ProductUpdate"));

                ProductUpdateMsg prodUpdate(recvBuffer);
                const ProductTransaction *pNextProductTransaction = const_cast<ProductTransaction *>(prodUpdate.getProductTransaction());

				if (pNextProductTransaction)
					Debug::trace("TestLeaks", "AutoWeighEvolWrapperMode::handleProductMessage():  pNextProductTransaction = 0x"  + Hex::toString((long) pNextProductTransaction) + "  for product# " + HobString(Long::toString(pNextProductTransaction->getProductNum())) + ",  Trans# = " + Long::toString(pNextProductTransaction->getTransactionNum()));

                if (debuggingOpModeProd)
                {       
					HobString curWrapStateStr;
					WrapperSetWrapMode::getWrapModeText(curWrapState, curWrapStateStr);

					Debug::trace("OperatingMode.Product", HobString("Product Update: ") + HobString(Hex::toString((UINT32) pNextProductTransaction->getTransactionNum())));
					HobWeight packageWeight = pNextProductTransaction->getWeigherStatus()->getWeight();
					Debug::trace("OperatingMode.Product", "     packageWeight: " + HobString(Integer::toString(packageWeight)));
					Debug::trace("OperatingMode.Product", "    machineRunning: " + HobString(Bool::toString(machineRunning)));
					Debug::trace("OperatingMode.Product", "   laStateMachine1: " + inttostring(laStateMachine1));
					Debug::trace("OperatingMode.Product", "   laStateMachine2: " + inttostring(laStateMachine2));
					Debug::trace("OperatingMode.Product", "   laStateMachine3: " + inttostring(laStateMachine3));
					Debug::trace("OperatingMode.Product", "       largeMotion: " + HobString(Bool::toString(largeMotion)));
					Debug::trace("OperatingMode.Product", "  wrapStateMachine: " + HobString(Integer::toString(wrapStateMachine)));
					Debug::trace("OperatingMode.Product", "      curWrapState: " + curWrapStateStr);
					Debug::trace("OperatingMode.Product", "      wrapOnlyMode: " + HobString(Bool::toString(wrapOnlyMode)));
					Debug::trace("OperatingMode.Product", "       wrStateInfo: " + HobString(Integer::toString(wrStateInfo)));
					Debug::trace("OperatingMode.Product", " packageMap.size(): " + HobString(Integer::toString(packageMap.size())));
					Debug::trace("OperatingMode.Product", "packageMap.empty(): " + HobString(Bool::toString(packageMap.empty())));
					Debug::trace("OperatingMode.Product", "   pPendingPackage: " + HobString(Hex::toString((UINT32) pPendingPackage)));
					Debug::trace("OperatingMode.Product", HobString("pNextProductTransaction: ") + HobString(Hex::toString((UINT32) pNextProductTransaction)));
                }

                if (curWrapState == WrapperSetWrapMode::ManualWeigh)
                {
                    // Don't care about the state of the wrapper in these two cases, since the package is not entering
                    // the wrapper for these states.  We only want a label printed.
                    Debug::trace("WrapperMode.ProductUpdate", "Wrapping condition 1: curWrapState == ManualWeigh");

                    pCurrentProductTransaction = pNextProductTransaction;
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", "ManualWeigh. Set pCurrentProductTransaction = " + HobString(Hex::toString((long long) pCurrentProductTransaction)));

	    			wrPackageWeight = (HobWeight) pCurrentProductTransaction->getWeigherStatus()->getWeight();

		    		if (Configuration::getInstance()->isScaleAvoir() && pCurrentProductTransaction->getWeigherStatus()->isDualRangeEnabled())
			    		wrPackageWeight = (wrPackageWeight + 5) / 10;		//	Round up to 2 decimal places

				    wrapPackage(wrPackageWeight);
                }
                else if (curWrapState == WrapperSetWrapMode::AutoManualApply)
                {
                    bool isProdRandomWt = true;
                    Product *pTheProduct = (pNextProductTransaction ? pNextProductTransaction->getProduct() : 0);
                    if (pTheProduct)
                        isProdRandomWt = pTheProduct->isRandomWeight();
                    if (debuggingOpModeProd)
                        Debug::trace("OperatingMode.Product", "AutoManualApply: packageMap.size() = " + inttostring(packageMap.size()) +
                                "   MANUAL_MaxLabels = " +      inttostring(MANUAL_MaxLabels) +
                                "   largeMotion: " + booltostring(largeMotion) +
                                "   pTheProduct = " +   HobString(Hex::toString((long long) pTheProduct)) +
                                "   isProdRandomWt() = " +      booltostring(isProdRandomWt));

                    if (packageMap.size() < MANUAL_MaxLabels && (largeMotion || (!largeMotion && !isProdRandomWt)))
                    {
                        if (debuggingOpModeProd)
                            Debug::trace("OperatingMode.Product", "AutoManualApply. Conditions are good. Calling wrapPackage()");

                        pCurrentProductTransaction = pNextProductTransaction;
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", "AutoManualApply. Set pCurrentProductTransaction = " + HobString(Hex::toString((long long) pCurrentProductTransaction)));

	        			wrPackageWeight = (HobWeight) pCurrentProductTransaction->getWeigherStatus()->getWeight();

		        		if (Configuration::getInstance()->isScaleAvoir() && pCurrentProductTransaction->getWeigherStatus()->isDualRangeEnabled())
			        		wrPackageWeight = (wrPackageWeight + 5) / 10;		//	Round up to 2 decimal places

				        wrapPackage(wrPackageWeight);
                    }
                    else
                    {
                        if (debuggingOpModeProd)
                            Debug::trace("OperatingMode.Product", "AutoManualApply. packageMap.size() and/or largeMotion do not meet criteria to print. Not calling wrapPackage().");

                        if (pPendingPackage)
                        {
                            if (debuggingOpModeProd)
                                Debug::trace("OperatingMode.Product", HobString("sendProductComplete() for Product trans#: ") + HobString(Hex::toString(pPendingPackage->getTransactionNum())));
                            sendProductComplete(const_cast <ProductTransaction *> (pPendingPackage), false);
                        }
                        pPendingPackage = pNextProductTransaction;
                        if (debuggingOpModeProd)
                            Debug::trace("OperatingMode.Product", HobString("pPendingPackage is now Product trans#: ") + HobString(Hex::toString(pPendingPackage->getTransactionNum())));
                    }
                }

                // WR_STANDBY -- NO WR_PAUSED STATE
                else if ((opModeStatus.isWrapperGhostPackage() == true) || (wrapOnlyMode == true) /* || (wrStateInfo == EvolWrapperStatusInfo::Standby) */)
                {
                    Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 1");

                    sendProductComplete(pNextProductTransaction, false);
                    if (pPendingPackage)
                    {
                        sendProductComplete(pPendingPackage, false);
                        pPendingPackage = NULL;
                    }
                }
                else if (wrapStateMachine == awmWrapError)
                {
                    Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 2: wrapper error");

                    sendProductComplete(pNextProductTransaction, false);
                    if (pPendingPackage)
                    {
                        sendProductComplete(pPendingPackage, false);
                        pPendingPackage = NULL;
                    }
                }
                else if (opModeStatus.isBakeryCLAPackageMissing())
                {
                    Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 2X:  CLAPackageMissing error");

                    sendProductComplete(pNextProductTransaction, false);
                    if (pPendingPackage)
                    {
                        sendProductComplete(pPendingPackage, false);
                        pPendingPackage = NULL;
                    }
                    applyError = true;
                    applyErrorHandled = false;
                }
                else if (opModeStatus.isBakeryCLALabelMissing())
                {
                    Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 3XXXX:  LabelMissing error");

                    sendProductComplete(pNextProductTransaction, false);
                    if (pPendingPackage)
                    {
                        sendProductComplete(pPendingPackage, false);
                        pPendingPackage = NULL;
                    }
                    labelIsMissing = true;
                    applyErrorHandled = false;
                }
                /*
                   else if (packageWeightIsTooHeavyForAWS(packageWeight))
                   {
                   Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 3: too heavy package");

                   sendProductComplete(pNextProductTransaction, false);
                // beep on this Message!!
                setPackageOverWeight();
                }
                */
                else
                {
                    if (wrapStateMachine == awmWrapChangingProduct &&
                            (machineRunning || wrStateInfo == EvolWrapperStatusInfo::Standby) &&
                            largeMotion)
                    {
                        Debug::trace("WrapperMode.ProductUpdate", "Not wrapping yet:  wrapStateMachine == awmWrapChangingProduct");

                        if (pPendingPackage)
                        {
                            sendProductComplete(pPendingPackage, false);
                            pPendingPackage = NULL;
                        }

                        if (pChangingProductPendingPackage)
                            sendProductComplete(pChangingProductPendingPackage, false);
                        pChangingProductPendingPackage = pNextProductTransaction;
                    }
                    else if (packageMap.size() >= AUTO_MaxLabels)
                    {
                        Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 4: too many packages");

                        handlePrinterOverflow(AUTO_MaxLabels);
                        sendProductComplete(pNextProductTransaction, false);
                    }
                    else if (wrapStateMachine == awmWrapOpenWindow &&
                            (machineRunning || wrStateInfo == EvolWrapperStatusInfo::Standby) &&
                            largeMotion)
                    {
                        Debug::trace("WrapperMode.ProductUpdate", "Wrapping condition 2: open window");

                        pCurrentProductTransaction = pNextProductTransaction;
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", "Open window. Set pCurrentProductTransaction = " + HobString(Hex::toString((long long) pCurrentProductTransaction)));

	        			wrPackageWeight = (HobWeight) pCurrentProductTransaction->getWeigherStatus()->getWeight();
		 
		        		if (Configuration::getInstance()->isScaleAvoir() && pCurrentProductTransaction->getWeigherStatus()->isDualRangeEnabled())
			        		wrPackageWeight = (wrPackageWeight + 5) / 10;		//	Round up to 2 decimal places

				        wrapPackage(wrPackageWeight);
                    }
                    else if (wrapStateMachine != awmWrapOpenWindow)
                    {
                        Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 8:  wrapStateMachine = " + inttostring(wrapStateMachine));
                    }
                    else if ((machineRunning == false) && largeMotion)
                    {
                        if (packageMap.size() < MANUAL_MaxLabels)
                        {
                            Debug::trace("WrapperMode.ProductUpdate", "Wrapping condition 3: large motion");

                            pCurrentProductTransaction = pNextProductTransaction;
                            Debug::trace("AutoWeighEvolWrapperBakeryMode", "Large motion. Set pCurrentProductTransaction = " + HobString(Hex::toString((long long) pCurrentProductTransaction)));

	            			wrPackageWeight = (HobWeight) pCurrentProductTransaction->getWeigherStatus()->getWeight();
			 
		            		if (Configuration::getInstance()->isScaleAvoir() && pCurrentProductTransaction->getWeigherStatus()->isDualRangeEnabled())
			    	        	wrPackageWeight = (wrPackageWeight + 5) / 10;		//	Round up to 2 decimal places

	        			    wrapPackage(wrPackageWeight);
                        }
                        else
                        {
                            Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 5 ???: too many packages");

                            if (pPendingPackage)
                                sendProductComplete(pPendingPackage, false);
                            pPendingPackage = pNextProductTransaction;
                        }
                    }
                    else
                    {
                        if (Debug::isTracing("WrapperMode.ProductUpdate"))
                        {
                            Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 6    *************************");
                            Debug::trace("WrapperMode.ProductUpdate", "wrapStateMachine = " + inttostring(wrapStateMachine));
                            Debug::trace("WrapperMode.ProductUpdate", "wrapStateMachine == awmWrapOpenWindow: " +
                                    HobString((wrapStateMachine == awmWrapOpenWindow) ? "true" : "false"));
                            Debug::trace("WrapperMode.ProductUpdate", "machineRunning: " + HobString(machineRunning ? "true" : "false"));
                            Debug::trace("WrapperMode.ProductUpdate", "largeMotion: " + HobString(largeMotion ? "true" : "false"));

                            Debug::trace("WrapperMode.ProductUpdate", "laStateMachine1: " + inttostring(laStateMachine1));
                            Debug::trace("WrapperMode.ProductUpdate", "laStateMachine2: " + inttostring(laStateMachine2));
                            Debug::trace("WrapperMode.ProductUpdate", "laStateMachine3: " + inttostring(laStateMachine3));
                            Debug::trace("WrapperMode.ProductUpdate", "END Not wrapping condition 6    *********************");
                        }

                        sendProductComplete(pNextProductTransaction, false);
                        missedProductUpdates++;
                    }
                }
                break;
            }
        case ProductStatusEvent:
            {
                if (debuggingOpModeProd)
                    Debug::trace("OperatingMode.Product", "ProductDev|ProductStatusEvent received");

                ProductStatusMsg prodStatus(recvBuffer);
                if (prodStatus.getStatus() == ProductStatusMsg::WeightBelowMinimumWeightToPrint)
                {
                    if (debuggingOpModeProd)
                        Debug::trace("OperatingMode.Product", "ProductDev|ProductStatusEvent received - Ghost Package: 'packageWeight' is less than min weight to print");
                    setGhostPackage();
                    ghostPackage = true;
                }
                break;
            }
        default:
            {
                break;
            }
    }                                   
}

void AutoWeighEvolWrapperBakeryMode::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer)
{
    switch (prEvent)
    {
        case PrStatus: 
            {
                Debug::trace("AutoWeighEvolWrapperBakeryMode", "  PrStatus");
                PrinterStatus prStatus(recvBuffer);
                PrinterType printerType = prStatus.getPrinterType();

                if (prStatus.isLabelMissing())
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", "  PrStatus:  Label is Missing");
                else if (prStatus.isLabelSizing())
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", "  PrStatus:  Label is Sizing");
                else if (prStatus.isOutOfMedia())
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", "  PrStatus:  Out Of Media");
                else if (prStatus.isMediaJammed())
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", "  PrStatus:  Media is Jammed");

                if (prStatus.isPriorityError() == true)
                {
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", "  PrStatus:  isPriorityError() == true");
                    updatePrinterHeadUp(printerType);

                    if (wrapStateMachine != awmWrapPrinterHeadUp)
                    {
                        emptyPackageMap();
                        resetWrapperMode();
                        resetPackIdData();
                    }
                }
                else if (prStatus.isSizeCmdComplete() == true)
                {
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", "  PrStatus:  isSizeCmdComplete() == true");
                    updateSizeCmdComplete(printerType);

                    if (isPrinterHeadUp() == false)
                    {
                        if ((getNewProduct() == true) || (pCurrentProduct != NULL))
                        {
                            changeProduct();
                            preImagePrinters(); 

                            openWrapWindow();
                        }
                        else
                        {
                            wrapStateMachine = awmWrapInitial;
                            Debug::trace("WrapperMode.StateMachine", "wrapStateMachine: awmWrapInitial");
                        }
                    }
                }
                else
                {
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", "  PrStatus:  Check other states.");
                    if (prStatus.isTakeLabelError())
                    {
                        HobString msgDebug = "PrStatus:  TakeLabel Error  -  ";
                        if (printerType == FirstPrinter)
                        {
                            laStateMachine1 = wmLaError;
                            msgDebug.append("laStateMachine1 = wmLaError");
                        }
                        else if (printerType == SecondPrinter)
                        {
                            laStateMachine2 = wmLaError;
                            msgDebug.append("laStateMachine2 = wmLaError");
                        }
                        else if (printerType == ThirdPrinter)
                        {
                            laStateMachine3 = wmLaError;
                            msgDebug.append("laStateMachine3 = wmLaError");
                        }
                        else
                            msgDebug.append("???");
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
                    }
                    else
                    {
                        HobString msgDebug = "";
                        if ((printerType == FirstPrinter) && (laStateMachine1 == wmLaError))
                        {
                            laStateMachine1 = wmLaLabelTaken;
                            msgDebug.append("PrStatus:  TakeLabel Error cleared.  laStateMachine1 = wmLaLabelTaken");
                        }
                        else if ((printerType == SecondPrinter) && (laStateMachine2 == wmLaError))
                        {
                            laStateMachine2 = wmLaLabelTaken;
                            msgDebug.append("PrStatus:  TakeLabel Error cleared.  laStateMachine2 = wmLaLabelTaken");
                        }
                        else if ((printerType == ThirdPrinter) && (laStateMachine3 == wmLaError))
                        {
                            laStateMachine3 = wmLaLabelTaken;
                            msgDebug.append("PrStatus:  TakeLabel Error cleared.  laStateMachine3 = wmLaLabelTaken");
                        }
                        if (!msgDebug.empty())
                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
                    }
                    if (prStatus.isLabelTakenError())
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", "PrStatus:  LabelTaken Error!");
                }
                if (prStatus.isLabelMissing())
                    labelIsMissing = true;
                else
                    labelIsMissing = false;
                Debug::trace("AutoWeighEvolWrapperBakeryMode", "PrStatus:  labelIsMissing = " + inttostring(labelIsMissing));
                break;
            }
            // We listen to this message with the only purpose of relaying this
            // information to the corresponding label applier device.
        case PrLabelReady: 
            {
                bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
                HobString msgDebug;
                if (traceOn)
                {
                    msgDebug = "PrLabelReady";
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
                    msgDebug.append(":   ");
                }

                PrinterLabelReady prLabelReady(recvBuffer);
                PrinterType printerType = prLabelReady.getPrinterType();
#if 0
                if (traceOn)
                {
                    if (!pLabelApplierManager)
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "No pLabelApplierManager!");
                    else
                    {
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "pLAMgr canRespondToPrimaryStation = " + booltostring(pLabelApplierManager->canRespondToPrimaryStation()));
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "pLAMgr  canRespondToSecondStation = " + booltostring(pLabelApplierManager->canRespondToSecondStation()));
                    }

                    if (!pBakeryLabelApplierManager)
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "No pBakeryLabelApplierManager!");
                    else
                    {
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "pBLAMgr canRespondToPrimaryStation = " + booltostring(pBakeryLabelApplierManager->canRespondToPrimaryStation()));
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "pBLAMgr  canRespondToSecondStation = " + booltostring(pBakeryLabelApplierManager->canRespondToSecondStation()));
                    }
                }
#endif
                if ( printerType == FirstPrinter )
                {
                    if (traceOn)
                    {
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "printerType = FirstPrinter");
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "waitingForLabelReady = " + booltostring(waitingForLabelReady));
                    }
                    laStateMachine1 = wmLaLabelReady;
                    Debug::trace("WrapperMode.StateMachine", "laStateMachine1 = wmLaLabelReady");

                    if (!applyError && !packageMap.empty())
                    {
                        if (pBakeryLabelApplierManager && pBakeryLabelApplierManager->canRespondToPrimaryStation())
                        {
                            setCLAReadyPackId(prLabelReady.getPackId());

                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "BakeryLabelApplierManager canRespondToPrimaryStation.");
                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "packId = " + inttostring(prLabelReady.getPackId()) + ". Append to bakeryLabelReady.");
                            bakeryLabelReady.push_back(prLabelReady.getPackId());
                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "bakeryLabelReady.push_back(" + inttostring(prLabelReady.getPackId()) + ")");
                            if (waitingForLabelReady)
                            {
                                claPrintPackage(bakeryPackId);
                                waitingForLabelReady = false;
                                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Now waitingForLabelReady = false");
                            }
                        }
                        else if (pLabelApplierManager && pLabelApplierManager->canRespondToPrimaryStation())
                        {
                            setAWSReadyPackId(prLabelReady.getPackId());

                            sendLabelReady(PrimaryPrinterStation, prLabelReady.getPackId());
                        }
                    }
                }
                else if ( printerType == SecondPrinter )
                {
                    if (traceOn)
                    {
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "printerType = SecondPrinter");
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "waitingForLabelReady = " + booltostring(waitingForLabelReady));
                    }
                    laStateMachine2 = wmLaLabelReady;
                    Debug::trace("WrapperMode.StateMachine", "laStateMachine2 = wmLaLabelReady");

                    if (!applyError && !packageMap.empty())
                    {
                        if (pBakeryLabelApplierManager && pBakeryLabelApplierManager->canRespondToSecondStation())
                        {
                            setCLAReadyPackId(prLabelReady.getPackId());

                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "BakeryLabelApplierManager canRespondToSecondStation.");
                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "packId = " + inttostring(prLabelReady.getPackId()) + ". Append to bakeryLabelReady.");
                            bakeryLabelReady.push_back(prLabelReady.getPackId());
                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "bakeryLabelReady.push_back(" + inttostring(prLabelReady.getPackId()) + ")");
                            if (waitingForLabelReady)
                            {
                                claPrintPackage(bakeryPackId);
                                waitingForLabelReady = false;
                                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Now waitingForLabelReady = false");
                            }
                        }
                        else if (pLabelApplierManager && pLabelApplierManager->canRespondToSecondStation())
                        {
                            setAWSReadyPackId(prLabelReady.getPackId());

                            sendLabelReady(SecondaryPrinterStation, prLabelReady.getPackId());
                        }
                    }
                }
                else if ( printerType == ThirdPrinter )
                {
                    if (traceOn)
                    {
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "printerType = ThirdPrinter");
                    }
                    laStateMachine3 = wmLaLabelReady;
                    Debug::trace("WrapperMode.StateMachine", "laStateMachine3 = wmLaLabelReady");

                    sendLabelReady(ThirdPrinterStation, prLabelReady.getPackId());
                }
                if (traceOn)
                {
                    HobString theSize = bakeryLabelReady.empty() ? "0" : inttostring(bakeryLabelReady.size());
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "bakeryLabelReady.size() = " + theSize);
                }
                break;
            }   
            // We listen to this message with the only purpose of relaying this
            // information to the corresponding label applier device.
        case PrLabelTaken: 
            {
                bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
                HobString msgDebug;

                PrinterLabelReady prLabelTaken(recvBuffer);
                PrinterType printerType = prLabelTaken.getPrinterType();
                if (traceOn)
                {
                    if ( printerType == FirstPrinter )
                        msgDebug = "PrLabelTaken1";
                    else
                        msgDebug = "PrLabelTaken2";
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
                    msgDebug.append(":   ");
                }

                if (laStateMachine1 == wmLaError || laStateMachine2 == wmLaError || laStateMachine3 == wmLaError)
                    requestWrapperStatus();

                if ( printerType == FirstPrinter )
                {
                    laStateMachine1 = wmLaLabelTaken;
                    Debug::trace("WrapperMode.StateMachine", "laStateMachine1 = wmLaLabelTaken");
                    if (traceOn)
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "printerType = FirstPrinter");

                    if (!applyError && !packageMap.empty())
                    {
                        if (pBakeryLabelApplierManager && pBakeryLabelApplierManager->canRespondToPrimaryStation())
                        {
                            setCLATakenPackId(prLabelTaken.getPackId());
                            if ( !(curWrapState == WrapperSetWrapMode::AutoManualApply || curWrapState == WrapperSetWrapMode::ManualWeigh) &&
                                    prLabelTaken.getPackId() != claCurrentPackId )
                            {
                                if (traceOn)
                                {
                                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Error!  Label taken id = " + inttostring(prLabelTaken.getPackId()) + ",  claCurrentPackId = " + inttostring(claCurrentPackId) + " when they should be equal.");
                                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "(wrapStateMachine == awmWrapError) = " + booltostring(wrapStateMachine == awmWrapError));
                                }
                                applyError = true;
                            }
                            else
                            {
                                claCurrentPackId = 0;
                            }
                            if (traceOn)
                            {
                                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Now claCurrentPackId = " + inttostring(claCurrentPackId));
                                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "BakeryLabelApplierManager canRespondToPrimaryStation.");
                                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "bakeryPackId = " + inttostring(bakeryPackId) + ".");
                            }
                            LabelApplierLabelTaken labelTaken(this, PrimaryPrinterStation, bakeryPackId);
                            dispatchEvent(&labelTaken, this);

                            if (traceOn)
                                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "packId = " + inttostring(prLabelTaken.getPackId()) + ".");
                            if (bakeryLabelReady.empty())
                            {
                                if (traceOn)
                                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "bakeryLabelReady.size() is empty! No packId to remove.");
                            }
                            else
                            {
                                //      Remove from the bakeryLabelReady list.
                                bool foundIt = false;
                                deque<UINT8>::iterator pos = bakeryLabelReady.begin();
                                while (pos != bakeryLabelReady.end())
                                {
                                    if (*pos == prLabelTaken.getPackId())
                                    {
                                        if (traceOn)
                                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Removing from bakeryLabelReady for packId: " + inttostring(prLabelTaken.getPackId()) + ".");
                                        bakeryLabelReady.erase(pos);
                                        foundIt = true;
                                        break;
                                    }
                                }
                                if (!foundIt && traceOn)
                                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Could not remove from bakeryLabelReady for packId: " + inttostring(prLabelTaken.getPackId()) + ".");

                                //      Remove from the bakeryPackages list.
                                BakeryPackageMap::iterator pos2 = bakeryPackages.find(prLabelTaken.getPackId());
                                if (pos2 != bakeryPackages.end())
                                {
                                    if (traceOn)
                                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Removing from bakeryPackages for packId: " + inttostring(prLabelTaken.getPackId()) + ".");
                                    bakeryPackages.erase(pos2);
                                    HobString theSize = bakeryPackages.empty() ? "0" : inttostring(bakeryPackages.size());
                                    if (traceOn)
                                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "bakeryPackages.size() = " + theSize);
                                }
                                else
                                {
                                    if (traceOn)
                                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Could not find packId to remove from bakeryPackages!");
                                }
                            }
                            if  ( !(gotPrintPackageMsg || curWrapState == WrapperSetWrapMode::AutoManualApply || curWrapState == WrapperSetWrapMode::ManualWeigh) &&
                                    !packageMap.empty() )
                            {
                                applyError = true;
                                if (traceOn)
                                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Error! Did not receive the CLA_PRINT_PACKAGE msg.");
                            }
                            gotPrintPackageMsg = false;
                        }
                        else if (pLabelApplierManager && pLabelApplierManager->canRespondToPrimaryStation())
                        {
                            setAWSTakenPackId(prLabelTaken.getPackId());

                            sendLabelTaken(PrimaryPrinterStation, prLabelTaken.getPackId());
                        }
                    }
                }
                else if ( printerType == SecondPrinter )
                {
                    laStateMachine2 = wmLaLabelTaken;
                    Debug::trace("WrapperMode.StateMachine", "laStateMachine2 = wmLaLabelTaken");
                    if (traceOn)
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "printerType = SecondPrinter");

                    if (!applyError && !packageMap.empty())
                    {
                        if (pBakeryLabelApplierManager && pBakeryLabelApplierManager->canRespondToSecondStation())
                        {
                            setCLATakenPackId(prLabelTaken.getPackId());
                            if ( !(curWrapState == WrapperSetWrapMode::AutoManualApply || curWrapState == WrapperSetWrapMode::ManualWeigh) &&
                                    prLabelTaken.getPackId() != claCurrentPackId )
                            {
                                if (traceOn)
                                {
                                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Error!  Label taken id = " + inttostring(prLabelTaken.getPackId()) + ",  claCurrentPackId = " + inttostring(claCurrentPackId) + " when they should be equal.");
                                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "(wrapStateMachine == awmWrapError) = " + booltostring(wrapStateMachine == awmWrapError));
                                }
                                applyError = true;
                            }
                            else
                            {
                                claCurrentPackId = 0;
                            }
                            if (traceOn)
                            {
                                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Now claCurrentPackId = " + inttostring(claCurrentPackId));
                                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "BakeryLabelApplierManager canRespondToPrimaryStation.");
                                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "bakeryPackId = " + inttostring(bakeryPackId) + ".");
                            }
                            LabelApplierLabelTaken labelTaken(this, SecondaryPrinterStation, bakeryPackId);
                            labelTaken.setCommSwitchToPrimary(true);
                            dispatchEvent(&labelTaken, this);

                            if (traceOn)
                                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "packId = " + inttostring(prLabelTaken.getPackId()) + ". Set CommSwitchToPrimary to true.");
                            if (bakeryLabelReady.empty())
                            {
                                if (traceOn)
                                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "bakeryLabelReady.size() is empty! No packId to remove.");
                            }
                            else
                            {
                                //      Remove from the bakeryLabelReady list.
                                bool foundIt = false;
                                deque<UINT8>::iterator pos = bakeryLabelReady.begin();
                                while (pos != bakeryLabelReady.end())
                                {
                                    if (*pos == prLabelTaken.getPackId())
                                    {
                                        if (traceOn)
                                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Removing from bakeryLabelReady for packId: " + inttostring(prLabelTaken.getPackId()) + ".");
                                        bakeryLabelReady.erase(pos);
                                        foundIt = true;
                                        break;
                                    }
                                }
                                if (!foundIt && traceOn)
                                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Could not remove from bakeryLabelReady for packId: " + inttostring(prLabelTaken.getPackId()) + ".");

                                //      Remove from the bakeryPackages list.
                                BakeryPackageMap::iterator pos2 = bakeryPackages.find(prLabelTaken.getPackId());
                                if (pos2 != bakeryPackages.end())
                                {
                                    if (traceOn)
                                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Removing from bakeryPackages for packId: " + inttostring(prLabelTaken.getPackId()) + ".");
                                    bakeryPackages.erase(pos2);
                                    HobString theSize = bakeryPackages.empty() ? "0" : inttostring(bakeryPackages.size());
                                    if (traceOn)
                                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "bakeryPackages.size() = " + theSize);
                                }
                                else
                                {
                                    if (traceOn)
                                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Could not find packId to remove from bakeryPackages!");
                                }
                            }
                            if ( !(gotPrintPackageMsg || curWrapState == WrapperSetWrapMode::AutoManualApply || curWrapState == WrapperSetWrapMode::ManualWeigh) &&
                                    !packageMap.empty() )
                            {
                                applyError = true;
                                if (traceOn)
                                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Error! Did not receive the CLA_PRINT_PACKAGE msg.");
                            }
                            gotPrintPackageMsg = false;
                        }
                        else if (pLabelApplierManager && pLabelApplierManager->canRespondToSecondStation())
                        {
                            setAWSTakenPackId(prLabelTaken.getPackId());

                            sendLabelTaken(SecondaryPrinterStation, prLabelTaken.getPackId());
                        }
                    }
                }
                else if ( printerType == ThirdPrinter )
                {
                    laStateMachine3 = wmLaLabelTaken;
                    Debug::trace("WrapperMode.StateMachine", "laStateMachine3 = wmLaLabelTaken");
                    if (traceOn)
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "printerType = ThirdPrinter");
                    sendLabelTaken(ThirdPrinterStation, prLabelTaken.getPackId());

                }
                break;
            }   
        case PrLabelComplete: 
            {
                bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
                HobString msgDebug;
                if (traceOn)
                {
                    msgDebug = "PrLabelComplete";
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
                    msgDebug.append(":   ");
                }

                PrinterLabelComplete prLabelComplete(recvBuffer);
                if (traceOn)
                {
                    HobString msgDebug2 = msgDebug;
                    msgDebug.append("  transNum = ").append(Integer::toString(prLabelComplete.getRecordNumber()));
                    msgDebug.append(",  pProductTransaction = ").append(Hex::toString((long long) prLabelComplete.getProductTransaction()));
                    if (prLabelComplete.getProductTransaction())
                        msgDebug.append(",  transactionNum = ").append(Integer::toString(prLabelComplete.getProductTransaction()->getTransactionNum()));
                    msgDebug.append(",  printOk = ").append(booltostring(prLabelComplete.isPrintOk()));
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
                }

                if (updateProductComplete(prLabelComplete) == true)
                {
                    laStateMachine1 = wmLaLabelTaken;
                    Debug::trace("WrapperMode.StateMachine", "laStateMachine1 = wmLaLabelTaken");

                    laStateMachine2 = wmLaLabelTaken;
                    Debug::trace("WrapperMode.StateMachine", "laStateMachine2 = wmLaLabelTaken");

                    laStateMachine3 = wmLaLabelTaken;
                    Debug::trace("WrapperMode.StateMachine", "laStateMachine3 = wmLaLabelTaken");

                    if ((packageMap.empty() == true) && (wrapStateMachine == awmWrapChangingProduct))
                    {
                        Debug::trace("WrapperMode.StateMachine", "PrLabelComplete in awmWrapChangingProduct state");

                        if (getNewProduct() == true)
                        {
                            changeProduct();
                            preImagePrinters();
                        }

                        openWrapWindow();
                        pPendingPackage = pChangingProductPendingPackage;
                        pChangingProductPendingPackage = NULL;
                    }
                    else
                        unlockWeigher();

					if (pPendingPackage)
					{
						pCurrentProductTransaction = pPendingPackage;
						Debug::trace("AutoWeighEvolWrapperBakeryMode", "Set pCurrentProductTransaction to pPendingPackage = " + HobString(Hex::toString((UINT32) pCurrentProductTransaction)));
						pPendingPackage = NULL;
						largeMotion = true;

	        			wrPackageWeight = (HobWeight) pCurrentProductTransaction->getWeigherStatus()->getWeight();
		 
	            		if (Configuration::getInstance()->isScaleAvoir() && pCurrentProductTransaction->getWeigherStatus()->isDualRangeEnabled())
		    	        	wrPackageWeight = (wrPackageWeight + 5) / 10;		//	Round up to 2 decimal places

	    			    wrapPackage(wrPackageWeight);
					}
                }
                break;
            }   
        default:
            OperatingMode::handlePrinterMessage(prEvent, recvBuffer);
            break;
    }
}

void AutoWeighEvolWrapperBakeryMode::handleWeigherMessage(WgEvent wgEvent, const char *recvBuffer)
{
    switch (wgEvent)
    {
        case WgStatus:
            {
                WeigherStatus wgStatus(recvBuffer);

                if (Debug::isTracing("WrapperMode.WeigherStatus") == true)
                    wgStatus.show();

                HobWeight packageWeight = (HobWeight) wgStatus.getWeight();
                bool nonZeroStableWeight = (wgStatus.isLargeMotion() == false) && (wgStatus.isSmallMotion() == false) && (wgStatus.isWithinZeroQuarterGrad() == false) && (packageWeight != 0L);

                if (pChangingProductPendingPackage)
                {
                    Debug::trace("WrapperMode.ProductChange", "handleWeigherMessage - WgStatus - pChangingProductPendingPackage not null; cancelling");
                    sendProductComplete(pChangingProductPendingPackage, false);
                    pChangingProductPendingPackage = NULL;
                }

                if ((opModeStatus.isWrapperOverWeight() == true) &&
                        ((wgStatus.isLargeMotion() == true) || (wgStatus.isSmallMotion() == true) || (wgStatus.isZeroStableWeight() == true)))
                {
                    clearPackageOverWeight();
                }

                // WR_STANDBY -- NO WR_PAUSED STATE
                if (nonZeroStableWeight && (packageWeight < Configuration::getInstance()->getMinWeightToPrint()) && (wrapStateMachine == awmWrapOpenWindow) && largeMotion /* && pCurrentProduct */)
                {
                    Debug::trace("WrapperMode.WeigherStatus", "Ghost Package: 'nonZeroStableWeight' is true and 'packageWeight' is less than min weight to print and 'wrapStateMachine' is awmWrapOpenWindow");

                    setGhostPackage();
                    ghostPackage = true;
                }
                else if ((wgStatus.isPositiveNonZeroStableWeight() == true) && 
                        (packageWeight >= Configuration::getInstance()->getMinWeightToPrint()) &&
                        (largeMotion == true))
                {
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", "handleWeigherMessage(WgStatus):  We should be about ready to print, after some checks.");
                    /*
                       if (packageWeightIsTooHeavyForAWS(packageWeight))
                       {
                    // beep on this Message!!
                    setPackageOverWeight();
                    }
                    else
                    {
                    */
                    if (wrapStateMachine == awmWrapError &&
                            curWrapState != WrapperSetWrapMode::ManualWeigh &&
                            curWrapState != WrapperSetWrapMode::AutoManualApply)
                    {
                        Debug::trace("WrapperMode.WeigherStatus", "'wrapStateMachine' is awmWrapError");

                        // redisplay the error!
                        dispatchEvent(&opModeStatus, this);
                    }

                    HobString theState;
                    EvolWrapperStatusInfo::getWrapperStateName(wrStateInfo, theState);
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", "handleWeigherMessage(WgStatus):  wrStateInfo = " + theState);

                    if (wrStateInfo == EvolWrapperStatusInfo::Running || wrStateInfo == EvolWrapperStatusInfo::Standby ||
                            wrStateInfo == EvolWrapperStatusInfo::Homing || wrStateInfo == EvolWrapperStatusInfo::Idle ||
							wrStateInfo == EvolWrapperStatusInfo::BuildingAirPressure )
                    {
                        if (wrapOnlyMode)
						{
							if (Configuration::getInstance()->isScaleAvoir() && wgStatus.isDualRangeEnabled())
								packageWeight = (packageWeight + 5) / 10;		//	Round up to 2 decimal places
							wrapPackage(packageWeight);
						}
                        else
                        {
                            wgStatus.setEventType(WgStatusAndPrint);
                            dispatchEvent(&wgStatus, this);
                        }
                    }
                    else
                    {
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", "handleWeigherMessage(WgStatus):  We're doing nothing!");
                    }
                    //                          }
                }
                else
                {
                    if (wgStatus.isLargeMotion() == true)
                    {
                        Debug::trace("WrapperMode.WeigherStatus", "Large Motion");
                        largeMotion = true;

                        if ((machineRunning == false) && (pPendingPackage != NULL))
                        {
                            sendProductComplete(pPendingPackage, false);
                            pPendingPackage = NULL;
                        }
                    }

                    if (wgStatus.isZeroStableWeight() == true)
                    {
                        Debug::trace("WrapperMode.WeigherStatus", "Zero Stable Weight");

                        if (opModeStatus.isWrapperGhostPackage() == true)
                        {
                            Debug::trace("WrapperMode.WeigherStatus", "We had a GhostPackage");
                            clearGhostPackage();
                            requestWrapperStatus();
                        }
                        else
                        {
                            Debug::trace("WrapperMode.WeigherStatus", "No GhostPackage");
                            if (machineRunning == false)
                            {
                                if (pPendingPackage)
                                {
                                    sendProductComplete(pPendingPackage, false);
                                    pPendingPackage = NULL;
                                }

                                if (wrStateInfo == EvolWrapperStatusInfo::Standby)
                                {
                                    clearPressStart();
                                }
                            }
                        }
                    }
                }

                lastWeigherStatus = wgStatus;

                break;
            }
        case WgCachedStatus:
            {
                Debug::trace("WrapperMode.WeigherCachedStatus", "received a WgCachedStatus");

                WeigherCachedStatus wgCachedStatus(recvBuffer);

                if (Debug::isTracing("WrapperMode.WeigherCachedStatus") == true)
                    wgCachedStatus.show();

                if ( wgCachedStatus.isLargeMotion() )
                {
                    Debug::trace("WrapperMode.WeigherCachedStatus", "setting largeMotion to true");
                    largeMotion = true;
                }
                else
                    Debug::trace("WrapperMode.WeigherCachedStatus", "WgCachedStatus large motion wasn't set");

                break;
            }
        default:
            {
                break;
            }
    }                                           
}

void AutoWeighEvolWrapperBakeryMode::handleWrapperMessage(WrEvent wrEvent, const char *recvBuffer)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug = "";
    if (traceOn)
    {
        msgDebug = "handleWrapperMessage(";
        msgDebug.append(Integer::toString(wrEvent)).append(")");
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        msgDebug.append(":  ");
    }

    switch (wrEvent)
    {
        // This message appears to come from the wrapper device with current
        // settings for film and sealer belt at the device itself.
        case WrEvolInquire:
            handleWrapperInquire(recvBuffer);
            break;

            // This message comes from the wrapper device with current status
            // information.
        case WrEvolStatus:
            handleWrapperEvolStatus(recvBuffer);
            break;

            // This message comes from the wrapper device to indicate it is ready to
            // wrap a package
        case WrOpen:
            {
                Debug::trace("WrapperMode.StateMachine", "WrOpen--received");

                unlockWeigher();
                if ( wrapStateMachine == awmWrapWrap )
                    openWrapWindow();

                break;
            }

            // This message is sent by RTC to report Operation Mode changes in the UI
        case WrSetWrapMode:
            {
                WrapperSetWrapMode wrapState(recvBuffer);
                curWrapState = wrapState.getWrapMode();
                if ( Debug::isTracing("WrapperMode.WrapperSetWrapMode") )
                    wrapState.show();

                handleWrapSetMode(wrapState.getWrapMode());

                break;
            }

        case WrReqSettings:
            handleWrapperRequestSettings();
            break;

            // This message appears to pass wrapper settings from the Operator's screen
            // to the wrapper itself.
        case WrEvolControl:
            handleWrapperControl(recvBuffer);
            break;

        case WrTest:
            {
                Debug::trace("WrapperMode.StateMachine", "AutoWeighEvolWrapperBakeryMode, WrTest--received");
                /*if (wrStateInfo == EvolWrapperStatusInfo::Standby)
                  {
                  setPressStart();
                  }
                  else if (machineRunning == true)*/
                if (machineRunning == false)
                {
                    Debug::trace("WrapperMode.StateMachine", "AutoWeighEvolWrapperBakeryMode, calling sendWrapperTest()");
                    WrapperTest wrTest(recvBuffer);
                    sendWrapperTest(wrTest.getWrTestCode());
                }
                break;
            }

        case WrEvolConfig:
            handleWrapperConfig(recvBuffer);
            break;

        default:
            break;
    }                                           
}

void AutoWeighEvolWrapperBakeryMode::handleWrapperEvolStatus(const char *recvBuffer)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "handleWrapperEvolStatus()";
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        msgDebug.append(":  ");
    }
    EvolWrapperStatus statusMsg(recvBuffer);

    if (traceOn)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "**********   Start EvolWrapperStatus show()   **********");
        statusMsg.show();
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "**********    End  EvolWrapperStatus show()   **********");
    }

    if ((statusMsg.isPeripheralComm() == true) && (statusMsg.isManagerDeleted() == false))
    {
        wrStateInfo = statusMsg.getStatusInfo().getWrapperState();
        EvolWrapperStatusInfo::EvolWrapperState lastStateInfo = lastWrapperStatus.getStatusInfo().getWrapperState();
        if (traceOn)
        {
            HobString msgDebug2 = msgDebug;
            HobString tempState;

            EvolWrapperStatusInfo::getWrapperStateName(wrStateInfo, tempState);
            msgDebug2.append("wrStateInfo = ").append(tempState);
            msgDebug2.append(",   mainPower = ").append(Bool::toString(statusMsg.getStatusInfo().getMainPower()));
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);

            msgDebug2 = msgDebug;
            EvolWrapperStatusInfo::getWrapperStateName(lastStateInfo, tempState);
            msgDebug2.append("lastStateInfo = ").append(tempState);
            msgDebug2.append(",   mainPower = ").append(Bool::toString(lastWrapperStatus.getStatusInfo().getMainPower()));
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
        }

        if (((wrStateInfo == EvolWrapperStatusInfo::Homing) && statusMsg.getStatusInfo().getMainPower()) &&
                ((lastStateInfo != EvolWrapperStatusInfo::Homing) || !(lastWrapperStatus.getStatusInfo().getMainPower())))
        {
            //  Set EvolWrapperHoming only if the state is Homing AND we have MainPower.
            opModeStatus.setEvolWrapperHoming();
            dispatchEvent(&opModeStatus, this);
        }
        else if ((wrStateInfo != EvolWrapperStatusInfo::Homing) && (lastStateInfo == EvolWrapperStatusInfo::Homing))
        {
            opModeStatus.clearEvolWrapperHoming();
            dispatchEvent(&opModeStatus, this);
        }

        if (((wrStateInfo == EvolWrapperStatusInfo::BuildingAirPressure) && statusMsg.getStatusInfo().getMainPower()) &&
                ((lastStateInfo != EvolWrapperStatusInfo::BuildingAirPressure) || !(lastWrapperStatus.getStatusInfo().getMainPower())))
        {
            //  Set EvolWrapperHoming only if the state is BuildingAirPressure AND we have MainPower.
            opModeStatus.setEvolWrapperBuildingAirPressure();
            dispatchEvent(&opModeStatus, this);
        }
        else if ((wrStateInfo != EvolWrapperStatusInfo::BuildingAirPressure) && (lastStateInfo == EvolWrapperStatusInfo::BuildingAirPressure))
        {
            opModeStatus.clearEvolWrapperBuildingAirPressure();
            dispatchEvent(&opModeStatus, this);
        }

        //              wrapperSettings.setTemperatureInfo(statusMsg.getTemperatureInfo());

        machineRunning = (wrStateInfo == EvolWrapperStatusInfo::Running);
        /*
           if (pCurrentProduct && wrStatus.getPackageInfo((UINT16) IntakePosition).isGhostPackage())
           {
           setGhostPackage();
           sendPrinterReset(FirstPrinter);
           sendPrinterReset(SecondPrinter);
           }
           */

        if (((wrStateInfo == EvolWrapperStatusInfo::Standby) || (wrStateInfo == EvolWrapperStatusInfo::Homing) || (wrStateInfo == EvolWrapperStatusInfo::BuildingAirPressure)) && (opModeStatus.isBakeryCLAPackageMissing() || opModeStatus.isBakeryCLALabelMissing()))
            clearCLAPackageMissing();

        EvolWrapperErrorInfo currentErrorInfo = statusMsg.getCurrErrorInfo();
        EvolWrapperErrorInfo::EvolWrapperErrorSource errorSource = currentErrorInfo.getEvolWrapperErrorSource();
        EvolWrapperErrorInfo::EvolWrapperErrorSource currErrorSource = opModeStatus.getEvolWrapperErrorSource();
        EvolWrapperErrorInfo::EvolWrapperError error = currentErrorInfo.getEvolWrapperError(errorSource);
        EvolWrapperErrorInfo::EvolWrapperError currError = opModeStatus.getEvolWrapperError();

        if ((errorSource != currErrorSource) || (error != currError))
        {
            if (traceOn)
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Error is different.  error = " + inttostring(error));

            // leave the error message up until the stop switch is cycled
            if (errorSource != EvolWrapperErrorInfo::NoWrapperErrorSource)
            {
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "New errorSource.");
                setNewEvolWrapperError(error, errorSource);
            }
            else if (wrStateInfo == EvolWrapperStatusInfo::Standby)
            {
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Wrapper in Standby.");
                setNewEvolWrapperError(error, errorSource);

                if (wrapStateMachine == awmWrapError)
                {
                    if (traceOn)
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Error cleared!");

                    resetPackIdData();

                    if (pBakeryLabelApplierManager)
                    {
                        if (traceOn)
                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "pBakeryLabelApplierManager->wandUpOnBakeryCLA().");
                        pBakeryLabelApplierManager->wandUpOnBakeryCLA();
                    }

                    if (pCurrentProduct)
                    {
                        wrapStateMachine = awmWrapWrap;
                        Debug::trace("WrapperMode.StateMachine", "handleWrapperEvolStatus():   wrapStateMachine: awmWrapWrap");
                    }
                    else
                    {
                        wrapStateMachine = awmWrapInitial;
                        Debug::trace("WrapperMode.StateMachine", "handleWrapperEvolStatus():   wrapStateMachine: awmWrapInitial");
                    }
                }
            }
        }

        if (errorSource == EvolWrapperErrorInfo::NoWrapperErrorSource && wrapStateMachine != awmWrapError)
        {
            if (traceOn)
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "errorSource == NoWrapperErrorSource");
            if ((lastStateInfo == EvolWrapperStatusInfo::Standby)/* && (wrStateInfo != EvolWrapperStatusInfo::Standby)*/)
            {
                if (pCurrentProductTransaction)
                {
                    sendWrapperAbort();
                    // restart the system because this is bad!
                    invalidatePackage(pCurrentProductTransaction);
                    //pCurrentProductTransaction = NULL;
                    if (traceOn)
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Standby:  sendWrapperAbort() & invalidatePackage()");
                }
            }

            if (machineRunning)
            {
                if (pPendingPackage)
                {
                    // make the pending one the current one!
                    if (traceOn)
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "pPendingPackage is now the current one.");
                    pCurrentProductTransaction = pPendingPackage;
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", "Set pCurrentProductTransaction to pPendingPackage = " + HobString(Hex::toString((long long) pCurrentProductTransaction)));
                    pPendingPackage = NULL;

        			wrPackageWeight = (HobWeight) pCurrentProductTransaction->getWeigherStatus()->getWeight();
	 
            		if (Configuration::getInstance()->isScaleAvoir() && pCurrentProductTransaction->getWeigherStatus()->isDualRangeEnabled())
	    	        	wrPackageWeight = (wrPackageWeight + 5) / 10;		//	Round up to 2 decimal places

    			    wrapPackage(wrPackageWeight);
                }
            }
        }
        else
        {
            if (wrapStateMachine == awmWrapError)
            {
                if (traceOn)
                {
                    HobString msgDebug2 = msgDebug;
                    msgDebug2.append("Wrapper Error already detected!   isNewEvolWrapperError = ").append(Bool::toString(opModeStatus.isNewEvolWrapperError()));
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
                }
            }
            else
            {
                HobString msgDebug2;
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Wrapper Error Detected!");

                wrapStateMachine = awmWrapError;
                Debug::trace("WrapperMode.StateMachine", "wrapStateMachine: awmWrapError");

                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Wrapper Error - beeping 3 times");
                ServiceSpeaker::badBeep(3);

                if (traceOn)
                {
                    msgDebug2 = msgDebug + "Wrapper Error: ";
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "machineRunning = " + Bool::toString(machineRunning));
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "     mainPower = " + HobString(Bool::toString(lastWrapperStatus.getStatusInfo().getMainPower())));
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "   errorSource = " + EvolWrapperErrorInfo::getEvolWrapperErrorSourceText(errorSource));
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "         error = " + EvolWrapperErrorInfo::getEvolWrapperErrorText(error));
                    if (pPendingPackage)
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", 
                                msgDebug2 + 
                                "pPendingPackage->getTransactionNum() = " + 
                                inttostring( static_cast <int> (pPendingPackage->getTransactionNum())));
                    if (pCurrentProductTransaction)
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", 
                                msgDebug2 + 
                                "pCurrentProductTransaction->getTransactionNum() = " + 
                                inttostring( static_cast <int> (pCurrentProductTransaction->getTransactionNum())));
                }
                if (machineRunning && lastWrapperStatus.getStatusInfo().getMainPower() && (errorSource == EvolWrapperErrorInfo::PrimaryConveyorSource))
                {
                    if (traceOn)
                    {
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "I think we can remove the last transaction:  packId = " + inttostring(packId));
                        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "pCurrentProductTransaction = 0x" + Hex::toString((long long) pCurrentProductTransaction) + ",  pPendingPackage = 0x" + Hex::toString((long long) pPendingPackage));
                    }
                    //  Since the error occurred on the Intake while the machine was running,
                    //  we can remove the currentProductTransaction and any pendingPackage.
                    //  This will allow us to apply labels to the packages processed prior to the error.
                    if (pCurrentProductTransaction)
                    {
                        HobRecordNum transNum = pCurrentProductTransaction->getTransactionNum();
                        if (traceOn)
                            Debug::trace("AutoWeighEvolWrapperBakeryMode", 
                                    msgDebug + 
                                    "Wrapper Error: sendProductComplete on pCurrentProductTransaction # " + 
                                    inttostring( static_cast <int> (transNum)));
                        sendProductComplete(const_cast <ProductTransaction *> (pCurrentProductTransaction), false);

                        //      Remove it from the package map
                        PackageMap::iterator pos1 = packageMap.find(transNum);
                        if (pos1 != packageMap.end())
                        {
                            UINT8 tmpPackId = 0;
                            OpModePackage *pPackage = pos1->second;
                            if (pPackage)
                                tmpPackId = pPackage->getPackId();

                            packageMap.erase(pos1);
                            if (traceOn)
                                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Wrapper Error: Removing transaction from packageMap.");

                            //  Now remove it from the bakery packages
                            if (tmpPackId)
                            {
                                BakeryPackageMap::iterator pos = bakeryPackages.find(tmpPackId);
                                if (pos != bakeryPackages.end())
                                    bakeryPackages.erase(pos);
                                if (traceOn)
                                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Wrapper Error: Removing transaction from bakeryPackages.");
                            }
                        }
                        pCurrentProductTransaction = 0;
                    }
                    if (pPendingPackage)
                    {
                        if (traceOn)
                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Wrapper Error: sendProductComplete on pPendingPackage");
                        sendProductComplete(const_cast <ProductTransaction *> (pPendingPackage), false);
                        pPendingPackage = 0;
                    }
                }
            }
        }
    }
    if (Debug::isTracing("Clare"))
    {
        HobString msgDebug2 = "AutoWeighEvolWrapperBakeryMode::handleWrapperEvolStatus()";
        msgDebug2.append("\n           errorSource = ").append(Integer::toString(opModeStatus.getEvolWrapperErrorSource()));
        msgDebug2.append("\n                 error = ").append(Integer::toString(opModeStatus.getEvolWrapperError()));
        msgDebug2.append("\n                packId = ").append(Integer::toString(packId));
        msgDebug2.append("\n        machineRunning = ").append(Bool::toString(machineRunning));
        msgDebug2.append("\n      wrapStateMachine = ").append(Integer::toString(wrapStateMachine));
        msgDebug2.append("\n isNewEvolWrapperError = ").append(Bool::toString(opModeStatus.isNewEvolWrapperError()));
        Debug::trace("Clare", msgDebug2);
    }

    lastWrapperStatus = statusMsg;
}

void AutoWeighEvolWrapperBakeryMode::wrapPackage(HobWeight packageWeight)
{
    bool traceOn = Debug::isTracing("WrapperMode.Wrap");
    HobString msgDebug;

    /* removed for port -- ats
       if (traceOn)
       {
       msgDebug = "wrapPackage():    ";
       HobString curWrapStateStr;
       WrapperSetWrapMode::getWrapModeText(curWrapState, curWrapStateStr);

       if (pCurrentProductTransaction)
       Debug::trace("WrapperMode.Wrap", msgDebug + "transactionNum: 0x" + HobString(Hex::toString(pCurrentProductTransaction->getTransactionNum())));
       else
       Debug::trace("WrapperMode.Wrap", msgDebug + "No transactionNum. pCurrentProductTransaction is 0.");
       Debug::trace("WrapperMode.Wrap", msgDebug + "  curWrapState:" + curWrapStateStr);
       Debug::trace("WrapperMode.Wrap", msgDebug + "   wrStateInfo:" + HobString(Integer::toString((int)wrStateInfo)));
       }
       */

    if ((curWrapState == WrapperSetWrapMode::ManualWeigh) || (curWrapState == WrapperSetWrapMode::AutoManualApply))
    {
        if (traceOn)
            Debug::trace("WrapperMode.Wrap", msgDebug + "Won't wrap because the weighMode is ManualWeigh, or AutoManualApply");
    }
    else if (packageWeightIsTooHeavyForAWS(packageWeight))
    {
        if (traceOn)
        {
            HobString msgDebug2 = msgDebug;
            msgDebug2.append("Won't wrap due to package too heavy. (");
            msgDebug2.append(inttostring(static_cast<int>(packageWeight))).append(" >= ");
            msgDebug2.append(inttostring(static_cast<int>(getMaxAWSPackageWeight()))).append(")");
            Debug::trace("WrapperMode.Wrap", msgDebug);
        }
        setPackageOverWeight();
    }
    else
    {
        if (wrStateInfo == EvolWrapperStatusInfo::Standby)
            setPressStart();

        if (wrStateInfo == EvolWrapperStatusInfo::Standby || wrStateInfo == EvolWrapperStatusInfo::Homing ||
                wrStateInfo == EvolWrapperStatusInfo::Idle || wrStateInfo == EvolWrapperStatusInfo::BuildingAirPressure || laStateMachine1 == wmLaError || laStateMachine2 == wmLaError || laStateMachine3 == wmLaError)
        {
            if (traceOn)
            {
                Debug::trace("WrapperMode.Wrap", "wrapPackage():    Won't wrap because the wrapper is not ready. Exiting!");
                HobString msgDebug2 = msgDebug;
                msgDebug2.append("wrStateInfo = ").append(Integer::toString(wrStateInfo));
                msgDebug2.append("  laStateMachine1 = ").append(Integer::toString(laStateMachine1));
                msgDebug2.append("  laStateMachine2 = ").append(Integer::toString(laStateMachine2));
                msgDebug2.append("  laStateMachine3 = ").append(Integer::toString(laStateMachine3));
                Debug::trace("WrapperMode.Wrap", msgDebug2);
            }
            if (laStateMachine1 != wmLaError && laStateMachine2 != wmLaError && laStateMachine3 != wmLaError)
            {
                pPendingPackage = pCurrentProductTransaction;
                if (traceOn)
                    Debug::trace("WrapperMode.Wrap", msgDebug + "pPendingPackage = pCurrentProductTransaction");
            }
            pCurrentProductTransaction = NULL;
            Debug::trace("AutoWeighEvolWrapperBakeryMode", "wrapPackage():    Set pCurrentProductTransaction = " + HobString(Hex::toString((long long) pCurrentProductTransaction)));
            return;
        }
        else
        {
            if (opModeStatus.isWrapperPressStart())
                clearPressStart();

            if (resetPackIdData_)
            {
                //      If this test is true, there was some sort of error. Since we reset the pack id data when the wrapper stop button
                //      is pressed, if any labels are taken after that has occurred, the pack id data will change based on the label taken.
                //      If labels are taken before the wrapper stop button is pressed, the "Packages may not be labeled..." error won't happen.
                //      To eliminate this problem of order, we will keep resetPackIdData_ set to true until we are ready to run the wrapper again.
                //      By clearing the pack id data before we start with a new package, we can prevent the "Packages may not be labeled..."
                //      error message from always popping up.
                resetPackIdData();
                resetPackIdData_ = false;
                if (traceOn)
                    Debug::trace("WrapperMode.Wrap", msgDebug + "resetPackIdData_ = false");
            }
            // This must be in an else condition, unlike the code for the UWS in AutoWeighWrapperMode, 
            // because the evolution wrapper won't queue up the actual wrap command like the UWS appears to do.

            // This function sends the WrWrap message to the wrapper. It also sets
            // the state machine to awmWrapWrap and "locks" the weigher.
            if (traceOn)
                Debug::trace("WrapperMode.Wrap", msgDebug + "Calling sendWrapPackage()");
            sendWrapPackage(wrapperSettings.getIntakeMode().getIntakeMode(), packId, wrapProduct, (UINT16)packageWeight);
        }
    }

    if (wrapOnlyMode == true)
    {
        incPackId();
    }
    else
    {
        if (forcePreImage)
        {
            pCurrentProductTransaction->setForcePreImage(forcePreImage);
            forcePreImage = false;
        }
        // This function puts a new package in the package map, incrementing the
        // current packId, and requests all post-image needed.
        generateNewPackage3(pCurrentProductTransaction);        
    }

    largeMotion = false;
}

void AutoWeighEvolWrapperBakeryMode::firstPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
    _Mybase::firstPrinterAttached(pPrinterMgr);

    if ((getNewProduct() == true) || (pCurrentProduct != NULL))
    {
        changeProduct();
        preImagePrinters();     
    }

    openWrapWindow();
}

void AutoWeighEvolWrapperBakeryMode::firstPrinterDetached()
{
    _Mybase::firstPrinterDetached();

    resetWrapperMode();
}

void AutoWeighEvolWrapperBakeryMode::secondPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
    _Mybase::secondPrinterAttached(pPrinterMgr);

    if ((getNewProduct() == true) || (pCurrentProduct != NULL))
    {
        changeProduct();
        preImagePrinters();     
    }

    openWrapWindow();
}

void AutoWeighEvolWrapperBakeryMode::secondPrinterDetached()
{
    _Mybase::secondPrinterDetached();

    resetWrapperMode();
}

void AutoWeighEvolWrapperBakeryMode::thirdPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
    _Mybase::thirdPrinterAttached(pPrinterMgr);

    if ((getNewProduct() == true) || (pCurrentProduct != NULL))
    {
        changeProduct();
        preImagePrinters();     
    }

    openWrapWindow();
}

void AutoWeighEvolWrapperBakeryMode::thirdPrinterDetached()
{
    _Mybase::thirdPrinterDetached();

    resetWrapperMode();
}

void AutoWeighEvolWrapperBakeryMode::labelApplierAttached(Hobart::CEvolMsgReceiver *pLabelApplierManager)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "labelApplierAttached()";
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        msgDebug.append(":  ");
    }

    _Mybase::labelApplierAttached( pLabelApplierManager );

    if (pCurrentProduct)
    {
        //      The AWS went down for some reason and is now back up. We need to resend a LaSetup message for the current Product.

        //      The fact that we now have a LabelApplierManager does not mean it has registered for all its messages
        //      and is ready to receive messages at this point. Since we can't tell when it is ready,
        //      we will set this flag and send it after we send a WrControl message to the Wrapper.
        if (traceOn)
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "updateApplierSetup = true");
        updateApplierSetup = true;
    }
}

void AutoWeighEvolWrapperBakeryMode::wrapperAttached(Hobart::CEvolMsgReceiver *pWrapperManager, WrapperSetWrapMode::WrapMode wrapMode)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "wrapperAttached()";
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        msgDebug.append(":  ");
    }

    _Mybase::wrapperAttached( pWrapperManager );

    if (curWrapState != wrapMode)
    {
        if (traceOn)
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Changing WrapMode");
        handleWrapSetMode(wrapMode);
    }

    if (pWeigherManager != NULL)
    {
        if (traceOn)
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "We have pWeigherManager");
        setWrapperMode(WrOperateMode);
        requestWrapperStatus();
        requestWrapperInquire();
        openWrapWindow();
    }
}

void AutoWeighEvolWrapperBakeryMode::wrapperDetached()
{
    Debug::trace("AutoWeighEvolWrapperBakeryMode", "wrapperDetached()");
    _Mybase::wrapperDetached();

    resetWrapperMode();
    setNewEvolWrapperError(EvolWrapperErrorInfo::NoWrapperError, EvolWrapperErrorInfo::NoWrapperErrorSource);
}

void AutoWeighEvolWrapperBakeryMode::weigherAttached(Hobart::CEvolMsgReceiver *pWeigherManager)
{
    _Mybase::weigherAttached( pWeigherManager );

    setWeigherMode(ScalePrepackMode);

    if (pWrapperManager != NULL)
    {
        setWrapperMode(WrOperateMode);
    }
}

void AutoWeighEvolWrapperBakeryMode::weigherDetached()
{
    _Mybase::weigherDetached();

    resetWrapperMode();
}

void AutoWeighEvolWrapperBakeryMode::changeProduct()
{
    Debug::trace("AutoWeighEvolWrapperBakeryMode", "changeProduct()");

    OperatingMode::changeProduct();

    activePackId = packId;
    claCurrentPackId = 0;

    if (pCurrentProduct && pCurrentProduct->getIntakeMode() != WrapperIntakeMode::DefaultIntake)
    {
        setIntakeMode(pCurrentProduct->getIntakeMode());
    }
}

void AutoWeighEvolWrapperBakeryMode::openWrapWindow()
{
    if (wrapStateMachine != awmWrapError)
    {
        unlockWeigher();
        wrapStateMachine = awmWrapOpenWindow;
        Debug::trace("WrapperMode.StateMachine", "wrapStateMachine: awmWrapOpenWindow");
    }
}

void AutoWeighEvolWrapperBakeryMode::sendWrapPackage(WrapperIntakeMode::IntakeMode intakeMode, UINT8 packageId, bool pullFilm, UINT16 packageWeight)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug = "", msgDebug2 = "";
    if (traceOn)
    {
        msgDebug = "sendWrapPackage():  ";
        msgDebug2 = msgDebug;
        msgDebug2.append("intakeMode = ").append(Integer::toString(intakeMode));
        msgDebug2.append("   pullFilm = ").append(Bool::toString(pullFilm));
        msgDebug2.append("   updateApplierSetup = ").append(Bool::toString(updateApplierSetup));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);

        msgDebug2 = msgDebug;
        msgDebug2.append("packageId = ").append(Integer::toString(packageId));
        msgDebug2.append("   pCurrentProductTransaction # = ").append((pCurrentProductTransaction ? Long::toString(pCurrentProductTransaction->getTransactionNum()) : "0"));
        msgDebug2.append("   packageWeight = ").append(Integer::toString(packageWeight));
        msgDebug2.append("   pPendingPackage # = ").append((pPendingPackage ? Long::toString(pPendingPackage->getTransactionNum()) : "0"));
    }
    UINT32 pluNumber = (pCurrentProduct ? pCurrentProduct->getProductNum() : 0);

    if (traceOn)
    {
        msgDebug2.append("   pluNumber =").append(Long::toString(pluNumber));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
    }

    EvolWrapperWrap wrWrap(this, intakeMode, packageId, pullFilm, packageWeight, pluNumber);
    dispatchEvent(&wrWrap, this);

    wrapStateMachine = awmWrapWrap;
    Debug::trace("WrapperMode.StateMachine", "wrapStateMachine: awmWrapWrap");
    lockWeigher();
}

void AutoWeighEvolWrapperBakeryMode::handleWrapperInquire(const char *recvBuffer)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug = "";
    if (traceOn)
    {
        msgDebug = "handleWrapperInquire()";
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        msgDebug.append(":  ");
    }
    if (!recvBuffer)
    {
        if (traceOn)
        {
            HobString msgDebug2 = "recvBuffer is 0. Exiting!!!";
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + msgDebug2);
        }
        return;
    }

    EvolWrapperTemperatureInfo tempInfo;
    EvolWrapperInquire wrInquire(recvBuffer);

    EvolWrapperHeaterConfig heaterConfig = wrInquire.getHeaterConfig();

    HeaterConfiguration filmHeaterConfig = heaterConfig.getFilmHeaterConfig();
    tempInfo.setFilmSetting(filmHeaterConfig.getDefaultTempSetting());

    HeaterConfiguration sealerHeaterConfig = heaterConfig.getSealerHeaterConfig();
    tempInfo.setSealerBeltSetting(sealerHeaterConfig.getDefaultTempSetting());

    wrapperSettings.setTemperatureInfo(tempInfo);
#if 0 //min
    EvolWrapperGeneralConfig wrapperConfig = wrInquire.getWrapperConfig();

    wrapperSettings.setUpperRollWidth(wrapperConfig.getTopFilmRollDimensions().getRollWidth());
    wrapperSettings.setLowerRollWidth(wrapperConfig.getBottomFilmRollDimensions().getRollWidth());
#endif //emin
    settingsInitialized = true;
    if (traceOn)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "UpperRollWidth = " + inttostring(wrapperSettings.getUpperRollWidth()));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "LowerRollWidth = " + inttostring(wrapperSettings.getLowerRollWidth()));
    }

    if (pLabelApplierManager && pBakeryLabelApplierManager)
    {
        EvolLabelApplierConfiguration lac = wrInquire.getLabelApplierConfig();
        if (traceOn)
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Wrapper LA isSecondCLAAttached = " + booltostring(lac.isSecondCLAAttached()));
        HobString dischargeStr;
        if (lac.getSideDischargeType() == LeftDischarge)
            dischargeStr = "LeftDischarge";
        else
            dischargeStr = "RightDischarge";
        if (traceOn)
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Wrapper LA SideDischargeType = " + dischargeStr);

        if (lac.getSideDischargeType() == RightDischarge)
        {
            //  AWS LabelApplier
            pLabelApplierManager->setSecondPrinterStationOnly();
            if (traceOn)
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "pLabelApplierManager->setSecondPrinterStationOnly()");

            //  CLA
            pBakeryLabelApplierManager->setPrimaryPrinterStationOnly();
            if (traceOn)
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "pBakeryLabelApplierManager->setPrimaryPrinterStationOnly(),  bakeryStation = PrimaryPrinterStation");
        }
        else
        {
            //  AWS LabelApplier
            pLabelApplierManager->setPrimaryPrinterStationOnly();
            if (traceOn)
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "pLabelApplierManager->setPrimaryPrinterStationOnly()");

            //  CLA
            pBakeryLabelApplierManager->setSecondPrinterStationOnly();
            if (traceOn)
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "pBakeryLabelApplierManager->setSecondPrinterStationOnly(),   bakeryStation = SecondaryPrinterStation");
        }
    }
    // send control message if anything is changed!

    if (traceOn)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "   --  wrapperSettings.show() from wrInquire  ********");
        wrapperSettings.show();
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "   --  ***********************************************");
    }
    if (reqSettingsPending)
    {
        if (traceOn)
        {
            msgDebug.append("reqSettingsPending - dispatchEvent WrapperSettings.");
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        }
        dispatchEvent(&wrapperSettings, this);
        reqSettingsPending = false;
    }
    if (updateApplierSetup)
    {
        //      If the AWS went down, we can't send this message until the LabelApplierManager is up & running.
        //      By now, it should be.
        if (traceOn)
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Calling sendLabelApplierSetup().");
        sendLabelApplierSetup();
        updateApplierSetup = false;
    }
}


void AutoWeighEvolWrapperBakeryMode::handleWrapperControl(const char *recvBuffer)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug = "";
    if (traceOn)
    {
        msgDebug = "handleWrapperControl()";
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        msgDebug.append(":  ");
    }
    if (!recvBuffer)
    {
        if (traceOn)
        {
            HobString msgDebug2 = "recvBuffer is 0. Exiting!!!";
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + msgDebug2);
        }
        return;
    }

    EvolWrapperControl wrControl(recvBuffer);

    if (traceOn)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Showing received EvolWrapperControl object -----------------");
        wrControl.show();
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Done showing received EvolWrapperControl object ------------");
    }

    //  Do some wrapper settings upkeep
    setIntakeMode(wrControl.getIntakeMode());

    wrapperSettings.setFilmWidth(wrControl.getFilmWidthSelection());
    wrapperSettings.setFilmTension(wrControl.getFilmTension());
    wrapperSettings.setFilmTensionFrontToBack(wrControl.getFilmTensionFrontToBack());
    wrapperSettings.setFilmTensionRightToLeft(wrControl.getFilmTensionRightToLeft());

    EvolWrapperTemperatureInfo tempInfo;
    tempInfo.setSealerBeltHeaterEnabled(wrControl.isSealerBeltHeaterEnabled());
    tempInfo.setSealerBeltSetting(wrControl.getSealerBeltTempSetting());
    tempInfo.setFilmTempEnabled(wrControl.isFilmSupplyHeaterEnabled());
    tempInfo.setFilmSetting(wrControl.getFilmSupplyTempSetting());

    wrapperSettings.setTemperatureInfo(tempInfo);
    if (traceOn)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Updated wrapperSettings.show()  ********");
        wrapperSettings.show();
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "****************************************");
    }

    wrControl.setSourceObject(this);

    if (pWrapperManager != NULL)
    {
        const char *pCC = wrControl.serialize();
        pWrapperManager->send(pCC, wrControl.getSerializedLength());
    }
}


void AutoWeighEvolWrapperBakeryMode::setIntakeMode(WrapperIntakeMode::IntakeMode intakeMode)
{
    if (intakeMode == WrapperIntakeMode::DefaultIntake)
    {
        intakeMode = (WrapperIntakeMode::IntakeMode) Configuration::getInstance()->getDefaultIntakeMode();
    }
    wrapperSettings.setIntakeMode(intakeMode);
}

void AutoWeighEvolWrapperBakeryMode::handleWrapSetMode(WrapperSetWrapMode::WrapMode wrapState)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug = "";

    if (traceOn)
    {
        msgDebug = "handleWrapSetMode():  ";

        HobString msgDebug2 = msgDebug;
        msgDebug2.append("New wrapState is ");
        switch (wrapState)
        {
            case WrapperSetWrapMode::WrapOn:
                msgDebug2.append("WrapOn");
                break;
            case WrapperSetWrapMode::WrapOff:
                msgDebug2.append("WrapOff");
                break;
            case WrapperSetWrapMode::WrapOnly:
                msgDebug2.append("WrapOnly");
                break;
            case WrapperSetWrapMode::AutoManualApply:
                msgDebug2.append("AutoManualApply");
                break;
            case WrapperSetWrapMode::UnknownMode:
                msgDebug2.append("UnknownMode");
                break;
            case WrapperSetWrapMode::ManualWeigh:
                msgDebug2.append("ManualWeigh");
                break;
            default:
                msgDebug2.append("???");
                break;
        }               
        msgDebug2.append(" [").append(Integer::toString(wrapState));
        msgDebug2.append("],  curWrapState was ").append(Integer::toString(curWrapState));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
    }

    curWrapState = wrapState;
    wrapProduct = (wrapState == WrapperSetWrapMode::WrapOnly) || (wrapState == WrapperSetWrapMode::WrapOn);
    if (traceOn)
    {
        HobString msgDebug2 = msgDebug;
        msgDebug2.append("wrapProduct = ").append(Bool::toString(wrapProduct));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
    }

    switch (wrapState)
    {
        case WrapperSetWrapMode::WrapOn:
        case WrapperSetWrapMode::WrapOff:
            setWeigherMode(ScalePrepackMode);
            setWrapOnlyMode(false);
            break;
        case WrapperSetWrapMode::WrapOnly:
            setWeigherMode(ScalePrepackMode);
            setWrapOnlyMode(true);
            break;
        case WrapperSetWrapMode::UnknownMode:
        case WrapperSetWrapMode::ManualWeigh:
        default:
            setWeigherMode(ScaleManualMode);
            setWrapOnlyMode(false);
            break;
    }
}

void AutoWeighEvolWrapperBakeryMode::handleWrapperRequestSettings()
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug = "";
    if (traceOn)
    {
        msgDebug = "handleWrapperRequestSettings()";
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        msgDebug.append(":   ");
    }

    if (settingsInitialized)
    {
        if (traceOn)
        {
            msgDebug.append("dispatchEvent WrapperSettings.");
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        }
        dispatchEvent(&wrapperSettings, this);
        reqSettingsPending = false;
    }
    else
    {
        if (traceOn)
        {
            msgDebug.append("WrapperSettings not initialized! Can't send anything yet!!!");
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        }
        reqSettingsPending = true;
    }
}

void AutoWeighEvolWrapperBakeryMode::handleLabelApplierMessage(LaEvent applierEvent, const char *recvBuffer)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "handleLabelApplierMessage()";
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        msgDebug.append(":  ");
    }
    if (!recvBuffer)
    {
        if (traceOn)
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "recvBuffer is 0. Exiting!!!");
        return;
    }

    switch (applierEvent)
    {
        case LaEvolInquire:
            {
                EvolLabelApplierConfiguration lac(recvBuffer);
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "LaEvolInquire:  Wrapper LA isSecondCLAAttached = " + booltostring(lac.isSecondCLAAttached()));
                HobString dischargeStr;
                if (lac.getSideDischargeType() == LeftDischarge)
                    dischargeStr = "LeftDischarge";
                else
                    dischargeStr = "RightDischarge";
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "LaEvolInquire:  Wrapper LA SideDischargeType = " + dischargeStr);
                break;
            }
        case LaPrintPackage:
            {
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "LaPrintPackage");
                LabelApplierPrint lap(recvBuffer);

                gotPrintPackageMsg = true;
                claPrintPackage(lap.getPackageId());
                break;
            }
        default:
            break;
    }
    OperatingMode::handleLabelApplierMessage(applierEvent, recvBuffer);
}

void AutoWeighEvolWrapperBakeryMode::generateNewPackage3(const ProductTransaction *UNUSED(pPT))
{
    UNREFERENCED_PARAMETER(pPT);

    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug("generateNewPackage3:   ");
    if (traceOn)
    {
        HobString msgDebug2 = msgDebug;
        msgDebug2.append("pCurrentProductTransaction = ").append(Hex::toString((long long) pCurrentProductTransaction)).append("\n");
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "packId = " + inttostring(packId));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "printerScenario = " + inttostring(printerScenario));
    }

    if (!pCurrentProductTransaction)
    {
        if (traceOn)
        {
            HobString msgDebug2 = msgDebug;
            msgDebug2.append("pCurrentProductTransaction is 0. Exiting!!!");
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
        }
        return;
    }
    const ProductTransaction *pTrans = 0;
    Product *tempProduct = pCurrentProductTransaction->getProduct();
    OpModePackage *pPackage = new OpModePackage(pCurrentProductTransaction, packId);
    packageMap.insert(make_pair(pCurrentProductTransaction->getTransactionNum(), pPackage));

    HobString pScenario = "printerScenario = ";

    if (printerScenario == PrimaryPrinterOnly)
    {
        pScenario.append("PrimaryPrinterOnly");
        pPackage->setFirstPrinter(pFirstPrinter);

        postImagePrinter(FirstPrinter, pFirstPrinter, packId);
    }
    else if (printerScenario == PrimaryPrinterBakeryCLA)
    {
        pScenario.append("PrimaryPrinterBakeryCLA");
        pPackage->setFirstPrinter(pFirstPrinter);

        if (tempProduct)
        {
            LabelType *tempLabel = tempProduct->getLabelType(FirstPrinter);
            if (tempLabel && (tempLabel->getLabelTypeNum() > 0))
            {
                pTrans = pCurrentProductTransaction;
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "postImage PrimaryPrinterBakeryCLA for packId " + inttostring(packId));
                postImagePrinter(FirstPrinter, pFirstPrinter, packId);
            }
            else
            {
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "No PrimaryPrinterBakeryCLA label to print for packId " + inttostring(packId));
                pTrans = 0;
            }

            //  If there is no label type, setup the package with a null ProductTransaction.
            if (traceOn)
            {
                HobString msgDebug2 = msgDebug;
                msgDebug2.append("bakeryPackages.insert(make_pair(").append(inttostring(packId)).append(", 0x").append(Hex::toString((long long) pTrans)).append(")) - PrimaryPrinterBakeryCLA");
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
            }
            bakeryPackages.insert(make_pair(packId, pTrans));
        }
    }
    else if (printerScenario == SecondaryPrinterOnly)
    {
        pScenario.append("SecondPrinterOnly");
        pPackage->setSecondPrinter(pSecondPrinter);

        postImagePrinter(SecondPrinter, pSecondPrinter, packId);
    }
    else if (printerScenario == SecondPrinterBakeryCLA)
    {
        pScenario.append("SecondPrinterBakeryCLA");
        pPackage->setSecondPrinter(pSecondPrinter);

        if (tempProduct)
        {
            LabelType *tempLabel = tempProduct->getLabelType(SecondPrinter);
            if (tempLabel && (tempLabel->getLabelTypeNum() > 0))
            {
                pTrans = pCurrentProductTransaction;
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "postImage SecondPrinterBakeryCLA for packId " + inttostring(packId));
                postImagePrinter(SecondPrinter, pSecondPrinter, packId);
            }
            else
            {
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "No SecondPrinterBakeryCLA label to print for packId " + inttostring(packId));
                pTrans = 0;
            }

            //  If there is no label type, setup the package with a null ProductTransaction.
            if (traceOn)
            {
                HobString msgDebug2 = msgDebug;
                msgDebug2.append("bakeryPackages.insert(make_pair(").append(inttostring(packId)).append(", 0x").append(Hex::toString((long long) pTrans)).append(")) - SecondPrinterBakeryCLA");
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
            }
            bakeryPackages.insert(make_pair(packId, pTrans));
        }
    }
    else if (printerScenario == ThirdPrinterOnly)
    {
        pScenario.append("ThirdPrinterOnly");
        pPackage->setThirdPrinter(pThirdPrinter);

        postImagePrinter(ThirdPrinter, pThirdPrinter, packId);
    }
    else if (printerScenario == PrimaryPrinterAndSecondaryPrinter)
    {
        pScenario.append("PrimaryAndSecondPrinter");
        pPackage->setFirstPrinter(pFirstPrinter);
        pPackage->setSecondPrinter(pSecondPrinter);

        postImagePrinter(FirstPrinter, pFirstPrinter, packId);
        postImagePrinter(SecondPrinter, pSecondPrinter, packId);
    }
    else if (printerScenario == PrimaryPrinterBakeryCLAAndSecondPrinter)
    {
        pScenario.append("PrimaryPrinterBakeryCLAAndSecondPrinter");
        pPackage->setFirstPrinter(pFirstPrinter);
        pPackage->setSecondPrinter(pSecondPrinter);

        if (tempProduct)
        {
            LabelType *tempLabel = tempProduct->getLabelType(FirstPrinter);
            if (tempLabel && (tempLabel->getLabelTypeNum() > 0))
            {
                pTrans = pCurrentProductTransaction;
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "postImage PrimaryPrinterBakeryCLAAndSecondPrinter for packId " + inttostring(packId));
                postImagePrinter(FirstPrinter, pFirstPrinter, packId);
            }
            else
            {
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "No PrimaryPrinterBakeryCLAAndSecondPrinter label to print for packId " + inttostring(packId));
                pTrans = 0;
            }

            //  If there is no label type, setup the package with a null ProductTransaction.
            if (traceOn)
            {
                HobString msgDebug2 = msgDebug;
                msgDebug2.append("bakeryPackages.insert(make_pair(").append(inttostring(packId)).append(", 0x").append(Hex::toString((long long) pTrans)).append(")) - PrimaryPrinterBakeryCLAAndSecondPrinter");
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
            }
            bakeryPackages.insert(make_pair(packId, pTrans));
        }
        postImagePrinter(SecondPrinter, pSecondPrinter, packId);
    }
    else if (printerScenario == PrimaryPrinterAndSecondPrinterBakeryCLA)
    {
        pScenario.append("PrimaryPrinterAndSecondPrinterBakeryCLA");
        pPackage->setFirstPrinter(pFirstPrinter);
        pPackage->setSecondPrinter(pSecondPrinter);

        postImagePrinter(FirstPrinter, pFirstPrinter, packId);
        if (tempProduct)
        {
            LabelType *tempLabel = tempProduct->getLabelType(SecondPrinter);
            if (tempLabel && (tempLabel->getLabelTypeNum() > 0))
            {
                pTrans = pCurrentProductTransaction;
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "postImage PrimaryPrinterAndSecondPrinterBakeryCLA for packId " + inttostring(packId));
                postImagePrinter(SecondPrinter, pSecondPrinter, packId);
            }
            else
            {
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "No PrimaryPrinterAndSecondPrinterBakeryCLA label to print for packId " + inttostring(packId));
                pTrans = 0;
            }

            //  If there is no label type, setup the package with a null ProductTransaction.
            if (traceOn)
            {
                HobString msgDebug2 = msgDebug;
                msgDebug2.append("bakeryPackages.insert(make_pair(").append(inttostring(packId)).append(", 0x").append(Hex::toString((long long) pTrans)).append(")) - PrimaryPrinterAndSecondPrinterBakeryCLA");
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
            }
            bakeryPackages.insert(make_pair(packId, pTrans));
        }
    }
    else if (printerScenario == PrimarySecondAndThirdPrinter)
    {
        pScenario.append("PrimarySecondAndThirdPrinter");
        pPackage->setFirstPrinter(pFirstPrinter);
        pPackage->setSecondPrinter(pSecondPrinter);
        pPackage->setThirdPrinter(pThirdPrinter);

        postImagePrinter(FirstPrinter, pFirstPrinter, packId);
        postImagePrinter(SecondPrinter, pSecondPrinter, packId);
        postImagePrinter(ThirdPrinter, pThirdPrinter, packId);
    }
    else
    {
        pScenario.append("Invalid printerScenario: ").append(Integer::toString(printerScenario));
    }
    bool traceOnPackId = Debug::isTracing("WrapperMode.PackId");
    if (traceOnPackId)
    {
        Debug::trace("WrapperMode.PackId", msgDebug + "bakeryPackages.insert()  --------------------------------------------");
        Debug::trace("WrapperMode.PackId", msgDebug + "pCurrentProductTransaction = " + Hex::toString((long long) pCurrentProductTransaction));
        Debug::trace("WrapperMode.PackId", msgDebug + "            transactionNum = " + Integer::toString(pCurrentProductTransaction->getTransactionNum()));
        Debug::trace("WrapperMode.PackId", msgDebug + "                    packId = " + Integer::toString(packId));
        Debug::trace("WrapperMode.PackId", msgDebug + "     bakeryPackages.size() = " + Integer::toString((int)bakeryPackages.size()));
        Debug::trace("WrapperMode.PackId", msgDebug + "------------------------------------------------------------------------");
    }
    if (traceOn)
    {
        HobString msgDebug2 = msgDebug;
        msgDebug2.append(pScenario);
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2);
    }
    if (traceOnPackId)
    {
        Debug::trace("WrapperMode.PackId", msgDebug + "packageMap.insert()  ==================================================");
        Debug::trace("WrapperMode.PackId", msgDebug + "pCurrentProductTransaction = " + Hex::toString((long long) pCurrentProductTransaction));
        Debug::trace("WrapperMode.PackId", msgDebug + "            transactionNum = " + Integer::toString(pCurrentProductTransaction->getTransactionNum()));
        Debug::trace("WrapperMode.PackId", msgDebug + "                    packId = " + Integer::toString(packId));
        Debug::trace("WrapperMode.PackId", msgDebug + "         packageMap.size() = " + Integer::toString((int)packageMap.size()));
        Debug::trace("WrapperMode.PackId", msgDebug + "=======================================================================");
    }
    incPackId();
}


void AutoWeighEvolWrapperBakeryMode::claPrintPackage(UINT8 pkgId)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "claPrintPackage(";
        msgDebug.append(inttostring(pkgId));
        msgDebug.append("):  ");
        //              Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "wrapOnlyMode = " + booltostring(wrapOnlyMode));
    }

    bakeryPackId = pkgId;
    if (traceOn)
    {
        HobString theSize = bakeryLabelReady.empty() ? "0" : inttostring(bakeryLabelReady.size());
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "  bakeryLabelReady.size() = " + theSize);
        theSize = bakeryPackages.empty() ? "0" : inttostring(bakeryPackages.size());
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "    bakeryPackages.size() = " + theSize);
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "1a:  waitingForLabelReady = " + booltostring(waitingForLabelReady));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "claCurrentPackId = " + inttostring(claCurrentPackId));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "  CLAReadyPackId = " + inttostring(CLAReadyPackId));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "                wrapOnlyMode = " + booltostring(wrapOnlyMode));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "  (ps == PrimaryPrinterOnly) = " + booltostring(printerScenario == PrimaryPrinterOnly));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "(ps == SecondPrinterOnly) = " + booltostring(printerScenario == SecondaryPrinterOnly));
    }
    if (wrapOnlyMode || (printerScenario == PrimaryPrinterOnly) || (printerScenario == SecondaryPrinterOnly) || (printerScenario == ThirdPrinterOnly))
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "No label to apply on the CLA.");
        incActivePackId();
    }
    else
    {
        if (claCurrentPackId != 0)
        {
            if (traceOn)
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Error!  (claCurrentPackId != 0)");
            applyError = true;
        }
        else if (bakeryLabelReady.empty())
        {
            if (bakeryPackages.empty())
            {
                EvolWrapperErrorInfo::EvolWrapperError wrError = opModeStatus.getEvolWrapperError();
                if (wrError == EvolWrapperErrorInfo::NoWrapperError)
                {
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Error! bakeryPackages is empty also! pBakeryLabelApplierManager->resetBakeryCLA().");
                    if (pBakeryLabelApplierManager)
                        pBakeryLabelApplierManager->resetBakeryCLA();
                }
                else
                {
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "XXX  There was a wrapper error! Let's see what happens if we let this go.");
                }
            }
            else
            {
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "No PrLabelReady yet!  waitingForLabelReady = true");
                waitingForLabelReady = true;
            }
        }
        else
        {
            //  Pull the first package from bakeryLabelReady. It should be the one that was ready.
            UINT8 pkgId = bakeryLabelReady.front();
            if (traceOn)
            {
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "           pkgId = " + inttostring(pkgId));
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "    activePackId = " + inttostring(activePackId));
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "claCurrentPackId = " + inttostring(claCurrentPackId));
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "  AWSTakenPackId = " + inttostring(AWSTakenPackId));

                HobString prString;
                if (printerScenario == PrimaryPrinterBakeryCLAAndSecondPrinter)
                    prString = "PrimaryPrinterBakeryCLAAndSecondPrinter";
                else if (printerScenario == PrimaryPrinterAndSecondPrinterBakeryCLA)
                    prString = "PrimaryPrinterAndSecondPrinterBakeryCLA";
                else if (printerScenario == PrimaryPrinterBakeryCLA)
                    prString = "PrimaryPrinterBakeryCLA";
                else if (printerScenario == SecondPrinterBakeryCLA)
                    prString = "SecondPrinterBakeryCLA";
                if (prString.empty())
                    prString = inttostring(printerScenario);
                Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + " printerScenario = " + prString + ".");
            }

            if (pkgId != activePackId)
            {
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Error!  (pkgId != activePackId)");
                applyError = true;
            }
            /* ATTENTION: this change I made is intended for right hand side discharge.
               It may not work for left hand side discharge. */
            else if (AWSTakenPackId != pkgId && AWSTakenPackId > 0 &&
                    (printerScenario == PrimaryPrinterBakeryCLAAndSecondPrinter || printerScenario == PrimaryPrinterAndSecondPrinterBakeryCLA) )
            {
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Error!  More than 1 label has been taken before this message.");
                applyError = true;
            }
            else
            {
                claCurrentPackId = activePackId;
                if (traceOn)
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Now claCurrentPackId = " + inttostring(claCurrentPackId));

                bool foundIt = false;
                BakeryPackageMap::iterator pos = bakeryPackages.find(pkgId);
                if (pos != bakeryPackages.end())
                    foundIt = true;

                if (foundIt)
                {
                    //  We have a match.
                    if (pBakeryLabelApplierManager)
                    {
                        if (pBakeryLabelApplierManager->canRespondToPrimaryStation())
                        {
                            sendLabelApplierCloseWindow(bakeryPackId);

                            LabelApplierLabelReady labelReady(this, PrimaryPrinterStation, bakeryPackId);
                            dispatchEvent(&labelReady, this);
                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Sending LabelReady to PrimaryPrinterStation.  bakeryPackId = " + inttostring(bakeryPackId));
                        }
                        else if (pBakeryLabelApplierManager->canRespondToSecondStation())
                        {
                            sendLabelApplierCloseWindow(bakeryPackId);

                            LabelApplierLabelReady labelReady(this, SecondaryPrinterStation, bakeryPackId);
                            labelReady.setCommSwitchToPrimary(true);
                            dispatchEvent(&labelReady, this);
                            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Sending LabelReady to SecondaryPrinterStation.  bakeryPackId = " + inttostring(bakeryPackId));
                        }
                    }
                }
                else
                {
                    Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Could not find a match for " + inttostring(pkgId) + " in the bakeryLabelReady map.");
                }
            }
        }
        if (traceOn)
        {
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "1b:  waitingForLabelReady = " + booltostring(waitingForLabelReady));
        }
    }
}


void AutoWeighEvolWrapperBakeryMode::setWrapOnlyMode(bool value)
{
    if (wrapOnlyMode != value)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", "setWrapOnlyMode(" + booltostring(value) + ")");
        wrapOnlyMode = value;

        //      Update the CLA so it knows if it is going to apply a label or not.
        sendLabelApplierSetup();
    }
}


void AutoWeighEvolWrapperBakeryMode::sendLabelApplierSetup()
{
    if (pCurrentProduct)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", "sendLabelApplierSetup():  const Product newProduct(const_cast<Product *>(pCurrentProduct)).");
        const Product newProduct(const_cast<Product *>(pCurrentProduct));
        OperatingMode::sendLabelApplierSetup(newProduct);
    }
    else
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", "sendLabelApplierSetup():  const Product newProduct.");
        const Product newProduct;
        OperatingMode::sendLabelApplierSetup(newProduct);
    }
}


void AutoWeighEvolWrapperBakeryMode::emptyPackageMap()
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug, msgDebug2, msgDebug3;
    if (traceOn)
    {
        msgDebug = "emptyPackageMap():   ";
        HobString msgDebug2 = msgDebug;
        msgDebug2.append("Start:    ");
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "bakeryLabelReady.size() = " + inttostring(bakeryLabelReady.size()));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "      packageMap.size() = " + inttostring(packageMap.size()));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "  bakeryPackages.size() = " + inttostring(bakeryPackages.size()));
    }

    OperatingMode::emptyPackageMap();           //      Handle clearing out all of the packageMap.
    if (traceOn)
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Clearing the bakeryPackages map.");
    bakeryPackages.clear();


    if (bakeryLabelReady.size())
    {
        if (traceOn)
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Clearing the bakeryLabelReady map.");
        bakeryLabelReady.clear();
    }

    if (traceOn)
    {
        HobString msgDebug2 = msgDebug;
        msgDebug2.append("End:    ");
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "bakeryLabelReady.size() = " + inttostring(bakeryLabelReady.size()));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "      packageMap.size() = " + inttostring(packageMap.size()));
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug2 + "  bakeryPackages.size() = " + inttostring(bakeryPackages.size()));
    }
}


void AutoWeighEvolWrapperBakeryMode::clearCLAPackageMissing()
{
    Debug::trace("AutoWeighEvolWrapperBakeryMode", "clearCLAPackageMissing()");
    resetPackIdData();

    applyError = false;
    applyErrorHandled = false;

    if (pBakeryLabelApplierManager)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", "clearCLAPackageMissing():  pBakeryLabelApplierManager->resetBakeryCLA().");
        pBakeryLabelApplierManager->resetBakeryCLA();
    }
    {

        labelIsMissing = false;
        applyErrorHandled = false;
        opModeStatus.clearBakeryCLALabelMissing();
    }
    opModeStatus.clearBakeryCLAPackageMissing();
    dispatchEvent(&opModeStatus, this);

    openWrapWindow();
}

void AutoWeighEvolWrapperBakeryMode::setCLAPackageMissing()
{
    Debug::trace("AutoWeighEvolWrapperBakeryMode", "setCLAPackageMissing()");
    opModeStatus.setBakeryCLAPackageMissing();
    dispatchEvent(&opModeStatus, this);

    applyErrorHandled = true;

    ServiceSpeaker::badBeep(2);
    resetWrapperMode();

    emptyPackageMap();

    sendPrinterReset(FirstPrinter);
    sendPrinterReset(SecondPrinter);
}

void AutoWeighEvolWrapperBakeryMode::incActivePackId()
{
    UINT8 packId = activePackId + 1;
    activePackId = packId ? packId : 1; 
    Debug::trace("AutoWeighEvolWrapperBakeryMode.test", "incActivePackId():  activePackId = " + inttostring(activePackId));
}

void AutoWeighEvolWrapperBakeryMode::setAWSTakenPackId(UINT8 id)
{
    Debug::trace("AutoWeighEvolWrapperBakeryMode.test", "setAWSTakenPackId(" + inttostring(id) + ")");
    AWSTakenPackId = id;
}

void AutoWeighEvolWrapperBakeryMode::setCLATakenPackId(UINT8 id)
{
    Debug::trace("AutoWeighEvolWrapperBakeryMode.test", "setCLATakenPackId(" + inttostring(id) + ")");
    CLATakenPackId = id;
    /* ATTENTION: this change I made is intended for right hand side discharge.
       It may not work for left hand side discharge. */
    if ((CLATakenPackId > AWSTakenPackId) && (AWSTakenPackId > 0) &&
            (printerScenario == PrimaryPrinterBakeryCLAAndSecondPrinter || printerScenario == PrimaryPrinterAndSecondPrinterBakeryCLA) )
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode.test", "setCLATakenPackId(" + inttostring(id) + "):  Error! AWSTakenPackId = " + inttostring(AWSTakenPackId));
        applyError = true;
    }
}

void AutoWeighEvolWrapperBakeryMode::setAWSReadyPackId(UINT8 id)
{
    Debug::trace("AutoWeighEvolWrapperBakeryMode.test", "setAWSReadyPackId(" + inttostring(id) + ")");
    AWSReadyPackId = id;
}

void AutoWeighEvolWrapperBakeryMode::setCLAReadyPackId(UINT8 id)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode.test");
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "setCLAReadyPackId(";
        msgDebug.append(inttostring(id));
        msgDebug.append("):  ");

        HobString prString;
        if (printerScenario == PrimaryPrinterBakeryCLAAndSecondPrinter)
            prString = "PrimaryPrinterBakeryCLAAndSecondPrinter";
        else if (printerScenario == PrimaryPrinterAndSecondPrinterBakeryCLA)
            prString = "PrimaryPrinterAndSecondPrinterBakeryCLA";
        else if (printerScenario == PrimaryPrinterBakeryCLA)
            prString = "PrimaryPrinterBakeryCLA";
        else if (printerScenario == SecondPrinterBakeryCLA)
            prString = "SecondPrinterBakeryCLA";
        Debug::trace("AutoWeighEvolWrapperBakeryMode.test", "setCLAReadyPackId(" + inttostring(id) + "):  printerScenario = " + prString);
    }

    CLAReadyPackId = id;
    /* ATTENTION: Do we need to set applyError to true being careful with whether
       printerScenario is PrimaryPrinterBakeryCLAAndSecondPrinter or PrimaryPrinterAndSecondPrinterBakeryCLA? */
    if ((CLAReadyPackId > AWSReadyPackId) && (AWSReadyPackId > 0))
        Debug::trace("AutoWeighEvolWrapperBakeryMode.test", msgDebug + "Error! AWSReadyPackId = " + inttostring(AWSReadyPackId));
}


bool AutoWeighEvolWrapperBakeryMode::updateProductComplete(PrinterLabelComplete &prLabelComplete)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode.test");
    HobString msgDebug;
    if (traceOn)
    {
        msgDebug = "updateProductComplete()";
        Debug::trace("AutoWeighEvolWrapperBakeryMode.test", msgDebug);
        msgDebug.append(":  ");
    }
    bool done = OperatingMode::updateProductComplete(prLabelComplete);
    if (done)
    {
        incActivePackId();

        if (traceOn)
        {
            Debug::trace("AutoWeighEvolWrapperBakeryMode.test", msgDebug + "  activePackId = " + inttostring(activePackId));
            //                  Debug::trace("AutoWeighEvolWrapperBakeryMode.test", msgDebug + "AWSReadyPackId = " + inttostring(AWSReadyPackId));
            Debug::trace("AutoWeighEvolWrapperBakeryMode.test", msgDebug + "AWSTakenPackId = " + inttostring(AWSTakenPackId));
            //                  Debug::trace("AutoWeighEvolWrapperBakeryMode.test", msgDebug + "CLAReadyPackId = " + inttostring(CLAReadyPackId));
            Debug::trace("AutoWeighEvolWrapperBakeryMode.test", msgDebug + "CLATakenPackId = " + inttostring(CLATakenPackId));
        }
    }
    return done;
}

void AutoWeighEvolWrapperBakeryMode::resetPackIdData()
{
    Debug::trace("AutoWeighEvolWrapperBakeryMode", "resetPackIdData()");
    AWSReadyPackId = 0;
    AWSTakenPackId = 0;
    CLAReadyPackId = 0;
    CLATakenPackId = 0;
    activePackId = packId;
    claCurrentPackId = 0;
    waitingForLabelReady = false;

    resetPackIdData_ = true;
}


void AutoWeighEvolWrapperBakeryMode::handleWrapperConfig(const char *recvBuffer)
{
    bool traceOn = Debug::isTracing("AutoWeighEvolWrapperBakeryMode");
    HobString msgDebug = "";
    if (traceOn)
    {
        msgDebug = "handleWrapperConfig()";
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug);
        msgDebug.append(":  ");
    }
    if (!recvBuffer)
    {
        if (traceOn)
        {
            HobString msgDebug2 = "recvBuffer is 0. Exiting!!!";
            Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + msgDebug2);
        }
        return;
    }

    EvolWrapperConfiguration wrConfig(recvBuffer);

    if (traceOn)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Showing received EvolWrapperConfiguration object -----------------");
        wrConfig.show();
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Done showing received EvolWrapperConfiguration object ------------");
    }

    //  Update the wrapperSettings with the possibly changed Upper and Lower film widths
    EvolWrapperGeneralConfig wrGenConfig = wrConfig.getWrapperConfig();
    wrapperSettings.setUpperRollWidth(wrGenConfig.getTopFilmRollDimensions().getRollWidth());
    wrapperSettings.setLowerRollWidth(wrGenConfig.getBottomFilmRollDimensions().getRollWidth());

    if (traceOn)
    {
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Showing current wrapperSettings -----------------");
        wrapperSettings.show();
        Debug::trace("AutoWeighEvolWrapperBakeryMode", msgDebug + "Done showing current wrapperSettings ------------");
    }
}
