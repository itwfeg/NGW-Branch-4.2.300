/* LX Scale Project
 * COPYRIGHT (c) 2011.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */
#include "GCDSession.h"
#include "ProductChangeMsg.h"
#include "ProductManager.h"
#include "WeigherEvent.h"
#include "ConfigurationEvent.h"
#include "CfgGCD.h"
#include "SystemEvent.h"
#include "WeigherManager.h"
#include "APCCommManager.h"
#include "HfcWeigherStatus.h"
#include "SessionMessage.h"
#include "ResponseMessageManager.h"
#include "HfcWeigherConfig.h"
#include "NumericManager.h"
#include "SessionManager.h"
#include "RealTimeController.h"
#include "SystemStatusInfo.h"
#include "CMReaderWriterGCD.h"
#include "GCDSystemStatus.h"
#include "GenericHfcObject.h"
#include "GCDDevFileUploadMsg.h"
#include "GCDDevNoParamMsg.h"
#include "GCDDevStringParamMsg.h"
#include "GCDDevLanguageAndWeighModeMsg.h"
#include "GCDDevSetVolumeMsg.h"
#include "GCDDevSetPlaybackWaitTimeMsg.h"
#include "GCDDevLoadPlaylistMsg.h"
#include "GCDDevTestColorMsg.h"
#include "GCDFtpUpdateMsg.h"
#include "GCDDevHeartbeatReqMsg.h"
#include "GCDDevSetTimeInfo.h"
#include "GCDDevLoadDisplayObjectXMLMsg.h"
#include "HfcGCDDevStatus.h"
#include "HfcGCDDevFileInfo.h"
#include "HfcGCDDevRequestFileUpload.h"
#include "HfcGCDDevError.h"
#include "PeripheralRequestCodeUpdate.h"
#include "PeripheralCodeUpdate.h"
#include "utilunicode.h"
#include "Hex.h"
#include "ULong.h"
#include "Integer.h"
#include "UInteger.h"
#include "Bool.h"

#include "SendGcdPlaylistErrorEmailEvent.h"
#include "SendGcdFtpErrorEmailEvent.h"
#include "MessageLogger.h"

#include "ProductText.h"
#include "Category.h"
#include "Ping.h"

#include <HobartCustomerHook.h>

#if !defined( __linux__)
#include <Wtsapi32.h>
#include <Userenv.h>
#endif

#include "ProductStatusMsg.h"

#define MAX_CONCCURENT_GCD_UPLOADS	1
#define MAX_CONCCURENT_MEDIA_DOWNLOADS	1

#define TICKS_MAX_HEARTBEAT_ELAPSED 5000
#define TICKS_MAX_SEND_DATA_PACK_ELAPSED 5000
#define TICKS_MAX_GENERIC_TIMEOUT 60000
#define TICKS_MIN_GCD_DATA_FIELD_CHANGE 250
#define MAX_MISSED_HEARTBEAT_COUNTER 4
#define MAX_MISSED_HEARTBEAT_CONNECTING_COUNTER 32

#define COUNT_MAX_SESSION_SEMAPHORE 32

static DWORD GetElapsedTicks(DWORD dwTick0, DWORD dwTickN)
{
	// PAL_absclass::PAL_getTickCount() wraps around every 49.7 days need to check for that 
	return (dwTick0 > dwTickN) ? ULONG_MAX - dwTick0 + dwTickN : dwTickN - dwTick0;
}

GCDSession::GCDSession(CommMechanism *uiClient, bool /*enableInput*/) :
 	Session(uiClient)
{
	//Debug::trace( "GCDSession", "GCDSession::GCDSession()" );

#if 0
	// note: the GCD is already powered, reconfiguring and powering up will reset it again
	APCCommManager *pAPCCommManager = APCCommManager::getInstance();
	if ( pAPCCommManager )
	{
		pAPCCommManager->SysCfgGCD();
		m_fHaveReconfiguredACP = true;
		pAPCCommManager->SysGCDPwrOn();
	}
#endif

	init();									//	GCDR-205
	setSessionType(SESSIONTYPE_GCD);		//	GCDR-191
	
	//Debug::trace( "GCDSession", "GCDSession::GCDSession():  DONE." );
}

GCDSession::~GCDSession()
{
	//Debug::trace( "GCDSession", "GCDSession::~GCDSession()" );

	{
		Util::CLockCriticalSection lock( m_childThreadsCs );

		for ( int i = 0; m_childThreads.size(); ++i )
			if ( m_childThreads[i] )
			{
				delete m_childThreads[i];
				m_childThreads[i] = NULL;
			}

		m_childThreads.clear();
	}

	if (m_pGcdMsgSocket)
	{
		CloseSocket();
		delete m_pGcdMsgSocket;
		m_pGcdMsgSocket = NULL;
	}

	if (-1 != m_hGcdDiscoverySocket)
	{
		PAL_absclass::PAL_closesocket(m_hGcdDiscoverySocket);
		m_hGcdDiscoverySocket = -1;
	}
}

#if !defined( __linux__)
// This is all windows only code. Windows must timeout it's 
// DHCP before it will go to an auto-ip. That takes about
// a minute after the RNDIS adapter shows up. Ends up taking
// around two minutes or longer to boot the GCD. The scale is
// operational through part of this so that's bad. 
// Linux doesn't have the limitation

bool GCDSession::GetPortData(HANDLE HubHandle, UCHAR PortCount, int HubDepth, unsigned short int idVendor, unsigned short int idProduct) 
{
	DWORD						BytesReturned;
	bool						Success;
	ULONG						PortIndex;
	UCHAR						PortStatus;
	WCHAR						ConnectedHubName[256] = L"\\\\.\\";
	HANDLE						ConnectedHubHandle;
	NodeInformation				NodeInfo;
	NodeConnectionInformation	ConnectionInformation;
	bool						res = false;

	struct CONNECTED_HUB
	{
		ULONG ConnectionIndex; 
		ULONG ActualLength; 
		WCHAR Name[256];
	} ConnectedHub;

	// Iterate over the ports to discover what is connected to each one
	for( PortIndex = 1; PortIndex < (ULONG)PortCount + 1; PortIndex++ ) 
	{
		ConnectionInformation.ConnectionIndex = PortIndex;
		Success = DeviceIoControl(HubHandle, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION, &ConnectionInformation,sizeof(ConnectionInformation), &ConnectionInformation, sizeof(ConnectionInformation), &BytesReturned,NULL)? true: false;
		PortStatus = ConnectionInformation.ConnectionStatus[0]; // Save some typing!
		
		// Now the connection specific information
		if (PortStatus == DeviceConnected)
		{
			// have a device or a hub connected to this port
			if (!ConnectionInformation.DeviceIsHub) 
			{
				unsigned short int thisVendor = ((ConnectionInformation.DeviceDescriptor.idVendor[0] & 0x00FF ) | ((ConnectionInformation.DeviceDescriptor.idVendor[1]  << 8) & 0xFF00 ));
				unsigned short int thisProduct = ((ConnectionInformation.DeviceDescriptor.idProduct[0] & 0x00FF) | ((ConnectionInformation.DeviceDescriptor.idProduct[1] << 8) & 0xFF00 ));
				if( thisVendor == idVendor && thisProduct == idProduct)
				{
					return( true ); //hit
				}
			}
			else
			{
				// Get the system name of the connected hub so that we can make a connection to it
				ConnectedHub.ConnectionIndex = PortIndex;
				Success = DeviceIoControl(HubHandle, IOCTL_USB_GET_NODE_CONNECTION_NAME, &ConnectedHub,sizeof(ConnectedHub), &ConnectedHub, sizeof(ConnectedHub), &BytesReturned, NULL)? true: false;
				if( Success )
				{
					wsprintf( ConnectedHubName, L"\\\\.\\%s" , ConnectedHub.Name);
					ConnectedHubHandle = CreateFile(ConnectedHubName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
					if( INVALID_HANDLE_VALUE != ConnectedHubHandle )
					{
						// Connected hub is open. Collect the node information
						Success = DeviceIoControl(ConnectedHubHandle, IOCTL_USB_GET_NODE_INFORMATION, &NodeInfo, sizeof(NodeInformation), &NodeInfo, sizeof(NodeInformation), &BytesReturned, NULL)? true: false;
						if( Success )
						{
							res = GetPortData(ConnectedHubHandle, NodeInfo.HubDescriptor.bNumberOfPorts, HubDepth+1, idVendor, idProduct); // recur...
						}
						CloseHandle(ConnectedHubHandle);
					}
				}
				return( res );
			}
		}
	}
	return( false );
}

bool GCDSession::EnumerateHostController(HANDLE HostControllerHandle, unsigned short int idVendor, unsigned short int idProduct) 
{
	struct 
	{
		ULONG Length; 
		WCHAR Name[256];
	} UnicodeName;
	
	DWORD			BytesReturned;
	bool			Success;
	WCHAR			RootHubName[256] = L"\\\\.\\";
	HANDLE			RootHubHandle;
	NodeInformation NodeInfo;
	bool			res = false;

	// First get the system name of the host controller for display
	Success = DeviceIoControl(HostControllerHandle, IOCTL_GET_HCD_DRIVERKEY_NAME, &UnicodeName, sizeof(UnicodeName),&UnicodeName, sizeof(UnicodeName), &BytesReturned, NULL)? true: false;
	if(Success) 
	{
		// Now get the system name of it's root hub for interrogation
		Success = DeviceIoControl(HostControllerHandle, IOCTL_USB_GET_ROOT_HUB_NAME, &UnicodeName, sizeof(UnicodeName), &UnicodeName, sizeof(UnicodeName), &BytesReturned, NULL)? true: false;
		if( Success )
		{
			// Now open the root hub. Need to construct a char name from "\\.\" + UnicodeName
			wsprintf( RootHubName, L"\\\\.\\%s", UnicodeName.Name );
			RootHubHandle = CreateFile( RootHubName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
			if( INVALID_HANDLE_VALUE != RootHubHandle )
			{
				// Root hub is open. Collect the node information
				Success = DeviceIoControl(RootHubHandle, IOCTL_USB_GET_NODE_INFORMATION, &NodeInfo, sizeof(NodeInformation), &NodeInfo, sizeof(NodeInformation), &BytesReturned, NULL)? true: false;
				if( Success )
				{
					// Can now iterate over the ports
					res = GetPortData(RootHubHandle, NodeInfo.HubDescriptor.bNumberOfPorts, 0, idVendor, idProduct);
				}
				CloseHandle(RootHubHandle);
			}
		}
	}
	return res;
}

bool GCDSession::IsGCDThere( unsigned short int vid, unsigned short int pid )
{
	HANDLE	HostControllerHandle;
	WCHAR	HostControllerName[] = L"\\\\.\\HCD0";
	int		ii;
	bool	bFound = false;

//	Debug::trace("GCDStartup", "GCDSession::IsGCDThere(" + Hex::toString(vid) + ", " + Hex::toString(pid) + ")");
	// Search for USB host controllers.
	for( ii = 0; ii < 10; ii++ )
	{
		HostControllerName[7] = ii + '0';
		HostControllerHandle = CreateFile(HostControllerName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if( HostControllerHandle != INVALID_HANDLE_VALUE )
		{
			bFound = EnumerateHostController(HostControllerHandle, vid, pid );
			CloseHandle(HostControllerHandle);
		}
		if( bFound ) return( true );
	}
	return(false);
}

// Fire the thread that waits for the RNDIS adapter to appear
// so we can give it a random link-local IP. The RNDIS adapter
// takes about twenty seconds to show up once the GCD is booted.
void GCDSession::setLLIP( int seconds )
{
	// Static since we are passing it's address into a thread
	//Debug::trace("GCDStartup", HobString("GCDSession::setLLIP(") + Integer::toString(seconds) + ") starting thread waitForRNDISAdapter");
	static int sec = seconds;
	::CloseHandle(CreateThread( NULL, 0, GCDSession::waitForRNDISAdapter, &sec, 0, NULL ));
}

// This function will let us bypass the DHCP server stuff
// and just generate our own link-local address. This should 
// fix the unconfigurable DHCP sixty second timeout on XP
IPAddr GCDSession::SetLinkLocalIPAddress( DWORD adapterIndex, IPAddr ip )
{
	//IPAddr	ip				= 0;
	bool	bDone			= false;
	UINT	count			= 12;		// How many different IP's to try
	UCHAR	byteOne; 
	UCHAR	byteTwo;
	UINT	seed;
	IPAddr	mask;
	ULONG	context;
	ULONG	instance;
	char	buf1[16];
	bool	bDuplicatedIp;
	DWORD	dwRetVal;

	// Loop until we find an IP not in use. In
	// all probability, this will only loop once.
	while( count-- && !bDone )
	{
		if( !ip )
		{
			// Pick a random link-local IP address
			seed = GetTickCount();
			srand( seed );
			byteOne = UCHAR( 0xFF & rand() );
			byteTwo = UCHAR( 0xFF & rand() );
			// Format it and the subnet mask so it works in inet_addr call
			sprintf(buf1, "%d.%d.%d.%d",169,254,byteTwo,byteOne);
			ip = inet_addr(buf1);
		}

		mask = inet_addr("255.255.0.0");

		// ping it a few times to make sure we can have it. (should only be the gcd)
		std::string pingTarget(buf1);
		bDuplicatedIp = (Hobart::CPing::Ping(std::string(pingTarget), !HobIpAddress::isValidIPAddress(pingTarget), 5 ) );
		if( !bDuplicatedIp )
		{
			// not found out there. Pretty much means it's not the same as the GCD so add it and return
			if( NO_ERROR == (dwRetVal = AddIPAddress(ip, mask, adapterIndex, &context, &instance)))
			{
				bDone = true;
			}
		}
	}
	return( ip );
} 

// Thread function to add an IP that the GCD can use. Need a second
// thread so the scale boot isn't delayed or the missing peripheral list 
// triggered while we wait for the RNDIS adapter to show up. It exits when
// a new IP has been added.
DWORD GCDSession::waitForRNDISAdapter( LPVOID pSeconds )
{
	// Check here for existence of the network adapter rather than with the 
	// heardbeat which caused the missing peripheral screen to show up.
	PIP_ADAPTER_ADDRESSES	pAdapterAddresses	= NULL;
	PIP_ADAPTER_ADDRESSES	pCurrent			= NULL;
	ULONG					outBufferLength		= 0;
	int						counter;
	bool					bDone				= false;
	bool					bDone2				= false;
	int						res					= 0;
	DWORD					index				= 0;
	IPAddr					ip					= 0;


	// Set the number of iterations to wait for the adapter to show
	counter = pSeconds? (*((int*)pSeconds) * 2): 80;

	//Debug::trace("GCDStartup", HobString("GCDSession::waitforRNDISAdapter() has just been entered."));
	// Loop until we are done or timeout
	while( counter-- && !bDone )
	{
		// checking every half second for the RNDIS adapter.
		Sleep(500);
		bDone = IsGCDThere( GCDVID, GCDPID );
		// Found it.
		// Tell the periperal manager that the GCD exists and exit the loop
//	GCDR-205:	We don't say when it's attached - the USBManager does.
//		if( bDone )
//		{
//			RealTimeController::getInstance()->GCDAttached(GCDUsb);
//			Debug::trace("GCDStartup", HobString("GCDSession::waitforRNDISAdapter() has just told the peripheral Mgr that the GCD exists."));
//		}
	}

	Debug::trace("GCDStartup", HobString("GCDSession::waitforRNDISAdapter() RNDIS adapter find loop completed."));
	// Set the number of iterations to wait for the adapter to show
	counter = pSeconds? (*((int*)pSeconds) * 2): 80;
	bDone = false;

	// Loop until we are done or timeout
	while( counter-- && !bDone )
	{
		// checking every half second for the RNDIS adapter.
		Sleep(500);

		// Clean up for this iteration. Need to keep
		if(pAdapterAddresses) 
		{
			free(pAdapterAddresses);
		}
		pAdapterAddresses = NULL;
		outBufferLength = 0;

		// See how much memory we need to get information about the adapters.
		if( ERROR_BUFFER_OVERFLOW == GetAdaptersAddresses( AF_INET, GAA_FLAG_SKIP_ANYCAST|GAA_FLAG_SKIP_DNS_SERVER, 0, NULL, &outBufferLength) )
		{
			// Size is OK
			if( outBufferLength )
			{
				// Get the mem we need on this iteration
				pAdapterAddresses = (PIP_ADAPTER_ADDRESSES)calloc(outBufferLength, 1);
				if( pAdapterAddresses )
				{
					// Now really get the memory we need
					if( ( !(res = GetAdaptersAddresses( AF_INET, GAA_FLAG_SKIP_ANYCAST|GAA_FLAG_SKIP_DNS_SERVER, 0, pAdapterAddresses, &outBufferLength ) ) ) )
					{
						// See if there's an RNDIS adapter
						pCurrent = pAdapterAddresses;
						bDone2 = false;
						while( pCurrent && !bDone2 )
						{
							// RNDIS adpater always has this in it description field
							if( NULL != wcsstr( pCurrent->Description, L"RNDIS" ) )
							{
								// This adds the random IP. Saving it so we can delete
								// all the other IP's on this NIC. That doesn't work though.
								// Appears you can't delete the auto-ip address for some reason.
								// May need more on this. (Maybe set it to static instead of DHCP?)
								if( ip = GCDSession::SetLinkLocalIPAddress( pCurrent->IfIndex, ip ) )
								{
									bDone = true;
								}
								bDone2 = true; // Exits the inner adapter enumeration loop
							}
							else
							{
								// So we can look at the next adapter in the list
								pCurrent = pCurrent->Next;
							}
						}
					}
				}
			}
		}
	}
	Debug::trace("GCDStartup", HobString("GCDSession::waitforRNDISAdapter() RNDIS adapter cleanup loop completed."));
	if(pAdapterAddresses) 
	{
		free(pAdapterAddresses);
	}

#if 0
	// This is if'd out. Looks like it's not possible to 
	// remove the auto ip from the system.
	// Left the code in case we figure it out.
	//
	// Hang around for a couple minutes more to
	// remove the DHCP ip.
	if( bDone )
	{
		RemoveOtherIP( index, ip );
	}
#endif
	return( 0 );
}

#if 0
// left here in case a cure is found. It's be nice to not
// have two IP's on one machine.
void GCDSession::RemoveOtherIP( DWORD indx, IPAddr ip )
{
	// Get the context of the DHCP adapter.
	// This will come later so we need to loop
	// until it does

	DWORD counter = 240; // Four minutes then give up 
	DWORD bufSize = 0;
	bool bDone = false;
	PIP_ADAPTER_INFO dapterInfo = NULL;
	PIP_ADDR_STRING pCurrent;
	DWORD res;
	IPAddr thisIp;

	while( counter-- && !bDone )
	{
		Sleep(1000);

		if( dapterInfo )
		{
			free(dapterInfo);
			dapterInfo = NULL;
		}

		if( (res = ERROR_BUFFER_OVERFLOW == GetAdaptersInfo( NULL, &bufSize ) ) )
		{
			if( bufSize )
			{
				dapterInfo = (PIP_ADAPTER_INFO)calloc( bufSize, 1 );
				if( dapterInfo )
				{
					if( (NO_ERROR == (res = GetAdaptersInfo( dapterInfo, &bufSize ) ) ) )
					{
						if( NULL != strstr( dapterInfo->Description, "RNDIS" ) )
						{
							// loop through each IP now
							pCurrent = &(dapterInfo->IpAddressList);
							while( pCurrent )
							{
								thisIp = inet_addr(pCurrent->IpAddress.String );
								if( ip != thisIp && thisIp != (IPAddr)0 )
								{
									if( NO_ERROR == (res = DeleteIPAddress(pCurrent->Context) ) )
									{
										bDone = true; // Found the right adpater again.
									}
								}
								pCurrent = pCurrent->Next;
							}
						}
					}
				}
			}
		}
	}
	if( dapterInfo )
	{
		free(dapterInfo);
		dapterInfo = NULL;
	}

}
#endif
#endif

void GCDSession::init()
{
	Debug::trace( "GCDStartup",  "GCDSession::init()" );

	//Debug::trace("GCD", HobString("Initializing GCD, registering events, launching application"));

	m_fShuttingDown = FALSE;
	m_fHaveReconfiguredACP = FALSE;

	m_gcdLayoutNumber		    = 1;
	m_gcdColorScheme		    = 1;
	m_gcdUseTransactionalLayout = false;
	m_gcdIdleAdvertType			= 0;

	m_gcdCurrencySymbol		= "$";
	m_gcdUnitSeparator		= ".";

	m_scaleWeighMode		= (WeighMode) -1; // was WeighMode_Avoir... we dont know until we are told

	for ( int i = 0; i < GCDDevDataMsgProvider::Field_Count; ++i )
	{
		m_currentGcdData[i] = "";
		m_lastGcdData[i] = "";
	}

	DWORD dwCurrentTick = PAL_absclass::PAL_getTickCount();

	m_dwTickCheckHeartbeatMark = dwCurrentTick;
	SetHeartbeat(dwCurrentTick, HeartbeatState_Connecting);

	m_connectingState = ConnectingState_Start;

	m_playlistUpdateState = PlaylistUpdateState_None;

	m_dwTickResendDataMark = dwCurrentTick;
	m_dwGCDFieldDataChangeTickMark = dwCurrentTick;

	m_pGcdCurrentProduct = NULL;

	m_ftpUpdateIsInteractive = false;

	m_gcdIsConnected = PAL_absclass::PAL_createEvent(NULL, TRUE, FALSE, NULL);

	m_fGcdStoreLogoUploaded = FALSE;
	m_fGcdDisplayXMLLoaded = FALSE;
	m_fGcdPlaylistLoaded = FALSE;

	m_hGcdDiscoverySocket = -1;

	m_pGcdMsgSocket = new Socket();
	m_pGcdMsgSocket->setMyReaderWriter(new CMReaderWriterGCD);
	
#if defined( __linux__ )
	m_pGcdMsgSocket->setSendFlags(MSG_NOSIGNAL);
	m_pGcdMsgSocket->setRecvFlags(MSG_NOSIGNAL);
#endif

	m_gcdSocketDestId = myMessenger.addCommMechanism(m_pGcdMsgSocket);
	

	m_uploadFileThreadSemaphore = PAL_absclass::PAL_createSemaphore(NULL, MAX_CONCCURENT_GCD_UPLOADS, MAX_CONCCURENT_GCD_UPLOADS, NULL);

	m_downloadFileThreadSemaphore = PAL_absclass::PAL_createSemaphore(NULL, MAX_CONCCURENT_MEDIA_DOWNLOADS, MAX_CONCCURENT_MEDIA_DOWNLOADS, NULL);

	memset( &updateError, 0, sizeof(updateError) );
	
	m_bCancelTransfer = FALSE;

	m_gcdRestartCount = 0; // FT-1059

	// FT-1362
	m_isTransactionalGCDLicensed = Configuration::getInstance()->isTransactionalGCDLicensed();

	// Normally the GCD is attached to the APC via a USB network cable.  Then the GCD can be
	// powered and reset via APC controls.  When this variable is FALSE we will not issue APC
	// commands.
	m_fGCDAttachedToAPC = TRUE;

	// Randy QMX DEMO
	if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::GCDENABLEDSTANDALONE) )
	{
		if (Debug::isTracing("GCD"))
			Debug::trace("GCD", HobString("GCDSession::InitializeGCDVariables: have standalone GCD"));
				
		HobString ipAddressGCD = Configuration::getInstance()->toStringCfgI(CfgInternal::IPADDRESSGCD);
		int gcdServerPort = Configuration::getInstance()->toIntCfgI(CfgInternal::GCDSERVERPORT);

		m_pGcdMsgSocket->setAddress(ipAddressGCD.c_str());
		m_pGcdMsgSocket->setPort(gcdServerPort);

		m_fGCDAttachedToAPC = FALSE;

#if defined( __linux__)
		//determine if the GCD is physically connected to the system controller
		//this script checks a GPIO
		if( system("/usr/local/hobart/python/getGcdStatus.sh") )
		{
			Debug::trace("GCD", "GCDSession::init - Native GCD is physically attached");
			RealTimeController::getInstance()->GCDAttached(GCDNative);
		}
		else
		{
			Debug::trace("GCD", "GCDSession::init - Native GCD is not physically attached ");
			return;
		}
#else
		// TODO
		//determine if the GCD is physically connected to the system controller
#endif

	}

	currentPlaylistFileName_ = "";

	GcdRestarted = TRUE;
	StartGCD();
	//Debug::trace("GCD", HobString("GCDSession::init() calling StartGCD"));

#if !defined( __linux__)
	// Hang out until the IP address shows up
	if ( m_fGCDAttachedToAPC )
	{
		setLLIP( 40 );
		//setLLIP( 140 );  //Changed time out from 40 seconds to 140 seconds
	}
#endif

	//Debug::trace("GCD", HobString("GCDSession::init: thread is launched, now calling ProcessReloadGCDManager"));
	//Debug::trace( "GCDStartup",  "GCDSession::init():  DONE." );
}

void GCDSession::DebugTrace(LPCSTR szFormat, ... )
{
	va_list ap;
	va_start(ap, szFormat);
	char szTrace[256] = { 0 };
	vsprintf(szTrace, szFormat, ap);
	Debug::trace("GCD", szTrace);
	va_end(ap);
}

HobString GCDSession::GetData(const Field field) const
{
	Util::CLockCriticalSection lock( m_gcdDataCs );

	return m_currentGcdData[field];
}

// Returns state of enumerated type GCDInitialized. At present if == NOT_INITIALIZED
// Added for GCDR-183
bool GCDSession::getGCDOperational(void)
{
	if ( GCDOperational == INITIALIZED )
	{
		// when connected return connection status
		if ( GCDOperational >= CONNECTED )
			return ( WAIT_OBJECT_0 == PAL_absclass::PAL_waitForSingleObj( m_gcdIsConnected, 0 ) );

		return true;
	}

	return false;
}
bool GCDSession::gcdCanTalk(void)
{
	if ( GCDOperational >= CONNECTED )
		return true;
			
	return false;
}
// Puts the FTP/SFTP path into a known format so
// subsequent appends and prepends don't end up
// in things like doubled slashes
void GCDSession::trimFTPDirSeparators( std::string& s, char charToTrim )
{
	string::size_type pos = s.find_last_not_of(charToTrim);
	if(pos != string::npos) 
	{
		s.erase(pos + 1);
		pos = s.find_first_not_of(charToTrim);
		if(pos != string::npos)
		{
			s.erase(0, pos);
		}
	}
	else
	{
		s.erase(s.begin(), s.end());
	}
}

std::string GCDSession::validateFtpPath(std::string &server, std::string &path, int protocol)
{
#if defined ( __linux__ )
	// Trim forward slashes for linux
	trimFTPDirSeparators(server,'/');
	trimFTPDirSeparators(path,'/');
#else
	// for paths stored in windows format,
	// switching to cURL requires everything
	// to be in unix style paths
	trimFTPDirSeparators(server,'\\');
	trimFTPDirSeparators(path,'\\');
#endif

	// Finish up the string construction. Result must be in a URL and
	// we put the trailing slash because even if there is no file
	// to append, the cURL library needs the trailing backslash to know
	// that it's a directory and not a file. (This sort of mirrors the upgrade methods.)
	std::string fullPath((protocol==PAL_INTERNET_SERVICE_FTP)? "ftp://":"sftp://");
	fullPath += server;
	fullPath += "/";
	if( path.length() )
	{
		fullPath += path;
		fullPath += "/";
	}

	return( fullPath );
} 

// Constructs the final URL (for the cURL FTP call) 
// and sets the path and port depending on the
// protocol field stored in the DB and the DOM (It's "FTP" or "SFTP"
std::string GCDSession::GetPortProtocolUrl( std::string server, std::string dir, INTERNET_PORT* port, int* protocol )
{
	Configuration *config = Configuration::getInstance();

	std::string gcdProtocol = config->toStringCfg(fieldOffsetCfgGCDDownloadProtocol);
	if( 0 == gcdProtocol.compare( "SFTP" ) )
	{
		*protocol = PAL_INTERNET_SERVICE_SFTP;
		*port = PAL_INTERNET_DEFAULT_SFTP_PORT;
	}
	else
	{
		*protocol = PAL_INTERNET_SERVICE_FTP;
		*port = PAL_INTERNET_DEFAULT_FTP_PORT;
	}
	// move to both (and with a choice for sftp) since now using curl on windows
	std::string url = validateFtpPath(server, dir, *protocol);

	return(url);
}

// Utility function to convert any strings
// with back slashes (windows dir separator) to
// forward slashes needed by cURL calls
void GCDSession::fixTheSlashesForFtp( std::string& s )
{
	// Just step through each and replace any '\' with '/'
	for (int i = 0; i < static_cast< int >(s.length()); ++ i)
	{
		if ('\\' == s[i])
		{
			s[i] = '/';
		}
	}
}

void GCDSession::ShutDownSession()
{
	Debug::trace( "GCDStartup", "GCDSession::ShutDownSession() called" );
	m_fShuttingDown = TRUE;

	setStatus(AbstractSession::STATUS_TERMINATED);
}

void GCDSession::SetNonTransactionGCDWeight()
{
	Debug::trace( "GCD.NonTrans", "SetNonTransactionGCDWeight()" );
	Util::CLockCriticalSection lock( m_gcdDataCs );

	HfcWeigherStatus *ws( getCurrentWeight() );

	if ( ws )
	{
		ws->determineDisplayValues();
		m_currentGcdData[GCDDevDataMsgProvider::Field_WgtDisp] = ws->getWeightDisplay();

		//center of zero annunciator
		m_currentGcdData[GCDDevDataMsgProvider::Field_CZAnnunc] = ((ws->isZeroStableWeight()) ? "CZ" : "-");
	}

	Debug::trace( "GCD.NonTrans", "SetNonTransactionGCDWeight() done" );
}

BOOL GCDSession::DidGCDFieldDataChange()
{
	// if this data changed within the last 250ms then don't send yet
	// "They are sent only when data changes, up to four times a second." (LLD)
	if ( GetElapsedTicks(m_dwGCDFieldDataChangeTickMark, PAL_absclass::PAL_getTickCount()) < TICKS_MIN_GCD_DATA_FIELD_CHANGE )
		return false;

	Util::CLockCriticalSection lock( m_gcdDataCs );

	for ( int i = 0; i < GCDDevDataMsgProvider::Field_Count; ++i )
		if ( 0 != m_currentGcdData[i].compare( m_lastGcdData[i] ) )
			return true;

	return false;
}

void GCDSession::SaveGCDValues()
{
	Debug::trace( "GCD", "SaveGCDValues()" );
	Util::CLockCriticalSection lock( m_gcdDataCs );

	//save off all gcd data
	for ( int i = 0; i < GCDDevDataMsgProvider::Field_Count; ++i )
		m_lastGcdData[i] = m_currentGcdData[i];
}

void GCDSession::RestoreGCDValues()
{
	Debug::trace( "GCD", "RestoreGCDValues()" );
	Util::CLockCriticalSection lock( m_gcdDataCs );

	for ( int i = 0; i < GCDDevDataMsgProvider::Field_Count; ++i )
		if ( 0 != m_currentGcdData[i].compare( m_lastGcdData[i] ) )
			m_currentGcdData[i] = m_lastGcdData[i];
}

void GCDSession::ResetGCDValues()
{
	Debug::trace( "GCD.reset", "ResetGCDValues()" );
	Util::CLockCriticalSection lock( m_gcdDataCs );

	//reset variables
	for ( int i = 0; i < GCDDevDataMsgProvider::Field_Count; ++i )
	{
		// FT-580: do not reset prepack indicator
		if ( GCDDevDataMsgProvider::Field_PrepackAnnunc == i )
			continue;

		m_currentGcdData[i] = "";
	}
}

void GCDSession::updatePrepackAnnunciator(NmState state1)
{
	Util::CLockCriticalSection lock( m_gcdDataCs );

	m_currentGcdData[GCDDevDataMsgProvider::Field_PrepackAnnunc] = (state1 == NmOff) ? "-" : "PP";
}

void GCDSession::UpdateProductWeightData(HfcProductUpdateValuesMsg &hpuvm)
{
	Product *prod( hpuvm.getProductTransaction() ? hpuvm.getProductTransaction()->getProduct() : NULL );

	if ( prod && prod->getItemIsMealSideItem() )
		return;

	Util::CLockCriticalSection lock( m_gcdDataCs );

	m_currentGcdData[GCDDevDataMsgProvider::Field_NetAnnunc] = "-";
	m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopUnitPrice] = "";
	m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopTotalPrice] = "";
	m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopSavings] = "";

	if ( !prod || prod->isMealBuilderItem() )
	{
		//tare
		m_currentGcdData[GCDDevDataMsgProvider::Field_TareDisp] = "";

		//net weight
		m_currentGcdData[GCDDevDataMsgProvider::Field_WgtDisp] = "";
	}
	else
	{
		//tare
		m_currentGcdData[GCDDevDataMsgProvider::Field_TareDisp] = hpuvm.gettotalTareDisplay();

		//net weight
		m_currentGcdData[GCDDevDataMsgProvider::Field_WgtDisp] = hpuvm.getNetWeightDisplay();
	}

	//unit price
	m_currentGcdData[GCDDevDataMsgProvider::Field_UPriceDisp] = hpuvm.getUnitPriceDisplay();

	//total price
	m_currentGcdData[GCDDevDataMsgProvider::Field_TPriceDisp] = hpuvm.getTotalPriceDisplay();

	//net annunciator
	// FT-881: handle manual weight product
	if (( prod ) &&	( prod->hasKbdNetWeightLabel() ))
	{
		if ( Debug::isTracing( "GCD" ) )
			Debug::trace( "GCD", "Net Annunc being set to MAN" );

		m_currentGcdData[GCDDevDataMsgProvider::Field_NetAnnunc] = "MAN";
	}

	// Update the 'net' annunciator (if tare > zero, the the weight displayed will be a net weight, gross - tare)
	else if ( 0 < hpuvm.GetTareWeight())
	{
		if ( Debug::isTracing( "GCD" ) )
			Debug::trace( "GCD", "Net Annunc being set to NT" );

		m_currentGcdData[GCDDevDataMsgProvider::Field_NetAnnunc] = "NT";
	}
	// Added the randowmweight and bycount check below for JIRA HW-79
	else if (prod) //Short circuit protection for evaluations below
	{
		if(prod->isRandomWeight() || prod->isByCountWithWeight())
		{
			if ( Debug::isTracing( "GCD" ) )
				Debug::trace( "GCD", "Net Annunc being set to NT" );

			m_currentGcdData[GCDDevDataMsgProvider::Field_NetAnnunc] = "NT";
		}
	}
	

	//lbs for pricing
	if ( Configuration::getInstance()->isPoundsForPricingEnabled() )
	{
		if ( 0 < hpuvm.getPoundsForDisplay().length() )
			m_currentGcdData[GCDDevDataMsgProvider::Field_WtForAnnunc] = hpuvm.getPoundsForDisplay();
		else
			m_currentGcdData[GCDDevDataMsgProvider::Field_WtForAnnunc] = "-";
	}
	//fractional pricing
	else if ( Configuration::getInstance()->isFractionalPricingEnabled() )
	{
		if ( 0 < hpuvm.getFractionalPricingForDisplay().length() )
			m_currentGcdData[GCDDevDataMsgProvider::Field_FractionalAnnunc] = hpuvm.getFractionalPricingForDisplay();
		else
			m_currentGcdData[GCDDevDataMsgProvider::Field_FractionalAnnunc] = "-";
	}

	//lbs for pricing
	if ( Configuration::getInstance()->isPoundsForPricingEnabled() )
	{
		if ( 0 < hpuvm.getPoundsForDisplay().length() )
			m_currentGcdData[GCDDevDataMsgProvider::Field_WtForAnnunc] = hpuvm.getPoundsForDisplay();
		else
			m_currentGcdData[GCDDevDataMsgProvider::Field_WtForAnnunc] = "-";
	}

	//zero indicator
	m_currentGcdData[GCDDevDataMsgProvider::Field_CZAnnunc] = (hpuvm.isZeroIndicator()) ? "CZ" : "-";	//	GCDR-201

	if (( prod ) && ( prod->hasFrequentShopperLabel() ))
	{
		//frequent shopper unit price
		m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopUnitPrice] = hpuvm.getFsUnitPriceDisplay();

		//frequent shopper total price
		m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopTotalPrice] = hpuvm.getFsTotalPriceDisplay();

		//frequent shopper savings
		m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopSavings] = hpuvm.getFsTotalSavingsDisplay();
	}

	// PTR 1015: Add unit price for Fixed Weight items
	if (( prod ) && ( Product::PRODUCTTYPE_FIXEDWEIGHT == prod->getProductType() ))
	{
		if ( 0 < prod->getFixedWeight() && !prod->isMealBuilderItem() )
		{
			m_currentGcdData[GCDDevDataMsgProvider::Field_UPriceDisp] =
				Configuration::getInstance()->formatHobPrice( hpuvm.getProductTransaction()->getFixedWeightUnitPrice() );

			if ( prod->hasFrequentShopperLabel() )
				m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopUnitPrice] =
					Configuration::getInstance()->formatHobPrice( hpuvm.getProductTransaction()->getFixedWeightDiscountUnitPrice1() );
		}
		else
		{
			m_currentGcdData[GCDDevDataMsgProvider::Field_UPriceDisp] = "";
			m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopUnitPrice] = "";
		}
	}
}

void GCDSession::HandleGCDProductChange()
{
	Debug::trace( "GCD", "HandleGCDProductChange()" );
	Util::CLockCriticalSection lock( m_gcdDataCs );

	m_currentGcdData[GCDDevDataMsgProvider::Field_WtForAnnunc] = "-";
	m_currentGcdData[GCDDevDataMsgProvider::Field_FractionalAnnunc] = "-";
	m_currentGcdData[GCDDevDataMsgProvider::Field_PluNum] = "";
	m_currentGcdData[GCDDevDataMsgProvider::Field_ProdDescription] = "";
	m_currentGcdData[GCDDevDataMsgProvider::Field_MarqueeString] = "";
	m_currentGcdData[GCDDevDataMsgProvider::Field_AdvertType] = "";
	m_currentGcdData[GCDDevDataMsgProvider::Field_AdvertData] = "";
	m_currentGcdData[GCDDevDataMsgProvider::Field_UnitPriceAnnunc] = "-";

	//If there is a product, get it's non-changing data
	if ( m_pGcdCurrentProduct )
	{
		Debug::trace( "GCD", "handle product details..." );

		Configuration *config( Configuration::getInstance() );

		//plu
 		m_currentGcdData[GCDDevDataMsgProvider::Field_PluNum] = Integer::toString(m_pGcdCurrentProduct->getProductNum());

		switch( m_pGcdCurrentProduct->getProductType() ) 
		{
		case Product::PRODUCTTYPE_FIXEDWEIGHT:
			m_currentGcdData[GCDDevDataMsgProvider::Field_PluType] = "2";
			break;
		case Product::PRODUCTTYPE_BYCOUNT:
			m_currentGcdData[GCDDevDataMsgProvider::Field_PluType] = "3";
			break;
		case Product::PRODUCTTYPE_FLUIDOUNCE:
			m_currentGcdData[GCDDevDataMsgProvider::Field_PluType] = "4";
			break;
		default:
			// Product::PRODUCTTYPE_RANDOMWEIGHT or unknown
			m_currentGcdData[GCDDevDataMsgProvider::Field_PluType] = "1";
		}

		// SOF-2379
		if (( Product::PRODUCTTYPE_BYCOUNT == m_pGcdCurrentProduct->getProductType() ) ||
			( Product::PRODUCTTYPE_FLUIDOUNCE == m_pGcdCurrentProduct->getProductType() ))
		{
			if ( 1 < config->toIntCfg( fieldOffsetCfgUnitPricePrintMethod ) )
				m_currentGcdData[GCDDevDataMsgProvider::Field_UnitPriceAnnunc] = "up";
		}

		//product description
		HobString tmpDesc = m_pGcdCurrentProduct->getDescription();
		tmpDesc.replaceChar(_LF_, ' ');		//	GCDR-5: Replace linefeed chars with spaces for the GCD.
		tmpDesc.replaceChar(_CR_, ' ');		//	GCDR-5: Replace carriage return chars with spaces for the GCD.
		m_currentGcdData[GCDDevDataMsgProvider::Field_ProdDescription] = tmpDesc;

		//marquee string
		if ( m_pGcdCurrentProduct->getProductText( psttMarqueeText ) )
			m_currentGcdData[GCDDevDataMsgProvider::Field_MarqueeString] = m_pGcdCurrentProduct->getProductText(psttMarqueeText)->getText();

		// FT-1362: send item advert
		int advertType( config->toIntCfg( fieldOffsetCfgGCDProductAdvertType ) );

		// product graphic 1-4
		if (( AT_None < advertType ) && ( advertType <= AT_ProductGraphic4 ))
		{
			int graphicIndex( Graphic::GRAPHIC1 );
			Graphic *graphic( NULL );

			if ( AT_ProductGraphic4 == advertType )
				graphicIndex = Graphic::GRAPHIC4;
			else if ( AT_ProductGraphic3 == advertType )
				graphicIndex = Graphic::GRAPHIC3;
			else if ( AT_ProductGraphic2 == advertType )
				graphicIndex = Graphic::GRAPHIC2;

			if ( (graphic = m_pGcdCurrentProduct->getGraphic( graphicIndex )) )
				//if ( RECORD_OK == graphic->getErrorCode() ) // WEH-45: ignore error code
				{
					File graphicFile( graphic->getGraphicFile() );

					m_currentGcdData[GCDDevDataMsgProvider::Field_AdvertType] = "1";
					m_currentGcdData[GCDDevDataMsgProvider::Field_AdvertData] = graphicFile.getFullPath();
				}
		}
		else if (advertType == AT_CategoryPromo)
		{
			Category *prodCategory = m_pGcdCurrentProduct->getCategory();
			if (!prodCategory)
				Debug::trace("GCDSession", "HandleGCDProductChange():  The Product does not have a Category.");
			else
			{
				HobString playList = prodCategory->getPlaylist();
				if (playList.empty())
					Debug::trace("GCDSession", "HandleGCDProductChange():  The Category for the Product does not have a Playlist.");
				else
				{
					Debug::trace("GCDSession", "HandleGCDProductChange():  (GCDSessionDev, GcdSessionDisplayItemTransPlaylist):  [" + playList + "]");
					int type = 3;
					UpdateProductAdvert( type, playList );
				}
			}
		}

		if ( m_pGcdCurrentProduct->hasKbdNetWeightLabel() )
		{
			m_currentGcdData[GCDDevDataMsgProvider::Field_TPriceDisp] = "";
			m_currentGcdData[GCDDevDataMsgProvider::Field_UPriceDisp] = "";
			m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopTotalPrice] = "";
			m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopSavings] = "";
			m_currentGcdData[GCDDevDataMsgProvider::Field_TareDisp] = "";
			m_currentGcdData[GCDDevDataMsgProvider::Field_WgtDisp] = "";
		}
	}

	Debug::trace( "GCD", "HandleGCDProductChange() done" );
}

void GCDSession::SetStoreMarqueeMessage()
{
	Debug::trace( "GCD", "SetStoreMarqueeMessage()" );
	Util::CLockCriticalSection lock( m_gcdDataCs );

    HobString permanentMarquee = "";

	// Permanent Marquee would be in dept 0, not the currently active Dept
	Util::CSmartPtr<ProductText> mqText(
            ProductText::findRecord(psttMarqueeText, Configuration::getInstance()->getPermanentMarqueeNumber(), 0L, this));

    if ( mqText.notNull() )
        permanentMarquee = mqText->getText();
	
	if ( permanentMarquee.length() )
		m_currentGcdData[GCDDevDataMsgProvider::Field_StoreMarqueeString] = permanentMarquee.c_str();
		
	// FT-825 for empty (blank) marquee send space instead
	else
		m_currentGcdData[GCDDevDataMsgProvider::Field_StoreMarqueeString] = " ";
}

BOOL GCDSession::TimeToSendDataPack()
{
	DWORD dwCurrentTick = PAL_absclass::PAL_getTickCount();
	if (GetElapsedTicks(m_dwTickResendDataMark, dwCurrentTick) > TICKS_MAX_SEND_DATA_PACK_ELAPSED)
	{
		// send it now, and again in 4.5 seconds
		m_dwTickResendDataMark = dwCurrentTick;
		return true;
	}
	return false;
}

void GCDSession::GotHeartbeat()
{
	SetHeartbeat(PAL_absclass::PAL_getTickCount(), HeartbeatState_Connected);

	if (Debug::isTracing("GCD.heartbeat"))
		Debug::trace("GCD.heartbeat", HobString("GotHeartbeat: found heartbeat again"));

	if (!SessionManager::getInstance()->isGCDAttached())
	{
		if (Configuration::getInstance()->isRealTimeControllerEnabled() && RealTimeController::getInstance())
		{
			if (Debug::isTracing("GCD.heartbeat"))
				Debug::trace("GCD.heartbeat", HobString("heartbeat received: ADD GCD TO PERIPHERALS"));

			// Add GCD to the peripheral list in RealTimeController
			if ( m_fGCDAttachedToAPC )
				RealTimeController::getInstance()->GCDAttached(GCDUsb);
			else
				RealTimeController::getInstance()->GCDAttached(GCDNative);

			//	GCDR-162:	This makes no sense to me so I am commenting it out and adding the registerEvent for SysReqStatus to the
			//				init function. This code was the reason I could never request a SystemStatus message from the GCD.
			// Register for inquires to system status
			//registerEvent(GCDSessionDev, SysReqStatus);
		}
	}

	// clear any 'lost connection' or 'restarting' modal dialogs
	SessionManager *pSesMan = SessionManager::getInstance();
	if (pSesMan)
	{
		Session* pSession = pSesMan->getPrimaryConsoleSession();
		if (pSession)
		{
			if (Debug::isTracing("GCD.heartbeat"))
				Debug::trace("GCD.heartbeat", HobString("GotHeartbeat: removing dialogs"));
			pSession->removeDialogMessage(ErrGCDSessionDev, GCDSessionLostConnectionToGCD, HfcResponse::RESPONSESTYLE_OK);
			pSession->removeDialogMessage(ErrGCDSessionDev, GCDSessionRestartingGCD, HfcResponse::RESPONSESTYLE_OK);
		}
	}
}

void GCDSession::MissedHeartbeat()
{
	Util::CLockCriticalSection lock( m_heartbeatCounterCs );

	// Send a Heartbeat request to GCD
	// If GCD gets this it will respond back with a Heartbeat to
	// Let us know it is alive and capable of processing information
	GCDDevHeartbeatReqMsg hb;

	if ( Debug::isTracing("GCD") )
		DebugTrace( "Sending: %hs", hb.serialize() );

	send( &hb, m_gcdSocketDestId );

	if ( Debug::isTracing("GCD") )
		Debug::trace( "GCD", "Missed heartbeat\n" );

	++m_missingHeartBeatCount;

	switch ( m_heartbeatState )
	{
	case HeartbeatState_Connecting:
		if ( m_missingHeartBeatCount <= MAX_MISSED_HEARTBEAT_CONNECTING_COUNTER )
			return;
		break;
	default:
		if ( m_missingHeartBeatCount <= MAX_MISSED_HEARTBEAT_COUNTER )
			return;
		break;
	}

	Session *pSession = SessionManager::getInstance()->getPrimaryConsoleSession();

	if (pSession)
	{
		pSession->addDialogMessage(ErrGCDSessionDev, GCDSessionLostConnectionToGCD, HfcResponse::RESPONSESTYLE_OK);
	}

	RestartGCD();
	Debug::trace("GCD.heartbeat", HobString("GCDSession::MissedHeartbeat(): Restarted the GCD"));
}

void GCDSession::SetHeartbeat(DWORD dwCurrentTick, HeartbeatState state)
{
	Util::CLockCriticalSection lock(m_heartbeatCounterCs);
	m_missingHeartBeatCount = 0;
	m_heartbeatState = state;
	m_dwTickHeartbeatMark = dwCurrentTick;
}

void GCDSession::CheckHeartbeat()
{
	if (gcdCanTalk())
	{
		Util::CLockCriticalSection lock(m_heartbeatCounterCs);

		DWORD dwCurrentTick = PAL_absclass::PAL_getTickCount();

		if ( GetElapsedTicks( m_dwTickCheckHeartbeatMark, dwCurrentTick ) > TICKS_MAX_HEARTBEAT_ELAPSED )
		{
			m_dwTickCheckHeartbeatMark = dwCurrentTick;

			// Send a Heartbeat request to GCD
			// If GCD gets this it will respond back with a Heartbeat to
			// Let us know it is alive and capable of processing information
			GCDDevHeartbeatReqMsg hb;

			if ( Debug::isTracing("GCD") )
				DebugTrace( "Sending: %hs", hb.serialize() );

			send( &hb, m_gcdSocketDestId );

			if ( GetElapsedTicks( m_dwTickHeartbeatMark, dwCurrentTick ) > TICKS_MAX_HEARTBEAT_ELAPSED )
				MissedHeartbeat();
		}
	}
}

void GCDSession::ShutdownGCD()
{
	Debug::trace("GCDStartup", HobString("GCDSession::ShutdownGCD()"));
	APCCommManager *pAPCCommManager = NULL;

	if ( m_fGCDAttachedToAPC )
	{
		pAPCCommManager = APCCommManager::getInstance();
		if (!pAPCCommManager)
			return;
	}
	
	m_bCancelTransfer = TRUE;

	// tell the GCD manager to shut down
	GCDDevShutdownDeviceMsg msg;
	send(&msg, m_gcdSocketDestId);

	if ( !m_fHaveReconfiguredACP )
	{
		if ( pAPCCommManager )
		{
			if (Debug::isTracing("GCD"))
				Debug::trace("GCD", HobString("pAPCCommManager->SysCfgGCD() called from GCDSession."));
			pAPCCommManager->SysCfgGCD();
		}

		m_fHaveReconfiguredACP = TRUE;
	}

	Debug::trace("GCD", HobString("pAPCCommManager->SysGCDPwrOff() called from GCD Session."));

	CloseSocket();

	if ( pAPCCommManager )
	{
		if (Debug::isTracing("GCD"))
			Debug::trace("GCD", HobString("pAPCCommManager->SysGCDPwrOff()."));
		pAPCCommManager->SysGCDPwrOff();
	}

	{
		Util::CLockCriticalSection lock(m_playlistUpdateCs);

		m_playlistUpdateState = PlaylistUpdateState_None;
	}
}

void GCDSession::StartGCD()
{
	Debug::trace("GCDStartup", HobString("GCDSession::StartGCD()"));
	APCCommManager *pAPCCommManager = NULL;

	if ( m_fGCDAttachedToAPC )
	{
		pAPCCommManager = APCCommManager::getInstance();
		if (!pAPCCommManager)
			return;
	}

	if ( pAPCCommManager )
	{
		// Send a "sysGCDPwrOn" message to the APC. This will cause the GCD to power up and enumerate.
		if (Debug::isTracing("GCD"))
			Debug::trace("GCD", HobString("GCDSession::StartGCD() calling pAPCCommManager->SysGCDPwrOn()"));
		pAPCCommManager->SysGCDPwrOn();
	}

	SetHeartbeat(PAL_absclass::PAL_getTickCount(), HeartbeatState_Connecting);

	m_connectingState = ConnectingState_Start;

	m_fSentPeripheralRequestCodeUpdate = FALSE;

	m_bCancelTransfer = FALSE;

	GCDOperational = NOT_INITIALIZED;	// GCDR-183
}

void GCDSession::RestartGCD()
{
	Session *pSession = SessionManager::getInstance()->getPrimaryConsoleSession();
	Debug::trace( "GCDStartup", "GCDSession::RestartGCD()" );

	if ( pSession )
	{
		pSession->addDialogMessage( ErrGCDSessionDev, GCDSessionRestartingGCD, HfcResponse::RESPONSESTYLE_OK );

		// FT-1059: restart of GCD process on HTi scale
		if ( pSession->isPrimarySession() )
			if ( Configuration::getInstance()->toBoolCfgI( CfgInternal::GCDENABLEDSTANDALONE ) )
				if ( m_gcdRestartCount < MAX_RESTARTS )
				{
					int rc;
#if defined( __linux__ )
					static const char stopGcdProcess[] = "pkill gcd";
					static const char startGcdProcess[] = "DISPLAY=:0 /usr/bin/gcd -platform linuxfb:fb=/dev/fb2 &";

					Debug::trace( "GCDStartup", "killing the GCD process..." );
					rc = system( stopGcdProcess );
					Debug::trace( "GCDStartup", "result=" + Integer::toString( rc ) );

					Debug::trace( "GCDStartup", "starting the GCD process..." );
					rc = system( startGcdProcess );
					Debug::trace( "GCDStartup", "result=" + Integer::toString( rc ) );
#else
					// HW-47
					static const char stopGcdProcess[] = "taskkill /F /IM gcd.exe";
					static const char startGcdProcess[] = "c:\\program files\\hobart\\gcd.exe";

					Debug::trace( "GCDStartup", "killing the GCD process..." );
					rc = system( stopGcdProcess );
					Debug::trace( "GCDStartup", "result=" + Integer::toString( rc ) );

					Debug::trace( "GCDStartup", "starting the GCD process..." );
					rc = RunInteractiveProcess( startGcdProcess );
					Debug::trace( "GCDStartup", "result=" + Integer::toString( rc ) );

#endif
					MessageLogger::logMessage( MessageLogger::InfoLog, "GCD process was killed and restarted (" + Integer::toString( ++m_gcdRestartCount ) + ")!" );
				}
	}

	if ( SessionManager::getInstance()->isGCDAttached() )
	{
		// Set the GCD as detached so that we stop attempting to reset the GCD
		if (( Configuration::getInstance()->isRealTimeControllerEnabled() ) && ( RealTimeController::getInstance() ))
		{
			// Set GCD status in the peripheral list in RealTimeController
			RealTimeController::getInstance()->GCDDetached();
			Debug::trace( "GCD", "RestartGCD: Telling the RTC that the GCD is detached" );
		}
	}

	Debug::trace( "GCD", "GCDSession::RestartGCD() calling ShutdownGCD" );
	ShutdownGCD();

	GcdRestarted = TRUE;	// GCDR-183
	StartGCD();

	Debug::trace( "GCD", "GCDSession::RestartGCD() called StartGCD" );

#if !defined( __linux__)

	// Hang out until the IP address shows up
	if ( m_fGCDAttachedToAPC )
	{
		setLLIP( 40 );
		//setLLIP( 140 );  //Changed time out from 40 seconds to 140 seconds
	}

	Debug::trace( "GCD", "GCDSession::RestartGCD() called setLLIP(40)" );
#endif
}

#if !defined( __linux__)
HANDLE GCDSession::GetCurrentUserToken()
{
	BOOL bRet;
	
	// Get the list of all terminal sessions 
    PWTS_SESSION_INFO pSessionInfo( NULL );
    DWORD dwCount( 0 );

    if ( !(bRet = WTSEnumerateSessions( WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount )) )
	{
		Debug::trace( "GCD", "Failed to enumerate sessions: " + Integer::toString(GetLastError()) );
        return NULL;
	}

    // look over obtained list in search of the active session
    int dwSessionId( 0 );

    for ( DWORD i( 0 ); i < dwCount; ++i )
    {
        WTS_SESSION_INFO si( pSessionInfo[i] );

        if ( WTSActive == si.State )
        { 
            // If the current session is active \96 store its ID
            dwSessionId = si.SessionId;
            break;
        }
    } 	
	
	WTSFreeMemory( pSessionInfo );

    // Get token of the logged in user by the active session ID
    HANDLE currentToken( NULL );

    if ( !(bRet = WTSQueryUserToken( dwSessionId, &currentToken )) )
    {
		Debug::trace( "GCD", "Error getting token for session id " + Integer::toString( dwSessionId ) + ": " + Integer::toString( GetLastError() ) );
        return NULL;
    }

	// Duplicate token for caller
    HANDLE primaryToken( NULL );

    if ( !(bRet = DuplicateTokenEx( currentToken, TOKEN_ASSIGN_PRIMARY | TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &primaryToken )) )
		Debug::trace( "GCD", "Error duplicating token: " + Integer::toString( GetLastError() ) );

	CloseHandle( currentToken );

    return primaryToken;
}

BOOL GCDSession::RunInteractiveProcess( const std::string& processPath, const std::string& arguments )
{
	BOOL bRet;
	
	// Format command string
    std::string command( "\"" + processPath + "\"" );
	TCHAR commandLine[1024];

    if ( arguments.length() )
        command += " " + arguments;

	if ( sizeof(commandLine) <= arguments.length() )
    {
		Debug::trace( "GCD", "Command line too long!" );
		return FALSE;
    }

	mbstowcs( commandLine, command.c_str(), 1024 );

	// Note:
	// The scale service runs in an isolated session 0... this behavior is new to Windows starting
	// in Vista. In order to launch interactive applications we must find the desktop session and
	// impersonate it. Otherwise and UI will run in the isolated session and not be seen.

    // Get token of the current user
    HANDLE primaryToken( GetCurrentUserToken() );

    if ( !primaryToken )
    {
		Debug::trace( "GCD", "Cannot run process bad token" );
        return FALSE;
    }

    // Get all necessary environment variables of logged in user
    // to pass them to the process
    void *lpEnvironment( NULL ); 

    if ( !(bRet = CreateEnvironmentBlock( &lpEnvironment, primaryToken, FALSE )) )
    {
		Debug::trace( "GCD", "Error creating env block " + Integer::toString( GetLastError() ) );

		CloseHandle( primaryToken );
		return FALSE;
    }

    // Start the process on behalf of the current user 
    STARTUPINFO startupInfo;
	ZeroMemory( &startupInfo, sizeof(startupInfo) );
    startupInfo.cb = sizeof(startupInfo);
	startupInfo.lpDesktop = TEXT("winsta0\\default");

    PROCESS_INFORMATION processInfo;
	ZeroMemory( &processInfo, sizeof(processInfo) );

	SECURITY_ATTRIBUTES securityAttrib;
	ZeroMemory( &securityAttrib, sizeof(securityAttrib) );
	securityAttrib.nLength = sizeof(securityAttrib);

	Debug::trace( "GCD", "Creating process '" + command + "'..." );

    bRet = CreateProcessAsUser(
		primaryToken,
		NULL,
		commandLine,
		&securityAttrib,
		&securityAttrib,
		FALSE,
		NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT,
		lpEnvironment,
		NULL,
		&startupInfo,
		&processInfo );

	if ( !bRet )
		Debug::trace( "GCD", "Error creating process " + Integer::toString( GetLastError() ) );
	else
	{
		Debug::trace( "GCD", "Process Id " + Integer::toString( processInfo.dwProcessId ) + " started!" );

		CloseHandle( processInfo.hProcess );
		CloseHandle( processInfo.hThread );
	}
	
	DestroyEnvironmentBlock( lpEnvironment );
    CloseHandle( primaryToken );

    return bRet;
}
#endif

void GCDSession::CheckConnection()
{
	ConnectingState prevConnectingState;
	
	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	do
	{
		prevConnectingState = m_connectingState;
		
		switch (m_connectingState)
		{
		case ConnectingState_Start:
				
			if ( m_fGCDAttachedToAPC )
			{
				//Debug::trace("GCDStartup", HobString("ConnectingState_Start:  (m_hGcdDiscoverySocket == -1) = ") + Bool::toString((m_hGcdDiscoverySocket == -1)));
				int error = 0;
				if (-1 != m_hGcdDiscoverySocket)
				{
					//Debug::trace("GCDStartup", "ConnectingState_Start:  PAL_closesocket(m_hGcdDiscoverySocket)");
					error = PAL_absclass::PAL_closesocket(m_hGcdDiscoverySocket);
					if (-1 == error)		//	GCDR-205: This was not always getting initialized properly.
					{
						error = PAL_absclass::PAL_wsaGetLastError();
						Debug::trace("GCDStartup", "Call to PAL_closesocket failed with error " + Integer::toString(error));
					}
				}

				m_hGcdDiscoverySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

				u_long nonBlockingIo = 1;
				error = PAL_absclass::PAL_ioctlsocket(m_hGcdDiscoverySocket, PAL_FIONBIO, (u_long*)&nonBlockingIo);
				if (-1 == error)
				{
					error = PAL_absclass::PAL_wsaGetLastError();
					//Debug::trace("GCDStartup", "Call to PAL_ioctlsocket on gcd discovery socket failed with error " + Integer::toString(error));
					DebugTrace("Call to PAL_ioctlsocket on gcd discovery socket failed with error %d.", error);
				}

				sockaddr_in hlxAddrIn;
				hlxAddrIn.sin_family = AF_INET;
				hlxAddrIn.sin_addr.s_addr = htonl(INADDR_ANY);
				hlxAddrIn.sin_port = htons(config->toIntCfgI(CfgInternal::GCDDISCOVERYPORT));
				error = ::bind(m_hGcdDiscoverySocket, reinterpret_cast<const sockaddr*>(&hlxAddrIn), sizeof(sockaddr_in)); // MSVC2010
				if (error == 0)
				{
					m_connectingState = ConnectingState_DiscoveryWaiting;
					Debug::trace("GCDStartup", HobString("m_connectingState = ConnectingState_DiscoveryWaiting"));
				}
				else if (-1 == error)
				{
					error = PAL_absclass::PAL_wsaGetLastError();
					DebugTrace("Call to bind on gcd discovery socket failed with error %d.", error);
				}
			}

			// Native GCD (i.e. Fastrack)			
			else
			{
				Debug::trace("GCDStartup", HobString("ConnectingState_Start:  checking native GCD socket..."));

				// no APC so just connect to already running GCD application
				if ( m_pGcdMsgSocket )
				{
					Debug::trace("GCDStartup", HobString("ConnectingState_Start:  connecting..."));

					m_pGcdMsgSocket->ensureValid();
					m_pGcdMsgSocket->setNonBlockingIO();
					m_pGcdMsgSocket->sConnect2();

					if ( m_pGcdMsgSocket->isValid() )
					{
						m_sendErrors = 0;
						PAL_absclass::PAL_setEvent( m_gcdIsConnected );
							
						m_connectingState = ConnectingState_Connected; 
						GCDOperational = CONNECTED;		// GCDR-205
						Debug::trace("GCDStartup", HobString("m_connectingState = ConnectingState_Connected"));
					}
				}
			}
			
			break;	//	Seems like there should be a break here. [CCD 06/07/13]
			
		case ConnectingState_DiscoveryWaiting:
			{
				Debug::trace("GCDStartup", "ConnectingState_DiscoveryWaiting");

				char recvBuff[256];
				memset(recvBuff, 0, sizeof(recvBuff));

				sockaddr_in fromAddrIn;
				int fromAddrInSize = sizeof(fromAddrIn);

#if defined( __linux__ )
				int nRecv = recvfrom(m_hGcdDiscoverySocket, recvBuff, sizeof(recvBuff), MSG_NOSIGNAL, reinterpret_cast<sockaddr*>(&fromAddrIn), reinterpret_cast<socklen_t*>(&fromAddrInSize));
#else
				int nRecv = recvfrom(m_hGcdDiscoverySocket, recvBuff, sizeof(recvBuff), 0, reinterpret_cast<sockaddr*>(&fromAddrIn), &fromAddrInSize);
#endif
				if (nRecv > 0)
				{
					std::string gcdDiscoveryMessage = config->toStringCfgI(CfgInternal::GCDDISCOVERYMESSAGE);
					if (util_string::stristr(recvBuff, gcdDiscoveryMessage.c_str()) != 0)
					{
						PAL_absclass::PAL_closesocket(m_hGcdDiscoverySocket);
						m_hGcdDiscoverySocket = -1;

						m_ipAddressGcd = inet_ntoa(fromAddrIn.sin_addr);

						m_pGcdMsgSocket->ensureValid();
						m_pGcdMsgSocket->setNonBlockingIO();
						m_pGcdMsgSocket->setAddress(m_ipAddressGcd.c_str());
						m_pGcdMsgSocket->setPort(config->toIntCfgI(CfgInternal::GCDSERVERPORT));
						m_connectingState = ConnectingState_DiscoveryDone; 
						Debug::trace("GCDStartup", HobString("m_connectingState = ConnectingState_DiscoveryDone"));
					}
				}
				else if (-1 == nRecv)
				{
					int error = PAL_absclass::PAL_wsaGetLastError();
					if (WSAEWOULDBLOCK != error)
					{
						//Debug::trace("GCDStartup", "Call to recvfrom on gcd discovery socket failed with error " + Integer::toString(error));
						DebugTrace("Call to recvfrom on gcd discovery socket failed with error %d.", error);
					}
				}
			}
			break;
			
		case ConnectingState_DiscoveryDone:
			{
				Debug::trace("GCDStartup", "ConnectingState_DiscoveryDone");
				m_pGcdMsgSocket->ensureValid();
				m_pGcdMsgSocket->setNonBlockingIO();
				if (m_pGcdMsgSocket->sConnect3())
				{
					m_connectingState = ConnectingState_Connected; 
					GCDOperational = CONNECTED;		// GCDR-205
					Debug::trace("GCDStartup", HobString("m_connectingState = ConnectingState_Connected"));
				}
			}
			break;
			
		case ConnectingState_Connected:
			{
				Debug::trace("GCDStartup", "ConnectingState_Connected");
				PAL_TIME_ZONE_STRUCT timeZone;
				if (PAL_TIME_ZONE_INVALID != PAL_absclass::PAL_getTimeZoneInformation(&timeZone))
				{
					Debug::trace( "GCDStartup", "Sending Time Info Message..." );

					PAL_TIME_STRUCT localTime;
					PAL_absclass::PAL_getLocalTime(&localTime);
					GCDDevSetTimeInfo setTimeInfoMsg(&timeZone, &localTime);
					send(&setTimeInfoMsg, m_gcdSocketDestId);
				}
				else
				{
					DebugTrace("Call to PAL_absclass::PAL_getTimeZoneInformation failed with error %d.", ::PAL_absclass::PAL_getLastError());
				}

				if ( m_fGCDAttachedToAPC )
				{
					m_fGcdStoreLogoUploaded = FALSE;
					Debug::trace( "GCDStartup", "Update GCDStoreLogo..." );
					if (UpdateGCDStoreLogo())
					{
						m_connectingState = ConnectingState_StoreLogoUpdateWaiting;
						m_dwTickStoreLogoUpdateMark = PAL_absclass::PAL_getTickCount();
					}
					else
					{
						m_connectingState = ConnectingState_StoreLogoUpdateDone;
					}
				}

				// Native GCD (i.e. Fastrack)			
				else
				{
					// nothing to do, image filename is embedded in the layout file
					m_connectingState = ConnectingState_StoreLogoUpdateDone;
					m_fGcdStoreLogoUploaded = TRUE;
				}
			}
			break;
			
		case ConnectingState_StoreLogoUpdateWaiting:
			{
				if ((m_fGcdStoreLogoUploaded) || (GetElapsedTicks(m_dwTickStoreLogoUpdateMark, PAL_absclass::PAL_getTickCount()) > TICKS_MAX_GENERIC_TIMEOUT))
				{
					m_connectingState = ConnectingState_StoreLogoUpdateDone;
				}
			}
			break;
			
		case ConnectingState_StoreLogoUpdateDone:
			{
				m_fGcdDisplayXMLLoaded = FALSE;

				sendLoadDisplayObjectXMLToGCD();
				
				m_connectingState = ConnectingState_LoadDisplayXMLWaiting;
				m_dwTickLoadDisplayXMLMark = PAL_absclass::PAL_getTickCount();
			}
			break;
			
		case ConnectingState_LoadDisplayXMLWaiting:
			{
				if ((m_fGcdDisplayXMLLoaded) || (GetElapsedTicks(m_dwTickLoadDisplayXMLMark, PAL_absclass::PAL_getTickCount()) > TICKS_MAX_GENERIC_TIMEOUT))
				{
					m_connectingState = ConnectingState_LoadDisplayXMLDone;
				}
			}
			break;
			
		case ConnectingState_LoadDisplayXMLDone:
			{
				m_fGcdPlaylistLoaded = FALSE;

				// send Playback Wait Time from configuration
				GCDDevSetPlaybackWaitTimeMsg setPlaybackWaitTime(Integer::toString(config->toIntCfg(fieldOffsetCfgGCDPlaybackWaitTime)));
				send(&setPlaybackWaitTime, m_gcdSocketDestId);

				// send Playlist file name from configuration
				//Debug::trace( "GCD", "Sending GCD Playlist File to use...\n" );
				//Debug::trace( "GCD", gcd.getGCDPlaylist() );
				sendPlaylistFilenameToGCD();

				m_connectingState = ConnectingState_LoadPlaylistWaiting;
				m_dwTickLoadPlaylistMark = PAL_absclass::PAL_getTickCount();
			}
			break;
			
		case ConnectingState_LoadPlaylistWaiting:
			{
				if ((m_fGcdPlaylistLoaded) || (GetElapsedTicks(m_dwTickLoadPlaylistMark, PAL_absclass::PAL_getTickCount()) > TICKS_MAX_GENERIC_TIMEOUT))
				{
					m_connectingState = ConnectingState_LoadPlaylistDone;
				}
			}
			break;
			
		case ConnectingState_LoadPlaylistDone:
			{
				// send language and weigh mode
				//Debug::trace( "GCD", "Send GCD Language and Weigh Mode...\n" );
				GCDDevLanguageAndWeighModeMsg languageAndWeighModeMsg((GcdLanguage)config->toIntCfg(fieldOffsetCfgGCDLanguage), 
					((MetSig::isScaleAvoir()) ? WeighMode_Avoir : WeighMode_Metric) );
				send(&languageAndWeighModeMsg, m_gcdSocketDestId);

				// send Volume setting from configuration
				GCDDevSetVolumeMsg setVolumeMsg(config->toIntCfg(fieldOffsetCfgGCDVolume));
				send(&setVolumeMsg, m_gcdSocketDestId);

				// FT-841
				m_gcdUnitSeparator = std::string( 1, config->getDecimalSeparator() );
				m_gcdCurrencySymbol = config->getPrimaryMonetarySymbol();

				GCDDevSetUnitSeparatorMsg setUnitSeparatorMsg( m_gcdUnitSeparator );
				send( &setUnitSeparatorMsg, m_gcdSocketDestId );

				GCDDevSetCurrencySymbolMsg setCurrencySymbolMsg( m_gcdCurrencySymbol );
				send( &setCurrencySymbolMsg, m_gcdSocketDestId );

				// send Permenant Marquee message if it exists
				ResetGCDValues();
				SetStoreMarqueeMessage();
				GCDDevDataMsg msg(this);
				send(&msg, m_gcdSocketDestId);
				
				Debug::trace("GCDStartup", "Set GCD Operational");
				//Debug::trace( "GCD", "Set GCD Operational" );
				m_connectingState = ConnectingState_Complete;
			}
			break;
			
		case ConnectingState_Complete:
			{
				Debug::trace("GCDStartup", "GCD connecting state complete!");

				//	If there is a product and we have done a restart GCDR-183
				if (( m_pGcdCurrentProduct ) && ( GcdRestarted ))
				{
					Debug::trace("GCDStartup", "resend product due to restart");

					RestoreGCDValues();
					HandleGCDProductChange();

					GCDDevDataMsg msg( this );
					send( &msg, m_gcdSocketDestId );
				}

				GcdRestarted = FALSE;			// GCDR-183
				GCDOperational = INITIALIZED;	// GCDR-183
			}
			break;
			
		}
	}
	while (prevConnectingState != m_connectingState);
}

void GCDSession::FirmwareSourceUploadFinished(UploadFileInfo* pFileInfo, FileUploadFinishedState UNUSED(finishedState), LPCSTR UNUSED(szErrorDescription))
{
	HobString deviceApplicationUpdatesDir = Configuration::getInstance()->toStringCfgI(CfgInternal::DEVICEAPPLICATIONUPDATESDIR);
	HobString deviceApplicationUpdatesSourceFileName = Configuration::getInstance()->toStringCfgI(CfgInternal::DEVICEAPPLICATIONUPDATESSOURCEFILENAME);

	util_filesystem::Filename destinationBinPath(deviceApplicationUpdatesDir.c_str(), deviceApplicationUpdatesSourceFileName.c_str());
	if (0 == util_string::stricmp(pFileInfo->m_remotePath.getFullPath().c_str(), destinationBinPath.getFullPath().c_str()))
	{
		GCDDevStartFirmwareUpdateMsg msg(pFileInfo->m_remotePath.getFullPath().c_str());
		send(&msg, m_gcdSocketDestId);
	}
}

void GCDSession::PlaylistUploadFinished(UploadFileInfo* pFileInfo, FileUploadFinishedState finishedState, LPCSTR szErrorDescription)
{
	Util::CLockCriticalSection lock(m_playlistUpdateCs);

	HobString deviceApplicationUpdatesDir = Configuration::getInstance()->toStringCfgI(CfgInternal::DEVICEAPPLICATIONUPDATESDIR);

	if (0 != util_string::stricmp(pFileInfo->m_localPath.getFullPath().c_str(), pFileInfo->m_sourcePath.getFullPath().c_str()))
	{
		if (Debug::isTracing("GCD"))
			DebugTrace("Deleting file %s from temp location after upload.", pFileInfo->m_localPath.getFullPath().c_str());
		std::wstring localFileFullPath = util_string::stringtowstring(pFileInfo->m_localPath.getFullPath());
		if (!PAL_absclass::PAL_deleteFile(localFileFullPath.c_str()))
		{
			DebugTrace("PAL_absclass::PAL_deleteFile of file %s failed with error %d.", localFileFullPath.c_str(), PAL_absclass::PAL_getLastError());
		}
	}

	if (FileUploadFinishedState_Fail == finishedState)
	{
		m_playlistUpdateState = PlaylistUpdateState_None;

		if (szErrorDescription)
		{
			// create email and send
			SendGcdFtpErrorEmailEvent sgpeee(szErrorDescription);
			dispatchEvent(&sgpeee);
		}
	}
	else
	{
		util_filesystem::Filename destinationPlaylist(Configuration::getInstance()->toStringCfg(fieldOffsetCfgGCDPlaylistFile));
		destinationPlaylist.setPath(deviceApplicationUpdatesDir.c_str());
		if (0 == util_string::stricmp(pFileInfo->m_remotePath.getFullPath().c_str(), destinationPlaylist.getFullPath().c_str()))
		{
			GCDDevStartPlaylistUpdateMsg msg(destinationPlaylist.getFullPath().c_str());
			send(&msg, m_gcdSocketDestId);
		}
	}
}

void GCDSession::PlaylistRequestedFileUploadFinished(UploadFileInfo* pFileInfo, FileUploadFinishedState finishedState, LPCSTR szErrorDescription)
{
	Util::CLockCriticalSection lock(m_playlistUpdateCs);

	if (0 != util_string::stricmp(pFileInfo->m_localPath.getFullPath().c_str(), pFileInfo->m_sourcePath.getFullPath().c_str()))
	{
		if (Debug::isTracing("GCD"))
			DebugTrace("Deleting file %s from temp location after upload.", pFileInfo->m_localPath.getFullPath().c_str());
		std::wstring localFileFullPath = util_string::stringtowstring(pFileInfo->m_localPath.getFullPath());
		if (!PAL_absclass::PAL_deleteFile(localFileFullPath.c_str()))
		{
			DebugTrace("PAL_absclass::PAL_deleteFile of file %s failed with error %d.", localFileFullPath.c_str(), PAL_absclass::PAL_getLastError());
		}
	}

	if (FileUploadFinishedState_Fail == finishedState)
	{
		m_playlistUpdateState = PlaylistUpdateState_None;

		if (szErrorDescription)
		{
			// create email and send
			SendGcdFtpErrorEmailEvent sgpeee(szErrorDescription);
			dispatchEvent(&sgpeee);
		}
	}
	else
	{
		if (PlaylistUpdateState_None != m_playlistUpdateState)
		{
			std::list< RequestFileUpload >::iterator it;
			for (it = m_playlistRequestFileUploads.begin(); m_playlistRequestFileUploads.end() != it; ++ it)
			{
				if (0 == util_string::stricmp(pFileInfo->m_remotePath.getFullPath().c_str(), it->m_remotePath.getFullPath().c_str()))
				{
					m_playlistRequestFileUploads.erase(it);
					break;
				}
			}

			if (!m_playlistRequestFileUploads.empty())
			{
				switch (m_playlistUpdateState)
				{
				case PlaylistUpdateState_Usb:
					UploadFile(m_playlistRequestFileUploads.back().m_sourcePath, m_playlistRequestFileUploads.back().m_remotePath,
						TRUE, NULL, NULL, THREAD_PRIORITY_NORMAL, &GCDSession::PlaylistRequestedFileUploadFinished);
					break;
				case PlaylistUpdateState_Ftp:
					UploadFtpFile(m_ftpUpdateAddress, m_ftpUpdateUsername, m_ftpUpdatePassword, m_playlistRequestFileUploads.back().m_sourcePath,
						m_playlistRequestFileUploads.back().m_remotePath, TRUE, THREAD_PRIORITY_NORMAL, &GCDSession::PlaylistRequestedFileUploadFinished);
					break;
				default:
					break;
				}
			}
		}
	}
}

void GCDSession::StoreLogoUploadFinished(UploadFileInfo* UNUSED(pFileInfo), FileUploadFinishedState UNUSED(finishedState), LPCSTR UNUSED(szErrorDescription))
{
	m_fGcdStoreLogoUploaded = TRUE;
}

void GCDSession::noMessageFromMessengerProcessingImpl()
{
	Debug::trace( "GCDSessionIdle", "noMessageFromMessengerProcessingImpl()" );

	//Only send messages if the scale is not in the process of shutting down
	if (( !m_fShuttingDown ) && ( gcdCanTalk() ))
	{
		//Only start to send weight data when we've started up the GCD Manager and went through the entire process
		if ( SessionManager::getInstance()->isGCDAttached() )
		{
			Configuration *config = Configuration::getInstance();

			//if no product, reset all values and set the non transaction weight (in case we still have data in them)
			if ( !m_pGcdCurrentProduct )
			{
				//reset all product values, 
				Debug::trace( "GCDSessionIdle", "resetting gcd values" );
				ResetGCDValues();
			}

			//center of zero annunciator
			HfcWeigherStatus *ws( getCurrentWeight() );

			if ( ws )
			{
				Debug::trace( "GCDSessionIdle", "setting dr" );
				Util::CLockCriticalSection lock( m_gcdDataCs );
				bool dualRange = false;
				if ( config )
					dualRange = MetSig::isWeigherConfiguredForDualRange();

				// FT-89: CZ set from product message
				//m_currentGcdData[GCDDevDataMsgProvider::Field_CZAnnunc] = ((getCurrentWeight()->isZeroStableWeight()) ? "CZ" : "-");
				m_currentGcdData[GCDDevDataMsgProvider::Field_DualRangeEnabled] = (( dualRange ) ? "DR" : "-");
					
				if ( dualRange )
					m_currentGcdData[GCDDevDataMsgProvider::Field_DualRangeDirection] = Integer::toString( ws->getDualRangeDirection() );
			}

			// FT-433 weigher attached
			if ( config )
			{
				Debug::trace( "GCDSessionIdle", "setting weigher" );
				Util::CLockCriticalSection lock( m_gcdDataCs );

				m_currentGcdData[GCDDevDataMsgProvider::Field_WeigherAttached] = (config->isWeigherAttached() ? "W" : "");
			}

			//Set the non-transaction weight value to send
			if ( !m_pGcdCurrentProduct )
			{
				Debug::trace( "GCDSessionIdle", "setting non-transaction weight" );
				SetNonTransactionGCDWeight();
			}

			//only send weight data if any of the weight data changed
			if (( DidGCDFieldDataChange() ) || ( TimeToSendDataPack() ))
			{
				DWORD dwCurrentTick = PAL_absclass::PAL_getTickCount();

				// reset time to send to 4.5 seconds from now
				m_dwTickResendDataMark = dwCurrentTick;

				// don't send future data changes until 250ms from now
				// "They are sent only when data changes, up to four times a second." (LLD)
				m_dwGCDFieldDataChangeTickMark = dwCurrentTick;

				//clear the buffer we use to hold the data string sent to GCD
				GCDDevDataMsg msg( this );

				//send data via named pipe if there is data
				Debug::trace( "GCDSessionIdle", "sending..." );
				send( &msg, m_gcdSocketDestId );

				Debug::trace( "GCDSessionIdle", "save values" );
				SaveGCDValues();
			}
		}
	}

	Debug::trace( "GCDSessionIdle", "noMessageFromMessengerProcessingImpl() done" );
}

void GCDSession::noMessageFromMessengerProcessing()
{
	Debug::trace( "GCDSessionIdle", "noMessageFromMessengerProcessing()" );

	if ( !m_fShuttingDown )
	{
		CheckHeartbeat();
		CheckConnection();
	}

	noMessageFromMessengerProcessingImpl();
	PAL_absclass::PAL_sleep( 100 ); // FT-634: moved here from impl function

	Debug::trace( "GCDSessionIdle", "noMessageFromMessengerProcessing() done" );
}

void GCDSession::showErrorStateToUser( UpdateError* ue, ShowState ss )
{
	bool showError;
	bool isMedia;
	bool isUSB;
	Session *pSession;
	enum ErrGCDSessionDev errCode;
	static enum ErrGCDSessionDev prevErrorCode = GCDSessionNothing;

	// No critical section here but that's only
	// becuase everywhere it's used is already
	// protected. Don't update ue data without
	// making sure it's protected by m_playlistUpdateCs
	
	// to adjust according to what ends up
	// being the policy. USB/FTP done
	// messages differ
	switch( ue->method )
	{
		case USBMediaManual:
			showError = true;
			isUSB = true;
			isMedia = true;
			break;
		case USBMediaAutomatic:
			showError = false;
			isUSB = true;
			isMedia = true;
			break;
		case FTPMediaManual:
			showError = true;
			isUSB = false;
			isMedia = true;
			break;
		case FTPMediaAutomatic:
			showError = false;
			isUSB = false;
			isMedia = true;
			break;
		case USBSystemManual:
			showError = true;
			isUSB = true;
			isMedia = false;
			break;
		case USBSystemAutomatic:
			showError = false;
			isUSB = true;
			isMedia = false;
			break;
		case FTPSystemManual:
			showError = true;
			isUSB = false;
			isMedia = false;
			break;
		case FTPSystemAutomatic:
			showError = false;
			isUSB = false;
			isMedia = false;
			break;
		default:
			showError = false;
			isUSB = false;
			isMedia = false;
			break;
	}

	if( showError )
	{
		if( ue->isError )
		{
			if( ue->verbose )
			{
				// to do: Here we need to figure out how to show a custom string
				// and do that instead
				errCode = isUSB? GCDSessionNothing: GCDSessionNothing;
			}
			else
			{
				if( isMedia )
				{
					errCode = isUSB? GCDSessionMediaUSBUpdateFailure: GCDSessionMediaFTPUpdateFailure;
				}
				else
				{
					// to do: we should have a usb specific version for symetry
					errCode = isUSB? GCDSessionFirmwareUSBUpdateFailed: GCDSessionFirmwareFTPUpdateFailed;
				}
			}
		}
		else 
		{
			// There is no error so if 
			// we are supposed to show something
			// show success, which can vary by USB
			// or media update. If at the beginning
			// Of the process just show that

			// This is when an update starts manually
			if( ue->atBeginning )
			{
				errCode = isMedia? GCDSessionUpdatingGCDMedia: GCDSessionUpdatingGCDFirmware;
			}
			// This is when an update finishes successfully
			else
			{
				if( isMedia )
				{
					errCode = isUSB? GCDSessionMediaUSBUpdateSuccess: GCDSessionMediaFTPUpdateSuccess;
				}
				else
				{
					errCode = isUSB? GCDSessionFirmwareUSBUpdateSuccess: GCDSessionFirmwareFTPUpdateSuccess;
				}
			}
		} // isError

		pSession = SessionManager::getInstance()->getPrimaryConsoleSession();
		if( pSession )
		{
			// handle the showstate overrides that depend on the error context
			if( showError && ((ss == AlwaysShowError) || (ss == ShowOnlyIfError && ue->isError) || (ss == ShowOnlyIfNotError && !ue->isError)) )
			{
				// Sometimes we get the same or nearly the same error more than once. (Ex: a known fail on the scale but the GCD sees it too and sends an error back.)
				if( prevErrorCode != errCode )
				{
					pSession->removeDialogMessage(ErrGCDSessionDev, prevErrorCode, HfcResponse::RESPONSESTYLE_OK);
					pSession->addDialogMessage( ErrGCDSessionDev, errCode, HfcResponse::RESPONSESTYLE_OK);
					prevErrorCode = errCode;
				}
			}
		}
	} //showError
}


// override from AbstractSession
//virtual
void GCDSession::initializationBeforeRun()
{
    Session::initializationBeforeRun();

	//register the events we care about
	registerEvent(ProductDev, ProductChangeSmart);
	registerEvent(ProductDev, ProductStatusEvent);
	registerEvent(SessionDev, SnShutdownNonPrimary);
	registerEvent(ConfigurationDev, CfgSaved);
	registerEvent(Weigher, WgStatus); // note: this is processed in the base class

	registerEvent(GCDSessionDev, GcdSessionRestart);
	registerEvent(GCDSessionDev, GcdSessionShuttingDown);
	registerEvent(GCDSessionDev, GcdSessionColorTestOn);
	registerEvent(GCDSessionDev, GcdSessionColorTestWhiteOn);
	registerEvent(GCDSessionDev, GcdSessionColorTestBlackOn);
	registerEvent(GCDSessionDev, GcdSessionColorTestOff);
	registerEvent(GCDSessionDev, GcdSessionEnglish);
	registerEvent(GCDSessionDev, GcdSessionFrench);
	registerEvent(GCDSessionDev, GcdSessionSpanish);
	registerEvent(GCDSessionDev, GcdSessionAvoir);
	registerEvent(GCDSessionDev, GcdSessionMetric);
	registerEvent(GCDSessionDev, GcdSessionSetVolume);
	registerEvent(GCDSessionDev, GcdSessionNowServingCustomer);
	registerEvent(GCDSessionDev, GcdSessionDismissNowServingCustomer);
	registerEvent(GCDSessionDev, GcdSessionUpdateViaFtpNow);
	registerEvent(GCDSessionDev, GcdSessionUpdateViaUsbNow);
	registerEvent(GCDSessionDev, GcdSessionSetPlaybackWaitTime);
	registerEvent(GCDSessionDev, GcdSessionRNDISSettingsChanged);
	registerEvent(GCDSessionDev, GcdSessionUpdateLayout);
	registerEvent(GCDSessionDev, GcdSessionUpdatePlaylist);
	registerEvent(GCDSessionDev, SysReqStatus);
	registerEvent(GCDSessionDev, GcdSessionDisplayItemTransImage);
	registerEvent(GCDSessionDev, GcdSessionDisplayItemTransText);
	registerEvent(GCDSessionDev, GcdSessionDisplayItemTransPlaylist);
	registerEvent(GCDSessionDev, GcdSessionColorTestBlankScreenOn);
	registerEvent(GCDSessionDev, GcdSessionColorTestBlankScreenOff);

	registerEvent(VersionManagerDev, PeripheralUpdateCode);

	SetStoreMarqueeMessage();

	//prepack annunciator
	if ( Configuration::getInstance()->isPrepackScale() )
		m_currentGcdData[GCDDevDataMsgProvider::Field_PrepackAnnunc] = "PP";

}


void GCDSession::sessionMessageProcess(SessionMessage *msg)
{
	//Debug::trace("GCDSession", "sessionMessageProcess()");
	if (!m_fShuttingDown)
	{
		CheckHeartbeat();
		CheckConnection();
	}

	if (!msg)
		return;

	Configuration *config = Configuration::getInstance();
	if (!config)
		return;

	DebugTrace( "sessionMessageProcess(): dev %d enter...", msg->getDeviceType() );

	UINT8 eventType;
	switch(msg->getDeviceType())
	{
		case GCDDev:
			{
				//Debug::trace("GCDSession", "sessionMessageProcess() from GCDDev");
				GotHeartbeat();

				eventType = msg->getEventType();

				DebugTrace( "sessionMessageProcess(): GCDDev event %d enter...", eventType );

				switch (eventType)
				{
				case GcdStatus:
					{
						Debug::trace("GCDSession", "Received GCDDev, GcdStatus");
						HfcGCDDevStatus* pStatusObject = dynamic_cast< HfcGCDDevStatus* >(msg->getHfcObject());
						if (!pStatusObject)
						{
							return;
						}

						const UINT8 partNumber[] = { 1,2,3,4,5,6,7,8,9,0 };
						SystemStatusInfo *statusInfo = new SystemStatusInfo(0, 0, 0, 0, partNumber, 0, 0);
						GCDSystemStatus gcdStatus(this, statusInfo, 1, 1, 0, 1, 0, 1, 0, 1, 0);
						gcdStatus.setDDRTestPassed(pStatusObject->getDDRTestPassed());
						gcdStatus.setBoardVersion(pStatusObject->getBoardVersion());
						gcdStatus.setWinCEVersion(pStatusObject->getWinCEVersion());
						gcdStatus.setGCDDLLSoftwareVersion(pStatusObject->getGcdDllVersion());
						gcdStatus.setGCDMGRSoftwareVersion(pStatusObject->getGcdMgrVersion());
						const char *buf = gcdStatus.serialize();
						UINT len = gcdStatus.getSerializedLength();
						Debug::trace("handleGraphicalCustomerDisplayMessage", HobString("sending gcd system status: ") + HobString(Hex::toString(buf, len)));
						dispatchEvent(&gcdStatus, this);
						delete statusInfo;

						if (!m_fSentPeripheralRequestCodeUpdate)
						{
							PeripheralRequestCodeUpdate reqGcdManagerPrnCodeUpdate(0, pStatusObject->getBoardVersion(), pStatusObject->getGcdMgrVersion());
							dispatchEvent(&reqGcdManagerPrnCodeUpdate);
							m_fSentPeripheralRequestCodeUpdate = TRUE;
						}
					}
					break;
				case GcdFileInfo:
					{
						HfcGCDDevFileInfo* pFileInfoObject = dynamic_cast< HfcGCDDevFileInfo* >(msg->getHfcObject());
						if (!pFileInfoObject)
						{
							return;
						}

						Util::CLockCriticalSection lock(m_updatingFilesCs);
						std::list< UploadFileInfo* >::iterator it = m_updatingFiles.begin();
						while (it != m_updatingFiles.end())
						{
							UploadFileInfo* pUpdateFileInfo = *it;
							if (util_string::stricmp(pUpdateFileInfo->m_remotePath.getFullPath().c_str(), pFileInfoObject->getPath()) == 0)
							{
								// Continuing special case of an expected file missing from server.
								// This is my quick way to check for a missing file
								// If both sides are all zeros, then it's a missing
								// file and we need to send the uploaded message anyway
								// or the gcd loses track. Other errors (for now)
								// are a fail and are unprdictable.
								int tempTotals = pFileInfoObject->getFileSize() + pFileInfoObject->getLastWriteTime() + pUpdateFileInfo->m_dwLocalFileSize + pUpdateFileInfo->m_dwLocalLastWriteTime;
								if ( (tempTotals != 0) ||
									 (pFileInfoObject->getFileSize() != pUpdateFileInfo->m_dwLocalFileSize) ||
									 (pUpdateFileInfo->m_dwLocalLastWriteTime > pFileInfoObject->getLastWriteTime() )
									)
								{
									StartChildThread(&GCDSession::FinishUploadFile, pUpdateFileInfo, "GCDSession::FinishUploadFile", FALSE, pUpdateFileInfo->m_finishThreadPriority);
								}
								else
								{
									if (pUpdateFileInfo->m_fpFileUploadFinished)
									{
										(this->*pUpdateFileInfo->m_fpFileUploadFinished)(pUpdateFileInfo, FileUploadFinishedState_AlreadyExists, NULL);
									}
									delete pUpdateFileInfo;
								}
								it = m_updatingFiles.erase(it);
							}
							else
							{
								++ it;
							}
						}
					}
					break;
				case GcdRequestFileUpload:
					{
						Util::CLockCriticalSection lock(m_playlistUpdateCs);

						HfcGCDDevRequestFileUpload* pFileUploadObject = dynamic_cast< HfcGCDDevRequestFileUpload* >(msg->getHfcObject());
						if (!pFileUploadObject)
						{
							return;
						}

						RequestFileUpload playlisRequestFileUpload;
						playlisRequestFileUpload.m_sourcePath.setPath(m_updateDirectoryPath.getPath());
						playlisRequestFileUpload.m_sourcePath.setFileName(pFileUploadObject->getFilename());
						playlisRequestFileUpload.m_remotePath.setFullPath(pFileUploadObject->getUploadPath());

						if (m_playlistRequestFileUploads.empty())
						{
							switch (m_playlistUpdateState)
							{
							case PlaylistUpdateState_Usb:
								UploadFile(playlisRequestFileUpload.m_sourcePath, playlisRequestFileUpload.m_remotePath,
									TRUE, NULL, NULL, THREAD_PRIORITY_NORMAL, &GCDSession::PlaylistRequestedFileUploadFinished);
								break;
							case PlaylistUpdateState_Ftp:
								UploadFtpFile(m_ftpUpdateAddress, m_ftpUpdateUsername, m_ftpUpdatePassword, playlisRequestFileUpload.m_sourcePath,
									playlisRequestFileUpload.m_remotePath, TRUE, THREAD_PRIORITY_NORMAL, &GCDSession::PlaylistRequestedFileUploadFinished);
								break;
							default:
								break;
							}
						}

						m_playlistRequestFileUploads.push_front(playlisRequestFileUpload);
					}
					break;
				case GcdFirmwareUpdateStaged:
					{
						//Debug::trace("GCDSession", "sessionMessageProcess() GCDDev, GcdFirmwareUpdateStaged");
						Session *pSession = SessionManager::getInstance()->getPrimaryConsoleSession();
						if (pSession)
						{
							pSession->addDialogMessage(ErrGCDSessionDev, GCDSessionUpdatingGCDFirmware, HfcResponse::RESPONSESTYLE_OK);
						}

						RestartGCD();
//						Debug::trace("GCD", HobString("sessionMessageProcess: case:GcdFirmwareUpdateStaged callingRestartGCD"));
					}
					break;
				case GcdFirmwareUpdateCompleted:
					{
						//Debug::trace("GCDSession", "sessionMessageProcess() GCDDev, GcdFirmwareUpdateCompleted");
						Session *pSession = SessionManager::getInstance()->getPrimaryConsoleSession();
						if (pSession)
						{
							pSession->removeDialogMessage(ErrGCDSessionDev, GCDSessionUpdatingGCDFirmware, HfcResponse::RESPONSESTYLE_OK);
						}

						RestartGCD();
//						Debug::trace("GCD", HobString("sessionMessageProcess: case:GcdFirmwareUpdateCompleted callingRestartGCD"));
					}
					break;
				case GcdPlaylistUpdateStaged:
					{
						//Session *pSession = SessionManager::getInstance()->getPrimaryConsoleSession();
						//if (pSession)
						//{
						//	pSession->addDialogMessage(ErrGCDSessionDev, GCDSessionUpdatingGCDMedia, HfcResponse::RESPONSESTYLE_OK);
						//}

//						if (Debug::isTracing("GCD"))
//							Debug::trace("GCD", HobString("GCD playlist update has staged."));
					}
					break;
				case GcdPlaylistUpdateCompleted:
					{
						Util::CLockCriticalSection lock(m_playlistUpdateCs);

						if (Debug::isTracing("GCD"))
							Debug::trace("GCD", HobString("GCD playlist update has completed, loading playlist."));

						// Tell the user what happened (if there's a user to see it.)
						showErrorStateToUser( &updateError, AlwaysShowError );

						m_playlistUpdateState = PlaylistUpdateState_None;
						sendPlaylistFilenameToGCD();
					}
					break;
				case GcdHeartbeatResponse:
					{
						GotHeartbeat();
					}
					break;
				case GcdLoadDisplayObjectXMLResponse:
					{
						if (Debug::isTracing("GCD"))
							Debug::trace("GCD", HobString("Received LoadDisplayObject Response from GCD"));
						m_fGcdDisplayXMLLoaded = TRUE;
					}
					break;
				case GcdLoadPlaylistResponse:
					{
						if (Debug::isTracing("GCD"))
							Debug::trace("GCD", HobString("Received LoadPlaylist Response from GCD"));
						m_fGcdPlaylistLoaded = TRUE;
					}
					break;
				case GcdError:
					{
						if (Debug::isTracing("GCD"))
							Debug::trace("GCD", HobString("!!!Received Error from GCD!!!"));

						HfcGCDDevError *pError = dynamic_cast< HfcGCDDevError* >(msg->getHfcObject());

						HobString err_msg(pError->getDescription());
						if (Debug::isTracing("GCD"))
							Debug::trace("GCD", HobString(pError->getDescription()));

						// create email and send
						SendGcdPlaylistErrorEmailEvent sgpeee(err_msg);
						dispatchEvent(&sgpeee);

						switch (pError->getCode())
						{
							case HfcGCDDevError::Code_DirectShowException:
							{
								HobString err_msg(pError->getDescription());
								HobString file_url(pError->getFileUrl());
								// create email and send
								SendGcdPlaylistErrorEmailEvent sgpeee(err_msg, file_url);
								dispatchEvent(&sgpeee);
								break;
							}
							case HfcGCDDevError::Code_ApplicationUpdateCaughtException:
							case HfcGCDDevError::Code_TimerTickCaughtException:
							case HfcGCDDevError::Code_PlaylistUpdateCaughtException:
							{
								HobString err_msg(pError->getDescription());

								// create email and send
								SendGcdFtpErrorEmailEvent sgpeee(err_msg);
								dispatchEvent(&sgpeee);
								if (HfcGCDDevError::Code_PlaylistUpdateCaughtException == pError->getCode())
								{
									Util::CLockCriticalSection lock(m_playlistUpdateCs);

									m_playlistUpdateState = PlaylistUpdateState_None;
								}
								break;
							}
							default:
							{
								Debug::trace("GCDSession", "Received GCDDev, default");
								break;
							}
						}
					}
					break;
				}
			}
			break;

		case GCDSessionDev:
			//Debug::trace("GCDSession", "sessionMessageProcess() from GCDSessionDev");
			
			// Do nothing if the GCD is not attached
			if (!SessionManager::getInstance()->isGCDAttached())
			{
				return;
			}

			eventType = msg->getEventType();

			DebugTrace( "sessionMessageProcess(): GCDSessionDev event %d enter...", eventType );

			switch (eventType)
			{
				case GcdSessionRestart:
					{
						//Debug::trace("GCDSession", "Received GCDSessionDev, GcdSessionRestart");
						RestartGCD();
						Debug::trace("GCD", HobString("sessionMessageProcess: case GCDSessionDev: case GcdSessionRestart: calling RestartGCD"));
					}
					break;
				case GcdSessionColorTestOn:
				case GcdSessionColorTestWhiteOn:
				case GcdSessionColorTestOff:
				case GcdSessionColorTestBlackOn:
				case GcdSessionColorTestBlankScreenOn:
				case GcdSessionColorTestBlankScreenOff:
				{
						HobString customImage;

						GenericHfcObject *pHfcObj( dynamic_cast<GenericHfcObject*>( msg->getHfcObject() ) );

						if ( pHfcObj )
							customImage = pHfcObj->getData();

						Debug::trace( "GCD", "sessionMessageProcess: case GCDSessionDev: event type " +
								Integer::toString( eventType ) + " '" + customImage + "'" );

						int gcdMsgType = 0;

						if ( GcdSessionColorTestOn == eventType )
						{
							gcdMsgType = 1;

							if ( customImage.length() )
								gcdMsgType = 3;
						}
						else if ( GcdSessionColorTestWhiteOn == eventType )
							gcdMsgType = 2;
						else if (GcdSessionColorTestBlackOn == eventType)
							gcdMsgType = 4;
						else if (GcdSessionColorTestBlankScreenOn == eventType)
						{
							gcdMsgType = 6;
							Debug::trace("GCD", HobString("Sending ColorTest GcdSessionColorTestBlankScreenOn"));
						}
						else if (GcdSessionColorTestBlankScreenOff == eventType)
						{
							gcdMsgType = 7;
							Debug::trace("GCD", HobString("Sending ColorTest GcdSessionColorTestBlankScreenOff"));
						}

						GCDDevTestColorMsg testColorMsg(gcdMsgType, customImage);
						send(&testColorMsg, m_gcdSocketDestId);
						
						if (Debug::isTracing("GCD"))
							Debug::trace("GCD", HobString("Sending ColorTest Cmd"));
					}
					break;
				// FT-1458
				case GcdSessionNowServingCustomer:
					{
						GenericHfcObject *pHfcObj( dynamic_cast<GenericHfcObject*>( msg->getHfcObject() ) );

						if ( !pHfcObj )
							return;

						HobString customer( pHfcObj->getData() );

						GCDDevNowServingCustomerNumberMsg nowServingMsg( customer );
						send( &nowServingMsg, m_gcdSocketDestId );

						if ( Debug::isTracing("GCD") )
							Debug::trace( "GCD", HobString( "Now Serving Customer: " ) + customer );
					}
					break;
				// FT-1488
				case GcdSessionDismissNowServingCustomer:
					{
						HobString customer( "" );

						GCDDevDismissNowServingCustomerNumberMsg dismissNowServingMsg( customer );
						send( &dismissNowServingMsg, m_gcdSocketDestId );

						if ( Debug::isTracing("GCD") )
							Debug::trace( "GCD", HobString( "Dismiss Now Serving Customer" ) );
					}
					break;
				case GcdSessionEnglish:
					{
						GCDDevLanguageAndWeighModeMsg languageAndWeighModeMsg(GcdLanguage_English);
						config->setCfg(fieldOffsetCfgGCDLanguage, inttostring((int)GcdLanguage_English));
						config->save(this);
						send(&languageAndWeighModeMsg, m_gcdSocketDestId);
						if (Debug::isTracing("GCD"))
							Debug::trace("GCD", HobString("Sending English Language Cmd"));
					}
					break;
				case GcdSessionFrench:
					{
						GCDDevLanguageAndWeighModeMsg languageAndWeighModeMsg(GcdLanguage_French);
						config->setCfg(fieldOffsetCfgGCDLanguage, inttostring((int)GcdLanguage_French));
						config->save(this);
						send(&languageAndWeighModeMsg, m_gcdSocketDestId);
						if (Debug::isTracing("GCD"))
							Debug::trace("GCD", HobString("Sending French Language Cmd"));
					}
					break;
				case GcdSessionSpanish:
					{
						GCDDevLanguageAndWeighModeMsg languageAndWeighModeMsg(GcdLanguage_Spanish);
						config->setCfg(fieldOffsetCfgGCDLanguage, inttostring((int)GcdLanguage_Spanish));
						config->save(this);
						send(&languageAndWeighModeMsg, m_gcdSocketDestId);
						if (Debug::isTracing("GCD"))
							Debug::trace("GCD", HobString("Sending Spanish Language Cmd"));
					}
					break;
				case GcdSessionAvoir:
				case GcdSessionMetric:
					{
						GCDDevLanguageAndWeighModeMsg languageAndWeighModeMsg((eventType == GcdSessionMetric) ? WeighMode_Metric : WeighMode_Avoir);
						send(&languageAndWeighModeMsg, m_gcdSocketDestId);
						Debug::trace("GCD", HobString("Sending Weigh Mode Cmd"));
					}
					break;
				case GcdSessionSetVolume:
					{
						GenericHfcObject* pHfcObj = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
						if (!pHfcObj)
						{
							return;
						}

						GCDDevSetVolumeMsg setVolumeMsg(Integer::valueOf(pHfcObj->getData()));
						send(&setVolumeMsg, m_gcdSocketDestId);
						Debug::trace("GCD", HobString("Sending SetVolume Cmd"));
					}
					break;
				case GcdSessionSetPlaybackWaitTime:
					{
						GenericHfcObject* pHfcObj = dynamic_cast<GenericHfcObject*>(msg->getHfcObject());
						if (!pHfcObj)
						{
							return;
						}

						GCDDevSetPlaybackWaitTimeMsg setPlaybackWaitTimeMsg(pHfcObj->getData());
						send(&setPlaybackWaitTimeMsg, m_gcdSocketDestId);
						Debug::trace("GCD", HobString("Sending SetPlaybackWaitTime Cmd"));
					}
					break;
				case GcdSessionUpdateViaFtpNow:
					{
						Util::CLockCriticalSection lock(m_playlistUpdateCs);

						HobString deviceApplicationUpdatesDir = config->toStringCfgI(CfgInternal::DEVICEAPPLICATIONUPDATESDIR);

						GCDFtpUpdateMsg ftpUpdateMsg(GCDSessionDev, GcdSessionUpdateViaFtpNow, dynamic_cast<GenericHfcObject *>(msg->getHfcObject())->getData());

						if (PlaylistUpdateState_None == m_playlistUpdateState)
						{
							m_playlistRequestFileUploads.clear();

							m_playlistUpdateState = PlaylistUpdateState_Ftp;
							m_ftpUpdateAddress = ftpUpdateMsg.getFtpAddress();
							m_ftpUpdateUsername = ftpUpdateMsg.getFtpUser();
							m_ftpUpdatePassword = ftpUpdateMsg.getFtpPassword();
							m_updateDirectoryPath.setPath(ftpUpdateMsg.getFtpDirectory());
							util_filesystem::Filename sourcePlaylist(config->toStringCfg(fieldOffsetCfgGCDPlaylistFile));
							sourcePlaylist.setPath(m_updateDirectoryPath.getPath());
							util_filesystem::Filename destinationPlaylist(config->toStringCfg(fieldOffsetCfgGCDPlaylistFile));
							destinationPlaylist.setPath(deviceApplicationUpdatesDir.c_str());

							// This time, this should display a 'we're starting the FTP update message but only if the update is via button push
							// Init the error data
							updateError.atBeginning = true;
							updateError.category = OS;
							updateError.errorNumber = 0;
							updateError.extendedError = 0;
							updateError.isError = false;
							updateError.method = ftpUpdateMsg.getAutomatic()? FTPMediaAutomatic: FTPMediaManual; // this variable may be used wrong but it works for now
							updateError.verbose = false;
							updateError.additionalText = "";
							showErrorStateToUser( &updateError, AlwaysShowError ); 

							UploadFtpFile(m_ftpUpdateAddress, m_ftpUpdateUsername, m_ftpUpdatePassword, sourcePlaylist,	destinationPlaylist, FALSE, THREAD_PRIORITY_NORMAL, &GCDSession::PlaylistUploadFinished);
						}
						else
						{
							// to do: need to add this state in. One for USB and one for FTP.
							Session *pSession = SessionManager::getInstance()->getPrimaryConsoleSession();
							if (pSession)
							{
								pSession->addDialogMessage(ErrGCDSessionDev, GCDSessionMediaUpdateInProgressGCD, HfcResponse::RESPONSESTYLE_OK);
							}
						}
					}
					break;
				case GcdSessionUpdateViaUsbNow:
					{
						Util::CLockCriticalSection lock(m_playlistUpdateCs);

						HobString deviceApplicationUpdatesDir = config->toStringCfgI(CfgInternal::DEVICEAPPLICATIONUPDATESDIR);

						if (PlaylistUpdateState_None == m_playlistUpdateState)
						{
							m_playlistRequestFileUploads.clear();

							m_playlistUpdateState = PlaylistUpdateState_Usb;
							HobString str = dynamic_cast<GenericHfcObject *> (msg->getHfcObject())->getData();
							util_filesystem::Filename fullFileNameFromParameter(str);
							m_updateDirectoryPath.setPath(str.c_str());
							util_filesystem::Filename sourcePlaylist(config->toStringCfg(fieldOffsetCfgGCDPlaylistFile));
							sourcePlaylist.setPath(m_updateDirectoryPath.getPath());
							util_filesystem::Filename destinationPlaylist(config->toStringCfg(fieldOffsetCfgGCDPlaylistFile));
							destinationPlaylist.setPath(deviceApplicationUpdatesDir.c_str());

							// This time, this should display a 'we're starting the USB update message. Shows always since that's the only way to do it
							// Init the error data
							// to do: macro this.
							updateError.atBeginning		= true;
							updateError.category		= OS;
							updateError.errorNumber		= 0;
							updateError.isError			= false;
							updateError.extendedError   = 0;
							updateError.method			= FTPMediaManual;
							updateError.verbose			= false;
							updateError.additionalText	= "";
							showErrorStateToUser( &updateError, AlwaysShowError ); 

							UploadFile(sourcePlaylist, destinationPlaylist, FALSE, NULL, NULL, THREAD_PRIORITY_NORMAL, &GCDSession::PlaylistUploadFinished);
						}
						else
						{
							Session *pSession = SessionManager::getInstance()->getPrimaryConsoleSession();
							if (pSession)
							{
								pSession->addDialogMessage(ErrGCDSessionDev, GCDSessionMediaUpdateInProgressGCD, HfcResponse::RESPONSESTYLE_OK);
							}
						}
					}
					break;
					
				case GcdSessionUpdateLayout:
					{
						// FT-1362
						Debug::trace( "GCD", "Update GCD DisplayObjects...\n" );

						sendLoadDisplayObjectXMLToGCD();
					}
					break;
					
				case GcdSessionUpdatePlaylist:
					{
						// send Playlist file name from configuration
						Debug::trace( "GCD", "Updating GCD Playlist File to use [" + config->toStringCfg(fieldOffsetCfgGCDPlaylistFile) + "]" );

						sendPlaylistFilenameToGCD();
					}
					break;

				case GcdSessionDisplayItemTransImage:
				case GcdSessionDisplayItemTransText:
				case GcdSessionDisplayItemTransPlaylist:
					{
						GenericHfcObject *pHfcObj( dynamic_cast<GenericHfcObject*>( msg->getHfcObject() ) );

						if ( !pHfcObj )
							return;

						int type( 0 );
						std::string data( pHfcObj->getData() );

						if ( GcdSessionDisplayItemTransImage == eventType )
						{
							Debug::trace( "GCD", "GcdSessionDisplayItemTransImage:  type = 1" );
							type = 1;
						}
						else if ( GcdSessionDisplayItemTransText == eventType )
						{
							Debug::trace( "GCD", "GcdSessionDisplayItemTransText:  type = 2" );
							type = 2;
						}
						else if ( GcdSessionDisplayItemTransPlaylist == eventType )
						{
							Debug::trace( "GCD", "GcdSessionDisplayItemTransPlaylist:  type = 3" );
							type = 3;
						}

						UpdateProductAdvert( type, data );
					}
					break;

				case SysReqStatus:
					{
						Debug::trace("GCDSession", "Received GCDSessionDev, SysReqStatus");
						registerEvent(GCDSessionDev, SysStatus, false);

						GCDDevGetStatusMsg msg;
						send(&msg, m_gcdSocketDestId);
					}
					break;
				case SysStatus:
					{
						// Send status to the UI
						Debug::trace("GCDSession", "Received GCDSessionDev, SysStatus");
						deregisterEvent(GCDSessionDev, SysStatus);

						// send version info to hook
#if defined( __linux__ )
	                    if (( Configuration::getInstance()->getCustomerHooksEnabled() ) && ( SessionManager::getInstance()->getPrimaryConsoleSession() ))
						{
							PeripheralManager *pm( SessionManager::getInstance()->getPrimaryConsoleSession()->getPeripheralManager() );

							if ( pm )
							{
								GCDSystemStatus systemStatusMsg( msg->getOriginalBuff().data() );
								
								const HobVersion swver( systemStatusMsg.getGCDMGRSoftwareVersion().getRevMajMinStr() );
								const HobVersion hwver( systemStatusMsg.getBoardVersion().getRevMajMinStr() );

								const std::string sw( swver.getRevisionString() );
								const std::string hw( hwver.getRevisionString() );

								const HfcPeripheralList list( pm->getCurrentList() );

								// GCDSystemStatus
								systemStatusMsg.show();

								Debug::trace("Session", "gcd versions: sw " + sw + ", hw " + hw);
								hobart_cust_hook_update_peripheral_version( GCDSessionDev, 0, list.getGCD(), sw.c_str(), hw.c_str() );
							}
						}
#endif

						msg->setFormatType(BrowserFormat);
#if 0
		//	Trying to get this to all Sessions, but it's not working yet.
						dispatchEvent(msg);
#else
						int bytesSent = 0;
						SessionManager	*pSesMan = NULL;
						Session			*pSession = NULL;
						pSesMan = SessionManager::getInstance();
						if (pSesMan)
							pSession = pSesMan->getPrimaryConsoleSession();
						if (pSession)
							bytesSent = pSession->send(msg, Messenger::DEST_UI);
					
						if (bytesSent < 0)
						{
							Debug::trace("handleGraphicalCustomerDisplayMessage", "GCDSessionDev, SysStatus RESPONSE MESSAGE COULD NOT BE SENT");
						}
#endif
					}
					break;
			}
			break;

		case VersionManagerDev:
			{
				Debug::trace("GCDSession", "sessionMessageProcess() from VersionManagerDev");
				eventType = msg->getEventType();
				switch (eventType)
				{
				case PeripheralUpdateCode:

					//Debug::trace("GCDSession", "Received VersionManagerDev, PeripheralUpdateCode");
					// Do nothing if the GCD is not attached
					if (!SessionManager::getInstance()->isGCDAttached())
					{
						return;
					}

					// message contains a PeripheralCodeUpdate object
					PeripheralCodeUpdate codeUpdate(msg->getOriginalBuff().data());

					if (codeUpdate.getVersionType() != HobVersion::GCDAppSW)
					{
						return;
					}

					switch (codeUpdate.getResponseType())
					{
					case SoftwareSystem::AvailableSameVersion:
						Debug::trace("GCDSession", HobString("AvailableSameVersion") );
						break;
					case SoftwareSystem::AvailableUpgrade:
						{
							HobString deviceApplicationUpdatesDir = config->toStringCfgI(CfgInternal::DEVICEAPPLICATIONUPDATESDIR);
							HobString deviceApplicationUpdatesSourceFileName = config->toStringCfgI(CfgInternal::DEVICEAPPLICATIONUPDATESSOURCEFILENAME);

							Debug::trace("GCDSession", HobString("AvailableUpgrade") );
							util_filesystem::Filename sourceBinPath(codeUpdate.getPath()->getFullPath(), deviceApplicationUpdatesSourceFileName.c_str());
							util_filesystem::File sourceBin(sourceBinPath);
							if (sourceBin.exists())
							{
								util_filesystem::Filename destinationBinPath(deviceApplicationUpdatesDir.c_str(), deviceApplicationUpdatesSourceFileName.c_str());
								UploadFile(sourceBinPath, destinationBinPath, FALSE, NULL, NULL, THREAD_PRIORITY_NORMAL, &GCDSession::FirmwareSourceUploadFinished);
							}
						}
						break;
					case SoftwareSystem::NotAvailable:
						// TODO: use default firmware file
						Debug::trace("GCDSession", HobString("NotAvailable") );
						break;
					case SoftwareSystem::retryCntrOver:
						{
							// TODO: abort and prevent further update attempts
							Debug::trace("GCDSession", HobString("retryCntrOver") );
						}
						break;
					default:
						{
							Debug::trace("GCDSession", HobString("unknown 0x") + Hex::toString((long)codeUpdate.getResponseType()) );
						}
						break;
					}

					break;
				}
			}
			break;

		case ConfigurationDev:
			{
				Debug::trace( "GCDSession", "sessionMessageProcess() from ConfigurationDev" );

				if ( CfgSaved == msg->getEventType() )
				{
					Configuration *config( Configuration::getInstance() );

					//configuration has changed, need to check if GCD Manager needs to be reloaded
					int layoutNum = config->toIntCfg( fieldOffsetCfgGCDLayoutNumber );
					int colorSchemeNum = config->toIntCfg( fieldOffsetCfgGCDColorSchemeNumber );
					bool useTransactionalLayout = config->toBoolCfg( fieldOffsetCfgGCDUseTransactionalLayout );
					int idleAdvertType = config->toIntCfg( fieldOffsetCfgGCDIdleAdvertType );
					HobString idleWebsiteAddress = config->toStringCfg( fieldOffsetCfgGCDIdleWebsiteAddress );

					// Update store marquee message if necessary
					SetStoreMarqueeMessage();

					if ( DidGCDFieldDataChange() )
					{
						//clear the buffer we use to hold the data string sent to GCD
						GCDDevDataMsg dataMsg( this );
						send( &dataMsg, m_gcdSocketDestId );

						SaveGCDValues();
					}

					//if layout Number or color scheme has changed, we need to kill existing GCD Manager
					//and launch a new one with the proper Top Level XML copied over
					if (( m_gcdLayoutNumber != layoutNum ) ||
						( m_gcdColorScheme != colorSchemeNum ) ||
						( m_gcdUseTransactionalLayout != useTransactionalLayout ) ||
						( m_gcdIdleAdvertType != idleAdvertType ) ||
						( m_gcdIdleWebsiteAddress != idleWebsiteAddress ))
					{
						m_gcdLayoutNumber = layoutNum;
						m_gcdColorScheme = colorSchemeNum;
						m_gcdUseTransactionalLayout = useTransactionalLayout;
						m_gcdIdleAdvertType = idleAdvertType;
						m_gcdIdleWebsiteAddress = idleWebsiteAddress;

						sendLoadDisplayObjectXMLToGCD();
					}

					WeighMode currentWeighMode = ((MetSig::isScaleAvoir()) ? WeighMode_Avoir : WeighMode_Metric);

					if ( m_scaleWeighMode != currentWeighMode )
					{
						m_scaleWeighMode = currentWeighMode;
						
						GCDDevLanguageAndWeighModeMsg languageAndWeighModeMsg( m_scaleWeighMode );
						send( &languageAndWeighModeMsg, m_gcdSocketDestId );
					}

					// FT-841
					std::string gcdUnitSeparator = std::string( 1, config->getDecimalSeparator() );
					std::string gcdCurrencySymbol = config->getPrimaryMonetarySymbol();

					if ( m_gcdUnitSeparator != gcdUnitSeparator )
					{
						m_gcdUnitSeparator = gcdUnitSeparator;

						GCDDevSetUnitSeparatorMsg setUnitSeparatorMsg( m_gcdUnitSeparator );
						send( &setUnitSeparatorMsg, m_gcdSocketDestId );
					}

					if ( m_gcdCurrencySymbol != gcdCurrencySymbol )
					{
						m_gcdCurrencySymbol = gcdCurrencySymbol;

						GCDDevSetCurrencySymbolMsg setCurrencySymbolMsg( m_gcdCurrencySymbol );
						send( &setCurrencySymbolMsg, m_gcdSocketDestId );
					}

					if (currentPlaylistFileName_ != config->toStringCfg(fieldOffsetCfgGCDPlaylistFile))
						sendPlaylistFilenameToGCD();
				}
			}
			break;

		case SessionDev:
		{
			//Debug::trace("GCDSession", "sessionMessageProcess() from SessionDev");
			//We only care about shutdown, anything else with session fall through to default case
			if (msg->getEventType() == SnShutdownNonPrimary)
			{
				//Debug::trace("GCDSession", "Received SessionDev, SnShutdownNonPrimary");
				//stop sending status messages to GCD Manager
				m_fShuttingDown = TRUE;

				// turn off the power to the GCD
				ShutdownGCD();
				//Debug::trace("GCD", HobString("GCDSession::sessionMessageProcess() case SessionDev: calling ShutdownGCD"));
			}
		}
		break;
		case ProductDev:
		{
			Debug::trace("GCDSession", "sessionMessageProcess() from ProductDev");

			//We only care about product change, anything else with product fall through to default case
			if (msg->getEventType() == ProductChangeSmart)
			{
				//Debug::trace("GCDSession", "Received ProductDev, ProductChange");
				SessionManager *pSesMan = SessionManager::getInstance();
				if (pSesMan)
				{
					Session* pSession = pSesMan->getPrimaryConsoleSession();
					if (pSession)
						currentMode = pSession->getCurrentMode();
				}

				//get the product
				HobString buff(msg->getOriginalBuff());
				ProductChangeMsg prodChange(buff.c_str());
				ProductManager *productManager = (ProductManager *) prodChange.getProductMgr();

				if (productManager)
				{
					Util::CSmartPtr<Product> prod = productManager->getCurrentProductSmartPtr();

					// FT-466 ignore dummy product
					if (( prod ) && ( prod->isDummyProduct() || prod->getItemIsMealSideItem() ))
					{
						Debug::trace("GCD", HobString("Ignoring dummy product"));
					}
					else
					{
						//Debug::trace("GCD", HobString("ProductChange received - new currentProduct is: ") + Hex::toString((LONG)(LONG_PTR)(prodChange.getProduct())));

						m_pGcdCurrentProduct = prod;

						HandleGCDProductChange();
					}
				}
			}
			else if ( msg->getEventType() == ProductStatusEvent )
			{
				// Will get these to update the total price on the customer display.
				// The first use of this was for Build-A-Meal, when sides are not free, we need to display the running total price to the customer

				HobString buff(msg->getOriginalBuff());
				ProductStatusMsg masterProduct(buff.c_str());
				if (ProductStatusMsg::ProductTransactionUpdate == masterProduct.getStatus() )
				{
					ProductTransaction *trans( masterProduct.getProductTransaction() );

					Debug::trace("GCDSession", "handling ProductStatus ProductTransactionUpdate");
					HfcProductUpdateValuesMsg hpuvm(trans);
					hpuvm.determineDisplayValues(BrowserFormat);

					Debug::trace("GCDSession", "handling ProductTransactionUpdate, new total price: " + hpuvm.getTotalPriceDisplay());

					// WEH-129: only update total price
					Util::CLockCriticalSection lock( m_gcdDataCs );
					m_currentGcdData[GCDDevDataMsgProvider::Field_TPriceDisp] = hpuvm.getTotalPriceDisplay();
					m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopTotalPrice] = hpuvm.getFsTotalPriceDisplay();
					m_currentGcdData[GCDDevDataMsgProvider::Field_FreqShopSavings] = hpuvm.getFsTotalSavingsDisplay();
				}
			}
		}
		break;

		default:
		{
			if (!((Error == msg->getDeviceType()) && (msg->getEventType() == ErrEOF)))
				Debug::trace("GCDSession", "sessionMessageProcess():  Received " + inttostring(msg->getDeviceType()) + ", " + inttostring(msg->getEventType()));
			if ((Error == msg->getDeviceType()) && (msg->getEventType() == ErrEOF))
			{
				// this is the message we get when there is no message from a custom added communication message
				// so we'll call the implimentation of the override that the UI destination normally uses
				noMessageFromMessengerProcessingImpl();
			}
			else
			{
				// Need to know what mode the primary console is in to process a WgStatus message correctly.
				// GCDSession is considered to be in the same mode as the primary console.
				SessionManager *pSesMan = SessionManager::getInstance();
				if (pSesMan)
				{
					Session* pSession = pSesMan->getPrimaryConsoleSession();
					if (pSession)
						currentMode = pSession->getCurrentMode();
				}

				Debug::trace("GCDSession", "sessionMessageProcess():  default case [" + inttostring(msg->getDeviceType()) + ", " + inttostring(msg->getEventType()) + "],  Calling Session::sessionMessageProcess()");
				Session::sessionMessageProcess(msg); // pass onto base class
			}
		}
		break;
	}
	//Debug::trace("GCDSession", "sessionMessageProcess():  DONE");
}

//
// CopyStoreLogo
//
// This function copies storelogo.* from HLX to GCD
//
BOOL GCDSession::UpdateGCDStoreLogo()
{
	if (Debug::isTracing("GCD"))
		Debug::trace("GCD", HobString("CopyStoreLogo"));

	LPCSTR rgszStoreLogoFileExts[] = { "jpg", "png", "bmp", "gif", "tif", "tiff" };

	util_filesystem::Filename localStoreLogoPath;
	DWORD dwLocalStoreLogoSize = 0;
	DWORD dwLocalStoreLogoLastWriteTime = 0;

	Configuration *config = Configuration::getInstance();
	HobString desktopStoreLogoPath = config->toStringCfgI(CfgInternal::DESKTOPSTORELOGOPATH);
	HobString desktopStoreFileName = config->toStringCfgI(CfgInternal::DESKTOPSTORELOGOFILENAME);
	HobString deviceStoreLogoPath = config->toStringCfgI(CfgInternal::DEVICESTORELOGOPATH);
	HobString deviceStoreFileName = config->toStringCfgI(CfgInternal::DEVICESTORELOGOFILENAME);

	std::vector< std::string > directoryList = PAL_absclass::PAL_getDirectoryList(desktopStoreLogoPath.c_str());

	DWORD i;
	// Find the newest Storelogo.* image file on the HLX
	for (i = 0; i < (sizeof(rgszStoreLogoFileExts)/sizeof(LPCTSTR)); i++)
	{
		util_filesystem::Filename localPath(desktopStoreLogoPath, desktopStoreFileName);
		localPath.setFileNameExtension(rgszStoreLogoFileExts[i]);

		std::vector< std::string >::iterator filenameIt;
		for (filenameIt = directoryList.begin(); filenameIt != directoryList.end(); ++ filenameIt)
		{
			if (0 == util_string::stricmp(filenameIt->c_str(), localPath.getFileName().c_str()))
			{
				struct stat localFileStat;
				stat(localPath.getFullPath().c_str(), &localFileStat);

				// Is it the newest file found so far?
				if (static_cast<DWORD>(localFileStat.st_mtime) > dwLocalStoreLogoLastWriteTime)
				{
					localStoreLogoPath = localPath;
					dwLocalStoreLogoSize = static_cast<DWORD>(localFileStat.st_size);
					dwLocalStoreLogoLastWriteTime = static_cast<DWORD>(localFileStat.st_mtime);
				}
			}
		}
	}

	// If a file was not found, return false
	if (dwLocalStoreLogoSize > 0)
	{
		util_filesystem::Filename remoteStoreLogoPath(deviceStoreLogoPath.c_str(), deviceStoreFileName);
		remoteStoreLogoPath.setFileNameExtension(localStoreLogoPath.getFileNameExtension());

		UploadFile(localStoreLogoPath, remoteStoreLogoPath, TRUE, &dwLocalStoreLogoSize, &dwLocalStoreLogoLastWriteTime, THREAD_PRIORITY_NORMAL, &GCDSession::StoreLogoUploadFinished);
		return TRUE;
	}
	else
	{
		Debug::trace("GCD", HobString("Failed to find storelogo file on scale"));
		return FALSE;
	}
}

void GCDSession::UpdateProductAdvert( int type, const std::string& data )
{
	Util::CLockCriticalSection lock( m_gcdDataCs );

	m_currentGcdData[GCDDevDataMsgProvider::Field_AdvertType] = Integer::toString( type );
	m_currentGcdData[GCDDevDataMsgProvider::Field_AdvertData] = data;
}

BOOL GCDSession::StartChildThread(ChildThread::RunMethod fpRun, void *pParam, LPCSTR szName, BOOL fRestartActive, int priority)
{
	Util::CLockCriticalSection lock(m_childThreadsCs);

	ChildThread* pChildThreadRun = NULL;

	//Debug::trace("GCDSession", HobString("StartChildThread()"));
	if (fRestartActive)
	{
		DWORD i;
		for (i = 0; i < m_childThreads.size(); ++ i)
		{
			if (m_childThreads[i]->IsRunning())
			{
				if (m_childThreads[i]->m_fpRun == fpRun)
				{
					pChildThreadRun = m_childThreads[i];
					if (!pChildThreadRun->Stop(1000))
					{
						//Debug::trace("GCDSession", HobString("StartChildThread():  returning false!"));
						return FALSE;
					}
					pChildThreadRun->m_pParam = pParam;
					pChildThreadRun->SetPriority(priority);
				}
			}
		}
	}

	if (!pChildThreadRun)
	{
		DWORD i;
		for (i = 0; i < m_childThreads.size(); ++ i)
		{
			if (!m_childThreads[i]->IsRunning())
			{
				pChildThreadRun = m_childThreads[i];
				pChildThreadRun->m_fpRun = fpRun;
				pChildThreadRun->m_pParam = pParam;
				pChildThreadRun->SetPriority(priority);
			}
		}
	}

	if (!pChildThreadRun)
	{
		pChildThreadRun = new ChildThread(this, fpRun, pParam, szName, priority);
		m_childThreads.push_back(pChildThreadRun);
	}

	pChildThreadRun->Start();

	//Debug::trace("GCDSession", HobString("StartChildThread():  returning TRUE"));
	return TRUE;
}

void GCDSession::UploadRemoteFile(const util_filesystem::Filename& localPath, const util_filesystem::Filename& remotePath,
	BOOL fCheckNew, int threadPriority, FileUploadFinishedMethod fpFileUploadFinished)
{
	HobString localTempDir = Configuration::getInstance()->toStringCfgI(CfgInternal::LOCALTEMPDIR);

	UploadFileInfo* pUploadFileInfo			= new UploadFileInfo();
	pUploadFileInfo->m_sourcePath			= localPath;
	pUploadFileInfo->m_localPath			= pUploadFileInfo->m_sourcePath.getFullPath();
	pUploadFileInfo->m_localPath.setPath(localTempDir.c_str());
	pUploadFileInfo->m_remotePath			= remotePath;
	pUploadFileInfo->m_fCheckNew			= fCheckNew;
	pUploadFileInfo->m_finishThreadPriority = threadPriority;
	pUploadFileInfo->m_fpFileUploadFinished = fpFileUploadFinished;

	StartChildThread(&GCDSession::FinishUploadRemoteFile, pUploadFileInfo, "GCDSession::FinishUploadRemoteFile", FALSE, pUploadFileInfo->m_finishThreadPriority);
}

DWORD GCDSession::FinishUploadRemoteFile(HANDLE UNUSED(hExitEvent), Util::CTLSThread* UNUSED(pThread), void *pParam)
{
	DWORD dwReturn = ERROR_SUCCESS;
	std::string error;
	UploadFileInfo* pUploadFileInfo = reinterpret_cast< UploadFileInfo* >(pParam);

	util_filesystem::File sourceFile(pUploadFileInfo->m_sourcePath);
	util_filesystem::File localFile(pUploadFileInfo->m_localPath);
	util_filesystem::Directory localDirectory(pUploadFileInfo->m_localPath.getPath());

	// Reset for when this finishes
	updateError.atBeginning = false;
	updateError.category = OS;
	updateError.errorNumber = 0;
	updateError.extendedError = 0;
	updateError.isError = false;
// 	counting on the start to set the updateError.method = ftpUpdateMsg.getAutomatic()? FTPMediaAutomatic: FTPMediaManual; // this variable may be used wrong but it works for now
	updateError.verbose = false;
	updateError.additionalText = "";

	if (!localDirectory.exists())
	{
		if (!localDirectory.create())
		{
			dwReturn = PAL_absclass::PAL_getLastError();
			DebugTrace("Directory::create of %s failed with error %d", localDirectory.getFullPath().c_str(), dwReturn);
			error.resize(128);
			sprintf(&error[0], "System error %d occurred while creating temp directory %s for transfer", dwReturn, localDirectory.getFullPath().c_str());

			// Set up the error info
			updateError.isError = true;
			updateError.errorNumber = dwReturn;
			updateError.extendedError = 0;
			updateError.category = OS;
		}
	}

	if (ERROR_SUCCESS == dwReturn)
	{
		if (!sourceFile.copyTo(localFile))
		{
			dwReturn = PAL_absclass::PAL_getLastError();
			DebugTrace("File::copyTo from %s to %s failed with error %d", sourceFile.getFullPath().c_str(), localFile.getFullPath().c_str(), dwReturn);
			error.resize(128);
			sprintf(&error[0], "System error %d occurred while copying from %s to %s", dwReturn, sourceFile.getFullPath().c_str(), localFile.getFullPath().c_str());

			// Set up the error info
			updateError.isError = true;
			updateError.errorNumber = dwReturn;
			updateError.extendedError = 0;
			updateError.category = OS;
		}
	}

	if( m_bCancelTransfer )
	{
		m_bCancelTransfer = FALSE;
		(this->*pUploadFileInfo->m_fpFileUploadFinished)(pUploadFileInfo, FileUploadFinishedState_Fail, error.c_str());
		updateError.isError = false;
	}
	else if (ERROR_SUCCESS == dwReturn)
	{
		struct stat localFileStat;
		stat(pUploadFileInfo->m_localPath.getFullPath().c_str(), &localFileStat);
		pUploadFileInfo->m_dwLocalFileSize = static_cast<DWORD>(localFileStat.st_size);
		pUploadFileInfo->m_dwLocalLastWriteTime = static_cast<DWORD>(localFileStat.st_mtime);

		if (pUploadFileInfo->m_fCheckNew)
		{
			Util::CLockCriticalSection lock(m_updatingFilesCs);

			GCDDevGetFileInfoMsg getFileInfoMsg(pUploadFileInfo->m_remotePath.getFullPath().c_str());
			send(&getFileInfoMsg, m_gcdSocketDestId);

			m_updatingFiles.push_back(pUploadFileInfo);
		}
		else
		{
			StartChildThread(&GCDSession::FinishUploadFile, pUploadFileInfo, "GCDSession::FinishUploadFile", FALSE, pUploadFileInfo->m_finishThreadPriority);
		}
	}
	else
	{
		if (pUploadFileInfo->m_fpFileUploadFinished)
		{
			error.append(" source path = ");
			error.append(pUploadFileInfo->m_sourcePath.getFullPath());
			(this->*pUploadFileInfo->m_fpFileUploadFinished)(pUploadFileInfo, FileUploadFinishedState_Fail, error.c_str());

			// Set up the error info
			updateError.isError = true;
			updateError.errorNumber = dwReturn;
			updateError.extendedError = 0;
			updateError.category = OS;
		}

		// call here becuase we won't get to the finished state on the gcd
		showErrorStateToUser( &updateError, ShowOnlyIfError );

		// send file upload message whether or not there was success, to notify GCD upload was attempted
		GCDDevFileUploadMsg fileUploadMsg(pUploadFileInfo->m_remotePath.getFullPath().c_str(), pUploadFileInfo->m_dwLocalFileSize, pUploadFileInfo->m_dwLocalLastWriteTime);
		send(&fileUploadMsg, m_gcdSocketDestId);
	}

	return dwReturn;
}

// Initiates the download of the playlist file
// Scale uses that to know whether or not to go
// get the rest of the files.
void GCDSession::UploadFtpFile(const std::string& address, const std::string& user,
	const std::string& password, util_filesystem::Filename ftpPath, const util_filesystem::Filename& remotePath, BOOL fCheckNew,
	int threadPriority, FileUploadFinishedMethod fpFileUploadFinished)
{
	HobString localTempDir = Configuration::getInstance()->toStringCfgI(CfgInternal::LOCALTEMPDIR);

	UploadFileInfo* pUploadFileInfo = new UploadFileInfo();
	pUploadFileInfo->m_address = address;
	pUploadFileInfo->m_ftpUser = user;
	pUploadFileInfo->m_ftpPassword = password;
	pUploadFileInfo->m_sourcePath = ftpPath;
	pUploadFileInfo->m_localPath = pUploadFileInfo->m_sourcePath.getFullPath();
	pUploadFileInfo->m_localPath.setPath(localTempDir.c_str());
	pUploadFileInfo->m_remotePath = remotePath;
	pUploadFileInfo->m_fCheckNew = fCheckNew;
	pUploadFileInfo->m_finishThreadPriority = threadPriority;
	pUploadFileInfo->m_fpFileUploadFinished = fpFileUploadFinished;

	StartChildThread(&GCDSession::FinishUploadFtpFile, pUploadFileInfo, "GCDSession::FinishUploadFtpFile", FALSE, pUploadFileInfo->m_finishThreadPriority);
}

// Do the (now FTP/SFTP) download of the playlist file
// 
DWORD GCDSession::FinishUploadFtpFile(HANDLE UNUSED(hExitEvent), Util::CTLSThread* UNUSED(pThread), void *pParam)
{
	DWORD dwReturn = ERROR_SUCCESS;
	bool succeedAnyway = false;
	std::string error;
	UploadFileInfo* pUploadFileInfo = reinterpret_cast< UploadFileInfo* >(pParam);

	// These variables are needed to diffrentiate the cURL call into
	// either FTP or SFTP. The URL between the two only differs by the
	// first character but the port and (unused) protocol variable
	// also change. Format of the URL should be ftp://<server>/<then the rest> or
	// sftp://<server>/<then the rest>. Forward slashes even on windows and a
	// trailing slash if you mean a directory (rather than a file.)
	INTERNET_PORT	port;
	int				protocol;
	std::string		url = GetPortProtocolUrl( pUploadFileInfo->m_address, "", &port, &protocol );

	// Reset for when this finishes
	updateError.atBeginning = false;
	updateError.category = OS;
	updateError.errorNumber = 0;
	updateError.extendedError = 0;
	updateError.isError = false;
// 	counting on the start to set the updateError.method = ftpUpdateMsg.getAutomatic()? FTPMediaAutomatic: FTPMediaManual; // this variable may be used wrong but it works for now
	updateError.verbose = false;
	updateError.additionalText = "";

	DWORD dwWait = PAL_absclass::PAL_waitForSingleObj(m_downloadFileThreadSemaphore, INFINITE);
	if (WAIT_OBJECT_0 == dwWait)
	{
		PAL_INET_HANDLE hNet = PAL_absclass::PAL_internetOpen("GCDSession::FinishUploadFtpFile", PAL_INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
		if (hNet)
		{
			// PAL call will manipulate these into something the cURL library can use.
			LPCSTR szUser	  = (pUploadFileInfo->m_ftpUser.length() > 0) ? pUploadFileInfo->m_ftpUser.c_str() : NULL;
			LPCSTR szPassword = (pUploadFileInfo->m_ftpPassword.length() > 0) ? pUploadFileInfo->m_ftpPassword.c_str() : NULL;
			
 		    // PAL, internally sets up the cURL libraries with this call so the rest work. Sored state referenced by
			// the hFtp handle.
			PAL_INET_HANDLE hFtp = PAL_absclass::PAL_internetConnect(hNet, url.c_str(),	port, szUser, szPassword, protocol, 0, 0);
			if (hFtp)
			{
				// Check for and create the directory on the local (scale's) file sytem if needed. Default location
				// is C:\\Program Files\\Hobart\\gcdTmp and is kept in the config.dat file.
				util_filesystem::Directory localDirectory(pUploadFileInfo->m_localPath.getPath());
				if (!localDirectory.exists())
				{
					if (!localDirectory.create())
					{
						dwReturn = PAL_absclass::PAL_getLastError();
						DebugTrace("Directory::create of %s failed with error %d", localDirectory.getFullPath().c_str(), dwReturn);
						error.resize(128);
						sprintf(&error[0], "System error %d occurred while creating temp directory %s for transfer", dwReturn, localDirectory.getFullPath().c_str());
					}
				}

				if (ERROR_SUCCESS == dwReturn)
				{
					url = pUploadFileInfo->m_sourcePath.getPath();
					url += "/";
					url += pUploadFileInfo->m_sourcePath.getFileName();
					//std::string fullSourcePath = pUploadFileInfo->m_sourcePath.getFullPath();
					// correct file path separator issue for dealing with a Linux FTP server
					
					// Make sure we have the right slash.
					fixTheSlashesForFtp( url );

					// Setup complete. Download the file from the ftp/SFTP server to the local file system.
					bool success = PAL_absclass::PAL_ftpGetFile(hFtp, url.c_str(), pUploadFileInfo->m_localPath.getFullPath().c_str(),false, PAL_FILE_ATTR_NORMAL, PAL_FTP_TRANSFER_TYPE_BINARY, 0);
					if (!success)
					{
						dwReturn = PAL_absclass::PAL_getLastError();
						if (ERROR_INTERNET_EXTENDED_ERROR == dwReturn)
						{
							// turn a file not found into a success and let the GCD sort it out.
							// This might grow into a list of errors we would want to ignore.
							// For now, only file-not-found.
							if( PAL_absclass::getExtendedError() == 78 )
							{
								succeedAnyway = true;
							}
							GetLastErrorExtendedResponse(&dwReturn, &error);
							DebugTrace("GCDSession::FinishUploadFtpFile: PAL_absclass::PAL_ftpGetFile failed. Error (%d), %s", dwReturn, error.c_str());

							updateError.category = CURL;
							updateError.extendedError = PAL_absclass::getExtendedError();
						}
						else
						{
							DebugTrace("GCDSession::FinishUploadFtpFile: PAL_absclass::PAL_ftpGetFile failed. Error (%d)", dwReturn);
							error.resize(128);
							sprintf(&error[0], "System error %d occurred on FTP get.", dwReturn);

							updateError.category = OS;
							updateError.extendedError = 0;
						}
					}
				}

				// Done. Tell PAL to close up all the handles it's opened. (Three I think.)
				PAL_absclass::PAL_internetCloseHandle(hFtp);
			}
			else
			{
				// Assumes PAL has set the error. Need to check this to make sure since
				// migration from wininet to cURL
				dwReturn = PAL_absclass::PAL_getLastError();

				updateError.isError = true;
				updateError.errorNumber = dwReturn;

				if (ERROR_INTERNET_EXTENDED_ERROR == dwReturn)
				{
					GetLastErrorExtendedResponse(&dwReturn, &error);
					DebugTrace("GCDSession::FinishUploadFtpFile: PAL_absclass::PAL_internetConnect failed. Error (%d), %s", dwReturn, error.c_str());

					updateError.category = CURL;
					updateError.extendedError = PAL_absclass::getExtendedError();
				}
				else
				{
					DebugTrace("GCDSession::FinishUploadFtpFile: PAL_absclass::PAL_internetConnect failed. Error (%d)", dwReturn);
					error.resize(128);
					sprintf(&error[0], "System error %d occurred on FTP connection.", dwReturn);

					updateError.category = OS;
					updateError.extendedError = 0;
				}
			}
			PAL_absclass::PAL_internetCloseHandle(hNet);
		}
		else
		{
			// InternetOpen failed. (Pretty unlikely)

			// Assumes PAL has set the error. Need to check this to make sure since
			// migration from wininet to cURL
			dwReturn = PAL_absclass::PAL_getLastError();

			updateError.isError = true;
			updateError.errorNumber = dwReturn;

			if (ERROR_INTERNET_EXTENDED_ERROR == dwReturn)
			{
				GetLastErrorExtendedResponse(&dwReturn, &error);
				DebugTrace("GCDSession::FinishUploadFtpFile: PAL_absclass::PAL_InternetOpen failed. Error (%d), %s", dwReturn, error.c_str());

				updateError.category = CURL;
				updateError.extendedError = PAL_absclass::getExtendedError();
			}
			else
			{
				DebugTrace("GCDSession::FinishUploadFile: PAL_InternetOpen failed. Error (%d)", dwReturn);
				error.resize(128);
				sprintf(&error[0], "System error %d occurred on FTP open.", dwReturn);

				updateError.category = OS;
				updateError.extendedError = 0;
			}
		}

		showErrorStateToUser( &updateError, ShowOnlyIfError );

		PAL_absclass::PAL_incSemaphore(m_downloadFileThreadSemaphore, 1, NULL);
	}

	if( m_bCancelTransfer )
	{
		m_bCancelTransfer = FALSE;
		(this->*pUploadFileInfo->m_fpFileUploadFinished)(pUploadFileInfo, FileUploadFinishedState_Fail, error.c_str());
		updateError.isError = false;
	}
	else if (ERROR_SUCCESS == dwReturn || succeedAnyway )
	{
		if( succeedAnyway )
		{
			pUploadFileInfo->m_dwLocalFileSize = 0;
			pUploadFileInfo->m_dwLocalLastWriteTime = 0;
		}
		else
		{
			struct stat localFileStat;
			stat(pUploadFileInfo->m_localPath.getFullPath().c_str(), &localFileStat);
			pUploadFileInfo->m_dwLocalFileSize = static_cast<DWORD>(localFileStat.st_size);
			pUploadFileInfo->m_dwLocalLastWriteTime = static_cast<DWORD>(localFileStat.st_mtime);
		}

		if (pUploadFileInfo->m_fCheckNew)
		{
			Util::CLockCriticalSection lock(m_updatingFilesCs);

			GCDDevGetFileInfoMsg getFileInfoMsg(pUploadFileInfo->m_remotePath.getFullPath().c_str());
			send(&getFileInfoMsg, m_gcdSocketDestId);

			m_updatingFiles.push_back(pUploadFileInfo);
		}
		else
		{
			StartChildThread(&GCDSession::FinishUploadFile, pUploadFileInfo, "GCDSession::FinishUploadFile", FALSE, pUploadFileInfo->m_finishThreadPriority);
		}
	}
	else
	{
		// an error we don't want to ignore
		if (pUploadFileInfo->m_fpFileUploadFinished)
		{
			error.append(" source path = ");
			error.append(pUploadFileInfo->m_sourcePath.getFullPath());
			(this->*pUploadFileInfo->m_fpFileUploadFinished)(pUploadFileInfo, FileUploadFinishedState_Fail, error.c_str());
		}

		// send file upload message whether or not there was success, to notify GCD upload was attempted
		GCDDevFileUploadMsg fileUploadMsg(pUploadFileInfo->m_remotePath.getFullPath().c_str(), pUploadFileInfo->m_dwLocalFileSize, pUploadFileInfo->m_dwLocalLastWriteTime);
		send(&fileUploadMsg, m_gcdSocketDestId);
	}

	return dwReturn;
}

// This initiates an upload of files to the GCD
void GCDSession::UploadFile(const util_filesystem::Filename& localPath, const util_filesystem::Filename& remotePath, BOOL fCheckNew,
	const DWORD* pdwLocalFileSize, const DWORD* pdwLocalLastWriteTime, int threadPriority, FileUploadFinishedMethod fpFileUploadFinished)
{
	UploadFileInfo* pUploadFileInfo = new UploadFileInfo();

	pUploadFileInfo->m_sourcePath = localPath;
	pUploadFileInfo->m_localPath = localPath;

	struct stat localFileStat;
	localFileStat.st_size = 0;
	localFileStat.st_mtime = 0;

	if (pdwLocalFileSize)
	{
		pUploadFileInfo->m_dwLocalFileSize = *pdwLocalFileSize;
	}
	else
	{
		if (0 == localFileStat.st_size)
		{
			stat(pUploadFileInfo->m_localPath.getFullPath().c_str(), &localFileStat);
		}
		pUploadFileInfo->m_dwLocalFileSize = static_cast<DWORD>(localFileStat.st_size);
	}

	if (pdwLocalLastWriteTime)
	{
		pUploadFileInfo->m_dwLocalLastWriteTime = *pdwLocalLastWriteTime;
	}
	else
	{
		if (0 == localFileStat.st_mtime)
		{
			stat(pUploadFileInfo->m_localPath.getFullPath().c_str(), &localFileStat);
		}
		pUploadFileInfo->m_dwLocalLastWriteTime = static_cast<DWORD>(localFileStat.st_mtime);
	}

	pUploadFileInfo->m_remotePath = remotePath;
	pUploadFileInfo->m_finishThreadPriority = threadPriority;
	pUploadFileInfo->m_fpFileUploadFinished = fpFileUploadFinished;

	if (fCheckNew)
	{
		Util::CLockCriticalSection lock(m_updatingFilesCs);

		GCDDevGetFileInfoMsg getFileInfoMsg(remotePath.getFullPath().c_str());
		send(&getFileInfoMsg, m_gcdSocketDestId);

		m_updatingFiles.push_back(pUploadFileInfo);
	}
	else
	{
		StartChildThread(&GCDSession::FinishUploadFile, pUploadFileInfo, "GCDSession::FinishUploadFile", FALSE, pUploadFileInfo->m_finishThreadPriority);
	}
}

DWORD GCDSession::FinishUploadFile(HANDLE UNUSED(hExitEvent), Util::CTLSThread* UNUSED(pThread), void *pParam)
{
	DWORD dwReturn = ERROR_SUCCESS;
	std::string error;
	UploadFileInfo* pUploadFileInfo = reinterpret_cast< UploadFileInfo* >(pParam);
	
	// Reset for when this finishes
	updateError.atBeginning = false;
	updateError.category = OS;
	updateError.errorNumber = 0;
	updateError.extendedError = 0;
	updateError.isError = false;
// 	counting on the start to set the updateError.method
	updateError.verbose = false;
	updateError.additionalText = "";

	// Make sure the local file exists. This will prevent an error if the previous get doesn't work
	// or if the file isn't on the usb drive. This function moves files from the local scale
	// drive (usb or temp dir) to the GCD.
	struct stat st;
	memset( &st, 0, sizeof(st) );

	if( 0 == stat( pUploadFileInfo->m_localPath.getFullPath().c_str(), &st ) && st.st_size != 0 )
	{
		// This upload to the GCD is always FTP so we don't need to check anything.
		// Return should be something like ftp://server/
		std::string empty("");
		std::string url = validateFtpPath(m_ipAddressGcd, empty, PAL_INTERNET_SERVICE_FTP);
		
		DWORD dwWait = PAL_absclass::PAL_waitForSingleObj(m_uploadFileThreadSemaphore, INFINITE);
		if (WAIT_OBJECT_0 == dwWait)
		{
			// Open some handles
			PAL_INET_HANDLE hNet = PAL_absclass::PAL_internetOpen("GCDSession::FinishUploadFile", PAL_INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
			if (hNet)
			{
				// Open some more... (saves a bit of global state for PAL and the cURL libraries for later use.)
				PAL_INET_HANDLE hFtp = PAL_absclass::PAL_internetConnect(hNet, url.c_str(), PAL_INTERNET_DEFAULT_FTP_PORT, NULL, NULL, PAL_INTERNET_SERVICE_FTP, 0, 0);
				if (hFtp)
				{
					bool success = false;

					// Make the url of the GCD directory look right. Form is like server/sdmemory/blahhh/
					std::string path(pUploadFileInfo->m_remotePath.getPath());
					trimFTPDirSeparators(path, '\\' ); // get rid of all trailing/leading dir separators.
					path = "/" + path + "/";
					fixTheSlashesForFtp( path );
	 
					// now looks like /dir/dir/...

					// Let PAL/cURL create the remote directory on the GCD
					success = PAL_absclass::PAL_ftpCreateDirectory( hFtp, path.c_str() );
					if (!success)
					{
						dwReturn = PAL_absclass::PAL_getLastError();

						updateError.isError = true;
						updateError.errorNumber = dwReturn;

						if (ERROR_INTERNET_EXTENDED_ERROR == dwReturn)
						{
							GetLastErrorExtendedResponse(&dwReturn, &error);
							DebugTrace("GCDSession::FinishUploadFtpFile: PAL_absclass::FtpCreateDirectoryA failed. Error (%d), %s", dwReturn, error.c_str());

							updateError.category = CURL;
							updateError.extendedError = PAL_absclass::getExtendedError();
						}
						else
						{
							DebugTrace("GCDSession::FinishUploadFile: FtpCreateDirectoryA failed. Error (%d)", dwReturn);
							error.resize(128);
							sprintf(&error[0], "System error %d occurred on FTP open.", dwReturn);

							updateError.category = OS;
							updateError.extendedError = 0;
						}
					}

					// Make the url of the GCD directory look right. Form is like sdmemory/blahhh/playlist.txt or whatever. 
					path = pUploadFileInfo->m_remotePath.getFullPath();
					trimFTPDirSeparators(path, '\\' ); // get rid of all trailing/leading dir separators.
					// Note: do not prepend a leading '/'. PAL does this when it attaches the server name kept in the handle
					fixTheSlashesForFtp( path );
					
					// upload the local file to the GCD file. Both should be fully named. Local path is a local name and remote
					// path is in the form of a URL
 					success = PAL_absclass::PAL_ftpPutFile(hFtp, pUploadFileInfo->m_localPath.getFullPath().c_str(), path.c_str(), 0, 0);
					if (!success)
					{
						dwReturn = PAL_absclass::PAL_getLastError();

						updateError.isError = true;
						updateError.errorNumber = dwReturn;

						if (ERROR_INTERNET_EXTENDED_ERROR == dwReturn)
						{
							GetLastErrorExtendedResponse(&dwReturn, &error);
							DebugTrace("GCDSession::FinishUploadFile: FtpPutFileA failed. Error (%d), %s", dwReturn, error.c_str());

							updateError.category = CURL;
							updateError.extendedError = PAL_absclass::getExtendedError();
						}
						else
						{
							DebugTrace("GCDSession::FinishUploadFile: FtpPutFileA failed. Error (%d)", dwReturn);
							error.resize(128);
							sprintf(&error[0], "System error %d occurred on FTP open.", dwReturn);

							updateError.category = OS;
							updateError.extendedError = 0;
						}
					}
					PAL_absclass::PAL_internetCloseHandle(hFtp);
				}
				else
				{
					dwReturn = PAL_absclass::PAL_getLastError();

					updateError.isError = true;
					updateError.errorNumber = dwReturn;

					if (ERROR_INTERNET_EXTENDED_ERROR == dwReturn)
					{
						GetLastErrorExtendedResponse(&dwReturn, &error);
						DebugTrace("GCDSession::FinishUploadFile: PAL_absclass::PAL_internetConnect failed. Error (%d), %s", dwReturn, error.c_str());

						updateError.category = CURL;
						updateError.extendedError = PAL_absclass::getExtendedError();
					}
					else
					{
						DebugTrace("GCDSession::FinishUploadFile: PAL_absclass::PAL_internetConnect failed. Error (%d)", dwReturn);
						error.resize(128);
						sprintf(&error[0], "System error %d occurred on FTP open.", dwReturn);

						updateError.category = OS;
						updateError.extendedError = 0;
					}
				}
				PAL_absclass::PAL_internetCloseHandle(hNet);
			}
			else
			{
				dwReturn = PAL_absclass::PAL_getLastError();

				updateError.isError = true;
				updateError.errorNumber = dwReturn;

				if (ERROR_INTERNET_EXTENDED_ERROR == dwReturn)
				{
					GetLastErrorExtendedResponse(&dwReturn, &error);
					DebugTrace("GCDSession::FinishUploadFile: InternetOpen failed. Error (%d)", dwReturn);

					updateError.category = CURL;
					updateError.extendedError = PAL_absclass::getExtendedError();
				}
				else
				{
					DebugTrace("GCDSession::FinishUploadFile: InternetOpen failed. Error (%d)", dwReturn);
					error.resize(128);
					sprintf(&error[0], "System error %d occurred on FTP open.", dwReturn);

					updateError.category = OS;
					updateError.extendedError = 0;
				}
			}
			PAL_absclass::PAL_incSemaphore(m_uploadFileThreadSemaphore, 1, NULL);
		}
	} // stat/file exists
	
	// note. If the file isn't there, we let the GCD raise the error when it goes to use it.
	showErrorStateToUser( &updateError, ShowOnlyIfError );

	// send file upload message whether or not there was success, to notify GCD upload was attempted
	GCDDevFileUploadMsg fileUploadMsg(pUploadFileInfo->m_remotePath.getFullPath().c_str(), pUploadFileInfo->m_dwLocalFileSize, pUploadFileInfo->m_dwLocalLastWriteTime);
	send(&fileUploadMsg, m_gcdSocketDestId);

	if( m_bCancelTransfer )
	{
		m_bCancelTransfer = FALSE;
		(this->*pUploadFileInfo->m_fpFileUploadFinished)(pUploadFileInfo, FileUploadFinishedState_Fail, error.c_str());
		updateError.isError = false;
	}
	else if (ERROR_SUCCESS == dwReturn)
	{
		if (pUploadFileInfo->m_fpFileUploadFinished)
		{
			(this->*pUploadFileInfo->m_fpFileUploadFinished)(pUploadFileInfo, FileUploadFinishedState_Success, NULL);
		}
	}
	else
	{
		if (pUploadFileInfo->m_fpFileUploadFinished)
		{
			error.append(" source path = ");
			error.append(pUploadFileInfo->m_sourcePath.getFullPath());
			error.append(" GCD path = ");
			error.append(pUploadFileInfo->m_remotePath.getFullPath());
			(this->*pUploadFileInfo->m_fpFileUploadFinished)(pUploadFileInfo, FileUploadFinishedState_Fail, error.c_str());
		}
	}

	delete pUploadFileInfo;

	return dwReturn;
}

void GCDSession::GetLastErrorExtendedResponse(DWORD* pdwError, std::string* pResponse)
{
	DWORD dwErrorBufLength = 64;
	pResponse->clear();
	pResponse->resize(dwErrorBufLength);
#if defined (PAL_WIN32_RUN)
	if (!PAL_absclass::PAL_InternetGetLastResponseInfo(pdwError, const_cast< char * >(pResponse->data()), &dwErrorBufLength))
	{
		if (PAL_absclass::PAL_getLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			//Debug::trace("GCDSession", HobString("GetLastErrorExtendedResponse():  ERROR_INSUFFICIENT_BUFFER"));
			pResponse->clear();
			pResponse->resize(dwErrorBufLength);
			PAL_absclass::PAL_InternetGetLastResponseInfo(pdwError, const_cast< char * >(pResponse->data()), &dwErrorBufLength);
		}
	}
#endif
}

int GCDSession::send( EvolMessage *message, int destination, bool shouldDelayAfterSending )
{
	int result = -1;

	if (( message ) && ( m_gcdSocketDestId == destination ))
	{
		Util::CLockCriticalSection lock( m_sendMessageCs );

		// only send if we have a connection
		if ( WAIT_OBJECT_0 == PAL_absclass::PAL_waitForSingleObj( m_gcdIsConnected, 0 ) )
		{
			Debug::trace( "GCD", "GCDSession::send() - msg:" + currentPlaylistFileName_);

			result = Session::send( message, destination, shouldDelayAfterSending );

			// bad result, did we lose our connection?
			if ( result <= 0 )
				++m_sendErrors;
			else
				m_sendErrors = 0;

			// reset connection upon reaching max errors
			if ( m_sendErrors >= MAX_SEND_ERRORS )
				CloseSocket();
		}
	}

	return result;
}

void GCDSession::CloseSocket()
{
	// no longer connected
	PAL_absclass::PAL_resetEvent( m_gcdIsConnected );

	// close the socket
	m_pGcdMsgSocket->cmClose();
}

void GCDSession::sendPlaylistFilenameToGCD()
{
	currentPlaylistFileName_ = Configuration::getInstance()->toStringCfg(fieldOffsetCfgGCDPlaylistFile);
	Debug::trace( "GCD", "GCDSession::sendPlaylistFilenameToGCD() - notifying GCD of playlist file name:" + currentPlaylistFileName_);

	GCDDevLoadPlaylistMsg loadPlaylistMsg(currentPlaylistFileName_);
	send(&loadPlaylistMsg, m_gcdSocketDestId);
}

void GCDSession::sendLoadDisplayObjectXMLToGCD()
{
	Configuration *config( Configuration::getInstance() );

	HobString displayObjectXmlPath = config->toStringCfgI( CfgInternal::GCDDISPLAYOBJECTXMLPATHFORMAT );

	int gcdLayoutNumber = config->toIntCfg( fieldOffsetCfgGCDLayoutNumber );
	int gcdColorScheme = config->toIntCfg( fieldOffsetCfgGCDColorSchemeNumber );

	// FT-1362
	if ( m_isTransactionalGCDLicensed )
		if ( config->toBoolCfg( fieldOffsetCfgGCDUseTransactionalLayout ) )
			++gcdColorScheme;

	char szFilename[MAX_PATH] = { 0 };
	sprintf( szFilename, displayObjectXmlPath.c_str(), gcdLayoutNumber, gcdColorScheme );

	int gcdIdleAdvertType = config->toIntCfg( fieldOffsetCfgGCDIdleAdvertType );
	HobString gcdIdleWebsiteAddress = config->toStringCfg( fieldOffsetCfgGCDIdleWebsiteAddress );

	if ( !m_isTransactionalGCDLicensed )
		gcdIdleAdvertType = 0;

	GCDDevLoadDisplayObjectXMLMsg loadDisplayObjectXMLMsg( szFilename, gcdIdleAdvertType, gcdIdleWebsiteAddress.c_str() );
	send(&loadDisplayObjectXMLMsg, m_gcdSocketDestId);
}

void GCDSession::sendProductRelatedPlaylistFilenameToGCD(HobString playlist)
{
	Debug::trace( "GCD", "GCDSession::sendProductRelatedPlaylistFilenameToGCD() - notifying GCD of playlist file name:" + playlist);

	GCDDevLoadPlaylistMsg loadPlaylistMsg(playlist);
	send(&loadPlaylistMsg, m_gcdSocketDestId);
}

void GCDSession::clearProductRelatedPlaylistFilenameOnGCD()
{
	Debug::trace( "GCD", "GCDSession::clearProductRelatedPlaylistFilenameOnGCD():  Clear the Product/Category related playlist.");

	//	We will send whatever is the General playlist defined in Configuration to get rid of the Product playlist.
	sendPlaylistFilenameToGCD();
}
