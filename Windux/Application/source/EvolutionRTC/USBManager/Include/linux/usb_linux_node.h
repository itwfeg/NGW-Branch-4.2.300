#ifndef USB_LINUX_NODE_H
#define USB_LINUX_NODE_H

//#define HOB_DIR_DIVIDER         '/'
//#define HOB_DEV_BASE_DIR        "/dev/bus/usb/"

class IUsbCodeUpdater;

class CUsbLinuxNode : public CUsbNode
{
    public:

        CUsbLinuxNode();
        ~CUsbLinuxNode();

        USBD_NODE_ID Initialize(void *device);
        bool isValid() const;

    private:
        friend class UsbManagerLinux;

        libusb_config_descriptor* dev_config;
};


#endif  // USB_LINUX_NODE_H
