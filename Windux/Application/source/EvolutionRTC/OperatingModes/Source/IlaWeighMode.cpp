
#include "IlaWeighMode.h"

#include "WeigherStatus.h"
#include "WeigherRequestStatus.h"


#include "PrinterManager.h"
#include "PrinterStatus.h"
#include "PrinterLabelComplete.h"

#include "ProductUpdateMsg.h"
#include "ProductChangeMsg.h"

#include "ProductTransaction.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "HobString.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int IlaWeighMode::_dataType_ = 0;
std::string IlaWeighMode::_dataTypeName_("IlaWeighMode"); 
#endif


IlaWeighMode::IlaWeighMode(const Util::CSmartPtr<Product> &productSmartPtr,
							//WeigherManager *pWeigherManager,
							//PrinterManager *pFirstPrinter,
							//PrinterManager *pSecondPrinter) : OperatingMode("tILAWeigh", 
							Hobart::CEvolMsgReceiver *pWeigherManager,
							Hobart::CEvolMsgReceiver *pFirstPrinter,
							Hobart::CEvolMsgReceiver *pSecondPrinter) : OperatingMode("tILAWeigh", 
								   												productSmartPtr,
								   												pWeigherManager,
								   												pFirstPrinter,
																				pSecondPrinter),
															   		ILA_MaxLabelsRunning(2),
																	ILA_MaxLabelsManual(1),
																	TakeLabelTimeout(5)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	ilaOff = true;
	Debug::trace("IlaWeighMode.IlaOff", HobString("ilaOff: ") + Bool::toString(ilaOff));

	registerEvent(ProductDev, ProductChangeSmart);
	registerEvent(ProductDev, ProductUpdate);

//	registerEvent(LabelApplier, LaILAOpenWindow);
//	registerEvent(LabelApplier, LaILACloseWindow);
//	registerEvent(LabelApplier, LaILAForceLabel);
//	registerEvent(LabelApplier, LaILAOff);

	registerEvent(Printer, PrStatus);
	registerEvent(Printer, PrLabelComplete);
	registerEvent(Printer, PrConfig);

	registerEvent(Weigher, WgStatus);

	setWeigherMode(ScalePrepackMode);
	requestWeigherStatus();

	setLabelApplierMode(SemiAuto);		//	What should this be now???	//Per Carlos for USB CLA

	stateMachine = ilawmInitial;
	Debug::trace("IlaWeighMode.StateMachine", "stateMachine = ilawmInitial");

	laStateMachine1 = wmLaInitial;
	Debug::trace("IlaWeighMode.StateMachine", "laStateMachine1 = wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("IlaWeighMode.StateMachine", "laStateMachine2 = wmLaInitial");

	forceVoidLabel = false;
	voidLabelCount = 0;
	Debug::trace("IlaWeighMode.VoidLabels", HobString("voidLabelCount: ") + Integer::toString(voidLabelCount));

	missedProductUpdates = 0;
	Debug::trace("IlaWeighMode.MissedProducts", HobString("missedProductUpdates: ") + Integer::toString(missedProductUpdates));

	takeLabelTime = 5;
	setTakeLabelTime(FirstPrinter);
	setTakeLabelTime(SecondPrinter, TakeLabelTimeout);
//	setTakeLabelTime(ThirdPrinter, TakeLabelTimeout);		//	Not supported yet.

	if (productSmartPtr.get())
	{
		Util::CSmartPtr<Product> newProductSmartPtr(productSmartPtr);
		handleProductSmartPtr(newProductSmartPtr);
	}
}


IlaWeighMode::~IlaWeighMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif	
	deregisterAllEvents();
	
	ShutdownNicely();

	setLabelApplierMode(Conveyor);		//	Exiting IlaWeighMode
//	resetLabelApplier();
}				


#include "ProductManager.h"

void IlaWeighMode::handleProductSmartPtr(Util::CSmartPtr<Product> &pCurrentProductSmartPtr)
{
	bool isTracing = Debug::isTracing("IlaWeighMode.ChangeProduct");

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
				Debug::trace("IlaWeighMode.ChangeProduct", HobString("handleProductSmartPtr():  Clearing ProductRequiredToWrap message"));
			opModeStatus.clearProductRequiredToWrap();
			dispatchEvent(&opModeStatus);
		}

		productQ.push(pCurrentProductSmartPtr);

		stateMachine = ilawmChangingProduct;
		Debug::trace("IlaWeighMode.StateMachine", "stateMachine = ilawmChangingProduct");

		if (isTracing)
			Debug::trace("IlaWeighMode.ChangeProduct", HobString("ProductChange - packageMap size #2 is") + HobString(Integer::toString((int)packageMap.size())));

		if (pPendingPackage != NULL)
		{
			if (isTracing)
				Debug::trace("IlaWeighMode.ChangeProduct", HobString("ProductChange - pPendingPackage isn't NULL : 0x") + Hex::toString((long long) pPendingPackage));
			sendProductComplete((const ProductTransaction *) pPendingPackage, false);
			pPendingPackage = NULL;
		}

		if (packageMap.empty() && getNewProduct())
		{
			if (isTracing)
				Debug::trace("IlaWeighMode.ChangeProduct", HobString("ProductChange - packageMap is empty and getNewProduct()"));
			changeProduct();
			preImagePrinters();
			openWindow();
		}
	}
}

void IlaWeighMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
	switch (productEvent)
	{
		// Operator has chosen a new product
		case ProductChangeSmart:
		{
			Debug::trace("IlaWeighMode.ChangeProduct", HobString("ProductChange - packageMap size is") + HobString(Integer::toString((int)packageMap.size())));
			ProductChangeMsg prodChange(recvBuffer);
			Debug::trace("IlaWeighMode.ChangeProduct", HobString("ProductChange: 0x") + Hex::toString((long long)  prodChange.getProduct()));

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
			ProductTransaction *pNextProductTransaction = (ProductTransaction *) prodUpdate.getProductTransaction();
			//	PTR 706: Let the Operating mode know if we do or don't need the weigherStatus/largeMotion to print a label.
			bool ignoreWeigherStatus = prodUpdate.getIgnoreWeigherStatus();

			// only process the transaction if it isn't NULL; if NULL, just ignore it.
			if (pNextProductTransaction)
			{
				bool isTracing = Debug::isTracing("IlaWeighMode.ProductTransaction");

				int mapsize = (int)packageMap.size();

				if (isTracing)
				{
					Debug::trace("IlaWeighMode.ProductTransaction", HobString("ProductUpdate - largeMotion=") + HobString(Bool::toString(largeMotion)));
					Debug::trace("IlaWeighMode.ProductTransaction", HobString("ProductUpdate - ilaOff=") + HobString(Bool::toString(ilaOff)));
					Debug::trace("IlaWeighMode.ProductTransaction", HobString("ProductUpdate - ignoreWeigherStatus=") + HobString(Bool::toString(ignoreWeigherStatus)));
					Debug::trace("IlaWeighMode.ProductTransaction", HobString("ProductUpdate - stateMachine == ilawmOpenWindow =") + HobString(Bool::toString(stateMachine == ilawmOpenWindow)));
					Debug::trace("IlaWeighMode.ProductTransaction", HobString("ProductUpdate - stateMachine=") + HobString(Integer::toString(stateMachine)));
					Debug::trace("IlaWeighMode.ProductTransaction", HobString("ProductUpdate - packageMap.size()=") + HobString(Integer::toString(mapsize)));
				}

				if (((mapsize < ILA_MaxLabelsManual) && (ilaOff == true)) ||
					 ((mapsize < ILA_MaxLabelsRunning) && (ilaOff == false) && (stateMachine == ilawmOpenWindow)))
				{
					if (largeMotion || ignoreWeigherStatus)
					{
						if (isTracing)
							Debug::trace("IlaWeighMode.ProductTransaction", HobString("Next Product: 0x") + Hex::toString((long long) pNextProductTransaction));
						pCurrentProductTransaction = pNextProductTransaction;
						generateNewPackage(pCurrentProductTransaction);
						if (isTracing)
							Debug::trace("IlaWeighMode.ProductTransaction", HobString("back from generateNewPackage 0x") + Hex::toString((long long) pNextProductTransaction));

						if (ilaOff == true)
						{
                            // HT-317 Keyboard Entered Weight will not print a label
							if( pCurrentProduct != NULL  && pCurrentProduct->getProductNum() > 0 )
							{
								if( (Configuration::getInstance()->isServiceCounterScale()) && (pCurrentProduct->hasKbdNetWeightLabel()) && (pCurrentProduct->isRandomWeight()) )
								{
									Debug::trace("IlaWeighMode", "We are still turkey weigher so set largeMotion = TRUE");
									largeMotion = true;
								}
								else if( Configuration::getInstance()->isServiceCounterScale() )
								{
									openWindow();
								}
							}
							else
							{
								largeMotion = false;    // original code was just this.
							}

						}
						else
						{
							closeWindow();
						}

						if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale())
						{
							// We aren't actually printing a label, we are displaying it to the screen (PJR-82).
							// So we won't be getting back a Label Complete message from the PrinterManager, since there won't be a label, and it won't be taken.
							// The UI will eventually respond back to the Session that the operator is finished with the displayed label, but the Session is not
							// notifying the mode that this happened; the mode still thinks it is printing the label.
							// So, act as if the label is complete once the Printer postImages it.
							// The code is from the handlePrinterMessage() - PrLabelComplete.
							Debug::trace("IlaWeighMode.TestScreenLabel", "handleProductMessage() if ss and printToScreen");

							Debug::trace("IlaWeighMode", HobString("PrLabelComplete - before calling getNewProduct() "));
							emptyPackageMap(false);
							if (getNewProduct() == true)
							{
								Debug::trace("IlaWeighMode", HobString("PrLabelComplete - getNewProduct() returned true "));
								changeProduct();
								preImagePrinters();
								openWindow();
							}

							laStateMachine1 = wmLaInitial;
							Debug::trace("IlaWeighMode.StateMachine", "laStateMachine1 = wmLaInitial");
						}

					}
					else
					{
						sendProductComplete(const_cast <ProductTransaction *> (pNextProductTransaction), false);
						missedProductUpdates++;
					}
				}
				else if (ilaOff == true &&	stateMachine == ilawmOpenWindow)
				{
					if (largeMotion)
					{
						if (pPendingPackage != NULL)
							sendProductComplete((const ProductTransaction *) pPendingPackage, false);

						pPendingPackage = pNextProductTransaction;
						if (isTracing)
							Debug::trace("IlaWeighMode.ProductTransaction", HobString("pPendingPackage: 0x") + Hex::toString((long long) pPendingPackage));
					}
					else
					{
						sendProductComplete(const_cast <ProductTransaction *> (pNextProductTransaction), false);
						missedProductUpdates++;
					}
				}
				else
				{
					bool isTracing2 = Debug::isTracing("IlaWeighMode.MissedProducts");
					if (isTracing2)
					{
						Debug::trace("IlaWeighMode.MissedProducts", HobString("IlaWeighMode -- ERROR!! Missed Product Update: 0x") + HobString(Hex::toString((long long) pNextProductTransaction)));
						Debug::trace("IlaWeighMode.MissedProducts", HobString("packageMap.size(): ") + HobString(Integer::toString((int)packageMap.size())));
						Debug::trace("IlaWeighMode.MissedProducts", HobString("stateMachine: ") + HobString(Integer::toString(stateMachine)));
						Debug::trace("IlaWeighMode.MissedProducts", HobString("ilaOff: ") + HobString(Bool::toString(ilaOff)));
					}

					// This is an error condition!!
					sendProductComplete((const ProductTransaction *) pNextProductTransaction, false);
					missedProductUpdates++;

					if (isTracing2)
						Debug::trace("IlaWeighMode.MissedProducts", HobString("missedProductUpdates: ") + Integer::toString(missedProductUpdates));
				}
			}
			break;
		}

		default:
		{
			break;
		}
	}					
}

void IlaWeighMode::handleLabelApplierMessage(LaEvent laEvent, const char * /* recvBuffer */)
{
	switch (laEvent)
	{
		case LaILAOpenWindow:
		{
			Debug::trace("IlaWeighMode", "handleLabelApplierMessage(LaILAOpenWindow) - Doing nothing!");
//			if (ilaOff == true)
//			{
//				ilaOff = false;
//				Debug::trace("IlaWeighMode.IlaOff", HobString("ilaOff: ") + Bool::toString(ilaOff));
//
//				if (pPendingPackage != NULL)
//				{
//					sendProductComplete((const ProductTransaction *) pPendingPackage, false);
//				}
//
//				pPendingPackage = NULL;
//
//				setWeigherMode(ScalePrepackMode);
//			}
//			else
//			{
//				if (packageMap.size() > 0)
//				{
//					forceVoidLabel = true;
//					Debug::trace("IlaWeighMode.VoidLabels", "setting forceVoidLabel");
//				}
//			}
//
//			if (stateMachine == ilawmCloseWindow)
//			{								
//				openWindow();
//			}
		}
		break;
		case LaILACloseWindow:
		{
			Debug::trace("IlaWeighMode", "handleLabelApplierMessage(LaILACloseWindow) - Doing nothing!");
//			Debug::trace("IlaWeighMode.VoidLabels", HobString("voidLabelCount: ") + Integer::toString(voidLabelCount));
//
//			if (((stateMachine == ilawmChangingProduct) || (stateMachine == ilawmPrinterHeadUp)) == false)
//			{								
//				closeWindow();
//			}
		}
		break;
		case LaILAForceLabel:
		{
			Debug::trace("IlaWeighMode", "handleLabelApplierMessage(LaILAForceLabel) - Doing nothing!");
//			Debug::trace("IlaWeighMode.VoidLabels", HobString("voidLabelCount: ") + Integer::toString(voidLabelCount));
//
//			// If "Label Not Taken" print a Void Label!!!
//			if ((forceVoidLabel == true) && (laStateMachine1 == wmLaLabelReady))
//			{
//				Debug::trace("IlaWeighMode.VoidLabels", "Forcing Void Label ...");
//
//				productFailed();
//				sendVoidLabel(FirstPrinter, pFirstPrinter);
//
//				voidLabelCount++;
//				Debug::trace("IlaWeighMode.VoidLabels", HobString("voidLabelCount: ") + Integer::toString(voidLabelCount));
//
//				laStateMachine1 = wmLaIlaVoidLabel;
//				Debug::trace("IlaWeighMode.StateMachine", "laStateMachine1 = wmLaIlaVoidLabel");
//			}
//
//			forceVoidLabel = false;
//			Debug::trace("IlaWeighMode.VoidLabels", "clearing forceVoidLabel");
		}
		break;
		case LaILAOff:
		{
			Debug::trace("IlaWeighMode", "handleLabelApplierMessage(LaILAOff) - Doing nothing!");
//
//			if (ilaOff == false)
//			{
//				ilaOff = true;
//				Debug::trace("IlaWeighMode.IlaOff", HobString("ilaOff: ") + Bool::toString(ilaOff));
//
//				setWeigherMode(ScaleManualMode);
//				unlockWeigher();
//			}
		}
		break;
		default:
		{
			Debug::trace("IlaWeighMode", "handleLabelApplierMessage(default) - Doing nothing!");
			break;
		}
	}						
}

void IlaWeighMode::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer)
{
	switch (prEvent)
	{
		case PrStatus: 
		{
			if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale())
			{
				if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeSearchKeypadFlashkeys) && (Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors) == SelfServOption_PrintToScreenAndLabel))
					Debug::trace("IlaWeighMode.TestScreenLabel", "IlaWeighMode Don't skip handlePrinterMessage(PrStatus) if BigY");
				else
				{
					Debug::trace("IlaWeighMode.TestScreenLabel", "IlaWeighMode SKIP handlePrinterMessage(PrStatus) if print to screen ONLY");
					return;
				}
			}

			Debug::trace("IlaWeighMode", "handlePrinterMessage(PrStatus)");
			PrinterStatus prStatus(recvBuffer);
			PrinterType printerType = prStatus.getPrinterType();

			if ((printerType == FirstPrinter) && (prStatus.isPriorityError() == true))
			{
				updatePrinterHeadUp(printerType);
				
				if (stateMachine != ilawmPrinterHeadUp)
				{
					productFailed();
					emptyProductQ();
					stateMachine = ilawmPrinterHeadUp;
					Debug::trace("IlaWeighMode.StateMachine", "stateMachine = ilawmPrinterHeadUp");

					laStateMachine1 = wmLaInitial;
					Debug::trace("IlaWeighMode.StateMachine", "laStateMachine1 = wmLaInitial");
				}
			}
			else if ((printerType == FirstPrinter) && (prStatus.isSizeCmdComplete() == true))
			{
				updateSizeCmdComplete(printerType);
				
				if (isPrinterHeadUp() == false)
				{
					if (getNewProduct() == true)
					{
						changeProduct();
						preImagePrinters();	
						openWindow();
					}
					else
					{
						stateMachine = ilawmInitial;
						Debug::trace("IlaWeighMode.StateMachine", "stateMachine = ilawmInitial");
					}

					laStateMachine1 = wmLaInitial;
					Debug::trace("IlaWeighMode.StateMachine", "laStateMachine1 = wmLaInitial");
				}
			}
			else if ((printerType == FirstPrinter) &&
					 ((laStateMachine1 == wmLaInitial) || (laStateMachine1 == wmLaLabelTaken)) &&
					 (prStatus.isLabelReady() == true))
			{
				laStateMachine1 = wmLaLabelReady;
				Debug::trace("IlaWeighMode.StateMachine", "laStateMachine1 = wmLaLabelReady");
			}
			else if ((printerType == FirstPrinter) && 
					((prStatus.isLabelMissing() == true) || 
					 (( (laStateMachine1 == wmLaLabelReady) || (laStateMachine1 == wmLaIlaVoidLabel)) && 
					 (prStatus.isLabelTaken() == true))))
			{
				laStateMachine1 = wmLaLabelTaken;
				Debug::trace("IlaWeighMode.StateMachine", "laStateMachine1 = wmLaLabelTaken");

				unlockWeigher();
			}
			break;
		}
		case PrLabelComplete: 
		{						
			Debug::trace("IlaWeighMode", "handlePrinterMessage(PrLabelComplete)");
			PrinterLabelComplete prLabelComplete(recvBuffer);

			Debug::trace("IlaWeighMode", HobString("PrLabelComplete - before calling updateProductComplete"));
			if (updateProductComplete(prLabelComplete) == true)
			{
				Debug::trace("IlaWeighMode", HobString("PrLabelComplete - updateProductComplete returned true "));
				Debug::trace("IlaWeighMode", HobString("PrLabelComplete - packageMap size is") + HobString(Integer::toString((int)packageMap.size())));
				Debug::trace("IlaWeighMode", HobString("PrLabelComplete - stateMachine == ilawmChangingProduct: ") + Bool::toString((stateMachine == ilawmChangingProduct)));
				if ((packageMap.size() == 0) && (stateMachine == ilawmChangingProduct))
				{
					Debug::trace("IlaWeighMode", HobString("PrLabelComplete - before calling getNewProduct() "));
					if (getNewProduct() == true)
					{
						Debug::trace("IlaWeighMode", HobString("PrLabelComplete - getNewProduct() returned true "));
						changeProduct();
						preImagePrinters();
						openWindow();
					}

					laStateMachine1 = wmLaInitial;
					Debug::trace("IlaWeighMode.StateMachine", "laStateMachine1 = wmLaInitial");
				}

				if ((ilaOff == true) && (pPendingPackage != NULL))
				{
					pCurrentProductTransaction = pPendingPackage;
					pPendingPackage = NULL;
					generateNewPackage(pCurrentProductTransaction);
					largeMotion = false;
				}
			}
			break;
		}	

		case PrConfig:
			Debug::trace("IlaWeighMode", "handlePrinterMessage(PrConfig)");
			handlePrinterConfigInOpMode(recvBuffer);
			break;

		default:
			Debug::trace("IlaWeighMode", "handlePrinterMessage(default)");
			OperatingMode::handlePrinterMessage(prEvent, recvBuffer);
			break;
	}
}

void IlaWeighMode::handleWeigherMessage(WgEvent wgEvent, const char *recvBuffer)
{
	switch (wgEvent)
	{
		case WgStatus:
		{
			Debug::trace("IlaWeighMode", "handleWeigherMessage(WgStatus)");
			WeigherStatus wgStatus(recvBuffer);

			if (wgStatus.isPositiveNonZeroStableWeight() == true)
			{
				Debug::trace("IlaWeighMode.WeigherStatus", "Positive, NonZero, Stable Weight");
			}

			// HA-65
			if ((wgStatus.isPositiveNonZeroStableWeight() == true) && (largeMotion == true) && Configuration::getInstance()->isPrepackScale())
			{
//				Debug::trace("IlaWeighMode", HobString("handleWeigherMessage(WgStatus):  pCurrentProduct = 0x") + Hex::toString((long long) pCurrentProduct));
//				Debug::trace("IlaWeighMode", HobString("handleWeigherMessage(WgStatus):  ilaOff = ") + Bool::toString(ilaOff));
				if ((pCurrentProduct == 0) || (pCurrentProduct->getProductNum() == 0))
				{
					Debug::trace("IlaWeighMode", HobString("handleWeigherMessage(WgStatus):  Sending ProductRequiredToWrap message"));
					opModeStatus.setProductRequiredToWrap();
					dispatchEvent(&opModeStatus);
					largeMotion = false;
					break;
				}
			}

			if (wgStatus.isLargeMotion() == true)
			{
				Debug::trace("IlaWeighMode.WeigherStatus", "Large Motion");
				largeMotion = true;

				if ((ilaOff == true) &&	(pPendingPackage != NULL))
				{
					sendProductComplete(pPendingPackage, false);
					pPendingPackage = NULL;
				}
			}

			if ((ilaOff == true) &&	(pPendingPackage != NULL) && (wgStatus.isZeroStableWeight() == true))
			{
				sendProductComplete(pPendingPackage, false);
				pPendingPackage = NULL;
			}
		}
		break;

		default:
		{
			Debug::trace("IlaWeighMode", "handleWeigherMessage(default):  Do nothing.");
			break;
		}
	}						
}

void IlaWeighMode::firstPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	Debug::trace("IlaWeighMode.StateMachine", "firstPrinterAttached 0x:" + HobString(Hex::toString((long long) pPrinterMgr)));

	_Mybase::firstPrinterAttached(pPrinterMgr);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	openWindow();

	laStateMachine1 = wmLaInitial;
	Debug::trace("IlaWeighMode.StateMachine", "laStateMachine1 = wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("IlaWeighMode.StateMachine", "laStateMachine2 = wmLaInitial");
}

void IlaWeighMode::firstPrinterDetached()
{
	Debug::trace("IlaWeighMode.StateMachine", "firstPrinterDetached 0x:" + HobString(Hex::toString((long long) pFirstPrinter)));

	if (pFirstPrinter)
	{
		bool fake( isFirstPrinterManagerFake() );

		_Mybase::firstPrinterDetached();

		if (!fake)
		{
			productFailed();
			if (pPendingPackage != NULL)
				sendProductComplete((const ProductTransaction *) pPendingPackage, false);
			pPendingPackage = NULL;
		}
	}
}

void IlaWeighMode::secondPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	Debug::trace("IlaWeighMode.StateMachine", "secondPrinterAttached 0x:" + HobString(Hex::toString((long long) pPrinterMgr)));

	_Mybase::secondPrinterAttached(pPrinterMgr);

	if ((getNewProduct() == true) || (pCurrentProduct != NULL))
	{
		changeProduct();
		preImagePrinters();	
	}

	openWindow();

	laStateMachine1 = wmLaInitial;
	Debug::trace("IlaWeighMode.StateMachine", "laStateMachine1 = wmLaInitial");

	laStateMachine2 = wmLaInitial;
	Debug::trace("IlaWeighMode.StateMachine", "laStateMachine2 = wmLaInitial");
}

void IlaWeighMode::secondPrinterDetached()
{
	Debug::trace("IlaWeighMode.StateMachine", "secondPrinterDetached 0x:" + HobString(Hex::toString((long long) pSecondPrinter)));

	if (pSecondPrinter)
	{
		bool fake( isSecondPrinterManagerFake() );

		_Mybase::secondPrinterDetached();

		if (!fake)
		{
			productFailed();
			if (pPendingPackage)
				sendProductComplete((const ProductTransaction *) pPendingPackage, false);
			pPendingPackage = NULL;
		}
	}
}

void IlaWeighMode::productFailed()
{
	if (pCurrentProductTransaction != NULL)
	{
		sendProductComplete(const_cast <ProductTransaction *> (pCurrentProductTransaction), false);
		pCurrentProductTransaction = NULL;
	}

	laStateMachine1 = wmLaInitial;
	Debug::trace("IlaWeighMode.StateMachine", "laStateMachine1 = wmLaInitial");

	emptyPackageMap();
	unlockWeigher();
}

void IlaWeighMode::closeWindow()
{
	stateMachine = ilawmCloseWindow;
	Debug::trace("IlaWeighMode.StateMachine", "stateMachine = ilawmCloseWindow");

	lockWeigher();
}

void IlaWeighMode::openWindow()
{
	if (stateMachine != ilawmOpenWindow)
	{
		stateMachine = ilawmOpenWindow;
		Debug::trace("IlaWeighMode.StateMachine", "stateMachine = ilawmOpenWindow");

		unlockWeigher();
	}
}
