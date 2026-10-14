/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef RFIDMANAGER_H
#define RFIDMANAGER_H


#include "CEvolMsgReceiver.h"
#include "CHobartTimer.h"
#include "Tiris.h"


#include "util/thread/mutex.h"
#if defined( __linux__ )
    #include <termios.h>
    #include <unistd.h>
#endif


class RFIDManager : public Hobart::CEvolMsgReceiver, public Util::CRunnableClass, public Hobart::CHobartTimer
{
	public:

		void init();

		void connect(LPCWSTR comPort);
		void disconnect();

		virtual void processEvent( Util::CSmartPtr< EvolMessageWrapper> & msg ); //lcg rev 0.1
		virtual void timeoutProcess( __int32 timer_id );

		Util::CSmartPtr< EvolMessage> processEventExternal(Util::CSmartPtr< EvolMessageWrapper> & msg );

		static RFIDManager *getInstance();
		static void deleteInstance();

		UINT8 CalculateBCC(UINT8 * buffer, DWORD len);


	private:

        #define COM_ERROR 0xFFFFFFFF
        #define TERMINATE_THREAD 0xFFFFFFFE
        #define COM_TIMED_OUT 0xFFFFFFFD
        #define COM_BAD_BCC 0xFFFFFFFC
        #define COM_SUCCESS 0

        #define OPEN_COM_RETRY_COUNT            (5)

    #if defined(PAL_WIN32_RUN)
        #define PAL_INVALID_COM_HANDLE_VALUE    (INVALID_HANDLE_VALUE)

        #define PAL_OVERLAPPED                  OVERLAPPED
        #define PAL_POVERLAPPED                 LPOVERLAPPED

    #elif   defined( __linux__ )
        #define PAL_INVALID_COM_HANDLE_VALUE    (-1)

        // VS8/VC/PlatformSDK/Include/WinBase.h
        typedef struct _PAL_OVERLAPPED          // We are not going to use it in Linux
        {
        #if 0
            union
            {
                struct
                {
                    DWORD   Offset;
                    DWORD   OffsetHigh;
                };
                PVOID   Pointer;
            };
        #endif
            HANDLE  hEvent;
        }   PAL_OVERLAPPED, *PAL_POVERLAPPED;

    #else
        #error "Define error handle value & OVERLAPPED structure"
    #endif

		enum RFIDManagerTimer
		{
			NoIdTimer = 1
		};

		UINT8 readSingleBuffer[5];
		UINT8 readContinuousNormalBuffer[24];
		UINT8 readContinuousLineBuffer[24];
		UINT8 requestVersionBuffer[5];

		UINT8 currentId_[SIZE_ID_BYTES];
		UINT8 TIRIS_SoftwareVersion_;	//RFID modules software version number

		static const UINT8 NULLID_[SIZE_ID_BYTES];
    #if defined(PAL_WIN32_RUN)
		DCB commProperties_;
		HANDLE hComPort_;
    #else
        struct termios commProperties_;
        int hComPort_;
    #endif
		Util::CSmartPtr<Util::CTLSThread>	RFIDManagerThread_;
		bool noIdTimerStarted_;
		DWORD noIdDebounceOutCntr_;
		DWORD noIdDebounceInCntr_;
		unsigned long debounceTime_;
		bool debounceTimerComplete_;
		bool online_;

		bool tracing_;
		bool tracingRun_;
		bool tracingEvent_;
		bool tracingError_;


		/* --- Constructors and destructor --- */
		RFIDManager();
		// Do not allow the default copy constructor.
		RFIDManager(const RFIDManager&);
		~RFIDManager();


		/* --- Instance Function Members --- */
		// Do not allow the default assignment operator.
		RFIDManager operator=(const RFIDManager& rhs);

		virtual DWORD Run();

		DWORD WriteBytesBlocking(DWORD bytesToWrite, UINT8* buffer, PAL_POVERLAPPED pOVL, DWORD *pBytesWrote);
		void ProcessReadPacket(BYTE* buffer, DWORD bytesRead);
		DWORD ReadPacket(UINT8* buffer, DWORD bufferLen, PAL_POVERLAPPED pOVL, DWORD *pTotalBytesRead);
		DWORD ReadBytesBlocking(DWORD bytesToRead, UINT8 * buffer, PAL_POVERLAPPED pOVL, DWORD *pBytesRead);

		bool CheckBCC(BYTE* buffer, DWORD bytesRead);
    #if defined(PAL_WIN32_RUN)
		std::string getLastErrorMessage(DWORD error);
    #endif
        void PAL_showError(const std::string& text, PAL_ERRNO_TYPE error_no = PAL_absclass::PAL_getLastError());

		/* --- Class Data Members --- */
		// Singleton data members
		static RFIDManager *_instance_;
		static util_thread::Mutex<> _singletonMutex_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //RFIDMANAGER_H




