#if 0
#define USING_WRAPPER_CUBE
#endif

#include "AutoWeighEvolWrapperMode.h"

#include "PrinterManager.h"
#include "PrinterStatus.h"
#include "PrinterLabelReady.h"
#include "PrinterLabelTaken.h"
#include "PrinterLabelComplete.h"

#include "WrapperManager.h"
#include "EvolWrapperControl.h"
#include "EvolWrapperInquire.h"
#include "EvolWrapperConfiguration.h"
#include "EvolWrapperWrap.h"
#include "WrapperErrorInfo.h"
#include "EvolWrapperPackageDischarge.h"

#include "WeigherCachedStatus.h"
#include "HfcWeigherStatus.h"

#include "ProductUpdateMsg.h"
#include "ProductChangeMsg.h"
#include "ProductStatusMsg.h"

#include "ProductTransaction.h"
#include "ServiceSpeaker.h"
#include "OpModePackage.h"

#include "Configuration.h"

#include "FpmManager.h"

#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AutoWeighEvolWrapperMode::dataType = 0;
std::string AutoWeighEvolWrapperMode::dataTypeName("AutoWeighEvolWrapperMode"); 
#endif

AutoWeighEvolWrapperMode::AutoWeighEvolWrapperMode(WrapperSetWrapMode::WrapMode wrapMode,
										   WrapperIntakeMode::IntakeMode intakeMode,
										   const Util::CSmartPtr<Product> &productSmartPtr,
										   //WeigherManager *pWeigherManager,
										   //PrinterManager *pFirstPrinter,
										   //PrinterManager *pSecondPrinter,
										   //PrinterManager *pThirdPrinter,
										   //LabelApplierManager *pLabelApplierManager,
										   //WrapperManager *pWrapperManager) :	OperatingMode("tEvWrapAW",
										   Hobart::CEvolMsgReceiver *pWeigherManager,
										   Hobart::CEvolMsgReceiver *pFirstPrinter,
										   Hobart::CEvolMsgReceiver *pSecondPrinter,
										   Hobart::CEvolMsgReceiver *pThirdPrinter,
										   Hobart::CEvolMsgReceiver *pLabelApplierManager,
										   Hobart::CEvolMsgReceiver *pWrapperManager) :	OperatingMode("tEvWrapAW",
										   												   productSmartPtr,
																						   pWeigherManager,
										   												   pFirstPrinter,
										   												   pSecondPrinter,
										   												   pThirdPrinter,
																						   pLabelApplierManager,
										   												   pWrapperManager),
										   									AUTO_MaxLabels(6),
																			MANUAL_MaxLabels(1),
																			TakeLabelTimeout(12)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("AWEWM", "Constructor:  this = " + ptrtostring(this));

	iAmActive = false;
	missedProductUpdates = 0;
	wrapOnlyMode = false;
	machineRunning = true;
    printerHasErrors_ = false;

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

	if (Debug::isTracing("AutoWeighEvolWrapperMode"))
	{
		HobString msgDebug = "Constructor: wrapMode = ";
		msgDebug.append(Integer::toString(wrapMode));
		msgDebug.append("  intakeMode = ").append(Integer::toString(intakeMode));
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
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

	pkgDischargeError = MaxPrinterTypes;

	if (Debug::isTracing("AutoWeighEvolWrapperMode"))
	{
		HobString msgDebug = "activeFunctionTask: myInitialWrapMode = ";
		msgDebug.append(Integer::toString(myInitialWrapMode));
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
	}
	handleWrapSetMode(myInitialWrapMode);

	setTakeLabelTime(FirstPrinter, TakeLabelTimeout);
	setTakeLabelTime(SecondPrinter, TakeLabelTimeout);
	setTakeLabelTime(ThirdPrinter, TakeLabelTimeout);

#if 1
	if (productSmartPtr.get())
	{
		Util::CSmartPtr<Product> newProductSmartPtr(productSmartPtr);
		Debug::trace("AWEWM", "Have a ProductSmartPtr");
		handleProductSmartPtr(newProductSmartPtr);
	}
#else
	if (productSmartPtr.get())
	{
		pCurrentProductSmartPtr = productSmartPtr;
		pCurrentProduct = pCurrentProductSmartPtr.get();
		Debug::trace("AutoWeighEvolWrapperMode", "constructor: get pCurrentProduct from pCurrentProductSmartPtr");

		OperatingMode::changeProduct();
		preImagePrinters();
	}
#endif
	if (Debug::isTracing("AutoWeighEvolWrapperMode"))
	{
		HobString msgDebug = "constructor: pCurrentProduct = ";
		msgDebug.append(util_string::ptrtostring(pCurrentProduct));
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
	}

	setWrapperMode(WrOperateMode);

	openWrapWindow();

	registerEvent(ProductDev, ProductChange);
    registerEvent(ProductDev, ProductChangeSmart);
	registerEvent(ProductDev, ProductUpdate);
	registerEvent(ProductDev, ProductStatusEvent);

	registerEvent(Printer, PrStatus);
	registerEvent(Printer, PrLabelReady);
	registerEvent(Printer, PrLabelTaken);
	registerEvent(Printer, PrLabelComplete);
	registerEvent(Printer, PrStockMetrics);
	registerEvent(Printer, PrNomSize);
	registerEvent(Printer, PrConfig);

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
	registerEvent(Wrapper, WrEvolPackageDischarge);

	iAmActive = true;		//	All registered, now we are active.

	requestWeigherStatus();
	requestWrapperStatus();
	requestWrapperInquire();

	requestLabelSize();
	Debug::trace("AWEWM", "Constructor:  this = " + ptrtostring(this) + ",  Done.");
}


AutoWeighEvolWrapperMode::~AutoWeighEvolWrapperMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	iAmActive = false;

	Debug::trace("AWEWM", "Destructor:  this = " + ptrtostring(this));

	deregisterAllEvents();
	
	emptyPackageMap();

//	sendPrinterReset(FirstPrinter);
//	sendPrinterReset(SecondPrinter);
//	sendPrinterReset(ThirdPrinter);

//	WrapperIntakeMode intakeModeMsg(this, wrapperSettings.getIntakeMode().getIntakeMode());
//	dispatchEvent(&intakeModeMsg);
	Debug::trace("AutoWeighEvolWrapperMode", HobString(tName) + HobString(" -- Going away!!!"));
	Debug::trace("AWEWM", "Destructor:  this = " + ptrtostring(this) + ",  Done.");
}				

void AutoWeighEvolWrapperMode::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{	
	if (!iAmActive)
	{
		Debug::trace("AWEWM", "processEvent:  I am NOT active [" + ptrtostring(this) + "]");
		return;
	}
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
		default:
		{
			break;
		}
	}	
}

#include "ProductManager.h"
void AutoWeighEvolWrapperMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
	HobString msgDebug("handleProductMessage() - ");
	bool debugOpModeProd = Debug::isTracing("OperatingMode.Product");
	bool debugWMPU = Debug::isTracing("WrapperMode.ProductUpdate");
	HobWeight wrPackageWeight;

	switch (productEvent)
	{
		// Operator has chosen a new product
		//case ProductChange:
		case ProductChangeSmart:
		{
			Debug::trace("AWEWM", HobString("ProductChangeSmart"));
			Debug::trace("WrapperMode.ProductChange", HobString("ProductChangeSmart Entry") );
            if( wrapStateMachine == awmWrapError )
            {
				Debug::trace("AWEWM", HobString("ProductChangeSmart:  wrapStateMachine == awmWrapError, Exiting!"));
                break;
            }
			updateApplierSetup = false;			//	A new product nullifies this condition.

			ProductChangeMsg prodChange(recvBuffer);
			Debug::trace("WrapperMode.ProductChange", HobString("packageMap.size(): ") + HobString(Integer::toString((int)packageMap.size())));
//			Debug::trace("WrapperMode.ProductChange", HobString("packageMap.empty(): ") + HobString(Bool::toString(packageMap.empty())));

//			Product *pProduct = 0;
			ProductManager *productManager = (ProductManager *) prodChange.getProductMgr();
			if (productManager)
			{
				pCurrentProductSmartPtr = productManager->getCurrentProductSmartPtr();
				handleProductSmartPtr(pCurrentProductSmartPtr);
//				pProduct = pCurrentProductSmartPtr.get();
//				Debug::trace("AutoWeighEvolWrapperMode", HobString("ProductChangeSmart:  pProduct = ") + util_string::ptrtostring(pProduct));
			}

//			Debug::trace("WrapperMode.ProductChange", HobString("ProductChange: 0x") + util_string::ptrtostring(pProduct));
//			if (pProduct == NULL)
//			{
//				pCurrentProduct = 0;		//	AWL-113
//				return;
//			}

//			// If changing the plu on the fly more than once before the next package is taken in, remove the previous
//			// plu from the product queue (but only if there has been no stable weight for a package placed on the intake).
//			// This assumes they can only have one other plu in the queue.
//			if (wrapStateMachine == awmWrapChangingProduct)
//			{
//				emptyProductQ();
//				if (pChangingProductPendingPackage)
//				{
//					Debug::trace("WrapperMode.ProductChange", "pChangingProductPendingPackage not null. Cancelling!");
//					sendProductComplete(pChangingProductPendingPackage, false);
//					pChangingProductPendingPackage = NULL;
//				}
//			}

//			if (pPendingPackage != NULL)
//			{
//				Debug::trace("WrapperMode.ProductChange", "pPendingPackage not null; cancelling");
//				sendProductComplete(pPendingPackage, false);
//				pPendingPackage = NULL;
//			}

//			forcePreImage = true;
//			productQ.push(pCurrentProductSmartPtr);
//			wrapStateMachine = awmWrapChangingProduct;
//			Debug::trace("WrapperMode.StateMachine", "wrapStateMachine: awmWrapChangingProduct");

//            if (packageMap.empty() && getNewProduct())
//			{
//				Debug::trace("WrapperMode.ProductChange", "changing product with empty pkg map");
//				changeProduct();
//				preImagePrinters();
//				openWrapWindow();
//			}
//          if( getNewProduct() && pCurrentProductSmartPtr.get())
//			{
//				Debug::trace("WrapperMode.ProductChange", "Sending LabelApplierSetup on PLU change");
//                sendLabelApplierSetup(pCurrentProductSmartPtr.get());
//			}
            
            Debug::trace("WrapperMode.Product", HobString("ProductChangeSmart Exit") );
			break;
		}

		case ProductUpdate:
		{
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + HobString("ProductUpdate"));

			ProductUpdateMsg prodUpdate(recvBuffer);
			const ProductTransaction *pNextProductTransaction = const_cast<ProductTransaction *>(prodUpdate.getProductTransaction());

			if (pNextProductTransaction == NULL)
			{
				Debug::trace("AutoWeighEvolWrapperMode", HobString("ProductUpdate: pNextProductTransaction == NULL") );
				break;
			}

			if (debugOpModeProd)
			{
#ifdef AWL_94
//	AWL-94: The following line can potentially be a problem for the Keyboard net weight PLU because there may not be a weigherStatus.
				HobWeight packageWeight = pNextProductTransaction->getWeigherStatus()->getWeight();
#else
//	Testing		HobWeight tmpWeight = 0;
//				const HfcWeigherStatus *wgStatus = pNextProductTransaction->getWeigherStatus();
//				Debug::trace("OperatingMode.Product", HobString("  **  pNextProductTransaction  wgStatus = ") + HobString(Hex::toString((UINT32) wgStatus)));
//				if (wgStatus)
//				{
//					tmpWeight = wgStatus->getWeight();
//					Debug::trace("OperatingMode.Product", HobString("  **  pNextProductTransaction  tmpWeight = ") + HobString(Integer::toString(tmpWeight)));
//				}
				HobWeight packageWeight = pNextProductTransaction->getTotalNetWeight();
#endif

				Debug::trace("OperatingMode.Product", HobString("          packageWeight: ") + HobString(Integer::toString(packageWeight)));
				Debug::trace("OperatingMode.Product", HobString("         machineRunning: ") + HobString(Bool::toString(machineRunning)));
				Debug::trace("OperatingMode.Product", HobString("            largeMotion: ") + HobString(Bool::toString(largeMotion)));
				Debug::trace("OperatingMode.Product", HobString("       wrapStateMachine: ") + HobString(Integer::toString(wrapStateMachine)));
				Debug::trace("OperatingMode.Product", HobString("      printerHasErrors_: ") + HobString(Bool::toString(printerHasErrors_)));
				//Debug::trace("OperatingMode.Product", HobString("           curWrapState: ") + curWrapStateStr);
				Debug::trace("OperatingMode.Product", HobString("           wrapOnlyMode: ") + HobString(Bool::toString(wrapOnlyMode)));
				Debug::trace("OperatingMode.Product", HobString("            wrStateInfo: ") + HobString(Integer::toString(wrStateInfo)));
				Debug::trace("OperatingMode.Product", HobString("      packageMap.size(): ") + HobString(Integer::toString((int)packageMap.size())));
				Debug::trace("OperatingMode.Product", HobString("     packageMap.empty(): ") + HobString(Bool::toString(packageMap.empty())));
				Debug::trace("OperatingMode.Product", HobString("        pPendingPackage: ") + util_string::ptrtostring(pPendingPackage));
				Debug::trace("OperatingMode.Product", HobString("   getIgnoreWeighMode(): ") + HobString(Bool::toString(prodUpdate.getIgnoreWeighMode())));
			}

			if ( prodUpdate.getIgnoreWeighMode() == true) 			//	HA-160:  Print the label, regardless of the weigh mode.
			{
				pCurrentProductTransaction = pNextProductTransaction;
			    wrapPackage(0, prodUpdate.getIgnoreWeighMode());	
			}
			else if ( curWrapState == WrapperSetWrapMode::ManualWeigh )
			{
				// Don't care about the state of the wrapper in these two cases, since the package is not entering
				// the wrapper for these states.  We only want a label printed.
				if (debugWMPU)
					Debug::trace("WrapperMode.ProductUpdate", "Condition 1: (curWrapState == ManualWeigh) || (prodUpdate.getIgnoreWeighMode() == true)");

				pCurrentProductTransaction = pNextProductTransaction;
				Debug::trace("AutoWeighEvolWrapperMode", "ManualWeigh. Set pCurrentProductTransaction = " + util_string::ptrtostring(pCurrentProductTransaction));

				if ( !pCurrentProductTransaction )
				{
					if (debugOpModeProd)
						Debug::trace("OperatingMode.Product", HobString("ProductUpdate: pCurrentProductTransaction == NULL ... breaking early"));
					break;
				}

				const HfcWeigherStatus *wgStatus( pCurrentProductTransaction->getWeigherStatus() );

				wrPackageWeight = pCurrentProductTransaction->getTotalNetWeight();

				if (debugOpModeProd)
					Debug::trace("OperatingMode.Product", HobString("  **  pCurrentProductTransaction  wrPackageWeight: ") + HobString(Integer::toString(wrPackageWeight)));

				if ( !wgStatus )
				{
					if (debugOpModeProd)
						Debug::trace("OperatingMode.Product", HobString("ProductUpdate: wgStatus == NULL"));
				}
				else
				{
					//Debug::trace("OperatingMode.Product", HobString("ProductUpdate: wgStatus != NULL"));

					if ( ( MetSig::isScaleAvoir() ) && ( wgStatus->isDualRangeEnabled() ) )
					{
						if (debugOpModeProd)
							Debug::trace("OperatingMode.Product", HobString("ProductUpdate: isScaleAvoir && isDualRanged"));
						wrPackageWeight = Rounding::roundWeightToTwoImpliedDecimalPlaces(wrPackageWeight);
						if (debugOpModeProd)
							Debug::trace("OperatingMode.Product", HobString("wrPackageWeight: ")+HobString(Long::toString(wrPackageWeight)));
					}
				}

			    wrapPackage(wrPackageWeight);
			}
			else if (curWrapState == WrapperSetWrapMode::AutoManualApply)
			{
				if (debugWMPU)
					Debug::trace("WrapperMode.ProductUpdate", "Condition 2: (curWrapState == AutoManualApply)");
				bool isProdRandomWt = true;
				Product *pTheProduct = (pNextProductTransaction ? pNextProductTransaction->getProduct() : 0);
				if (pTheProduct)
					isProdRandomWt = pTheProduct->isRandomWeight();
				if (debugOpModeProd)
				{
					Debug::trace("OperatingMode.Product", "AutoManualApply: packageMap.size() = " +	inttostring(packageMap.size()) +
															"   MANUAL_MaxLabels = " +	inttostring(MANUAL_MaxLabels) +
															"   largeMotion: " + booltostring(largeMotion) +
															"   pTheProduct = " + util_string::ptrtostring(pTheProduct) +
															"   isProdRandomWt() = " +	booltostring(isProdRandomWt));
				}

				if (packageMap.size() < MANUAL_MaxLabels && (largeMotion || (!largeMotion && !isProdRandomWt)))
				{
					if (debugOpModeProd)
						Debug::trace("OperatingMode.Product", "AutoManualApply. Conditions are good. Calling wrapPackage()");

					pCurrentProductTransaction = pNextProductTransaction;
					Debug::trace("AutoWeighEvolWrapperMode", "AutoManualApply. Set pCurrentProductTransaction = " + util_string::ptrtostring(pCurrentProductTransaction) + ".  Call wrapThatPackage(pCurrentProductTransaction)");
					wrapThatPackage(pCurrentProductTransaction);
				}
				else
				{
					if (debugOpModeProd)
						Debug::trace("OperatingMode.Product", "AutoManualApply. packageMap.size() and/or largeMotion DO NOT meet criteria to print. NOT CALLING wrapPackage().");

					if (pPendingPackage)
					{
						if (debugOpModeProd)
							Debug::trace("OperatingMode.Product", HobString("sendProductComplete() for Product trans#: ") + HobString(Hex::toString(pPendingPackage->getTransactionNum())));
						sendProductComplete(const_cast <ProductTransaction *> (pPendingPackage), false);
						pPendingPackage = 0;
					}
					pPendingPackage = pNextProductTransaction;
					if (pPendingPackage && debugOpModeProd)
						Debug::trace("OperatingMode.Product", HobString("pPendingPackage is now Product trans#: ") + HobString(Hex::toString(pPendingPackage->getTransactionNum())));
				}
		 	}
			// WR_STANDBY -- NO WR_PAUSED STATE
			else if ((opModeStatus.isWrapperGhostPackage()) || (opModeStatus.isWrapperPressStart()) || (wrapOnlyMode) /* || (wrStateInfo == EvolWrapperStatusInfo::Standby) */)
			{
				/* NGW-239 */
				Debug::trace("WrapperMode.ProductUpdate", "Condition 3: Wrap the package!");
				if( pCurrentProductTransaction != NULL ) {
					wrPackageWeight = pCurrentProductTransaction->getTotalNetWeight();
					wrapPackage(wrPackageWeight);
				} else {
					Debug::trace("WrapperMode.ProductUpdate", "pCurrentProductTransaction == NULL");
					pCurrentProductTransaction = pNextProductTransaction;
					wrPackageWeight = pCurrentProductTransaction->getTotalNetWeight();
					wrapPackage(wrPackageWeight);
				}
			}
			else if (wrapStateMachine == awmWrapError)
			{
				if (debugWMPU)
				{
					Debug::trace("WrapperMode.ProductUpdate", "Condition 4: (wrapStateMachine == awmWrapError)");
					Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 2:  (wrapStateMachine == awmWrapError)");
				}

				sendProductComplete(pNextProductTransaction, false);
				if (pPendingPackage)
				{
					sendProductComplete(pPendingPackage, false);
					pPendingPackage = NULL;
				}
			}
			else
			{
				if (debugWMPU)
					Debug::trace("WrapperMode.ProductUpdate", "Condition 5");
				if (wrapStateMachine == awmWrapChangingProduct &&
						(machineRunning || wrStateInfo == EvolWrapperStatusInfo::Standby || wrStateInfo == EvolWrapperStatusInfo::Disabled) &&
						largeMotion)
				{
					if (debugWMPU)
						Debug::trace("WrapperMode.ProductUpdate", "Not wrapping yet:  wrapStateMachine == awmWrapChangingProduct");

					if (pPendingPackage)
					{
						sendProductComplete(pPendingPackage, false);
						pPendingPackage = NULL;
					}

					if (pChangingProductPendingPackage)
						sendProductComplete(pChangingProductPendingPackage, false);
					pChangingProductPendingPackage = pNextProductTransaction;
					if (debugWMPU)
						Debug::trace("WrapperMode.ProductUpdate", "Setting pChangingProductPendingPackage = " + util_string::ptrtostring(pChangingProductPendingPackage));
				}
				else if (packageMap.size() >= AUTO_MaxLabels)
				{
					if (debugWMPU)
						Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 4: too many packages: " + inttostring(packageMap.size()));

					handlePrinterOverflow(AUTO_MaxLabels);
					sendProductComplete(pNextProductTransaction, false);
				}
				else if (wrapStateMachine == awmWrapOpenWindow &&
							(machineRunning || wrStateInfo == EvolWrapperStatusInfo::Standby || wrStateInfo == EvolWrapperStatusInfo::Disabled) &&
							largeMotion)
				{
					if (debugWMPU)
						Debug::trace("WrapperMode.ProductUpdate", "Wrapping condition 2:  (wrapStateMachine == awmWrapOpenWindow) && largeMotion && ((machineRunning = " + HobString(Bool::toString(machineRunning)) + ") || ((wrStateInfo == Standby) = " + HobString(Bool::toString(wrStateInfo == EvolWrapperStatusInfo::Standby)));

					pCurrentProductTransaction = pNextProductTransaction;
					Debug::trace("AutoWeighEvolWrapperMode", "Open window. Set pCurrentProductTransaction = " + util_string::ptrtostring(pCurrentProductTransaction));

					//	HA-180: We were crashing because we were not checking for NULL pointers.
					if (pCurrentProductTransaction)
					{
						const HfcWeigherStatus *wgStatus( pCurrentProductTransaction->getWeigherStatus() );

						Debug::trace("AutoWeighEvolWrapperMode", "Open window.  wgStatus = " + util_string::ptrtostring(wgStatus));

						if ( wgStatus )
	        			{
	        				wrPackageWeight = (HobWeight) wgStatus->getWeight();

			        		if (( MetSig::isScaleAvoir() ) && ( wgStatus->isDualRangeEnabled() ))
				        		wrPackageWeight = Rounding::roundWeightToTwoImpliedDecimalPlaces(wrPackageWeight);		//	Round up to 2 decimal places
	        			}
						else
						{
							wrPackageWeight = pCurrentProductTransaction->getTotalNetWeight();
						}

						Debug::trace("AutoWeighEvolWrapperMode", "Open window.  wrPackageWeight = " + HobString(Integer::toString(wrPackageWeight)));

						wrapPackage(wrPackageWeight);
					}
				}
				else if (wrapStateMachine != awmWrapOpenWindow)
				{
					if (debugWMPU)
						Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 8:  wrapStateMachine = " + inttostring(wrapStateMachine));

					//	Dump this transaction until we get an open window message.
					sendProductComplete(pNextProductTransaction, false);
				}
				else if ((machineRunning == false) && largeMotion)
				{
					Debug::trace("AutoWeighEvolWrapperMode", "(machineRunning == false) && largeMotion");
					if (packageMap.size() < MANUAL_MaxLabels)
					{
						if (debugWMPU)
							Debug::trace("WrapperMode.ProductUpdate", "Wrapping condition 3:  largeMotion && (machineRunning = " + HobString(Bool::toString(machineRunning)) + ")");

						pCurrentProductTransaction = pNextProductTransaction;
						Debug::trace("AutoWeighEvolWrapperMode", "Large motion. Set pCurrentProductTransaction = " + util_string::ptrtostring(pCurrentProductTransaction) + ".  Call wrapThatPackage(pCurrentProductTransaction)");
						wrapThatPackage(pCurrentProductTransaction);
					}
					else
					{
						if (debugWMPU)
							Debug::trace("WrapperMode.ProductUpdate", "Not wrapping condition 5:  Too many packages!");

						if (pPendingPackage)
							sendProductComplete(pPendingPackage, false);
						pPendingPackage = pNextProductTransaction;
					}
				}
				else
				{
					if (debugWMPU)
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
			if (debugOpModeProd)
				Debug::trace("OperatingMode.Product", "ProductDev, ProductStatusEvent received");

			ProductStatusMsg prodStatus(recvBuffer);
			if (prodStatus.getStatus() == ProductStatusMsg::WeightBelowMinimumWeightToPrint)
			{
				if (debugOpModeProd)
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

void AutoWeighEvolWrapperMode::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer)
{
	switch (prEvent)
	{
		case PrStatus: 
		{
			PrinterStatus prStatus(recvBuffer);
			PrinterType printerType = prStatus.getPrinterType();

            if( printerHasErrors(prStatus) && !printerHasErrors_ )  
            {
				printerHasErrors_ = true;

                Debug::trace("AutoWeighEvolWrapperMode", "handlePrinterMessage(): PrStatus: Entry printerHasErrors!");

                if( prStatus.isHeadDisengaged() )
                    updatePrinterHeadUp(printerType);

                if ( prStatus.isTakeLabelError() )
                    Debug::trace("AutoWeighEvolWrapperMode", "handlePrinterMessage(): PrStatus: isTakeLabelError");
                else if ( prStatus.isLabelTakenError() )
                	Debug::trace("AutoWeighEvolWrapperMode", "handlePrinterMessage(): PrStatus: isLabelTakenError");

                wrapStateMachine = awmWrapError;
                emptyPackageMap();

                if (pPendingPackage != NULL)
	            {
		            sendProductComplete(const_cast <ProductTransaction *> (pPendingPackage), false);
		            pPendingPackage = NULL;
	            }

	            if (pCurrentProductTransaction != NULL)
	            {
		            sendProductComplete(const_cast <ProductTransaction *> (pCurrentProductTransaction), false);
		            pCurrentProductTransaction = NULL;
	            }

                Debug::trace("AutoWeighEvolWrapperMode", "handlePrinterMessage(): PrStatus: Exit printerHasErrors!");
            }
            else if( !printerHasErrors(prStatus) && printerHasErrors_ ) 
            {
                printerHasErrors_ = false;
                if (pCurrentProduct)
		        {
            		unlockWeigher(); // NGW-15
			        wrapStateMachine = awmWrapOpenWindow;		//	HA-182
					Debug::trace("WrapperMode.StateMachine", "handlePrinterMessage(PrStatus):  Printer error cleared!  Set wrapStateMachine = awmWrapOpenWindow");
		        }
		        else
		        {
			        wrapStateMachine = awmWrapInitial;
			        Debug::trace("WrapperMode.StateMachine", "handlePrinterMessage(PrStatus):  Printer error cleared!  Set wrapStateMachine = awmWrapInitial");
		        }
            }

			else if ( prStatus.isSizeCmdComplete() )
			{
                Debug::trace("AutoWeighEvolWrapperMode", "handlePrinterMessage(PrStatus):  Printer size complete");
				updateSizeCmdComplete(printerType);
				
                Debug::trace("WrapperMode.StateMachine", "handlePrinterMessage(PrStatus):  Set wrapStateMachine = awmWrapInitial");
                wrapStateMachine = awmWrapInitial;

                Debug::trace("AutoWeighEvolWrapperMode", "handlePrinterMessage(PrStatus):  Printer head is down");
				if ( getNewProduct() || pCurrentProduct )
				{
                    Debug::trace("AutoWeighEvolWrapperMode", "handlePrinterMessage(PrStatus):  Getting new product and opening window");
					changeProduct();
					preImagePrinters();	

					openWrapWindow();
				}
			}
			break;
		}
		// We listen to this message with the only purpose of relaying this
		// information to the corresponding label applier device.
		case PrLabelReady: 
		{
			bool traceOn = Debug::isTracing("AutoWeighEvolWrapperMode");
			HobString msgDebug;
			HobString msgDebug2;
			if (traceOn)
			{
				msgDebug = "  PrLabelReady";
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
				msgDebug.append(":   ");
				msgDebug2 = msgDebug;
			}
			PrinterLabelReady prLabelReady(recvBuffer);
			PrinterType printerType = prLabelReady.getPrinterType();

			if ( printerType == FirstPrinter )
			{
				laStateMachine1 = wmLaLabelReady;
				Debug::trace("WrapperMode.StateMachine", "laStateMachine1 = wmLaLabelReady");

				sendLabelReady(PrimaryPrinterStation, prLabelReady.getPackId());
			}
			else if ( printerType == SecondPrinter )
			{
				laStateMachine2 = wmLaLabelReady;
				Debug::trace("WrapperMode.StateMachine", "laStateMachine2 = wmLaLabelReady");

				sendLabelReady(SecondaryPrinterStation, prLabelReady.getPackId());
			}
			else if (printerType == ThirdPrinter)
			{
				laStateMachine3 = wmLaLabelReady;
				Debug::trace("WrapperMode.StateMachine", "laStateMachine3 = wmLaLabelReady");

				sendLabelReady(ThirdPrinterStation, prLabelReady.getPackId());
			}
			if (traceOn)
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug2 + "Trans# = " + HobString(Integer::toString(prLabelReady.getRecordNumber())) + ",  Transaction = " + util_string::ptrtostring(prLabelReady.getProductTransaction()) + ",  packId = " + HobString(Integer::toString(prLabelReady.getPackId())));
			break;
		}	
		// We listen to this message with the only purpose of relaying this
		// information to the corresponding label applier device.
		case PrLabelTaken: 
		{
			bool traceOn = Debug::isTracing("AutoWeighEvolWrapperMode");
			HobString msgDebug;
			HobString msgDebug2;
			if (traceOn)
			{
				msgDebug = "  PrLabelTaken";
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
				msgDebug.append(":   ");
				msgDebug2 = msgDebug;
			}

			PrinterLabelReady prLabelTaken(recvBuffer);
			PrinterType printerType = prLabelTaken.getPrinterType();

			if (laStateMachine1 == wmLaError || laStateMachine2 == wmLaError || laStateMachine3 == wmLaError)
				requestWrapperStatus();

			if ( printerType == FirstPrinter )
			{
				if (opModeStatus.isPkgDischargeError(FirstPrinter))
				{
					if (traceOn)
						Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Calling sendPkgDischargeError(FirstPrinter, false)");
					sendPkgDischargeError(FirstPrinter, false);		//	Clear this error.
					pkgDischargeError = MaxPrinterTypes;
				}
				laStateMachine1 = wmLaLabelTaken;
				Debug::trace("WrapperMode.StateMachine", "laStateMachine1 = wmLaLabelTaken");

				sendLabelTaken(PrimaryPrinterStation, prLabelTaken.getPackId());
			}
			else if ( printerType == SecondPrinter )
			{
				if (opModeStatus.isPkgDischargeError(SecondPrinter))
				{
					if (traceOn)
						Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Calling sendPkgDischargeError(SecondPrinter, false)");
					sendPkgDischargeError(SecondPrinter, false);		//	Clear this error.
					pkgDischargeError = MaxPrinterTypes;
				}
				laStateMachine2 = wmLaLabelTaken;
				Debug::trace("WrapperMode.StateMachine", "laStateMachine2 = wmLaLabelTaken");

				sendLabelTaken(SecondaryPrinterStation, prLabelTaken.getPackId());
			}
			else if ( printerType == ThirdPrinter )
			{
				if (opModeStatus.isPkgDischargeError(ThirdPrinter))
				{
					if (traceOn)
						Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Calling sendPkgDischargeError(ThirdPrinter, false)");
					sendPkgDischargeError(ThirdPrinter, false);		//	Clear this error.
					pkgDischargeError = MaxPrinterTypes;
				}
				laStateMachine3 = wmLaLabelTaken;
				Debug::trace("WrapperMode.StateMachine", "laStateMachine3 = wmLaLabelTaken");

				sendLabelTaken(ThirdPrinterStation, prLabelTaken.getPackId());
			}
			if (traceOn)
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug2 + "Trans# = " + HobString(Integer::toString(prLabelTaken.getRecordNumber())) + ",  Transaction = " + util_string::ptrtostring(prLabelTaken.getProductTransaction()) + ",  packId = " + HobString(Integer::toString(prLabelTaken.getPackId())));
			break;
		}	
		case PrLabelComplete: 
		{
			HobWeight wrPackageWeight;
			Debug::trace("AutoWeighEvolWrapperMode", "handlePrinterMessage(PrLabelComplete)");
			PrinterLabelComplete prLabelComplete(recvBuffer);

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
				{
					/* why are we unlocking the weigher on a label complete message????
					 * the only time the weigher should be unlocked is on an open....
					 * -- ats 01162017
					 unlockWeigher();
					 */
				}

				if (pPendingPackage)
				{
					pCurrentProductTransaction = pPendingPackage;
					pPendingPackage = NULL;
					Debug::trace("AutoWeighEvolWrapperMode", "PrLabelComplete:  largeMotion = true");
					largeMotion = true;

					const HfcWeigherStatus *wgStatus(pCurrentProductTransaction->getWeigherStatus());
					if (wgStatus)
					{
						wrPackageWeight = (HobWeight)wgStatus->getWeight();

						if (MetSig::isScaleAvoir() && wgStatus->isDualRangeEnabled())
							wrPackageWeight = Rounding::roundWeightToTwoImpliedDecimalPlaces(wrPackageWeight);		//	Round up to 2 decimal places
					}
					else
					{
						wrPackageWeight = pCurrentProductTransaction->getTotalNetWeight();
					}

					wrapPackage(wrPackageWeight);
				}
				Debug::trace("AutoWeighEvolWrapperMode", "handlePrinterMessage(PrLabelComplete):  Done");
			}
			break;
		}

		case PrConfig:
			Debug::trace("AutoWeighEvolWrapperMode", "handlePrinterMessage(PrConfig)");
			handlePrinterConfigInOpMode(recvBuffer);
			break;

		default:
			OperatingMode::handlePrinterMessage(prEvent, recvBuffer);
			break;
	}
}

void AutoWeighEvolWrapperMode::handleWeigherMessage(WgEvent wgEvent, const char *recvBuffer)
{
	switch (wgEvent)
	{
		case WgStatus:
		{
			Debug::trace("AutoWeighEvolWrapperMode.WgStatus", "handleWeigherMessage(WgStatus)");
			WeigherStatus wgStatus(recvBuffer);

			if (Debug::isTracing("WrapperMode.WeigherStatus") == true)
				wgStatus.show();

			HobWeight packageWeight = (HobWeight) wgStatus.getWeight();
			bool nonZeroStableWeight = (wgStatus.isLargeMotion() == false) && (wgStatus.isSmallMotion() == false) && (wgStatus.isWithinZeroQuarterGrad() == false) && (packageWeight != 0L);
			Debug::trace("AutoWeighEvolWrapperMode.WgStatus", "handleWeigherMessage(WgStatus):  packageWeight = " + longtostring(packageWeight) + ",  nonZeroStableWeight = " + booltostring(nonZeroStableWeight));

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
				Debug::trace("WrapperMode.WeigherStatus", "Ghost Package: 'nonZeroStableWeight' is true, largeMotion, 'packageWeight' is less than min weight to print and 'wrapStateMachine' is awmWrapOpenWindow");

				setGhostPackage();
				ghostPackage = true;
			}
			else if ((wgStatus.isPositiveNonZeroStableWeight() == true) && 
				(packageWeight >= Configuration::getInstance()->getMinWeightToPrint()) &&
				(largeMotion == true))
			{
				if (ghostPackage)
				{
					Debug::trace("WrapperMode.WeigherStatus", "Ghost Package: Clearing the message.");
					clearGhostPackage();
					ghostPackage = false;
				}
	
				Debug::trace("WrapperMode.WeigherStatus", "handleWeigherMessage(WgStatus):  largeMotion == true");
				if (wrapStateMachine == awmWrapError &&
					 curWrapState != WrapperSetWrapMode::ManualWeigh &&
					 curWrapState != WrapperSetWrapMode::AutoManualApply)
				{
					Debug::trace("WrapperMode.WeigherStatus", "'wrapStateMachine' is awmWrapError");

					// redisplay the error!
					dispatchEvent(&opModeStatus);
				}

				HobString theState;
				EvolWrapperStatusInfo::getWrapperStateName(wrStateInfo, theState);
				Debug::trace("WrapperMode.WeigherStatus", "handleWeigherMessage(WgStatus):  wrStateInfo = " + theState);

				if ((wrStateInfo == EvolWrapperStatusInfo::Unknown) &&
						 (lastWrapperStatus.isPeripheralComm() == true) && (lastWrapperStatus.isManagerDeleted() == true))
				{
					Debug::trace("AutoWeighEvolWrapperMode.WgStatus", HobString("handleWeigherMessage(WgStatus):  Wrapper has detached"));
				}

				Configuration *pConfig = Configuration::getInstance();

				if (wrapOnlyMode)
					Debug::trace("AutoWeighEvolWrapperMode.WgStatus", "wrapOnlyMode=True");
				else
					Debug::trace("AutoWeighEvolWrapperMode.WgStatus", "wrapOnlyMode=False");

				if( pConfig->isScaleModelWrapOnly() )
					Debug::trace("AutoWeighEvolWrapperMode.WgStatus", "isScaleModelWrapOnly=True");
				else
					Debug::trace("AutoWeighEvolWrapperMode.WgStatus", "isScaleModelWrapOnly=False");

				if (wrapOnlyMode &&
					 (wrStateInfo == EvolWrapperStatusInfo::Running || wrStateInfo == EvolWrapperStatusInfo::Standby  || wrStateInfo == EvolWrapperStatusInfo::Disabled ||
					  wrStateInfo == EvolWrapperStatusInfo::Homing || wrStateInfo == EvolWrapperStatusInfo::Idle || wrStateInfo == EvolWrapperStatusInfo::BuildingAirPressure ||
					  pConfig->isScaleModelWrapOnly() )) // SOF-789: Wrap-only
				{
					//	PTR 1661: We don't have a transaction here, so check for dualRange based on the weigher model.
					if ( MetSig::isScaleAvoir() && MetSig::isWeigherDualRangeCapable() )
					{
						packageWeight = Rounding::roundWeightToTwoImpliedDecimalPlaces(packageWeight);		//	Round up to 2 decimal places
					}
					wrapPackage(packageWeight);
				}
				else
				{
					// HA-65
					Debug::trace("AutoWeighEvolWrapperMode.WgStatus", HobString("handleWeigherMessage(WgStatus):  pCurrentProduct = ") + ptrtostring(pCurrentProduct));
					Debug::trace("AutoWeighEvolWrapperMode.WgStatus", HobString("handleWeigherMessage(WgStatus):  curWrapState = ") + Integer::toString(curWrapState));
					if ( (((pCurrentProduct == 0) || (pCurrentProduct && pCurrentProduct->getProductNum() == 0)) && (curWrapState != WrapperSetWrapMode::ManualWeigh) && (packageWeight > 0)) 			
						 || (pCurrentProductSmartPtr == 0) )//SOF-5191 entering PLU of 0
					{  
						Debug::trace("AutoWeighEvolWrapperMode.WgStatus", HobString("handleWeigherMessage(WgStatus):  Sending ProductRequiredToWrap message"));
						opModeStatus.setProductRequiredToWrap();
						dispatchEvent(&opModeStatus);
						largeMotion = false;
						Debug::trace("AutoWeighEvolWrapperMode.WgStatus", HobString("handleWeigherMessage(WgStatus):  largeMotion = false"));
					}
					Debug::trace("WrapperMode.WeigherStatus", "handleWeigherMessage(WgStatus):  wgStatus.setEventType(WgStatusAndPrint) and dispatch it.");
					wgStatus.setEventType(WgStatusAndPrint);
					dispatchEvent(&wgStatus);
				}
			}
			else
			{
				if (wgStatus.isLargeMotion() == true)
				{
					Debug::trace("WrapperMode.WeigherStatus", "Large Motion:  largeMotion = true");
					largeMotion = true;

					if ((machineRunning == false) && (pPendingPackage != NULL))
					{
						sendProductComplete(pPendingPackage, false);
						pPendingPackage = NULL;

						clearPressStart();		//	Let the next stable package put up the message, since keeping it up prevents us from receiving a ProductUpdate. [CCD Sep 2015]
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
						Debug::trace("WrapperMode.WeigherStatus", "No GhostPackage. Clear any pending packages.");
						if (pPendingPackage)
						{
							sendProductComplete(pPendingPackage, false);
							pPendingPackage = NULL;
						}

						if ( !machineRunning )
							if ((wrStateInfo == EvolWrapperStatusInfo::Standby) || (wrStateInfo == EvolWrapperStatusInfo::Disabled))
							{
								Debug::trace("WrapperMode.WeigherStatus", "(!machineRunning && (wrStateInfo == EvolWrapperStatusInfo::Standby)). clearPressStart()");
								clearPressStart();
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
				Debug::trace("WrapperMode.WeigherCachedStatus", "largeMotion = true");
				largeMotion = true;
			}
			else
			{
				Debug::trace("WrapperMode.WeigherCachedStatus", "WgCachedStatus large motion wasn't set");
			}
			break;
		}
		default:
		{
			break;
		}
	}						
}

void AutoWeighEvolWrapperMode::handleWrapperMessage(WrEvent wrEvent, const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighEvolWrapperMode");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperMessage(";
		msgDebug.append(Integer::toString(wrEvent)).append(")");
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
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
			//if ( wrapStateMachine == awmWrapWrap )
				openWrapWindow();

			break;
		}

		// This message is sent by RTC to report Operation Mode changes in the UI
		case WrSetWrapMode:
		{
			WrapperSetWrapMode wrapState(recvBuffer);
			curWrapState = wrapState.getWrapMode();
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
			Debug::trace("WrapperMode.StateMachine", "AutoWeighEvolWrapperMode, WrTest--received");

			if (machineRunning == false)
			{
				Debug::trace("WrapperMode.StateMachine", "AutoWeighEvolWrapperMode, calling sendWrapperTest()");
				WrapperTest wrTest(recvBuffer);
				sendWrapperTest(wrTest.getWrTestCode());
			}

			break;
		}
		case WrEvolConfig:
		{
			handleWrapperConfig(recvBuffer);			
			break;
		}
		case WrEvolPackageDischarge:
		{
			if (traceOn)
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "WrEvolPackageDischarge");
			handleWrapperPackageDischarge(recvBuffer);
			break;
		}
		default:
			if (traceOn)
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Unknown Event!");
			break;
	}						
}

void AutoWeighEvolWrapperMode::handleWrapperEvolStatus(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighEvolWrapperMode");
	HobString msgDebug;
//	HobWeight wrPackageWeight;
	if (traceOn)
	{
		msgDebug = "handleWrapperEvolStatus()";
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
		msgDebug.append(":  ");
	}
	EvolWrapperStatus statusMsg(recvBuffer);

#if 0
	if (traceOn)
	{
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "**********   Start EvolWrapperStatus show()   **********");
		statusMsg.show();
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "**********    End  EvolWrapperStatus show()   **********");
	}
#endif

	Debug::trace("EvolWrapperStatus.NGW", "wrStateInfo: " + Integer::toString( wrStateInfo ) );
	Debug::trace("EvolWrapperStatus.NGW", "pCurrentProduct: " + Hex::toString( (void*)pCurrentProduct ) );

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
//			Debug::trace("AutoWeighEvolWrapperMode", msgDebug2);

			msgDebug2 = msgDebug;
			EvolWrapperStatusInfo::getWrapperStateName(lastStateInfo, tempState);
			msgDebug2.append("lastStateInfo = ").append(tempState);
			msgDebug2.append(",   mainPower = ").append(Bool::toString(lastWrapperStatus.getStatusInfo().getMainPower()));
//			Debug::trace("AutoWeighEvolWrapperMode", msgDebug2);
		}

		if (((wrStateInfo == EvolWrapperStatusInfo::Homing) && statusMsg.getStatusInfo().getMainPower()) &&
			((lastStateInfo != EvolWrapperStatusInfo::Homing) || !(lastWrapperStatus.getStatusInfo().getMainPower())))
		{
			Debug::trace("EvolWrapperStatus.NGW", "Homing!");
			//	Set EvolWrapperHoming only if the state is Homing AND we have MainPower.
			opModeStatus.setEvolWrapperHoming();
			dispatchEvent(&opModeStatus);
		}
		else if ((wrStateInfo != EvolWrapperStatusInfo::Homing) && (lastStateInfo == EvolWrapperStatusInfo::Homing))
		{
			Debug::trace("EvolWrapperStatus.NGW", " Clear Homing");

			opModeStatus.clearEvolWrapperHoming();
			dispatchEvent(&opModeStatus);

			//	Request a weigher status, in case a package is sitting on the platter.
			if (pCurrentProduct)
			{
				requestWeigherStatus();
			}
			Debug::trace("AutoWeighEvolWrapperMode", HobString(tName) + HobString(": lastStateInfo was Homing but wrStateInfo is NOT Homing"));
		}
		else if (((wrStateInfo == EvolWrapperStatusInfo::BuildingAirPressure) && statusMsg.getStatusInfo().getMainPower()) &&
			((lastStateInfo != EvolWrapperStatusInfo::BuildingAirPressure) || !(lastWrapperStatus.getStatusInfo().getMainPower())))
		{
			Debug::trace("EvolWrapperStatus.NGW", " Building Air Pressure");
			//	Set EvolWrapperHoming only if the state is BuildingAirPressure AND we have MainPower.
			opModeStatus.setEvolWrapperBuildingAirPressure();
			dispatchEvent(&opModeStatus);
		}
		else if ((wrStateInfo != EvolWrapperStatusInfo::BuildingAirPressure) && (lastStateInfo == EvolWrapperStatusInfo::BuildingAirPressure))
		{
			opModeStatus.clearEvolWrapperBuildingAirPressure();
			dispatchEvent(&opModeStatus);
			Debug::trace("EvolWrapperStatus.NGW", " Clear Building Air Pressure");
			//	Request a weigher status, in case a package is sitting on the platter.
			if (pCurrentProduct)
			{
				requestWeigherStatus();
			}
			Debug::trace("AutoWeighEvolWrapperMode", HobString(tName) + HobString(": lastStateInfo was BuildingAirPressure but wrStateInfo is NOT BuildingAirPressure"));
		}
		else if ((opModeStatus.isWrapperPressStart()) && (wrStateInfo == EvolWrapperStatusInfo::Running) &&
				((lastStateInfo == EvolWrapperStatusInfo::Standby) || (lastStateInfo == EvolWrapperStatusInfo::Disabled)))
		{
			Debug::trace("EvolWrapperStatus.NGW", " Clear Press Start");
			if (traceOn)
				Debug::trace("AutoWeighEvolWrapperMode", HobString(tName) + "(opModeStatus.isWrapperPressStart()). clearPressStart()");
			clearPressStart();
		}

//		wrapperSettings.setTemperatureInfo(statusMsg.getTemperatureInfo());

#ifdef USING_WRAPPER_CUBE
		wrStateInfo = EvolWrapperStatusInfo::Running;
#endif
		machineRunning = (wrStateInfo == EvolWrapperStatusInfo::Running);

		if (traceOn)
		{
			if (machineRunning)
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug + HobString("The wrapper is running"));
			else
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug + HobString("The wrapper is *NOT* running: state ") + Integer::toString(wrStateInfo));
		}

/*
		if (pCurrentProduct && wrStatus.getPackageInfo((UINT16) IntakePosition).isGhostPackage())
		{
			setGhostPackage();
			sendPrinterReset(FirstPrinter);
			sendPrinterReset(SecondPrinter);
		}
*/

		EvolWrapperErrorInfo currentErrorInfo = statusMsg.getCurrErrorInfo();
		EvolWrapperErrorInfo::EvolWrapperErrorSource errorSource = currentErrorInfo.getEvolWrapperErrorSource();
		EvolWrapperErrorInfo::EvolWrapperErrorSource currErrorSource = opModeStatus.getEvolWrapperErrorSource();
		EvolWrapperErrorInfo::EvolWrapperError error = currentErrorInfo.getEvolWrapperError(errorSource);
		EvolWrapperErrorInfo::EvolWrapperError currError = opModeStatus.getEvolWrapperError();

		if (error == EvolWrapperErrorInfo::GhostPackageDetected)
		{
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Clearing the GhostPackageDetected error as it is not applicable here.  errorSource = " + inttostring(errorSource));
			error = EvolWrapperErrorInfo::NoWrapperError;
			errorSource = EvolWrapperErrorInfo::NoWrapperErrorSource;
		}

#ifdef USING_WRAPPER_CUBE
		if ((errorSource != currErrorSource) || (error != currError))
		{
			if (traceOn)
			{
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "    ****    USING_WRAPPER_CUBE    ****");
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Before:  errorSource = " + inttostring(errorSource) + ",  error = " + inttostring(error));
			}
			if ((error == EvolWrapperErrorInfo::PrimaryConveyorNotAtHome) || (error == EvolWrapperErrorInfo::Node2_MotorFuseBlown))
			{
				//	These errors perpetually show up using a wrapper cube and interfere with testing.
				//	Therefore, we will ignore these while using the cube.
				errorSource = currErrorSource;
				error = currError;
			}
			if (traceOn)
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "After:  errorSource = " + inttostring(errorSource) + ",  error = " + inttostring(error));
		}
#endif

		if ((errorSource != currErrorSource) || (error != currError))
		{
			if (traceOn)
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug + " Error is different");

			// leave the error message up until the stop switch is cycled
			if (errorSource != EvolWrapperErrorInfo::NoWrapperErrorSource)
			{
				// NGW-255
				// unlock weigher for certain errors that remove high power, otherwise wrapper
				// appears to be locked up
				// are there more cases here???
				if ( EvolWrapperStatusInfo::Running == wrStateInfo )
					if (( EvolWrapperErrorInfo::PrimaryConveyorBlocked == error ) ||
						( EvolWrapperErrorInfo::PackageBlockingIntake == error ) ||
						( EvolWrapperErrorInfo::SecondaryShiftLeftLimitError == error ) ||
						( EvolWrapperErrorInfo::SecondaryShiftRightLimitError == error ))
					{
						if (traceOn)
							Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Unlocking weigher due to error message");

						unlockWeigher();
					}

				setNewEvolWrapperError(error, errorSource);
			}
			else if ((wrStateInfo == EvolWrapperStatusInfo::Standby) || (wrStateInfo == EvolWrapperStatusInfo::Disabled))
			{
				setNewEvolWrapperError(error, errorSource);

				if (wrapStateMachine == awmWrapError)
				{
					if (traceOn)
						Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Error cleared!");

                    if( !printerHasErrors_ )
                    {
                        if (pCurrentProduct)
				        {
					        wrapStateMachine = awmWrapWrap;
					        Debug::trace("WrapperMode.StateMachine", "handleWrapperEvolStatus():   Set wrapStateMachine = awmWrapWrap");
				        }
				        else
				        {
					        wrapStateMachine = awmWrapInitial;
					        Debug::trace("WrapperMode.StateMachine", "handleWrapperEvolStatus():   Set wrapStateMachine = awmWrapInitial");
				        }
                    }
				}
			}
		}

		if (errorSource == EvolWrapperErrorInfo::NoWrapperErrorSource && wrapStateMachine != awmWrapError)
		{
			if (traceOn)
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "errorSource == NoWrapperErrorSource");
			if ((lastStateInfo == EvolWrapperStatusInfo::Standby)/* && (wrStateInfo != EvolWrapperStatusInfo::Standby)*/)
			{

				if (pCurrentProductTransaction)
				{
					sendWrapperAbort();
					// restart the system because this is bad!
					invalidatePackage(pCurrentProductTransaction);
					//pCurrentProductTransaction = NULL;
					if (traceOn)
						Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Standby:  sendWrapperAbort() & invalidatePackage()");
				}
			}

			if (machineRunning)
			{
				Debug::trace("EvolWrapperStatus.NGW", "MachineRunning: Yes");
				if (pPendingPackage && !opModeStatus.isAnyPkgDischargeError())
				{
					Debug::trace("EvolWrapperStatus.NGW", "Pending Package");
					// make the pending one the current one!
					if (traceOn)
						Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "pPendingPackage is now the current one.");
					pCurrentProductTransaction = pPendingPackage;
					pPendingPackage = NULL;
					Debug::trace("AutoWeighEvolWrapperMode", "Set pCurrentProductTransaction to pPendingPackage = " + util_string::ptrtostring(pCurrentProductTransaction) + ".  Call wrapThatPackage(pCurrentProductTransaction)");

					wrapThatPackage(pCurrentProductTransaction);
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
					Debug::trace("AutoWeighEvolWrapperMode", msgDebug2);
				}
			}
			else
			{
				if (traceOn)
					Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Wrapper Error Detected!");

				wrapStateMachine = awmWrapError;
				emptyPackageMap();

				Debug::trace("WrapperMode.StateMachine", "wrapStateMachine: awmWrapError");

				if (traceOn)
					Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Wrapper Error - beeping 3 times");
			
				ServiceSpeaker::badBeep(3);
			}
		}
	}

	HobString msgDebug2 = "AutoWeighEvolWrapperMode::handleWrapperEvolStatus()";
	msgDebug2.append("\n           errorSource = ").append(Integer::toString(opModeStatus.getEvolWrapperErrorSource()));
	msgDebug2.append("\n                 error = ").append(Integer::toString(opModeStatus.getEvolWrapperError()));
	msgDebug2.append("\n                packId = ").append(Integer::toString(packId));
	msgDebug2.append("\n        machineRunning = ").append(Bool::toString(machineRunning));
	msgDebug2.append("\n      wrapStateMachine = ").append(Integer::toString(wrapStateMachine));
	msgDebug2.append("\n isNewEvolWrapperError = ").append(Bool::toString(opModeStatus.isNewEvolWrapperError()));
	Debug::trace("Clare", msgDebug2);

	lastWrapperStatus = statusMsg;
}

void AutoWeighEvolWrapperMode::wrapThatPackage(const ProductTransaction *pCurrentProductTrans)
{
	// HA-180: We were crashing because we were not checking for NULL pointers.
	if ( !pCurrentProductTrans )
	{
		Debug::trace("AutoWeighEvolWrapperMode", "wrapThatPackage():  no current product");
		return;
	}

	const HfcWeigherStatus *wgStatus( pCurrentProductTrans->getWeigherStatus() );
	Debug::trace("AutoWeighEvolWrapperMode", "wrapThatPackage():  wgStatus = " + ptrtostring(wgStatus));

	HobWeight wrPackageWeight;
	if ( wgStatus )
	{
		wrPackageWeight = (HobWeight) wgStatus->getWeight();
		if (( MetSig::isScaleAvoir() ) && ( wgStatus->isDualRangeEnabled() ))
			wrPackageWeight = Rounding::roundWeightToTwoImpliedDecimalPlaces(wrPackageWeight);		//	Round up to 2 decimal places
	}
	else
	{
		wrPackageWeight = pCurrentProductTrans->getTotalNetWeight();
	}

	Debug::trace("AutoWeighEvolWrapperMode", "wrapThatPackage():  wrPackageWeight = " + HobString(Integer::toString(wrPackageWeight)));

	wrapPackage( wrPackageWeight );
}

void AutoWeighEvolWrapperMode::wrapPackage(HobWeight packageWeight, bool ignoreWeighMode)
{
	bool traceOn = Debug::isTracing("WrapperMode.Wrap");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "AutoWeighEvolWrapperMode::wrapPackage():    ";
		HobString curWrapStateStr;
		//WrapperSetWrapMode::getWrapModeText(curWrapState, curWrapStateStr);

		if (pCurrentProductTransaction)		//	Check because we don't always have a transaction coming in here.
			Debug::trace("WrapperMode.Wrap", msgDebug + "0x" + HobString(Hex::toString(pCurrentProductTransaction->getTransactionNum())));
		Debug::trace("WrapperMode.Wrap", msgDebug + "   curWrapState:" + HobString(Integer::toString((int)curWrapState)));
		Debug::trace("WrapperMode.Wrap", msgDebug + "    wrStateInfo:" + HobString(Integer::toString((int)wrStateInfo)));
		Debug::trace("WrapperMode.Wrap", msgDebug + "ignoreWeighMode:" + HobString(Bool::toString(ignoreWeighMode)));
	}

	//	HA-160:  To print a product support text label, we just want to print regardless of the Weigh mode.
	//			 We don't need the wrapper to take in a package to print an Expanded text label.
	if (ignoreWeighMode == true)
	{
		if (traceOn)
			Debug::trace("WrapperMode.Wrap", msgDebug + "ignoreWeighMode - Just print the label");
	}
	else if ((curWrapState == WrapperSetWrapMode::ManualWeigh) || (curWrapState == WrapperSetWrapMode::AutoManualApply))
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
			msgDebug2.append(inttostring((int)packageWeight)).append(" >= ");
			msgDebug2.append(inttostring((int)getMaxAWSPackageWeight())).append(")");
			Debug::trace("WrapperMode.Wrap", msgDebug);
		}
		setPackageOverWeight();
	}
	else
	{
		if ((wrStateInfo == EvolWrapperStatusInfo::Standby) || (wrStateInfo == EvolWrapperStatusInfo::Disabled))
		{
			if (traceOn)
				Debug::trace("WrapperMode.Wrap", "(wrStateInfo == EvolWrapperStatusInfo::Standby). setPressStart()");
			setPressStart();
		}

		bool labelApplySyncErr = opModeStatus.isAnyPkgDischargeError();
		if (traceOn)
		{
			Debug::trace("WrapperMode.Wrap", msgDebug + "labelApplySyncErr = " + Bool::toString(labelApplySyncErr));
			Debug::trace("WrapperMode.Wrap", msgDebug + "pkgDischargeError = " + Integer::toString(pkgDischargeError));
			Debug::trace("WrapperMode.Wrap", msgDebug + "(laStateMachine1 == wmLaError) is " + Bool::toString(laStateMachine1 == wmLaError));
			Debug::trace("WrapperMode.Wrap", msgDebug + "(laStateMachine2 == wmLaError) is " + Bool::toString(laStateMachine2 == wmLaError));
		}
		if (pPendingPackage)
		{
			Debug::trace("AutoWeighEvolWrapperMode", "wrapPackage():  pPendingPackage not null. Calling sendProductComplete(pPendingPackage, false)");
			sendProductComplete(pPendingPackage, false);
			pPendingPackage = 0;
		}
		if (wrStateInfo == EvolWrapperStatusInfo::Standby || wrStateInfo == EvolWrapperStatusInfo::Disabled || wrStateInfo == EvolWrapperStatusInfo::Homing || wrStateInfo == EvolWrapperStatusInfo::Idle || wrStateInfo == EvolWrapperStatusInfo::BuildingAirPressure ||
			 laStateMachine1 == wmLaError || laStateMachine2 == wmLaError || laStateMachine3 == wmLaError ||
			 labelApplySyncErr)
		{
			Debug::trace("AutoWeighEvolWrapperMode", "wrapPackage():  Won't wrap because the wrapper is not ready. Exiting!");
			if (traceOn)
			{
				Debug::trace("WrapperMode.Wrap", "wrapPackage():    Won't wrap because the wrapper is not ready. Exiting!");
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("wrStateInfo = ").append(Integer::toString(wrStateInfo));
				msgDebug2.append("  laStateMachine1 = ").append(Integer::toString(laStateMachine1));
				msgDebug2.append("  laStateMachine2 = ").append(Integer::toString(laStateMachine2));
				msgDebug2.append("  laStateMachine3 = ").append(Integer::toString(laStateMachine3));
				Debug::trace("WrapperMode.Wrap", msgDebug2);
				Debug::trace("WrapperMode.Wrap", msgDebug + "pPendingPackage = pCurrentProductTransaction");
			}
			if (labelApplySyncErr)
				sendProductComplete(pCurrentProductTransaction, false);
			else
				pPendingPackage = pCurrentProductTransaction;
			pCurrentProductTransaction = NULL;
			Debug::trace("AutoWeighEvolWrapperMode", "wrapPackage():  pPendingPackage = " + util_string::ptrtostring(pPendingPackage));
			return;
		}
		else
		{
			if (opModeStatus.isWrapperPressStart())
			{
				if (traceOn)
					Debug::trace("WrapperMode.Wrap", "(opModeStatus.isWrapperPressStart()). clearPressStart()");
				clearPressStart();
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

	if ((wrapOnlyMode == true) && !ignoreWeighMode)
	{
		incPackId();
	}
	else
	{
		if (forcePreImage)
		{
			if (pCurrentProductTransaction)
				pCurrentProductTransaction->setForcePreImage(forcePreImage);
			forcePreImage = false;
		}
		// This function puts a new package in the package map, incrementing the
		// current packId, and requests all post-image needed.
		generateNewPackage(pCurrentProductTransaction);	
	}

	Debug::trace("AutoWeighEvolWrapperMode", "wrapPackage():  Setting largeMotion = false");
	largeMotion = false;
}

void AutoWeighEvolWrapperMode::firstPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::firstPrinterAttached(pPrinterMgr);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	openWrapWindow();
}

void AutoWeighEvolWrapperMode::firstPrinterDetached()
{
	_Mybase::firstPrinterDetached();

	resetWrapperMode();
}

void AutoWeighEvolWrapperMode::secondPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::secondPrinterAttached(pPrinterMgr);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	openWrapWindow();
}

void AutoWeighEvolWrapperMode::secondPrinterDetached()
{
	_Mybase::secondPrinterDetached();

	resetWrapperMode();
}

void AutoWeighEvolWrapperMode::thirdPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::thirdPrinterAttached(pPrinterMgr);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	openWrapWindow();
}

void AutoWeighEvolWrapperMode::thirdPrinterDetached()
{
	_Mybase::thirdPrinterDetached();

	resetWrapperMode();
}

void AutoWeighEvolWrapperMode::labelApplierAttached(Hobart::CEvolMsgReceiver *pLabelApplierManager)
{
	bool traceOn = Debug::isTracing("AutoWeighEvolWrapperMode");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "labelApplierAttached()";
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
		msgDebug.append(":  ");
	}

	_Mybase::labelApplierAttached( pLabelApplierManager );

	if (pCurrentProduct)
	{
		//	The AWS went down for some reason and is now back up. We need to resend a LaSetup message for the current Product.

		//	The fact that we now have a LabelApplierManager does not mean it has registered for all its messages
		//	and is ready to receive messages at this point. Since we can't tell when it is ready,
		//	we will set this flag and send it after we send a WrControl message to the Wrapper.
		if (traceOn)
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "updateApplierSetup = true");
		updateApplierSetup = true;
	}
}

void AutoWeighEvolWrapperMode::wrapperAttached(Hobart::CEvolMsgReceiver *pWrapperManager, WrapperSetWrapMode::WrapMode wrapMode)
{
	_Mybase::wrapperAttached( pWrapperManager );

	if (curWrapState != wrapMode)
	{
		handleWrapSetMode(wrapMode);
	}

	if (pWeigherManager != NULL)
	{
		setWrapperMode(WrOperateMode);
		requestWrapperStatus();
		requestWrapperInquire();
		openWrapWindow();
	}
}

void AutoWeighEvolWrapperMode::wrapperDetached()
{
	Debug::trace("AutoWeighEvolWrapperMode", "wrapperDetached()");
	_Mybase::wrapperDetached();

	wrStateInfo = EvolWrapperStatusInfo::Unknown;

	resetWrapperMode();
	setNewEvolWrapperError(EvolWrapperErrorInfo::NoWrapperError, EvolWrapperErrorInfo::NoWrapperErrorSource);
}

void AutoWeighEvolWrapperMode::weigherAttached(Hobart::CEvolMsgReceiver *pWeigherManager)
{
	_Mybase::weigherAttached( pWeigherManager );

	setWeigherMode(ScalePrepackMode);

	if (pWrapperManager != NULL)
	{
		setWrapperMode(WrOperateMode);
	}
}

void AutoWeighEvolWrapperMode::weigherDetached()
{
	_Mybase::weigherDetached();

	resetWrapperMode();
}

void AutoWeighEvolWrapperMode::changeProduct()
{
	Debug::trace("AutoWeighEvolWrapperMode", "changeProduct()");

	OperatingMode::changeProduct();

	if (pCurrentProduct && pCurrentProduct->getIntakeMode() != WrapperIntakeMode::DefaultIntake)
	{
		setIntakeMode(pCurrentProduct->getIntakeMode());
	}
}

void AutoWeighEvolWrapperMode::openWrapWindow()
{
	if (wrapStateMachine != awmWrapError)
	{
		unlockWeigher();
		wrapStateMachine = awmWrapOpenWindow;
		Debug::trace("WrapperMode.StateMachine", "openWrapWindow():   wrapStateMachine: awmWrapOpenWindow");
	}
}

void AutoWeighEvolWrapperMode::sendWrapPackage(WrapperIntakeMode::IntakeMode intakeMode, UINT8 packageId, bool pullFilm, UINT16 packageWeight)
{
	UINT32 pluNumber = (pCurrentProduct ? pCurrentProduct->getProductNum() : 0);

	Debug::trace("EvolWrapperStatus.NGW", "sendWrapPackage() intakeMode: " + Integer::toString( intakeMode ) + " packageId: " + Integer::toString( packageId ));
	Debug::trace("EvolWrapperStatus.NGW", "sendWrapPackage() packageWeight: " + Integer::toString( packageWeight ) + " pullFilm: " + Integer::toString( pullFilm ));

	EvolWrapperWrap wrWrap(this, intakeMode, packageId, pullFilm, packageWeight, pluNumber);
	dispatchEvent(&wrWrap);

	wrapStateMachine = awmWrapWrap;
	Debug::trace("WrapperMode.StateMachine", "wrapStateMachine: awmWrapWrap");
	lockWeigher();
}

void AutoWeighEvolWrapperMode::handleWrapperInquire(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighEvolWrapperMode");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperInquire()";
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
		msgDebug.append(":  ");
	}
	if (!recvBuffer)
	{
		if (traceOn)
		{
			HobString msgDebug2 = "recvBuffer is 0. Exiting!!!";
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + msgDebug2);
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

	EvolWrapperGeneralConfig wrapperConfig = wrInquire.getWrapperConfig();

	wrapperSettings.setUpperRollWidth(wrapperConfig.getTopFilmRollDimensions().getRollWidth());
	wrapperSettings.setLowerRollWidth(wrapperConfig.getBottomFilmRollDimensions().getRollWidth());

	settingsInitialized = true;

	// send control message if anything is changed!

	if (traceOn)
	{
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "   --  wrapperSettings.show() from wrInquire  ********");
		wrapperSettings.show();
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "   --  ***********************************************");
	}
	if (reqSettingsPending)
	{
		if (traceOn)
		{
			msgDebug.append("reqSettingsPending - dispatchEvent WrapperSettings.");
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
		}
		dispatchEvent(&wrapperSettings);
		reqSettingsPending = false;
	}

	if (updateApplierSetup || FpmManager::getInstance()->isWrapperTwoPrinter())
	{
		//	If the AWS went down, we can't send this message until the LabelApplierManager is up & running.
		//	By now, it should be.
		if (traceOn)
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Calling sendLabelApplierSetup().");
		
		/*This is a patch: race condition that I cannot at present recreate... (session deletes current product without informing RTC)
		  Possible fix would be to allow the realtime controller to create a copy of the product 
		  instead of using a pointer to the product. I talked with Larry, he will fix product */

		if( pCurrentProduct != NULL )
		{
		    if((pCurrentProduct->getProductType() != Product::PRODUCTTYPE_UNKNOWN) && (pCurrentProduct->getProductType() < Product::PRODUCTTYPE_INVALID))
		    {
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "sendLabelApplierSetup() sending....");

			    sendLabelApplierSetup(*pCurrentProduct);
		    }
		    else
		    {
			    Debug::trace("AutoWeighEvolWrapperMode", HobString("Product uninitialized!!!!!"));
		    }
        }
		updateApplierSetup = false;
	}	
}


void AutoWeighEvolWrapperMode::handleWrapperControl(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighEvolWrapperMode");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperControl()";
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
		msgDebug.append(":  ");
	}
	if (!recvBuffer)
	{
		if (traceOn)
		{
			HobString msgDebug2 = "recvBuffer is 0. Exiting!!!";
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + msgDebug2);
		}
		return;
	}

	EvolWrapperControl wrControl(recvBuffer);

	if (traceOn)
	{
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Showing received EvolWrapperControl object -----------------");
		wrControl.show();
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Done showing received EvolWrapperControl object ------------");
	}

	//	Do some wrapper settings upkeep
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
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Updated wrapperSettings.show()  ********");
		wrapperSettings.show();
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "****************************************");
	}

	wrControl.setSourceObject(this);

	if (pWrapperManager != NULL)
	{
		const char *pCC = wrControl.serialize();
		pWrapperManager->send(pCC, wrControl.getSerializedLength());
	}
}


void AutoWeighEvolWrapperMode::setIntakeMode(WrapperIntakeMode::IntakeMode intakeMode)
{
	if (intakeMode == WrapperIntakeMode::DefaultIntake)
	{
		intakeMode = (WrapperIntakeMode::IntakeMode) Configuration::getInstance()->getDefaultIntakeMode();
	}
	wrapperSettings.setIntakeMode(intakeMode);
}

void AutoWeighEvolWrapperMode::handleWrapSetMode(WrapperSetWrapMode::WrapMode wrapState)
{
	bool traceOn = Debug::isTracing("AutoWeighEvolWrapperMode");
	HobString msgDebug = "";

	if (traceOn)
	{
		msgDebug = "handleWrapSetMode():  ";

		HobString msgDebug2 = msgDebug;
		msgDebug2.append("wrapState = ").append(Integer::toString(wrapState)).append(" / ");
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
		msgDebug2.append(", curWrapState was ").append(Integer::toString(curWrapState));
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug2);
	}

	curWrapState = wrapState;
	wrapProduct = (wrapState == WrapperSetWrapMode::WrapOnly) || (wrapState == WrapperSetWrapMode::WrapOn);
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("wrapProduct = ").append(Bool::toString(wrapProduct));
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug2);
	}

	// SOF-789: Wrap-only
	Configuration *pConfig = Configuration::getInstance();
	if (pConfig->isScaleModelWrapOnly())
	{
		setWeigherMode(ScalePrepackMode);
		wrapOnlyMode = true;
	}
	else
	{
		switch (wrapState)
		{
			case WrapperSetWrapMode::WrapOn:
			case WrapperSetWrapMode::WrapOff:
				setWeigherMode(ScalePrepackMode);
				wrapOnlyMode = false;
			break;
			case WrapperSetWrapMode::WrapOnly:
				setWeigherMode(ScalePrepackMode);
				wrapOnlyMode = true;
			break;
			case WrapperSetWrapMode::UnknownMode:
			case WrapperSetWrapMode::ManualWeigh:
			default:
				setWeigherMode(ScaleManualMode);
				wrapOnlyMode = false;
			break;
		}
	}

	if (wrapOnlyMode)
		Debug::trace("AutoWeighEvolWrapperMode", "In handleWrapSetMode(): wrapOnlyMode=True");
	else
		Debug::trace("AutoWeighEvolWrapperMode", "In handleWrapSetMode(): wrapOnlyMode=False");

}

void AutoWeighEvolWrapperMode::handleWrapperRequestSettings()
{
	bool traceOn = Debug::isTracing("AutoWeighEvolWrapperMode");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperRequestSettings()";
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
		msgDebug.append(":   ");
	}

	if (settingsInitialized)
	{
		if (traceOn)
		{
			msgDebug.append("dispatchEvent WrapperSettings.");
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
		}
		dispatchEvent(&wrapperSettings);
		reqSettingsPending = false;
	}
	else
	{
		if (traceOn)
		{
			msgDebug.append("WrapperSettings not initialized! Can't send anything yet!!!");
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
		}
		reqSettingsPending = true;

		// Need to get type of system
		FpmManager *pSerialFlash = FpmManager::getInstance();
		std::string style = pSerialFlash->getMFGSystemStyle();

		// SOF-2870, SOF-2871, SOF-2872
		if ((style == "NGW3") || (style == "NGW2")) 
		{
			// FULL CAN - Need to dispatch the inquire request
			if (traceOn)
			{
				msgDebug = "";
				msgDebug.append("FULL CAN ... Need to request WrReqInquire");
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
			}

			WrapperGeneric msg( WrReqInquire, this );
			dispatchEvent( &msg );
		}
	}
}


void AutoWeighEvolWrapperMode::handleWrapperConfig(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighEvolWrapperMode");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperConfig()";
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
		msgDebug.append(":  ");
	}
	if (!recvBuffer)
	{
		if (traceOn)
		{
			HobString msgDebug2 = "recvBuffer is 0. Exiting!!!";
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + msgDebug2);
		}
		return;
	}

	EvolWrapperConfiguration wrConfig(recvBuffer);

	if (traceOn)
	{
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Showing received EvolWrapperConfiguration object -----------------");
		wrConfig.show();
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Done showing received EvolWrapperConfiguration object ------------");
	}

	//	Update the wrapperSettings with the possibly changed Upper and Lower film widths
	EvolWrapperGeneralConfig wrGenConfig = wrConfig.getWrapperConfig();
	wrapperSettings.setUpperRollWidth(wrGenConfig.getTopFilmRollDimensions().getRollWidth());
	wrapperSettings.setLowerRollWidth(wrGenConfig.getBottomFilmRollDimensions().getRollWidth());

	if (traceOn)
	{
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Showing current wrapperSettings -----------------");
		wrapperSettings.show();
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Done showing current wrapperSettings ------------");
	}
}

void AutoWeighEvolWrapperMode::handleWrapperPackageDischarge(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighEvolWrapperMode");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "handleWrapperPackageDischarge()";
		Debug::trace("AutoWeighEvolWrapperMode", msgDebug);
		msgDebug.append(":  ");
	}
	EvolWrapperPackageDischarge statusMsg(recvBuffer);
//	statusMsg.show();

	const OpModePackage *pPackage = getPackage(statusMsg.getPackId());
	if (traceOn)
	    Debug::trace("AutoWeighEvolWrapperMode", msgDebug + " wrapStateMachine:" + Integer::toString(wrapStateMachine));
	if (!pPackage)
	{
		if (traceOn)
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "pPackage is 0. Package is complete.");
	}
	else
	{
		//	We have a problem here. The package has been discharged by the wrapper
		//	but the transaction is not complete for that package. A label must be left behind.
		if (traceOn)
		{
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Current states:");
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "   laStateMachine1 = " + inttostring(laStateMachine1));
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "   laStateMachine2 = " + inttostring(laStateMachine2));
			Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "   laStateMachine3 = " + inttostring(laStateMachine3));
		}
		pkgDischargeError = MaxPrinterTypes;
		if (pPackage->isCompleted(FirstPrinter) == false)	
		{
			if (laStateMachine1 == wmLaLabelReady)
				pkgDischargeError = FirstPrinter;
		}	
		else if (pPackage->isCompleted(SecondPrinter) == false)	
		{
			if (laStateMachine2 == wmLaLabelReady)
				pkgDischargeError = SecondPrinter;
		}	
		else if (pPackage->isCompleted(ThirdPrinter) == false)	
		{
			if (laStateMachine3 == wmLaLabelReady)
				pkgDischargeError = ThirdPrinter;
		}	
		if (pkgDischargeError != MaxPrinterTypes)
		{
			if (traceOn)
				Debug::trace("AutoWeighEvolWrapperMode", msgDebug + "Calling sendPkgDischargeError()");
			sendPkgDischargeError(pkgDischargeError);

			emptyPackageMap();
			sendPrinterReset(FirstPrinter);
			sendPrinterReset(SecondPrinter);
			sendPrinterReset(ThirdPrinter);
		}
	}
}

bool AutoWeighEvolWrapperMode::printerHasErrors(PrinterStatus prStatus)
{
	//Prevent the wrapper from wrapping when certain errors are present
	switch( curWrapState )
	{
		case WrapperSetWrapMode::WrapOn:
		case WrapperSetWrapMode::WrapOff:
			return ( prStatus.isPriorityError() || prStatus.isTakeLabelError() );
		case WrapperSetWrapMode::WrapOnly:
		default:
            return false;
	}
}


void AutoWeighEvolWrapperMode::handleProductSmartPtr(Util::CSmartPtr<Product> &pCurrentProductSmartPtr)
{
	bool isTracing = Debug::isTracing("AutoWeighEvolWrapperMode");

	Product *pProduct = 0;
	pProduct = pCurrentProductSmartPtr.get();

#ifdef EXTRA_DEBUG
	Debug::trace("AutoWeighEvolWrapperMode", "handleProductSmartPtr():  Currently packId = " + HobString(Integer::toString(packId)));
#endif
	if (opModeStatus.isProductRequiredToWrap())
	{
		Debug::trace("AutoWeighEvolWrapperMode.laSetup", "AutoWeighEvolWrapperMode::handleProductSmartPtr(): opModeStatus.isProductRequiredToWrap()");
		Debug::trace("AutoWeighEvolWrapperMode", HobString("handleProductSmartPtr():  Clearing ProductRequiredToWrap message"));
		opModeStatus.clearProductRequiredToWrap();
		dispatchEvent(&opModeStatus);
	}

	// If changing the plu on the fly more than once before the next package is taken in, remove the previous
	// plu from the product queue (but only if there has been no stable weight for a package placed on the intake).
	// This assumes they can only have one other plu in the queue.
	if (wrapStateMachine == awmWrapChangingProduct)
	{
		emptyProductQ();
		if (pChangingProductPendingPackage)
		{
			Debug::trace("WrapperMode.ProductChange", "pChangingProductPendingPackage not null. Cancelling!");
			sendProductComplete(pChangingProductPendingPackage, false);
			pChangingProductPendingPackage = NULL;
		}
	}

	if (pPendingPackage != NULL)
	{
		if (isTracing)
			Debug::trace("AutoWeighEvolWrapperMode", HobString("handleProductSmartPtr():  pPendingPackage isn't NULL : 0x") + Hex::toString((long long) pPendingPackage));
		sendProductComplete((const ProductTransaction *) pPendingPackage, false);
		pPendingPackage = NULL;
	}

	// Only push the new product onto the product queue if it isn't NULL.
	// Can get a NULL when UI looks up product 0 - ie. clearing the product.
	if (pProduct == NULL)
		Debug::trace("AutoWeighEvolWrapperMode.laSetup", "AutoWeighEvolWrapperMode::handleProductSmartPtr(): IF pProduct == NULL");   // NGW-108 moved logic here and re-worked; was: pCurrentProduct = 0;		//	AWL-113
	else
	{
		forcePreImage = true;
		productQ.push(pCurrentProductSmartPtr);

		wrapStateMachine = awmWrapChangingProduct;
		Debug::trace("WrapperMode.StateMachine", "handleProductSmartPtr():  wrapStateMachine: awmWrapChangingProduct");

		if (isTracing)
			Debug::trace("AutoWeighEvolWrapperMode", HobString("handleProductSmartPtr():  packageMap size #2 is") + HobString(Integer::toString((int)packageMap.size())));

		if ( ( getNewProduct() ) && 
			 ( packageMap.empty() || ( FpmManager::getInstance()->isWrapperTwoPrinter() && pCurrentProductSmartPtr.get() ) )  )
		{
			if (isTracing)
				Debug::trace("AutoWeighEvolWrapperMode", HobString("handleProductSmartPtr():  packageMap is empty and getNewProduct()"));

			changeProduct();
			preImagePrinters();
			openWrapWindow();

			laStateMachine1 = wmLaInitial;
			Debug::trace("WrapperMode.StateMachine", "handleProductSmartPtr():  laStateMachine1: wmLaInitial");

			laStateMachine2 = wmLaInitial;
			Debug::trace("WrapperMode.StateMachine", "handleProductSmartPtr():  laStateMachine2: wmLaInitial");

			laStateMachine3 = wmLaInitial;
			Debug::trace("WrapperMode.StateMachine", "handleProductSmartPtr():  laStateMachine3: wmLaInitial");
		}
		else
		{
			// SOF-5560 Changing PLU or just Label Placement "on the fly" doesn't apply label correctly
			if (wrapStateMachine == awmWrapChangingProduct)
			{
				// Debug::trace("AutoWeighEvolWrapperMode.laSetup", "AutoWeighEvolWrapperMode::handleProductSmartPtr(): IF wrapStateMachine == awmWrapChangingProduct so CALL sendLabelApplierSetup()");
				sendLabelApplierSetup(pCurrentProductSmartPtr.get());			// added
			}

		}
		
		// I think this IF below only happens when you change PLU on the fly. In any case, this does not work when changing PLUs on the fly
		// but leaving it here for one printer systems just in case (2-1-2023 its not working for one printer systems either now. Dont know if it did)
		//  This can be removed if you get rid of two printer system check in the above if statement
		// jhc 02-01-2023 - I think when this worked on a 1P it would go inside this IF right here, but now it's not. The IF hasn't changed
		// from 4.1.300 to 4.2.0_5.2.0.  The only thing I can think is if the way the pCurrentProductSmartPtr works changed.

		// I think this only happens when you change PLU on the fly. In any case, this does not work when changing PLUs on the fly
		// but leaving it here for one printer systems just in case. This can be removed if you get rid of two printer system check
		// in the above if statement
        if( getNewProduct() && pCurrentProductSmartPtr.get())
		{
			if (isTracing)
				Debug::trace("AutoWeighEvolWrapperMode", HobString("handleProductSmartPtr():  Sending LabelApplierSetup on PLU change"));
			if( !FpmManager::getInstance()->isWrapperTwoPrinter() )
			{
                sendLabelApplierSetup(pCurrentProductSmartPtr.get());
			}
		}
	}
}
