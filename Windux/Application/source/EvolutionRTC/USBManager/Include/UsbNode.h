#ifndef USBNODE_H
#define USBNODE_H

#include "evolution.h"
#include "SUsbDescriptors.h"
#include "device_event.h"

class IUsbCodeUpdater;

class CUsbNode
{
    public:

        CUsbNode();
        virtual ~CUsbNode();

#if defined ( __linux__ )
        virtual USBD_NODE_ID Initialize(void *UNUSED(device))=0;
#else
        virtual USBD_NODE_ID Initialize(DEVICE_INFO UNUSED(dev_i))=0;
#endif

        virtual bool isValid() const {return true;}

        const std::string& getDevicePath();
        const std::string& getDeviceInstanceId();
        const std::string& getHardwareId();
        const std::string& getFriendlyName();
        const std::string& getMfrName();
        const std::string& getServiceName();

        USHORT getProductId();
        USHORT getVendorId();
        USHORT getDeviceVersion();
#if defined ( __linux__ )
        uint8_t getInterfaceClass();
        uint8_t getDeviceProtocol();
        uint8_t getiInterface();
        uint8_t getBusNumber() { return bus; }
        uint8_t getDeviceAddress() { return address; }
        void show();
#endif

        SUsbDescriptors& getDescriptors();

    protected:

        friend class UsbManager;

#if defined ( __linux__ )
        friend class UsbManagerLinux;
#else
        friend class UsbManagerWin32;
#endif

        USBD_NODE_ID createNodeId(char * str);
        unsigned char DevicePath[MAX_PATH+1];
        unsigned char DeviceInstanceId[MAX_PATH+1];

        std::string strPath;
        std::string strInstance;

        std::string DeviceDesc;     //SPDRP_DEVICEDESC
        std::string HardwareId;     //SPDRP_HARDWAREID
        std::string FriendlyName;   //SPDRP_FRIENDLYNAME
        std::string MfrName;        //SPDRP_MFG
        std::string ServiceName;    //SPDRP_SERVICE

        USHORT idProduct;           // from HardwareId
        USHORT idVendor;            // from HardwareId
        USHORT bcdDevice;           // from HardwareId
        USHORT nInterface;          // from HardwareId
#if defined ( __linux__ )
        uint8_t bInterfaceClass;
        uint8_t bInterfaceProtocol;
        uint8_t biInterface;
        uint8_t bus;
        uint8_t address;
#endif

        enum
        {
            UNKNOWN   // unknown state
                ,DETECTED  // device was detected
                ,UPDATING  // under control of Updater
                ,RUNNING   // under control of RTC
                ,DETACHED  // logically detached by RTC or Updater
                ,REMOVED   // removal notification was received
                ,ZOMBIE    // terminal failure state (unknown problem)
                ,STORAGE   // USB storage device
                ,HID
                ,HID_KEYBOARD
                ,HID_MOUSE
                ,HID_BARCODE
                ,UNMANAGED // device is not managed by this service
        } state;

        bool bDetected;         // housekeeping flag
        SUsbDescriptors desc;   // USB descriptors
        IUsbCodeUpdater* pUpdater; // code updater
        HANDLE hDevNotify;      // handle for device notifications
        bool bServiceObject;    // true if comm manager active
};


#endif //USBNODE_H

