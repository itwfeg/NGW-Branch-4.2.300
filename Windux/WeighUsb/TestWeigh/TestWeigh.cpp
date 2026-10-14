/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

RWBulk.c

Abstract:

Console test app for BulkUsb.sys driver

Environment:

user mode only

Notes:

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
PURPOSE.

Copyright (c) 1997-1998 Microsoft Corporation.  All Rights Reserved.


Revision History:

11/17/97: created

--*/

#pragma warning( disable : 4200)

extern "C"
{
#include <windows.h>

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "devioctl.h"

#include <setupapi.h>
#include <basetyps.h>
#include "..\WeighUsb\WghIoctl.h"

#include "usbdi.h"
};

#if DBG
#define ASSERT( x ) assert( x )
#else
#define ASSERT
#endif

#define MAX_WGHBUFFER_SIZE  64

#define HeaderTypeSys       0
#define HeaderTypeWg        1

char g_devPath[MAX_PATH] = "";

BOOL DumpUsbConfigFlag = FALSE;


unsigned char   *pInBuf = NULL;
unsigned char   *pOutBuf = NULL;

HANDLE  hRead   = INVALID_HANDLE_VALUE;
HANDLE  hWrite  = INVALID_HANDLE_VALUE;

char inPipe[32] = "PIPE00";     // pipe name for bulk input pipe
char outPipe[32] = "PIPE01";    // pipe name for bulk output pipe

BOOL fDumpUsbConfig = FALSE;    // flags set in response to console command line switches
BOOL fDumpReadData = FALSE;
BOOL fRead = FALSE;
BOOL fWrite = FALSE;

int gDebugLevel = 1;      // higher == more verbose, default is 1, 0 turns off all

ULONG IterationCount = 1; //count of iterations of the test we are to perform
int WriteLen = 0;         // #bytes to write
int ReadLen = 0;          // #bytes to read

#define MFR_VID       0x128c
#define PID_PRN       0x0200
#define PID_SCALE     0x0204
#define PID_SCALEBOOT 0x8400
#define PID_PRNBOOT   0x8200
#define PID_LSMN      0x0001

const int g_known_device_pids[] =
{
     MFR_VID
    ,PID_PRN
    ,PID_SCALE
    ,PID_SCALEBOOT
    ,PID_PRNBOOT
    ,PID_LSMN
};


BOOL
UsbWeigherRead(HANDLE hRd, unsigned char * buf, int ReadSize);

BOOL
UsbWeigherWrite(HANDLE hWr, unsigned char * buf, int WriteSize);

BOOL
SendRequestMode(unsigned char MsgCode, char *sMsg, unsigned char wMode);

BOOL
ReceiveResponse();

BOOL
SendRequest(unsigned char MsgCode, char *sMsg, int HeaderType);

void
ListDevices();

void
GetDevicePath();


unsigned char WghHeader[] = {   0x00, 0x00,         // Physical Souce Address
                                0x00, 0x09,         // Logical Source Address, 0x09 = Main controller board.
                                0x55, 0x55,         // Physical Destination Address
                                0x00, 0x06,         // Logical Destination Address, 0x06 = Weigher function.
                                0x00, 0x01 };       // Length of the data will follow, set to 1 for now.

unsigned char SysHeader[] = {   0x00, 0x00,         // Physical Souce Address
                                0x00, 0x09,         // Logical Source Address, 0x09 = Main controller board.
                                0x55, 0x55,         // Physical Destination Address
                                0x00, 0x08,         // Logical Destination Address, 0x08 = System function.
                                0x00, 0x01 };       // Length of the data will follow, set to 1 for now.

//
//  OpenOneDevice
//

HANDLE
OpenOneDevice ( IN HDEVINFO                   HardwareDeviceInfo,
                IN PSP_DEVICE_INTERFACE_DATA    DeviceInfoData,
                IN char *devName)
/*++
   Routine Description:

   Given the HardwareDeviceInfo, representing a handle to the plug and
   play information, and deviceInfoData, representing a specific usb device,
   open that device and fill in all the relevant information in the given
   USB_DEVICE_DESCRIPTOR structure.

   Arguments:

   HardwareDeviceInfo:  handle to info obtained from Pnp mgr via SetupDiGetClassDevs()
   DeviceInfoData:      ptr to info obtained via SetupDiEnumDeviceInterfaces()

   Return Value:

   return HANDLE if the open and initialization was successfull,
   else INVLAID_HANDLE_VALUE.

--*/
{
    PSP_DEVICE_INTERFACE_DETAIL_DATA     functionClassDeviceData = NULL;
    ULONG                                predictedLength = 0;
    ULONG                                requiredLength = 0;
    HANDLE                               hOut = INVALID_HANDLE_VALUE;

    //
    // allocate a function class device data structure to receive the
    // goods about this particular device.
    //
    if (SetupDiGetDeviceInterfaceDetail(HardwareDeviceInfo,
                                        DeviceInfoData,
                                        NULL,   // probing so no output buffer yet
                                        0,      // probing so output buffer length of zero
                                        &requiredLength,
                                        NULL))  // not interested in the specific dev-node
    {
        return INVALID_HANDLE_VALUE;
    }

    if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
    {
        return INVALID_HANDLE_VALUE;
    }

    predictedLength = requiredLength;
    // sizeof (SP_FNCLASS_DEVICE_DATA) + 512;

    functionClassDeviceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc (predictedLength);

    if (NULL == functionClassDeviceData)
    {
        return INVALID_HANDLE_VALUE;
    }

    functionClassDeviceData->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);

    //
    // Retrieve the information from Plug and Play.
    //
    if (! SetupDiGetDeviceInterfaceDetail (    HardwareDeviceInfo,
                                               DeviceInfoData,
                                               functionClassDeviceData,
                                               predictedLength,
                                               &requiredLength,
                                               NULL))
    {
        free( functionClassDeviceData );
        printf( "OpenOneDevice: failed to open '%s' - device not present\n", devName );
        return INVALID_HANDLE_VALUE;
    }

    strcpy( devName,functionClassDeviceData->DevicePath) ;
    //printf("Attempting to open '%s'\n", devName);

    hOut = CreateFile(functionClassDeviceData->DevicePath,
                      GENERIC_READ | GENERIC_WRITE,
                      FILE_SHARE_READ | FILE_SHARE_WRITE,
                      NULL,         // No SECURITY_ATTRIBUTES structure
                      OPEN_EXISTING,// No special create flags
                      0,            // No special attributes
                      NULL);        // No template file

    if (INVALID_HANDLE_VALUE == hOut)
    {
        //printf( "OpenOneDevice: Failed to open '%s', CreateFile returned 0x%x\n", devName, GetLastError() );
    }
    else
    {
        //printf( "OpenOneDevice: opened '%s'\n", devName );
    }

    free( functionClassDeviceData );
    return hOut;
}


//
//  OpenUsbDevice
//

HANDLE
OpenUsbDevice( LPGUID  pGuid, char *outNameBuf)
/*++
   Routine Description:

   Do the required PnP things in order to find
   the next available proper device in the system at this time.

   Arguments:

   pGuid:      ptr to GUID registered by the driver itself
   outNameBuf: the generated name for this device

   Return Value:

   return HANDLE if the open and initialization was successful,
   else INVLAID_HANDLE_VALUE.
--*/
{
    ULONG                    NumberDevices;
    HANDLE                   hOut = INVALID_HANDLE_VALUE;
    HDEVINFO                 hardwareDeviceInfo;
    SP_DEVICE_INTERFACE_DATA deviceInfoData;
    ULONG                    i;
    BOOLEAN                  done;
    PUSB_DEVICE_DESCRIPTOR   usbDeviceInst;
    PUSB_DEVICE_DESCRIPTOR   *UsbDevices = &usbDeviceInst;
    PUSB_DEVICE_DESCRIPTOR   tempDevDesc;

    *UsbDevices = NULL;
    tempDevDesc = NULL;
    NumberDevices = 0;

    //
    // Open a handle to the plug and play dev node.
    // SetupDiGetClassDevs() returns a device information set that contains info on all
    // installed devices of a specified class.
    //
    hardwareDeviceInfo = SetupDiGetClassDevs(pGuid,
                                             NULL, // Define no enumerator (global)
                                             NULL, // Define no
                                             (DIGCF_PRESENT | // Only Devices present
                                              DIGCF_DEVICEINTERFACE)); // Function class devices.

    if (hardwareDeviceInfo == INVALID_HANDLE_VALUE)
    {
        return INVALID_HANDLE_VALUE;
    }

    //
    // Take a wild guess at the number of devices we have;
    // Be prepared to realloc and retry if there are more than we guessed
    //
    NumberDevices = 4;
    done = FALSE;
    deviceInfoData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);

    i=0;

    while (!done)
    {
      NumberDevices *= 2;

      if (*UsbDevices)
      {
            tempDevDesc = (PUSB_DEVICE_DESCRIPTOR) realloc(*UsbDevices,
                                  (NumberDevices * sizeof (USB_DEVICE_DESCRIPTOR)));
            if(tempDevDesc)
            {
                *UsbDevices = tempDevDesc;
                tempDevDesc = NULL;
            }
            else
            {
                free(*UsbDevices);
                *UsbDevices = NULL;
            }
        }
        else
        {
            *UsbDevices = (PUSB_DEVICE_DESCRIPTOR) calloc(NumberDevices, sizeof(USB_DEVICE_DESCRIPTOR));
        }

        if (NULL == *UsbDevices)
        {
            // SetupDiDestroyDeviceInfoList destroys a device information set
            // and frees all associated memory.

            SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

            return INVALID_HANDLE_VALUE;
        }

        usbDeviceInst = *UsbDevices + i;

        for (; i < NumberDevices; i++)
        {
            // SetupDiEnumDeviceInterfaces() returns information about device interfaces
            // exposed by one or more devices. Each call returns information about one interface;
            // the routine can be called repeatedly to get information about several interfaces
            // exposed by one or more devices.

            if (SetupDiEnumDeviceInterfaces(hardwareDeviceInfo,
                                            0, // We don't care about specific PDOs
                                            pGuid,
                                            i,
                                            &deviceInfoData))
            {
                hOut = OpenOneDevice(hardwareDeviceInfo,
                                     &deviceInfoData,
                                     outNameBuf);

                if (hOut != INVALID_HANDLE_VALUE)
                {
                    done = TRUE;
                    break;
                }
            }
            else
            {
                if (ERROR_NO_MORE_ITEMS == GetLastError())
                {
                    done = TRUE;
                    break;
                }
            }
        }
    }

    NumberDevices = i;

    // SetupDiDestroyDeviceInfoList() destroys a device information set
    // and frees all associated memory.

    SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

    free(*UsbDevices);

    return hOut;
}




BOOL
GetUsbDeviceFileName (
    LPGUID  pGuid,
    char   *outNameBuf
)
/*++
   Routine Description:

   Given a ptr to a driver-registered GUID, give us a string with the device name
   that can be used in a CreateFile() call.
   Actually briefly opens and closes the device and sets outBuf if successfull;
   returns FALSE if not

   Arguments:

   pGuid:      ptr to GUID registered by the driver itself
   outNameBuf: the generated zero-terminated name for this device

   Return Value:

   TRUE on success else FALSE

--*/
{
    HANDLE hDev = OpenUsbDevice(pGuid, outNameBuf);

    if (hDev != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hDev);
        return TRUE;
    }
    return FALSE;

}

HANDLE
WghOpenDev()
{
    HANDLE hDEV =  OpenUsbDevice((LPGUID)&GUID_CLASS_WEIGHING_USB, g_devPath);

    if (hDEV == INVALID_HANDLE_VALUE)
    {
        printf("Failed to open (%s) = %d", g_devPath, GetLastError());
    }
    else
    {
        printf("DeviceName = (%s)\n", g_devPath);
    }

    return hDEV;
}

HANDLE
WghOpenFile( char *filename, BOOL bUseOverlapped)
/*++
   Routine Description:

   Called by main() to open an instance of our device after obtaining its name

   Arguments:

   None

   Return Value:

   Device handle on success else NULL

--*/
{
    int     success = 1;
    HANDLE  hFile;

    if ( !GetUsbDeviceFileName((LPGUID) &GUID_CLASS_WEIGHING_USB, g_devPath) )
    {
        printf("WghOpenFile: GetUsbDeviceFileName failed, error 0x%x\n", GetLastError());
        return  INVALID_HANDLE_VALUE;
    }

    strcat (g_devPath, "\\");

    if ((strlen(g_devPath) + strlen(filename)) > MAX_PATH-1)
    {
        printf(("Failed to open handle - filename too long\n"));
        return INVALID_HANDLE_VALUE;
    }

    strcat (g_devPath, filename);

    //printf("g_devPath = (%s)\n", g_devPath);

    hFile = CreateFile( g_devPath,
                        GENERIC_WRITE | GENERIC_READ,
                        FILE_SHARE_WRITE | FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        bUseOverlapped ? FILE_FLAG_OVERLAPPED : 0,
                        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("WghOpenFile: Failed to open '%s', error 0x%x\n", g_devPath, GetLastError());
        success = 0;
    }
    printf("WghOpenFile: opened '%s'\n", g_devPath);
    return hFile;
}

//
//  WghUsage
//

void
WghUsage()
/*++
   Routine Description:

   Called by main() to dump usage info to the console when
   the app is called with no parms or with an invalid parm

   Arguments:

   None

   Return Value:

   None

--*/
{
    printf("\n");
    printf("***** Weighing USB test *****\n");
    printf("\n");
    printf("-w Wake up the weigher\n");
    printf("-c Change the configuration of the weigher\n");
    printf("-i Inquire the configuration of weigher\n");
    printf("-m Set the mode of weigher\n");
    printf("-r Reset the weigher\n");
    printf("-s Weigher status\n");
    printf("-e Weigher enable\n");
    printf("-d Weigher disable\n");
    printf("-z Weigher re-zero\n");
    printf("-p to print *** USB configuration and pipe infomation ***\n");
}


void
wghParse(int argc, char *argv[])
/*++
   Routine Description:

   Called by main() to parse command line parms

   Arguments:

   argc and argv that was passed to main()

   Return Value:

   Sets global flags as per user function request

--*/
{
    int             i;
    unsigned char   RdataLength;
    unsigned char   leftover;

    if ( argc < 2 )
            WghUsage();

    for (i=0; i<argc; i++)
    {
        if (argv[i][0] == '-' || argv[i][0] == '/')
        {
            switch(argv[i][1])
            {
            case 'm':
            case 'M':
                {
    //              SendRequestMode(WG_MODE, "MODE:SCALE_WEIGHT_TRACKING_MODE", SCALE_WEIGHT_TRACKING_MODE);
    //              Sleep(500);
    //              ReceiveResponse();
    //
    //              Sleep(500);
    //              SendRequestMode(WG_MODE, "MODE:SCALE_PREPACK_MODE", SCALE_PREPACK_MODE);
    //              Sleep(500);
    //              ReceiveResponse();
    //
    //              Sleep(500);
    //              SendRequestMode(WG_MODE, "MODE:SCALE_DUMB_MODE", SCALE_DUMB_MODE);
    //              Sleep(500);
    //              ReceiveResponse();
    //
    //              Sleep(500);
    //              SendRequestMode(WG_MODE, "MODE:SCALE_MANUAL_MODE", SCALE_MANUAL_MODE);
    //              Sleep(500);
    //              ReceiveResponse();
    //
    //              Sleep(500);
                    SendRequestMode(WG_MODE, "MODE:SCALE_WEIGHT_TRACKING_MODE", SCALE_WEIGHT_TRACKING_MODE);
                    //Sleep(500);
                    ReceiveResponse();
                    break;
                }

            case 'w':
            case 'W':
                {
                    SendRequest(WG_WAKEUP, "WAKEUP", HeaderTypeWg);
                    //Sleep(500);
                    ReceiveResponse();
                    break;
                }

            case 's':
            case 'S':
                {
                    SendRequest(WG_STATUS, "STATUS", HeaderTypeSys);
                    //Sleep(500);
                    ReceiveResponse();
                    break;
                }

            case 'd':
            case 'D':
                {
                    SendRequest(WG_DISABLE, "DISABLE", HeaderTypeWg);
                    break;
                }

            case 'e':
            case 'E':
                {
                    SendRequest(WG_ENABLE, "ENABLE", HeaderTypeWg);
                    break;
                }

            case 'r':
            case 'R':
                {
                    SendRequest(WG_RESET, "RESET", HeaderTypeWg);
                    break;
                }

            case 'z':
            case 'Z':
                {
                    SendRequest(WG_REZERO, "REZERO", HeaderTypeWg);
                    //Sleep(500);
                    ReceiveResponse();
                    break;
                }

            case 'c':
            case 'C':
                {
                    printf("\n* Send CONFIGURATION command to Weigher *\n");
                    printf("\n***** This feature is not implemented until more info from customer *****\n");
                    break;
                }

             case 'i':
             case 'I':
                {
                    SendRequest(WG_INQUIRE, "INQUIRE", HeaderTypeWg);
                    //Sleep(500);
                    ReceiveResponse();
                    break;
                }

            case 'p':
            case 'P':
                {
                    DumpUsbConfigFlag = TRUE;
                    break;
                }
/*
            // load numeric value from command line arg
            case 'x':
            case 'X':
                if (argc > i+1)
                {
                    WriteLen = atoi(&argv[i+1][0]);
                    fWrite = TRUE;
                }
                i++;
                break;
*/
            default:
                WghUsage();
            }
        }
    }
}
//
//  usbDescriptorTypeString
//

char
*usbDescriptorTypeString(UCHAR bDescriptorType )
{
    switch(bDescriptorType)
    {

    case USB_DEVICE_DESCRIPTOR_TYPE:
            return "USB_DEVICE_DESCRIPTOR_TYPE";

    case USB_CONFIGURATION_DESCRIPTOR_TYPE:
            return "USB_CONFIGURATION_DESCRIPTOR_TYPE";


    case USB_STRING_DESCRIPTOR_TYPE:
            return "USB_STRING_DESCRIPTOR_TYPE";


    case USB_INTERFACE_DESCRIPTOR_TYPE:
            return "USB_INTERFACE_DESCRIPTOR_TYPE";


    case USB_ENDPOINT_DESCRIPTOR_TYPE:
            return "USB_ENDPOINT_DESCRIPTOR_TYPE";

    default:
            return "??? UNKNOWN!!";
    }
}

//
//  usbEndPointTypeString
//

char
*usbEndPointTypeString(UCHAR bmAttributes)
{
    UINT typ = bmAttributes & USB_ENDPOINT_TYPE_MASK;

    switch( typ)
    {
    case USB_ENDPOINT_TYPE_INTERRUPT:
            return "USB_ENDPOINT_TYPE_INTERRUPT";

    case USB_ENDPOINT_TYPE_BULK:
            return "USB_ENDPOINT_TYPE_BULK";

    case USB_ENDPOINT_TYPE_ISOCHRONOUS:
            return "USB_ENDPOINT_TYPE_ISOCHRONOUS";

    case USB_ENDPOINT_TYPE_CONTROL:
            return "USB_ENDPOINT_TYPE_CONTROL";

    default:
            return "??? UNKNOWN!!";
    }
}

//
//  usbConfigAttributesString
//

char
*usbConfigAttributesString(UCHAR bmAttributes)
{
    UINT typ = bmAttributes & USB_CONFIG_POWERED_MASK;

    switch( typ)
    {
        case USB_CONFIG_BUS_POWERED:
                return "USB_CONFIG_BUS_POWERED";

        case USB_CONFIG_SELF_POWERED:
                return "USB_CONFIG_SELF_POWERED";

        case USB_CONFIG_REMOTE_WAKEUP:
                return "USB_CONFIG_REMOTE_WAKEUP";

        default:
                return "??? UNKNOWN!!";
    }
}
//
//  print_USB_CONFIGURATION_DESCRIPTOR
//

void
print_USB_CONFIGURATION_DESCRIPTOR(PUSB_CONFIGURATION_DESCRIPTOR cd)
{
    printf("\n===================\nUSB_CONFIGURATION_DESCRIPTOR\n");
    printf("bLength = 0x%x, decimal %d\n", cd->bLength, cd->bLength);
    printf("bDescriptorType = 0x%x ( %s )\n", cd->bDescriptorType, usbDescriptorTypeString( cd->bDescriptorType ));
    printf("wTotalLength = 0x%x, decimal %d\n", cd->wTotalLength, cd->wTotalLength);
    printf("bNumInterfaces = 0x%x, decimal %d\n", cd->bNumInterfaces, cd->bNumInterfaces);
    printf("bConfigurationValue = 0x%x, decimal %d\n", cd->bConfigurationValue, cd->bConfigurationValue);
    printf("iConfiguration = 0x%x, decimal %d\n", cd->iConfiguration, cd->iConfiguration);
    printf("bmAttributes = 0x%x ( %s )\n", cd->bmAttributes, usbConfigAttributesString( cd->bmAttributes ));
    printf("MaxPower = 0x%x, decimal %d\n", cd->MaxPower, cd->MaxPower);
}

//
//  print_USB_INTERFACE_DESCRIPTOR
//

void
print_USB_INTERFACE_DESCRIPTOR(PUSB_INTERFACE_DESCRIPTOR id, UINT ix)
{
    printf("\n-----------------------------\nUSB_INTERFACE_DESCRIPTOR #%d\n", ix);
    printf("bLength = 0x%x\n", id->bLength);
    printf("bDescriptorType = 0x%x ( %s )\n", id->bDescriptorType, usbDescriptorTypeString( id->bDescriptorType ));
    printf("bInterfaceNumber = 0x%x\n", id->bInterfaceNumber);
    printf("bAlternateSetting = 0x%x\n", id->bAlternateSetting);
    printf("bNumEndpoints = 0x%x\n", id->bNumEndpoints);
    printf("bInterfaceClass = 0x%x\n", id->bInterfaceClass);
    printf("bInterfaceSubClass = 0x%x\n", id->bInterfaceSubClass);
    printf("bInterfaceProtocol = 0x%x\n", id->bInterfaceProtocol);
    printf("bInterface = 0x%x\n", id->iInterface);
}

//
//  print_USB_ENDPOINT_DESCRIPTOR
//
void
print_USB_ENDPOINT_DESCRIPTOR(PUSB_ENDPOINT_DESCRIPTOR ed, int i)
{
    printf("------------------------------\nUSB_ENDPOINT_DESCRIPTOR for Pipe%02d\n", i);
    printf("bLength = 0x%x\n", ed->bLength);
    printf("bDescriptorType = 0x%x ( %s )\n", ed->bDescriptorType, usbDescriptorTypeString( ed->bDescriptorType ));

    if ( USB_ENDPOINT_DIRECTION_IN( ed->bEndpointAddress ) )
    {
        printf("bEndpointAddress= 0x%x ( INPUT )\n", ed->bEndpointAddress);
    }
    else
        {
            printf("bEndpointAddress= 0x%x ( OUTPUT )\n", ed->bEndpointAddress);
        }

    printf("bmAttributes= 0x%x ( %s )\n", ed->bmAttributes, usbEndPointTypeString ( ed->bmAttributes ));
    printf("wMaxPacketSize= 0x%x, decimal %d\n", ed->wMaxPacketSize, ed->wMaxPacketSize);
    printf("bInterval = 0x%x, decimal %d\n", ed->bInterval, ed->bInterval);
}

//
//  UsbConfig
//

void
UsbConfig( HANDLE hDEV )
{
    UINT    success;
    int     siz;
    DWORD   nBytes;
    char    buf[256];

    PUSB_CONFIGURATION_DESCRIPTOR cd;
    PUSB_INTERFACE_DESCRIPTOR id;
    PUSB_ENDPOINT_DESCRIPTOR ed;

    siz = sizeof(buf);

    if (hDEV == INVALID_HANDLE_VALUE)
    {
        printf("USB Device not open");
        return;
    }

    success = DeviceIoControl(hDEV, IOCTL_USB_GET_CONFIG_DESCRIPTOR, buf, siz, buf, siz, &nBytes,   NULL);

    printf("Get USB Configuration Request complete, success = %d nBytes = %d\n", success, nBytes);

    if (success)
    {
        ULONG i;
        UINT  j, n;
        char *pch;

        pch = buf;
        n = 0;

        cd = (PUSB_CONFIGURATION_DESCRIPTOR) pch;
        print_USB_CONFIGURATION_DESCRIPTOR( cd );

        pch += cd->bLength;

        do
        {
            id = (PUSB_INTERFACE_DESCRIPTOR) pch;
            print_USB_INTERFACE_DESCRIPTOR(id, n++);
            pch += id->bLength;

            for (j=0; j<id->bNumEndpoints; j++)
            {
                ed = (PUSB_ENDPOINT_DESCRIPTOR) pch;
                print_USB_ENDPOINT_DESCRIPTOR(ed,j);
                pch += ed->bLength;
            }
            i = (ULONG)(pch - buf);

        } while (i<cd->wTotalLength);
    }
    return;
}

//
//  UsbDeviceDescp
//

void
UsbDeviceDescp( HANDLE hDEV )
{
    UINT    success;
    int     siz;
    DWORD   nBytes;
    char    buf[256];

    PUSB_DEVICE_DESCRIPTOR  pDevice;

    siz = sizeof(buf);

    if (hDEV == INVALID_HANDLE_VALUE)
    {
        printf("USB Device not open");
        return;
    }

    success = DeviceIoControl(hDEV, IOCTL_USB_GET_DEVICE_DESCRIPTOR, buf, siz, buf, siz, &nBytes,   NULL);

    printf("Get USB device descriptor Request complete, success = %d nBytes = %d\n", success, nBytes);

    if (success)
    {
        ULONG i;
        UINT  j, n;
        char *pch;

        pch = buf;
        n = 0;

        pDevice = (PUSB_DEVICE_DESCRIPTOR) pch;

        printf("\n-----------------------------\nUSB_DEVICE_DESCRIPTOR\n");
        printf("bLength = 0x%x\n", pDevice->bLength);
        printf("bDescriptorType = 0x%x ( %s )\n", pDevice->bDescriptorType, usbDescriptorTypeString( pDevice->bDescriptorType ));
        printf("bcdUSB = 0x%x\n", pDevice->bcdUSB);
        printf("bDeviceClass = 0x%x\n", pDevice->bDeviceClass);
        printf("bDeviceSubClass = 0x%x\n", pDevice->bDeviceSubClass);
        printf("bDeviceProtocol = 0x%x\n", pDevice->bDeviceProtocol);
        printf("bMaxPacketSize0 = 0x%x\n", pDevice->bMaxPacketSize0);
        printf("idVendor = 0x%x\n", pDevice->idVendor);
        printf("idProduct = 0x%x\n", pDevice->idProduct);
        printf("bcdDevice = 0x%x\n", pDevice->bcdDevice);
        printf("iManufacturer = 0x%x\n", pDevice->iManufacturer);
        printf("iProduct = 0x%x\n", pDevice->iProduct);
        printf("iSerialNumber = 0x%x\n", pDevice->iSerialNumber);
        printf("bNumConfigurations = 0x%x\n", pDevice->bNumConfigurations);
    }
    return;
}

//
//  UsbResetPipe
//

void
UsbResetPipe( HANDLE hDEV )
{
    char    buf[256];
    int     siz;
    DWORD   nBytes;

    siz = sizeof(buf);

    if (hDEV == INVALID_HANDLE_VALUE)
    {
        printf("UsbResetPipe: USB Device not open");
        return;
    }

    DeviceIoControl(hDEV, IOCTL_USB_RESET_PIPE, buf, siz, buf, siz, &nBytes, NULL);
}

//
//  UsbWeigherRead
//

#define _T(x) x

HANDLE m_hReaderThreadSignal = CreateEvent(NULL, TRUE, FALSE, NULL);

BOOL
UsbWeigherRead(HANDLE hRd, unsigned char * buf, int ReadSize)
{
    BOOL ret = FALSE;

    //bool m_bReaderRunning = false;
    HANDLE m_hReaderThread;
    const DWORD OVERLAPPED_READ_WAIT_TIME = 5000;
    const DWORD MAX_READ_THREAD_ERRORS = 20;

    //const int MaxReadSize = 512;
    int nErrors = 0;
    DWORD Result = ERROR_GEN_FAILURE;
    DWORD WaitResult;
    BOOL bResult;
    DWORD NumBytesRead;
    OVERLAPPED ReadState = {0};
    bool bGotInput = false;

    //m_bReaderRunning = true;
    printf ("UsbWeigherRead: entry. n=%u\n",ReadSize);

    // validate file and buffer parameters
    ASSERT(hRd != INVALID_HANDLE_VALUE);
    ASSERT(buf);
    ASSERT(ReadSize <= MAX_WGHBUFFER_SIZE);

    // create overlapped event
    ReadState.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (ReadState.hEvent == 0)
    {
        printf(_T("UsbWeigherRead: failed to create overlapped event\n"));
        goto readerExit;
    }

    HANDLE WaitEvents[2] = {m_hReaderThreadSignal, ReadState.hEvent};

    //while (m_bReaderRunning)
    {
        bResult = ReadFile(hRd, buf, ReadSize, &NumBytesRead, &ReadState);

        if (bResult == FALSE)
        {
            Result = GetLastError();
            if (Result == ERROR_IO_PENDING)
            {
                // wait for data
                WaitResult = WaitForMultipleObjects(sizeof(WaitEvents)/sizeof(WaitEvents[0]), WaitEvents, FALSE, OVERLAPPED_READ_WAIT_TIME);

                if (WaitResult == WAIT_TIMEOUT)
                {
                    Result = WAIT_TIMEOUT;
                    printf(_T("UsbWeigherRead: timeout in WaitForMultipleObjects, %u\n"),GetLastError());
                    //m_bReaderRunning = false;
                }
                //else if (WaitResult == WAIT_OBJECT_0)
                //{
                //    // If m_hReaderThreadSignal is signaled, exit thread.
                //    m_bReaderRunning = false;
                //}
                else if (WaitResult == WAIT_OBJECT_0 + 1)
                {
                    // ReadState.hEvent is signaled, the overlapped read has completed

                    bResult = GetOverlappedResult(hRd, &ReadState, &NumBytesRead, TRUE);
                    if (bResult == TRUE)
                    {
                        bGotInput = true;
                        Result = ERROR_SUCCESS;
                        printf(_T("UsbWeigherRead: received %u bytes data, NumBytesRead\n"));
                    }
                    else
                    {
                        Result = GetLastError();
                        printf(_T("UsbWeigherRead: GetOverlappedResult failed, %u\n"),Result);
                        //nErrors++;
                        //Sleep(0);
                    }
                }
                else  // WaitResult == ??
                {
                   printf(_T("UsbWeigherRead: WaitForMultipleObjects returned  %u\n"),WaitResult);
                   Result = GetLastError();
                   //nErrors++;
                   //Sleep(0);
                }
            }
            else
            {
                printf(_T("UsbWeigherRead: ReadFile returned error %u\n"),Result);
                //nErrors++;
                //Sleep(0);
            }
        }
        else  // bResult == TRUE
        {
            // first call to ReadFile returned data
            bGotInput = true;
            Result = ERROR_SUCCESS;
            printf(_T("UsbWeigherRead: received data on initial call\n"));
        }

        if (bGotInput)
        {
            // success
            bGotInput = false;

            // dump hex
            for (DWORD i = 0; i < NumBytesRead; i++)
            {
                printf("0x%02x ", buf[i]);
            }
            printf("\n\n");
        }
    }

readerExit:
    if (ERROR_SUCCESS != Result)
        CancelIo( hRd );

    if (ReadState.hEvent)
        CloseHandle(ReadState.hEvent);
    //m_bReaderRunning = false;
    printf(_T("UsbWeigherRead: exit\n"));

    return ret;
}

//
//  UsbWeigherWrite
//

BOOL
UsbWeigherWrite(int WriteSize)
{
    DWORD nBytesWrite, i;
    BOOL ret = FALSE;

    if (WriteSize > MAX_WGHBUFFER_SIZE)
    {
        printf("UsbWeigherWrite :: Fail to write because the request size is too large\n");
        return FALSE;
    }

    if ((pOutBuf == NULL) || (hWrite == INVALID_HANDLE_VALUE))
    {
        printf("UsbWeigherWrite :: Invalid handle\n");
        return FALSE;
    }

    if (WriteFile(hWrite, pOutBuf, WriteSize, &nBytesWrite, NULL))
    {
        printf("HEADER/DATA(%u/%u):: ",nBytesWrite,WriteSize);

        for (i = 0; i < nBytesWrite; i++)
        {
            printf("0x%02x ", pOutBuf[i]);
        }
        printf("\n");
        ret = TRUE;
    }
    else
    {
        printf("UsbWeigherWrite :: WriteFile failed with 0x%x\n", GetLastError());
    }
    return ret;
}

//
//  SendRequest
//

BOOL
SendRequest(unsigned char MsgCode, char *sMsg, int HeaderType)
{
    WgRequest *pReq;

    printf("\n\n* Send %s command to Weigher *\n\n", sMsg);

    if (HeaderType == HeaderTypeWg)
        memcpy(pOutBuf, WghHeader, sizeof(WghHeader));
    else
        memcpy(pOutBuf, SysHeader, sizeof(SysHeader));

    pOutBuf[sizeof(WghHeader) - 1] = sizeof(WgRequest);

    if (UsbWeigherWrite(sizeof(WghHeader)))
    {
        memset(pOutBuf, 0, sizeof(WghHeader));
        pReq = (WgRequest *)pOutBuf;
        pReq->function_code = MsgCode;
        pReq->pad0 = 0;

        if (!UsbWeigherWrite(sizeof(WgRequest)))
        {
            printf("\n* Send %s command DATA failed *\n", sMsg);
            return FALSE;
        }
    }
    else
    {
        printf("\n* Send %s command HEADER failed *\n", sMsg);
        return FALSE;
    }

    return TRUE;
}

//
//  ReceiveResponse
//

BOOL
ReceiveResponse()
{
    unsigned char   RdataLength;
    unsigned char   leftover;

    printf("\n\n* Receive response from Weigher *\n\n");

    memset(pInBuf, 0, MAX_WGHBUFFER_SIZE);

    if (UsbWeigherRead(hRead, pInBuf, sizeof(WghHeader)))
    {
        RdataLength = pInBuf[sizeof(WghHeader) - 1];
        memset(pInBuf, 0, MAX_WGHBUFFER_SIZE);

        leftover = 0;

        while(RdataLength)
        {
            if (RdataLength > MAX_WGHBUFFER_SIZE)
            {
                leftover = RdataLength - MAX_WGHBUFFER_SIZE;
                RdataLength = MAX_WGHBUFFER_SIZE;
            }
            //Sleep(500);

            if (!UsbWeigherRead(hRead, pInBuf, RdataLength))
                break;

            RdataLength = leftover;
            leftover = 0;
        }
    }
    return TRUE;
}

//
//  SendRequestMode
//

BOOL
SendRequestMode(unsigned char MsgCode, char *sMsg, unsigned char wMode)
{
    WgMode *pMode;

    printf("\n\n* Send %s command to Weigher *\n\n", sMsg);
    memcpy(pOutBuf, WghHeader, sizeof(WghHeader));
    pOutBuf[sizeof(WghHeader) - 1] = sizeof(WgRequest);

    if (UsbWeigherWrite(sizeof(WghHeader)))
    {
        memset(pOutBuf, 0, sizeof(WghHeader));
        pMode = (WgMode *)pOutBuf;
        pMode->function_code = MsgCode;
        pMode->pad0 = 0;
        pMode->mode = wMode;
        pMode->pad1 = 0;

        if (!UsbWeigherWrite(sizeof(WgMode)))
        {
            printf("\n* Send %s command DATA failed *\n", sMsg);
            return FALSE;
        }
    }
    else
    {
        printf("\n* Send %s command HEADER failed *\n", sMsg);
        return FALSE;
    }

    return TRUE;
}

//
//  wghParse
//


//
// main
//

int _cdecl main(int argc, char *argv[])
{
    HANDLE hDEV;

    pInBuf  = (unsigned char *) malloc(MAX_WGHBUFFER_SIZE);
    pOutBuf = (unsigned char *) malloc(MAX_WGHBUFFER_SIZE);

    ListDevices();
    GetDevicePath();

    hRead   = WghOpenFile(inPipe, TRUE);
    hWrite  = WghOpenFile(outPipe, FALSE);

    wghParse(argc, argv );

    if( DumpUsbConfigFlag )
    {
        if ((hDEV = WghOpenDev()) != INVALID_HANDLE_VALUE)
        {
            UsbConfig(hDEV);
            UsbDeviceDescp(hDEV);
            CloseHandle(hDEV);
        }
    }

    //CloseHandle(rtp.DataReadyEvent);

    if (pInBuf)
        free(pInBuf);

    if (pOutBuf)
        free(pOutBuf);

    if(hRead != INVALID_HANDLE_VALUE)
        CloseHandle(hRead);

    if(hWrite != INVALID_HANDLE_VALUE)
        CloseHandle(hWrite);

    return 0;
}


void
ListDevices( )
{
    ULONG                    NumberDevices=0;
    HANDLE                   hOut = INVALID_HANDLE_VALUE;
    HDEVINFO                 hardwareDeviceInfo;
    SP_DEVICE_INTERFACE_DATA deviceInfoData;
    ULONG                    i;
    BOOLEAN                  done;
    PUSB_DEVICE_DESCRIPTOR   usbDeviceInst;
    PUSB_DEVICE_DESCRIPTOR   *UsbDevices = &usbDeviceInst;
    char fname[MAX_PATH];
    LPGUID  pGuid = (LPGUID) &GUID_CLASS_WEIGHING_USB;

    *UsbDevices = NULL;
    NumberDevices = 0;

    hardwareDeviceInfo = SetupDiGetClassDevs ( pGuid, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));
    NumberDevices = 4;
    done = FALSE;
    deviceInfoData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);

    i = 0;

    while (!done)
    {
        NumberDevices *= 2;

        if (*UsbDevices)
        {
            *UsbDevices = (PUSB_DEVICE_DESCRIPTOR)realloc (*UsbDevices, (NumberDevices * sizeof (USB_DEVICE_DESCRIPTOR)));
        }
        else
        {
            *UsbDevices = (PUSB_DEVICE_DESCRIPTOR)calloc (NumberDevices, sizeof (USB_DEVICE_DESCRIPTOR));
        }

        if (NULL == *UsbDevices)
        {
            break; // while
        }

        usbDeviceInst = *UsbDevices + i;

        for (; i < NumberDevices; i++)
        {
            if (SetupDiEnumDeviceInterfaces (  hardwareDeviceInfo, 0, pGuid, i, &deviceInfoData))
            {
                hOut = OpenOneDevice (hardwareDeviceInfo, &deviceInfoData, fname);

                if ( hOut != INVALID_HANDLE_VALUE )
                {
                    done = TRUE;
                    break;  // for
                }
            }
            else
            {
                if (ERROR_NO_MORE_ITEMS == GetLastError())
                {
                    done = TRUE;
                    break;  // for
                }
            }
        }
    }
    NumberDevices = i;
    SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
    if (UsbDevices)
        free ( *UsbDevices );
}


void GetDevicePath()
{
    LPGUID  pGuid = (LPGUID) &GUID_CLASS_WEIGHING_USB;
}

