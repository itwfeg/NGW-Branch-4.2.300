/**
 * @file SSPrinterManager.cpp
 * @copyright Copyright (c) 2021 Hobart Corportation
 * @author Aaron Swift
 * @brief Responsible for freestanding self service scale printer 
 * @{
 */
#include "SSPrinterManager.h"
#include "EssPrinterTeachTable.h"
#include "EvolPrinterTeachTable.h"
#include "PrinterEvent.h"
#include "EvolMessage.h"
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
#include "PrinterSystemStatus.h"
#include "PrinterSystemRequestStatus.h"

#include "PrinterEnable.h"
#include "PrinterDisable.h"
#include "PrinterVersion.h"

#include "PrinterRequestCalibrate.h"
#include "PrinterFactoryDefaultsComplete.h"

#include "RendererManager.h"
#include "RendererRequestSession.h"
#include "RendererSessionId.h"
#include "RendererSetSource.h"
#include "RendererSetDestination.h"
#include "RendererRequestRendering.h"
#include "RendererRenderStart.h"
#include "RendererRenderComplete.h"
#include "RendererCloseSession.h"

#include "PrinterRequestStatus.h"
#include "PrinterRequestInquire.h"
#include "PrinterRequestWakeup.h"
#include "PrinterHeadWear.h"
#include "PrinterGapCalComplete.h"
#include "PrinterKpcError.h"
#include "KpcSystemErrors.h"

#include "Integer.h"
#include "Product.h"
#include "ProductTransaction.h"
#include "ProductText.h"
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
#include "FpmManager.h"

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
#include "PythonScripter.h"
#include "CRendererUtil.h"
#include "SystemPrinters.h"

#include <list>
#include <cstring>
#include <vector>
using namespace std;



const unsigned int PrinterManager::_edgeMargin_ = 13;	// Offset from edges of label stock

SSPrinterManager::SSPrinterManager( Hobart::CEvolMsgReceiver *pTranslator_, PrinterType id_,
								LabelWidth labelWidth_, UINT16 bufferSize_, UINT16 printheadSize_, UINT32 transferSize_, PrinterKind prKind_ ) :
								PrinterManager( pTranslator_, NULL, id_, labelWidth_, bufferSize_,  printheadSize_, 0x4646, 0, prKind_ )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif


	Debug::trace("SSPrinterManager", " Starting...");

#if defined ( __linux__ )

	pTranslator = pTranslator_;
	stationId = id_;
	labelWidth = labelWidth_;
	bufferSize = bufferSize_;
	printheadSize = printheadSize_;
	transferSize = transferSize_;
	prKind = prKind_;

	Debug::trace("SSPrinterManager", "SSPrinterManager: Station id: " + Integer::toString(stationId));

	//open label data channel to printer
	pRS485Manager = RS485Manager::getInstance();

	if( pRS485Manager != NULL )
		pRS485Manager->setTargetSize( bufferSize );

	if( prKind == SSKPCPrinter ) {
		pMyTeachTable = EssPrinterTeachTable::getInstance();
		printLabelCentered = false;
		labelWidthStatus = true;

	} else {
		Debug::trace("SSPrinterManager.Error", " Error unsupported Printer type: " + Integer::toString( prKind ));
		pMyTeachTable = NULL;

		printLabelCentered = true;
		labelWidthStatus = false;
	}

	pConfig = Configuration::getInstance();

	labelPresent = false;
	sendTestLabel = false;
	printingVoidLabel = false;
	continuousTestLabels = false;
	testLabelData = NULL;
	testLabelDataSize = 0;
	saveTestLabelType = PrNoTestLabel;
	testLabelType = PrNoTestLabel;
	applySecurityLabel = PrNoSecurityLabel;

	executingCommand = false;

	printerResetting = false;


	printerCalibrating = false;


	resetPrinter = false;


	enableSizing = true;

	printerAdvancing = false;


	measuredSize = 0;
	actualSize = 0;
	labelDataSize = 0;
	labelData = NULL;

	outOfDataBuffers = false;


	trackSensors = PrinterOff;

	pCurrentProduct = NULL;

	currentProductSmartPtr = Util::CSmartPtr<Product>();
	newProductSmartPtr = Util::CSmartPtr<Product>();

	pCurrentLabel = NULL;

	pCurrentProductTransaction = NULL;
	currentPackId = 0;

	myRendererSessionId = 0;

	// NGW-261
	for ( size_t i( NUM_LABEL_BUFFERS ); i--; )
		labelBuffer_[i] = NULL;

	labelBufferIndex_ = 0;

	pDestinationData = NULL;
	for (int i = 0; i < (short)MaxPrinterTypes; i++)
		imageControlBlock[i] = 0;
	setAllLabelsToBePrinted(false);

	pMyRenderer = RendererManager::getInstance();

	// get a session with the renderer!!
	RendererRequestSession rdReqSession( this );

	const char *pCC = rdReqSession.serialize();
	int mLength = rdReqSession.getSerializedLength();
	pMyRenderer->send( pCC, mLength );

	ticksPerSecond = 1000;

	xferLabelStart = 0;
	xferLabelFinish = 0;

	// Timer for LabelTaken to happen after LabelReady occurs
	if( pConfig->isSelfServiceScale() == true ) {
		takeLabelTime = ticksPerSecond / 2;
	} else {
		takeLabelTime = 3 * ticksPerSecond;
	}

	// We want two requests per second
	requestSensorTime = ticksPerSecond / 2;

	printerProbeTime = 1000000 * ticksPerSecond;
	Debug::trace("SSPrinterManager.Probe", HobString(" probeTime: ") + Integer::toString(printerProbeTime));

	printheadPowerTime = 30 * ticksPerSecond;

	continuousLabelTime = ticksPerSecond / 8;

	printHeelFirst = pConfig->isLabelFeedHeelFirst();

	requestSensorReading = false;

	linesToPrint = 0;
	takeLabelError = false;

	streamingLabels = false;

	usingContinuousStock = false;

	lastStatusMsg.setPrinterType( stationId );

	printerConfig.setPrinterType( stationId );
	lastSensorsMsg.setPrinterType( stationId );

	if( pConfig->isUpdatePrinterConfig() == true ) {
		sendPrinterConfig( PrPermanent );
	}

	printerStockMetrics.setPrinterType( stationId );

	//TO DO: ask larry about this line....
	printerStockMetrics.setLabelLengthInLines( (LabelLengthLines) Configuration::getInstance()->toIntCfgI("fakePrinterLabelLength") );
	stockLoaded.setLength( (UINT16)Configuration::getInstance()->toIntCfgI( CfgInternal::DIMBLENGTHCONTINUOUS ) );
	stockLoaded.setWidth( WideLabel );
	stockLoaded.setLabelOrientation( PortraitLabel );
	stockLoaded.setWidthDots( WideLabelWidth );


	//	PTR 1578:	Make sure we set this for the Status msg also, so we don't report an error
	//				for the Primary (default) when it is for another printer.
	lastStatusMsg.setPrinterType( stationId );

	pendingLabelXfer = false;

	testLabelPrintInProgress = false;

	registerEvents();

	printerState = PrinterUnsized;
	Debug::trace("SSPrinterManager.StateMachine", " PrinterUnsized");

	printerMode = UnsizedLabel;

	probeTimeoutCntr_ = 0;

	currentTransNum = 0;

	previousPrinterCounts_ = printerCounts_ = 0;

	FpmManager *pSerialFlash = FpmManager::getInstance();
	if( pSerialFlash != NULL )
	{
		std::string style = pSerialFlash->getMFGSystemStyle();
		if( ( style == "HT" ) || ( style == "HTK" ) ) {
			MtdScaleData *pPrinterStatistics_ = pSerialFlash->readScaleData();
			if( pPrinterStatistics_ != NULL ) {
				//if( printerType == FirstPrinter )
				if ( id_ == FirstPrinter)
					previousPrinterCounts_ = printerCounts_ = pPrinterStatistics_->getPrimaryPrinterCounts();
				//else if (printerType == SecondPrinter)
				else if ( id_ == SecondPrinter)
					previousPrinterCounts_ = printerCounts_ = pPrinterStatistics_->getSecondaryPrinterCounts();
				//else if (printerType == ThirdPrinter)
				else if ( id_ == ThirdPrinter)
					previousPrinterCounts_ = printerCounts_ = pPrinterStatistics_->getTertiaryPrinterCounts();

				Debug::trace("SSPrinterManager.Timer", "initialized printerCounts_: " + Integer::toString(printerCounts_));
			}
			Debug::trace("SSPrinterManager.Timer", "Created printer label counter timer.");
			createTimer(( 1000 * 60 ), labelStatisticsTimer, CHobartTimer::PERIODIC_TIMER);
        } 
	} else {
		Debug::trace("SSPrinterManager.Timer", "pSerialFlash: NULL");
	}

#if defined (_WIN32)

	if (Configuration::getInstance()->isPrepackScale())
	   cPrinterThread = new Util::CTLSThread( *this, 16, 0, "SSPrinterManager Thread" );
	else
	   cPrinterThread = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "SSPrinterManager Thread" );

#else

   /*Linux only -- bump thread priority to resolve streaming 8" label issue*/
   cPrinterThread = new Util::CTLSThread( *this, 1, 0, "SSPrinterManager Thread" );

#endif

#endif  // #ifdef linux

}

SSPrinterManager::~SSPrinterManager()
{
#if defined ( __linux__ )
	if( cPrinterThread ) {
		cPrinterThread->StopThread();
		cPrinterThread->WaitForCompletion();
	}

	shutdownTimerThreads();
	ShutdownNicely();

	Debug::trace("SSPrinterManager", " ~PrinterManager():  " + HobString(Hex::toString((long long) this)));
	// Deregister for all the OperationMode messages

	lastStatusMsg.setManagerDeleted();
	dispatchEvent( &lastStatusMsg );

	//tell the thread to exit
	m_ExitEvent.EventSet();
	Debug::trace("SSPrinterManager", "~PrinterManager():  m_ExitEvent.EventSet()");

	//wait for the thread to exit before deleting all the resources!
	if( cPrinterThread ) {
		while ( cPrinterThread->IsRunning() )
			PAL_absclass::PAL_sleep( 1 );
		cPrinterThread = NULL;
	}
	Debug::trace("SSPrinterManager", "~PrinterManager():  cPrinterThread = NULL");

	cancelTimer( printerProbe );
	cancelTimer( takeLabelTimerExpired );
	cancelTimer( sensorTimerExpired );

	Debug::trace("SSPrinterManager.Desc", " Calling BitMap::freeBitMap()");
	for (int i = 0; i < (short)MaxPrinterTypes; i++)
		bitMap[i].freeBitMap();

	Debug::trace("SSPrinterManager.Desc", " Calling rotatedBitMap::freeBitMap()");
	for (int i = 0; i < (short)MaxPrinterTypes; i++)
		rotatedBitMap[i].freeBitMap();


	if( ( testLabelType != PrVoidLabel ) && ( testLabelData != NULL ) ) {
		delete [] testLabelData;
	}

	if( pMyRenderer != NULL ) {
		RendererCloseSession rdCloseSession( this, myRendererSessionId );

		const char *pCC = rdCloseSession.serialize();
		int mLength = rdCloseSession.getSerializedLength();

		pMyRenderer->send( pCC, mLength );
	}

	emptyCommandQ();
	emptyIndirectGraphics();
	emptyEmbeddedLabelBitMaps();

	for (int i = 0; i < (short)MaxPrinterTypes; i++)
		if ( imageControlBlock[i] )
			delete imageControlBlock[i];

	// NGW-261
	for ( size_t i( NUM_LABEL_BUFFERS ); i--; )
		if ( labelBuffer_[i] )
			delete labelBuffer_[i];

#endif		// #ifdef linux
	
	Debug::trace("SSPrinterManager", " ~PrinterManager():  Goodbye.");
}

void SSPrinterManager::registerEvents()
{

	registerEvent( Printer, PrReqStockMetrics );
	registerEvent( Printer, PrReqPrinterType );
	registerEvent( Printer, PrReqKind );
	registerEvent( Printer, PrPreImage );
	registerEvent( Printer, PrPostImage );
	registerEvent( Printer, PrTestLabel );
	registerEvent( Printer, PrReqLabelWidth );


	if (prKind == SSKPCPrinter) {
		registerEvent( Printer, PrLabelTransferReady );
	}

	if( stationId == FirstPrinter )
		registerEvent( Printer, PrSetupForNormalSizing );

	registerEvent( Printer, PrWakeup );
	registerEvent( Printer, PrEnable );
	registerEvent( Printer, PrDisable );
	registerEvent( Printer, PrTest );
	registerEvent( Printer, PrReset );
	registerEvent( Printer, PrResetImmediate );
	registerEvent( Printer, PrConfig );
	registerEvent( Printer, PrCommand );
	registerEvent( Printer, PrFlush );
	registerEvent( Printer, PrReqPrintheadPower );
	registerEvent( Printer, PrMode );
	registerEvent( Printer, PrRam );
	registerEvent( Printer, PrReqWakeup );
	registerEvent( Printer, PrReqInquire );
	registerEvent( Printer, PrReqSensors );
	registerEvent( Printer, PrReqCalibrate );
	registerEvent( Printer, PrSensorsTracking );
	registerEvent( Printer, PrLabelWidth );
	registerEvent( Printer, PrNewOrder );
	registerEvent( Printer, PrTakeLabelTime );
	registerEvent( Printer, PrAdvLabelStock );
	registerEvent( Printer, PrNewAddress );
	registerEvent( Printer, PrSizing );
	registerEvent( Printer, PrDeletePackage );
	registerEvent( Printer, PrReqLabelSizing );
	registerEvent( Printer, PrUseContinuousStock );
	registerEvent( Printer, PrResetFactoryDefaults );

	registerEvent( Printer, PrVersion );
	registerEvent( Printer, PrKPCError );
	registerEvent( KPCSystem, KpcError );
	registerEvent( Printer, PrLabelTransferComplete);

	registerEvent(PrinterSystem, SysReqStatus);
}

DWORD SSPrinterManager::Run()
{
#if defined ( __linux__ )
	try
	{
		Debug::trace("SSPrinterManager.Run", std::string(" start run thread "));

		if( stationId == FirstPrinter && pConfig->isConserveContinuousStockByNotSizingEnabled() ) {
			if( pConfig->toIntCfg(fieldOffsetCfgPrinter1LabelLength) == LengthContinuous_Lines ) {
				Debug::trace("SSPrinterManager", std::string(" start run thread - setting up for usingContinuousStock"));
				usingContinuousStock = true;

				if( printHeelFirst == true )
					printerMode =  ContinuousLabelHeel;
				else
					printerMode =  ContinuousLabelHead;

			} else if( pConfig->toIntCfg(fieldOffsetCfgPrinter1LabelLength) > 0 ) {
				Debug::trace("SSPrinterManager", std::string(" start run thread - setting up for die cut labels, length:" + inttostring(pConfig->toIntCfg(fieldOffsetCfgPrinter1LabelLength))));
				if( printHeelFirst == true )
					printerMode =  DieCutLabelHeel;
				else
					printerMode =  DieCutLabelHead;
			}
		}

		teachPrinter( printerMode );

		createTimer( printerProbeTime, printerProbe, CHobartTimer::PERIODIC_TIMER );

		requestStatusMsg();
		requestInquireMsg();

		while( !m_ExitEvent.IsEventSet() ) {

			if( (resetPrinter == true) && ( !( ( printerState == PrinterXferringLabel ) ||
							( printerState == PrinterTakingLabel  ) ||
							( printerState == PrinterLabelIsReady ) ||
							( printerState == PrinterTryingToSize ) ||
							( printerState == PrinterFinding ) ) ) ) {

					resetPrinter = false;

					sendPrinterReset( 0 );
					emptyProductTransactionQ( false );
					emptyCommandQ();

					pCurrentProduct = NULL;
					pCurrentLabel = NULL;

					Debug::trace("SSPrinterManager.PrinterImaging", " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));

					currentProductSmartPtr = Util::CSmartPtr<Product>();
					newProductSmartPtr = Util::CSmartPtr<Product>();

					dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, false);
					pCurrentProductTransaction = NULL;
					currentPackId = 0;

					if( printerState != PrinterTryingToSize ) {
						printerState = PrinterReadyToPrint;
						Debug::trace("SSPrinterManager.StateMachine", " 1) PrinterReadyToPrint");
					}
				}


				if( executingCommand == false ) {
					PrinterCommand *pCommand = popCommandQ();

					if( pCommand ) {
						if( pTranslator != NULL ) {
							Debug::trace("SSPrinterManager.Run", "executingCommand == false and have a command to execute and have a printer");
							executingCommand = true;

							if(pCommand->getPrinterCommand() == CalibrateCommand) {
								printerCalibrating = true;
								Debug::trace("SSPrinterManager.PrinterCalibration", " printerCalibrating: " + HobString(Bool::toString(printerCalibrating)));

								PrinterRequestCalibrate reqCalib( this, stationId, (PrinterCalibrateType) pCommand->getValue() );

								const char *pCC = reqCalib.serialize();
								int mLength = reqCalib.getSerializedLength();
								pTranslator->send( pCC, mLength );
							} else {
								Debug::trace("SSPrinterManager.LabelTransfer", " Sending PrintCommand");

								const char *pCC = pCommand->serialize();
								int mLength = pCommand->getSerializedLength();
								pTranslator->send( pCC, mLength );
							}
						}
						delete pCommand;
					}
				}

				if( printerBusy() == false ) {
					if( takeLabelError == true && takeLabelTime != 0 ) {
						Debug::trace("SSPrinterManager.Run", "printer is not busy and takeLabelError == true");

						takeLabelError = false;

						PrinterStatus prStatus( lastStatusMsg );

						prStatus.setTakeLabelError();

						dispatchEvent( &prStatus );

						lastStatusMsg.setTakeLabelError(); // make sure it is on!!
					}

					// try to print the next label
					if( sendTestLabel == true ) {
						Debug::trace("SSPrinterManager.Run", "printer is not busy and sendTestLabel == true");

						currentVirtuallyPrintingLabelNum = MaxPrinterTypes;

						if( printerState == PrinterPriorityError ) {
							cancelTimer( continuousLabelTimerExpired );
							teachPrinter( printerMode );
							printerState = PrinterReadyToPreImage;
						} else if( printerState == PrinterLabelIsReady || printerState == PrinterReadyToXfer ) {
							if( testLabelType == PrVoidLabel ) {
								printingVoidLabel = true;
							} else {
								testLabelPrintInProgress = true;//at this time we stop new label data from being created.
							}

							emptyProductTransactionQ( false );

							if( pCurrentProductTransaction != NULL ) {
								// Kill the currently printing label
								dispatchLabelComplete( pCurrentProductTransaction, currentTransNum, false );

								pCurrentProductTransaction = NULL;
								currentPackId = 0;
							}

							if( sendTestLabel == true ) {
								Debug::trace("SSPrinterManager.PrinterReset", "resetPrinter 0");
								sendPrinterReset( 1 );
								emptyCommandQ();

								printTestLabel();
							}
						}

						sendTestLabel = false;
					} else if( ( ( printerState == PrinterReadyToPrint ) || ( printerState == PrinterReadyToPreImage ) ) &&
							    (myRendererSessionId != 0)) {
						//Debug::trace("SSPrinterManager.Run", " Run() - printer is not busy and printerState == PrinterReadyToPrint || printerState == PrinterReadyToPreImage");

						// look at the next productTransaction
						productTransactionId  prodPackId;

						peekProductTransactionQ( &prodPackId );

						ProductTransaction *pNextProductTransaction = prodPackId.pProductTransaction;
						if( pNextProductTransaction ) {
							Product *pNextProduct = pNextProductTransaction->getProduct();

							if( pNextProduct ) {
								LabelType *pNextLabel = pNextProduct->getLabelType( (short)stationId );

								Debug::trace("SSPrinterManager.Product", " printerState: " + HobString(Integer::toString( printerState)));

								// Is it a new Product
								if( pCurrentProduct && (pNextProduct->getProductNum() == pCurrentProduct->getProductNum() ) &&
										( virtualPrintingEnabled || (pCurrentLabel != NULL && pNextLabel == pCurrentLabel) ) &&
										!pNextProductTransaction->doForcePreImage() ) {
									pCurrentProductTransaction = pNextProductTransaction;
									Debug::trace("SSPrinterManager.Product", " PostImaging Product Transaction: " + HobString(Hex::toString((long long) pCurrentProductTransaction)));

									productTransactionId tempProdPackId;

									popProductTransactionQ( &tempProdPackId );

									currentPackId = tempProdPackId.packId;
									currentTransNum = tempProdPackId.transactionNum;

									// start postimaging all labels for the transaction - post image all of them consecutively
									printerState = PrinterPostImaging;
									Debug::trace("SSPrinterManager.StateMachine", " PrinterPostImaging");


									bool moreLabelsToPostImage = false;
									PrinterType labelNumToPostImage = stationId;
									if( virtualPrintingEnabled ) {
										moreLabelsToPostImage = true;
										labelNumToPostImage = FirstPrinter;
									}

									do {
										if( imageControlBlock[labelNumToPostImage] ) {
											imageControlBlock[labelNumToPostImage]->setImageFlags( PostImage );
											updateFormPostImageFields( *imageControlBlock[labelNumToPostImage] );


											RendererSetSource rdSetSource( this, myRendererSessionId, (UINT8 *) imageControlBlock[labelNumToPostImage] );
											const char *pCC = rdSetSource.serialize();
											int mLength = rdSetSource.getSerializedLength();
											pMyRenderer->send( pCC, mLength );

											RendererSetDestination rdSetDestination( this, myRendererSessionId, (UINT8 *) bitMap[labelNumToPostImage].getBitMapBase() );
											pCC = rdSetDestination.serialize();
											mLength = rdSetDestination.getSerializedLength();
											pMyRenderer->send( pCC, mLength );

											RendererRequestRendering rdReqRendering( this, myRendererSessionId );
											pCC = rdReqRendering.serialize();
											mLength = rdReqRendering.getSerializedLength();
											pMyRenderer->send( pCC, mLength );
										}

										if( moreLabelsToPostImage ) {
											while ( labelNumToPostImage < MaxPrinterTypes ) {
												short i = (short)labelNumToPostImage;
												i++;
												labelNumToPostImage = (PrinterType)i;
												if( labelNumToPostImage >= MaxPrinterTypes )
													moreLabelsToPostImage = false;
												else if( isLabelToBePrinted(labelNumToPostImage) )
													break;
											}
										}

									} while( moreLabelsToPostImage );
								} else {
									currentProductSmartPtr = prodPackId.productSmartPtr;
									pCurrentProduct = pNextProduct;  // or could use currentProductSmartPtr.get(), but they should be the same!
									Debug::trace("SSPrinterManager.PrinterImaging",  " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));
									preImageLabel();
									pNextProductTransaction->setForcePreImage( false );
								}
							}
						}
					}

					if( ( printerState == PrinterUnsized ) && ( enableSizing == true ) ) {
						Debug::trace("SSPrinterManager.Run", "printer is not busy and printerState == PrinterUnsized && enableSizing == true");

						// tell printer to size its label!!
						queueCommand(SizeCommand, PrNoSecurityLabel, PrNoIndirection, 0x0);

						printerState = PrinterTryingToSize;
						Debug::trace("SSPrinterManager.StateMachine", " PrinterTryingToSize");

						PrinterStatus prStatus( lastStatusMsg );
						prStatus.setLabelSizing();
						dispatchEvent(&prStatus);

						lastStatusMsg.setLabelSizing(); // make sure it is on!!

						pCurrentProduct = NULL;
						Debug::trace("SSPrinterManager.PrinterImaging", " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));
						currentProductSmartPtr = Util::CSmartPtr<Product>();

						labelPresent = false;
						Debug::trace("SSPrinterManager.LabelMissing", " labelPresent");
					}
				}


				if( requestSensorReading == true ) {
					requestSensorMsg();

					requestSensorReading = false;
				}

				PAL_absclass::PAL_sleep( 10 );
		}
	}
	catch(...)
	{
	    Debug::trace("SSPrinterManager.Error", HobString("Run():  catch(...)"));
		LogCatch;
	}

	#endif // #ifdef linux

	return 0;
}


void SSPrinterManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
#if defined ( __linux__ )
	Util::CLockCriticalSection lock( csLock_ );
	Debug::trace("SSPrinterManager", "  processEvent device:" + inttostring((int)msg->getDeviceType()) + "  event:" + inttostring((int)msg->getEventType()));

	if( msg.get() != NULL ) {
		switch( msg->getDeviceType() )
		{
			case Printer:
				handlePrinterMessage( (PrEvent)msg->getEventType(), msg->Data(), (UINT16)msg->Length() );
				break;
			case PrinterSystem:
				handlePrinterSysMessage( (SysEvent) msg->getEventType(), msg->Data() );
				break;
			case KPCSystem:
				handleKpcSystemError( (KpcSystemEvent)msg->getEventType(), msg->Data() );
				break;
			case Renderer:
				handleRendererMessage( (RdEvent)msg->getEventType(), msg->Data() );
				break;
			default:
				break;
		}
	}
#endif // #ifdef linux
}

void SSPrinterManager::handlePrinterMessage(PrEvent prEvent, const char *recvBuffer, UINT16 bufSize)
{
	Debug::trace("SSPrinterManager","prEvent = " + Integer::toString(prEvent));
#if defined ( __linux__ )
	if( prEvent == PrReqPrinterType ) {
		sendPrinterType();
		sendPrinterKind();
	} else {
		PrinterGeneric printerGeneric( recvBuffer );

		//No sense processing message if not my type or we have not been assigned a translator!
		if( ( printerGeneric.getPrinterType() == stationId ) && ( pTranslator != NULL ) ) {
			switch (prEvent)
			{
				case PrWakeup: {
					Debug::trace("SSPrinterManager.Message", " PrWakeup");
					clearPrinterProbe();
				}
				break;
				case PrConfig: {
					Debug::trace("SSPrinterManager.Message", " PrConfig");
					PrinterConfiguration prConfig( recvBuffer );

					if( Debug::isTracing("PrinterManager.PrinterConfig") == true ) {
						prConfig.show();
					}

					// only set label width on prepack printers
					if( prKind == KPPPrinter ) {
						prConfig.setLabelWidth( labelWidth );
					}

					if( printerConfig.equals(prConfig) == false ) {
						printerConfig = prConfig;
						sendPrinterConfig( printerConfig.getConfigType() );
					}

					updateLabelWidth( printerConfig.getLabelWidth() );
					updateHorizontalOffset();

					if( printerState == PrinterReadyToPrint ) {
						printerState = PrinterReadyToPreImage;
						Debug::trace("SSPrinterManager.StateMachine", " PrinterReadyToPreImage 5");
					}

					PrTestLabelType saveTestLabelType = testLabelType;
					testLabelType = PrNoTestLabel;
					createTestLabel( saveTestLabelType );

					if( prKind == KPPPrinter ) {
						printerStockMetrics.setLabelWidth( printerConfig.getLabelWidth() );
					} else {
						printerStockMetrics.setLabelWidth( labelWidth );
					}

					printerStockMetrics.setLabelWidthInDots( stockLoaded.getWidthDots() );

					dispatchEvent( &printerStockMetrics );
					Debug::trace("SSPrinterManager.PrinterSize", " Dispatched printerStockMetrics -- PrConfig");
				}
				break;
				case PrInquire: {
					PrinterInquire printerInquire( recvBuffer );
					printerInquire.setSourceObject( this );

					if( rcvdInquire == true ) {
						if( printerConfig.update(printerInquire) == true || requestedPrinterInquire ) {
							requestedPrinterInquire = false;
							Debug::trace("SSPrinterManager.PrinterInquire", "  1) PrInquire:  Calling sendPrinterConfig()");
							// LAP - Why are we writing the config back to the printer?  We just read it (PrInquire); only a write config (PrConfig) should write it back.
							// So I'm commenting out the following line, which writes it back to the printer.
							//sendPrinterConfig(printerInquire.getConfigType());
						}
					} else {
						rcvdInquire = true;

						if( stationId != printerInquire.getPrinterType() ) {
							Debug::trace("SSPrinterManager.PrinterInquire", " changing PrinterType to:" + HobString(Integer::toString(stationId)));
							printerInquire.setPrinterType( stationId );
						}

						if(((prKind == EssPrinter) || (prKind == EscPrinter) || (prKind == ApcPrinter)) && (labelWidth != printerInquire.getLabelWidth())) {
							printerInquire.setLabelWidth(labelWidth);

							// change and update config right here
							printerConfig.update(printerInquire);
							// LAP - Why are we writing the config back to the printer?  We just read it (PrInquire); only a write config (PrConfig) should write it back.
							// So I'm commenting out the following line, which writes it back to the printer.
							//sendPrinterConfig(printerInquire.getConfigType());
						} else {
							if( printerConfig.update( printerInquire ) == true || requestedPrinterInquire ) {
								requestedPrinterInquire = false;
								Debug::trace("SSPrinterManager.PrinterInquire", "  2) PrInquire:  Calling sendPrinterConfig()");
								// LAP - Why are we writing the config back to the printer?  We just read it (PrInquire); only a write config (PrConfig) should write it back.
								// So I'm commenting out the following line, which writes it back to the printer.
								//sendPrinterConfig(printerInquire.getConfigType());
							}
						}

						sendPrinterType();
					}

					updateLabelWidth(printerInquire.getLabelWidth());
					updateHorizontalOffset();

					if( printerState == PrinterReadyToPrint ) {
						printerState = PrinterReadyToPreImage;
						Debug::trace("SSPrinterManager.StateMachine", " PrinterReadyToPreImage 5");
					}

					PrTestLabelType saveTestLabelType = testLabelType;
					testLabelType = PrNoTestLabel;
					createTestLabel( saveTestLabelType );

					printerStockMetrics.setLabelWidth(printerInquire.getLabelWidth());
					printerStockMetrics.setLabelWidthInDots(stockLoaded.getWidthDots());

					dispatchEvent( &printerStockMetrics );
					Debug::trace("SSPrinterManager.PrinterSize", " Dispatched printerStockMetrics -- PrInquire");

					if( Debug::isTracing("PrinterManager.PrinterInquire") == true ) {
						printerInquire.show();
					}

					dispatchEvent( &printerInquire );
					clearPrinterProbe();
				}
				break;
				case PrResetFactoryDefaults: {
					Debug::trace("SSPrinterManager.Message", " PrResetFactoryDefaults");
					if( pTranslator != NULL ) {
						pTranslator->send( (const char*) recvBuffer, bufSize );
					}
				}
				break;
				case PrResetFactoryDefaultsComplete: {
					Debug::trace("SSPrinterManager.Message", " PrResetFactoryDefaultsComplete");
					PrinterFactoryDefaultsComplete msg( recvBuffer );

					if( !msg.getSavedStatus() ) {
						Debug::trace("SSPrinterManager", "Printer factory defaults corruption!" + Integer::toString(msg.getSavedStatus()));
					} else {
						Debug::trace("SSPrinterManager", "Printer factory defaults saved: " + Integer::toString(msg.getSavedStatus()));
					}

					dispatchEvent( &msg );
					clearPrinterProbe();
				}
				break;
				case PrVersion: {
					Debug::trace("SSPrinterManager.Message", " PrVersion");
					PrinterVersion version( recvBuffer );
					PrinterSystemStatus sysVer( this, FirstPrinter, prKind );

					sysVer.setSoftwareVersion( version.getSoftwareMajor(), version.getSoftwareMinor() );
					sysVer.setHardwareVersion( version.getHardwareMajor(), version.getHardwareMinor() );
					dispatchEvent( &sysVer );
				}
				break;
				case PrStatus: {
					Debug::trace("SSPrinterManager.Message", "PrStatus");

					if( Configuration::getInstance()->toBoolCfg( fieldOffsetCfgSelfServePrintToScreen ) &&
						Configuration::getInstance()->isSelfServiceScale() ) {
						Debug::trace("SSPrinterManager", "PrStatus being ignored due to 'print to screen'");
						return;
					}

					bool isTracing = Debug::isTracing("PrinterManager.PrinterStatus");
					if( isTracing )
						Debug::trace("SSPrinterManager.PrinterStatus", "PrStatus");
					clearPrinterProbe();
					PrinterStatus printerStatus( recvBuffer );

					printerStatus.setSourceObject( this );
					printerStatus.setPrinterType( stationId );

					if (Debug::isTracing("SSPrinterManager.PrinterStatus") == true)
					{
						printerStatus.show();
					}

					if(Debug::isTracing("PrinterManager.PrinterStatus"))
					{
						Debug::trace("PrinterManager.PrinterStatus", "Backend-StateMachine: " +  Integer::toString(printerState));
						Debug::trace("PrinterManager.PrinterStatus", "printerStatus.state: 0x" + Hex::toString(printerStatus.getState()));
						Debug::trace("PrinterManager.PrinterStatus", "printerStatus.sensor: 0x" + Hex::toString(printerStatus.getSensor()));
						Debug::trace("PrinterManager.PrinterStatus", "printerStatus.user: 0x" + Hex::toString(printerStatus.getUser()));
						Debug::trace("PrinterManager.PrinterStatus", "printerStatus.counter: 0x" + Hex::toString(printerStatus.getCounter()));
						Debug::trace("PrinterManager.PrinterStatus", "printerStatus.error: 0x" + Hex::toString(printerStatus.getError()));
					}

					if( stationId == FirstPrinter && pConfig->isConserveContinuousStockByNotSizingEnabled() ) {
						// If we know we are using continuous stock, set the Printer Status message to indicate continuous,
						// even if die-cut stock is loaded
						if( usingContinuousStock ) {
							printerStatus.setCounter( 0 );
							if( isTracing )
								Debug::trace("SSPrinterManager.PrinterStatus", " PrStatus at beginning of handler - setting counter to 0 for continuous labels");
						}
					}

					if( ( lastStatusMsg.isPeripheralComm() == false ) && ( printerStatus.isPeripheralComm() == true ) ) {
						sendPrinterType();
					}

					if( printerStatus.isLabelTaken() == false ) {
						labelPresent = true;
						if( isTracing )
							Debug::trace("SSPrinterManager.PrinterStatus", " labelPresent");
					}

					// the printer can only detect this after the label stops printing.
					// it's possible for the user to remove label from printer quickly and
					// turn on the error bit.
					if( (labelPresent == true) && (printerStatus.isLabelMissing() == true) ) {
						printerStatus.clearLabelMissing();
						if (isTracing)
							Debug::trace("SSPrinterManager.PrinterStatus", " False Label Missing Detected");
					}

					if( isTracing )
						Debug::trace("SSPrinterManager.PrinterStatus", " StateMachine: " + HobString(Integer::toString(printerState)));
					if( ( printerStatus.isPriorityError() == true ) && !( ( printerState == PrinterPriorityError ) ||
					    ( printerState == PrinterHeadDisengaged ) ) ) {
						if (isTracing)
							Debug::trace("SSPrinterManager.PrinterStatus", " checking PriorityError");
						if( continuousTestLabels == true ) {
							continuousTestLabels = false;
							cancelTimer( continuousLabelTimerExpired );
						}
						testLabelPrintInProgress = false;

						if( ( printerState == PrinterTryingToSize ) && ( printerStatus.isFindCmdComplete() || printerStatus.isFindwoLabelTakenCmdComplete() ) &&
							  printerStockMetrics.isContinuousStock() && printerStatus.isMediaJammed() ) {
							// When the printer is working on the FIND command in the teach table for sizing, it will report that
							// the media is jammed when it can't find the black bar - which it won't find with continuous stock.
							// So, don't consider this specific situation a PrinterPriorityError.
							printerStatus.clearMediaJammed();
						} else {
							executingCommand = false;
							if( pendingLabelXfer == true ) {
								pendingLabelXfer = false;

								sendRequestLabelTransfer( 0, NULL, false, true );
								Debug::trace("SSPrinterManager.LabelTransfer", " Aborting up on Bulk Transfer");

								labelDataSize = 0;
								labelData = NULL;
								outOfDataBuffers = false;
							}

							printerState = PrinterPriorityError;
							Debug::trace("SSPrinterManager.StateMachine", " PrinterPriorityError");

							Debug::trace("SSPrinterManager.PrinterReset", " resetPrinter1");
							sendPrinterFlush();
							emptyProductTransactionQ( false );
							emptyCommandQ();

							pCurrentProduct = NULL;
							Debug::trace("SSPrinterManager.PrinterImaging", " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));
							currentProductSmartPtr = Util::CSmartPtr<Product>();

							pCurrentLabel = NULL;

							if( printingVoidLabel == true ) {
								printingVoidLabel = false;

								if( testLabelType == PrVoidLabel ) {
									testLabelType = saveTestLabelType;
								}
							}

							dispatchLabelComplete( pCurrentProductTransaction, currentTransNum, false );
							pCurrentProductTransaction = NULL;
							currentPackId = 0;
						}
					}

					Debug::trace("SSPrinterManager.LabelTransfer", " outOfDataBuffers: " + HobString(Bool::toString(outOfDataBuffers)));
					if( ( outOfDataBuffers == false ) && ( printerStatus.isOutOfDataBuffers() == true ) ) {
						outOfDataBuffers = true;
					}

					if( ( outOfDataBuffers == true ) && ( printerStatus.isOutOfDataBuffers() == false ) ) {
						outOfDataBuffers = false;
						if( ( labelDataSize > 0 ) && ( pendingLabelXfer == false ) )
							sendLabelData();
					}

					if( ( printerResetting == true ) && ( printerStatus.isResetCmdComplete() == true ) ) {
						executingCommand = false;
						printerResetting = false;
						Debug::trace("SSPrinterManager.PrinterReset", " 2printerResetting: " + HobString(Bool::toString(printerResetting)));

					} else if( ( printerCalibrating == true ) && ( printerStatus.isCalibrateCmdComplete() == true ) ) {
						executingCommand = false;
						printerCalibrating = false;
						Debug::trace("SSPrinterManager.PrinterCalibration", " printerCalibrating: " + HobString(Bool::toString(printerCalibrating)));
					} else if( ( ( printerAdvancing == true ) && ( printerStatus.isAdvanceCmdComplete() ) == true ) ) {
						executingCommand = false;
						printerAdvancing = false;
						Debug::trace("SSPrinterManager.PrinterAdvance", " printerAdvancing: " + HobString(Bool::toString(printerAdvancing)));
					} else {
						if( lastStatusMsg.isTakeLabelError() == true ) {
							printerStatus.setTakeLabelError();
						}

						if( lastStatusMsg.isLabelSizing() == true ) {
							printerStatus.setLabelSizing();
						}

						if( lastStatusMsg.isPeripheralComm() == false ) {
							printerStatus.clearPeripheralComm();
						}

						if( printerStatus.isHeadDisengaged() == true ) {
							if( printerState != PrinterHeadDisengaged ) {
								sendPrinterReset( 2 );

								printerState = PrinterHeadDisengaged;
								Debug::trace("SSPrinterManager.StateMachine", " PrinterHeadDisengaged");

								stockLoaded.setLength( 0 );
								printerStockMetrics.setLabelLengthInLines( 0 );

								if( labelWidthStatus == true ) {
									stockLoaded.setWidth( UnknownLabel );
									stockLoaded.setWidthDots( UnknownLabelWidth );
									printerStockMetrics.setLabelLengthInLines( 0 );
									printerStockMetrics.setLabelWidth( UnknownLabel );
								}

								dispatchEvent( &printerStockMetrics );
								Debug::trace("SSPrinterManager.PrinterSize", " Dispatched printerStockMetrics -- HeadDisengaged");
							}
						} else {
							if( printerState == PrinterHeadDisengaged ) {
								printerState = PrinterUnsized;
								Debug::trace("SSPrinterManager.StateMachine", " PrinterUnsized");
							}
						}

						if( ( ( printerState == PrinterXferringLabel ) && ( printerStatus.isTaking() ) == true ) ) {
							printerState = PrinterTakingLabel;
							Debug::trace("SSPrinterManager.StateMachine", " PrinterTakingLabel");
						}

						if( ( printerState == PrinterTakingLabel ) && ( printerStatus.isLabelTaken() == false || takeLabelTime == 0 ) ) {
							printerState = PrinterLabelIsReady;

							printerStatus.setLabelReady();
							if( takeLabelTime != 0 ) {
								createTimer( takeLabelTime, takeLabelTimerExpired, true );
							}

							dispatchLabelReady( pCurrentProductTransaction, currentTransNum, currentPackId );
						}

						if( (printerStatus.isPrintCmdComplete() == true && takeLabelTime != 0) ||
							( printerState == PrinterLabelIsReady && takeLabelTime == 0 ) ) {
							bool haveAnotherLabelToPrint = false;  // for virtual printing

							dispatchLabelTaken( pCurrentProductTransaction, currentTransNum, currentPackId );
							executingCommand = false;
							Debug::trace("SSPrinterManager.StateMachine", " Print Command Complete");

							if( takeLabelError == true ) {
								Debug::trace("SSPrinterManager.StateMachine", " *** takeLabelError == true ***");
							}

							takeLabelError = false;

							if( takeLabelTime == 0 ) {
								sendPrinterFlush();
								sendPrinterReset( 0 );
								emptyCommandQ();
								sendPrinterEnableTakeup();
							}

							if( printingVoidLabel == true ) {
								printingVoidLabel = false;

								if( testLabelType == PrVoidLabel ) {
									testLabelType = saveTestLabelType;
								}
							} else {
								if( printerStatus.isLabelMissing() == true ) {
									// no label was printed
									Debug::trace("SSPrinterManager.LabelMissing", " Label Missing");
									dispatchLabelComplete( pCurrentProductTransaction, currentTransNum, false );
								} else {
									// if virtual printing, every single label does not count as a label printed.
									// If multiple labels are printed because the product has multiple label types set up (2 for example),
									// in production entry mode, we only incrementNumOfLabelsPrinted when both labels have printed, not each one.

									// if virtual printer, here is where we should go ahead and print the next label
									if( virtualPrintingEnabled ) {
										for( int i = (short)currentVirtuallyPrintingLabelNum + 1; i < (short)MaxPrinterTypes; i++ ) {
											if( imageControlBlock[i] ) {
												currentVirtuallyPrintingLabelNum = (PrinterType)i;
												haveAnotherLabelToPrint = true;
												break;
											}
										}

										if( haveAnotherLabelToPrint )
											startPrintingLabel( currentVirtuallyPrintingLabelNum );
									}

									if( !virtualPrintingEnabled || !haveAnotherLabelToPrint ) {
										if( pCurrentProductTransaction )
											const_cast<ProductTransaction *>(pCurrentProductTransaction)->incrementNumOfLabelsPrinted();

										Debug::trace("SSPrinterManager.StateMachine", "PrStatus dispatching label complete");
										dispatchLabelComplete( pCurrentProductTransaction, currentTransNum, true );
										pCurrentProductTransaction = NULL;
										currentPackId = 0;
									}
								}
							}

							testLabelPrintInProgress = false;

							if( continuousTestLabels == true ) {
								createTimer(continuousLabelTime, continuousLabelTimerExpired);
							} else {
								//	PTR 1399: Don't change to PrinterReadyToPrint if (printerState == PrinterPriorityError)
								if ( !haveAnotherLabelToPrint && ( printerState != PrinterPriorityError ) &&
																 ( printerState != PrinterUnsized ) &&
																 ( printerState != PrinterHeadDisengaged ) ) {
									printerState = PrinterReadyToPrint;
									Debug::trace("SSPrinterManager.StateMachine", " 3) PrinterReadyToPrint");
								}
							}
						}

						if( printerState == PrinterTryingToSize ) {
							bool isTracingSize = Debug::isTracing("PrinterManager.PrinterSize");
							if (isTracingSize)
								Debug::trace("SSPrinterManager.PrinterSize", " handling PrStatus - entering block for PrinterTryingToSize");
							if( printerStatus.isSizeCmdComplete() == true )  {
								if (isTracingSize)
									Debug::trace("SSPrinterManager.PrinterSize", " handling PrStatus - isSizeCmdComplete() == true");
								executingCommand = false;
								printerStatus.clearLabelSizing();

								measuredSize = printerStatus.getCounter();
								actualSize = (UINT16)printerStockMetrics.getActualLabelLengthLines(measuredSize, prKind);

								if( isTracingSize ) {
									Debug::trace("SSPrinterManager.PrinterSize", " Size Command Completed");
									Debug::trace("SSPrinterManager.PrinterSize", " Actual Size: " + HobString(Integer::toString(actualSize)));
									Debug::trace("SSPrinterManager.PrinterSize",  " Measured Size: " + HobString(Integer::toString(measuredSize)));
									Debug::trace("SSPrinterManager.PrinterSize", " printerMode: " + HobString(Integer::toString(printerMode)));
								}

								if( actualSize > 0 ) {

									if (isTracingSize)
										Debug::trace("SSPrinterManager.PrinterSize", " handling PrStatus - actualSize > 0");
									// Sized a die-cut label
									if( measuredSize > 0 ) {
										usingContinuousStock = false;

										if( isTracingSize )
											Debug::trace("SSPrinterManager.PrinterSize", " handling PrStatus - measuredSize > 0");
										if( printHeelFirst == true ) {
											printerMode =  DieCutLabelHeel;
										} else {
											printerMode =  DieCutLabelHead;
										}

										if( pTranslator != NULL ) {
											Debug::trace("SSPrinterManager.PrinterNomSize", " handling printerStatus.isSizeCmdComplete() - NOT continuous - actualSize=" + inttostring(actualSize) + "   measuredSize=" + inttostring(measuredSize));
											PrinterNomSize prNomSize( this, stationId, actualSize, measuredSize );

											const char *pCC = prNomSize.serialize();
											pTranslator->send( pCC, prNomSize.getSerializedLength() );

											printerStockMetrics.setLabelLengthInLines( actualSize );

											if( labelWidthStatus == true ) {
												if( printerStatus.isNarrowLabel() == true ) {
													stockLoaded.setWidth( NarrowLabel );
													printerStockMetrics.setLabelWidth( NarrowLabel );
												} else {
													stockLoaded.setWidth( WideLabel );
													printerStockMetrics.setLabelWidth( WideLabel );
												}

												updateLabelWidth( printerStockMetrics.getLabelWidth() );
											}
										}
										teachPrinter(printerMode);
									}
								} else {
									if( isTracingSize )
										Debug::trace("SSPrinterManager.PrinterSize", " handling PrStatus - actualSize = 0");
									if( !printerStatus.isOutOfMedia() ) {

										if( isTracingSize )
											Debug::trace("SSPrinterManager.PrinterSize", " handling PrStatus - Length determined to be 0, but not out of media, so assume is continuous stock");

										// Length determined to be 0, but not out of media, so assume is continuous stock.
										usingContinuousStock = true;

										measuredSize = LengthContinuous_Lines;
										actualSize = LengthContinuous_Lines;

										if( printHeelFirst == true )
											printerMode =  ContinuousLabelHeel;
										else
											printerMode =  ContinuousLabelHead;

										if( pTranslator != NULL ) {
											Debug::trace("SSPrinterManager.PrinterNomSize", " handling printerStatus.isSizeCmdComplete() - continuous - actualSize=" + inttostring(actualSize) + "   measuredSize=" + inttostring(measuredSize));
											PrinterNomSize prNomSize( this, stationId, actualSize, measuredSize );

											const char *pCC = prNomSize.serialize();
											pTranslator->send( pCC, prNomSize.getSerializedLength() );

											printerStockMetrics.setContinuousStock();

											if( labelWidthStatus == true ) {
												if( printerStatus.isNarrowLabel() == true ) {
													stockLoaded.setWidth( NarrowLabel );
													printerStockMetrics.setLabelWidth( NarrowLabel );
												} else {
													stockLoaded.setWidth( WideLabel );
													printerStockMetrics.setLabelWidth( WideLabel );
												}

												updateLabelWidth( printerStockMetrics.getLabelWidth() );

												if( printerConfig.getLabelWidth() != printerStockMetrics.getLabelWidth() ) {
													printerConfig.setLabelWidth( printerStockMetrics.getLabelWidth() );
													sendPrinterConfig( printerConfig.getConfigType() );
												}
											}
										}

										teachPrinter( printerMode );
									} else {
										if( isTracingSize )
											Debug::trace("SSPrinterManager.PrinterSize", " handling PrStatus - calling setLabelSizingFailed()");
										printerStatus.setLabelSizingFailed();
									}
								}

								//create a label stock descriptor
								stockLoaded.setLength( actualSize );
								stockLoaded.setWidth( labelWidth );
								stockLoaded.setSkew( 0 );

								PrTestLabelType saveTestLabelType = testLabelType;
								testLabelType = PrNoTestLabel;
								createTestLabel( saveTestLabelType );

								// Only issue the Find command if we are using die-cut labels, not continuous stock,
								// since the printer isn't going to find the sync bar on continuous stock.
								// This is what was wasting the 2nd 10" of continuous stock when sizing!
								if( !usingContinuousStock ) {
									if( takeLabelTime == 0 ) {
										queueCommand( FindwoLabelTakenCommand, PrNoSecurityLabel, PrNoIndirection, 0x0 );
										if (isTracing)
											Debug::trace("SSPrinterManager.PrinterStatus", " sent FindwoLabelTakenCommand Command!");
									} else {
										queueCommand( FindCommand, PrNoSecurityLabel, PrNoIndirection, 0x0 );
										if( isTracing )
											Debug::trace("SSPrinterManager.PrinterStatus", " sent Find Command!");
									}
								}
								labelPresent = false;
								Debug::trace("SSPrinterManager.LabelMissing", " labelPresent");
							}

							if( printerStatus.isLabelTaken() == false && takeLabelTime != 0 ) {
								printerStatus.setTakeLabelError();
								createTimer( takeLabelTime, takeLabelTimerExpired, true );
							}

							// Finish the sizing algorithm, and set up for printing again, if we have continuous stock,
							// or when the Find command (that we issued directly above) finishes for die-cut labels
							if( ( printerStatus.isSizeCmdComplete() && usingContinuousStock ) ||
							     printerStatus.isFindCmdComplete() ||
							     printerStatus.isFindwoLabelTakenCmdComplete() ) {
								executingCommand = false;

								printerState = PrinterReadyToPreImage;
								Debug::trace("SSPrinterManager.StateMachine", " PrinterReadyToPreImage 2");

								checkLabelWidth( printerStatus );

								if( newProductSmartPtr.get() ) {
									currentProductSmartPtr = newProductSmartPtr;
									pCurrentProduct = currentProductSmartPtr.get();
									newProductSmartPtr = Util::CSmartPtr<Product>();

									Debug::trace("SSPrinterManager.PrinterImaging", " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));
									if( labelWidth != UnknownLabel ) {
										preImageLabel();
									}
								}

								dispatchEvent( &printerStockMetrics );
								if( isTracingSize )
									Debug::trace("SSPrinterManager.PrinterSize", " Dispatched printerStockMetrics -- SizingComplete");

								// Don't just send these next commands if we have continuous stock, send for die cut too.
								// We noticed that sometimes after sizing, the printer's stepper motor would be engaged, making a high pitched sound.
								// These commands got rid of it.
								sendPrinterFlush();
								sendPrinterReset( 0 );
								emptyCommandQ();
								sendPrinterEnableTakeup();
							}
						}
					}

					if( printerStatus.isLabelTaken() == true || printerStatus.isHeadDisengaged() == true ) {
						printerStatus.clearTakeLabelError();
						cancelTimer( takeLabelTimerExpired );
					}

					if( printerStatus.isOutOfMedia() && printerStatus.isMediaJammed() ) {
						// When we are out of labels the media jammed flag is set also, reporting Jammed first, then Out of labels.
						// Just report that we are Out of labels.
						printerStatus.clearMediaJammed();
					}

					dispatchEvent( &printerStatus );
					lastStatusMsg = printerStatus;
				}
				break;

				case PrReset:
				case PrResetImmediate: {
					Debug::trace("SSPrinterManager.Message", ((prEvent == PrReset) ? " PrReset" : "PrResetImmediate"));
					if( printerState != PrinterTryingToSize ) {
						if( prEvent == PrReset && ( ( printerState == PrinterXferringLabel ) ||
						  ( printerState == PrinterTakingLabel ) || ( printerState == PrinterLabelIsReady ) ) ) {
							resetPrinter = true;
							Debug::trace("SSPrinterManager.PrinterReset", " resetPrinter: " + HobString(Bool::toString(resetPrinter)));
						} else {
							Debug::trace("SSPrinterManager.PrinterReset", " resetPrinter3");
							sendPrinterReset( 3 );
							emptyProductTransactionQ( false );
							emptyCommandQ();

							pCurrentProduct = NULL;
							pCurrentLabel = NULL;
							Debug::trace("SSPrinterManager.PrinterImaging", " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));
							currentProductSmartPtr = Util::CSmartPtr<Product>();
							newProductSmartPtr = Util::CSmartPtr<Product>();

							// FT-882 do not dispatch label complete message as pCurrentProductTransaction is most likely bad
							//dispatchLabelComplete(pCurrentProductTransaction, currentTransNum, false);
							pCurrentProductTransaction = NULL;
							currentPackId = 0;

							if( printerState != PrinterTryingToSize ) {
								printerState = PrinterReadyToPrint;
								Debug::trace("SSPrinterManager.StateMachine", " 4) PrinterReadyToPrint");
							}
						}
					}
				}
				break;
				case PrSizing: {
					Debug::trace("SSPrinterManager.Message", " PrSizing");
					PrinterSizing prSizing( recvBuffer );

					if( Debug::isTracing("PrinterManager.PrinterSize") == true ) {
						prSizing.show();
					}

					enableSizing = prSizing.isSizing();
				}
				break;
				case PrNomSize: {
					Debug::trace("SSPrinterManager.Message", " PrNomSize");

					PrinterNomSize printerSize(this, stationId, actualSize, measuredSize);
					dispatchEvent(&printerSize);
				}
				break;
				case PrReqCalibrate: {
					Debug::trace("SSPrinterManager.Message", " PrReqCalibrate");

					PrinterRequestCalibrate prReqCalib( recvBuffer );

					if( Debug::isTracing("PrinterManager.PrinterCalibration") == true ) {
						prReqCalib.show();
					}

					if(printerState == PrinterTryingToSize ) {
						Debug::trace("SSPrinterManager.PrinterReset", " resetPrinter4");
						sendPrinterReset( 4 );
					}

					if( prReqCalib.getPrinterCalibrateType() == PrCalibrateLabelTaken ) {
						lastStatusMsg.setCalibrateCmdComplete();
						dispatchEvent( &lastStatusMsg );
					} else {
						queueCommand( CalibrateCommand, PrNoSecurityLabel, PrNoIndirection, (UINT16)prReqCalib.getPrinterCalibrateType() );
					}
				}
				break;
				case PrReqInquire: {
					if( pTranslator != NULL) {
						requestedPrinterInquire = true;
						pTranslator->send( (const char*) recvBuffer, bufSize );
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
				case PrTest:
				case PrMask: {
					if( pTranslator != NULL) {
						pTranslator->send( (const char*) recvBuffer, bufSize );
					}
				}
				break;
				case PrReqPrintheadPower: {
					Debug::trace("SSPrinterManager.Message", " PrReqPrintheadPower");
					if( pTranslator != NULL ) {
						PrinterRequestPrintheadPower prPower( recvBuffer );
						pTranslator->send( (const char*) recvBuffer, bufSize );

						if( prPower.isPowerState() == true ) {
							Debug::trace("SSPrinterManager.PrintheadPower", " enabling power timer");
							createTimer( printheadPowerTime, printheadPowerTimerExpired );
						} else {
							Debug::trace("SSPrinterManager.PrintheadPower", " disbling power timer");
							cancelTimer( printheadPowerTimerExpired );
						}

						if( Debug::isTracing("PrinterManager.PrintheadPower") == true ) {
							prPower.show();
						}
					}
				}
				break;
				case PrReqSensors: {
					Debug::trace("SSPrinterManager.Message", " PrReqSensors");
					if( pTranslator != NULL ) {
						pTranslator->send( (const char*)recvBuffer, bufSize );

						lastSensorsMsg = PrinterSensors();
					}
				}
				break;
				case PrReqStockMetrics: {
					Debug::trace("SSPrinterManager.Message", " PrReqStockMetrics");
					if( Debug::isTracing("PrinterManager.PrinterStockMetrics") == true ) {
						printerStockMetrics.show();
					}

					dispatchEvent( &printerStockMetrics );
				}
				break;
				case PrSensorsTracking: {
					Debug::trace("SSPrinterManager.Message", " PrSensorsTracking");
					PrinterSensorsTracking prTracking( recvBuffer );

					trackSensors = prTracking.getSensorsTracking();

					if( trackSensors > 0 )
						Debug::trace("SSPrinterManager.PrinterSensors", "trackSensors: " + HobString(Bool::toString(true)));
					else
						Debug::trace("SSPrinterManager.PrinterSensors", "trackSensors: " + HobString(Bool::toString(false)));

					if( trackSensors > 0 ) {
						lastSensorsMsg = PrinterSensors();
						Debug::trace("SSPrinterManager.PrinterSensors", "start timer val: " + HobString(Integer::toString(requestSensorTime)));
						createTimer( requestSensorTime, sensorTimerExpired, CHobartTimer::PERIODIC_TIMER );
					} else {
						cancelTimer( sensorTimerExpired );
					}
				}
				break;
				case PrSensors: {
					Debug::trace("SSPrinterManager.Message", " PrSensors");
					PrinterSensors printerSensors( recvBuffer );

					// Since prepack printers do not return this!
					if( prKind == KPPPrinter ) {
						printerSensors.setLabelWidth( labelWidth );
					}

					if( printerSensors.equals( lastSensorsMsg ) == false ) {
						printerSensors.setSourceObject( this );
						printerSensors.setPrinterType( stationId );

						if (Debug::isTracing("PrinterManager.PrinterSensors") == true) {
							printerSensors.show();
						}

						lastSensorsMsg = printerSensors;
						dispatchEvent( &printerSensors );
					}

					clearPrinterProbe();
				}
				break;
				case PrTestLabel: {
					bool isTracing = Debug::isTracing("PrinterManager.TestLabel");
					if( isTracing )
						Debug::trace("SSPrinterManager.TestLabel", " PrTestLabel  printerState:" + inttostring(printerState));
					PrinterPrintTestLabel testLabel( recvBuffer );
					PrTestLabelType labelType = testLabel.getPrTestLabelType();
					PrTestLabelQuantity labelQuantity = testLabel.getTestLabelQuantity();
					applySecurityLabel = testLabel.getPrTestCommandOptions();
					bool canPrint = ( printerCalibrating == false ) &&
									( printerResetting == false ) &&
									( ( printerState == PrinterReadyToPrint ) ||
									( printerState == PrinterReadyToPreImage ) ||
									( printerState == PrinterPreImaging ) );

					Debug::trace("SSPrinterManager.TestLabel", " canPrint: " + inttostring(canPrint) + " printer state: " + Integer::toString(printerState) );
					if( isTracing ) {
						testLabel.show();
					}

					if( labelType == PrVoidLabel ) {
						saveTestLabelType = testLabelType;
						testLabelType = PrVoidLabel;

						sendTestLabel = true;
						printerState = PrinterLabelIsReady;
						Debug::trace("SSPrinterManager.StateMachine", " PrinterLabelIsReady");
					} else {
						switch( labelQuantity )
						{
							case PrOneTestLabel:
								if( canPrint == true ) {
									if( createTestLabel( labelType ) == true ) {
										sendTestLabel = true;
										printerState = PrinterLabelIsReady;
										Debug::trace("SSPrinterManager.StateMachine", " PrinterLabelIsReady");
									}
								} else {
									Debug::trace("PrinterManager.PrinterStatus", "StateMachine: " +  Integer::toString(printerState));
								}
								break;
							case PrStartContinuous:
								if( ( canPrint == true ) && ( continuousTestLabels == false ) ) {
									if( createTestLabel( labelType ) == true ) {
										continuousTestLabels = true;

										if( printerMode == ContinuousLabelHeel || printerMode == ContinuousLabelHead ) {
											if( printHeelFirst == true )
												teachPrinter( ContinuousStreamHeel, 0, OutOfDataBuffersBit, TakingBit );
											else
												teachPrinter( ContinuousStreamHead, 0, OutOfDataBuffersBit, TakingBit );
										} else if ( printerMode != DieCutStreamHeel && printerMode != DieCutStreamHead ) {
											if( printHeelFirst == true )
												teachPrinter( DieCutStreamHeel, 0, OutOfDataBuffersBit, TakingBit );
											else
												teachPrinter( DieCutStreamHead, 0, OutOfDataBuffersBit, TakingBit );
										}

										sendTestLabel = true;
										printerState = PrinterLabelIsReady;
										Debug::trace("SSPrinterManager.StateMachine", " PrinterLabelIsReady");
									}
								}
								break;
							case PrStopContinuous:
								if( continuousTestLabels == true ) {
									printerState = PrinterPriorityError;

									sendTestLabel = true;
									continuousTestLabels = false;
								}
								break;
						}
					}
				}
				break;
				case PrReqLabelWidth: {
					Debug::trace("SSPrinterManager.LabelWidth", " PrReqLabelWidth");
					PrinterLabelWidth prLabelWidth( this, stationId, labelWidth );

					dispatchEvent( &prLabelWidth );
				}
				break;
				//TO DO: Label width should be determined from one source not (metrics, status, sensors msgs).
				case PrLabelWidth: {
					Debug::trace("SSPrinterManager.LabelWidth", " PrLabelWidth");
					if( labelWidthStatus == false ) {
						PrinterLabelWidth prLabelWidth( recvBuffer );

						LabelWidth newLabelWidth = prLabelWidth.getLabelWidth();

						printerConfig.setLabelWidth( newLabelWidth );
						sendPrinterConfig( PrPermanent );

						stockLoaded.setWidth( newLabelWidth );

						updateLabelWidth( newLabelWidth );
						updateHorizontalOffset();
						preImageLabel();

						PrTestLabelType saveTestLabelType = testLabelType;
						testLabelType = PrNoTestLabel;
						createTestLabel( saveTestLabelType );

						printerStockMetrics.setLabelWidth( newLabelWidth );
						printerStockMetrics.setLabelWidthInDots( stockLoaded.getWidthDots() );

						dispatchEvent( &printerStockMetrics );
						Debug::trace("SSPrinterManager.PrinterSize", " Dispatched printerStockMetrics -- PrLabelWidth");
					}
				}
				break;
				// this is here so that the printer order can be changed (first, second, third printer).
				// TO DO: since we no longer have node id's this will have to be changed....
				case PrNewOrder: {
					Debug::trace("SSPrinterManager.PrNewPrinterType", " PrNewOrder");
					PrinterNewPrinterType newPrinterType( recvBuffer );

					if (Debug::isTracing("PrinterManager.PrinterTypeShow") == true) {
						newPrinterType.show();
					}
					Debug::trace("SSPrinterManager.PrNewOrder", " PrNewOrder");

					PrinterType prType = newPrinterType.getNewPrinterType();

					// update peripheral configuration
					if( prType != stationId ) {

						Debug::trace("SSPrinterManager.PrinterType", " changing to PrinterType: " + HobString(Integer::toString((UINT16) prType)) );
						stationId = prType;

						printerConfig.setPrinterType(prType);

						const char *pCC = newPrinterType.serialize();
						pTranslator->send( pCC, newPrinterType.getSerializedLength() );

						printerStockMetrics.setPrinterType(prType);

						dispatchEvent(&printerStockMetrics);
						Debug::trace("SSPrinterManager.PrinterSize", " Dispatched printerStockMetrics -- PrNewOrder");
					}

					sendPrinterType();
					sendPrinterKind();
				}
				break;
				case PrTakeLabelTime: {
					Debug::trace("SSPrinterManager.PrinterImaging", " PrTakeLabelTime");
					PrinterTakeLabelTime prTakeLabelTime( recvBuffer );

					if( pConfig->isSelfServiceScale() == false )
					{
						takeLabelTime = prTakeLabelTime.getTakeLabelTime() * ticksPerSecond;
						if( takeLabelTime == 0 )
						{
							cancelTimer( takeLabelTimerExpired );

							sendPrinterFlush();
							sendPrinterReset( 0 );
							emptyCommandQ();
							sendPrinterEnableTakeup();
						}
					}
				}
				break;
				case PrPreImage: {
					bool isTracing = Debug::isTracing("PrinterManager.PrinterImaging");
					if (isTracing)
						Debug::trace("SSPrinterManager.PrinterImaging", " PrPreImage");
					emptyProductTransactionQ(false);

					PrinterPreImage preImage(recvBuffer);

					if (Debug::isTracing("PrinterManager.PrinterPreImage") == true) {
						preImage.show();
					}

					// If the the user is in normal mode and pop the cassette before going to self-service mode with 'print to screen' mode enabled,
					// the existing printer status would prevent the PrinterManager from rendering and displaying the label.  So reset the flags preventing
					// the pre-image so that the pre-image will occur.
					if( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgSelfServePrintToScreen) && Configuration::getInstance()->isSelfServiceScale() ) {
						printerState = PrinterReadyToPreImage;
						labelWidth = WideLabel;
					}

					if( ( printerState == PrinterReadyToPreImage ) || ( printerState == PrinterReadyToPrint ) ) {
						pCurrentProduct = NULL;
						currentProductSmartPtr = Util::CSmartPtr<Product>();

						if( labelWidth != UnknownLabel ) {
							if (isTracing)
								Debug::trace("SSPrinterManager.PrinterImaging", " Pre-image Now");

							Util::CSmartPtr<Product> *pCurrentProductSmartPtr = (Util::CSmartPtr<Product> *) preImage.getProductSmartPtr();
							if(pCurrentProductSmartPtr) {
								currentProductSmartPtr = *pCurrentProductSmartPtr;
								pCurrentProduct = currentProductSmartPtr.get();

								preImageLabel();
							}
						}

						if (isTracing)
							Debug::trace("SSPrinterManager.PrinterImaging", " pCurrentProduct: " + HobString(Hex::toString((long long) pCurrentProduct)));
					}else if ( ( printerState == PrinterLabelIsReady ) ||
							 ( ( labelWidth != UnknownLabel ) && ( ( printerState == PrinterUnsized ) || ( printerState == PrinterTryingToSize ) ) ) ) {
						if (isTracing) {
							Debug::trace("SSPrinterManager.PrinterImaging", " Pre-image Later printerState: " + HobString(Integer::toString(printerState)));
							Debug::trace("SSPrinterManager.PrinterImaging", " Pre-image Later labelWidth: " + HobString(Integer::toString(labelWidth)));
							Debug::trace("SSPrinterManager.PrinterImaging", " Pre-image Later");
						}

						Util::CSmartPtr<Product> *pCurrentProductSmartPtr = (Util::CSmartPtr<Product> *) preImage.getProductSmartPtr();
						if( pCurrentProductSmartPtr ) {
							newProductSmartPtr = *pCurrentProductSmartPtr;
							if (isTracing)
								Debug::trace("SSPrinterManager.PrinterImaging", " pNewProduct: " + HobString(Hex::toString((long long) newProductSmartPtr.get())));
						}
					} else {
						if (isTracing)
							Debug::trace("SSPrinterManager.PrinterImaging", " Can't pre-image now! printerState: " + HobString(Integer::toString(printerState)));
					}
				}
				break;
				case PrPostImage: {
					Debug::trace("SSPrinterManager.PrinterImaging", " PrPostImage");

					// Store ProductTransaction into productTA Q!!
					PrinterPostImage postImage( recvBuffer );

					const ProductTransaction *pPT = postImage.getProductTransaction();
					UINT8 packId = postImage.getPackId();
					Util::CSmartPtr<Product> *pProductSmartPtr = (Util::CSmartPtr<Product> *) postImage.getProductSmartPtr();

					if( printingVoidLabel == true ) {
						Debug::trace("SSPrinterManager.PrinterImaging", " printingVoidLabel");
						dispatchLabelComplete((ProductTransaction *) pPT, currentTransNum, false);
					} else {
						// Store ProductTransaction into productTA Q!!
						Debug::trace("SSPrinterManager.PrinterImaging", " Store ProductTransaction into  Q");
						pushProductTransactionQ( const_cast<ProductTransaction *>(pPT), packId, *pProductSmartPtr );
					}
				}
				break;
				case PrLabelTransferReady: {
					Debug::trace("SSPrinterManager.LabelTransfer", "Sending Label Data");
					sendLabelData();
					break;
				}
				case PrLabelTransferComplete: {
					pendingLabelXfer = false;

					Debug::trace("SSPrinterManager.LabelTransfer", " PrLabelTransferComplete");
					Debug::trace("SSPrinterManager.LabelTransfer", "printerState: " + Integer::toString(printerState));
					Debug::trace("SSPrinterManager.LabelTransfer", " outOfDataBuffers: " + HobString(Bool::toString(outOfDataBuffers)));
					if( printerState == PrinterReadyToXfer ) {

						if( pCurrentProductTransaction != NULL ) {
							if( pCurrentProductTransaction->printSecurityLabel() )
								applySecurityLabel = PrApplySecurityLabel;
						}
						queueCommand( PrintCommand, applySecurityLabel, PrRam0, (UINT16)linesToPrint );

						//after printing, reset applySecurityLabel
						applySecurityLabel = PrNoSecurityLabel;

						labelPresent = false;
						Debug::trace("SSPrinterManager.LabelMissing", " labelPresent");

						printerState = PrinterXferringLabel;
						Debug::trace("SSPrinterManager.StateMachine", " PrinterXferringLabel");
					}
					clearPrinterProbe();
				}
				break;
				case PrLabelTransferFailed: {
					pendingLabelXfer = false;

					Debug::trace("SSPrinterManager.LabelTransfer", " PrLabelTransferFailed");
					labelDataSize = 0;
					labelData = NULL;

					if( continuousTestLabels == true ) {
						continuousTestLabels = false;
						cancelTimer( continuousLabelTimerExpired );
					}
					testLabelPrintInProgress = false;

					dispatchLabelComplete( pCurrentProductTransaction, currentTransNum, false );
					pCurrentProductTransaction = NULL;
					currentPackId = 0;

					printerState = PrinterReadyToPrint;
					Debug::trace("SSPrinterManager.StateMachine", " 5) PrinterReadyToPrint");
				}
				break;
				case PrAdvLabelStock: {
					printerAdvancing = true;
					Debug::trace("SSPrinterManager.PrinterAdvance", " printerAdvancing: " + HobString(Bool::toString(printerAdvancing)));

					PrinterAdvanceLabelStock advLabelStock( recvBuffer );

					if( Debug::isTracing("PrinterManager.PrinterAdvance") == true ) {
						advLabelStock.show();
					}

					queueCommand(AdvanceCommand, PrNoSecurityLabel, PrRam1, advLabelStock.getNumSteps());
				}
				break;
				case PrReqKind: {
					sendPrinterKind();
				}
				break;
				case PrDeletePackage: {
					PrinterDeletePackage prDeletePackage( recvBuffer );

					const ProductTransaction *pPT = prDeletePackage.getProductTransaction();

					if( removeProductTransactionFromQ( const_cast<ProductTransaction *>(pPT) ) ) {
						Debug::trace("SSPrinterManager.StateMachine", "PrDeletePackage dispatching label complete");
						dispatchLabelComplete( pPT, currentTransNum, false );
					}
				}
				break;
				case PrReqLabelSizing: {
					// Need to check if this message is even for this PrinterManager
					Debug::trace("SSPrinterManager.PrinterSize", " handling PrReqLabelSizing");

					PrinterGeneric prReqLabelSizing( recvBuffer );
					if( prReqLabelSizing.getPrinterType() == stationId ) {
						// SOF-1798 - The cause of the crash was that the label was being rendered, due to the change in label stock when
						// going from opted-in continuous labels, to die cut labels when pressing the "Size Labels" button on the Size Labels
						// panel.  To remedy, I am including the code that updates printer stock metrics when the cassette is ejected
						// (Headup condition), prior to sizing.  The labelMediaError param in the LabelType object wasn't being set to
						// LabelWontFit, as was done when the cassette was ejected.

						stockLoaded.setLength( 0 );
						printerStockMetrics.setLabelLengthInLines( 0 );

						if( labelWidthStatus == true ) {
							stockLoaded.setWidth( UnknownLabel );
							stockLoaded.setWidthDots( UnknownLabelWidth );
							printerStockMetrics.setLabelLengthInLines( 0 );
							printerStockMetrics.setLabelWidth( UnknownLabel );
						}
						dispatchEvent( &printerStockMetrics );
						Debug::trace("SSPrinterManager.PrinterSize", " Dispatched printerStockMetrics -- PrReqLabelSizing pretending HeadDisengaged");

						// end of SOF-1798 fix
						usingContinuousStock = false;

						sendPrinterFlush();
						sendPrinterReset( 0 );
						emptyCommandQ();

						// SOF-1593 - user needs to opt-in for saving continuous label stock.
						// If we are set for continuous stock, we don't size labels, so we have to reprogram the teach tables to size labels.
						if( pMyTeachTable )
							pMyTeachTable->recreateTeachTable();
						teachPrinterNormalSizing();

						printerState = PrinterUnsized;
						enableSizing = true;

						Debug::trace("SSPrinterManager.PrinterSize", " handling PrReqLabelSizing - taught printer to size normally");
					}
					break;
				}
				case PrUseContinuousStock: {
					Debug::trace("SSPrinterManager.PrinterSize", " handling PrUseContinuousStock");

					PrinterGeneric prUseContinuousStock( recvBuffer );
					if( prUseContinuousStock.getPrinterType() == stationId && stationId == FirstPrinter ) {
						Debug::trace("SSPrinterManager.PrinterSize", " handling PrUseContinuousStock for my printer and is printer 1");

						usingContinuousStock = true;

						// Status message would have the number of lines set to 0 when it sizes continuous stock
						lastStatusMsg.setCounter( 0 );

						measuredSize = Configuration::getInstance()->toIntCfgI( CfgInternal::DIMBLENGTHCONTINUOUS );
						actualSize = measuredSize;

						if( printHeelFirst == true )
							printerMode = ContinuousLabelHeel;
						else
							printerMode = ContinuousLabelHead;

						if( pTranslator != NULL ) {
							Debug::trace("SSPrinterManager.PrinterSize", " handling PrUseContinuousStock - continuous - actualSize:=" + inttostring(actualSize) + "   measuredSize:=" + inttostring(measuredSize));
							PrinterNomSize prNomSize(this, stationId, actualSize, measuredSize);

							const char *pCC = prNomSize.serialize();
							pTranslator->send(pCC, prNomSize.getSerializedLength());
						}

						// SOF-1593 - user needs to opt-in for saving continuous label stock.
						// If we are set for die-cut stock, we size labels, so we have to reprogram the teach table to not size labels.
						if (pMyTeachTable)
							pMyTeachTable->recreateTeachTable();
						teachPrinter( printerMode );
						Debug::trace("SSPrinterManager.PrinterSize", " handling PrUseContinuousStock - taught printer to size for continuous");

						stockLoaded.setLength( actualSize );
						stockLoaded.setWidth( labelWidth );
						stockLoaded.setSkew( 0 );
						Debug::trace("SSPrinterManager.PrinterSize", " handling PrUseContinuousStock - setStockLoaded vals - length:" + inttostring(actualSize) + "   width:" + inttostring(labelWidth));

						printerStockMetrics.setContinuousStock();

						printerState = PrinterUnsized;
						enableSizing = true;

						Debug::trace("SSPrinterManager.PrinterSize", " handling PrUseContinuousStock - end");
					}
					break;
				}

				case PrSetupForNormalSizing: {
					Debug::trace("SSPrinterManager.PrinterSize", " handling PrSetupForNormalSizing");

					// If we are set for continuous stock, we don't size labels, so we have to reprogram the teach table to size labels.
					// So just re-teach the sizing command in case we are set for continuous.
					usingContinuousStock = false;
					teachPrinterNormalSizing();
					break;
				}
				case PrHeadDotWear: {
					PrinterHeadWear dotWear( recvBuffer );
					Debug::trace("SSPrinterManager.PrinterSize", " handling PrHeadDotWear");
					dotWear.show();

					dispatchEvent( &dotWear );
					clearPrinterProbe();
					break;
				}
				case PrKPCError: {
					PrinterKpcError error( recvBuffer );
					std::string str = "KPC Firmware: " + error.getErrorDescription();
					Debug::trace("SSPrinterManager.Error", str);
					break;
				}
				default:
				{
					Debug::trace("SSPrinterManager.Error", " Some Other PrEvent:" + HobString(Integer::toString((UINT16) prEvent)));
				}
				break;
			}
		} else {
			if( pTranslator == NULL ) {
				Debug::trace("SSPrinterManager.Error", "translator is NULL!");
			}
		}
	}
#endif // #ifdef linux
}

void SSPrinterManager::handlePrinterSysMessage(SysEvent sysEvent, const char *recvBuffer)
{
	switch (sysEvent)
	{
		case SysReqStatus: {
			Debug::trace("SSPrinterManager", "handlePrinterSysMessage()  SysReqStatus");
			PrinterSystemRequestStatus psrs(recvBuffer);

			if ((psrs.getPrinterType() == stationId ) && ( pTranslator != NULL ) ) {
				PrinterGeneric msg( PrReqVersion, this, stationId );
				const char *pCC = msg.serialize();
				pTranslator->send(pCC, msg.getSerializedLength());
			}
		}
		break;
		default: {
			Debug::trace("SSPrinterManager", "Unsupported CAN Printer system event: " + HobString( Integer::toString( (UINT16)sysEvent ) ) );
		}
		break;
	}
}

void SSPrinterManager::handleKpcSystemError(KpcSystemEvent event, const char *recvBuffer)
{
	switch (event)
	{
		case KpcError: {
			KpcSystemErrors error( recvBuffer );
			std::string str = "KPC System: " + error.getErrorDescription();
			Debug::trace("SSPrinterManager.Error", str);
			break;
		}
	}
}


void SSPrinterManager::startPrintingLabel( PrinterType labelNumToPrint )
{
#if defined ( __linux__ )
	const bool tracing( Debug::isTracing( "PrinterManager.PrinterImaging" ) );

	ImageControl *ic;

	// Download and print the label here!!
	if( !pTranslator )
		return;
	else if( !(ic = imageControlBlock[labelNumToPrint]) )
		return;

	// Flush the printer
	sendPrinterEnableTakeup();
	sendPrinterFlush();

	if ( ic->getPrintingRotatedBitmap() )
		labelDataSize = ic->getRotatedBitMapSize();
	else
		labelDataSize = ic->getImageBitMapSize();

	linesToPrint = labelDataSize / ( printheadSize / 8 );
	if( tracing ) {
		Debug::trace("SSPrinterManager.StateMachine", " labelDataSize: " + HobString(Integer::toString(labelDataSize)));
		Debug::trace("SSPrinterManager.StateMachine", " linesToPrint: " + HobString(Integer::toString(linesToPrint)));
	}

	if( printerStockMetrics.isContinuousStock() && ic->isImageOptionsScrunch() ) {
		UINT16 verticalOffset = ic->getOffsetFromTable((INT16)linesToPrint);
		if( verticalOffset ) {
			linesToPrint -= verticalOffset;
			labelDataSize = linesToPrint * (printheadSize / 8);
			if( tracing ) {
				Debug::trace("SSPrinterManager.StateMachine", " corrected labelDataSize: " + HobString(Integer::toString(labelDataSize)));
				Debug::trace("SSPrinterManager.StateMachine", " corrected linesToPrint: " + HobString(Integer::toString(linesToPrint)));
			}
		}
	}

	printerState = PrinterReadyToXfer;
	Debug::trace("SSPrinterManager.StateMachine", " PrinterReadyToXfer");

	xferLabelStart = PAL_absclass::PAL_getTickCount();

	if( ic->getPrintingRotatedBitmap() )
		pDestinationData = rotatedBitMap[labelNumToPrint].getBitMapBase();
	else
		pDestinationData = bitMap[labelNumToPrint].getBitMapBase();

	if( printHeelFirst ) {
		Debug::trace("SSPrinterManager.StateMachine", " print heel first");
		labelData = (UINT16 *) ( (long long) pDestinationData + (long long) labelDataSize);
	} else {
		Debug::trace("SSPrinterManager.StateMachine", " print normal");
		labelData = pDestinationData;
	}
	sendLabelData();
#endif // #ifdef linux
}

void SSPrinterManager::maskCommand(UINT8 sensorMask, UINT8 sensor2Mask, UINT8 userMask)
{
	if (pTranslator != NULL) {
		PrinterMask printerMask( this, stationId, sensorMask, sensor2Mask, userMask );

		const char *pCC = printerMask.serialize();
		int mLength = printerMask.getSerializedLength();

		pTranslator->send( pCC, mLength );
	}
}

void SSPrinterManager::teachCommand(PrinterCommandType command, PrinterModeType mode)
{
	if( ( pTranslator != NULL ) && ( pMyTeachTable != NULL ) ) {
		PrinterTeach printerTeach( this, stationId, command, pMyTeachTable->getCommandOperationList( command, mode ) );

		const char *pCC = printerTeach.serialize();
		int mLength = printerTeach.getSerializedLength();

		pTranslator->send( pCC, mLength );
	}
}

void SSPrinterManager::sendPrinterReset( UINT8 location )
{
#if defined ( __linux__ )
	printerResetting = true;
	printerCalibrating = false;
	printerAdvancing = false;

	if( pTranslator != NULL ) {
		
	    /* SOF-3524*/
		if( ( printerStockMetrics.isContinuousStock() ) && ( printerState == PrinterXferringLabel ) ) {
			Debug::trace("PrinterManager.PrinterReset", "Do not quit in middle of print job!!! location: " + Integer::toString(location));
			return;
		}		
		PrinterReset printerReset(this, myPrinterType);

		const char *pCC = printerReset.serialize();
		pTranslator->send( pCC, printerReset.getSerializedLength() );
	}
#endif // #ifdef linux
}

void SSPrinterManager::sendPrinterConfig( PrConfigType configType )
{
	if (pTranslator != NULL) {
		printerConfig.setConfigType( configType );

		const char *pCC = printerConfig.serialize();
		pTranslator->send( pCC, printerConfig.getSerializedLength() );
	}

}

void SSPrinterManager::sendPrinterFlush()
{
	if (pTranslator != NULL) {
		PrinterFlush printerFlush( this, stationId );

		const char *pCC = printerFlush.serialize();
		pTranslator->send( pCC, printerFlush.getSerializedLength() );
	}
}

void SSPrinterManager::sendPrinterEnableTakeup()
{
	//CAN Prepack printer support
	if ( ( pTranslator != NULL ) && ( prKind == KPPPrinter ) ) {
		PrinterEnableTakeup printerEnableTakeup( this, stationId );

		const char *pCC = printerEnableTakeup.serialize();
		pTranslator->send( pCC, printerEnableTakeup.getSerializedLength() );
	}
}

void SSPrinterManager::sendRequestLabelTransfer(UINT16 bytesToXmit, UINT16 *pUint16, bool sendSetupXferMessage, bool cancelPendingTransfer)
{
	if (pTranslator != NULL) {
		Debug::trace("SSPrinterManager.LabelTransfer", "Sending Request to open 485!");
		// Download and print the label here!!
		PrinterRequestLabelTransfer printerRequestLabelTransfer(this, stationId, bytesToXmit, 0, pUint16, 0, 0);

		const char *pCC = printerRequestLabelTransfer.serialize();
		pTranslator->send( pCC, printerRequestLabelTransfer.getSerializedLength() );

		pendingLabelXfer = true;
	}
}

void SSPrinterManager::sendLabelDataHead()
{
	Debug::trace( "SSPrinterManager.LabelTransfer", "sendLabelDataHead() labelDataSize: " + Integer::toString(labelDataSize) +
			      " bufferSize: " + Integer::toString(bufferSize) );

#if defined ( __linux__ )
	if( labelDataSize <= bufferSize ) {

		Debug::trace("SSPrinterManager.LabelTransfer", "Sending Request to open 485!");

		PrinterRequestLabelTransfer requestLabelTransfer(this, stationId, labelDataSize, labelDataSize, labelData, 0, 0);
		const char *pCC = requestLabelTransfer.serialize();
		pTranslator->send( pCC, requestLabelTransfer.getSerializedLength() );

		pRS485Manager->assignLabelImage( stationId, (unsigned char *)labelData, labelDataSize, transferSize );

		//no need to send more data the 485Manager handles transfer of entire image.
		pendingLabelXfer = false;

		labelDataSize = 0;
		labelData = NULL;
	} else {
		Debug::trace("SSPrinterManager.LabelTransfer", "Sending Request to open 485!");

		PrinterRequestLabelTransfer requestLabelTransfer(this, stationId, ( bufferSize - transferSize ), labelDataSize, labelData, 0, 0);
		const char *pCC = requestLabelTransfer.serialize();
		pTranslator->send( pCC, requestLabelTransfer.getSerializedLength() );

		pRS485Manager->assignLabelImage( stationId, (unsigned char *)labelData, ( bufferSize - transferSize ), transferSize );

		labelDataSize -= ( bufferSize - transferSize );
		labelData = (UINT16 *) ((long long) labelData + (long long) bufferSize);
	}
#endif // #ifdef linux
}

void SSPrinterManager::sendLabelDataHeel()
{

	Debug::trace( "SSPrinterManager.LabelTransfer", "sendLabelDataHeel() labelDataSize: " + Integer::toString(labelDataSize) +
			      " bufferSize: " + Integer::toString(bufferSize) );

#if defined ( __linux__ )
	if( labelDataSize <= bufferSize ) {

		Debug::trace("SSPrinterManager.LabelTransfer", "Sending Request to open 485! transfer size: " + Integer::toString(labelDataSize));
		PrinterRequestLabelTransfer requestLabelTransfer(this, stationId, labelDataSize, labelDataSize, labelData, 0, 0);

		const char *pCC = requestLabelTransfer.serialize();
		pTranslator->send( pCC, requestLabelTransfer.getSerializedLength() );

		pRS485Manager->assignLabelImage( stationId, (unsigned char *)((long long) labelData - (long long) labelDataSize),
										 labelDataSize, transferSize );

		//no need to send more data the 485Manager handles transfer of entire image.
		pendingLabelXfer = false;

		labelDataSize = 0;
		labelData = NULL;
	} else {
		Debug::trace("SSPrinterManager.LabelTransfer", "Sending Request to open 485! transfer size: " + Integer::toString(( bufferSize - transferSize )));

		PrinterRequestLabelTransfer requestLabelTransfer(this, stationId, ( bufferSize - transferSize ), labelDataSize, labelData, 0, 0);
		const char *pCC = requestLabelTransfer.serialize();
		pTranslator->send( pCC, requestLabelTransfer.getSerializedLength() );

		unsigned char *pChar = (unsigned char *) ((long long) labelData - (long long) bufferSize);
		pRS485Manager->assignLabelImage( stationId, (unsigned char *)pChar, ( bufferSize - transferSize ), transferSize );

		labelDataSize -= ( bufferSize - transferSize );
		labelData = (UINT16 *)pChar;
	}
#endif // #ifdef linux
}

void SSPrinterManager::sendPrinterType()
{
	Debug::trace("SSPrinterManager.PrinterType", " sendPrinterType");

	PrinterPrinterType prType( this, stationId, 0, 0 );
	dispatchEvent( &prType );

	if( Debug::isTracing("PrinterManager.PrinterTypeShow") == true ) {
		prType.show();
	}
}


void SSPrinterManager::requestStatusMsg()
{
	if( pTranslator != NULL ) {
		PrinterRequestStatus prs( this, stationId );

		const char *pCC = prs.serialize();
		pTranslator->send( pCC, prs.getSerializedLength() );
	}
}

void SSPrinterManager::requestInquireMsg()
{
	Debug::trace("SSPrinterManager.PrinterInquire", "  requestInquireMsg():   stationId = " + HobString(Hex::toString(stationId)));

	if( pTranslator != NULL ) {
		Debug::trace("SSPrinterManager.PrinterInquire", "  requestInquireMsg():   Requesting the Inquire message.");
		PrinterRequestInquire pri( this, stationId );

		const char *pCC = pri.serialize();
		pTranslator->send( pCC, pri.getSerializedLength() );
	}
}

void SSPrinterManager::requestSensorMsg()
{
	if( ( pTranslator != NULL ) && ( printerResetting == false ) ) {
		PrinterRequestSensors prs( this, stationId );

		const char *pCC = prs.serialize();
		pTranslator->send( pCC, prs.getSerializedLength() );
	}
}

void SSPrinterManager::timeoutProcess( __int32 timer_id )
{
#if defined ( __linux__ )
	Debug::trace("SSPrinterManager.Timer", "timeoutProcess: " + Integer::toString(timer_id));

	if( timer_id == printerProbe ) {
		probeTimeoutCntr_++;
		switch( probeTimeoutCntr_ )
		{
			case 0:
				break;
			case 1:
				{
					requestWakeupMsg();
				}
				break;
			case 2:
				{
					requestWakeupMsg();
				}
				break;
			case 3:
				{

					Debug::trace("SSPrinterManager.PrinterProbe", HobString("3) Printer Lost Contact"));

					PrinterGeneric msg( PrKPCDetached, this, stationId  );
					dispatchEvent( &msg );

					probeTimeoutCntr_ = 0;

					if( lastStatusMsg.isPeripheralComm() == true ) {
						Debug::trace("SSPrinterManager.PrinterProbe", " sending lost comm message");

						lastStatusMsg.clearPeripheralComm();
						dispatchEvent(&lastStatusMsg);
					}
				}
				break;
			default:
				break;
		}
	} else if( timer_id == sensorTimerExpired ) {
		if( trackSensors == PrinterOn ) {
			requestSensorReading = true;
		}
	} else if( timer_id == takeLabelTimerExpired ) {
		takeLabelError = true;
	} else if( timer_id == printheadPowerTimerExpired ) {
		Debug::trace("SSPrinterManager.Timer", " Timeout:printheadPowerTimerExpired");

		PrintheadPower printheadPower (this, stationId, PrinterOff, PrinterOn);
		dispatchEvent( &printheadPower );

		if( pTranslator != NULL) {
			const char *pCC = printheadPower.serialize();
			pTranslator->send( pCC, printheadPower.getSerializedLength() );
		}

		if ( Debug::isTracing("PrinterManager.PrintheadPower") == true ) {
			printheadPower.show();
		}
	} else if ( timer_id == continuousLabelTimerExpired ) {
		Debug::trace("SSPrinterManager.Timer", " Timeout:continuousLabelTimerExpired");
		sendTestLabel = true;
		printerState = PrinterLabelIsReady;
		Debug::trace("SSPrinterManager.StateMachine", " PrinterLabelIsReady");
	} else if( timer_id == labelStatisticsTimer ) {
		if( (printerCounts_ > 0) && (printerCounts_ != previousPrinterCounts_) ) {
			FpmManager *pSerialFlash = FpmManager::getInstance();
			if( pSerialFlash != NULL ) {
                std::string style = pSerialFlash->getMFGSystemStyle();
                if( ( style == "HT" ) || ( style == "HTK" ) || ( style == "FSS" ) ) {
                    MtdScaleData *pData = pSerialFlash->readScaleData();
                    if( pData != NULL ) {
        				if( stationId == FirstPrinter )
        					pData->setPrimaryPrinterCounter( previousPrinterCounts_ = printerCounts_ );
        				else if( stationId == SecondPrinter )
        					pData->setSecondaryPrinterCounter( previousPrinterCounts_ = printerCounts_ );
        				else if( stationId == ThirdPrinter )
                			pData->setTertiaryPrinterCounter( previousPrinterCounts_ = printerCounts_ );

                        if( pSerialFlash->writeScaleData() != 0 )
                            Debug::trace("SSPrinterManager.Timer", "printer counter saved count: " + Integer::toString(printerCounts_));
                        else
                            Debug::trace("SSPrinterManager.Timer", "Failed to save printer counter!");
                    }
                } 
			}
		}
	} else if (timer_id == testLabelTimerTimeout) {
		Debug::trace("SSPrinterManager.Timer", "Create checkerboard test label and print");

		if( createTestLabel( PrCheckerboard ) ) {
			printTestLabel();
		} else {
			Debug::trace("SSPrinterManager.Timer", "Failed to create test label!");
		}
	}
#endif // #ifdef linux
}