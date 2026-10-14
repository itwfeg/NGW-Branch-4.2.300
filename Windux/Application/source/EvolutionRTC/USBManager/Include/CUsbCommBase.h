#if !defined CUsbCommBase_h
#define CUsbCommBase_h

#include "PAL_platformheader.h"

#if defined ( __linux__ )
#include <libusb-1.0/libusb.h>
#endif

#include "CUsbPipe.h"
#include "CSmartPtr.h"
#include "UsbManager.h"
#include <map>

class CUsbCommBase
{
    public:
        //CUsbCommBase( SUsbDescriptors * descs );
        CUsbCommBase( SUsbDescriptors * descs, USBD_NODE_ID node );
        virtual ~CUsbCommBase();

        virtual void processUsbMessage( std::string & /*msg*/, CUsbPipe::EvolPipeType /*pipe_type*/ ) {};
        virtual void sendBulkMessageCallBack( DWORD /*result*/, bool /*user_defined_error*/ = false ) {};
        virtual void receiveErrorCallBack( CUsbPipe::EvolPipeType /*pipe_type*/, DWORD /*error*/, bool /*user_defined_error*/ = false ) {};

        virtual bool isValidHeader( const char* /*header*/ ) const {return true;}
        virtual bool locateNextHeader( const char* /*msg*/, unsigned int /*msgLen*/, unsigned int& /*address*/ ) const {return false;}

        bool HandlePipeError( CUsbPipe* p, DWORD error );

        std::string& getDevicePath();


#if defined ( __linux__ )
        libusb_device_handle *libusb_handle; //added for debug -- ats
        //for debug only
        long getMyPipeHandle() {return (long)libusb_handle;}
#endif




    protected:
        void createPipes();
        void closePipes();
        void startPipes();
        void startAPipe(CUsbPipe::EvolPipeType pipeType);
        //void stopPipes();

        bool sendUsbBulkMessage( const char * msg, uint32_t length );
        void cancelBulkTransfer();

        void sendUsbInterruptMessage( const char * msg, uint32_t length, DWORD & result, bool & is_custom_code );

        void printUSBError( DWORD result, bool user_defined_error = false );

    private:
        USBD_NODE_ID usbNode;

        // Reverse position to match initialization order in constructor
        SUsbDescriptors * usbDescriptors;
        bool pipesEnabled;

#if defined ( __linux__ )
        bool bRestoreKernelDriver;
        std::map< CUsbPipe::EvolPipeType, CUsbPipe *> pipeMap;
        pthread_mutex_t usbMutex;
        pthread_mutex_t pipeMapCS;
#else
        std::map< CUsbPipe::EvolPipeType, Util::CSmartPtr< CUsbPipe > > pipeMap;
        Util::CSmartCriticalSection  pipeMapCS; 
#endif  // __linux__

#ifdef USE_ACCOUNTANT
        static int dataType;
        static std::string dataTypeName;
#endif
};

#endif //#if !defined CUsbCommBase_h
