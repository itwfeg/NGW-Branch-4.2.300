#include "EvolMessage.h"
#include "OperatingMode.h"
#include "RealTimeController.h"

#include "WeigherManager.h"
#include "WeigherNormal.h"
#include "WeigherMode.h"
#include "WeigherLock.h"
#include "WeigherRequestStatus.h"

#include "Product.h"
#include "ProductTransaction.h"
#include "LabelType.h"
#include "Configuration.h"

#include "ProductChangeMsg.h"
#include "ProductCompleteMsg.h"

#include "PrinterManager.h"
#include "PrinterPreImage.h"
#include "PrinterPostImage.h"
#include "PrinterTakeLabelTime.h"
#include "PrinterLabelComplete.h"
#include "PrinterPrintTestLabel.h"
#include "PrinterReset.h"
#include "PrinterNomSize.h"
#include "PrinterStockMetrics.h"
#include "PrinterDeletePackage.h"

#include "LabelApplierManager.h"
#include "LabelApplierMode.h"
#include "LabelApplierStatus.h"
#include "LabelApplierRequestStatus.h"
#include "LabelApplierRequestOff.h"
#include "LabelApplierLabelReady.h"
#include "LabelApplierPackageReady.h"
#include "LabelApplierLabelTaken.h"
#include "LabelApplierRequestInquire.h"
#include "LabelApplierReset.h"
#include "LabelApplierOn.h"
#include "LabelApplierRequestCloseWindow.h"
#include "LabelApplierPositionInfo.h"

#include "WrapperManager.h"
#include "WrapperMode.h"
#include "WrapperAbort.h"
#include "WrapperRequestStatus.h"
#include "WrapperRequestInquire.h"

#include "FpmManager.h"

#include "OpModePackage.h"

#include "ServiceSpeaker.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "Long.h"
#include "HobString.h"
#include "utilstring.h"
#include "Configuration.h"

#include "ManualWeighMode.h"
#include "IlaWeighMode.h"
#include "ProductionEntryMode.h"


using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int OperatingMode::dataType = 0;
std::string OperatingMode::dataTypeName("OperatingMode"); 
#endif

OperatingMode::OperatingMode(
	std::string tName,
	Product *pCurrentProduct,
	Hobart::CEvolMsgReceiver *pWeigherManager,
	Hobart::CEvolMsgReceiver *pFirstPrinter,
	Hobart::CEvolMsgReceiver *pSecondPrinter,
	Hobart::CEvolMsgReceiver *pThirdPrinter,
	Hobart::CEvolMsgReceiver *pLabelApplierManager,
	Hobart::CEvolMsgReceiver *pWrapperManager ) :
		Hobart::CEvolMsgReceiver("OperatingMode"),
		tName(tName),
		pCurrentProduct(pCurrentProduct),
		pCurrentProductTransaction(NULL),
		pPendingPackage(NULL),
		pWeigherManager(pWeigherManager),
		pFirstPrinter(pFirstPrinter),
		pSecondPrinter(pSecondPrinter),
		pThirdPrinter(pThirdPrinter),
		pLabelApplierManager(pLabelApplierManager),
		pWrapperManager(pWrapperManager),
		printerScenario(NoPrinters),
		laSetup(dynamic_cast<Hobart::CEvolMsgReceiver *> (this), 0, NoRotationSelected, LocNoneSelected, 0, NoRotationSelected, LocNoneSelected),
		wrapStateMachine(awmWrapInitial),
		weigherLocked(false),
		takeLabelTime(3),
		packId(1),
		wrapOnlyMode(true),
		machineRunning(false),
		largeMotion(true),
		laStateMachine1(wmLaInitial),
		laStateMachine2(wmLaInitial),
		modeType(UnknownOpMode),
		firstPrinterLabelLength(0),
		secondPrinterLabelLength(0),
		thirdPrinterLabelLength(0),
		ghostPackage(false),
		pCurrentProductSmartPtr(Util::CSmartPtr<Product>()),
		printerHeadUpState(PrinterNoHeadUp),
		pConfig_( Configuration::getInstance() ),
		weigher_( dynamic_cast<WeigherManager*>( pWeigherManager ) ),
		firstPrinter_( dynamic_cast<PrinterManager*>( pFirstPrinter ) ),
		secondPrinter_( dynamic_cast<PrinterManager*>( pSecondPrinter ) ),
		thirdPrinter_( dynamic_cast<PrinterManager*>( pThirdPrinter ) ),
		labelApplier_( dynamic_cast<LabelApplierManager*>( pLabelApplierManager ) ),
		wrapper_( dynamic_cast<WrapperManager*>( pWrapperManager ) )
{ 
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("OperatingMode", HobString("constructor 1  pWeigherManager: 0x") + HobString(Hex::toString((long long) pWeigherManager)));
	Debug::trace("OperatingMode", HobString("constructor 1  pFirstPrinter: 0x") + HobString(Hex::toString((long long) pFirstPrinter)));
	Debug::trace("OperatingMode", HobString("constructor 1  pSecondPrinter: 0x") + HobString(Hex::toString((long long) pSecondPrinter)));
	Debug::trace("OperatingMode", HobString("constructor 1  pThirdPrinter: 0x") + HobString(Hex::toString((long long) pThirdPrinter)));
	Debug::trace("OperatingMode", HobString("constructor 1  pLabelApplierManager: 0x") + HobString(Hex::toString((long long) pLabelApplierManager)));
	Debug::trace("OperatingMode", HobString("constructor 1  pWrapperManager: 0x") + HobString(Hex::toString((long long) pWrapperManager)));

    registerEvent(Printer, PrNomSize);
    registerEvent(Printer, PrStockMetrics);
    requestLabelSize();

	// Label testing code. On only when config (configc.dat or config.dat
	// contains the following entry:
	// opModeStreamingLabelTest=F	# allows psuedo production mode for printing a continuous stream of labels
	if(	pConfig_->isOpModeStreamingLabelTestEnabled())
	{
		setTakeLabelTime(FirstPrinter, 0);
		setTakeLabelTime(SecondPrinter, 0);
		setTakeLabelTime(ThirdPrinter, 0);	
	}
}


OperatingMode::OperatingMode(
	std::string tName,
	const Util::CSmartPtr<Product> &pCurrentProductSmartPtr,
	Hobart::CEvolMsgReceiver *pWeigherManager,
	Hobart::CEvolMsgReceiver *pFirstPrinter,
	Hobart::CEvolMsgReceiver *pSecondPrinter,
	Hobart::CEvolMsgReceiver *pThirdPrinter,
	Hobart::CEvolMsgReceiver *pLabelApplierManager,
	Hobart::CEvolMsgReceiver *pWrapperManager ) :
		Hobart::CEvolMsgReceiver("OperatingMode"),
		tName(tName),
		pCurrentProduct(NULL),
		pCurrentProductTransaction(NULL),
		pPendingPackage(NULL),
		pWeigherManager(pWeigherManager),
		pFirstPrinter(pFirstPrinter),
		pSecondPrinter(pSecondPrinter),
		pThirdPrinter(pThirdPrinter),
		pLabelApplierManager(pLabelApplierManager),
		pWrapperManager(pWrapperManager),
		printerScenario(NoPrinters),
		laSetup(dynamic_cast<Hobart::CEvolMsgReceiver *> (this), 0, NoRotationSelected, LocNoneSelected, 0, NoRotationSelected, LocNoneSelected),
		wrapStateMachine(awmWrapInitial),
		weigherLocked(false),
		takeLabelTime(3),
		packId(1),
		wrapOnlyMode(true),
		machineRunning(false),
		largeMotion(true),
		laStateMachine1(wmLaInitial),
		laStateMachine2(wmLaInitial),
		modeType(UnknownOpMode),
		thirdPrinterLabelLength(0),
		ghostPackage(false),
		pCurrentProductSmartPtr(Util::CSmartPtr<Product>()),
		printerHeadUpState(PrinterNoHeadUp),
		pConfig_( Configuration::getInstance() ),
		weigher_( dynamic_cast<WeigherManager*>( pWeigherManager ) ),
		firstPrinter_( dynamic_cast<PrinterManager*>( pFirstPrinter ) ),
		secondPrinter_( dynamic_cast<PrinterManager*>( pSecondPrinter ) ),
		thirdPrinter_( dynamic_cast<PrinterManager*>( pThirdPrinter ) ),
		labelApplier_( dynamic_cast<LabelApplierManager*>( pLabelApplierManager ) ),
		wrapper_( dynamic_cast<WrapperManager*>( pWrapperManager ) )
{ 
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("OperatingMode", HobString("constructor 2  pWeigherManager: 0x") + HobString(Hex::toString((long long) pWeigherManager)));
	Debug::trace("OperatingMode", HobString("constructor 2  pFirstPrinter: 0x") + HobString(Hex::toString((long long) pFirstPrinter)));
	Debug::trace("OperatingMode", HobString("constructor 2  pSecondPrinter: 0x") + HobString(Hex::toString((long long) pSecondPrinter)));
	Debug::trace("OperatingMode", HobString("constructor 2  pThirdPrinter: 0x") + HobString(Hex::toString((long long) pThirdPrinter)));
	Debug::trace("OperatingMode", HobString("constructor 2  pLabelApplierManager: 0x") + HobString(Hex::toString((long long) pLabelApplierManager)));
	Debug::trace("OperatingMode", HobString("constructor 2  pWrapperManager: 0x") + HobString(Hex::toString((long long) pWrapperManager)));

    registerEvent(Printer, PrNomSize);
    registerEvent(Printer, PrStockMetrics);
    requestLabelSize();

	pCurrentProduct = pCurrentProductSmartPtr.get();
}

OperatingMode::~OperatingMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	unlockWeigher();

	// this will be done in DTOR or CEvolMsgReceiver
	//ShutdownNicely();
}

void OperatingMode::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
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

void OperatingMode::handleProductMessage(ProductEvent productEvent, const char *recvBuffer)
{
	if (productEvent)
	{};

	if (recvBuffer)
	{};

	Debug::trace("OperatingMode", "handleProductMessage()");
	//switch (productEvent)
	//{
	//  default:
	//  {
	//  }
	//}								 
}

void OperatingMode::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer)
{
    Debug::trace("OperatingMode", HobString("handlePrinterMessage - prEvent:") + inttostring(prEvent));

    switch (prEvent)
    {
        case PrNomSize:
            {
                PrinterNomSize nomSize(recvBuffer);
                bool sendSetup = false;

				Debug::trace("OperatingMode.PrinterMessage", "Got PrNomSize message. PrinterType = " +
						inttostring(nomSize.getPrinterType()) + ", ActualSize = " + inttostring(nomSize.getActualSize()));

				switch (nomSize.getPrinterType())
				{
					case FirstPrinter:
						if (laSetup.getPrimaryLabelSize() != nomSize.getActualSize())
						{
							Debug::trace("OperatingMode.PrinterMessage", "Going to set PrimaryLabelSize");

							firstPrinterLabelLength = nomSize.getActualSize();
							laSetup.setPrimaryLabelSize(firstPrinterLabelLength);
							sendSetup = true;
						}
						break;
					case SecondPrinter:
						if (laSetup.getSecondaryLabelSize() != nomSize.getActualSize())
						{
							secondPrinterLabelLength = nomSize.getActualSize();
							laSetup.setSecondaryLabelSize(secondPrinterLabelLength);
							sendSetup = true;
						}
						break;
					case ThirdPrinter:
						thirdPrinterLabelLength = nomSize.getActualSize();
						break;
					default:
						break;
				}

                if ((pCurrentProduct != NULL) && (sendSetup == true))
                {
                    sendLabelApplierSetup(*pCurrentProduct);
                }
            }
            break;
        case PrStockMetrics:
            {
				Debug::trace("OperatingMode", HobString("handlePrinterMessage - PrStockMetrics"));

				PrinterStockMetrics prMetrics(recvBuffer);
				Debug::trace("OperatingMode", string("handlePrinterMessage - PrStockMetrics - calling setTakeLabelTime printerType:") + inttostring(prMetrics.getPrinterType()));
				setTakeLabelTime(prMetrics.getPrinterType());

				switch (prMetrics.getPrinterType())
				{
					case FirstPrinter:
						firstPrinterLabelLength = prMetrics.getLabelLengthInLines();
						laSetup.setPrimaryLabelSize(firstPrinterLabelLength);
						break;
					case SecondPrinter:
						secondPrinterLabelLength = prMetrics.getLabelLengthInLines();
						laSetup.setSecondaryLabelSize(secondPrinterLabelLength);
						break;
					case ThirdPrinter:
						thirdPrinterLabelLength = prMetrics.getLabelLengthInLines();
						break;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}
}

void OperatingMode::handleWrapperMessage(WrEvent wrEvent, const char *recvBuffer)
{
	switch (wrEvent)
	{
		case WrStatus:
			handleWrapperStatus(recvBuffer);
			break;
		case WrEvolStatus:
			handleWrapperEvolStatus(recvBuffer);
			break;
		default:
			{
				break;
			}
	}										   
}

void OperatingMode::handleLabelApplierMessage(LaEvent laEvent, const char * /* recvBuffer */)
{
	if (laEvent)
	{};

	//switch (laEvent)
	//{
	//  default:
	//  {
	//		  break;
	//  }
	//}										 
}

void OperatingMode::handleWeigherMessage(WgEvent wgEvent, const char *recvBuffer)
{
	if (wgEvent)
	{};

	if (recvBuffer)
	{};

	//switch (wgEvent)
	//{
	//  default:
	//  {
	//		  break;
	//  }
	//}										 
}

void OperatingMode::firstPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
    firstPrinter_ = dynamic_cast<PrinterManager*>( pFirstPrinter = pPrinterMgr );

    Debug::trace("OperatingMode", HobString("attaching pFirstPrinter: 0x") + HobString(Hex::toString((long long) pFirstPrinter)));
    setTakeLabelTime(FirstPrinter);
}

void OperatingMode::firstPrinterDetached()
{
    Debug::trace("OperatingMode", HobString("detaching pFirstPrinter: 0x") + HobString(Hex::toString((long long) pFirstPrinter)));
    pFirstPrinter = firstPrinter_ = NULL;
}

bool OperatingMode::isFirstPrinterManagerFake()
{
	return (( firstPrinter_ ) && ( firstPrinter_->isFakePrinterManager() ));
}

void OperatingMode::secondPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
    secondPrinter_ = dynamic_cast<PrinterManager*>( pSecondPrinter = pPrinterMgr );

    Debug::trace("OperatingMode", HobString("attaching pSecondPrinter: 0x") + HobString(Hex::toString((long long) pSecondPrinter)));
    setTakeLabelTime(SecondPrinter);
}

void OperatingMode::secondPrinterDetached()
{
    Debug::trace("OperatingMode", HobString("detaching pSecondPrinter: 0x") + HobString(Hex::toString((long long) pSecondPrinter)));
    pSecondPrinter = secondPrinter_ = NULL;
}

bool OperatingMode::isSecondPrinterManagerFake()
{
	return (( secondPrinter_ ) && ( secondPrinter_->isFakePrinterManager() ));
}

void OperatingMode::thirdPrinterAttached(Hobart::CEvolMsgReceiver *pPrinterMgr)
{
    thirdPrinter_ = dynamic_cast<PrinterManager*>( pThirdPrinter = pPrinterMgr );

    Debug::trace("OperatingMode", HobString("attaching pThirdPrinter: 0x") + HobString(Hex::toString((long long) pThirdPrinter)));
    setTakeLabelTime(ThirdPrinter);
}

void OperatingMode::thirdPrinterDetached()
{
    Debug::trace("OperatingMode", HobString("detaching pThirdPrinter: 0x") + HobString(Hex::toString((long long) pThirdPrinter)));
    pThirdPrinter = thirdPrinter_ = NULL;
}

bool OperatingMode::isThirdPrinterManagerFake()
{
	return (( thirdPrinter_ ) && ( thirdPrinter_->isFakePrinterManager() ));
}

void OperatingMode::labelApplierAttached(Hobart::CEvolMsgReceiver *pLabelApplierMgr)
{
	labelApplier_ = dynamic_cast<LabelApplierManager*>( pLabelApplierManager = pLabelApplierMgr );
}

void OperatingMode::labelApplierDetached()
{
	pLabelApplierManager = labelApplier_ = NULL;
}

void OperatingMode::wrapperAttached(Hobart::CEvolMsgReceiver *pWrapperMgr, WrapperSetWrapMode::WrapMode wrapMode)
{
	Debug::trace("OperatingMode", "wrapperAttached()");
	wrapper_ = dynamic_cast<WrapperManager*>( pWrapperManager = pWrapperMgr );
}

void OperatingMode::wrapperDetached()
{
	Debug::trace("OperatingMode", "wrapperDetached()");
	pWrapperManager = wrapper_ = NULL;
}

void OperatingMode::weigherAttached(Hobart::CEvolMsgReceiver *pWeigherMgr)
{
	weigher_ = dynamic_cast<WeigherManager*>( pWeigherManager = pWeigherMgr );
}

void OperatingMode::weigherDetached()
{
	pWeigherManager = weigher_ = NULL;
}

OperatingMode::OpModeType OperatingMode::getModeType()
{
	return modeType;
}

void OperatingMode::changeProduct()
{
	Debug::trace("OperatingMode", "changeProduct() entered");
	emptyPackageMap();
	largeMotion = true;

	setPrinterScenario();
}

void OperatingMode::setPrinterScenario()
{
	if (firstPrinter_)
		firstPrinter_->setAllLabelsToBePrinted(false);
	if (secondPrinter_)
		secondPrinter_->setAllLabelsToBePrinted(false);
	if (thirdPrinter_)
		thirdPrinter_->setAllLabelsToBePrinted(false);

    printerScenario = NoPrinters;
	bool simPrint = Configuration::getInstance()->toBoolCfgI(CfgInternal::SIMULATEPRINTING);
	if (simPrint)
		printerScenario = PrimaryPrinterOnly;

    if (pCurrentProduct != NULL)
    {
		if ( pCurrentProduct->isDummyProduct() )
		{
			if ( pCurrentProduct->getLabelType(FirstPrinter) && pFirstPrinter )
				printerScenario = SupportTextOnPrimary;
			else if ( pCurrentProduct->getLabelType(SecondPrinter) && pSecondPrinter )
				printerScenario = SupportTextOnSecondary;
			else if ( pCurrentProduct->getLabelType(ThirdPrinter) && pThirdPrinter )
				printerScenario = SupportTextOnTertiary;

			Debug::trace("OperatingMode.changeProduct", HobString("printing support text, printerScenario:") + inttostring((int)printerScenario) );
		}
		else
		{
			if ( firstPrinter_ )
			{
				LabelType *cpLabel = const_cast<Product *> (pCurrentProduct)->getLabelType(FirstPrinter);
				Debug::trace("OperatingMode.changeProduct", HobString("FirstPrinter cpLabel: 0x") + HobString(Hex::toString((long long) (cpLabel))));
				if ( cpLabel )
					Debug::trace("OperatingMode.changeProduct", HobString("FirstPrinter getLabelMediaError: ") + HobString(Integer::toString(cpLabel->getLabelMediaError())));

				if ( cpLabel && cpLabel->isLabelMediaError() == false )
				{
					firstPrinter_->setLabelToBePrinted(FirstPrinter, true);
					printerScenario = PrimaryPrinterOnly;
				}

			if (firstPrinter_ && firstPrinter_->isVirtualPrintingEnabled())
			{
				cpLabel = const_cast<Product *> (pCurrentProduct)->getLabelType(SecondPrinter);
				Debug::trace("OperatingMode.changeProduct", HobString("second cpLabel: 0x") + HobString(Hex::toString((long long) (cpLabel))));
				if (cpLabel != NULL)
					Debug::trace("OperatingMode.changeProduct", HobString("second getLabelMediaError: ") + HobString(Integer::toString(cpLabel->getLabelMediaError())));

					if ( cpLabel && cpLabel->isLabelMediaError() == true )
						printerScenario = NoPrinters;
					else if ( cpLabel )
					{
						if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgVirtualPrintingOverride2ndPrinter) && secondPrinter_ &&
							 	( dynamic_cast<ManualWeighMode*>(this) || dynamic_cast<IlaWeighMode*>(this) || dynamic_cast<ProductionEntryMode*>(this) ) )
						{
							firstPrinter_->setLabelToBePrinted(SecondPrinter, false);
							secondPrinter_->setLabelToBePrinted(SecondPrinter, true);

							if (printerScenario == NoPrinters)
								printerScenario = SecondPrinterOnly;
							else
								printerScenario = PrimaryAndSecondPrinter;
						}
						else
						{
							firstPrinter_->setLabelToBePrinted(SecondPrinter, true);
							if (printerScenario == NoPrinters)
								printerScenario = PrimaryPrinterOnly;
						}
					}

					cpLabel = const_cast<Product *> (pCurrentProduct)->getLabelType(ThirdPrinter);
					Debug::trace("OperatingMode.changeProduct", HobString("third cpLabel: 0x") + HobString(Hex::toString((long long) (cpLabel))));
					if ( cpLabel )
						Debug::trace("OperatingMode.changeProduct", HobString("third getLabelMediaError: ") + HobString(Integer::toString(cpLabel->getLabelMediaError())));

					if ( cpLabel && cpLabel->isLabelMediaError() == true )
						printerScenario = NoPrinters;
					else if ( cpLabel )
					{
						if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgVirtualPrintingOverride3rdPrinter) && thirdPrinter_ &&
								( dynamic_cast<ManualWeighMode*>(this) || dynamic_cast<ProductionEntryMode*>(this) ) )
						{
							firstPrinter_->setLabelToBePrinted(ThirdPrinter, false);
							thirdPrinter_->setLabelToBePrinted(ThirdPrinter, true);

							if ( printerScenario == NoPrinters )
								printerScenario = ThirdPrinterOnly;
							else if ( printerScenario == PrimaryPrinterOnly )
								printerScenario = PrimaryAndThirdPrinter;
							else if ( printerScenario == SecondPrinterOnly )
								printerScenario = SecondAndThirdPrinter;
							else if ( printerScenario == PrimaryAndSecondPrinter )
								printerScenario = PrimarySecondAndThirdPrinter;
						}
						else
						{
							firstPrinter_->setLabelToBePrinted(ThirdPrinter, true);
							if (printerScenario == NoPrinters)
								printerScenario = PrimaryPrinterOnly;
							else if (printerScenario == SecondPrinterOnly)
								printerScenario = PrimaryAndSecondPrinter;
						}
					}

				}
			}
			else
			{
				Debug::trace("OperatingMode.changeProduct", "pFirstPrinter == NULL");
			}


			Debug::trace("OperatingMode.changeProduct", HobString("setPrinterScenario - after looking at FirstPrinter printerScenario: ") + HobString(Integer::toString(printerScenario)));

			if ( secondPrinter_ && ( !firstPrinter_ || firstPrinter_->isVirtualPrintingEnabled() == false ) )
			{
				LabelType *cpLabel = 0;
				if ( pCurrentProduct )
					cpLabel = const_cast<Product *> (pCurrentProduct)->getLabelType(SecondPrinter);

				if ( cpLabel && cpLabel->isLabelMediaError() == false )
				{
					secondPrinter_->setLabelToBePrinted(SecondPrinter, true);

					if (printerScenario == NoPrinters)
						printerScenario = SecondPrinterOnly;
					else
						printerScenario = PrimaryAndSecondPrinter;
				}
			}
			else
			{
				Debug::trace("OperatingMode.changeProduct", "pSecondPrinter == NULL");
			}


			Debug::trace("OperatingMode.changeProduct", HobString("setPrinterScenario - after looking at SecondPrinter printerScenario: ") + HobString(Integer::toString(printerScenario)));

			if ( thirdPrinter_ && ( !firstPrinter_ || firstPrinter_->isVirtualPrintingEnabled() == false ) )
			{
				LabelType *cpLabel = 0;
				if ( pCurrentProduct )
					cpLabel = const_cast<Product *> (pCurrentProduct)->getLabelType(ThirdPrinter);

				if ( cpLabel && cpLabel->isLabelMediaError() == false )
				{
					thirdPrinter_->setLabelToBePrinted(ThirdPrinter, true);

					if ( printerScenario == NoPrinters )
						printerScenario = ThirdPrinterOnly;
					else if ( printerScenario == PrimaryPrinterOnly )
						printerScenario = PrimaryAndThirdPrinter;
					else if ( printerScenario == SecondPrinterOnly )
						printerScenario = SecondAndThirdPrinter;
					else if ( printerScenario == PrimaryAndSecondPrinter )
						printerScenario = PrimarySecondAndThirdPrinter;
				}
			}

			Debug::trace("OperatingMode.changeProduct", HobString("setPrinterScenario - after looking at ThirdPrinter printerScenario: ") + HobString(Integer::toString(printerScenario)));

		}
    }
    else
    {
        Debug::trace("OperatingMode.changeProduct", "pCurrentProduct == NULL");
    }

	if (pCurrentProduct != NULL)
	{
		Debug::trace("OperatingMode", "changeProduct() calling sendLabelApplierSetup()");
		sendLabelApplierSetup(*pCurrentProduct);
	}

	Debug::trace("OperatingMode.changeProduct", HobString("leaving setPrinterScenario - printerScenario: ") + HobString(Integer::toString(printerScenario)));

}

void OperatingMode::unlockWeigher()
{
	Debug::trace("OperatingMode.Weigher", HobString("unlockWeigher():  weigherLocked = ") + booltostring(weigherLocked));

	if ( weigherLocked )
	{
		WeigherNormal wgNormal(this);
		dispatchEvent(&wgNormal);

		weigherLocked = false;
	}
}

void OperatingMode::lockWeigher()
{
	Debug::trace("OperatingMode.Weigher", HobString("lockWeigher"));

	WeigherLock wgLock(this);
	dispatchEvent(&wgLock);

	weigherLocked = true;
}

void OperatingMode::setTakeLabelTime(PrinterType printerType, UINT32 tlTime)
{
	Debug::trace("OperatingMode", string("OperatingMode::setTakeLabelTime - printer:") + inttostring(printerType) + "  time:" + longtostring(tlTime));
	takeLabelTime = tlTime;
	setTakeLabelTime(printerType);
}

void OperatingMode::setTakeLabelTime(PrinterType printerType)
{
	UINT32 theTime = takeLabelTime;

	bool isTracing = ( Debug::isTracing("OperatingMode") ) ? true : false;
	if (isTracing)
	{
		Debug::trace("OperatingMode", HobString("setTakeLabelTime - ignoring label taken? ") + booltostring(Configuration::getInstance()->toBoolCfg(fieldOffsetCfgContinuousStockIgnoreLabelTaken)));

		if ( printerType == FirstPrinter  && firstPrinter_ )
    		Debug::trace("OperatingMode", HobString("setTakeLabelTime - 1st prt cont? ") + booltostring(firstPrinter_->isContinuousStock()));

		if ( printerType == SecondPrinter  && secondPrinter_ )
    		Debug::trace("OperatingMode", HobString("setTakeLabelTime - 2nd prt cont? ") + booltostring(secondPrinter_->isContinuousStock()));

		if ( printerType == ThirdPrinter  && thirdPrinter_ )
    		Debug::trace("OperatingMode", HobString("setTakeLabelTime - 3rd prt cont? ") + booltostring(thirdPrinter_->isContinuousStock()));
	}

	if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgContinuousStockIgnoreLabelTaken) &&
	     ( ( printerType == FirstPrinter  && firstPrinter_  && firstPrinter_->isContinuousStock()  ) ||
	       ( printerType == SecondPrinter && secondPrinter_ && secondPrinter_->isContinuousStock() ) ||
	       ( printerType == ThirdPrinter  && thirdPrinter_  && thirdPrinter_->isContinuousStock()  ) ||
		   ( Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgIIgnoreLabelTakenDieCut) ) ) ) // ignore even in die cut stock
		theTime = 0;

	if (isTracing)
	{
		Debug::trace("OperatingMode", HobString("setTakeLabelTime - printerType:") + inttostring((int)printerType));
		Debug::trace("OperatingMode", HobString("setTakeLabelTime - theTime:") + inttostring(theTime));
	}

    PrinterTakeLabelTime prTakeLabelTime(this, printerType, theTime);
    dispatchEvent(&prTakeLabelTime);
}

bool OperatingMode::getNewProduct()
{
	bool rv = false;

	if (productQ.empty() == false)
	{
		Debug::trace("OperatingMode", HobString("getNewProduct() - product queue length before popping product is: ") + HobString(Integer::toString((int)productQ.size())));

		//pCurrentProduct = productQ.front();
		pCurrentProductSmartPtr = productQ.front();
		pCurrentProduct = pCurrentProductSmartPtr.get();

		pCurrentProductTransaction = NULL; // SOF-5071

		productQ.pop();

		Debug::trace("OperatingMode", HobString("getNewProduct() - current product set from queue to: ") + HobString(Hex::toString((long long)pCurrentProduct)));
		rv = true;
	}
	else
		Debug::trace("OperatingMode", HobString("getNewProduct() - product queue is empty"));

	return rv;
}

void OperatingMode::emptyProductQ()
{
	Debug::trace("OperatingMode", HobString("emptyProductQ() - emptying product queue"));
	while (productQ.empty() == false)
	{
		productQ.pop();
	}
}

void OperatingMode::preImagePrinters()
{
	if ((pFirstPrinter != NULL) &&
		((printerScenario == PrimaryPrinterOnly) || (printerScenario == PrimaryPrinterBakeryCLA)
												 || (printerScenario == PrimaryAndSecondPrinter)
												 || (printerScenario == PrimaryPrinterAndSecondPrinterBakeryCLA)
												 || (printerScenario == PrimaryAndThirdPrinter)
												 || (printerScenario == PrimarySecondAndThirdPrinter)
		 || printerScenario == SupportTextOnPrimary))
	{
		preImagePrinter(FirstPrinter, pFirstPrinter);
	}

	if ((pSecondPrinter != NULL) &&
		((printerScenario == SecondPrinterOnly) || (printerScenario == SecondPrinterBakeryCLA)
												   || (printerScenario == PrimaryAndSecondPrinter)
												   || (printerScenario == PrimaryPrinterAndSecondPrinterBakeryCLA)
												   || (printerScenario == SecondAndThirdPrinter)
												   || (printerScenario == PrimarySecondAndThirdPrinter)
		 || printerScenario == SupportTextOnSecondary))
	{
		preImagePrinter(SecondPrinter, pSecondPrinter);
	}

	if ((pThirdPrinter != NULL) &&
		((printerScenario == ThirdPrinterOnly) || (printerScenario == PrimaryAndThirdPrinter)
												   || (printerScenario == SecondAndThirdPrinter)
												   || (printerScenario == PrimarySecondAndThirdPrinter)
		 || printerScenario == SupportTextOnTertiary))
	{
		preImagePrinter(ThirdPrinter, pThirdPrinter);
	}
}

//void OperatingMode::preImagePrinter(PrinterType printerType, PrinterManager *pMgr)
void OperatingMode::preImagePrinter(PrinterType printerType, Hobart::CEvolMsgReceiver *pMgr)
{
	if (pMgr != NULL)
	{
		Debug::trace("PrinterManager.PrinterPreImage", HobString("OperatingMode::preImagePrinter"));

		PrinterPreImage preImage(this, printerType, &pCurrentProductSmartPtr);

		const char *pCC = preImage.serialize();
		int mLength = preImage.getSerializedLength();

		pMgr->send(pCC, mLength);
	}
}

//void OperatingMode::postImagePrinter(PrinterType printerType, PrinterManager *pMgr, UINT8 packId)
void OperatingMode::postImagePrinter(PrinterType printerType, Hobart::CEvolMsgReceiver *pMgr, UINT8 packId)
{
	Debug::trace("OperatingMode.Printer", "In postImagePrinter");

	if (pMgr != NULL)
	{
		PrinterPostImage postImage(this, printerType, pCurrentProductTransaction, packId, &pCurrentProductSmartPtr);

		const char *pCC = postImage.serialize();
		int mLength = postImage.getSerializedLength();

		Debug::trace("OperatingMode.Printer", "Sending PrinterPostImage printer to printer-type " +
				inttostring(printerType));

		pMgr->send(pCC, mLength);
	}
}

void OperatingMode::generateNewPackage(const ProductTransaction *pPT)
{
	bool traceOn = Debug::isTracing("OperatingMode");
	HobString msgDebug("generateNewPackage - ");
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("pCurrentProductTransaction = ").append(Hex::toString((long long) pCurrentProductTransaction)).append("\n");
		msgDebug2.append("pPT = ").append(Hex::toString((long long) pPT)).append("   And is not used at all here");
		Debug::trace("OperatingMode", msgDebug2);
	}

	if (pPT == NULL)
		return;

	if (pCurrentProductTransaction == NULL)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("pCurrentProductTransaction is NULL, exiting!!!");
			Debug::trace("OperatingMode", msgDebug2);
		}
		return;
	}
	OpModePackage *pPackage = new OpModePackage(pCurrentProductTransaction, packId);
	packageMap.insert(make_pair(pCurrentProductTransaction->getTransactionNum(), pPackage));

    HobString pScenario = "printerScenario = ";
    if ( printerScenario == PrimaryPrinterOnly || printerScenario == SupportTextOnPrimary )
    {
        pScenario.append("PrimaryPrinterOnly||SupportTextOnPrimary");
        pPackage->setFirstPrinter(pFirstPrinter);
        postImagePrinter(FirstPrinter, pFirstPrinter, packId);
    }
	else if (printerScenario == SecondPrinterOnly || printerScenario == SupportTextOnSecondary )
	{
		pScenario.append("SecondPrinterOnly||SupportTextOnSecondary");
		pPackage->setSecondPrinter(pSecondPrinter);
		postImagePrinter(SecondPrinter, pSecondPrinter, packId);
	}
	else if ( printerScenario == SupportTextOnTertiary )
	{
		pScenario.append("SupportTextOnTertiary");
        pPackage->setSecondPrinter(pThirdPrinter);
        postImagePrinter(ThirdPrinter, pThirdPrinter, packId);
    }
	else if (printerScenario == PrimaryAndSecondPrinter)
	{
		pScenario.append("PrimaryAndSecondPrinter");
		pPackage->setFirstPrinter(pFirstPrinter);
		pPackage->setSecondPrinter(pSecondPrinter);
		postImagePrinter(FirstPrinter, pFirstPrinter, packId);
		postImagePrinter(SecondPrinter, pSecondPrinter, packId);
	}
	else if (printerScenario == PrimaryAndThirdPrinter)
	{
		pScenario.append("PrimaryAndThirdPrinter");
		pPackage->setFirstPrinter(pFirstPrinter);
		pPackage->setThirdPrinter(pThirdPrinter);
		postImagePrinter(FirstPrinter, pFirstPrinter, packId);
		postImagePrinter(ThirdPrinter, pThirdPrinter, packId);
	}
	else if (printerScenario == SecondAndThirdPrinter)
	{
		pScenario.append("SecondAndThirdPrinter");
		pPackage->setSecondPrinter(pSecondPrinter);
		pPackage->setThirdPrinter(pThirdPrinter);
		postImagePrinter(SecondPrinter, pSecondPrinter, packId);
		postImagePrinter(ThirdPrinter, pThirdPrinter, packId);
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
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append(pScenario);
		Debug::trace("OperatingMode", msgDebug2);
	}

	if (Debug::isTracing("WrapperMode.PackId"))
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("pushing packId: ").append(Integer::toString(packId)).append("\n");
		msgDebug2.append("packageMap.size(): ").append(Integer::toString((int)packageMap.size()));
		Debug::trace("WrapperMode.PackId", msgDebug2);
	}
	incPackId();
}

const OpModePackage *OperatingMode::getPackage(UINT16 packId)
{
	PackageMap::iterator pos = packageMap.begin();

	OpModePackage *pPackage = NULL;
	while ((pos != packageMap.end()) && (pPackage == NULL))
	{
		if ((pos->second)->getPackId() == packId)
		{
			pPackage = pos->second;
		}
		else
		{
			pos++;
		}
	}

	return pPackage;	
}

void OperatingMode::deletePrinterPackage(PrinterType prType, UINT16 packId)
{
	if (prType)
	{};

	const ProductTransaction *pPT = removePackId(packId);

	if (pPT != NULL)
	{
		sendProductComplete(pPT, false);

		// The following two lines were moved to the sendProductComplete method!!! 3/18/06 LAP
		//PrinterDeletePackage prDeletePackage(this, prType, pPT);
		//dispatchEvent(&prDeletePackage);
	}
}

const ProductTransaction *OperatingMode::removePackId(UINT16 packId)
{
	Debug::trace("OperatingMode", "removePackId():  packId = " + inttostring(packId));

	const ProductTransaction *rv = NULL;
	PackageMap::iterator pos = packageMap.begin();

	OpModePackage *pPackage = NULL;
	while ((pos != packageMap.end()) && (pPackage == NULL))
	{
		if ((pos->second)->getPackId() == packId)
		{
			pPackage = pos->second;
		}
		else
		{
			pos++;
		}
	}

	if (pPackage != NULL)
	{
		rv = pPackage->getProductTransaction();

		Debug::trace("OperatingMode", "removePackId():  Deleting 0x0" + HobString(Hex::toString((long long) pPackage)));
		delete pPackage;
		packageMap.erase(pos);
	}

	return rv;
}

UINT8 OperatingMode::getPackId(const ProductTransaction *pPT)
{
	UINT8 rv = 0;

	if (pPT != NULL)
	{
		PackageMap::iterator pos = packageMap.find(pPT->getTransactionNum());

		if (pos != packageMap.end())
		{
			rv = (pos->second)->getPackId();
		}
	}

	return rv;
}

void OperatingMode::invalidatePackage(const ProductTransaction *pPT)
{
	PackageMap::iterator pos = packageMap.find(pPT->getTransactionNum());

	if (pos != packageMap.end())
	{
		(pos->second)->setInvalidPackage();
	}
}

UINT8 OperatingMode::incPackId()
{
	UINT8 packId = this->packId + 1;
	this->packId = packId ? packId : 1; 

	Debug::trace("OperatingMode", "incPackId():  New packId = " + HobString(Integer::toString(this->packId)));
	return this->packId;
}

void OperatingMode::emptyPackageMap(bool alsoSendProductCompleteMsg)
{
	Debug::trace("OperatingMode", "entering emptyPackageMap()");

	PackageMap::iterator pos = packageMap.begin();
	while (pos != packageMap.end())
	{
		OpModePackage *pPackage = pos->second;

		if ((pPackage != NULL) && (pPackage->getProductTransaction() != NULL))
		{
			if (alsoSendProductCompleteMsg)
				sendProductComplete(pPackage->getProductTransaction(), false);

			if (pCurrentProductTransaction != NULL && pCurrentProductTransaction == pPackage->getProductTransaction())
			{
				Debug::trace("OperatingMode", "emptyPackageMap() - setting pCurrentProductTransaction to NULL");
				pCurrentProductTransaction = NULL;
			}

			delete pPackage;
		}

		packageMap.erase(pos++);
	}

	Debug::trace("OperatingMode", "exiting emptyPackageMap()");
}

bool OperatingMode::updateProductComplete(PrinterLabelComplete &prLabelComplete)
{
	bool traceOn = Debug::isTracing("OperatingMode");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "updateProductComplete():  ";

	bool rv = false;
	HobRecordNum recordNum = prLabelComplete.getRecordNumber();
	const ProductTransaction *pPT = prLabelComplete.getProductTransaction();

	PackageMap::iterator pos;
	OpModePackage *pPackage = NULL;

	if (traceOn)
	{
		Debug::trace("OperatingMode", msgDebug + " packageMap.size(): " + HobString(Integer::toString((int)packageMap.size())));
		Debug::trace("OperatingMode", msgDebug + "packageMap.empty(): " + HobString(Bool::toString(packageMap.empty())));
		Debug::trace("OperatingMode", msgDebug + "         recordNum: " + Integer::toString(recordNum));
	}

	if (!packageMap.empty() && (pos = packageMap.find(recordNum)) != packageMap.end())
	{
		if (traceOn)
			Debug::trace("OperatingMode", msgDebug + "Found the transaction in the packageMap");
		pPackage = pos->second;
	}

	if (pPackage != NULL)
	{
		if (prLabelComplete.isPrintOk() == true)
		{
			if (traceOn)
				Debug::trace("OperatingMode", msgDebug + "isPrintOk is true");
			PrinterType prType = prLabelComplete.getPrinterType();

			if (prType == FirstPrinter)
			{
				if (traceOn)
					Debug::trace("OperatingMode", msgDebug + "FirstPrinter completed.");
				pPackage->setCompleted(pFirstPrinter);
			}
			else if (prType == SecondPrinter)
			{
				if (traceOn)
					Debug::trace("OperatingMode", msgDebug + "SecondPrinter completed.");
				pPackage->setCompleted(pSecondPrinter);
			}
			else if(prType == ThirdPrinter)
			{
				if (traceOn)
					Debug::trace("OperatingMode", msgDebug + "ThirdPrinter completed.");
				pPackage->setCompleted(pThirdPrinter);
			}

			if (pPackage->isTotalCompleted() == true)
			{
				if (traceOn)
					Debug::trace("OperatingMode", msgDebug + "All done. Calling sendProductComplete - deleting from packageMap");
				sendProductComplete(pPT, !pPackage->getInvalidPackage());

				packageMap.erase(pos);
				delete pPackage;

				rv = true;
			}
		}
		else
		{
			if (traceOn)
				Debug::trace("OperatingMode", msgDebug + "isPrintOk is false!  Deleting from packageMap");
			sendProductComplete(pPT, false);

			packageMap.erase(pos);
			delete pPackage;
		}

		if (pCurrentProductTransaction != NULL && pCurrentProductTransaction == pPT)
		{
			if (traceOn)
				Debug::trace("OperatingMode", msgDebug + "Setting pCurrentProductTransaction to NULL.");
			pCurrentProductTransaction = NULL;
		}
		else
			Debug::trace("OperatingMode", msgDebug + "pCurrentProductTransaction already set to NULL");
	}
	else
	{
		if (traceOn)
			Debug::trace("OperatingMode", msgDebug + "Did NOT find the transaction in the packageMap!");
            if( pPT )
            {
                sendProductComplete(pPT, false);
                Debug::trace("OperatingMode", msgDebug + "Sending product complete failed since it isn't null");
            }
	}
	return rv;
}

void OperatingMode::sendProductComplete(const ProductTransaction *pCPT, bool packageSuccess)
{
	bool traceOn = Debug::isTracing("OperatingMode.Product");
	HobString msgDebug("sendProductComplete");
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("(pCPT = 0x0").append(Hex::toString((long long) pCPT));
		msgDebug2.append(", packageSuccess = ").append(Bool::toString(packageSuccess)).append(")");
		Debug::trace("OperatingMode.Product", msgDebug2);
		msgDebug.append("():   ");
	}

	if (pConfig_->toBoolCfgI(CfgInternal::SIMULATEPRINTING))
		packageSuccess = true;

	if (pCPT != NULL)
	{
		
		if (pConfig_->isPrepackScale() == true)
		{
			//tell the printer to delete from queue before we remove the product
			if (!packageSuccess)
			{
				PrinterDeletePackage prDeletePackage1(this, FirstPrinter, pCPT);
				dispatchEvent(&prDeletePackage1);

				PrinterDeletePackage prDeletePackage2(this, SecondPrinter, pCPT);
				dispatchEvent(&prDeletePackage2);

				PrinterDeletePackage prDeletePackage3(this, ThirdPrinter, pCPT);
				dispatchEvent(&prDeletePackage3);
			}			
		}
		
		ProductCompleteMsg prodCompMessage(this, pCPT->getTransactionNum(), pCPT, packageSuccess);
		dispatchEvent(&prodCompMessage);

		if (packageSuccess == true)
		{
			Debug::trace("OperatingMode.Product", HobString("Product Success: ") + HobString(Hex::toString(pCPT->getTransactionNum())));
		}
		else
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				const Product * theProduct = pCPT->getProduct();

				msgDebug2.append("For this transaction:\n");
				msgDebug2.append("   product = ").append(util_string::ptrtostring(theProduct)).append("\n");

				if (theProduct)
				{
					msgDebug2.append("   productNum = ").append(Long::toString(theProduct->getProductNum())).append("\n");
					msgDebug2.append("   description = ").append(theProduct->getDescription()).append("\n");
					msgDebug2.append("Product Failed: ").append(Hex::toString(pCPT->getTransactionNum()));
				}
				Debug::trace("OperatingMode.Product", msgDebug2);
			}
		}
	}
}

bool OperatingMode::isPrinterHeadUp()
{
	return !(printerHeadUpState == PrinterNoHeadUp);
}

void OperatingMode::updatePrinterHeadUp(PrinterType printerType)
{
	switch (printerType)
	{
		case FirstPrinter:
			if (!((printerHeadUpState == PrinterHeadUp_1)  ||
						(printerHeadUpState == PrinterHeadUp_12) ||
						(printerHeadUpState == PrinterHeadUp_13) ||
						(printerHeadUpState == PrinterHeadUp_123)))
			{
				switch (printerHeadUpState)
				{
					case PrinterHeadUp_2:	   printerHeadUpState = PrinterHeadUp_12;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_12");
												break;
					case PrinterHeadUp_3:	   printerHeadUpState = PrinterHeadUp_13;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_13");
												break;
					case PrinterHeadUp_23:	  printerHeadUpState = PrinterHeadUp_123;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_123");
												break;
					default:					printerHeadUpState = PrinterHeadUp_1;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_1");
												break;
				}
			}
			break;
		case SecondPrinter:
			if (!((printerHeadUpState == PrinterHeadUp_2)  ||
						(printerHeadUpState == PrinterHeadUp_12) ||
						(printerHeadUpState == PrinterHeadUp_23) ||
						(printerHeadUpState == PrinterHeadUp_123)))
			{
				switch (printerHeadUpState)
				{
					case PrinterHeadUp_1:	   printerHeadUpState = PrinterHeadUp_12;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_12");
												break;
					case PrinterHeadUp_3:	   printerHeadUpState = PrinterHeadUp_23;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_23");
												break;
					case PrinterHeadUp_13:	  printerHeadUpState = PrinterHeadUp_123;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_123");
												break;
					default:					printerHeadUpState = PrinterHeadUp_2;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_2");
												break;
				}
			}
			break;
		case ThirdPrinter:
			if (!((printerHeadUpState == PrinterHeadUp_3)  ||
						(printerHeadUpState == PrinterHeadUp_13) ||
						(printerHeadUpState == PrinterHeadUp_23) ||
						(printerHeadUpState == PrinterHeadUp_123)))
			{
				switch (printerHeadUpState)
				{
					case PrinterHeadUp_1:	   printerHeadUpState = PrinterHeadUp_13;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_13");
												break;
					case PrinterHeadUp_2:	   printerHeadUpState = PrinterHeadUp_23;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_23");
												break;
					case PrinterHeadUp_12:	  printerHeadUpState = PrinterHeadUp_123;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_123");
												break;
					default:					printerHeadUpState = PrinterHeadUp_3;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_3");
												break;
				}
			}
			break;
		default:						Debug::trace("OperatingMode.PrinterHeadUp", "Unknown Printhead");
										break;
	}
}

void OperatingMode::updateSizeCmdComplete(PrinterType printerType)
{
	switch (printerType)
	{
		case FirstPrinter:
			if (((printerHeadUpState == PrinterHeadUp_1)  ||
						(printerHeadUpState == PrinterHeadUp_12) ||
						(printerHeadUpState == PrinterHeadUp_13) ||
						(printerHeadUpState == PrinterHeadUp_123)))
			{
				switch (printerHeadUpState)
				{
					case PrinterHeadUp_1:	   printerHeadUpState = PrinterNoHeadUp;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterNoHeadUp");
												break;
					case PrinterHeadUp_12:	  printerHeadUpState = PrinterHeadUp_2;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_2");
												break;
					case PrinterHeadUp_13:	  printerHeadUpState = PrinterHeadUp_3;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_3");
												break;
					case PrinterHeadUp_123:	 printerHeadUpState = PrinterHeadUp_23;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_23");
												break;
					default:					Debug::trace("OperatingMode.PrinterHeadUp", "first printhead supposedly not up");
												break;
				}
			}
			break;
		case SecondPrinter:
			if (((printerHeadUpState == PrinterHeadUp_2)  ||
						(printerHeadUpState == PrinterHeadUp_12) ||
						(printerHeadUpState == PrinterHeadUp_23) ||
						(printerHeadUpState == PrinterHeadUp_123)))
			{
				switch (printerHeadUpState)
				{
					case PrinterHeadUp_2:	   printerHeadUpState = PrinterNoHeadUp;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterNoHeadUp");
												break;
					case PrinterHeadUp_12:	  printerHeadUpState = PrinterHeadUp_1;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_1");
												break;
					case PrinterHeadUp_23:	  printerHeadUpState = PrinterHeadUp_3;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_3");
												break;
					case PrinterHeadUp_123:	 printerHeadUpState = PrinterHeadUp_13;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_13");
												break;
					default:					Debug::trace("OperatingMode.PrinterHeadUp", "second printhead supposedly not up");
												break;
				}
			}
			break;
		case ThirdPrinter:
			if (((printerHeadUpState == PrinterHeadUp_3)  ||
						(printerHeadUpState == PrinterHeadUp_13) ||
						(printerHeadUpState == PrinterHeadUp_23) ||
						(printerHeadUpState == PrinterHeadUp_123)))
			{
				switch (printerHeadUpState)
				{
					case PrinterHeadUp_3:	   printerHeadUpState = PrinterNoHeadUp;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterNoHeadUp");
												break;
					case PrinterHeadUp_13:	  printerHeadUpState = PrinterHeadUp_1;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_1");
												break;
					case PrinterHeadUp_23:	  printerHeadUpState = PrinterHeadUp_2;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_2");
												break;
					case PrinterHeadUp_123:	 printerHeadUpState = PrinterHeadUp_12;
												Debug::trace("OperatingMode.PrinterHeadUp", "printerHeadUpState: = PrinterHeadUp_12");
												break;
					default:					Debug::trace("OperatingMode.PrinterHeadUp", "third printhead supposedly not up");
												break;
				}
			}
			break;
		default:						Debug::trace("OperatingMode.PrinterHeadUp", "Unknown Printhead");
										break;
	}

	setTakeLabelTime(printerType);
}

void OperatingMode::sendVoidLabel(PrinterType prType, PrinterManager *pPrinter)
{
	if (pPrinter != NULL)
	{
		ServiceSpeaker::badBeep(3);

		PrinterPrintTestLabel voidLabel(this, prType, PrVoidLabel);

		const char *pCC = voidLabel.serialize();
		int mLength = voidLabel.getSerializedLength();

		pPrinter->send(pCC, mLength);
	}
}

void OperatingMode::setWeigherMode(WgOperatingMode opMode)
{
	if (pWeigherManager != NULL)
	{
		WeigherMode wm(this, opMode);

		const char *pCC = wm.serialize();
		pWeigherManager->send(pCC,  wm.getSerializedLength());
	}
}

void OperatingMode::requestWeigherStatus()
{
	if (pWeigherManager != NULL)
	{
		WeigherRequestStatus reqStatus(this);

		const char *pCC = reqStatus.serialize();
		pWeigherManager->send(pCC,  reqStatus.getSerializedLength());
	}
}

void OperatingMode::setLabelApplierMode(LaOperatingMode opMode)
{
	//	Note:	We don't send LabelApplierMode messages to the EvolLabelApplier,
	//			but we do send them to the new USB CompactLabelApplier. [CCD SEP 18, 2013]
	if ((pWrapperManager == NULL) && (pLabelApplierManager != NULL))
	{
		Debug::trace("OperatingMode", HobString("setLabelApplierMode():  LaOperatingMode = ") + HobString(Integer::toString((int)opMode)));
		LabelApplierMode lam(this, opMode);

		const char *pCC = lam.serialize();
		pLabelApplierManager->send(pCC,  lam.getSerializedLength());
	}
	else
	{
		Debug::trace("OperatingMode", HobString("setLabelApplierMode():  Problem!  pWrapperManager = ") + HobString(Hex::toString((long long)pWrapperManager)) +
										HobString(",  pLabelApplierManager = ") + HobString(Hex::toString((long long)pLabelApplierManager)));
	}
}

void OperatingMode::requestLabelApplierStatus()
{
	//	Note:	We don't send LabelApplierRequestStatus messages to the EvolLabelApplier,
	//			but we do send them to the new USB CompactLabelApplier. [CCD SEP 18, 2013]
	if ((pWrapperManager == NULL) && (pLabelApplierManager != NULL))
	{
		LabelApplierRequestStatus reqStatus(this);

		const char *pCC = reqStatus.serialize();
		pLabelApplierManager->send(pCC,  reqStatus.getSerializedLength());
	}
	else
	{
		Debug::trace("OperatingMode", HobString("requestLabelApplierStatus():  Problem!  pWrapperManager = ") + HobString(Hex::toString((long long)pWrapperManager)) +
										HobString(",  pLabelApplierManager = ") + HobString(Hex::toString((long long)pLabelApplierManager)));
	}
}

void OperatingMode::setWrapperMode(WrOpMode opMode)
{
	if (pWrapperManager != NULL)
	{
		WrapperMode wm(this, opMode);

		const char *pCC = wm.serialize();
		pWrapperManager->send(pCC,  wm.getSerializedLength());
	}
}

void OperatingMode::setLabelApplierOff()
{
	//message is not supported by EvolWrapper
	if ((pWrapperManager == NULL) && (pLabelApplierManager != NULL))
	{
		LabelApplierRequestOff laOff(this);

		const char *pCC = laOff.serialize();
		pLabelApplierManager->send(pCC,  laOff.getSerializedLength());
	}
	else
	{
		Debug::trace("OperatingMode", HobString("setLabelApplierOff():  Problem!  pWrapperManager = ") + HobString(Hex::toString((long long)pWrapperManager)) +
										HobString(",  pLabelApplierManager = ") + HobString(Hex::toString((long long)pLabelApplierManager)));
	}
}

void OperatingMode::sendWrapperTest(WrapperTest::WrTestCode testCode)
{
	WrapperTest wt(this, testCode);
	const char *pCC = wt.serialize();
	pWrapperManager->send(pCC, wt.getSerializedLength());
}

void OperatingMode::sendWrapperAbort()
{
	WrapperManager *mgr;

	if ( !pWrapperManager )
		return;

	// this message is not supported by the EvolWrapper
	if (( mgr = dynamic_cast<WrapperManager*>( pWrapperManager ) ) &&
		( !mgr->isWrapperEvolWrapper() ))
	{
		WrapperAbort wrAbort(this);

		const char *pCC = wrAbort.serialize();
		pWrapperManager->send(pCC,  wrAbort.getSerializedLength());
	}
}

void OperatingMode::requestWrapperStatus()
{
	if (pWrapperManager != NULL)
	{
		WrapperRequestStatus reqStatus(this);

		const char *pCC = reqStatus.serialize();
		pWrapperManager->send(pCC,  reqStatus.getSerializedLength());
	}
}

void OperatingMode::requestWrapperInquire()
{
	if (pWrapperManager != NULL)
	{
		WrapperRequestInquire reqInquire(this);

		const char *pCC = reqInquire.serialize();
		pWrapperManager->send(pCC,  reqInquire.getSerializedLength());
	}
}

void OperatingMode::handlePrinterOverflow(UINT16 maxLabels)
{
	bool of1 = packageMapOverflow(FirstPrinter, maxLabels);
	bool of2 = packageMapOverflow(SecondPrinter, maxLabels);

	if (of1 || of2)
	{
		emptyPackageMap();
		sendPrinterReset(FirstPrinter);
		sendPrinterReset(SecondPrinter);

		if (of1)
		{
			sendPrinterOverflow(FirstPrinter);
		}

		if (of2)
		{
			sendPrinterOverflow(SecondPrinter);
		}
	}
}

bool OperatingMode::packageMapOverflow(PrinterType prType, UINT16 maxLabels)
{
	PackageMap::iterator pos = packageMap.begin();
	UINT16 count = 0;

	while (pos != packageMap.end() && (count < maxLabels))
	{
		OpModePackage *pPackage = pos->second;

		if (pPackage->isCompleted(prType) == false)
		{
			count++;
		}

		pos++;
	}

	return (count >= maxLabels);		
}

void OperatingMode::sendPrinterReset(PrinterType prType)
{
	PrinterReset prReset(this, prType);
	dispatchEvent(&prReset);
}

void OperatingMode::sendPrinterResetImmediate(PrinterType prType)
{
	PrinterReset prReset(this, prType);
	prReset.setEventType(PrResetImmediate);
	dispatchEvent(&prReset);
}

void OperatingMode::sendPrinterOverflow(PrinterType prType)
{
	opModeStatus.setPrinterOverflow(prType);
	dispatchEvent(&opModeStatus);

	// clear the error! The operator must hit cancel!
	opModeStatus.setPrinterOverflow(prType, false);
}

void OperatingMode::setNewEvolWrapperError(const EvolWrapperErrorInfo::EvolWrapperError &wrapperError,
		const EvolWrapperErrorInfo::EvolWrapperErrorSource &wrapperErrorSource)
{
	Debug::trace("OperatingMode", "setNewEvolWrapperError(" + inttostring(wrapperError) + ", " + inttostring(wrapperErrorSource) + ")");
	opModeStatus.setNewEvolWrapperError(wrapperError, wrapperErrorSource);
	dispatchEvent(&opModeStatus);
}

void OperatingMode::setNewWrapperError(const WrapperErrorInfo::WrapperErrorSource &wrapperError)
{
	Debug::trace("OperatingMode", "setNewWrapperError()");
	opModeStatus.setNewWrapperError(wrapperError);
	dispatchEvent(&opModeStatus);
}

void OperatingMode::clearPressStart()
{
	Debug::trace("OperatingMode", "clearPressStart()");
	opModeStatus.clearWrapperPressStart();
	dispatchEvent(&opModeStatus);
}

void OperatingMode::setPressStart()
{
	Debug::trace("OperatingMode", "setPressStart()");
	opModeStatus.setWrapperPressStart();
	dispatchEvent(&opModeStatus);
}

void OperatingMode::clearGhostPackage()
{
	Debug::trace("OperatingMode", "clearGhostPackage()");
	opModeStatus.clearWrapperGhostPackage();
	dispatchEvent(&opModeStatus);
}

void OperatingMode::setGhostPackage()
{
	Debug::trace("OperatingMode", "setGhostPackage()");
	opModeStatus.setWrapperGhostPackage();
	dispatchEvent(&opModeStatus);
}

void OperatingMode::clearPackageOverWeight()
{
	Debug::trace("OperatingMode", "clearPackageOverWeight()");
	opModeStatus.clearWrapperOverWeight();
	dispatchEvent(&opModeStatus);
}

void OperatingMode::setPackageOverWeight()
{
	Debug::trace("OperatingMode", "setPackageOverWeight()");
	opModeStatus.setWrapperOverWeight();
	clearPressStart();		//	Don't want them to press the Start button at this point.
}

void OperatingMode::resetWrapper()
{
	bool traceOn = Debug::isTracing("WrapperMode.StateMachine");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "resetWrapper():  ";

	Debug::trace("OperatingMode", "resetWrapper()");
	unlockWeigher();
	setLabelApplierOff();

	if (wrapStateMachine != awmWrapError)
	{
		wrapStateMachine = awmWrapInitial;
		laStateMachine1 = wmLaIdle;
		laStateMachine2 = wmLaIdle;
		laStateMachine3 = wmLaIdle;
		if (traceOn)
		{
			Debug::trace("WrapperMode.StateMachine", msgDebug + "wrapStateMachine: awmWrapInitial");
			Debug::trace("WrapperMode.StateMachine", msgDebug + "laStateMachine1:  wmLaIdle");
			Debug::trace("WrapperMode.StateMachine", msgDebug + "laStateMachine2:  wmLaIdle");
			Debug::trace("WrapperMode.StateMachine", msgDebug + "laStateMachine3:  wmLaIdle");
		}
	}
	else
	{
		laStateMachine1 = (laStateMachine1 == wmLaLabelReady) ? wmLaError : wmLaIdle;
		laStateMachine2 = (laStateMachine2 == wmLaLabelReady) ? wmLaError : wmLaIdle;
		laStateMachine3 = (laStateMachine3 == wmLaLabelReady) ? wmLaError : wmLaIdle;
		if (traceOn)
		{
			Debug::trace("WrapperMode.StateMachine", msgDebug + "laStateMachine1a: " + HobString((laStateMachine1 == wmLaError) ? "wmLaError" : "wmLaIdle"));
			Debug::trace("WrapperMode.StateMachine", msgDebug + "laStateMachine2a: " + HobString((laStateMachine2 == wmLaError) ? "wmLaError" : "wmLaIdle"));
			Debug::trace("WrapperMode.StateMachine", msgDebug + "laStateMachine3a: " + HobString((laStateMachine3 == wmLaError) ? "wmLaError" : "wmLaIdle"));
		}
	}
	setLabelApplierMode(Conveyor);
}

void OperatingMode::resetWrapperMode()
{
#if 0	/*
		*	Carlos Removed 10/2/14 in Windux
		*	This interferes and causes issues when handling
		*	Wrapper errors via the status message. It will actually
		*	endup sending Printer Resets continously.
		*/
	Debug::trace("OperatingMode", "resetWrapperMode()");
	wrapStateMachine = awmWrapPrinterHeadUp;
	Debug::trace("WrapperMode.StateMachine", "wrapStateMachine: awmWrapPrinterHeadUp");
#endif

	laStateMachine1 = wmLaIdle;
	Debug::trace("WrapperMode.StateMachine", "resetWrapperMode():  laStateMachine1 = wmLaIdle");

	laStateMachine2 = wmLaIdle;
	Debug::trace("WrapperMode.StateMachine", "resetWrapperMode():  laStateMachine2 = wmLaIdle");

	laStateMachine3 = wmLaIdle;
	Debug::trace("WrapperMode.StateMachine", "resetWrapperMode():  laStateMachine3 = wmLaIdle");

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

	unlockWeigher();
}

void OperatingMode::handleWrapperStatus(const char *recvBuffer)
{
	Debug::trace("OperatingMode", "handleWrapperStatus()");
	WrapperStatus wrStatus(recvBuffer);
	WrapperStateInfo wrStateInfo = wrStatus.getStatusInfo().getStateInfo();
	WrapperErrorInfo::WrapperErrorSource errorSource = wrStatus.getErrorInfo().getErrorSource();

	if (errorSource != opModeStatus.getWrapperError())
	{
		// leave the error message up until the stop switch is cycled
		if ((errorSource != WrapperErrorInfo::NoErrorSource) ||
				((errorSource == WrapperErrorInfo::NoErrorSource) && (wrStateInfo.isStandBy())))
		{
			setNewWrapperError(errorSource);

			if (errorSource == WrapperErrorInfo::NoErrorSource)
			{
				Debug::trace("WrapperMode.WrapperError", "-------------------------------Wrapper Error Cleared!");
			}
		}
	}

	if (errorSource != WrapperErrorInfo::NoErrorSource)
	{
		if (wrapStateMachine == awmWrapError)
		{
			Debug::trace("WrapperMode.WrapperError", "-------------------------------Wrapper already detected!");
		}
		else
		{
			Debug::trace("WrapperMode.WrapperError", "-------------------------------Wrapper Error Detected!");
		}
	}
}

void OperatingMode::handleWrapperEvolStatus(const char *recvBuffer)
{
	if (recvBuffer)
	{};
}

void OperatingMode::handleWrapperInquire(const char *recvBuffer)
{
	if (recvBuffer)
	{};
}

void OperatingMode::handleWrapperControl(const char *recvBuffer)
{
	if (recvBuffer)
	{};
}

void OperatingMode::sendLabelReady(LaStationID stationId, UINT8 packageId)
{
	if (pCurrentProduct != NULL)
	{
		LabelApplierLabelReady labelReady(this, stationId, packageId);
		dispatchEvent(&labelReady);
	}
}

void OperatingMode::sendPackageReady(UINT8 packageId)
{
	//Debug::trace("WrapperMode.WrapperStatus", "OperatingMode::sendPackageReady");

	if (pCurrentProduct != NULL || wrapOnlyMode)
	{
		//Debug::trace("WrapperMode.WrapperStatus", "OperatingMode::sendPackageReady actually sending message because pCurrentProduct != NULL or wrapOnlyMode is true");
		LabelApplierPackageReady laPackageReady(this, packageId);
		dispatchEvent(&laPackageReady);
	}
}

void OperatingMode::sendLabelTaken(LaStationID stationId, UINT8 packageId)
{

	if (pCurrentProduct != NULL)
	{
		Debug::trace("LabelApplierManager.Message", "OperatingMode::sendLabelTaken");
		LabelApplierLabelTaken labelTaken(this, stationId, packageId);
		dispatchEvent(&labelTaken);
	}
}

void OperatingMode::requestLabelApplierInquire()
{
	Debug::trace("OperatingMode", "requestLabelApplierInquire()");
	LabelApplierRequestInquire lari(this);
	dispatchEvent(&lari);
}

void OperatingMode::resetLabelApplier()
{
	Debug::trace("OperatingMode", "resetLabelApplier()");
	LabelApplierReset laReset(this);
	dispatchEvent(&laReset);
}

void OperatingMode::sendLabelApplierOn()
{
	Debug::trace("OperatingMode", "sendLabelApplierOn()");
	LabelApplierOn laOn(this);
	dispatchEvent(&laOn);
}

void OperatingMode::sendLabelApplierCloseWindow(UINT16 packId)
{
	Debug::trace("OperatingMode", "sendLabelApplierCloseWindow(" + inttostring(packId) + ")");
	LabelApplierRequestCloseWindow closeWindow(this, (UINT8)packId);
	dispatchEvent(&closeWindow);
}

LaPosLoc OperatingMode::setLabelPlacement(LaPosLoc laPlacement)
{
	LaPosLoc rv = (LaPosLoc) Configuration::getInstance()->getDefaultLabelPlacement();

	if (laPlacement != LocNoneSelected)
	{
		rv = laPlacement;
	}

	return rv;
}

void OperatingMode::sendLabelApplierSetup(const Product &product)
{
	if (Debug::isTracing("OperatingMode"))
	{
		Debug::trace("OperatingMode", " entering sendLabelApplierSetup()");
	}

	HobRecordNum num = product.getProductNum();
	Debug::trace("OperatingMode", "Number: " + longtostring(num));

	if (FpmManager::getInstance()->isWrapperTwoPrinter())
	{
		int secondLabelLength = 0;
		int secondaryLabelRotated = 0;
		LaPosLoc secondLabelPosition = LocNoneSelected;
		short secondLabelRotation = 0;

		// check if PLU contains second label
		LabelType *secondLabelType = product.getLabelType(SecondPrinter);
		if(secondLabelType)
		{
			secondLabelLength = secondPrinterLabelLength;
			secondaryLabelRotated = Configuration::getInstance()->toIntCfg(fieldOffsetCfgSecondaryLabelStockRotated);
			secondLabelPosition = (LaPosLoc)product.getLabelPlacement(SecondPrinter);
			secondLabelRotation = product.getLabelRotation(SecondPrinter);

			if( secondLabelPosition == LocNoneSelected ) // system default
			{
				secondLabelPosition = (LaPosLoc)product.getScaleConfig()->getSecDefaultLabelPlacement();
			}
		}

		LaPosLoc primaryLabelPosition = (LaPosLoc)product.getLabelPlacement(FirstPrinter);
		if( primaryLabelPosition == LocNoneSelected ) // system default
		{
			primaryLabelPosition = (LaPosLoc)product.getScaleConfig()->getDefaultLabelPlacement();
		}

		Debug::trace("LabelApplierManager.SendTwoLabelPositions", "OperatingMode::sendLabelApplierSetup() first: " + Integer::toString(firstPrinterLabelLength));
		Debug::trace("LabelApplierManager.SendTwoLabelPositions", "OperatingMode::sendLabelApplierSetup() second: " + Integer::toString(secondPrinterLabelLength));

		Debug::trace("LabelApplierManager.SendTwoLabelPositions", "OperatingMode::sendLabelApplierSetup() first placement: " + Integer::toString(product.getLabelPlacement(FirstPrinter)));
		Debug::trace("LabelApplierManager.SendTwoLabelPositions", "OperatingMode::sendLabelApplierSetup() second placement: " + Integer::toString(product.getLabelPlacement(SecondPrinter)));

		Debug::trace("LabelApplierManager.SendTwoLabelPositions", "OperatingMode::sendLabelApplierSetup() first rotation: " + Integer::toString(product.getLabelRotation(FirstPrinter)));
		Debug::trace("LabelApplierManager.SendTwoLabelPositions", "OperatingMode::sendLabelApplierSetup() second rotation: " + Integer::toString(product.getLabelPlacement(SecondPrinter)));

		Debug::trace("OperatingMode", "wrapOnlyMode: " + booltostring(wrapOnlyMode));

		// This will stop the printer plate positioning screen from popping when we are in wrap only mode and request a new PLU
		int firstLabelLength = firstPrinterLabelLength;
		if( wrapOnlyMode )
		{
			firstLabelLength = 0;
			secondLabelLength = 0;
		}

		LabelApplierPositionInfo newMsg(
			this,
			0,
			product.getLabelRotation(FirstPrinter),
			secondLabelRotation,
			setLabelPlacement(primaryLabelPosition),
			secondLabelPosition,
			firstLabelLength,
			secondLabelLength,
			secondaryLabelRotated
		);

		dispatchEvent(&newMsg);
	}
	else
	{
	    laSetup.setPrimaryLabelRotationDegrees(product.getLabelRotation(FirstPrinter));
	    laSetup.setPrimaryLabelPlacement(setLabelPlacement((LaPosLoc) product.getLabelPlacement(FirstPrinter)));
	    laSetup.setSecondaryLabelRotationDegrees(product.getLabelRotation(SecondPrinter));
	    laSetup.setSecondaryLabelPlacement(setLabelPlacement((LaPosLoc) product.getLabelPlacement(SecondPrinter)));

	    if (Debug::isTracing("OperatingMode"))
	    {
			Debug::trace("OperatingMode", "sendLabelApplierSetup() on a one printer system");
		    laSetup.show();
	    }
	    dispatchEvent(&laSetup);
	}

	PAL_absclass::PAL_sleep( 0 );	   // ***** WARNING: Win for rescheduling, how about Linux ?
}

void OperatingMode::requestLabelSize()
{
	PrinterNomSize nomSize(this, FirstPrinter);

    if (pFirstPrinter)
    {
        nomSize.setPrinterType(FirstPrinter);

		const char *pCC = nomSize.serialize();
		int mLength = nomSize.getSerializedLength();

		pFirstPrinter->send(pCC, mLength);
	}

    if (pSecondPrinter)
    {
        nomSize.setPrinterType(SecondPrinter);

		const char *pCC = nomSize.serialize();
		int mLength = nomSize.getSerializedLength();

		pSecondPrinter->send(pCC, mLength);
	}

    if (pThirdPrinter)
    {
        nomSize.setPrinterType(ThirdPrinter);

		const char *pCC = nomSize.serialize();
		int mLength = nomSize.getSerializedLength();

		pThirdPrinter->send(pCC, mLength);
	}

    if (pCurrentProduct)
    {
        sendLabelApplierSetup(*pCurrentProduct);
    }
}


void OperatingMode::changeProduct2(const Product *pProduct)
{
	bool traceOn = Debug::isTracing("OperatingMode.changeProduct");
	if (traceOn)
		Debug::trace("OperatingMode.changeProduct", "////////  changeProduct2(): entering");

    if ( pCurrentProduct != pProduct || printerScenario == NoPrinters )
    {
        pCurrentProduct = pProduct;
		pCurrentProductTransaction = NULL; // SOF-5071

        printerScenario = NoPrinters;

        if (pCurrentProduct)
        {
            if (pFirstPrinter)
            {
                LabelType *cpLabel = const_cast<Product *> (pCurrentProduct)->getLabelType(FirstPrinter);
				if (traceOn)
				{
					Debug::trace("OperatingMode.changeProduct", HobString("cpLabel: 0x") + HobString(Hex::toString((long long) (cpLabel))));
					if (cpLabel)
						Debug::trace("OperatingMode.changeProduct", HobString("getLabelMediaError: ") + HobString(Integer::toString(cpLabel->getLabelMediaError())));
				}

                if (cpLabel && (cpLabel->isLabelMediaError() == false))
                {
                    printerScenario = PrimaryPrinterOnly;
					if (traceOn)
						Debug::trace("OperatingMode.changeProduct", "Setting printerScenario to PrimaryPrinterOnly.");
				}
			}

            if (pSecondPrinter)
            {
                LabelType *cpLabel = const_cast<Product *> (pCurrentProduct)->getLabelType(SecondPrinter);
				if (traceOn)
				{
					Debug::trace("OperatingMode.changeProduct", "SecondPrinter:");
					Debug::trace("OperatingMode.changeProduct", HobString("cpLabel: 0x") + HobString(Hex::toString((long long) (cpLabel))));
					if (cpLabel)
						Debug::trace("OperatingMode.changeProduct", HobString("getLabelMediaError: ") + HobString(Integer::toString(cpLabel->getLabelMediaError())));
				}

                if (cpLabel && (cpLabel->isLabelMediaError() == false))
                {
                    if (printerScenario == NoPrinters)
                    {
						printerScenario = SecondPrinterOnly;
						if (traceOn)
							Debug::trace("OperatingMode.changeProduct", "Setting printerScenario to SecondPrinterOnly.");
					}
					else
					{
						printerScenario = PrimaryAndSecondPrinter;
						if (traceOn)
							Debug::trace("OperatingMode.changeProduct", "Setting printerScenario to PrimaryAndSecondPrinter.");
					}
				}
			}

			if (pThirdPrinter)
			{
				LabelType *cpLabel = const_cast<Product *> (pCurrentProduct)->getLabelType(ThirdPrinter);
				if (traceOn)
				{
					Debug::trace("OperatingMode.changeProduct", "pThirdPrinter:");
					Debug::trace("OperatingMode.changeProduct", HobString("cpLabel: 0x") + HobString(Hex::toString((long long) (cpLabel))));
					if (cpLabel)
						Debug::trace("OperatingMode.changeProduct", HobString("getLabelMediaError: ") + HobString(Integer::toString(cpLabel->getLabelMediaError())));
				}

				if (cpLabel && (cpLabel->isLabelMediaError() == false))
				{
					if (printerScenario == NoPrinters)
					{
						printerScenario = ThirdPrinterOnly;
						if (traceOn)
							Debug::trace("OperatingMode.changeProduct", "Setting printerScenario to ThirdPrinterOnly.");
					}
					else if (printerScenario == PrimaryPrinterOnly)
					{
						printerScenario = PrimaryAndThirdPrinter;
						if (traceOn)
							Debug::trace("OperatingMode.changeProduct", "Setting printerScenario to PrimaryAndThirdPrinter.");
					}
					else if (printerScenario == SecondPrinterOnly)
					{
						printerScenario = SecondAndThirdPrinter;
						if (traceOn)
							Debug::trace("OperatingMode.changeProduct", "Setting printerScenario to SecondAndThirdPrinter.");
					}
					else if (printerScenario == PrimaryAndSecondPrinter)
					{
						printerScenario = PrimarySecondAndThirdPrinter;
						if (traceOn)
							Debug::trace("OperatingMode.changeProduct", "Setting printerScenario to PrimarySecondAndThirdPrinter.");
					}
				}
			}
		}
	}
	if (traceOn)
		Debug::trace("OperatingMode.changeProduct", "////////  changeProduct2(): exiting");
}


void OperatingMode::generateNewPackage2(const ProductTransaction *pPT)
{
	bool traceOn = Debug::isTracing("OperatingMode");
	HobString msgDebug("generateNewPackage2 - ");
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("pCurrentProductTransaction = ").append(Hex::toString((long long) pCurrentProductTransaction)).append("\n");
		msgDebug2.append("pPT = ").append(Hex::toString((long long) pPT)).append("   And is not used at all here");
		Debug::trace("OperatingMode", msgDebug2);
	}

	if (pCurrentProductTransaction == NULL)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("pCurrentProductTransaction is NULL, exiting!!!");
			Debug::trace("OperatingMode", msgDebug2);
		}
		return;
	}
	OpModePackage *pPackage = new OpModePackage(pCurrentProductTransaction, packId);
	packageMap.insert(make_pair(pCurrentProductTransaction->getTransactionNum(), pPackage));

	if (Configuration::getInstance()->isPrepackScale() == true)
	{
		if (pCurrentProductTransaction->doForcePreImage())
		{
			printerScenario = NoPrinters;
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Setting printerScenario to NoPrinters since pCurrentProductTransaction->doForcePreImage() is true. Must have changed a label type.");
				Debug::trace("OperatingMode", msgDebug2);
			}
		}
	}
	
	const Product *pProduct = pCurrentProductTransaction->getProduct();
	changeProduct2(pProduct);
	if (pProduct != NULL)
	{
		HobString pScenario = "printerScenario = ";
		if (printerScenario == PrimaryPrinterOnly)
		{
			pScenario.append("PrimaryPrinterOnly");
			pPackage->setFirstPrinter(pFirstPrinter);
			pPackage->setPrimaryPlacement(setLabelPlacement((LaPosLoc) pProduct->getLabelPlacement(FirstPrinter)));
			pPackage->setPrimaryRotation((LaRotation) pProduct->getLabelRotation(FirstPrinter));
			postImagePrinter(FirstPrinter, pFirstPrinter, packId);
		}
		else if (printerScenario == SecondPrinterOnly)
		{
			pScenario.append("SecondPrinterOnly");
			pPackage->setSecondPrinter(pSecondPrinter);
			pPackage->setSecondaryPlacement(setLabelPlacement((LaPosLoc) pProduct->getLabelPlacement(SecondPrinter)));
			pPackage->setSecondaryRotation((LaRotation) pProduct->getLabelRotation(SecondPrinter));
			postImagePrinter(SecondPrinter, pSecondPrinter, packId);
		}
		else if (printerScenario == PrimaryAndSecondPrinter)
		{
			pScenario.append("PrimaryAndSecondPrinter");
			pPackage->setFirstPrinter(pFirstPrinter);
			pPackage->setPrimaryPlacement(setLabelPlacement((LaPosLoc) pProduct->getLabelPlacement(FirstPrinter)));
			pPackage->setPrimaryRotation((LaRotation) pProduct->getLabelRotation(FirstPrinter));
			pPackage->setSecondPrinter(pSecondPrinter);
			pPackage->setSecondaryPlacement(setLabelPlacement((LaPosLoc) pProduct->getLabelPlacement(SecondPrinter)));
			pPackage->setSecondaryRotation((LaRotation) pProduct->getLabelRotation(SecondPrinter));
			postImagePrinter(FirstPrinter, pFirstPrinter, packId);
			postImagePrinter(SecondPrinter, pSecondPrinter, packId);
		}
		else if (printerScenario == PrimaryAndThirdPrinter)
		{
			pScenario.append("PrimaryAndSecondPrinter");
			pPackage->setFirstPrinter(pFirstPrinter);
			pPackage->setPrimaryPlacement(setLabelPlacement((LaPosLoc) pProduct->getLabelPlacement(FirstPrinter)));
			pPackage->setPrimaryRotation((LaRotation) pProduct->getLabelRotation(FirstPrinter));

			pPackage->setThirdPrinter(pThirdPrinter);
			//	Currently, we don't have a third printer with a label applier. Maybe some day we will.
			postImagePrinter(FirstPrinter, pFirstPrinter, packId);
			postImagePrinter(ThirdPrinter, pThirdPrinter, packId);
		}
		else if (printerScenario == SecondAndThirdPrinter)
		{
			pScenario.append("SecondAndThirdPrinter");
			pPackage->setSecondPrinter(pSecondPrinter);
			pPackage->setSecondaryPlacement(setLabelPlacement((LaPosLoc) pProduct->getLabelPlacement(SecondPrinter)));
			pPackage->setSecondaryRotation((LaRotation) pProduct->getLabelRotation(SecondPrinter));

			pPackage->setThirdPrinter(pThirdPrinter);
			//	Currently, we don't have a third printer with a label applier. Maybe some day we will.
			postImagePrinter(SecondPrinter, pSecondPrinter, packId);
			postImagePrinter(ThirdPrinter, pThirdPrinter, packId);
		}
		else if (printerScenario == PrimarySecondAndThirdPrinter)
		{
			pScenario.append("PrimarySecondAndThirdPrinter");

			pPackage->setFirstPrinter(pFirstPrinter);
			pPackage->setPrimaryPlacement(setLabelPlacement((LaPosLoc) pProduct->getLabelPlacement(FirstPrinter)));
			pPackage->setPrimaryRotation((LaRotation) pProduct->getLabelRotation(FirstPrinter));

			pPackage->setSecondPrinter(pSecondPrinter);
			pPackage->setSecondaryPlacement(setLabelPlacement((LaPosLoc) pProduct->getLabelPlacement(SecondPrinter)));
			pPackage->setSecondaryRotation((LaRotation) pProduct->getLabelRotation(SecondPrinter));

			pPackage->setThirdPrinter(pThirdPrinter);
			//	Currently, we don't have a third printer with a label applier. Maybe some day we will.
			postImagePrinter(FirstPrinter, pFirstPrinter, packId);
			postImagePrinter(SecondPrinter, pSecondPrinter, packId);
			postImagePrinter(ThirdPrinter, pThirdPrinter, packId);
		}
		else
		{
			pScenario.append("Invalid printerScenario: ").append(Integer::toString(printerScenario));
		}
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append(pScenario);
			Debug::trace("OperatingMode", msgDebug2);
		}

		if (Debug::isTracing("WrapperMode.PackId"))
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("pushing packId: ").append(Integer::toString((int)packId)).append("\n");
			msgDebug2.append("packageMap.size(): ").append(Integer::toString((int)packageMap.size()));
			Debug::trace("WrapperMode.PackId", msgDebug2);
		}
		incPackId();
	}
}

HobWeight OperatingMode::getMaxAWSPackageWeight() const
{
	if (MetSig::isScaleMetric())
		return MaxAWSPkgWtMetric;
	else
	{
		HobWeight RetVal;

		if ( MetSig::isWeigherConfiguredForDualRange()  ||
		     Configuration::getInstance()->getWeigherModel() == WmAWS2 )
			RetVal = MaxAWSPkgWtAvoirDR;  // Dual Range Support (haberrc)
		else
			RetVal = MaxAWSPkgWtAvoir;
		
		return RetVal;
	}
}

bool OperatingMode::packageWeightIsTooHeavyForAWS(HobWeight pkgWeight) const
{
	if (pkgWeight >= getMaxAWSPackageWeight())
		return true;
	else
		return false;
}

void OperatingMode::sendPkgDischargeError(PrinterType prType, bool setIt)
{
	Debug::trace("WrapperMode.StateMachine", "OperatingMode::sendPkgDischargeError():  prType = " + inttostring(prType) + ",  setIt = " + booltostring(setIt));
	if (setIt)
		opModeStatus.setPkgDischargeError(prType);
	else
		opModeStatus.clearPkgDischargeError(prType);
	dispatchEvent(&opModeStatus);
}

void OperatingMode::handlePrinterConfigInOpMode(const char *recvBuffer)
{
	//	PTR 78:  If the printer configuration has changed in Operate mode, we need to preImage the label again.
	if (!recvBuffer)
		return;

	PrinterConfiguration prConfig(recvBuffer);

	Debug::trace("OperatingMode", "handlePrinterConfigInOpMode():  Preimage printer " + inttostring(prConfig.getPrinterType()) + ".");
	switch (prConfig.getPrinterType())
	{
		case FirstPrinter:
			preImagePrinter(FirstPrinter, pFirstPrinter);
			break;
		case SecondPrinter:
			preImagePrinter(SecondPrinter, pSecondPrinter);
			break;
		case ThirdPrinter:
			preImagePrinter(ThirdPrinter, pThirdPrinter);
			break;
		default:
			Debug::trace("OperatingMode", "handlePrinterConfigInOpMode():  Not a valid printer!");
			break;
	}
}
