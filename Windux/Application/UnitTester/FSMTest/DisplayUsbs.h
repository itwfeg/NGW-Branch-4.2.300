#if !defined DisplayUsbs_h
#define DisplayUsbs_h

//#include "stdafx.h"
#include "objbase.h"
//#include "winioctl.h"
#include "..\..\3RDPartySoftware\WinDDK\inc\wxp\usbioctl.h" // MSVC2010
#include "stdio.h"
#include "stdlib.h"

// Define all stuctures using UCHAR or BOOLEAN so that the variables are not 'aligned' by the compiler
struct DESCRIPTOR_REQUEST 
{
    ULONG ConnectionIndex;
    struct {UCHAR bmRequest; UCHAR bRequest; UCHAR wValue[2]; UCHAR wIndex[2]; UCHAR wLength[2];} SetupPacket;
    UCHAR Data[2048];
};

struct DEVICE_DESCRIPTOR 
{
    UCHAR bLength; 
    UCHAR bDescriptorType; 
    UCHAR bcdUSB[2];
    UCHAR bDeviceClass; 
    UCHAR bDeviceSubClass; 
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0; 
    UCHAR idVendor[2]; 
    UCHAR idProduct[2];
    UCHAR bcdDevice[2]; 
    UCHAR iManufacturer; 
    UCHAR iProduct;
    UCHAR iSerialNumber; 
    UCHAR bNumConfigurations;
};

struct HUB_DESCRIPTOR 
{
    UCHAR bDescriptorLength; 
    UCHAR bDescriptorType; 
    UCHAR bNumberOfPorts;
    UCHAR wHubCharacteristics[2]; 
    UCHAR bPowerOnToPowerGood; 
    UCHAR bHubControlCurrent;
    UCHAR bRemoveAndPowerMask[64];
};

struct NODE_INFORMATION 
{
    USB_HUB_NODE NodeType; 
    HUB_DESCRIPTOR HubDescriptor; 
    BOOLEAN HubIsBusPowered;
};

struct NODE_CONNECTION_INFORMATION 
{
    ULONG ConnectionIndex; 
    DEVICE_DESCRIPTOR DeviceDescriptor; 
    UCHAR CurrentConfigurationValue;
    BOOLEAN LowSpeed; 
    BOOLEAN DeviceIsHub; 
    UCHAR DeviceAddress[2];
    UCHAR NumberOfOpenPipes[4]; 
    UCHAR ConnectionStatus[4]; 
    USB_PIPE_INFO PipeList[32];
};


namespace Test
{
    const char ClassName[] [20] = 
    {
        "Reserved", "Audio", "Communications", "Human Interface",
        "Monitor", "Physical Interface", "Power", "Printer",
        "Storage", "Hub", "Vendor Specific", "*ILLEGAL VALUE*"
    };

    const char ConnectionStatus[] [30] =
    {
        "No device connected", "Device connected", "Device FAILED enumeration",
        "Device general FAILURE", "Device caused overcurrent", "Not enough power for device"
    };

    class DisplayUsbs
    {
        public:
            USHORT DisplayStringDescriptor (HANDLE HubHandle, ULONG PortIndex, USHORT LanguageID, UCHAR Index); 
            USHORT DisplayDeviceDescriptor (HANDLE HubHandle, ULONG PortIndex, USHORT LanguageID, PUCHAR BufferPtr); 
            USHORT DisplayConfigurationDescriptor(HANDLE HubHandle, ULONG PortIndex, USHORT LanguageID); 
            void GetPortData(HANDLE HubHandle, UCHAR PortCount, int HubDepth); 
            DWORD EnumerateHostController(HANDLE HostControllerHandle); 
            int main(/*int argc, char* argv[]*/);

            bool DEBUG; // Set to true to enable DEBUG messages
            SECURITY_ATTRIBUTES SA; // Needed for Win2000
    };

} //end of namespace

#endif //#if !defined DisplayUsbs_h
