#include "usb_win_node.h"
#include "UsbNode.h"
#include <setupapi.h>
#include "Debug.h"
#include "Hex.h"
#include "HobString.h"

#include <algorithm>
#include <string>


/// Query the USB device and initialize the class data members.
USBD_NODE_ID CUsbWinNode::Initialize(DEVICE_INFO dev_i)
{
    PSP_DEVICE_INTERFACE_DETAIL_DATA_A  pIntfcDetailData = NULL;
    PBYTE                               pRegProp = NULL;
    BOOL                                Result = 0;
    ULONG                               Length = 0;
    ULONG                               Required = 0;
    USBD_NODE_ID                        nodeId = 0;

	HANDLE hDevInfo = dev_i.dev_info;
	SP_DEVICE_INTERFACE_DATA devIntfcData = dev_i.dev_if_dat;
	SP_DEVINFO_DATA devInfoData = dev_i.dev_info_dat;

    // Capture the device path by reading a 
    // SP_DEVICE_INTERFACE_DETAIL_DATA_A structure. This is a 
    // 2-stage process. The first call to SetupDiGetDeviceInterfaceDetailA
    // gets the length of the structure into Required. The returned 
    // Result is FALSE indicating error, and if we callled 
    // GetLastError, we should get ERROR_INSUFFICIENT_BUFFER. 
    // In this particular case, we skip all that mumbo jumbo and 
    // just use the darned value.

    Result = SetupDiGetDeviceInterfaceDetailA(
        hDevInfo,       //IN HDEVINFO  DeviceInfoSet,
        &devIntfcData,  //IN PSP_DEVICE_INTERFACE_DATA  DeviceInterfaceData,
        (PSP_DEVICE_INTERFACE_DETAIL_DATA_A) NULL, //OUT PSP_DEVICE_INTERFACE_DETAIL_DATA_A  DeviceInterfaceDetailData..OPTIONAL,
        0,              //IN DWORD  DeviceInterfaceDetailDataSize,
        &Required,      //OUT PDWORD  RequiredSize..OPTIONAL,
        NULL);          //OUT PSP_DEVINFO_DATA  DeviceInfoData  OPTIONAL

    //if (!Result) 
    //{
    //    DWORD error = GetLastError();
    //    DBGASSERT(error==ERROR_INSUFFICIENT_BUFFER);
    //}

    // verify our buffer is big enough to hold the device path
    if ( (Required-sizeof(DWORD)) > sizeof(DevicePath))
    {
        Debug::trace("UsbManager.Attach", HobString("CUsbNode::Initialize: device path too long, skipping"));
        goto InitExit; 
    }

    // Using the length that we just obtained, alloc memory for the struct.
    Length = Required;
    pIntfcDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_A) malloc(Length);

    // As before, we are required to preset the cbSize element prior to the call.
    pIntfcDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A); 

    // Call the function a 2nd time, but pass in the buffer Length.
    // On return, the struct will be populated with the device interface detail.
    // The only element of interest is the DevicePath.

    Result = SetupDiGetDeviceInterfaceDetailA(
        hDevInfo,        //IN HDEVINFO  DeviceInfoSet,
        &devIntfcData,   //IN PSP_DEVICE_INTERFACE_DATA  DeviceInterfaceData,
        pIntfcDetailData,      //OUT PSP_DEVICE_INTERFACE_DETAIL_DATA_A  DeviceInterfaceDetailData..OPTIONAL,
        Length,          //IN DWORD  DeviceInterfaceDetailDataSize,
        NULL,            //OUT PDWORD  RequiredSize..OPTIONAL,
        NULL);           //OUT PSP_DEVINFO_DATA  DeviceInfoData  OPTIONAL

    if (!Result) 
    {
        DWORD error = GetLastError();
        Debug::trace("UsbManager.Attach", HobString("CUsbNode::Initialize: SetupDiGetDeviceInterfaceDetailA failed, error ") + Hex::toString(error));
        goto InitExit;       
    }


    // zero our buffer and copy the device path
    SecureZeroMemory(DevicePath,sizeof(DevicePath));
    memcpy(DevicePath,pIntfcDetailData->DevicePath,Required-sizeof(DWORD));
    strPath = (const char *)DevicePath;
    std::transform(strPath.begin(), strPath.end(), strPath.begin(), (int(*)(int)) toupper);


    // Free the memory used by the interface detail data structure
    // since we have the device path and are finished with it.
    free(pIntfcDetailData);
    pIntfcDetailData = 0;

    // Now, using the same device info set which gave us the
    // device path, get the SP_DEVINFO_DATA struct, which we then
    // use to obtain the unique device instance ID.

    // Preset the cbSize element of the struct prior to API call.
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    // Again, the two-stage query: 1st call gets the length of
    // the device instance ID, the 2nd call gets the actual data.
    Result =  SetupDiGetDeviceInstanceIdA(
                hDevInfo,     // IN HDEVINFO DeviceInfoSet,
                &devInfoData, // IN PSP_DEVINFO_DATA DeviceInfoData,
                (PSTR)NULL,   // OUT PTSTR DeviceInstanceId,
                0,            // IN DWORD DeviceInstanceIdSize,
                &Required     // OUT PDWORD RequiredSize OPTIONAL
                );

    //if (!Result) 
    //{
    //    DWORD error = GetLastError();
    //    DBGASSERT(error==ERROR_INSUFFICIENT_BUFFER);
    //}

    // verify our buffer is at least as big as the device instance id
    if (Required > sizeof(DeviceInstanceId) )
    {
        Debug::trace("UsbManager.Attach", HobString("CUsbNode::Initialize: device instance ID too long, skipping"));
        goto InitExit; 
    }

    // zero our buffer and call the API to load it
    SecureZeroMemory(DeviceInstanceId,sizeof(DevicePath));
    Result =  SetupDiGetDeviceInstanceIdA(
                hDevInfo,                         // IN HDEVINFO DeviceInfoSet,
                &devInfoData,                     // IN PSP_DEVINFO_DATA DeviceInfoData,
                (PSTR)DeviceInstanceId,   // OUT PTSTR DeviceInstanceId,
                sizeof(DeviceInstanceId), // IN DWORD DeviceInstanceIdSize,
                NULL                              // OUT PDWORD RequiredSize OPTIONAL
                );

    if (!Result) 
    {
        DWORD error = GetLastError();
        Debug::trace("UsbManager.Attach", HobString("CUsbNode::Initialize: SetupDiGetDeviceInstanceIdA failed, error ") + Hex::toString(error));
        goto InitExit;       
    }

    strInstance = (const char *)DeviceInstanceId;
    std::transform(strInstance.begin(), strInstance.end(), strInstance.begin(), (int(*)(int)) toupper);

    // Load various strings from the driver's registry key
    // We are primarily interested in the Hardware ID.

    struct _pstr 
    {
        DWORD id;
        std::string* str;
    } propertyString[] =
    {
         {SPDRP_DEVICEDESC,   &DeviceDesc   }
        ,{SPDRP_HARDWAREID,   &HardwareId   }
        ,{SPDRP_FRIENDLYNAME, &FriendlyName }
        ,{SPDRP_MFG,          &MfrName      }
        ,{SPDRP_SERVICE,      &ServiceName  }
    };

    for (int i=0; i < sizeof(propertyString)/sizeof(struct _pstr); i++)
    {
        // Two step process again. Get the registry property buffer size.

        Result = SetupDiGetDeviceRegistryPropertyA(
                    hDevInfo,              //IN HDEVINFO  DeviceInfoSet,
                    &devInfoData,          //IN PSP_DEVINFO_DATA  DeviceInfoData,
                    propertyString[i].id,  //IN DWORD  Property,
                    NULL,                  //OUT PDWORD  PropertyRegDataType..OPTIONAL,
                    NULL,                  //OUT PBYTE  PropertyBuffer,
                    0,                     //IN DWORD  PropertyBufferSize,
                    &Required              //OUT PDWORD  RequiredSize  OPTIONAL
                    );
        
        //if (!Result) 
        //{
        //    DWORD error = GetLastError();
        //    DBGASSERT(error==ERROR_INSUFFICIENT_BUFFER);
        //}

        // Using the length that we just obtained, alloc memory

        Length = Required;
        pRegProp = (PBYTE) malloc(Length);

        Result = SetupDiGetDeviceRegistryPropertyA(
                    hDevInfo,              //IN HDEVINFO  DeviceInfoSet,
                    &devInfoData,          //IN PSP_DEVINFO_DATA  DeviceInfoData,
                    propertyString[i].id,  //IN DWORD  Property,
                    NULL,                  //OUT PDWORD  PropertyRegDataType..OPTIONAL,
                    pRegProp,              //OUT PBYTE  PropertyBuffer,
                    Length,                //IN DWORD  PropertyBufferSize,
                    NULL                   //OUT PDWORD  RequiredSize  OPTIONAL
                    );
     
        if (!Result) 
        {
//            DWORD error = GetLastError();
            //Debug::trace("UsbManager.Attach", HobString("CUsbNode::Initialize: SetupDiGetDeviceRegistryPropertyA failed, error ") + Hex::toString(error));
            //if (ERROR_INVALID_DATA == error)
            {
                // blank the string and keep going
                propertyString[i].str->clear();
                free(pRegProp);
                continue;
            }
            // some other error, fail
            // goto InitExit;       
        }

        // init the class member from the registry property 
        std::string& s = *(propertyString[i].str);
        s = (const char *)pRegProp;
        std::transform(s.begin(), s.end(), s.begin(), (int(*)(int)) toupper);

        // Free the memory used for the registry property
        free(pRegProp);
        pRegProp = 0;
    }


    // At this point, we have successfully read the device path,
    // device instance ID, hardware ID and several other strings. 
    // We now have all the information required to uniquely identify 
    // the USB device and load the appropriate support classes.

    // Parse the HardwareId string to initialize the remaining members.

    idProduct = (USHORT) Hex::longValueOf(HardwareId.substr(HardwareId.find("PID_")+4,4).c_str());
    idVendor  = (USHORT) Hex::longValueOf(HardwareId.substr(HardwareId.find("VID_")+4,4).c_str());
	if (HardwareId.find("REV_") != std::string::npos)
        bcdDevice = (USHORT) Hex::longValueOf(HardwareId.substr(HardwareId.find("REV_")+4,4).c_str());
    else
        bcdDevice = 0;
    if (HardwareId.find("MI_") != std::string::npos)
        nInterface= (USHORT) Hex::longValueOf(HardwareId.substr(HardwareId.find("MI_")+3,4).c_str());
    else
        nInterface = 0;

    state = UNKNOWN;
    desc.buf[0];
    desc.configDesc = 0;
    desc.deviceDesc = 0;
    desc.interfaceDesc = 0;
    desc.endpointDescs.clear();
    pUpdater = 0;

    nodeId = createNodeId((char*) this->DeviceInstanceId);

    hDevNotify = INVALID_HANDLE_VALUE;
    bServiceObject = false;

InitExit:
    if (pIntfcDetailData)
        free(pIntfcDetailData);
    if (pRegProp)
        free(pRegProp);

    return nodeId;

}



