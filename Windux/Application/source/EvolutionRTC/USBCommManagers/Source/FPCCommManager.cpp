#include <PAL_platformheader.h>

#include "RealTimeController.h"
#include "UsbManager.h"
#include "SessionManager.h"
#include "PrinterManager.h"
#include "FPCCommManager.h"
#include "USBEvolHeader.h"

#include "Configuration.h"

#include "Printer.h"
#include "PrinterWakeup.h"
#include "PrinterConfiguration.h"
#include "PrinterFlush.h"
#include "PrinterInquire.h"
#include "PrinterStatus.h"
#include "PrinterSensors.h"
#include "PrinterMode.h"
#include "PrinterRequestPrintheadPower.h"
#include "PrinterCommand.h"
#include "PrinterNomSize.h"
#include "PrinterRam.h"
#include "PrinterTest.h"
#include "PrinterMask.h"
#include "PrinterTeach.h"
#include "PrinterRequestCalibrate.h"
#include "PrinterRequestVirtualLabel.h"
#include "PrinterRequestLabelTransfer.h"
#include "PrinterRequestWakeup.h"
#include "PrinterLabelTransferComplete.h"
#include "PrinterLabelTransferFailed.h"
#include "PrinterNewPrinterType.h"
#include "CommandOperationList.h"
#include "PrinterFlashStatus.h"

#include "PrinterSystemWakeup.h"
#include "SystemStatusInfo.h"
#include "PrinterSystemStatus.h"
#include "PrinterSystemConfiguration.h"
#include "PrinterSystemInquire.h"
#include "PrinterSystemCalibrationSwitch.h"

#include "PeripheralRequestCodeUpdate.h"

#include "WeigherCommManager.h"
#include "USBEvolHeader.h"

#include "Weigher.h"
#include "WeigherWakeup.h"
#include "WeigherStatus.h"
#include "WeigherInquire.h"
#include "WeigherConfiguration.h"
#include "WeigherMode.h"
#include "WrapperOpen.h"
#include "WeigherControl.h"
#include "WeigherRequestWakeup.h"
#include "WeigherManager.h"

#include "WeigherSystemWakeup.h"
#include "UsbWeigherSystemStatus.h"
#include "WeigherSystemConfiguration.h"
#include "WeigherSystemInquire.h"

#include "SystemReset.h"
#include "SystemConfigureGCD.h"
#include "SystemGCDPowerOn.h"
#include "SystemGCDPowerOff.h"

#include "UsbAddComponent.h"
#include "UsbRemoveComponent.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "UInteger.h"
#include "Bool.h"
#include "Hex.h"

#include "MsgTime.h"


#define SWAP_BYTES(x)        (MSB(x) | LSB(x) << 8)  

// PTR_508: Head Temp calculation moved from PrinterSensors to its own Comm manager.
const UINT16 FPCCommManager::phTempsC[81] = {232,231,230,229,227,226,225,223,222,220,219,217,216,214,212,211,
											209,207,205,203,202,200,198,195,193,191,189,187,185,183,180,178,
											176,173,171,169,166,164,161,160,156,154,152,149,147,144,142,140,
											137,134,132,130,127,125,122,120,118,115,113,111,109,106,104,102,
											100,98,96,94,92,90,88,86,84,82,80,79,77,75,73,72,70};


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int FPCCommManager::dataType = 0;
std::string FPCCommManager::dataTypeName("FPCCommManager");
#endif

FPCCommManager *FPCCommManager::pThis = NULL;

FPCCommManager::FPCCommManager(USBD_NODE_ID node_id, SUsbDescriptors * descs ) : CUsbCommBase( descs, node_id ), nodeId_( node_id )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("FPCCommManager", "FPCCommManager");
	FPCCommManager::pThis = this;
	
	this->prKind_ = ApcPrinter;
	this->myPrinterType_ = FirstPrinter;


	pConfig_ = Configuration::getInstance();

	pMyPrinterMgr_ = NULL;
	pMyWeigher_ = NULL;

	printerRequestedStatus_ = false;
	weigherRequestedStatus_ = false;

	weigherRequestedInquire_ = false;
	printerRequestedInquire_ = false;


	pBulkData_ = NULL;
	sendingBulkData_ = false;
	bulkDataLength_ = 0;


	registerEvent(System, SysReset);

	createPipes();
	startPipes();

	getHardwareRevision();

	UsbAddComponent addComponent(nodeId_, hardwareVersion_);
	dispatchEvent(&addComponent);

#ifdef SEND_INITIAL_WAKEUP_REQUEST_TO_APC
    sendInitialWakeupRequests();
#endif    

#ifdef FAKE_SOCKET_WEIGHER
	agentAttached_ = false;
	FPCCommManagerThread_ = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "FPCCommManager Thread" , false, false, false);
	if (FPCCommManagerThread_)
		FPCCommManagerThread_->RestartThread();
#endif

}

FPCCommManager::~FPCCommManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
#ifdef FAKE_SOCKET_WEIGHER
	if (FPCCommManagerThread_)
	{
		FPCCommManagerThread_->StopThread();
		FPCCommManagerThread_->WaitForCompletion();
	}
#endif

	Debug::trace("FPCCommManager", "~FPCCommManager");

	UsbRemoveComponent removeMe(nodeId_, this);
	dispatchEvent(&removeMe);

	ShutdownNicely();

	deregisterAllEvents();
   
	if (pMyPrinterMgr_)
	{
		delete pMyPrinterMgr_;
		pMyPrinterMgr_ = NULL;
	}

	if (pMyWeigher_)
	{
		delete pMyWeigher_;
		pMyWeigher_ = NULL;
	}


}

void FPCCommManager::SysCfgGCD()
{
	SystemConfigureGCD sysCfgGCD;
	DWORD result = OK;
	bool is_custom_code = false;
	sendUsbInterruptMessage( ( const char * ) sysCfgGCD.serialize(), sysCfgGCD.getMessageLength(), result, is_custom_code );
}

void FPCCommManager::SysGCDPwrOff()
{
	SystemGCDPowerOff sysGCDPowerOff;
	DWORD result = OK;
	bool is_custom_code = false;
	sendUsbInterruptMessage( ( const char * ) sysGCDPowerOff.serialize(), sysGCDPowerOff.getMessageLength(), result, is_custom_code );
}

void FPCCommManager::SysGCDPwrOn()
{
	SystemGCDPowerOn sysGCDPowerOn;
	DWORD result = OK;
	bool is_custom_code = false;
	sendUsbInterruptMessage( ( const char * ) sysGCDPowerOn.serialize(), sysGCDPowerOn.getMessageLength(), result, is_custom_code );
}

PrinterType FPCCommManager::getPrinterType()
{
	return myPrinterType_;
}

USBD_NODE_ID FPCCommManager::getNodeID()
{
	return nodeId_;
}

void FPCCommManager::getHardwareRevision()
{
//TO DO:
	hardwareVersion_.setVersionType(HobVersion::ApcPrinterHW);
	hardwareVersion_.setMajorRevision(1);
	hardwareVersion_.setMinorRevision(0);

}

#ifdef FAKE_SOCKET_WEIGHER


DWORD FPCCommManager::Run()
{
	Debug::trace("FPCCommManager.Socket", HobString("starting run thread. ") );

	clientSocket_ = 0;

	serverSocket_.setPort(8000);//6001);
	/* bind socket to local address */
	serverSocket_.sBind();
	/* create queue for client connection requests */
	serverSocket_.sListen();

	serverSocket_.setNonBlocking();

	while (!m_ExitEvent.IsEventSet())
	{

		{
			Util::CLockCriticalSection lock( fakeWeigherCS_);

			clientSocket_ = serverSocket_.sAccept();
		}

		if (clientSocket_)
		{
			int bytesRead = 0;
			Debug::trace("FPCCommManager.Socket", HobString("start - Server accepted new client connection on socket: ") + HobString(clientSocket_->toString()));
			string msg("");
			clientSocket_->setNonBlockingIO();
			bool socketAlive = true;

			agentAttached_ = true;

			{
				PrinterGeneric pg(PrAgentAttached);

				const char *pPsMsg = pg.serialize();
				int mLength = pg.getSerializedLength();

				pMyPrinterMgr_->send(pPsMsg, mLength);
			}

			while (!m_ExitEvent.IsEventSet() && socketAlive)
			{
				Util::CSmartPtr<USBEvolHeader> header;

				msg.clear();

				int result = readAMessageFromSocket(clientSocket_, msg, header);

				if (result < 0)
					socketAlive = false;
				else if (result > 0)
					handleASocketMessage(msg, header);


				}

			Debug::trace("FPCCommManager.Socket", HobString("socket closing. socket alive = ") + Bool::toString(socketAlive) + " kill event = " +Bool::toString(m_ExitEvent.IsEventSet()));

			agentAttached_ = false;

				{
				PrinterGeneric pg(PrAgentDetached);

				const char *pPsMsg = pg.serialize();
				int mLength = pg.getSerializedLength();

				pMyPrinterMgr_->send(pPsMsg, mLength);
			}

			{
				Util::CLockCriticalSection lock( fakeWeigherCS_);

				delete clientSocket_;
				clientSocket_ = 0;

			}

					}
					else
			WaitForSingleObject(m_ExitEvent, 1000);

	}

	return 0;
}

void FPCCommManager::handleASocketMessage(const string & msg, Util::CSmartPtr<USBEvolHeader> & header)
					{

	UINT16 logicalAddress = header->getSourceLogicalAddress();

	const char * ptr = msg.c_str() + 10;

	switch (logicalAddress)
	{
		case USBEvolHeader::WEIGHER:
		{
			Debug::trace("FPCCommManager.Socket", HobString("Handling weigher message from Socket") );
			//right now only handlling weigher messages from agent.

			UltimaWgStatus *statusMsg = (UltimaWgStatus *) ptr;

			WeigherStatus ws(this); // use default constructor
			ws.setStatusBits(statusMsg->status);
			ws.setCounts(statusMsg->counts);
			ws.setRawCounts(statusMsg->raw_counts);

			const char *pWsMsg = ws.serialize();
			int mLength = ws.getSerializedLength();

			if (pMyWeigher_ != NULL)
				pMyWeigher_->send(pWsMsg, mLength);
		}
		break;
		case USBEvolHeader::EVOL_PRINTER:
		{

			Debug::trace("FPCCommManager.Socket", HobString("Handling A Printer message from Socket") );

			PrinterGeneric pg(PrVirtualLabelComplete);

			const char *pPsMsg = pg.serialize();
			int mLength = pg.getSerializedLength();

			pMyPrinterMgr_->send(pPsMsg, mLength);
		}
		break;
	}
					
}

int FPCCommManager::readAMessageFromSocket(Socket *clientSocket, string & msg, Util::CSmartPtr<USBEvolHeader> & header)
{
	int result = 0, bytesRead = 0;

	bytesRead = readBlockFromSocket( clientSocket , msg , USBEvolHeader::SIZE, 0);
	if (bytesRead < 0)
	{
		Debug::trace("FPCCommManager.Socket", HobString("Problem reading header") );
		result = bytesRead;
	}
	else if (bytesRead == USBEvolHeader::SIZE)
	{
		header = new USBEvolHeader ((const char *)msg.c_str(), false);

		int bytesInBody = header->getMessageLength();

		UINT16 physicalAddress = header->getSourcePhysicalAddress();
		UINT16 logicalAddress = header->getSourceLogicalAddress();

		Debug::trace("FPCCommManager.Socket", HobString("bytes in body: ") + Integer::toString(bytesInBody));
		Debug::trace("FPCCommManager.Socket", HobString("source physical address: ") + Integer::toString(physicalAddress));
		Debug::trace("FPCCommManager.Socket", HobString("source logical address: ") + Integer::toString(logicalAddress));

		switch (logicalAddress)
		{
			case USBEvolHeader::EVOL_PRINTER:
			case USBEvolHeader::WEIGHER:
			{

				if (bytesInBody < 1024)// limit to 1k. just incase we have a garbage header.
				{

					bytesRead = readBlockFromSocket( clientSocket_ , msg , bytesInBody, 0);
					if (bytesRead < 0)
					{
						Debug::trace("FPCCommManager.Socket", HobString("Problem reading body") );
						result = bytesRead;
					}
					else if (bytesRead == bytesInBody)
					{
						Debug::trace("FPCCommManager.Socket", "Received a complete message");
						result = 1;
					}
				}

			}
		}
	}
	return result;

}


int FPCCommManager::readBlockFromSocket(Socket *clientSocket, string & msg , int bytesToRead, UINT32 timeOutMS)
{
	int sleepTimeMS = 10;
	int totalBytesRead = 0;
	char buffer[1024];
	int maxBytes = sizeof(buffer);
	bool timeOutOccured = false;

	Util::CMSGTime startTime = Util::CMSGTime::GetTheCurrentTime();

	while (!m_ExitEvent.IsEventSet() && (totalBytesRead < bytesToRead) && !timeOutOccured)
	{
		int err = 0, bytesRead = 0, bytesRequested = (bytesToRead - totalBytesRead);

		if (bytesRequested > maxBytes)
			bytesRequested = maxBytes;

		bytesRead = clientSocket->receive((char *)buffer, bytesRequested, false);
		err = WSAGetLastError();

		if ((bytesRead == ERROR_CODE) && (err != WSAEWOULDBLOCK) )
		{
			
			Debug::trace("FPCCommManager.Socket", HobString("socket error. stop receiving data err:") + Integer::toString(err));
			return -1;
		}
		else if (bytesRead == 0)
		{

			Debug::trace("FPCCommManager.Socket", HobString("Connection gracefully closed"));
			return -1;

		}
		else if (bytesRead > 0)
		{
			msg.append(buffer, bytesRead);
			totalBytesRead += bytesRead;

			Debug::trace("FPCCommManager.Socket", HobString("received bytes: ") + Integer::toString(bytesRead));

		}

		if (totalBytesRead < bytesToRead)
		{
			Sleep(sleepTimeMS);

			if (timeOutMS > 0)
			{
				Util::CMSGTime ct = Util::CMSGTime::GetTheCurrentTime();
				unsigned __int64 elapse_time = ( ct - startTime ).GetSpanInMilliseconds();

				if (elapse_time > timeOutMS)
				{
					Debug::trace("FPCCommManager.Socket", HobString("Timed out on socket!"));
					timeOutOccured = true;
				}
			}

		}

	}

	return totalBytesRead;
}
#endif

void FPCCommManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{

	try
	{

		switch (msg->getDeviceType())
		{
			case System				:	handleSysEvent((SysEvent) msg->getEventType(), msg->Data() );
										break;
			case Printer			:	handlePrinterEvent((PrEvent) msg->getEventType(), msg->Data());
										break;
			case PrinterSystem		:	handlePrinterSysEvent((SysEvent) msg->getEventType(), msg->Data());
							 			break;
			//copy from WeigherCommmanager
			case Weigher			:	handleWeigherEvent( (WgEvent) msg->getEventType(), msg->Data() );
										break;
			case WeigherSystem		:	handleWeigherSysEvent( (SysEvent) msg->getEventType(), msg->Data() );
										break;

			default					:
										break;
		}

	}
	catch (...)
	{
		Debug::trace("FPCCommManager.Message.Out", "FPCCommManager::processEvent. caught exception");
	}

}


bool FPCCommManager::handlePrinterEvent(PrEvent prEventType, const char *buffer)
{
	char str[300];
	DWORD result = OK;
	bool is_custom_code = false;

	switch (prEventType)
	{
		case PrConfig:
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrConfig");

								PrinterConfiguration prConfig(buffer);
								PrConfigMsg prConfigMsg;

								prConfigMsg.prEvent = PrConfig;
								prConfigMsg.prConfigType = (UINT16)((PrConfigType) htons((u_short)prConfig.getConfigType()));

								prConfig.getDeviceSignature((UINT8 *) &prConfigMsg.prConfiguration.deviceSignature);
								prConfigMsg.prConfiguration.printerType = (UINT8) prConfig.getPrinterType();

								prConfigMsg.prConfiguration.labelWidth = (UINT8) prConfig.getLabelWidth();
								prConfigMsg.prConfiguration.mediaSensorAdjustment = prConfig.getMediaSensorAdjustment();
								prConfigMsg.prConfiguration.outOfMediaCount = prConfig.getOutOfMediaCount();
								prConfigMsg.prConfiguration.printContrast = htons(prConfig.getPrintContrast());
								prConfigMsg.prConfiguration.configWord0 = htons(prConfig.getConfigWord0());			// Used on "European" labels
								prConfigMsg.prConfiguration.configWord1 = htons(prConfig.getConfigWord1());			// Used on "European" labels
								prConfigMsg.prConfiguration.configWord2 = htons(prConfig.getConfigWord2());		// Used on "European" labels
								prConfigMsg.prConfiguration.configWord3 = htons(prConfig.getConfigWord3());	// Used on "European" labels
								prConfigMsg.prConfiguration.configWord4 = htons(prConfig.getConfigWord4());
								prConfigMsg.prConfiguration.configWord5 = htons(prConfig.getConfigWord5());
								prConfigMsg.prConfiguration.configWord6 = htons(prConfig.getConfigWord6());
								prConfigMsg.prConfiguration.configWord7 = htons(prConfig.getConfigWord7());
								prConfigMsg.prConfiguration.configWord8 = htons(prConfig.getConfigWord8());
								prConfigMsg.prConfiguration.configWord9 = htons(prConfig.getConfigWord9());
								prConfigMsg.prConfiguration.configWord10 = htons(prConfig.getConfigWord10());
								prConfigMsg.prConfiguration.configWord11 = htons(prConfig.getConfigWord11());
								prConfigMsg.prConfiguration.configWord12 = htons(prConfig.getConfigWord12());
								prConfigMsg.prConfiguration.configWord13 = htons(prConfig.getConfigWord13());
								prConfigMsg.prConfiguration.mediaSensorType = (UINT16)((MediaSensorType) htons((u_short)prConfig.getMediaSensorType()));
								prConfigMsg.prConfiguration.slaEjectLine = htons(prConfig.getSLAEjectLine());
								prConfigMsg.prConfiguration.printPosition = htons(prConfig.getPrintPositionAbsolute());

								unsigned short msgLength = sizeof(prConfigMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &prConfigMsg, sizeof(prConfigMsg));

								sendUsbInterruptMessage( ( const char * ) str,  sizeof(prConfigMsg) + 3, result, is_custom_code );
								
							}
							break;
		case PrFlush:
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrFlush");

								PrGenericMsg genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.prEvent = PrFlush;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
								
							}
							break;
		case PrReset:		
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrReset");

								PrGenericMsg genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.prEvent = PrReset;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
								
							}
							break;
		case PrEnable:		
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrEnable");

								PrGenericMsg genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.prEvent = PrEnable;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
							
							}
							break;
		case PrDisable:	
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrDisable");

								PrGenericMsg genMsg;

								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.prEvent = PrDisable;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
								
							}
							break;
		case PrMode:
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrMode");

								// Constuct the Evol Message Buffer!!
								PrinterMode prMode(buffer);
								PrModeMsg prModeMsg;

								prModeMsg.prEvent = PrMode;
								prModeMsg.mode = (UINT8)prMode.getPrinterMode();

								unsigned short msgLength = sizeof(prModeMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &prModeMsg, sizeof(prModeMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(prModeMsg) + 3, result, is_custom_code );
								
							}
							break;
		case PrReqPrintheadPower:	
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrReqPrintheadPower");
		case PrPrintheadPower:	
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrPrintheadPower");

								if (prEventType == PrReqPrintheadPower)
								{
									Debug::trace("FPCCommManager.Printer.Message.Out", "PrinterCommManager -- PrReqPrintheadPower");
								}
								else
								{
									Debug::trace("FPCCommManager.Printer.Message.Out", "PrinterCommManager -- PrPrintheadPower");
								}

								// Constuct the Evol Message Buffer!!
								PrinterRequestPrintheadPower powerMsg(buffer);
								PrPowerMsg prPowerMsg;

								prPowerMsg.prEvent = PrPrintheadPower;
								prPowerMsg.powerState = htons((u_short)powerMsg.getPowerState());
								prPowerMsg.scProtection = htons((u_short)powerMsg.getShortCircuitProtection());

								unsigned short msgLength = sizeof(prPowerMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &prPowerMsg, sizeof(prPowerMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(prPowerMsg) + 3, result, is_custom_code );

								Debug::trace("FPCCommManager.Printer.Message.Out", HobString("prPowerMsg.powerState: ") +
																			  HobString(Integer::toString(prPowerMsg.powerState)));
								Debug::trace("FPCCommManager.Printer.Message.Out", HobString("prPowerMsg.scProtection: ") +
																			  HobString(Integer::toString(prPowerMsg.scProtection)));

								
							}
							break;
		case PrCommand:	
						{
							Debug::trace("FPCCommManager.Printer.Message.Out", "PrCommand");

							// Constuct the Evol Message Buffer!!
							PrinterCommand printerCommand(buffer);
							PrCommandMsg prCommandMsg;

							prCommandMsg.prEvent = PrCommand;
							prCommandMsg.printerCommandType = (UINT8) printerCommand.getPrinterCommand();
							prCommandMsg.printerCommandOptions = (UINT8) printerCommand.getPrinterCommandOptions();
							prCommandMsg.printerIndirection = (UINT16)((PrinterIndirection) htons((u_short)printerCommand.getPrinterIndirection()));
							prCommandMsg.value = htons(printerCommand.getValue());

							unsigned short msgLength = sizeof(prCommandMsg);
							str[0] = USBEvolHeader::EVOL_PRINTER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &prCommandMsg, sizeof(prCommandMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(prCommandMsg) + 3, result, is_custom_code );
							
						}
						break;
		case PrRam:		
						{
							Debug::trace("FPCCommManager.Printer.Message.Out", "PrRam");

							// Constuct the Evol Message Buffer!!
							PrinterRam printerRam(buffer);
							PrRamMsg prRamMsg;

							prRamMsg.prEvent = PrRam;

							for (int i = 0; i < PrNumRamLocations; i++)
							{
								prRamMsg.ram[i].location = (UINT16) htons((u_short)printerRam.getRamIndirection((UINT16)i));
								prRamMsg.ram[i].value = htons(printerRam.getRamValue((UINT16)i));
							}

							unsigned short msgLength = sizeof(prRamMsg);
							str[0] = USBEvolHeader::EVOL_PRINTER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &prRamMsg, sizeof(prRamMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(prRamMsg) + 3, result, is_custom_code );
						
						}
						break;
		case PrTest:
						{
							Debug::trace("FPCCommManager.Printer.Message.Out", "PrTest");

							// Constuct the Evol Message Buffer!!
							PrinterTest printerTest(buffer);
							PrTestMsg prTestMsg;

							prTestMsg.prEvent = PrTest;
							prTestMsg.data = printerTest.getData();

							unsigned short msgLength = sizeof(prTestMsg);
							str[0] = USBEvolHeader::EVOL_PRINTER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &prTestMsg, sizeof(prTestMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(prTestMsg) + 3, result, is_custom_code );
					
						}
						break;
		case PrNomSize:
						{
							Debug::trace("FPCCommManager.Printer.Message.Out", "PrNomSize");

							// Constuct the Evol Message Buffer!!
							PrinterNomSize printerSize(buffer);
							PrNomSizeMsg prNomSizeMsg;

							prNomSizeMsg.prEvent = PrNomSize;
							prNomSizeMsg.actualSize = htons(printerSize.getActualSize());
							prNomSizeMsg.measuredSize = htons(printerSize.getMeasuredSize());

							unsigned short msgLength = sizeof(prNomSizeMsg);
							str[0] = USBEvolHeader::EVOL_PRINTER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &prNomSizeMsg, sizeof(prNomSizeMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(prNomSizeMsg) + 3, result, is_custom_code );
						
						}
						break;
		case PrMask:
						{
							Debug::trace("FPCCommManager.Printer.Message.Out", "PrMask");

							// Constuct the Evol Message Buffer!!
							PrinterMask printerMask(buffer);
							PrMaskMsg prMaskMsg;

							prMaskMsg.prEvent = PrMask;
							prMaskMsg.maskInfo.sensorMask = printerMask.getSensorMask();
							prMaskMsg.maskInfo.sensor2Mask = printerMask.getSensor2Mask();
							prMaskMsg.maskInfo.userMask = printerMask.getUserMask();

							unsigned short msgLength = sizeof(prMaskMsg);
							str[0] = USBEvolHeader::EVOL_PRINTER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &prMaskMsg, sizeof(prMaskMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(prMaskMsg) + 3, result, is_custom_code );
							
						}
						break;
		case PrTeach:	
						{
							Debug::trace("FPCCommManager.Printer.Message.Out", "PrTeach");

							// Constuct 							the Evol Message Buffer!!
							PrinterTeach printerTeach(buffer);
							PrTeachMsg prTeachMsg;

							CommandOperationList *pCOL = printerTeach.getCommandOperationList();

							memset((UINT8 *) &prTeachMsg.operation, (UINT8) 0x00, sizeof(prTeachMsg.operation));

							prTeachMsg.prEvent = PrTeach;
							prTeachMsg.command = (UINT8)printerTeach.getPrinterCommandType();

							prTeachMsg.entries = htons(pCOL->getCommandBlock((CommandOperationMsg *) &(prTeachMsg.operation), true));

							unsigned short msgLength = sizeof(prTeachMsg);
							str[0] = USBEvolHeader::EVOL_PRINTER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &prTeachMsg, sizeof(prTeachMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(prTeachMsg) + 3, result, is_custom_code );
							
						}
						break;
		case PrReqStatus:	
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrReqStatus");

								PrGenericMsg genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.prEvent = PrStatus;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
								
							}
							break;
		case PrReqInquire:	
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrReqInquire");

								PrGenericMsg genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.prEvent = PrInquire;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
								
							}
							break;
		case PrReqWakeup:
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrReqWakeup");

								PrinterRequestWakeup prReqWakeup(buffer);
				
								PrGenericMsg genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.prEvent = PrWakeup;

								if (pMyPrinterMgr_ == NULL)
								{
									usbEvolHdr_.setDestinationPhysicalAddress(prReqWakeup.getDefaultHardwareAddress());
								}

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
								
							}
							break;
		case PrEnableTakeup:
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrEnableTakeup");

								PrGenericMsg genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.prEvent = PrEnableTakeup;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
							
							}
							break;
		case PrReqSensors:
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrReqSensors");

								PrGenericMsg genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.prEvent = PrSensors;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
								
							}
							break;
		case PrReqCalibrate:
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrReqCalibrate");

								// Constuct the Evol Message Buffer!!
								PrinterRequestCalibrate printerRequestCalibrate(buffer);

								PrCalibrateMsg prCalibrateMsg;

								prCalibrateMsg.prEvent = PrCalibrate;
								prCalibrateMsg.printerCalibrateType = (UINT8) printerRequestCalibrate.getPrinterCalibrateType();

								unsigned short msgLength = sizeof(prCalibrateMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &prCalibrateMsg, sizeof(prCalibrateMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(prCalibrateMsg) + 3, result, is_custom_code );
													
							}
							break;
		case PrReqLabelTransfer:	
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrReqLabelTransfer");

								PrinterRequestLabelTransfer printerRequestLabelTransfer(buffer);

								UINT16 *pData = (UINT16 *) printerRequestLabelTransfer.getDataPointer();
								UINT16 totalLength = printerRequestLabelTransfer.getTotalDataLength();
								UINT16 actualLength = printerRequestLabelTransfer.getActualDataLength();
								bool sendSetupXferMessage = printerRequestLabelTransfer.isSendSetupXferMessage();
								bool cancelPendingTransfer = printerRequestLabelTransfer.isCancelPendingTransfer();

								if (cancelPendingTransfer == true)
								{
									Debug::trace("FPCCommManager.Printer.Message.Out", "aborting bulk data transfer at this time");
									cancelBulkTransfer();									
								}
								else
								{
									if (sendingBulkData_ == true)
									{
										Debug::trace("FPCCommManager.Printer.Message.Out", "cannot send bulk data at this time");
										Debug::trace("FPCCommManager.Printer.Message.Out", HobString("pBulkData: 0x") +
																				HobString(Hex::toString((long long) (pBulkData_))));
										Debug::trace("FPCCommManager.Printer.Message.Out", HobString("bulkDataLength: ") +
																				HobString(UInteger::toString(bulkDataLength_)));
										if (pMyPrinterMgr_ != NULL)
										{
											PrinterLabelTransferFailed ltFailed(this, myPrinterType_, totalLength, actualLength, (UINT16 *) pData);

											const char *pltFailed = ltFailed.serialize();
											int mLength = ltFailed.getSerializedLength();

											pMyPrinterMgr_->send(pltFailed, mLength);
										}
									}
									else
									{
										if (sendSetupXferMessage == true)
										{
											PrReqLabelTransferMsg requestMessage;
											memset((char *) &requestMessage, 0, sizeof(requestMessage));

											requestMessage.prEvent = (PrEvent) 0x02; // COMM_SETUPBULKOUTTRANSFER
											requestMessage.bufferSize = htons(totalLength);

											unsigned short msgLength = sizeof(requestMessage);
											str[0] = USBEvolHeader::EVOL_PRINTER;
											memcpy(&str[1], &msgLength, sizeof(msgLength));

											memcpy( &str[3], (char *) &requestMessage, sizeof(requestMessage));
											sendUsbInterruptMessage( ( const char * ) str,  sizeof(requestMessage) + 3, result, is_custom_code );
											
										}


										// Set up Label Data Transfer
										pBulkData_ = pData;
										bulkDataLength_ = totalLength;

										Debug::trace("FPCCommManager.Printer.Message.Out", HobString("pBulkData: 0x") +
																				HobString(Hex::toString((long long) (pBulkData_))));
										Debug::trace("FPCCommManager.Printer.Message.Out", HobString("bulkDataLength:") +
																				HobString(UInteger::toString(bulkDataLength_)));

										sendUsbBulkMessage( ( const char * ) pData, totalLength );


									}
								}
							}
							break;
#ifdef FAKE_SOCKET_WEIGHER
		case PrReqVirtualLabel:	
							{
								char str[255];
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrReqVirtualLabel");

								PrinterRequestVirtualLabel printerRequestVirtualLabel(buffer);

								PrCommandMsg prCommandMsg;

								prCommandMsg.prEvent = PrCommand;
								prCommandMsg.printerCommandType = PrintCommand;
								prCommandMsg.printerCommandOptions = PrNoSecurityLabel;
								prCommandMsg.printerIndirection = (PrinterIndirection) htons(PrRam0);
								prCommandMsg.value = htons(printerRequestVirtualLabel.getLinesToPrint());

								usbEvolHdr_.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
								usbEvolHdr_.setMessageLength(sizeof(prCommandMsg));
								usbEvolHdr_.Serialize( str );

								memcpy( &str[10], (char *) &prCommandMsg, sizeof(prCommandMsg));

								if (agentAttached_)
								{
									Util::CLockCriticalSection lock( fakeWeigherCS_);

									if (clientSocket_)
									{
										Debug::trace("FPCCommManager.Socket", HobString("sending printer command size:") + Integer::toString(sizeof(prCommandMsg) + USBEvolHeader::SIZE));
										clientSocket_->rawSend(str, USBEvolHeader::SIZE);
										clientSocket_->rawSend(&str[10], sizeof(prCommandMsg));

										char *pLabelData = (char *)printerRequestVirtualLabel.getDataPointer();
										int bytesToSend = printerRequestVirtualLabel.getTotalDataLength();

										if ((pLabelData != NULL) && (bytesToSend > 0))
										{
											int bytesSent = clientSocket_->rawSend(pLabelData, bytesToSend);
											Debug::trace("FPCCommManager.Socket", HobString("send complete bytes actually sent:") + Integer::toString(bytesSent));
											delete[] pLabelData;
										}
									}
								}
							}
							break;
#endif
		case PrNewOrder:
							{
								Debug::trace("FPCCommManager.Printer.Message.Out", "PrNewOrder");

								PrinterNewPrinterType newPrinterType(buffer);
								
								myPrinterType_ = newPrinterType.getNewPrinterType();
							}
							break;
		default		:
					Debug::trace("FPCCommManager.Error", HobString("Unknown printer message type: 0x") +
															 HobString(Hex::toString(prEventType)));
					break;
	}

	if ( result != OK ) //error
	{
		printUSBError( result, is_custom_code );
	}

	return true;
}

bool FPCCommManager::handlePrinterSysEvent(SysEvent sysEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	switch (sysEventType)
	{
		case SysReqWakeup:
						{
							Debug::trace("FPCCommManager.PrinterSys.Message.Out", "SysReqWakeup");

							SysGenericMsg genMsg;
							memset((char *) &genMsg, 0, sizeof(genMsg));

							genMsg.function_code = SysWakeup;

							unsigned short msgLength = sizeof(genMsg);
							str[0] = USBEvolHeader::EVOL_PRINTER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
							
						}
						break;
		case SysConfig:
						{
							Debug::trace("FPCCommManager.PrinterSys.Message.Out", "SysConfig");

							PrinterSystemConfiguration prSysConfig(buffer);
							prSysConfig.show();

							PrSysConfigMsg sysConfigMsg;
							memset((char *) &sysConfigMsg, 0, sizeof(sysConfigMsg));

							sysConfigMsg.function_code = SysConfig;
							sysConfigMsg.configType = (UINT16)((SysConfigType) htons((u_short)prSysConfig.getSysConfigType()));
							prSysConfig.getDeviceSignature((UINT8 *) &sysConfigMsg.system_config.device_signature);
							sysConfigMsg.system_config.station_address = (DeviceAddress) htons(prSysConfig.getStationAddress());
							sysConfigMsg.system_config.controller_address = (DeviceAddress) htons(prSysConfig.getControllerAddress());
							sysConfigMsg.hardware_major_revision = prSysConfig.getHardwareVersionMajor();
							sysConfigMsg.hardware_minor_revision = prSysConfig.getHardwareVersionMinor();

							unsigned short msgLength = sizeof(sysConfigMsg);
							str[0] = USBEvolHeader::EVOL_PRINTER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &sysConfigMsg, sizeof(sysConfigMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(sysConfigMsg) + 3, result, is_custom_code );
							
						}
						break;
		case SysReqStatus:
						{
							Debug::trace("FPCCommManager.PrinterSys.Message.Out", std::string("SysReqStatus:") +
								Bool::toString(printerRequestedStatus_) + Bool::toString(weigherRequestedStatus_));

							if (!weigherRequestedStatus_)//!printerRequestedStatus_
							{
								printerRequestedStatus_ = true;

								SysGenericMsg genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.function_code = SysStatus;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::EVOL_PRINTER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );

							}
							else
								printerRequestedStatus_ = true;
							
						}
						break;
		case SysReqInquire:
						{
							Debug::trace("FPCCommManager.PrinterSys.Message.Out", "SysReqInquire");

							printerRequestedInquire_ = true;

							SysGenericMsg genMsg;
							memset((char *) &genMsg, 0, sizeof(genMsg));

							genMsg.function_code = SysInquire;

							unsigned short msgLength = sizeof(genMsg);
							str[0] = USBEvolHeader::EVOL_PRINTER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
											
						}
						break;
		case SysReqFlashStatus:
						{
							Debug::trace("FPCCommManager.PrinterSys.Message.Out", "SysReqFlashStatus");

							SysGenericMsg genMsg;
							memset((char *) &genMsg, 0, sizeof(genMsg));

							genMsg.function_code = SysFlashStatus;

							unsigned short msgLength = sizeof(genMsg);
							str[0] = USBEvolHeader::EVOL_PRINTER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
										
						}
						break;
		default				:
						{
							Debug::trace("FPCCommManager.Error", HobString("Unknown printer system message type: 0x") +
																	 HobString(Hex::toString(sysEventType)));
						}
						break;
	}

	if ( result != OK ) //error
	{
		printUSBError( result, is_custom_code );
	}

	return true;
}

bool FPCCommManager::handleWeigherEvent(WgEvent wgEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	switch (wgEventType)
	{
		case WgConfig:
							{
								Debug::trace("FPCCommManager.Weigher.Message.Out", "WgConfig");

								WeigherConfiguration wgConfig(buffer);

								if (Debug::isTracing("FPCCommManager.wgConfig"))
								{
									Debug::trace("FPCCommManager.wgConfig", "sending wgConfig");
									wgConfig.show();
								}

								Usb2WgConfig usbWgConfig;
								memset((char *) &usbWgConfig, 0, sizeof(usbWgConfig));

								usbWgConfig.function_code = WgConfig;
								usbWgConfig.configType = (UINT16)((WgConfigType) htons((u_short)wgConfig.getConfigType()));
								wgConfig.getDeviceSignature((UINT8 *) &usbWgConfig.config.ultimaWgCfg.device_signature);
								usbWgConfig.config.ultimaWgCfg.center_of_maintenance_zone = htonl(wgConfig.getCenterOfMaintenanceZone());
								usbWgConfig.config.ultimaWgCfg.scale_factor = htonl(wgConfig.getScaleFactor());
								usbWgConfig.config.ultimaWgCfg.max_weight = htonl(wgConfig.getMaxWeight());
								usbWgConfig.config.ultimaWgCfg.gain_factor = htonl(wgConfig.getGainFactor());
								usbWgConfig.config.ultimaWgCfg.zero_reference = htonl(wgConfig.getZeroReference());
								usbWgConfig.config.ultimaWgCfg.calibration_reference = htonl(wgConfig.getCalibrationReference());
								usbWgConfig.config.ultimaWgCfg.rate_option_time = htons(wgConfig.getRateOptionTime());
								usbWgConfig.config.ultimaWgCfg.auto_zero_maintenance_limit = htons(wgConfig.getAutoZeroMaintenanceLimit());
								usbWgConfig.config.ultimaWgCfg.prepack_motion_count = htons(wgConfig.getPrepackMotionCount());
								usbWgConfig.config.ultimaWgCfg.initialize_zero_time = htons(wgConfig.getInitializeZeroTime());
								usbWgConfig.config.ultimaWgCfg.auto_zero_time = htons(3);
								usbWgConfig.config.ultimaWgCfg.rate_option_limit = htons(wgConfig.getRateOptionLimit());
								usbWgConfig.config.ultimaWgCfg.zero_reference_limit = htons(wgConfig.getZeroReferenceLimit());
								usbWgConfig.config.ultimaWgCfg.calibration_reference_limit = htons(wgConfig.getCalibrationReferenceLimit());
								usbWgConfig.config.ultimaWgCfg.small_motion_limit = htons(wgConfig.getSmallMotionLimit());
								usbWgConfig.config.ultimaWgCfg.large_motion_limit = htons(wgConfig.getLargeMotionLimit());
								usbWgConfig.config.ultimaWgCfg.large_motion_count = htons(wgConfig.getLargeMotionCount());
								usbWgConfig.config.ultimaWgCfg.small_motion_count = htons(wgConfig.getSmallMotionCount());
								usbWgConfig.config.ultimaWgCfg.no_motion_count = htons(wgConfig.getNoMotionCount());
								usbWgConfig.config.ultimaWgCfg.zero_tracking_option = htons(wgConfig.getZeroTrackingOption());
								usbWgConfig.config.ultimaWgCfg.negative_weight_option = htons(wgConfig.getNegativeWeightOption());
								usbWgConfig.config.ultimaWgCfg.powerup_maintenance_zone_limit = htons(wgConfig.getPowerupMaintenanceZoneLimit());
								usbWgConfig.config.ultimaWgCfg.operate_maintenance_zone_limit = htons(wgConfig.getOperateMaintenanceZoneLimit());
								usbWgConfig.config.ultimaWgCfg.filter_speed = (UINT8)wgConfig.getFilterSpeed();
								usbWgConfig.config.ultimaWgCfg.reference_voltage = wgConfig.getReferenceVoltage();
								usbWgConfig.config.ultimaWgCfg.last_calibration_date = htonl(wgConfig.getLastCalibrationDate());
								usbWgConfig.config.ultimaWgCfg.number_of_calibrations = htons(wgConfig.getNumberOfCalibrations());
								usbWgConfig.config.ultimaWgCfg.last_configuration_date = htonl(wgConfig.getLastConfigurationDate());
								usbWgConfig.config.ultimaWgCfg.number_of_configurations = htons(wgConfig.getNumberOfConfigurations());
								usbWgConfig.config.weigher_type = (UINT8) wgConfig.getWeigherType();

								usbWgConfig.config.flags = (UINT8) wgConfig.getFlags();
								usbWgConfig.config.min_weight_to_print = htonl(wgConfig.getMinWeightToPrint());
								usbWgConfig.config.weigher_model = (UINT8) wgConfig.getWeigherModel();

								unsigned short msgLength = sizeof(usbWgConfig);
								str[0] = USBEvolHeader::WEIGHER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &usbWgConfig, sizeof(usbWgConfig));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(usbWgConfig) + 3, result, is_custom_code );
							}
							break;
		case WgMode:
							{
								Debug::trace("FPCCommManager.Weigher.Message.Out", "WgMode");

								// Constuct the Evol Message Buffer!!
								WeigherMode wgMode(buffer);
								UltimaWgMode ultimaWgMode;

								ultimaWgMode.function_code = WgMode;
								ultimaWgMode.mode = (UINT8)wgMode.getMode();

								unsigned short msgLength = sizeof(ultimaWgMode);
								str[0] = USBEvolHeader::WEIGHER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &ultimaWgMode, sizeof(ultimaWgMode));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaWgMode) + 3, result, is_custom_code );
								
							}
							break;
		case WgControl:
							{
								Debug::trace("FPCCommManager.Weigher.Message.Out", "WgControl");

								// Constuct the Evol Message Buffer!!
								WeigherControl wgControl(buffer);
								UltimaWgControl ultimaWgControl;

								ultimaWgControl.function_code = WgControl;
								ultimaWgControl.control = wgControl.getControl();

								unsigned short msgLength = sizeof(ultimaWgControl);
								str[0] = USBEvolHeader::WEIGHER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &ultimaWgControl, sizeof(ultimaWgControl));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(ultimaWgControl) + 3, result, is_custom_code );
																
							}
							break;
		case WgReqStatus:
							{
								Debug::trace("FPCCommManager.Weigher.Message.Out", "WgReqStatus");

								UltimaWgGeneric genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.function_code = WgStatus;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::WEIGHER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );

#ifdef FAKE_SOCKET_WEIGHER

								if (agentAttached_)
								{
									Util::CLockCriticalSection lock( fakeWeigherCS_);

									if (clientSocket_)
									{
										Debug::trace("FPCCommManager.Socket", HobString("sending request status size:") + Integer::toString(sizeof(genMsg) + USBEvolHeader::SIZE));
										clientSocket_->rawSend(str, USBEvolHeader::SIZE);
										clientSocket_->rawSend(&str[10], sizeof(genMsg));
									}
								}
#endif

							}
							break;
		case WgReqInquire:
							{
								Debug::trace("FPCCommManager.Weigher.Message.Out", "WgReqInquire");

								UltimaWgGeneric genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.function_code = WgInquire;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::WEIGHER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
								
							}
							break;
		case WgReqWakeup:
							{
								Debug::trace("FPCCommManager.Weigher.Message.Out", "WgReqWakeup");

								WeigherRequestWakeup wgReqWakeup(buffer);

								UltimaWgGeneric genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.function_code = WgWakeup;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::WEIGHER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
							}
							break;
		case WgEnable:
							{
								Debug::trace("FPCCommManager.Weigher.Message.Out", "WgEnable");

								UltimaWgGeneric genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.function_code = WgEnable;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::WEIGHER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
							}
							break;
		case WgDisable:
							{
								Debug::trace("FPCCommManager.Weigher.Message.Out", "WgDisable");

								UltimaWgGeneric genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.function_code = WgDisable;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::WEIGHER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
								
							}
							break;
		case WgRezero:
							{
								Debug::trace("FPCCommManager.Weigher.Message.Out", "WgRezero");

								UltimaWgGeneric genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.function_code = WgRezero;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::WEIGHER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
								
							}
							break;
		case WgReset:
							{
								Debug::trace("FPCCommManager.Weigher.Message.Out", "WgReset");

								UltimaWgGeneric genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.function_code = WgReset;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::WEIGHER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
								
							}
							break;
		default		:
					Debug::trace("FPCCommManager.Error", HobString("Unknown weigher message type: 0x") +
															 HobString(Hex::toString(wgEventType)));
					break;
	}

	if ( result != OK ) //error
	{
		printUSBError( result, is_custom_code );
	}

	return true;
}

bool FPCCommManager::handleWeigherSysEvent(SysEvent sysEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	switch (sysEventType)
	{
		case SysReqWakeup:
						{
							Debug::trace("FPCCommManager.WeigherSys.Message.Out","SysReqWakeup");

							SysGenericMsg genMsg;
							memset((char *) &genMsg, 0, sizeof(genMsg));

							genMsg.function_code = SysWakeup;

							unsigned short msgLength = sizeof(genMsg);
							str[0] = USBEvolHeader::WEIGHER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
						}
						break;
		case SysConfig:
						{
							Debug::trace("FPCCommManager.WeigherSys.Message.Out","SysConfig");

							WeigherSystemConfiguration wgSysConfig(buffer);

							SysConfigMsg sysConfig;

							sysConfig.function_code = SysConfig;
							sysConfig.configType = (UINT16)((SysConfigType) htons((u_short)wgSysConfig.getSysConfigType()));
							wgSysConfig.getDeviceSignature((UINT8 *) &sysConfig.system_config.device_signature);
							sysConfig.system_config.station_address = (DeviceAddress) htons(wgSysConfig.getStationAddress());
							sysConfig.system_config.controller_address = (DeviceAddress) htons(wgSysConfig.getControllerAddress());

							unsigned short msgLength = sizeof(sysConfig);
							str[0] = USBEvolHeader::WEIGHER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &sysConfig, sizeof(sysConfig));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(sysConfig) + 3, result, is_custom_code );
						}
						break;
		case SysReqStatus:
						{
							if (!printerRequestedStatus_ )// !weigherRequestedStatus_
							{
								weigherRequestedStatus_ = true;

								SysGenericMsg genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.function_code = SysStatus;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::WEIGHER;
								memcpy(&str[1], &msgLength, sizeof(msgLength));


								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );

							}
							else
								weigherRequestedStatus_ = true;
						}
						break;
		case SysReqInquire:
						{
							Debug::trace("FPCCommManager.WeigherSys.Message.Out", "SysReqInquire");

							weigherRequestedInquire_ = true;

							SysGenericMsg genMsg;
							memset((char *) &genMsg, 0, sizeof(genMsg));

							genMsg.function_code = SysInquire;

							unsigned short msgLength = sizeof(genMsg);
							str[0] = USBEvolHeader::WEIGHER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));


							memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
						}
						break;
		case SysReset:{	
							Debug::trace("FPCCommManager.WeigherSys.Message.Out", "SysReset");

							SysGenericMsg genMsg;
							memset((char *) &genMsg, 0, sizeof(genMsg));

							genMsg.function_code = SysReset;

							unsigned short msgLength = sizeof(genMsg);
							str[0] = USBEvolHeader::WEIGHER;
							memcpy(&str[1], &msgLength, sizeof(msgLength));

							memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
						}
						break;
		default:
					Debug::trace("FPCCommManager.Error", HobString("Unknown weigher system message type: 0x") +
															 HobString(Hex::toString(sysEventType)));
					break;
	}

	if ( result != OK ) //error
	{
		printUSBError( result, is_custom_code );
	}

	return true;
}

bool FPCCommManager::handleSysEvent(SysEvent sysEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	switch (sysEventType)
	{
		case SysReset:{	
							Debug::trace("FPCCommManager.System.Message.Out", "SysReset");
							SystemReset sysReset(buffer);

							if (sysReset.getUsbNodeId() == nodeId_)
							{
									
								SysGenericMsg genMsg;
								memset((char *) &genMsg, 0, sizeof(genMsg));

								genMsg.function_code = SysReset;

								unsigned short msgLength = sizeof(genMsg);
								str[0] = USBEvolHeader::SYSTEM;
								memcpy(&str[1], &msgLength, sizeof(msgLength));

								memcpy( &str[3], (char *) &genMsg, sizeof(genMsg));
								sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 3, result, is_custom_code );
							
							}
						}
						break;
		default				:
						{
							Debug::trace("FPCCommManager.Error", HobString("Unknown system message type: 0x") +
																	 HobString(Hex::toString(sysEventType)));
						}
						break;
	}
	
	if ( result != OK ) //error
	{
		printUSBError( result, is_custom_code );
	}

	return true;	
}

void FPCCommManager::processUsbMessage( std::string & msg, CUsbPipe::EvolPipeType UNUSED(pipe_type) )
{
	UNREFERENCED_PARAMETER(pipe_type);
		
	const char * ptr = msg.c_str();

	if (*ptr == USBEvolHeader::SYSTEM)
	{
		ptr+=3;
		receivedWeigherSysMessage((SysGenericMsg *) ptr, 0);
		receivedPrinterSysMessage((SysGenericMsg *) ptr, 0);
	
	} 
	else if (*ptr == USBEvolHeader::EVOL_PRINTER)
	{
		ptr+=3;
		receivedPrinterDevMessage((PrGenericMsg *) ptr, 0, 0);
	}
	else if (*ptr == USBEvolHeader::WEIGHER)
	{
		ptr+=3;
		receivedWeigherDevMessage((UltimaWgGeneric *) ptr, 0);
	}
}

void FPCCommManager::receivedPrinterSysMessage(SysGenericMsg *genMsg, UINT16 UNUSED(peripheralAddress))
{
	UNREFERENCED_PARAMETER(peripheralAddress);
		
	switch (genMsg->function_code)
	{
		case SysWakeup:{	
							Debug::trace("FPCCommManager.PrinterSys.Message.In", "SysWakeup");

							SysWakeupMsg *sysWakeupMsg = (SysWakeupMsg *) genMsg;

							if (pMyPrinterMgr_ != NULL)
							{
							 	PrinterSystemWakeup pswu(this, myPrinterType_, (SysCfgStatus) sysWakeupMsg->sysCfgStatus);

								const char *pPswuMsg = pswu.serialize();
								int mLength = pswu.getSerializedLength();

								pMyPrinterMgr_->send(pPswuMsg, mLength);
							}
						}
						break;
		case SysStatus:	{
							Debug::trace("FPCCommManager.PrinterSys.Message.In", "SysStatus");

							PrSysStatusMsg *prSysStatus = (PrSysStatusMsg *) genMsg;

							SystemStatusInfo *pSystemStatus = new SystemStatusInfo((ntohs(prSysStatus->system_status.bit_error)),
																				   (ntohs(prSysStatus->system_status.address_error)),
																				   (ntohs(prSysStatus->system_status.rom_checksum)),
																				   (ntohl(prSysStatus->system_status.flags)),
																				    prSysStatus->system_status.partno,
																				   (ntohs(prSysStatus->system_status.system_checksum)),
																				   (ntohl(prSysStatus->system_status.init_flags)));

							if (printerRequestedStatus_)
							{
								printerRequestedStatus_ = false;

								if (pMyPrinterMgr_ != NULL)
								{

									PrinterSystemStatus pss(this,
															myPrinterType_,
															prKind_,
															pSystemStatus,
															ntohs(prSysStatus->software_major_revision),
															ntohs(prSysStatus->software_minor_revision),
															(UINT8)hardwareVersion_.getMajorRevision(),
															(UINT8)hardwareVersion_.getMinorRevision());

									pss.show();

									const char *pPssMsg = pss.serialize();
									int mLength = pss.getSerializedLength();

									pMyPrinterMgr_->send(pPssMsg, mLength);

								}

							}
							delete pSystemStatus;
						}
						break;
		case SysInquire:{	
							Debug::trace("FPCCommManager.PrinterSys.Message.In", "SysInquire");

							ApcInquireMsg *sysInquireMsg = (ApcInquireMsg *) genMsg;

							if (printerRequestedInquire_)
							{
								printerRequestedInquire_ = false;

								if (pMyPrinterMgr_ != NULL)
								{
							 		PrinterSystemInquire psi(this,
															 myPrinterType_,
															 prKind_,
							 								 (SysConfigType) ntohs(sysInquireMsg->configType),
							 								 (UINT8 *) &sysInquireMsg->device_signature,
							 								 (DeviceAddress) ntohs(sysInquireMsg->station_address),
							 								 (DeviceAddress) ntohs(sysInquireMsg->controller_address),
															 sysInquireMsg->hardware_major_revision,
															 sysInquireMsg->hardware_minor_revision);

									const char *pPsiMsg = psi.serialize();
									int mLength = psi.getSerializedLength();

									pMyPrinterMgr_->send(pPsiMsg, mLength);
								}
							}
						}
						break;
		case SysCalibSwitch:
						{
							Debug::trace("FPCCommManager.PrinterSys.Message.In", "SysCalibSwitch");

							SysCalibSwitchMsg *systemCalibMsg = (SysCalibSwitchMsg *) genMsg;

							if (pMyPrinterMgr_ != NULL)
							{
								PrinterSystemCalibrationSwitch calibSwitch(this);

								if (( systemCalibMsg->calib_switch_state) > 0)
								{
									calibSwitch.setSystemCalibrationSwitch();
								}
								else
								{
									calibSwitch.clearSystemCalibrationSwitch();
								}

								if (Debug::isTracing("FPCCommManager.WeigherSystemCalibrationSwitch") == true)
								{
									calibSwitch.show();
								}

								const char *pPscsMsg = calibSwitch.serialize();
								int mLength = calibSwitch.getSerializedLength();

								pMyPrinterMgr_->send(pPscsMsg, mLength);
							}
						}
						break;
		case SysFlashStatus:
						{
							Debug::trace("FPCCommManager.PrinterSys.Message.In", "SysFlashStatus");

							SysFlashStatusMsg *flashStatusMsg = (SysFlashStatusMsg *) genMsg;

							if (pMyPrinterMgr_ != NULL)
							{
								PrinterFlashStatus flashStat(this, prKind_, flashStatusMsg);


								if (Debug::isTracing("FPCCommManager.FlashStatus") == true)
								{
									flashStat.show();
								}

								const char *pPfsMsg = flashStat.serialize();
								int mLength = flashStat.getSerializedLength();

								pMyPrinterMgr_->send(pPfsMsg, mLength);
							}
						}
						break;
		default:
						Debug::trace("FPCCommManager.Error", HobString("Printer Error: Unknown printer system message type: 0x") +
																 HobString(Hex::toString(genMsg->function_code)));
						break;
	}

}

void FPCCommManager::receivedPrinterDevMessage(PrGenericMsg *genMsg, UINT16 peripheralAddress, UINT16 UNUSED(logicalAddress))
{
	UNREFERENCED_PARAMETER(logicalAddress);
	
	switch (genMsg->prEvent)
	{
		case PrWakeup:	{
							Debug::trace("FPCCommManager.Printer.Message.In", "PrWakeup");

								PrWakeupMsg *prWakeupMsg = (PrWakeupMsg *) genMsg;

								myPrinterType_ = (PrinterType) prWakeupMsg->printerType;
								printerBufferSize_ = ntohs(prWakeupMsg->bufferSize);
								printheadSize_ = ntohs(prWakeupMsg->printheadSize);


								// Find out if this is first WakeUp Message
								if (pMyPrinterMgr_ == NULL)
								{
									Debug::trace("FPCCommManager.Printer.Message.In", "************************************************");
									Debug::trace("FPCCommManager.Printer.Message.In", HobString("printerType: ") +
																				 Integer::toString((UINT16) myPrinterType_));
									Debug::trace("FPCCommManager.Printer.Message.In", HobString("labelWidth: ") +
																				 Integer::toString((UINT16) prWakeupMsg->labelWidth));
									Debug::trace("FPCCommManager.Printer.Message.In", HobString("bufferSize: ") +
																				 Integer::toString(printerBufferSize_));
									Debug::trace("FPCCommManager.Printer.Message.In", HobString("printheadSize: ") +
																				 Integer::toString((UINT16) printheadSize_));
									
									if (prWakeupMsg->prCfgStatus)
										Debug::trace("FPCCommManager.Printer.Message.In", HobString("validConfig: ") + Bool::toString(true));
									else
										Debug::trace("FPCCommManager.Printer.Message.In", HobString("validConfig: ") + Bool::toString(false));

									// Create a new thread
									pMyPrinterMgr_ = RealTimeController::getInstance()->NewESCPrinter(nodeId_, this, NULL,
																									(PrinterType) prWakeupMsg->printerType,
																									(LabelWidth) prWakeupMsg->labelWidth,
																									printerBufferSize_,
																									printheadSize_ * 8, // wants to be in dots
																									peripheralAddress,
																									prKind_);


									usbEvolHdr_.setDestinationPhysicalAddress(peripheralAddress);

								}
								else
								{
									PrinterWakeup printerWakeup(this, (PrinterType) prWakeupMsg->printerType,
															   (LabelWidth) prWakeupMsg->labelWidth,
															   printerBufferSize_,
															   printheadSize_ * 8, // wants to be in dots
															   (PrCfgStatus) prWakeupMsg->prCfgStatus);

									const char *pWuMsg = printerWakeup.serialize();
									int mLength = printerWakeup.getSerializedLength();

									pMyPrinterMgr_->send(pWuMsg, mLength);
								}
						} 
						break;
		case PrInquire:	{	
							Debug::trace("FPCCommManager.Printer.Message.In", "PrInquire");

							PrInquireMsg *inquireMsg = (PrInquireMsg *) genMsg;

							if (pMyPrinterMgr_ != NULL)
							{
								PrinterInquire prInquire(this, myPrinterType_); // use default constructor

								prInquire.setConfigType((PrConfigType) ntohs(inquireMsg->prConfigType));
								prInquire.setDeviceSignature(inquireMsg->prConfiguration.deviceSignature);
								prInquire.setPrinterType((PrinterType) inquireMsg->prConfiguration.printerType);
								prInquire.setLabelWidth((LabelWidth) inquireMsg->prConfiguration.labelWidth);
								prInquire.setMediaSensorAdjustment(inquireMsg->prConfiguration.mediaSensorAdjustment);
								prInquire.setOutOfMediaCount(inquireMsg->prConfiguration.outOfMediaCount);		// Immutable!! Set to 200 on Ultima
								prInquire.setPrintContrast(ntohs(inquireMsg->prConfiguration.printContrast));	// "Print Contrast" on Ultima Configuration.
								prInquire.setConfigWord0(ntohs(inquireMsg->prConfiguration.configWord0));				// Used on "European" labels
								prInquire.setConfigWord1(ntohs(inquireMsg->prConfiguration.configWord1));			// Used on "European" labels
								prInquire.setConfigWord2(ntohs(inquireMsg->prConfiguration.configWord2));		// Used on "European" labels
								prInquire.setConfigWord3(ntohs(inquireMsg->prConfiguration.configWord3));	// "Expel Position" on Ultima Configuration.
								prInquire.setConfigWord4(ntohs(inquireMsg->prConfiguration.configWord4));			// Used on "European" labels
								prInquire.setConfigWord5(ntohs(inquireMsg->prConfiguration.configWord5));
								prInquire.setConfigWord6(ntohs(inquireMsg->prConfiguration.configWord6));
								prInquire.setConfigWord7(ntohs(inquireMsg->prConfiguration.configWord7));
								prInquire.setConfigWord8(ntohs(inquireMsg->prConfiguration.configWord8));
								prInquire.setConfigWord9(ntohs(inquireMsg->prConfiguration.configWord9));
								prInquire.setConfigWord10(ntohs(inquireMsg->prConfiguration.configWord10));
								prInquire.setConfigWord11(ntohs(inquireMsg->prConfiguration.configWord11));
								prInquire.setConfigWord12(ntohs(inquireMsg->prConfiguration.configWord12));
								prInquire.setConfigWord13(ntohs(inquireMsg->prConfiguration.configWord13));
								prInquire.setMediaSensorType((MediaSensorType) ntohs(inquireMsg->prConfiguration.mediaSensorType));
								prInquire.setSLAEjectLine(ntohs(inquireMsg->prConfiguration.slaEjectLine));
								prInquire.setPrintPositionAbsolute(ntohs(inquireMsg->prConfiguration.printPosition));

								const char *pWiMsg = prInquire.serialize();
								int mLength = prInquire.getSerializedLength();

								pMyPrinterMgr_->send(pWiMsg, mLength);
							}
						}
						break;
		case PrStatus:	{
							Debug::trace("FPCCommManager.Printer.Message.In", "PrStatus");

							PrStatusMsg *statusMsg = (PrStatusMsg *) genMsg;

							if (pMyPrinterMgr_ != NULL)
							{
								PrinterStatus ps(this, myPrinterType_); // use default constructor

								ps.setState(statusMsg->state);
								ps.setCommand(statusMsg->command);
								ps.setError(statusMsg->error);
								ps.setHistory(statusMsg->history);
								ps.setSensor(statusMsg->sensor);
								ps.setSensor2(statusMsg->sensor2);
								ps.setUser(statusMsg->user);
								ps.setCounter(ntohs(statusMsg->counter));
								ps.setSensorMask(statusMsg->maskInfo.sensorMask);
								ps.setSensor2Mask(statusMsg->maskInfo.sensor2Mask);
								ps.setUserMask(statusMsg->maskInfo.userMask);

								const char *pPsMsg = ps.serialize();
								int mLength = ps.getSerializedLength();

								pMyPrinterMgr_->send(pPsMsg, mLength);
							}
						}
						break;

		case PrSensors:	{	
							Debug::trace("FPCCommManager.Printer.Message.In", "PrSensors");

							if (pMyPrinterMgr_ != NULL)
							{
								PrSensorsMsg *prSensorMsg = (PrSensorsMsg *) genMsg;

								// Create a PrSensorMsg and send it to the printer manager!
								PrinterSensors printerSensors(this, myPrinterType_); // use default constructor

								UINT16 printheadReading = ntohs(prSensorMsg->headupReading);
								PrinterHeadUpType prHeadUp = PrinterHeadEngaged;
								if (printheadReading >= 190)
								{
									prHeadUp = PrinterHeadDisengaged;
								}

								INT16 labelWidthReading = ntohs(prSensorMsg->labelWidthReading);
								LabelWidth labelWidth = UnknownLabel;
								if ((0 <= labelWidthReading) && (labelWidthReading <= 64))
								{
									labelWidth = NarrowLabel;
								}
								else if ((64 < labelWidthReading) && (labelWidthReading <= 190))
								{
									labelWidth = WideLabel;
								}
							
								printerSensors.setHeadUp(prHeadUp);
								printerSensors.setLabelWidth(labelWidth);
								printerSensors.setRegulatorTemperature(ntohs(prSensorMsg->regulatorTemperatureReading));

// PTR_508: Head Temp calculation moved from PrinterSensors to its own Comm manager.
								UINT16 headTemperature = ntohs(prSensorMsg->headTemperatureReading);
								int size = sizeof(phTempsC)/sizeof(phTempsC[0]);
								int c = 0;

								for (c = 0; c < size; c++)
								{
									if (headTemperature >= phTempsC[c])
										break;
								}
								printerSensors.setHeadTemperature((UINT16)c);
								printerSensors.setHeadVoltageCounts(ntohs(prSensorMsg->headVoltageReading));
								printerSensors.setHeadCurrent(ntohs(prSensorMsg->headCurrentReading));
								printerSensors.setLabelHighAverage(ntohs(prSensorMsg->labelHighAverage));
								printerSensors.setLabelLowAverage(ntohs(prSensorMsg->labelLowAverage));
								printerSensors.setLabelThreshold(ntohs(prSensorMsg->labelThreshold));
								printerSensors.setLabel(ntohs(prSensorMsg->labelReading));
								printerSensors.setMediaHighAverage(ntohs(prSensorMsg->mediaHighAverage));
								printerSensors.setMediaLowAverage(ntohs(prSensorMsg->mediaLowAverage));
								printerSensors.setMediaThreshold(ntohs(prSensorMsg->mediaThreshold));
								printerSensors.setMedia(ntohs(prSensorMsg->mediaReading));

								const char *pWsMsg = printerSensors.serialize();
								int mLength = printerSensors.getSerializedLength();

								pMyPrinterMgr_->send(pWsMsg, mLength);
							
							}
						} 
						break;

		default:		
						break;
	}

}

void FPCCommManager::receivedWeigherSysMessage(SysGenericMsg *genMsg, UINT16 physicalAddress)
{

	switch (genMsg->function_code)
	{
		case SysWakeup:{	
							Debug::trace("FPCCommManager.WeigherSys.Message.In", "SysWakeup");

							SysWakeupMsg *sysWakeupMsg = (SysWakeupMsg *) genMsg;

						 	WeigherSystemWakeup wswu(this, (SysCfgStatus) sysWakeupMsg->sysCfgStatus);

							const char *pWswuMsg = wswu.serialize();
							int mLength = wswu.getSerializedLength();
							
							if (pMyWeigher_ != NULL)
								pMyWeigher_->send(pWswuMsg, mLength);
						}
						break;
		case SysStatus:	{
							Debug::trace("FPCCommManager.WeigherSys.Message.In", "SysStatus");
							if (weigherRequestedStatus_)
							{
								weigherRequestedStatus_ = false;

								if (pMyWeigher_ != NULL)
								{
									ApcSysStatusMsg *sysStatusMsg = (ApcSysStatusMsg *) genMsg;

									UsbWeigherSystemStatus sysStatus(this);

									sysStatus.setFlags(ntohl(sysStatusMsg->system_status.flags));
									sysStatus.setInitFlags(ntohl(sysStatusMsg->system_status.init_flags));
									sysStatus.setSerialNumber(ntohl(sysStatusMsg->weigherSerialNum));
									sysStatus.setSoftwareType(HobVersion::ApcPrinterSW);
									sysStatus.setSoftwareVersion(ntohs(sysStatusMsg->software_major_revision), ntohs(sysStatusMsg->software_minor_revision));
									sysStatus.setHardwareType(HobVersion::ApcPrinterHW);
									sysStatus.setHardwareVersion((UINT8)hardwareVersion_.getMajorRevision(), (UINT8)hardwareVersion_.getMinorRevision());

									sysStatus.show();

									const char *pSysStatus = sysStatus.serialize();
									int mLength = sysStatus.getSerializedLength();
									
									pMyWeigher_->send(pSysStatus, mLength);
								}
							}
						}
						break;
		case SysInquire:{	
							Debug::trace("FPCCommManager.WeigherSys.Message.In", "SysInquire");

							ApcInquireMsg *systemInquireMsg = (ApcInquireMsg *) genMsg;

							if (weigherRequestedInquire_)
							{
								weigherRequestedInquire_ = false;

								if (pMyWeigher_ != NULL)
								{
						 			WeigherSystemInquire wsi(this,
						 									 (SysConfigType) ntohs(systemInquireMsg->configType),
						 									 (UINT8 *) &systemInquireMsg->device_signature,
						 									 (DeviceAddress) ntohs(systemInquireMsg->station_address),
						 									 (DeviceAddress) ntohs(systemInquireMsg->controller_address));

									const char *pWsiMsg = wsi.serialize();
									int mLength = wsi.getSerializedLength();

									pMyWeigher_->send(pWsiMsg, mLength);
								}
							}
						}
						break;
		default:
						Debug::trace("FPCCommManager.Error", HobString("Received Unknown weigher system message type: 0x") +
																 HobString(Hex::toString(genMsg->function_code)));
						break;
	}

}

void FPCCommManager::receivedWeigherDevMessage(UltimaWgGeneric *genMsg, UINT16 physicalAddress) // , HobThread *pHobThread)
{
	switch (genMsg->function_code)
	{
		case WgWakeup:	{	
							Debug::trace("FPCCommManager.Weigher.Message.In", "WgWakeup");

							UltimaWgWakeup *wakeupMsg = (UltimaWgWakeup *) genMsg;

							if (pMyWeigher_ == NULL)
							{
								Debug::trace("FPCCommManager.WeigherWakeup", "************************************************");

						   		// Create a new thread
								pMyWeigher_ = RealTimeController::getInstance()->NewUSBWeigher(nodeId_, this, physicalAddress, WgUsbApc);

								usbEvolHdr_.setDestinationPhysicalAddress(physicalAddress);
							}
							else
							{
								WeigherWakeup ww(this, (WgCfgStatus) wakeupMsg->wgCfgStatus);

								const char *pWwMsg = ww.serialize();
								int mLength = ww.getSerializedLength();

								pMyWeigher_->send(pWwMsg, mLength);
							}
						} 
						break;
		case WgStatus:	{
							Debug::trace("FPCCommManager.Weigher.Message.In", "WgStatus");
#ifdef FAKE_SOCKET_WEIGHER
							if (!agentAttached_)
#endif
							{
								UltimaWgStatus *statusMsg = (UltimaWgStatus *) genMsg;

								WeigherStatus ws(this); // use default constructor
								ws.setStatusBits(ntohs(statusMsg->status));
								ws.setCounts(ntohl(statusMsg->counts));
								ws.setRawCounts(ntohl(statusMsg->raw_counts));

								if ((Debug::isTracing("FPCCommManager.WeigherStatus") == true) && (ws.isPositiveNonZeroStableWeight() == true))
								{
									ws.show();
								}

								const char *pWsMsg = ws.serialize();
								int mLength = ws.getSerializedLength();

								if (pMyWeigher_ != NULL)
									pMyWeigher_->send(pWsMsg, mLength);
							}
						}
						break;

		case WgInquire:	{	
							Debug::trace("FPCCommManager.Weigher.Message.In", "WgInquire");

							Usb2WgInquire *inquireMsg = (Usb2WgInquire *) genMsg;

							WeigherInquire wi(this); // use default constructor

							wi.setConfigType((WgConfigType) ntohs(inquireMsg->configType));
							wi.setDeviceSignature((UINT8 *) &(inquireMsg->config.ultimaWgCfg.device_signature));
							wi.setCenterOfMaintenanceZone(ntohl(inquireMsg->config.ultimaWgCfg.center_of_maintenance_zone));
							wi.setScaleFactor(ntohl(inquireMsg->config.ultimaWgCfg.scale_factor));
							wi.setMaxWeight(ntohl(inquireMsg->config.ultimaWgCfg.max_weight));
							wi.setGainFactor(ntohl(inquireMsg->config.ultimaWgCfg.gain_factor));
							wi.setZeroReference(ntohl(inquireMsg->config.ultimaWgCfg.zero_reference));
							wi.setCalibrationReference(ntohl(inquireMsg->config.ultimaWgCfg.calibration_reference));
							wi.setRateOptionTime(ntohs(inquireMsg->config.ultimaWgCfg.rate_option_time));
							wi.setAutoZeroMaintenanceLimit(ntohs(inquireMsg->config.ultimaWgCfg.auto_zero_maintenance_limit));
							wi.setPrepackMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.prepack_motion_count));
							wi.setInitializeZeroTime(ntohs(inquireMsg->config.ultimaWgCfg.initialize_zero_time));
							wi.setAutoZeroTime(ntohs(inquireMsg->config.ultimaWgCfg.auto_zero_time));
							wi.setRateOptionLimit(ntohs(inquireMsg->config.ultimaWgCfg.rate_option_limit));
							wi.setZeroReferenceLimit(ntohs(inquireMsg->config.ultimaWgCfg.zero_reference_limit));
							wi.setCalibrationReferenceLimit(ntohs(inquireMsg->config.ultimaWgCfg.calibration_reference_limit));
							wi.setSmallMotionLimit(ntohs(inquireMsg->config.ultimaWgCfg.small_motion_limit));
							wi.setLargeMotionLimit(ntohs(inquireMsg->config.ultimaWgCfg.large_motion_limit));
							wi.setLargeMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.large_motion_count));
							wi.setSmallMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.small_motion_count));
							wi.setNoMotionCount(ntohs(inquireMsg->config.ultimaWgCfg.no_motion_count));
							wi.setZeroTrackingOption(ntohs(inquireMsg->config.ultimaWgCfg.zero_tracking_option));
							wi.setNegativeWeightOption(ntohs(inquireMsg->config.ultimaWgCfg.negative_weight_option));
							wi.setPowerupMaintenanceZoneLimit(ntohs(inquireMsg->config.ultimaWgCfg.powerup_maintenance_zone_limit));
							wi.setOperateMaintenanceZoneLimit(ntohs(inquireMsg->config.ultimaWgCfg.operate_maintenance_zone_limit));
							wi.setFilterSpeed((WgFilterSpeed) inquireMsg->config.ultimaWgCfg.filter_speed);
							wi.setLastCalibrationDate(ntohl(inquireMsg->config.ultimaWgCfg.last_calibration_date));
							wi.setNumberOfCalibrations(ntohs(inquireMsg->config.ultimaWgCfg.number_of_calibrations));
							wi.setLastConfigurationDate(ntohl(inquireMsg->config.ultimaWgCfg.last_configuration_date));
							wi.setNumberOfConfigurations(ntohs(inquireMsg->config.ultimaWgCfg.number_of_configurations));
							wi.setWeigherType((WeigherType) (inquireMsg->config.weigher_type));

							wi.setFlags((UINT8)(inquireMsg->config.flags));
							wi.setMinWeightToPrint(ntohl(inquireMsg->config.min_weight_to_print));
							wi.setWeigherModel((WeigherModel) (inquireMsg->config.weigher_model));

							if (Debug::isTracing("FPCCommManager.WeigherInquire") == true)
							{
								wi.show();
							}

							if (wi.getAutoZeroTime() != 3)
							{
								wi.setAutoZeroTime(3);								
							}
							
							const char *pWiMsg = wi.serialize();
							int mLength = wi.getSerializedLength();

							if (pMyWeigher_ != NULL)
								pMyWeigher_->send(pWiMsg, mLength);
						}
						break;
		default:
						Debug::trace("FPCCommManager.Error", HobString("Received Unknown weigher message type: 0x") +
																 HobString(Hex::toString(genMsg->function_code)));
						break;
	}
}

void FPCCommManager::sendBulkMessageCallBack( DWORD result, bool user_defined_error )
{
	Debug::trace("FPCCommManager.UsbIrp", "sendBulkDataCallback");

	UINT16 *pOldBuldData = pBulkData_;

	sendingBulkData_ = false;


	if ( result == CUsbPipe::TimeOut && user_defined_error )
	{
		Debug::trace("FPCCommManager.BulkData", "Bulk Transfer ERROR: S_usbHcdLib_TIMEOUT");
		/*Debug::trace("FPCCommManager.BulkData", HobString("Needed Transfered: ") +
												HobString(Integer::toString(pESCCommManager->irpBulkOut_.bfrList [0].bfrLen)));;
		Debug::trace("FPCCommManager.BulkData", HobString("Actual Transfered: ") +
												HobString(Integer::toString(pESCCommManager->irpBulkOut_.bfrList [0].actLen)));;*/
	}

	if (result == OK)
	{
		Debug::trace("FPCCommManager.UsbIrp", "transfer OK");

		if ( pMyPrinterMgr_ != NULL)
		{

			PrinterLabelTransferComplete ltComplete(this,
													myPrinterType_,
													bulkDataLength_,
													bulkDataLength_,
													pOldBuldData);

			pBulkData_ = NULL;
			bulkDataLength_ = 0;

			const char *pltComplete = ltComplete.serialize();
			int mLength = ltComplete.getSerializedLength();

			pMyPrinterMgr_->send(pltComplete, mLength);
		}
	}
	else
	{
		Debug::trace("FPCCommManager.Error", "sendBulkDataCallback Printer Irp Not Successful");
		UsbManager::getInstance()->printUSBError(result);

		

		if (pMyPrinterMgr_ != NULL)
		{
			PrinterLabelTransferFailed ltFailed(this,
												myPrinterType_,
												bulkDataLength_, //lcg rev 0.2pESCCommManager->irpBulkOut_.bfrList [0].bfrLen,
												0, //lcg rev 0.1pESCCommManager->irpBulkOut_.bfrList [0].actLen,
												pOldBuldData);

			const char *pltFailed = ltFailed.serialize();
			int mLength = ltFailed.getSerializedLength();

			pMyPrinterMgr_->send(pltFailed, mLength);
		}
		pBulkData_ = NULL;
		bulkDataLength_ = 0;
	}
}

void FPCCommManager::receiveErrorCallBack( CUsbPipe::EvolPipeType pipe_type, DWORD error, bool user_defined_error )
{
	printUSBError( error, user_defined_error );

	Debug::trace("FPCCommManager.Error", std::string("***receiveErrorCallBack***::error = ") + Integer::toString(error) +
		std::string("pipe type: ") + Integer::toString(pipe_type));
	//when the usb device is unplugged, the following error on the receive is ERROR_GEN_FAILURE.
	//since reads are always outstanding we are only checking for this condition on the receive.
	//we could check for loss of usb comm on the output. but this would be overkill.
#if defined(LINUX_FIX)
    //if (error == PAL_ERR_GENERAL_FAILURE)
#else
	//if (error == ERROR_GEN_FAILURE)
#endif
	//	stopPipes();
}

#ifdef SEND_INITIAL_WAKEUP_REQUEST_TO_FPC
void FPCCommManager::sendInitialWakeupRequests()
{
   //case SysReqWakeup:
    {
 	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

        Debug::trace("FPCCommManager", "sending initial SysWakeup");

        SysGenericMsg genMsg;
        memset((char *) &genMsg, 0, sizeof(genMsg));

        genMsg.function_code = SysWakeup;

        usbEvolHdr_.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
        usbEvolHdr_.setMessageLength(sizeof(genMsg));
        usbEvolHdr_.Serialize( str );

        memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
        sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

    }
    //case PrReqWakeup:
    {
 	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	    Debug::trace("FPCCommManager", "sending initial PrReqWakeup");

	    PrinterRequestWakeup prReqWakeup(this);

	    PrGenericMsg genMsg;
	    memset((char *) &genMsg, 0, sizeof(genMsg));

	    genMsg.prEvent = PrWakeup;

	    if (pMyPrinterMgr_ == NULL)
	    {
		    usbEvolHdr_.setDestinationPhysicalAddress(prReqWakeup.getDefaultHardwareAddress());
	    }

	    usbEvolHdr_.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
	    usbEvolHdr_.setMessageLength(sizeof(genMsg));
	    usbEvolHdr_.Serialize( str );

	    memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
	    sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
    	
    }

}
#endif // SEND_INITIAL_WAKEUP_REQUEST_TO_APC
