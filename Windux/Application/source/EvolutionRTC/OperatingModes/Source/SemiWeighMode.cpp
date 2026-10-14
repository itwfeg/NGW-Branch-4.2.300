#include "OpModeEvent.h"
#include "SemiWeighMode.h"

#include "WeigherStatus.h"
#include "WeigherRequestStatus.h"

#include "LabelApplierManager.h"
#include "LabelApplierOpenWindow.h"
#include "LabelApplierCloseWindow.h"

#include "PrinterManager.h"
#include "PrinterStatus.h"
#include "PrinterLabelReady.h"
#include "PrinterLabelTaken.h"
#include "PrinterLabelComplete.h"

#include "ProductUpdateMsg.h"
#include "ProductChangeMsg.h"

#include "Configuration.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SemiWeighMode::_dataType_ = 0;
std::string SemiWeighMode::_dataTypeName_("SemiWeighMode"); 
#endif

SemiWeighMode::SemiWeighMode(const Util::CSmartPtr<Product> &productSmartPtr,
							 //WeigherManager *pWeigherManager,
							 //PrinterManager *pFirstPrinter,
							 //PrinterManager *pSecondPrinter,
							 //PrinterManager *pThirdPrinter,
							 //LabelApplierManager *pLabelApplierManager,
							 //WrapperManager *pWrapperManager) :	OperatingMode("tSemiWeigh",
							 Hobart::CEvolMsgReceiver *pWeigherManager,
							 Hobart::CEvolMsgReceiver *pFirstPrinter,
							 Hobart::CEvolMsgReceiver *pSecondPrinter,
							 Hobart::CEvolMsgReceiver *pThirdPrinter,
							 Hobart::CEvolMsgReceiver *pLabelApplierManager,
							 Hobart::CEvolMsgReceiver *pWrapperManager) :	OperatingMode("tSemiWeigh",
																			  productSmartPtr,
																			  pWeigherManager,
																			  pFirstPrinter,
																			  pSecondPrinter,
																			  pThirdPrinter,
																			  pLabelApplierManager,
																			  pWrapperManager),
														   		SEMI_MaxLabels(3)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

    registerEvent(ProductDev, ProductChangeSmart);
	registerEvent(ProductDev, ProductUpdate);

	registerEvent(Printer, PrStatus);
	registerEvent(Printer, PrLabelReady);
	registerEvent(Printer, PrLabelTaken);
	registerEvent(Printer, PrLabelComplete);
	registerEvent(Printer, PrConfig);

	registerEvent(LabelApplier, LaStatus);
	registerEvent(LabelApplier, LaOpenWindow);
	registerEvent(LabelApplier, LaCloseWindow);		//	Do we really need this?

	registerEvent(Weigher, WgStatus);

	setWeigherMode(ScalePrepackMode);
	requestWeigherStatus();

	setLabelApplierMode(SemiAuto);			//	Per Carlos for USB CLA

//	resetLabelApplier();
	requestLabelApplierStatus();

	stateMachine = swmInitial;
	Debug::trace("SemiWeighMode.StateMachine", "SemiWeighMode():  stateMachine:  swmInitial");

	laStateMachine1 = wmLaInitial;
	Debug::trace("SemiWeighMode.StateMachine", "SemiWeighMode():  laStateMachine1:  wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("SemiWeighMode.StateMachine", "SemiWeighMode():  laStateMachine2:  wmLaInitial");

	missedProductUpdates = 0;
	checkingForErrors = false;//Alex Zavala 12/2

	takeLabelTime = 5;
	setTakeLabelTime(FirstPrinter);
	setTakeLabelTime(SecondPrinter);

	if (productSmartPtr.get())
	{
		Util::CSmartPtr<Product> newProductSmartPtr(productSmartPtr);
		handleProductSmartPtr(newProductSmartPtr);
	}
}

SemiWeighMode::~SemiWeighMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	deregisterAllEvents();
	
	setLabelApplierMode(Conveyor);		//	Exiting SemiWeighMode
//	resetLabelApplier();

	ShutdownNicely();
}				


#include "ProductManager.h"

void SemiWeighMode::handleProductSmartPtr(Util::CSmartPtr<Product> &pCurrentProductSmartPtr)
{
	bool isTracing = Debug::isTracing("SemiWeighMode.ChangeProduct");

	Product *pProduct = 0;
	pProduct = pCurrentProductSmartPtr.get();

	// Only push the new product onto the product queue if it isn't NULL.
	// Can get a NULL when UI looks up product 0 - ie. clearing the product.
	if (pProduct == NULL)
		pCurrentProduct = 0;		//	AWL-113
	else
	{
		if (opModeStatus.isProductRequiredToWrap())
		{
			if (isTracing)
				Debug::trace("SemiWeighMode.ChangeProduct", HobString("handleProductSmartPtr():  Clearing ProductRequiredToWrap message"));
			opModeStatus.clearProductRequiredToWrap();
			dispatchEvent(&opModeStatus);
		}

		productQ.push(pCurrentProductSmartPtr);

		stateMachine = swmChangingProduct;
		Debug::trace("SemiWeighMode.StateMachine", "handleProductSmartPtr():  stateMachine:  swmChangingProduct");

		if (isTracing)
			Debug::trace("SemiWeighMode.ChangeProduct", HobString("ProductChange - packageMap size #2 is") + HobString(Integer::toString((int)packageMap.size())));

		if (packageMap.empty() && getNewProduct())
		{
			if (isTracing)
				Debug::trace("SemiWeighMode.ChangeProduct", HobString("ProductChange - packageMap is empty and getNewProduct()"));
			changeProduct();
			preImagePrinters();
			//openWindow();
			unlockWeigher();//Alex Zavala 11/27
		}
	}
}

void SemiWeighMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
	switch (productEvent)
	{
		// Operator has chosen a new product
		case ProductChangeSmart:
		{
			Debug::trace("SemiWeighMode", "handleProductMessage(ProductChangeSmart)");
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
			Debug::trace("SemiWeighMode", "handleProductMessage(ProductUpdate)");
			ProductUpdateMsg prodUpdate(recvBuffer);
			ProductTransaction *pNextProductTransaction = (ProductTransaction *) prodUpdate.getProductTransaction();
			Debug::trace("SemiWeighMode.Product", HobString("pNextProductTransaction: 0x") +
												  HobString(Hex::toString((long long) pNextProductTransaction)));
			Debug::trace("SemiWeighMode.Product", HobString("largeMotion: ") + HobString(Bool::toString(largeMotion)));
			Debug::trace("SemiWeighMode.Product", HobString("stateMachine: ") + HobString(Integer::toString(stateMachine)));

			if ((stateMachine == swmCLAError) || lastStatusMsg.isError())
			{
				Debug::trace("SemiWeighMode.Product", "Not labeling condition:  (stateMachine == swmCLAError)");

				sendProductComplete(pNextProductTransaction, false);
				if (pPendingPackage)
				{
					sendProductComplete(pPendingPackage, false);
					pPendingPackage = 0;
				}
			}
			else if ((packageMap.size() < SEMI_MaxLabels) && (stateMachine == swmOpenWindow) && largeMotion)
			{
				Debug::trace("SemiWeighMode", "GENERATED NEW PKG");
				stateMachine = swmCloseWindow;
				largeMotion = false;
				pCurrentProductTransaction = pNextProductTransaction;
				sendLabelApplierCloseWindow(packId);
				generateNewPackage(pCurrentProductTransaction);
				
				Debug::trace("SemiWeighMode.StateMachine", "handleProductMessage():  stateMachine:  swmCloseWindow");
			}
			else
			{
				// This is an error condition!!
				sendProductComplete(pNextProductTransaction, false);
				missedProductUpdates++;
				Debug::trace("SemiWeighMode.Error", HobString("Missed Product Update: ") + HobString(Integer::toString(missedProductUpdates)));
				Debug::trace("SemiWeighMode.Product", HobString("pNextProductTransaction: 0x") +
													  HobString(Hex::toString((long long) pNextProductTransaction)));
			}
			break;
		}

		default:
		{
			Debug::trace("SemiWeighMode", "handleProductMessage(default)");
			break;
		}
	}					
}

void SemiWeighMode::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer)
{
	switch (prEvent)
	{
		case PrStatus: 
		{
			PrinterStatus prStatus(recvBuffer);
			PrinterType printerType = prStatus.getPrinterType();

			if (prStatus.isHeadDisengaged() == true)
			{
				updatePrinterHeadUp(printerType);
				
				if (stateMachine != swmPrinterHeadUp)
				{
					stateMachine = swmPrinterHeadUp;
					Debug::trace("SemiWeighMode.StateMachine", "handlePrinterMessage():  stateMachine:  swmPrinterHeadUp");

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
						//openWindow();//unlockWeigher()??
						unlockWeigher();//Alex Zavala 11/27
						Debug::trace("SemiWeighMode", "PREIMAGING NEW PRODUCT");
					}
					else
					{
						stateMachine = swmInitial;
						Debug::trace("SemiWeighMode.StateMachine", "handlePrinterMessage():  stateMachine:  swmInitial");
					}

					laStateMachine1 = wmLaInitial;
					Debug::trace("SemiWeighMode.StateMachine", "handlePrinterMessage():  laStateMachine1:  wmLaInitial");

					laStateMachine2 = wmLaInitial;
					Debug::trace("SemiWeighMode.StateMachine", "handlePrinterMessage():  laStateMachine2:  wmLaInitial");
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
				Debug::trace("SemiWeighMode.StateMachine", "laStateMachine1 = wmLaLabelReady");

				sendLabelReady(PrimaryPrinterStation, prLabelReady.getPackId());
			}
			else if (printerType == SecondPrinter)
			{
				laStateMachine2 = wmLaLabelReady;
				Debug::trace("SemiWeighMode.StateMachine", "laStateMachine2 = wmLaLabelReady");

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
				Debug::trace("SemiWeighMode.StateMachine", "laStateMachine1 = wmLaLabelTaken");

				sendLabelTaken(PrimaryPrinterStation, prLabelTaken.getPackId());
			}
			else if (printerType == SecondPrinter)
			{
				laStateMachine2 = wmLaLabelTaken;
				Debug::trace("SemiWeighMode.StateMachine", "laStateMachine2 = wmLaLabelTaken");

				sendLabelTaken(SecondaryPrinterStation, prLabelTaken.getPackId());
			}
			break;
		}	
		case PrLabelComplete: 
		{						
			PrinterLabelComplete prLabelComplete(recvBuffer);

			if (updateProductComplete(prLabelComplete) == true)
			{
				if (packageMap.empty() && (stateMachine == swmChangingProduct))
				{
					if (getNewProduct() == true)
					{
						changeProduct();
						preImagePrinters();
						//openWindow();
						unlockWeigher();

						laStateMachine1 = wmLaInitial;
						Debug::trace("SemiWeighMode.StateMachine", "laStateMachine1:  wmLaInitial");

						laStateMachine2 = wmLaInitial;
						Debug::trace("SemiWeighMode.StateMachine", "laStateMachine2:  wmLaInitial");
					}
				}
			}
			break;
		}	

		case PrConfig:
			Debug::trace("SemiWeighMode", "handlePrinterMessage(PrConfig)");
			handlePrinterConfigInOpMode(recvBuffer);
			break;

		default:
			OperatingMode::handlePrinterMessage(prEvent, recvBuffer);
			break;
	}
}

void SemiWeighMode::handleLabelApplierMessage(LaEvent laEvent, const char *recvBuffer)
{
	switch (laEvent)
	{
		case LaStatus:
		{
			Debug::trace("SemiWeighMode", "handleLabelApplierMessage(LaStatus)");

			LabelApplierStatus laStatus(recvBuffer);

			laStatus.show();

			if (laStatus.isError() && (stateMachine != swmCLAError))
			{
				stateMachine = swmCLAError;
				Debug::trace("SemiWeighMode", "handleLabelApplierMessage(LaStatus):  Set stateMachine = swmCLAError");
			}
			else if ((stateMachine == swmCLAError) && !laStatus.isError())
			{
				//	If we already received an error, and the error flag is no longer set,
				//	go to open window state.
				Debug::trace("SemiWeighMode", "handleLabelApplierMessage(LaStatus): ERROR CLEARED");
				stateMachine = swmCloseWindow;
				//checkingForErrors = false;
				requestWeigherStatus();
			}
			else if (checkingForErrors && !laStatus.isError())
			{
				Debug::trace("SemiWeighMode", "handleLabelApplierMessage(LaStatus): OPENING WINDOW");
				//checkingForErrors=false;
				openWindow();
				requestWeigherStatus();
			}
			checkingForErrors = false;
			Debug::trace("SemiWeighMode", "handleLabelApplierMessage(LaStatus): checkingForErrors = false");
			lastStatusMsg = laStatus;
			
			break;
		}
		default:
		{
			Debug::trace("SemiWeighMode", "handleLabelApplierMessage(default):  event = " + HobString(Integer::toString(laEvent)));
			OperatingMode::handleLabelApplierMessage(laEvent, recvBuffer);
			break;
		}
	}						
}

void SemiWeighMode::handleWeigherMessage(WgEvent wgEvent, const char *recvBuffer)
{
	switch (wgEvent)
	{
		case WgStatus:
		{
			WeigherStatus wgStatus(recvBuffer);

			if (wgStatus.isPositiveNonZeroStableWeight() == true)
			{
				Debug::trace("SemiWeighMode.StableWeight", "Positive, NonZero, Stable Weight");
			}

			// HA-65
			if ((wgStatus.isPositiveNonZeroStableWeight() == true) && (largeMotion == true) && Configuration::getInstance()->isPrepackScale())
			{
				Debug::trace("SemiWeighMode", HobString("handleWeigherMessage(WgStatus):  pCurrentProduct = 0x") + Hex::toString((long long) pCurrentProduct));
				if ((pCurrentProduct == 0) || (pCurrentProduct->getProductNum() == 0))
				{
					Debug::trace("SemiWeighMode", HobString("handleWeigherMessage(WgStatus):  Sending ProductRequiredToWrap message"));
					opModeStatus.setProductRequiredToWrap();
					dispatchEvent(&opModeStatus);
					largeMotion = false;
					break;
				}
			}
			Debug::trace("SemiWeighMode","handleWeigherMessage(WgStatus) XXX");

			if (Configuration::getInstance()->isPrepackScale() == true)
			{
				Debug::trace("SemiWeighMode","handleWeigherMessage(WgStatus) isPrepackScale is true");

				if (wgStatus.isLargeMotion() == true)
					largeMotion = true;

				/*//Dont think we need this Alex Zavala 12/16
				if ((stateMachine == swmModeError && wgStatus.isZeroStableWeight()) ||
					 (stateMachine == swmCloseWindow && (wgStatus.isSmallMotion() || wgStatus.isLargeMotion())))
				{
					//openWindow();
					Debug::trace("SemiWeighMode", "CLOSEWINDOW CHECKING FOR MORE ERRORS");
					checkingForErrors=true;
					requestLabelApplierStatus();//Alex Zavala 11/27
				}*/

				if( largeMotion &&
					wgStatus.isPositiveNonZeroStableWeight() &&
					 wgStatus.getWeight() >= Configuration::getInstance()->getMinWeightToPrint() &&
					 stateMachine!=swmOpenWindow &&
					 checkingForErrors==false)
				{
					Debug::trace("SemiWeighMode", "POSITIVE STABLE WEIGHT AND CHECKING FOR ERRORS");
					checkingForErrors=true;
					requestLabelApplierStatus();
				}
/*
				if (largeMotion &&
					 wgStatus.isPositiveNonZeroStableWeight() &&
					 wgStatus.getWeight() >= Configuration::getInstance()->getMinWeightToPrint() &&
					 stateMachine == swmOpenWindow)
				{
					Debug::trace("SemiWeighMode", "SENDING PRINT");
					wgStatus.setEventType(WgStatusAndPrint);
					dispatchEvent(&wgStatus, this);
				}
				*/
				Debug::trace("SemiWeighMode", HobString("largeMotion:") + HobString(Bool::toString(largeMotion)) );
				Debug::trace("SemiWeighMode", HobString("positiveNonZeroWeight:") + HobString(Bool::toString(wgStatus.isPositiveNonZeroStableWeight())) );
				Debug::trace("SemiWeighMode", HobString("wgStateMachine:") + HobString(Integer::toString(stateMachine)) );
				Debug::trace("SemiWeighMode", HobString("checkingForErrors:") + HobString(Bool::toString(checkingForErrors)) );
			}
			else
			{
				if ((stateMachine == swmCloseWindow) && ((wgStatus.isSmallMotion() == true) || (wgStatus.isLargeMotion() == true)))
				{
					openWindow();
				}
			}
		}
		break;

		default:
		{
			break;
		}
	}						
}

void SemiWeighMode::firstPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::firstPrinterAttached(pPrinterMgr);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	if (Configuration::getInstance()->isPrepackScale() == false)
		openWindow();
	else
	{
		//openWindow();//Maybe change this to unlockWeigher() and only change states when we receive a status with no errors
		unlockWeigher();
	}

	laStateMachine1 = wmLaInitial;
	Debug::trace("SemiWeighMode.StateMachine", "laStateMachine1:  wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("SemiWeighMode.StateMachine", "laStateMachine2:  wmLaInitial");
}

void SemiWeighMode::firstPrinterDetached()
{
	_Mybase::firstPrinterDetached();

	productFailed();
}

void SemiWeighMode::secondPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::secondPrinterAttached(pPrinterMgr);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	if (Configuration::getInstance()->isPrepackScale() == false)
		openWindow();
	else
	{
		//openWindow();
		unlockWeigher();
	}

	laStateMachine1 = wmLaInitial;
	Debug::trace("SemiWeighMode.StateMachine", "laStateMachine1:  wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("SemiWeighMode.StateMachine", "laStateMachine2:  wmLaInitial");
}

void SemiWeighMode::secondPrinterDetached()
{
	_Mybase::secondPrinterDetached();

	productFailed();
}

void SemiWeighMode::thirdPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::thirdPrinterAttached(pPrinterMgr);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	if (Configuration::getInstance()->isPrepackScale() == false)
		openWindow();
	else
	{
		//openWindow();
		unlockWeigher();
	}

	laStateMachine1 = wmLaInitial;
	Debug::trace("SemiWeighMode.StateMachine", "laStateMachine1:  wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("SemiWeighMode.StateMachine", "laStateMachine2:  wmLaInitial");
}

void SemiWeighMode::thirdPrinterDetached()
{
	_Mybase::thirdPrinterDetached();

	productFailed();
}

void SemiWeighMode::labelApplierAttached(Hobart::CEvolMsgReceiver *pLabelApplierManager)
{
	Debug::trace("SemiWeighMode", "labelApplierAttached()");
	_Mybase::labelApplierAttached( pLabelApplierManager );

	setLabelApplierMode(SemiAuto);			//	Per Carlos for USB CLA

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	unlockWeigher();
	stateMachine = swmCloseWindow;
}

void SemiWeighMode::labelApplierDetached()
{
	Debug::trace("SemiWeighMode", "labelApplierDetached()");
	_Mybase::labelApplierDetached();

	productFailed();
}

void SemiWeighMode::productFailed()
{
	if (pCurrentProductTransaction != NULL)
	{
		sendProductComplete(pCurrentProductTransaction, false);
		pCurrentProductTransaction = NULL;
	}

	laStateMachine1 = wmLaInitial;
	Debug::trace("SemiWeighMode.StateMachine", "laStateMachine1:  wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("SemiWeighMode.StateMachine", "laStateMachine2:  wmLaInitial");

	// Just in case the weigher is locked!!
	unlockWeigher();
}

void SemiWeighMode::openWindow()
{
	stateMachine = swmOpenWindow;
	Debug::trace("SemiWeighMode.StateMachine", "openWindow():  stateMachine:  swmOpenWindow");

	unlockWeigher();
}
