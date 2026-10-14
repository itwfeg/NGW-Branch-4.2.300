#include "PAL_platformheader.h"

#include "UsbNode.h"
#if !defined ( __linux__ )
#include <setupapi.h>
#endif
#include "Debug.h"
#include "Hex.h"
#include "Integer.h"
#include "HobString.h"

#include <algorithm>
#include <string>

CUsbNode::CUsbNode()
{
}


CUsbNode::~CUsbNode()
{
}

#if defined ( __linux__ )

uint8_t CUsbNode::getInterfaceClass()
{
    return bInterfaceClass;
}
uint8_t CUsbNode::getDeviceProtocol()
{
    return bInterfaceProtocol;
}
uint8_t CUsbNode::getiInterface()
{
	return biInterface;
}
void CUsbNode::show()
{
	Debug::trace("CUsbNode", "device descriptor: ");
	Debug::trace("CUsbNode", "device class: " + Integer::toString(desc.dev_desc.bDeviceClass));
	Debug::trace("CUsbNode", "device protocol: " + Integer::toString(bInterfaceProtocol));
	Debug::trace("CUsbNode", "device subclass: " + Integer::toString(desc.dev_desc.bDeviceSubClass));
	Debug::trace("CUsbNode", "bcd device: " + Integer::toString(desc.dev_desc.bcdDevice));
	Debug::trace("CUsbNode", "bcd USB: " + Integer::toString(desc.dev_desc.bcdUSB));
	Debug::trace("CUsbNode", "vendor id: " + Hex::toString(desc.dev_desc.idVendor));
	Debug::trace("CUsbNode", "product id: " + Hex::toString(desc.dev_desc.idProduct));
}

#endif

// return device PID
USHORT CUsbNode::getProductId()
{
    return idProduct;
}

// return device VID
USHORT CUsbNode::getVendorId()
{
    return idVendor;
}

// return device version (descriptor bcdDevice field)
USHORT CUsbNode::getDeviceVersion()
{
    return bcdDevice;
}

// return hardware ID string (OS-specific)
const std::string& CUsbNode::getHardwareId()
{
    return HardwareId;
}

// return friendly name string (OS-specific)
const std::string& CUsbNode::getFriendlyName()
{
    return FriendlyName;
}

// return mfr name string (OS-specific)
const std::string& CUsbNode::getMfrName()
{
    return MfrName;
}

// return name of associated service (OS-specific)
// typically, this is the base name of the client driver.
const std::string& CUsbNode::getServiceName()
{
    return ServiceName;
}

// return device path string (OS-specific)
const std::string& CUsbNode::getDevicePath()
{
    return strPath;
}

// return device instance ID string (OS-specific)
const std::string& CUsbNode::getDeviceInstanceId()
{
    return strInstance;
}

// return reference to internal descriptor struct
SUsbDescriptors& CUsbNode::getDescriptors()
{
    return desc;
}

// Hash the supplied string to create a unique 32-bit value
// Intended for use with the device instance ID string.
USBD_NODE_ID CUsbNode::createNodeId(char *str)
{
    unsigned long hash = 0;
    unsigned int c;
    // magic multiplier k=33
    while ((c = (unsigned int) *str++))
        hash = ((hash << 5) + hash) + c;
    return (USBD_NODE_ID) ((long long)hash);
}

#if !defined ( __linux__ )

#endif//LINUX_FIX
