#if !defined CUsbPipe_h
#define CUsbPipe_h

#if defined( __linux__ )
#include "PAL_platformheader.h"
#endif

#include "SUsbDescriptors.h"
#include "CSmartPtr.h"
#include "CTLSThread.h"
#include "CCriticalSection.h"

class CUsbCommBase;
class CUsbPipe : public Util::CRunnableClass
{
    public:
        enum EvolPipeType
        {
            InPipe      = 1,
            OutPipe,
            BulkPipe,
            UnknownPipe,
            InvalidPipe,
        };

        enum EvolResult
        {
            TimeOut                     = 1,
            PipeNotExist,
            CorruptEvolMsgHeader
        };

        typedef enum
        {
        	NO_Protocol,
        	LSM_Protocol,
        	HLX_Access_Protocol,
        	Fastrack_Protocol
        }HobProtocol;

#if defined ( __linux__ )
        CUsbPipe( CUsbCommBase * base, const libusb_endpoint_descriptor * descs, uint32_t pipe_no );
#else
        CUsbPipe( CUsbCommBase * base, SUsbDescriptors * descs, uint32_t pipe_no );
#endif
        virtual ~CUsbPipe();
        EvolPipeType GetEvolPipeType();
        void sendUsbBulkMessage( const char * msg, uint32_t length );
        void cancelReadingWriting();
        void sendUsbInterruptMessage( const char * msg, uint32_t length, DWORD & result, bool & is_custom_code );
        void startPipe();
        HANDLE getPipeHandle() { return pipeHandle; }
        HANDLE getThreadHandle() { return readWriteThread->GetHandle(); }
        void closePipeHandle();
        uint32_t getPipeNumber() { return pipe_no; }

        void setPipeProtocol(unsigned short productID);

    private:
        DWORD Run();
        void send( const char * msg, uint32_t length, bool is_bulk_msg, DWORD & result, bool & is_custom_code );
        void receive();

    private:
        HANDLE  pipeHandle;
        CUsbCommBase * usbCommManager;
#if defined ( __linux__ )
        Util::CTLSThread       *readWriteThread;
        libusb_device_handle    *handle;
        //libusb_device        *device;
        const libusb_endpoint_descriptor *ept;
        CUsbPipe *pipes;
#else
        Util::CSmartPtr<Util::CTLSThread>       readWriteThread;
        PUSB_ENDPOINT_DESCRIPTOR endPointDesc;
#endif
        uint32_t pipe_no;
#if defined ( __linux__ )
        pthread_mutex_t *usb_mutex;
#endif
        static Util::CSmartCriticalSection  criticalSection;

        EvolPipeType evolPipeType;
        Util::CMSGEvent cancelIoEvent;
        Util::CMSGEvent bulkSendEvent;
        const char * outgoingBulkMessages;

        uint32_t outgoingBulkMessageLength;

        HobProtocol pipeProtocol_;

        static const int HOB_LSMN_PROD_ID 				= 0x0001;
		static const int HOB_ACCESS_WRAPPER_PROD_ID 	= 0x0300;
		static const int HOB_ACCESS_WRAPPER_BL_PROD_ID	= 0x8300;
		static const int HOB_ACCESS_WEIGHER_BL_PROD_ID 	= 0x8400;
		static const int HOB_ACCESS_WEIGHER_PROD_ID 	= 0x0400;
		static const int HOB_ACCESS_PRINTER_BL_PROD_ID 	= 0x8200;
		static const int HOB_ACCESS_PRINTER_PROD_ID 	= 0x0200;
		static const int HOB_HLX_APC_BL_PROD_ID			= 0x8204;
		static const int HOB_HLX_APC_PROD_ID			= 0x0204;
		static const int HOB_ACCESS_ARM_WGH_BL_PROD_ID 	= 0x8401;
		static const int HOB_ACCESS_ARM_WGH_PROD_ID 	= 0x0401;
		static const int HOB_ACCESS_ARM_PTR_BL_PORD_ID	= 0x8205;
		static const int HOB_ACCESS_ARM_PRINTER_PROD_ID = 0x0205;
		static const int HOB_ACCESS_CLA_PROD_ID		= 0x0500;
		static const int HOB_ACCESS_CLA_BL_PROD_ID	= 0x8500;
		static const int HOB_FASTRACK_FPC				= 0x0501;

#if defined ( __linux__ )
        static const uint32_t READ_TIME_OUT = 3000;
#else
        static const uint32_t READ_TIME_OUT = 300000;		//	Per Aaron Swift
#endif
        static const uint32_t OUT_WRITE_TIME_OUT = 2000;	//	Per Aaron Swift
        static const uint32_t BULK_WRITE_TIME_OUT = 15000;

        DWORD Run_InPipe();
        DWORD Run_BulkPipe();

        // MsgTimeout specifies the number of milliseconds that the USB message
        // must be completed in, once started. The value can be dynamically adjusted
        // at runtime to allow for longer device response times e.g. when downloading
        // device firmware.  It defaults to the XXX_TIME_OUT values specified above,
        // corresponding to the selected pipe type.

        DWORD MsgTimeout;  // milliseconds
#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif

};

#endif //#if !defined CUsbPipe_h
