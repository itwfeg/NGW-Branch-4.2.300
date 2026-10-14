/* This file is used for testing the application.
 * It is a test Printer Manager used for testing the application.
 * Larry
 */
 

//#include "Printer.h"
#include "PrinterManager.h"
#include "PrinterGeneric.h"
#include "PrinterWakeup.h"
#include "PrinterConfiguration.h"
#include "PrinterInquire.h"
#include "PrinterStatus.h"
#include "PrinterMode.h"
#include "PrinterCommand.h"
#include "PrinterRam.h"
#include "PrinterTest.h"
#include "PrinterTeach.h"
#include "PrinterMask.h"
#include "PrinterSensorsTracking.h"
#include "PrinterSensors.h"

#include "PrinterFlush.h"
#include "PrinterPreImage.h"
#include "PrinterPostImage.h"
#include "PrinterRequestLabelTransfer.h"

#include "PrinterSystemStatus.h"
#include "PrinterSystemConfiguration.h"
#include "PrinterSystemInquire.h"
#include "PrinterSystemRequestStatus.h"
#include "PrinterSystemRequestInquire.h"

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
#include "PrinterRequestCalibrate.h"

#include "Product.h"
#include "ProductTransaction.h"
#include "LabelType.h"
#include "SystemStatusInfo.h"

#include "ProductUpdateMsg.h"

#include "PrSession.h"
#include "MessageQueue.h"
#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Hex.h"
#include "Configuration.h"

PrSession::PrSession() : HobThread((FUNCPTR) activeFunction, "tPrintSession", 100, 0, 4096, 10, 1024, MSG_Q_FIFO)
{
/*	this->pMyPrinter = pMyPrinter;
	this->myPrinterType = printerType;
*/
	registerEvent(Printer, PrReqInquire, msgQId);
	registerEvent(Printer, PrReqStatus, msgQId);
	registerEvent(Printer, PrPreImage, msgQId);
	registerEvent(Printer, PrPostImage, msgQId);

	registerEvent(Printer, PrConfig, msgQId);
	registerEvent(Printer, PrSensorsTracking, msgQId);
	registerEvent(Printer, PrReqCalibrate, msgQId);
	registerEvent(Printer, PrPostImage, msgQId);

	registerEvent(PrinterSystem, SysConfig, msgQId);
	registerEvent(PrinterSystem, SysReqStatus, msgQId);
	registerEvent(PrinterSystem, SysReqInquire, msgQId);

	registerEvent(ProductDev, ProductChange, msgQId);
	registerEvent(ProductDev, ProductUpdate, msgQId);


	pCurrentProduct = NULL;
	pCurrentProductTransaction = NULL;
	pCurrentLabel = NULL;
 	
 	preImaging = false;
	postImaging = false;

	myRendererSessionId = 0;
	pSourceData = NULL;
	pDestinationData = NULL;
/*	pMyRenderer = RendererManager::getInstance();

	// get a session with the renderer!!
	RendererRequestSession rdReqSession(this);

	const char *pCC = rdReqSession.serialize();
	int mLength = rdReqSession.getSerializedLength();

	pMyRenderer->send(pCC, mLength);
*/
	takingLabel = false;
	takingLabelTimer = 0;

	labelTaken = false;
	labelTakenTimer = 0;

	printComplete = true;
	printCompleteTimer = 0;

	activeFunctionStart = true;
}

PrSession::~PrSession()
{
	deregisterAllEvents(msgQId);
}

int PrSession::activeFunction(void *pObject)
{
	return ((PrSession *) pObject)->activeFunctionTask();
}

int PrSession::activeFunctionTask()
{
	char recvBuffer[16 * 1024]; // CSM-120
	int msgSize = 0;

	while (!activeFunctionStart)
	{
		Debug::trace("PrSession", "PrSession Manager waiting to continue with active function ... ");
		taskDelay(10);
	}

	int i = 0;
	int sensorsFrequency = 25;

	while (1)
	{
//		cout << "PrSession::numMsgs = " << pThreadQ->numMsgs() << endl;

		testPrSession(i++);

		if ((printerOnOrOff == PrinterOn) && ((sensorsFrequency == 0) || ((i % sensorsFrequency) == 0)))
			sendPrinterSensors();

		if (pThreadQ->numMsgs() > 0)
		{
			msgSize = pThreadQ->receive(recvBuffer, sizeof(recvBuffer), WAIT_FOREVER);

			if (msgSize > 0)
			{
//				cout << "Message Arrived PrSession -- Size: " << msgSize << endl;
				EvolMessage evolMessage(recvBuffer);

				switch (evolMessage.getDeviceType())
				{
					case Printer		:	handlePrSessionEvent((PrEvent) evolMessage.getEventType(), recvBuffer);
											break;
					case PrinterSystem	:	handlePrinterSystemEvent((SysEvent) evolMessage.getEventType(), recvBuffer);
											break;
					case Renderer		:	handleRendererMessage((RdEvent) evolMessage.getEventType(), recvBuffer);
											break;
					case OperationMode	:	handleOperationModeMessage((OpModeEvent) evolMessage.getEventType(), recvBuffer);
											break;
					case ProductDev		:	handleProductEvent((ProductEvent) evolMessage.getEventType(), recvBuffer);
											break;
					default				:
											break;
				}
			}
			else
			{
				cout << "PrSession errno: " << errno << endl;
			}
		}

		// try to print the next label
		if ((pCurrentLabel != NULL) && 
			(postImaging == false) && (preImaging == false) &&
			(myRendererSessionId != 0) &&
			(productTransactionQ.empty() == false) &&
			(printComplete == true))
		{
			// look at the next productTransaction
			ProductTransaction *pNextProductTransaction = productTransactionQ.front();

			Product *pNextProduct = (Product *) pNextProductTransaction->getProduct();

			// Is it a new Product
			if (pNextProduct->getProductNum() == pCurrentProduct->getProductNum())
			{
				pCurrentProductTransaction = pNextProductTransaction;
				productTransactionQ.pop();

				// start postimaging the label
				postImaging = true;

				RendererRequestRendering rdReqRendering(this, myRendererSessionId);

				const char *pCC = rdReqRendering.serialize();
				int mLength = rdReqRendering.getSerializedLength();

				pMyRenderer->send(pCC, mLength);

				printComplete = false;
			}
			else
			{
				pCurrentProduct = pNextProduct;
				preImageLabel();
			}
		}

		taskDelay(1);
	}

	return 0;
}


void PrSession::handleProductEvent(ProductEvent prodEvent, char *recvBuffer)
{
	switch (prodEvent)
	{
		case ProductChange:
			{
				cout << "PrSession -- ProductChange!" << endl;
			}
			break;

		case ProductUpdate:
			{
				cout << "PrSession -- ProductUpdate!" << endl;
			}
			break;

		default:
			{
				cout << "PrSession -- Some Other ProductEvent: !" << (UINT16) prodEvent << endl;
			}
			break;
	}
}

void PrSession::handlePrSessionEvent(PrEvent prEvent, char *recvBuffer)
{
	switch (prEvent)
	{
		case PrWakeup:
			{
				cout << "PrSession -- PrWakeup!" << endl;
			}
			break;
		case PrConfig:
			{
				cout << "PrSession -- PrConfig!" << endl;

				PrinterConfiguration pc(recvBuffer);
				pc.setSourceObject(this);

				const char *buf = pc.serialize();
				UINT len = pc.getSerializedLength();
				Debug::trace("PrSession", HobString("received Printer config : ") + HobString(Hex::toString(buf, len)));
			}
			break;
		case PrInquire:
			{
				cout << "PrSession -- PrInquire! : THIS SHOULD NOT HAPPEN!!!" << endl;
			}
			break;
		case PrReset:
			{
				cout << "PrSession -- PrReset!" << endl;
			}
			break;
		case PrStatus:	// This will come from the peripheral only!`
			{
				cout << "PrSession -- PrStatus!" << endl;
			}
			break;
		case PrEnable:		
			{
				cout << "PrSession -- PrEnable!" << endl;
			}
			break;
		case PrPreImage:		
			{
				cout << "PrSession: " << (UINT16) myPrinterType << "  -- PrPreImage!" << endl;
				// empty out the transactionQ.
				while (productTransactionQ.empty() == false)
				{
					productTransactionQ.pop();
				}
								
				PrinterPreImage preImage(recvBuffer);
				pCurrentProduct = (Product *) preImage.getProduct();

				preImageLabel();
			}
			break;
		case PrPostImage:		
			{
				cout << "PrSession: " << (UINT16) myPrinterType << "  -- PrPostImage!" << endl;

				// Store ProductTransaction into productTA Q!!
				PrinterPostImage postImage(recvBuffer);
				productTransactionQ.push((ProductTransaction *) postImage.getProductTransaction());
			}
			break;
		case PrReqInquire:		
			{
				PrinterRequestInquire pri(recvBuffer);		//	Get the printerType
				PrinterType printerType = pri.getPrinterType();

				PrinterInquire pi(this);

				pi.setPrinterType(printerType);
				if(printerType == SecondPrinter)
				{
					pi.setLabelWidth(WideLabel);
					pi.setMediaSensorType(PrShootThrough);
					pi.setPrintPosition(32);
					pi.setConfigWord3(16);		//	Expel position
					pi.setPrintContrast(8);
				}
				else
				if(printerType == ThirdPrinter)
				{
					pi.setLabelWidth(NarrowLabel);
					pi.setMediaSensorType(PrShootThrough);
					pi.setPrintPosition(48);
					pi.setConfigWord3(32);		//	Expel position
					pi.setPrintContrast(57);
				}
				else
				{
					pi.setLabelWidth(UFWLabel);
					pi.setMediaSensorType(PrReflective);
					pi.setPrintPosition(16);
					pi.setConfigWord3(8);		//	Expel position
					pi.setPrintContrast(11);
				}


				const char *buf = pi.serialize();
				UINT len = pi.getSerializedLength();
				Debug::trace("PrSession", HobString("sending printer inquire: ") + HobString(Hex::toString(buf, len)));

				dispatchEvent(&pi, this);
			}
			break;
		case PrReqStatus:		
			{
				if (myPrinterType == FirstPrinter)
				{
					labelTakenTimer = 10;
				}
				else if (myPrinterType == SecondPrinter)
				{
					labelTakenTimer = 300;
				}
				cout << "PrSession -- PrReqStatus!" << endl;
			}
			break;
		case PrSensorsTracking:		
			{
				PrinterSensorsTracking prs(recvBuffer);		//	Get the printerType

				myPrinterType = prs.getPrinterType();
				printerOnOrOff = prs.getSensorsTracking();

				if(printerOnOrOff == PrinterOn)
				{
					initSensorsTracking(myPrinterType);
				}
				Debug::trace("PrSession", HobString("Turning Printer sensors: ") + ((printerOnOrOff) ? HobString("ON") : HobString("OFF")));
			}
			break;
			// These events shouldn't come into the Printer Manager!
		case PrFlush:  	cout << "PrSession -- PrFlush!" << endl;
			break;
		default			:
			{
				cout << "PrSession -- Some Other PrEvent: !" << (UINT16) prEvent << endl;
			}
			break;
	}
}

void PrSession::handlePrinterSystemEvent(SysEvent sysEvent, char *recvBuffer)
{
	switch (sysEvent)
	{
		case SysConfig:
		{
			PrinterSystemConfiguration psc(recvBuffer);
			psc.setSourceObject(this);

			const char *buf = psc.serialize();
			UINT len = psc.getSerializedLength();
			Debug::trace("PrSession", HobString("received Printer System configuration: ") + HobString(Hex::toString(buf, len)));
		}
		break;

		default:
		{
			cout << "PrinterSystem -- Some Other SysEvent: !" << (UINT16) sysEvent << endl;
		}
		break;
	}
}

void PrSession::handleRendererMessage(RdEvent rdEvent, char *recvBuffer)
{
	switch (rdEvent)
	{
		case RdSessionId:
		{
			cout << "PrSession: " << (UINT16) myPrinterType << " -- RdSessionId!" << endl;
			RendererSessionId rendererSessionId(recvBuffer);

			myRendererSessionId = rendererSessionId.getSessionId();

			RendererSetSource rdSetSource(this, myRendererSessionId, pSourceData);

			const char *pCC = rdSetSource.serialize();
			int mLength = rdSetSource.getSerializedLength();

			pMyRenderer->send(pCC, mLength);

			RendererSetDestination rdSetDestination(this, myRendererSessionId, pDestinationData);

			pCC = rdSetDestination.serialize();
			mLength = rdSetDestination.getSerializedLength();

			pMyRenderer->send(pCC, mLength);

		}
		break;
		case RdRenderStart:
		{
			cout << "PrSession: " << (UINT16) myPrinterType << " -- RdRenderStart!" << endl;
			// do nothing! Possibly, start a timer to see how long Rendering takes!!
		}
		break;
		case RdRenderComplete:
		{
			cout << "PrSession: " << (UINT16) myPrinterType << " -- RdRenderComplete!" << endl;

			// Possibly, stop the Rendering timer!!
			if (preImaging == true)
			{
				preImaging = false;
			}
			else if (postImaging == true)
			{
				postImaging = false;
				pCurrentProductTransaction = NULL;

				takingLabel = true;
				takingLabelTimer = 100; // then turn on taking!!

				labelTaken = true;

				// These go to the printer periferal.
				dispatchEvent(new PrinterFlush(this), this);
			}
		}
		break;
		default			:
		break;
	}
}

void PrSession::handleOperationModeMessage(OpModeEvent opModeEvent, char *recvBuffer)
{
	switch (opModeEvent)
	{
		case OpModeIdle:
		{
			pCurrentLabel = NULL;
			cout << "PrSession -- OpModeIdle!" << endl;
		}
		break;
		case OpModeOperate:
		{
			cout << "PrSession -- OpModeOperate!" << endl;
		}
		break;
		case OpModePluEntry:
		{
			cout << "PrSession -- OpModePluEntry!" << endl;
		}
		break;
		case OpModeAutoWeigh:
		{
			pCurrentLabel = NULL;
			cout << "PrSession -- OpModeAutoWeigh!" << endl;
		}
		break;
		case OpModeSemiWeigh:
		{
			pCurrentLabel = NULL;
			cout << "PrSession -- OpModeSemiWeigh!" << endl;
		}
		break;
		case OpModeManualWeigh:
		{
			pCurrentLabel = NULL;
			cout << "PrSession -- OpModeManualWeigh!" << endl;
		}
		break;
		case OpModeSupervisor:
		{
			cout << "PrSession -- OpModeSupervisor!" << endl;
		}
		break;
		case OpModeService:
		{
			cout << "PrSession -- OpModeService!" << endl;
		}
		break;
		case OpModePowerDown:
		{
			cout << "PrSession -- OpModePowerDown!" << endl;
		}
		break;
		case OpModeMonitor:
		{
			cout << "PrSession -- OpModeMonitor!" << endl;
		}
		break;
		default:
		break;
	}					
}

void PrSession::preImageLabel()
{
	if (pCurrentProduct != NULL)
	{
		if (myPrinterType == FirstPrinter)
		{
//			pCurrentLabel = (LabelType *) pCurrentProduct->getLabelType1();
			pCurrentLabel = (LabelType *) pCurrentProduct->getLabelType(FirstPrinter);
		}
		else if (myPrinterType == SecondPrinter)
		{
//			pCurrentLabel = (LabelType *) pCurrentProduct->getLabelType2();
			pCurrentLabel = (LabelType *) pCurrentProduct->getLabelType(SecondPrinter);
		}
		else
		{
			pCurrentLabel = NULL;
		}

		if (pCurrentLabel != NULL)
		{
			RendererRequestRendering rdReqRendering(this, myRendererSessionId);

			const char *pCC = rdReqRendering.serialize();
			int mLength = rdReqRendering.getSerializedLength();

			pMyRenderer->send(pCC, mLength);
		}
	}
}

void PrSession::testPrSession(int i)
{
/*
	static bool quit = false;

	if ((i % 0x100) == 0)
	{
		cout << "i = " << i << " quit = " << (int) quit << endl;
	}

	if (((i % 0x300) == 0) && (quit == false))
	{
		cout << "PrinterManager -- ReqInquire!" << endl;
		PrinterRequestInquire pri(this);

		const char *pCC = pri.serialize();
		pMyPrinter->send(pCC, pri.getSerializedLength());
	}

	if (((i % 0x400) == 0) && (quit == false))
	{
		cout << "PrinterManager -- SysReqInquire!" << endl;
		PrinterSystemRequestInquire psri(this);

		const char *pCC = psri.serialize();
		pMyPrinter->send(pCC, psri.getSerializedLength());
	}

	if ((i >= 3000) && (i % 0x3000) == 0)
	{
		quit = true;
		PrinterRequestLabelTransfer printerRequestLabelTransfer(this);

		const char *pCC = printerRequestLabelTransfer.serialize();
		int mLength = printerRequestLabelTransfer.getSerializedLength();

		pMyPrinter->send(pCC, mLength);

	}

	if ((i >= 3000) && (i % 0x4000) == 0)
	{
		// tell printer to size its label!!

		runCommand(PrintCommand, PrNoSecurityLabel, PrNoIndirection, 0x0140);
	}

	if ((i >= 3000) && (i % 0x4300) == 0)
	{
		// tell printer to size its label!!

		runCommand(EjectCommand, PrNoSecurityLabel, PrNoIndirection, 0x1000);
	}
*/
}

/*
void PrSession::testPrSession(int i)
{
	if (takingLabel == true)
	{
		takingLabelTimer--;

		if (takingLabelTimer == 0)
		{
			PrinterStatus prStatus(this);

			prStatus.setLabelReady();
			prStatus.clearLabelTaken();

			dispatchEvent(&prStatus, this);

			takingLabel = false;
		}
	}

	else if (labelTaken == true)
	{
		labelTakenTimer--;

		if (labelTakenTimer == 0)
		{
			PrinterStatus prStatus(this);

			prStatus.clearLabelReady();
			prStatus.setLabelTaken();
			
			dispatchEvent(&prStatus, this);

			labelTaken = false;

			printCompleteTimer = 5;
		}
	}

	else if (printComplete == false)
	{
		printCompleteTimer--;

		if (printCompleteTimer == 0)
		{
			PrinterStatus prStatus(this);

			prStatus.clearLabelReady();
			prStatus.setLabelTaken();
			prStatus.setCommandComplete();
			
			dispatchEvent(&prStatus, this);

			printComplete = true;
		}
	}
}
*/

void PrSession::initSensorsTracking(PrinterType printerType)
{
	index = 0;
}

void PrSession::sendPrinterSensors()
{
	PrinterHeadUpType headup = PrinterHeadEngaged;
	LabelWidth labelWidth = WideLabel;
	UINT16 regulatorTemperature = 259;
	UINT16 headTemperature = 183;
	UINT16 headVoltage = 122;
	UINT16 headCurrent = 12;
	UINT16 labelHighAverage = 150;
	UINT16 labelLowAverage = 34;
	UINT16 labelThreshold = 57;
	UINT16 label = 2;
	UINT16 mediaHighAverage = 1500;
	UINT16 mediaLowAverage = 340;
	UINT16 mediaThreshold = 570;
	UINT16 media = 20;

	if (myPrinterType == SecondPrinter)
	{
		labelWidth = NarrowLabel;
		headTemperature = 152;
	}
	else if (myPrinterType == ThirdPrinter)
	{
		labelWidth = UFWLabel;
		headTemperature = 201;
	}

	PrinterSensors ps(this,	myPrinterType, headup, labelWidth, (regulatorTemperature + index),
				   			(headTemperature + index), (headVoltage + index), (headCurrent + index),
				   			(labelHighAverage + index), (labelLowAverage + index), (labelThreshold + index), (label + index),
				   			(mediaHighAverage + index), (mediaLowAverage + index), (mediaThreshold + index), (media + index));
	index += 1;

	const char *buf = ps.serialize();
	UINT len = ps.getSerializedLength();
	Debug::trace("PrSession", HobString("sending printer sensors: ") + HobString(Hex::toString(buf, len)));

	dispatchEvent(&ps, this);
}
