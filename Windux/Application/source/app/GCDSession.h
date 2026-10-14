#ifndef GCDSESSION_H
#define GCDSESSION_H

#include "Session.h"
#include "Product.h"
#include "GCDEvent.h"
#include "Numeric.h"
#include "HfcProductUpdateValuesMsg.h"
#include "HobSocket.h"
#include "HobVersion.h"
#include "GCDDevDataMsg.h"
#include "../util/filesys/filename.h"
#include "../util/filesys/directory.h"

// for the USB enumeration
#if !defined( __linux__ )
#include "objbase.h"
#include "winioctl.h"
#include "../../3RDPartySoftware/WinDDK/inc/wxp/usbioctl.h" // MSVC2010
#endif

class APCCommManager;
class Socket;

class GCDSession : public Session, GCDDevDataMsgProvider
{
    public:

		GCDSession(CommMechanism *uiClient = 0, bool enableInput = true);
		~GCDSession();

		void init();
		void ProcessReloadGCDManager();

		void sessionMessageProcess(SessionMessage *msg);

		void updatePrepackAnnunciator(NmState state1);

		void UpdateProductWeightData(HfcProductUpdateValuesMsg &hpuvm);

		void ShutDownSession();

	    int send( EvolMessage *message, int destination, bool shouldDelayAfterSending = true );

		HobString GetData(const Field field) const;
		
		bool getGCDOperational();	// GCDR-183
		bool gcdCanTalk();			// GCDR-205

		void sendProductRelatedPlaylistFilenameToGCD(HobString playlist);
		void clearProductRelatedPlaylistFilenameOnGCD();


	protected:

        virtual void initializationBeforeRun();  // override from AbstractSession
        // Not used right now - virtual void finalizationAfterRun();  // override from AbstractSession


	private:
		// These few structs, etc are used to enumerate the
		// usb devices and we look for ours. The ip
		// address method is not fast enough.

		// If the GCD changes, these MUST change. If they don't it won't take longer
		// to boot but the chance of the missing GCD screen is higher. (probably 100%)
		//	TODO: We should probably be referencing these by way of the UsbManager [CCD - June 2013]
		#define GCDVID 0x128c		//	HOBART_VENDOR_ID	UsbManager.h [CCD - June 2013]
		#define GCDPID 0x0502		//	HOBART_GCD_ID		UsbManager.h [CCD - June 2013]
		
		// Added for GCDR2-183, allows knowing if the GCD is both connected and fully initialized since these states are different on the GCD
		enum GCDInitialized
		{
			NOT_INITIALIZED = 0,
			CONNECTED,			//	ConnectingState_Connected: The socket connection has been made. We can now talk to the GCD.
			INITIALIZED			//	ConnectingState_Complete: The GCD is initialized and ready to go.
		};
		GCDInitialized GCDOperational;

#if defined( __linux__ )
		enum USB_HUB_NODE
		{ 
			UsbHub       = 0,
			UsbMIParent  = 1
		};

		typedef struct _USB_ENDPOINT_DESCRIPTOR
		{
			UCHAR  bLength;
			UCHAR  bDescriptorType;
			UCHAR  bEndpointAddress;
			UCHAR  bmAttributes;
			USHORT wMaxPacketSize;
			UCHAR  bInterval;
		} USB_ENDPOINT_DESCRIPTOR, *PUSB_ENDPOINT_DESCRIPTOR;

		typedef struct _USB_PIPE_INFO
		{
			USB_ENDPOINT_DESCRIPTOR EndpointDescriptor;
			ULONG                   ScheduleOffset;
		} USB_PIPE_INFO, *PUSB_PIPE_INFO;
#endif

		typedef struct DEVICE_DESCRIPTOR 
		{
			UCHAR bLength; 
			UCHAR bDescriptorType; 
			UCHAR bcdUSB[2];
			UCHAR bDeviceClass; 
			UCHAR bDeviceSubClass; 
			UCHAR bDeviceProtocol;
			UCHAR bMaxPacketSize0; 
			UCHAR idVendor[2]; 
			UCHAR idProduct[2];
			UCHAR bcdDevice[2]; 
			UCHAR iManufacturer; 
			UCHAR iProduct;
			UCHAR iSerialNumber; 
			UCHAR bNumConfigurations;
		} DeviceDescription;

		typedef struct HUB_DESCRIPTOR
		{
			UCHAR bDescriptorLength; 
			UCHAR bDescriptorType; 
			UCHAR bNumberOfPorts;
			UCHAR wHubCharacteristics[2]; 
			UCHAR bPowerOnToPowerGood; 
			UCHAR bHubControlCurrent;
			UCHAR bRemoveAndPowerMask[64];
		} HubDescriptor;

		typedef struct NODE_INFORMATION
		{
			USB_HUB_NODE	NodeType;
			HUB_DESCRIPTOR	HubDescriptor;
			BOOLEAN			HubIsBusPowered;
		} NodeInformation;

		typedef struct NODE_CONNECTION_INFORMATION
		{
			ULONG				ConnectionIndex;
			DEVICE_DESCRIPTOR	DeviceDescriptor; 
			UCHAR				CurrentConfigurationValue;
			BOOLEAN				LowSpeed; 
			BOOLEAN				DeviceIsHub;
			UCHAR				DeviceAddress[2];
			UCHAR				NumberOfOpenPipes[4]; 
			UCHAR				ConnectionStatus[4]; 
			USB_PIPE_INFO		PipeList[32];
		} NodeConnectionInformation;

		static bool IsGCDThere( unsigned short int vid, unsigned short int pid );
		static bool EnumerateHostController(HANDLE HostControllerHandle, unsigned short int idVendor, unsigned short int idProduct);
		static bool GetPortData(HANDLE HubHandle, UCHAR PortCount, int HubDepth, unsigned short int idVendor, unsigned short int idProduct);
	
		enum UpdateMethod
		{
			USBMediaManual,
			USBMediaAutomatic,
			FTPMediaManual,
			FTPMediaAutomatic,
			USBSystemManual,
			USBSystemAutomatic,
			FTPSystemManual,
			FTPSystemAutomatic
		};

		enum ErrorCategory
		{
			OS,
			CURL,
			GCD
		};

		enum ShowState
		{
			AlwaysShowError,
			NeverShowError,
			ShowOnlyIfError,
			ShowOnlyIfNotError
		};

		enum AdvertType
		{
			AT_None,
			AT_ProductGraphic1,
			AT_ProductGraphic2,
			AT_ProductGraphic3,
			AT_ProductGraphic4,
			AT_CategoryPromo,		//	Implemented May 2015
			AT_ExpandedText1,		//	Possible future support
			AT_ExpandedText2,		//	Possible future support
			AT_Recipe,				//	Possible future support
			AT_Allergens,			//	Possible future support
			AT_SuggestiveSelling,	//	Possible future support
			AT_Nutridata			//	Possible future support
		};

		class UpdateError
		{
			public:
				bool			isError;
				UpdateMethod	method;
				ErrorCategory	category;
				unsigned int	errorNumber;
				unsigned int	extendedError;
				bool			atBeginning;
				bool			verbose;
				HobString		additionalText;
		};

		HobString getcURLErrorText( unsigned int e );
		HobString getOSErrorText( unsigned int e );
		void showErrorStateToUser( UpdateError* ue, ShowState ss );
		UpdateError updateError;

		// Helper functions for cURL mods
		std::string GetPortProtocolUrl( std::string server, std::string dir, INTERNET_PORT* port, int* protocolType );
		void trimFTPDirSeparators( std::string& s, char charToTrim );
		std::string validateFtpPath(std::string &server, std::string &path, int protocol);
		void fixTheSlashesForFtp( std::string& s );
		bool m_ftpUpdateIsInteractive;

#if !defined( __linux__)
		static DWORD WINAPI waitForRNDISAdapter( LPVOID seconds );
		static IPAddr WINAPI SetLinkLocalIPAddress( DWORD adapterIndex, IPAddr ip );
		static void setLLIP( int seconds );
		// static void RemoveOtherIP( DWORD indx, IPAddr ip );

#endif
		class ChildThread : public Util::CRunnableClass
		{
		public:

			typedef DWORD (GCDSession::*RunMethod)(HANDLE hExitEvent, Util::CTLSThread* pThread, void *pParam);

			ChildThread(GCDSession *pParent, ChildThread::RunMethod fpRun, void *pParam, LPCSTR szName, int priority) :
				m_pParam(pParam), m_fpRun(fpRun), m_pParent(pParent), m_pThread(NULL)
			{
				m_pThread = new Util::CTLSThread(*this, priority, 0, szName, false);
			}

			~ChildThread()
			{
				m_pThread->StopThread();
				m_pThread->WaitForCompletion();
				delete m_pThread;
			}

			DWORD Run() { return (m_pParent->*m_fpRun)(m_ExitEvent, m_pThread, m_pParam); }

			BOOL IsRunning() { return WAIT_TIMEOUT == PAL_absclass::PAL_waitForSingleObj(m_pThread->GetHandle(), 0); }

			BOOL Stop(DWORD dwTimeout) { m_pThread->StopThread(); return m_pThread->WaitForCompletion(dwTimeout); }

			void Start() { m_pThread->RestartThread(); }

			int GetPriority() const { return m_pThread->GetPriority(); }

			void SetPriority(int priority) { m_pThread->SetPriority(priority); }

			void *m_pParam;
			ChildThread::RunMethod m_fpRun;

		private:

			GCDSession *m_pParent;
			Util::CTLSThread* m_pThread;
		};

		class UploadFileInfo;

		enum FileUploadFinishedState
		{
			FileUploadFinishedState_AlreadyExists,
			FileUploadFinishedState_Success,
			FileUploadFinishedState_Fail,
		};

		typedef void (GCDSession::*FileUploadFinishedMethod)(UploadFileInfo* pFileInfo, FileUploadFinishedState finishedState, LPCSTR szErrorDescription);

		class UploadFileInfo
		{
		public:

			util_filesystem::Filename m_localPath;
			DWORD m_dwLocalFileSize;
			DWORD m_dwLocalLastWriteTime;
			util_filesystem::Filename m_remotePath;
			BOOL m_fCheckNew;
			std::string m_address;
			std::string m_ftpUser;
			std::string m_ftpPassword;
			util_filesystem::Filename m_sourcePath;
			int m_finishThreadPriority;
			FileUploadFinishedMethod m_fpFileUploadFinished;
		};

		class RequestFileUpload
		{
		public:
			util_filesystem::Filename m_sourcePath;
			util_filesystem::Filename m_remotePath;
		};

		enum PlaylistUpdateState
		{
			PlaylistUpdateState_None,
			PlaylistUpdateState_Ftp,
			PlaylistUpdateState_Usb
		};

		enum ConnectingState
		{
			ConnectingState_Start,
			ConnectingState_DiscoveryWaiting,
			ConnectingState_DiscoveryDone,
			ConnectingState_Connected,
			ConnectingState_StoreLogoUpdateWaiting,
			ConnectingState_StoreLogoUpdateDone,
			ConnectingState_LoadDisplayXMLWaiting,
			ConnectingState_LoadDisplayXMLDone,
			ConnectingState_LoadPlaylistWaiting,
			ConnectingState_LoadPlaylistDone,
			ConnectingState_Complete,
		};

		enum HeartbeatState
		{
			HeartbeatState_Connecting,
			HeartbeatState_Connected
		};

		enum {MAX_SEND_ERRORS = 10};
		enum {MAX_RESTARTS = 32};
		
		// for use when RNDIS settings change or GCD is restarted vis UI
		void ShutdownGCD();
		void StartGCD();

		void noMessageFromMessengerProcessing();  // is virtual override
		void noMessageFromMessengerProcessingImpl();
		BOOL StartChildThread(ChildThread::RunMethod fpRun, void *pParam, LPCSTR szName, BOOL fRestartActive, int priority);
		void SetNonTransactionGCDWeight();
		BOOL DidGCDFieldDataChange();
		void SaveGCDValues();
		void RestoreGCDValues();
		void RestartGCD();
		void SetHeartbeat(DWORD dwCurrentTick, HeartbeatState state);
		void CheckHeartbeat();
		void MissedHeartbeat();
		void MissedMaxHeartbeats();
		void GotHeartbeat();
		void CheckConnection();
		void ResetGCDValues();
		void HandleGCDProductChange();
		void SetStoreMarqueeMessage();
		BOOL TimeToSendDataPack();
		void UploadFile(const util_filesystem::Filename& localPath, const util_filesystem::Filename& remotePath,
			BOOL fCheckNew, const DWORD* pdwLocalFileSize, const DWORD* pdwLocalLastWriteTime, int threadPriority, FileUploadFinishedMethod fpFileUploadFinished);
		BOOL UpdateGCDStoreLogo();
		void UpdateProductAdvert( int type, const std::string& data );
		DWORD FinishUploadFile(HANDLE hExitEvent, Util::CTLSThread* pThread, void *pParam);
		void UploadRemoteFile(const util_filesystem::Filename& localPath, const util_filesystem::Filename& remotePath,
			BOOL fCheckNew, int threadPriority, FileUploadFinishedMethod fpFileUploadFinished);
		DWORD FinishUploadRemoteFile(HANDLE hExitEvent, Util::CTLSThread* pThread, void *pParam);
		void UploadFtpFile(const std::string& address, const std::string& user, const std::string& password,
			util_filesystem::Filename m_ftpPath, const util_filesystem::Filename& remotePath, BOOL fCheckNew, int threadPriority, FileUploadFinishedMethod fpFileUploadFinished);
		DWORD FinishUploadFtpFile(HANDLE hExitEvent, Util::CTLSThread* pThread, void *pParam);
		void FirmwareSourceUploadFinished(UploadFileInfo* pFileInfo, FileUploadFinishedState finishedState, LPCSTR szErrorDescription);
		void PlaylistUploadFinished(UploadFileInfo* pFileInfo, FileUploadFinishedState finishedState, LPCSTR szErrorDescription);
		void StoreLogoUploadFinished(UploadFileInfo* pFileInfo, FileUploadFinishedState finishedState, LPCSTR szErrorDescription);
		void PlaylistRequestedFileUploadFinished(UploadFileInfo* pFileInfo, FileUploadFinishedState finishedState, LPCSTR szErrorDescription);
		void CloseSocket();

		static void DebugTrace(LPCSTR szFormat, ... );
		static void GetLastErrorExtendedResponse(DWORD* pdwError, std::string* pResponse);

#if !defined( __linux__)
		static HANDLE GetCurrentUserToken();
		static BOOL RunInteractiveProcess( const std::string& processPath, const std::string& arguments = std::string() );
#endif

		BOOL m_fShuttingDown;
		HeartbeatState m_heartbeatState;
		ConnectingState m_connectingState;
		DWORD m_dwTickStoreLogoUpdateMark;
		DWORD m_dwTickLoadDisplayXMLMark;
		DWORD m_dwTickLoadPlaylistMark;
		BOOL m_fHaveReconfiguredACP;
		Util::CSmartPtr<Product> m_pGcdCurrentProduct;
		mutable Util::CSmartCriticalSection m_gcdDataCs;
		HobString m_currentGcdData[GCDDevDataMsgProvider::Field_Count];
		HobString m_lastGcdData[GCDDevDataMsgProvider::Field_Count];
		DWORD m_dwTickResendDataMark;
		DWORD m_dwGCDFieldDataChangeTickMark;
		DWORD m_dwTickHeartbeatMark;
		DWORD m_dwTickCheckHeartbeatMark;

		int m_gcdLayoutNumber;
		int m_gcdColorScheme;
		bool m_gcdUseTransactionalLayout;
		int m_gcdIdleAdvertType;
		HobString m_gcdIdleWebsiteAddress;

		std::string m_gcdUnitSeparator; // FT-841
		std::string m_gcdCurrencySymbol; //FT-841
		WeighMode m_scaleWeighMode;
		PAL_SOCKET m_hGcdDiscoverySocket;
		Socket* m_pGcdMsgSocket;
		int m_gcdSocketDestId;
		std::string m_ipAddressGcd;
		mutable Util::CSmartCriticalSection m_updatingFilesCs;
		std::list< UploadFileInfo* > m_updatingFiles;
		mutable Util::CSmartCriticalSection m_childThreadsCs;
		std::vector< ChildThread* > m_childThreads;
		Util::CSmartHandle m_uploadFileThreadSemaphore;
		Util::CSmartHandle m_downloadFileThreadSemaphore;
		BOOL m_fSentPeripheralRequestCodeUpdate;
		PlaylistUpdateState m_playlistUpdateState;
		mutable Util::CSmartCriticalSection m_playlistUpdateCs;
		std::list< RequestFileUpload > m_playlistRequestFileUploads;
		std::string m_ftpUpdateAddress;
		std::string m_ftpUpdateUsername;
		std::string m_ftpUpdatePassword;
		util_filesystem::Filename m_updateDirectoryPath;
		int m_missingHeartBeatCount;
		mutable Util::CSmartCriticalSection m_heartbeatCounterCs;
		mutable Util::CSmartCriticalSection m_sendMessageCs;
		BOOL m_fGcdStoreLogoUploaded;
		BOOL m_fGcdDisplayXMLLoaded;
		BOOL m_fGcdPlaylistLoaded;
		BOOL m_bCancelTransfer;
		BOOL GcdRestarted;	//	GCDR-183
		BOOL m_fGCDAttachedToAPC;
		Util::CSmartHandle m_gcdIsConnected;
		unsigned int m_sendErrors;
		unsigned int m_gcdRestartCount;

		bool m_isTransactionalGCDLicensed;

		std::string currentPlaylistFileName_;
		void sendPlaylistFilenameToGCD();

		void sendLoadDisplayObjectXMLToGCD();

};

#endif //GCDSESSION_H
