#include "local.h"

#include "Descriptor.h"
#include "StringDesc0.h"
#include "StringDescUnicode.h"
#include "StringDescArray.h"
#include "EndpointDesc.h"
#include "InterfaceDesc.h"
#include "ConfigDesc.h"
#include "DeviceDesc.h"

#include "../include/sll_list.h"
#include "../include/event.h"
#include "../include/timer.h"
#include "../include/queue.h"
#include "../include/library.h"
#include "../include/comm.h"
#include "../include/system.h"

static DeviceDescriptor *dd; 
static StringDescriptorArray *sda;

void USBDP_InitDescriptors(void)
{
	ConfigurationDescriptor *cd = NULL;
	InterfaceDescriptor *id = NULL;
	EndpointDescriptor *ed = NULL;
 

	usb_word hobartFEG[] = {'H', 'O', 'B', 'A', 'R', 'T', ' ', 'F', 'E', 'G'};
	usb_word EvoPrinterBoard[] = {'A','c','c','e','s','s',' ','W','e','i','g','h','e','r'};
 usb_word serialNum[] = {'/0'};
	usb_word configName[] = {'C', 'o', 'n', 'f', 'i', 'g', ' ', 'N', 'u', 'm', 'b', 'e', 'r', ' ', '0'};
	usb_word interfaceName[] = {'I', 'n', 't', 'e', 'r', 'f', 'a', 'c', 'e', ' ', 'N', 'a', 'm', 'e'};

	/* We want:										*/
	/*   Device Descriptor -						                */
	/*				  EP0 = 8 byte packets			                */
	/*          1 - Configuration					                        */
	/*                configValue = 1				                        */
	/*			1 - Interface						        */
	/*          2 - Endpoints						                */
	/*             1 - IN							                */
	/*                    64 byte packets			                                */
	/*                    Interrupt					                        */
	/*                    10 ms polling interval	                                        */
	/*             2 - OUT							                */
	/*                    64 byte packets			                                */
	/*                    Interrupt					                        */
	/*                    10 ms polling interval	                                        */

	/* get a strind descriptor array */
	sda = SDA_new();

	/* Only US English is supported */
	SDA_addLanguageID(sda, USB_ENGLISH_US);

	/* create a device descriptor */
	dd = DD_new();

	/* set the values */
	/* USB Spec Version defaults to 1.1 */
	/* set class, subclass and protocol to 0-- let the interface do it */
	DD_setDeviceClass(dd, 0x00);
	DD_setDeviceSubclass(dd, 0x00);
	DD_setDeviceProtocol(dd, 0x00);

	/* EP0 size is 8 bytes on the USBN9603 */
	DD_setMaxPacketSize(dd, 8);

	/* set Vendor specific items */
	DD_setVendorID(dd, HOBART_VENDOR_ID);

 /* Product ID */
 
 DD_setProductID(dd, ACCESS_WEIGHER_PID);
 

	DD_setDeviceReleaseNum(dd, (usb_word)GetHardwareRevision());

	/* set String Descriptor IDs */
	DD_setManufacturerName(dd, sda, hobartFEG, sizeof(hobartFEG) / 2);
	DD_setProductName(dd, sda, EvoPrinterBoard, sizeof(EvoPrinterBoard) / 2);
	DD_setSerialNumber(dd, sda, serialNum, sizeof(serialNum) / 2);

	/* numConfigs is defaulted to one because configuration descriptor is created when a */
	/* device descriptor is created. So, get the configuration descriptor */
	cd = DD_getConfiguration(dd, 1);

	CD_setConfigName(cd, sda, configName, sizeof(configName) / 2);
	CD_setSelfPowered(cd);
	CD_setRemoteWakeup(cd);
	CD_setMaxPower(cd, 100); 

	/* A default interface (0) is created when a configuration is generated. */
	/* set up the interface specific stuff */
	id = CD_getInterface(cd, 0);
	ID_setInterfaceName(id, sda, interfaceName, sizeof(interfaceName) / 2);
	ID_setInterfaceClass(id, USB_VENDOR_SPECIFIC);
	ID_setInterfaceSubclass(id, USB_VENDOR_SPECIFIC);
	ID_setInterfaceProtocol(id, USB_VENDOR_SPECIFIC);

	/* create the endpoint descriptor for ep1 IN = "Low Speed IN Pipe" */
	/* use the Config Desc. so that the total config length is correct! */
	ed = CD_addInEndpoint(cd, 0, EP1_ADDR);

	/* set up the IN endpoint specific stuff */
	ED_setMaxPacketSize(ed, 64);
	ED_setInterrupt(ed);
	ED_setPollingInterval(ed, 10);
	
	/* create the endpoint descriptor for ep2 OUT = "Low Speed OUT Pipe" */
	/* use the Config Desc. so that the total config length is correct! */
	ed = CD_addOutEndpoint(cd, 0, EP3_ADDR);

	/* set up the OUT endpoint specific stuff */
	ED_setMaxPacketSize(ed, 64);
	ED_setInterrupt(ed);
	ED_setPollingInterval(ed, 10);

	/* create the endpoint descriptor for ep4 OUT */
	/* use the Config Desc. so that the total config length is correct! */
	ed = CD_addOutEndpoint(cd, 0, EP2_ADDR);

	/* set up the OUT endpoint specific stuff */
	ED_setMaxPacketSize(ed, 64);
	ED_setBulk(ed);
		
}

/* Use this function with an already allocated binStr pointer */
usb_int USBDP_GetStringDescriptor(usb_int index, usb_int langID, usb_binaryString *binStr)
{
	usb_int rv = -1;

	if (index == 0)
	{
		rv = (usb_int) SDA_getBinaryStringSD0(sda, binStr);
	}
	else
	{
		if (SDA_findLanguageID(sda, langID) >= 0)
		{
			rv = (usb_int) SDA_getBinaryStringSDU(sda, index, binStr);
		}
	}
	
	return rv;
}

/* Use this function with an already allocated binStr pointer */
usb_int USBDP_GetDeviceDescriptor(usb_binaryString *binStr)
{
	return (usb_int) DD_getBinaryString(dd, binStr);
}

/* Use this function with an already allocated binStr pointer */
/* returns the entire configuration with Interfaces and endpoints */
usb_int USBDP_GetConfigurationDescriptor(usb_byte configNum, usb_binaryString *binStr)
{
	usb_int rv = -1;

	/* numConfigs is defaulted to one because onfiguration descriptor is created when a */
	/* device descriptor is created. So, get the configuration descriptor */

	ConfigurationDescriptor *cd = DD_getConfiguration(dd, configNum);

	if (cd !=NULL)
	{
		rv = (usb_int) CD_getTotalBinaryString(cd, binStr);
	}

	return rv;
}

void USBDP_DeleteDescriptors(DeviceDescriptor *dd, StringDescriptorArray *sda)
{
	/* deallocate the storage */
	DD_delete(dd, sda);
	SDA_delete(sda);
}
