#if 1
#define DEBUG_ME
#endif

#include "Debug.h"
#include "Hex.h"
#include "Integer.h"
#include "HobString.h"
#include "evolution.h"
#include "SUsbDescriptors.h"
#include "UsbNode.h"

#include "device_event.h"

#include <libusb-1.0/libusb.h>

#include "linux/usb_linux_node.h"

CUsbLinuxNode::CUsbLinuxNode()
{
	dev_config = NULL;
}

CUsbLinuxNode::~CUsbLinuxNode()
{
	if ( dev_config )
	{
		delete [] dev_config;
		dev_config = NULL;
	}
}

USBD_NODE_ID CUsbLinuxNode::Initialize(void *vDevice)
{
	int lResult;

    USBD_NODE_ID nodeId( NULL );
    libusb_device *device( static_cast<libusb_device *>( vDevice ) );

    if ( 0 != (lResult = libusb_get_device_descriptor( device, &desc.dev_desc )) )
		Debug::trace("CUsbLinuxNode.Error", "CUsbLinuxNode::Initialize():  libusb_get_device_descriptor returned " + Integer::toString(lResult));
    else
    {
        // clear the pUpdate pointer
        pUpdater = 0;

        desc.dev = device;
        char device_id[64];

		memset((void *)&device_id, 0, size_t(sizeof(device_id)));

        idVendor = desc.dev_desc.idVendor;
        idProduct = desc.dev_desc.idProduct;
        bcdDevice = desc.dev_desc.bcdDevice;

        sprintf(device_id, "%s%03d/%03d", 
                "/dev/bus/usb/",
                libusb_get_bus_number(device),
                libusb_get_device_address(device));

		bus = libusb_get_bus_number(device);
		address = libusb_get_device_address(device);

		Debug::trace("CUsbLinuxNode", HobString("CUsbLinuxNode::Initialize(): device_id = ") + HobString(device_id));

		strPath = (const char *)device_id;

        Debug::trace("CUsbLinuxNode", 
                HobString("Vendor:") + Hex::toString((ULONG)idVendor) + 
                HobString(" Product:") + Hex::toString((ULONG)idProduct) + 
                HobString(" bcdDevice:") + Hex::toString((ULONG)bcdDevice) ); 
        nodeId = createNodeId(device_id);

        bServiceObject = false;

        if ( !desc.dev_desc.bNumConfigurations )
        {
    		Debug::trace("CUsbLinuxNode", HobString("CUsbLinuxNode::Initialize(): there are NO configurations for this device!"));
    		desc.dev_config = NULL;

			bInterfaceClass = bInterfaceProtocol = biInterface = 0;
        }
        else
        {
    		Debug::trace("CUsbLinuxNode", HobString("CUsbLinuxNode::Initialize(): Num configs: " + Integer::toString(desc.dev_desc.bNumConfigurations)));
    		desc.dev_config = dev_config = new libusb_config_descriptor[desc.dev_desc.bNumConfigurations];

			for ( int i = 0; i < desc.dev_desc.bNumConfigurations; ++i )
			{
				if ( 0 != (lResult = libusb_get_config_descriptor( device, i, &desc.dev_config )) )
					Debug::trace("CUsbLinuxNode.Error", "CUsbLinuxNode::Initialize():  libusb_get_config_descriptor returned " + Integer::toString(lResult));
				else
				{
					Debug::trace("CUsbLinuxNode", "CUsbLinuxNode::Initialize():  Successful libusb_get_config_descriptor");
					Debug::trace("CUsbLinuxNode", "CUsbLinuxNode::Initialize():  Num interfaces: " + Integer::toString(desc.dev_config->bNumInterfaces));

					if ( desc.dev_config->bNumInterfaces )
					{
						bInterfaceClass = desc.dev_config->interface[0].altsetting[0].bInterfaceClass;
						bInterfaceProtocol = desc.dev_config->interface[0].altsetting[0].bInterfaceProtocol;
						biInterface = desc.dev_config->interface[0].altsetting[0].iInterface;
					}
				}
			}

			libusb_ref_device( device );
        }
    }

    return nodeId;
}


bool CUsbLinuxNode::isValid() const
{
	if ( !desc.dev_config )
		return false;
	else if ( !desc.dev_config->bNumInterfaces )
		return false;

	return true;
}

