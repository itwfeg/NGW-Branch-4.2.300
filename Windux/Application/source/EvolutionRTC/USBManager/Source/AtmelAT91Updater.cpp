/*
#include "EvolMessage.h"
#include "IPeripheralUpdater.h"

#include "VersionManagerEvent.h"
#include "PeripheralCodeUpdate.h"
#include "PeripheralRequestCodeUpdate.h"
#include "VmStatusMsg.h"

#include "filesys\file.h"
#include "filesys\directory.h"
#include "HobVersion.h"

*/

#include "AtmelAT91Updater.h"
#include "UsbManager.h"
#include "Debug.h"
#include "Hex.h"
#include "FCError.h"

/*min*/
#include "HobString.h"
/*emin*/

AtmelAT91Updater::AtmelAT91Updater()
{
}

AtmelAT91Updater::~AtmelAT91Updater()
{
}


#import "AT91Boot_DLL.tlb" rename_namespace("AT91BOOTDLL_Lib")
using namespace AT91BOOTDLL_Lib;
typedef int AT91BOOT_HANDLE_TYPE;

STATUS AtmelAT91Updater::scanAndGoAtmelBootDevice(USBD_NODE_ID nodeId)
{

    USHORT myPid = UsbManager::getInstance()->getProductId(nodeId);
    USHORT myVid = UsbManager::getInstance()->getVendorId(nodeId);

#define DBGU_CIDR_MASK ~0x000F001F

    STATUS st = ERROR_CODE;
    int i = 0;
    unsigned char *pTest[MAX_NUM_AT91_DEVICES] = {0};
    AT91BOOT_HANDLE_TYPE h_handle = 0;

    if (FAILED(CoInitialize(NULL)))
    {
        Debug::trace("UsbManager.Attach", HobString("scanAndGoAtmelBootDevice: failed to initialize COM."));
        return st;
    }

    try
    {
        int uValue, error_code;
        bool bDeviceFound = false;

        // COM Object Creation
        IAT91BootDLLPtr pAT91BootDLL(__uuidof(AT91BootDLL));

        // MANDATORY: Allocate memory for each string in the table
        for (i=0; i<MAX_NUM_AT91_DEVICES; i++)
            pTest[i] = (unsigned char *)malloc(100);

        // Scan for connected AT91 devices
        pAT91BootDLL->AT91Boot_Scan((unsigned char *)pTest);

        // Examine the VID/PID of each device.
        // When the matching VID/PID is found, download the firmware.

        int nDeviceIndex = 0;
        
        while (!bDeviceFound && (nDeviceIndex < MAX_NUM_AT91_DEVICES))
        {
            // Open next Device in the list
            pAT91BootDLL->AT91Boot_Open(pTest[nDeviceIndex++], &h_handle);

            if (h_handle != 0)
            {
                Debug::trace("AtmelAT91Updater", HobString("scanAndGoAtmelBootDevice: AT91Boot_Open succeeded"));

                pAT91BootDLL->AT91Boot_Read_Int(h_handle, &uValue, 0xFFFFF240, NULL);

                if ((uValue & DBGU_CIDR_MASK) == (SAM7S256_DBGU_CIDR & DBGU_CIDR_MASK))
                {
                    short vendId, prodId;

                    pAT91BootDLL->AT91Boot_Read_Short(h_handle, &vendId, AT91_CMD_ADDR_VENDOR_ID, &error_code);
                    pAT91BootDLL->AT91Boot_Read_Short(h_handle, &prodId, AT91_CMD_ADDR_PRODUCT_ID, &error_code);

                    if ( ((vendId == myVid) && (prodId == myPid)) )
                    {
                        int startAddr = AT91_FIRMWARE_START_ADDRESS;

                        Debug::trace("AtmelAT91Updater", HobString("scanAndGoAtmelBootDevice: found device. starting firmware at address = ") + Hex::toString((ULONG)startAddr));

                        pAT91BootDLL->AT91Boot_Go(h_handle, startAddr, &error_code);

                        if (error_code == AT91C_BOOT_DLL_OK)
                        {
                            Debug::trace("AtmelAT91Updater", HobString("scanAndGoAtmelBootDevice: successful start printer firmware") );
                            st = OK;
                            bDeviceFound = true;
                        }
                        else
                        {
                            Debug::trace("AtmelAT91Updater", HobString("scanAndGoAtmelBootDevice: failed to start printer firmware") );
                            break;
                        }
                    }
                    else
                    {
                        //Debug::trace("AtmelAT91Updater", HobString("unrecognized product id ") + HobString(Hex::toString((ULONG)prodId)));
                    }
                }
                else
                {
                    //Debug::trace("AtmelAT91Updater", HobString("scanAndGoAtmelBootDevice::failed to find device") );
                }

                // close device instance handle
                pAT91BootDLL->AT91Boot_Close(h_handle);
            }
            else
            {
                //Debug::trace("AtmelAT91Updater", HobString("scanAndGoAtmelBootDevice:: failed to open device instance") );
            }
        }

        // free the allocated memory 
        for (i=0; i<MAX_NUM_AT91_DEVICES; i++)
        {
            if (pTest[i] != 0)
                free (pTest[i]);
        }
    }
    catch (...) //(const _com_error& Err)
    {
        Debug::trace("AtmelAT91Updater", HobString("caught exception") );
    }

    CoUninitialize();
    return st;
}

