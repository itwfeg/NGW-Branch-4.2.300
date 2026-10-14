#ifndef ATMELDEVICEHANDLERLINUX_H
#define ATMELDEVICEHANDLERLINUX_H

#include "EvolMessage.h"
#include "UsbManager.h"

class AtmelDeviceHandler  : public Hobart::CEvolMsgReceiver
{
public:

private:
    // singleton - private constructor
    AtmelDeviceHandler();
    static AtmelDeviceHandler *instance;

public:
    static AtmelDeviceHandler* getInstance();
    ~AtmelDeviceHandler();

    // overrides for CEvolMsgReceiver
    virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

    void DeviceArrival(USBD_NODE_ID usbNode);
    void DeviceRemoval(USBD_NODE_ID usbNode);

private:
    typedef int AT91_DEVICE_HANDLE;
    typedef struct
    {
        USHORT VendorId;
        USHORT ProductId;
        UCHAR  HardwareVerMajor; // bcd
        UCHAR  HardwareVerMinor; // bcd
		USHORT SoftwareVerMajor;
        USHORT SoftwareVerMinor;
		USHORT checksum;
    } AT91ProductInfo;

    typedef struct _PrivateMsg345234
    {
        int magic;
        enum {Init, Deinit, Arrival, Removal} op;
        USBD_NODE_ID            node;
    } PrivateMsg_t;

    static const int msg_magic_value = 0xdeadbeef;

    bool HandlePrivateMessage(Util::CSmartPtr< EvolMessageWrapper> & msg);


private:
	bool timedOut_;
    DWORD responseTimeoutTime_;
	DWORD timeoutDuration_;	
    static const DWORD defaultTimeoutDuration = 10000;
    AT91_DEVICE_HANDLE hDevice;

    // number of simultaneously connected AT91 devices
    static const int MAX_NUM_AT91_DEVICES=6;
    
    unsigned char m_DevList[MAX_NUM_AT91_DEVICES*100];

    // DBGU_CIDR = Debug unit Chip ID Register
    static const unsigned int DBGU_CIDR_ADDR=0xFFFFF240;

    // mask unwanted bit fields in CIDR - SRAMSIZ and VERSION
    static const unsigned int DBGU_CIDR_MASK=0xFFF0FFE0;

    // DBGU_CIDR value for SAM7S256 devices
    static const unsigned int SAM7S256_DBGU_CIDR=0x270B0940;

    // Device               DBGU_CIDR value (w/o mask)
    // AT91SAM7S16          0x27050240
    // AT91SAM7S161         0x27050241
    // AT91SAM7S32          0x27080340
    // AT91SAM7S321         0x27080342
    // AT91SAM7S64          0x27090540
    // AT91SAM7S128         0x270A0740
    // AT91SAM7S128 Rev B   0x270C0741
    // AT91SAM7S256         0x270B0940
    // AT91SAM7S256 Rev B   0x270D0941
    // AT91SAM7S512         0x270B0A40

    // offsets for AT91 on-chip product-specific data
    // stored in first page of the third flash sector.
    static const unsigned int AT91_CMD_ADDR_VENDOR_ID=0x108000;  // 16 bits, binary
    static const unsigned int AT91_CMD_ADDR_PRODUCT_ID=0x108002; // 16 bits, binary
    static const unsigned int AT91_CMD_HWVERSION_MAJOR=0x108004; // 8 bits, BCD encoded
    static const unsigned int AT91_CMD_HWVERSION_MINOR=0x108005; // 8 bits, BCD encoded
    static const unsigned int AT91_CMD_SWVERSION_MAJOR=0x13FFE0;
	static const unsigned int AT91_CMD_SWVERSION_MINOR=0x13FFE2;
	static const unsigned int AT91_CMD_CHECKSUM=0x108006;        // 16 bits, binary

    // entry point of application firmware.
    static const unsigned int AT91_FIRMWARE_START_ADDRESS=0x108200;


#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};


#endif // ATMELDEVICEHANDLERLINUX_H

