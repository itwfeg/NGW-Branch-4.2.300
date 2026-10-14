#include "AutoWeighWrapperMode.h"

#include "Configuration.h"
#include "LabelApplierOpenWindow.h"
#include "LabelApplierStatus.h"
#include "LabelApplierInquire.h"

#include "PrinterManager.h"
#include "PrinterStatus.h"
#include "PrinterLabelReady.h"
#include "PrinterLabelTaken.h"
#include "PrinterLabelComplete.h"

#include "WrapperManager.h"
#include "WrapperOpen.h"
#include "WrapperWrap.h"
#include "WrapperErrorInfo.h"
#include "WrapperInquire.h"
#include "WrapperControl.h"

#include "WeigherCachedStatus.h"
#include "HfcWeigherStatus.h"

#include "ProductUpdateMsg.h"
#include "ProductChangeMsg.h"
#include "ProductStatusMsg.h"

#include "ProductTransaction.h"
#include "OpModePackage.h"

#include "ServiceSpeaker.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "utilstring.h"

#include "PAL_platformheader.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AutoWeighWrapperMode::dataType = 0;
std::string AutoWeighWrapperMode::dataTypeName("AutoWeighWrapperMode"); 
#endif

AutoWeighWrapperMode::AutoWeighWrapperMode(WrapperSetWrapMode::WrapMode wrapMode,
										   WrapperIntakeMode::IntakeMode intakeMode,
										   const Util::CSmartPtr<Product> &productSmartPtr,
										   //WeigherManager *pWeigherManager,
										   //PrinterManager *pFirstPrinter,
										   //PrinterManager *pSecondPrinter,
										   //PrinterManager *pThirdPrinter,
										   //LabelApplierManager *pLabelApplierManager,
										   //WrapperManager *pWrapperManager) :	OperatingMode("tWrapAW",
										   Hobart::CEvolMsgReceiver *pWeigherManager,
										   Hobart::CEvolMsgReceiver *pFirstPrinter,
										   Hobart::CEvolMsgReceiver *pSecondPrinter,
										   Hobart::CEvolMsgReceiver *pThirdPrinter,
										   Hobart::CEvolMsgReceiver *pLabelApplierManager,
										   Hobart::CEvolMsgReceiver *pWrapperManager) :	OperatingMode("tWrapAW",
										   												   productSmartPtr,
																						   pWeigherManager,
										   												   pFirstPrinter,
										   												   pSecondPrinter,
										   												   pThirdPrinter,
																						   pLabelApplierManager,
										   												   pWrapperManager),
										   									AUTO_MaxLabels(6),
										   									MANUAL_MaxLabels(1)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	missedProductUpdates = 0;
	wrapOnlyMode = false;
	machineRunning = true;

	wrapStateMachine = awmWrapInitial;
	Debug::trace("AutoWeighAutoWeighWrapperMode.StateMachine", "wrapStateMachine: awmWrapInitial");

	laStateMachine1 = wmLaLabelTaken;
	Debug::trace("AutoWeighAutoWeighWrapperMode.StateMachine", "laStateMachine1 = wmLaLabelTaken");

	laStateMachine2 = wmLaLabelTaken;
	Debug::trace("AutoWeighAutoWeighWrapperMode.StateMachine", "laStateMachine2: wmLaLabelTaken");

	forcePreImage = false;

	modeType = UwsWeighmode;

	if (Debug::isTracing("AutoWeighWrapperMode"))
	{
		HobString msgDebug = "constructor: wrapMode = ";
		msgDebug.append(Integer::toString(wrapMode));
		msgDebug.append("  intakeMode = ").append(Integer::toString(intakeMode));
		Debug::trace("AutoWeighWrapperMode", msgDebug);
	}
	myInitialWrapMode = wrapMode;

	setIntakeMode((WrapperIntakeMode::IntakeMode) Configuration::getInstance()->getDefaultIntakeMode());
	wrapperSettings.setFilmTension((WrFilmTension) Configuration::getInstance()->getDefaultFilmTension());
	settingsInitialized = false;
	reqSettingsPending = false;
	labelPosition = LocNoneSelected;

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

	registerEvent(LabelApplier, LaStatus);
	registerEvent(LabelApplier, LaOff);
	registerEvent(LabelApplier, LaInquire);

	registerEvent(Weigher, WgStatus);
	registerEvent(Weigher, WgCachedStatus);

	registerEvent(Wrapper, WrInquire);
	registerEvent(Wrapper, WrOpen);
	registerEvent(Wrapper, WrStatus);
	registerEvent(Wrapper, WrSetWrapMode);
	registerEvent(Wrapper, WrControl);
	registerEvent(Wrapper, WrReqSettings);
	registerEvent(Wrapper, WrTest);


	if (Debug::isTracing("AutoWeighWrapperMode"))
	{
		HobString msgDebug = "activeFunctionTask: myInitialWrapMode = ";
		msgDebug.append(Integer::toString(myInitialWrapMode));
		Debug::trace("AutoWeighWrapperMode", msgDebug);
	}
	handleWrapSetMode(myInitialWrapMode);


	requestWeigherStatus();
	requestLabelApplierStatus();
	requestLabelApplierInquire();
	requestWrapperStatus();
	requestWrapperInquire();
	setWrapperMode(WrOperateMode);

	takeLabelTime = 15;
	setTakeLabelTime(FirstPrinter);
	setTakeLabelTime(SecondPrinter);
	setTakeLabelTime(ThirdPrinter);

	openWrapWindow();

	requestLabelSize();

	if (productSmartPtr.get())
	{
		Util::CSmartPtr<Product> newProductSmartPtr(productSmartPtr);
		handleProductSmartPtr(newProductSmartPtr);
	}
}


AutoWeighWrapperMode::~AutoWeighWrapperMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	deregisterAllEvents();
	
	emptyPackageMap();

	sendPrinterReset(FirstPrinter);
	sendPrinterReset(SecondPrinter);
	sendPrinterReset(ThirdPrinter);

	setLabelApplierMode(Conveyor);
	resetLabelApplier();

	WrapperIntakeMode intakeModeMsg(this, wrapperSettings.getIntakeMode().getIntakeMode());
	dispatchEvent(&intakeModeMsg);

	ShutdownNicely();

	Debug::trace("OperatingMode", HobString(tName) + HobString(" -- Going away!!!"));
}				


void AutoWeighWrapperMode::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
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
		case LabelApplier:
		{
			handleLabelApplierMessage((LaEvent) msg->getEventType(), msg->Data());
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

void AutoWeighWrapperMode::handleProductSmartPtr(Util::CSmartPtr<Product> &pCurrentProductSmartPtr)
{
	Product *pProduct = 0;
	pProduct = pCurrentProductSmartPtr.get();

	sendLabelApplierSetup(const_cast <Product *> (pProduct));
}

void AutoWeighWrapperMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
	HobString msgDebug = "";

	if (traceOn)
	{
		msgDebug.append("handleProductMessage():  productEvent = ").append(Integer::toString(productEvent));
		Debug::trace("AutoWeighWrapperMode", msgDebug);
	}
	bool debuggingOpModeProd = Debug::isTracing("OperatingMode.Product");

	switch (productEvent)
	{
		// Operator has chosen a new product
		case ProductChangeSmart:
		{
			ProductChangeMsg prodChange(recvBuffer);

			ProductManager *productManager = (ProductManager *) prodChange.getProductMgr();
			if (productManager)
			{
				pCurrentProductSmartPtr = productManager->getCurrentProductSmartPtr();
				handleProductSmartPtr(pCurrentProductSmartPtr);
			}

			break;
		}

		case ProductUpdate:
		{
			ProductUpdateMsg prodUpdate(recvBuffer);
			const ProductTransaction *pNextProductTransaction = const_cast<ProductTransaction *>(prodUpdate.getProductTransaction());

			if (debuggingOpModeProd)
			{
				Debug::trace("OperatingMode.Product", HobString("Product Update: ") + HobString(Hex::toString(pNextProductTransaction->getTransactionNum())));
				HobWeight packageWeight = pNextProductTransaction->getWeigherStatus()->getWeight();
				Debug::trace("OperatingMode.Product", HobString("packageWeight: ") + HobString(Integer::toString(packageWeight)));
				Debug::trace("OperatingMode.Product", HobString("machineRunning: ") + HobString(Bool::toString(machineRunning)));
				Debug::trace("OperatingMode.Product", HobString("largeMotion: ") + HobString(Bool::toString(largeMotion)));
				Debug::trace("OperatingMode.Product", HobString("packageMap.size(): ") + HobString(Integer::toString((int)packageMap.size())));
				Debug::trace("OperatingMode.Product", HobString("wrapStateMachine == awmWrapOpenWindow: ") + HobString(Bool::toString(wrapStateMachine == awmWrapOpenWindow)));
			}

			if ((curWrapState == WrapperSetWrapMode::ManualWeigh && packageMap.size() < MANUAL_MaxLabels))
			{
				if (debuggingOpModeProd)
					Debug::trace("OperatingMode.Product", string("Got ProductUpdate message - 'curWrapState' is WrapperSetWrapMode::ManualWeigh or (WrapperSetWrapMode::AutoManualApply && largeMotion)"));

				pCurrentProductTransaction = pNextProductTransaction;
				wrapPackage(pCurrentProductTransaction->getWeigherStatus()->getWeight());
			}
			else if (curWrapState == WrapperSetWrapMode::AutoManualApply)
			{
				if (packageMap.size() < MANUAL_MaxLabels && largeMotion)
				{
					if (debuggingOpModeProd)
						Debug::trace("OperatingMode.Product", "AutoManualApply - Got ProductUpdate message. 'packageMap.size()'=" +
										inttostring(packageMap.size()) + "- is less than MANUAL_MaxLabels, and largeMotion - GOING AHEAD AND CALLING wrapPackage()");

					pCurrentProductTransaction = pNextProductTransaction;
					wrapPackage(pCurrentProductTransaction->getWeigherStatus()->getWeight());
				}
				else
				{
					if (debuggingOpModeProd)
					{
						Debug::trace("OperatingMode.Product", "AutoManualApply - Got ProductUpdate message. 'packageMap.size()'=" +
										inttostring(packageMap.size()) + "- is greater than or equal to MANUAL_MaxLabels, or largeMotion is false - NOT!! CALLING wrapPackage()");
						Debug::trace("OperatingMode.Product", HobString("largeMotion: ") + HobString(Bool::toString(largeMotion)));
					}

					if (pPendingPackage)
					{
						if (debuggingOpModeProd)
							Debug::trace("OperatingMode.Product", HobString("sending complete for Product trans#: ") + HobString(Hex::toString(pPendingPackage->getTransactionNum())));
						sendProductComplete(const_cast <ProductTransaction *> (pPendingPackage), false);
					}
					pPendingPackage = pNextProductTransaction;
					if (debuggingOpModeProd)
						Debug::trace("OperatingMode.Product", HobString("pPendingPackage is now Product trans#: ") + HobString(Hex::toString(pPendingPackage->getTransactionNum())));
				}
			}
			else if ((ghostPackage == true) || (wrapOnlyMode == true) ||
				((packageMap.size() >= MANUAL_MaxLabels) && (curWrapState == WrapperSetWrapMode::ManualWeigh)))
			{
				if (debuggingOpModeProd)
					Debug::trace("OperatingMode.Product", "Ignoring ProductUpdate message. 'ghostPackage' is " + 
									booltostring(ghostPackage) + ". 'wrapOnlyMode' is " + booltostring(wrapOnlyMode) +
									". 'curWrapState' is " + inttostring(curWrapState));

				sendProductComplete(const_cast <ProductTransaction *> (pNextProductTransaction), false);
				if (pPendingPackage)
				{
					sendProductComplete(const_cast <ProductTransaction *> (pPendingPackage), false);
					pPendingPackage = NULL;
					if (debuggingOpModeProd)
						Debug::trace("OperatingMode.Product", "1 setting pPendingPackage to NULL");
				}
			}
			else if (wrapStateMachine == awmWrapError)
			{
				// redisplay the error!
				dispatchEvent(&opModeStatus);
				
				// if there is an error, the machine is not running!
				if (packageMap.size() < MANUAL_MaxLabels)
				{
					if (debuggingOpModeProd)
						Debug::trace("OperatingMode.Product", "Got ProductUpdate message. 'wrapStateMachine' is awmWrapError but packageMap.size() is" + 
										inttostring(packageMap.size()) + ", less than MANUAL_MaxLabels");

					pCurrentProductTransaction = pNextProductTransaction;
					wrapPackage(pCurrentProductTransaction->getWeigherStatus()->getWeight());
				}
				else
				{
					if (debuggingOpModeProd)
							Debug::trace("OperatingMode.Product", "Ignoring ProductUpdate message. 'wrapStateMachine' is awmWrapError and packageMap.size() is " + 
										inttostring(packageMap.size()) + ", greater or equal to MANUAL_MaxLabels");

					sendProductComplete(const_cast <ProductTransaction *> (pPendingPackage), false);
					pPendingPackage = pNextProductTransaction;
				}
			}
			else
			{
				if (packageMap.size() >= AUTO_MaxLabels)
				{
					if (debuggingOpModeProd)
						Debug::trace("OperatingMode.Product", "Ignoring ProductUpdate message. packageMap.size() is " + 
										inttostring(packageMap.size()) + ", greater or equal to MANUAL_MaxLabels");

					handlePrinterOverflow(AUTO_MaxLabels);
					sendProductComplete(const_cast <ProductTransaction *> (pNextProductTransaction), false);
				}
				else if ((wrapStateMachine == awmWrapOpenWindow) && (machineRunning || wrStateInfo.isPaused()) && largeMotion)
				{
					if (debuggingOpModeProd)
						Debug::trace("OperatingMode.Product", string("Got ProductUpdate message. ('wrapStateMachine' is awmWrapOpenWindow) and (machineRunning -") +
										booltostring(machineRunning) + string("- or wrStateInfo.isPaused() -") + booltostring(wrStateInfo.isPaused()) +
										"-) and ('curWrapState' is WrapperSetWrapMode::ManualWeigh -" + booltostring(curWrapState == WrapperSetWrapMode::ManualWeigh) +
										"- or 'largeMotion' -" + booltostring(largeMotion) + "-)");

					pCurrentProductTransaction = pNextProductTransaction;
					wrapPackage(pCurrentProductTransaction->getWeigherStatus()->getWeight());
				}
				else if ((machineRunning == false) && largeMotion)
				{
					if (packageMap.size() < MANUAL_MaxLabels)
					{
						if (debuggingOpModeProd)
							Debug::trace("OperatingMode.Product", "Got ProductUpdate message. ('machineRunning' is false) and 'largeMotion' is true and 'packageMap.size()' -" +
											inttostring(packageMap.size()) + "- is less than MANUAL_MaxLabels");

						pCurrentProductTransaction = pNextProductTransaction;
						wrapPackage(pCurrentProductTransaction->getWeigherStatus()->getWeight());
					}
					else
					{
						if (debuggingOpModeProd)
							Debug::trace("OperatingMode.Product", "Ignoring ProductUpdate message. 'machineRunning' is false and 'largeMotion' is true but packageMap.size() is " + 
											inttostring(packageMap.size()) + ", greater or equal to MANUAL_MaxLabels");

						sendProductComplete(const_cast <ProductTransaction *> (pPendingPackage), false);
						pPendingPackage = pNextProductTransaction;
					}
				}
				else
				{
					if (debuggingOpModeProd)
						Debug::trace("OperatingMode.Product", "Ignoring ProductUpdate message. unknown condition. 'machineRunning' is " +
										booltostring(machineRunning) + ". 'largeMotion' is " + booltostring(largeMotion) +
										" 'wrapStateMachine == awmWrapOpenWindow' is " + booltostring(wrapStateMachine == awmWrapOpenWindow) +
										" and 'curWrapState' is WrapperSetWrapMode::ManualWeigh' is " + booltostring(curWrapState == WrapperSetWrapMode::ManualWeigh) + ".");

					sendProductComplete(const_cast <ProductTransaction *> (pNextProductTransaction), false);
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

void AutoWeighWrapperMode::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
	HobString msgDebug = "";

	if (traceOn)
	{
		msgDebug.append("handlePrinterMessage():  ");

		HobString tempStr = "  prEvent = ";
		tempStr.append(Integer::toString(prEvent));
		Debug::trace("AutoWeighWrapperMode", msgDebug + tempStr);
	}
	switch (prEvent)
	{
		case PrStatus: 
		{
			PrinterStatus prStatus(recvBuffer);
			PrinterType printerType = prStatus.getPrinterType();

			if (prStatus.isPriorityError() == true)
			{
				updatePrinterHeadUp(printerType);
				
				if (wrapStateMachine != awmWrapPrinterHeadUp)
				{
					resetWrapperMode();
				}
			}
			else if (prStatus.isSizeCmdComplete() == true)
			{
				updateSizeCmdComplete(printerType);
				
				if (isPrinterHeadUp() == false)
				{
					openWrapWindow();
				}
			}
			break;
		}
		case PrLabelReady: 
		{
			PrinterLabelReady prLabelReady(recvBuffer);
			PrinterType printerType = prLabelReady.getPrinterType();

			if (printerType == FirstPrinter)
			{
				laStateMachine1 = wmLaLabelReady;
				Debug::trace("AutoWeighAutoWeighWrapperMode.StateMachine", "laStateMachine1 = wmLaLabelReady");

				sendLabelReady(PrimaryPrinterStation, prLabelReady.getPackId());
			}
			else if (printerType == SecondPrinter)
			{
				laStateMachine2 = wmLaLabelReady;
				Debug::trace("AutoWeighWrapperMode.StateMachine", "laStateMachine2 = wmLaLabelReady");

				sendLabelReady(SecondaryPrinterStation, prLabelReady.getPackId());
			}
			break;
		}	
		case PrLabelTaken: 
		{
			PrinterLabelReady prLabelTaken(recvBuffer);
			PrinterType printerType = prLabelTaken.getPrinterType();

			if (printerType == FirstPrinter)
			{
				laStateMachine1 = wmLaLabelTaken;
				Debug::trace("AutoWeighWrapperMode.StateMachine", "laStateMachine1 = wmLaLabelTaken");


				sendLabelTaken(PrimaryPrinterStation, prLabelTaken.getPackId());
			}
			else if (printerType == SecondPrinter)
			{
				laStateMachine2 = wmLaLabelTaken;
				Debug::trace("AutoWeighWrapperMode.StateMachine", "laStateMachine2 = wmLaLabelTaken");

				sendLabelTaken(SecondaryPrinterStation, prLabelTaken.getPackId());
			}
			break;
		}	
		case PrLabelComplete: 
		{
			PrinterLabelComplete prLabelComplete(recvBuffer);
			if (updateProductComplete(prLabelComplete) == true)
			{
				laStateMachine1 = wmLaLabelTaken;
				Debug::trace("AutoWeighWrapperMode.StateMachine", "laStateMachine1 = wmLaLabelTaken");

				laStateMachine2 = wmLaLabelTaken;
				Debug::trace("AutoWeighWrapperMode.StateMachine", "laStateMachine2 = wmLaLabelTaken");

/*
				if ((packageMap.empty() == true) && (wrapStateMachine == awmWrapChangingProduct))
				{
					Debug::trace("AutoWeighWrapperMode.StateMachine", "PrLabelComplete in awmWrapChangingProduct state");

					if (getNewProduct() == true)
					{
						changeProduct();
						preImagePrinters();
					}

					openWrapWindow();
				}
				else
					unlockWeigher();
*/

				if (machineRunning == false)
				{
					unlockWeigher();

					if (pPendingPackage != NULL)
					{
						Debug::trace("OperatingMode.Product", "pPendingPackage isn't null in PrLabelComplete");
						pCurrentProductTransaction = pPendingPackage;
						pPendingPackage = NULL;
						wrapPackage(pCurrentProductTransaction->getWeigherStatus()->getWeight());
						largeMotion = false;
					}
				}
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					HobString tempStr = "PrLabelComplete:  machineRunning = ";
					tempStr.append(Bool::toString(machineRunning));
					Debug::trace("AutoWeighWrapperMode", msgDebug2 + tempStr);

					tempStr = "pCurrentProductTransaction = ";
					tempStr.append(Hex::toString((long long) pCurrentProductTransaction));
					Debug::trace("AutoWeighWrapperMode", msgDebug2 + tempStr);

					tempStr = "pPendingPackage = ";
					tempStr.append(Hex::toString((long long) pPendingPackage));
					Debug::trace("AutoWeighWrapperMode", msgDebug2 + tempStr);
				}
			}
			break;
		}	

		case PrConfig:
			Debug::trace("AutoWeighWrapperMode", "handlePrinterMessage(PrConfig)");
			handlePrinterConfigInOpMode(recvBuffer);
			break;

		default:
			OperatingMode::handlePrinterMessage(prEvent, recvBuffer);
			break;
	}
}

void AutoWeighWrapperMode::handleLabelApplierMessage(LaEvent laEvent, const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
	HobString msgDebug = "";

	if (traceOn)
	{
		msgDebug.append("handleLabelApplierMessage():  ");

		HobString tempStr = "  laEvent = ";
		tempStr.append(Integer::toString(laEvent));
		Debug::trace("AutoWeighWrapperMode", msgDebug + tempStr);
	}
	switch (laEvent)
	{
		case LaOff:
			openWrapWindow();
			break;
		case LaInquire:
			if (recvBuffer)
			{
				LabelApplierInquire laInquire(recvBuffer);
				if (laInquire.isLeftToRight())
					labelPosition = (LaPosLoc) laInquire.getMiniStationLabelPlacement();
				else if (laInquire.getMiniStationLabelPlacement() == LaLeadingEdge)
					labelPosition = LocLeft;
				else
					labelPosition = LocRight;
				if (traceOn)
					Debug::trace("AutoWeighWrapperMode", msgDebug + "LaInquire - labelPosition = " + ((labelPosition == LocRight) ? "LocRight" : (labelPosition == LocLeft) ? "LocLeft" : "LocNoneSelected"));
				return;
			}
			break;			
		default:
			OperatingMode::handleLabelApplierMessage(laEvent, recvBuffer);
			break;
	}						
}

void AutoWeighWrapperMode::handleWeigherMessage(WgEvent wgEvent, const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
	HobString msgDebug = "";

	if (traceOn)
	{
		msgDebug.append("handleWeigherMessage():  ");

		HobString tempStr = "  wgEvent = ";
		tempStr.append(Integer::toString(wgEvent));
		Debug::trace("AutoWeighWrapperMode", msgDebug + tempStr);
	}
	switch (wgEvent)
	{
		case WgStatus:
		{
			WeigherStatus wgStatus(recvBuffer);

			if (Debug::isTracing("WrapperMode.WeigherStatus"))
				wgStatus.show();

			HobWeight packageWeight = wgStatus.getWeight();
			bool nonZeroStableWeight = (wgStatus.isLargeMotion() == false) && (wgStatus.isSmallMotion() == false) && (wgStatus.isWithinZeroQuarterGrad() == false) && (packageWeight != 0L);

			if ((opModeStatus.isWrapperOverWeight() == true) && (wgStatus.isLargeMotion() || wgStatus.isZeroStableWeight() || packageWeight < MaxPackageWeight))
			{
				clearPackageOverWeight();
			}

			if (nonZeroStableWeight && (packageWeight < Configuration::getInstance()->getMinWeightToPrint()) && (wrapStateMachine == awmWrapOpenWindow) && largeMotion /* && pCurrentProduct */)
			{
				Debug::trace("WrapperMode.WeigherStatus", "Ghost Package: 'nonZeroStableWeight' is true and 'packageWeight' is less than min weight to print and 'wrapStateMachine' is awmWrapOpenWindow");

				setGhostPackage();
				ghostPackage = true;
			}
			else if (wgStatus.isPositiveNonZeroStableWeight() && largeMotion && !ghostPackage)
			{
				Debug::trace("WrapperMode.WeigherStatus", HobString("isPositiveNonZeroStableWeight and 'largeMotion' and not 'ghostpackage'   wrapOnlyMode:") + HobString(Bool::toString(wrapOnlyMode)));
				if (wrapStateMachine == awmWrapError &&
					 curWrapState != WrapperSetWrapMode::ManualWeigh &&
					 curWrapState != WrapperSetWrapMode::AutoManualApply)
				{
					Debug::trace("WrapperMode.WeigherStatus", "'wrapStateMachine' is awmWrapError");

					// redisplay the error!
					dispatchEvent(&opModeStatus);
				}

				if (wrapOnlyMode && packageWeight >= Configuration::getInstance()->getMinWeightToPrint())
					wrapPackage(packageWeight);
				else
				{
					wgStatus.setEventType(WgStatusAndPrint);
					dispatchEvent(&wgStatus);
				}
			}
			else if (wgStatus.isLargeMotion() == true)
			{
				Debug::trace("WrapperMode.WeigherStatus", "Large Motion");
				largeMotion = true;

				if ((machineRunning == false) && (pPendingPackage != NULL))
				{
					Debug::trace("WrapperMode.WeigherStatus", "sendingProductComplete(false)");

					sendProductComplete(pPendingPackage, false);
					pPendingPackage = NULL;
				}
			}
			else if (wgStatus.isZeroStableWeight() == true)
			{
				Debug::trace("WrapperMode.WeigherStatus", "'isZeroStableWeight' is true");

				if (ghostPackage == true)
				{
					Debug::trace("WrapperMode.WeigherStatus", "'ghostPackage' was true");

					clearGhostPackage();
					ghostPackage = false;
				}
				else
				{
					Debug::trace("WrapperMode.WeigherStatus", "'ghostPackage' is false");

					if (machineRunning == false)
					{
						Debug::trace("WrapperMode.WeigherStatus", "'machineRunning' is false");

						if (pCurrentProductTransaction != NULL)
						{
							Debug::trace("WrapperMode.WeigherStatus", "'pCurrentProductTransaction' exists, send wrapperAbort");

							sendWrapperAbort();
						}

						if (pPendingPackage)
						{
							Debug::trace("WrapperMode.WeigherStatus", "'pPendingPackage' is not null, sendingProductComplete(false)");

							sendProductComplete(pPendingPackage, false);
							pPendingPackage = NULL;
						}

						if (wrStateInfo.isStandBy())
						{
							Debug::trace("WrapperMode.WeigherStatus", "'wrStateInfo.isStandBy()' is true");

							clearPressStart();
						}
					}
				}
			}
			else
			{
				Debug::trace("WrapperMode.WeigherStatus", "unknown condition");

				if (ghostPackage == true)
				{
					Debug::trace("WrapperMode.WeigherStatus", "unknown condition: 'ghostPackage' is true");

					setGhostPackage();
				}
			}

			lastWeigherStatus = wgStatus;
			break;
		}

		case WgCachedStatus:
		{
			WeigherCachedStatus wgCachedStatus(recvBuffer);

			if (Debug::isTracing("WrapperMode.WeigherCachedStatus") == true)
				wgCachedStatus.show();

			if ( wgCachedStatus.isLargeMotion() )
			{
				Debug::trace("WrapperMode.WeigherStatus", "got WeigherCachedStatus with large motion");
				largeMotion = true;
			}
			break;
		}

		default:
		{
			break;
		}
	}						
}

void AutoWeighWrapperMode::handleWrapperMessage(WrEvent wrEvent, const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
	HobString msgDebug = "";

	if (traceOn)
	{
		msgDebug.append("handleWrapperMessage():  ");

		HobString tempStr = "wrEvent = ";
		tempStr.append(Integer::toString(wrEvent));
		Debug::trace("AutoWeighWrapperMode", msgDebug + tempStr);
	}
	switch (wrEvent)
	{
		case WrInquire:
			handleWrapperInquire(recvBuffer);
			break;
		case WrStatus:
			handleWrapperStatus(recvBuffer);
			break;
		case WrOpen:
		{
			unlockWeigher();
			if (wrapStateMachine == awmWrapWrap)
			{
				wrapStateMachine = awmWrapOpenWindow;
				Debug::trace("AutoWeighWrapperMode.StateMachine", "wrapStateMachine: awmWrapOpenWindow");
			}
			break;
		}
		case WrSetWrapMode:
		{
			WrapperSetWrapMode wrapState(recvBuffer);
			curWrapState = wrapState.getWrapMode();

			handleWrapSetMode(wrapState.getWrapMode());
			break;
		}
		case WrReqSettings:
			handleWrapperRequestSettings();
			break;
		case WrControl:
			handleWrapperControl(recvBuffer);
			break;
		case WrTest:
		if (machineRunning == false)
		{
			WrapperTest wrTest(recvBuffer);
			sendWrapperTest(wrTest.getWrTestCode());
			break;
		}
		default:
		{
			if (traceOn)
				Debug::trace("AutoWeighWrapperMode", msgDebug + HobString(" - Default case!"));
			break;
		}
	}						
}

void AutoWeighWrapperMode::handleWrapperStatus(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
	bool traceOnStatus = Debug::isTracing("WrapperMode.WrapperStatus");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperStatus()";
		Debug::trace("AutoWeighWrapperMode", msgDebug);
	}

	WrapperStatus wrStatus(recvBuffer);
	if (traceOn)
	{
		Debug::trace("WrapperMode.WrapperStatus", "handleWrapperStatus ----------------- Wrapper Status");
		wrStatus.show();
	}

	if ((wrStatus.isPeripheralComm() == true) && (wrStatus.isManagerDeleted() == false))
	{
		wrStateInfo = wrStatus.getStatusInfo().getStateInfo();
		WrapperStateInfo lastStateInfo(lastStatusMsg.getStatusInfo().getStateInfo());
		WrapperErrorInfo::WrapperErrorSource errorSource = wrStatus.getErrorInfo().getErrorSource();

		if ((machineRunning == false) && (wrStateInfo.isRunning() == true))
		{
			Debug::trace("AutoWeighWrapperMode.StateMachine", "wrapper started running");
			sendLabelApplierOn();
		}

		machineRunning = wrStateInfo.isRunning();

		wrapperSettings.setHeaterInfo(wrStatus.getHeaterInfo());

		if (errorSource != opModeStatus.getWrapperError() || wrStatus.getErrorInfo().isStopSwitchInterlock())
		{
			// leave the error message up until the stop switch is cycled
			if ((errorSource != WrapperErrorInfo::NoErrorSource) ||
				((errorSource == WrapperErrorInfo::NoErrorSource) && (wrStateInfo.isStandBy())))
			{
				setNewWrapperError(errorSource);

				if ((errorSource == WrapperErrorInfo::NoErrorSource) && (wrapStateMachine == awmWrapError))
				{
					Debug::trace("WrapperMode.WrapperError", "-------------------------------Wrapper Error Cleared!");

					if (pPendingPackage)
					{
						sendWrapperAbort();

						// restart the system because this is bad!
						invalidatePackage(pCurrentProductTransaction);

						// make the pending one the current one!
						pCurrentProductTransaction = pPendingPackage;
						pPendingPackage = NULL;

						wrapPackage(pCurrentProductTransaction->getWeigherStatus()->getWeight());
					}
					else
					{
						wrapStateMachine = awmWrapOpenWindow;
						Debug::trace("AutoWeighWrapperMode.StateMachine", "wrapStateMachine: awmWrapOpenWindow");
					}
				}
			}
		}

		if (errorSource == WrapperErrorInfo::NoErrorSource)
		{
			static int lastPackageReadyId = -1;

			bool bRequestWrapperStatus = false;
			int newPackageReadyId = -1;
			WrapperPackageInfo sealerBeltPackageInfo(wrStatus.getPackageInfo((UINT16) SealerBeltPosition));
			WrapperPackageInfo dischargePackageInfo(wrStatus.getPackageInfo((UINT16) DischargePosition));

			if ( dischargePackageInfo.isPresent() && (int)dischargePackageInfo.getPackageId() != lastPackageReadyId )
			{
				newPackageReadyId = dischargePackageInfo.getPackageId();  // Emergency condition
				bRequestWrapperStatus = true;
				Debug::trace("WrapperMode.PackageReady", msgDebug + "Using DischargePackageInfo to get packId (" +
						inttostring(newPackageReadyId) + ") for PackageReady message");
			}
			else if ( sealerBeltPackageInfo.isPresent() && (int)sealerBeltPackageInfo.getPackageId() != lastPackageReadyId )
			{
				newPackageReadyId = sealerBeltPackageInfo.getPackageId(); // Normal condition
				Debug::trace("WrapperMode.PackageReady", msgDebug + "Using SealerBeltPackageInfo to get packId (" +
						inttostring(newPackageReadyId) + ") for PackageReady message");
			}


//			WrapperPackageInfo thisPackageInfo(wrStatus.getPackageInfo((UINT16) SealerBeltPosition));
//			WrapperPackageInfo lastPackageInfo(lastStatusMsg.getPackageInfo((UINT16) SealerBeltPosition));

			if (traceOnStatus)
				Debug::trace("WrapperMode.WrapperStatus", "No error in status");

//			if ((thisPackageInfo.isPresent() == true) &&
//				((lastPackageInfo.isPresent() == false) || (thisPackageInfo.getPackageId() != lastPackageInfo.getPackageId())))
			if ( newPackageReadyId != -1 )
			{
				const OpModePackage *pPackage = getPackage((UINT16)newPackageReadyId);

				if (pPackage && pPackage->newLabelApplierSetup(laSetup) == true)
				{
					if (traceOnStatus)
						Debug::trace("WrapperMode.WrapperStatus", "Going to send LabelApplierSetup message");

					//sendLabelApplierSetup(*(pPackage->getProductTransaction()->getProduct()));
				}
				else
				{
					if (traceOnStatus)
					{
						HobString msgDebug2 = "Package Id: ";
						//msgDebug2.append(inttostring(thisPackageInfo.getPackageId()));
						//msgDebug2.append("  produced package ").append(ptrtostring(pPackage));
						Debug::trace("WrapperMode.WrapperStatus", msgDebug2);
					laSetup.show();
				}
				}
				
				sendPackageReady((UINT8)newPackageReadyId);

				lastPackageReadyId = newPackageReadyId;

				if ( bRequestWrapperStatus )
					requestWrapperStatus();
			}
			else
			{
				if (traceOnStatus)
				{
					HobString msgDebug2 = "Status' package info reports  '";
					//msgDebug2.append(thisPackageInfo.isPresent() ? "present" : "not present");
					//msgDebug2.append("', while last package was '").append(lastPackageInfo.isPresent() ? "present" : "not present").append("'.");
					Debug::trace("WrapperMode.WrapperStatus", msgDebug2);
				}
			}

			if (((wrStateInfo.isIdle() == true) && (lastStateInfo.isIdle() == false)) || 
				((wrStateInfo.isPaused() == true) && (lastStateInfo.isPaused() == false)))
			{
				resetWrapper();
			}

			if ((wrStateInfo.isStandBy() == true) && (packageMap.size() >= 1))
			{
				setPressStart();
			}

			if (pCurrentProduct && wrStatus.getPackageInfo((UINT16) IntakePosition).isGhostPackage())
			{
				UINT16 ghostPackId = (UINT16)wrStatus.getPackageInfo((UINT16) IntakePosition).getPackageId();
				Debug::trace("WrapperMode.GhostPackage", "packageId: " + HobString(Integer::toString(ghostPackId)));

				deletePrinterPackage(FirstPrinter, ghostPackId);
				deletePrinterPackage(SecondPrinter, ghostPackId);

				if (lastWeigherStatus.isZeroStableWeight() == false)
				{
					setGhostPackage();
					ghostPackage = true;
				}
			}

			if (wrStateInfo.isRunning() == true && opModeStatus.isWrapperPressStart())
				clearPressStart();

			if (lastStateInfo.isStandBy() == true)
			{
				if (wrStateInfo.isRunning() == true)
				{
					if (pCurrentProductTransaction)
					{
						sendLabelApplierOn();
						setLabelApplierMode(Auto);

						if (pPendingPackage)
						{
							sendWrapperAbort();

							// restart the system because this is bad!
							invalidatePackage(pCurrentProductTransaction);

							// make the pending one the current one!
							pCurrentProductTransaction = pPendingPackage;
							pPendingPackage = NULL;

							wrapPackage(pCurrentProductTransaction->getWeigherStatus()->getWeight());
						}
					}
				}
			}
		}
		else
		{
			if (wrapStateMachine == awmWrapError)
			{
				Debug::trace("WrapperMode.WrapperError", "-------------------------------Wrapper Error already detected!");
			}
			else
			{
				Debug::trace("WrapperMode.WrapperError", "-------------------------------Wrapper Error Detected!");

				resetWrapper();

				wrapStateMachine = awmWrapError;
				Debug::trace("AutoWeighWrapperMode.StateMachine", "wrapStateMachine: awmWrapError");

				emptyPackageMap();
				sendPrinterReset(FirstPrinter);
				sendPrinterReset(SecondPrinter);
			}		
		}
	}

	lastStatusMsg = wrStatus;
}

void AutoWeighWrapperMode::wrapPackage(HobWeight packageWeight)
{
	bool traceOn = Debug::isTracing("WrapperMode.Wrap");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "wrapPackage():    ";
		HobString numStr;
		if (pCurrentProductTransaction)
			Debug::trace("WrapperMode.Wrap", msgDebug + "transactionNum: 0x" + HobString(Hex::toString(pCurrentProductTransaction->getTransactionNum())));
		else
			Debug::trace("WrapperMode.Wrap", msgDebug + "No transactionNum. pCurrentProductTransaction is 0.");
		Debug::trace("WrapperMode.Wrap", msgDebug + "  curWrapState: " + inttostring(curWrapState));
		Debug::trace("WrapperMode.Wrap", msgDebug + "   wrStateInfo: " + inttostring(wrStateInfo.getWrapperState()));
	}

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
			msgDebug2.append(inttostring((int)packageWeight)).append(" >= ");
			msgDebug2.append(inttostring((int)getMaxAWSPackageWeight())).append(")");
			Debug::trace("WrapperMode.Wrap", msgDebug);
		}
		setPackageOverWeight();
	}
	else
	{
		if (wrStateInfo.isStandBy() == true)
			setPressStart();
		else if (wrStateInfo.isPaused() == true)
		{
			if (traceOn)
			{
				Debug::trace("WrapperMode.Wrap", msgDebug + "  wrStateInfo.isIdle() = " + Bool::toString(wrStateInfo.isIdle()));
				Debug::trace("WrapperMode.Wrap", msgDebug + "wrStateInfo.isPaused() = " + Bool::toString(wrStateInfo.isPaused()));
			}
			sendLabelApplierOn();
			setLabelApplierMode(Auto);
		}
		if (traceOn)
			Debug::trace("WrapperMode.Wrap", msgDebug + "Calling sendWrapPackage()");
		sendWrapPackage(wrapperSettings.getIntakeMode().getIntakeMode(), packId, wrapProduct, (UINT16)packageWeight);
	}

	if (wrapOnlyMode == true)
	{
		incPackId();
		if (Debug::isTracing("AutoWeighWrapperMode"))
		{
			Debug::trace("AutoWeighWrapperMode", "************   Wrap Only - show laSetup  **********");
			laSetup.show();
			Debug::trace("AutoWeighWrapperMode", "***************************************************");
		}
	}
	else
	{
		if (forcePreImage && pCurrentProductTransaction)
		{
			pCurrentProductTransaction->setForcePreImage(forcePreImage);
			forcePreImage = false;
		}
		generateNewPackage2(pCurrentProductTransaction);	
	}
	largeMotion = false;
}

void AutoWeighWrapperMode::firstPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	Debug::trace("AutoWeighWrapperMode", "firstPrinterAttached()");

	_Mybase::firstPrinterAttached(pPrinterMgr);

	openWrapWindow();
}

void AutoWeighWrapperMode::firstPrinterDetached()
{
	Debug::trace("AutoWeighWrapperMode", "firstPrinterDetached()");
	_Mybase::firstPrinterDetached();

	resetWrapperMode();
}

void AutoWeighWrapperMode::secondPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	Debug::trace("AutoWeighWrapperMode", "secondPrinterAttached()");

	_Mybase::secondPrinterAttached(pPrinterMgr);

	openWrapWindow();
}

void AutoWeighWrapperMode::secondPrinterDetached()
{
	Debug::trace("AutoWeighWrapperMode", "secondPrinterDetached()");

	_Mybase::secondPrinterDetached();

	resetWrapperMode();
}

void AutoWeighWrapperMode::thirdPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	Debug::trace("AutoWeighWrapperMode", "thirdPrinterAttached()");

	_Mybase::thirdPrinterAttached(pPrinterMgr);

	openWrapWindow();
}

void AutoWeighWrapperMode::thirdPrinterDetached()
{
	Debug::trace("AutoWeighWrapperMode", "thirdPrinterDetached()");

	_Mybase::thirdPrinterDetached();

	resetWrapperMode();
}

void AutoWeighWrapperMode::wrapperAttached(Hobart::CEvolMsgReceiver *pWrapperManager, WrapperSetWrapMode::WrapMode wrapMode)
{
	Debug::trace("AutoWeighWrapperMode", "wrapperAttached()");

	_Mybase::wrapperAttached( pWrapperManager );

	if (curWrapState != wrapMode)
	{
		handleWrapSetMode(wrapMode);
	}

	if (pWeigherManager != NULL)
	{
		setWrapperMode(WrOperateMode);
		requestWrapperStatus();
		Debug::trace("AutoWeighWrapperMode", "Requesting a WrapperInquire.");
		requestWrapperInquire();
	}
}

void AutoWeighWrapperMode::wrapperDetached()
{
	Debug::trace("AutoWeighWrapperMode", "wrapperDetached()");

	_Mybase::wrapperDetached();

	resetWrapperMode();
	setNewWrapperError(WrapperErrorInfo::NoErrorSource);
}

void AutoWeighWrapperMode::labelApplierAttached(Hobart::CEvolMsgReceiver *pLabelApplierManager)
{
	Debug::trace("AutoWeighWrapperMode", "labelApplierAttached()");

	_Mybase::labelApplierAttached( pLabelApplierManager );

	setLabelApplierMode(Auto);
	setWrapperMode(WrOperateMode);


	openWrapWindow();
}

void AutoWeighWrapperMode::labelApplierDetached()
{
	Debug::trace("AutoWeighWrapperMode", "labelApplierDetached()");

	_Mybase::labelApplierDetached();
	resetWrapperMode();
}

void AutoWeighWrapperMode::weigherAttached(Hobart::CEvolMsgReceiver *pWeigherManager)
{
	Debug::trace("AutoWeighWrapperMode", "weigherAttached()");

	_Mybase::weigherAttached( pWeigherManager );
	setWeigherMode(ScalePrepackMode);

	if (pWrapperManager != NULL)
	{
		setWrapperMode(WrOperateMode);
	}
}

void AutoWeighWrapperMode::weigherDetached()
{
	Debug::trace("AutoWeighWrapperMode", "weigherDetached()");

	_Mybase::weigherDetached();

	resetWrapperMode();
}

void AutoWeighWrapperMode::changeProduct()
{
	Debug::trace("AutoWeighWrapperMode", "changeProduct()");

	OperatingMode::changeProduct();

	if (pCurrentProduct)
	{
		setIntakeMode(pCurrentProduct->getIntakeMode());
	}
}

void AutoWeighWrapperMode::openWrapWindow()
{
	Debug::trace("AutoWeighWrapperMode", "openWrapWindow()");

	if (wrapStateMachine != awmWrapError)
	{
		unlockWeigher();
		wrapStateMachine = awmWrapOpenWindow;
		Debug::trace("AutoWeighWrapperMode.StateMachine", "wrapStateMachine: awmWrapOpenWindow");
	}
}

void AutoWeighWrapperMode::sendWrapPackage(WrapperIntakeMode::IntakeMode intakeMode, UINT8 packageId, bool pullFilm, UINT16 packageWeight,	WrPackCode packCode)
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
	HobString msgDebug = "";

	if (traceOn)
	{
		msgDebug = "sendWrapPackage():  ";

		HobString msgDebug2 = msgDebug;
		msgDebug2.append("packageId = ").append(Integer::toString(packageId));
		msgDebug2.append(", pullFilm = ").append(Bool::toString(pullFilm));
		Debug::trace("AutoWeighWrapperMode", msgDebug2);
	}

	WrapperWrap wrWrap(this, intakeMode, packageId, pullFilm, packageWeight, packCode);
	dispatchEvent(&wrWrap);

	wrapStateMachine = awmWrapWrap;
	Debug::trace("AutoWeighWrapperMode.StateMachine", "wrapStateMachine: awmWrapWrap");
	lockWeigher();
}

void AutoWeighWrapperMode::handleWrapperInquire(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperInquire()";
		Debug::trace("AutoWeighWrapperMode", msgDebug);
		msgDebug.append(":  ");
	}
	if (!recvBuffer)
	{
		if (traceOn)
		{
			HobString msgDebug2 = "recvBuffer is 0. Exiting!!!";
			Debug::trace("AutoWeighWrapperMode", msgDebug + msgDebug2);
		}
		return;
	}

	WrapperInquire wrInquire(recvBuffer);

	wrapperSettings.getHeaterInfo().setCelsius(wrInquire.isCelsiusEnable());
	wrapperSettings.getHeaterInfo().setSetting(wrInquire.getHeaterSetting());
	wrapperSettings.getHeaterInfo().setState(wrInquire.getHeaterEnable());
	wrapperSettings.setFilmWidth(wrInquire.getPackWidth());
	settingsInitialized = true;

	if (wrInquire.getFilmTension() != wrapperSettings.getFilmTension())
	{
		if (traceOn)
			Debug::trace("AutoWeighWrapperMode", msgDebug + "   Sending a WrapperControl msg to RTC because the FilmTension is different!");

		if (!pWrapperManager)
		{
			if (traceOn)
			{
				HobString msgDebug2 = "   pWrapperManager is NULL!";
				Debug::trace("AutoWeighWrapperMode", msgDebug + msgDebug2);
			}
		}
		else
		{
			WrapperControl wrControl(this);

			wrControl.setFilmWidth(wrapperSettings.getFilmWidth());
			wrControl.setFilmTension(wrapperSettings.getFilmTension());
			wrControl.setHeaterSetting(wrapperSettings.getHeaterInfo().getSetting());
			wrControl.setHeaterEnable(wrapperSettings.getHeaterInfo().isState());
			wrControl.setIntakeMode(wrapperSettings.getIntakeMode().getIntakeMode());

			const char *pCC = wrControl.serialize();
			pWrapperManager->send(pCC, wrControl.getSerializedLength());
		}
	}
	if (traceOn)
	{
		Debug::trace("AutoWeighWrapperMode", msgDebug + "   ****  wrapperSettings.show()  ****");
		wrapperSettings.show();
		Debug::trace("AutoWeighWrapperMode", msgDebug + "   **********************************");
	}
	if (reqSettingsPending)
	{
		if (traceOn)
		{
			msgDebug.append("reqSettingsPending - dispatchEvent WrapperSettings.");
			Debug::trace("AutoWeighWrapperMode", msgDebug);
		}
		dispatchEvent(&wrapperSettings);
		reqSettingsPending = false;
	}
}

void AutoWeighWrapperMode::handleWrapperControl(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperControl()";
		Debug::trace("AutoWeighWrapperMode", msgDebug);
		msgDebug.append(":  ");
	}
	if (!recvBuffer)
	{
		if (traceOn)
		{
			HobString msgDebug2 = "recvBuffer is 0. Exiting!!!";
			Debug::trace("AutoWeighWrapperMode", msgDebug + msgDebug2);
		}
		return;
	}

	WrapperControl wrControl(recvBuffer);
	wrControl.setSourceObject(this);

	if (pWrapperManager != NULL)
	{
		const char *pCC = wrControl.serialize();
		pWrapperManager->send(pCC, wrControl.getSerializedLength());
	}

	setIntakeMode(wrControl.getIntakeMode());
	wrapperSettings.setFilmTension(wrControl.getFilmTension());
	wrapperSettings.setFilmWidth(wrControl.getFilmWidth());
	wrapperSettings.getHeaterInfo().setSetting(wrControl.getHeaterSetting());
	wrapperSettings.getHeaterInfo().setState(wrControl.isHeaterEnable());
	if (traceOn)
	{
		Debug::trace("AutoWeighWrapperMode", msgDebug + "wrapperSettings.show()  ********");
		wrapperSettings.show();
		Debug::trace("AutoWeighWrapperMode", msgDebug + "********************************");
	}
}

void AutoWeighWrapperMode::setIntakeMode(WrapperIntakeMode::IntakeMode intakeMode)
{
	Debug::trace("AutoWeighWrapperMode", "setIntakeMode()");

	if (intakeMode == WrapperIntakeMode::DefaultIntake)
	{
		intakeMode = (WrapperIntakeMode::IntakeMode) Configuration::getInstance()->getDefaultIntakeMode();
	}
	else if (intakeMode == WrapperIntakeMode::FastIntake)
	{
		intakeMode = WrapperIntakeMode::NormalIntake;
	}
	wrapperSettings.setIntakeMode(intakeMode);
}

void AutoWeighWrapperMode::handleWrapSetMode(WrapperSetWrapMode::WrapMode wrapState)
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
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
		Debug::trace("AutoWeighWrapperMode", msgDebug2);
	}

	curWrapState = wrapState;
	wrapProduct = (wrapState == WrapperSetWrapMode::WrapOnly) || (wrapState == WrapperSetWrapMode::WrapOn);
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("wrapProduct = ").append(Bool::toString(wrapProduct));
		Debug::trace("AutoWeighWrapperMode", msgDebug2);
	}

	switch (wrapState)
	{
		case WrapperSetWrapMode::WrapOn:
		case WrapperSetWrapMode::WrapOff:
		{
			setWeigherMode(ScalePrepackMode);
			setLabelApplierMode(Auto);
			wrapOnlyMode = false;
			break;
		}
		case WrapperSetWrapMode::WrapOnly:
		{
			setWeigherMode(ScalePrepackMode);
			setLabelApplierMode(Conveyor);
			wrapOnlyMode = true;
			break;
		}
		case WrapperSetWrapMode::AutoManualApply:
		{
			setWeigherMode(ScaleManualMode);
			setLabelApplierMode(Conveyor);
			break;
		}
		case WrapperSetWrapMode::UnknownMode:
		case WrapperSetWrapMode::ManualWeigh:
		default:
		{
			setWeigherMode(ScaleManualMode);
			setLabelApplierMode(Conveyor);
			wrapOnlyMode = false;
			break;
		}
	}
}

void AutoWeighWrapperMode::handleWrapperRequestSettings()
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "handleWrapperRequestSettings()";
		Debug::trace("AutoWeighWrapperMode", msgDebug);
		msgDebug.append(":   ");
	}

	if (settingsInitialized)
	{
		if (traceOn)
		{
			msgDebug.append("dispatchEvent WrapperSettings.");
			Debug::trace("AutoWeighWrapperMode", msgDebug);
		}
		dispatchEvent(&wrapperSettings);
		reqSettingsPending = false;
	}
	else
	{
		if (traceOn)
		{
			msgDebug.append("WrapperSettings not initialized! Can't send anything yet!!!");
			Debug::trace("AutoWeighWrapperMode", msgDebug);
		}
		reqSettingsPending = true;
	}
}

void AutoWeighWrapperMode::sendLabelApplierSetup(Product *curProduct)
{
	bool traceOn = Debug::isTracing("AutoWeighWrapperMode");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "sendLabelApplierSetup()";
		Debug::trace("AutoWeighWrapperMode", msgDebug);
		msgDebug.append(":  ");
	}
	if (!curProduct)
	{
		Debug::trace("AutoWeighWrapperMode", msgDebug + "curProduct is 0!  Can't send a LabelApplierSetup msg!");
	}
	else
	{
		laSetup.setPrimaryLabelRotationDegrees((UINT16) curProduct->getLabelRotation(FirstPrinter));
		laSetup.setPrimaryLabelPlacement(labelPosition);

		if (laSetup.getSecondaryLabelSize() > 0)
		{
			laSetup.setSecondaryLabelRotationDegrees((UINT16) curProduct->getLabelRotation(SecondPrinter));
			laSetup.setSecondaryLabelPlacement((labelPosition == LocRight) ? LocLeft : LocRight);
		}

		if (traceOn)
		{
			Debug::trace("AutoWeighWrapperMode", msgDebug + "**********************  laSetup.show()");
			laSetup.show();
			Debug::trace("AutoWeighWrapperMode", msgDebug + "**************************************");
		}
		Debug::trace("LabelApplierManager.Message", " sendLabelApplierSetup()");
		dispatchEvent(&laSetup, this);

		PAL_absclass::PAL_sleep(0);
	}
}
