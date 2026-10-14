#include "vxWorks.h"

#include "iostream"
using namespace std;

//#include "string.h"
#include "taskLib.h"
#include "usbPlatform.h"
#include "usbdLib.h"
#include "usbLib.h"
//#include "drv/usb/usbUhci.h"			/* UHCI definitions */
//#include "drv/usb/usbHcdUhciLib.h"		/* UHCI HCD entry point, etc. */
#include "usbPciLib.h"
#include "usbHid.h"


static USBD_CLIENT_HANDLE usbdLSMNHandle;
static UINT8 descBuffer[USB_MAX_DESCR_LEN];
static UINT16 descLength;

static USBD_PIPE_HANDLE inPipeHandle;
static USBD_PIPE_HANDLE outPipeHandle;
static USB_IRP irpIN;
static USB_IRP irpOUT;
static UINT8 bfrIN[128];
static UINT8 bfrOUT[128];
static UINT8 ultimaMessage[64];
static INT16 ultimaMessLen;

static void recvDataIrp(UINT dataLength);
static void recvCommHeaderIrp(void);

static void sendDataIrp(UINT dataLength);
static void sendCommHeaderIrp(void);

static pUSB_DEVICE_DESCR GetDeviceDescriptor(USBD_NODE_ID nodeId)
{
	pUSB_DEVICE_DESCR pDD = NULL;

	if (usbdDescriptorGet(	usbdLSMNHandle,
							nodeId,
							USB_RT_STANDARD | USB_RT_DEVICE,
							USB_DESCR_DEVICE,
							0,
							0,
							sizeof(descBuffer),
							descBuffer,
							&descLength) == OK)
	{
		pDD = (pUSB_DEVICE_DESCR) usbDescrParse(descBuffer, descLength, USB_DESCR_DEVICE);
	}

	return pDD;
}

static void receivedUltimaMessage(void)
{
	cout << "Received Ultima Message: ";

	if (ultimaMessage[19] == 0x00)
	{
		cout << "Weigher WakeUp!" << endl;
		sendCommHeaderIrp();
	}
	else if (ultimaMessage[19] == 0x04)
	{
		cout << "Weigher Status!" << endl;
	}

	for (int i = 0; i < ultimaMessLen; i++)
	{
		cout << (UINT16) ultimaMessage[i] << " ";
	}

	cout << endl;
}

static void recvCommHeaderCallback(pVOID p)
{
	pUSB_IRP pIrp = (pUSB_IRP) p;

	if (pIrp->result == OK)
	{
		ultimaMessLen = 0;
		memcpy(ultimaMessage, bfrIN, pIrp->bfrList [0].actLen);
		ultimaMessLen += pIrp->bfrList [0].actLen;
	
		recvDataIrp(bfrIN[16]);
	}
	else
	{
		cout << "recvCommHeaderCallback Irp Not Successfull!!" << endl;
	}
}

static void recvDataCallback(pVOID p)
{
	pUSB_IRP pIrp = (pUSB_IRP) p;

	if (pIrp->result == OK)
	{
		memcpy(&ultimaMessage[ultimaMessLen], bfrIN, pIrp->bfrList [0].actLen);
		ultimaMessLen += pIrp->bfrList [0].actLen;

		receivedUltimaMessage();
		recvCommHeaderIrp();
	}
	else
	{
		cout << "recvDataCallback Irp Not Successfull!!" << endl;
	}
}

static void recvCommHeaderIrp(void)
{
	memset (&irpIN, 0, sizeof (irpIN));

	irpIN.irpLen = sizeof (irpIN);
	irpIN.userCallback = recvCommHeaderCallback;
	irpIN.transferLen = 64;
	irpIN.bfrCount = 1;
	irpIN.timeout = USB_TIMEOUT_NONE;
	irpIN.bfrList [0].pid = USB_PID_IN;
	irpIN.bfrList [0].pBfr = bfrIN;
	irpIN.bfrList [0].bfrLen = 64;

	if (usbdTransfer (usbdLSMNHandle, inPipeHandle, &irpIN) != OK)
	{
	    cout << "recvCommHeaderIrp usbdTransfer() failed" << endl;
	}											
}

static void recvDataIrp(UINT dataLength)
{
	memset (&irpIN, 0, sizeof (irpIN));

	irpIN.irpLen = sizeof (irpIN);
	irpIN.userCallback = recvDataCallback;
	irpIN.transferLen = 64;
	irpIN.bfrCount = 1;
	irpIN.timeout = USB_TIMEOUT_NONE;
	irpIN.bfrList [0].pid = USB_PID_IN;
	irpIN.bfrList [0].pBfr = bfrIN;
	irpIN.bfrList [0].bfrLen = 64;

	if (usbdTransfer (usbdLSMNHandle, inPipeHandle, &irpIN) != OK)
	{
	    cout << "recvDataIrp usbdTransfer() failed" << endl;
	}											
}

static void sendCommHeaderCallback(pVOID p)
{
	pUSB_IRP pIrp = (pUSB_IRP) p;

	if (pIrp->result == OK)
	{
		sendDataIrp(4);
	}
	else
	{
		cout << "sendCommHeaderCallback Irp Not Successfull!!" << endl;
	}
}

static void sendDataCallback(pVOID p)
{
	pUSB_IRP pIrp = (pUSB_IRP) p;

	if (pIrp->result != OK)
	{
		cout << "sendDataCallback Irp Not Successfull!!" << endl;
	}
}

static void sendCommHeaderIrp(void)
{
	memset (&irpOUT, 0, sizeof (irpOUT));

	bfrOUT[0] = 0x00;
	bfrOUT[1] = 0x00;
	bfrOUT[2] = 0x00;
	bfrOUT[3] = 0x00;
	bfrOUT[4] = 0x06;
	bfrOUT[5] = 0x55;
	bfrOUT[6] = 0x55;
	bfrOUT[7] = 0x00;
	bfrOUT[8] = 0x06;
	bfrOUT[9] = 0x00;
	bfrOUT[10] = 0x06;
	bfrOUT[11] = 0x09;
	bfrOUT[12] = 0x07;
	bfrOUT[13] = 0x03;
	bfrOUT[14] = 0x00;
	bfrOUT[15] = 0x00;
	bfrOUT[16] = 0x04;
	bfrOUT[17] = 0x00;
	bfrOUT[18] = 0x06;

	irpOUT.irpLen = sizeof (irpOUT);
	irpOUT.userCallback = sendCommHeaderCallback;
	irpOUT.transferLen = 19;
	irpOUT.bfrCount = 1;
	irpOUT.timeout = USB_TIMEOUT_NONE;
	irpOUT.bfrList [0].pid = USB_PID_OUT;
	irpOUT.bfrList [0].pBfr = bfrOUT;
	irpOUT.bfrList [0].bfrLen = 19;

	if (usbdTransfer (usbdLSMNHandle, outPipeHandle, &irpOUT) != OK)
	{
	    cout << "sendCommHeaderIrp usbdTransfer() failed" << endl;
	}											
}

static void sendDataIrp(UINT dataLength)
{
	memset (&irpOUT, 0, sizeof (irpOUT));

	bfrOUT[0] = 0x07;
	bfrOUT[1] = 0x47;
	bfrOUT[2] = 0x01;
	bfrOUT[3] = 0x00;

	irpOUT.irpLen = sizeof (irpOUT);
	irpOUT.userCallback = sendDataCallback;
	irpOUT.transferLen = 4;
	irpOUT.bfrCount = 1;
	irpOUT.timeout = USB_TIMEOUT_NONE;
	irpOUT.bfrList [0].pid = USB_PID_OUT;
	irpOUT.bfrList [0].pBfr = bfrOUT;
	irpOUT.bfrList [0].bfrLen = 4;

	if (usbdTransfer (usbdLSMNHandle, outPipeHandle, &irpOUT) != OK)
	{
	    cout << "sendDataIrp usbdTransfer() failed" << endl;
	}											
}

static void attachCallback(USBD_NODE_ID nodeId, UINT16 attachAction, UINT16 configuration, UINT16 interface, UINT16 deviceClass,
						   UINT16 deviceSubclass, UINT16 deviceProtocol)
{
	// Depending on the attachAction, add or remove a device.
	pUSB_DEVICE_DESCR pDD = NULL;

	switch (attachAction)
	{
		case USBD_DYNA_ATTACH:	if ((pDD = GetDeviceDescriptor(nodeId)) != NULL)
								{
									if ((pDD->vendor == 0xfffe) && (pDD->product == 0x0001))
									{
										cout << "Hobart LSMN/USB Attached!" << endl;

										cout << "device descriptor!" << endl;
										cout << (UINT) pDD->length << endl;
										cout << (UINT) pDD->descriptorType << endl;
										cout << pDD->bcdUsb << endl;
										cout << (UINT) pDD->deviceClass << endl;
										cout << (UINT) pDD->deviceSubClass << endl;
										cout << (UINT) pDD->deviceProtocol << endl;
										cout << (UINT) pDD->maxPacketSize0 << endl;
										cout << pDD->vendor << endl;
										cout << pDD->product << endl;
										cout << pDD->bcdDevice << endl;
										cout << (UINT) pDD->manufacturerIndex << endl;
										cout << (UINT) pDD->productIndex << endl;
										cout << (UINT) pDD->serialNumberIndex << endl;
										cout << (UINT) pDD->numConfigurations << endl;

										usbdConfigurationSet(usbdLSMNHandle, nodeId, 1, 0);

										if (usbdPipeCreate(	usbdLSMNHandle,
															nodeId,
															0x02,
															1,
															0,
															USB_XFRTYPE_INTERRUPT,
															USB_DIR_OUT,
															64,
															64,
															10,
															&outPipeHandle) == OK)
										{
											cout << "OUT Pipe Created!" << endl;

											if (usbdPipeCreate(	usbdLSMNHandle,
																nodeId,
																0x01,
																1,
																0,
																USB_XFRTYPE_INTERRUPT,
																USB_DIR_IN,
																64,
																64,
																10,
																&inPipeHandle) == OK)
											{
												cout << "IN Pipe Created!" << endl;
												recvCommHeaderIrp();
											}
											else
											{
												cout << "IN Pipe Creation Failed!" << endl;
											}
										}
										else
										{
											cout << "OUT Pipe Creation Failed!" << endl;
										}
									}
									else
									{
										cout << "Not an Evolution device Attached" << endl;
									}
								}
								break;
		case USBD_DYNA_REMOVE:	cout << "Device Detached! Node ID: " << nodeId << endl;
								break;
	}
}


#include "RealTimeController.h"
#include "DispatchEvent.h"
#include "EventManager.h"
#include "EvolMessage.h"
#include "Printer.h"
#include "OperationModeManualWeigh.h"
#include "OperationModeSemiWeigh.h"

#include "LabelType.h"
#include "Product.h"
#include "ProductChangeMsg.h"

int usbDriver(void)
{
	// Initialize the USBD.

	cout.flags(ios::showbase | ios::hex);

	cout << "Before RTC created!" << endl;

	RealTimeController RTC;

	cout << "After RTC created!" << endl;

	// When instantiating printers in the real world, initially they will all
	// be Primary Printers. The second printer will have to try to be secondary.
	// The third will try to be secondary and then tercerary. We allow no more than
	// three printers in a system.
//	RTC.NewUSBPrinter((USBD_NODE_ID) 0x64, (HobThread *) NULL, FirstPrinter);
//	RTC.NewUSBPrinter((USBD_NODE_ID) 0x64, (HobThread *) NULL, SecondPrinter);

//	EventManager *pEventMgr = EventManager::getInstance();

	// Change mode to SemiWeighMode
//	OperationModeSemiWeigh e;

/*	taskDelay(3000);

	OperationModeManualWeigh e;

	DispatchEvent de((EvolMessage *) &e, pEventMgr);

	const char *pDisMsg = de.serialize();
	int mLength = de.getSerializedLength();
 
	pEventMgr->send(pDisMsg, mLength);

	taskDelay(500);

	LabelType *pPrimaryLabel = new LabelType(0x01D6, NarrowLabel, PortraitLabel, 103);
	LabelType *pSecondaryLabel = new LabelType(0x01D6, NarrowLabel, PortraitLabel, 102);

	Product *pProduct = new Product();

	pProduct->setLabelType1(pPrimaryLabel);
	pProduct->setLabelRotation1(90);
	pProduct->setLabelType2(pSecondaryLabel);
	pProduct->setLabelRotation2(180);

	// Let Mode know PLU is ready
	ProductChangeMsg productChange;
	productChange.setProduct((const Product *) pProduct);

	DispatchEvent de1(&productChange, pEventMgr);

	pDisMsg = de1.serialize();
	mLength = de1.getSerializedLength();

	pEventMgr->send(pDisMsg, mLength);
*/
	int item = -1;

	while (item < 0)
	{
		taskDelay(2000);

/*		if (item == -1)
		{
			pProduct->setLabelType1(pPrimaryLabel);
			pProduct->setLabelRotation1(90);
			pProduct->setLabelType2(NULL);
			pProduct->setLabelRotation2(0);

			DispatchEvent de1(&productChange, pEventMgr);

			pDisMsg = de1.serialize();
			mLength = de1.getSerializedLength();

			pEventMgr->send(pDisMsg, mLength);
		}
		else if (item == -2)
		{
			pProduct->setLabelType1(NULL);
			pProduct->setLabelRotation1(0);
			pProduct->setLabelType2(pSecondaryLabel);
			pProduct->setLabelRotation2(180);

			DispatchEvent de1(&productChange, pEventMgr);

			pDisMsg = de1.serialize();
			mLength = de1.getSerializedLength();

			pEventMgr->send(pDisMsg, mLength);
		} */
		
		item--;
	}

/*	if (usbdInitialize() == OK)
	{
		UINT8 busNo;
		UINT8 deviceNo;
		UINT8 funcNo;
		PCI_CFG_HEADER pciCfgHdr;
		GENERIC_HANDLE uhciAttachToken;

		if (usbPciClassFind(UHCI_CLASS, UHCI_SUBCLASS, UHCI_PGMIF, 0, &busNo, &deviceNo, &funcNo))
		{
			usbPciConfigHeaderGet(busNo, deviceNo, funcNo, &pciCfgHdr);

			if (usbdHcdAttach(usbHcdUhciExec, &pciCfgHdr, &uhciAttachToken) == OK)
			{
				if (usbdClientRegister("LSMN_CONVERTER", &usbdLSMNHandle) == OK)
				{
					if (usbdDynamicAttachRegister(usbdLSMNHandle, USBD_NOTIFY_ALL, USBD_NOTIFY_ALL, USBD_NOTIFY_ALL, attachCallback) == OK)
					{
						int inChar = -1;						
						while (inChar == -1)
						{
							taskDelay(100);
							inChar = cin.get();
						}

						usbdDynamicAttachUnRegister(usbdLSMNHandle, 0xFF, 0xFF, USBD_NOTIFY_ALL, attachCallback);
					}
			 

					usbdClientUnregister(usbdLSMNHandle);
				}

				usbdHcdDetach(uhciAttachToken);
			}
		}

		usbdShutdown();
	}
*/
	cout << "Goodbye!!" << endl;
	
	return 0;
}

