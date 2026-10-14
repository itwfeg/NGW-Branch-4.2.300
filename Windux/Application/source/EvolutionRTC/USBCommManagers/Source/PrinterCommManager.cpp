
#include <PAL_platformheader.h>
//#include <winsock2.h>	  /* for htons, ntohs, etc. */ // replaced by PAL header


#include "Configuration.h"
#include "RealTimeController.h"
#include "PrinterManager.h"
#include "UsbManager.h"
#include "PrinterCommManager.h"

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

#include "SystemReset.h"

#include "UsbRemoveComponent.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "UInteger.h"
#include "Bool.h"
#include "Hex.h"

#include "HobSocket.h"
// temporary
#include "VoidLabelBits.h"

#define SWAP_BYTES(x)		(MSB(x) | LSB(x) << 8)  

#define EP2_MAX_PACKET_SIZE 64
#define EP4_MAX_PACKET_SIZE 64

bool PrinterCommManager::voidLabelSwapped = false;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterCommManager::dataType = 0;
std::string PrinterCommManager::dataTypeName("PrinterCommManager"); 
#endif

//PrinterCommManager::PrinterCommManager(PrinterKind prKind)
PrinterCommManager::PrinterCommManager(USBD_NODE_ID node_id, SUsbDescriptors * descs ): Hobart::CEvolMsgReceiver("PrinterCommManager"), CUsbCommBase( descs, node_id ), nodeId( node_id )
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("PrinterCommManager", HobString("PrinterCommManager"));

	busSuspended = false;

	//use the product id to determine the kind of printer -- ats
	//new XPE needs kind but for now eppPrinter.
	// Updated to use UsbManager.h definitions and EPP2 kind. (haberrc)
	if (UsbManager::getInstance()->getProductId(node_id) == HOBART_EVO_PRINTER_ID)
	{
		this->prKind = EppPrinter;															  
	}
	else if (UsbManager::getInstance()->getProductId(node_id) == HOBART_XPE_PRINTER_ID)
	{
		this->prKind = Epp2Printer;
	}
	else if(UsbManager::getInstance()->getProductId(node_id) == HOBART_ESS_PRINTER_ID)
	{
		this->prKind = EssPrinter;
	}
	else if (UsbManager::getInstance()->getProductId(node_id) == HOBART_ESC_PRINTER_ID)
	{
		this->prKind = EscPrinter;
	}
	else if (UsbManager::getInstance()->getProductId(node_id) == HOBART_XPE_ST_PRINTER_ID)
	{
		this->prKind = Epp3Printer;
	}

	nodeId = node_id;
	myPrinterType = FirstPrinter;

	pConfig = Configuration::getInstance();

	pMyPrinterMgr = NULL;
	myDevicePhysicalAddress = 0;

	xmitting = false;

	sendingBulkData = false;
	pBulkData = NULL;
	bulkDataLength = 0;

	intXferTimeOut = pConfig->getInterruptXferTimeout();
	bulkXferTimeOut = pConfig->getBulkXferTimeout();

	bUSBCommOn = true;

	//Debug::trace("PrinterCommManager", HobString("Create Pipes:"));

	createPipes();
	startPipes();

#if defined ( __linux__ )
    Debug::trace("PrinterCommManager", HobString("PipeHandle =") + Hex::toString(getMyPipeHandle()));
#endif

	printerWakeUp();
	Debug::trace("PrinterCommManager", HobString("PrinterCommManager constructor is done."));
}

PrinterCommManager::~PrinterCommManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
	
	Debug::trace("PrinterCommManager", HobString("~PrinterCommManager(): Entry"));

	UsbRemoveComponent removeMe(nodeId);
	dispatchEvent(&removeMe);

	deregisterAllEvents();
	Debug::trace("PrinterCommManager", "~PrinterCommManager():  deregisterAllEvents()");  

	//shutdown the msg thread
	ShutdownNicely();
	Debug::trace("PrinterCommManager", "~PrinterCommManager():  ShutdownNicely()");  

	if (pMyPrinterMgr != NULL)
	{
		// delete my PrinterManager
		delete pMyPrinterMgr;
		pMyPrinterMgr = NULL;
	}
	Debug::trace("PrinterCommManager", "~PrinterCommManager():  pMyPrinterMgr = NULL");  

	Debug::trace("PrinterCommManager", HobString("~PrinterCommManager():  Exit."));
}

bool PrinterCommManager::isUSBCommEnabled()
{
	return bUSBCommOn;
}
PrinterType PrinterCommManager::getPrinterType()
{
	return myPrinterType;
}

USBD_NODE_ID PrinterCommManager::getNodeID()
{
	return nodeId;
}


void PrinterCommManager::processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg )
{
	/* don't think this is needed anymore --  ats 
		if (voidLabelSwapped == false)
		{
		voidLabelSwapped = true;
		swapBytes(voidLabelBits, sizeof(voidLabelBits) / 2);
		voidLabelSwapped = true;
		}
		*/
	evolHeader.setSourcePhysicalAddress(0x00);
	evolHeader.setSourceLogicalAddress(USBEvolHeader::USER_INTERFACE);

	if (busSuspended == false)
	{
		switch (msg->getDeviceType())
		{
			case Printer	 	:	handlePrinterEvent((PrEvent) msg->getEventType(), msg->Data());
				break;
			case PrinterSystem	:	handlePrinterSysEvent((SysEvent) msg->getEventType(), msg->Data());
				break;
			case System			:	handleSysEvent((SysEvent) msg->getEventType(), msg->Data());
				break;
			default			 	:
				break;
		}
	}
}


void PrinterCommManager::processUsbMessage( std::string & msg, CUsbPipe::EvolPipeType UNUSED(pipe_type) )
{
	// I think messages from Firmware come into here, at least PrinterMessages do
	//Debug::trace("PrinterCommManager", "processUsbMessage() - INCOMING usb messages (along with receivedPrinterMessage())");

	UNREFERENCED_PARAMETER(pipe_type);

	const char * ptr = msg.c_str();

	USBEvolHeader usbEvolHeader(ptr, false);

	UINT16 physicalAddress = usbEvolHeader.getSourcePhysicalAddress();
	UINT16 logicalAddress = usbEvolHeader.getSourceLogicalAddress();

	ptr+= 10;
	if (logicalAddress == USBEvolHeader::SYSTEM)
	{
		receivedPrinterSysMessage((SysGenericMsg *) ptr, physicalAddress); 
	}
	else if (logicalAddress == USBEvolHeader::EVOL_PRINTER)
	{
		receivedPrinterDevMessage((PrGenericMsg *) ptr, physicalAddress, logicalAddress); 
	}
	else
	{
		ptr-= 10;
		Debug::trace("PrinterCommManager.Error", HobString("Bad Header!!!:")  + Hex::toString( ptr, 10, true ));
		ptr += 10; //index past the header and print the message code.
		Debug::trace("PrinterCommManager.Error", HobString("body: ") + Hex::toString( ptr, 2, true ));
	}
}


bool PrinterCommManager::handlePrinterEvent(PrEvent prEventType, const char *buffer)
{
	// These are printer messages that are sent to the Firmware - with the sendUsbInterruptMessage() sending it across the Pipe
	//Debug::trace("PrinterComm.Message.Out", "PrinterCommManager::handlePrinterEvent() START - maybe see PrConfig with prEventType: " + Integer::toString(prEventType) );	
	//Debug::trace("PrinterComm.Message.Out", "PrinterCommManager::handlePrinterEvent() These are the Printer Messages that are sent to the Firmware");

	char str[512];
	DWORD result = OK;
	bool is_custom_code = false;
	bool sendPeripheralMessage = false;

#if defined ( __linux__ )
	memset(str, 0, 512);
#endif

	switch (prEventType)
	{
		case PrConfig:
			{
				Debug::trace("PrinterComm.Message.Out", "PrinterCommManager::handlePrinterEvent() SEND PrConfig (7,1)");			// I DONT SEE THIS	

				PrinterConfiguration prConfig(buffer);
				PrConfigMsg prConfigMsg;

				prConfigMsg.prEvent = PrConfig;
				prConfigMsg.prConfigType = (UINT16)((PrConfigType) htons((u_short)prConfig.getConfigType()));

				prConfig.getDeviceSignature((UINT8 *) &prConfigMsg.prConfiguration.deviceSignature);
				prConfigMsg.prConfiguration.printerType = (UINT8)prConfig.getPrinterType();

				prConfigMsg.prConfiguration.labelWidth = (UINT8)prConfig.getLabelWidth();
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
				prConfigMsg.prConfiguration.configWord13 = htons((UINT16)prConfig.getConfigWord13());
				prConfigMsg.prConfiguration.mediaSensorType = (UINT16)((MediaSensorType) htons((u_short)prConfig.getMediaSensorType()));
				prConfigMsg.prConfiguration.slaEjectLine = htons(prConfig.getSLAEjectLine());
				prConfigMsg.prConfiguration.printPosition = htons(prConfig.getPrintPositionAbsolute());

				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.printerType: ") + HobString(Integer::toString(prConfigMsg.prConfiguration.printerType)));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.getLabelWidth: ") + HobString(Integer::toString(prConfigMsg.prConfiguration.labelWidth)));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.mediaSensorAdjustment: ") + HobString(Integer::toString(prConfigMsg.prConfiguration.mediaSensorAdjustment)));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.outOfMediaCount: ") + HobString(Integer::toString(prConfigMsg.prConfiguration.outOfMediaCount)));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.printContrast: ") + HobString(Integer::toString(prConfigMsg.prConfiguration.printContrast)));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.mediaSensorType: ") + HobString(Integer::toString(prConfigMsg.prConfiguration.mediaSensorType)));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.slaEjectLine: ") + HobString(Integer::toString(prConfigMsg.prConfiguration.slaEjectLine)));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.printPosition: ") + HobString(Integer::toString(prConfigMsg.prConfiguration.printPosition)));

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(prConfigMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &prConfigMsg, sizeof(prConfigMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(prConfigMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrFlush:
			{
				Debug::trace("PrinterComm.Message.Out", "PrFlush");	

				PrGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.prEvent = PrFlush;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrReset:		
			{
				Debug::trace("PrinterComm.Message.Out", "PrReset");	

				PrGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.prEvent = PrReset;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrEnable:		
			{
				Debug::trace("PrinterComm.Message.Out", "PrEnable");	

				PrGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.prEvent = PrEnable;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrDisable:	
			{
				Debug::trace("PrinterComm.Message.Out", "PrDisable");	

				PrGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.prEvent = PrDisable;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrMode:
			{
				Debug::trace("PrinterComm.Message.Out", "PrMode");	

				// Constuct the Evol Message Buffer!!
				PrinterMode prMode(buffer);
				PrModeMsg prModeMsg;

				prModeMsg.prEvent = PrMode;
				prModeMsg.mode = (UINT8)prMode.getPrinterMode();

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(prModeMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &prModeMsg, sizeof(prModeMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(prModeMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrReqPrintheadPower:	
		case PrPrintheadPower:	
			{
				if (prEventType == PrReqPrintheadPower)
				{
					Debug::trace("PrinterComm.Message.Out", "PrReqPrintheadPower");	
					Debug::trace("PrinterManager.PrintheadPower", "PrinterCommManager -- PrReqPrintheadPower");
				}
				else
				{
					Debug::trace("PrinterComm.Message.Out", "PrPrintheadPower");	
					Debug::trace("PrinterManager.PrintheadPower", "PrinterCommManager -- PrPrintheadPower");
				}

				// Constuct the Evol Message Buffer!!
				PrinterRequestPrintheadPower powerMsg(buffer);
				PrPowerMsg prPowerMsg;

				prPowerMsg.prEvent = PrPrintheadPower;
				prPowerMsg.powerState = htons((u_short)powerMsg.getPowerState());
				prPowerMsg.scProtection = htons((u_short)powerMsg.getShortCircuitProtection());

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(prPowerMsg));
				evolHeader.Serialize( str );

				Debug::trace("PrinterManager.PrintheadPower", HobString("prPowerMsg.powerState: ") +
						HobString(Integer::toString(prPowerMsg.powerState)));
				Debug::trace("PrinterManager.PrintheadPower", HobString("prPowerMsg.scProtection: ") +
						HobString(Integer::toString(prPowerMsg.scProtection)));

				memcpy( &str[10], (char *) &prPowerMsg, sizeof(prPowerMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(prPowerMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrCommand:	
			{
				// Constuct the Evol Message Buffer!!
				PrinterCommand printerCommand(buffer);
				switch ( printerCommand.getPrinterCommand() )
				{
					case AdvanceCommand:
						Debug::trace("PrinterComm.Message.Out", "PrCommand: AdvanceCommand");
						break;
					case PrintCommand:
						Debug::trace("PrinterComm.Message.Out", "PrCommand: PrintCommand");
						break;
					case EjectCommand:
						Debug::trace("PrinterComm.Message.Out", "PrCommand: EjectCommand");
						break;
					case SyncCommand:
						Debug::trace("PrinterComm.Message.Out", "PrCommand: SyncCommand");
						break;
					case FindCommand:
						Debug::trace("PrinterComm.Message.Out", "PrCommand: FindCommand");
						break;
					case SizeCommand:
						Debug::trace("PrinterComm.Message.Out", "PrCommand: SizeCommand");
						break;
					default:
						Debug::trace("PrinterComm.Message.Out", "PrCommand: UNTOUGHT COMMAND !!??");
						break;
				}

				PrCommandMsg prCommandMsg;

				prCommandMsg.prEvent = PrCommand;
				prCommandMsg.printerCommandType = (UINT8) printerCommand.getPrinterCommand();
				prCommandMsg.printerCommandOptions = (UINT8) printerCommand.getPrinterCommandOptions();
				prCommandMsg.printerIndirection = (UINT16)((PrinterIndirection) htons((u_short)printerCommand.getPrinterIndirection()));
				prCommandMsg.value = htons(printerCommand.getValue());

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(prCommandMsg));
				evolHeader.Serialize( str );

				memcpy(&str[10], (char *) &prCommandMsg, sizeof(prCommandMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(prCommandMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrRam:		
			{
				Debug::trace("PrinterComm.Message.Out", "PrRam");	

				// Constuct the Evol Message Buffer!!
				PrinterRam printerRam(buffer);
				PrRamMsg prRamMsg;

				prRamMsg.prEvent = PrRam;

				for (int i = 0; i < PrNumRamLocations; i++)
				{
					prRamMsg.ram[i].location = (UINT16)((PrinterIndirection) htons((u_short)printerRam.getRamIndirection((UINT16)i)));
					prRamMsg.ram[i].value = htons(printerRam.getRamValue((UINT16)i));
				}

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(prRamMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &prRamMsg, sizeof(prRamMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(prRamMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrTest:
			{
				Debug::trace("PrinterComm.Message.Out", "PrTest");	

				// Constuct the Evol Message Buffer!!
				PrinterTest printerTest(buffer);
				PrTestMsg prTestMsg;

				prTestMsg.prEvent = PrTest;
				prTestMsg.data = printerTest.getData();

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(prTestMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &prTestMsg, sizeof(prTestMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(prTestMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrNomSize:
			{
				Debug::trace("PrinterComm.Message.Out", "PrNomSize");	

				// Constuct the Evol Message Buffer!!
				PrinterNomSize printerSize(buffer);
				PrNomSizeMsg prNomSizeMsg;

				prNomSizeMsg.prEvent = PrNomSize;
				prNomSizeMsg.actualSize = htons(printerSize.getActualSize());
				prNomSizeMsg.measuredSize = htons(printerSize.getMeasuredSize());

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(prNomSizeMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &prNomSizeMsg, sizeof(prNomSizeMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(prNomSizeMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrMask:
			{
				Debug::trace("PrinterComm.Message.Out", "PrMask");	

				// Constuct the Evol Message Buffer!!
				PrinterMask printerMask(buffer);
				PrMaskMsg prMaskMsg;

				prMaskMsg.prEvent = PrMask;
				prMaskMsg.maskInfo.sensorMask = printerMask.getSensorMask();
				prMaskMsg.maskInfo.sensor2Mask = printerMask.getSensor2Mask();
				prMaskMsg.maskInfo.userMask = printerMask.getUserMask();

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(prMaskMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &prMaskMsg, sizeof(prMaskMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(prMaskMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrTeach:	
			{
				Debug::trace("PrinterComm.Message.Out", "PrTeach");	

				// Constuct the Evol Message Buffer!!
				PrinterTeach printerTeach(buffer);
				PrTeachMsg prTeachMsg;

				CommandOperationList *pCOL = printerTeach.getCommandOperationList();

				memset((UINT8 *) &prTeachMsg.operation, (UINT8) 0x00, sizeof(prTeachMsg.operation));

				prTeachMsg.prEvent = PrTeach;
				prTeachMsg.command = (UINT8)printerTeach.getPrinterCommandType();

				prTeachMsg.entries = htons(pCOL->getCommandBlock((CommandOperationMsg *) &(prTeachMsg.operation), true));

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(prTeachMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &prTeachMsg, sizeof(prTeachMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(prTeachMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrReqStatus:	
			{
				Debug::trace("PrinterComm.Message.Out", "PrReqStatus");	

				PrGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.prEvent = PrStatus;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrReqInquire:	
			{
				Debug::trace("PrinterComm.Message.Out", "PrinterCommManager::handlePrinterEvent() case PrReqInquire (7,22) - now where does it go? Firmware?");	

				PrGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.prEvent = PrInquire;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrReqWakeup:
			{
				Debug::trace("PrinterComm.Message.Probe", "PrReqWakeup");	

				PrinterRequestWakeup prReqWakeup(buffer);

				PrGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.prEvent = PrWakeup;

				if (pMyPrinterMgr == NULL)
				{
					evolHeader.setDestinationPhysicalAddress(prReqWakeup.getDefaultHardwareAddress());
				}

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrEnableTakeup:
			{
				Debug::trace("PrinterComm.Message.Out", "PrEnableTakeup");	

				PrGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.prEvent = PrEnableTakeup;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrReqSensors:
			{
				Debug::trace("PrinterComm.Message.Out", "PrReqSensors");	
				Debug::trace("PrinterManager.PrinterTrackSensors", "PrReqSensors");	

				PrGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.prEvent = PrSensors;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrReqCalibrate:
			{
				Debug::trace("PrinterComm.Message.Out", "PrReqCalibrate");	

				// Constuct the Evol Message Buffer!!
				PrinterRequestCalibrate printerRequestCalibrate(buffer);

				PrCalibrateMsg prCalibrateMsg;

				prCalibrateMsg.prEvent = PrCalibrate;
				prCalibrateMsg.printerCalibrateType = (UINT8) printerRequestCalibrate.getPrinterCalibrateType();

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
				evolHeader.setMessageLength(sizeof(prCalibrateMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &prCalibrateMsg, sizeof(prCalibrateMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(prCalibrateMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case PrReqLabelTransfer:	
			{
				Debug::trace("PrinterComm.Message.Out", "PrReqLabelTransfer");	

				PrinterRequestLabelTransfer printerRequestLabelTransfer(buffer);

				UINT16 *pData = (UINT16 *) printerRequestLabelTransfer.getDataPointer();
				UINT16 totalLength = printerRequestLabelTransfer.getTotalDataLength();
				UINT16 actualLength = printerRequestLabelTransfer.getActualDataLength();
				bool sendSetupXferMessage = printerRequestLabelTransfer.isSendSetupXferMessage();
				bool cancelPendingTransfer = printerRequestLabelTransfer.isCancelPendingTransfer();

				if (cancelPendingTransfer == true)
				{
					Debug::trace("PrinterManager.BulkData", "aborting bulk data transfer at this time");

					cancelBulkTransfer();
				}
				else
				{
					if (sendingBulkData == true)
					{
						Debug::trace("PrinterManager.BulkData", "cannot send bulk data at this time");
						Debug::trace("PrinterManager.BulkData", HobString("pBulkData: 0x") +
								HobString(Hex::toString((long long) (pBulkData))));
						Debug::trace("PrinterManager.BulkData", HobString("bulkDataLength: 0x") +
								HobString(UInteger::toString(bulkDataLength)));
						if (pMyPrinterMgr != NULL)
						{
							PrinterLabelTransferFailed ltFailed(this, myPrinterType, totalLength, actualLength, (UINT16 *) pData);

							const char *pltFailed = ltFailed.serialize();
							int mLength = ltFailed.getSerializedLength();

							pMyPrinterMgr->send(pltFailed, mLength);
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

							evolHeader.setDestinationLogicalAddress(USBEvolHeader::COMM);
							evolHeader.setMessageLength(sizeof(requestMessage));
							evolHeader.Serialize( str );

							memcpy( &str[10], (char *) &requestMessage, sizeof(requestMessage));
							sendUsbInterruptMessage( ( const char * ) str,  sizeof(requestMessage) + 10, result, is_custom_code );

						}

						// Set up Label Data Transfer
						pBulkData = pData;
						bulkDataLength = totalLength;

						sendUsbBulkMessage( ( const char * ) pData, totalLength );
					}
				}
			}
			break;
		case PrNewOrder:
			{
				Debug::trace("PrinterComm.Message.Out", "PrNewOrder");

				PrinterNewPrinterType newPrinterType(buffer);

				myPrinterType = newPrinterType.getNewPrinterType();
			}
			break;
		default		:
			Debug::trace("PrinterComm.Error", HobString("Unknown printer message type: 0x") +
					HobString(Hex::toString(prEventType)));
			break;
	}

	return sendPeripheralMessage;
}

bool PrinterCommManager::handlePrinterSysEvent(SysEvent sysEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;
	bool sendPeripheralMessage = false;

	switch (sysEventType)
	{
		case SysReqWakeup:
			{
				Debug::trace("PrinterComm.SystemMessage.Out", "SysReqWakeup");	

				SysGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.function_code = SysWakeup;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case SysConfig:
			{
				Debug::trace("PrinterComm.SystemMessage.Out", "SysConfig");	

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

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
				evolHeader.setMessageLength(sizeof(sysConfigMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &sysConfigMsg, sizeof(sysConfigMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(sysConfigMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case SysReqStatus:
			{
				Debug::trace("PrinterComm.SystemMessage.Out", "SysReqStatus");	

				SysGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.function_code = SysStatus;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case SysReqInquire:
			{
				Debug::trace("PrinterComm.SystemMessage.Out", "SysReqInquire");	

				SysGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.function_code = SysInquire;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;
			}
			break;
		case SysReqFlashStatus:
			{
				Debug::trace("PrinterComm.SystemMessage.Out", "SysReqFlashStatus");	

				SysGenericMsg genMsg;
				memset((char *) &genMsg, 0, sizeof(genMsg));

				genMsg.function_code = SysFlashStatus;

				evolHeader.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
				evolHeader.setMessageLength(sizeof(genMsg));
				evolHeader.Serialize( str );

				memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
				sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

				sendPeripheralMessage = true;

			}
			break;
		default:
			{
				Debug::trace("PrinterComm.Error", HobString("Unknown printer system message type: 0x") +
						HobString(Hex::toString(sysEventType)));
			}
			break;
	}
	return sendPeripheralMessage;
}

bool PrinterCommManager::handleSysEvent(SysEvent sysEventType, const char *buffer)
{
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;
	bool sendPeripheralMessage = false;

	switch (sysEventType)
	{
		case SysReset:{	
						  Debug::trace("PrinterComm.Message", "SysReset");
						  SystemReset sysReset(buffer);

						  if (sysReset.getUsbNodeId() == nodeId)
						  {

							  SysGenericMsg genMsg;
							  memset((char *) &genMsg, 0, sizeof(genMsg));

							  genMsg.function_code = SysReset;

							  evolHeader.setDestinationLogicalAddress(USBEvolHeader::SYSTEM);
							  evolHeader.setMessageLength(sizeof(genMsg));
							  evolHeader.Serialize( str );

							  memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
							  sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );

							  sendPeripheralMessage = true;
						  }
					  }
					  break;
		default				:
					  {
						  Debug::trace("PrinterCommManager.Error", HobString("Unknown system message type: 0x") +
								  HobString(Hex::toString(sysEventType)));
					  }
					  break;
	}

	return sendPeripheralMessage;
}

void PrinterCommManager::receivedPrinterMessage(unsigned char * printerMess)
{
	//Debug::trace("PrinterCommManager", "receivedPrinterMessage() INCOMING Printer messages (along with processUsbMessags())");

	USBEvolHeader usbEvolHeader((char *)printerMess, true);

	UINT16 physicalAddress = usbEvolHeader.getSourcePhysicalAddress();
	UINT16 logicalAddress = usbEvolHeader.getSourceLogicalAddress();

	if (logicalAddress == USBEvolHeader::SYSTEM)
	{
		receivedPrinterSysMessage((SysGenericMsg *) &printerMess[10], physicalAddress);
	}
	else
	{
		receivedPrinterDevMessage((PrGenericMsg *) &printerMess[10], physicalAddress, logicalAddress);
	}

}

void PrinterCommManager::receivedPrinterSysMessage(SysGenericMsg *genMsg, UINT16 UNUSED(peripheralAddress))
{
	UNREFERENCED_PARAMETER(peripheralAddress);

	switch (genMsg->function_code)
	{
		case SysWakeup:
		{	
			Debug::trace("PrinterComm.SystemMessage.In", "SysWakeup");	

			SysWakeupMsg *sysWakeupMsg = (SysWakeupMsg *) genMsg;

			if (pMyPrinterMgr != NULL)
			{
				PrinterSystemWakeup pswu(this, myPrinterType, (SysCfgStatus) sysWakeupMsg->sysCfgStatus);

				const char *pPswuMsg = pswu.serialize();
				int mLength = pswu.getSerializedLength();

				pMyPrinterMgr->send(pPswuMsg, mLength);
			}
		}
		break;
		case SysStatus:
		{
							Debug::trace("PrinterComm.SystemMessage.In", "SysStatus");	

							PrSysStatusMsg *prSysStatus = (PrSysStatusMsg *) genMsg;

							SystemStatusInfo *pSystemStatus = new SystemStatusInfo((ntohs(prSysStatus->system_status.bit_error)),
									(ntohs(prSysStatus->system_status.address_error)),
									(ntohs(prSysStatus->system_status.rom_checksum)),
									(ntohl(prSysStatus->system_status.flags)),
									prSysStatus->system_status.partno,
									(ntohs(prSysStatus->system_status.system_checksum)),
									(ntohl(prSysStatus->system_status.init_flags)));

							if (pMyPrinterMgr != NULL)
							{
								PrinterSystemStatus pss(this,
										myPrinterType,
										prKind,
										pSystemStatus,
										ntohs(prSysStatus->software_major_revision),
										ntohs(prSysStatus->software_minor_revision),
										prSysStatus->hardware_major_revision,
										prSysStatus->hardware_minor_revision);

								pss.show();

								const char *pPssMsg = pss.serialize();
								int mLength = pss.getSerializedLength();

								pMyPrinterMgr->send(pPssMsg, mLength);
							}

							delete pSystemStatus;
						}
						break;
		case SysInquire:{	
							Debug::trace("PrinterComm.SystemMessage.In", "SysInquire");	

							PrSysConfigMsg *sysInquireMsg = (PrSysConfigMsg *) genMsg;

							if (pMyPrinterMgr != NULL)
							{
								PrinterSystemInquire psi(this,
										myPrinterType,
										prKind,
										(SysConfigType) ntohs(sysInquireMsg->configType),
										(UINT8 *) &sysInquireMsg->system_config.device_signature,
										(DeviceAddress) ntohs(sysInquireMsg->system_config.station_address),
										(DeviceAddress) ntohs(sysInquireMsg->system_config.controller_address),
										sysInquireMsg->hardware_major_revision,
										sysInquireMsg->hardware_minor_revision);

								const char *pPsiMsg = psi.serialize();
								int mLength = psi.getSerializedLength();

								pMyPrinterMgr->send(pPsiMsg, mLength);
							}
						}
						break;
		case SysCalibSwitch:
						{
							Debug::trace("PrinterComm.SystemMessage.In", "SysCalibSwitch");	

							SysCalibSwitchMsg *systemCalibMsg = (SysCalibSwitchMsg *) genMsg;

							if (pMyPrinterMgr != NULL)
							{
								PrinterSystemCalibrationSwitch calibSwitch(this);

								if (systemCalibMsg->calib_switch_state > 0)
								{
									calibSwitch.setSystemCalibrationSwitch();
								}
								else
								{
									calibSwitch.clearSystemCalibrationSwitch();
								}

								if (Debug::isTracing("PrinterManager.WeigherSystemCalibrationSwitch") == true)
								{
									calibSwitch.show();
								}

								const char *pPscsMsg = calibSwitch.serialize();
								int mLength = calibSwitch.getSerializedLength();

								pMyPrinterMgr->send(pPscsMsg, mLength);
							}
						}
						break;
		case SysFlashStatus:
						{
							Debug::trace("PrinterComm.SystemMessage.In", "SysFlashStatus");	

							SysFlashStatusMsg *flashStatusMsg = (SysFlashStatusMsg *) genMsg;

							if (pMyPrinterMgr != NULL)
							{
								PrinterFlashStatus flashStat(this, prKind, flashStatusMsg);


								if (Debug::isTracing("PrinterManager.FlashStatus") == true)
								{
									flashStat.show();
								}

								const char *pPfsMsg = flashStat.serialize();
								int mLength = flashStat.getSerializedLength();

								pMyPrinterMgr->send(pPfsMsg, mLength);
							}
						}
						break;
		default:
						Debug::trace("PrinterComm.Error", HobString("Printer Error: Unknown printer system message type: 0x") +
								HobString(Hex::toString(genMsg->function_code)));
						break;
	}
}

void PrinterCommManager::receivedPrinterDevMessage(PrGenericMsg *genMsg, UINT16 peripheralAddress, UINT16 UNUSED(logicalAddress))
{
	//Debug::trace("PrinterCommManager", "receivedPrinterDevMessage() START - FROM receivedPrinterMessage() or processUsbMessage() FROM the Firmware");

	UNREFERENCED_PARAMETER(logicalAddress);

	switch (genMsg->prEvent)
	{
		case PrWakeup:
		{
			Debug::trace("PrinterComm.Message.PrWakeup", "PrWakeup");	

			PrWakeupMsg *prWakeupMsg = (PrWakeupMsg *) genMsg;

			myPrinterType = (PrinterType) prWakeupMsg->printerType;
			printerBufferSize = ntohs(prWakeupMsg->bufferSize);
			printheadSize = ntohs(prWakeupMsg->printheadSize);

			bulkXferTimeOut = (UINT32) (((double) printerBufferSize / (double) printheadSize) * 0.8);

			// Find out if this is first WakeUp Message
			if (pMyPrinterMgr == NULL)
			{
				Debug::trace("PrinterManager.PrinterWakeup", "************************************************");
				Debug::trace("PrinterManager.PrinterWakeup", HobString("printerType: ") +
						Integer::toString((UINT16) myPrinterType));
				Debug::trace("PrinterManager.PrinterWakeup", HobString("labelWidth: ") +
						Integer::toString((UINT16) prWakeupMsg->labelWidth));
				Debug::trace("PrinterManager.PrinterWakeup", HobString("bufferSize: ") +
						Integer::toString(printerBufferSize));
				Debug::trace("PrinterManager.PrinterWakeup", HobString("printheadSize: ") +
						Integer::toString((UINT16) printheadSize));
				
				if (prWakeupMsg->prCfgStatus > 0)
					Debug::trace("PrinterManager.PrinterWakeup", HobString("validConfig: ") +
							Bool::toString(true));
				else
					Debug::trace("PrinterManager.PrinterWakeup", HobString("validConfig: ") +
							Bool::toString(false));
				
				Debug::trace("PrinterManager.PrinterWakeup", HobString("bulkXferTimeOut: ") +
						Integer::toString((UINT16) bulkXferTimeOut));

				// Create a new thread
				pMyPrinterMgr = RealTimeController::getInstance()->NewUSBPrinter(nodeId, this, NULL,
						(PrinterType) prWakeupMsg->printerType,
						(LabelWidth) prWakeupMsg->labelWidth,
						printerBufferSize,
						printheadSize * 8, // wants to be in dots
						peripheralAddress,
						prKind);

				myDevicePhysicalAddress = peripheralAddress;

				evolHeader.setDestinationPhysicalAddress(myDevicePhysicalAddress);

			}
			else
			{
				PrinterWakeup printerWakeup(this, (PrinterType) prWakeupMsg->printerType,
						(LabelWidth) prWakeupMsg->labelWidth,
						printerBufferSize,
						printheadSize * 8, // wants to be in dots
						(PrCfgStatus) prWakeupMsg->prCfgStatus);

				const char *pWuMsg = printerWakeup.serialize();
				int mLength = printerWakeup.getSerializedLength();

				pMyPrinterMgr->send(pWuMsg, mLength);
			}
		} 
		break;
		case PrInquire:
		{	
			Debug::trace("PrinterComm.Message.In", "PrinterCommManager::receivedPrinterDevMessage() case PrInquire (7,2) to UI from FW");	

			PrInquireMsg *inquireMsg = (PrInquireMsg *) genMsg;

			if (pMyPrinterMgr != NULL)
			{
				PrinterInquire prInquire(this, myPrinterType); // use default constructor

				prInquire.setConfigType((PrConfigType) ntohs(inquireMsg->prConfigType));
				prInquire.setDeviceSignature(inquireMsg->prConfiguration.deviceSignature);
				prInquire.setPrinterType((PrinterType) inquireMsg->prConfiguration.printerType);
				prInquire.setLabelWidth((LabelWidth) inquireMsg->prConfiguration.labelWidth);
				prInquire.setMediaSensorAdjustment(inquireMsg->prConfiguration.mediaSensorAdjustment);
				prInquire.setOutOfMediaCount(inquireMsg->prConfiguration.outOfMediaCount);				// Immutable!! Set to 200 on Ultima
				prInquire.setPrintContrast(ntohs(inquireMsg->prConfiguration.printContrast));			// "Print Contrast" on Ultima Configuration.
				prInquire.setConfigWord0(ntohs(inquireMsg->prConfiguration.configWord0));				// Used on "European" labels
				prInquire.setConfigWord1(ntohs(inquireMsg->prConfiguration.configWord1));				// Used on "European" labels
				prInquire.setConfigWord2(ntohs(inquireMsg->prConfiguration.configWord2));				// Used on "European" labels
				prInquire.setConfigWord3(ntohs(inquireMsg->prConfiguration.configWord3));				// "Expel Position" on Ultima Configuration.
				prInquire.setConfigWord4(ntohs(inquireMsg->prConfiguration.configWord4));				// Used on "European" labels
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

				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.printerType: ")				+ HobString(Integer::toString(prInquire.getPrinterType() )));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.getLabelWidth: ")			+ HobString(Integer::toString(prInquire.getLabelWidth() )));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.mediaSensorAdjustment: ")	+ HobString(Integer::toString(prInquire.getMediaSensorAdjustment() )));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.outOfMediaCount: ")			+ HobString(Integer::toString(prInquire.getOutOfMediaCount() )));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.printContrast: ")			+ HobString(Integer::toString(prInquire.getPrintContrast() )));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.mediaSensorType: ")			+ HobString(Integer::toString(prInquire.getMediaSensorType() )));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.slaEjectLine: ")				+ HobString(Integer::toString(prInquire.getSLAEjectLine() )));
				//Debug::trace("PrinterCommManager.PrConfig", HobString("PrinterCommManager::handlePrinterEvent PrConfig.printPosition: ")			+ HobString(Integer::toString(prInquire.getPrintPositionAbsolute() )));

				const char *pWiMsg = prInquire.serialize();
				int mLength = prInquire.getSerializedLength();

				pMyPrinterMgr->send(pWiMsg, mLength);
			}
		}
		break;

		case PrStatus:
		{
			Debug::trace("PrinterComm.Message.In", "PrStatus");	

			PrStatusMsg *statusMsg = (PrStatusMsg *) genMsg;

			if (pMyPrinterMgr != NULL)
			{
				PrinterStatus ps(this, myPrinterType); // use default constructor

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

				pMyPrinterMgr->send(pPsMsg, mLength);
			}
		}
		break;

		case PrSensors:
		{	
			Debug::trace("PrinterComm.Message.In", "PrSensors");	
			Debug::trace("PrinterManager.PrinterTrackSensors", "PrSensors");	

			if (pMyPrinterMgr != NULL)
			{
				PrSensorsMsg *prSensorMsg = (PrSensorsMsg *) genMsg;

				// Create a PrSensorMsg and send it to the printer manager!
				PrinterSensors printerSensors(this, myPrinterType); // use default constructor

				UINT32 headTemp = (UINT32) (-0.490798 * (double) ntohs(prSensorMsg->headTemperatureReading) + 114.355828);

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
				printerSensors.setHeadTemperature((UINT16)headTemp);
                if(prKind == EppPrinter)
                {
                	printerSensors.setHeadVoltageCounts(ntohs(prSensorMsg->headVoltageReading), EppPrinter);
                }
                else
                {
                	//EPP2 printer has same scale factor as the ApcPrinter!!
                	printerSensors.setHeadVoltageCounts(ntohs(prSensorMsg->headVoltageReading), ApcPrinter);
                }
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

				pMyPrinterMgr->send(pWsMsg, mLength);

			}
		} 
		break;
		case VpShutdown:
		{
			Debug::trace("PrinterComm.Message.In", "VpShutdown");	
			break;
		}
		case VpIOCfg:
		{
			Debug::trace("PrinterComm.Message", "VpIOCfg");	
			break;
		}
		default:		
		Debug::trace("PrinterComm.Error", HobString("Printer Error: Unknown printer message type: 0x") +
				HobString(Hex::toString(genMsg->prEvent)));
		break;
	}
}



void PrinterCommManager::printerWakeUp()
{
	Debug::trace("PrinterCommManager", "printerWakeUp()");
	char str[255];
	DWORD result = OK;
	bool is_custom_code = false;

	PrGenericMsg genMsg;
	memset((char *) &genMsg, 0, sizeof(genMsg));

	genMsg.prEvent = PrWakeup;
	evolHeader.setSourcePhysicalAddress(0x0000);
	evolHeader.setSourceLogicalAddress(0x0009);
	evolHeader.setDestinationPhysicalAddress(0x4646);
	evolHeader.setDestinationLogicalAddress(USBEvolHeader::EVOL_PRINTER);
	evolHeader.setMessageLength(sizeof(genMsg));
	evolHeader.Serialize( str );

	memcpy( &str[10], (char *) &genMsg, sizeof(genMsg));
	sendUsbInterruptMessage( ( const char * ) str,  sizeof(genMsg) + 10, result, is_custom_code );
}

void PrinterCommManager::sendBulkMessageCallBack( DWORD result, bool user_defined_error )
{
	Debug::trace("PrinterCommManager.UsbIrp", "sendBulkDataCallback");	

	UINT16 *pOldBuldData = pBulkData;

	sendingBulkData = false;


	if ( result == CUsbPipe::TimeOut && user_defined_error )
	{
		Debug::trace("PrinterCommManager.BulkData", "Bulk Transfer ERROR: S_usbHcdLib_TIMEOUT");
		/*Debug::trace("APCCommManager.BulkData", HobString("Needed Transfered: ") +
		  HobString(Integer::toString(pESCCommManager->irpBulkOut_.bfrList [0].bfrLen)));;
		  Debug::trace("APCCommManager.BulkData", HobString("Actual Transfered: ") +
		  HobString(Integer::toString(pESCCommManager->irpBulkOut_.bfrList [0].actLen)));;*/
	}

	if (result == OK)
	{
		Debug::trace("PrinterCommManager.UsbIrp", "transfer OK");	

		if ( pMyPrinterMgr != NULL)
		{

			PrinterLabelTransferComplete ltComplete(this,
					myPrinterType,
					bulkDataLength,
					bulkDataLength,
					pOldBuldData);

			pBulkData = NULL;
			bulkDataLength = 0;

			const char *pltComplete = ltComplete.serialize();
			int mLength = ltComplete.getSerializedLength();

			pMyPrinterMgr->send(pltComplete, mLength);
		}
	}
	else
	{
		Debug::trace("PrinterCommManager.Error", "sendBulkDataCallback Printer Irp Not Successful");
		UsbManager::getInstance()->printUSBError(result);



		if (pMyPrinterMgr != NULL)
		{
			PrinterLabelTransferFailed ltFailed(this,
					myPrinterType,
					bulkDataLength, //lcg rev 0.2pESCCommManager->irpBulkOut_.bfrList [0].bfrLen,
					0, //lcg rev 0.1pESCCommManager->irpBulkOut_.bfrList [0].actLen,
					pOldBuldData);

			const char *pltFailed = ltFailed.serialize();
			int mLength = ltFailed.getSerializedLength();

			pMyPrinterMgr->send(pltFailed, mLength);
		}
		pBulkData = NULL;
		bulkDataLength = 0;
	}
}

void PrinterCommManager::receiveErrorCallBack( CUsbPipe::EvolPipeType pipe_type, DWORD error, bool user_defined_error )
{
	printUSBError( error, user_defined_error );

	Debug::trace("PrinterCommManager.Error", std::string("***receiveErrorCallBack***::error = ") + Integer::toString(error) +
			std::string("pipe type: ") + Integer::toString(pipe_type));
}
