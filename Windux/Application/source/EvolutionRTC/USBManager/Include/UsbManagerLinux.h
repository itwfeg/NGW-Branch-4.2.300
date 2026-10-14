#ifndef USBMANAGERLINUX_H
#define USBMANAGERLINUX_H

#include "device_event.h"
#include "HIDDeviceEvent.h"
#include "UsbNode.h"

#define HOB_THUMBD_DIR          "/dev/HobartThmb"
#define HOB_MAX_THMBD           127
#define HOB_CURR_DIR            "."
#define HOB_PREV_DIR            ".."

//#define DBGU_CIDR_MASK          ~0x000F001F

class UsbManagerLinux : public UsbManager
{
    public:
        UsbManagerLinux();
        ~UsbManagerLinux();
        SUsbDescriptors * getUsbDescs( USBD_NODE_ID  nodeId );
        void HandleDeviceNotification(PEVENT_INFO ev_info, int32_t vid = 0, int32_t pid = 0 );
        int ProgramAtmel(const char *path, const char *filename, bool bForceIt=false);
        bool isBulkStorageDevicePresent() const;

		// SOF-2931
		bool isSambaPresent() const;
        void discovery();
    private:
        void UsbShowDevDesc(struct usb_device_descriptor *desc);
        void UsbShowConfigDesc(struct usb_config_descriptor *dev_config);
        void UsbShowIfaceDesc(struct usb_interface_descriptor *dev_iface);
        void UsbShowEpDesc(struct usb_endpoint_descriptor *dev_ep);
        BOOL findDevice(int32_t vid, int32_t pid,
                struct usb_device_descriptor **dev_desc,
                struct usb_config_descriptor **dev_config,
                struct usb_interface_descriptor **dev_iface,
                struct usb_endpoint_descriptor **dev_ep);
        void getFileFromPath(const char *path, char *file);

        USBD_NODE_ID NodeIdFromDeviceName(const char *devName);
        unsigned char getBusFromDeviceName( const char *pDevName );
        unsigned char getAddressFromDeviceName( const char *pDevName );
        
        int getAT91response(int fd, char *psResponse);
        int parse8(char *ptr);
        int parse16(char *ptr);
        int parse_line(char *line, int *bytes, int *addr, int *num, int *code);
        int sendFile(const int fd, const char *filename);
        int program_atmel(const char *path, const char *filename, bool bForceIt=false);

        int addrpref;
        char psResponse[4096];
        bool addSamba;

        //Non managed device types
        typedef enum
        {
            NM_HID_UNKOWN,
            NM_HID_STORAGE,
            NM_HID_MOUSE,
            NM_HID_KEYBOARD,
            NM_HID_BARSCANNER
        }NMTypes;

        typedef struct 
        {
            NMTypes device;
            USBD_NODE_ID id;
        }NMTrack;
        std::vector< NMTrack > nmList;


#ifdef USE_ACCOUNTANT
        static int _dataType_;
        static std::string _dataTypeName_;
#endif
    protected:
        
        void UpdateDeviceMap();
        CUsbLinuxNode *findMyDevice( int32_t vid, int32_t pid, USBD_NODE_ID nodeId, uint8_t bus = 0, uint8_t address = 0 ); //SOF-4076
        void updateNMList( USBD_NODE_ID id );        
        bool searchForKeyFiles();

		// SOF-2931
		void resetSamba();

};


#endif //USBMANAGERLINUX_H

