#include "PrinterEvent.h"
#include "EvolMessage.h"

#include "PrinterManager.h"
#include "PrinterTeachTable.h"
#include "EssPrinterTeachTable.h"
#include "EvolPrinterTeachTable.h"
#include "StEppPrinterTeachTable.h"
#include "PrinterWakeup.h"
#include "PrinterConfiguration.h"
#include "PrinterInquire.h"
#include "PrinterStatus.h"
#include "PrinterMode.h"
#include "PrinterRequestPrintheadPower.h"
#include "PrinterCommand.h"
#include "PrinterNomSize.h"
#include "PrinterStockMetrics.h"
#include "PrinterRam.h"
#include "PrinterTest.h"
#include "PrinterTeach.h"
#include "PrinterMask.h"
#include "PrinterRequestSensors.h"
#include "PrinterSensors.h"
#include "PrinterSensorsTracking.h"
#include "PrinterPrintTestLabel.h"
#include "PrinterLabelWidth.h"
#include "PrinterNewPrinterType.h"
#include "PrinterPrinterType.h"
#include "PrinterPrinterKind.h"
#include "PrinterTakeLabelTime.h"
#include "PrinterAdvanceLabelStock.h"
#include "PrinterNewAddress.h"
#include "PrinterSizing.h"

#include "PrinterFlush.h"
#include "PrinterEnableTakeup.h"
#include "PrinterReset.h"
#include "PrinterPreImage.h"
#include "PrinterPostImage.h"
#include "PrinterLabelReady.h"
#include "PrinterLabelTaken.h"
#include "PrinterLabelComplete.h"
#include "PrinterDeletePackage.h"
#include "PrinterRequestLabelTransfer.h"
#include "PrinterRequestVirtualLabel.h"
#include "PrinterLabelTransferFailed.h"
#include "PrinterLabelImageSize.h"

#include "PrinterEnable.h"
#include "PrinterDisable.h"
#include "PrinterVersion.h"
#include "PrinterHeadType.h"

#include "PrinterSystemWakeup.h"
#include "PrinterSystemStatus.h"
#include "PrinterSystemConfiguration.h"
#include "PrinterSystemInquire.h"
#include "PrinterSystemRequestWakeup.h"
#include "PrinterSystemRequestStatus.h"
#include "PrinterSystemRequestInquire.h"
#include "PrinterSystemCalibrationSwitch.h"
#include "PrinterRequestCalibrate.h"
#include "PrinterFactoryDefaultsComplete.h"
#include "PrinterKpcError.h"
#include "KpcSystemErrors.h"

#include "RendererManager.h"
#include "RendererRequestSession.h"
#include "RendererSessionId.h"
#include "RendererSetSource.h"
#include "RendererSetDestination.h"
#include "RendererRequestRendering.h"
#include "RendererRenderStart.h"
#include "RendererRenderComplete.h"
#include "RendererCloseSession.h"
#include "RendererInitDotHeadWear.h"
#include "RendererNewDotHeadWear.h"

#include "PrinterRequestStatus.h"
#include "PrinterRequestInquire.h"
#include "PrinterRequestWakeup.h"
#include "PrinterHeadWear.h"

#include "PrinterHeadDotStatus.h"
#include "PrinterHeadDotAck.h"
#include "PrinterRequestHeadDotStatus.h"
#include "PrinterRequestHeadDotWear.h"
#include "PrinterTestCassette.h"

#include "PrinterDonePostImagingSelfServeBarcodeLabelImage.h"
#include "PrinterGapCalStatus.h"
#include "Integer.h"
#include "Product.h"
#include "ProductTransaction.h"
#include "ProductText.h"
#include "RemotePrintRequest.h"
#include "RemotePrintTemplateRequest.h"
#include "Graphic.h"
#include "GraphicImage.h"
#include "LabelType.h"
#include "Barcode.h"
#include "Nutrifacts.h"
#include "NutriDataColumn.h"
#include "ProductTransactionMaster.h"
#include "Messages/UIDialogMsg.h"

#include "Configuration.h"
#include "SessionManagerHelper.h"
#include "HfcWeigherStatus.h"

#include "LabelStockDescriptor.h"
#include "FormRecord.h"
#include "ImageControl.h"
#include "BitMap.h"

#include "Pen.h"

#include "VoidLabelBits.h"

#include "Debug.h"
#include "Bool.h"
#include "Hex.h"
#include "Double.h"
#include "Long.h"
#include "Power.h"
#include "CEvolMsgReceiver.h"

#include "PythonScripter.h"
#include "CRendererUtil.h"
#include "SystemPrinters.h"
#include "FpmManager.h"
#include "BoardSiteGeneric.h"
#include "RealTimeController.h"
#include "LabelApplierSetup.h"

#include <ProductTotalManager.h>
#include <SessionManager.h>
#include <Session.h>

#include "OnlineOrderProduct.h"

#include <HfcResponse.h>
#include <Messages/RebootMessage.h>

#include <HobartCustomerHook.h>

//SOF-5482
#include <HfcAccessKeyActivation.h>
#include <AccessKeys.h>


#include <list>
#include <cstring>
#include <thread>
#include <vector>

#ifdef _WIN32
#include "minwindef.h"
#endif

#include "epcrfid.h"

using namespace std;


#if defined(PAL_WIN32_RUN)
#define PACKED
#pragma pack(push)
#pragma pack(1)
#else
#define PACKED __attribute__((__packed__))
#endif


#if defined ( __linux__ )
typedef struct PACKED tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct PACKED tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#endif

#if defined(PAL_WIN32_RUN)
#pragma pack(pop)
#endif



const unsigned int PrinterManager::_edgeMargin_ = 13;	// Offset from edges of label stock


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterManager::dataType = 0;
std::string PrinterManager::dataTypeName("PrinterManager"); 
#endif


PrinterManager::PrinterManager(Hobart::CEvolMsgReceiver *pMyPrinter,
								PrinterTeachTable * /* pMyTeachTable */,
								PrinterType printerType,
								LabelWidth labelWidth,
								UINT16 bufferSize,
								UINT16 printheadSize,
								UINT16 physicalAddress,
								USBD_NODE_ID usbNodeId,
								PrinterKind prKind, ULONG transferSize_)
	: Hobart::CEvolMsgReceiver("PrinterManager"),
		printerSysConfig(this, printerType, prKind, SysTemporary, (UINT8 *) "SYS", physicalAddress, 0, 0, 0),
		printerTraceString(HobString("PrinterManager: ") + HobString(Integer::toString((UINT32) printerType)) + HobString(" - NodeId: ") + HobString(Hex::toString((unsigned long) usbNodeId)) + "  ")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("PrinterManager", string("Constructor PrinterManager:") + util_string::ptrtostring(this) + "  pMyPrinter:" + util_string::ptrtostring(pMyPrinter)  + " - started");

	this->pMyPrinter = pMyPrinter;
	this->myPrinterType = printerType;
	this->labelWidth = UnknownLabel;
	this->bufferSize = bufferSize;
	this->printheadSize = printheadSize;
	this->physicalAddress = physicalAddress;
	this->usbNodeId = usbNodeId;
	this->prKind = prKind;
	this->lastDotStatus = ALL_DOTS_GOOD;
	
	rfidModuleInit = false;
	transferSize = transferSize_;
	initialDotWearComplete = false;
	wasDotWearEnabled = Configuration::getInstance()->isDotWearEnabled();
	Debug::trace("PrinterManager.new", printerTraceString + " PrinterManager():  " + HobString(Hex::toString((long long) this)) + ",  myPrinterType = " + Integer::toString(myPrinterType));

	pRS485Manager = NULL;

	renderingRFIDVoidLabelType = false;
	needToReimageRFIDVoidLabelType = false;
	printerStatePriorToRenderingRFIDVoidLabelType = PrinterReadyToPrint;  // initialize it to a valid value
	//Debug::trace("PrinterManager.rfid", printerTraceString + " ctor() - needToReimageRFIDVoidLabelType: " + booltostring(needToReimageRFIDVoidLabelType));
	//Debug::trace("PrinterManager.rfid", printerTraceString + " ctor() - renderingRFIDVoidLabelType: " + booltostring(renderingRFIDVoidLabelType));

	if ( printerType == FirstPrinter )
	{
		if(rfidModuleInit == false)
		{
			Debug::trace("PrinterManager.rfid", "rfidModuleInit true timer started");

			rfidModuleInit = true;

			rfidQryTime = 1000;

			createTimer(rfidQryTime, rfidQryTimer, true);
		}
	}

	if (prKind == EppPrinter)
	{
		Debug::trace("PrinterManager.new", printerTraceString + " NEW EvolPrinterTeachTable() instance and EPP2 Commands");
		this->pMyTeachTable = EvolPrinterTeachTable::getInstance();
		printLabelCentered = true;
		getLabelWidthFromStatus = false;
	}
	//Alex Zavala's fix for Epp2 missing teach tables
	else if (prKind == Epp2Printer)
	{
		Debug::trace("PrinterManager.new", printerTraceString + "PrinterManager::CTOR() - NEW EvolPrinterTeachTable() instance and EPP2 Commands");
		this->pMyTeachTable = EvolPrinterTeachTable::getInstance();
		this->pMyTeachTable->show();
		printLabelCentered = true;
		getLabelWidthFromStatus = false;
		virtualPrintingEnabled = false;
	}
	else if (prKind == Epp3Printer)
	{
		Debug::trace("PrinterManager.new", printerTraceString + "PrinterManager::CTOR() - NEW StEppPrinterTeachTable instance and EPP3 Commands");
		this->pMyTeachTable = StEppPrinterTeachTable::getInstance();
		this->pMyTeachTable->show();
		printLabelCentered = true;
		getLabelWidthFromStatus = false;
		virtualPrintingEnabled = false;
	}
	else if (prKind == EssPrinter)
	{
		Debug::trace("PrinterManager.new", printerTraceString + " Selecting EssPrinter");
		this->pMyTeachTable = EssPrinterTeachTable::getInstance();
		printLabelCentered = false;
		getLabelWidthFromStatus = true;
	}
	else if (prKind == EscPrinter)
	{
		Debug::trace("PrinterManager.new", printerTraceString + " Selecting EscPrinter");
		this->pMyTeachTable = EssPrinterTeachTable::getInstance();
		printLabelCentered = false;
		getLabelWidthFromStatus = true;
	}
	else if (prKind == ApcPrinter)
	{
		Debug::trace("PrinterManager.new", printerTraceString + " Selecting ApcPrinter");
		this->pMyTeachTable = EssPrinterTeachTable::getInstance();
		printLabelCentered = false;
		getLabelWidthFromStatus = true;
	}
	else if ( (prKind == KPCPrinter) || (prKind == SSKPCPrinter) ) 
	{
		//open label data channel to printer
		pRS485Manager = RS485Manager::getInstance();

		Debug::trace("PrinterManager.new", "prKind: " + Integer::toString(prKind) );
		Debug::trace("PrinterManager.new", printerTraceString + " Selecting KPCPrinter");
		this->pMyTeachTable = EssPrinterTeachTable::getInstance();
		Debug::trace("PrinterManager.new", printerTraceString + " Selecting KPCPrinter Step 1");
		this->pMyTeachTable->show();
		Debug::trace("PrinterManager.new", printerTraceString + " Selecting KPCPrinter Step 2");
		printLabelCentered = false;
		Debug::trace("PrinterManager.new", printerTraceString + " Selecting KPCPrinter Step 3");
		getLabelWidthFromStatus = true;
		Debug::trace("PrinterManager.new", printerTraceString + " Finished selecting KPCPrinter");
	}
	else
	{
		Debug::trace("PrinterManager.new", printerTraceString + " Selecting No TeachTable");
		printLabelCentered = true;
		getLabelWidthFromStatus = false;
		this->pMyTeachTable = NULL;
	}

	Debug::trace("PrinterManager", printerTraceString + " past prKind");

	pConfig = Configuration::getInstance();

	Debug::trace("PrinterManager", printerTraceString + " got pConfig");

	labelPresent = false;
	sendTestLabel = false;
	resetUPCBarcode = false;
	printingVoidLabel = false;
	continuousTestLabels = false;
	
	testLabelData = NULL;
	testLabelDataSize = 0;
	saveTestLabelType = PrNoTestLabel;
	testLabelType = PrNoTestLabel;
	applySecurityLabel = PrNoSecurityLabel;

	executingCommand = false;

	printerResetting = false;
	Debug::trace("PrinterManager.PrinterReset", printerTraceString + " 1printerResetting: " + HobString(Bool::toString(printerResetting)));

	printerCalibrating = false;
	Debug::trace("PrinterManager.PrinterCalibration", printerTraceString + " printerCalibrating: " + HobString(Bool::toString(printerCalibrating)));

	resetPrinter = false;
	Debug::trace("PrinterManager.PrinterReset", printerTraceString + " resetPrinter: " + HobString(Bool::toString(resetPrinter)));

	enableSizing = true;
	Debug::trace("PrinterTeachTable", printerTraceString + " enableSizing: " + HobString(Bool::toString(enableSizing)));
	printerAdvancing = false;
	Debug::trace("PrinterManager.PrinterAdvance", printerTraceString + " printerAdvancing: " + HobString(Bool::toString(printerAdvancing)));

	measuredSize = 0;
	actualSize = 0;
	labelDataSize = 0;
	labelData = NULL;

	outOfDataBuffers = false;
	Debug::trace("PrinterManager.LabelTransfer", printerTraceString + " outOfDataBuffers: " + HobString(Bool::toString(outOfDataBuffers)));

	trackSensors = PrinterOff;

	pCurrentProduct = NULL;
	Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));

	currentProductSmartPtr = Util::CSmartPtr<Product>();
	newProductSmartPtr = Util::CSmartPtr<Product>();

	Debug::trace("PrinterManager", printerTraceString + " got smartPointers");

	pCurrentLabel = NULL;

	pCurrentProductTransaction = NULL;
	currentPackId = 0;

	myRendererSessionId = 0;

	// NGW-261
	for ( size_t i( NUM_LABEL_BUFFERS ); i--; )
		labelBuffer_[i] = NULL;

	Debug::trace("PrinterManager", printerTraceString + " got labelBuffer");

	labelBufferIndex_ = 0;

	pDestinationData = NULL;
	for (int i = 0; i < NewMaxPrinterTypes; i++)
		imageControlBlock[i] = 0;
	setAllLabelsToBePrinted(false);

	for (int i = 0; i < NewMaxPrinterTypes; i++)
		bitMap[i] = new BitMap;
	for (int i = 0; i < MaxPrinterTypes; i++)
		rotatedBitMap[i] = new BitMap;

	Debug::trace("PrinterManager", printerTraceString + " got imageControlBlock");

	pMyRenderer = RendererManager::getInstance();

	Debug::trace("PrinterManager", printerTraceString + " got renderer");

	// get a session with the renderer!!
	RendererRequestSession rdReqSession(this);
	rdReqSession.setPrinterType(printerType);

	Debug::trace("PrinterManager", printerTraceString + " got session with renderer");

	const char *pCC = rdReqSession.serialize();
	int mLength = rdReqSession.getSerializedLength();

	pMyRenderer->send(pCC, mLength);

	ticksPerSecond = 1000; 

	xferLabelStart = 0;
	xferLabelFinish = 0;

	tstLabelWaitCnts_ = 0;
	
	if (pConfig->isSelfServiceScale() == true)
	{
		takeLabelTime = ticksPerSecond / 2; // Timer for LabelTaken to happen after LabelReady occurs
	}
	else
	{
		takeLabelTime = 3 * ticksPerSecond;	// Timer for LabelTaken to happen after LabelReady occurs
	}

	requestSensorTime = ticksPerSecond / 2; // We want two requests per second

	if ( (prKind == KPCPrinter) || ( prKind == SSKPCPrinter ) ) {
		printerProbeTime = FOURSECONDS * ticksPerSecond;
	} else {
		printerProbeTime = pConfig->getPrinterProbeTime() * ticksPerSecond; // Probe for Printer Every Ten Seconds if no messages
	}
	Debug::trace("PrinterManager.PrinterProbe", printerTraceString + HobString(" printerProbeTime: ") + Integer::toString(printerProbeTime));

	#ifdef PR_DEFAULTS_TEST
		printerTestMsgTime = (10 * 1000 ) + 500;
		createTimer( printerTestMsgTime, PrinterTests, CHobartTimer::PERIODIC_TIMER );

		testCounter = 0;
		testMsgCount = 0;
		failedErases = 0;
	#endif

	printheadPowerTime = 30 * ticksPerSecond;

	continuousLabelTime = ticksPerSecond / 8;

	printHeelFirst = pConfig->isLabelFeedHeelFirst();
	pHeelBufferStart = pHeelBuffer = NULL;
	imageFlip = false;
	adjTopMargin  = 0;

	Debug::trace("PrinterManager.PrLabelSensorLabel", "PrinterManager CTOR SET printHeelFirst: " + booltostring(printHeelFirst) );

	requestSensorReading = false;

	linesToPrint = 0;
	takeLabelError = false;

	streamingLabels = false;

	usingContinuousStock = false;
	isCutterInstalled = false;
	//SOF-5355 hack hack!! in for demo with jim 	
	//printToScreenFlag_ = false;

	pjr413HasQRLabel = false;

	isCutterEnabled = Configuration::getInstance()->toBoolCfg(fieldOffsetCfgIsCutterEnabled);
	Debug::trace("PrinterManager.Cutter", "PrinterManager isCutterEnabled: " + util_string::booltostring(isCutterEnabled) );

	noCutOption = Configuration::getInstance()->toBoolCfg(fieldOffsetCfgVirtualCombinePrint);				//SOF-3847 3848

	lastStatusMsg.setPrinterType(myPrinterType);
	printerSysConfig.setPrinterType(myPrinterType);
	printerConfig.setPrinterType(myPrinterType);
	lastSensorsMsg.setPrinterType(myPrinterType);

	Debug::trace("PrinterManager", printerTraceString + " set some printerTypes");

	if ( pConfig->isUpdatePrinterConfig() == true && prKind != FakePrinter )
	{
		sendPrinterConfig(PrPermanent);
	}

	Debug::trace("PrinterManager", printerTraceString + " send some printerConfig");

	printerStockMetrics.setPrinterType(myPrinterType);

    if ( prKind == FakePrinter )
    {
		this->labelWidth = labelWidth;   // PJR-444 - SOF-5063 - Print wider labels on Zebra printer
		centeringOffset = 0;
		printerStockMetrics.setLabelWidth(labelWidth);   // PJR-444 - SOF-5063 - Print wider labels on Zebra printer
		if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgChoseToUseContinuousStock))
			printerStockMetrics.setContinuousStock();
		else
			printerStockMetrics.setLabelLengthInLines( (LabelLengthLines) Configuration::getInstance()->toIntCfgI("fakePrinterLabelLength") );
		stockLoaded.setLength(Configuration::getInstance()->toIntCfgI("fakePrinterLabelLength"));
		stockLoaded.setWidth(WideLabel);
		stockLoaded.setLabelOrientation(PortraitLabel);
		//stockLoaded.setWidthDots(labelWidth);   // PJR-444 - SOF-5063 - Print wider labels on Zebra printer
		stockLoaded.setWidthDots( PrinterStockMetrics::getLabelWidthDots(labelWidth) );
	}
	else
		/* added for freestanding scale development -- TO DO */
		printerStockMetrics.setLabelWidth(WideLabel);


	if ( prKind != FakePrinter )
		sendPrinterKind();

	//	PTR 1578:	Make sure we set this for the Status msg also, so we don't report an error
	//				for the Primary (default) when it is for another printer.
	lastStatusMsg.setPrinterType(myPrinterType);

	pendingLabelXfer = false;

	testLabelPrintInProgress = false;

	virtualPrintingEnabled = false;
	currentVirtuallyPrintingLabelNum = FirstPrinter;
	
	cassetteTest = false;

	mfgKey_ = false;
	
	registerEvent(Printer, PrReqStatus);
	registerEvent(Printer, PrReqStockMetrics);
	registerEvent(Printer, PrReqPrinterType);
	registerEvent(Printer, PrReqKind);
	registerEvent(Printer, PrPreImage);
	registerEvent(Printer, PrPreImageBarcode);
	registerEvent(Printer, PrPostImageBarcode);
	registerEvent(Printer, PrPostImage);
	registerEvent(Printer, PrTestLabel);
	registerEvent(Printer, PrReqLabelWidth);

	registerEvent(Printer, PrCutterEnabled);
	registerEvent(Printer, PrCutterDisabled);
	
	if ( (prKind == Epp2Printer) || (prKind == EppPrinter) || (prKind == Epp3Printer) ) {
		registerEvent(LabelApplier, LaSetup);
	}
	
	
	if ( (prKind == KPCPrinter) || (prKind == SSKPCPrinter) ) {
		registerEvent(Printer, PrLabelTransferReady);
		registerEvent( Printer, PrLabelTransferComplete);
		registerEvent( Printer, PrVersion );
		registerEvent( Printer, PrKPCError );
		registerEvent( KPCSystem, KpcError );

    registerEvent( Printer, PrHeadDotWear);				// call a setObject on itself so it wont call itself.

    registerEvent(Printer, PrHeadDotStatus);          // not sure if we need this
    
    registerEvent(Printer, PrCutterStatus);
		registerEvent(Printer, PrEjectCut);

		registerEvent(Printer, PrReqTestCassette );
		registerEvent(Printer, PrRfidTestRequest);
		registerEvent(Printer, PrRfidCalRequest);
		registerEvent(Printer, PrRfidUpgradeRequest);
		registerEvent(Printer, PrRfidQryRequest);
		registerEvent(Printer, PrCutterBitDflt);
		registerEvent(Printer, PrResetTeach);				//SOF-3847 3848
		registerEvent(OperationMode, OpModeManualWeigh);	//SOF-3847 3848
		registerEvent(OperationMode, OpModeProductionEntry);//SOF-3847 3848
		registerEvent(OperationMode, OpModeAutoManualApply);		//SOF-3847 3848
		registerEvent( ConfigurationDev, CfgSaved );		//SOF-3980
		
		registerEvent( Printer, PrStartGapCalibration );		//SOF-4727
		registerEvent( Printer, PrGapCalStatus ); 				//SOF-4727
		registerEvent( Printer, PrGapCalNext );					//SOF-4727
		registerEvent( Printer, PrTest );

		//SOF-5482
		registerEvent(SessionDev, SnKeyActivation);
	

		this->requestCutterStatus();
	}

	if( prKind != FakePrinter )
	{
		registerEvent(Printer, PrLabelTransferReady);
		registerEvent( Printer, PrLabelTransferComplete);
		registerEvent(Printer, PrWakeup);
		registerEvent(Printer, PrEnable);
		registerEvent(Printer, PrDisable);
		registerEvent(Printer, PrTest);
		registerEvent(Printer, PrReset);
		registerEvent(Printer, PrResetImmediate);
		registerEvent(Printer, PrConfig);
		registerEvent(Printer, PrCommand);
		registerEvent(Printer, PrFlush);
		registerEvent(Printer, PrReqPrintheadPower);
		registerEvent(Printer, PrMode);
		registerEvent(Printer, PrRam);
		registerEvent(Printer, PrReqWakeup);
		registerEvent(Printer, PrReqInquire);
		registerEvent(Printer, PrReqSensors);
		registerEvent(Printer, PrReqCalibrate);
		registerEvent(Printer, PrSensorsTracking);
		registerEvent(Printer, PrLabelWidth);
		registerEvent(Printer, PrNewOrder);
		registerEvent(Printer, PrTakeLabelTime);
		registerEvent(Printer, PrAdvLabelStock);
		registerEvent(Printer, PrNewAddress);
		registerEvent(Printer, PrSizing);
		registerEvent(Printer, PrDeletePackage);
		registerEvent(Printer, PrReqLabelSizing);
		registerEvent(Printer, PrUseContinuousStock);
		registerEvent(Printer, PrResetFactoryDefaults);
		registerEvent(Printer, PrVersion);
		registerEvent(PrinterSystem, SysReqStatus);		//	HTA-71
		registerEvent(Printer, PrCutterStatus);
		registerEvent(Printer, PrReqCutterStatus);
		registerEvent( Printer, PrCutterCut);
		registerEvent(Printer, PrEjectCut);
		registerEvent(Printer, PrReqTestCassette );
		registerEvent(Printer, PrRfidTestRequest);
		registerEvent(Printer, PrRfidCalRequest);
		registerEvent(Printer, PrRfidUpgradeRequest);
		registerEvent(Printer, PrRfidQryRequest);
		registerEvent(Printer, PrCutterBitDflt);
		registerEvent(Printer, PrResetTeach);				//SOF-3847 3848
		registerEvent(OperationMode, OpModeManualWeigh);	//SOF-3847 3848
		registerEvent(OperationMode, OpModeProductionEntry);//SOF-3847 3848
		registerEvent(OperationMode, OpModeAutoManualApply);		//SOF-3847 3848

		registerEvent( ConfigurationDev, CfgSaved );		//SOF-3980
		
		receivedPrinterInquire = false;
		requestedPrinterInquire = false;
		this->requestCutterStatus();
		requestSystemStatusMsg();
		requestSystemInquireMsg();
	}

	if ( printerType == FirstPrinter )
		registerEvent(Printer, PrSetupForNormalSizing);

	printerState = PrinterUnsized;

	//SOF-5312											//SOF-5304
	if( ( ( ( prKind == SSKPCPrinter ) || ( prKind == KPCPrinter ) ) && ( pConfig->isConserveContinuousStockByNotSizingEnabled() ) ) ) {
		Debug::trace("PrinterManager", printerTraceString + " Use Continuous stock mode");
		printerMode = ContinuousShootLabelHeelCut;
		
		
		//SOF-5312
		measuredSize = Configuration::getInstance()->toIntCfgI(CfgInternal::DIMBLENGTHCONTINUOUS);
		actualSize = measuredSize;
		PrinterNomSize prNomSize(this, myPrinterType, actualSize, measuredSize);
		const char *pCC = prNomSize.serialize();
		pMyPrinter->send(pCC, prNomSize.getSerializedLength());

		stockLoaded.setLength(actualSize);
		stockLoaded.setWidth(labelWidth);
		stockLoaded.setSkew(0);
		printerStockMetrics.setContinuousStock();

		printerState = PrinterReadyToPreImage;
		enableSizing = false;
		printerStockMetrics.setLabelWidth( WideLabel );
		dispatchEvent( &printerStockMetrics );

		//SOF-5304: if printer has not sized (upgrade) then tell printer we are using continuous stock
		//					save the configuration value for future reboots of the scale	
		if( !( pConfig->toIntCfg(fieldOffsetCfgPrinter1LabelLength) == LengthContinuous_Lines ) ) {
			Debug::trace("PrinterManager", "Inform printer we are using continuous stock.");
			PrinterGeneric gen( PrUseContinuous, this, myPrinterType );		
			const char *pCC = gen.serialize();
			pMyPrinter->send( pCC, gen.getSerializedLength() );											
		}

	} else {		
		Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterUnsized");
	  printerMode = UnsizedLabel;
		//SOF-5312
		Debug::trace("PrinterManager.StateMachine", " printerState: " + Integer::toString(printerState) );	
		Debug::trace("PrinterManager.First", "firstBoot: true");	
		stockLoaded.setLength(0);
		enableSizing = true;
		firstBoot = true;
	}
	
	//SOF-5124
	if( ( pConfig->toIntCfg(fieldOffsetCfgPrinter1LabelLength) == LengthContinuous_Lines ) && ( prKind == SSKPCPrinter ) ){		
		Debug::trace("PrinterManager", "Inform printer we are using continuous stock.");
		PrinterGeneric gen( PrUseContinuous, this, myPrinterType );		
		const char *pCC = gen.serialize();
		pMyPrinter->send( pCC, gen.getSerializedLength() );								
	}

	probeTimeoutCntr_ = 0;

	currentTransNum = 0;

	previousPrinterCounts_ = printerCounts_ = 0;

	FpmManager *pSerialFlash = FpmManager::getInstance();
	if( pSerialFlash != NULL )
	{
		std::string style = pSerialFlash->getMFGSystemStyle();
		if( ( style == "HT" ) || ( style == "HTK" ) || ( style == "FSS" ) )
		{
			MtdScaleData *pPrinterStatistics_ = pSerialFlash->readScaleData();
			if( pPrinterStatistics_ != NULL )
			{
				if( printerType == FirstPrinter )
					previousPrinterCounts_ = printerCounts_ = pPrinterStatistics_->getPrimaryPrinterCounts();
				else if (printerType == SecondPrinter)
					previousPrinterCounts_ = printerCounts_ = pPrinterStatistics_->getSecondaryPrinterCounts();
				else if (printerType == ThirdPrinter)
					previousPrinterCounts_ = printerCounts_ = pPrinterStatistics_->getTertiaryPrinterCounts();

				Debug::trace("PrinterManager.Timer", "initialized printerCounts_: " + Integer::toString(printerCounts_));
			}
			Debug::trace("PrinterManager.Timer", "Created printer label counter timer.");
			createTimer(( 10 * 60 * 1000 ), labelStatisticsTimer, CHobartTimer::PERIODIC_TIMER); // every 10 min
        }
		else if( ( style == "EPCP" ) || ( style == "AWS3" ) || ( style == "NGW1" ) || ( style == "NGW2" ) )
		{
			MtdNGWWrapperData *pPrinterStatisticsNGW_ = pSerialFlash->readWrapperData();
			if( pPrinterStatisticsNGW_ != NULL )
			{
				if( printerType == FirstPrinter )
					previousPrinterCounts_ = printerCounts_ = pPrinterStatisticsNGW_->getPrimaryPrinterCounts();
				else if (printerType == SecondPrinter)
					previousPrinterCounts_ = printerCounts_ = pPrinterStatisticsNGW_->getSecondaryPrinterCounts();
				else if (printerType == ThirdPrinter)
					previousPrinterCounts_ = printerCounts_ = pPrinterStatisticsNGW_->getTertiaryPrinterCounts();

				Debug::trace("PrinterManager.Timer", "initialized printerCounts_: " + Integer::toString(printerCounts_));
			}
			Debug::trace("PrinterManager.Timer", "Created printer label counter timer.");
			createTimer(( 10 * 60 * 1000 ), labelStatisticsTimer, CHobartTimer::PERIODIC_TIMER); // every 10 min
		}
		else
		{
			Debug::trace("PrinterManager.Timer", "Scale style not equal to AWS3.");
		}
	}
	else
	{
		Debug::trace("PrinterManager.Timer", "pSerialFlash: NULL");
	}


	//SOF-5603	fire the cutter on power up
	std::string style = pSerialFlash->getMFGSystemStyle();
	if( ( style == "FSS" ) || ( style == "FSV" ) ) {
		Debug::trace("PrinterManager.Cutter", "Sending cut commnad.");
		PrinterGeneric cStatus(PrCutterCut, this, myPrinterType);

		const char *pCC = cStatus.serialize();
		pMyPrinter->send(pCC, cStatus.getSerializedLength());
	}

#ifdef FAKE_SOCKET_WEIGHER
	agentAttached_ = false;
#endif
   
#if defined (_WIN32)  	
   
	if (Configuration::getInstance()->isPrepackScale())
	   printerManagerThread = new Util::CTLSThread( *this, 16, 0, "PrinterManager Thread" );			
	else
	   printerManagerThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "PrinterManager Thread" );			

#else
  
   /*Linux only -- bump thread priority to resolve streaming 8" label issue*/   
   printerManagerThread = new Util::CTLSThread( *this, 1, 0, "PrinterManager Thread" );			
   
#endif	

	RendererInitDotHeadWear init(this, (this->printheadSize / 8)); //bits to bytes
	init.show();
	dispatchEvent(&init);


	if ( prKind == FakePrinter )
	{
		printerState = PrinterReadyToPreImage;
		receivedPrinterFeedback();
	}

	Debug::trace("PrinterManager", string("Constructor PrinterManager:") + util_string::ptrtostring(this) + " - ended");
}

PrinterManager::~PrinterManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("PrinterManager", string("Destructor PrinterManager:") + util_string::ptrtostring(this) + "  pMyPrinter:" + util_string::ptrtostring(pMyPrinter) + " - started");

	deregisterAllEvents();

	if ( printerManagerThread )
	{
		printerManagerThread->StopThread();
		printerManagerThread->WaitForCompletion();
	}

	shutdownTimerThreads();
	ShutdownNicely();

	Debug::trace("PrinterManager.new", printerTraceString + " ~PrinterManager():  " + HobString(Hex::toString((long long) this)));
	// Deregister for all the OperationMode messages

	if (prKind != FakePrinter)
	{
		lastStatusMsg.setManagerDeleted();
		dispatchEvent(&lastStatusMsg);
	}

	//tell the thread to exit
	m_ExitEvent.EventSet();
	Debug::trace("PrinterManager", "~PrinterManager():  m_ExitEvent.EventSet()");  

	//wait for the thread to exit before deleting all the resources!
	if ( printerManagerThread )
	{
		while ( printerManagerThread->IsRunning() )
			PAL_absclass::PAL_sleep( 1 );
		printerManagerThread = NULL;
	}
	Debug::trace("PrinterManager", "~PrinterManager():  printerManagerThread = NULL");  
	
	if ( prKind != FakePrinter )
		cancelTimer(printerProbe);  
	cancelTimer(takeLabelTimerExpired);
	cancelTimer(sensorTimerExpired);

	Debug::trace("PrinterManager.Desc", printerTraceString + " Calling BitMap::freeBitMap()");
	for (int i = 0; i < NewMaxPrinterTypes; i++)
		delete bitMap[i];

	Debug::trace("PrinterManager.Desc", printerTraceString + " Calling rotatedBitMap::freeBitMap()");
	for (int i = 0; i < MaxPrinterTypes; i++)
		delete rotatedBitMap[i];

	deleteImageControl( NULL, 0 );
	deleteBitMap( NULL, 0 );

	if ((testLabelType != PrVoidLabel) && (testLabelData != NULL))
	{
		delete [] testLabelData;
	}

	if( (pHeelBufferStart) && ( pHeelBuffer ) && ( pHeelBuffer != NULL ) )
		delete [] pHeelBufferStart;

	if (pMyRenderer != NULL)
	{
		RendererCloseSession rdCloseSession(this, myRendererSessionId);

		const char *pCC = rdCloseSession.serialize();
		int mLength = rdCloseSession.getSerializedLength();

		pMyRenderer->send(pCC, mLength);
	}

	emptyCommandQ();
	emptyIndirectGraphics();
	emptyEmbeddedLabelBitMaps();

	for (int i = 0; i < (short)NewMaxPrinterTypes; i++)
		if ( imageControlBlock[i] )
			delete imageControlBlock[i];

	// NGW-261
	for ( size_t i( NUM_LABEL_BUFFERS ); i--; )
		if ( labelBuffer_[i] )
			delete labelBuffer_[i];

	Debug::trace("PrinterManager", string("Destructor PrinterManager:") + util_string::ptrtostring(this) + " - ended");
}

DWORD PrinterManager::Run()
{
	try
	{
		
		Debug::trace("PrinterManager.Run", printerTraceString + std::string(" start run thread "));
		if ( myPrinterType == FirstPrinter && pConfig->isConserveContinuousStockByNotSizingEnabled())
		{
			if ( pConfig->toIntCfg(fieldOffsetCfgPrinter1LabelLength) == LengthContinuous_Lines )
			{
				Debug::trace("PrinterManager", printerTraceString + std::string(" start run thread - setting up for usingContinuousStock"));
				usingContinuousStock = true;
				if(isCutterInstalled && isCutterEnabled){				
					takeLabelTime = 0;
				} else { //SOF-3990
					if( !isCutterInstalled ) {
						Debug::trace("PrinterManager.TakeLabelTime", " ignore label taken: " + Integer::toString(Configuration::getInstance()->toBoolCfg(fieldOffsetCfgContinuousStockIgnoreLabelTaken))); 						
						if( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgContinuousStockIgnoreLabelTaken)) {							
							//SOF-3993 always look at label taken in manual and prepack modes reguardless of setting
							if ( RealTimeController::getInstance()->getCurrentOpModeEvent() == OpModeProductionEntry ) {							
								Debug::trace("PrinterManager.TakeLabelTime", "Production Entry takeLabelTime: 0");
								takeLabelTime = 0;						
							} else {
								Debug::trace("PrinterManager.TakeLabelTime", "Non Production Entry takeLabelTime: 5000");
								takeLabelTime = ticksPerSecond / 2;					
							}							
						} 
					}					
				}

				if (printHeelFirst == true)
					if(isCutterInstalled && isCutterEnabled){
						if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
						    printerMode =  ContinuousLabelHeelCut;
						} else {
							printerMode =  ContinuousShootLabelHeelCut;
						}
						takeLabelTime = ticksPerSecond / 2;					
					} else {
						printerMode =  ContinuousLabelHeel;	
					}
				else
					if(isCutterInstalled && isCutterEnabled) {
						printerMode =  ContinuousLabelHeadCut;
						takeLabelTime = ticksPerSecond / 2;
					} else {
						printerMode =  ContinuousLabelHead;
					}
			}
			else if ( pConfig->toIntCfg(fieldOffsetCfgPrinter1LabelLength) > 0 )
			{
				Debug::trace("PrinterManager", printerTraceString + std::string(" start run thread - setting up for die cut labels, length:" + inttostring(pConfig->toIntCfg(fieldOffsetCfgPrinter1LabelLength))));
				if( printHeelFirst == true ) {
					if( prKind != SSKPCPrinter ) {
					    printerMode =  DieCutLabelHeel;
				    } else {
              printerMode = DieCutShootHeel;
					}
				} else {
					if( prKind != SSKPCPrinter ) {
					    printerMode =  DieCutLabelHead;
					} else {
						printerMode =  DieCutShootHead;
					}
				}
			}	
		}
		
		bool needToTeach = true;
		
		if ( ( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) || ( prKind == SSKPCPrinter ) ) && ( isCutterInstalled ) && ( isCutterEnabled ) && ( noCutOption ))
		{
			if ( handleMode( RealTimeController::getInstance()->getCurrentOpModeEvent() ) )
				needToTeach = false;
		}

		if ( needToTeach )
			teachPrinter(printerMode);

		createTimer(printerProbeTime, printerProbe, CHobartTimer::PERIODIC_TIMER);

		requestSensorMsg();
		requestStatusMsg();
		requestInquireMsg();
		requestSystemStatusMsg();
		requestSystemInquireMsg();

		//SOF-3875			//SOF-5130: have we gone threw first boot? 
		if (( ( prKind == KPCPrinter ) || ( prKind == SSKPCPrinter ) ) && ( isCutterEnabled ) && ( !firstBoot ) ) {
			//Do not size linerless stock 			
			measuredSize = LengthContinuous_Lines;
			actualSize = LengthContinuous_Lines;
			printerState = PrinterReadyToPreImage;

			//if cutter is enabled then we assume continuous stock.
			usingContinuousStock = true;
			
			stockLoaded.setLength(actualSize);
			stockLoaded.setWidth(labelWidth);
			stockLoaded.setSkew(0);
			printerStockMetrics.setContinuousStock();

			PrinterNomSize prNomSize(this, myPrinterType, actualSize, measuredSize);
			const char *pCC = prNomSize.serialize();
			pMyPrinter->send(pCC, prNomSize.getSerializedLength());

		} else {
			//if HTm  without cutter 
			if ( ( prKind == KPCPrinter ) && ( pConfig->isConserveContinuousStockByNotSizingEnabled() )
			    && ( !isCutterEnabled ) && ( !firstBoot ) ) {
				Debug::trace("PrinterManager.ConserveStock", "HTm:  ConserveContinuousStock is set.");
				//Do not size conserve stock is set
				measuredSize = LengthContinuous_Lines;
				actualSize = LengthContinuous_Lines;
				printerState = PrinterReadyToPreImage;

				//if cutter is enabled then we assume continuous stock.
				usingContinuousStock = true;
				
				stockLoaded.setLength(actualSize);
				stockLoaded.setWidth(labelWidth);
				stockLoaded.setSkew(0);
				printerStockMetrics.setContinuousStock();

				PrinterNomSize prNomSize(this, myPrinterType, actualSize, measuredSize);
				const char *pCC = prNomSize.serialize();
				pMyPrinter->send(pCC, prNomSize.getSerializedLength());
			} 					
			//SOF-5130: allow FSSS to size on first boot when continuous stock is loaded.
			if( stockLoaded.getLength() != 0 ) {	//SOF-5297
				Debug::trace("PrinterManager.First", "firstBoot: false");
				firstBoot = false;
			}
			//SOF-5064
			if( ( prKind == ApcPrinter ) && ( pConfig->isConserveContinuousStockByNotSizingEnabled() ) ){
				Debug::trace("PrinterManager.Teach", "Mode: " + Integer::toString(printerMode) );
				if( printerMode == UnsizedLabel ) {
					printerMode = ContinuousLabelHeel;
					teachPrinter(printerMode);
				}
				//Do not size linerless stock 			
				measuredSize = LengthContinuous_Lines;
				actualSize = LengthContinuous_Lines;
				printerState = PrinterReadyToPreImage;

				//if cutter is enabled then we assume continuous stock.
				usingContinuousStock = true;
				
				stockLoaded.setLength(actualSize);
				stockLoaded.setWidth(labelWidth);
				stockLoaded.setSkew(0);
				printerStockMetrics.setContinuousStock();

				PrinterNomSize prNomSize(this, myPrinterType, actualSize, measuredSize);
				const char *pCC = prNomSize.serialize();
				pMyPrinter->send(pCC, prNomSize.getSerializedLength());

				printerState = PrinterReadyToPrint;
			}
		}

//DISABLED UNTIL LATER
#if 0
		//Run dot wear if enabled
		if (( prKind == KPCPrinter ) &&
			( Configuration::getInstance()->isDotWearLicensed() ) &&
			( Configuration::getInstance()->isDotWearEnabled() ))
		{
			Debug::trace("PrinterManager.HeadDot","Request Head Dot Status...");
			requestHeadDotStatus();
		}
		else
		{
			Debug::trace("PrinterManager.HeadDot","NOT Requesting Head Dot Status");
			initialDotWearComplete = true;
		}
#else
		initialDotWearComplete = true;
#endif
		while (!m_ExitEvent.IsEventSet())
		{
			{//here so that the run thread will release the critical section.

				Util::CLockCriticalSection lock( printerManagerCS );

				if(Configuration::getInstance()->isDotWearEnabled() != wasDotWearEnabled){
					wasDotWearEnabled = Configuration::getInstance()->isDotWearEnabled();
					//If we disabled dot wear we need to tell the renderer
					if(!Configuration::getInstance()->isDotWearEnabled()){
#if defined ( __linux__ )
						UINT8 bits[this->printheadSize]= {0};
						std::fill_n(bits,this->printheadSize, 0xFF );
#else
						UINT8 bits[448] = { 1 };	// jhc - this is strictly to run on windows I got 448 from Aaron The #linux line will not compile on windows bc windows expects the size variable to be a const
#endif
						RendererNewDotHeadWear rWear(this, bits, this->printheadSize);
						rWear.show();
						dispatchEvent(&rWear);
					}
				}
				if ((resetPrinter == true) && (!((printerState == PrinterXferringLabel) ||
								(printerState == PrinterTakingLabel) ||
								(printerState == PrinterLabelIsReady) ||
								(printerState == PrinterTryingToSize) ||
								(printerState == PrinterFinding))))
				{
					resetPrinter = false;
					Debug::trace("PrinterManager.PrinterReset", printerTraceString + " resetPrinter: " + HobString(Bool::toString(resetPrinter)));

					sendPrinterReset(0);
					emptyProductTransactionQ(false);
					emptyCommandQ();

					pCurrentProduct = NULL;
					pCurrentLabel = NULL;
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));
					currentProductSmartPtr = Util::CSmartPtr<Product>();
					newProductSmartPtr = Util::CSmartPtr<Product>();

					dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, false);
					pCurrentProductTransaction = NULL;
					currentPackId = 0;

					if (printerState != PrinterTryingToSize)
					{
						printerState = PrinterReadyToPrint;
						Debug::trace("PrinterManager.StateMachine", printerTraceString + " 1) PrinterReadyToPrint");
					}
				}

				if( printerState == PrinterPriorityErrorStop ) {
					//wait until the printer is done printing label or we time out waiting.
					Debug::trace("PrinterManager.Run", " PrinterPriorityErrorStop: wait ");
					if( lastStatusMsg.isPrintCmdComplete() ) {
						// SOF-3861
						Debug::trace("PrinterManager.Run", " PrinterPriorityErrorStop: label complete!");
						if( ( prKind == KPCPrinter ) || ( prKind == SSKPCPrinter ) ) {
							testLabelPrintInProgress = true;
							emptyProductTransactionQ(false);
							//flush any image in the KPC image buffer.
							sendPrinterReset(1);
							emptyCommandQ();
							tstLabelWaitCnts_ = 0;
							printerState = PrinterPriorityErrorRestart;							
							Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterPriorityErrorRestart");			
						}
					} else {
						// has the 485 Manager transfered the image?
						if( !pRS485Manager->isBusy() ) {
							tstLabelWaitCnts_++;
							//have reached timeout while waiting for label to finish?
							if( tstLabelWaitCnts_ == stopTimeout_ ) {
								Debug::trace("PrinterManager.Run", " PrinterPriorityErrorStop: Timeout!");
								tstLabelWaitCnts_ = 0;
								lastStatusMsg.setCommand( PrintCmdComplete );
							}	
						}
					}
				}

				//SOF-5066
				if( printerState == PrinterWaitToExpelAndCut ) {
					PrinterStatus prStatus(lastStatusMsg);
					Debug::trace("PrinterManager.StateMachine", " internal state: " + Integer::toString( prStatus.getState() ));
					if( lastCutterStatusMsg.getInterlock() ) {
							//printer has aborted all commands at this point.
							executingCommand = false;
							lastStatusMsg.setCommand( PrintCmdComplete );

							Debug::trace("PrinterManager.StateMachine", " PrinterWaitToExpelAndCut: interlock closed");
							queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, 100); //150
							queueCommand(CutRetractCommand, PrNoSecurityLabel, PrRam2,-100);	//-100 //-25 									
														
							printerState = PrinterPriorityErrorRestart;	//SOF-5157
					} else {
						Debug::trace("PrinterManager.StateMachine", " PrinterWaitToExpelAndCut: request status");
						requestCutterStatus();
						//don't spam the printer with requests
						PAL_absclass::PAL_sleep(250);
					}
				}
				//SOF-5083 
				if( printerState == PrinterWaitInterlock ) {					
					if( lastCutterStatusMsg.getInterlock() ) {
						if( pConfig->isConserveContinuousStockByNotSizingEnabled() ) {
							Debug::trace("PrinterManager.interlock", "AdvanceCommand");	
							queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, 850); 
							queueCommand(CutRetractCommand, PrNoSecurityLabel, PrRam2,-100); //-50																		
							printerState = PrinterReadyToPrint;
							Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterReadyToPrint");
						} else {
							Debug::trace("PrinterManager.interlock", "PrinterUnsized");	
							clearLabelDimensions();
							sendPrinterFlush();
							sendPrinterReset(0);
							emptyCommandQ();

							if (pMyTeachTable)
								pMyTeachTable->recreateTeachTable();
							teachPrinterNormalSizing();
					
							PrinterGeneric gen( PrUseContinuous, this, myPrinterType );		
							const char *pCC = gen.serialize();
							pMyPrinter->send( pCC, gen.getSerializedLength() );								

							printerState = PrinterUnsized;
							Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterUnsized");
							enableSizing = true;	
#if 0							
							//if we are in production entry then finish cleaning up.
							OpModeEvent mEvent = RealTimeController::getInstance()->getCurrentOpModeEvent();
							if( mEvent == OpModeProductionEntry) {
								Debug::trace("PrinterManager.interlock"," cleanup after production entry");
								emptyProductTransactionQ(false);
								pCurrentProduct = NULL;
								pCurrentLabel = NULL;
								currentProductSmartPtr = Util::CSmartPtr<Product>();
								newProductSmartPtr = Util::CSmartPtr<Product>();

								dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, false);
								pCurrentProductTransaction = NULL;
								currentPackId = 0;								
							}						
#endif							
						}
					} else {
						Debug::trace("PrinterManager.interlock", "interlock is open report!");	
						requestCutterStatus();
						//don't spam the printer with requests
						PAL_absclass::PAL_sleep(250);
					}	
				}

				if (executingCommand == false)
				{
					PrinterCommand *pCommand = popCommandQ();

					if (pCommand)
					{
						if (pMyPrinter != NULL)
						{
							Debug::trace("PrinterManager", printerTraceString + " Run() - executingCommand == false and have a command to execute and have a printer");
							Debug::trace("PrinterManager", printerTraceString + " Run() - sending PrCommand: " + Integer::toString(pCommand->getPrinterCommand()));
							
							if( ( pCommand->getPrinterCommand() != BackupCommand ) && ( prKind == SSKPCPrinter ) ) {
							    executingCommand = true;
							}
							Debug::trace("PrinterManager.CMD","(1)executingCommand: " + Integer::toString(executingCommand));

							if (pCommand->getPrinterCommand() == CalibrateCommand)
							{
								printerCalibrating = true;
								Debug::trace("PrinterManager.PrinterCalibration", printerTraceString + " printerCalibrating: " + HobString(Bool::toString(printerCalibrating)));

								PrinterRequestCalibrate reqCalib(this, myPrinterType, (PrinterCalibrateType) pCommand->getValue());

								const char *pCC = reqCalib.serialize();
								int mLength = reqCalib.getSerializedLength();
								pMyPrinter->send(pCC, mLength);
							}
							else
							{
								
								if (pCommand->getPrinterCommand() == PrintCommand ) {
									Debug::trace("PrinterManager.LabelTransfer", " Sending PrintCommand");

								} else if (pCommand->getPrinterCommand() == AdvanceCommand){
									printerAdvancing = true;
								}
								
								const char *pCC = pCommand->serialize();
								int mLength = pCommand->getSerializedLength();
								pMyPrinter->send(pCC, mLength);
							}
						}

						delete pCommand;
					}
				}

				if (printerBusy() == false)
				{
					if (!imageControlBlock[ExtraPrinter_UsedForRFIDVoidLabel] && stockLoaded.getLength() > 0 &&
						!needToReimageRFIDVoidLabelType && !renderingRFIDVoidLabelType /*&& pConfig->toBoolCfg(fieldOffsetCfgEnableRfidPrinting)*/)
					{
						needToReimageRFIDVoidLabelType = true;
						//Debug::trace("PrinterManager.rfid", printerTraceString + " Run() - imageControlBlock[ExtraPrinter_UsedForRFIDVoidLabel] is null - setting needToReimageRFIDVoidLabelType: " + booltostring(needToReimageRFIDVoidLabelType));
					}

					if ((needToReimageRFIDVoidLabelType || renderingRFIDVoidLabelType) && !pCurrentProductTransaction && !pCurrentProduct)
					{
						//Debug::trace("PrinterManager.rfid", printerTraceString + " Run() - printer is not busy - needToReimageRFIDVoidLabelType: " + booltostring(needToReimageRFIDVoidLabelType));
						//Debug::trace("PrinterManager.rfid", printerTraceString + " Run() - printer is not busy - renderingRFIDVoidLabelType: " + booltostring(renderingRFIDVoidLabelType));
						if (needToReimageRFIDVoidLabelType)
						{
							//Debug::trace("PrinterManager.rfid", printerTraceString + " Run() - needToReimageRFIDVoidLabelType, so calling reimageRFIDVoidLabelType()");
							needToReimageRFIDVoidLabelType = false;
							//Debug::trace("PrinterManager.rfid", printerTraceString + " Run() - resetting needToReimageRFIDVoidLabelType: " + booltostring(needToReimageRFIDVoidLabelType));
							reimageRFIDVoidLabelType();
						}
						//else
							//Debug::trace("PrinterManager.rfid", printerTraceString + " Run() - renderingRFIDVoidLabelType, so waiting until reimageRFIDVoidLabelType() is finished");
					}
					else
					{

						if (takeLabelError == true && takeLabelTime != 0)
						{
							Debug::trace("PrinterManager", printerTraceString + " Run() - printer is not busy and takeLabelError == true");

							takeLabelError = false;

							PrinterStatus prStatus(lastStatusMsg);

							prStatus.setTakeLabelError();
							//	PTR 1471 - Don't need 2 identical Take label messages for 1 error.			prStatus.setLabelTakenError(); // to generate a beep!!

							dispatchEvent(&prStatus);

							lastStatusMsg.setTakeLabelError(); // make sure it is on!!
						}
						else if (takeLabelError == true && noCutOption) {

							takeLabelError = false;

							PrinterStatus prStatus(lastStatusMsg);

							prStatus.setTakeLabelError();

							dispatchEvent(&prStatus);

							lastStatusMsg.setTakeLabelError(); // make sure it is on!!

						}

						// try to print the next label
						if (sendTestLabel == true)
						{
							Debug::trace("PrinterManager", printerTraceString + " Run() - printer is not busy and sendTestLabel == true");

							currentVirtuallyPrintingLabelNum = MaxPrinterTypes;

							if (printerState == PrinterPriorityError)
							{
								cancelTimer(continuousLabelTimerExpired);
								teachPrinter(printerMode);
								printerState = PrinterReadyToPreImage;
								Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterReadyToPreImage");
							}
							else if (printerState == PrinterLabelIsReady || printerState == PrinterReadyToXfer)
							{
								if (testLabelType == PrVoidLabel)
								{
									printingVoidLabel = true;
								}
								else
									testLabelPrintInProgress = true;//at this time we stop new label data from being created.

								emptyProductTransactionQ(false);

								if (pCurrentProductTransaction != NULL)
								{
									// Kill the currently printing label
									dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, false);
									pCurrentProductTransaction = NULL;
									currentPackId = 0;
								}

								if (sendTestLabel == true)
								{
									Debug::trace("PrinterManager.PrinterReset", printerTraceString + " resetPrinter0");
									sendPrinterReset(1);
									emptyCommandQ();

									printTestLabel();
								}
							}

							sendTestLabel = false;
						}

						else if (((printerState == PrinterReadyToPrint) || (printerState == PrinterReadyToPreImage)) && (myRendererSessionId != 0))
						{
							//Debug::trace("PrinterManager", printerTraceString + " Run() - printer is not busy and printerState == PrinterReadyToPrint || printerState == PrinterReadyToPreImage");

							// look at the next productTransaction
							productTransactionId  prodPackId;

							peekProductTransactionQ(&prodPackId);

							ProductTransaction* pNextProductTransaction = prodPackId.pProductTransaction;
							if (pNextProductTransaction)
							{
								Product* pNextProduct = pNextProductTransaction->getProduct();
								if (pNextProduct)
								{
									LabelType* pNextLabel = pNextProduct->getLabelType((short)myPrinterType);

									Debug::trace("PrinterManager.Product", printerTraceString + " printerState: " + HobString(Integer::toString(printerState)));

									// Is it a new Product
									if (pCurrentProduct && (pNextProduct->getProductNum() == pCurrentProduct->getProductNum()) &&
										(virtualPrintingEnabled || (pCurrentLabel != NULL && pNextLabel == pCurrentLabel)) &&
										!pNextProductTransaction->doForcePreImage())
									{
										pCurrentProductTransaction = pNextProductTransaction;
										Debug::trace("PrinterManager.Product", printerTraceString + " PostImaging Product Transaction: " + HobString(Hex::toString((long long)pCurrentProductTransaction)));

										productTransactionId tempProdPackId;

										popProductTransactionQ(&tempProdPackId);

										currentPackId = tempProdPackId.packId;
										currentTransNum = tempProdPackId.transactionNum;

										// start postimaging all labels for the transaction - post image all of them consecutively
										printerState = PrinterPostImaging;
										Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterPostImaging");


										bool moreLabelsToPostImage = false;
										PrinterType labelNumToPostImage = myPrinterType;
										if (virtualPrintingEnabled)
										{
											moreLabelsToPostImage = true;
											labelNumToPostImage = FirstPrinter;
										}

										do
										{
											if (imageControlBlock[labelNumToPostImage])
											{
												imageControlBlock[labelNumToPostImage]->setImageFlags(PostImage);
												updateFormPostImageFields(*imageControlBlock[labelNumToPostImage]);

												if (Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgISaveLabel))
												{
													static int filenameStringCounter = 0;
													std::string filenameString;

#if defined(PAL_WIN32_RUN)
													filenameString = string("C:\\Program Files\\Hobart\\LabelImage");
#else
													filenameString = string("/etc/hobart/LabelImage");
#endif
													filenameString.append(inttostring(++filenameStringCounter)).append(".bmp");

													Debug::trace("RendererManager.saveLabel", "Before post imaging, PrinterManager set filename to save label to file: " + filenameString);
													imageControlBlock[labelNumToPostImage]->setSaveToFilenameString(filenameString);
												}


												RendererSetSource rdSetSource(this, myRendererSessionId, (UINT8*)imageControlBlock[labelNumToPostImage]);
												const char* pCC = rdSetSource.serialize();
												int mLength = rdSetSource.getSerializedLength();
												pMyRenderer->send(pCC, mLength);

												RendererSetDestination rdSetDestination(this, myRendererSessionId, (UINT8*)bitMap[labelNumToPostImage]->getBitMapBase());
												pCC = rdSetDestination.serialize();
												mLength = rdSetDestination.getSerializedLength();
												pMyRenderer->send(pCC, mLength);

												RendererRequestRendering rdReqRendering(this, myRendererSessionId);
												pCC = rdReqRendering.serialize();
												mLength = rdReqRendering.getSerializedLength();
												pMyRenderer->send(pCC, mLength);
											}

											if (moreLabelsToPostImage)
											{
												while (labelNumToPostImage < MaxPrinterTypes)
												{
													short i = (short)labelNumToPostImage;
													i++;
													labelNumToPostImage = (PrinterType)i;
													if (labelNumToPostImage >= MaxPrinterTypes)
														moreLabelsToPostImage = false;
													else if (isLabelToBePrinted(labelNumToPostImage))
														break;
												}
											}

										} while (moreLabelsToPostImage);
										Debug::trace("PrinterManager", "No more labels to Post image");
									}
									else
									{
										currentProductSmartPtr = prodPackId.productSmartPtr;
										pCurrentProduct = pNextProduct;  // or could use currentProductSmartPtr.get(), but they should be the same!
										Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pCurrentProduct: " + HobString(Hex::toString((long long)pCurrentProduct)));
										preImageLabel();
										pNextProductTransaction->setForcePreImage(false);
									}
								}
							}
						}
						// SOF-4727
						//if (((printerState == PrinterUnsized) && (enableSizing == true) && initialDotWearComplete)
						if ((printerState == PrinterUnsized) &&
							(enableSizing == true) &&
							initialDotWearComplete && !printerCalibrating &&
							(pConfig->isConserveContinuousStockByNotSizingEnabled() == false))
						{
							Debug::trace("PrinterTeachTable", " Run() - printer is not busy and printerState == //SOF-5355 hack hack!! && enableSizing == true");
							//SOF-5184
							//if(!( isCutterInstalled && !lastCutterStatusMsg.getInterlock() && isCutterEnabled ) ){
								// tell printer to size its label!!
								//delay 500ms to allow for cutter to close
							if (isCutterEnabled && isCutterInstalled)
								PAL_absclass::PAL_sleep(500);					// not allowed on windows
							queueCommand(SizeCommand, PrNoSecurityLabel, PrNoIndirection, 0x0);

							printerState = PrinterTryingToSize;
							Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterTryingToSize");

							PrinterStatus prStatus(lastStatusMsg);
							prStatus.setLabelSizing();
							dispatchEvent(&prStatus);

							lastStatusMsg.setLabelSizing(); // make sure it is on!!

							pCurrentProduct = NULL;
							Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pCurrentProduct: " + HobString(Hex::toString((long long)pCurrentProduct)));
							currentProductSmartPtr = Util::CSmartPtr<Product>();

							labelPresent = false;
							Debug::trace("PrinterManager.LabelMissing", printerTraceString + " labelPresent");
							//}
						}
					}
				}

				if (requestSensorReading == true)
				{
					requestSensorMsg();

					requestSensorReading = false;
				}
			}

			PAL_absclass::PAL_sleep( 10 );
		}
	}
	catch(...)
	{
	    Debug::trace("PrinterManager.error", HobString("Run():  catch(...)"));
		LogCatch;
	}

	return 0;
}


void PrinterManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	Util::CLockCriticalSection lock( printerManagerCS );
	Debug::trace("PrinterManager", printerTraceString + "  processEvent() device:" + inttostring((int)msg->getDeviceType()) + "  event:" + inttostring((int)msg->getEventType()));

	if ( msg.get() != NULL )	
	{
		switch (msg->getDeviceType())
		{
			case Printer:
				handlePrinterMessage((PrEvent) msg->getEventType(), msg->Data(), (UINT16)msg->Length() );
				break;
			case PrinterSystem:
				handlePrinterSysMessage((SysEvent) msg->getEventType(), msg->Data());
				break;
			case KPCSystem:
				handleKpcSystemError( (KpcSystemEvent)msg->getEventType(), msg->Data() );
			case Renderer:
				handleRendererMessage((RdEvent) msg->getEventType(), msg->Data());
				break;
			case OperationMode:
				handleModeMessage( (OpModeEvent) msg->getEventType(), msg->Data());	
				break;
			case ConfigurationDev: 
				handleConfigMessage( (ConfigurationEvent) msg->getEventType(), msg->Data() );
				break;
			case LabelApplier:
				handleApplierMessage( (LaEvent)msg->getEventType(), msg->Data());
			    break;
			default:
				break;
		}
	}
}

void PrinterManager::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer, UINT16 bufSize)
{
	Debug::trace("PrinterManager","handlePrinterMessage() prEvent = " + Integer::toString(prEvent));
	
	/*
	if (prEvent == PrRfidQryRequest)
	{
		if(rfidModuleInit == false)
		{
			Debug::trace("PrinterManager.rfid", "rfidModuleInit true timer started");

			rfidModuleInit = true;

			rfidQryTime = 1000;

			createTimer(rfidQryTime, rfidQryTimer, true);
		}
		else if(rfidModuleInit == true)
		{
			Debug::trace("PrinterManager.rfid", "rfidModuleInit false timer canceled");

			//rfidModuleInit = false;

			//cancelTimer(rfidQryTimer);
		} 
	}
	*/
	if (prEvent == PrRfidTestRequest)
	{
		bool tagWritten = false;
		int resultCode = 1; 
		char rcexBuffer[256] = { 0 };

		cancelTimer(rfidQryTimer);

		string rfidProductType = "";

		if(isWrapperPrinter())
		{
			FpmManager *pSerialFlash( FpmManager::getInstance() );

			const std::string style( pSerialFlash->getMFGSystemStyle() );

			if( style == "EPCP" )
			{
				rfidProductType = "PCP";
			}
			else
			{
				rfidProductType = "NGW";
			}
		}
		else
		{
			rfidProductType = "GTX";
		}		

		HobString action("EPC|" + Configuration::getInstance()->toStringCfg(fieldOffsetCfgRfidEncodeMethod) + "|" + "200000000004" + "|" + "260916" + "|" + "0" + "|" + "1" + "|" + "2" + "|" + "0" + "|" + rfidProductType);

		hobart_cust_hook_custom_action(action.c_str(), &resultCode, rcexBuffer, sizeof(rcexBuffer));
		
		if(resultCode == 0)
		{
			tagWritten = true;
		}
		else
		{
			tagWritten = false;
		}

		createTimer(rfidQryTime, rfidQryTimer, true);

		PrinterGeneric result(tagWritten ? PrRfidTestPassed : PrRfidTestFailed, this, FirstPrinter);
		dispatchEvent(&result);

		return;
	}

	if (prEvent == PrRfidCalRequest)
	{
		int resultCode = 1; 
		char rcexBuffer[256] = { 0 };

		cancelTimer(rfidQryTimer);

		string rfidProductType = "";

		if(isWrapperPrinter())
		{
			rfidProductType = "EPP";
		}
		else
		{
			rfidProductType = "GTX";
		}		

		HobString action("CRP|" + Configuration::getInstance()->toStringCfg(fieldOffsetCfgRfidEncodeMethod) + "|" + "200000000004" + "|" + "260916" + "|" + "0" + "|" + "1" + "|" + "2" + "|" + "0" + "|" + rfidProductType);

		hobart_cust_hook_custom_action(action.c_str(), &resultCode, rcexBuffer, sizeof(rcexBuffer));
		
		rfidCalTime = 5000; 
		createTimer(rfidCalTime, rfidCalTimer, true);

		return;
	}

	if (prEvent == PrRfidUpgradeRequest)
	{
		int resultCode = 1; 
		char rcexBuffer[256] = { 0 };

		cancelTimer(rfidQryTimer);

		string rfidProductType = "";

		if(isWrapperPrinter())
		{
			FpmManager *pSerialFlash( FpmManager::getInstance() );

			const std::string style( pSerialFlash->getMFGSystemStyle() );

			if( style == "EPCP" )
			{
				rfidProductType = "PCP";
			}
			else
			{
				rfidProductType = "NGW";
			}
		}
		else
		{
			rfidProductType = "GTX";
		}		

		HobString action("UPG|" + Configuration::getInstance()->toStringCfg(fieldOffsetCfgRfidEncodeMethod) + "|" + "200000000004" + "|" + "260916" + "|" + "0" + "|" + "1" + "|" + "2" + "|" + "0" + "|" + rfidProductType);

		hobart_cust_hook_custom_action(action.c_str(), &resultCode, rcexBuffer, sizeof(rcexBuffer));

		rfidUpgTime = 5000; 
		createTimer(rfidUpgTime, rfidUpgTimer, true);

		return;
	}

	if (prEvent == PrReqPrinterType)
	{
		Debug::trace("PrinterManager.PrinterType", printerTraceString + " PrReqPrinterType");
		sendPrinterType();
		sendPrinterKind();
	}
	else
	{
		bool processMsg = true;
		PrinterGeneric printerGeneric(recvBuffer);

		Debug::trace("PrinterManager","handlePrinterMessage() printer type = " + Integer::toString(printerGeneric.getPrinterType()));
		Debug::trace("PrinterManager","handlePrinterMessage() my type = " + Integer::toString(myPrinterType));
		Debug::trace("PrinterManager","handlePrinterMessage() getSourceObject = " + Hex::toString((void*)printerGeneric.getSourceObject()));
		Debug::trace("PrinterManager","handlePrinterMessage() pMyPrinter = " + Hex::toString((void*)pMyPrinter));

		if ((printerGeneric.getPrinterType() != myPrinterType) && (printerGeneric.getSourceObject() != pMyPrinter))
		{
			Debug::trace("PrinterManager", "handlePrinterMessage() FAILED initial IF test ");
			processMsg = false;
		}
		else if (prEvent == PrNewOrder)
		{
			PrinterNewPrinterType newPrinterType(recvBuffer);
			newPrinterType.show();
			if (newPrinterType.getUsbNodeId() != usbNodeId)
			{
				Debug::trace("PrinterManager.PrinterType", printerTraceString + "  handlePrinterMessage():  Wrong usbNodeId for PrNewPrinterType message! [" + HobString(Hex::toString((long long) usbNodeId)) + "]");
				processMsg = false;
			}
		}

		if (processMsg)
		{
			switch (prEvent)
			{
				case PrWakeup:
				{
					PrinterWakeup wakeup(recvBuffer);
					//static const uint8_t fakeWearArray[56] =  {0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					//									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					//									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					//									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					//									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					//									0x00,0x00,0x00,0x00,0x01};
					//RendererNewDotHeadWear wear(this, fakeWearArray, sizeof(fakeWearArray));
					//dispatchEvent(&wear);
					Debug::trace("PrinterManager.Message", printerTraceString + " PrWakeup");
					receivedPrinterFeedback();

				}
				break;
				case PrConfig:
				{
					Debug::trace("PrinterManager.Message", printerTraceString + " PrConfig");
					bool isTracing = Debug::isTracing("PrinterManager.PrinterConfig");
					if (isTracing)
						Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " PrConfig");
						
					if (pMyPrinter != NULL)
					{
						PrinterConfiguration prConfig(recvBuffer);

						if (isTracing)
						    prConfig.show();

						// don't allow label width changes for counter scales via PrConfig, since they get the label width from the PrStatus when sizing, and from PrSensors
						if (isTracing)
							Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " PrConfig - Checking prKind = " + inttostring((int)prKind));
						if ( isCounterScalePrinter() )
						{
							if (isTracing)
								Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " Calling prConfig.setLabelWidth() labelWidth = " + inttostring((int)labelWidth));
							prConfig.setLabelWidth(labelWidth);
						}

						if (printerConfig.equals(prConfig) == false)
						{
							if (isTracing)
							{
								Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " printerConfig different than prConfig, setting them equal " );
								Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " printerConfig ----- ");
								printerConfig.show();
								Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " prConfig ----- ");
								prConfig.show();
							}

							printerConfig = prConfig;

							if (isTracing)
							{
								Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " printerConfig after setting it to same as prConfig ----- ");
								printerConfig.show();

								Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " Calling sendPrinterConfig() when handling PrConfig, configtype = " + inttostring((int)printerConfig.getConfigType()));
							}
							sendPrinterConfig(printerConfig.getConfigType());
						}

						checkLabelWidth(printerConfig.getLabelWidth());
					}
				}
				break;
				case PrInquire:
				{
					Debug::trace("PrinterManager.Message", printerTraceString + " PrInquire:  receivedPrinterInquire = " + Bool::toString(receivedPrinterInquire));
					bool isTracing = Debug::isTracing("PrinterManager.PrinterInquire");
					if (isTracing)
					{
						Debug::trace("PrinterManager.PrinterInquire", printerTraceString + " PrInquire:  receivedPrinterInquire = " + Bool::toString(receivedPrinterInquire));
						Debug::trace("PrinterManager.PrinterInquire", printerTraceString + " PrInquire:  prKind = " + Integer::toString(prKind));
					}

					PrinterInquire printerInquire(recvBuffer);
					printerInquire.setSourceObject(this);

					if (isTracing)
					{
						printerInquire.show();
						Debug::trace("PrinterManager.PrinterInquire", printerTraceString + " PrInquire:  before updating printerConfig -----");
						printerConfig.show();
					}

					// If this is the first PrInquire received since rebooting, do this
					if (!receivedPrinterInquire)
					{
						if (myPrinterType != printerInquire.getPrinterType())
						{
							if (isTracing)
								Debug::trace("PrinterManager.PrinterInquire", printerTraceString + " changing PrinterType to:" + inttostring(myPrinterType));
							printerInquire.setPrinterType(myPrinterType);
						}

						if (isTracing)
							Debug::trace("PrinterManager.PrinterInquire", printerTraceString + " localVar labelWidth is currently:" + inttostring((int)labelWidth) + "  label width in PrInquire received:" + inttostring((int)printerInquire.getLabelWidth()));

						// labelWidth is initialized to UnknownLabel.  It will get set either from the first read of the Printer EEP, or from sizing.  If it's still its initial value,
						// set it from the PrInquire data; if it's already different, then a PrStatus or PrSensors message (APC printer - not EPP printer) updated it, so leave it alone.
						if ( isCounterScalePrinter()  && (labelWidth == UnknownLabel) )
						{
							if (isTracing)
								Debug::trace("PrinterManager.PrinterInquire", printerTraceString + " setting label width in printerInquire received to:" + inttostring((int)labelWidth));
							labelWidth = printerInquire.getLabelWidth();
						}
					}

					if ( isCounterScalePrinter() && (labelWidth != UnknownLabel) )
					{
						/*****************************************************************************************************************************************************
						*   In the following call to update(), the labelWidth in the printerConfig gets updated from what's in the printerInquire received. 
						*   It shouldn't if it was updated from a PrStatus or PrSensors (ApcPrinter) received before this PrInquire is received, so
						*   set the labelWidth in the PrInquires message from our local var; it's the most current.
						***************************************************************************************************************************************************/
						printerInquire.setLabelWidth(labelWidth);
					}

					printerConfig.update(printerInquire);

					if (isTracing)
					{
						Debug::trace("PrinterManager.PrinterInquire", printerTraceString + " PrInquire:  after updating printerConfig -----");
						printerConfig.show();
					}

					requestedPrinterInquire = false;

					if (!receivedPrinterInquire)
					{
						receivedPrinterInquire = true;
						sendPrinterType();
					}

					checkLabelWidth(printerConfig.getLabelWidth());

					if (isTracing)
					{
						Debug::trace("PrinterManager.PrinterInquire", printerTraceString + " Dispatching PrInquire:");
						printerInquire.show();
					}

					dispatchEvent(&printerInquire);
					receivedPrinterFeedback();
				}
				break;
				case PrResetFactoryDefaults:
				{
					Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " PrResetFactoryDefaults");
					if (pMyPrinter != NULL)
					{
						pMyPrinter->send((const char*) recvBuffer, bufSize);
					}
				}
				break;
				case PrResetFactoryDefaultsComplete:
				{
					Debug::trace("PrinterManager", printerTraceString + " PrResetFactoryDefaultsComplete");
					PrinterFactoryDefaultsComplete msg(recvBuffer);

					#ifdef PR_DEFAULTS_TEST
					testMsgCount++;
					testCounter = 0;
					#endif

					if( !msg.getSavedStatus() ) {
						Debug::trace("PrinterManager", "Printer factory defaults corruption!" + Integer::toString(msg.getSavedStatus()));

#if defined ( __linux__ )
						util_filesystem::File  boardSiteFile(BOARD_SITE_FILE_NAME);
						//are we running board site tests?
						if ( boardSiteFile.exists() )  {

							BoardSiteGeneric msg( BrdSitePeripheralFlashFail, this ); 
							dispatchEvent( &msg );
						}
#endif

						#ifdef PR_DEFAULTS_TEST
						failedErases++;
						#endif

					} else {
						Debug::trace("BoardSiteTestManager", "Printer factory defaults saved: " + Integer::toString(msg.getSavedStatus()));

#if defined ( __linux__ )
						util_filesystem::File  boardSiteFile(BOARD_SITE_FILE_NAME);
						//are we running board site tests?
						if ( boardSiteFile.exists() )  {
							BoardSiteGeneric msg( BrdSitePeripheralFlashPass, this ); 
							dispatchEvent( &msg );						
						}
#endif
					}

					#ifdef PR_DEFAULTS_TEST
					Debug::trace("PrinterManager", "Total test messages: " + Integer::toString(testMsgCount) + " Total failures: " + Integer::toString(failedErases));
					#endif

					dispatchEvent(&msg);
					receivedPrinterFeedback();
				}
				break;
				case PrVersion:
				{
					PrinterVersion version(recvBuffer);
					version.show();
					
					if( prKind == KPCPrinter ) {
					    PrinterSystemStatus sysVer( this, FirstPrinter, KPCPrinter );
					    sysVer.setSoftwareVersion( version.getSoftwareMajor(), version.getSoftwareMinor(), version.getSoftwareBuild() );
					    sysVer.setHardwareVersion( version.getHardwareMajor(), version.getHardwareMinor() );
					    sysVer.show();
					    dispatchEvent( &sysVer );
				    }
					else if( prKind == SSKPCPrinter ) { 
						PrinterSystemStatus sysVer( this, FirstPrinter, SSKPCPrinter );
						sysVer.setSoftwareVersion( version.getSoftwareMajor(), version.getSoftwareMinor(), version.getSoftwareBuild() );
						sysVer.setHardwareVersion( version.getHardwareMajor(), version.getHardwareMinor() );
						sysVer.show();
						dispatchEvent( &sysVer );
					}
					else { 
						PrinterSystemStatus sysVer( this, FirstPrinter, ApcPrinter );
						sysVer.setSoftwareVersion( version.getSoftwareMajor(), version.getSoftwareMinor(), version.getSoftwareBuild() );
						sysVer.setHardwareVersion( version.getHardwareMajor(), version.getHardwareMinor() );
						sysVer.show();
						dispatchEvent( &sysVer );					
					}
				}
				break;
				case PrHeadType:
				{
					PrinterHeadType PHeadType(recvBuffer);
					PHeadType.show();
					Debug::trace("PrinterManager.HeadType"," handling Headtype Message.");

					Debug::trace("PrinterManager.HeadType", "PrHeadType "  + Integer::toString( PHeadType.getHeadType() ));
					Debug::trace("PrinterManager.HeadType", "PrHeadSize "  + Integer::toString( PHeadType.getHeadSize() ));
					dispatchEvent( &PHeadType );

				}
				break;
#ifdef FAKE_SOCKET_WEIGHER
				case PrAgentAttached:
				{
					agentAttached_ = true;				
				}
				break;
				case PrAgentDetached:
				{
					agentAttached_ = false;																	
				}
				break;
				case PrVirtualLabelComplete:
				{
					dispatchLabelTaken(pCurrentProductTransaction, currentTransNum, currentPackId);
					executingCommand = false;
					Debug::trace("PrinterManager.StateMachine", printerTraceString + " Virtual Print Command Complete");


					takeLabelError = false;

					if (pCurrentProductTransaction)
						const_cast<ProductTransaction *>(pCurrentProductTransaction)->incrementNumOfLabelsPrinted();

					Debug::trace("PrinterManager.StateMachine", "PrVirtualLabelComplete dispatching label complete");
					dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, true);
					//SOF-5075 for completeness
					pCurrentProductTransaction = NULL;
					currentTransNum = 0;


					testLabelPrintInProgress = false;

					printerState = PrinterReadyToPrint;
					Debug::trace("PrinterManager.StateMachine", printerTraceString + " 2) PrinterReadyToPrint");

				}
				break;
#endif
				case PrStatus:
				{
					Debug::trace("PrinterManager.Message", printerTraceString + "PrStatus");

					if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale())
					{
						if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeSearchKeypadFlashkeys) && (Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors) == SelfServOption_PrintToScreenAndLabel))
							Debug::trace("PrinterManager", "PrinterManager Don't skip handlePrinterMessage(PrStatus) if BigY");
						else
						{
							Debug::trace("PrinterManager", "PrinterManager SKIP handlePrinterMessage(PrStatus) if Not BigY");
							return;
						}
					}

					bool isTracing = Debug::isTracing("PrinterManager.PrinterStatus");
					if (isTracing)
						Debug::trace("PrinterManager.PrinterStatus", printerTraceString + "PrStatus");
					receivedPrinterFeedback();
					PrinterStatus printerStatus(recvBuffer);

					printerStatus.setSourceObject(this);
					printerStatus.setPrinterType(myPrinterType);

					//Debug::trace("PrinterManager.StateMachine", "printerStatus.state: 0x" + Hex::toString(printerStatus.getState()));
					Debug::trace("PrinterManager.PrinterStatus", "printerStatus.sensor: 0x" + Hex::toString(printerStatus.getSensor()));
					Debug::trace("PrinterManager.PrinterStatus", "printerStatus.user: 0x" + Hex::toString(printerStatus.getUser()));
					Debug::trace("PrinterManager.PrinterStatus", "printerStatus.counter: 0x" + Hex::toString(printerStatus.getCounter()));

					if ( myPrinterType == FirstPrinter && pConfig->isConserveContinuousStockByNotSizingEnabled() )
					{
						// If we know we are using continuous stock, set the Printer Status message to indicate continuous, even if die-cut stock is loaded
						if (usingContinuousStock)
						{
							printerStatus.setCounter(0);
							if (isTracing)
								Debug::trace("PrinterManager.PrinterStatus", printerTraceString + " PrStatus at beginning of handler - setting counter to 0 for continuous labels");
						}
					}

					if (isTracing)
					{
						Debug::trace("PrinterManager.PrinterStatus", printerTraceString + "PrStatus at beginning of handler *****************************************");
						printerStatus.show();
						Debug::trace("PrinterManager.PrinterStatus", printerTraceString + "**************************************************************************");
						//PAL_absclass::PAL_sleep( 500 );
					}

					if ((lastStatusMsg.isPeripheralComm() == false) && (printerStatus.isPeripheralComm() == true))
					{
						sendPrinterType();
					}

					if (printerStatus.isLabelTaken() == false)
					{
						labelPresent = true;
						if (isTracing)
							Debug::trace("PrinterManager.PrinterStatus", printerTraceString + " labelPresent");
					}

					// SOF-3858 clear missing label if cutter is installed and enabled.
					// SOF-3856 this will allow 2nd label to print.
					if( prKind != SSKPCPrinter ) {	// SOF-5057
						if( isCutterInstalled && isCutterEnabled ) {
							printerStatus.clearLabelMissing();	
							lastStatusMsg.clearLabelMissing();
						}
					}

					// the printer can only detect this after the label stops printing.
					// it's possible for the user to remove label from printer quickly and
					// turn on the error bit.
					if ((labelPresent == true) && (printerStatus.isLabelMissing() == true))
					{
						Debug::trace("PrinterManager.PrinterStatus", printerTraceString + " False Label Missing Detected");
						if( ( prKind != KPCPrinter ) && ( prKind != SSKPCPrinter ) ) {						
							printerStatus.clearLabelMissing();
							if (isTracing)
								Debug::trace("PrinterManager.PrinterStatus", printerTraceString + " False Label Missing Detected");
						}
					}

					// SOF-5381 - this was only reported on FSS, so only going to apply this there.
					if (SSKPCPrinter == prKind && printerState == PrinterReadyToPrint && pCurrentProductTransaction != NULL && printerStatus.isLabelTaken())
					{
						// then there's no reason to show the take label video...
						dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, true);
						pCurrentProductTransaction = NULL;
						currentTransNum = 0;
						currentPackId = 0;
					}

					if (isTracing)
						Debug::trace("PrinterManager.PrinterStatus", printerTraceString + " StateMachine: " + HobString(Integer::toString(printerState)));
					if ((printerStatus.isPriorityError() == true) && !((printerState == PrinterPriorityError) || (printerState == PrinterHeadDisengaged)))
					{
						if (isTracing)
							Debug::trace("PrinterManager.PrinterStatus", printerTraceString + " checking PriorityError");
						if (continuousTestLabels == true)
						{
							continuousTestLabels = false;
							cancelTimer( continuousLabelTimerExpired );
						}
						testLabelPrintInProgress = false;

						if ((printerState == PrinterTryingToSize) && (printerStatus.isFindCmdComplete() || printerStatus.isFindwoLabelTakenCmdComplete()) && printerStockMetrics.isContinuousStock() && printerStatus.isMediaJammed())
						{
							// When the printer is working on the FIND command in the teach table for sizing, it will report that
							// the media is jammed when it can't find the black bar - which it won't find with continuous stock.
							// So, don't consider this specific situation a PrinterPriorityError.
							printerStatus.clearMediaJammed();
						}
						else
						{
							executingCommand = false;
							Debug::trace("PrinterManager.CMD","(2)executingCommand: " + Integer::toString(executingCommand));
							if (pendingLabelXfer == true)
							{
								pendingLabelXfer = false;

								sendRequestLabelTransfer(0, NULL, false, true);
								Debug::trace("PrinterManager.LabelTransfer", printerTraceString + " Aborting up on Bulk Transfer");

								labelDataSize = 0;
								labelData = NULL;
								outOfDataBuffers = false;
							}

							
							printerState = PrinterPriorityError;
							Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterPriorityError");

//if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale() )
//{
//	Debug::trace("PrinterManager.TestScreenLabel", "handlePrinterMessage(PrStatus) still do printerState = PrinterReadyToPreImage;");
//	printerState = PrinterReadyToPreImage;
//	labelWidth = WideLabel;
//}

							Debug::trace("PrinterManager.PrinterReset", printerTraceString + " resetPrinter1");
							sendPrinterFlush();
							emptyProductTransactionQ(false);
							emptyCommandQ();
							

							pCurrentProduct = NULL;
							Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));
							currentProductSmartPtr = Util::CSmartPtr<Product>();

							pCurrentLabel = NULL;

							if (printingVoidLabel == true)
							{
								printingVoidLabel = false;

								if (testLabelType == PrVoidLabel)
								{
									testLabelType = saveTestLabelType;
								}
							}
							if( ( pCurrentProductTransaction != NULL ) && ( currentTransNum != 0 ) ) {
								dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, false);
								pCurrentProductTransaction = NULL;
								currentPackId = 0;
								currentTransNum = 0;
							}
						}
					}

					Debug::trace("PrinterManager.LabelTransfer", printerTraceString + " outOfDataBuffers: " + HobString(Bool::toString(outOfDataBuffers)));
					if ((outOfDataBuffers == false) && (printerStatus.isOutOfDataBuffers() == true))
					{
						if( ( prKind == KPCPrinter ) || ( prKind == SSKPCPrinter ) ) {
							if( labelDataSize > 0 ) {
								Debug::trace("PrinterManager.LabelTransfer", "Sending rest of image!");
								sendLabelData();
							}
						} else {
							outOfDataBuffers = true;
						}
					}

					if ((outOfDataBuffers == true) && (printerStatus.isOutOfDataBuffers() == false))
					{
						outOfDataBuffers = false;
						if ((labelDataSize > 0) && (pendingLabelXfer == false))
							sendLabelData();
					}

					Debug::trace("PrinterManager", "PrinterStatus.command " + Hex::toString(printerStatus.getCommand()) );

					if ((printerResetting == true) && (printerStatus.isResetCmdComplete() == true))
					{
						executingCommand = false;
						Debug::trace("PrinterManager.CMD","(3)executingCommand: " + Integer::toString(executingCommand));
						printerResetting = false;
						Debug::trace("PrinterManager.PrinterReset", printerTraceString + " 2printerResetting: " + HobString(Bool::toString(printerResetting)));

					}
					else if ((printerCalibrating == true) && (printerStatus.isCalibrateCmdComplete() == true))
					{
						executingCommand = false;
						Debug::trace("PrinterManager.CMD","(4)executingCommand: " + Integer::toString(executingCommand));
						printerCalibrating = false;
						Debug::trace("PrinterManager.PrinterCalibration", printerTraceString + " printerCalibrating: " + HobString(Bool::toString(printerCalibrating)));
					}
					else if (((printerAdvancing == true) && (printerStatus.isAdvanceCmdComplete()) == true))// && (pcb->commands.resetting == FALSE))
					{
						executingCommand = false;
						Debug::trace("PrinterManager.CMD","(5)executingCommand: " + Integer::toString(executingCommand));
						printerAdvancing = false;
						Debug::trace("PrinterManager.PrinterAdvance", printerTraceString + " printerAdvancing: " + HobString(Bool::toString(printerAdvancing)));
					}
					else if ( printerStatus.isCutRetractCmdComplete() && printerState != PrinterTryingToSize){
						executingCommand = false;
						Debug::trace("PrinterManager.CMD","(6)executingCommand: " + Integer::toString(executingCommand));
						Debug::trace("PrinterManager.PrinterAdvance", printerTraceString + " printerAdvancing: " + HobString(Bool::toString(printerAdvancing)));
					}
					else
					{
						if (lastStatusMsg.isTakeLabelError() == true)
						{
							printerStatus.setTakeLabelError();							
						}

						if (lastStatusMsg.isLabelSizing() == true)
						{
							printerStatus.setLabelSizing();
						}

						if (lastStatusMsg.isPeripheralComm() == false)
						{
							printerStatus.clearPeripheralComm();
						}

						if (printerStatus.isHeadDisengaged() == true)
						{
							Debug::trace("PrinterManager.StateMachine", "printerState3: " + Integer::toString(printerState)); 
							if (printerState != PrinterHeadDisengaged)
							{

								if( !printerCalibrating ) { 	//SOF-4727
									if( ( prKind != KPCPrinter ) || !Configuration::getInstance()->isConserveContinuousStockByNotSizingEnabled() ) { 
							   		Debug::trace("PrinterManager.StateMachine", " sendPrinterReset(2)");
							   		sendPrinterReset(2);
							   	}
							   	printerState = PrinterHeadDisengaged;
							   	Debug::trace("PrinterManager.StateMachine", " PrinterHeadDisengaged");

							 	 	if( prKind != SSKPCPrinter ) {	// SOF-5062		
							 	 			Debug::trace("PrinterManager.StateMachine", "Configuration::getInstance()->isConserveContinuousStockByNotSizingEnabled(): " + Integer::toString(Configuration::getInstance()->isConserveContinuousStockByNotSizingEnabled()));
							   			if( !Configuration::getInstance()->isConserveContinuousStockByNotSizingEnabled() ) {
							   				clearLabelDimensions();
							   				Debug::trace("PrinterManager.StateMachine", " clearLabelDimensions()");							
							   			}	   	
							   	} else {											   						  
							   		if( prKind == SSKPCPrinter ) { 
							   			//SOF-5357: if we are in self service mode printing to the screen then do not clear label dimnesions 
							   			if( ( !isCutterEnabled ) && ( !Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen ) ) ){
							   				clearLabelDimensions();
							   				Debug::trace("PrinterManager.StateMachine", " clearLabelDimensions()");	
							   			}
							   		} else {
							   			if( !isCutterEnabled ) {
							   				clearLabelDimensions();
							   				Debug::trace("PrinterManager.StateMachine", " clearLabelDimensions()");
							   			}								   			
							   		}
							   	}
								}
							}						  
						} else {
							if (printerState == PrinterHeadDisengaged)
							{
								if( prKind != SSKPCPrinter ) { // SOF-5062
									//Debug::trace("PrinterManager.StateMachine", " PrinterHeadDisengaged printerStockMetrics.isContinuousStock(): " + Integer::toString(printerStockMetrics.isContinuousStock()));
									if( !Configuration::getInstance()->isConserveContinuousStockByNotSizingEnabled() )  {
										Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterUnsized");
										printerState = PrinterUnsized;	
									} else {
										Debug::trace("PrinterManager.StateMachine", " => PrinterReadyToPreImage");
										printerState = PrinterReadyToPreImage;	

										LabelWidth printerStatusLabelWidth = ( !getLabelWidthFromStatus ) ? labelWidth : ( ( printerStatus.isNarrowLabel() ) ? NarrowLabel : WideLabel );
										printerConfig.setLabelWidth(printerStatusLabelWidth);
										checkLabelWidth(printerStatusLabelWidth);
	
										printerStatus.setCommand( SizeCmdComplete ); 
										usingContinuousStock = true;							 
									}
								} else {									
									//SOF-5083: if interlock is open then wait for closure.
									if( ( prKind == SSKPCPrinter ) && printerStockMetrics.isContinuousStock() &&
										  !lastCutterStatusMsg.getInterlock() ) {
										Debug::trace("PrinterManager.StateMachine", "SSKPCPrinter: PrinterHeadDisengaged");
										printerState = PrinterWaitInterlock;
										Debug::trace("PrinterManager.StateMachine", "SSKPCPrinter: PrinterWaitInterlock");
									} else {
										Debug::trace("PrinterManager.StateMachine", "SSKPCPrinter: PrinterHeadDisengaged isContinuousStock(): " + Integer::toString(printerStockMetrics.isContinuousStock()));
										Debug::trace("PrinterManager.StateMachine", "SSKPCPrinter: mode: " + Integer::toString(pConfig->isConserveContinuousStockByNotSizingEnabled()));
										Debug::trace("PrinterManager.StateMachine", "SSKPCPrinter: interlock: " + Integer::toString(lastCutterStatusMsg.getInterlock()));
										//SOF-5308: if the interlock is closed but the head is up (service bypass interlock).
										if( ( prKind == SSKPCPrinter ) && printerStockMetrics.isContinuousStock() &&
												lastCutterStatusMsg.getInterlock() && pConfig->isConserveContinuousStockByNotSizingEnabled() ) {
											Debug::trace("PrinterManager.StateMachine", "SSKPCPrinter: Continuous mode, head engaged, eject and cut");
											printerState = PrinterWaitInterlock;
										} else {
											Debug::trace("PrinterManager.StateMachine", "SSKPCPrinter: Sizing mode, head engaged size");
											printerState = PrinterUnsized;								
										}
										//SOF-5357: if the interlock is open and we are using diecut labels then wait for closure.
										if( ( prKind == SSKPCPrinter ) && ( !printerStockMetrics.isContinuousStock() ) && 
											  !lastCutterStatusMsg.getInterlock() ) {
												Debug::trace("PrinterManager.StateMachine", "SSKPCPrinter: PrinterHeadDisengaged => Die Cut");
												printerState = PrinterWaitInterlock;
												Debug::trace("PrinterManager.StateMachine", "SSKPCPrinter: PrinterWaitInterlock");
										}
									}
								}								
							}
						}


						if (((printerState == PrinterXferringLabel) && (printerStatus.isTaking()) == true) && (printerStatus.getCommand() != VirtualLabelTaken))
						{
							printerState = PrinterTakingLabel;
							Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterTakingLabel");
							//SOF-5066
							if( isCutterInstalled && isCutterEnabled ) {																
								if( !lastCutterStatusMsg.getInterlock() ) { 	
									requestCutterStatus();
									Debug::trace("PrinterManager.interlock", "interlock is open report!");
									

									//wait for interlock closure to expel and cut unless we are in service.				 		//SOF-5161
									if( ( ( prKind == SSKPCPrinter ) && printerStockMetrics.isContinuousStock() ) && !printerCalibrating ) { 
										Debug::trace("PrinterManager.StateMachine", "PrinterWaitToExpelAndCut" );
										printerState = PrinterWaitToExpelAndCut;
									} 
								}
							}
						}

						//SOF-3861
						if( ( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) || ( prKind == SSKPCPrinter ) ) && ( printerState == PrinterXferringLabel ) && 
						    ( takeLabelTime == 0 ) && printerStatus.isPrintCmdComplete() ) {
							printerState = PrinterLabelIsReady;
						}

					    // SOF-5395 This breaks Production Entry streaming of die cut labels so put an #if 0 around it!
					    // WAIT - we still need this because of SOF-5497 4.2 NGW "Take Label" prompt given a very fast, NGW will only take one pkg at a time
					    // so I ADDED a condition around it that says DO THIS ONLY IF IF NOT Production Entry which protects SOF-5395
						//SOF-5369 Don't Apply short label take up time on PrePack Models
					    OpModeEvent mEvent = RealTimeController::getInstance()->getCurrentOpModeEvent();
					    if ((mEvent != OpModeProductionEntry))
					    {
						    if( ( prKind != EppPrinter ) && ( prKind != Epp2Printer ) && (prKind != Epp3Printer) )
						    {		
							    if (printerState == PrinterTakingLabel && isDieCutloaded()) {
								    takeLabelTime = ticksPerSecond / 2;
							    }
						    }
                            // FAIL SAFE - already know we are NOT Prod Entry
							if ( ( pConfig->isSelfServiceScale() == false ) && ( PrTakeLabelTime <= 500 ) )
							{
								if ( ( (printerState == PrinterTakingLabel) && ( !isCutterInstalled ) && ( isDieCutloaded() ) ) && ( (mEvent == OpModeAutoWeigh ) || ( mEvent == OpModeSemiWeigh ) ) )
								{
									// A lot of tests to get here.  Very specific.  If we have LABEL+WRAP+APPLY ALL on, or LABEL+APPLY - set timer to 12000
									Debug::trace("PrinterManager.TakeLabelTimeJoe", "========== #0 ORIGINAL UDPATE THE TIMER UPDATE THE TIMER UPDATE THE TIMER ==========");
									takeLabelTime = 12000;
								}
							}
					    }


						if ((printerState == PrinterTakingLabel) && (printerStatus.isLabelTaken() == false || takeLabelTime == 0))// && (pcb->commands.resetting == FALSE))
						{
							
							printerState = PrinterLabelIsReady;
							printerStatus.setLabelReady();
							if( ( takeLabelTime != 0 ) && ( !isCombinedVirtualPrint() ) )
							{
								Debug::trace("PrinterManager.TakeLabelTime", "createTimer: 3");
								createTimer(takeLabelTime, takeLabelTimerExpired, true);
							}							
							dispatchLabelReady(pCurrentProductTransaction, currentTransNum, currentPackId);
						} 
							
						

						Debug::trace("PrinterManager.PrinterStatus", printerTraceString + " takeLabelTime: " + Integer::toString(takeLabelTime));

						// --------------------------
						// KPC ERRATA And Workarounds
						// --------------------------
						bool kpcPrinterIgnoreMultipleLabelTakenMessages = false;
						bool kpcPrinterHavePriorityErrorContinuousStock = false;

						// SOF-3560
						if ( ( ( KPCPrinter == prKind ) ||( prKind == SSKPCPrinter ) ) && 
						     ( printerStatus.isPrintCmdComplete() ) && 
							 ( ( PrinterLabelIsReady == printerState ) || 
							 ( PrinterTakingLabel == printerState ) || 
							 ( PrinterXferringLabel == printerState ) ) )

							kpcPrinterIgnoreMultipleLabelTakenMessages = true;
						//SOF-3560
						if ( ( ApcPrinter == prKind ) || ( USBPrinter == prKind ) || ( Epp2Printer == prKind ) || ( Epp3Printer == prKind ) || ( FakePrinter == prKind ) ){
							kpcPrinterIgnoreMultipleLabelTakenMessages = true;
						}

						// SOF-3553
						if ( ( ( KPCPrinter == prKind ) || ( prKind == SSKPCPrinter ) ) && 
						     ( PrinterPriorityError == printerState ) && 
							 ( printerStockMetrics.isContinuousStock() ))
							kpcPrinterHavePriorityErrorContinuousStock = true;
						
						//SOF-3998
						if( ( ( KPCPrinter == prKind ) || ( prKind == SSKPCPrinter ) ) &&
						    ( isDieCutloaded() ) && ( printerStatus.isLabelTaken() ) && 
							( printerState == PrinterReadyToPrint) )
							kpcPrinterIgnoreMultipleLabelTakenMessages = true;
						

				
					    //OpModeEvent mEvent = RealTimeController::getInstance()->getCurrentOpModeEvent();
						//SOF-5366 ingnore multiple label taken messages for delhaize
						if( ( ( KPCPrinter == prKind ) ) && ( mEvent == OpModeProductionEntry ) &&
						    ( isContinuousStock() ) && ( printerStatus.isLabelTaken() ) && 
							  ( printerState == PrinterTakingLabel ) ) {							
							kpcPrinterIgnoreMultipleLabelTakenMessages = true;
						}

						Debug::trace("PrinterManager.PrinterStatus", printerTraceString + " KPC ERRATA 1: " + Bool::toString(kpcPrinterIgnoreMultipleLabelTakenMessages));
						Debug::trace("PrinterManager.PrinterStatus", printerTraceString + " KPC ERRATA 2: " + Bool::toString(kpcPrinterHavePriorityErrorContinuousStock));

						// ---------------
						Debug::trace("PrinterManager.StateMachine", "isPrintCmdComplete(): " + Integer::toString( printerStatus.isPrintCmdComplete() ));
						//Debug::trace("PrinterManager.StateMachine", "printerState: " + Integer::toString( printerState ));
						Debug::trace("PrinterManager.StateMachine", "isLabelTaken(): " + Integer::toString( printerStatus.isLabelTaken() ));
						//Debug::trace("PrinterManager.StateMachine", "takeLabelTime: " + Integer::toString( takeLabelTime ));
						if (( printerStatus.isPrintCmdComplete() && takeLabelTime != 0 && kpcPrinterIgnoreMultipleLabelTakenMessages ) || 
							( printerState == PrinterLabelIsReady && takeLabelTime == 0 ) || 
							( kpcPrinterHavePriorityErrorContinuousStock ) || ( printerState == PrinterPriorityErrorRestart ))							
						{
							bool haveAnotherLabelToPrint = false;  // for virtual printing
							// SOF-3861
							if( ( KPCPrinter == prKind ) || ( SSKPCPrinter == prKind ) ) {
								if( printerState == PrinterPriorityErrorRestart ) {
									printerState = PrinterLabelIsReady;	
								}
								sendTestLabel = false;
							}
							dispatchLabelTaken(pCurrentProductTransaction, currentTransNum, currentPackId);
							executingCommand = false;
							Debug::trace("PrinterManager.CMD","(7)executingCommand: " + Integer::toString(executingCommand));
							Debug::trace("PrinterManager.StateMachine", printerTraceString + " Print Command Complete");

							if ( (pHeelBufferStart) && (pHeelBuffer != NULL) && (pHeelBuffer != nullptr) )
							{
								Debug::trace("PrinterManager.StateMachine", "delete[] pHeelBufferStart");
								delete[] pHeelBufferStart;		// SOF-3325	Failed test : Allow unlimited number of sides IN /trunk 02/24/2020
								pHeelBuffer = nullptr;
								pHeelBufferStart = nullptr;
							}

							if (takeLabelError == true)
							{
								Debug::trace("PrinterManager.StateMachine", printerTraceString + " *** takeLabelError == true ***");
							}

							takeLabelError = false;

							if (takeLabelTime == 0)
							{
								sendPrinterFlush();
								sendPrinterReset(0);
								emptyCommandQ();
								sendPrinterEnableTakeup();
							}

							if (printingVoidLabel == true)
							{
								printingVoidLabel = false;

								if (testLabelType == PrVoidLabel)
								{
									testLabelType = saveTestLabelType;
								}
							}
							else
							{
								if (printerStatus.isLabelMissing() == true)
								{
									// no label was printed
									Debug::trace("PrinterManager.LabelMissing", printerTraceString + " Label Missing");
									dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, false);
									//SOF-5075 
									pCurrentProductTransaction = NULL;
									currentTransNum = 0;

								}
								else
								{
									Debug::trace("PrinterManager.StateMachine", "Virtual Printing Enabled " + Long::toString(virtualPrintingEnabled));

									// if virtual printing, every single label does not count as a label printed.
									// If multiple labels are printed because the product has multiple label types set up (2 for example),
									// in production entry mode, we only incrementNumOfLabelsPrinted when both labels have printed, not each one. 

									// if virtual printer, here is where we should go ahead and print the next label
									if ( (virtualPrintingEnabled) && ( !kpcPrinterHavePriorityErrorContinuousStock ) )
									{
										for (int i = (short)currentVirtuallyPrintingLabelNum + 1; i < (short)MaxPrinterTypes; i++)
										{
											if (imageControlBlock[i])
											{
												currentVirtuallyPrintingLabelNum = (PrinterType)i;
												haveAnotherLabelToPrint = true;
												break;
											}
										}

										Debug::trace("PrinterManager.StateMachine", "HaveAnotherLabelToPrint " + Long::toString(haveAnotherLabelToPrint));

										if ( haveAnotherLabelToPrint ) {
											Debug::trace("PrinterManager.StateMachine", "Have another label to print calling startPrintingLabel()");
											startPrintingLabel(currentVirtuallyPrintingLabelNum);
										} else {
											//no more virtual labels to print.  
											if( noCutOption && isCutterInstalled  && isCutterEnabled &&
											 ( ( printerMode == ContinuousLabelHeelCombined ) || 					//SOF-3933
											   ( printerMode == ContinuousLabelHeadCombined ) ) ) {	
												queueCommand( VirtualLabelTaken, PrNoSecurityLabel, PrNoIndirection, 0 );
												Debug::trace("PrinterManager.StateMachine", "Cut Label");											  
											}
										}

										//SOF-3933 re-teach printer if we just printed a test label from Printer Settings.
										if( reTeach ) {
											handleMode( RealTimeController::getInstance()->getCurrentOpModeEvent() ); 
											reTeach = false;
										}

									} else {
										haveAnotherLabelToPrint = false;
									}

									if (!virtualPrintingEnabled || !haveAnotherLabelToPrint)
									{
										Debug::trace("PrinterManager.StateMachine", "Current mode: " + Integer::toString(RealTimeController::getInstance()->getCurrentOpModeEvent()));							
										// SOF-5113 / SOF-5395: only wait to dispatch when in self serve mode or production entry. 
										//											This keep the video up and playing in self service mode until
										//											the user picks the label. This also keep production entry from
									  //											printing additional labels until label has been picked.
									  //											This only applies to FSSS. 
										OpModeEvent mEvent = RealTimeController::getInstance()->getCurrentOpModeEvent();										
										if ( ( prKind == SSKPCPrinter ) && 
											  ( Configuration::getInstance()->isSelfServiceScale() || mEvent == OpModeProductionEntry ) )
										{
											Debug::trace("PrinterManager.StateMachine", "Wait to dispatch label complete");
											if ( printerStatus.isLabelTaken() )
											{													
												if (pCurrentProductTransaction)
												{
													const_cast<ProductTransaction *>(pCurrentProductTransaction)->incrementNumOfLabelsPrinted();
													Debug::trace("PrinterManager.StateMachine", "Number printed: " + Integer::toString(const_cast<ProductTransaction *>(pCurrentProductTransaction)->getNumOfLabelsPrinted()) );											
												}

												Debug::trace("PrinterManager.StateMachine", "Dispatching label complete");											
												dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, true);
												pCurrentProductTransaction = NULL;
												currentPackId = 0;
											}
											else
											{
												Debug::trace("PrinterManager.TakeLabelTime", "printerState: " + Integer::toString(printerState));
												if( mEvent == OpModeProductionEntry && isDieCutloaded() && !printerStatus.isLabelTaken() && takeLabelTime == 0 )
												{
													takeLabelTime = ticksPerSecond / 2;
													Debug::trace("PrinterManager.TakeLabelTime", "createTimer: 2ab");
													createTimer(takeLabelTime, takeLabelTimerExpired, true);
												}												
											}
										} 
										else
										{	
											Debug::trace("PrinterManager.rfid", "!SSKPCPrinter or !selfserve and !ProductionEntry");
											if ( pCurrentProductTransaction )
											{
												Debug::trace("PrinterManager.rfid", string("pCurrentProductTransaction:") + ptrtostring(pCurrentProductTransaction));
												if ( pCurrentProductTransaction->isRfidVoidLabelPrinted() )
													Debug::trace("PrinterManager.rfid", "printed RFID Void label - didn't increment labels printed: " + inttostring(pCurrentProductTransaction->getNumOfLabelsPrinted()));
												else
												{
													const_cast<ProductTransaction*>(pCurrentProductTransaction)->incrementNumOfLabelsPrinted();
													Debug::trace("PrinterManager.StateMachine", "Number printed: " + Integer::toString(const_cast<ProductTransaction*>(pCurrentProductTransaction)->getNumOfLabelsPrinted()));
												}
											}
																							
											Debug::trace("PrinterManager.StateMachine", "Dispatching label complete");
											dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, true);
											pCurrentProductTransaction = NULL;
											currentPackId = 0;											
										}

										if( ( isCombinedVirtualPrint() ) && ( ( printerMode == ContinuousLabelHeadCombined  ) || 
											( printerMode == ContinuousLabelHeelCombined ) ) && ( !testLabelPrintInProgress ) ) {
											printerState = PrinterLabelIsReady;
											printerStatus.clearLabelTaken();
											printerStatus.setLabelReady();
											Debug::trace("PrinterManager.TakeLabelTime", "createTimer: 2");
											createTimer(3000, takeLabelTimerExpired, true);
										}
									} else {						
										// SOF-5113 / SOF-5395: only wait to dispatch when in self serve mode or production entry. 
										//											This keep the video up and playing in self service mode until
										//											the user picks the label. This also keep production entry from
									  //											printing additional labels until label has been picked.
									  //											This only applies to FSSS. 
										if( !haveAnotherLabelToPrint ) {
											OpModeEvent mEvent = RealTimeController::getInstance()->getCurrentOpModeEvent();										
											if( ( prKind == SSKPCPrinter ) && ( ( Configuration::getInstance()->isSelfServiceScale() ) || 
																													( mEvent == OpModeProductionEntry ) ) ) {
												Debug::trace("PrinterManager.StateMachine", "Wait to dispatch label complete");
												if( printerStatus.isLabelTaken() )
												{	
													if (pCurrentProductTransaction) {
														const_cast<ProductTransaction *>(pCurrentProductTransaction)->incrementNumOfLabelsPrinted();
														Debug::trace("PrinterManager.StateMachine", "Number printed: " + Integer::toString(const_cast<ProductTransaction *>(pCurrentProductTransaction)->getNumOfLabelsPrinted()) );											
													}

													Debug::trace("PrinterManager.StateMachine", "Dispatching label complete");											
													dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, true);
													pCurrentProductTransaction = NULL;
													currentPackId = 0;
												}
											} else {																								
													if (pCurrentProductTransaction) {
														const_cast<ProductTransaction *>(pCurrentProductTransaction)->incrementNumOfLabelsPrinted();
														Debug::trace("PrinterManager.StateMachine", "Number printed: " + Integer::toString(const_cast<ProductTransaction *>(pCurrentProductTransaction)->getNumOfLabelsPrinted()) );											
													}

													Debug::trace("PrinterManager.StateMachine", "Dispatching label complete");
													dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, true);
													pCurrentProductTransaction = NULL;
													currentPackId = 0;											
											}										
										}
									}
								}
							}

							Debug::trace("PrinterManager.StateMachine", "**testLabelPrintInProgress: " + Integer::toString(testLabelPrintInProgress) );
							Debug::trace("PrinterManager.StateMachine", "**printerMode:" + Integer::toString(printerMode) );
							Debug::trace("PrinterManager.StateMachine", "**prKind:" + Integer::toString(prKind) );
							// Freestanding service scale need to retract after test label print.
							if( testLabelPrintInProgress && ( ( printerMode == ContinuousShootLabelHeelCut) || 
							  ( printerMode == ContinuousLabelHeelCombined ) ) &&  ( SSKPCPrinter == prKind ) ) {
								/* SOF-4914 */
								//queueCommand( BackupCommand, PrNoSecurityLabel, PrNoIndirection, 0 );
							}

							testLabelPrintInProgress = false;

							if (continuousTestLabels == true)
							{
								createTimer(continuousLabelTime, continuousLabelTimerExpired);
							}
							else
							{

								//	PTR 1399: Don't change to PrinterReadyToPrint if (printerState == PrinterPriorityError)
								if ( !haveAnotherLabelToPrint && (printerState != PrinterPriorityError) && 
																 (printerState != PrinterUnsized) && 
																 (printerState != PrinterHeadDisengaged) )
								{
									if( SSKPCPrinter == prKind ) {										
										if( ( takeLabelTime != 0 ) && !printerStatus.isLabelTaken() && printerState != PrinterLabelIsReady ) {
											printerState = PrinterLabelIsReady;
											printerStatus.clearLabelTaken();
											printerStatus.setLabelReady();
											Debug::trace("PrinterManager.TakeLabelTime", "createTimer: 2a");
											createTimer(takeLabelTime, takeLabelTimerExpired, true);
										} else {
											//if( (printerState == PrinterLabelIsReady) && printerStatus.isLabelTaken() )
											printerState = PrinterReadyToPrint;
											Debug::trace("PrinterManager.StateMachine", printerTraceString + " 3a) PrinterReadyToPrint");
										}
									} else {
									    printerState = PrinterReadyToPrint;
									    Debug::trace("PrinterManager.StateMachine", printerTraceString + " 3) PrinterReadyToPrint");
								  }
							  }
						  }
						} else {	
							//SOF-5366 if we are in production entry and get a false trip of label sensor.
							OpModeEvent mEvent = RealTimeController::getInstance()->getCurrentOpModeEvent();						
							if( ( KPCPrinter == prKind ) && ( printerState == PrinterReadyToPrint ) && 
									( printerStatus.isLabelTaken() ) && ( mEvent == OpModeProductionEntry ) ) {
									Debug::trace("PrinterManager.LabelSensor", "Clear label taken");
									printerStatus.clearLabelTaken();

									//did we also miss sending the label complete? 
									if( pCurrentProductTransaction != NULL )  {
										Debug::trace("PrinterManager.StateMachine", " SOF-5366 Fix: dispatchLabelComplete");
										dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, true);
										pCurrentProductTransaction = NULL;
										currentPackId = 0;
									}

							}
						}
						
						if (printerState == PrinterTryingToSize)
						{
							bool isTracingSize = Debug::isTracing("PrinterTeachTable");
							printerStatus.show();
							if (isTracingSize)
								Debug::trace("PrinterTeachTable", printerTraceString + " handling PrStatus - entering block for PrinterTryingToSize");
							if (printerStatus.isSizeCmdComplete() == true)
							{
								if (isTracingSize)
									Debug::trace("PrinterTeachTable", printerTraceString + " handling PrStatus - isSizeCmdComplete() == true");
								executingCommand = false;
								Debug::trace("PrinterManager.CMD","(8)executingCommand: " + Integer::toString(executingCommand));
								printerStatus.clearLabelSizing();
								/* This would only be used if we are trying to minimize the waste when sizing die-cut labels.
								* This functionality is not currently working, so it is being commented out.
								* I intend to return to this in the future.
								if ( printerMode == DieCutLabelHead || printerMode == DieCutLabelHeel || printerMode == DieCutStreamHead || printerMode == DieCutStreamHeel )
								{
									measuredSize = 1;
									actualSize = pConfig->toIntCfg(fieldOffsetCfgPrinter1LabelLength + myPrinterType);
								}
								else
								{
								*/
									measuredSize = printerStatus.getCounter();
									actualSize = (UINT16)printerStockMetrics.getActualLabelLengthLines(measuredSize, prKind);

									if( cassetteTest ) {
										cassetteTest = false;
										PrinterTestCassette msg( this, measuredSize, actualSize );
										msg.show();
										dispatchEvent( &msg );
									}

								/*
								}
								*/

								if (isTracingSize)
								{
									Debug::trace("PrinterTeachTable", printerTraceString + " Size Command Completed");
									Debug::trace("PrinterTeachTable", printerTraceString + " Actual Size: " + HobString(Integer::toString(actualSize)));
									Debug::trace("PrinterTeachTable", printerTraceString +  " Measured Size: " + HobString(Integer::toString(measuredSize)));
									Debug::trace("PrinterTeachTable", printerTraceString + " printerMode: " + HobString(Integer::toString(printerMode)));
								}

								if (actualSize > 0)
								{
									// Sized a die-cut label

									if (isTracingSize)
										Debug::trace("PrinterTeachTable", printerTraceString + " handling PrStatus - actualSize > 0");

									if (measuredSize > 0)
									{
										usingContinuousStock = false;
										

										//SOF-5110 if using die cut labels then do not enable cutter.
										if( prKind == SSKPCPrinter ) {
											Debug::trace("PrinterManager.Cutter", "Disable cutter and send to session." );
											Configuration::getInstance()->setCutterEnabled(false);
											
											PrinterGeneric gen( PrCutterEnableSave, this, myPrinterType );											
											dispatchEvent( &gen );

											isCutterEnabled = false;
										}

										// Refer to SOF-5623 for the code below, because I have a hunch that this code being here is why 5623 came up
										//SOF-3980
										if( takeLabelTime == 0 ) {
											Debug::trace("PrinterManager.TakeLabelTime", "*********** handling PrStatus setting takeLabelTime to .5 sec:  sized die-cut labels, and takeLabelTime is currently 0");
											takeLabelTime = ticksPerSecond / 2;
										}
				
										if (isTracingSize)
											Debug::trace("PrinterTeachTable", printerTraceString + " handling PrStatus - measuredSize > 0");
										if (printHeelFirst == true)
										{
											if( prKind != SSKPCPrinter ) {
											    printerMode =  DieCutLabelHeel;
											} else {
												printerMode = DieCutShootHeel;
											}
										}
										else
										{
											if( prKind != SSKPCPrinter ) {
											    printerMode =  DieCutLabelHead;
											} else {
												printerMode = DieCutShootHead;
											}
										}

										if (pMyPrinter != NULL)
										{
											Debug::trace("PrinterManager.PrinterNomSize", printerTraceString + " handling printerStatus.isSizeCmdComplete() - NOT continuous - actualSize=" + inttostring(actualSize) + "   measuredSize=" + inttostring(measuredSize));	
											PrinterNomSize prNomSize(this, myPrinterType, actualSize, measuredSize);

											const char *pCC = prNomSize.serialize();
											pMyPrinter->send(pCC, prNomSize.getSerializedLength());

											printerStockMetrics.setLabelLengthInLines(actualSize);
										}

										teachPrinter(printerMode);		// FAILS #5
									}
								}
								else
								{
									if (isTracingSize)
										Debug::trace("PrinterTeachTable", printerTraceString + " handling PrStatus - actualSize = 0");
									if ( !printerStatus.isOutOfMedia() | !printerStatus.isOutOfStock() )
									{
										// Length determined to be 0, but not out of media, so assume is continuous stock.

										if (isTracingSize)
											Debug::trace("PrinterTeachTable", printerTraceString + " handling PrStatus - Length determined to be 0, but not out of media, so assume is continuous stock");

										usingContinuousStock = true;

										//SOF-5110 if using die cut labels then do not enable cutter.
										if( prKind == SSKPCPrinter ) {
											Debug::trace("PrinterManager.Cutter", "Enable cutter and send to session." );
											Configuration::getInstance()->setCutterEnabled(true);

											PrinterGeneric gen( PrCutterEnableSave, this, myPrinterType );											
											dispatchEvent( &gen );					
											
											isCutterEnabled = true;											
										}

										measuredSize = LengthContinuous_Lines;
										actualSize = LengthContinuous_Lines;

										if (printHeelFirst == true) 
										{
											if(isCutterInstalled && isCutterEnabled) 
											{
												if( noCutOption ) 
												{ 	//SOF-3847 3848
													Debug::trace("PrinterManager.VirtualCombine", "ContinuousLabelHeelCombined noCutOption");
													printerMode =  ContinuousLabelHeelCombined;													
													takeLabelTime = 0;														
												} else {
													if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
													    printerMode =  ContinuousLabelHeelCut;
													} else {
														printerMode =  ContinuousShootLabelHeelCut;
													}
													takeLabelTime = ticksPerSecond / 2;
												}
											} else {
												printerMode =  ContinuousLabelHeel;
											}
										} else {
											if(isCutterInstalled && isCutterEnabled) {
												if( noCutOption ) { 	//SOF-3847 3848
													Debug::trace("PrinterManager.VirtualCombine", "ContinuousLabelHead noCutOption");
													printerMode =  ContinuousLabelHeadCombined;													
													takeLabelTime = 0;														
												} else {
													printerMode =  ContinuousLabelHeadCut;
													takeLabelTime = ticksPerSecond / 2;
												}
											} else {
												printerMode =  ContinuousLabelHead;
											}
										}

										if (pMyPrinter != NULL)
										{
											Debug::trace("PrinterManager.PrinterNomSize", printerTraceString + " handling printerStatus.isSizeCmdComplete() - continuous - actualSize=" + inttostring(actualSize) + "   measuredSize=" + inttostring(measuredSize));	
											PrinterNomSize prNomSize(this, myPrinterType, actualSize, measuredSize);

											const char *pCC = prNomSize.serialize();
											pMyPrinter->send(pCC, prNomSize.getSerializedLength());

											printerStockMetrics.setContinuousStock();
										}

										teachPrinter(printerMode);
									}
									else
									{
										if (isTracingSize)
											Debug::trace("PrinterTeachTable", printerTraceString + " handling PrStatus - calling setLabelSizingFailed()");
										printerStatus.setLabelSizingFailed();
									}
									
								}

								//create a label stock descriptor
								stockLoaded.setLength(actualSize);
								stockLoaded.setWidth(labelWidth);
								stockLoaded.setSkew(0);

								PrTestLabelType saveTestLabelType = testLabelType;
								testLabelType = PrNoTestLabel;
								createTestLabel(saveTestLabelType);

								// Only issue the Find command if we are using die-cut labels, not continuous stock,
								// since the printer isn't going to find the sync bar on continuous stock.
								// This is what was wasting the 2nd 10" of continuous stock when sizing!
								if (!usingContinuousStock)
								{
									if (takeLabelTime == 0)
									{
										queueCommand(FindwoLabelTakenCommand, PrNoSecurityLabel, PrNoIndirection, 0x0);
										if (isTracing)
											Debug::trace("PrinterManager.PrinterStatus", printerTraceString + " sent FindwoLabelTakenCommand Command!");
									}
									else
									{
										queueCommand(FindCommand, PrNoSecurityLabel, PrNoIndirection, 0x0);
										if (isTracing)
											Debug::trace("PrinterManager.PrinterStatus", printerTraceString + " sent Find Command!");
									}
								}
								labelPresent = false;
								Debug::trace("PrinterManager.LabelMissing", printerTraceString + " labelPresent");
							}

							if (printerStatus.isLabelTaken() == false && takeLabelTime != 0)
							{
								Debug::trace("PrinterManager.TakeLabelTime","isCombinedVirtualPrint():" + Integer::toString(isCombinedVirtualPrint())); 
								Debug::trace("PrinterManager.TakeLabelTime","usingContinuousStock:" + Integer::toString(usingContinuousStock)); 
								if ( (	!isCombinedVirtualPrint() ) && ( !usingContinuousStock ) ) {
									printerStatus.setTakeLabelError();
									Debug::trace("PrinterManager.TakeLabelTime", "createTimer: 1");
									createTimer(takeLabelTime, takeLabelTimerExpired, true);
								} 
							}

							// Finish the sizing algorithm, and set up for printing again, if we have continuous stock,
							// or when the Find command (that we issued directly above) finishes for die-cut labels
							if ( (printerStatus.isSizeCmdComplete() && usingContinuousStock) ||
							     printerStatus.isFindCmdComplete() ||
							     printerStatus.isFindwoLabelTakenCmdComplete() )
							{
								executingCommand = false;
								Debug::trace("PrinterManager.CMD","(9)executingCommand: " + Integer::toString(executingCommand));

								printerState = PrinterReadyToPreImage;
								Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterReadyToPreImage 2");

								LabelWidth printerStatusLabelWidth = ( !getLabelWidthFromStatus ) ? labelWidth : ( ( printerStatus.isNarrowLabel() ) ? NarrowLabel : WideLabel );
								printerConfig.setLabelWidth(printerStatusLabelWidth);
								checkLabelWidth(printerStatusLabelWidth);

								if ( printerStatus.isSizeCmdComplete() && pConfig->toBoolCfg(fieldOffsetCfgEnableRfidPrinting) )
								{
									//Debug::trace("PrinterManager.rfid", printerTraceString + " printerStatus.isSizeCmdComplete() - setting needToReimageRFIDVoidLabelType: " + booltostring(needToReimageRFIDVoidLabelType));
									needToReimageRFIDVoidLabelType = true;
								}

								if (newProductSmartPtr.get())
								{
									currentProductSmartPtr = newProductSmartPtr;
									pCurrentProduct = currentProductSmartPtr.get();
									newProductSmartPtr = Util::CSmartPtr<Product>();

									Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));
									if (labelWidth != UnknownLabel)
									{
										preImageLabel();
									}
								}

								// Don't just send these next commands if we have continuous stock, send for die cut too.
								// We noticed that sometimes after sizing, the printer's stepper motor would be engaged, making a high pitched sound.
								// These commands got rid of it.
								//if (printerStockMetrics.isContinuousStock())
								//{
									sendPrinterFlush();
									sendPrinterReset(0);
									emptyCommandQ();
									sendPrinterEnableTakeup();
								//}
							}

							if( isCutterInstalled && isCutterEnabled && printerState == PrinterReadyToPreImage ) {
								if(usingContinuousStock) {
									//continuous stock so we dont really care where we cut so cut now and retract
									Debug::trace("PrinterManager.Cutter", "Sending Cut Retract");
									// SOF - 5820 - ApcPrinter added to this conditional because cutter wasn't firing with sizing operations ARA cutter units
									if( prKind == KPCPrinter || prKind == ApcPrinter ) { 
										queueCommand(CutRetractCommand, PrNoSecurityLabel, PrRam2,-270);
										/* Advance the label stock to reseat the printhead on the stops */
										queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, 75); //50
									} else if( prKind == SSKPCPrinter ) {
										queueCommand(CutRetractCommand, PrNoSecurityLabel, PrRam2,-100); //-75//-25 == 1/8" Scott freestanding scale test
									}
								} else {
									//die-cut stock so we need to eject to be in between the labels and then cut
									queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, 215);//203 steps per inch
									if( prKind == KPCPrinter ) {
									    queueCommand(CutRetractCommand, PrNoSecurityLabel, PrRam2, -225);
									    queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, 30);
									} else {
										queueCommand(CutRetractCommand, PrNoSecurityLabel, PrRam2, -100);
									}									
								}
							}
						}
					}

					if (printerStatus.isLabelTaken() == true || printerStatus.isHeadDisengaged() == true)
					{
						if( !noCutOption ) {
							printerStatus.clearTakeLabelError();
							cancelTimer(takeLabelTimerExpired); 
							Debug::trace("PrinterManager.TakeLabelTime", "cancelTimer() 1");
						} else {
							//SOF-3980 This fixes when a db is loaded with the noCutOption enabled on a non cutter scale!
							if( !isCutterInstalled ) {
								if( !isDieCutloaded() ) {
									printerStatus.clearTakeLabelError();
									cancelTimer(takeLabelTimerExpired); 
									Debug::trace("PrinterManager.TakeLabelTime", "cancelTimer() 1a");
								}
							}
							//SOF-3998
							if( ( ( KPCPrinter == prKind ) || ( prKind == SSKPCPrinter ) ) && 
								( isDieCutloaded() ) && ( printerState == PrinterReadyToPrint) ) {
								printerStatus.clearTakeLabelError();
								cancelTimer(takeLabelTimerExpired); 
								Debug::trace("PrinterManager.TakeLabelTime", "cancelTimer() 1b");
							}

							if( printerState == PrinterTryingToSize ) {
								printerStatus.clearTakeLabelError();
								cancelTimer(takeLabelTimerExpired); 
								Debug::trace("PrinterManager.TakeLabelTime", "cancelTimer() 2");
							} else if( ( printerState == PrinterReadyToPrint ) || ( printerState == PrinterXferringLabel ) ) {
								if( printerStatus.isVirtualLabelComplete() ) {
									printerStatus.clearTakeLabelError();
									cancelTimer(takeLabelTimerExpired); 
									Debug::trace("PrinterManager.TakeLabelTime", "cancelTimer() 3");
								}
							}			
						}
					}

					if (printerStatus.isOutOfMedia() && printerStatus.isMediaJammed())
					{
						//	PTR 1574:	When we are out of labels the media jammed flag is set also, reporting Jammed first, then Out of labels.
						//				Just report that we are Out of labels.
						printerStatus.clearMediaJammed();
					}

					if( printerStatus.isOutOfStock() ) {
						Debug::trace("PrinterManager.Error", "printerStatus.isOutOfStock(): is set!");
					}
					
					dispatchEvent(&printerStatus);
					lastStatusMsg = printerStatus;

					if (isTracing)
					{
						Debug::trace("PrinterManager.PrinterStatus", printerTraceString + "PrStatus - handling finished!  *****************************************");
						printerStatus.show();
						Debug::trace("PrinterManager.PrinterStatus", printerTraceString + "**************************************************************************");
						//PAL_absclass::PAL_sleep( 500 );
					}
				}
				break;

				case PrReset:
				case PrResetImmediate:
				{
					Debug::trace("PrinterManager.Message", printerTraceString + ((prEvent == PrReset) ? " PrReset" : "PrResetImmediate"));
					if (printerState != PrinterTryingToSize)
					{
						if (prEvent == PrReset && ((printerState == PrinterXferringLabel) || (printerState == PrinterTakingLabel) || (printerState == PrinterLabelIsReady)))
						{
							resetPrinter = true;
							Debug::trace("PrinterManager.PrinterReset", printerTraceString + " resetPrinter: " + HobString(Bool::toString(resetPrinter)));
						}
						else
						{
							Debug::trace("PrinterManager.PrinterReset", printerTraceString + " resetPrinter3");
							sendPrinterReset(3);
							emptyProductTransactionQ(false);
							emptyCommandQ();

							pCurrentProduct = NULL;
							pCurrentLabel = NULL;
							Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));
							currentProductSmartPtr = Util::CSmartPtr<Product>();
							newProductSmartPtr = Util::CSmartPtr<Product>();

							// FT-882 do not dispatch label complete message as pCurrentProductTransaction is most likely bad
							//dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, false);
							pCurrentProductTransaction = NULL;
							currentPackId = 0;

							if (printerState != PrinterTryingToSize)
							{
								printerState = PrinterReadyToPrint;
								Debug::trace("PrinterManager.StateMachine", printerTraceString + " 4) PrinterReadyToPrint takeLabelTime: " + Integer::toString(takeLabelTime) );								
							}
						}
					}
				}
				break;
				case PrSizing:
				{
					Debug::trace("PrinterManager.Message", printerTraceString + " PrSizing");
					PrinterSizing prSizing(recvBuffer);

					if (Debug::isTracing("PrinterTeachTable") == true)
					{
						prSizing.show();
					}

					enableSizing = prSizing.isSizing();
				}
				break;
				case PrNomSize:
				{
					Debug::trace("PrinterManager.Message", printerTraceString + " PrNomSize");

					// Constuct the Evol Message Buffer!!
					PrinterNomSize printerSize(this, myPrinterType, actualSize, measuredSize);
					dispatchEvent(&printerSize);
					break;
				}
				break;
				case PrReqCalibrate:
				{
					Debug::trace("PrinterManager.Message", printerTraceString + " PrReqCalibrate");

					PrinterRequestCalibrate prReqCalib(recvBuffer);

					if (Debug::isTracing("PrinterManager.PrinterCalibration") == true)
					{
						prReqCalib.show();
					}

					if (printerState == PrinterTryingToSize)
					{
						Debug::trace("PrinterManager.PrinterReset", printerTraceString + " resetPrinter4");
						sendPrinterReset(4);
					}

					if ( isCounterScalePrinter() && (prReqCalib.getPrinterCalibrateType() == PrCalibrateLabelTaken) )
					{
						lastStatusMsg.setCalibrateCmdComplete();
						dispatchEvent(&lastStatusMsg);
					}
					else
					{
						queueCommand(CalibrateCommand, PrNoSecurityLabel, PrNoIndirection, (UINT16) prReqCalib.getPrinterCalibrateType());
					}
				}
				break;
				case PrReqInquire:
				{
					Debug::trace("PrinterManager.PrReqInquire", "PrReqInquire");
					if (pMyPrinter != NULL)
					{
						requestedPrinterInquire = true;
						pMyPrinter->send((const char*) recvBuffer, bufSize);
					}
				}
				break;
				case PrEnable:
				case PrDisable:
				case PrFlush:
				case PrReqWakeup:
				case PrReqStatus:
				case PrMode:
				case PrCommand:
				case PrRam:
				case PrMask:
				{
					if (pMyPrinter != NULL)
					{
						pMyPrinter->send((const char*) recvBuffer, bufSize);
					}
				}
				break;
				case PrTest:
				{
					Debug::trace("PrinterManager.Message", "PrTest");
					PrinterTest	test(recvBuffer);
					if (pMyPrinter != NULL)
					{
						pMyPrinter->send((const char*) recvBuffer, bufSize);
					}
				}
				break;
				case PrReqPrintheadPower:
				{
					Debug::trace("PrinterManager.Message", printerTraceString + " PrReqPrintheadPower");
					if (pMyPrinter != NULL)
					{
						PrinterRequestPrintheadPower prPower(recvBuffer);
						pMyPrinter->send((const char*) recvBuffer, bufSize);

						if (prPower.isPowerState() == true)
						{
							Debug::trace("PrinterManager.PrintheadPower", printerTraceString + " enabling power timer");
							createTimer(printheadPowerTime, printheadPowerTimerExpired);
						}
						else
						{
							Debug::trace("PrinterManager.PrintheadPower", printerTraceString + " disbling power timer");
							cancelTimer( printheadPowerTimerExpired);
						}

						if (Debug::isTracing("PrinterManager.PrintheadPower") == true)
						{
							prPower.show();
						}
					}
				}
				break;
				case PrReqSensors:
				{
					Debug::trace("PrinterManager.Message", printerTraceString + " PrReqSensors");
					if (pMyPrinter != NULL)
					{
						pMyPrinter->send((const char*) recvBuffer, bufSize);

						lastSensorsMsg = PrinterSensors();
					}
				}
				break;
				case PrReqStockMetrics:
				{
					Debug::trace("PrinterManager.Message", printerTraceString + " PrReqStockMetrics");
					if (Debug::isTracing("PrinterManager.PrinterStockMetrics") == true)
					{
						printerStockMetrics.show();
					}

					printerStockMetrics.show();
					dispatchEvent(&printerStockMetrics);
					Debug::trace("PrinterTeachTable", printerTraceString + " Dispatched printerStockMetrics -- PrReqStockMetrics");
				}
				break;
				case PrSensorsTracking:
				{
					Debug::trace("PrinterManager.Message", printerTraceString + " handlePrinterMessage() PrSensorsTracking");
					PrinterSensorsTracking prTracking(recvBuffer);

					trackSensors = prTracking.getSensorsTracking();

					if (trackSensors > 0)
						Debug::trace("PrinterManager.PrinterSensors", printerTraceString + " handlePrinterMessage() trackSensors: " + HobString(Bool::toString(true)));
					else
						Debug::trace("PrinterManager.PrinterSensors", printerTraceString + " handlePrinterMessage() trackSensors: " + HobString(Bool::toString(false)));

					if (trackSensors > 0)
					{
						lastSensorsMsg = PrinterSensors();
						Debug::trace("PrinterManager.PrinterSensors", printerTraceString + "  handlePrinterMessage() start timer val: " + HobString(Integer::toString(requestSensorTime)));
						createTimer(requestSensorTime, sensorTimerExpired, CHobartTimer::PERIODIC_TIMER);
					}
					else
					{
						cancelTimer( sensorTimerExpired );
					}
				}
				break;
				case PrSensors:
				{
					//Debug::trace("PrinterManager.Message", printerTraceString + " PrSensors");
					PrinterSensors printerSensors(recvBuffer);
					const char * m = printerSensors.serialize();
					Debug::trace("PrinterManager.PrinterSensors", printerTraceString + " PrSensors: " + Hex::toString(m, printerSensors.getSerializedLength()));

					if (isCounterScalePrinter())
					{
						// EvolPrinters do not return this!
						Debug::trace("PrinterManager.PrinterSensors", printerTraceString + " PrSensors - isCounterScalePrinter(), labelWidth in msg is: " + inttostring((int)printerSensors.getLabelWidth()) + "  labelWidth currently is: " + inttostring((int)labelWidth));
						if (labelWidth == UnknownLabel)
							labelWidth = printerSensors.getLabelWidth();
						Debug::trace("PrinterManager.PrinterSensors", printerTraceString + " PrSensors - var labelWidth is now: " + inttostring((int)labelWidth));
					}
					else if ( isWrapperPrinter() )	// EPP Printers don't have label width in the Sensors msg
					{
						// EvolPrinters do not return this!
						Debug::trace("PrinterManager.PrinterSensors", printerTraceString + " PrSensors - isWrapperPrinter() so set labelWidth in msg to var labelWidth: " + inttostring((int)labelWidth));
						printerSensors.setLabelWidth(labelWidth);
					}


					if (printerSensors.equals(lastSensorsMsg) == false)
					{
						printerSensors.setSourceObject(this);
						printerSensors.setPrinterType(myPrinterType);

						if (Debug::isTracing("PrinterManager.PrinterSensors") == true)
						{
							printerSensors.show();
						}

						lastSensorsMsg = printerSensors;
						dispatchEvent(&printerSensors);
					}

					receivedPrinterFeedback();
				}
				break;
				case PrTestLabel:
				{					
					//SOF-3933 if we are in production entry mode and the disable cutting between labels is enabled then switch teach tables.
					if( ( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) || ( prKind == SSKPCPrinter ) ) &&
                        ( isCutterInstalled ) && ( isCutterEnabled ) && ( noCutOption )) {
						OpModeEvent mEvent = RealTimeController::getInstance()->getCurrentOpModeEvent();
						if( ( mEvent == OpModeProductionEntry ) || ( mEvent == OpModeService ) || ( mEvent == OpModeSupervisor ) ||
						    ( mEvent == OpModeManualWeigh ) || ( mEvent == OpModeAutoManualApply ) || 
							( mEvent == OpModeAccumulateItems ) || ( mEvent == OpModeBuildAMeal ) ) {
							if (printHeelFirst == true) {
								if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
								    printerMode =  ContinuousLabelHeelCut;
								} else {
									printerMode =  ContinuousShootLabelHeelCut;
								}
								takeLabelTime = ticksPerSecond / 2;					
							} else {
								printerMode =  ContinuousLabelHeadCut;
								takeLabelTime = ticksPerSecond / 2;					
							}
							teachPrinter( printerMode );
							if( ( mEvent == OpModeProductionEntry ) || ( mEvent == OpModeManualWeigh ) || 
							    ( mEvent == OpModeAutoManualApply ) || ( mEvent == OpModeAccumulateItems ) ||
								( mEvent == OpModeBuildAMeal ) ) {
								reTeach = true;
							}
						}						
					}
										
					bool isTracing = Debug::isTracing("PrinterManager.TestLabel");
					if (isTracing)
						Debug::trace("PrinterManager.TestLabel", printerTraceString + " handlePrinterMessage() case PrTestLabel  printerState:" + inttostring(printerState));
					PrinterPrintTestLabel testLabel(recvBuffer);
					PrTestLabelType labelType = testLabel.getPrTestLabelType();
					PrTestLabelQuantity labelQuantity = testLabel.getTestLabelQuantity();
					applySecurityLabel = testLabel.getPrTestCommandOptions();//Alex Zavala 3/20/14
					bool canPrint = (printerCalibrating == false) &&
						(printerResetting == false) &&
						((printerState == PrinterReadyToPrint) ||
						 (printerState == PrinterReadyToPreImage) ||
						 (printerState == PrinterPreImaging) ||
						 (printerState == PrinterReadyToXfer ) );
					if (isTracing)
					{
						Debug::trace("PrinterManager.TestLabel", printerTraceString + " handlePrinterMessage() case PrTestLabel canPrint - printerCalibrating: " + ((printerCalibrating == false) ? "false" : "true"));
						Debug::trace("PrinterManager.TestLabel", printerTraceString + " handlePrinterMessage() case PrTestLabel canPrint - printerResetting: " + ((printerResetting == false) ? "false" : "true"));
						Debug::trace("PrinterManager.TestLabel", printerTraceString + " handlePrinterMessage() case PrTestLabel canPrint - printerState = PrinterReadyToPrint: " + ((printerState == PrinterReadyToPrint) ? "true" : "false"));
						Debug::trace("PrinterManager.TestLabel", printerTraceString + " handlePrinterMessage() case PrTestLabel canPrint - printerState = PrinterReadyToPreImage: " + ((printerState == PrinterReadyToPreImage) ? "true" : "false"));
						Debug::trace("PrinterManager.TestLabel", printerTraceString + " handlePrinterMessage() case PrTestLabel canPrint - printerState = PrinterPreImaging: " + ((printerState == PrinterPreImaging) ? "true" : "false"));

						testLabel.show();
					}

					if (labelType == PrVoidLabel)
					{
						saveTestLabelType = testLabelType;
						testLabelType = PrVoidLabel;

						Debug::trace("PrinterManager.TestLabel", printerTraceString + " handlePrinterMessage() case PrTestLabel SET sendTestLabel = true " );
						
                        sendTestLabel = true;
                        printerState = PrinterLabelIsReady;
						Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterLabelIsReady");
					}
					else
					{
						switch (labelQuantity)
						{
							Debug::trace("PrinterManager.TestLabel", " handlePrinterMessage() switch (labelQuantity)");
							case PrOneTestLabel:
								Debug::trace("PrinterManager.TestLabel", " handlePrinterMessage() switch (labelQuantity) case PrOneTestLabel");
								if (canPrint == true)
								{
									Debug::trace("PrinterManager.TestLabel", " handlePrinterMessage() case PrOneTestLabel if (canPrint == true)");
									Debug::trace("PrinterManager.TestLabel", " handlePrinterMessage() BEFORE call createTestLabel()");
									if (createTestLabel(labelType) == true)
									{
										Debug::trace("PrinterManager.TestLabel", " handlePrinterMessage() AFTER call createTestLabel() SUCCESS");
										Debug::trace("PrinterManager.TestLabel", " handlePrinterMessage() case PrOneTestLabel SET sendTestLabel = true ");
										sendTestLabel = true;
										printerState = PrinterLabelIsReady;
										Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterLabelIsReady");
									}
									else
										Debug::trace("PrinterManager.TestLabel", " handlePrinterMessage() AFTER call createTestLabel() FAILED");

									Debug::trace("PrinterManager.TestLabel", " handlePrinterMessage() NOW what do we do?  Where is the call to print?");
								}
								break;
							case PrStartContinuous:
								if ((canPrint == true) && (continuousTestLabels == false))
								{
									if (createTestLabel(labelType) == true)
									{
										continuousTestLabels = true;

										if (printerMode == ContinuousLabelHeel || printerMode == ContinuousLabelHead || 
										    printerMode == ContinuousLabelHeelCut || printerMode == ContinuousLabelHeadCut ||
											printerMode == ContinuousShootLabelHeelCut )
										{
											if(isCutterInstalled && isCutterEnabled){
												if(printHeelFirst == true)
													teachPrinter(ContinuousStreamHeelCut, 0, OutOfDataBuffersBit, TakingBit);
												else
													teachPrinter(ContinuousStreamHeadCut, 0, OutOfDataBuffersBit, TakingBit);
											}else{
												if (printHeelFirst == true)
													teachPrinter(ContinuousStreamHeel, 0, OutOfDataBuffersBit, TakingBit);
												else
													teachPrinter(ContinuousStreamHead, 0, OutOfDataBuffersBit, TakingBit);
											}
										}
										else if (printerMode != DieCutStreamHeel && printerMode != DieCutStreamHead)
										{
											if( prKind != SSKPCPrinter ) {
											    if (printHeelFirst == true)
												    teachPrinter(DieCutStreamHeel, 0, OutOfDataBuffersBit, TakingBit);
											    else
												    teachPrinter(DieCutStreamHead, 0, OutOfDataBuffersBit, TakingBit);
											} else {
												if (printHeelFirst == true)
													teachPrinter(DieCutShootStreamHeel, 0, OutOfDataBuffersBit, TakingBit);
												else
													teachPrinter(DieCutShootStreamHead, 0, OutOfDataBuffersBit, TakingBit);
											}
										}

										sendTestLabel = true;
										printerState = PrinterLabelIsReady;
										Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterLabelIsReady");
									}
								}
								break;
							case PrStopContinuous:
								if (continuousTestLabels == true)
								{
									Debug::trace("PrinterManager.StateMachine", "PrStopContinuous!! printerState:" + Integer::toString( printerState ) );								
									//SOF-3861
									if( ( KPCPrinter == prKind ) || ( prKind == SSKPCPrinter  ) ) {
										printerState = PrinterPriorityErrorStop;
									} else {
										printerState = PrinterPriorityError;
									}
									sendTestLabel = true;
									continuousTestLabels = false;

									//printerState = PrinterPriorityError;
									//cancelTimer( continuousLabelTimerExpired );
									//teachPrinter(printerMode);
								}
								break;
						}
					}
				}
				break;
				case PrReqLabelWidth:
				{
					Debug::trace("PrinterManager.LabelWidth", printerTraceString + " PrReqLabelWidth");
					PrinterLabelWidth prLabelWidth(this, myPrinterType, labelWidth);

					dispatchEvent(&prLabelWidth);
				}
				break;
				case PrLabelWidth:
				{
					Debug::trace("PrinterManager.LabelWidth", printerTraceString + " PrLabelWidth");
					if (getLabelWidthFromStatus == false)
					{
						PrinterLabelWidth prLabelWidth(recvBuffer);

						LabelWidth newLabelWidth = prLabelWidth.getLabelWidth();

						printerConfig.setLabelWidth(newLabelWidth);
						Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " Calling sendPrinterConfig() when handling PrLabelWidth, configtype = PrPermanent");
						sendPrinterConfig(PrPermanent);

						checkLabelWidth(newLabelWidth);
					}
				}
				break;
				case PrNewOrder:
				{
					Debug::trace("PrinterManager.PrNewPrinterType", printerTraceString + " PrNewPrinterType");
					PrinterNewPrinterType newPrinterType(recvBuffer);

					if (Debug::isTracing("PrinterManager.PrinterTypeShow") == true)
					{
						newPrinterType.show();
					}
					Debug::trace("PrinterManager.PrNewPrinterType", printerTraceString + " PrNewPrinterType");

					PrinterType prType = newPrinterType.getNewPrinterType();
					// update peripheral configuration

					Debug::trace("PrinterManager.PrNewPrinterType", printerTraceString + "  myPrinterType:" + inttostring((int)myPrinterType) + "  newtype:" + inttostring((int)prType));
					Debug::trace("PrinterManager.PrNewPrinterType", printerTraceString + "  physicalAddress:" + inttostring((int)physicalAddress) + "  newphysicalAddress:" + inttostring((int)newPrinterType.getPhysicalAddress()));
					USBD_NODE_ID newusbid= newPrinterType.getUsbNodeId();
					string newusb= ulongtostring(newusbid);
					string oldusb= ulongtostring(usbNodeId);
					Debug::trace("PrinterManager.PrNewPrinterType", printerTraceString + "  usbNodeID:" + oldusb + "  newusbNodeID:" + newusb);

					if ((prType != myPrinterType) && (newusbid == usbNodeId) /*(newPrinterType.getPhysicalAddress() == physicalAddress)*/ )

					//if ((prType != myPrinterType) && (newPrinterType.getPhysicalAddress() == physicalAddress))
					{
#ifndef CHECK_FT
#if 0
						if (pMyPrinter != NULL)
						{
							pMyPrinter->send((const char*) recvBuffer, bufSize);
						}
#endif
#endif

						Debug::trace("PrinterManager.PrinterType", printerTraceString + " changing to PrinterType: " + HobString(Integer::toString((UINT16) prType)) + "  -  Node Id " + HobString(Hex::toString((long long) newPrinterType.getUsbNodeId())));
						myPrinterType = prType;

						printerConfig.setPrinterType(prType);
						Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " Calling sendPrinterConfig() when handling PrNewPrinterType, configtype = PrPermanent");
						sendPrinterConfig(PrPermanent);

						printerStockMetrics.setPrinterType(prType);

						printerStockMetrics.show();
						dispatchEvent(&printerStockMetrics);
						Debug::trace("PrinterTeachTable", printerTraceString + " Dispatched printerStockMetrics -- PrNewPrinterType");
					}

					sendPrinterType();
					sendPrinterKind();
				}
				break;
				case PrNewAddress:
				{
					Debug::trace("PrinterManager.LabelWidth", printerTraceString + " PrNewAddress");
					PrinterNewAddress newAddress(recvBuffer);

					if (Debug::isTracing("PrinterManager.PrinterType") == true)
					{
						newAddress.show();
					}

					if (physicalAddress != newAddress.getPhysicalAddress())
					{
						physicalAddress = newAddress.getPhysicalAddress();
						printerSysConfig.setStationAddress(physicalAddress);
						sendPrinterSystemConfig(SysPermanent);
					}

					sendPrinterType();
				}
				break;
				case PrTakeLabelTime:
                {
					PrinterTakeLabelTime prTakeLabelTime(recvBuffer);
					Debug::trace("PrinterManager.TakeLabelTimeJoe", "START CASE PrTakeLabelTime: ");
					Debug::trace("PrinterManager.TakeLabelTimeJoe", "pConfig->isSelfServiceScale(): " + Integer::toString(pConfig->isSelfServiceScale()));
					Debug::trace("PrinterManager.TakeLabelTimeJoe", "Time: " + Integer::toString((prTakeLabelTime.getTakeLabelTime() * ticksPerSecond)));
					Debug::trace("PrinterManager.TakeLabelTimeJoe", " isCutterInstalled: " + Integer::toString(isCutterInstalled));
					Debug::trace("PrinterManager.TakeLabelTimeJoe", " isDieCutloaded: " + Integer::toString(isDieCutloaded()));

					// Self service scale is to ignore disable of take label time. -- Jim H.
					// All other scales should use unless they have a cutter attached or using continuous stock and in PE mode. -- Jim H. 
					if (pConfig->isSelfServiceScale() == false) 
					{
						//SOF-3949 //SOF-5482
						Debug::trace("PrinterManager.TakeLabelTimeJoe", "isSelfService IS FALSE (should see)");
						if ((OpModeProductionEntry == RealTimeController::getInstance()->getCurrentOpModeEvent() && (!isCutterInstalled) && (isDieCutloaded())))
						{
							Debug::trace("PrinterManager.TakeLabelTimeJoe", "OpModeProductionEntry IS TRUE (better not see)");
							takeLabelTime = prTakeLabelTime.getTakeLabelTime() * ticksPerSecond;

							//ignoring label taken sensor this is only done on continuous stock.
							if (takeLabelTime == 0) {
								Debug::trace("PrinterManager.TakeLabelTimeJoe", "ProdEntry IS TRUE AND now we canceled the timer");

								cancelTimer(takeLabelTimerExpired);
								sendPrinterEnableTakeup();
							}
							else
								Debug::trace("PrinterManager.TakeLabelTimeJoe", "isSelfService IS FALSE AND ProdEntry IS TRUE ELSE did nothing (BETTER not see this)");
						}
						else
						{
							Debug::trace("PrinterManager.TakeLabelTimeJoe", "isSelfService IS FALSE and we ELSE did nothing");
							FpmManager* pSerialFlash(FpmManager::getInstance());
							const std::string style(pSerialFlash->getMFGSystemStyle());
							if( (style == "NGW1") || (style == "NGW2") )
							{
								Debug::trace("PrinterManager.TakeLabelTimeJoe", "isSelfService IS FALSE AND for a wrapper we still need to set the takeLabelTime");
								takeLabelTime = prTakeLabelTime.getTakeLabelTime() * ticksPerSecond;
							}
						}
					}
					else {
						// SelfService scales will never run Production Entry but we might want to all for engineering tests
						Debug::trace("PrinterManager.TakeLabelTimeJoe", "isSelfService IS TRUE which is should never be on a WRAPPER. I better not see this");

						if ((OpModeProductionEntry == RealTimeController::getInstance()->getCurrentOpModeEvent() &&
							(!isCutterInstalled) && (isDieCutloaded()) && (mfgKey_))) {
							takeLabelTime = prTakeLabelTime.getTakeLabelTime() * ticksPerSecond;

							//ignoring label taken sensor this is only done on continuous stock.
							if (takeLabelTime == 0) {
								cancelTimer(takeLabelTimerExpired);
								sendPrinterEnableTakeup();
							}
						}
					}
				}
				break;

				// PJR-383
				case PrPreImageBarcode:
				{
					bool isTracing = Debug::isTracing("SelfServeBarcodeLabel");
					if (isTracing)
						Debug::trace("SelfServeBarcodeLabel", printerTraceString + " PrPreImageBarcode");

					PrinterPreImage preImage(recvBuffer);

					pCurrentProduct = NULL;
					currentProductSmartPtr = Util::CSmartPtr<Product>();
					labelWidth = WideLabel;
					PrinterType myPrinter = ExtraPrinter_UsedForSelfServeScreenBarcodeLabel;

					Util::CSmartPtr<Product> *pCurrentProductSmartPtr = (Util::CSmartPtr<Product> *) preImage.getProductSmartPtr();
					if (pCurrentProductSmartPtr)
					{
						currentProductSmartPtr = *pCurrentProductSmartPtr;
						pCurrentProduct = currentProductSmartPtr.get();

						Debug::trace("SelfServeBarcodeLabel", " Calling BitMap::freeBitMap()");
						deleteBitMap( bitMap[myPrinter] );
						bitMap[myPrinter] = new BitMap;

						if ( imageControlBlock[myPrinter] )
						{
							Debug::trace("SelfServeBarcodeLabel", printerTraceString + " Going to delete imageControlBlock " + ptrtostring(imageControlBlock[myPrinter]));

							deleteImageControl( imageControlBlock[myPrinter] );
							imageControlBlock[myPrinter] = 0;
						}

						pCurrentLabel = pCurrentProduct->getLabelType((short)myPrinter);

						if (pCurrentLabel &&
							pCurrentLabel->getErrorCode() == RECORD_OK //  &&
							//  (!pCurrentProduct->isPrintLabelToPrinter() ||
							//  (pCurrentLabel->getLabelStockDescriptor() && pCurrentLabel->getLabelStockDescriptor()->getWidthDots() <= stockLoaded.getWidthDots()))
						   )
						{
							if (isTracing)
								Debug::trace("SelfServeBarcodeLabel", printerTraceString + " before calling preImageALabel - pCurrentProduct: " + HobString(Hex::toString((long long)pCurrentProduct)));
							preImageALabel(pCurrentLabel, myPrinter);
						}

						if (isTracing)
							Debug::trace("SelfServeBarcodeLabel", printerTraceString + " end of PrPreImageBarcode - pCurrentProduct: " + HobString(Hex::toString((long long)pCurrentProduct)));
						//PAL_absclass::PAL_sleep(500);
					}
					break;
				}
				// PJR-383
				case PrPostImageBarcode:
				{
					bool isTracing = Debug::isTracing("SelfServeBarcodeLabel");
					if (isTracing)
						Debug::trace("SelfServeBarcodeLabel", printerTraceString + " PrPostImageBarcode");

					// Store ProductTransaction into productTA Q!!
					PrinterPostImage postImage(recvBuffer);

					// PJR-383
					pCurrentProductTransaction = const_cast<ProductTransaction *>(postImage.getProductTransaction());
					if (pCurrentProductTransaction)
					{
						PrinterType myPrinterType = ExtraPrinter_UsedForSelfServeScreenBarcodeLabel;

						if ( imageControlBlock[myPrinterType] && imageControlBlock[myPrinterType]->getImageFlags() != PostImage )
						{
							if (isTracing)
								Debug::trace("SelfServeBarcodeLabel", printerTraceString + " before calling updateFormPostImageFields");
							imageControlBlock[myPrinterType]->setImageFlags(PostImage);
							updateFormPostImageFields(*imageControlBlock[myPrinterType]);

							if (isTracing)
								Debug::trace("SelfServeBarcodeLabel", printerTraceString + " PrPostImageBarcode:  before sending Render commands");
							
							//std::string filenameString = Configuration::getInstance()->toStringCfgI(fieldOffsetCfgIPrintToScreenBarcodeFilename);
							if (++Configuration::_printToScreenBarcodeFilenameCtr_ > 9)
								Configuration::_printToScreenBarcodeFilenameCtr_ = 1;	// Do not rollback to 0!!
					
							std::string filenameString = Configuration::getInstance()->buildPrintToScreenBarcodeFilename(Configuration::_printToScreenBarcodeFilenameCtr_);

							if ((pConfig->toIntCfg(fieldOffsetCfgSelfServeFixedPlu) > 0) && !pjr413HasQRLabel)
								filenameString = "NO_QR";

							if (isTracing)
								Debug::trace("SelfServeBarcodeLabel", printerTraceString + " PrPostImageBarcode:  New barcode label filename: " + filenameString);

							// In this next block, I'm deleting the file with the number after the one we are going to create, just so that if anyone needed to find out
							// which file is supposed to be displayed, they would have a reference point; should be displaying the one numbered before the one that's not there.
							string filenameString2 = Configuration::getInstance()->buildPrintToScreenBarcodeFilename(Configuration::_printToScreenBarcodeFilenameCtr_ + 1);
							File barcodeFile(filenameString2);
							if (barcodeFile.exists())
							{
								barcodeFile.remove();
								if (isTracing)
									Debug::trace("SelfServeBarcodeLabel", printerTraceString + " PrPostImageBarcode:  Deleted barcode label file: " + filenameString2);
							}

							Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " PRPostImageBarcode: before setting filename for saving label to file: " + filenameString);
							imageControlBlock[myPrinterType]->setSaveToFilenameString(filenameString);
							imageControlBlock[myPrinterType]->setDigimarcEnabled(false);

							RendererSetSource rdSetSource(this, myRendererSessionId, (UINT8 *)imageControlBlock[myPrinterType]);
							const char *pCC = rdSetSource.serialize();
							int mLength = rdSetSource.getSerializedLength();
							pMyRenderer->send(pCC, mLength);

							RendererSetDestination rdSetDestination(this, myRendererSessionId, (UINT8 *)bitMap[myPrinterType]->getBitMapBase());
							pCC = rdSetDestination.serialize();
							mLength = rdSetDestination.getSerializedLength();
							pMyRenderer->send(pCC, mLength);

							RendererRequestRendering rdReqRendering(this, myRendererSessionId);
							pCC = rdReqRendering.serialize();
							mLength = rdReqRendering.getSerializedLength();
							pMyRenderer->send(pCC, mLength);
						}
					}
					
					if (isTracing)
						Debug::trace("SelfServeBarcodeLabel", printerTraceString + " end of PrPostImageBarcode - pCurrentProduct: " + HobString(Hex::toString((long long)pCurrentProduct)));
					break;
				}

				case PrPreImage:
				{
					bool isTracing = Debug::isTracing("PrinterManager.PrinterImaging");
					if (isTracing)
						Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " PrPreImage");
					emptyProductTransactionQ(false);

					PrinterPreImage preImage(recvBuffer);

					if (Debug::isTracing("PrinterManager.PrinterPreImage") == true)
					{
						preImage.show();
					}

					// If the the user is in normal mode and pop the cassette before going to self-service mode with 'print to screen' mode enabled,
					// the existing printer status would prevent the PrinterManager from rendering and displaying the label.  So reset the flags preventing
					// the pre-image so that the pre-image will occur.
					if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale() )
					{
						Debug::trace("PrinterManager.TestScreenLabel", "handlePrinterMessage(PrStatus) still do printerState = PrinterReadyToPreImage;");
						printerState = PrinterReadyToPreImage;
						labelWidth = WideLabel;
					}

					if ((printerState == PrinterReadyToPreImage) || (printerState == PrinterReadyToPrint))
					{
						pCurrentProduct = NULL;
						currentProductSmartPtr = Util::CSmartPtr<Product>();

						if (labelWidth != UnknownLabel)
						{
							if (isTracing)
								Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " Pre-image Now");

							Util::CSmartPtr<Product> *pCurrentProductSmartPtr = (Util::CSmartPtr<Product> *) preImage.getProductSmartPtr();
							if (pCurrentProductSmartPtr)
							{
								currentProductSmartPtr = *pCurrentProductSmartPtr;
								pCurrentProduct = currentProductSmartPtr.get();

								preImageLabel();
							}
						}

						if (isTracing)
							Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));
					}
					else if ((printerState == PrinterLabelIsReady) ||
							((labelWidth != UnknownLabel) && ((printerState == PrinterUnsized) || (printerState == PrinterTryingToSize))))
					{
						if (isTracing)
						{
							Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " Pre-image Later printerState: " + HobString(Integer::toString(printerState)));
							Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " Pre-image Later labelWidth: " + HobString(Integer::toString(labelWidth)));
							Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " Pre-image Later");
						}

						Util::CSmartPtr<Product> *pCurrentProductSmartPtr = (Util::CSmartPtr<Product> *) preImage.getProductSmartPtr();
						if (pCurrentProductSmartPtr)
						{
							newProductSmartPtr = *pCurrentProductSmartPtr;
							if (isTracing)
								Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pNewProduct: " + HobString(Hex::toString((long long) newProductSmartPtr.get())));
						}
					}
					else
					{
						if (isTracing)
							Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " Can't pre-image now! printerState: " + HobString(Integer::toString(printerState)));
					}
				}
				break;
				case PrPostImage:
				{
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " PrPostImage");

					// Store ProductTransaction into productTA Q!!
					PrinterPostImage postImage(recvBuffer);

					const ProductTransaction *pPT = postImage.getProductTransaction();
					UINT8 packId = postImage.getPackId();
					Util::CSmartPtr<Product> *pProductSmartPtr = (Util::CSmartPtr<Product> *) postImage.getProductSmartPtr();

					if (printingVoidLabel == true)
					{
						Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " printingVoidLabel");
						dispatchLabelComplete((ProductTransaction *) pPT, currentTransNum, false);
						//SOF-5075 
						pCurrentProductTransaction = NULL;
						currentTransNum = 0;
					}
					else
					{
						/*
						   if (pCurrentProduct == NULL)
						   {
						   Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " Discarding Product Transaction");
						   dispatchLabelComplete((ProductTransaction *) pPT, false);
						   }
						   else
						   */
						{
							// Store ProductTransaction into productTA Q!!
							Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " Store ProductTransaction into  Q");
							pushProductTransactionQ(const_cast<ProductTransaction *>(pPT), packId, *pProductSmartPtr);
						}
					}
				}
				break;
				case PrLabelTransferReady:
				{
					Debug::trace("PrinterManager.LabelTransfer", "Sending Label Data");
					Debug::trace("PrinterManager.PrLabelSensorLabel", " handlePrintMessage() case PrLabelTransferReady CALL sendLabelData()");
					sendLabelData();
					break;
				}
				case PrLabelTransferComplete:
				{
					Debug::trace("PrinterManager.PrLabelSensorLabel", " handlePrintMessage() case PrLabelTransferComplete START");

					pendingLabelXfer = false;

					Debug::trace("PrinterManager.LabelTransfer", printerTraceString + " PrLabelTransferComplete");
					Debug::trace("PrinterManager.LabelTransfer", printerTraceString + " outOfDataBuffers: " + HobString(Bool::toString(outOfDataBuffers)));
					Debug::trace("PrinterManager.StateMachine", printerTraceString + " Printer current State: " + Integer::toString(printerState));
					if (printerState == PrinterReadyToXfer )
					{
						//queueCommand(PrintCommand, PrNoSecurityLabel, PrRam0, (UINT16)linesToPrint);// linesToPrint);
						if(pCurrentProductTransaction != NULL)
						{
							if(pCurrentProductTransaction->printSecurityLabel())
								applySecurityLabel = PrApplySecurityLabel;
						}
						//SOF-4056 how could we be executing a command when we are here!
						if( ( ( KPCPrinter == prKind ) || ( prKind == ApcPrinter ) || ( prKind == SSKPCPrinter ) ) && ( executingCommand ) ) {
							Debug::trace("PrinterManager.LabelTransfer", "executingCommand == true!!!!");
							executingCommand = false;
							Debug::trace("PrinterManager.CMD","(10)executingCommand: " + Integer::toString(executingCommand));
						}

						queueCommand(PrintCommand, applySecurityLabel, PrRam0, (UINT16)linesToPrint);//Alex Zavala 3/20/14

						//after printing, reset applySecurityLabel
						applySecurityLabel = PrNoSecurityLabel;

						labelPresent = false;
						Debug::trace("PrinterManager.LabelMissing", printerTraceString + " labelPresent");

						printerState = PrinterXferringLabel;
						Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterXferringLabel");
					}
					if( ( prKind != KPCPrinter ) && ( prKind != SSKPCPrinter ) ) {
						if (labelDataSize > 0)
						{
							Debug::trace("PrinterManager.PrLabelSensorLabel", " handlePrintMessage() case PrLabelTransferComplete IF labelDataSize > 0");
							
							if (outOfDataBuffers == false)
							{
								Debug::trace("PrinterManager.PrLabelSensorLabel", " handlePrintMessage() case PrLabelTransferComplete CALL sendLabelData()" );

								sendLabelData();
							}
						}
						else
						{
							Debug::trace("PrinterManager.PrLabelSensorLabel", " handlePrintMessage() case PrLabelTransferComplete ELSE labelDataSize > 0");

							xferLabelFinish = PAL_absclass::PAL_getTickCount();

							labelData = NULL;

							Debug::trace("PrinterManager.LabelTransfer", printerTraceString + " Label Transfer Time (ms): " + HobString(Integer::toString((xferLabelFinish - xferLabelStart) * 1000 / ticksPerSecond)));
						}
					} 
					receivedPrinterFeedback();
				}
				break;
				case PrLabelTransferFailed:
				{
					Debug::trace("PrinterManager.PrLabelSensorLabel", " handlePrintMessage() case PrLabelTransferFailed START");
					pendingLabelXfer = false;

					Debug::trace("PrinterManager.LabelTransfer", printerTraceString + " PrLabelTransferFailed");

					labelDataSize = 0;
					labelData = NULL;

					if (continuousTestLabels == true)
					{
						continuousTestLabels = false;
						cancelTimer(continuousLabelTimerExpired);
					}
					testLabelPrintInProgress = false;

					dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, false);
					pCurrentProductTransaction = NULL;
					currentPackId = 0;

					printerState = PrinterReadyToPrint;
					Debug::trace("PrinterManager.StateMachine", printerTraceString + " 5) PrinterReadyToPrint");
				}
				break;
				case PrAdvLabelStock:
				{
					Debug::trace("PrinterManager.PrinterAdvance", printerTraceString + " printerAdvancing: " + HobString(Bool::toString(printerAdvancing)));

					PrinterAdvanceLabelStock advLabelStock(recvBuffer);

					if (Debug::isTracing("PrinterManager.PrinterAdvance") == true)
					{
						advLabelStock.show();
					}

					queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, advLabelStock.getNumSteps());
				}
				break;
				case PrReqKind:
	 			{
					sendPrinterKind();
				}
				break;
				case PrDeletePackage:
				{
					PrinterDeletePackage prDeletePackage(recvBuffer);

					const ProductTransaction *pPT = prDeletePackage.getProductTransaction();
					 
					if (removeProductTransactionFromQ(const_cast<ProductTransaction *>(pPT)))
					{
						Debug::trace("PrinterManager.StateMachine", "PrDeletePackage dispatching label complete");
						dispatchLabelComplete(pPT, currentTransNum, false);
						//SOF-5075 
						pCurrentProductTransaction = NULL;
						currentTransNum = 0;

					}
				}
				break;
				case PrReqLabelSizing:
				{
					// Need to check if this message is even for this PrinterManager
					Debug::trace("PrinterTeachTable", printerTraceString + " handling PrReqLabelSizing");

					PrinterGeneric prReqLabelSizing(recvBuffer);
					if ( prReqLabelSizing.getPrinterType() == myPrinterType )
					{
						Debug::trace("PrinterTeachTable", printerTraceString + " handling PrReqLabelSizing for my printer" + inttostring(myPrinterType));

						// SOF-1798 - The cause of the crash was that the label was being rendered, due to the change in label stock when
						// going from opted-in continuous labels, to die cut labels when pressing the "Size Labels" button on the Size Labels
						// panel.  To remedy, I am including the code that updates printer stock metrics when the cassette is ejected
						// (Headup condition), prior to sizing.  The labelMediaError param in the LabelType object wasn't being set to
						// LabelWontFit, as was done when the cassette was ejected.
						clearLabelDimensions();
						// end of SOF-1798 fix

						usingContinuousStock = false;

						sendPrinterFlush();
						sendPrinterReset(0);
						emptyCommandQ();

						// SOF-1593 - user needs to opt-in for saving continuous label stock.
						// If we are set for continuous stock, we don't size labels, so we have to reprogram the teach tables to size labels.
						if (pMyTeachTable)
							pMyTeachTable->recreateTeachTable();
						teachPrinterNormalSizing();

						printerState = PrinterUnsized;
						enableSizing = true;

						Debug::trace("PrinterTeachTable", printerTraceString + " handling PrReqLabelSizing - taught printer to size normally");
					}
					break;
				}
				case PrUseContinuousStock:
				{
					// Need to check if this message is even for this PrinterManager
					Debug::trace("PrinterManager.handleContinuous", " handling PrUseContinuousStock");

					PrinterGeneric prUseContinuousStock(recvBuffer);
					if ( prUseContinuousStock.getPrinterType() == myPrinterType && myPrinterType == FirstPrinter )
					{
						Debug::trace("PrinterTeachTable", printerTraceString + " handling PrUseContinuousStock for my printer and is printer 1");

						usingContinuousStock = true;
						
						if( prKind != SSKPCPrinter ) {	// SOF-5057
							if(isCutterInstalled && isCutterEnabled){ 
								Debug::trace("PrinterManager.handleContinuous", " takeLabelTime = 0");
								takeLabelTime = 0; 
							}
						} else {
							if( isCutterInstalled && isCutterEnabled && printerMode != ContinuousShootLabelHeelCut ) {
								takeLabelTime = 0;
							}
						}
						// Status message would have the number of lines set to 0 when it sizes continuous stock
						lastStatusMsg.setCounter(0);

						measuredSize = Configuration::getInstance()->toIntCfgI(CfgInternal::DIMBLENGTHCONTINUOUS);
						actualSize = measuredSize;

						if (printHeelFirst == true){
							if(isCutterInstalled && isCutterEnabled){
								if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
										Debug::trace("PrinterManager.handleContinuous", " setting mode: ContinuousLabelHeelCut");
								    printerMode =  ContinuousLabelHeelCut;
							    } else {
							    	Debug::trace("PrinterManager.handleContinuous", " setting mode: ContinuousShootLabelHeelCut");
									printerMode =  ContinuousShootLabelHeelCut;
								}
							} else {
								Debug::trace("PrinterManager.handleContinuous", " setting mode: ContinuousLabelHeel");
								printerMode =  ContinuousLabelHeel;
							}
						}
						else{
							if(isCutterInstalled && isCutterEnabled){
								Debug::trace("PrinterManager.handleContinuous", " setting mode: ContinuousLabelHeadCut");
								printerMode =  ContinuousLabelHeadCut;
							}else{
								Debug::trace("PrinterManager.handleContinuous", " setting mode: ContinuousLabelHead");
								printerMode =  ContinuousLabelHead;
							}
						}

						if (pMyPrinter != NULL)
						{
							Debug::trace("PrinterTeachTable", printerTraceString + " handling PrUseContinuousStock - continuous - actualSize:=" + inttostring(actualSize) + "   measuredSize:=" + inttostring(measuredSize));	
							PrinterNomSize prNomSize(this, myPrinterType, actualSize, measuredSize);
							Debug::trace("PrinterManager.handleContinuous", " sending prNomSize");
							const char *pCC = prNomSize.serialize();
							pMyPrinter->send(pCC, prNomSize.getSerializedLength());
						}

						//no need to set for Freestanding self service scale because continuous is tied to cutter enable.
						if( ( prKind != SSKPCPrinter ) && ( !isCutterEnabled ) ) {
							// SOF-1593 - user needs to opt-in for saving continuous label stock.
							// If we are set for die-cut stock, we size labels, so we have to reprogram the teach table to not size labels.
							if (pMyTeachTable)
								pMyTeachTable->recreateTeachTable();	
							Debug::trace("PrinterManager.handleContinuous", " teachPrinter()");
							Debug::trace("PrinterManager.Teach","15 mode: " + Integer::toString(printerMode) );						
							teachPrinter(printerMode);
							Debug::trace("PrinterTeachTable", printerTraceString + " handling PrUseContinuousStock - taught printer to size for continuous");

							stockLoaded.setLength(actualSize);
							stockLoaded.setWidth(labelWidth);
							stockLoaded.setSkew(0);
							Debug::trace("PrinterTeachTable", printerTraceString + " handling PrUseContinuousStock - setStockLoaded vals - length:" + inttostring(actualSize) + "   width:" + inttostring(labelWidth));	
						} else {
							if( ( prKind == KPCPrinter ) && ( !isCutterInstalled ) ) {
								if (pMyTeachTable)
									pMyTeachTable->recreateTeachTable();	
								Debug::trace("PrinterManager.handleContinuous", " KPCPrinter: teachPrinter()");
								teachPrinter(printerMode);
								stockLoaded.setLength(actualSize);
								stockLoaded.setWidth(labelWidth);
								stockLoaded.setSkew(0);
							}
						}
						
						printerStockMetrics.setContinuousStock();

//						if( prKind != ApcPrinter ) {
							// SOF-4958 do not size if we are a freestanding self-service printer 
//							if( ( prKind != SSKPCPrinter ) && ( prKind != KPCPrinter ) ) {
//									Debug::trace("PrinterManager.handleContinuous", "setting printerState: PrinterUnsized");
//							    printerState = PrinterUnsized;
//							    enableSizing = true;
//							} else {
								Debug::trace("PrinterManager.handleContinuous", "setting printerState: PrinterReadyToPreImage");
								printerState = PrinterReadyToPreImage;
								enableSizing = false;
								printerStockMetrics.setLabelWidth( WideLabel );
								dispatchEvent( &printerStockMetrics );
//							}
//						}

						Debug::trace("PrinterTeachTable", printerTraceString + " handling PrUseContinuousStock - end");

						//dispatchEvent(&printerStockMetrics);
						//Debug::trace("PrinterTeachTable", printerTraceString + " handling PrUseContinuousStock - dispatched printerStockMetrics");
					}
					break;
				}

				case PrSetupForNormalSizing:
				{
					Debug::trace("PrinterTeachTable", printerTraceString + " handling PrSetupForNormalSizing");

					// If we are set for continuous stock, we don't size labels, so we have to reprogram the teach table to size labels.
					// So just re-teach the sizing command in case we are set for continuous.
					usingContinuousStock = false;
					teachPrinterNormalSizing();
					break;
				}
				case PrHeadDotWear:
				{
					if( (Configuration::getInstance()->isDotWearLicensed()) && (Configuration::getInstance()->isDotWearEnabled())){
						Debug::trace("PrinterManager.HeadDot", " handlePrinterMessage case PrHeadDotWear.");

						PrinterHeadWear dotWear( recvBuffer );
						dotWear.show();


						dotWear.setSourceObject(this);  // jhc NOTE: This call makes this message not come back to itself
						dispatchEvent(&dotWear);	// send up to the SessionPrinter to pass along back to the UI
						
						// only report to the renderer if we have both good and bad dots. If all dots are bad or good we dont blit
						if (lastDotStatus == DOTS_MIXED){							
							//convert from 1 dot per byte to 4 dot per byte because the renderer treats marginal and bad dots the same 
							Debug::trace("PrinterManager.HeadDot"," handling PrHeadDotWear. Head Size " + Integer::toString(dotWear.getHeadSize()));
							this->marginalBits.clear();
							uint8_t compressedDot = 0xFF;
							if(!printHeelFirst){
								for (int i =0; i < dotWear.getHeadSize(); i++){
									//Set bit to 0 if dot is bad.
									if(dotWear.getDotStatus(i) != DotGood){
										compressedDot = compressedDot & ~(1 << (i %8));
									}
									if(i % 8 == 0){
										marginalBits.push_back(compressedDot);
										compressedDot = 0xFF;
									}
								}
							}else{
								for (int i = (dotWear.getHeadSize() -1); i >=0; i--){
									//Set bit to 0 if dot is bad.
									if(dotWear.getDotStatus(i) != DotGood){
										compressedDot = compressedDot & ~(1 << (i %8));
									}
									if(i % 8 == 0){
										marginalBits.push_back(compressedDot);
										compressedDot = 0xFF;
									}
								}
							}

							RendererNewDotHeadWear rWear(this, marginalBits);
							rWear.show();
							dispatchEvent(&rWear);
							receivedPrinterFeedback();
						}
					}
					break;
				}
				case PrKPCError: {
					PrinterKpcError error( recvBuffer );
					std::string str = "KPC Firmware: " + error.getErrorDescription();
					Debug::trace("PrinterManager.Error", str);
					break;
				}
				case PrHeadDotStatus:
				{
					if(!initialDotWearComplete){
						initialDotWearComplete = true;
					}
					Debug::trace("PrinterManager.HeadDot"," handlePrinterMessage case PrHeadDotStatus.");
					Debug::trace("PrinterManager.HeadDot", std::string("case PrHeadDotStatus bufSize: ") + Integer::toString(bufSize) );
					Debug::trace("PrinterManager.HeadDot", std::string("case PrHeadDotStatus recvBuffer: ") + HobString(Hex::toString(recvBuffer, bufSize)));
					
					PrinterHeadDotStatus status( recvBuffer );
					lastDotStatus = (DotWearHeadStatus) status.getStatus();

					status.setSourceObject(this);  // jhc NOTE: This call makes this message not come back to itself
					dispatchEvent(&status);	// send up to the SessionPrinter to pass along back to the UI

					if(status.getStatus()  == DOTS_MIXED)
					{
						Debug::trace("PrinterManager.HeadDot","IF Requesting head dot wear.");
						
						requestHeadDotWear();
					}
					else
					{
						if(status.getStatus()  == ALL_DOTS_BAD){
							requestHeadDotWear();
						}
#if defined ( __linux__ )
						UINT8 bits[this->printheadSize]= {0};
						std::fill_n(bits,this->printheadSize, 0xFF );
#else
						UINT8 bits[448] = { 1 };	// jhc - this is strictly to run on windows I got 448 from Aaron The #linux line will not compile on windows bc windows expects the size variable to be a const
#endif
						Debug::trace("PrinterManager.HeadDot", "ELSE Requesting head dot wear so actual wear.");

						RendererNewDotHeadWear rWear(this, bits, this->printheadSize);
						rWear.show();
						dispatchEvent(&rWear);
					}
					break;
				}
				case PrReqCutterStatus:
				{
					Debug::trace("PrinterManager.Cutter"," handling PrReqCutterStatus.");
					if (pMyPrinter != NULL)
					{
						pMyPrinter->send((const char*) recvBuffer, bufSize);
					}
					break;
				}
				
				case PrCutterCut:
				{
					Debug::trace("PrinterManager.Cutter"," handling PrCutterCut.");
					if (pMyPrinter != NULL)
					{
						pMyPrinter->send((const char*) recvBuffer, bufSize);
					}
					break;
				}
				
				case PrCutterStatus:
				{
					Debug::trace("PrinterManager.Cutter"," handling PrCutterStatus.");

					const bool isCutterInstalledBefore( isCutterInstalled );

					PrinterCutterStatus stat(recvBuffer);
					isCutterInstalled = stat.getInstalled();
					lastCutterStatusMsg = stat;

					if (( !isCutterInstalledBefore ) && ( isCutterInstalledAndEnabled() ))
						handleMode( RealTimeController::getInstance()->getCurrentOpModeEvent() ); 
	
					stat.show();

          stat.setSourceObject(this);         // jhc - prevent calling yourself.
					dispatchEvent(&stat);

#if defined ( __linux__ )
					util_filesystem::File  boardSiteFile(BOARD_SITE_FILE_NAME);
					//are we running board site tests?
					if ( boardSiteFile.exists() )  {
						BoardSiteGeneric msg( BrdSitePeripheralCutterStat, this ); 
						dispatchEvent( &msg );
					}
#endif
					break;
				}

				case PrCutterEnabled:
				{
					Debug::trace("PrinterManager.Cutter", " if case PrCutterEnabled handle Enabling the Cutter");

					isCutterEnabled = true;

					if (printHeelFirst == true)
					{
						if (isCutterInstalled)
						{
							if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
							    printerMode = ContinuousLabelHeelCut;
							} else {
								printerMode =  ContinuousShootLabelHeelCut;
							}
						}
						else
						{
							printerMode = ContinuousLabelHeel;
					  }
					}
					else
					{
						if (isCutterInstalled){	
							printerMode = ContinuousLabelHeadCut;
						} else {
							printerMode = ContinuousLabelHead;
						}
					}

					teachPrinter(printerMode);

					break;
				}

				case PrCutterDisabled:
				{
					Debug::trace("PrinterManager.Cutter", " if case PrCutterDisabled handle Disabling the Cutter");

					isCutterEnabled = false;

					if( printHeelFirst == true ) 
					{
						if( ( isCutterInstalled ) && ( isCutterEnabled ) ) {
							if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
							    printerMode = ContinuousLabelHeelCut;
						  } else {
		              printerMode =  ContinuousShootLabelHeelCut;              
            	}
            } else {
							//SOF-5029
							if( prKind == KPCPrinter ) {
								printerMode = ContinuousLabelHeel;
							} else {
								printerMode = DieCutShootHeel;
							}
				    }
					} 
					else 
					{
						if( ( isCutterInstalled ) && ( isCutterEnabled ) )
							printerMode = ContinuousLabelHeadCut;
						else
							printerMode = ContinuousLabelHead;
					}
					teachPrinter(printerMode);

					break;
				}

				case PrEjectCut:
				{
					Debug::trace("PrinterManager.Cutter", "Processing PrEject Cutter");
					if(usingContinuousStock){
						if( prKind != SSKPCPrinter ) {
					    //continuous stock so we dont really care where we cut so cut now and retract
					    Debug::trace("PrinterManager.Cutter", "Continuious Cut");
					    queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, 609);//203 steps per inch so eject 3 inches							
							queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, 25);
							queueCommand(CutRetractCommand, PrNoSecurityLabel, PrRam2,-275); //-25 == 1/8" Scott freestanding scale test
						} else {
							Debug::trace("PrinterManager.Cutter", "SSKPCPrinter -- Continuious Cut");
							queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, 609);//203 steps per inch so eject 3" inches
							queueCommand(CutRetractCommand, PrNoSecurityLabel, PrRam2,-101); //retract 1""
						}
					}else{
						//die-cut stock so we need to eject to be in between the labels and then cut
						queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, 215);//203 steps per inch
						queueCommand(CutRetractCommand, PrNoSecurityLabel, PrRam2, -225);
						queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, 30);
					}
					break;
				}
				//SOF-3751 Test to validate print roller is within spec.
				case PrReqTestCassette: 
				{	
					Debug::trace("PrinterManager", "Processing PrReqTestCassette");	
					cassetteTest = true;			
					queueCommand(SizeCommand, PrNoSecurityLabel, PrNoIndirection, 0x0);
					printerState = PrinterTryingToSize;
					resetPrinter = false;
					executingCommand = false;
                    Debug::trace("PrinterManager.CMD","(11)executingCommand: " + Integer::toString(executingCommand));
					PrinterStatus prStatus(lastStatusMsg);
					prStatus.setLabelSizing();
					dispatchEvent(&prStatus);

					lastStatusMsg.setLabelSizing(); // make sure it is on!!

					pCurrentProduct = NULL;
					currentProductSmartPtr = Util::CSmartPtr<Product>();

					labelPresent = false;
					break;
				}
				// SOF-3887 
				case PrCutterBitDflt:
				{
					Debug::trace("PrinterManager.Message", "PrCutterBitDflt");
					if( pMyPrinter != NULL ) {
						pMyPrinter->send( (const char*)recvBuffer, bufSize );
					}
					break;
				}
				case PrResetTeach: 
				{
					Debug::trace("PrinterManager.Message", "PrResetTeach");
					noCutOption = Configuration::getInstance()->toBoolCfg(fieldOffsetCfgVirtualCombinePrint);				//SOF-3847 3848
					if( ( prKind == SSKPCPrinter ) || ( prKind == KPCPrinter ) ) {
						if( noCutOption ) { 	//SOF-3847 3848
							Debug::trace("PrinterManager.Message", "PrResetTeach: ContinuousLabelHeadCombined CutOption");
							printerMode =  ContinuousLabelHeelCombined;
							takeLabelTime = 0;							
						} else {
							if( isCutterInstalled && isCutterEnabled ) {
								if (printHeelFirst == true)//Fix SOF-5041
								{
									Debug::trace("PrinterManager.Message", "PrResetTeach: ContinuousLabelHeelCut noCutOption");
									printerMode =  ContinuousLabelHeelCut;
								}
								else
								{
									Debug::trace("PrinterManager.Message", "PrResetTeach: ContinuousLabelHeadCut noCutOption");
									printerMode =  ContinuousLabelHeadCut;
								}
							}
							takeLabelTime = ticksPerSecond / 2;						
						}
					}
					teachPrinter(printerMode);					
					break;
				}
				case PrStartGapCalibration:
				{
					Debug::trace("PrinterManager.Gap", "PrStartGapCalibration printerCalibrating == true");
					printerCalibrating = true;
					Debug::trace("PrinterManager.Gap", "printerState: " + Integer::toString(printerState) );
					//SOF-5161 do not expel and cut if we are calibrating the shoot through sensor
					if( ( printerState == PrinterWaitToExpelAndCut ) || ( printerState == PrinterWaitInterlock ) 
						 || ( printerState == PrinterPriorityError )  || ( printerState == PrinterHeadDisengaged ) ) {
						Debug::trace("PrinterManager.Gap", "Change Printer State:  PrinterReadyToPrint");
						printerState = PrinterReadyToPrint;
					}

					PrinterGeneric cal( recvBuffer );
					const char *pCC = cal.serialize();
					pMyPrinter->send(pCC, cal.getSerializedLength());
					break;
				}
				case PrGapCalStatus:
				{
					Debug::trace("PrinterManager.Message", "PrGapCalStatus");
					PrinterGapCalStatus status( recvBuffer );
					status.setSourceObject(this);
					
					Debug::trace("PrinterManager.Gap", "State: " + Integer::toString(status.getState() ) );
					//SOF-5107 size after shoot through calibration
					if( status.getState() == 6 ) {

						clearLabelDimensions();

						usingContinuousStock = false;

						sendPrinterFlush();
						sendPrinterReset(0);
						emptyCommandQ();

						teachPrinterNormalSizing();
						Debug::trace("PrinterManager.Gap", "printerCalibrating == false");
						printerCalibrating = false;	//SOF-5161
						
						printerState = PrinterUnsized;
						enableSizing = true;						
					}
					dispatchEvent( &status );
					break;
				}
				case PrGapCalNext:
				{
					Debug::trace("PrinterManager.Gap", "PrGapCalNext");
					PrinterGeneric next( recvBuffer );
					const char *pCC = next.serialize();
					pMyPrinter->send(pCC, next.getSerializedLength());
					break;
				}
				default:
				{
					Debug::trace("PrinterManager.Error", printerTraceString + " Some Other PrEvent:" + HobString(Integer::toString((UINT16) prEvent)));
				}
				break;
			}
		}
		else
		{
			switch (prEvent)
			{
				case PrCutterEnabled:
				{
					Debug::trace("PrinterManager.Cutter", "else case PrCutterEnabled handle Enabling the Cutter");

					isCutterEnabled = true;


					// SOF-5062
					if( ( prKind == SSKPCPrinter ) && isCutterEnabled ) {
						usingContinuousStock = true;
						lastStatusMsg.setCounter(0);

						measuredSize = Configuration::getInstance()->toIntCfgI(CfgInternal::DIMBLENGTHCONTINUOUS);
						actualSize = measuredSize;
						PrinterNomSize prNomSize(this, myPrinterType, actualSize, measuredSize);
						const char *pCC = prNomSize.serialize();
						pMyPrinter->send(pCC, prNomSize.getSerializedLength());

						stockLoaded.setLength(actualSize);
						stockLoaded.setWidth(labelWidth);
						stockLoaded.setSkew(0);
						printerStockMetrics.setContinuousStock();

						printerState = PrinterReadyToPreImage;
						enableSizing = false;
						printerStockMetrics.setLabelWidth( WideLabel );
						dispatchEvent( &printerStockMetrics );
					}


					if (printHeelFirst == true)
					{
						if (isCutterInstalled) {
							if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
							    printerMode = ContinuousLabelHeelCut;
							} else {
								printerMode =  ContinuousShootLabelHeelCut;
							}
						} else {
							printerMode = ContinuousLabelHeel;
					    }
					}
					else
					{
						if (isCutterInstalled)
							printerMode = ContinuousLabelHeadCut;
						else
							printerMode = ContinuousLabelHead;
					}

					teachPrinter(printerMode);

					break;
				}

				case PrCutterDisabled:
				{
					Debug::trace("PrinterManager.Cutter", " else case PrCutterDisabled handle Disabling the Cutter");

					isCutterEnabled = false;

					if( printHeelFirst == true )
					{
						if( ( isCutterInstalled ) && ( isCutterEnabled ) ) {
							if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
								Debug::trace("PrinterManager.Cutter", " 1 prKind: " + Integer::toString(prKind) + " enable: " + Integer::toString(isCutterEnabled));
								printerMode = ContinuousLabelHeelCut;
							} else {
								printerMode =  ContinuousShootLabelHeelCut;
							}
						} else {
							//SOF-5029
							if( prKind == KPCPrinter ) {
								printerMode = ContinuousLabelHeel;
							} else {
								printerMode = DieCutShootHeel;
							}
						}
					}
					else
					{
						if( ( isCutterInstalled ) && ( isCutterEnabled ) )
							printerMode = ContinuousLabelHeadCut;
						else
							printerMode = ContinuousLabelHead;
					}

					teachPrinter(printerMode);

					break;
				}
				default:
				{
					Debug::trace("PrinterManager.Error", printerTraceString + " Some Other PrEvent:" + HobString(Integer::toString((UINT16)prEvent)));
				}
				break;
			}
		}
	}
}

void PrinterManager::handleKpcSystemError(KpcSystemEvent event, const char *recvBuffer)
{
	switch (event)
	{
		case KpcError: {
			KpcSystemErrors error( recvBuffer );
			std::string str = "KPC System: " + error.getErrorDescription();
			Debug::trace("CANPrinterManager.Error", str);
			break;
		}
	}
}

void PrinterManager::handlePrinterSysMessage(SysEvent sysEvent, const char *recvBuffer)
{
	switch (sysEvent)
	{
		case SysWakeup:
			{
				Debug::trace("PrinterManager.PrinterSystemWakeup", printerTraceString + " SysWakeup");
				PrinterSystemWakeup psw(recvBuffer);

				psw.setSourceObject(this);
				psw.setPrinterType(myPrinterType);
				dispatchEvent(&psw);

				if (Debug::isTracing("PrinterManager.PrinterSystemConfiguration") == true)
				{
					psw.show();
				}

				receivedPrinterFeedback();
			}
			break;
		case SysConfig:
			{
				Debug::trace("PrinterManager.PrinterSystemConfiguration", printerTraceString + " SysConfig");
				PrinterSystemConfiguration psc(recvBuffer);

				if (Debug::isTracing("PrinterManager.PrinterSystemConfiguration") == true)
				{
					psc.show();
				}

				if ((psc.getPrinterType() == myPrinterType) && (pMyPrinter != NULL))
				{
					const char *pCC = psc.serialize();
					pMyPrinter->send(pCC, psc.getSerializedLength());

					if (printerSysConfig.equals(psc) == false)
					{
						printerSysConfig = psc;
						sendPrinterSystemConfig(printerSysConfig.getSysConfigType());
					}
				}
			}
			break;
		case SysInquire:
			{
				Debug::trace("PrinterManager.PrinterSystemInquire", printerTraceString + " SysInquire");
				PrinterSystemInquire psi(recvBuffer);

				psi.setSourceObject(this);
				psi.setPrinterType(myPrinterType);
				dispatchEvent(&psi);

				if (Debug::isTracing("PrinterManager.PrinterSystemInquire") == true)
				{
					psi.show();
				}

				printerSysConfig.update(psi);

				receivedPrinterFeedback();
			}
			break;
		case SysStatus:
			{
				Debug::trace("PrinterManager.PrinterSystemStatus", printerTraceString + " SysStatus");
				PrinterSystemStatus pss(recvBuffer);

				pss.setSourceObject(this);
				pss.setPrinterType(myPrinterType);
				dispatchEvent(&pss);

				if (Debug::isTracing("PrinterManager.PrinterSystemStatus") == true)
				{
					pss.show();
				}

				receivedPrinterFeedback();
			}
			break;
		case SysReqStatus:
			{
				Debug::trace("PrinterManager.PrinterSystemRequestStatus", printerTraceString + " SysReqStatus");
				PrinterSystemRequestStatus psrs(recvBuffer);

				if ((psrs.getPrinterType() == myPrinterType) && (pMyPrinter != NULL))
				{
					if( ( prKind == KPCPrinter ) || ( prKind == SSKPCPrinter ) ) {
						PrinterGeneric msg( PrReqVersion, this, myPrinterType);
						const char *pCC = msg.serialize();
						pMyPrinter->send(pCC, msg.getSerializedLength());


					} else {
						psrs.setSourceObject(this);

						const char *pCC = psrs.serialize();
						pMyPrinter->send(pCC, psrs.getSerializedLength());

						if (Debug::isTracing("PrinterManager.PrinterSystemRequestStatus") == true)
						{
							psrs.show();
						}
					}
				}
			}
			break;
		case SysReqInquire:
			{
				Debug::trace("PrinterManager.PrinterSystemRequestInquire", printerTraceString + " SysReqInquire");
				PrinterSystemRequestInquire psri(recvBuffer);

				if ((psri.getPrinterType() == myPrinterType) && (pMyPrinter != NULL))
				{
					psri.setSourceObject(this);

					const char *pCC = psri.serialize();
					pMyPrinter->send(pCC, psri.getSerializedLength());

					if (Debug::isTracing("PrinterManager.PrinterSystemRequestInquire") == true)
					{
						psri.show();
					}
				}
			}
			break;
		case SysCalibSwitch:
			{
				Debug::trace("PrinterManager.WeigherSystemCalibrationSwitch", printerTraceString + " SysCalibSwitch");
				PrinterSystemCalibrationSwitch calibSwitch(recvBuffer);

				calibSwitch.setSourceObject(this);
				calibSwitch.setDeviceType(WeigherSystem);
				dispatchEvent(&calibSwitch);

				if (Debug::isTracing("PrinterManager.WeigherSystemCalibrationSwitch") == true)
				{
					calibSwitch.show();
				}

				receivedPrinterFeedback();
			}
			break;
		default:
			{
				Debug::trace("PrinterManager.Error", printerTraceString + " Some Other System Event: " + HobString(Integer::toString((UINT16) sysEvent)));
			}
			break;
	}
}

void PrinterManager::handleRendererMessage(RdEvent rdEvent, const char *recvBuffer)
{
	switch ( rdEvent )
	{
	case RdSessionId: 
	{
		Debug::trace("PrinterManager.Renderer", printerTraceString + " RdSessionId");
		RendererSessionId rendererSessionId(recvBuffer);

		myRendererSessionId = rendererSessionId.getSessionId();
		break;
	}

	case RdRenderStart:
		Debug::trace("PrinterManager.Renderer", printerTraceString + " RdRenderStart");
		break;

	case RdRenderComplete: 
	{
		Debug::trace("PrinterManager.Renderer", printerTraceString + " RdRenderComplete");
		RendererRenderComplete renderComplete(recvBuffer);

		if (renderComplete.getTicks() < 0)
		{
			// Yea, that's right, this is how I'm notifying that the self serve barcode label imaging is done rendering, by setting the tick counts to -42.
			// At least that's how I started to do it.  When we made a rotationg number of barcode image files, I now use the negative of the filename's counter value.
			// And this is why we don't rotate back to 0, since you can't have -0.
			// Really, it's a great idea, and it'll work!
			Debug::trace("SelfServeBarcodeLabel", printerTraceString + " Render complete for selfServe barcode label, sending message to Session");
			PrinterDonePostImagingSelfServeBarcodeLabelImage donePostImagingSelfServeBarcodeLabelImageMsg(0, FirstPrinter, -renderComplete.getTicks());
			dispatchEvent(&donePostImagingSelfServeBarcodeLabelImageMsg);
		}

		// --------------------------------------------------------------------
		// PRE IMAGE COMPLETE
		// --------------------------------------------------------------------
		if ( PrinterPreImaging == printerState )
		{
			if (renderingRFIDVoidLabelType)
			{
				Debug::trace("PrinterManager.rfid", printerTraceString + " renderingRFIDVoidLabelType preImagingFinished finished");
				renderingRFIDVoidLabelType = false;
				printerState = printerStatePriorToRenderingRFIDVoidLabelType;
				//printerState = PrinterReadyToPrint;  // PrinterReadyToPrint
				Debug::trace("PrinterManager.rfid", printerTraceString + " renderingRFIDVoidLabelType = false, printerState = " + inttostring(printerState));
			}
			else
			{
				bool preImagingFinished(true);

				if (virtualPrintingEnabled)
				{
					for (int i = 0; i < MaxPrinterTypes; ++i)
						if ((imageControlBlock[i]) && (0 != imageControlBlock[i]->getImageFlags()))
							preImagingFinished = false;
				}

				if (preImagingFinished)
				{
					printerState = PrinterReadyToPrint;
					//SOF-
					if ((takeLabelTime) && ((printerMode == ContinuousLabelHeelCombined) ||
						(printerMode == ContinuousLabelHeadCombined))) {
						takeLabelTime = 0;
					}
					Debug::trace("PrinterManager.StateMachine", printerTraceString + " 6) PrinterReadyToPrint  takeLabelTime: " + Integer::toString(takeLabelTime));
				}
				else
				{
					// We still have other label types that need imaged for this product/transaction.  We're not done rendering yet.
					Debug::trace("PrinterManager.StateMachine", printerTraceString + " not switching state yet, in handling RdRenderComplete");
				}
			}
		}

		// --------------------------------------------------------------------
		// POST IMAGE COMPLETE
		// --------------------------------------------------------------------
		else if ( PrinterPostImaging == printerState )
		{
			bool postImagingFinished( true );

			if ( virtualPrintingEnabled) 
			{
				for ( int i = 0; i < MaxPrinterTypes; i++ )
					if (( imageControlBlock[i] ) && ( 0 != imageControlBlock[i]->getImageFlags() ))
						postImagingFinished = false;
			}

			Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " PostImage Complete: 0x" + HobString(Hex::toString((long long) pCurrentProductTransaction)));

			// make sure the RS485 is ready if we are a kpc
			Debug::trace("PrinterManager", "prKind: " + Integer::toString(prKind));
			bool kpcready( false );

			if ( ( KPCPrinter == prKind ) || ( prKind == SSKPCPrinter ) )
			{
				Debug::trace("PrinterManager", "Kpc Printer, Checking RS485Manager..." );

				if ( !pRS485Manager )
					Debug::trace("PrinterManager.Error", "RS485Manger is NULL");
				else
				{
					const int sleepTime( 100 );

					for ( int maxwait( 30000 / sleepTime ); maxwait--; )
					{
						if ( RS485Manager::_Init == pRS485Manager->getState() )
						{
							kpcready = true;
							break;
						}

						Debug::trace("PrinterManager", "Kpc Printer, Wait RS485Manager READY..." );
						PAL_absclass::PAL_sleep( sleepTime );
					}
				}
			}
			else
			{
				Debug::trace("PrinterManager", "We Are Not A KPC by default we are ready" );
				kpcready = true;
			}
			
			Debug::trace("PrinterManager", "Kpc Ready: " + Integer::toString(kpcready));

			// For now, we won't continue on to printing any labels until all of them have been completely rendered.
			// If we have a performance issue, I can come back and look at this.
			if (( postImagingFinished ) && ( kpcready ))
			{
				PrinterType labelNumToStartPrinting( myPrinterType );

				Debug::trace("PrinterManager", "in postImagingFinished");

				// if virtual printing, go ahead and start printing the first label.
				// If not virtual printing, go ahead and print the label that this printer is set for.
				if ( virtualPrintingEnabled) 
				{
					for ( int i = 0; i < MaxPrinterTypes; ++i )
						if ( imageControlBlock[i] )
						{
							labelNumToStartPrinting = (PrinterType)i;
							break;
						}

					currentVirtuallyPrintingLabelNum = labelNumToStartPrinting;
				}

				// ----------------------------------------
				// Fake Printer
				// ----------------------------------------
				if ( isFakePrinterManager() )
				{
					if (( myPrinterType > FirstPrinter ) && ( myPrinterType < MaxPrinterTypes ))
					{
						if ( imageControlBlock[myPrinterType] )
						{
							const std::string filenameString( Configuration::getInstance()->toStringCfgI( CfgInternal::USBPRINTEROUTPUTFILENAME ) );

							PAL_absclass::PAL_sleep( 500 );

							Debug::trace("PrinterManager.PrinterImaging", "postImagingFinished - before saving label to file: " + filenameString);
							Hobart::CRendererUtil::SaveBitmapToFile( imageControlBlock[myPrinterType]->getImageBitMap(), filenameString );

							Debug::trace("PrinterManager.PrinterImaging", "postImagingFinished - back from saving label to file: " + filenameString);

							// If a Zebra printer is connected via USB, have it print the label.
							if ( streamingLabels )
							{
								SystemPrinters::printToUSBLabelPrinter( filenameString, pCurrentProductTransaction->getNumOfLabelsToPrint() );
								pCurrentProductTransaction->setNumOfLabelsPrinted( pCurrentProductTransaction->getNumOfLabelsToPrint() );
							}
							else
							{
								SystemPrinters::printToUSBLabelPrinter( filenameString, 1 );
								const_cast<ProductTransaction *>(pCurrentProductTransaction)->incrementNumOfLabelsPrinted();
							}
						}
					}

					if ( pCurrentProductTransaction )
						const_cast<ProductTransaction *>(pCurrentProductTransaction)->incrementNumOfLabelsPrinted();

					dispatchLabelComplete( pCurrentProductTransaction, currentTransNum, true );
					pCurrentProductTransaction = NULL;
					currentPackId = 0;

					printerState = PrinterReadyToPrint;
				}

				// ----------------------------------------
				// Self Service Print to Screen
				// ----------------------------------------
				else if (( Configuration::getInstance()->toBoolCfg( fieldOffsetCfgSelfServePrintToScreen ) ) &&
						 ( Configuration::getInstance()->isSelfServiceScale() ) ) //SOF-5355 hack hack!! in for demo with jim   && !printToScreenFlag_
				{
/* SOF-4405 - not sure this is needed anymore, print to screen and label at same time?
					// print to screen, but still need to do that label part in the else if the next condition too
					if (Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServeSearchKeypadFlashkeys) && (Configuration::getInstance()->toIntCfg(fieldOffsetCfgSelfServeIgnorePrinterErrors) == SelfServOption_PrintToScreenAndLabel))
					{
						Debug::trace("PrinterManager.TestScreenLabel", "handleRenderMessage() if BigY and printToScreen startPrintingLabel()");
						startPrintingLabel(labelNumToStartPrinting);
					}
*/
					//SOF-5355 hack hack!! in for demo with jim 	
					//printToScreenFlag_ = true;

					if ( imageControlBlock[myPrinterType] )
					{
						const std::string filenameString( Configuration::getInstance()->toStringCfgI( CfgInternal::SELFSERVEPRINTTOSCREENFILENAME ) );

						PAL_absclass::PAL_sleep(500);

						Debug::trace("PrinterManager.PrinterImaging", "postImagingFinished - before saving label to SelfServePrintToScreen file: " + filenameString);
						Hobart::CRendererUtil::SaveBitmapToFile( imageControlBlock[myPrinterType]->getImageBitMap(), filenameString );

						Debug::trace("PrinterManager.PrinterImaging", "postImagingFinished - back from saving label to SelfServePrintToScreen file: " + filenameString);
					}

					pCurrentProductTransaction = NULL;
					currentPackId = 0;
					printerState = PrinterReadyToPrint;
				}

				// ----------------------------------------
				// Normal Printer
				// ----------------------------------------
				else
				{
					//SOF-5355 hack hack!! in for demo with jim  	
					//printToScreenFlag_ = false;
					
					Debug::trace("PrinterManager", "StartPrintingLabel()");
					startPrintingLabel( labelNumToStartPrinting );
				}

				streamingLabels = false;
			}
		}
			
		break;
	}

	default:
		break;
	}
}

bool PrinterManager::handleMode( OpModeEvent mEvent )
{
	bool result = false;

	if( prKind == ApcPrinter ) {
		if( ( !isCutterInstalled ) && ( isCutterEnabled ) ) {
			isCutterEnabled = false; 
		}	
	}	

	switch ( mEvent )
	{
		case OpModeManualWeigh: {						
			if( printHeelFirst == true ) {
				if( isCutterEnabled ) {		
					if( !noCutOption ) { 	
						if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ){						
							if( !usingContinuousStock ) {
								printerMode = DieCutLabelHeel;
							} else {		
								if( isCutterInstalled ) {
									printerMode =  ContinuousLabelHeelCut;
								} else {
									printerMode =  ContinuousLabelHeel;	
								}
							}
						} else {		
							printerMode =  ContinuousShootLabelHeelCut;
						}
						takeLabelTime = ticksPerSecond / 2;
					} else {					
						printerMode =  ContinuousLabelHeelCombined;
						takeLabelTime = 0;		
					} 
					result = true;
					teachPrinter( printerMode );					
					Debug::trace("PrinterManager.VirtualCombine", "OpModeManualWeigh: printerMode = " + Integer::toString(printerMode));
				}
			} else {
				if( isCutterEnabled ) {
					if( !noCutOption ) {						
						printerMode =  ContinuousLabelHeadCut;
						takeLabelTime = ticksPerSecond / 2;
					} else {
						printerMode =  ContinuousLabelHeelCombined;
						takeLabelTime = 0;		
					} 
					result = true;
					teachPrinter( printerMode );
					Debug::trace("PrinterManager.VirtualCombine", "OpModeManualWeigh: printerMode = " + Integer::toString(printerMode));						
				}
			}			
			break;
		}
		
		case OpModeProductionEntry: {			
			if( ( isCutterEnabled ) && ( isCutterInstalled ) ) {
				noCutOption = Configuration::getInstance()->toBoolCfg(fieldOffsetCfgVirtualCombinePrint);				//SOF-3847 3848
				if( noCutOption ) {					
					if( printHeelFirst == true ) {						
						printerMode =  ContinuousLabelHeelCombined;											
					} else {
						printerMode =  ContinuousLabelHeadCombined;
					}
					result = true;
					takeLabelTime = 0;					
					teachPrinter(printerMode);					
					Debug::trace("PrinterManager.TakeLabelTime", " takeLabelTime = 0");
					Debug::trace("PrinterManager.VirtualCombine", "OpModeProductionEntry: printerMode = " + Integer::toString(printerMode));																
				} else {
					if( printHeelFirst == true ) {
						if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {							
						    printerMode =  ContinuousLabelHeelCut;											
					} else {
							printerMode =  ContinuousShootLabelHeelCut;
						}
					} else {
						printerMode =  ContinuousLabelHeadCut;
					}
					result = true;
					takeLabelTime = ticksPerSecond / 2;
					teachPrinter(printerMode);					
					Debug::trace("PrinterManager.TakeLabelTime", " takeLabelTime: " + Integer::toString( takeLabelTime ) );
					Debug::trace("PrinterManager.VirtualCombine", "OpModeProductionEntry: printerMode = " + Integer::toString(printerMode));																					
				}
			}
			break;

		}
		case OpModeAutoManualApply: {			
			if( printHeelFirst == true ) {
				if( isCutterEnabled ) {
					if( !noCutOption ) { 	
						if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
							if( !usingContinuousStock ) {
								printerMode =  DieCutLabelHeel;
							} else {							
								if( isCutterInstalled ) {
						    	printerMode =  ContinuousLabelHeelCut;
						  	} else {
						  		printerMode =  ContinuousLabelHeel;
						  	} 
						  }
						} else {
							printerMode =  ContinuousShootLabelHeelCut;
						}
						takeLabelTime = ticksPerSecond / 2;
					} else {
						printerMode =  ContinuousLabelHeelCombined;
						takeLabelTime = 0;
					} 
					result = true;
					teachPrinter(printerMode);
					Debug::trace("PrinterManager.VirtualCombine", "OpModeAutoManualApply: printerMode = " + Integer::toString(printerMode));
				}
			} else {
				if( isCutterEnabled ) {
					if( !noCutOption ) { 	
						printerMode =  ContinuousLabelHeadCut;
						takeLabelTime = ticksPerSecond / 2;						
					} else {
						printerMode =  ContinuousLabelHeadCombined;
						takeLabelTime = 0;
					}
					result = true;
					teachPrinter(printerMode);
					Debug::trace("PrinterManager.VirtualCombine", "OpModeAutoManualApply: printerMode = " + Integer::toString(printerMode));
				}
			}			
			break;
		}
		default: {
			Debug::trace("PrinterManager", "Unknown event: " + Integer::toString(mEvent));  	
		}
	}
	
	return result;
}

//SOF-3847 3848
void PrinterManager::handleModeMessage( OpModeEvent mEvent, const char* /*recvBuffer*/ )
{
	noCutOption = Configuration::getInstance()->toBoolCfg(fieldOffsetCfgVirtualCombinePrint);				//SOF-3847 3848
	switch ( mEvent )
	{
		case OpModeManualWeigh: {
			if( printHeelFirst == true ) {
				if(isCutterInstalled && isCutterEnabled){
					if( !noCutOption ) { 
						if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
						    printerMode =  ContinuousLabelHeelCut;
						} else {
							printerMode =  ContinuousShootLabelHeelCut;
						}
						takeLabelTime = ticksPerSecond / 2;						
					} else {
						printerMode = ContinuousLabelHeelCombined;
						takeLabelTime = 0;						
					}					
					teachPrinter(printerMode);
					Debug::trace("PrinterManager.VirtualCombine", "handleModeMessage() OpModeManualWeigh: printerMode = " + Integer::toString(printerMode));
				} else {
					//SOF-3993 non cutter scale
					if( !isCutterInstalled ) {
						if ( !Configuration::getInstance()->toBoolCfg(fieldOffsetCfgContinuousStockIgnoreLabelTaken) ) {
							takeLabelTime = ticksPerSecond / 2;													
						} else {
							//SOF-3998
							if( !isDieCutloaded() ) {
								takeLabelTime = 0;								
							} else {
								takeLabelTime = ticksPerSecond / 2;	
							}
						}
						Debug::trace("PrinterManager.VirtualCombine", "handleModeMessage() OpModeManualWeigh takeLabelTime: " + Integer::toString(takeLabelTime));
					}
				}
			} else {
				if(isCutterInstalled && isCutterEnabled){
					if( !noCutOption ) {
						printerMode =  ContinuousLabelHeadCut;
						takeLabelTime = ticksPerSecond / 2;
					} else {
						printerMode = ContinuousLabelHeadCombined;
						takeLabelTime = 0;
					}
					teachPrinter(printerMode);
					Debug::trace("PrinterManager.VirtualCombine", "handleModeMessage() OpModeManualWeigh: printerMode = " + Integer::toString(printerMode));
				}
			}			
			break;
		}
		
		case OpModeProductionEntry: {
			if(isCutterInstalled && isCutterEnabled) {				
				if( noCutOption ) {
					if( printHeelFirst == true ) {
						printerMode =  ContinuousLabelHeelCombined;											
					} else {
						printerMode =  ContinuousLabelHeadCombined;
					}
					takeLabelTime = 0;
					
				} else {
					if( printHeelFirst == true ) {
						if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
						    printerMode =  ContinuousLabelHeelCut;		
					    } else {
							printerMode =  ContinuousShootLabelHeelCut;
						}
					} else {
						printerMode =  ContinuousLabelHeadCut;				
					}
					takeLabelTime = ticksPerSecond / 2;
				}
				teachPrinter(printerMode);
				Debug::trace("PrinterManager.VirtualCombine", "handleModeMessage() OpModeProductionEntry: printerMode = " + Integer::toString(printerMode));
			} else {
				Debug::trace("PrinterManager.VirtualCombine", "handleModeMessage() OpModeProductionEntry: printerMode = " + Integer::toString(printerMode));
				//SOF-3993 non cutter scale
				if( ( Configuration::getInstance()->toBoolCfg( fieldOffsetCfgProductionEntryContinuousPrint ) ) ) {
					takeLabelTime = 0;
				}
			}
			break;

		}
		case OpModeAutoManualApply: {			
			if( printHeelFirst == true ) {
				if(isCutterInstalled && isCutterEnabled){
					if( !noCutOption ) { 
						if( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ) {
						    printerMode =  ContinuousLabelHeelCut;
						} else {
							printerMode =  ContinuousShootLabelHeelCut;
						}
						takeLabelTime = ticksPerSecond / 2;						
					} else {
						printerMode =  ContinuousLabelHeelCombined;
						takeLabelTime = 0;
					} 
					teachPrinter(printerMode);
					Debug::trace("PrinterManager.VirtualCombine", "handleModeMessage() OpModeAutoManualApply: printerMode = " + Integer::toString(printerMode));
				}
			} else {
				if(isCutterInstalled && isCutterEnabled){
					if( !noCutOption ) { 
						printerMode =  ContinuousLabelHeadCut;
						takeLabelTime = ticksPerSecond / 2;
					} else {
						printerMode =  ContinuousLabelHeadCombined;
						takeLabelTime = 0;
					}
					teachPrinter(printerMode);
					Debug::trace("PrinterManager.VirtualCombine", "handleModeMessage() OpModeAutoManualApply: printerMode = " + Integer::toString(printerMode));
				} else {
					//SOF-3993 non cutter scale
					if( !isCutterInstalled ) {
						if ( !Configuration::getInstance()->toBoolCfg(fieldOffsetCfgContinuousStockIgnoreLabelTaken) ) {
							takeLabelTime = ticksPerSecond / 2;													
						} else {
							takeLabelTime = 0;
						}
						Debug::trace("PrinterManager.VirtualCombine", "handleModeMessage() OpModeAutoManualApply takeLabelTime: " + Integer::toString(takeLabelTime));
					}
				}
			}			
			break;
		}
		default: {
			Debug::trace("PrinterManager", "Unknown event: " + Integer::toString(mEvent));  	
		}
	}
}

void PrinterManager::handleConfigMessage( ConfigurationEvent cEvent, const char* /*recvBuffer*/ )
{	
	switch( cEvent )
	{
		//check if any of our configuration values have changed.
		case CfgSaved: {			
			// Yes, the if and else if blocks below are essentially the same.  If you know why, put a comment in here and let everyone know.
			// 
			//only HTm Non Cutter scale
			if( ( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter) || ( prKind == SSKPCPrinter ) ) && !isCutterInstalled && !isCutterEnabled ) {
				if( ( Configuration::getInstance()->toBoolCfg( fieldOffsetCfgProductionEntryContinuousPrint )  ) && 
				    ( RealTimeController::getInstance()->getCurrentOpModeEvent() == OpModeProductionEntry ) ) { //SOF-3993
					takeLabelTime = 0;					
					Debug::trace("PrinterManager.TakeLabelTime", " handleConfigMessage: ProductionEntryContinuousPrint is enabled!"); 
				} else { //SOF-3990
					if ( !Configuration::getInstance()->toBoolCfg(fieldOffsetCfgContinuousStockIgnoreLabelTaken) ) {							
						takeLabelTime = ticksPerSecond / 2;					
						Debug::trace("PrinterManager.TakeLabelTime", " handleConfigMessage: ProductionEntryContinuousPrint is disabled!"); 
					} else {
						//SOF-3998 - TODO: CARLOS AND AARON WILL HAVE TO CHECK AND TEST THIS
						if( !isDieCutloaded() ) {	
							takeLabelTime = 0;
							Debug::trace("PrinterManager.TakeLabelTime", " handleConfigMessage: takeLabelTime 0!"); 
						} else {
							Debug::trace("PrinterManager.TakeLabelTime", " handleConfigMessage: takeLabelTime: " + Integer::toString(takeLabelTime)); 
							Debug::trace("PrinterManager.TakeLabelTime", " handleConfigMessage: using die cut labels.");
							takeLabelTime = ticksPerSecond / 2;	
						}
					}
				}
			} else if( ( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) ||  ( prKind == SSKPCPrinter ) ) && !isCutterInstalled && isCutterEnabled ) { //SOF-3980 db could have cutter enabled even when non cutter scale  
				if ((Configuration::getInstance()->toBoolCfg(fieldOffsetCfgProductionEntryContinuousPrint)) &&
					(RealTimeController::getInstance()->getCurrentOpModeEvent() == OpModeProductionEntry)) { //SOF-3993
					takeLabelTime = 0;
					Debug::trace("PrinterManager.TakeLabelTime", " handleConfigMessage A: ProductionEntryContinuousPrint is enabled!"); 
				} else { //SOF-3990
					if ( !Configuration::getInstance()->toBoolCfg(fieldOffsetCfgContinuousStockIgnoreLabelTaken) ) {								
						takeLabelTime = ticksPerSecond / 2;											
					} else {
						//SOF-3998 - TODO: CARLOS AND ARRON WILL HAVE TO CHECK AND TEST THIS
						if( !isDieCutloaded() ) {
							takeLabelTime = 0;							
						} else {
							takeLabelTime = ticksPerSecond / 2;
						}
					}
				}
			}
			break;
		}
	}
}

void PrinterManager::handleApplierMessage( LaEvent event, const char *recvBuffer )
{
	switch ( event )
	{
		case LaSetup: {			
			if( myPrinterType == FirstPrinter ) {
				LabelApplierSetup setup( recvBuffer );
				if( setup.getRotatePrimaryPrintImage() ) {
					//this will rotate the label image 180 degrees.
					Debug::trace("PrinterManager.LabelTransfer", "FirstPrinter LaSetup rotate label image 180 degrees.");
					imageFlip = true;
				} else {
					Debug::trace("PrinterManager.LabelTransfer", "FirstPrinter LaSetup calling heel first print.");
					imageFlip = false;
				}		
			}
			if( myPrinterType == SecondPrinter ) {
				LabelApplierSetup setup( recvBuffer );
				if( setup.getRotateSecondaryPrintImage() ) {
					//this will rotate the label image 180 degrees.
					Debug::trace("PrinterManager.LabelTransfer", "SecondPrinter LaSetup rotate label image 180 degrees.");
					imageFlip = true;
				} else {
					Debug::trace("PrinterManager.LabelTransfer", "SecondPrinter LaSetup calling heel first print.");
					imageFlip = false;
				}		
			} 
			break;
		}
		default: {
			break;
		}
	}
}

//SOF-5482
void PrinterManager::handleSessionDevMessage( SessionEvent e, const char *recvBuffer )
{
	switch ( e )
	{
    case SnKeyActivation:
		{
			const HfcAccessKeyActivation haka( recvBuffer );
			const AccessKeys accessKeys( haka.getKeys() );

			if ( accessKeys.isMFGTestKeyPresent() )
			{
				Debug::trace("PrinterManager", "Manufacturing key present");
				mfgKey_ = true;
			}
			else
			{
				Debug::trace("PrinterManager", "Manufacturing key removed");
				mfgKey_ = false;
			}

			break;
		}

	default:
		break;
	}
}

//SOF-3949
bool PrinterManager::isCombinedVirtualPrint()
{
	bool result = false;
	noCutOption = Configuration::getInstance()->toBoolCfg(fieldOffsetCfgVirtualCombinePrint);	
	if( ( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) || ( prKind == SSKPCPrinter ) ) && (noCutOption) && (isCutterInstalled) && (isCutterEnabled) ) 	
		result = true;
	return result;		
}


void PrinterManager::reimageRFIDVoidLabelType()
{
	/*
	if (!Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableRfidPrinting))
	{
		Debug::trace("PrinterManager.rfid", "RFID is NOT enabled, so no need for call to PrinterManager::reimageRFIDVoidLabelType()");
		return;
	}
	*/

	//Debug::trace("PrinterManager.rfid", "reimageRFIDVoidLabelType() - starting");

	/*  Commented out so we can reimage the void label, after sizing
	if (imageControlBlock[ExtraPrinter_UsedForRFIDVoidLabel])
	{
		Debug::trace("PrinterManager.rfid", "reimageRFIDVoidLabelType() - imageControlBlock[ExtraPrinter_UsedForRFIDVoidLabel] already set up: " + ptrtostring(imageControlBlock[ExtraPrinter_UsedForRFIDVoidLabel]));
		return;
	}
	*/

	LabelType* rfidVoidLabelType = pConfig->getRFIDVoidLabelType();
	//Debug::trace("PrinterManager.rfid", "reimageRFIDVoidLabelType() - label from getRFIDVoidLabelType: " + ptrtostring(rfidVoidLabelType));

	if (rfidVoidLabelType && rfidVoidLabelType->getErrorCode() == RECORD_OK)
	{
		rfidVoidLabelType->show();

		//if (rfidVoidLabelType->getLabelStockDescriptor() && rfidVoidLabelType->getLabelStockDescriptor()->getWidthDots() <= stockLoaded.getWidthDots())
			//Debug::trace("PrinterManager.rfid", "reimageRFIDVoidLabelType() - rfidVoidLabelType->getLabelStockDescriptor()->getWidthDots() <= stockLoaded.getWidthDots()");

		emptyIndirectGraphics();
		emptyEmbeddedLabelBitMaps();
		if (imageControlBlock[ExtraPrinter_UsedForRFIDVoidLabel])
		{
			delete imageControlBlock[ExtraPrinter_UsedForRFIDVoidLabel];
			imageControlBlock[ExtraPrinter_UsedForRFIDVoidLabel] = 0;
		}

		//Debug::trace("PrinterManager.rfid", "reimageRFIDVoidLabelType() - before preImageALabel()");
		//Debug::trace("PrinterManager.rfid", printerTraceString + " renderingRFIDVoidLabelType set to true");
		renderingRFIDVoidLabelType = true;
		printerStatePriorToRenderingRFIDVoidLabelType = printerState;
		preImageALabel(rfidVoidLabelType, ExtraPrinter_UsedForRFIDVoidLabel);

		//Debug::trace("PrinterManager.rfid", "reimageRFIDVoidLabelType() - imageControlBlock[ExtraPrinter_UsedForRFIDVoidLabel] set up: " + ptrtostring(imageControlBlock[ExtraPrinter_UsedForRFIDVoidLabel]));
	}
}

bool PrinterManager::checkIfRfidTagWritten(PrinterType labelNumToPrint)
{
	const bool tracing(Debug::isTracing("PrinterManager.rfid"));

	cancelTimer(rfidQryTimer);
	
	// If RFID printing is not enabled, ignore errors and pretend the operation was successful
	if (!pConfig->toBoolCfg(fieldOffsetCfgEnableRfidPrinting) &&
		pConfig->toBoolCfg(fieldOffsetCfgIgnoreRFIDWhenHardwarePresentButRFIDIsDisabled)) {
		if (tracing)
			Debug::trace("PrinterManager.rfid", "RFID is NOT enabled, so ignoring return code from writing the tag");
		return true;
	}

	if (!pCurrentProductTransaction)
	{
		if (tracing)
			Debug::trace("PrinterManager.rfid", "No pCurrentProductTransaction, so no tag write/early return");

		return true;
	}
		

	pCurrentProductTransaction->setOperatorRFID("");
	pCurrentProductTransaction->setRfidVoidLabelPrinted(false);

	if (labelNumToPrint >= MaxPrinterTypes)
	{
		if (tracing)
			Debug::trace("PrinterManager.rfid", string("labelNumToPrint passed in, ") + inttostring(labelNumToPrint) + ", doesn't need a RFID tag, so no tag needs to be written");
		return true;
	}
	if (!pCurrentProductTransaction || !pCurrentProductTransaction->getProduct())
	{
		if (tracing)
			Debug::trace("PrinterManager.rfid", "No pCurrentProductTransaction or no Product, so no tag needs to be written");
		return true;
	}

	HobString sellByDate;

	if ((pCurrentProduct->getLabelBeingPrinted() == Product::PRINTING_NEW_EXPIRATION_LABEL) ||
		(pCurrentProduct->getLabelBeingPrinted() == Product::PRINTING_EXISTING_EXPIRATION_LABEL))
	{
		sellByDate = (pCurrentProductTransaction->getExpirationLabelDateTime()).formatDate(FormatDate_YYMMDD_NoDelimiter);
		pCurrentProductTransaction->setTotalPrice(0);
		pCurrentProductTransaction->buildBarcodeStrings();

		Debug::trace("PrinterManager.rfid", "RFID Expiration label - creating new barcode data to send to the apphook");
	}
	else
	{
		sellByDate = (pCurrentProductTransaction->getShelfLife()).formatDate(FormatDate_YYMMDD_NoDelimiter);
		if (tracing)
			Debug::trace("PrinterManager.rfid", string("sellByDate: ") + sellByDate);
	}

	LabelType* labelType = pCurrentProductTransaction->getProduct()->getLabelType(labelNumToPrint);
	BarCodeNumber bcn(pCurrentProductTransaction->getBarCodeNum());
	BarcodeType bcType = static_cast<BarcodeType>(bcn.getBarcodeType());
	HobString barcodeString = bcn.getBarcodeString(static_cast<BarcodeType>(bcn.getBarcodeType()));

	if (tracing)
		Debug::trace("PrinterManager.rfid", "Barcode string: " + barcodeString);

	if (!barcodeString.size())
	{
		if (tracing)
			Debug::trace("PrinterManager.rfid", "No barcodeString for pCurrentProductTransaction, so no tag needs to be written");
		return true;
	}

	string base("0000000000");
	string storeIdStr;
	string scaleIdStr;
	string transIdStr;
	string transLabelNumStr;
	string rfidProductType = "EPP";

	Debug::trace("PrinterManager.rfid", "debug placeholder: ");

	if (Configuration::getInstance()->toStringCfg(fieldOffsetCfgRfidEncodeMethod) == "1")
	{
		storeIdStr = base + Configuration::getInstance()->toStringCfg(fieldOffsetCfgStoreId);
		storeIdStr = storeIdStr.substr(storeIdStr.length() - 4);

		scaleIdStr = base + Configuration::getInstance()->toStringCfg(fieldOffsetCfgAccumItemsScaleId);
		scaleIdStr = scaleIdStr.substr(scaleIdStr.length() - 1);

		transIdStr = base + longtostring(pCurrentProductTransaction->getTransactionId());
		transIdStr = transIdStr.substr(transIdStr.length() - 4);

		transLabelNumStr = base + inttostring(pCurrentProductTransaction->getNumOfLabelsPrinted());
		transLabelNumStr = transLabelNumStr.substr(transLabelNumStr.length() - 2);
	}
	else
	{
		storeIdStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgStoreId);
		scaleIdStr = Configuration::getInstance()->toStringCfg(fieldOffsetCfgAccumItemsScaleId);
		transIdStr = longtostring(pCurrentProductTransaction->getTransactionId());
		transLabelNumStr = inttostring(pCurrentProductTransaction->getNumOfLabelsPrinted());
	}

	// This is to prevent issues when calculating the EPC
	if (transIdStr == "-1") {
		transIdStr = "0";
	}

	/*
	if (tracing)
		Debug::trace("PrinterManager.rfid", string("Calling generateEpcRfid with: ") + storeIdStr + " , " + scaleIdStr + " , " + transIdStr + " , " + transLabelNumStr);
	string serialNumString( storeIdStr + scaleIdStr + transIdStr + transLabelNumStr );
	*/

	//SOF-6963 - QR Codes fail printer - We no longer need this code anymore becuase it is done in the hook
	// Plus this code prevents printing anything besides UPC and maybe EANs but nothing 2D, no QR, etc.
	/*
	string epcRfidString;
	//epcRfidString = generateEpcRfid( barcodeString, serialNumString );

	unsigned char epcRfidBuffer[12];
	generateEpcRfid( barcodeString, serialNumString, epcRfidBuffer);

	for (int i=0; i < 12; i++)
		epcRfidString.append( Hex::toString(epcRfidBuffer[i] ) );

	if (tracing)
		Debug::trace("PrinterManager.rfid", string("Result from generateEpcRfid: ") + epcRfidString);
	*/

	// Modify the action string to include barcodeString, sell by date, store ID, scale ID, transaction ID, and the transaction label number
	HobString action("EPC|" + Configuration::getInstance()->toStringCfg(fieldOffsetCfgRfidEncodeMethod) + "|" + barcodeString + "|" + sellByDate + "|" + storeIdStr + "|" + scaleIdStr + "|" + transIdStr + "|" + transLabelNumStr + "|" + rfidProductType);
	if (tracing)
		Debug::trace("PrinterManager.rfid", "Calling hobart_cust_hook_custom_action to write rfid tag - action: " + action);

	//const char *asdf = action.c_str();

	int resultCode = 42;  // just initialize to some value (that won't be returned)
	char rcexBuffer[256] = { 0 };

#if defined(__linux__)
	if (bcType == UPCA || bcType == EAN13) {
		if (tracing) {
			Debug::trace("PrinterManager.rfid", "UPCA/EAN13 barcode detected so calling custom action");
		}
		
		// The reason for this is because currently the apphook only supports UPCA barcodes since that is all Walmart currently uses for RFID		
		hobart_cust_hook_custom_action(action.c_str(), &resultCode, rcexBuffer, sizeof(rcexBuffer));
	}
	else {
		if (tracing)
			Debug::trace("PrinterManager.rfid", "NonUPCA barcode detected so skipping custom action");
		return true;
	}
#else
	resultCode = 0;
#endif

	createTimer(rfidQryTime, rfidQryTimer, true);

	if (tracing)
		Debug::trace("PrinterManager.rfid", string("Result Code of writing to rfid tag: ") + std::to_string(resultCode) + " - RFID tag" + ((resultCode) ? " NOT" : "") + " written successfully");

	// Only save the EPC tag to the transaction record if the tag was written successfully.
	// Handle RFID tag writing result
	if (resultCode == 0) {
		// RFID tag writing successful, set the OperatorRFID to the value from rcexBuffer
		pCurrentProductTransaction->setOperatorRFID(rcexBuffer);
	}
	else if (resultCode != -1) {
		// RFID tag writing failed, and it's not a unrecognized custom action
		if (pConfig->toBoolCfg(fieldOffsetCfgEnableRfidPrinting)) {
			pCurrentProductTransaction->setRfidVoidLabelPrinted(true);
			RFIDLabelCount_++;
			if (resultCode == 3) {
				UIDialogMsg dialog(this, ProductDev, ProductErrRFIDWriteFail, HfcResponse::RESPONSESTYLE_OK, false);
				dispatchEvent(&dialog);
				dialog.show();
				Debug::trace("PrinterManager.rfid", "RFID tag writing failed with error code " + std::to_string(resultCode) + ": RFID write failure");
			}
			else {
				// The board error dialog was sometimes getting displayed from other errors that are not necessarily a board error so we will only display the basic error for now
				UIDialogMsg dialog(this, ProductDev, ProductErrRFIDWriteFail, HfcResponse::RESPONSESTYLE_OK, false);
				//UIDialogMsg dialog(this, ProductDev, ProductErrRFIDWriteFail, HfcResponse::RESPONSESTYLE_OK, false);
				dispatchEvent(&dialog);
				dialog.show();
				Debug::trace("PrinterManager.rfid", "RFID tag writing failed with error code " + std::to_string(resultCode) + ": RFID write failure");
			}
		}
	}

	// If RFID printing is not enabled, ignore errors and pretend the operation was successful
	if (!pConfig->toBoolCfg(fieldOffsetCfgEnableRfidPrinting)) {
		if (tracing)
			Debug::trace("PrinterManager.rfid", "RFID is NOT enabled, so ignoring return code from writing the tag");
		return true;
	}

	return (resultCode == 0);
}


void PrinterManager::startPrintingLabel( PrinterType labelNumToPrint )
{
	const bool tracing( Debug::isTracing( "PrinterManager.PrinterImaging" ) );

	// Download and print the label here!!
	if ( !pMyPrinter )
		return;

	ImageControl *ic = 0;

	bool rfidCheck = true;

	if ( myPrinterType == FirstPrinter )
	{
		Debug::trace("PrinterManager.rfid", "checkIfRfidTagWritten called on firstPrinter");

		rfidCheck = checkIfRfidTagWritten(labelNumToPrint);
	}
	else
	{
		Debug::trace("PrinterManager.rfid", "checkIfRfidTagWritten SKIPPED on !firstPrinter");
	}

	if (rfidCheck == false)
	{
		Debug::trace("PrinterManager.rfid", "RFID tag not written successfully; printing VOID label");

		labelNumToPrint = ExtraPrinter_UsedForRFIDVoidLabel;
		// Initial try of printing the VoidLabel known to the PrinterManager; it was working, but printing garbage
		//testLabelType = PrVoidLabel;
		//sendTestLabel = true;
		//printTestLabel();
	}
	else
	{
		Debug::trace("PrinterManager.rfid", "RFID tag written successfully or is disabled; printing regular label");
	}

	ic = imageControlBlock[labelNumToPrint];

	if (tracing)
		Debug::trace("PrinterManager.PrinterImaging", "startPrintingLabel() - imageControlBlock used is: " + ptrtostring(ic));

	if (!ic)
	{
		if (tracing)
			Debug::trace("PrinterManager.PrinterImaging", "startPrintingLabel() - imageControlBlock is NULL for printerNum: " + inttostring(labelNumToPrint));
		return;
	}

	// Flush the printer
	sendPrinterEnableTakeup();
	sendPrinterFlush();

	if ( ic->getPrintingRotatedBitmap() )
		labelDataSize = ic->getRotatedBitMapSize();
	else
		labelDataSize = ic->getImageBitMapSize();

	linesToPrint = labelDataSize / (printheadSize / 8);
	if (tracing)
	{
		Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " labelDataSize: " + HobString(Integer::toString(labelDataSize)));
		Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " linesToPrint: " + HobString(Integer::toString(linesToPrint)));
	}

	if (printerStockMetrics.isContinuousStock() && ic->isImageOptionsScrunch())
	{
		UINT16 verticalOffset = ic->getOffsetFromTable((INT16)linesToPrint);

		if (verticalOffset)
		{
			linesToPrint -= verticalOffset;
			labelDataSize = linesToPrint * (printheadSize / 8);
			if (tracing)
			{
				Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " corrected labelDataSize: " + HobString(Integer::toString(labelDataSize)));
				Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " corrected linesToPrint: " + HobString(Integer::toString(linesToPrint)));
			}
		}
	}

	//KPC needs to know the label size before we start printing.
	if( ( prKind == KPCPrinter ) || ( prKind == SSKPCPrinter ) ) {
		PrinterLabelImageSize( this, myPrinterType, labelDataSize );
	}

	printerState = PrinterReadyToXfer;
	Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterReadyToXfer");

	xferLabelStart = PAL_absclass::PAL_getTickCount();

	if ( ic->getPrintingRotatedBitmap() )
		pDestinationData = rotatedBitMap[labelNumToPrint]->getBitMapBase();
	else
		pDestinationData = bitMap[labelNumToPrint]->getBitMapBase();

	if ( printHeelFirst )
	{
		if (tracing)
			Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " print heel first");
		labelData = (UINT16 *) ( (long long) pDestinationData + (long long) labelDataSize);
	}
	else
	{
		if (tracing)
			Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " print normal");
		labelData = pDestinationData;
	}

	// NGW-261
	// for wrapper we need to enforce a margin at the top of the label, otherwise they need to adjust
	// print position as the label roll empties
	if ( Configuration::getInstance()->isPrepackScale() )
		if (( !printerStockMetrics.isContinuousStock() ) && ( !ic->getPrintingRotatedBitmap() ))
		{
			unsigned char *buffer( new unsigned char[labelDataSize] );

			if (tracing)
				Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " adjusting label margins");

			// keep track of buffer
			const size_t index( labelBufferIndex_ % NUM_LABEL_BUFFERS );

			if ( labelBuffer_[index] )
				delete labelBuffer_[index];

			labelBuffer_[index] = buffer;
			++labelBufferIndex_;

			// copy and adjust the label buffer
			std::memcpy( buffer, pDestinationData, labelDataSize );
			//SOF-4628 Do not adjust margins if we are flipping the label image for a two printer system.
			//image flipping only occurs on two printer systems.
			if( !imageFlip ) { 
			    adjustMargins( ic, buffer );
			}
			// set label data
			labelData = (UINT16*) &buffer[printHeelFirst ? labelDataSize : 0];
			Debug::trace("PrinterManager.LabelTransfer", "Printer: " + Integer::toString( myPrinterType ) + "pointer labelData:" + Hex::toString((long long)labelData));
			Debug::trace("PrinterManager.LabelTransfer", "Printer: " + Integer::toString( myPrinterType ) + "buffer base:" + Hex::toString((long long)&buffer[0]));
		}

#ifdef FAKE_SOCKET_WEIGHER

	if (agentAttached_)
	{
		char * pBuff = new char[labelDataSize];
		memcpy(pBuff, pDestinationData, labelDataSize);
		sendRequestVirtualLabel(labelDataSize, (UINT16 *)pBuff, linesToPrint);
	}
	else
	{
		Debug::trace("PrinterManager.PrinterImaging", "==> " + printerTraceString + " send label data <==");
		sendLabelData();
	}
#else
	if (tracing)
		Debug::trace("PrinterManager.PrinterImaging", "==> " + printerTraceString + " send label data <==");
	sendLabelData();
#endif
}

// sort whitespace from biggest to smallest preferring lower positions over higher ones
bool compare_whitespace_length( const std::pair<size_t, size_t>& first, const std::pair<size_t, size_t>& second )
{
	if ( second.second == first.second )
		return (second.first < first.first);

	return (second.second < first.second);
}

// remove whitespace that is too small
bool whitespace_is_small( const std::pair<size_t, size_t>& obj )
{
	static const size_t minWhitespaceBetweenLines( 4 );

	return (obj.second <= minWhitespaceBetweenLines);
}

void PrinterManager::adjustMargins( const ImageControl* ic, unsigned char *buffer )
{
	static const size_t marginRequired( 40 );
	static const size_t minWhitespaceBetweenLines( 4 );

	if ( !ic )
		return;
	else if ( ic->getImageLines() <= marginRequired )
		return;
	else if ( !buffer )
		return;

	Debug::trace( "PrinterManager.margins", "checking label margins:" );
	Debug::trace( "PrinterManager.margins", "  required margin " + util_string::inttostring( marginRequired ) );
	Debug::trace( "PrinterManager.margins", "     label length " + util_string::inttostring( ic->getImageBitMapHeight() ) );

	BitMap *bitmap( ic->getImageBitMap() );

	if ( !bitmap )
		return;
	else if ( bitmap->getBitMapBaseSize() <= 0 )
		return;

	// pitch size is number of uint16 per line; multiply by 2 for number of byte per line
	const size_t bitmapPitch( 2 * bitmap->getBitMapPitch() );
	const size_t bitmapLength( ic->getImageBitMapHeight() );

	if ( bitmapPitch <= 0 )
		return;

	Debug::trace( "PrinterManager.margins", "      bitmap size " + util_string::inttostring( bitmap->getBitMapBaseSize() ) );
	Debug::trace( "PrinterManager.margins", "     bitmap pitch " + util_string::inttostring( bitmapPitch ) );

	// ------------------------------------------------------------------------
	// determine what lines are used
	// ------------------------------------------------------------------------

	std::vector<bool> used;
	used.resize( bitmapLength, false );

	for ( size_t y( bitmapLength ); y--; )
		for ( size_t x( bitmapPitch ); x--; )
			if ( buffer[bitmapPitch*y + x] )
			{
				used[y] = true;
				break;
			}

	// ------------------------------------------------------------------------
	// generate list of whitespace locations
	// ------------------------------------------------------------------------

	typedef std::pair<size_t, size_t> white; // y, len
	typedef std::list<white> whitespaceList;
	whitespaceList whitespace;

	bool within( false );

	for ( size_t y( 0 ); y < bitmapLength; ++y )
	{
		if ( used[y] )
			within = false;
		else
		{
			if ( within )
				++whitespace.back().second;
			else
			{
				whitespace.push_back( white( y, 1 ) );
				within = true;
			}
		}
	}

	// determine how much actual margin exists
	size_t topMargin( 0 );

	size_t bottomMargin( 0 );
	white bottomMarginWhite;

	if ( 0 == whitespace.front().first )
	{
		topMargin = whitespace.front().second;

		// remove the margin from list, its no longer needed after this
		whitespace.pop_front();
	}

	if ( used.size() == (whitespace.back().first + whitespace.back().second) )
	{
		bottomMargin = whitespace.back().second;

		bottomMarginWhite = whitespace.back();
		bottomMarginWhite.second += minWhitespaceBetweenLines;

		whitespace.pop_back();
	}

	Debug::trace( "PrinterManager.margins", "       top margin " + util_string::inttostring( topMargin ) );
	Debug::trace( "PrinterManager.margins", "    bottom margin " + util_string::inttostring( bottomMargin ) );

	// margin okay, nothing to do
	if ( marginRequired <= topMargin )
		return;

	size_t adjustmentLength( marginRequired - topMargin );
	//SOF-4592
	if( adjustmentLength ) {	
		//if we are going to flip the image then we need to remove the top margin otherwise the bottom get clipped!
		adjTopMargin = adjustmentLength;
		Debug::trace( "PrinterManager.margins", "adjTopMargin: " + util_string::inttostring( adjTopMargin ) );
	}
	Debug::trace( "PrinterManager.margins", "need to adjust " + util_string::inttostring( adjustmentLength ) + " lines" );

	// ------------------------------------------------------------------------
	// generate list of whitespace to use
	// we prefer larger sections of whitespace over the smaller ones
	// ------------------------------------------------------------------------

	// sort list
	whitespace.remove_if( whitespace_is_small );
	whitespace.sort( compare_whitespace_length );

	// add bottom margin back into list so we use it first
	if ( bottomMargin )
		whitespace.push_front( bottomMarginWhite );

	if ( Debug::isTracing( "PrinterManager.margins" ) )
	{
		Debug::trace( "PrinterManager.margins", "sorted whitespace locations:" );

		for ( whitespaceList::const_iterator w( whitespace.begin() ); w != whitespace.end(); ++w )
			Debug::trace( "PrinterManager.margins", "  " + util_string::inttostring( w->first ) + ": " + util_string::inttostring( w->second ) );
	}

	// ------------------------------------------------------------------------
	// compress the label and remove any whitespace remaining
	// use up the bottom margin first
	// ------------------------------------------------------------------------

	for ( whitespaceList::const_iterator w( whitespace.begin() ); w != whitespace.end(); ++w )
	{
#if defined(PAL_WIN32_RUN)
		const size_t offset( min( adjustmentLength, w->second - minWhitespaceBetweenLines ) );
#else
		const size_t offset(std::min(adjustmentLength, w->second - minWhitespaceBetweenLines));
#endif

		Debug::trace( "PrinterManager.margins", "using " + util_string::inttostring( offset ) + " lines at offset " + util_string::inttostring( w->first ));

		// move bitmap down into whitespace
		std::memmove( &buffer[bitmapPitch*offset], buffer, bitmapPitch * w->first );
		std::memset( buffer, 0, bitmapPitch*offset );

		// update our whitespace
		for ( whitespaceList::iterator w_( whitespace.begin() ); w_ != whitespace.end(); ++w_ )
			if ( w_->first < w->first )
				w_->first += offset;

		adjustmentLength -= offset;

		// we had enough room and are done
		if ( !adjustmentLength )
			return;
	}

	Debug::trace( "PrinterManager.margins", "could not adjust " + util_string::inttostring( adjustmentLength ) + " lines!" );
}

void PrinterManager::emptyIndirectGraphics()
{
	{
		graphicsContainerIterator pos = indirectGraphics.begin();

		while (pos != indirectGraphics.end())
		{
			Debug::trace("PrinterManager.IndirectGraphic", printerTraceString + " deleting Graphic -- " + HobString(pos->first) + " : 0x" + HobString(Hex::toString((long long) pos->second)));
			delete pos->second;
			pos++;
		}

		indirectGraphics.clear();
	}

	{
		bitmapsContainerIterator pos = indirectBitmaps.begin();

		while (pos != indirectBitmaps.end())
		{
			Debug::trace("PrinterManager.IndirectGraphic", printerTraceString + " deleting BitMap -- " + HobString(pos->first) + " : 0x" + HobString(Hex::toString((long long) pos->second)));
			delete pos->second;
			pos++;
		}

		indirectBitmaps.clear();
	}
}


void PrinterManager::emptyEmbeddedLabelBitMaps()
{
	labelBitMapsIterator pos = embeddedLabelBitMaps_.begin();
	while ( pos != embeddedLabelBitMaps_.end() )
	{
		Debug::trace("PrinterManager.IndirectGraphic", printerTraceString + " deleting BitMap -- " +
				" : 0x" + HobString(Hex::toString((long long) *pos)));

		delete *pos;

		++pos;
	}

	embeddedLabelBitMaps_.clear();
}


// The parameter, ignoreLoadedLabelStock, is defaulted to false, so that we check to see that the label would fit on the label stock.
// It is passed in as true when we want to render a label to a file, and don't care what the label stock is, since we won't be printing to it anyway.
void PrinterManager::preImageLabel()
{
	Debug::trace("PrinterManager.PrinterImaging", " preImageLabel() pCurrentProduct: 0x" + HobString(Hex::toString((long long) pCurrentProduct)));

	emptyIndirectGraphics();
	emptyEmbeddedLabelBitMaps();
	bool preImageError = false;

	for (int i = 0; i < (short)MaxPrinterTypes; i++)
	{
		// free the bitmap area if there is any!!
		// this method will not fail if there isn't any memory allocated!!
		Debug::trace("PrinterManager.PrinterImaging", " Calling BitMap::freeBitMap()");
		deleteBitMap( bitMap[i] );
		bitMap[i] = new BitMap;

		// SOF-5585
		Debug::trace("PrinterManager.PrinterImaging", " Going to delete imageControlBlock " + ptrtostring(imageControlBlock[i]));
		if ( imageControlBlock[i] )
		{
			deleteImageControl( imageControlBlock[i] );
			imageControlBlock[i] = 0;
		}
	}

	if (pCurrentProduct != NULL)
	{
		if( isCutterInstalled && isCutterEnabled ) {
			if( ( RealTimeController::getInstance()->getCurrentOpModeEvent() == OpModeManualWeigh ) || 
				( RealTimeController::getInstance()->getCurrentOpModeEvent() == OpModeAutoManualApply ) ||
				( RealTimeController::getInstance()->getCurrentOpModeEvent() == OpModeProductionEntry ) ||
				( RealTimeController::getInstance()->getCurrentOpModeEvent() == OpModeAccumulateItems ) ||
				( RealTimeController::getInstance()->getCurrentOpModeEvent() == OpModeBuildAMeal ) || 
				( RealTimeController::getInstance()->getCurrentOpModeEvent() == OpModeSelfService ) ) {
					pCurrentProduct->reverseLabelOrder();
				}
		}
		bool moreLabelsToPreImage = false;
		PrinterType labelNumToPreImage = myPrinterType;
		if (virtualPrintingEnabled)
		{
			moreLabelsToPreImage = true;
			labelNumToPreImage = FirstPrinter;
		}

		do
		{
			pCurrentLabel = pCurrentProduct->getLabelType((short)labelNumToPreImage);

			if (pCurrentLabel &&
					pCurrentLabel->getErrorCode() == RECORD_OK &&
					( !pCurrentProduct->isPrintLabelToPrinter() || isExternalLabelPrinterManager() /* PJR-444 - SOF-5063 - Print wider labels on Zebra printer */ ||
						( pCurrentLabel->getLabelStockDescriptor() && pCurrentLabel->getLabelStockDescriptor()->getWidthDots() <= stockLoaded.getWidthDots() ) ) )
			{
				Debug::trace("PrinterManager.PrinterImaging", "labelNumToPreImage: " + Integer::toString(labelNumToPreImage));
				preImageError = preImageALabel(pCurrentLabel, labelNumToPreImage);
			}
			else
			{
				if (pCurrentLabel == NULL)
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " preImageLabel() -- pCurrentLabel == NULL");
				else if (pCurrentLabel->getErrorCode() != RECORD_OK)
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " preImageLabel() -- pCurrentLabel->getErrorCode() != RECORD_OK");
				else if (pCurrentLabel->getLabelStockDescriptor() == 0)
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " preImageLabel() -- pCurrentLabel->getLabelStockDescriptor() == NULL");
				else if (pCurrentLabel->getLabelStockDescriptor()->getWidthDots() > stockLoaded.getWidthDots())
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " preImageLabel() -- Label Width > Stock Width (" +
							HobString(Integer::toString(pCurrentLabel->getLabelStockDescriptor()->getWidthDots())) +
							" > " + HobString(Integer::toString(stockLoaded.getWidthDots())) + ")");
			}

			if (moreLabelsToPreImage)
			{
				while ( labelNumToPreImage < MaxPrinterTypes )
				{
					short i = (short)labelNumToPreImage;
					i++;
					labelNumToPreImage = (PrinterType)i;
					if ( labelNumToPreImage >= MaxPrinterTypes )
						moreLabelsToPreImage = false;
					else if ( isLabelToBePrinted(labelNumToPreImage) )
						break;
				}
			}

		} while (moreLabelsToPreImage && !preImageError);
	}
	else
	{
		Debug::trace("PrinterManager.Error", printerTraceString + " preImageLabel() pCurrentProduct = NULL printerState: " + HobString(Integer::toString(printerState)));
	}

	if (preImageError)
	{
		// todo - Do we need to delete any labels that pre-imaged successfully?  For example, if label type 1 pre-images, but
		// label type 2 fails, do we need to delete any memory allocated during the pre-image of label type 1?

		if (((printerState == PrinterTryingToSize) ||
					(printerState == PrinterHeadDisengaged) ||
					(printerState == PrinterUnsized) ||
					(printerState == PrinterFinding) ||
					(printerResetting == true) ||
					(printerCalibrating == true)) == false)
		{
			printerState = PrinterReadyToPreImage;
			Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterReadyToPreImage 4");
		}
	}
}

// Windux IR #82 
void PrinterManager::CopyLabelStockDescriptor( PrinterType whichPrinter )
{
	// Windux IR #82		
	if (whichPrinter == FirstPrinter)
	{
		printer1Descriptor.setLength(stockLoaded.getLength());
		printer1Descriptor.setOriginalWidthDots(stockLoaded.getOriginalWidthDots());
		printer1Descriptor.setWidthDots(stockLoaded.getWidthDots());
		printer1Descriptor.setWidth(stockLoaded.getWidth());
		printer1Descriptor.setLabelOrientation(stockLoaded.getLabelOrientation());
		printer1Descriptor.setLabelOrientationDegrees(stockLoaded.getLabelOrientationDegrees());
		printer1Descriptor.setSkew(stockLoaded.getSkew());
	}
	else if (whichPrinter == SecondPrinter)
	{
		printer2Descriptor.setLength(stockLoaded.getLength());
		printer2Descriptor.setOriginalWidthDots(stockLoaded.getOriginalWidthDots());
		printer2Descriptor.setWidthDots(stockLoaded.getWidthDots());
		printer2Descriptor.setWidth(stockLoaded.getWidth());
		printer2Descriptor.setLabelOrientation(stockLoaded.getLabelOrientation());
		printer2Descriptor.setLabelOrientationDegrees(stockLoaded.getLabelOrientationDegrees());
		printer2Descriptor.setSkew(stockLoaded.getSkew());
	}
	else if (whichPrinter == ThirdPrinter)
	{
		printer3Descriptor.setLength(stockLoaded.getLength());
		printer3Descriptor.setOriginalWidthDots(stockLoaded.getOriginalWidthDots());
		printer3Descriptor.setWidthDots(stockLoaded.getWidthDots());
		printer3Descriptor.setWidth(stockLoaded.getWidth());
		printer3Descriptor.setLabelOrientation(stockLoaded.getLabelOrientation());
		printer3Descriptor.setLabelOrientationDegrees(stockLoaded.getLabelOrientationDegrees());
		printer3Descriptor.setSkew(stockLoaded.getSkew());
	}
	else if (whichPrinter == ExtraPrinter_UsedForSelfServeScreenBarcodeLabel)
	{
		printer4Descriptor.setLength(stockLoaded.getLength());
		printer4Descriptor.setOriginalWidthDots(stockLoaded.getOriginalWidthDots());
		printer4Descriptor.setWidthDots(stockLoaded.getWidthDots());
		printer4Descriptor.setWidth(stockLoaded.getWidth());
		printer4Descriptor.setLabelOrientation(stockLoaded.getLabelOrientation());
		printer4Descriptor.setLabelOrientationDegrees(stockLoaded.getLabelOrientationDegrees());
		printer4Descriptor.setSkew(stockLoaded.getSkew());
	}
	else if (whichPrinter == ExtraPrinter_UsedForRFIDVoidLabel)
	{
		printer5Descriptor.setLength(stockLoaded.getLength());
		printer5Descriptor.setOriginalWidthDots(stockLoaded.getOriginalWidthDots());
		printer5Descriptor.setWidthDots(stockLoaded.getWidthDots());
		printer5Descriptor.setWidth(stockLoaded.getWidth());
		printer5Descriptor.setLabelOrientation(stockLoaded.getLabelOrientation());
		printer5Descriptor.setLabelOrientationDegrees(stockLoaded.getLabelOrientationDegrees());
		printer5Descriptor.setSkew(stockLoaded.getSkew());
	}
}

bool PrinterManager::preImageALabel(LabelType *pCurrentLabel, PrinterType whichPrinter)
{
	bool isTracing = Debug::isTracing("PrinterManager.PrinterImaging");
	if (isTracing)
	{
		Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " preImageALabel() pCurrentProduct: 0x" + HobString(Hex::toString((long long)pCurrentProduct)) + "  whichPrinter: " + HobString(Integer::toString(whichPrinter)));
		Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " stockLoaded.show() 1");
		stockLoaded.show();
	}

	bool preImageError = true;

	UINT16 printheadSizeToUse = printheadSize;

	if (pCurrentLabel)
	{
		stockLoaded.setLabelOrientation(pCurrentLabel->getLabelStockDescriptor()->getLabelOrientation());
		stockLoaded.setLabelOrientationDegrees(pCurrentLabel->getLabelStockDescriptor()->getLabelOrientationDegrees());

		LabelStockDescriptor savedStockDescriptor;
		savedStockDescriptor.setLength(stockLoaded.getLength());
		savedStockDescriptor.setOriginalWidthDots(stockLoaded.getOriginalWidthDots());
		savedStockDescriptor.setWidthDots(stockLoaded.getWidthDots());
		savedStockDescriptor.setWidth(stockLoaded.getWidth());
		savedStockDescriptor.setLabelOrientation(stockLoaded.getLabelOrientation());
		savedStockDescriptor.setLabelOrientationDegrees(stockLoaded.getLabelOrientationDegrees());
		savedStockDescriptor.setSkew(stockLoaded.getSkew());

		bool printToScreen = false;
		if ( whichPrinter == ExtraPrinter_UsedForSelfServeScreenBarcodeLabel || (pCurrentProduct && !pCurrentProduct->isPrintLabelToPrinter()) )
			printToScreen = true;

		bool continuousStock = printerStockMetrics.isContinuousStock();  // || printToScreen;
		
		if (printToScreen || isExternalLabelPrinterManager() /* PJR-444 - SOF-5063 - Print wider labels on Zebra printer */)
		{
			//stockLoaded.setWidthDots(WideLabelWidth);
			//stockLoaded.setWidth(WideLabel);
			//stockLoaded.setWidthDots(UFWLabelWidth);
			//stockLoaded.setWidth(UFWLabel);

			// SOF-2813 - If the label they want to render for the screen (currently an option for self-serve mode) is a "Ultima Wide" label (3"), but we're on a service counter
			// scale with a wide printhead (2.25"), we need to temporarily use the EPP printhead size to correctly render the 3" wide label (used for the 'pitch').
			// And this has to be 640, not the UFWLabelWidth number of dots which is 609.
			printheadSizeToUse = ( pCurrentLabel->getLabelStockDescriptor()->getWidth() == UFWLabel ) ? UFWPrintHeadSize : pCurrentLabel->getLabelStockDescriptor()->getWidthDots();

			stockLoaded.setLength(pCurrentLabel->getLabelStockDescriptor()->getLength());
			stockLoaded.setOriginalWidthDots(pCurrentLabel->getLabelStockDescriptor()->getOriginalWidthDots());
			stockLoaded.setWidthDots(pCurrentLabel->getLabelStockDescriptor()->getWidthDots());
			stockLoaded.setWidth(pCurrentLabel->getLabelStockDescriptor()->getWidth());
			stockLoaded.setLabelOrientation(PortraitLabel);
			stockLoaded.setLabelOrientationDegrees(0);
			stockLoaded.setSkew(0);
		}

		//Windux IR #82 -> for multiple labels being printed, we were sending in the same LabelStockDescriptor
		if (isTracing)
		{
			Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " before CopyLabelStockDescriptor - stockLoaded.show() 2");
			stockLoaded.show();
		}

		CopyLabelStockDescriptor(whichPrinter);

		if (isTracing)
		{
			Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " after CopyLabelStockDescriptor - printer1Descriptor.show() 3");
			printer1Descriptor.show();
		}

		if (whichPrinter == FirstPrinter)
			imageControlBlock[whichPrinter] = new ImageControl(pCurrentLabel->getFormsList(), &printer1Descriptor);
		else if (whichPrinter == SecondPrinter)
			imageControlBlock[whichPrinter] = new ImageControl(pCurrentLabel->getFormsList(), &printer2Descriptor);
		else if (whichPrinter == ThirdPrinter)
			imageControlBlock[whichPrinter] = new ImageControl(pCurrentLabel->getFormsList(), &printer3Descriptor);		
		else if (whichPrinter == ExtraPrinter_UsedForSelfServeScreenBarcodeLabel)
			imageControlBlock[whichPrinter] = new ImageControl(pCurrentLabel->getFormsList(), &printer4Descriptor);
		else if (whichPrinter == ExtraPrinter_UsedForRFIDVoidLabel)
			imageControlBlock[whichPrinter] = new ImageControl(pCurrentLabel->getFormsList(), &printer5Descriptor);

		if (isTracing)
			Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " Created new imageControlBlock " + ptrtostring(imageControlBlock[whichPrinter]));

		bool imgOk = false;


		if ((stockLoaded.getLabelOrientationDegrees() == 0) ||
				(stockLoaded.getLabelOrientationDegrees() == 180))
		{
			UINT32 newSize = 0;

			if (continuousStock)
			{
				newSize = pCurrentLabel->getLabelStockDescriptor()->getLength();
				if (isTracing)
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString +  " (continuous stock)  bitmap bounds  right: " + HobString(Integer::toString(newSize)));
				/*
				   Debug::trace("PrinterManager.PrinterNomSize", printerTraceString + " preImageLabel - LabelOrientationDegrees() == 0 or 180 - isContinuousStock - size=" + HobString(Integer::toString(newSize)));	
				   PrinterNomSize prNomSize(this, myPrinterType, newSize, newSize);
				   const char *pCC = prNomSize.serialize();
				   pMyPrinter->send(pCC, prNomSize.getSerializedLength());
				   */
			}
			else
			{
				newSize = stockLoaded.getLength();
				if (isTracing)
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString +  "  bitmap bounds  right: " + HobString(Integer::toString(newSize)));
			}
			newSize *= (printheadSizeToUse / 8);


			//give the destination bitmap the size to allocate
			if (isTracing)
				Debug::trace("PrinterManager.PrinterImaging", printerTraceString + "  Allocation Bitmap Size: " + HobString(Integer::toString(newSize)));

			if ((pDestinationData = bitMap[whichPrinter]->allocateBitMap(newSize)) == NULL)
			{
				if (isTracing)
				{
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + "  BitMap allocation failed");
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pDestinationData == NULL");
				}
			}
			else
			{
				//used in message for setting the destination the renderer to blit into
				imageControlBlock[whichPrinter]->setImageBitMapSize(newSize);

				if (isTracing)
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + "  printheadSizeToUse: " + HobString(Integer::toString(printheadSizeToUse)));

				bitMap[whichPrinter]->setBitMapBounds(0, printheadSizeToUse, 0, ((continuousStock) ? pCurrentLabel->getLabelStockDescriptor()->getLength() : stockLoaded.getLength()));

				// Horizontal Offset for label size differences needed somewhere
				// around here!!
				if (isTracing)
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + "  centeringOffset: " +  HobString(Integer::toString(centeringOffset)));
				imageControlBlock[whichPrinter]->setCenteringOffset(centeringOffset);
				imageControlBlock[whichPrinter]->setHorizFormOffset((UINT16) pConfig->getHorizLabelSkew(myPrinterType));

				if (isTracing)
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + "  labelWidthInDots: " + HobString(Integer::toString(stockLoaded.getWidthDots())));
				imageControlBlock[whichPrinter]->setHorizFormWidth(stockLoaded.getWidthDots());

				//set the pitch for the bitmap
				INT16 bitMapPitch = ((bitMap[whichPrinter]->getBitMapBoundsRight() - bitMap[whichPrinter]->getBitMapBoundsLeft() - 1) / 16 + 1);

				bitMap[whichPrinter]->setBitMapPitch(bitMapPitch);

				imageControlBlock[whichPrinter]->setImageBitMap(bitMap[whichPrinter]);

				imageControlBlock[whichPrinter]->setPrintingRotatedBitmap(false);

				imgOk = true;
			}
		}
		else
		{
			UINT32 stockLength = 0;
			UINT32 stockWidth = 0;
			UINT32 unrotatedStockWidthInBytes = 0;

			UINT32 rotatedSize = 0;
			UINT32 unRotatedSize = 0;
			UINT16 *pUnRotatedData = NULL;

			if (continuousStock)
			{
				stockLength = pCurrentLabel->getLabelStockDescriptor()->getLength();
				stockWidth = pCurrentLabel->getLabelStockDescriptor()->getWidthDots();
				unrotatedStockWidthInBytes = (((stockLength + 15) / 16) * 2);

				rotatedSize =  stockLength * (printheadSizeToUse / 8);
				unRotatedSize =  stockWidth * unrotatedStockWidthInBytes;
				/*
				   Debug::trace("PrinterManager.PrinterNomSize", printerTraceString + " preImageLabel - LabelOrientationDegrees() != 0 or 180 - isContinuousStock - size=" + inttostring(stockLength));	
				   PrinterNomSize prNomSize(this, myPrinterType, stockLength, stockLength);
				   const char *pCC = prNomSize.serialize();
				   pMyPrinter->send(pCC, prNomSize.getSerializedLength());
				   */
			}
			else
			{
				stockLength = stockLoaded.getLength();
				stockWidth = stockLoaded.getWidthDots();
				unrotatedStockWidthInBytes = (((stockLength + 15) / 16) * 2);

				rotatedSize =  stockLength * (printheadSizeToUse / 8);
				unRotatedSize =  stockWidth * unrotatedStockWidthInBytes;
			}

			if (isTracing)
				Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " Calling rotatedBitMap::freeBitMap()");

			deleteBitMap( rotatedBitMap[whichPrinter] );
			rotatedBitMap[whichPrinter] = new BitMap;

			//give the destination bitmap the size to allocate
			if (isTracing)
			{
				Debug::trace("PrinterManager.PrinterImaging", HobString("PrinterManager: ") +
					HobString(Integer::toString(myPrinterType)) +
					HobString("  Allocation unrotatedBitmap Size: ") +
					HobString(Integer::toString(unRotatedSize)));

				Debug::trace("PrinterManager.PrinterImaging", HobString("PrinterManager: ") +
					HobString(Integer::toString(myPrinterType)) +
					HobString("  Allocation rotatedBitmap Size: ") +
					HobString(Integer::toString(rotatedSize)));
			}

			if ((pUnRotatedData = bitMap[whichPrinter]->allocateBitMap(unRotatedSize)) == NULL)
			{
				if (isTracing)
				{
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + "  BitMap allocation failed");
					Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pUnRotatedData == NULL");
				}
			}
			else
			{
				if ((pDestinationData = rotatedBitMap[whichPrinter]->allocateBitMap(rotatedSize)) == NULL)
				{
					if (isTracing)
					{
						Debug::trace("PrinterManager.PrinterImaging", printerTraceString + "  rotatedBitMap allocation failed");
						Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " pDestinationData == NULL");
					}
				}
				else
				{


					//we are going to rotate image so just ignore offsets for now.
					//we will do the offset at the time which we rotate the entire image.


					imageControlBlock[whichPrinter]->setHorizFormOffset(0);
					imageControlBlock[whichPrinter]->setCenteringOffset(0);
					imageControlBlock[whichPrinter]->setHorizFormWidth((UINT16)stockLength);
					imageControlBlock[whichPrinter]->setPrintingRotatedBitmap(true);

					//the bounds must be a multiple of 16(1 word). This is because the windml font
					//driver uses the bounds (not the pitch) as the pitch of the bitmap to draw in.
					//we have been getting lucky so far with portrait labels since the headsizes
					//have always been a multiple of 16 (448, 640)

					UINT32 unrotatedBitmapPitch = ((stockLength - 1) / 16) + 1;
					bitMap[whichPrinter]->setBitMapBounds(0, (INT16)unrotatedBitmapPitch * 16, 0, (INT16)stockWidth);
					bitMap[whichPrinter]->setBitMapPitch((INT16)unrotatedBitmapPitch);

					rotatedBitMap[whichPrinter]->setBitMapBounds(0, printheadSizeToUse, 0, (INT16)stockLength);
					rotatedBitMap[whichPrinter]->setBitMapPitch((rotatedBitMap[whichPrinter]->getBitMapBoundsRight() - rotatedBitMap[whichPrinter]->getBitMapBoundsLeft() - 1) / 16 + 1);

					imageControlBlock[whichPrinter]->setImageBitMap(bitMap[whichPrinter]);
					imageControlBlock[whichPrinter]->setImageBitMapSize(unRotatedSize);

					imageControlBlock[whichPrinter]->setRotatedBitMap(rotatedBitMap[whichPrinter]);
					imageControlBlock[whichPrinter]->setRotatedBitMapSize(rotatedSize);

					imageControlBlock[whichPrinter]->setRotatedImgOffset(centeringOffset + (UINT16) pConfig->getHorizLabelSkew(myPrinterType));

					if (isTracing)
						imageControlBlock[whichPrinter]->show();

					imgOk = true;
				}
			}
		}

		if (imgOk)
		{
			imageControlBlock[whichPrinter]->setImageFlags(PreImage);
			imageControlBlock[whichPrinter]->setImageLines(0);
			imageControlBlock[whichPrinter]->setImageValid(false);

			// old way this worked was to check if the label stock had a length of 0; then it is continuous stock
			// now we have a method to check it, so we have the following.
			if (continuousStock)
				//setting to scrunch will allow the renderer to manipulate the form field
				imageControlBlock[whichPrinter]->setImageOptions(Scrunch);
			else
				imageControlBlock[whichPrinter]->setImageOptions(0);

			pen.setPenColor(Black);	/* for test change back to black ats 06172002 */
			pen.setPenState(Down);
			pen.setPenLocation(0,0);
			pen.setPenSize(1,1);
			imageControlBlock[whichPrinter]->setPen(&pen);

			// Look for embedded labels and recursively embed image control blocks for them
			if (isTracing)
				Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " Going to process embedded image control blocks");
			processEmbeddedImageControlBlocks(imageControlBlock[whichPrinter], pCurrentLabel);

			// We postponed the pre-imaging until this point so that we can include any
			// embedded label and all its forms in that pre-imaging (before this used to
			// be done much earlier).
			if (isTracing)
				Debug::trace("PrinterManager.PrinterImaging", printerTraceString + " Going to updateFormPreImageFields");
			updateFormPreImageFields(*imageControlBlock[whichPrinter]);

			if (isTracing)
			{
				if (imageControlBlock[whichPrinter])
					imageControlBlock[whichPrinter]->show();
			}

			RendererSetSource rdSetSource(this, myRendererSessionId, (UINT8 *) imageControlBlock[whichPrinter]);
			const char *pCC = rdSetSource.serialize();
			int mLength = rdSetSource.getSerializedLength();
			pMyRenderer->send(pCC, mLength);

			RendererSetDestination rdSetDestination(this, myRendererSessionId, (UINT8 *) pDestinationData);
			pCC = rdSetDestination.serialize();
			mLength = rdSetDestination.getSerializedLength();
			pMyRenderer->send(pCC, mLength);

			RendererRequestRendering rdReqRendering(this, myRendererSessionId);  // outFilename set to empty string to indicate we don't want to write out the image to a file, since it's not complete yet
			pCC = rdReqRendering.serialize();
			mLength = rdReqRendering.getSerializedLength();
			pMyRenderer->send(pCC, mLength);

			printerState = PrinterPreImaging;
			Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterPreImaging");

			preImageError = false;
		}

		stockLoaded.setLength(savedStockDescriptor.getLength());
		stockLoaded.setOriginalWidthDots(savedStockDescriptor.getOriginalWidthDots());
		stockLoaded.setWidthDots(savedStockDescriptor.getWidthDots());
		stockLoaded.setWidth(savedStockDescriptor.getWidth());
		stockLoaded.setLabelOrientation(savedStockDescriptor.getLabelOrientation());
		stockLoaded.setLabelOrientationDegrees(savedStockDescriptor.getLabelOrientationDegrees());
		stockLoaded.setSkew(savedStockDescriptor.getSkew());
	}
	return (preImageError);
}



void PrinterManager::processEmbeddedImageControlBlocks(ImageControl* ic, LabelType* label)
{
	const LabelType::labelContainer& embeddedLabels = label->getEmbeddedLabels();
	LabelType::labelContainerIterator iter = embeddedLabels.begin();

	while ( iter != embeddedLabels.end() )
	{
		LabelType* embeddedLabel = *iter;
		if ( embeddedLabel )
		{
			Debug::trace("PrinterManager.embeddedLabels", printerTraceString + " Identified embedded label " +
				longtostring(embeddedLabel->getLabelTypeNum()));

			Debug::trace("PrinterManager.embeddedLabels", printerTraceString + " Creating embedded Image Control block ...");
			const LabelStockDescriptor* embeddedLSD = embeddedLabel->getLabelStockDescriptor();

			if ( !embeddedLSD )
				Debug::trace("PrinterManager.embeddedLabels", printerTraceString + " Bad label stock descriptor!");
			else
			{
				int labelNum = (UINT16)embeddedLabel->getLabelTypeNum();
				
				ImageControl* embeddedIC = ic->addEmbeddedLabel((UINT16) labelNum, embeddedLabel->getFormsList(), embeddedLSD);
				Debug::trace("PrinterManager.embeddedLabels", printerTraceString + " new Image Control block " + ptrtostring(embeddedIC));

				// Create the bitmap
				BitMap *bitMap = new BitMap();
				if ( embeddedLabel->getOrientation() == PortraitLabel )
					//				bitMap->setBitMapBounds(0, ((embeddedLSD->getOriginalWidthDots() + 15)/16)*16, 0, embeddedLSD->getLength());
					bitMap->setBitMapBounds(0, embeddedLSD->getWidthDots(), 0, embeddedLSD->getLength());
				else
					//				bitMap->setBitMapBounds(0, ((embeddedLSD->getLength() + 15)/16)*16, 0, embeddedLSD->getOriginalWidthDots());
					bitMap->setBitMapBounds(0, embeddedLSD->getLength(), 0, embeddedLSD->getWidthDots());
				bitMap->setBitMapPitch((bitMap->getBitMapBoundsRight() + 15)/16);

				int bitmapSizeInWords = bitMap->getBitMapPitch() * bitMap->getBitMapBoundsBottom();
				bitMap->allocateBitMap(bitmapSizeInWords);
				embeddedLabelBitMaps_.push_back(bitMap);
				Debug::trace("PrinterManager.embeddedLabels", printerTraceString + " Created and stored bitMap with size (" +
						inttostring(bitMap->getBitMapBoundsRight()) + "," +
						inttostring(bitMap->getBitMapBoundsBottom()) + ") using " + inttostring(bitmapSizeInWords) +
						" words to store it");

				embeddedIC->setImageBitMap(bitMap);
				embeddedIC->setImageBitMapSize(bitmapSizeInWords * 2);   // Size in bytes
				embeddedIC->setHorizFormWidth(bitMap->getBitMapBoundsRight());
				embeddedIC->setHorizFormOffset(0);
				embeddedIC->setCenteringOffset(0);
				embeddedIC->setImageFlags(PreImage);
				embeddedIC->setImageLines(0);
				embeddedIC->setImageOptions(0);
				embeddedIC->setPen(ic->getPen());

				processEmbeddedImageControlBlocks(embeddedIC, embeddedLabel);
			}
		}

		++iter;
	}
}


void PrinterManager::maskCommand(UINT8 sensorMask, UINT8 sensor2Mask, UINT8 userMask)
{
	if (pMyPrinter != NULL)
	{
		PrinterMask printerMask(this, myPrinterType, sensorMask, sensor2Mask, userMask);

		const char *pCC = printerMask.serialize();
		int mLength = printerMask.getSerializedLength();

		pMyPrinter->send(pCC, mLength);
	}
}

void PrinterManager::teachPrinter(PrinterModeType mode, UINT8 sensorMask, UINT8 sensor2Mask, UINT8 userMask)
{
	Debug::trace("PrinterManager.teachPrinter", printerTraceString + " Teaching Printer - mode: " + HobString(Integer::toString((UINT32) mode)));
	Debug::trace("PrinterManager.teachPrinter", "PrinterMode: " + Integer::toString(mode));	
	maskCommand(sensorMask, sensor2Mask, userMask);
	//	maskCommand(LabelTakenBit, OutOfDataBuffersBit, TakingBit);

	teachCommand(AdvanceCommand, mode);
	teachCommand(PrintCommand, mode);
	teachCommand(EjectCommand, mode);
	teachCommand(SyncCommand, mode);
	teachCommand(FindCommand, mode);
	teachCommand(SizeCommand, mode);
	teachCommand(FindwoLabelTakenCommand, mode);
}

void PrinterManager::teachPrinterNormalSizing()
{
	Debug::trace("PrinterManager.StateMachine", printerTraceString + " Teaching Printer Normal Size for UnsizedLabel mode");

	teachCommand(SizeCommand, UnsizedLabel);
}

void PrinterManager::teachCommand(PrinterCommandType command, PrinterModeType mode)
{
	if ((pMyPrinter != NULL) && (pMyTeachTable != NULL))
	{
		//Debug::trace("PrinterManager.teachCommand", "PrinterManager::teachCommand() - mode: " + HobString(Integer::toString((UINT32)mode)));
		//Debug::trace("PrinterManager.teachCommand", "PrinterManager::teachCommand() command: " + Integer::toString(command));

		PrinterTeach printerTeach(this, myPrinterType, command, pMyTeachTable->getCommandOperationList(command, mode));
		printerTeach.show();
		const char* pCC = printerTeach.serialize();
		int mLength = printerTeach.getSerializedLength();

		pMyPrinter->send(pCC, mLength);	
	}
}


void PrinterManager::sendPrinterFlush()
{
	if(pMyPrinter != NULL) {
		PrinterFlush printerFlush(this, myPrinterType);

		const char *pCC = printerFlush.serialize();
		pMyPrinter->send(pCC, printerFlush.getSerializedLength());
	}
}

void PrinterManager::sendPrinterEnableTakeup()
{
	if ( pMyPrinter != NULL && (isWrapperPrinter() || prKind == KPPPrinter) )	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
	{
		PrinterEnableTakeup printerEnableTakeup(this, myPrinterType);

		const char *pCC = printerEnableTakeup.serialize();
		pMyPrinter->send(pCC, printerEnableTakeup.getSerializedLength());
	}
}


// (SJJC) This message is only received by Real Time Controller in order to
// manage the Peripherals List for the User Interface. It tells the RTC that
// First/Second/Third (myPrinterType) Printer is of type EPP/ESS/ESC (prKind).
void PrinterManager::sendPrinterKind()
{
	PrinterPrinterKind printerPrinterKind(this, myPrinterType, prKind);
	dispatchEvent(&printerPrinterKind);
}


void PrinterManager::sendPrinterReset(UINT8 location)
{
	printerResetting = true;
	printerCalibrating = false;
	printerAdvancing = false;

	Debug::trace("PrinterManager.Gap", "Reset location: " + Integer::toString( location ));
	if (pMyPrinter != NULL)
	{
		
	    /* SOF-3524*/
		if( ( printerStockMetrics.isContinuousStock() ) && 
		    ( ( prKind == KPCPrinter ) || ( prKind == ApcPrinter ) || ( prKind == SSKPCPrinter ) ) && 
		    ( printerState == PrinterXferringLabel ) ) {
			Debug::trace("PrinterManager.PrinterReset", "Do not quit in middle of print job!!! location: " + Integer::toString(location));
			return;
		}
		
		Debug::trace("PrinterManager.PrinterReset", printerTraceString + " sendingPrinterReset: " + HobString(Integer::toString(location)));

		printerResetting = true;
		Debug::trace("PrinterManager.PrinterReset", printerTraceString + " 3printerResetting: " + HobString(Bool::toString(printerResetting)));

		printerCalibrating = false;
		Debug::trace("PrinterManager.PrinterCalibration", printerTraceString + " printerCalibrating: " + HobString(Bool::toString(printerCalibrating)));

		printerAdvancing = false;
		Debug::trace("PrinterManager.PrinterAdvance", printerTraceString + " printerAdvancing: " + HobString(Bool::toString(printerAdvancing)));

		PrinterReset printerReset(this, myPrinterType);

		const char *pCC = printerReset.serialize();
		pMyPrinter->send(pCC, printerReset.getSerializedLength());
	}
}

void PrinterManager::sendRequestLabelTransfer(UINT16 bytesToXmit, UINT16 *pUint16, bool sendSetupXferMessage, bool cancelPendingTransfer)
{
	if (pMyPrinter != NULL)
	{
		if( ( prKind == KPCPrinter ) || ( prKind == SSKPCPrinter ) ) {
//			PrinterRequestLabelTransfer printerRequestLabelTransfer(this, myPrinterType, bytesToXmit, 0, pUint16, 0, 0);
//			const char *pCC = printerRequestLabelTransfer.serialize();
//			pMyPrinter->send(pCC, printerRequestLabelTransfer.getSerializedLength());

		} else {
			// Download and print the label here!!
			PrinterRequestLabelTransfer printerRequestLabelTransfer(this, myPrinterType, bytesToXmit, 0, pUint16, sendSetupXferMessage, cancelPendingTransfer);
			const char *pCC = printerRequestLabelTransfer.serialize();
			
			Debug::trace("PrinterManager.LabelTransfer", " labeldata to print " + inttostring(printerRequestLabelTransfer.getSerializedLength()));
			pMyPrinter->send(pCC, printerRequestLabelTransfer.getSerializedLength());
		}
		pendingLabelXfer = true;
	}
}


#ifdef FAKE_SOCKET_WEIGHER
void PrinterManager::sendRequestVirtualLabel(UINT16 bytesToXmit, UINT16 *pUint16, INT32 linesToPrint)
{
	Debug::trace("PrinterManager.AutoTester", string("RequestVirtualLabel: size: ") + Integer::toString(bytesToXmit) + string("ptr: ") + Hex::toString((unsigned long)pUint16) );
	// Download and print the label here!!
	PrinterRequestVirtualLabel printerRequestVirtualLabel(this, myPrinterType, bytesToXmit, pUint16, linesToPrint);

	const char *pCC = printerRequestVirtualLabel.serialize();
	pMyPrinter->send(pCC, printerRequestVirtualLabel.getSerializedLength());
}
#endif


void PrinterManager::sendLabelData()
{
	Debug::trace("PrinterManager.PrLabelSensorLabel", " sendLabelData(): START" );

	if (printHeelFirst)
	{
		Debug::trace("PrinterManager.LabelTransfer", " sendLabelData() CALL sendLabelDataHeel(): ");
		sendLabelDataHeel();
	}
	else
	{
		Debug::trace("PrinterManager.LabelTransfer", " sendLabelData() CALL sendLabelDataHead(): ");
		sendLabelDataHead();
	}


	Debug::trace("PrinterManager.PrLabelSensorLabel", " sendLabelData(): END");
}

void PrinterManager::sendLabelDataHead()
{
	UINT16 bytesToXmit = 0;
	UINT16 *pUint16 = labelData;

	Debug::trace("PrinterManager.LabelTransfer", "sendLabelDataHead(): " );
	if( ( prKind == KPCPrinter ) || ( prKind == SSKPCPrinter ) )
	{

		Debug::trace("PrinterManager.LabelTransfer", "labelDataSize: " + Integer::toString(labelDataSize) );
		Debug::trace("PrinterManager.LabelTransfer", "buffer size: " + Integer::toString(bufferSize) );
		Debug::trace("PrinterManager.LabelTransfer", "transferSize: " + Integer::toString(transferSize) );

		if( labelDataSize <= bufferSize )
		{

			Debug::trace("PrinterManager.LabelTransfer", "Sending Request to open 485!");

			if ( !pRS485Manager->assignLabelImage( myPrinterType, (unsigned char *)labelData, labelDataSize, transferSize, true ) )
			{
				Debug::trace("PrinterManager.LabelTransfer", "Assign label image failed");
			}

			PrinterRequestLabelTransfer requestLabelTransfer(this, myPrinterType, labelDataSize, labelDataSize, labelData, 0, 0);
			const char *pCC = requestLabelTransfer.serialize();
			pMyPrinter->send( pCC, requestLabelTransfer.getSerializedLength() );

			//no need to send more data the 485Manager handles transfer of entire image.
			pendingLabelXfer = false;

			labelDataSize = 0;
			labelData = NULL;
		}
		else
		{	
			if ( !pRS485Manager->assignLabelImage( myPrinterType, (unsigned char *)labelData, bufferSize, transferSize, false ) )
			{
				Debug::trace("PrinterManager.LabelTransfer", "Assign label image failed");
			}

			Debug::trace("PrinterManager.LabelTransfer", "2 Sending Request to open 485!");

			Debug::trace("PrinterManager.LabelTransfer", "PrinterRequestLabelTransfer: " + Integer::toString(bufferSize) );
			PrinterRequestLabelTransfer requestLabelTransfer(this, myPrinterType, bufferSize, bufferSize, labelData, 0, 0);
			const char *pCC = requestLabelTransfer.serialize();
			pMyPrinter->send( pCC, requestLabelTransfer.getSerializedLength() );

			labelDataSize -= bufferSize;
			Debug::trace("PrinterManager.LabelTransfer", "remaining label size: " + Integer::toString(labelDataSize) );
			labelData = (UINT16 *) ((long long) labelData + (long long) bufferSize );
		}
	}
	else
	{
		if (labelDataSize <= bufferSize)
		{
			
			bytesToXmit = (UINT16)labelDataSize;
			pUint16 = labelData;

			labelDataSize = 0;
			labelData = NULL;
			Debug::trace("PrinterManager.LabelTransfer", "create pHeadBuffer:" + Hex::toString((long long)pHeelBuffer));
			Debug::trace("PrinterManager.LabelTransfer", "create pHeadBuffer:" + Hex::toString((long long)pHeelBufferStart));
			Debug::trace("PrinterManager.LabelTransfer", "labelDataSize: " + Integer::toString(labelDataSize));		// 34104
		}
		else
		{
			
			bytesToXmit = bufferSize;
			pUint16 = labelData;

			labelDataSize -= bufferSize;
			labelData = (UINT16 *) ((long long) pUint16 + (long long) bufferSize);
			Debug::trace("PrinterManager.LabelTransfer", "create pHeadBuffer:" + Hex::toString((long long)pHeelBuffer));
			Debug::trace("PrinterManager.LabelTransfer", "create pHeadBuffer:" + Hex::toString((long long)pHeelBufferStart));
			Debug::trace("PrinterManager.LabelTransfer", "labelDataSize: " + Integer::toString(labelDataSize));		// 34104

		}

		// if( prKind == KPCPrinter ) {
		// 	Debug::trace("PrinterManager.PrLabelSensorLabel", " sendLabelDataHEAD(): IF  prKind == KPCPrinter");
		// 	pRS485Manager->send( pUint16, labelDataSize );
		// } else {
		// 	Debug::trace("PrinterManager.PrLabelSensorLabel", " sendLabelDataHEAD(): ELSE  prKind == KPCPrinter");
		// 	sendRequestLabelTransfer(bytesToXmit, pUint16);
		// }
		// Debug::trace("PrinterManager.PrLabelSensorLabel", " sendLabelDataHEAD(): END");

		sendRequestLabelTransfer(bytesToXmit, pUint16);
	}

}

void PrinterManager::sendLabelDataHeel()
{
	UINT16 bytesToXmit = 0;
	UINT16 *pUint16 = NULL;

	Debug::trace("PrinterManager.LabelTransfer", "sendLabelDataHeel(): START " );
	if( ( prKind == KPCPrinter ) || ( prKind == SSKPCPrinter ) )
	{
		
		if(pHeelBuffer == nullptr)
		{
			pHeelBuffer = new UINT16 [labelDataSize];
			pHeelBufferStart = pHeelBuffer;
			Debug::trace("PrinterManager.LabelTransfer", "create pHeelBuffer:" + Hex::toString((long long)pHeelBuffer));
			Debug::trace("PrinterManager.LabelTransfer", "create pHeelBuffer:" + Hex::toString((long long)pHeelBufferStart));
			Debug::trace("PrinterManager.LabelTransfer", "labelDataSize: " + Integer::toString(labelDataSize));		// 34104
			flipLabelBufferHeel( labelDataSize );
			Debug::trace("PrinterManager.LabelTransfer", "sendLabelDataHeel() Flipped pHeelBuffer: " + Hex::toString((long long)pHeelBuffer));
			Debug::trace("PrinterManager.LabelTransfer", "sendLabelDataHeel() Flipped labelData: " + Hex::toString((long long)labelData));

#if 0
			std::string str = "";
			int i = 0;
			while (i < labelDataSize)
			{
				std::string s = "0x";
				s.append(Hex::toString(pHeelBuffer[i++]));
				s.append(",");
				str.append(s);
			}
			Debug::trace("PrinterManager.TestLabel", "sendLabelDataHeel() Flipped buffer pHeelBuffer: " + str);
#endif

		}
		

		Debug::trace("PrinterManager.LabelTransfer", "transferSize: " + Integer::toString(transferSize) );
		if( labelDataSize <= bufferSize ) 
		{
			if ( !pRS485Manager->assignLabelImage( myPrinterType, (unsigned char *)pHeelBuffer, labelDataSize, transferSize, false ) )
			{
				Debug::trace("PrinterManager.LabelTransfer", "Assign label image failed");
			}

			//assign label image first so that we don't get PrLabelTransferReady Before we have set up the 485 manager
			Debug::trace("PrinterManager.LabelTransfer", "Sending Request to open 485! transfer size: " + Integer::toString(labelDataSize));
			PrinterRequestLabelTransfer requestLabelTransfer(this, myPrinterType, labelDataSize, labelDataSize, labelData, 0, 0);

			const char *pCC = requestLabelTransfer.serialize();
			pMyPrinter->send( pCC, requestLabelTransfer.getSerializedLength() );

			//no need to send more data the 485Manager handles transfer of entire image.
			pendingLabelXfer = false;
			
			//set our pointer back to the top 
			pHeelBuffer = pHeelBufferStart; 
			labelDataSize = 0;
			labelData = NULL;
		}
		else
		{
			Debug::trace("CANPrinterManager.LabelTransfer", "Sending Request to open 485! transfer size: " + Integer::toString(( bufferSize - transferSize )));

			//assign label image first so that we don't get PrLabelTransferReady Before we have set up the 485 manager
			Debug::trace("PrinterManager.Flip", "top pHeelBuffer: " + Hex::toString((long long)pHeelBuffer) );
			if ( !pRS485Manager->assignLabelImage( myPrinterType, (unsigned char *)pHeelBuffer, bufferSize, transferSize, false ) )
			{
				Debug::trace("PrinterManager.LabelTransfer", "Assign label image failed");
			}

			//request to fill printers buffer for the first pass
			PrinterRequestLabelTransfer requestLabelTransfer(this, myPrinterType, bufferSize, bufferSize, labelData, 0, 0);
			const char *pCC = requestLabelTransfer.serialize();
			pMyPrinter->send( pCC, requestLabelTransfer.getSerializedLength() );

			Debug::trace("PrinterManager.Filp", "labelData: " + Hex::toString((long long)labelData));
			labelDataSize -= bufferSize;
			pHeelBuffer += ( bufferSize / 2); //indexing in words not bytes!
			Debug::trace("PrinterManager.LabelTransfer", "remaining label size2: " + Integer::toString(labelDataSize) );
			Debug::trace("PrinterManager.LabelTransfer", "Next labelData: " + Hex::toString((long long)labelData) );
		}
	}
	else
	{
		if( !imageFlip ) { 		
			Debug::trace("PrinterManager.LabelTransfer", "Printer: " + Integer::toString( myPrinterType ) + " No imageFlip" );
		    if (labelDataSize <= bufferSize)
		    {
			    bytesToXmit = (UINT16)labelDataSize;
			    pUint16 = (UINT16 *) ((long long) labelData - (long long) labelDataSize);

			    labelDataSize = 0;
			    labelData = NULL;
		    }
		    else
		    {
				Debug::trace("PrinterManager.LabelTransfer", "Printer: " + Integer::toString( myPrinterType ) + " sendLabelDataHeel() labelDataSize: " + Integer::toString(labelDataSize));		
			    bytesToXmit = bufferSize;
			    pUint16 = (UINT16 *) ((long long) labelData - (long long) bufferSize);

			    labelDataSize -= bufferSize;
			    labelData = pUint16;
		    }
			Debug::trace("PrinterManager.LabelTransfer", "Printer: " + Integer::toString( myPrinterType ) + " sendLabelDataHeel() labelDataSize: " + Integer::toString(labelDataSize) );
		} else {
			Debug::trace("PrinterManager.LabelTransfer", "imageFlip" );
			if( pHeelBuffer == nullptr ) { 
				pHeelBuffer = new UINT16 [labelDataSize];
				pHeelBufferStart = pHeelBuffer;								

				bytesToXmit = bufferSize;
				// We are already pointing at the bottom of bitmap. To flip we need to point to the top of bitmap.
				Debug::trace("PrinterManager.LabelTransfer", "bufferSize:" + Hex::toString(bufferSize));
				Debug::trace("PrinterManager.LabelTransfer", "labelDataSize: " + Integer::toString(labelDataSize));		// 34104

				flipLabelBufferHeel( labelDataSize );
							
				labelData = (UINT16 *) ( (long long) pHeelBufferStart + (long long) labelDataSize);								
				pUint16 = (UINT16 *) ((long long) labelData - (long long) bufferSize );
				labelData = pUint16;
				labelDataSize -= bufferSize;

				Debug::trace("PrinterManager.LabelTransfer", "Start pUint16:" + Hex::toString((long long)pUint16));
			} else  if (labelDataSize <= bufferSize) {
				pUint16 = (UINT16 *) ((long long) labelData - (long long) labelDataSize);
				Debug::trace("PrinterManager.LabelTransfer", "Printer: " + Integer::toString( myPrinterType ) + " Last: sendLabelDataHeel() pUint16: " + Hex::toString((long long)pUint16));		
				bytesToXmit = (UINT16)labelDataSize; 

				labelDataSize = 0;			
				labelData = NULL;
			} else {
				pUint16 = (UINT16 *) ((long long) labelData - (long long) bufferSize );
				labelData = pUint16;
				labelDataSize -= bufferSize;
				bytesToXmit = bufferSize;

				Debug::trace("PrinterManager.LabelTransfer", "Printer: " + Integer::toString( myPrinterType ) + " sendLabelDataHeel() pUint16: " + Hex::toString((long long)pUint16));		
			}
		}
		if( pUint16 != NULL ) {
		    sendRequestLabelTransfer(bytesToXmit, pUint16);
	    }
    }
}

void PrinterManager::flipLabelBufferHeel( int size )
{

	if( pHeelBuffer != NULL ) {
		
		UINT16 *pStart = pHeelBuffer;
		Debug::trace("PrinterManager.Flip", "flipLabelBufferHeel() START");
		Debug::trace("PrinterManager.Flip", "pHeelBuffer:" + Hex::toString((long long)pHeelBuffer));
		Debug::trace("PrinterManager.Flip", "size:" + Integer::toString(size));
		Debug::trace("PrinterManager.Flip", "printheadSize:" + Integer::toString(printheadSize));
		memset( (void *)pHeelBuffer, 0,  size );

		int numLines = ( size / ( printheadSize / 8 ) );
#if defined ( __linux__ )
		int lineSize = printheadSize / 16;
		unsigned char line[ lineSize ] = {0};
#else
		int lineSize = 28;
		unsigned char line[28] = { 0 };
#endif
		Debug::trace("PrinterManager.Flip", "numLines: " + Integer::toString(numLines));
		Debug::trace("PrinterManager.Flip", "lineSize: " + Integer::toString(lineSize));
		Debug::trace("PrinterManager.Flip", "labelDataSize: " + Integer::toString(labelDataSize));
		Debug::trace("PrinterManager.Flip", "end of line: " + Hex::toString((long long)labelData));
		
		
		UINT16 *pImageStart = NULL;		
		//if the top margin has been padded then remove! SOF-4592
#if defined ( __linux__ )
		unsigned int topMargin = abs(40 - adjTopMargin);	//SOF-4597 find the original margin.
#else
		int topMargin = abs(40 - adjTopMargin);	//SOF-4597 find the original margin.
#endif
		if( adjTopMargin ) {
			//we are copying the image from the top minus the padding whitespace down and placing in the heel buffer from the bottom up!
			pImageStart = (UINT16 *)((long long)labelData - (long long)( labelDataSize - ( adjTopMargin * lineSize) ) );
			//reset for next PLU.
			adjTopMargin = 0;
		} else {
	    //we are copying the image from the top down and placing in the heel buffer from the bottom up!
	    pImageStart = (UINT16 *)((long long)labelData - (long long)labelDataSize);
    }

		//start the buffer on the first line at the bottom of the image.
		pHeelBuffer = (UINT16 *)((long long)pHeelBuffer + (long long)size);
		for( int x = 0; x < numLines; x++ ) {

			for(int i = 0; i < lineSize; i++ ) {
				
				*pHeelBuffer++ = *pImageStart++;
			}
			pHeelBuffer -= ( 2 * lineSize );
		}
		pHeelBuffer = pStart;

		UINT16 *pFlip = pStart;
#if defined ( __linux__ )
		unsigned char a = 0, b = 0, newLine[ (lineSize * 2) ] = {0};
#else
		unsigned char a = 0, b = 0, newLine[(28 * 2)] = { 0 };
#endif
		//now to swap bytes and swap each half of line so that image is not mirrored.
		for( int x = 0; x < numLines; x++ ) {
			
			int y = (lineSize * 2) - 1;
			//int y = 0;
			//split the line in half 
			for(int i = 0; i < lineSize; i++ ) {				
				
				unsigned short z = *pFlip++;

				a = (unsigned char)( z & 0x00FF );	
				b = (unsigned char)( ( z & 0xFF00 ) >> 8 );				
				//save to the new line				
				newLine[y--] = b;
				newLine[y--] = a;
			}
			unsigned short *pWLine = (UINT16 *)&newLine[0];

			//flip new line word and copy back into heel buffer
			for(int i = 0; i < lineSize; i++ ) {
				pHeelBuffer[i] = flipWord( pWLine[i] );				
			}
			//advance to the next line
			pHeelBuffer = pFlip;			
		}
		pHeelBuffer = pStart;
		
		//SOF-5171, SOF-5725 This is also affecting HTm.
		if( ( prKind != SSKPCPrinter ) && ( prKind != KPCPrinter ) ) {
			//This only occurs on an prepack system.
			if( topMargin != 0 )
				memset( pStart, 0, ( topMargin * ( lineSize * 2 ) ) ); //SOF-4597 remove any artifacts from the top margin
		}

		#if 1	//add for debugging image.
		for( int x = 0; x < numLines; x++ ) {
			std::string str = "";
			for(int i = 0; i < lineSize; i++ ) {
				
				str.append( Hex::toString( *pStart++ ) );
			}
			Debug::trace("PrinterManager.Flip", "flipword() line: " + str );
		}
		#endif
	}		
}

UINT16 PrinterManager::flipWord( UINT16 labelWord )
{
	UINT16 destWord = 0;

	for ( int srcBit = 0x0001, destBit = 0x8000; srcBit & 0xFFFF; srcBit <<= 1, destBit >>= 1 )
	{
		if ( labelWord & srcBit )
			destWord |= destBit;
	}

	return destWord;
}

void PrinterManager::sendPrinterType()
{
	Debug::trace("PrinterManager.PrinterType", printerTraceString + " sendPrinterType");
	if( ( prKind == KPCPrinter ) || ( prKind == SSKPCPrinter ) ){
		PrinterPrinterType prType( this, myPrinterType, 0, 0 );
		dispatchEvent( &prType );
	} else {
		PrinterPrinterType prType(this, myPrinterType, usbNodeId, physicalAddress);
		dispatchEvent(&prType);
		if (Debug::isTracing("PrinterManager.PrinterTypeShow") == true)
		{
			prType.show();
		}
	}

}

void PrinterManager::requestWakeupMsg()
{
	if (pMyPrinter != NULL)
	{
		PrinterRequestWakeup prw(this);

		const char *pCC = prw.serialize();
		pMyPrinter->send(pCC, prw.getSerializedLength());

		Debug::trace("PrinterManager.PrinterProbe", printerTraceString + " requesting wakeup message");
	}
}

void PrinterManager::requestStatusMsg()
{
	if (pMyPrinter != NULL) {
		PrinterRequestStatus prs(this, myPrinterType);

		const char *pCC = prs.serialize();
		pMyPrinter->send(pCC, prs.getSerializedLength());
	}
}

void PrinterManager::requestInquireMsg()
{
	Debug::trace("PrinterManager.PrinterInquire", printerTraceString + "  requestInquireMsg():   myPrinterType = " + HobString(Hex::toString(myPrinterType)));

	if (pMyPrinter != NULL)
	{
		Debug::trace("PrinterManager.PrinterInquire", printerTraceString + "  requestInquireMsg():   Requesting the Inquire message.");
		PrinterRequestInquire pri(this, myPrinterType);

		const char *pCC = pri.serialize();
		pMyPrinter->send(pCC, pri.getSerializedLength());
	}
}

void PrinterManager::requestSensorMsg()
{
	if ((pMyPrinter != NULL) && (printerResetting == false))
	{
		PrinterRequestSensors prs(this, myPrinterType);

		const char *pCC = prs.serialize();
		pMyPrinter->send(pCC, prs.getSerializedLength());
	}
}

void PrinterManager::requestSystemWakeupMsg()
{
	if ((pMyPrinter != NULL) && (printerResetting == false))
	{
		PrinterSystemRequestWakeup prsw(this, myPrinterType);

		const char *pCC = prsw.serialize();
		pMyPrinter->send(pCC, prsw.getSerializedLength());
	}
}

void PrinterManager::requestSystemStatusMsg()
{
	if( ( prKind !=  KPCPrinter ) && ( prKind != SSKPCPrinter ) ) {
		if ((pMyPrinter != NULL) && (printerResetting == false))
		{
			PrinterSystemRequestStatus prss(this, myPrinterType);

			const char *pCC = prss.serialize();
			pMyPrinter->send(pCC, prss.getSerializedLength());
		}
	} else {
		Debug::trace("PrinterManager.Error", "SystemStatus unsupported message for KPCPrinter Printer type.");
	}
}

void PrinterManager::requestSystemInquireMsg()
{
	if( ( prKind !=  KPCPrinter ) && ( prKind != SSKPCPrinter ) ) {
		if ((pMyPrinter != NULL) && (printerResetting == false))
		{
			PrinterSystemRequestInquire prsi(this, myPrinterType);

			const char *pCC = prsi.serialize();
			pMyPrinter->send(pCC, prsi.getSerializedLength());
		}
	} else {
		Debug::trace("PrinterManager.Error", "SystemInquire message unsupported message for KPCPrinter Printer type.");
	}
}

void PrinterManager::requestHeadDotStatus()
{
	if( (Configuration::getInstance()->isDotWearLicensed()) && (Configuration::getInstance()->isDotWearEnabled()) ){
		if( ( prKind ==  KPCPrinter ) || ( prKind == SSKPCPrinter ) ) {
			Debug::trace("PrinterManager.CodeChecker"," Requesting head dot status.");
				PrinterRequestHeadDotStatus stat(this, myPrinterType, 0, prKind);
				dispatchEvent(&stat);
		} else {
			Debug::trace("PrinterManager.Error", "Head Dot Status unsupported for this printer type.");
		}
	} else {
		Debug::trace("PrinterManager.info", "Head Dot Status is not licensed.");
	}
}

void PrinterManager::requestHeadDotWear()
{
	if( ( prKind ==  KPCPrinter ) || ( prKind == SSKPCPrinter ) ) {
		Debug::trace("PrinterManager.CodeChecker"," Requesting Head Dot Wear.");
			PrinterRequestHeadDotWear wear(this, myPrinterType, 0, prKind);
			dispatchEvent(&wear);
	} else {
		Debug::trace("PrinterManager.Error", "Head Dot Wear unsupported for this printer type.");
	}
}

void PrinterManager::requestCutterStatus()
{
	if( ( prKind ==  KPCPrinter ) || ( prKind == SSKPCPrinter ) ) {
		Debug::trace("PrinterManager.Cutter"," Requesting Cutter Status.");
			PrinterGeneric cStatus(PrReqCutterStatus,this, myPrinterType);
			dispatchEvent(&cStatus);
	} else {

		Debug::trace("PrinterManager.ARACutter", printerTraceString + " requesting status message");
		PrinterGeneric cStatus(PrReqCutterStatus,this, myPrinterType);

		const char *pCC = cStatus.serialize();
		pMyPrinter->send(pCC, cStatus.getSerializedLength());
	}
}


void PrinterManager::timeoutProcess( __int32 timer_id )
{
	//JRB i dont think that this is necessary.
	//however, if you add code that deals with queues then
	//the following code must be uncommented out.
	//JRB	Util::CLockCriticalSection lock( printerManagerCS );
	Debug::trace("PrinterManager.Timer", "timeoutProcess: " + Integer::toString(timer_id));
	if ( timer_id == printerProbe && !isFakePrinterManager() )
	{
		probeTimeoutCntr_++;
		switch (probeTimeoutCntr_)
		{
			case 0:
				break;
			case 1:
				{
//					Debug::trace("PrinterManager.PrinterProbe", printerTraceString + HobString("1) Probe Timeout requesting wakeup "));
					requestWakeupMsg();
				}
				break;
			case 2:
				{
//					Debug::trace("PrinterManager.PrinterProbe", printerTraceString + HobString("2) Probe Timeout requesting wakeup"));
					requestWakeupMsg();
				}
				break;
			case 3:
				{

					Debug::trace("PrinterManager.PrinterProbe", printerTraceString + HobString("3) Printer Lost Contact"));

					if( ( prKind !=  KPCPrinter ) && ( prKind != SSKPCPrinter ) ) {
						PrinterGeneric msg( PrKPCDetached, this, myPrinterType  );
						dispatchEvent( &msg );
					}
					probeTimeoutCntr_ = 0;

					if (lastStatusMsg.isPeripheralComm() == true)
					{
						Debug::trace("PrinterManager.PrinterProbe", printerTraceString + " sending lost comm message");

						lastStatusMsg.clearPeripheralComm();
						dispatchEvent(&lastStatusMsg);
					}
				}
				break;
		}
	}
	else if ( timer_id == rfidQryTimer )
	{
		Debug::trace("PrinterManager.rfidQryTimer", " rfidQryTimer callback hit");
		
		bool qryDone = false;
		int resultCode = 1; 
		char rcexBuffer[256] = { 0 };

		string rfidProductType = "";

		if(isWrapperPrinter())
		{
			FpmManager *pSerialFlash( FpmManager::getInstance() );

			const std::string style( pSerialFlash->getMFGSystemStyle() );

			if( style == "EPCP" )
			{
				rfidProductType = "PCP";
			}
			else
			{
				rfidProductType = "NGW";
			}
		}
		else
		{
			rfidProductType = "GTX";
		}		

		HobString action("QRY|" + Configuration::getInstance()->toStringCfg(fieldOffsetCfgRfidEncodeMethod) + "|" + "200000000004" + "|" + "260916" + "|" + "0" + "|" + "1" + "|" + "2" + "|" + "0" + "|" + rfidProductType);

		Debug::trace("PrinterManager.rfidQryTimer", " calling QRY custom action");

		hobart_cust_hook_custom_action(action.c_str(), &resultCode, rcexBuffer, sizeof(rcexBuffer));

		Debug::trace("PrinterManager.rfidQryTimer", " rfidQryTimer result code = " + std::to_string(resultCode));
		
		if(resultCode == 1)
		{
			Debug::trace("PrinterManager.rfidQryTimer", " rfidQryTimer qryDone = busy");

			PrinterGeneric result(PrRfidQryBusy, this, FirstPrinter);
			dispatchEvent(&result);
		}
		else if(resultCode == 2)
		{
			Debug::trace("PrinterManager.rfidQryTimer", " rfidQryTimer qryDone = true");

			PrinterGeneric result(PrRfidQryPassed, this, FirstPrinter);
			dispatchEvent(&result);
		}	
		else if(resultCode == 3 || resultCode == 0)
		{
			Debug::trace("PrinterManager.rfidQryTimer", " rfidQryTimer qryDone = failed");

			PrinterGeneric result(PrRfidQryFailed, this, FirstPrinter);
			dispatchEvent(&result);
		}
	}
	else if ( timer_id == rfidCalTimer )
	{
		Debug::trace("PrinterManager.rfid", " rfidCalTimer callback hit");
		
		bool calDone = false;
		int resultCode = 1; 
		char rcexBuffer[256] = { 0 };

		string rfidProductType = "";

		if(isWrapperPrinter())
		{
			FpmManager *pSerialFlash( FpmManager::getInstance() );

			const std::string style( pSerialFlash->getMFGSystemStyle() );

			if( style == "EPCP" )
			{
				rfidProductType = "PCP";
			}
			else
			{
				rfidProductType = "NGW";
			}
		}
		else
		{
			rfidProductType = "GTX";
		}		

		HobString action("STS|" + Configuration::getInstance()->toStringCfg(fieldOffsetCfgRfidEncodeMethod) + "|" + "200000000004" + "|" + "260916" + "|" + "0" + "|" + "1" + "|" + "2" + "|" + "0" + "|" + rfidProductType);

		hobart_cust_hook_custom_action(action.c_str(), &resultCode, rcexBuffer, sizeof(rcexBuffer));

		Debug::trace("PrinterManager.rfid", " rfidCalTimer result code = " + std::to_string(resultCode));
		
		if(resultCode == 1)
		{
			Debug::trace("PrinterManager.rfid", " rfidCalTimer calDone = false");
		}
		else
		{
			Debug::trace("PrinterManager.rfid", " rfidCalTimer calDone = true");

			cancelTimer(rfidCalTimer);

			if(resultCode == 2)
			{
				calDone = true;
				createTimer(rfidQryTime, rfidQryTimer, true);
			}
			else
			{
				calDone = false;
			}

			PrinterGeneric result(calDone ? PrRfidCalPassed : PrRfidCalFailed, this, FirstPrinter);
			dispatchEvent(&result);
		}	
	}
	else if ( timer_id == rfidUpgTimer )
	{
		Debug::trace("PrinterManager.rfid", " rfidUpgTimer callback hit");
		
		bool upgDone = false;
		int resultCode = 1; 
		char rcexBuffer[256] = { 0 };

		string rfidProductType = "";

		if(isWrapperPrinter())
		{
			FpmManager *pSerialFlash( FpmManager::getInstance() );

			const std::string style( pSerialFlash->getMFGSystemStyle() );

			if( style == "EPCP" )
			{
				rfidProductType = "PCP";
			}
			else
			{
				rfidProductType = "NGW";
			}
		}
		else
		{
			rfidProductType = "GTX";
		}		

		HobString action("STS|" + Configuration::getInstance()->toStringCfg(fieldOffsetCfgRfidEncodeMethod) + "|" + "200000000004" + "|" + "260916" + "|" + "0" + "|" + "1" + "|" + "2" + "|" + "0" + "|" + rfidProductType);

		hobart_cust_hook_custom_action(action.c_str(), &resultCode, rcexBuffer, sizeof(rcexBuffer));

		Debug::trace("PrinterManager.rfid", " rfidUpgTimer result code = " + std::to_string(resultCode));
		
		if(resultCode == 1)
		{
			Debug::trace("PrinterManager.rfid", " rfidUpgTimer upgDone = false");
		}
		else
		{
			Debug::trace("PrinterManager.rfid", " rfidUpgTimer upgDone = true");

			cancelTimer(rfidUpgTimer);

			if(resultCode == 2)
			{
				upgDone = true;
				createTimer(rfidQryTime, rfidQryTimer, true);
			}
			else
			{
				upgDone = false;
			}

			PrinterGeneric result(upgDone ? PrRfidUpgradePassed : PrRfidUpgradeFailed, this, FirstPrinter);
			dispatchEvent(&result);
		}	
	}
	else if ( timer_id == sensorTimerExpired )
	{
		Debug::trace("PrinterManager.PrLabelSensorLabel", printerTraceString + " TimeoutProcess():PrLabelSensorLabel");
		if (trackSensors == PrinterOn)
		{
			requestSensorReading = true;
		}
	}
	else if ( timer_id == takeLabelTimerExpired )
	{
		Debug::trace("PrinterManager.TakeLabelTime", "timeout()");
		takeLabelError = true;
	}
	else if ( timer_id == printheadPowerTimerExpired )
	{
		Debug::trace("PrinterManager.Timer", printerTraceString + " Timeout:printheadPowerTimerExpired");

		PrintheadPower printheadPower (this, myPrinterType, PrinterOff, PrinterOn);
		dispatchEvent(&printheadPower);

		if (pMyPrinter != NULL)
		{
			const char *pCC = printheadPower.serialize();
			pMyPrinter->send(pCC, printheadPower.getSerializedLength());
		}

		if (Debug::isTracing("PrinterManager.PrintheadPower") == true)
		{
			printheadPower.show();
		}
	}
	else if ( timer_id == continuousLabelTimerExpired )
	{
		Debug::trace("PrinterManager.Timer", printerTraceString + " Timeout:continuousLabelTimerExpired");
		sendTestLabel = true;
		printerState = PrinterLabelIsReady;
		Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterLabelIsReady");
	}
	else if (timer_id == labelStatisticsTimer)
	{
		if( (printerCounts_ > 0) && (printerCounts_ != previousPrinterCounts_) )
		{
			std::thread t( &PrinterManager::writeScaleDataThread, this );
			t.detach();
		}
	}
	else if (timer_id == testLabelTimerTimeout)
	{
		Debug::trace("PrinterManager.Timer", "Create checkerboard test label and print");
		
		if(createTestLabel(PrCheckerboard))
		{
			printTestLabel();
		}
		else
		{
			Debug::trace("PrinterManager.Timer", "Failed to create test label!");
		}
	}
	#ifdef PR_DEFAULTS_TEST
	else if( timer_id == PrinterTests )
	{
		if( pMyPrinter != NULL )
		{
			Debug::trace("PrinterManager.Test", "Sending PrResetFactoryDefaults");

			PrinterGeneric msg(PrResetFactoryDefaults);
			msg.setSourceObject(this);
			const char *pCC = msg.serialize();
			pMyPrinter->send( pCC, msg.getSerializedLength() );
		}
	}
	#endif
}

void PrinterManager::sendPrinterConfig(PrConfigType configType)
{
	if (pMyPrinter != NULL)
	{
		if (Debug::isTracing("PrinterManager.PrinterConfig"))
		{
			Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " Sending PrConfig to printer - configType is " + inttostring(myPrinterType));
			printerConfig.show();
		}

		printerConfig.setConfigType(configType);

		const char *pCC = printerConfig.serialize();
		pMyPrinter->send(pCC, printerConfig.getSerializedLength());
	}
}

void PrinterManager::sendPrinterSystemConfig(SysConfigType sysConfigType)
{
	if (pMyPrinter != NULL)
	{
		Debug::trace("PrinterManager.PrinterSystemConfiguration", printerTraceString +  " Sending Printer System Config Message");

		if (Debug::isTracing("PrinterManager.PrinterSystemConfiguration"))
		{
			printerSysConfig.show();
		}

		printerSysConfig.setSysConfigType(sysConfigType);

		const char *pCC = printerSysConfig.serialize();
		pMyPrinter->send(pCC, printerSysConfig.getSerializedLength());
	}
}

void PrinterManager::updateFormPreImageFields(const ImageControl& imageControlBlock)
{
	bool tracing = Debug::isTracing("PrinterManager.PreImageField");

	NutriDataColumn* column1 = 0;
	NutriDataColumn* column2 = 0;
	NutriDataColumn* column3 = 0;
	NutriDataColumn* column4 = 0;

	if (tracing)
		Debug::trace("PrinterManager.PreImageField", "beginning " );

	imageControlBlock.initFormListIter();
	Debug::trace("PrinterManager.PreImageField", "after initFormListIter()" );
	while ( !imageControlBlock.isCurrentFormNull() )	 //added to process the whole label
	{
		if (tracing)
			Debug::trace("PrinterManager.PreImageField", "in while loop, current form is not null" );
		FormRecord *pForm = imageControlBlock.getFormFromList();
		if (tracing)
			Debug::trace("PrinterManager.PreImageField", printerTraceString + " pForm->getFieldType(): " +  HobString(Integer::toString((UINT16) (pForm->getFieldType()))));

		if ( pForm->isFormPreImage() || pForm->getFieldType() == INDIRECT_LABEL_REFERENCE )
		{
			if (tracing)
				Debug::trace("PrinterManager.PreImageField", printerTraceString + " pForm->getFieldType(): " +  HobString(Integer::toString((UINT16) (pForm->getFieldType()))));

			switch (pForm->getFieldType())
			{
				case DESCRIPTION:		// 0x01
					if (pCurrentProduct != NULL)
					{
						pForm->setData((UINT32 *) (pCurrentProduct->getDescription().c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " DESCRIPTION: " + HobString((const char *) (pForm->getData())));
					break;
				case EXPANDED_TEXT_1:	// 0x03
					if ((pCurrentProduct != NULL) && (pCurrentProduct->getProductText(psttExpandedText) != NULL))
					{
						pForm->setData((UINT32 *) (pCurrentProduct->getProductText(psttExpandedText)->getText().c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " EXPANDED_TEXT_1: " + HobString((const char *) (pForm->getData())));
					break;
				case SPECIAL_MESSAGE:	// 0x05
					if ((pCurrentProduct != NULL) && (pCurrentProduct->getProductText(psttSpecialMessage) != NULL))
					{
						pForm->setData((UINT32 *) (pCurrentProduct->getProductText(psttSpecialMessage)->getText().c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " SPECIAL_MESSAGE: " + HobString((const char *) (pForm->getData())));
					break;
				case PRODUCT_TEXT_5:	// 0xFC
					if ((pCurrentProduct != NULL) && (pCurrentProduct->getProductText(psttProductText5) != NULL))
					{
						pForm->setData((UINT32 *) (pCurrentProduct->getProductText(psttProductText5)->getText().c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " PRODUCT_TEXT_5: " + HobString((const char *) (pForm->getData())));
					break;
				case PRODUCT_TEXT_6:	// 0x7D
					if ((pCurrentProduct != NULL) && (pCurrentProduct->getProductText(psttProductText6) != NULL))
					{
						pForm->setData((UINT32 *) (pCurrentProduct->getProductText(psttProductText6)->getText().c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " PRODUCT_TEXT_6: " + HobString((const char *) (pForm->getData())));
					break;
				case PRODUCT_TEXT_7:	// 0x7E
					if ((pCurrentProduct != NULL) && (pCurrentProduct->getProductText(psttProductText7) != NULL))
					{
						pForm->setData((UINT32 *) (pCurrentProduct->getProductText(psttProductText7)->getText().c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " PRODUCT_TEXT_7: " + HobString((const char *) (pForm->getData())));
					break;
				case PRODUCT_TEXT_8:	// 0x7F
					if ((pCurrentProduct != NULL) && (pCurrentProduct->getProductText(psttProductText8) != NULL))
					{
						pForm->setData((UINT32 *) (pCurrentProduct->getProductText(psttProductText8)->getText().c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " PRODUCT_TEXT_8: " + HobString((const char *) (pForm->getData())));
					break;
				case PRODUCT_TEXT_9:	// 0x80
					if ((pCurrentProduct != NULL) && (pCurrentProduct->getProductText(psttProductText9) != NULL))
					{
						pForm->setData((UINT32 *) (pCurrentProduct->getProductText(psttProductText9)->getText().c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " PRODUCT_TEXT_9: " + HobString((const char *) (pForm->getData())));
					break;
				case STORE_NAME:		// 0x0F
					if (pConfig != NULL)
					{
						pForm->setData((UINT32 *) (pConfig->toStringCfg(fieldOffsetCfgStoreName).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " STORE_NAME: " + HobString((const char *) (pForm->getData())));
					break;
				case STORE_NUMBER:		// 0x10
					if (pConfig != NULL)
					{
						pForm->setData((UINT32 *) (pConfig->toStringCfg(fieldOffsetCfgStoreId).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " STORE_NUMBER: " + HobString((const char *) (pForm->getData())));
					break;
				case USER_LINE:			// 0x13
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " USER_LINE: ");
					break;
				case USER_GRAPHIC:		// 0x14
					userGraphicBitMap1.setBitMapBase((UINT16 *) NULL);
					userGraphicBitMap1.setBitMapBounds(0, 0, 0, 0);
					userGraphicBitMap1.setBitMapPitch(0);
					if (pCurrentProduct != NULL)
					{
						Graphic *pUserGraphic = pCurrentProduct->getGraphic(Graphic::GRAPHIC1);
						if (pUserGraphic != NULL && pUserGraphic->getErrorCode() == RECORD_OK)
						{
							Debug::trace("Graphic", printerTraceString + " user graphic name is: " + pUserGraphic->getGraphicName());

							GraphicImage *pGraphicImage = pUserGraphic->getGraphicImage();
							if (pGraphicImage)
							{
								if (Debug::isTracing("Graphic"))
								{
									Debug::trace("Graphic", printerTraceString + " user graphicImage address is: " + HobString(Hex::toString((long long)pGraphicImage)));
									Debug::trace("Graphic", printerTraceString + " user graphicImage bitmap address is: " + HobString(Hex::toString((long long)pGraphicImage->getBinaryImage())));
								}

								userGraphicBitMap1.setBitMapBase((UINT16 *) pGraphicImage->getBinaryImage());

								GraphicHeader *header = pGraphicImage->getHeader();
								userGraphicBitMap1.setBitMapBounds(header->getLeft(), header->getRight(), header->getTop(), header->getBottom());
								userGraphicBitMap1.setBitMapPitch(header->getPitch());
							}
						}
					}
					pForm->setBitMapData(&userGraphicBitMap1);
					break;

				case STORE_GRAPHIC:		// 0x15
					{
						storeGraphicBitMap.setBitMapBase((UINT16 *) NULL);
						storeGraphicBitMap.setBitMapBounds(0, 0, 0, 0);
						storeGraphicBitMap.setBitMapPitch(0);
						Graphic *pStoreGraphic = (Graphic *) pConfig->getStoreGraphic();

						if (pStoreGraphic != NULL && pStoreGraphic->getErrorCode() == RECORD_OK)
						{
							HobString h1 = pStoreGraphic->getGraphicName();
							Debug::trace("Graphic", printerTraceString + " store graphic name is: " + h1 );

							GraphicImage *pGraphicImage = pStoreGraphic->getGraphicImage();
							if (pGraphicImage)
							{
								char *pBitmap = pGraphicImage->getBinaryImage();
								if (Debug::isTracing("Graphic"))
								{
									Debug::trace("Graphic", printerTraceString + " store graphicImage address is: " + HobString(Hex::toString((long long)pGraphicImage)));
									Debug::trace("Graphic", printerTraceString + " store graphicBitMap address is: " + HobString(Hex::toString((long long) &storeGraphicBitMap)));
									Debug::trace("Graphic", printerTraceString + " store graphicImage bitmap address is: " + HobString(Hex::toString((long long)pBitmap)));
								}

								if (pBitmap)
								{
									storeGraphicBitMap.setBitMapBase((UINT16 *) pBitmap);

									GraphicHeader *header = pGraphicImage->getHeader();
									if (header)
									{
										storeGraphicBitMap.setBitMapBounds(header->getLeft(), header->getRight(), header->getTop(), header->getBottom());
										storeGraphicBitMap.setBitMapPitch(header->getPitch());
									}
								}
							}
						}
					}

					pForm->setBitMapData(&storeGraphicBitMap);
					break;

				case GENERIC_TEXT_1:	// 0x16
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(1).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_1: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_2:	// 0x17
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(2).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_2: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_3:	// 0x18
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(3).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_3: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_4:	// 0x19
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(4).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_4: " +	 HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_5:	// 0x1A
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(5).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_5: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_6:	// 0x1B
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(6).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_6: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_7:	// 0x1C
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(7).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_7: " +	HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_8:	// 0x1D
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(8).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_8: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_9:	// 0x1E
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(9).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_9: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_10:	// 0x1F
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(10).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_10: " + HobString((const char *) (pForm->getData())));
					break;
				case PRODUCT_NUMBER:	// 0x20
					if (pCurrentProduct != NULL)
					{
						pForm->setData((UINT32 *) (HobString(Integer::toString(pCurrentProduct->getProductNum())).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " PRODUCT_NUMBER: " + HobString((const char *) (pForm->getData())));
					break;
				case CURRENT_TIME:		// 0x33
					{
						HobDateTime now;
						pForm->setData((UINT32 *) (now.formatTime(FormatTime_HHMM, pConfig->getTimeDisplayFormat()).c_str())); // FT-612

						if (tracing)
							Debug::trace("PrinterManager.PreImageField", printerTraceString + " CURRENT_TIME: " + HobString((const char *) (pForm->getData())));
					}
					break;
				case TOTAL_WEIGHT:
					if ( tracing )
						Debug::trace( "PrinterManager.PreImageField", printerTraceString + " TOTAL_WEIGHT" );

					if ( pCurrentProduct )
					{
						const bool isRunTotalsLabel( Product::PRINTING_RUN_TOTALS_LABEL == pCurrentProduct->getLabelBeingPrinted() );

						HobString totalWeightString;

						if ( isRunTotalsLabel )
						{
							if ( tracing )
								Debug::trace( "PrinterManager.PreImageField", printerTraceString + " TOTAL_WEIGHT: have run totals label" );

							Session *session = SessionManager::getInstance()->getPrimaryConsoleSession();
							ProductRunTotal *productRunTotal = session ? session->getProductTotalManager()->getCurrentProductRunTotal() : ((ProductRunTotal*) NULL);

							if ( productRunTotal )
							{
								pConfig->formatWeight( productRunTotal->getWeight(), totalWeightString, true, DefaultPrecision );

								if ( MetSig::isScaleMetric() )
									totalWeightString.append( " kg" );
								else
									totalWeightString.append( " lb" );

								if ( tracing )
									Debug::trace( "PrinterManager.PreImageField", printerTraceString + " TOTAL_WEIGHT: " + Long::toString( productRunTotal->getWeight() ) + " " + totalWeightString );
							}
						}

						pForm->setData((UINT32 *) (totalWeightString.c_str()));
					}

					break;
				case TOTAL_PACKAGES:
					if ( tracing )
						Debug::trace( "PrinterManager.PreImageField", printerTraceString + " TOTAL_PACKAGES" );

					if ( pCurrentProduct )
					{
						const bool isRunTotalsLabel( Product::PRINTING_RUN_TOTALS_LABEL == pCurrentProduct->getLabelBeingPrinted() );

						HobString totalPackagesString;

						if ( isRunTotalsLabel )
						{
							if ( tracing )
								Debug::trace( "PrinterManager.PreImageField", printerTraceString + " TOTAL_PACKAGES: have run totals label" );

							Session *session = SessionManager::getInstance()->getPrimaryConsoleSession();
							ProductRunTotal *productRunTotal = session ? session->getProductTotalManager()->getCurrentProductRunTotal() : ((ProductRunTotal*) NULL);

							if ( productRunTotal )
							{
								totalPackagesString = Long::toString( productRunTotal->getPackages() );

								if ( tracing )
									Debug::trace( "PrinterManager.PreImageField", printerTraceString + " TOTAL_PACKAGES: " + Long::toString( productRunTotal->getPackages() ) + " " + totalPackagesString );
							}
						}

						pForm->setData((UINT32 *) (totalPackagesString.c_str()));
					}

					break;
				case INDIRECT_LABEL_REFERENCE:  // 0x35
					{
						bool tracingEL = Debug::isTracing("PrinterManager.embeddedLabels");
						if (tracingEL)
							Debug::trace("PrinterManager.embeddedLabels", printerTraceString + " Going to pre-image embedded label " +
									inttostring(pForm->getVerticalJustify()) + " with bitmap " + ptrtostring(pForm->getBitMapData()) +
									" and it " + (pForm->isDataPresent() ? "has" : "has no") + " data present");

						int labelNum = pForm->getVerticalJustify();
						
						// Recursively pre-image any embedded labels
						const ImageControl* embeddedImageControl = imageControlBlock.getEmbeddedImageControl((UINT16) labelNum);
						if ( embeddedImageControl )
						{
							updateFormPreImageFields(*embeddedImageControl);

							if (tracingEL)
								Debug::trace("PrinterManager.embeddedLabels", printerTraceString + " Setting form's bitmap data with " +
										ptrtostring(embeddedImageControl->getImageBitMap()));

							pForm->setBitMapData(embeddedImageControl->getImageBitMap());
						}
	    				else
						{
							if (tracingEL)
								Debug::trace("PrinterManager.embeddedLabels", printerTraceString + " Did not find embeddedImageControl * in the map");
	    				}

						if (tracingEL)
							Debug::trace("PrinterManager.embeddedLabels", printerTraceString + " Completed pre-imaging embedded label " +
									inttostring(pForm->getVerticalJustify()) + " with bitmap " + ptrtostring(pForm->getBitMapData()) +
									" and it " + (pForm->isDataPresent() ? "has" : "has no") + " data present");

						break;
					}
				case DIVIDING_LINE:		// 0x3B
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " DIVIDING_LINE: ");
					break;
				case CONSTANT_TEXT:			// 0x3d
					if (tracing)
					{
						char *data( (char*) pForm->getData() );

						if ( !data )
							Debug::trace("PrinterManager.PreImageField", printerTraceString +
									" NULL CONSTANT_TEXT");
						else
							Debug::trace("PrinterManager.PreImageField", printerTraceString +
									" CONSTANT_TEXT: " + string(data));
					}
					break;
				case GENERIC_TEXT_11:	// 0x3e
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(11).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString +
								" GENERIC_TEXT_11: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_12:	// 0x3f
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(12).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_12: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_13:	// 0x40
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(13).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_13: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_14:	// 0x41
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(14).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_14: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_15:	// 0x42
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(15).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_15: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_16:	// 0x43
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(16).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_16: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_17:	// 0x44
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(17).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_17: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_18:	// 0x45
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(18).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_18: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_19:	// 0x46
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(19).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_19: " + HobString((const char *) (pForm->getData())));
					break;
				case GENERIC_TEXT_20:	// 0x47
					if (pConfig->toBoolCfg(fieldOffsetCfgPFTextNamesPreprinted) == true)
					{
						pForm->setData((UINT32 *) (pConfig->getLabelText(20).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " GENERIC_TEXT_20: " + HobString((const char *) (pForm->getData())));
					break;
				case INDIRECT_GRAPHIC_REFERENCE:	// 0x48
					{
						BitMap *indirectGraphicBitMap = NULL;

						if (pCurrentProduct != NULL)
						{
							bitmapsContainerIterator pos = indirectBitmaps.find(pForm->getFamilyName());

							if (pos == indirectBitmaps.end())
							{
								bool insertedBM = false;
								BitMap *pBitMap = new BitMap();
								if (pBitMap != NULL)
								{
									pBitMap->setBitMapBase((UINT16 *) NULL);
									pBitMap->setBitMapBounds(0, 0, 0, 0);
									pBitMap->setBitMapPitch(0);

									Graphic *pIndirectGraphic = Graphic::findGraphic(pForm->getFamilyName());
									if (pIndirectGraphic != NULL)
									{
										if (pIndirectGraphic->getErrorCode() == RECORD_OK)
										{
											GraphicImage *pGraphicImage = pIndirectGraphic->getGraphicImage();
											if (pGraphicImage)
											{
												pBitMap->setBitMapBase((UINT16 *) pGraphicImage->getBinaryImage());

												GraphicHeader *header = pGraphicImage->getHeader();
												pBitMap->setBitMapBounds(header->getLeft(), header->getRight(), header->getTop(), header->getBottom());
												pBitMap->setBitMapPitch(header->getPitch());

												Debug::trace("PrinterManager.IndirectGraphic", printerTraceString + " Inserting BitMap -- " + HobString(pForm->getFamilyName()) +
														" : 0x" + HobString(Hex::toString((long long) pBitMap)));

												indirectGraphics.insert(make_pair(string(pForm->getFamilyName()), pIndirectGraphic));
												indirectBitmaps.insert(make_pair(string(pForm->getFamilyName()), pBitMap));
												indirectGraphicBitMap = pBitMap;
												insertedBM = true;
											}
										}

										if (!insertedBM)
											delete pIndirectGraphic;
									}

									if (!insertedBM)
										delete pBitMap;
								}
							}
							else
							{
								indirectGraphicBitMap = pos->second;
							}
						}

						pForm->setBitMapData(indirectGraphicBitMap);
					}
					break;

				case TOTAL_POINTS:		// 0x53
					if (pCurrentProduct != NULL)
					{
						pForm->setData((UINT32 *) (HobString(Integer::toString(pCurrentProduct->getPoints())).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " TOTAL_POINTS: " + HobString((const char *) (pForm->getData())));
					break;
				case NUTRI_COLUMN_1:	// 0x67
					{
						if ( !column1 && pCurrentProduct->getNutrifacts() )
							column1 = new NutriDataColumn(1, pCurrentProduct->getNutrifacts()->getColumn1Data());

						if ( column1 )
						{
							const char *tag( pForm->getTag() );

							if ( !tag )
							{
								Debug::trace("PrinterManager.PreImageField", printerTraceString +
										" Nutrition Column 1, NULL tag!");
							}
							else
							{
								string tagData( column1->getData(tag) );
								pForm->setData((UINT32*)(tagData.c_str()));

								if (tracing)
									Debug::trace("PrinterManager.PreImageField", printerTraceString +
											" Nutrition Column 1, tag: " + string(pForm->getTag()) +
											" data: " + tagData);
							}
						}
						else if (tracing)
							Debug::trace("PrinterManager.PreImageField", printerTraceString +
									" Nutrition Column 1, tag: " + string(pForm->getTag()) + " no data.");

						break;
					}
				case NUTRI_COLUMN_2:	// 0x68
					{
						if ( !column2 && pCurrentProduct->getNutrifacts() )
							column2 = new NutriDataColumn(2, pCurrentProduct->getNutrifacts()->getColumn2Data());

						if ( column2 )
						{
							pForm->setData((UINT32*)(column2->getData(pForm->getTag()).c_str()));

							if (tracing)
								Debug::trace("PrinterManager.PreImageField", printerTraceString +
										" Nutrition Column 2, tag: " + string(pForm->getTag()) +
										" data: " + string((const char*)(pForm->getData())));
						}
						else if (tracing)
							Debug::trace("PrinterManager.PreImageField", printerTraceString +
									" Nutrition Column 1, tag: " + string(pForm->getTag()) + " no data.");

						break;
					}
				case NUTRI_COLUMN_3:	// 0x69
					{
						if ( !column3 && pCurrentProduct->getNutrifacts() )
							column3 = new NutriDataColumn(3, pCurrentProduct->getNutrifacts()->getColumn3Data());

						if ( column3 )
						{
							pForm->setData((UINT32*)(column3->getData(pForm->getTag()).c_str()));

							if (tracing)
								Debug::trace("PrinterManager.PreImageField", printerTraceString +
										" Nutrition Column 1, tag: " + string(pForm->getTag()) +
										" data: " + string((const char*)(pForm->getData())));
						}
						else if (tracing)
							Debug::trace("PrinterManager.PreImageField", printerTraceString +
									" Nutrition Column 1, tag: " + string(pForm->getTag()) + " no data.");

						break;
					}
				case NUTRI_COLUMN_4:	// 0x6A
					{
						if ( !column4 && pCurrentProduct->getNutrifacts() )
							column4 = new NutriDataColumn(4, pCurrentProduct->getNutrifacts()->getColumn4Data());

						if ( column4 )
						{
							pForm->setData((UINT32*)(column4->getData(pForm->getTag()).c_str()));

							if (tracing)
								Debug::trace("PrinterManager.PreImageField", printerTraceString +
										" Nutrition Column 1, tag: " + string(pForm->getTag()) +
										" data: " + string((const char*)(pForm->getData())));
						}
						else if (tracing)
							Debug::trace("PrinterManager.PreImageField", printerTraceString +
									" Nutrition Column 1, tag: " + string(pForm->getTag()) + " no data.");

						break;
					}
				case COOL_TEXT:			// 0x71
				{
					HobString coolText;
					UINT32 *myData = 0;

					if ((pCurrentProduct != NULL) && pCurrentProduct->isCoolRequired())
					{
						coolText.assign(pCurrentProduct->getCOOLTextToPrint());
						myData = (UINT32 *) coolText.c_str();
					}
					pForm->setData(myData);
					if (tracing)
					{
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " COOL_TEXT: isCoolRequired = " + HobString(Bool::toString(pCurrentProduct->isCoolRequired())));
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " COOL_TEXT: " + HobString((const char *) (pForm->getData())));
					}
					break;
				}
				case TRACKING_TEXT:	 // 0x72
#if 1 // Fix PTR#57 (haberrc)
				{
					UINT32 *myData = 0;
					if ((pCurrentProduct != NULL) && pCurrentProduct->isCoolRequired())
						myData = (UINT32 *) pCurrentProduct->getCoolTrackingText().c_str();
					pForm->setData(myData);

					if (tracing)
					{
						Debug::trace("PrinterManager.PreImageField", printerTraceString + "TRACKING_TEXT: isCoolRequired = " + HobString(Bool::toString(pCurrentProduct->isCoolRequired())));
						Debug::trace("PrinterManager.PreImageField", printerTraceString + "TRACKING_TEXT: " + HobString((const char *) (pForm->getData())));
					}
					break;
				}
#else
					// Quantum scale does not print Tracking Text if cool is not required
					if ( pCurrentProduct != NULL && pCurrentProduct->isCoolRequired() )
					{
						pForm->setData((UINT32 *) (pCurrentProduct->getCoolTrackingText().c_str()));
						if (tracing)
							Debug::trace("PrinterManager.PreImageField", printerTraceString + " TRACKING_TEXT: " + HobString((const char *) (pForm->getData())));
					}
					break;
#endif
				case USER_GRAPHIC_2:	// 0x73
					userGraphicBitMap2.setBitMapBase((UINT16 *) NULL);
					userGraphicBitMap2.setBitMapBounds(0, 0, 0, 0);
					userGraphicBitMap2.setBitMapPitch(0);
					if (pCurrentProduct != NULL)
					{
						Graphic *pUserGraphic = pCurrentProduct->getGraphic(Graphic::GRAPHIC2);
						if (pUserGraphic != NULL && pUserGraphic->getErrorCode() == RECORD_OK)
						{
							Debug::trace("Graphic", printerTraceString + " user graphic 2 name is: " + pUserGraphic->getGraphicName());

							GraphicImage *pGraphicImage = pUserGraphic->getGraphicImage();
							if (pGraphicImage)
							{
								if (Debug::isTracing("Graphic"))
								{
									Debug::trace("Graphic", printerTraceString + " user graphicImage 2 address is: " + HobString(Hex::toString((long long)pGraphicImage)));
									Debug::trace("Graphic", printerTraceString + " user graphicImage 2 bitmap address is: " + HobString(Hex::toString((long long)pGraphicImage->getBinaryImage())));
								}

								userGraphicBitMap2.setBitMapBase((UINT16 *) pGraphicImage->getBinaryImage());

								GraphicHeader *header = pGraphicImage->getHeader();
								userGraphicBitMap2.setBitMapBounds(header->getLeft(), header->getRight(), header->getTop(), header->getBottom());
								userGraphicBitMap2.setBitMapPitch(header->getPitch());
							}
						}
					}
					pForm->setBitMapData(&userGraphicBitMap2);
					break;

				case USER_GRAPHIC_3:	// 0x74
					userGraphicBitMap3.setBitMapBase((UINT16 *) NULL);
					userGraphicBitMap3.setBitMapBounds(0, 0, 0, 0);
					userGraphicBitMap3.setBitMapPitch(0);
					if (pCurrentProduct != NULL)
					{
						Graphic *pUserGraphic = pCurrentProduct->getGraphic(Graphic::GRAPHIC3);
						if (pUserGraphic != NULL && pUserGraphic->getErrorCode() == RECORD_OK)
						{
							Debug::trace("Graphic", printerTraceString + " user graphic 3 name is: " + pUserGraphic->getGraphicName());

							GraphicImage *pGraphicImage = pUserGraphic->getGraphicImage();
							if (pGraphicImage)
							{
								if (Debug::isTracing("Graphic"))
								{
									Debug::trace("Graphic", printerTraceString + " user graphicImage 3 address is: " + HobString(Hex::toString((long long)pGraphicImage)));
									Debug::trace("Graphic", printerTraceString + " user graphicImage 3 bitmap address is: " + HobString(Hex::toString((long long)pGraphicImage->getBinaryImage())));
								}

								userGraphicBitMap3.setBitMapBase((UINT16 *) pGraphicImage->getBinaryImage());

								GraphicHeader *header = pGraphicImage->getHeader();
								userGraphicBitMap3.setBitMapBounds(header->getLeft(), header->getRight(), header->getTop(), header->getBottom());
								userGraphicBitMap3.setBitMapPitch(header->getPitch());
							}
						}
					}
					pForm->setBitMapData(&userGraphicBitMap3);
					break;

				case USER_GRAPHIC_4:	// 0x75
					userGraphicBitMap4.setBitMapBase((UINT16 *) NULL);
					userGraphicBitMap4.setBitMapBounds(0, 0, 0, 0);
					userGraphicBitMap4.setBitMapPitch(0);
					if (pCurrentProduct != NULL)
					{
						Graphic *pUserGraphic = pCurrentProduct->getGraphic(Graphic::GRAPHIC4);
						if (pUserGraphic != NULL && pUserGraphic->getErrorCode() == RECORD_OK)
						{
							Debug::trace("Graphic", printerTraceString + " user graphic 4 name is: " + pUserGraphic->getGraphicName());

							GraphicImage *pGraphicImage = pUserGraphic->getGraphicImage();
							if (pGraphicImage)
							{
								if (Debug::isTracing("Graphic"))
								{
									Debug::trace("Graphic", printerTraceString + " user graphicImage 4 address is: " + HobString(Hex::toString((long long)pGraphicImage)));
									Debug::trace("Graphic", printerTraceString + " user graphicImage 4 bitmap address is: " + HobString(Hex::toString((long long)pGraphicImage->getBinaryImage())));
								}

								userGraphicBitMap4.setBitMapBase((UINT16 *) pGraphicImage->getBinaryImage());

								GraphicHeader *header = pGraphicImage->getHeader();
								userGraphicBitMap4.setBitMapBounds(header->getLeft(), header->getRight(), header->getTop(), header->getBottom());
								userGraphicBitMap4.setBitMapPitch(header->getPitch());
							}
						}
					}
					pForm->setBitMapData(&userGraphicBitMap4);
					break;

                case UPC_NUMBER:
					if ( pCurrentProduct )
					{
						const bool isRunTotalsLabel( Product::PRINTING_RUN_TOTALS_LABEL == pCurrentProduct->getLabelBeingPrinted() );

						const BarCodeNumber barCodeNumber( pCurrentProduct->getBarCodeNum() );
						const HobString eanCode( barCodeNumber.getEANCode() );
						HobString upcNumber( barCodeNumber.getUPCNumber() );

						if(upcNumber.empty())
						{		
							upcNumber = barCodeNumber.getDerivedProductNumber();
						}

						HobString s;

						if ( isRunTotalsLabel )
						{
							if ( tracing )
								Debug::trace( "PrinterManager.PreImageField", printerTraceString + " UPC_NUMBER: have run totals label" );

							if ( upcNumber.length() )
								s = upcNumber;
							else
								s = eanCode;
						}
						else
						{
							if ( eanCode.length() )
								s = eanCode;
							else
								s = upcNumber;
						}

						pForm->setData( (UINT32*) s.c_str() );
					}

					if ( tracing )
						Debug::trace( "PrinterManager.PreImageField", printerTraceString + " UPC_NUMBER: " + HobString((const char *) (pForm->getData())) );

					break;

				case TEMPLATE_NUMBER:	// 0x82
				{
                                        int labelStringsOffset = (pForm->getFieldType() - TEMPLATE_TEXT_1) + 1;
                                        Debug::trace("ProductManager.remotePrint", string("labelStringsOffset : ") + Integer::toString(labelStringsOffset) );

                                        if ( (labelStringsOffset >= 0 ) && (pCurrentProduct != NULL) && (pCurrentProduct->getRemotePrintTemplateRequest() != NULL) )
					{
                                                pForm->setData((UINT32 *)(pCurrentProduct->getRemotePrintTemplateRequest()->getLabelString(labelStringsOffset).c_str()));
					}
                                
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " TEMPLATE_NUMBER: " + HobString((const char *)(pForm->getData())));
                                
					break;
				}

				case TEMPLATE_TEXT_1:	// 0x83(131)		0
				case TEMPLATE_TEXT_2:	// 0x84(132)		1
				case TEMPLATE_TEXT_3:	// 0x85(133)		2
				case TEMPLATE_TEXT_4:	// 0x86(134)		3
				case TEMPLATE_TEXT_5:	// 0x87(135)		4
				case TEMPLATE_TEXT_6:	// 0x88(136)		5
				case TEMPLATE_TEXT_7:	// 0x89(137)		6
				case TEMPLATE_TEXT_8:	// 0x8A(138)		7
				case TEMPLATE_TEXT_9:	// 0x8B(139)		8
				case TEMPLATE_TEXT_10:	// 0x8C(140)		9
				case TEMPLATE_TEXT_11:	// 0x8D(141)		10
				case TEMPLATE_TEXT_12:	// 0x8E(142)		11
				case TEMPLATE_TEXT_13:	// 0x8F(143)		12
				case TEMPLATE_TEXT_14:	// 0x90(144)		13
				case TEMPLATE_TEXT_15:	// 0x91(145)		14
				case TEMPLATE_TEXT_16:	// 0x92(146)		15
				case TEMPLATE_TEXT_17:	// 0x93(147)		16
				case TEMPLATE_TEXT_18:	// 0x94(148)		17
				case TEMPLATE_TEXT_19:	// 0x95(149)		18
				case TEMPLATE_TEXT_20:	// 0x96(150)		19
				case TEMPLATE_TEXT_21:	// 0x97(151)		20
				case TEMPLATE_TEXT_22:	// 0x98(152)		21
				case TEMPLATE_TEXT_23:	// 0x99(153)		22
				case TEMPLATE_TEXT_24:	// 0x9A(154)		23
				case TEMPLATE_TEXT_25:	// 0x9B(155)		24
				{
                                        int labelStringsOffset = (pForm->getFieldType() - TEMPLATE_TEXT_1) + 1 ;

					if ((pCurrentProduct != NULL) && (pCurrentProduct->getRemotePrintTemplateRequest() != NULL))
					{
						pForm->setData((UINT32 *)(pCurrentProduct->getRemotePrintTemplateRequest()->getLabelString(labelStringsOffset).c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " TEMPLATE_TEXT_: " + inttostring(labelStringsOffset) + HobString((const char *)(pForm->getData())));

					break;
				}


				// The next three cases are duplicated from the updateFormPostImageFields() method.  They are really pre-imaged fields, not post-imaged fields.
				case TOTAL_TYPE: // 38, 0x26
				{
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + "TOTAL_TYPE");
					if (pCurrentProduct != NULL && pCurrentProduct->getCurrentTransaction() != NULL)
					{
						HobString text = pCurrentProduct->getCurrentTransaction()->getTransactionTypeTextForLabel();
						if (tracing)
							Debug::trace("PrinterManager.PreImageField", printerTraceString + "TOTAL_TYPE text for label = " + text);
						if (text.size())
							pForm->setData((UINT32 *)(text.c_str()));
					}
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + "TOTAL_TYPE: " + HobString((const char *)(pForm->getData())));
					break;
				}
				case TOTAL_TYPE_NAME: // 39, 0x27
				{
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + "TOTAL_TYPE_NAME");
					if (pCurrentProduct != NULL && pCurrentProduct->getCurrentTransaction() != NULL)
					{
						HobString nameString = pCurrentProduct->getCurrentTransaction()->getTransactionTypeName();
						if (tracing)
							Debug::trace("PrinterManager.PreImageField", printerTraceString + "TOTAL_TYPE_NAME:  nameString = " + nameString);
						pForm->setData((UINT32 *)(nameString.c_str()));
					}
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + "TOTAL_TYPE_NAME: " + HobString((const char *)(pForm->getData())));
					break;
				}
				case TOTAL_TYPE_NUMBER: // 52, 0x34
				{
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + "TOTAL_TYPE_NUMBER");
					if (pCurrentProduct != NULL && pCurrentProduct->getCurrentTransaction() != NULL)
					{
						HobString numString;
						HobRecordNum transTypeNum = pCurrentProduct->getCurrentTransaction()->getTransactionTypeNumber();
						if (transTypeNum > 0)
						{
							numString = Integer::toString(transTypeNum);
							if (tracing)
								Debug::trace("PrinterManager.PreImageField", printerTraceString + "TOTAL_TYPE_NUMBER:  numString = " + numString);
						}
						pForm->setData((UINT32 *)(numString.c_str()));
					}
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + "TOTAL_TYPE_NUMBER: " + HobString((const char *)(pForm->getData())));
					break;
				}

				case CUSTOMER_NAME:
				{
					if (pCurrentProduct)
					{
						OnlineOrderProduct *pOnlineOrderProduct = dynamic_cast<OnlineOrderProduct *> (pCurrentProduct);
						if (pOnlineOrderProduct)
							pForm->setData((UINT32 *)(pOnlineOrderProduct->getCustomerName().c_str()));
					}
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " CUSTOMER_NAME: " + HobString((const char *)(pForm->getData())));
					break;
				}

				case ORDER_NUMBER:
				{
					if (pCurrentProduct)
					{
						OnlineOrderProduct *pOnlineOrderProduct = dynamic_cast<OnlineOrderProduct *> (pCurrentProduct);
						if (pOnlineOrderProduct)
							pForm->setData((UINT32 *)( longtostring( pOnlineOrderProduct->getOrderNum() ).c_str() ));
					}
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " ORDER_NUMBER: " + HobString((const char *)(pForm->getData())));
					break;
				}

				case PICKUP_TIME:
				{
					if (pCurrentProduct)
					{
						OnlineOrderProduct *pOnlineOrderProduct = dynamic_cast<OnlineOrderProduct *> (pCurrentProduct);
						if (pOnlineOrderProduct)
							pForm->setData((UINT32 *)(pOnlineOrderProduct->getPickupTime().c_str()));
					}
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " PICKUP_TIME: " + HobString((const char *)(pForm->getData())));
					break;
				}


				default:
					if (tracing)
						Debug::trace("PrinterManager.PreImageField", printerTraceString + " Unknown Form Type");
					break;
			}
		}
	}

	if ( column1 )
		delete column1;
	if ( column2 )
		delete column2;
	if ( column3 )
		delete column3;
	if ( column4 )
		delete column4;
	
	if (tracing)
		Debug::trace("PrinterManager.PreImageField", " LEAVING updateFormPreImageFields() ");
}


void PrinterManager::updateFormPostImageFields(const ImageControl& imageControlBlock)
{
	bool tracing = Debug::isTracing("PrinterManager.PostImageField");
	if (tracing)
		Debug::trace("PrinterManager.PostImageField", " ENTERING updateFormPostImageFields()  pCurrentProductTransaction=" + ptrtostring(pCurrentProductTransaction));

	pjr413HasQRLabel = false;
	bool useDefaultDecimalSeparator = false;
	bool currencySymbolPrinted = pConfig->toBoolCfg(fieldOffsetCfgPFCurrencySymbolPrinted);

	// SOF-451: Digimarc
	ImageControl *icb( const_cast<ImageControl*>( &imageControlBlock ) );

	icb->setDigimarcEnabled( Configuration::getInstance()->isDigimarcLicensed() && pConfig->toBoolCfg(fieldOffsetCfgDigimarcEnabled) && pCurrentProductTransaction );

	if ( icb->isDigimarcEnabled() )
	{
		Product* p( pCurrentProductTransaction->getProduct() );

		const HobString sellByDate( (p->getShelfLife()).formatDate( FormatDate_YYMMDD_NoDelimiter ) );

		// by count
		if ( pCurrentProductTransaction->getByCount() )
		{
			icb->setDigimarcCount( pCurrentProductTransaction->getByCount() );
			icb->setDigimarcWeightLb( 0 );
			icb->setDigimarcWeightKg( 0 );
		}
		// random weight
		else
		{
			icb->setDigimarcCount( 0 );

			if ( MetSig::isScaleAvoir() )
			{
				const HobWeight w( pCurrentProductTransaction->getTotalNetWeight() );

				if ( MetSig::isWeigherConfiguredForSingleRange() )
					icb->setDigimarcWeightLb( MetSig::divideWeightForSingleRange(w) );
				else
					icb->setDigimarcWeightLb( w );

				icb->setDigimarcWeightKg( 0 );
			}
			else
			{
				icb->setDigimarcWeightLb( 0 );
				icb->setDigimarcWeightKg( pCurrentProductTransaction->getTotalNetWeight() );
			}
		}

		icb->setDigimarcSellByDate( atoi( sellByDate.c_str() ) );
		icb->setDigimarcPrice( pCurrentProductTransaction->getTotalPrice() );
	}

	imageControlBlock.initFormListIter();

	while(!imageControlBlock.isCurrentFormNull())	//added to process the whole label
	{
		FormRecord *pForm = imageControlBlock.getFormFromList();

		if ( pForm->isFormPostImage() || pForm->getFieldType() == INDIRECT_LABEL_REFERENCE )
		{
			if (tracing)
				Debug::trace("PrinterManager.PostImageField", printerTraceString + " pForm->getFieldType(): " +
						HobString(Integer::toString((UINT16) (pForm->getFieldType()))));

			if ( pForm->getFieldType() != REPORT_STREAM && pForm->getFieldType() != INDIRECT_LABEL_REFERENCE )
				pForm->setData(NULL);

			switch (pForm->getFieldType())
			{
				case BARCODE:		// 0x00
					if ((pCurrentProductTransaction != NULL) && (pConfig->toBoolCfg(fieldOffsetCfgPFBarCodePrinted) == true))
					{
						std::string priceString = pConfig->formatHobPrice(pCurrentProductTransaction->getTotalPrice(), useDefaultDecimalSeparator);
						
						if (pConfig->digitsPrecision() == 0)
							priceString = cleanTrailingCurrencyMarks(priceString);
						
						double totalPrice = Double::valueOf(priceString.c_str());

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + " totalPrice: " + util_string::doubletostring(totalPrice));
						BarCodeNumber bcn(pCurrentProductTransaction->getBarCodeNum());

						// any previous barcodes Field SubTypes I need to reset before I get started?
						if (resetUPCBarcode)
						{
							resetUPCBarcode = false;
							short oldBarCodeType = UPCA;
							pForm->setFieldSubType((UINT8)(oldBarCodeType));
							pForm->setData((UINT32 *)(bcn.getBarcodeString((BarcodeType)oldBarCodeType).c_str()));
						}

						BarcodeType labelBarcodeType = MAX_BARCODE_TYPES;
						UINT8 subType = pForm->getFieldSubType();
						if (subType >= UPCA && subType < MAX_BARCODE_TYPES)
							labelBarcodeType = (BarcodeType) subType;
							
						switch (labelBarcodeType)
						{
							case EAN13:
							case EAN128C:
							case RSS14E:				// GS1 types
							case RSS14E_GTIN_13:
							case RSS14E_GTIN_12:
							case RSS14E_GTIN_8:
							case GS1_DATABAR_EXPANDED:
							{
								if (tracing)
								{
									Debug::trace("PrinterManager.PostImageField", printerTraceString + " Barcode type: " + inttostring(labelBarcodeType));
									Debug::trace("PrinterManager.PostImageField", printerTraceString + " Barcode String: " + HobString(bcn.getBarcodeString(labelBarcodeType).c_str()));
								}

								// For serialized barcodes
								if ( Configuration::getInstance()->toIntCfg(fieldOffsetCfgReadBarcodeFromCloud) == 1 && bcn.getBarcodeType() == EAN128C)
								{
									if (bcn.getBarcodeString(USER_SUPPLIED).size() > 0)
									{
										pForm->setData((UINT32*)(bcn.getBarcodeString(USER_SUPPLIED).c_str()));
										Debug::trace("BarcodeCloud", "Printing user supplied barcode from server");

									}
									else 
									{
										Debug::trace("BarcodeCloud", "Printing barcode generated locally");
										pForm->setData((UINT32*)(bcn.getBarcodeString(EAN128C).c_str()));

									}

									pForm->setFieldSubType((UINT8)(EAN128C));

								}
								
								else if (bcn.getBarcodeType() == USER_SUPPLIED)
								{
										// For Walmart Cake Label Project, where barcode string is passed in
										pForm->setData((UINT32*)(bcn.getBarcodeString(USER_SUPPLIED).c_str()));

								}
								else
								{	
									BarcodeType formatStringBarcodeType = (labelBarcodeType == EAN13 || labelBarcodeType == EAN128C) ? labelBarcodeType : GS1GTIN;
									unsigned int priceChars = bcn.getFormatString(formatStringBarcodeType).countConsecutiveChars('P');
									if ( priceChars == 0 || priceChars >= priceString.size() - 1 )
										pForm->setData((UINT32 *)(bcn.getBarcodeString(labelBarcodeType).c_str()));
								}	
								break;
							}

							// QRCode and DataMatrix
							case QR_LEVELM:
							case QR_LEVELL:
							case QR_LEVELQ:
							case QR_LEVELH:
							case DATAMATRIX:
							{
								pjr413HasQRLabel = true;
								HobString format;

								Debug::trace("PrinterManager.PostImageField", printerTraceString + " Barcode type: 2D (QR,DataMatrix)");

								// format from barcode
								if (DATAMATRIX == pForm->getFieldSubType())
									format = bcn.getFormatString(DATAMATRIX);
								else
									format = bcn.getFormatString(QR_CODE);

								// when user sets format via label type, use that instead
								if (pForm->getConstantData())
								{
									Debug::trace("PrinterManager.PostImageField", printerTraceString + " Using Label Type data for format!");
									format = HobString((char*)pForm->getConstantData());
								}

								Debug::trace("PrinterManager.PostImageField", printerTraceString + " Barcode Format: '" + format + "'");

								// generate code from format
								if (format.length())
								{
									const HobString data(bcn.getBarcodeStringFromFormat(format));
									pForm->setData((UINT32*)data.c_str());
								}
								break;
							}

							// UPC
							case UPCA:
							{
								if (bcn.getBarcodeType() == USER_SUPPLIED)     // For Walmart Cake Label Project, where barcode string is passed in
									pForm->setData((UINT32 *)(bcn.getBarcodeString(USER_SUPPLIED).c_str()));
								else if ((0.00 < totalPrice) && (totalPrice < 100.00)) // can't print UPC barcodes over $100
								{
									// SOF - 3220 PJR - 263 Switch barcode type on label when total price exceeds $99.99
									// Original Code when the total price is < $99.99
									pForm->setData((UINT32 *)(bcn.getBarcodeString(UPCA).c_str()));
								}
								else
								{
									// Existing code before PJR-263/SOF-3220 Switch barcode type on label when total price exceeds $99.99
									// just wondering, what happens if the barcode is 2 or 4?  No barcode for the label?
									int numberSys = Integer::valueOf(bcn.getUPCNumberSystem()); // prefix contains the UPC Number system.
									if (numberSys != 2 && numberSys != 4)
										pForm->setData((UINT32 *)(bcn.getBarcodeString(UPCA).c_str()));
									else if (pConfig->toBoolCfg(fieldOffsetCfgAutoUpdateUPCBarcodeOver99))
									{
										// SOF - 3220 PJR - 263 Switch barcode type on label when total price exceeds $99.99
										// TODO - update the label identifier in the label to be the newBarCodeType
										// See RTC/RendererMgr/HobartLabel.cpp setFieldSubType() where this is set

										short newBarCodeType = pConfig->toIntCfg(fieldOffsetCfgBarcodeTypeOver99);
										short newBarCodeWidth = pConfig->toIntCfg(fieldOffsetCfgBarcodeWidthOver99);
										short newBarcodeOffsetFromLeft = pConfig->toIntCfg(fieldOffsetBarcodeOffsetFromLeftOver99);
										if (newBarCodeType >= UPCA && newBarCodeType < MAX_BARCODE_TYPES)
										{
											resetUPCBarcode = true;
											pForm->setFieldSubType((UINT8)(newBarCodeType));

											// SOF-6286: For Costco:  When they want to print a different barcode than UPC-A when the price goes over $99.99,
											//                        use this width for the new barcode type indicated by fieldOffsetCfgBarcodeTypeOver99
											//                        Zero means don't change the width of the alternate barcode from the width of the original UPC-A barcode in the label type
											//                        and offset it the number of bits from the left side of the label (if barcodeOffsetFromLeft is not 0)
											if ( newBarCodeWidth > 0 )
												pForm->setSizeHorizontal( newBarCodeWidth );
											if ( newBarcodeOffsetFromLeft > 0 )
												pForm->setOriginHorizontal( newBarcodeOffsetFromLeft );

											HobString data;
											if (newBarCodeType == QR_LEVELM || newBarCodeType == QR_LEVELL || newBarCodeType == QR_LEVELQ || newBarCodeType == QR_LEVELH || newBarCodeType == DATAMATRIX)
											{
												HobString format;

												Debug::trace("PrinterManager.PostImageField", printerTraceString + " Barcode type: 2D (QR,DataMatrix)");

												// format from barcode
												if (DATAMATRIX == newBarCodeType)
													format = bcn.getFormatString(DATAMATRIX);
												else
													format = bcn.getFormatString(QR_CODE);

												// generate code from format
												if (format.length())
													data = bcn.getBarcodeStringFromFormat(format);
											}
											else
											{
												data = bcn.getBarcodeString((BarcodeType)newBarCodeType);
											}

											pForm->setData( (UINT32 *)( data.c_str() ) );
										}
									}
								}
								break;
							}

							default:
								break;
						}
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " BARCODE: " + HobString((const char *) (pForm->getData())));

					break;
				
				case PACKED_ON:		// 0x07
					if ((pCurrentProductTransaction != NULL) &&
							(pCurrentProduct->isDontPrintPackedOn() == false) &&
							(pConfig->toBoolCfg(fieldOffsetCfgPFPackedOnDatePrinted) == true))
					{
						HobString packedOnString;
						HobDateTime now;

						// Per QA (Jim H.), we need to back date the packed on date as well. 12/20/2007
						now.incrementDays(-pCurrentProduct->getBackDateDays());

						if (pConfig->toBoolCfg(fieldOffsetCfgPFPackedOnDateJulian) == true)
						{
							pConfig->formatDate(now, packedOnString, FormatDate_DDD, HobDateTime::_UsingPrimaryLanguageLocale_);
						}
						else
						{
							pConfig->formatDate(now, packedOnString, pConfig->getDateDisplayFormat(), HobDateTime::_UsingPrimaryLanguageLocale_);
						}

						pForm->setData((UINT32 *) (packedOnString.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " PACKED_ON: " + HobString((const char *) (pForm->getData())));
					break;
				case SELL_BY: 		// 0x08
					if ( pCurrentProductTransaction )
					{
						bool isExpLabel(( Product::PRINTING_NEW_EXPIRATION_LABEL == pCurrentProduct->getLabelBeingPrinted() ) ||
							( Product::PRINTING_EXISTING_EXPIRATION_LABEL == pCurrentProduct->getLabelBeingPrinted() ));

						// FT-1386: allow printing sell by for exiration labels
						// FT-1443: allow printing sell by for exiration labels
						if (( !pCurrentProduct->isDontPrintSellBy() && pConfig->toBoolCfg( fieldOffsetCfgPFSellByDatePrinted ) ) || ( pCurrentProduct->isDontPrintSellByOverridden() ) || ( isExpLabel ))
						{
							HobString sellByString;
							HobDateTime theDateTime( isExpLabel ? pCurrentProductTransaction->getExpirationLabelDateTime() : pCurrentProductTransaction->getShelfLife() );

							if ( pConfig->toBoolCfg( fieldOffsetCfgPFSellByDateJulian ) )
								pConfig->formatDate( theDateTime, sellByString, FormatDate_DDD, HobDateTime::_UsingPrimaryLanguageLocale_ );
							else
								pConfig->formatDate( theDateTime, sellByString, pConfig->getDateDisplayFormat(), HobDateTime::_UsingPrimaryLanguageLocale_ );

							pForm->setData((UINT32 *) (sellByString.c_str()));

							if ( tracing )
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " SELL_BY: " + HobString((const char *) (pForm->getData())));
						}
					}
					break;
				case BEST_BEFORE:  	// 0x09
					if ((pCurrentProductTransaction != NULL) && ( ( (pCurrentProduct->isDontPrintBestBefore() == false) && (pConfig->toBoolCfg(fieldOffsetCfgPFProductLifeDatePrinted) == true) ) || ( pCurrentProduct->isDontPrintBestBeforeOverridden() ) ) )
					{
						HobString bestBeforeString;
						if (pConfig->toBoolCfg(fieldOffsetCfgPFProductLifeDateJulian) == true)
						{
							pConfig->formatDate(pCurrentProductTransaction->getProductLife(), bestBeforeString, FormatDate_DDD, HobDateTime::_UsingPrimaryLanguageLocale_);
						}
						else
						{
							pConfig->formatDate(pCurrentProductTransaction->getProductLife(), bestBeforeString, pConfig->getDateDisplayFormat(), HobDateTime::_UsingPrimaryLanguageLocale_);
						}

						pForm->setData((UINT32 *) (bestBeforeString.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " BEST_BEFORE: 0x" + HobString(Hex::toString((long long) (pForm->getData()))) + "--" + HobString((const char *) (pForm->getData())));
					break;
				case NET_WEIGHT:	// 0x0A
					if (( pCurrentProduct != NULL ) && ( pCurrentProductTransaction != NULL ))
					{
						HobString netWeightString;

						if ( pCurrentProduct->isFixedWtOrFluidOz() )
						{
							if ( tracing )
							{
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " NET_WEIGHT: FixedWtOrFluidOz   pt_byCount = " + HobString(Integer::toString(pCurrentProductTransaction->getByCount())));
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " NET_WEIGHT: FixedWtOrFluidOz prod_byCount = " + HobString(Integer::toString(pCurrentProduct->getByCount())));
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " NET_WEIGHT: FixedWtOrFluidOz isByCountPrinted = " + pConfig->toStringCfg(fieldOffsetCfgPFByCountPrinted));
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " NET_WEIGHT: ByCount NetWeight = " + HobString(Long::toString(pCurrentProductTransaction->getTotalNetWeight())));
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " NET_WEIGHT: netWeightString1 = " + Integer::toString(pCurrentProductTransaction->getByCount()));
							}

							if ( ((pCurrentProductTransaction->getByCount() == 1) && (pConfig->toBoolCfg(fieldOffsetCfgPFByCountPrinted) == true)) ||
									(pCurrentProductTransaction->getByCount() > 1) )
							{
								netWeightString = Integer::toString(pCurrentProductTransaction->getByCount());
							}
						}

						else if ( pCurrentProduct->isByCount() )
						{
							if ( tracing )
							{
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " NET_WEIGHT: ByCount   pt_byCount = " + HobString(Integer::toString(pCurrentProductTransaction->getByCount())));
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " NET_WEIGHT: ByCount prod_byCount = " + HobString(Integer::toString(pCurrentProduct->getByCount())));
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " NET_WEIGHT: ByCount isWeightOnByCount = " + pConfig->toStringCfg(fieldOffsetCfgPFWeightOnByCountPrinted));
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " NET_WEIGHT: ByCount NetWeight = " + HobString(Long::toString(pCurrentProductTransaction->getTotalNetWeight())));
							}

							if ( 1 == pCurrentProductTransaction->getByCount() )
							{
								if (pConfig->toBoolCfg(fieldOffsetCfgPFOneOnByCountPrinted) == true)
								{
									netWeightString = "1";
								}
								else if (pConfig->toBoolCfg(fieldOffsetCfgPFWeightOnByCountPrinted) == true && pCurrentProductTransaction->getTotalNetWeight() > 0)
								{
									short formatIndicator = AppendWeightText;
									if (pCurrentProductTransaction->getWeigherStatus())
									{
										const HfcWeigherStatus *weight = pCurrentProductTransaction->getWeigherStatus();
										if (weight->isDualRangeEnabled() && weight->getDualRangeDirection() == DualRangeDown)
											formatIndicator |= DualRangeModeDown;
									}
									pConfig->formatWeight(pCurrentProductTransaction->getTotalNetWeight(), netWeightString, useDefaultDecimalSeparator, formatIndicator);
								}
							}
							else if ( 1 < pCurrentProductTransaction->getByCount() )
							{
								netWeightString = Integer::toString(pCurrentProductTransaction->getByCount());
							}
						}
						else
						{
							if ( tracing )
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " NET_WEIGHT: Random weight.");

							short formatIndicator = AppendWeightText;

							if ( pCurrentProductTransaction->getWeigherStatus() )
							{
								const HfcWeigherStatus *weight = pCurrentProductTransaction->getWeigherStatus();
								if (weight->isDualRangeEnabled() && weight->getDualRangeDirection() == DualRangeDown)
									formatIndicator |= DualRangeModeDown;

								// FT-1695 - Don't print the weight for a side item for BuildAMeal that is random weight,
								// since we aren't actually weighing the item.  Previously, this call to formatWeight was below the brace that immediately follows it.
								pConfig->formatWeight(pCurrentProductTransaction->getTotalNetWeight(), netWeightString, useDefaultDecimalSeparator, formatIndicator);
							}
						}

						pForm->setData((UINT32 *) (netWeightString.c_str()));
					}

					if ( tracing )
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " NET_WEIGHT: " + HobString((const char *) (pForm->getData())));

					break;
				case FORCED_KB_WEIGHT:	// 0x66
					{
						if ((pCurrentProduct != NULL) && (pCurrentProductTransaction != NULL) && ( pCurrentProduct->hasKbdNetWeightLabel() || pCurrentProduct->isCancelledKbdNetWeight() ) )
						{
							HobString netWeightString;
							short formatIndicator = AppendWeightText;
							if (pCurrentProduct->hasKbdNetWeightLabel())
								formatIndicator |= KeyboardNetWeight;

							if (pCurrentProduct->isCancelledKbdNetWeight())
							{
								// (Costco) If cancelled keyboard net weight, then it's random weight.
								// Grabbed this code from net weight above
								if (pCurrentProductTransaction->getWeigherStatus())
								{
									const HfcWeigherStatus* weight = pCurrentProductTransaction->getWeigherStatus();
									if (weight->isDualRangeEnabled() && weight->getDualRangeDirection() == DualRangeDown)
										formatIndicator |= DualRangeModeDown;
								}
							}

							if (tracing)
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " FORCED_KB_WEIGHT: Random weight.");
							pConfig->formatWeight( pCurrentProductTransaction->getTotalNetWeight(), netWeightString, useDefaultDecimalSeparator, formatIndicator );
							pForm->setData((UINT32 *) (netWeightString.c_str()));
						}

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + " FORCED_KB_WEIGHT: " + HobString((const char *) (pForm->getData())));
						break;
					}
				case FIXED_WEIGHT:		// 0x0B
				{
					if (pCurrentProduct != NULL && pCurrentProduct->getFixedWeight() > (HobWeight)0)
					{
						HobString fixedWeightString;
	  					if (pCurrentProduct->isFluidOunce())
							pConfig->formatVolume(pCurrentProduct->getFixedWeight(), fixedWeightString, FixedWeightText, useDefaultDecimalSeparator, true);
						else
							pConfig->formatWeight(pCurrentProduct->getFixedWeight(), fixedWeightString, useDefaultDecimalSeparator, FixedWeightText);
	  					
						//Debug::trace("PrinterManager.PostImageField", printerTraceString + " FIXED_WEIGHT: fixedWeightString2 = " + fixedWeightString );
						
	  					util_string::tolower(fixedWeightString); // FT-1356
						//Debug::trace("PrinterManager.PostImageField", printerTraceString + " FIXED_WEIGHT: fixedWeightString3 = " + fixedWeightString );

	  					util_string::replace(fixedWeightString,"net wt","NET WT"); // FT-1462
						//Debug::trace("PrinterManager.PostImageField", printerTraceString + " FIXED_WEIGHT: fixedWeightString4 = " + fixedWeightString );

						util_string::replace(fixedWeightString,"net wt","NET WT"); // FT-1462
						//Debug::trace("PrinterManager.PostImageField", printerTraceString + " FIXED_WEIGHT: fixedWeightString5 = " + fixedWeightString );

						pForm->setData((UINT32 *) (fixedWeightString.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " FIXED_WEIGHT: " + HobString((const char *) (pForm->getData())));
					break;
				}
				case METRIC_RULE_WEIGHT:
				{
					if (pCurrentProduct != NULL && pCurrentProduct->isFixedWtOrFluidOz() && pCurrentProduct->getFixedWeight() > (HobWeight)0)
					{
						HobString fixedWeightString;
	  					if (pCurrentProduct->isFluidOunce())
							pConfig->formatVolume(pCurrentProduct->getFixedWeight(), fixedWeightString, MetricRuleWeight, useDefaultDecimalSeparator);
						else
				   			pConfig->formatWeight(pCurrentProduct->getFixedWeight(), fixedWeightString, useDefaultDecimalSeparator, MetricRuleWeight);
						util_string::tolower(fixedWeightString); // FT-1356
	  					util_string::replace(fixedWeightString,"net wt","NET WT"); // FT-1462
						pForm->setData((UINT32 *) (fixedWeightString.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " METRIC_RULE_WEIGHT: " + HobString((const char *) (pForm->getData())));
					break;
				}
				case TARE:	 		// 0x0C
					if (pCurrentProductTransaction != NULL)
					{
						HobString totalTareString ;

						if ((pCurrentProduct->isRandomWeight() == true) ||
								((pCurrentProduct->isByCount() == true) &&
								 (pCurrentProductTransaction->getByCount() == 1) &&
								 (pConfig->toBoolCfg(fieldOffsetCfgPFWeightOnByCountPrinted) == true)))
						{
							//totalTareString = Integer::toString(pCurrentProductTransaction->getTotalTare());
							short formatIndicator = AppendWeightText;
							if (pCurrentProductTransaction->getWeigherStatus())
							{
								const HfcWeigherStatus *weight = pCurrentProductTransaction->getWeigherStatus();
								if (weight->isDualRangeEnabled() && weight->getDualRangeDirection() == DualRangeDown)
									formatIndicator |= DualRangeModeDown;
							}
							pConfig->formatWeight(pCurrentProductTransaction->getTotalTare(), totalTareString, useDefaultDecimalSeparator, formatIndicator);
						}

						pForm->setData((UINT32 *) (totalTareString.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " TARE: " + HobString((const char *) (pForm->getData())));
					break;
				case UNIT_PRICE:   	// 0x0D
					if ((pCurrentProduct != NULL) && (pCurrentProductTransaction != NULL))
					{
						HobString unitPriceString = "";
						pConfig->formatUnitPriceForPrinting(pCurrentProduct, pCurrentProductTransaction, unitPriceString);

						if (pConfig->digitsPrecision() == 0)
							unitPriceString = cleanHobTrailingCurrencyMarks(unitPriceString);

						pForm->setData((UINT32 *) (unitPriceString.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " UNIT_PRICE: " + HobString((const char *) (pForm->getData())));
					break;
				case TOTAL_PRICE:  	// 0x0E
					if (pCurrentProductTransaction != NULL)
					{
						const bool isRunTotalsLabel( Product::PRINTING_RUN_TOTALS_LABEL == pCurrentProduct->getLabelBeingPrinted() );

						std::string totalPriceString;

						if ( isRunTotalsLabel )
						{
							if ( tracing )
								Debug::trace( "PrinterManager.PostImageField", printerTraceString + " TOTAL_PRICE: have run totals label" );

							Session *session = SessionManager::getInstance()->getPrimaryConsoleSession();
							ProductRunTotal *productRunTotal = session ? session->getProductTotalManager()->getCurrentProductRunTotal() : ((ProductRunTotal*) NULL);

							if ( productRunTotal )
							{
								totalPriceString = pConfig->formatHobPrice(
									productRunTotal->getPrice(),
									useDefaultDecimalSeparator,
									currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0);
							}
						}
						else
						{
							totalPriceString = pConfig->formatHobPrice(
								pCurrentProductTransaction->getTotalPrice(),
								useDefaultDecimalSeparator,
								currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0);
						}

						if ( pConfig->digitsPrecision() == 0 )
							totalPriceString = cleanTrailingCurrencyMarks( totalPriceString );

						pForm->setData((UINT32 *) (totalPriceString.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " TOTAL_PRICE: " + HobString((const char *) (pForm->getData())));
					break;

					// The next three fields are typically pre-imaged fields, since their data can't be changed while the plu is active in operate mode.
					// However, the code to treat them aspost-maged was already here, so I have to leave them here in case there are any label types
					// out in the world that has them as post-imaged.  I don't want them to no longer work.  I did copy these three to updateFormPreImageFields().

					//	This field (TOTAL_TYPE) is used in Ultima only. I am using it here as a special case for Costco.
					//	Costco wants to print an 'R' for rewraps on the label.
					//	This will put the first letter of the total type name in this field.
					// 11/2018, We are changing this functionality to include the string to print in the Transaction Type table (doesn't just have to be the first letter of the transaction type).
				case TOTAL_TYPE: // 38, 0x26
					{
						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + "TOTAL_TYPE");
						if (pCurrentProductTransaction)
						{
								HobString text = pCurrentProductTransaction->getTransactionTypeTextForLabel();
								if (tracing)
									Debug::trace("PrinterManager.PostImageField", printerTraceString + "TOTAL_TYPE text for label = " + text);
								if (text.size())
									pForm->setData((UINT32 *) (text.c_str()));
						}
						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + "TOTAL_TYPE: " + HobString((const char *) (pForm->getData())));
						break;
					}
				case TOTAL_TYPE_NAME: // 39, 0x27
					{
						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + "TOTAL_TYPE_NAME");
						if (pCurrentProductTransaction)
						{
							HobString nameString = pCurrentProductTransaction->getTransactionTypeName();
							if (tracing)
								Debug::trace("PrinterManager.PostImageField", printerTraceString + "TOTAL_TYPE_NAME:  nameString = " + nameString);
							pForm->setData((UINT32 *) (nameString.c_str()));
						}
						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + "TOTAL_TYPE_NAME: " + HobString((const char *) (pForm->getData())));
						break;
					}
				case TOTAL_TYPE_NUMBER: // 52, 0x34
					{
						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + "TOTAL_TYPE_NUMBER");
						if (pCurrentProductTransaction)
						{
							HobString numString;
							if (pCurrentProductTransaction->getTransactionTypeNumber() > 0)
							{
								numString = Integer::toString(pCurrentProductTransaction->getTransactionTypeNumber());
								if (tracing)
									Debug::trace("PrinterManager.PostImageField", printerTraceString + "TOTAL_TYPE_NUMBER:  numString = " + numString);
							}
							pForm->setData((UINT32 *) (numString.c_str()));
						}
						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + "TOTAL_TYPE_NUMBER: " + HobString((const char *) (pForm->getData())));
						break;
					}

				case CURRENT_TIME:	// 0x33
					{
						HobDateTime now;
						pForm->setData((UINT32 *) (now.formatTime(FormatTime_HHMM, pConfig->getTimeDisplayFormat()).c_str())); // FT-612

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + " CURRENT_TIME: " + HobString((const char *) (pForm->getData())));
						break;
					}
				case INDIRECT_LABEL_REFERENCE:  // 0x35
					{
						if (tracing)
							Debug::trace("PrinterManager.embeddedLabels", printerTraceString + " Going to post-image embedded label " +
									inttostring(pForm->getVerticalJustify()) + " with bitmap " + ptrtostring(pForm->getBitMapData()) +
									" and it " + (pForm->isDataPresent() ? "has" : "has no") + " data present");

						const ImageControl* embeddedImageControl = 0;

						int accumItemListPosition = isLabelFormAccumulateItemsItemLabelType( pForm->getVerticalJustify() );
						if (accumItemListPosition != -1)
						{
							Debug::trace("PrinterManager.embeddedLabels", " embedded Label type 177, need to find the correct product ");
							int labelNum = pForm->getVerticalJustify();

							embeddedImageControl = imageControlBlock.getEmbeddedImageControl((UINT16) labelNum);
							Debug::trace("PrinterManager.embeddedLabels", " embedded Label type 177, lblnum=" + inttostring(labelNum) + "  imageControlBlock found is " + ptrtostring(embeddedImageControl));
							if ( embeddedImageControl )
							{
								ProductTransaction *pt = const_cast <ProductTransaction*> (pCurrentProductTransaction);

								ProductTransactionMaster *ptm = pt->getProductTransactionMaster();
								if (ptm)
								{
									Debug::trace("PrinterManager.embeddedLabels", " embedded Label type 177, current product is ProductTransactionMaster " + ptrtostring(pCurrentProductTransaction));

									Product *savedPCurrentProduct = pCurrentProduct;
									const ProductTransaction *savedPCurrentProductTransaction = pCurrentProductTransaction;

									pCurrentProductTransaction = ptm->getTransactionPositionNum(accumItemListPosition);
									Debug::trace("PrinterManager.embeddedLabels", " embedded Label type 177, product transaction to get data from is " + ptrtostring(pCurrentProductTransaction));
									if (pCurrentProductTransaction)
									{
										pCurrentProduct = pCurrentProductTransaction->getProduct();
										updateFormPostImageFields(*embeddedImageControl);
									}

									pCurrentProduct = savedPCurrentProduct;
									pCurrentProductTransaction = (ProductTransaction *)savedPCurrentProductTransaction;
									Debug::trace("PrinterManager.embeddedLabels", " embedded Label type 177, reset current product and transaction back to ProductTransactionMaster " + ptrtostring(pCurrentProductTransaction));
								}
								else
									Debug::trace("PrinterManager.embeddedLabels", " embedded Label type 177, current product is NOT ProductTransactionMaster " + ptrtostring(pCurrentProductTransaction));
							}
						}
						else
						{
							// Recursively post-image any embedded labels
							const ImageControl* embeddedImageControl = imageControlBlock.getEmbeddedImageControl(pForm->getVerticalJustify());
							if ( embeddedImageControl )
								updateFormPostImageFields(*embeddedImageControl);

							if (tracing)
								Debug::trace("PrinterManager.embeddedLabels", printerTraceString + " Completed post-imaging embedded label " +
										inttostring(pForm->getVerticalJustify()) + " with bitmap " + ptrtostring(pForm->getBitMapData()) +
										" and it " + (pForm->isDataPresent() ? "has" : "has no") + " data present");
						}
						break;
					}
				case DISCOUNT_PRICE: // 0x4C - from Quantum (DISCOUNT_TOTAL_PRICE1)
					if ((pCurrentProductTransaction != NULL) && (pCurrentProductTransaction->getDiscTotalPrice1() > 0))
					{
						//										HobString discTotalPrice1String;
						//										pConfig->formatPrice(pCurrentProductTransaction->getDiscTotalPrice1(), discTotalPrice1String);
						std::string discTotalPrice1String = pConfig->formatHobPrice(pCurrentProductTransaction->getDiscTotalPrice1(), useDefaultDecimalSeparator,
								currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0);

						//										if (currencySymbolPrinted == true)
						//										{
						//											discTotalPrice1String.insert(0, pConfig->getMonetarySymbol());
						//										}

						if (pConfig->digitsPrecision() == 0)
							discTotalPrice1String = cleanTrailingCurrencyMarks(discTotalPrice1String);

						pForm->setData((UINT32 *) (discTotalPrice1String.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " DISCOUNT_PRICE: " + HobString((const char *) (pForm->getData())));
					break;
				case DISCOUNT_SAVINGS:		// 0x4D - from Quantum (DISCOUNT_SAVINGS1)
					if ((pCurrentProductTransaction != NULL) && (pCurrentProductTransaction->getDiscTotalPrice1() > 0))
					{
						//										HobString discSavings1String;
						//										pConfig->formatPrice(pCurrentProductTransaction->getDiscTotalSavings1(), discSavings1String);
						std::string discSavings1String = pConfig->formatHobPrice(pCurrentProductTransaction->getDiscTotalSavings1(), useDefaultDecimalSeparator,
								currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0);

						//										if (currencySymbolPrinted == true)
						//										{
						//											discSavings1String.insert(0, pConfig->getMonetarySymbol());
						//										}

						if (pConfig->digitsPrecision() == 0)
							discSavings1String = cleanTrailingCurrencyMarks(discSavings1String);

						pForm->setData((UINT32 *) (discSavings1String.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " DISCOUNT_SAVINGS: " + HobString((const char *) (pForm->getData())));
					break;
				case DISCOUNT_UNIT_PRICE: // 0x4E
					if ((pCurrentProduct != NULL) && (pCurrentProductTransaction != NULL) && (pCurrentProductTransaction->getDiscTotalPrice1() > 0))
					{
						std::string discUnitPrice1String;

						if (pCurrentProduct->isRandomWeight() == true)
						{
							discUnitPrice1String = pConfig->formatHobPrice(pCurrentProductTransaction->getDiscUnitPrice1(), useDefaultDecimalSeparator,
									Configuration::AppendPerWeightPFFlag | (currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0));
							//											pConfig->formatPrice(pCurrentProductTransaction->getDiscUnitPrice1(), discUnitPrice1String, AppendPerWeightText);
						}
						else if (pCurrentProduct->isFixedWeight() == true)
						{
							if (pConfig->toBoolCfg(fieldOffsetCfgPFUnitPricePrinted) == true && pCurrentProductTransaction->getFixedWeight() > 0)
								discUnitPrice1String = pConfig->formatHobPrice(pCurrentProductTransaction->getFixedWeightDiscountUnitPrice1(), useDefaultDecimalSeparator,
										Configuration::AppendPerWeightPFFlag | (currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0));
							//												pConfig->formatPrice(pCurrentProductTransaction->getFixedWeightDiscountUnitPrice1(), discUnitPrice1String, AppendPerWeightText);
						}

						//										if (!discUnitPrice1String.empty() && currencySymbolPrinted == true)
						//										{
						//											discUnitPrice1String.insert(0, pConfig->getMonetarySymbol());
						//										}

						if (pConfig->digitsPrecision() == 0)
							discUnitPrice1String = cleanTrailingCurrencyMarks(discUnitPrice1String);

						pForm->setData((UINT32 *) (discUnitPrice1String.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " DISCOUNT_UNIT_PRICE: " + HobString((const char *) (pForm->getData())));
					break;

				case PORTION_COUNT:			// 0x4F
					{
						if (pCurrentProductTransaction && (pCurrentProductTransaction->getPortions() > 0))
						{
							HobString tempStr;

							tempStr = Integer::toString(pCurrentProductTransaction->getPortions());
							pForm->setData((UINT32 *) (tempStr.c_str()));
						}

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + "PORTION_COUNT: " + HobString((const char *) (pForm->getData())));
						break;
					}
				case AVG_PORTION_COST:		// 0x50
					{
						if (pCurrentProductTransaction && (pCurrentProductTransaction->getPortions() > 0)
								&& (pCurrentProductTransaction->getAvgPortionPrice() > 0))
						{
							HobString tempStr;

							pConfig->formatPrice(pCurrentProductTransaction->getAvgPortionPrice(), tempStr, useDefaultDecimalSeparator);

							if (!tempStr.empty() && currencySymbolPrinted == true)
							{
								tempStr.insert(0, pConfig->getMonetarySymbol());
							}

							if (pConfig->digitsPrecision() == 0)
								tempStr = cleanHobTrailingCurrencyMarks(tempStr);

							pForm->setData((UINT32 *) (tempStr.c_str()));
						}

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + "AVG_PORTION_COST: " + HobString((const char *) (pForm->getData())));
						break;
					}
				case AVG_PORTION_WEIGHT:	// 0x51
					{
						if (pCurrentProductTransaction && (pCurrentProductTransaction->getPortions() > 0)
								&& (pCurrentProductTransaction->getAvgPortionWeight() > 0))
						{
							HobString tempStr;

							pConfig->formatWeight(pCurrentProductTransaction->getAvgPortionWeight(), tempStr, useDefaultDecimalSeparator, AppendWeightText);
							pForm->setData((UINT32 *) (tempStr.c_str()));
						}

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + "AVG_PORTION_WEIGHT: " + HobString((const char *) (pForm->getData())));
						break;
					}
				case AVG_FS_PORTION_COST:	// 0x52
					{
						if (pCurrentProductTransaction && (pCurrentProductTransaction->getPortions() > 0)
								&& (pCurrentProductTransaction->getAvgFreqShopPortionPrice() > 0))
						{
							HobString tempStr;

							pConfig->formatPrice(pCurrentProductTransaction->getAvgFreqShopPortionPrice(), tempStr, useDefaultDecimalSeparator);

							if (!tempStr.empty() && currencySymbolPrinted == true)
							{
								tempStr.insert(0, pConfig->getMonetarySymbol());
							}

							if (pConfig->digitsPrecision() == 0)
								tempStr = cleanHobTrailingCurrencyMarks(tempStr);

							pForm->setData((UINT32 *) (tempStr.c_str()));
						}

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + "AVG_FS_PORTION_COST: " + HobString((const char *) (pForm->getData())));
						break;
					}
				case DISCOUNT_TOTAL_PRICE2:	// 0x6B
					if ((pCurrentProductTransaction != NULL) && (pCurrentProductTransaction->getDiscTotalPrice2() > 0))
					{
						//										HobString discTotalPrice2String;
						//										pConfig->formatPrice(pCurrentProductTransaction->getDiscTotalPrice2(), discTotalPrice2String);
						std::string discTotalPrice2String = pConfig->formatHobPrice(pCurrentProductTransaction->getDiscTotalPrice2(), useDefaultDecimalSeparator,
								currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0);

						//										if (currencySymbolPrinted == true)
						//										{
						//											discTotalPrice2String.insert(0, pConfig->getMonetarySymbol());
						//										}

						if (pConfig->digitsPrecision() == 0)
							discTotalPrice2String = cleanTrailingCurrencyMarks(discTotalPrice2String);

						pForm->setData((UINT32 *) (discTotalPrice2String.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " DISCOUNT_TOTAL_PRICE2: " + HobString((const char *) (pForm->getData())));
					break;
				case DISCOUNT_SAVINGS2:	// 0x6C
					if ((pCurrentProductTransaction != NULL) && (pCurrentProductTransaction->getDiscTotalPrice2() > 0))
					{
						//										HobString discSavings2String;
						//										pConfig->formatPrice(pCurrentProductTransaction->getDiscTotalSavings2(), discSavings2String);
						std::string discSavings2String = pConfig->formatHobPrice(pCurrentProductTransaction->getDiscTotalSavings2(), useDefaultDecimalSeparator,
								currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0);

						//										if (currencySymbolPrinted == true)
						//										{
						//											discSavings2String.insert(0, pConfig->getMonetarySymbol());
						//										}

						if (pConfig->digitsPrecision() == 0)
							discSavings2String = cleanTrailingCurrencyMarks(discSavings2String);

						pForm->setData((UINT32 *) (discSavings2String.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " DISCOUNT_SAVINGS2: " + HobString((const char *) (pForm->getData())));
					break;
				case DISCOUNT_UNIT_PRICE2: // 0x6D
					if ((pCurrentProduct != NULL) && (pCurrentProductTransaction != NULL) && (pCurrentProductTransaction->getDiscTotalPrice2() > 0))
					{
						std::string discUnitPrice2String;

						if (pCurrentProduct->isRandomWeight() == true)
						{
							discUnitPrice2String = pConfig->formatHobPrice(pCurrentProductTransaction->getDiscUnitPrice2(), useDefaultDecimalSeparator,
									Configuration::AppendPerWeightPFFlag | (currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0));
							//											pConfig->formatPrice(pCurrentProductTransaction->getDiscUnitPrice2(), discUnitPrice2String, AppendPerWeightText);
						}
						else if (pCurrentProduct->isFixedWeight() == true)
						{
							if (pCurrentProductTransaction->getFixedWeight() > 0)
								discUnitPrice2String = pConfig->formatHobPrice(pCurrentProductTransaction->getFixedWeightDiscountUnitPrice2(), useDefaultDecimalSeparator,
										Configuration::AppendPerWeightPFFlag | (currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0));
							//												pConfig->formatPrice(pCurrentProductTransaction->getFixedWeightDiscountUnitPrice2(), discUnitPrice2String, AppendPerWeightText);
						}

						//										if (!discUnitPrice2String.empty() && currencySymbolPrinted == true)
						//										{
						//											discUnitPrice2String.insert(0, pConfig->getMonetarySymbol());
						//										}

						if (pConfig->digitsPrecision() == 0)
							discUnitPrice2String = cleanTrailingCurrencyMarks(discUnitPrice2String);

						pForm->setData((UINT32 *) (discUnitPrice2String.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " DISCOUNT_UNIT_PRICE2: " + HobString((const char *) (pForm->getData())));
					break;
				case DISCOUNT_TOTAL_PRICE3:	// 0x6E
					if ((pCurrentProductTransaction != NULL) && (pCurrentProductTransaction->getDiscTotalPrice3() > 0))
					{
						//										HobString discTotalPrice3String;
						//										pConfig->formatPrice(pCurrentProductTransaction->getDiscTotalPrice3(), discTotalPrice3String);
						std::string discTotalPrice3String = pConfig->formatHobPrice(pCurrentProductTransaction->getDiscTotalPrice3(), useDefaultDecimalSeparator,
								currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0);

						//										if (currencySymbolPrinted == true)
						//										{
						//											discTotalPrice3String.insert(0, pConfig->getMonetarySymbol());
						//										}

						if (pConfig->digitsPrecision() == 0)
							discTotalPrice3String = cleanTrailingCurrencyMarks(discTotalPrice3String);

						pForm->setData((UINT32 *) (discTotalPrice3String.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " DISCOUNT_TOTAL_PRICE3: " + HobString((const char *) (pForm->getData())));
					break;

				case DISCOUNT_SAVINGS3:	// 0x6F
					if ((pCurrentProductTransaction != NULL) && (pCurrentProductTransaction->getDiscTotalPrice3() > 0))
					{
						//										HobString discSavings3String;
						//										pConfig->formatPrice(pCurrentProductTransaction->getDiscTotalSavings3(), discSavings3String);
						std::string discSavings3String = pConfig->formatHobPrice(pCurrentProductTransaction->getDiscTotalSavings3(), useDefaultDecimalSeparator,
								currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0);

						//										if (currencySymbolPrinted == true)
						//										{
						//											discSavings3String.insert(0, pConfig->getMonetarySymbol());
						//										}

						if (pConfig->digitsPrecision() == 0)
							discSavings3String = cleanTrailingCurrencyMarks(discSavings3String);

						pForm->setData((UINT32 *) (discSavings3String.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " DISCOUNT_SAVINGS3: " + HobString((const char *) (pForm->getData())));
					break;

				case DISCOUNT_UNIT_PRICE3: // 0x70
					if ((pCurrentProduct != NULL) && (pCurrentProductTransaction != NULL) && (pCurrentProductTransaction->getDiscTotalPrice3() > 0))
					{
						std::string discUnitPrice3String;

						if (pCurrentProduct->isRandomWeight() == true)
						{
							discUnitPrice3String = pConfig->formatHobPrice(pCurrentProductTransaction->getDiscUnitPrice3(), useDefaultDecimalSeparator,
									Configuration::AppendPerWeightPFFlag | (currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0));
							//											pConfig->formatPrice(pCurrentProductTransaction->getDiscUnitPrice3(), discUnitPrice3String, AppendPerWeightText);
						}
						else if (pCurrentProduct->isFixedWeight() == true)
						{
							if (pCurrentProductTransaction->getFixedWeight() > 0)
								discUnitPrice3String = pConfig->formatHobPrice(pCurrentProductTransaction->getFixedWeightDiscountUnitPrice3(), useDefaultDecimalSeparator,
										Configuration::AppendPerWeightPFFlag | (currencySymbolPrinted ? Configuration::UseMonetarySymbolPFFlag : 0));
							//												pConfig->formatPrice(pCurrentProductTransaction->getFixedWeightDiscountUnitPrice3(), discUnitPrice3String, AppendPerWeightText);
						}

						//										if (!discUnitPrice3String.empty() && currencySymbolPrinted == true)
						//										{
						//											discUnitPrice3String.insert(0, pConfig->getMonetarySymbol());
						//										}

						if (pConfig->digitsPrecision() == 0)
							discUnitPrice3String = cleanTrailingCurrencyMarks(discUnitPrice3String);

						pForm->setData((UINT32 *) (discUnitPrice3String.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " DISCOUNT_UNIT_PRICE3: " + HobString((const char *) (pForm->getData())));
					break;
				case REPORT_PAGE_NUMBER: // 0x77
					if (pCurrentProductTransaction != NULL)
					{
						HobString pageNumberString =
							Integer::toString(pCurrentProductTransaction->getReportPageNumber());

						pForm->setData((UINT32 *) (pageNumberString.c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " Report Page Number: " + HobString((const char *) (pForm->getData())));
					break;
				case SELL_BY_TIME:			//	0x78
					if ( pCurrentProductTransaction )
					{
						bool isExpLabel(( Product::PRINTING_NEW_EXPIRATION_LABEL == pCurrentProduct->getLabelBeingPrinted() ) ||
							( Product::PRINTING_EXISTING_EXPIRATION_LABEL == pCurrentProduct->getLabelBeingPrinted() ));

						// FT-1386: allow printing sell by for exiration labels
						// FT-1443: allow printing sell by for exiration labels
						if (( !pCurrentProduct->isDontPrintSellBy() && pConfig->toBoolCfg( fieldOffsetCfgPFSellByDatePrinted ) ) || ( isExpLabel ))
						{
							HobString sellByTimeString;
							HobDateTime theDateTime( isExpLabel ? pCurrentProductTransaction->getExpirationLabelDateTime() : pCurrentProductTransaction->getShelfLife() );

							pConfig->formatTime(theDateTime, sellByTimeString, pConfig->getTimeDisplayFormat());
							pForm->setData((UINT32 *) (sellByTimeString.c_str()));

							if ( tracing )
								Debug::trace("PrinterManager.PostImageField", printerTraceString + " SELL_BY_TIME: " + HobString((const char *) (pForm->getData())));
						}
					}
					break;
				case BEST_BEFORE_TIME:		//	0x79
					if ((pCurrentProductTransaction != NULL) && (pCurrentProduct->isDontPrintBestBefore() == false) && (pConfig->toBoolCfg(fieldOffsetCfgPFProductLifeDatePrinted) == true))
					{
						HobString bestBeforeTimeString;
						pConfig->formatTime(const_cast <ProductTransaction*> (pCurrentProductTransaction)->getProductLife(), bestBeforeTimeString, pConfig->getTimeDisplayFormat());
						pForm->setData((UINT32 *) (bestBeforeTimeString.c_str()));

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + " BEST_BEFORE_TIME: " + HobString((const char *) (pForm->getData())));
					}
					break;
				case PACKED_ON_TIME:		//	0x7A
					{
						HobDateTime now;
						HobString packedOnTimeString;
						pConfig->formatTime(now, packedOnTimeString, pConfig->getTimeDisplayFormat());
						pForm->setData((UINT32 *) (packedOnTimeString.c_str()));

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + " PACKED_ON_TIME: " + HobString((const char *) (pForm->getData())));
						break;
					}
				case SELL_BY_DAY_OF_WEEK:			//	0x7B
					if (pCurrentProductTransaction != NULL)
					{
						HobDateTime theDateTime = (pCurrentProduct->getLabelBeingPrinted() == Product::PRINTING_NEW_EXPIRATION_LABEL ||
								pCurrentProduct->getLabelBeingPrinted() == Product::PRINTING_EXISTING_EXPIRATION_LABEL)
							? pCurrentProductTransaction->getExpirationLabelDateTime() : pCurrentProductTransaction->getShelfLife();

						HobString dowString = theDateTime.getDayOfWeekString(HobDateTime::_UsingPrimaryLanguageLocale_);
						pForm->setData((UINT32 *) (dowString.c_str()));

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + " SELL_BY_DAY_OF_WEEK: " + HobString((const char *) (pForm->getData())));
					}
					break;
				case OPERATOR_NUMBER:		  // 0x11
					if (pCurrentProductTransaction != NULL)
					{
						pForm->setData((UINT32 *) (pCurrentProductTransaction->getOperatorId().c_str()));

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + " OPERATOR_NUMBER: " + HobString((const char *) (pForm->getData())));
					}
					break;
				case OP_ACCESS_LEVEL:          // 0x11
					if (pCurrentProductTransaction != NULL)
					{
						if (pCurrentProductTransaction->getOperatorAccessLevel().size() > 0)
							pForm->setData((UINT32 *) (pCurrentProductTransaction->getOperatorAccessLevel().c_str()));

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + " OP_ACCESS_LEVEL: " + HobString((const char *) (pForm->getData())));
					}
					break;
				case OPERATOR_NAME:				// 0x12
					if (pCurrentProductTransaction != NULL)
					{
						pForm->setData((UINT32 *) (pCurrentProductTransaction->getOperatorName().c_str()));

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + " OPERATOR_NAME: " + HobString((const char *) (pForm->getData())));
					}
					break;
				case TOTAL_ITEMS:          // 0x3C
					if (pCurrentProductTransaction != NULL)
					{
						HobString totalItemsStr = Integer::toString(pCurrentProductTransaction->getTotalItems());
						pForm->setData((UINT32 *) totalItemsStr.c_str());

						if (tracing)
							Debug::trace("PrinterManager.PostImageField", printerTraceString + " TOTAL_ITEMS: " + HobString((const char *) (pForm->getData())));
					}
					break;
				case DESCRIPTION:		// 0x01     NEEDED FOR RENDERING THE ACCUMULATED ITEMS LABEL!  THE DESCRIPTION IS POST-IMAGE IN LABEL TYPE 177.
					if (pCurrentProduct != NULL)
					{
						pForm->setData((UINT32 *) (pCurrentProduct->getDescription().c_str()));
					}

					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " DESCRIPTION: " + HobString((const char *) (pForm->getData())));
					break;

                case UPC_NUMBER:
					if ( pCurrentProduct )
					{
						const bool isRunTotalsLabel( Product::PRINTING_RUN_TOTALS_LABEL == pCurrentProduct->getLabelBeingPrinted() );

						const BarCodeNumber barCodeNumber( pCurrentProduct->getBarCodeNum() );
						const HobString eanCode( barCodeNumber.getEANCode() );
						const HobString upcNumber( barCodeNumber.getUPCNumber() );

						HobString s;

						if ( isRunTotalsLabel )
						{
							if ( tracing )
								Debug::trace( "PrinterManager.PostImageField", printerTraceString + " UPC_NUMBER: have run totals label" );

							if ( upcNumber.length() )
								s = upcNumber;
							else
								s = eanCode;
						}
						else
						{
							if ( eanCode.length() )
								s = eanCode;
							else
								s = upcNumber;
						}

						pForm->setData( (UINT32*) s.c_str() );
					}

					if ( tracing )
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " UPC_NUMBER: " + HobString((const char *) (pForm->getData())));

					break;

				case PRICE_CHANGED_LABEL_INDICATOR:
				{
					if (!(pConfig->toStringCfg(fieldOffsetCfgPriceChangedLabelIndicator).empty()) && pCurrentProductTransaction != NULL)
					{
						//long origUnitPrice = pCurrentProductTransaction->getOldUnitPrice();
						//long finalUnitPrice = pCurrentProductTransaction->getUnitPrice();
						if ( pCurrentProductTransaction->getUnitPrice() != pCurrentProductTransaction->getOldUnitPrice() )
							pForm->setData( (UINT32 *) pConfig->toStringCfg(fieldOffsetCfgPriceChangedLabelIndicator).c_str() );
					}
					if (tracing)
						Debug::trace("CANPrinterManager.PostImageField", printerTraceString + " PRICE_CHANGED_LABEL_INDICATOR: " + HobString((const char *)(pForm->getData())));
					break;
				}

				default:
					if (tracing)
						Debug::trace("PrinterManager.PostImageField", printerTraceString + " Unknown Form Type");
					break;
			}
		}
	}
	
	if (tracing)
		Debug::trace("PrinterManager.PostImageField", " LEAVING updateFormPostImageFields() ");

}

std::string PrinterManager::cleanTrailingCurrencyMarks(std::string priceStr)
{
	// dont know if I want to do this for everybody yet, but it was requested when international rounding
	// was added back in (up/down rounding) ( I might have to limit this to international. Maybe 2nd language?

	// get rid of trailing . or , when there is no digits following the Monetary decimal digits
	std::string newPrice = "";
	std::string::size_type pos1 = priceStr.find('.');
	std::string::size_type pos2 = priceStr.find(',');

	if (pos1 != std::string::npos)
	{
		std::string tmpPrice = priceStr;
		tmpPrice.erase(std::remove(tmpPrice.begin(), tmpPrice.end(), '.'), tmpPrice.end());
		newPrice = tmpPrice;
	}
	else if (pos2 != std::string::npos)
	{
		std::string tmpPrice = priceStr;
		tmpPrice.erase(std::remove(tmpPrice.begin(), tmpPrice.end(), ','), tmpPrice.end());
		newPrice = tmpPrice;
	}

	return newPrice;
}

HobString PrinterManager::cleanHobTrailingCurrencyMarks(HobString priceStr)
{

	// dont know if I want to do this for everybody yet, but it was requested when international rounding
	// was added back in (up/down rounding) ( I might have to limit this to international. Maybe 2nd language?

	// get rid of trailing . or , when there is no digits following the Monetary decimal digits
	HobString newPrice = "";
	std::string::size_type pos1 = priceStr.find('.');
	std::string::size_type pos2 = priceStr.find(',');
	
	if (pos1 != std::string::npos)
	{
		std::string tmpPrice = priceStr;
		tmpPrice.erase(std::remove(tmpPrice.begin(), tmpPrice.end(), '.'), tmpPrice.end());
		newPrice = tmpPrice;
	}
	else if (pos2 != std::string::npos)
	{
		std::string tmpPrice = priceStr;
		tmpPrice.erase(std::remove(tmpPrice.begin(), tmpPrice.end(), ','), tmpPrice.end());
		newPrice = tmpPrice;
	}

	return newPrice;
}

void PrinterManager::dispatchLabelComplete(const ProductTransaction *pPT, HobRecordNum transNum, bool labelOk)
{
	Debug::trace("PrinterManager.Product", printerTraceString +  + "dispatchLabelComplete():  Product Transaction: " + HobString(Hex::toString((long long) pPT)) + ",  transNum = " + HobString(Long::toString(transNum)) + ",  labelOk = " + HobString(Bool::toString(labelOk)));

	if (pPT != NULL)
	{
		if ( isFakePrinterManager() )
			labelOk = true;
		PrinterLabelComplete plc(this, myPrinterType, transNum, pPT, labelOk);
		dispatchEvent (&plc);
	}
}

void PrinterManager::dispatchLabelReady(const ProductTransaction *pPT, HobRecordNum transNum, UINT8 packId)
{
	if (pPT != NULL)
	{
		PrinterLabelReady prl(this, myPrinterType, transNum, pPT, packId);
		dispatchEvent(&prl);
	}

	++printerCounts_;
	Debug::trace("PrinterManager.CodeChecker", "printerCounts_: " + Integer::toString(printerCounts_));
	
	//BEFORE WE ENABLE WE NEED TO FIX THIS (make sure dot wear is enabled) and we need to make sure the user doesn't print while running dot wear
	#if 0
	if(printerCounts_ % codeCheckerFrequency_ == 0){
		PrinterRequestHeadDotStatus stat(this, myPrinterType, 0 , prKind);
		dispatchEvent (&stat);
		Debug::trace("PrinterManager.CodeChecker", "Requesting printhead status.");
	}
	#endif
	
}

void PrinterManager::dispatchLabelTaken(const ProductTransaction *pPT, HobRecordNum transNum, UINT8 packId)
{
	if (pPT != NULL)
	{
		PrinterLabelTaken plt(this, myPrinterType, transNum, pPT, packId);
		dispatchEvent (&plt);
	}
}

void PrinterManager::updateHorizontalOffset()
{
	if (printLabelCentered)
	{
		centeringOffset = (printheadSize - stockLoaded.getWidthDots()) / 2;
	}
	else
	{
		centeringOffset = 0;
	}

	Debug::trace("PrinterManager.LabelWidth", printerTraceString + " centeringOffset: " + HobString(Integer::toString(centeringOffset)));

	for (int i = 0; i < (short)MaxPrinterTypes; i++)
		if ( imageControlBlock[i] )
		{
			imageControlBlock[i]->setCenteringOffset(centeringOffset);
			imageControlBlock[i]->setHorizFormOffset((UINT16) pConfig->getHorizLabelSkew(myPrinterType));
		}
}

void PrinterManager::createCheckerboardLabel(UINT32 startingRow, UINT32 length)
{
	Debug::trace("PrinterManager.TestLabel", printerTraceString +
			" createCheckerboardLabel(): " + HobString(Integer::toString(length)));

	UINT16 rowByteWidth = printheadSize / 8;
	UINT16 leftMostBit = centeringOffset + _edgeMargin_ + (UINT16) pConfig->getHorizLabelSkew(myPrinterType);
	UINT16 rightMostBit = leftMostBit + stockLoaded.getWidthDots() - _edgeMargin_*2;
	UINT16 paddedRowByteWidth = rowByteWidth + 8;

	Debug::trace("PrinterManager.TestLabel", " createCheckerboardLabel(): " + printerTraceString + " rowByteWidth: " + HobString(Integer::toString(rowByteWidth)));
	Debug::trace("PrinterManager.TestLabel", " createCheckerboardLabel(): " + printerTraceString + " leftMostBit: " + HobString(Integer::toString(leftMostBit)));
	Debug::trace("PrinterManager.TestLabel", " createCheckerboardLabel(): " + printerTraceString + " rightMostBit: " + HobString(Integer::toString(rightMostBit)));
	Debug::trace("PrinterManager.TestLabel", " createCheckerboardLabel(): " + printerTraceString + " paddedRowByteWidth: " + HobString(Integer::toString(paddedRowByteWidth)));

	if (rightMostBit > printheadSize)
	{
		rightMostBit = printheadSize;
		Debug::trace("PrinterManager.TestLabel", " createCheckerboardLabel(): " + printerTraceString + " NOW rightMostBit = " + HobString(Integer::toString(rightMostBit)));
	}

	UINT8 * pattern1 = new UINT8 [paddedRowByteWidth]; 
	UINT8 * pattern2 = new UINT8 [paddedRowByteWidth]; 
	UINT8 patternSize = 16;

	memset(pattern1, '\0', paddedRowByteWidth ); 
	memset(pattern2, '\0', paddedRowByteWidth ); 

	UINT16 i = leftMostBit;
	while (i < rightMostBit)
	{
		UINT16 iMax = ((i+patternSize) < rightMostBit) ? i+patternSize : rightMostBit;
		Debug::trace("PrinterManager.TestLabel", " createCheckerboardLabel(): " + printerTraceString + " setting bits: " + HobString(Integer::toString(i)) + " -- " + HobString(Integer::toString(iMax)));
		bitSet(i, iMax - i, pattern1);
		i = i + 2*patternSize;
	}

	Debug::trace("PrinterManager.TestLabel", printerTraceString + " pattern1: 0x" + HobString(Hex::toString((const char *) pattern1, paddedRowByteWidth)));

	i = leftMostBit + patternSize;
	while (i < rightMostBit)
	{
		UINT16 iMax = ((i+patternSize) < rightMostBit) ? i+patternSize : rightMostBit;
		Debug::trace("PrinterManager.TestLabel", " createCheckerboardLabel(): " + printerTraceString + " setting bits: " + HobString(Integer::toString(i)) + " -- " + HobString(Integer::toString(iMax)));
		bitSet(i, iMax - i, pattern2);
		i = i + 2*patternSize;
	}

	Debug::trace("PrinterManager.TestLabel", " createCheckerboardLabel(): " + printerTraceString + " pattern2: 0x" + HobString(Hex::toString((const char *) pattern2, paddedRowByteWidth)));

	UINT8 *pTemp = (UINT8 *) testLabelData;
	UINT32 squareHeight = 16;

	UINT32 rowCount = startingRow;
	bool isPattern1 = true;
	
	Debug::trace("PrinterManager.TestLabel", " createCheckerboardLabel(): " + printerTraceString + " rowCount: " + HobString(Integer::toString(rowCount)));
	Debug::trace("PrinterManager.TestLabel", " createCheckerboardLabel(): " + printerTraceString + " length: " + HobString(Integer::toString(length)));

	while (rowCount < length)
	{
		if (isPattern1)
		{
			memcpy((UINT8 *) (pTemp + rowCount * rowByteWidth), pattern1, rowByteWidth);
		}
		else
		{
			memcpy((UINT8 *) (pTemp + rowCount * rowByteWidth), pattern2, rowByteWidth);
		}

		rowCount++;

		if ((rowCount % squareHeight) == 0)
		{
			isPattern1 = !isPattern1;
		}
	}
	delete [] pattern1; 
	delete [] pattern2; 


	if ( Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgISaveLabel) )
	{
		std::string filenameString;

#if defined(PAL_WIN32_RUN)
		filenameString = string("C:\\Program Files\\Hobart\\checkerboardLabel");
#else
		filenameString = string("/etc/hobart/checkerboardLabel");
#endif
		filenameString.append(".bmp");

		Debug::trace("PrinterManager.TestLabel", "PrinterManager::createCheckerboardLabel - before saving label to file: " + filenameString);

		BitMap *checkerBitMap = new BitMap();
		checkerBitMap->setBitMapBase(testLabelData);

		//checkerBitMap->setBitMapPitch(56);
		//INT16 bitMapPitch = (rightMostBit + 15)/16;
		INT16 bitMapPitch = ((rightMostBit - leftMostBit - 1) / 16 + 1);

		checkerBitMap->setBitMapPitch(bitMapPitch);
		checkerBitMap->setBitMapBounds(leftMostBit, rightMostBit, 0, length);
		Hobart::CRendererUtil::SaveBitmapToFile( checkerBitMap, filenameString );

		Debug::trace("PrinterManager.TestLabel", "PrinterManager::createCheckerboardLabel - back from saving label to file: " + filenameString);
	}
}

void PrinterManager::createVerticalLinesLabel(UINT32 startingRow, UINT32 length)
{
	Debug::trace("PrinterManager.TestLabel", printerTraceString +
			" createVerticalLinesLabel: " + HobString(Integer::toString(length)));

	// 8 dots per mm
	UINT8 *pTemp = (UINT8 *) testLabelData;
	//	const UINT16 pattern[8] = { 0x80, 0x40, 0x20, 0x10, 0x10, 0x20, 0x40, 0x80 };
	const UINT16 pattern[8] = { 0xC0, 0xE0, 0xC0, 0xE0, 0xC0, 0xE0, 0xC0, 0xE0 };

	UINT32 rowByteWidth = printheadSize / 8;
	// Change from dots to bytes.
	UINT32 leftMarginByte =  (centeringOffset + _edgeMargin_ + pConfig->getHorizLabelSkew(myPrinterType)) / 8;
	UINT32 rightMarginByte = (centeringOffset + pConfig->getHorizLabelSkew(myPrinterType) + stockLoaded.getWidthDots() - _edgeMargin_) / 8;

	Debug::trace("PrinterManager.TestLabel", printerTraceString + " rowByteWidth: " + HobString(Integer::toString(rowByteWidth)));
	Debug::trace("PrinterManager.TestLabel", printerTraceString + " leftMarginByte: " + HobString(Integer::toString(leftMarginByte)));
	Debug::trace("PrinterManager.TestLabel", printerTraceString + " rightMarginByte: " + HobString(Integer::toString(rightMarginByte)));

	UINT32 rowCount = startingRow;
	while (rowCount < length)
	{
		// fill in the row!!
		UINT32 startByte = leftMarginByte + (rowCount * rowByteWidth);
		UINT32 lastByte = rightMarginByte + (rowCount * rowByteWidth);

		for (UINT32 i = startByte; i < lastByte; i++)
		{
			pTemp[i] = (UINT8)pattern[i%8];
		}

		rowCount++;
	}
}

void PrinterManager::createHorizontalBarsLabel(UINT32 startingRow, UINT32 length)
{
	Debug::trace("PrinterManager.TestLabel", printerTraceString +
			" createHorizontalBarsLabel: " + HobString(Integer::toString(length)));

	UINT16 leftMostBit = centeringOffset + _edgeMargin_ + (UINT16) pConfig->getHorizLabelSkew(myPrinterType);

	UINT16 rightMostBit = leftMostBit + stockLoaded.getWidthDots() - _edgeMargin_*2;

	if (rightMostBit > printheadSize)
	{
		rightMostBit = printheadSize;
	}

	UINT32 barHeight = 1;
	UINT32 blankRowHeight = 2;
	UINT8 *pTemp = (UINT8 *) testLabelData;
	UINT32 printRowByteWidth = printheadSize / 8;

	UINT32 rowCount = startingRow ;
	while (rowCount < length)
	{
		for (UINT32 j = rowCount; ((j < (rowCount + barHeight)) && (j < (length))); j++)
		{
			bitSet(leftMostBit, rightMostBit - leftMostBit, (UINT8 *) &pTemp[j * printRowByteWidth]);
		}

		rowCount += (barHeight + blankRowHeight);

		// MAKE THE BLANK ROW 1 DOT LARGER THAN THE PREVIOUS BLANK ROW
		blankRowHeight++;
		// MAKE THE NEXT DARK ROW 2 DOTS LARGER THAN THE PREVIOUS DARK ROW
		barHeight += 2;
	}
}

void PrinterManager::createHighPowerLabel(UINT32 /* startingRow */, UINT32 length)
{
	Debug::trace("PrinterManager.TestLabel", printerTraceString +
			" createHorizontalBarsLabel: " + HobString(Integer::toString(length)));

	createCheckerboardLabel(0, 203);

	for (UINT32 i = 203; i < 353; i++)
	{
		createHorizontalBarsLabel(i,i+1);
	}

	createCheckerboardLabel(353, length);
}

void PrinterManager::createLabelSensorLabel( UINT32 startingRow, UINT32 length )
{
	Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel: START startingRow: " + HobString(Integer::toString(startingRow)));
	Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel: START length: " + HobString(Integer::toString(length)));
	Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel: START testLabelDataSize: " + HobString(Integer::toString(testLabelDataSize)));

	// ---- //

    unsigned char* pImage = NULL;
    unsigned int imageSize;

#if defined(PAL_WIN32_RUN)
	const std::string labelImagePath = string("C:\\Program Files\\Hobart\\labels\\");
#else
	const std::string labelImagePath = "/etc/hobart/";
#endif

    const std::string sensorLabel = "labelSensorTestLabel.bmp";

    std::string file = labelImagePath + sensorLabel;
    long fileSize = 0;

    // read test label images into image buffers
    std::ifstream f( file.data(), std::ios::binary );

    if ( !f.is_open() )
    {
        Debug::trace("PrinterManager.TestLabel", "creatLabelSensorLabel() Critical error: could not open file " + file);
        return;
    }

	// determine size of file
	f.seekg( 0, std::ios::end );
	fileSize = f.tellg();								// fileSize = 34166

	Debug::trace("PrinterManager.TestLabel", "creatLabelSensorLabel() fileSize size: " + Integer::toString(fileSize));

	if ( fileSize <= 0 )
		Debug::trace("PrinterManager.TestLabel", "creatLabelSensorLabel() Critical error file size is 0: " + file);
	else
	{
		//allocate space for our label images
		pImage = new unsigned char[fileSize];			// unsigned char = 1byte

		if ( pImage )
		{
			f.seekg( 0, std::ios::beg );				//back to the start of file
			f.read( (char*) pImage, fileSize );			// read whole file
		}
	}

	f.close();

    if ( !pImage )
        return;

    // get pointers to bitmap headers
    BITMAPFILEHEADER *fileHeader = (BITMAPFILEHEADER*) pImage;
    BITMAPINFOHEADER *infoHeader = (BITMAPINFOHEADER*) &pImage[sizeof(BITMAPFILEHEADER)];

    imageSize = fileSize - fileHeader->bfOffBits;		// calculate the size of our sensor label to print minus the header

    Debug::trace("PrinterManager.TestLabel", "creatLabelSensorLabel() Image size: " + Integer::toString(imageSize));

    // make sure mono bitmap
    if (( 1 != infoHeader->biPlanes ) || ( 1 != infoHeader->biBitCount ))
    {
        Debug::trace("PrinterManager.TestLabel", "creatLabelSensorLabel() Not a monochrome image!");

    	delete pImage;
        return;
    }

    // get image info
    unsigned int imageWidth = infoHeader->biWidth;
    unsigned int imageHeight = infoHeader->biHeight;

    unsigned int imageScanLineWidth = 8 * (imageSize / imageHeight);

    Debug::trace("PrinterManager.TestLabel", "creatLabelSensorLabel() Image width: " + Integer::toString(imageWidth));
    Debug::trace("PrinterManager.TestLabel", "creatLabelSensorLabel() Image height: " + Integer::toString(imageHeight));
    Debug::trace("PrinterManager.TestLabel", "creatLabelSensorLabel() Image scan line width: " + Integer::toString(imageScanLineWidth));

    if (( imageScanLineWidth < imageWidth ) || ( 0 != (imageSize % imageHeight )))
    {
        Debug::trace("PrinterManager.TestLabel", "creatLabelSensorLabel() Bad image dimensions!");

    	delete pImage;
        return;
    }

	// ---- //

	UINT16 rowByteWidth = printheadSize / 8;
	UINT16 leftMostBit = centeringOffset + _edgeMargin_ + (UINT16)pConfig->getHorizLabelSkew(myPrinterType);
	UINT16 rightMostBit = leftMostBit + stockLoaded.getWidthDots() - _edgeMargin_ * 2;

	Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel(): rowByteWidth: " + HobString(Integer::toString(rowByteWidth)));
	Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel(): leftMostBit: " + HobString(Integer::toString(leftMostBit)));
	Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel(): rightMostBit: " + HobString(Integer::toString(rightMostBit)));

	if ( rightMostBit > printheadSize )
	{
		rightMostBit = printheadSize;
		Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel():  NOW rightMostBit = " + HobString(Integer::toString(rightMostBit)));
	}

	UINT8* pImagePixels = &pImage[fileHeader->bfOffBits];
	UINT8 *pLabel = (UINT8*) testLabelData;

	// copy in bitmap image bits into our label buffer
	for ( unsigned int y( length ); startingRow <= y; --y )
	{
		if ( imageHeight <= y )
			continue;

		for ( unsigned int x( leftMostBit ); x <= rightMostBit; ++x )
		{
			if ( imageWidth <= x )
				break;

			UINT8 *imagePtr = &pImagePixels[((imageHeight-y-1) * (imageScanLineWidth/8)) + x/8];
			UINT8 imageMask = 0x80 >> (x%8);

			if ( imageMask & imagePtr[0] )
			{
				UINT8 *labelPtr = &pLabel[(y * rowByteWidth) + x/8];

				labelPtr[0] |= imageMask;
			}
		}
	}

	delete pImage;
	
	// ---- //
	
	// Now save the file off just to see what it does - right now it's printing junk to the file.  Unreadable blurs
	std::string filenameString;
	
#if defined(PAL_WIN32_RUN)
	filenameString = string("C:\\Hobart\\labels\\labelSensorTestLabelSaved");
#else
	filenameString = string("/etc/hobart/labelSensorTestLabelSaved");
#endif

	filenameString.append(".bmp");

	BitMap *sensorBitMap = new BitMap();												// Standard
	sensorBitMap->setBitMapBase(testLabelData);											// Standard

//	INT16 bitMapPitch = ((rightMostBit - leftMostBit - 1) / 16 + 1);					// Standard

	Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel(): SAVING TO FILE");
	Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel(): leftMostBit: " + HobString(Integer::toString(leftMostBit)));
	Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel(): rightMostBit: " + HobString(Integer::toString(rightMostBit)));
//	Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel(): bitMapPitch: " + HobString(Integer::toString(bitMapPitch)));
	Debug::trace("PrinterManager.TestLabel", " createLabelSensorLabel(): length: " + HobString(Integer::toString(length)));

	sensorBitMap->setBitMapPitch(rowByteWidth /*bitMapPitch*/);                                          // Standard
	sensorBitMap->setBitMapBounds(leftMostBit, rightMostBit, 0, length);                // Standard
	Hobart::CRendererUtil::SaveBitmapToFile(sensorBitMap, filenameString);              // Standard

	Debug::trace("PrinterManager.TestLabel", "PrinterManager::createLabelSensorLabel() - finished saving label to file: " + filenameString);
}

void PrinterManager::flipLabelBufferHeelForBMPImage(int size)
{

	if (pBmpLabelBuffer != NULL)
	{
		UINT16 *pStart = pBmpLabelBuffer;
		Debug::trace("PrinterManager.TestLabel", "flipLabelBufferHeelForBMPImage() START");
		Debug::trace("PrinterManager.TestLabel", "flipLabelBufferHeelForBMPImage() pBmpLabelBuffer: " + Hex::toString((long long)pBmpLabelBuffer));
		Debug::trace("PrinterManager.TestLabel", "size: " + Integer::toString(size));
		Debug::trace("PrinterManager.TestLabel", "printheadSize: " + Integer::toString(printheadSize));
		
		memset((void *)pBmpLabelBuffer, 0, size);

		int numLines = (size / (printheadSize / 8 ) );

#if defined ( __linux__ )
		int lineSize = printheadSize / 16;
		unsigned char line[lineSize] = { 0 };
#else
		int lineSize = 28;
		unsigned char line[28] = { 0 };
#endif
		Debug::trace("PrinterManager.TestLabel", "flipLabelBufferHeelForBMPImage() numLines: "    + Integer::toString(numLines));
		Debug::trace("PrinterManager.TestLabel", "flipLabelBufferHeelForBMPImage() lineSize: " + Integer::toString(lineSize));
		Debug::trace("PrinterManager.TestLabel", "flipLabelBufferHeelForBMPImage() labelDataSize: " + Integer::toString(labelDataSize));
		Debug::trace("PrinterManager.TestLabel", "flipLabelBufferHeelForBMPImage() end of line: " + Hex::toString((long long)testLabelData));

		//we are copying the image from the top down and placing in the heel buffer from the bottom up!
		UINT16 *pImageStart = (UINT16 *)((long long)testLabelData - (long long)labelDataSize);
		//start the buffer on the first line at the bottom of the image.
		pBmpLabelBuffer = (UINT16 *)((long long)pBmpLabelBuffer + (long long)size);
		
		Debug::trace("PrinterManager.TestLabel", "flipLabelBufferHeelForBMPImage() BEFORE for (int x = 0; x < numLines; x++): #1" );
		for (int x = 0; x < numLines; x++)
		{
			for (int i = 0; i < lineSize; i++)
			{
				*pBmpLabelBuffer++ = *pImageStart++;
			}
			pBmpLabelBuffer -= (2 * lineSize);
		}
		
		pBmpLabelBuffer = pStart;


#if 0	//add for debugging image.
		std::string str = "";
		for (int x = 0; x < numLines; x++)
		{
			for (int i = 0; i < lineSize; i++) {

				str.append(Hex::toString(*pStart++));
			}
		}
		Debug::trace("PrinterManager.TestLabel", "flipped pBmpLabelBuffer as is: " + str);
#endif
	}

	Debug::trace("PrinterManager.TestLabel", "flipLabelBufferHeelForBMPImage() END");
}

bool PrinterManager::createTestLabel(PrTestLabelType newTestLabelType)
{
	Util::CLockCriticalSection lock( printerManagerCS );

	bool rv = true; // did we create a test label?

	Debug::trace("PrinterManager.TestLabel", printerTraceString + "createTestLabel(): " + HobString(Integer::toString(newTestLabelType)));

	if (testLabelType != newTestLabelType)
	{
		Debug::trace("PrinterManager.TestLabel", printerTraceString + "createTestLabel(): testLabelType != newTestLabelType" );
		if (stockLoaded.getLength() > 0)
		{
			Debug::trace("PrinterManager.TestLabel", printerTraceString + "createTestLabel(): if stockLoaded > 0");
			if ((testLabelType != PrVoidLabel) && (testLabelData != NULL))
			{
				Debug::trace("PrinterManager.TestLabel", printerTraceString + " deleting old testLabeldata");
				delete [] testLabelData;
				testLabelData = NULL;
			}

			if (newTestLabelType != PrVoidLabel)
			{
				Debug::trace("PrinterManager.TestLabel", printerTraceString + "createTestLabel(): testLabelType != PrVoidLabel");
				if (!testLabelPrintInProgress)
				{
					Debug::trace("PrinterManager.TestLabel", printerTraceString + "createTestLabel(): !testLabelPrintInProgress");
					UINT16 lengthLoaded = (printerStockMetrics.isContinuousStock() ? (UINT16)Length3_000_Lines : stockLoaded.getLength());
					testLabelDataSize = lengthLoaded * (printheadSize / 16);
					Debug::trace("PrinterManager.TestLabel", "createTestLabel() lengthLoaded: " + HobString(Integer::toString(lengthLoaded)));
					Debug::trace("PrinterManager.TestLabel", "createTestLabel() printheadSize: " + HobString(Integer::toString(printheadSize)));
					Debug::trace("PrinterManager.TestLabel", "createTestLabel() testLabelDataSize: " + HobString(Integer::toString(testLabelDataSize)));

					if (testLabelDataSize > 0)
					{
						Debug::trace("PrinterManager.TestLabel", printerTraceString + "createTestLabel(): testLabelDataSize > 0");
						//KPC needs to know the label size before we start printing.
						if( ( prKind == KPCPrinter ) || ( prKind == SSKPCPrinter ) ) {
							Debug::trace("PrinterManager.LabelTransfer", "PrinterLabelImageSize: " + Integer::toString( ( testLabelDataSize * 2 ) ) );
							PrinterLabelImageSize msg( this, myPrinterType, ( testLabelDataSize * 2 ) );

							const char *pCC = msg.serialize();
							pMyPrinter->send( pCC, msg.getSerializedLength() );
						}

						testLabelData = new UINT16[testLabelDataSize];
						if (testLabelData)
						{
							Debug::trace("PrinterManager.TestLabel", printerTraceString + "createTestLabel(): testLabelData");
							memset(testLabelData, '\0', testLabelDataSize * 2);

							switch (newTestLabelType)
							{
								case PrHorizontalBars:
									createHorizontalBarsLabel(_edgeMargin_, lengthLoaded - _edgeMargin_*2);
									break;
								case PrVerticalLines:
									createVerticalLinesLabel(_edgeMargin_, lengthLoaded - _edgeMargin_*2);
									break;
								case PrHighPower:
									createHighPowerLabel(0, lengthLoaded);
									break;
								case PrLabelSensorLabel:
									Debug::trace("PrinterManager.PrLabelSensorLabel", " CreateTestLabel() testLabeldata: " + HobString(Integer::toString(testLabelDataSize)));
									
									// I thought this would initialize the buffer but it weirdly crashes the scale first time in
									//if ( testLabelData != NULL )
									//{
									//	Debug::trace("PrinterManager.TestLabel", printerTraceString + " deleting old testLabeldata IN CASE");	// first time in, the one at the top doesnt get hit
									//	delete[] testLabelData;
									//	testLabelData = NULL;
									//}

									//createLabelSensorLabel(0, lengthLoaded);                              // either way didn't seem to matter
									createLabelSensorLabel(_edgeMargin_, lengthLoaded - _edgeMargin_ * 2);  // offset everything
									
									break;
								case PrCheckerboard:
								default:
									createCheckerboardLabel(_edgeMargin_, lengthLoaded - _edgeMargin_*2);
									break;
							}

							testLabelType = newTestLabelType;
						}
						else
						{
							rv = false;
							Debug::trace("PrinterManager.TestLabel", printerTraceString + " new failed! we have a null pointer.");
						}
					}
					else
					{
						rv = false;
						Debug::trace("PrinterManager.TestLabel", printerTraceString + " size of test label out of bounds.");
					}
				}
				else
				{
					rv = false;
					Debug::trace("PrinterManager.TestLabel", printerTraceString + " label currently printing. label data locked down.");
				}
			}
		}
		else
		{
			Debug::trace("PrinterManager.TestLabel", printerTraceString + " stock loaded length == 0.");
			rv = false;
		}
	}

	return rv;
}

UINT16 *PrinterManager::getTestLabelData() const
{
	Util::CLockCriticalSection lock( printerManagerCS );
	return testLabelData;
}

bool PrinterManager::isCutterInstalledAndEnabled() const
{
	return (( isCutterInstalled ) && ( isCutterEnabled ));
}

void PrinterManager::printTestLabel()
{
	Debug::trace("PrinterManager.PrLabelSensorLabel", " printTestLabel() START labelDataSize: " + HobString(Integer::toString(labelDataSize)));
	Debug::trace("PrinterManager.PrLabelSensorLabel", " printTestLabel(): So does this get called in the run loop and if we have data we print?" );

	if (testLabelType == PrVoidLabel)
	{
		labelData = voidLabelBits;
		labelDataSize = sizeof(voidLabelBits);
	}
	else
	{
		labelData = testLabelData;
		labelDataSize = testLabelDataSize * 2; // testLabelDataSize is in Words!!!
	}

	linesToPrint = labelDataSize / (printheadSize / 8);

	Debug::trace("PrinterManager.LabelInfo", printerTraceString + " labelDataSize: : " + HobString(Integer::toString(labelDataSize)));
	Debug::trace("PrinterManager.LabelInfo", printerTraceString + " linesToPrint: : " + HobString(Integer::toString(linesToPrint)));

	sendPrinterEnableTakeup();
	sendPrinterFlush();

	if (printHeelFirst == true)
	{
		labelData = (UINT16 *) ((long long) labelData + (long long) labelDataSize);
	}

	Debug::trace("PrinterManager.PrLabelSensorLabel", " printTestLabel() CALL sendLabelData()");
	sendLabelData();

	printerState = PrinterReadyToXfer;
	Debug::trace("PrinterManager.StateMachine", printerTraceString + " PrinterReadyToXfer");

	Debug::trace("PrinterManager.PrLabelSensorLabel", " printTestLabel() DONE ");
}

void PrinterManager::receivedPrinterFeedback()
{
	Debug::trace("PrinterManager.PrinterProbe", printerTraceString + " receivedPrinterFeedback:resetting probe timer ");

	lastStatusMsg.setPeripheralComm();

	probeTimeoutCntr_ = 0;

}

bool PrinterManager::printerBusy()
{
	return ((printerResetting == true) ||
			(printerCalibrating == true) ||
			(printerAdvancing == true));
}

void PrinterManager::queueCommand(PrinterCommandType printerCommandType,
		PrinterCommandOptions printerCommandOptions,
		PrinterIndirection printerIndirection,
		UINT16 value)
{
	if (pMyPrinter != NULL)
	{
		//		Util::CLockCriticalSection lock( commandQCS );

		PrinterCommand *printerCommand = new PrinterCommand(this, myPrinterType, printerCommandType, printerCommandOptions, printerIndirection, value);
		commandQ.push(printerCommand);
	}
}

PrinterCommand * PrinterManager::popCommandQ()
{
	//	Util::CLockCriticalSection lock( commandQCS );

	if ((commandQ.size() > 0) && (executingCommand == false))
	{
		PrinterCommand *pCommand = commandQ.front();
		commandQ.pop();

		return pCommand;
	}

	return 0;
}

void PrinterManager::emptyCommandQ()
{
	//	Util::CLockCriticalSection lock( commandQCS );

	while (commandQ.size() > 0)
	{
		PrinterCommand *pCommand = commandQ.front();
		commandQ.pop();

		delete pCommand;
	}
}


void PrinterManager::emptyProductTransactionQ(bool dispatchCompleteMessage)
{
	Debug::trace("PrinterManager.Product", printerTraceString +  " emptyProductTransactionQ: " + HobString(Bool::toString(dispatchCompleteMessage)));

	while (productTransactionQ.empty() == false)
	{
		if (dispatchCompleteMessage == true)
		{
			productTransactionId prodPackId = productTransactionQ.front();
			if (prodPackId.pProductTransaction)
				dispatchLabelComplete(prodPackId.pProductTransaction, prodPackId.transactionNum, false);
				prodPackId.transactionNum = 0;
		}
		productTransactionQ.pop_front();
	}
}

bool PrinterManager::isProductTransactionQEmpty()
{
	return productTransactionQ.empty();	
}

void PrinterManager::peekProductTransactionQ(productTransactionId *pProdPackId)
{
	if (!productTransactionQ.empty())
	{
		*pProdPackId = productTransactionQ.front();	
	}
	else 
	{
		pProdPackId->pProductTransaction = 0;
		pProdPackId->transactionNum = 0;
		pProdPackId->packId = 0;
	}
}

void PrinterManager::popProductTransactionQ(productTransactionId *pProdPackId)
{
	if (!productTransactionQ.empty())
	{
		*pProdPackId = productTransactionQ.front();

		productTransactionQ.pop_front();
	}
	else
	{
		pProdPackId->pProductTransaction = 0;
		pProdPackId->transactionNum = 0;
		pProdPackId->packId = 0;
	}
}

bool PrinterManager::removeProductTransactionFromQ(ProductTransaction *pPT)
{
	bool found = false;

	if((productTransactionQ.empty() == false) && (pPT != NULL))
	{
		deque<productTransactionId>::iterator i = productTransactionQ.begin();

		for (; i != productTransactionQ.end(); i++)
		{
			if ((*i).pProductTransaction == pPT)
			{
				Debug::trace("PrinterManager.Product:", HobString("Removed from Queue"));
				found = true;
				productTransactionQ.erase(i);
				break;
			}		
		}
	}
	else
	{
		Debug::trace("PrinterManager.Product:", HobString("Queue empty:")+ HobString(Bool::toString(productTransactionQ.empty())));	
		Debug::trace("PrinterManager.Product:", HobString("ProductTransaction:")+ HobString(Hex::toString((long long)pPT )));	
	}

	return found;
}


void PrinterManager::pushProductTransactionQ(ProductTransaction *pPT, UINT8 packId, Util::CSmartPtr<Product> pProductSmartPtr)
{
	productTransactionId tempProdPackId;
	tempProdPackId.pProductTransaction = pPT;
	tempProdPackId.transactionNum = pPT ? pPT->getTransactionNum() : 0;
	tempProdPackId.packId = packId;
	tempProdPackId.productSmartPtr = pProductSmartPtr;

	productTransactionQ.push_back(tempProdPackId);	
}


void PrinterManager::clearLabelDimensions()
{
	stockLoaded.setLength(0);
	printerStockMetrics.setLabelLengthInLines(0);

	if (getLabelWidthFromStatus == true)
	{
		stockLoaded.setWidth(UnknownLabel);
		printerStockMetrics.setLabelWidth(UnknownLabel);
	}

	printerStockMetrics.show();
	dispatchEvent(&printerStockMetrics);
	Debug::trace("PrinterTeachTable", printerTraceString + " clearLabelDimensions() - Dispatched printerStockMetrics");
}

void PrinterManager::checkLabelWidth(LabelWidth printerStatusLabelWidth)
{
	bool isTracing = Debug::isTracing("PrinterManager.PrinterConfig");
	if (isTracing)
		Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " checkLabelWidth() - begin");
		
	// We used to only do this for HT scales (getLabelWidthFromStatus), not EPP, but now it's common code so do for all printers
	if (isTracing)
		Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " checkLabelWidth() - labelWidth passed in is:" + inttostring((int)printerStatusLabelWidth) + "  localVar labelWidth is currently:" + inttostring((int)labelWidth));

	labelWidth = printerStatusLabelWidth;

	stockLoaded.setWidth(printerStatusLabelWidth);

	updateHorizontalOffset();

	printerStockMetrics.setLabelWidth(printerStatusLabelWidth);
	printerStockMetrics.show();

	dispatchEvent(&printerStockMetrics);
	if (isTracing)
		Debug::trace("PrinterManager.PrinterConfig", printerTraceString + " checkLabelWidth() - Dispatched printerStockMetrics");

	//preImageLabel();
	if (printerState == PrinterReadyToPrint)
	{
		printerState = PrinterReadyToPreImage;
		Debug::trace("PrinterManager.StateMachine", printerTraceString + " checkLabelWidth() - PrinterReadyToPreImage 5");
	}

	PrTestLabelType saveTestLabelType = testLabelType;
	testLabelType = PrNoTestLabel;
	createTestLabel(saveTestLabelType);
}

// assumes buffer is initialized to all NULLs
#ifdef CHECK_FT
void PrinterManager::bitSet(UINT16 startBit, UINT16 numBits, UINT8 *buffer)
{
	for (UINT16 i = 0; i < startBit + numBits; i++)
	{
		if (i >= startBit)
		{
			buffer[i/8] |= (UINT8) (Power::power(2, (7-(i%8))));
		}
	}

	Debug::trace("PrinterManager.bitSet", printerTraceString + " buffer: 0x" + HobString(Hex::toString((const char *) buffer, 80)));
}
#else
//	GCDR-120	Modify bitSet() again after PTR 1230.
//				The checkerboard pattern was overwritting bits
//				on the right edge under certain criteria.
// PTR 1230 rewrite this function
// assumes buffer is initialized to all NULLs
// Note, this function is called many, many times when it creates a label bitmap. It needs to be
// very efficient. This could be even faster but it gets a label out in about 1/2 second.
void PrinterManager::bitSet(UINT16 startBit, UINT16 numBits, UINT8 *buffer)
{
	const UINT8 mask		= 0x80;
	const UINT8 bitsPerMask = 8;
	
	UINT16 ii;				// counter
	UINT16 firstFullByte;
	UINT16 lastPartialByte;
	UINT16 firstFullBit;
	UINT16 lastPartialBit;

	firstFullByte	= startBit / bitsPerMask;
	if (startBit % bitsPerMask)
		firstFullByte += 1;

	firstFullBit	= firstFullByte * bitsPerMask;					// Very first bit in the very first full byte

	lastPartialByte	= (startBit + numBits) / bitsPerMask;
	lastPartialBit = lastPartialByte * bitsPerMask;

	// Loop through the first dangling bits. There will be anywhere from 1 to 8
	for( ii = startBit; ii < firstFullBit; ii++)
	{
	//	myMask = (mask >> (UINT8)(ii % bitsPerMask));
	//	myByte = ii/bitsPerMask;
	//	buffer[myByte] |= myMask;
		buffer[ii/bitsPerMask] |= (mask >> (ii % bitsPerMask));
	}

	if (firstFullByte < lastPartialByte)
	{
		// Do all the full bytes one at a time.
		for( ii = firstFullByte; ii < lastPartialByte; ii++ )
		{
			buffer[ii] = 0xFF;
		}

	// Do the last few dangling bits. Might also be 1 - 8 of these
		for( ii = lastPartialBit; ii < (startBit + numBits); ii++ )
		{
		//	myMask = (mask >> (UINT8)(ii % bitsPerMask));
		//	myByte = ii/bitsPerMask;
		//	buffer[myByte] |= myMask;
			buffer[ii/bitsPerMask] |= (mask >> (ii % bitsPerMask));
		}
	}
}
#endif

int PrinterManager::testPrinter(int i)
{
	static bool quit = false;

	if (i == 0)
	{
		quit = false;
	}

	return i++;
}

bool PrinterManager::isWrapperPrinter() const
{
	return ( (prKind == EppPrinter) || (prKind == Epp2Printer) || (prKind == Epp3Printer) ) ? true : false;
}

bool PrinterManager::isCounterScalePrinter() const
{
	return ( (prKind == ApcPrinter) || (prKind == EssPrinter) || 
	         (prKind == EscPrinter) || (prKind == KPCPrinter) ||
			 (prKind == SSKPCPrinter) ) ? true : false;
}

bool PrinterManager::isVirtualPrintingEnabled() const
{
	Util::CLockCriticalSection lock( printerManagerCS );
	return virtualPrintingEnabled;
}
void PrinterManager::setVirtualPrintingEnabled(bool enabled)
{
	Util::CLockCriticalSection lock( printerManagerCS );
	//if( prKind != SSKPCPrinter ) {
	Debug::trace( "PrinterManager", "set virtual printing enabled " + Long::toString(enabled) );
	virtualPrintingEnabled = enabled;
	//}
}

// PJR-444 - SOF-5063 - Print wider labels on Zebra printer
bool PrinterManager::isExternalLabelPrinterManager() const
{
	Util::CLockCriticalSection lock(printerManagerCS);
	return (labelWidth == ExternalLabel);
}

bool PrinterManager::isFakePrinterManager() const
{
	Util::CLockCriticalSection lock( printerManagerCS );
	return (prKind == FakePrinter);
}

PrinterKind PrinterManager::getPrinterKind() const
{
	Util::CLockCriticalSection lock( printerManagerCS );
	return prKind;
}

PrinterType PrinterManager::getPrinterType() const
{
	Util::CLockCriticalSection lock( printerManagerCS );
	return myPrinterType;
}

bool PrinterManager::isLabelToBePrinted(PrinterType printerNum)
{
	Util::CLockCriticalSection lock( printerManagerCS );

	if ( printerNum < MaxPrinterTypes )
		return labelsToBePrinted[(int)printerNum];

	return false;
}

void PrinterManager::setLabelToBePrinted(PrinterType printerNum, bool printIt)
{
	Util::CLockCriticalSection lock( printerManagerCS );

	if ( printerNum < MaxPrinterTypes )
		labelsToBePrinted[(int)printerNum] = printIt;
}

void PrinterManager::setAllLabelsToBePrinted(bool printIt)
{
	Util::CLockCriticalSection lock( printerManagerCS );

	for (int i = (int)FirstPrinter; i < (int)MaxPrinterTypes; i++)
		labelsToBePrinted[i] = printIt;
}

USBD_NODE_ID PrinterManager::getUsbNodeId() const
{
	Util::CLockCriticalSection lock( printerManagerCS );
	return usbNodeId;
}

bool PrinterManager::getStreamingLabels() const
{
	Util::CLockCriticalSection lock( printerManagerCS );
	return streamingLabels;
}

void PrinterManager::setStreamingLabels(bool streaming)
{
	Util::CLockCriticalSection lock( printerManagerCS );
	streamingLabels = streaming;
}

bool PrinterManager::isContinuousStock() const
{
	Util::CLockCriticalSection lock( printerManagerCS );
	return printerStockMetrics.isContinuousStock();
}

bool PrinterManager::isDieCutloaded() const
{
	bool result = false;
	if( ( printerMode == DieCutLabelHeel ) || ( printerMode == DieCutLabelHead ) || ( printerMode == DieCutStreamHead ) || 
		( printerMode == DieCutStreamHeel ) || ( printerMode == DieCutShootHeel ) || ( printerMode == DieCutShootHead ) ) {
		result = true;
	}	
	Debug::trace("PrinterManager.TakeLabelTime", "printerMode: " + Integer::toString(printerMode) + " result: " + Integer::toString(result) );						
	return result;
}

void PrinterManager::writeScaleDataThread()
{
	FpmManager *pSerialFlash( FpmManager::getInstance() );

	if( !pSerialFlash )
		return;

#if defined (__linux__)
	sched_param params;
	params.sched_priority = 0;

	// lower thread priority
	if ( 0 != pthread_setschedparam( pthread_self(), SCHED_IDLE, &params ) )
		Debug::trace( "PrinterManager.Timer", "failed to set priority!" );
#endif

	const std::string style( pSerialFlash->getMFGSystemStyle() );

	if (( style == "HT" ) || ( style == "HTK" ) || ( style == "FSS" )  )
	{
		MtdScaleData *pData( pSerialFlash->readScaleData() );

		if ( pData )
		{
			if ( myPrinterType == FirstPrinter )
				pData->setPrimaryPrinterCounter( previousPrinterCounts_ = printerCounts_ );
			else if ( myPrinterType == SecondPrinter )
				pData->setSecondaryPrinterCounter( previousPrinterCounts_ = printerCounts_ );
			else if ( myPrinterType == ThirdPrinter )
				pData->setTertiaryPrinterCounter( previousPrinterCounts_ = printerCounts_ );

			if ( pSerialFlash->writeScaleData() != 0 )
				Debug::trace( "PrinterManager.Timer", "printer counter saved count: " + Integer::toString(printerCounts_) );
			else
				Debug::trace( "PrinterManager.Timer", "Failed to save printer counter!" );
		}
	}
	else if (( style == "EPCP" ) || ( style == "AWS3" ) || ( style == "NGW1" ) || ( style == "NGW2" ))
	{
		MtdNGWWrapperData *pData( pSerialFlash->readWrapperData() );

		if ( pData )
		{
			if ( myPrinterType == FirstPrinter )
				pData->setPrimaryPrinterCounter( previousPrinterCounts_ = printerCounts_ );
			else if ( myPrinterType == SecondPrinter )
				pData->setSecondaryPrinterCounter( previousPrinterCounts_ = printerCounts_ );
			else if ( myPrinterType == ThirdPrinter )
				pData->setTertiaryPrinterCounter( previousPrinterCounts_ = printerCounts_ );

			if ( pSerialFlash->writeNGWWrapperData() != 0 )
				Debug::trace( "PrinterManager.Timer", "printer counter saved count: " + Integer::toString(printerCounts_) );
			else
				Debug::trace( "PrinterManager.Timer", "Failed to save printer counter!" );
		}
	}
}

int PrinterManager::isLabelFormAccumulateItemsItemLabelType( UINT16 verticalJustifyNum ) const
{
	int ltNum = verticalJustifyNum / 100;
	if ( ltNum == Configuration::getInstance()->toIntCfgI(CfgInternal::ACCUMITEMSITEMLABELTYPE) )
		return verticalJustifyNum % 100;
	return -1;
}

void PrinterManager::deleteImageControl( ImageControl *obj, int count )
{
	doomedImageControl.push_back( obj );

	while ( count < doomedImageControl.size() )
	{
		delete doomedImageControl.front();
		doomedImageControl.pop_front();
	}
}

void PrinterManager::deleteBitMap( BitMap *obj, int count )
{
	doomedBitMap.push_back( obj );

	while ( count < doomedBitMap.size() )
	{
		delete doomedBitMap.front();
		doomedBitMap.pop_front();
	}
}

