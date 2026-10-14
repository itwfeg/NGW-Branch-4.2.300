#if 0
#define EXTRA_DEBUG
#endif

#include "OpModeEvent.h"
#include "AutoWeighMode.h"

#include "Configuration.h"

#include "WeigherStatus.h"
#include "WeigherRequestStatus.h"
#include "WeigherCachedStatus.h"

#include "LabelApplierUltimaAutoMode.h"
#include "LabelApplierOpenWindow.h"
#include "LabelApplierStatus.h"
#include "LabelApplierCloseWindow.h"

#include "PrinterManager.h"
#include "PrinterStatus.h"
#include "PrinterLabelReady.h"
#include "PrinterLabelTaken.h"
#include "PrinterLabelComplete.h"

#include "ProductUpdateMsg.h"
#include "ProductChangeMsg.h"

#include "ProductManager.h"
#include "ProductTransaction.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"
#include "Integer.h"
#include "Bool.h"

#include "CLAPackageDischarge.h"//Alex Zavala 1/3/14
#include "OpModePackage.h"//Alex Zavala 1/3/14

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int AutoWeighMode::_dataType_ = 0;
	std::string AutoWeighMode::_dataTypeName_("AutoWeighMode"); 
#endif

AutoWeighMode::AutoWeighMode(const Util::CSmartPtr<Product> &productSmartPtr,
							 //WeigherManager *pWeigherManager,
							 //PrinterManager *pFirstPrinter,
							 //PrinterManager *pSecondPrinter,
							 //PrinterManager *pThirdPrinter,
							 //LabelApplierManager *pLabelApplierManager,
							 //WrapperManager *pWrapperManager) :	OperatingMode("tAutoWeigh",
							 Hobart::CEvolMsgReceiver *pWeigherManager,
							 Hobart::CEvolMsgReceiver *pFirstPrinter,
							 Hobart::CEvolMsgReceiver *pSecondPrinter,
							 Hobart::CEvolMsgReceiver *pThirdPrinter,
							 Hobart::CEvolMsgReceiver *pLabelApplierManager,
							 Hobart::CEvolMsgReceiver *pWrapperManager) :	OperatingMode("tAutoWeigh",
																			  productSmartPtr,
																			  pWeigherManager,
																			  pFirstPrinter,
																			  pSecondPrinter,
																			  pThirdPrinter,
																			  pLabelApplierManager,
																			  pWrapperManager),
														   		AUTO_MaxLabels(3),
																MANUAL_MaxLabels(1)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("AutoWeighMode", "Constructor begin ********");
	registerEvent(ProductDev, ProductChangeSmart);
	registerEvent(ProductDev, ProductUpdate);

	registerEvent(Printer, PrStatus);
	registerEvent(Printer, PrLabelReady);
	registerEvent(Printer, PrLabelTaken);
	registerEvent(Printer, PrLabelComplete);
	registerEvent(Printer, PrConfig);

	registerEvent(LabelApplier, LaStatus);
	registerEvent(LabelApplier, LaOpenWindow);
	registerEvent(LabelApplier, LaOff);
	registerEvent(LabelApplier, LaUltimaAutoMode);
	registerEvent(LabelApplier, LaPackageDischarge);//Alex Zavala 1/3/14

	registerEvent(Weigher, WgStatus);

	setWeigherMode(ScalePrepackMode);
	requestWeigherStatus();

	setLabelApplierMode(Auto);
	requestLabelApplierStatus();

	wgStateMachine = awmWgInitial;
	Debug::trace("AutoWeighMode.StateMachine", "AutoWeighMode():  wgStateMachine: awmWgInitial");

	laStateMachine1 = wmLaInitial;
	Debug::trace("AutoWeighMode.StateMachine", "AutoWeighMode():  laStateMachine1: wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("AutoWeighMode.StateMachine", "AutoWeighMode():  laStateMachine2: wmLaInitial");

	laStateMachine3 = wmLaInitial;
	Debug::trace("AutoWeighMode.StateMachine", "AutoWeighMode():  laStateMachine3: wmLaInitial");

	Debug::trace("AutoWeighMode", "AutoWeighMode():  Setting autoManualWeigh = false.");
	autoManualWeigh = false;
	missedProductUpdates = 0;
	checkingForErrors = false;//Alex Zavala 12/16

	lastOpenWindowPackId = 0;
	pkgDischargeError = MaxPrinterTypes;

	takeLabelTime = 5;
	setTakeLabelTime(FirstPrinter);
	setTakeLabelTime(SecondPrinter);
	setTakeLabelTime(ThirdPrinter);

	requestLabelSize();

	if (productSmartPtr.get())
	{
		Util::CSmartPtr<Product> newProductSmartPtr(productSmartPtr);
		handleProductSmartPtr(newProductSmartPtr);
	}
	Debug::trace("AutoWeighMode", "******** Constructor done");
}


AutoWeighMode::~AutoWeighMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
	deregisterAllEvents();

	setLabelApplierMode(Conveyor);

	ShutdownNicely();
	//	resetLabelApplier();
	Debug::trace("AutoWeighMode", "Destructor. Bye!");
}				


void AutoWeighMode::handleProductSmartPtr(Util::CSmartPtr<Product> &pCurrentProductSmartPtr)
{
	bool isTracing = Debug::isTracing("AutoWeighMode.ChangeProduct");

	Product *pProduct = 0;
	pProduct = pCurrentProductSmartPtr.get();

#ifdef EXTRA_DEBUG
	Debug::trace("AutoWeighMode", "handleProductSmartPtr():  Currently packId = " + HobString(Integer::toString(packId)));
#endif
	// Only push the new product onto the product queue if it isn't NULL.
	// Can get a NULL when UI looks up product 0 - ie. clearing the product.
	if (pProduct == NULL)
		pCurrentProduct = 0;		//	AWL-113
	else
	{
		if (opModeStatus.isProductRequiredToWrap())
		{
			Debug::trace("AutoWeighMode", HobString("handleProductSmartPtr():  Clearing ProductRequiredToWrap message"));
			opModeStatus.clearProductRequiredToWrap();
			dispatchEvent(&opModeStatus);
		}

		productQ.push(pCurrentProductSmartPtr);

		wgStateMachine = awmWgChangingProduct;
		Debug::trace("AutoWeighMode.StateMachine", "handleProductSmartPtr():  wgStateMachine: awmWgChangingProduct");

		if (isTracing)
			Debug::trace("AutoWeighMode.ChangeProduct", HobString("ProductChange - packageMap size #2 is") + HobString(Integer::toString((int)packageMap.size())));

		if (pPendingPackage != NULL)
		{
			if (isTracing)
				Debug::trace("AutoWeighMode.ChangeProduct", HobString("ProductChange - pPendingPackage isn't NULL : 0x") + Hex::toString((long long) pPendingPackage));
			sendProductComplete((const ProductTransaction *) pPendingPackage, false);
			pPendingPackage = NULL;
		}

		if (packageMap.empty() && getNewProduct())
		{
			if (isTracing)
				Debug::trace("AutoWeighMode.ChangeProduct", HobString("ProductChange - packageMap is empty and getNewProduct()"));
			changeProduct();
			preImagePrinters();
			unlockWeigher();
			//openWindow();

			laStateMachine1 = wmLaInitial;
			Debug::trace("AutoWeighMode.StateMachine", "handleProductSmartPtr():  laStateMachine1: wmLaInitial");

			laStateMachine2 = wmLaInitial;
			Debug::trace("AutoWeighMode.StateMachine", "handleProductSmartPtr():  laStateMachine2: wmLaInitial");
		}
	}
}

void AutoWeighMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
#ifdef EXTRA_DEBUG
	Debug::trace("AutoWeighMode", "handleProductMessage():  Currently packId = " + HobString(Integer::toString(packId)));
	Debug::trace("AutoWeighMode.StateMachine", "handleProductMessage():    laStateMachine1 = " + HobString(Integer::toString(laStateMachine1)));
	Debug::trace("AutoWeighMode.StateMachine", "handleProductMessage():     wgStateMachine = " + HobString(Integer::toString(wgStateMachine)));
	Debug::trace("AutoWeighMode.StateMachine", "handleProductMessage():  pkgDischargeError = " + HobString(Integer::toString(pkgDischargeError)));
#endif
	switch (productEvent)
	{
		// Operator has chosen a new product
		case ProductChangeSmart:
		{
			Debug::trace("AutoWeighMode", "handleProductMessage(ProductChangeSmart)");
			ProductChangeMsg prodChange(recvBuffer);

// removed FHA 5/31/11, not used	Product *pProduct = 0;
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
			Debug::trace("AutoWeighMode", "handleProductMessage(ProductUpdate)");
			ProductUpdateMsg prodUpdate(recvBuffer);
			ProductTransaction *pNextProductTransaction = (ProductTransaction *) prodUpdate.getProductTransaction();

			Debug::trace("AutoWeighMode.Product", HobString("ProductUpdate:  pNextProductTransaction: 0x") +
												  HobString(Hex::toString((long long) pNextProductTransaction)));

			Debug::trace("AutoWeighMode.Product", HobString("ProductUpdate:  autoManualWeigh = ") + HobString(Bool::toString(autoManualWeigh)));
			Debug::trace("AutoWeighMode.Product", HobString("ProductUpdate:  	largeMotion = ") + HobString(Bool::toString(largeMotion)));
			Debug::trace("AutoWeighMode.Product", HobString("ProductUpdate:   wgStateMachine = ") + HobString(Integer::toString(wgStateMachine)));

			if (wgStateMachine == awmCLAError)
			{
				Debug::trace("AutoWeighMode.Product", "ProductUpdate:  Not labeling condition:  (wgStateMachine == awmCLAError)");

				sendProductComplete(pNextProductTransaction, false);
				if (pPendingPackage)
				{
					sendProductComplete(pPendingPackage, false);
					pPendingPackage = 0;
				}
                if( packId )
                {
                    Debug::trace("AutoWeighMode.Product", "ProductUpdate:  Received product update when in an error condition, closing the window for packId:" + HobString(Integer::toString(packId)));
                    sendLabelApplierCloseWindow(packId);
                    packId = 0;
                    Debug::trace("AutoWeighMode.Product", "ProductUpdate:  Set packId = " + HobString(Integer::toString(packId)));
                }
			}
			else if ((autoManualWeigh == true) && (largeMotion == true))
			{
				Debug::trace("AutoWeighMode.Product", "ProductUpdate:  (autoManualWeigh == true) && (largeMotion == true) - Process the package.");
				largeMotion = false;

				if (packageMap.size() < MANUAL_MaxLabels)
				{
					pCurrentProductTransaction = pNextProductTransaction;
					startNewPackage();
					unlockWeigher();
					//openWindow();
				}
				else
				{
					// store it
					if (pPendingPackage != NULL)
					{
						sendProductComplete((const ProductTransaction *) pPendingPackage, false);
					}
					pPendingPackage = pNextProductTransaction;
					Debug::trace("AutoWeighMode.Product", HobString("ProductUpdate:  pPendingPackage: 0x") + Hex::toString((long long) pPendingPackage));
				}
			}
			else if ((autoManualWeigh == false) && (packageMap.size() < AUTO_MaxLabels) && largeMotion &&
					((wgStateMachine == awmWgIdle) || (wgStateMachine == awmWgOpenWindow)))
			{
				Debug::trace("AutoWeighMode.Product", "ProductUpdate:  Process the package.");
				pCurrentProductTransaction = pNextProductTransaction;
				startNewPackage();
			}
			else
			{
				// This is an error condition!!
				if (Debug::isTracing("AutoWeighMode"))
				{
					Debug::trace("AutoWeighMode", HobString("ProductUpdate:  The comment says we have an error condition. What to do here?"));
					Debug::trace("AutoWeighMode", HobString("ProductUpdate:  Current state: ")+ HobString(Integer::toString(wgStateMachine)) );
					Debug::trace("AutoWeighMode", HobString("ProductUpdate:  autoManualWeigh: ")+ HobString(Bool::toString(autoManualWeigh)) );
					Debug::trace("AutoWeighMode", HobString("ProductUpdate:  largeMotion: ")+ HobString(Bool::toString(largeMotion)) );
					Debug::trace("AutoWeighMode", HobString("ProductUpdate:  packageMap.size() < AUTO_MaxLabels: ")+ HobString(Bool::toString(packageMap.size() < AUTO_MaxLabels)) );
				}
				Debug::trace("AutoWeighMode.Product", HobString("ProductUpdate:  1) pNextProductTransaction: 0x") + HobString(Hex::toString((long long) pNextProductTransaction)));
				sendProductComplete(pNextProductTransaction, false);

				missedProductUpdates++;
				Debug::trace("AutoWeighMode.Error", HobString("ProductUpdate:  Missed Product Update: ") + HobString(Integer::toString(missedProductUpdates)));
				Debug::trace("AutoWeighMode.Product", HobString("ProductUpdate:  2) pNextProductTransaction: 0x") + HobString(Hex::toString((long long) pNextProductTransaction)));
			}
			break;
		}

		default:
		{
			Debug::trace("AutoWeighMode", "handleProductMessage(???)");
			break;
		}
	}					
}

void AutoWeighMode::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer)
{
#ifdef EXTRA_DEBUG
	Debug::trace("AutoWeighMode", "handlePrinterMessage():  Currently packId = " + HobString(Integer::toString(packId)));
	Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage():    laStateMachine1 = " + HobString(Integer::toString(laStateMachine1)));
	Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage():     wgStateMachine = " + HobString(Integer::toString(wgStateMachine)));
	Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage():  pkgDischargeError = " + HobString(Integer::toString(pkgDischargeError)));
#endif
	switch (prEvent)
	{
		case PrStatus: 
		{
			Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrStatus)");
			PrinterStatus prStatus(recvBuffer);

#ifdef EXTRA_DEBUG
	Debug::trace("AutoWeighMode", "Partial PrinterStatus:   ****");

	Debug::trace("AutoWeighMode", HobString("labelReady: ") +
								  HobString(Bool::toString(prStatus.isLabelReady())));
	Debug::trace("AutoWeighMode", HobString("labelTakenError: ") +
								  HobString(Bool::toString(prStatus.isLabelTaken())));
	Debug::trace("AutoWeighMode", HobString("takeLabelError: ") +
								  HobString(Bool::toString(prStatus.isTakeLabelError())));
	Debug::trace("AutoWeighMode", HobString("state: 0x") +
								  HobString(Hex::toString(prStatus.getState())));
	Debug::trace("AutoWeighMode", HobString("command: 0x") +
								  HobString(Hex::toString(prStatus.getCommand())));
	Debug::trace("AutoWeighMode", HobString("error: 0x") +
								  HobString(Hex::toString(prStatus.getError())));
	Debug::trace("AutoWeighMode", HobString("takingBit: ") +
								  HobString(Bool::toString(prStatus.isTaking())));
	Debug::trace("AutoWeighMode", HobString("labelTakenBit: ") +
								  HobString(Bool::toString(prStatus.isLabelTaken())));
	Debug::trace("AutoWeighMode", HobString("FindwoLabelTakenCmdComplete: ") +
								  HobString(Bool::toString(prStatus.isFindwoLabelTakenCmdComplete())));
#endif
			PrinterType printerType = prStatus.getPrinterType();

			if (prStatus.isHeadDisengaged() == true)
			{
				updatePrinterHeadUp(printerType);
				
				if (wgStateMachine != awmWgPrinterHeadUp)
				{
					wgStateMachine = awmWgPrinterHeadUp;
					Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrStatus):  wgStateMachine: awmWgPrinterHeadUp");

					productFailed();
				}
			}
			else if (prStatus.isSizeCmdComplete() == true)
			{
				updateSizeCmdComplete(printerType);
				
				if (isPrinterHeadUp() == false)
				{
					if ((getNewProduct() == true) || (pCurrentProduct != NULL))
					{
						changeProduct();
						preImagePrinters();
						unlockWeigher();
						//openWindow();
						Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrStatus):  sizeCmdComplete");
					}
					else
					{
						wgStateMachine = awmWgInitial;
						Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessagePrStatus():  wgStateMachine: awmWgInitial");
					}

					laStateMachine1 = wmLaInitial;
					Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrStatus):  laStateMachine1: wmLaInitial");

					laStateMachine2 = wmLaInitial;
					Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrStatus):  laStateMachine2: wmLaInitial");
				}
			}
			break;
		}
		case PrLabelReady: 
		{
#ifdef EXTRA_DEBUG
			Debug::trace("AutoWeighMode", "handlePrinterMessage(PrLabelReady):  Currently packId = " + HobString(Integer::toString(packId)));
#endif

			PrinterLabelReady prLabelReady(recvBuffer);
#ifdef EXTRA_DEBUG
			Debug::trace("AutoWeighMode", "handlePrinterMessage(PrLabelReady):  prLabelReady.getPackId() = " + HobString(Integer::toString(prLabelReady.getPackId())));
#endif
			PrinterType printerType = prLabelReady.getPrinterType();

			if (printerType == FirstPrinter)
			{
				laStateMachine1 = wmLaLabelReady;
				Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrLabelReady):  laStateMachine1 = wmLaLabelReady");

				sendLabelReady(PrimaryPrinterStation, prLabelReady.getPackId());
			}
			else if (printerType == SecondPrinter)
			{
				laStateMachine2 = wmLaLabelReady;
				Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrLabelReady):  laStateMachine2 = wmLaLabelReady");

				sendLabelReady(SecondaryPrinterStation, prLabelReady.getPackId());
			}
			break;
		}	
		case PrLabelTaken: 
		{
#ifdef EXTRA_DEBUG
			Debug::trace("AutoWeighMode", "handlePrinterMessage(PrLabelTaken):  Currently packId = " + HobString(Integer::toString(packId)));
#endif

			PrinterLabelReady prLabelTaken(recvBuffer);
#ifdef EXTRA_DEBUG
			Debug::trace("AutoWeighMode", "handlePrinterMessage(PrLabelTaken):  prLabelTaken.getPackId() = " + HobString(Integer::toString(prLabelTaken.getPackId())));
#endif
			PrinterType printerType = prLabelTaken.getPrinterType();

			if (printerType == FirstPrinter)
			{
				laStateMachine1 = wmLaLabelTaken;
				Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrLabelTaken):  laStateMachine1 = wmLaLabelTaken");

				sendLabelTaken(PrimaryPrinterStation, prLabelTaken.getPackId());

				unlockWeigher();
				//openWindow();
			}
			else if (printerType == SecondPrinter)
			{
				laStateMachine2 = wmLaLabelTaken;
				Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrLabelTaken):  laStateMachine2 = wmLaLabelTaken");

				sendLabelTaken(SecondaryPrinterStation, prLabelTaken.getPackId());
			}
			pkgDischargeError = MaxPrinterTypes;		//	HA-147
			break;
		}	
		case PrLabelComplete: 
		{
#ifdef EXTRA_DEBUG
			Debug::trace("AutoWeighMode", "handlePrinterMessage(PrLabelComplete):  Currently packId = " + HobString(Integer::toString(packId)));
#endif

			PrinterLabelComplete prLabelComplete(recvBuffer);

			if (updateProductComplete(prLabelComplete) == true)
			{
				if (packageMap.empty() && (wgStateMachine == awmWgChangingProduct))
				{
					if (getNewProduct() == true)
					{
						//openWindow();
						changeProduct();
						preImagePrinters();
						unlockWeigher();
						Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrLabelComplete):  PrLabelComplete getNewProduct=true");

						laStateMachine1 = wmLaInitial;
						Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrLabelComplete):  laStateMachine1: wmLaInitial");

						laStateMachine2 = wmLaInitial;
						Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrLabelComplete):  laStateMachine2: wmLaInitial");
					}
				}
				else
				{
					// run it
					if ((autoManualWeigh == true) && (pPendingPackage != NULL))
					{
						Debug::trace("AutoWeighMode.StateMachine", "handlePrinterMessage(PrLabelComplete):  RUNNING PENDING PKG");
						pCurrentProductTransaction = pPendingPackage;
						pPendingPackage = NULL;
						startNewPackage();
						unlockWeigher();
						//openWindow();
					}
				}
			}
			break;
		}	

		case PrConfig:
			Debug::trace("AutoWeighMode", "handlePrinterMessage(PrConfig)");
			handlePrinterConfigInOpMode(recvBuffer);
			break;

		default:
			Debug::trace("AutoWeighMode", "handlePrinterMessage(DEFAULT)");
			OperatingMode::handlePrinterMessage(prEvent, recvBuffer);
			break;
	}
}

void AutoWeighMode::handleLabelApplierMessage(LaEvent laEvent, const char *recvBuffer)
{
#ifdef EXTRA_DEBUG
	Debug::trace("AutoWeighMode", "handleLabelApplierMessage():  Currently packId = " + HobString(Integer::toString(packId)));
	Debug::trace("AutoWeighMode.StateMachine", "handleLabelApplierMessage():    laStateMachine1 = " + HobString(Integer::toString(laStateMachine1)));
	Debug::trace("AutoWeighMode.StateMachine", "handleLabelApplierMessage():     wgStateMachine = " + HobString(Integer::toString(wgStateMachine)));
	Debug::trace("AutoWeighMode.StateMachine", "handleLabelApplierMessage():  pkgDischargeError = " + HobString(Integer::toString(pkgDischargeError)));
#endif
	switch (laEvent)
	{
		case LaOff:
		{
			Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaOff):  Setting autoManualWeigh = true!!!!!");
			autoManualWeigh = true;
			unlockWeigher();
			//openWindow();
			break;
		}
		case LaUltimaAutoMode:
		{
			Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaUltimaAutoMode)");
			LabelApplierUltimaAutoMode laUltimaAutoMode(recvBuffer);

			laUltimaAutoMode.show();

			// The CLA will send this message when the first sensor is tripped.
			// It is used for preventing early stable weights for long-tray packages from
			// being treated as valid weights and printing a label for them.
			// The open window is now received from the CLA when the second sensor clears.

			if (wgStateMachine == awmCLAError)
            {
				Debug::trace("AutoWeighMode", "LaUltimaAutoMode - doing nothing because of awmCLAError");
                //cancel this transaction and discharge the pkg
                /*if (pPendingPackage != NULL)

				{
					sendProductComplete(pPendingPackage, false);
					pPendingPackage = NULL;
				}

                if( pCurrentProductTransaction !=NULL )
                {
                    sendProductComplete(pCurrentProductTransaction, false);
                    pCurrentProductTransaction = NULL;

                }
                packId = laUltimaAutoMode.getPackageId();
                sendLabelApplierCloseWindow(packId);
                emptyPackageMap();
                emptyProductQ();
                requestLabelApplierStatus();//check if the error has been cleared
                */
            }
			else
			{
				Debug::trace("AutoWeighMode", "LaUltimaAutoMode:  Set autoManualWeigh = false.");
				autoManualWeigh = false;
				Debug::trace("AutoWeighMode", "LaUltimaAutoMode:  Currently wgStateMachine = " + HobString(Integer::toString(wgStateMachine)));
				wgStateMachine = awmWgCloseWindow;
				Debug::trace("AutoWeighMode.StateMachine", "LaUltimaAutoMode:  Set wgStateMachine = awmWgCloseWindow");
			}
			break;
		}
		case LaOpenWindow:
		{
			Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaOpenWindow)");
			LabelApplierOpenWindow laOpenWindow(recvBuffer);

			laOpenWindow.show();
#ifdef EXTRA_DEBUG
			Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaOpenWindow):  lastOpenWindowPackId = " + HobString(Integer::toString(lastOpenWindowPackId)));
#endif

			if (wgStateMachine == awmCLAError)
            {
				Debug::trace("AutoWeighMode", "LaOpenWindow - doing nothing because of awmCLAError");
                /*Debug::trace("AutoWeighMode", "LaOpenWindow:awmCLAError, closing window for package id:" + HobString(Integer::toString(laOpenWindow.getPackageId())));
                sendLabelApplierCloseWindow(laOpenWindow.getPackageId());

                //reject package, since we are in an error mode
                Debug::trace("AutoWeighMode", "LaOpenWindow:awmCLAError, clearing product transaction and pkg map");
                emptyPackageMap();
                emptyProductQ();
                //requestLabelApplierStatus();//check if the error has been cleared
*/
            }
			else
			{
				if (laOpenWindow.getPackageId() == lastOpenWindowPackId && lastOpenWindowPackId != 0)
					Debug::trace("AutoWeighMode", "LaOpenWindow received, but ignoring because received packId is same as last one received - last packId: " + HobString(Integer::toString(lastOpenWindowPackId)) + HobString("  LaOpenWindow packId: ") + HobString(Integer::toString(laOpenWindow.getPackageId())));
				else
				{
					packId = laOpenWindow.getPackageId();
					lastOpenWindowPackId = packId;
					Debug::trace("AutoWeighMode", "LaOpenWindow:  Set packId = " + HobString(Integer::toString(packId)));

					Debug::trace("AutoWeighMode", "LaOpenWindow:  Set autoManualWeigh = false.");
					autoManualWeigh = false;

					//openWindow();

					WeigherRequestStatus wrs(this);
					dispatchEvent(&wrs, this);
				}
			}
			break;
		}
		case LaCloseWindow:
		{
			Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaCloseWindow)");
			LabelApplierCloseWindow laCloseWindow(recvBuffer);

			laCloseWindow.show();

			wgStateMachine = awmWgCloseWindow;
			Debug::trace("AutoWeighMode.StateMachine", "LaCloseWindow:  Set wgStateMachine = awmWgCloseWindow");
			break;
		}
		case LaStatus:
		{
			Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaStatus)");
			LabelApplierStatus laStatus(recvBuffer);

#ifdef EXTRA_DEBUG
			Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaStatus):   laStatus.isError() = " + HobString(Bool::toString(laStatus.isError())) + "   wgStateMachine = " + HobString(Integer::toString(wgStateMachine)) + ",  checkingForErrors = " + HobString(Integer::toString(checkingForErrors)));
#endif
			if (laStatus.isError() && (wgStateMachine != awmCLAError))
			{
#ifdef EXTRA_DEBUG
				laStatus.show();
#endif

				if (wgStateMachine == awmWgCloseWindow)
				{
					//	Ditch the current package
					Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaStatus):   Need to ditch the current packId: " + HobString(Integer::toString(packId)) + ", but what do we do?");
				}
				wgStateMachine = awmCLAError;
				Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaStatus):   Set wgStateMachine = awmCLAError");

                //clear the pending package/current package and empty the pkg map
/*                if (pPendingPackage != NULL)
				{
					sendProductComplete(pPendingPackage, false);
					pPendingPackage = NULL;
				}
                if( pCurrentProductTransaction !=NULL )
                {
                    sendProductComplete(pCurrentProductTransaction, false);
                    pCurrentProductTransaction = NULL;
                }
                if( packId )//If we have a package id, close it
                {
                    sendLabelApplierCloseWindow(packId);
                    packId = 0;
                }
                emptyPackageMap();
                emptyProductQ();
*/
			}
			else if ((wgStateMachine == awmCLAError) && !laStatus.isError())
			{
				wgStateMachine = awmWgCloseWindow;		//	awmWgIdle;		//	awmWgInitial
 				Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaStatus):  Cleared the error. Set wgStateMachine = awmWgCloseWindow");

                //clear the pending package/current package and empty the pkg map
                /*if (pPendingPackage != NULL)
				{
					sendProductComplete(pPendingPackage, false);
					pPendingPackage = NULL;
				}
                if( pCurrentProductTransaction !=NULL )
                {
                    sendProductComplete(pCurrentProductTransaction, false);
                    pCurrentProductTransaction = NULL;
                }
                if( packId )//If we have a package id, close it
                {
                    sendLabelApplierCloseWindow(packId);
                    packId = 0;
                }
                emptyPackageMap();
                emptyProductQ();*/
				requestWeigherStatus();
			}
			else if(checkingForErrors && !laStatus.isError())//Alex Zavala 12/16
			{
				if (pkgDischargeError != MaxPrinterTypes)		//	HA-147
				{
#ifdef EXTRA_DEBUG
	 				Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaStatus):  (checkingForErrors && !laStatus.isError() && (pkgDischargeError != MaxPrinterTypes))");
					Debug::trace("AutoWeighMode", HobString("handleLabelApplierMessage(LaStatus):  pPendingPackage = 0x") + Hex::toString((long long) pPendingPackage));
					Debug::trace("AutoWeighMode", HobString("handleLabelApplierMessage(LaStatus):  pCurrentProductTransaction = 0x") + Hex::toString((long long) pCurrentProductTransaction));
#endif
	                //clear the pending package/current package and empty the pkg map
	                if (pPendingPackage != NULL)
					{
		 				Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaStatus):  sendProductComplete(pPendingPackage, false)");
						sendProductComplete(pPendingPackage, false);
						pPendingPackage = NULL;
					}
	                if( pCurrentProductTransaction !=NULL )
	                {
		 				Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaStatus):  sendProductComplete(pCurrentProductTransaction, false)");
	                    sendProductComplete(pCurrentProductTransaction, false);
	                    pCurrentProductTransaction = NULL;
	                }
	                if( packId )	//	If we have a package id, close it
	                {
		 				Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaStatus):  sendLabelApplierCloseWindow(" + Integer::toString(packId) + ")");
	                    sendLabelApplierCloseWindow(packId);
	                    packId = 0;
	                }
	                emptyPackageMap();
	                emptyProductQ();
				}
				else
				{
					Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaStatus):  OPENING THE WINDOW");
					autoManualWeigh = false;
					openWindow();
					requestWeigherStatus();
				}
			}
            else if(laStatus.isError() && wgStateMachine == awmCLAError)
            {
#ifdef EXTRA_DEBUG
				laStatus.show();
#endif

                Debug::trace("AutoWeighMode","handleLabelApplierMessage(LaStatus):  awmCLAError - doing nothing");
            }

			checkingForErrors = false;
			Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaStatus):  Set checkingForErrors = false");
			break;
		}
		case LaPackageDischarge:
		{
			Debug::trace("AutoWeighMode", "handleLabelApplierMessage(LaPackageDischarge)");
			handleCLAPackageDischarge(recvBuffer);
			break;
		}
		default:
		{
			Debug::trace("AutoWeighMode", "handleLabelApplierMessage(default):  event = " + HobString(Integer::toString(laEvent)));
			OperatingMode::handleLabelApplierMessage(laEvent, recvBuffer);
			break;
		}
	}
}

void AutoWeighMode::handleWeigherMessage(WgEvent wgEvent, const char *recvBuffer)
{
#ifdef EXTRA_DEBUG
	Debug::trace("AutoWeighMode", "handleWeigherMessage():  Currently packId = " + HobString(Integer::toString(packId)));
	Debug::trace("AutoWeighMode.StateMachine", "handleWeigherMessage():    laStateMachine1 = " + HobString(Integer::toString(laStateMachine1)));
	Debug::trace("AutoWeighMode.StateMachine", "handleWeigherMessage():     wgStateMachine = " + HobString(Integer::toString(wgStateMachine)));
	Debug::trace("AutoWeighMode.StateMachine", "handleWeigherMessage():  pkgDischargeError = " + HobString(Integer::toString(pkgDischargeError)));
#endif
	switch (wgEvent)
	{
		case WgStatus:
		{
			Debug::trace("AutoWeighMode", "handleWeigherMessage(WgStatus)");
			WeigherStatus wgStatus(recvBuffer);

			if ((pPendingPackage != NULL) && (wgStatus.isZeroStableWeight() == true))
			{
				sendProductComplete(pPendingPackage, false);
				pPendingPackage = NULL;
			}

        	if ((pCurrentProduct == 0) || (pCurrentProduct->getProductNum() == 0) && (Configuration::getInstance()->isPrepackScale()))	//HA-65, HA-67: Ensure a PLU is entered
			{
				if (wgStatus.isPositiveNonZeroStableWeight() == true)
				{
					Debug::trace("AutoWeighMode", HobString("handleWeigherMessage(WgStatus):  Sending ProductRequiredToWrap message"));
					opModeStatus.setProductRequiredToWrap();
					dispatchEvent(&opModeStatus);
					largeMotion = false;

	                //	Display the weight of the current package.
					Debug::trace("AutoWeighMode", "handleWeigherMessage: WgStatusAndPrint");
					wgStatus.setEventType(WgStatusAndPrint);
					dispatchEvent(&wgStatus);
				}

                Debug::trace("AutoWeighMode", "empty PLU! emptying package map and queue.");
                if( packId )	//HA-67: If we have a package id, close it
                {
                    sendLabelApplierCloseWindow(packId);
                    packId = 0;
 					Debug::trace("AutoWeighMode", "handleWeigherMessage: sendLabelApplierCloseWindow(" + HobString(Integer::toString(packId)) + ").  Set packId = 0");
                }
                emptyPackageMap();
                emptyProductQ();
				return;
			}

			if( largeMotion &&
				wgStatus.isPositiveNonZeroStableWeight() &&
				wgStatus.getWeight() >= Configuration::getInstance()->getMinWeightToPrint() &&
				 (autoManualWeigh == true || wgStateMachine == awmWgIdle || wgStateMachine == awmWgOpenWindow))
			{
                Debug::trace("AutoWeighMode", "handleWeigherMessage: WgStatusAndPrint");
				wgStatus.setEventType(WgStatusAndPrint);
				dispatchEvent(&wgStatus);
			}

			if( largeMotion &&
				wgStatus.isPositiveNonZeroStableWeight() &&
				wgStatus.getWeight() >= Configuration::getInstance()->getMinWeightToPrint() &&
				wgStateMachine!=awmWgOpenWindow &&
				checkingForErrors==false)
			{
				//check for errors
				Debug::trace("AutoWeighMode", "POSITIVE STABLE WEIGHT");
				checkingForErrors = true;
				requestLabelApplierStatus();
			}

			if (wgStatus.isLargeMotion() == true)
			{
				Debug::trace("AutoWeighMode", "Set largeMotion = true");
				largeMotion = true;
				if (pPendingPackage != NULL)
				{
					sendProductComplete(pPendingPackage, false);
					pPendingPackage = NULL;
				}
			}

			if (Debug::isTracing("AutoWeighMode"))
			{
				Debug::trace("AutoWeighMode", HobString("largeMotion:") + HobString(Bool::toString(largeMotion)) );
				Debug::trace("AutoWeighMode", HobString("positiveNonZeroWeight:") + HobString(Bool::toString(wgStatus.isPositiveNonZeroStableWeight())) );
				Debug::trace("AutoWeighMode", HobString("autoManualWeigh:") + HobString(Bool::toString(autoManualWeigh)) );
				Debug::trace("AutoWeighMode", HobString("wgStateMachine:") + HobString(Integer::toString(wgStateMachine)) );
				Debug::trace("AutoWeighMode", HobString("checkingForErrors:") + HobString(Bool::toString(checkingForErrors)) );
			}
			break;
		}

		case WgCachedStatus:
		{
			Debug::trace("AutoWeighMode", "handleWeigherMessage(WgCachedStatus)");
			Debug::trace("AutoWeighMode.WeigherCachedStatus", "received a WgCachedStatus");

			WeigherCachedStatus wgCachedStatus(recvBuffer);

			if (Debug::isTracing("AutoWeighMode.WeigherCachedStatus") == true)
				wgCachedStatus.show();

			if ( wgCachedStatus.isLargeMotion() )
			{
				Debug::trace("AutoWeighMode.WeigherCachedStatus", "Set largeMotion = true");
				largeMotion = true;
			}
			else
				Debug::trace("AutoWeighMode.WeigherCachedStatus", "WgCachedStatus large motion wasn't set");
			break;
		}

		default:
		{
			Debug::trace("AutoWeighMode", "handleWeigherMessage(default):  event = " + HobString(Integer::toString(wgEvent)) + ".  Doing nothing.");
			break;
		}
	}						
}

//void AutoWeighMode::firstPrinterAttached(PrinterManager *pPrinterMgr)
void AutoWeighMode::firstPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	OperatingMode::firstPrinterAttached(pPrinterMgr);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	unlockWeigher();
	//openWindow();

	laStateMachine1 = wmLaInitial;
	Debug::trace("AutoWeighMode.StateMachine", "firstPrinterAttached():  laStateMachine1: wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("AutoWeighMode.StateMachine", "firstPrinterAttached():  laStateMachine2: wmLaInitial");
}

void AutoWeighMode::firstPrinterDetached()
{
	_Mybase::firstPrinterDetached();

	productFailed();
}

void AutoWeighMode::secondPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::secondPrinterAttached(pPrinterMgr);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	unlockWeigher();
	//openWindow();

	laStateMachine1 = wmLaInitial;
	Debug::trace("AutoWeighMode.StateMachine", "secondPrinterAttached():  laStateMachine1: wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("AutoWeighMode.StateMachine", "secondPrinterAttached():  laStateMachine2: wmLaInitial");
}

void AutoWeighMode::secondPrinterDetached()
{
	_Mybase::secondPrinterDetached();

	productFailed();
}

void AutoWeighMode::thirdPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::thirdPrinterAttached(pPrinterMgr);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	unlockWeigher();
	//openWindow();

	laStateMachine1 = wmLaInitial;
	Debug::trace("AutoWeighMode.StateMachine", "thirdPrinterAttached():  laStateMachine1: wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("AutoWeighMode.StateMachine", "thirdPrinterAttached():  laStateMachine2: wmLaInitial");
}

void AutoWeighMode::thirdPrinterDetached()
{
	_Mybase::thirdPrinterDetached();

	productFailed();
}

void AutoWeighMode::labelApplierAttached(Hobart::CEvolMsgReceiver *pLabelApplierManager)
{
	_Mybase::labelApplierAttached( pLabelApplierManager );
	setLabelApplierMode(Auto);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	unlockWeigher();
	Debug::trace("AutoWeighMode", "labelApplierAttached():  Setting autoManualWeigh = true!!!!");
	autoManualWeigh = true;
	wgStateMachine = awmWgCloseWindow;
	Debug::trace("AutoWeighMode.StateMachine", "labelApplierAttached():  wgStateMachine: awmWgCloseWindow");
	//openWindow();
}

void AutoWeighMode::labelApplierDetached()
{
	_Mybase::labelApplierDetached();

	productFailed();
}

void AutoWeighMode::productFailed()
{
	if (pCurrentProductTransaction != NULL)
	{
		sendProductComplete(pCurrentProductTransaction, false);

		Debug::trace("AutoWeighMode.Error", HobString("Product Failed") +
											HobString(Hex::toString((long long) pCurrentProductTransaction)));
		Debug::trace("AutoWeighMode.Product", HobString("pCurrentProductTransaction: 0x") +
											  HobString(Hex::toString((long long) pCurrentProductTransaction)));

		pCurrentProductTransaction = NULL;
	}

	laStateMachine1 = wmLaInitial;
	Debug::trace("AutoWeighMode.StateMachine", "productFailed():  laStateMachine1: wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("AutoWeighMode.StateMachine", "productFailed():  laStateMachine2: wmLaInitial");

	// Just in case the weigher is locked!!
	unlockWeigher();
}

void AutoWeighMode::openWindow()
{
	wgStateMachine = awmWgOpenWindow;
	Debug::trace("AutoWeighMode.StateMachine", "openWindow():  wgStateMachine: awmWgOpenWindow");

	unlockWeigher();
}

void AutoWeighMode::startNewPackage()
{
	//lockWeigher();
	largeMotion = false;
	wgStateMachine = awmWgCloseWindow;
	Debug::trace("AutoWeighMode.StateMachine", "startNewPackage() - wgStateMachine: awmWgCloseWindow");

	Debug::trace("AutoWeighMode", "startNewPackage():  Will call sendLabelApplierCloseWindow. Current packId:" + HobString(Integer::toString(packId)));
	sendLabelApplierCloseWindow(packId);
	generateNewPackage2(pCurrentProductTransaction);
	
    if(0)//TODO: Figure out how we are going to be able to tell whether a CLA is attached!
	//if (pLabelApplierManager && pLabelApplierManager->isCompactLabelApplier_EPSAC2())
	{
		//	Clear the package ID. We'll get it from the CLA.
		Debug::trace("AutoWeighMode", "startNewPackage():  CLA attached.");
		packId = 0;
		Debug::trace("AutoWeighMode", "startNewPackage():  Set packId = 0.");
	}
	Debug::trace("AutoWeighMode", "startNewPackage():  packId = " + HobString(Integer::toString(packId)));

	//wgStateMachine = awmWgCloseWindow;
	//Debug::trace("AutoWeighMode.StateMachine", "startNewPackage() - wgStateMachine: awmWgCloseWindow");
}

void AutoWeighMode::handleCLAPackageDischarge(const char *recvBuffer)
{
	bool traceOn = Debug::isTracing("AutoWeighMode");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "handleCLAPackageDischarge()";
		Debug::trace("AutoWeighMode", msgDebug);
		msgDebug.append(":  ");
	}
	CLAPackageDischarge statusMsg(recvBuffer);
	Debug::trace("AutoWeighMode", "handleCLAPackageDischarge():  statusMsg.getPackageId() = " + HobString(Integer::toString(statusMsg.getPackageId())));
//	statusMsg.show();

	const OpModePackage *pPackage = getPackage((UINT16)statusMsg.getPackageId());
	if (!pPackage)
		Debug::trace("AutoWeighMode", msgDebug + "pPackage is 0. Package is complete.");
	else
	{
		//	We have a problem here. The package has been discharged by the CLA
		//	but the transaction is not complete for that package. A label must be left behind.
		if (traceOn)
		{
			Debug::trace("AutoWeighMode", msgDebug + "Current states:");
			Debug::trace("AutoWeighMode", msgDebug + "   laStateMachine1 = " + inttostring(laStateMachine1));
			Debug::trace("AutoWeighMode", msgDebug + "   laStateMachine2 = " + inttostring(laStateMachine2));
			Debug::trace("AutoWeighMode", msgDebug + "   laStateMachine3 = " + inttostring(laStateMachine3));
		}
		pkgDischargeError = MaxPrinterTypes;
		if (pPackage->isCompleted(FirstPrinter) == false)	
		{
			if (laStateMachine1 == wmLaLabelReady)
			{
				pkgDischargeError = FirstPrinter;
				Debug::trace("AutoWeighMode", msgDebug + "pkgDischargeError = FirstPrinter");
			}
		}	
		else if (pPackage->isCompleted(SecondPrinter) == false)	
		{
			if (laStateMachine2 == wmLaLabelReady)
			{
				pkgDischargeError = SecondPrinter;
				Debug::trace("AutoWeighMode", msgDebug + "pkgDischargeError = SecondPrinter");
			}
		}	
		else if (pPackage->isCompleted(ThirdPrinter) == false)	
		{
			if (laStateMachine3 == wmLaLabelReady)
			{
				pkgDischargeError = ThirdPrinter;
				Debug::trace("AutoWeighMode", msgDebug + "pkgDischargeError = ThirdPrinter");
			}
		}	
		if (pkgDischargeError != MaxPrinterTypes)
		{
			Debug::trace("AutoWeighMode", msgDebug + "Calling sendPkgDischargeError()");
			sendPkgDischargeError(pkgDischargeError);

            if( pCurrentProductTransaction !=NULL )
            {
				Debug::trace("AutoWeighMode", msgDebug + "sendProductComplete(pCurrentProductTransaction, false)");
                sendProductComplete(pCurrentProductTransaction, false);
                pCurrentProductTransaction = NULL;
            }
            if( packId )//If we have a package id, close it
            {
				Debug::trace("AutoWeighMode", msgDebug + "sendLabelApplierCloseWindow(" + HobString(Integer::toString(packId)) + ")");
                sendLabelApplierCloseWindow(packId);
                packId = 0;
            }

			emptyPackageMap();
            emptyProductQ();
		}
	}
}
