#ifndef ATMELDEVICEHANDLER_H
#define ATMELDEVICEHANDLER_H


#ifdef __linux__

#include <libusb-1.0/libusb.h>

#else
#define RPC_NO_WINDOWS_H
#include "smart_crit_sec.h"
#import "../CodeUpdater/AtmelAT91/AT91Boot_DLL.tlb" rename_namespace("AT91BOOTDLL_Lib")
#endif // __linux__

#include "EvolMessage.h"
#include "UsbManager.h"
#include "CHobartTimer.h"
#include "util/thread/mutex.h"

#include <mutex>

/*
#ifndef UDEV_PATCH
#define UDEV_PATCH
#endif
*/
class AtmelDeviceHandler  : public Hobart::CEvolMsgReceiver, public Hobart::CHobartTimer
{
    public:

    private:
        // singleton - private constructor
        AtmelDeviceHandler();
        static AtmelDeviceHandler *instance;
		static util_thread::Mutex<> instanceMutex_;

    public:
        static AtmelDeviceHandler* getInstance();
        ~AtmelDeviceHandler();

        // override method in CEvolMsgReceiver
        virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg );

        // overrides method CHobartTimer:ITimerHandler
        virtual void timeoutProcess( int32_t timer_id );

        void DeviceArrival(USBD_NODE_ID usbNode);
        void DeviceRemoval(USBD_NODE_ID usbNode);
		void ProcessDevice( USBD_NODE_ID usbNodeId, UINT8 bus, UINT8 address );
		void DeviceListArrival();
		bool isIdle();
        void cleanMap();

    private:
		#ifdef __linux__
        typedef libusb_device_handle * AT91_DEVICE_HANDLE;
		#else
        typedef int AT91_DEVICE_HANDLE;
		#endif

        typedef struct _PrivateMsgTag
        {
            UINT32 magic;
            enum opEnum_t {Init, Deinit, Arrival, Removal, Continue, Process, ListArrival} op;
            USBD_NODE_ID usbnode;
            UINT8 bus;
            UINT8 address;
        } PrivateMsg_t;

        static const UINT32 msg_magic_value = 0xdeadbeef;

        typedef struct 
        {
            UINT16 VendorId;
            UINT16 ProductId;
            UINT8  HardwareVerMajor; // bcd
            UINT8  HardwareVerMinor; // bcd
			UINT16 SoftwareVerMajor;
			UINT16 SoftwareVerMinor;
            UINT16 checksum;
        } AT91ProductInfo;
        AT91ProductInfo ProductInfo;

        typedef struct
        {
        	USBD_NODE_ID id;
	        UINT8 bus;	        //which bus device is on 
			UINT8 address;		//location of the device on the bus
        }AT91DeviceLoc;
        
        std::vector<AT91DeviceLoc> deviceList;
 
        bool HandlePrivateMessage(Util::CSmartPtr< EvolMessageWrapper> & msg);
        bool HandlePublicMessage(Util::CSmartPtr< EvolMessageWrapper> & msg);
        void SendPrivateMessage(PrivateMsg_t::opEnum_t op, USBD_NODE_ID node=0, UINT8 bus=0, UINT8 addr=0);

		bool handleDevicesOnBus();

        // Detach the nodeId from the USBManager
        void detachDevice(USBD_NODE_ID nodeId);

        // Scan the system for all AT91 devices, placing the device paths in DevList.
        int ScanDevices();

        // Read the product-specific data which is preprogrammed on-chip
        bool Identify(AT91_DEVICE_HANDLE h, AT91ProductInfo& info);

        void Process( USBD_NODE_ID id, UINT8 bus, UINT8 addr );
        // init HobVersion struct and request peripheral code update from VersionManager
        // VersionManager responds with PeripheralUpdateCode (or timeout)
        //      PeripheralRequestCodeUpdate reqCodeUpdate(usbNodeId, hardwareVersion, HobVersion(HobVersion::XXXXXXX, 0, 0));
        //      dispatchEvent(&reqCodeUpdate);
        bool RequestCodeUpdate(AT91ProductInfo& info);


        // verify the firmware file exists etc
        bool VerifyFirmwareFile(std::string& fwfilename);

        // read device flash into a buffer
        bool ReadDeviceFirmware(AT91_DEVICE_HANDLE h, char * flashBuf, UINT32 nBytes);

        // Download application firmware to device flash
        bool DownloadFirmware(AT91_DEVICE_HANDLE h, std::string& fwfilename);

        // Launch the device's "application" firmware 
        // causes bus detach and re-enumeration
        bool LaunchFirmware(AT91_DEVICE_HANDLE h);

		void handleResetFPCComponent();

#if !defined ( __linux__ )
        // init/deinit COM
        bool InitializeCOM();
        void UninitializeCOM();
#else
        void AT91Boot_Write_Int(AT91_DEVICE_HANDLE fd, int value, int address, int *error);
        void AT91Boot_Read_Int(AT91_DEVICE_HANDLE fd, int *value, int address, int *error);
        void AT91Boot_Read_Short(AT91_DEVICE_HANDLE fd, short *value, int address, int *error);
        void AT91Boot_Read_Byte(AT91_DEVICE_HANDLE fd, unsigned char *value, int address, int *error);
        void AT91Boot_SAM7S256_Send_Flash(AT91_DEVICE_HANDLE h, int start_addr, unsigned char* fwfileBuf, int fwLength, int *error_code);
        void AT91Boot_Go(AT91_DEVICE_HANDLE fd, int start_addr, int *error);

#endif

        // get the next path from DevList
        char * GetNextDevicePath();

        // send a private message to make processEvent continue running
        void ContinueProcessing();

        // operate the finite state machine
        void RunFSM();

        // set HobVersion based on AT91ProductInfo
        void SetVersionInfo(AT91ProductInfo& info, HobVersion& hwver, HobVersion& swver);
#ifdef UDEV_PATCH
        void scanForHobartDevices();
#endif
        void ReportStatusToVersionManager(VersionManagerDevError code, bool removingDialog = false);

#if defined ( __linux__ )
        AT91_DEVICE_HANDLE OpenDevice(USBD_NODE_ID nodeId);
#else
        AT91_DEVICE_HANDLE OpenDevice(char * devpath);
#endif
        void CloseDevice(AT91_DEVICE_HANDLE h);

        Util::CSmartCriticalSection m_cs;

        std::recursive_mutex mutex;

        bool bWaitTimeout;
        bool isPrepack;
        int identifyFailureCount;
        bool processingDevice;
        USBD_NODE_ID usbNodeId;
        int  nRemovalTimeout;
        bool bGotCodeUpdate;
        bool bNodesRemoved;
        bool needsUpdate;

		int hobDevice;

#ifdef UDEV_PATCH
        // int hobDevice;		// this was hidden in this UDEV_PATCH but its used everywhere in SetVersionInfo. So I moved it.
        bool patchComplete;
        //static const unsigned int UdevPatchTimeoutLimit=3000;
        static const unsigned int UdevPatchTimeoutLimit=10000;
#endif

        DWORD dwTimeoutStart;
        static const unsigned int WaitTimerDuration=2000;
        static const unsigned int RemovalTimerDuration=500;
        static const unsigned int RemovalTimeoutLimit=10000;
        
        enum 
        {
            WaitTimerId,
            RemovalTimerId
#ifdef UDEV_PATCH
			,PatchTimerId
#endif
        };

#if !defined ( __linux__ )
        // true = COM has been initialized
        bool bComInitialized;
        AT91BOOTDLL_Lib::IAT91BootDLLPtr pDll;
#endif

        // handle to AT91 device currently open
        AT91_DEVICE_HANDLE hOpenDevice;

        // the currently attached usb nodes
        std::vector<USBD_NODE_ID> usbNodeList;
        typedef std::vector<USBD_NODE_ID>::iterator usbNodeListIter;

        // number of simultaneously connected AT91 devices supported
        static const int MAX_NUM_AT91_DEVICES=10;

#if !defined ( __linux__ )
        char * DevListPtr[MAX_NUM_AT91_DEVICES];
        char * DevList[MAX_NUM_AT91_DEVICES];
        char * DevPath;
#else
        std::vector<std::string> Devs;
#endif

        // name (full path) of the firmware file for the current device
        std::string FirmwareFilename;

        // number of AT91 devices found during scan
        int nDevices;

        // index of current DevList entry
        int DevListIndex;

        typedef enum
        {
            // processing states
            S_IDLE=0
                ,S_SCAN
                ,S_IDENT
                ,S_WAITVER
                ,S_UPDATE
                ,S_DETACH
                ,S_PROCESS
#ifdef UDEV_PATCH
                ,S_UDEV_PATCH
#endif
        } ProcState_t;

        ProcState_t procState;
        const char* GetProcStateName(ProcState_t s);


        // DBGU_CIDR = Debug unit Chip ID Register 
        static const unsigned int DBGU_CIDR_ADDR=0xFFFFF240;

        // mask unwanted bit fields in CIDR - SRAMSIZ and VERSION
        //static const unsigned int DBGU_CIDR_MASK=0xFFF0FFE0;
#if defined ( __linux__ )
        /*currently only revisions A and B are supported at this time. 4/21/11
         *Programming algorithm changed for revision C. These changes yet to be identified. -- ats */

        //chip  revision A
        static const unsigned int SAM7S256A_DBGU_CIDR = 0x270D0940;
        //chip revision B
        static const unsigned int SAM7S256B_DBGU_CIDR = 0x270B0941;
        //chip  revision C
        static const unsigned int SAM7S256C_DBGU_CIDR = 0x270B0942;
        //chip  revision D
        static const unsigned int SAM7S256D_DBGU_CIDR = 0x270B0943;

        typedef enum
        {
        	NoRevision,
        	Revision_A,
        	Revision_B,
        	Revision_C,		//	Last buy date was 4/1/2013 according to the Atmel Product Obsolescence Info page.
			Revision_D
        }ChipRevision_t;

        ChipRevision_t chipRev;

#else
        // mask unwanted bit fields in CIDR - SRAMSIZ and VERSION
        static const unsigned int DBGU_CIDR_MASK_2=0xFFF0FFE0;

        // DBGU_CIDR value for SAM7S256 devices
        static const unsigned int SAM7S256_DBGU_CIDR=0x270B0940;
#endif
        // EFC_FMR register must be programmed 
        // prior to writing data to on-chip flash.
        // Value assumes target is running at 48MHz.
        static const unsigned int EFC_FMR_ADDR=0x00480100;
        static const unsigned int EFC_FMR_VALUE=0xFFFFFF60;


        // offsets for AT91 on-chip product-specific data 
        // stored in first page of the third flash sector.
        static const unsigned int SAM7S256_CMD_ADDR_VENDOR_ID	=0x00108000;  	// 16 bits, binary
        static const unsigned int SAM7S256_CMD_ADDR_PRODUCT_ID	=0x00108002; 	// 16 bits, binary
        static const unsigned int SAM7S256_CMD_HWVERSION_MAJOR	=0x00108004; 	// 8 bits, BCD encoded
        static const unsigned int SAM7S256_CMD_HWVERSION_MINOR	=0x00108005; 	// 8 bits, BCD encoded
        static const unsigned int SAM7S256_CMD_SWVERSION_MAJOR	=0x0013FFE0; 	// 8 bits, BCD encoded
        static const unsigned int SAM7S256_CMD_SWVERSION_MINOR	=0x0013FFE2; 	// 8 bits, BCD encoded
        static const unsigned int SAM7S256_CMD_CHECKSUM			=0x00108006;    // 16 bits, binary

        // entry point of application firmware.
        static const unsigned int SAM7S256_FIRMWARE_START_ADDRESS=0x00108200;
        static const unsigned int SAM7S256_FIRMWARE_FLASH_ADDRESS=0x00008200;
        //static const unsigned int SAM7S256_FIRMWARE_FLASH_ADDRESS=0x00008000;

        //static const unsigned int SAM7S256_FLASH_SIZE = (256*1024)-(SAM7S256_FIRMWARE_FLASH_ADDRESS);
        static const unsigned int SAM7S256_FLASH_SIZE = (256*1024);

        // FSM internal timing 
        static const unsigned int AT91_SCAN_HOLDOFF_INTERVAL=2000;
        static const unsigned int AT91_DEVICE_HOLDOFF_INTERVAL=1000;

#if defined ( __linux__ )
		/*Linux only: this replicates what the SAM-BA library in Windows sends to the device before
		the target image is sent. -- ats*/  
		static const unsigned char bulkInAddress = 130;
		static const unsigned char bulkOutAddress = 1;

        SUsbDescriptors *pMyDescriptors;

        void AT91Boot_SAM7S256_Send_LoaderImage(AT91_DEVICE_HANDLE h, int *error);

		static const unsigned int EFC_FMR_STATUS				= 0xFFFFFF68;
		static const unsigned int SAM7S256_LOADER_IMAGE_ADDRESS = 0x00201600;	//ram location for loader image 
		static const unsigned int SAM7S256_Page_Counter_Address = 0x00201500;	//ram location for page counter
        static const unsigned int SAM7S256_IMAGE_BUFF_ADDRESS	= 0x00201400;	//ram location for image buffer
		static const unsigned int SAM7S256_PAGE_SIZE			= 0x100;		//256 decimal
		static const unsigned int SAM7S256_LOADER_IMAGE_SIZE	= 1104;
		static const unsigned int SAM7S256_START_PAGE_OFFSET	= 0x82;			//pageOffset = startAddress / 256	=  0x8200 / 0x100
		static const unsigned int SAM7S256_MAX_PAGES			= 0x400;		// 0 -> 0x3ff = 1024 pages of flash
		static const unsigned int SAM7S256_GO_SIZE				= 10;

		static const unsigned char loaderImage[SAM7S256_LOADER_IMAGE_SIZE];
#endif


#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};

#endif // ATMELDEVICEHANDLER_H
