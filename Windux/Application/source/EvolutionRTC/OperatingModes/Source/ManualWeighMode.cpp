
#include "OpModeEvent.h"
#include "ManualWeighMode.h"
#include "Configuration.h"

#include "PrinterManager.h"
#include "PrinterStatus.h"
#include "PrinterLabelComplete.h"

#include "ProductUpdateMsg.h"
#include "ProductChangeMsg.h"

#include "ProductTransaction.h"
using namespace std;

#include "Integer.h"
#include "Hex.h"
#include "HobString.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ManualWeighMode::dataType = 0;
std::string ManualWeighMode::dataTypeName("ManualWeighMode"); 
#endif

ManualWeighMode::ManualWeighMode(const Util::CSmartPtr<Product> &pCurrentProductSmartPtr,
								 //WeigherManager *pWeigherManager,
								 //PrinterManager *pFirstPrinter, 
								 //PrinterManager *pSecondPrinter,
								 //PrinterManager *pThirdPrinter,
								 //LabelApplierManager *pLabelApplierMananger,
								 //WrapperManager *pWrapperManager) :	OperatingMode("tManWeigh",
								 Hobart::CEvolMsgReceiver *pWeigherManager,
								 Hobart::CEvolMsgReceiver *pFirstPrinter, 
								 Hobart::CEvolMsgReceiver *pSecondPrinter,
								 Hobart::CEvolMsgReceiver *pThirdPrinter,
								 Hobart::CEvolMsgReceiver *pLabelApplierMananger,
								 Hobart::CEvolMsgReceiver *pWrapperManager) :	OperatingMode("tManWeigh",
																				  pCurrentProductSmartPtr,
																				  pWeigherManager,
																				  pFirstPrinter,
																				  pSecondPrinter,
																				  pThirdPrinter,
																				  pLabelApplierMananger,
																				  pWrapperManager)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	registerEvent(ProductDev, ProductChange);
	registerEvent(ProductDev, ProductUpdate);
    registerEvent(ProductDev, ProductChangeSmart);

	registerEvent(Printer, PrStatus);
	registerEvent(Printer, PrLabelComplete);
	registerEvent(Printer, PrConfig);

	stateMachine = mwmInitial;
	Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmInitial");

	sendWrapperAbort();

	setLabelApplierMode(Manual);		//	Per Carlos for USB CLA
	setWeigherMode(ScaleManualMode);
	requestWeigherStatus();

	takeLabelTime = 5;
	setTakeLabelTime(FirstPrinter);
	setTakeLabelTime(SecondPrinter);
	setTakeLabelTime(ThirdPrinter);

	if (pCurrentProductSmartPtr.get())
	{
		Util::CSmartPtr<Product> newProductSmartPtr(pCurrentProductSmartPtr);
		handleProductSmartPtr(newProductSmartPtr);
	}
}

ManualWeighMode::ManualWeighMode(Product *pCurrentProduct,
								 //WeigherManager *pWeigherManager,
								 //PrinterManager *pFirstPrinter, 
								 //PrinterManager *pSecondPrinter,
								 //PrinterManager *pThirdPrinter,
								 //LabelApplierManager *pLabelApplierMananger,
								 //WrapperManager *pWrapperManager) :	OperatingMode("tManWeigh",
								 Hobart::CEvolMsgReceiver *pWeigherManager,
								 Hobart::CEvolMsgReceiver *pFirstPrinter, 
								 Hobart::CEvolMsgReceiver *pSecondPrinter,
								 Hobart::CEvolMsgReceiver *pThirdPrinter,
								 Hobart::CEvolMsgReceiver *pLabelApplierMananger,
								 Hobart::CEvolMsgReceiver *pWrapperManager) :	OperatingMode("tManWeigh",
																				  pCurrentProduct,
																				  pWeigherManager,
																				  pFirstPrinter,
																				  pSecondPrinter,
																				  pThirdPrinter,
																				  pLabelApplierMananger,
																				  pWrapperManager)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	//registerEvent(ProductDev, ProductChange);
	registerEvent(ProductDev, ProductUpdate);
    registerEvent(ProductDev, ProductChangeSmart);

	registerEvent(Printer, PrStatus);
	registerEvent(Printer, PrLabelComplete);
	registerEvent(Printer, PrConfig);

	stateMachine = mwmInitial;
	Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmInitial");

	sendWrapperAbort();

	// set the CLA mode to Manual
	setLabelApplierMode(Manual);
	setWeigherMode(ScaleManualMode);
	requestWeigherStatus();

	takeLabelTime = 5;
	setTakeLabelTime(FirstPrinter);
	setTakeLabelTime(SecondPrinter);
	setTakeLabelTime(ThirdPrinter);

	if (pCurrentProduct != NULL)
	{
		ProductChangeMsg pcMsg(this, (const Product *) pCurrentProduct);

		char *pCC = (char *) pcMsg.serialize();
		handleProductMessage((ProductEvent) pcMsg.getEventType(), pCC);
	}
}


ManualWeighMode::~ManualWeighMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	deregisterAllEvents();

	ShutdownNicely();

	setLabelApplierMode(Conveyor);
}				

void ManualWeighMode::weigherAttached(Hobart::CEvolMsgReceiver *pWeigherManager)
{
	_Mybase::weigherAttached( pWeigherManager );

	setWeigherMode(ScaleManualMode);
}

void ManualWeighMode::firstPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	Debug::trace("ManualWeighMode.StateMachine", "firstPrinterAttached 0x:" + HobString(Hex::toString((long long) pPrinterMgr)));

	_Mybase::firstPrinterAttached(pPrinterMgr);

	changeProduct();
	preImagePrinters();	

	stateMachine = mwmWaitingForProductUpdate;
	Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForProductUpdate");
}

void ManualWeighMode::firstPrinterDetached()
{
	Debug::trace("ManualWeighMode.StateMachine", "firstPrinterDetached 0x:" + HobString(Hex::toString((long long) pFirstPrinter)));

	if (pFirstPrinter)
	{
		bool fake( isFirstPrinterManagerFake() );

		_Mybase::firstPrinterDetached();

		if (!fake)
		{
			productFailed();
			stateMachine = mwmInitial;
			Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmInitial");
		}
	}
}

void ManualWeighMode::secondPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::secondPrinterAttached(pPrinterMgr);

	changeProduct();
	preImagePrinters();

	stateMachine = mwmWaitingForProductUpdate;
	Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForProductUpdate");
}

void ManualWeighMode::secondPrinterDetached()
{
	if (pSecondPrinter)
	{
		bool fake( isSecondPrinterManagerFake() );

		_Mybase::secondPrinterDetached();

		if (!fake)
		{
			productFailed();
			stateMachine = mwmInitial;
			Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmInitial");
		}
	}
}

void ManualWeighMode::thirdPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
	_Mybase::thirdPrinterAttached(pPrinterMgr);

	changeProduct();
	preImagePrinters();

	stateMachine = mwmWaitingForProductUpdate;
	Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForProductUpdate");
}

void ManualWeighMode::thirdPrinterDetached()
{
	if (pThirdPrinter)
	{
		bool fake( isThirdPrinterManagerFake() );

		_Mybase::thirdPrinterDetached();

		if (!fake)
		{
			productFailed();
			stateMachine = mwmInitial;
			Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmInitial");
		}
	}
}

void ManualWeighMode::labelApplierAttached(Hobart::CEvolMsgReceiver *pLabelApplierManager)
{
	Debug::trace("ManualWeighMode", "labelApplierAttached()");
	_Mybase::labelApplierAttached( pLabelApplierManager );

	setLabelApplierMode(Manual);		//	Per Carlos for USB CLA
}

void ManualWeighMode::labelApplierDetached()
{
	Debug::trace("ManualWeighMode", "labelApplierDetached()");
	_Mybase::labelApplierDetached();

	productFailed();
}

#include "ProductManager.h"

void ManualWeighMode::handleProductSmartPtr(Util::CSmartPtr<Product> &pCurrentProductSmartPtr)
{
	Product *pProduct = 0;

	pProduct = pCurrentProductSmartPtr.get();

	emptyProductQ();
Debug::trace("ManualWeighMode", "handleProductSmartPtr product*=" + util_string::ptrtostring(pProduct));

	if (pProduct)
		productQ.push(pCurrentProductSmartPtr);
	else
	{
		pCurrentProduct = 0;
		pCurrentProductTransaction = 0;
	}

Debug::trace("ManualWeighMode.StateMachine", "stateMachine is " + util_string::inttostring(stateMachine));

	//if (stateMachine != mwmWrapPrinterHeadUp)
	if (stateMachine == mwmInitial || stateMachine == mwmWaitingForProductUpdate || 
		(Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale()) )
	{
		Debug::trace("ManualWeighMode.TestScreenLabel", "ManualWeighMode Go ahead and do handleProductSmartPtr if printToScreen #1");
		if (getNewProduct() == true)
		{
			changeProduct();
			preImagePrinters();	

			stateMachine = mwmWaitingForProductUpdate;
			Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForProductUpdate");
		}
		else if (Configuration::getInstance()->isSelfServiceScale())
		{
			// there was a system crash when booting up a SelfServe scale, not taking the label that sizes during bootup,
			// lookup a plu, press the 'Start Over' button, and then take the label.  The printer manager was
			// told to pre-image the label, but since sizing hadn't totally finished, the PrinterManager just
			// held on to the pointer to the current product; it would have pe imaged when the sizing finished.
			// But pressing 'Start Over' deleted that current product, but no one passed that along to the
			// PrinterManager, which resulted in it having a pointer to a product that has been deleted by the Session.
			// For now, since self-serve scales are the only ones that don't require the operator taking the label before
			// entering the first plu number, I'm only having this code execute if this is a self-serve scale.

			pCurrentProduct = 0;
			preImagePrinters();	
		}
	}
}


void ManualWeighMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
	switch (productEvent)
	{
		// Operator has chosen a new product
		//case ProductChange:
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
			const ProductTransaction *pNextProductTransaction = prodUpdate.getProductTransaction();

/*
			if (Configuration::getInstance()->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
			{
				if (pNextProductTransaction)
					sendProductComplete(pNextProductTransaction, true);
				break;
			}
*/

			if (stateMachine == mwmWaitingForProductUpdate)
			{
				pCurrentProductTransaction = pNextProductTransaction;

				if ( printerScenario == PrimaryPrinterOnly || printerScenario == SupportTextOnPrimary )
				{
					postImagePrinter(FirstPrinter, pFirstPrinter);

					stateMachine = mwmPrintingPrimary;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmPrintingPrimary");
				}
				else if ( printerScenario == SecondPrinterOnly || printerScenario == SupportTextOnSecondary )
				{
					postImagePrinter(SecondPrinter, pSecondPrinter );

					stateMachine = mwmPrintingSecond;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmPrintingSecond");
				}
				else if ( printerScenario == ThirdPrinterOnly || printerScenario == SupportTextOnTertiary )
				{
					postImagePrinter(ThirdPrinter, pThirdPrinter);

					stateMachine = mwmPrintingThird;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmPrintingThird");
				}
				else if (printerScenario == PrimaryAndSecondPrinter)
				{
					postImagePrinter(FirstPrinter, pFirstPrinter);
					postImagePrinter(SecondPrinter, pSecondPrinter);

					stateMachine = mwmPrintingPrimaryAndSecond;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmPrintingPrimaryAndSecond");
				}
				else if (printerScenario == PrimaryAndThirdPrinter)
				{
					postImagePrinter(FirstPrinter, pFirstPrinter);
					postImagePrinter(ThirdPrinter, pThirdPrinter);

					stateMachine = mwmPrintingPrimaryAndThird;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmPrintingPrimaryAndThird");
				}
				else if (printerScenario == SecondAndThirdPrinter)
				{
					postImagePrinter(SecondPrinter, pSecondPrinter);
					postImagePrinter(ThirdPrinter, pThirdPrinter);

					stateMachine = mwmPrintingSecondAndThird;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmPrintingSecondAndThird");
				}
				else if (printerScenario == PrimarySecondAndThirdPrinter)
				{
					postImagePrinter(FirstPrinter, pFirstPrinter);
					postImagePrinter(SecondPrinter, pSecondPrinter);
					postImagePrinter(ThirdPrinter, pThirdPrinter);

					stateMachine = mwmPrintingPrimarySecondAndThird;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmPrintingPrimarySecondAndThird");
				}
				else
				{
					Debug::trace("ManualWeighMode.Error", "Invalid printerScenario");
				}

				if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale() )
				{
					// We aren't actually printing a label, we are displaying it to the screen (PJR-82).
					// So we won't be getting back a Label Complete message from the PrinterManager, since there won't be a label, and it won't be taken.
					// The UI will eventually respond back to the Session that the operator is finished with the displayed label, but the Session is not
					// notifying the mode that this happened; the mode still thinks it is printing the label.
					// So, act as if the label is complete once the Printer postImages it.
					// The code is from the handlePrinterMessage() - PrLabelComplete.

					Debug::trace("ManualWeighMode.TestScreenLabel", "ManualWeighMode Go ahead and do handleProductMessage if printToScreen #1");

					stateMachine = mwmWaitingForProductUpdate;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForProductUpdate");

					if (getNewProduct() == true)
					{
						changeProduct();
						preImagePrinters();
					}
				}


			}
			else
			{
				// This is an error condition!!
				Debug::trace("ManualWeighMode.Error", "Missed Product Update");
				sendProductComplete(pNextProductTransaction, false);
			}

/*
			if (Configuration::getInstance()->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
			{
				PAL_absclass::PAL_sleep(2000);
				sendProductComplete(pNextProductTransaction, true);
			}
*/

			break;
		}

		default:
		{
			Debug::trace("ManualWeighMode", "handleProductMessage():  ???");
			break;
		}
	}					
}

void ManualWeighMode::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer)
{
	switch (prEvent)
	{
		case PrStatus: 
		{	
			if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale())
			{
				if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeSearchKeypadFlashkeys) && (Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors) == SelfServOption_PrintToScreenAndLabel))
					Debug::trace("ManualWeighMode.TestScreenLabel", "ManualWeighMode Don't skip handlePrinterMessage(PrStatus) if BigY");
				else
				{
					Debug::trace("ManualWeighMode.TestScreenLabel", "ManualWeighMode SKIP handlePrinterMessage(PrStatus) if print to screen ONLY");
					return;
				}
			}

			PrinterStatus prStatus(recvBuffer);
			PrinterType printerType = prStatus.getPrinterType();

			if (prStatus.isHeadDisengaged() == true)
			{
				updatePrinterHeadUp(printerType);
				
				if (stateMachine != mwmWrapPrinterHeadUp)
				{
					stateMachine = mwmWrapPrinterHeadUp;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWrapPrinterHeadUp");

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

						stateMachine = mwmWaitingForProductUpdate;
						Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForProductUpdate");
					}
					else
					{
						stateMachine = mwmInitial;
						Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmInitial");
					}
				}
			}
			break;
		}	
		case PrLabelComplete: 
		{						
			Debug::trace("ManualWeighMode.StateMachine", HobString("mwm - handling PrLabelComplete - stateMachine upon entry is: ") + HobString(Integer::toString(stateMachine)));

			PrinterLabelComplete prLabelComplete(recvBuffer);
			PrinterType printerType = prLabelComplete.getPrinterType();
			const ProductTransaction *pPT = prLabelComplete.getProductTransaction();
			bool printOk = prLabelComplete.isPrintOk();

			if ( (((stateMachine == mwmPrintingPrimary || stateMachine == mwmWaitingForProductUpdate) || (stateMachine == mwmWaitingForPrimary)) && (printerType == FirstPrinter)) ||
				 (((stateMachine == mwmPrintingSecond)  || (stateMachine == mwmWaitingForSecond))  && (printerType == SecondPrinter)) ||
				 (((stateMachine == mwmPrintingThird)   || (stateMachine == mwmWaitingForThird))   && (printerType == ThirdPrinter)) )
			{
				sendProductComplete(pPT, printOk);

				stateMachine = mwmWaitingForProductUpdate;
				Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForProductUpdate");

				if (getNewProduct() == true)
				{
					changeProduct();
					preImagePrinters();
				}
			}
			else
			{
				if (((stateMachine == mwmPrintingPrimaryAndSecond || stateMachine == mwmWaitingForPrimaryAndSecond) && printerType == SecondPrinter) ||
					((stateMachine == mwmPrintingPrimaryAndThird || stateMachine == mwmWaitingForPrimaryAndThird) && printerType == ThirdPrinter))
				{
					stateMachine = mwmWaitingForPrimary;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForPrimary");
				}
				else if (((stateMachine == mwmPrintingPrimaryAndSecond || stateMachine == mwmWaitingForPrimaryAndSecond) && printerType == FirstPrinter) ||
						 ((stateMachine == mwmPrintingSecondAndThird || stateMachine == mwmWaitingForSecondAndThird) && printerType == ThirdPrinter))
				{
					stateMachine = mwmWaitingForSecond;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForSecond");
				}
				else if (((stateMachine == mwmPrintingPrimaryAndThird || stateMachine == mwmWaitingForPrimaryAndThird) && printerType == FirstPrinter) ||
						 ((stateMachine == mwmPrintingSecondAndThird || stateMachine == mwmWaitingForSecondAndThird) && printerType == SecondPrinter))
				{
					stateMachine = mwmWaitingForThird;
					Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForThird");
				}
				else if (stateMachine == mwmPrintingPrimarySecondAndThird)
				{
					if (printerType == FirstPrinter)
					{
						stateMachine = mwmWaitingForSecondAndThird;
						Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForSecondAndThird");
					}
					else if (printerType == SecondPrinter)
					{
						stateMachine = mwmWaitingForPrimaryAndThird;
						Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForPrimaryAndThird");
					}
					else if (printerType == ThirdPrinter)
					{
						stateMachine = mwmWaitingForPrimaryAndSecond;
						Debug::trace("ManualWeighMode.StateMachine", "stateMachine = mwmWaitingForPrimaryAndSecond");
					}
				}
			}
			break;
		}	

		case PrConfig:
			Debug::trace("ManualWeighMode", "handlePrinterMessage(PrConfig)");
			handlePrinterConfigInOpMode(recvBuffer);
			break;

		default:
		{
			OperatingMode::handlePrinterMessage(prEvent, recvBuffer);
			break;
		}
	}
}

void ManualWeighMode::productFailed()
{
	Debug::trace("ManualWeighMode", "productFailed()");

	sendProductComplete(pCurrentProductTransaction, false);
	pCurrentProductTransaction = NULL;

	unlockWeigher();
}
