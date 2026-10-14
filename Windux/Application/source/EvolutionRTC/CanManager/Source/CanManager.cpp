/**
 * @file CanManager.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Manages the traffic to and from the CAN bus
 * Created on: Jun 24, 2015
 */

#include "CanManager.h"
#include "Integer.h"
#include "Debug.h"
#include "Hex.h"
#include "HobString.h"
#include "FpmManager.h"

util_thread::Mutex<> CanManager::instanceMutex_;
CanManager *CanManager::pInstancePrimary = (CanManager*) NULL;
CanManager *CanManager::pInstanceBoot = (CanManager*) NULL;

#if defined ( __linux__ )

const can_filter CanManager::aws3Filter[Ngw1FilterSize] =  { {0x54b, Mask8Range}, {0x596, Mask16Range}, {0x534, Mask4Range}, {0x510, MaskMatch},
															 {0x710, Mask16Range} };


const can_filter CanManager::ngw3Filter[NGW3FilterSize] = { {0x510, MaskMatch},   {0x530, Mask16Range}, {0x540, Mask16Range}, {0x579, MaskMatch},
															{0x580, Mask16Range}, {0x590, Mask16Range}, {0x5A1, Mask16Range}, {0x5B0, Mask16Range},
															{0x5C0, Mask16Range}, {0x5D0, Mask16Range}, {0x5E0, Mask16Range}, {0x5F0, Mask16Range},
															{0x710, Mask16Range}, {0x610, Mask16Range}, {0x620, Mask16Range}, 
															{0x070, Mask4Range}, {0x074, MaskMatch}, {0x071, MaskMatch}, {0x072, MaskMatch},
															{0x073, MaskMatch}, {0x0D0, MaskMatch}, {0x0D1, MaskMatch}, {0x0D2, MaskMatch}, {0x0D3, MaskMatch},
															{0x0D4, MaskMatch}
														};

const can_filter CanManager::htiFilter[Hti3FilterSize] = { {0x510, MaskMatch}, {0x530, Mask16Range}, {0x540, Mask16Range}, {0x579, MaskMatch},
															{0x580, Mask16Range}, {0x590, Mask16Range}, {0x5A1, Mask16Range}, {0x5B0, Mask16Range},
															{0x5C0, Mask16Range}, {0x5D0, Mask16Range}, {0x5E0, Mask16Range}, {0x5F0, Mask16Range},
															{0x710, Mask16Range},
															{0x400, Mask16Range}, {0x421, Mask16Range}, {0x430, Mask16Range}, {0x440, Mask16Range},
															{0x450, Mask16Range}, {0x4a0, Mask16Range}, {0x4b0, Mask16Range}, {0x40f, MaskMatch},
															{0x410, Mask16Range}, {0x490, Mask16Range}, {0x100, Mask16Range}, {0x110, Mask16Range},
															{0x175, Mask16Range}, {0x180, Mask16Range}, {0x190, Mask16Range}, {0x1A0, Mask16Range},
															{0x070, Mask4Range}, {0x074, MaskMatch}, 
															{0x0D0, Mask4Range}, {0x0D4, MaskMatch},
															{0x0F1, Mask16Range}};

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
CanManager *CanManager::getInstance( CANBUS bus )
{
	CanManager *instance( _PrimaryBus == bus ? pInstancePrimary : pInstanceBoot );

	if ( instance )
	    return instance;

	instanceMutex_.acquire();

	instance = ( _PrimaryBus == bus ? pInstancePrimary : pInstanceBoot );

	if ( !instance )
	{
		instance = new CanManager( bus );

#if defined ( __linux__ ) 
		if ( !instance->isOpen() )
		{
			delete instance;
			instance = NULL;
		}
#endif
		if ( _PrimaryBus == bus )
			pInstancePrimary = instance;
		else
			pInstanceBoot = instance;
	}

	instanceMutex_.release();
	return instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CanManager::CanManager( CANBUS bus )
{
    Debug::trace( "CanManager", "Entered constructor");
#if defined ( __linux__ )
    interfaceOpen = false;

    myRunThread = NULL;

    if ( bus == _PrimaryBus )
    	ifname = "can1";
    else
    	ifname = "can0";

    open();
#endif

    Debug::trace( "CanManager", "Exit constructor");
}


///////////////////////////////////////////////////////////////////////////////////////////////////
CanManager::~CanManager()
{
	Debug::trace("CanManager", "~CanManager(): --Entry");

#if defined ( __linux__ )
	//tell the thread to exit
	m_ExitEvent.EventSet();

	//wait for the thread to exit before deleting all the resources!
	if (myRunThread) {
		while (myRunThread->IsRunning()) {
			PAL_absclass::PAL_sleep(1);
		}
		delete myRunThread;
		myRunThread = NULL;
	}

	if (pTranslator)
	{
		delete pTranslator;
		pTranslator = NULL;
	}

	//close the CAN interface
	if (sockFd > 0)
	{
		struct ifreq ifr;
		strcpy(ifr.ifr_name, ifname.c_str());

	    if ( ioctl(sockFd, SIOCGIFINDEX, &ifr) < 0 )
	        Debug::trace("CanManager.Error", " CanManager(): Failed to index: " + ifname + " " + Integer::toString(ifr.ifr_ifindex));
	    else
	    {
			ifr.ifr_flags &= ~(IFF_UP);
			if (ioctl(sockFd, SIOCSIFFLAGS, &ifr) < 0)
				Debug::trace( "CanManager.Error", "shutdown(): Error closing " + ifname + " interface" );
			else
				Debug::trace( "CanManager",  "shutdown(): " + ifname + " interface closed" );
	    }

		close(sockFd);
		sockFd = 0;
	}

    interfaceOpen = false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CanManager::Run()
{
	int errors( 0 );

	Debug::trace( "CanManager", "Run(): Entering CAN receiver Run Thread" );

#if defined ( __linux__ )

	do
	{
		// try 1 second timeout for now...
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		fd_set rdSet;
		fd_set errSet;

		FD_ZERO( &rdSet );
		FD_SET( sockFd, &rdSet );

		FD_ZERO( &errSet );
		FD_SET( sockFd, &errSet );

		const int result( select( sockFd+1, &rdSet, NULL, &errSet, &timeout ) );

		if ( result < 0 )
			Debug::trace( "CanManager.Error", "Run(): Select failed " + Integer::toString(result) );

		else if (( 0 < result ) && ( FD_ISSET( sockFd, &rdSet ) ))
		{
			bool done( false );

			errors = 0;

			// read all pending data
			do
			{
				struct can_frame rxFrame;
				rxFrame.can_id = 0;

				const int recvbytes( read( sockFd, &rxFrame, sizeof(struct can_frame) ) );

				// validate
				if ( recvbytes != sizeof(struct can_frame) )
				{
					if (( EAGAIN != errno ) && ( EWOULDBLOCK != errno ))
						Debug::trace( "CanManager.Error", "Run(): Recv bytes error: " + Integer::toString(recvbytes) );

					done = true;
				}

				// process message
				else if ( 0 == rxFrame.can_id )
					Debug::trace( "CanManager.Error", "Run(): Bad Frame detected" );
				else if ( rxFrame.can_id & CAN_EFF_FLAG )
					Debug::trace( "CanManager.Error", "Run(): EFF/SFF is set in the MSB: " + Hex::toString(rxFrame.can_id) );
				else if ( rxFrame.can_id & CAN_RTR_FLAG )
					Debug::trace( "CanManager.Error", "Run(): remote transmission request: " + Hex::toString(rxFrame.can_id) );
				else if ( rxFrame.can_id & CAN_ERR_FLAG )
					Debug::trace( "CanManager.Error", "Run(): error message frame: " + Hex::toString(rxFrame.can_id) );
				else
				{
					rxFrame.can_id &= CAN_SFF_MASK;

					if (( pTranslator ) && ( pTranslator->isTranslatable( rxFrame.can_id ) )){
						Debug::trace( "CanManager.RX", "Run(): Translate Message: 0x" + Hex::toString(rxFrame.can_id) );
						pTranslator->translateMessage( &rxFrame );
					}
					else
					{
						Debug::trace( "CanManager.RX", "Run(): Failed To Translate Message: 0x" + Hex::toString(rxFrame.can_id) );
					}

				}

				// rate limit
				//PAL_absclass::PAL_sleep( 1 );

			} while ( !done );

			FD_CLR( sockFd, &rdSet );
		}
		else if (( 0 < result ) && ( FD_ISSET( sockFd, &errSet ) ))
		{
			++errors;

			Debug::trace( "CanManager.Error", "Run(): CAN Error detected: " + Integer::toString(errors) );

			FD_CLR( sockFd, &errSet );
		}

	} while ( !m_ExitEvent.IsEventSet() );

#endif

	Debug::trace( "CanManager", "Run(): Exiting Run Thread" );
	return 0;
}


#if defined ( __linux__ ) 

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanManager::open()
{
	// nothing to do
	if ( interfaceOpen )
		return;

    //get our scale style
    FpmManager *pSerialFlash = FpmManager::getInstance();
    std::string systemType = pSerialFlash->getMFGSystemStyle();

	Debug::trace( "CanManager", "systemType = : " + systemType + " ifname = " + ifname);

    if( ( systemType == "NGW3" ) || ( systemType == "NGW2" ) ) {
		wrapperCntl.setDeviceTypeWrapper();
		wrapperCntl.releaseResetDevice();
    }
    if( ( systemType == "AWS3" ) || ( systemType == "NGW1" ) ) {
    	aws3WrapperCntl.setDeviceTypeAWS3Wrapper();
    	aws3WrapperCntl.releaseResetDevice();

    }

    pTranslator = NULL;

    // setup CAN socket
    if((sockFd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
        Debug::trace( "CanManager.Error", "CanManager(): Failed to open socket. Error: " + Integer::toString(sockFd));
    else
    {
        //setup can message filtering for AWS3/NGW1 wrapper
        if( ( systemType == "AWS3" ) || ( systemType == "NGW1" ) ) {
    		if( setsockopt( sockFd, SOL_CAN_RAW, CAN_RAW_FILTER, &aws3Filter, sizeof(aws3Filter) ) < 0 ) {
    			Debug::trace( "CanManager.Error", "CanManager(): Failed to setup filtering: " + Integer::toString(sockFd));
    		}
        }
        //setup can message filtering for NGW3/NGW2 wrapper
        if( ( systemType == "NGW3" ) || ( systemType == "NGW2" ) ) {
    		if( setsockopt( sockFd, SOL_CAN_RAW, CAN_RAW_FILTER, &ngw3Filter, sizeof(ngw3Filter) ) < 0 ) {
    			Debug::trace( "CanManager.Error", "CanManager(): Failed to setup filtering: " + Integer::toString(sockFd));
    		}
        }

	    //setup can message filtering for value max service scale
	    if( ( systemType == "HTK" )  || ( systemType == "FSS" ) ) {
			if( setsockopt( sockFd, SOL_CAN_RAW, CAN_RAW_FILTER, &htiFilter, sizeof(htiFilter) ) < 0 ) {
				Debug::trace( "CanManager.Error", "CanManager(): Failed to setup filtering: " + Integer::toString(sockFd));
			}
	    }

        // setup error reporting
		can_err_mask_t err_mask( CAN_ERR_MASK ); //( CAN_ERR_TX_TIMEOUT | CAN_ERR_BUSOFF );

		if ( setsockopt( sockFd, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask, sizeof(err_mask) )  < 0 )
			Debug::trace( "CanManager.Error", "CanManager(): Failed to setup error masking: " + Integer::toString(sockFd));

		fcntl(sockFd, F_SETFL, O_NONBLOCK);

		struct ifreq ifr;
		strcpy(ifr.ifr_name, ifname.c_str());

		if( ioctl(sockFd, SIOCGIFINDEX, &ifr) < 0 ) {
			std::string str(ifr.ifr_name);
			Debug::trace("CanManager.Error", " CanManager(): Failed to index: " + str + " " + Integer::toString(ifr.ifr_ifindex));
		}

		printf("%s at index %d\n", ifname.data(), ifr.ifr_ifindex);

		// bind
		memset( &socAddr, 0, sizeof(struct sockaddr_can) );
		socAddr.can_family  = AF_CAN;
		socAddr.can_ifindex = ifr.ifr_ifindex;
		int result = bind(sockFd, (struct sockaddr *)&socAddr, sizeof(socAddr));

		if( result < 0 )
			Debug::trace( "CanManager.Error", "CanManager(): Failed to bind socket: " + Integer::toString(result) );
		else
		{
			/* bring up the interface */
			ifr.ifr_flags |= (IFF_UP);
			if (ioctl(sockFd, SIOCSIFFLAGS, &ifr) < 0)
				Debug::trace( "CanManager.Error", "Failed to bring interface up." );
			else
			{
				Debug::trace( "CanManager", "open():" + ifname + " interface ready.");
				interfaceOpen = true;

				result = bind(sockFd, (struct sockaddr *)&socAddr, sizeof(socAddr));
				if( result < 0 )
					Debug::trace( "CanManager.Error", "CanManager(): Failed to bind socket (again!): " + Integer::toString(result) );

				/* DO NOT start thread until our translator has been initialized!
				myRunThread = new Util::CTLSThread( *this, 1, 0, "CanManager Thread" ); */
			}
		}
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void CanManager::shutdown()
{
	Debug::trace( "CanManager", "shutdown()" );

	instanceMutex_.acquire();

	// zero out instance
    if ( pInstancePrimary == this )
    {
    	delete pInstancePrimary;
    	pInstancePrimary = NULL;
    }
    else if ( pInstanceBoot == this )
    {
    	delete pInstanceBoot;
    	pInstanceBoot = NULL;
    }

    instanceMutex_.release();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanManager::initializeTranslator()
{
	if ( !pTranslator )
	{
		if ( (pTranslator = new CanTranslator()) )
		{
			pTranslator->assignCanManager( this );

			Debug::trace( "CanManager", "initializeTranslator(): initialized start thread!" );
			myRunThread = new Util::CTLSThread( *this, 1, 0, "CanManager Thread" );
		}
		else
		{
			Debug::trace( "CanManager.Error", "initializeTranslator(): Failed to allocate CanTranslator!" );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CanManager::probeBus()
{
    //get our scale style
    FpmManager *pSerialFlash = FpmManager::getInstance();
    std::string systemType = pSerialFlash->getMFGSystemStyle();

    //we handle probing of the bus differently for NGW2 and AWS3
    if( systemType == "NGW2" )
    {
    	if ( !pTranslator )
    		Debug::trace( "CanManager", "probeBus(): CanTranslator not initialized!" + Hex::toString((const char*)pTranslator) );
    	else
    	{
    		pTranslator->sendRequestWeigherWakeup();
    		pTranslator->sendRequestAllPrinterWakeup();
    		pTranslator->sendRequestWrapperWakeup();
    	}
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int CanManager::send(can_frame *pCanFrame)
{
	Util::CLockCriticalSection lock( sendLock );

	int totalbytes( 0 );
	int retry( 0 );

	do
	{
		const int nbytes( write( sockFd, pCanFrame, sizeof(struct can_frame) ) );

        if ( 0 < nbytes )
        	totalbytes += nbytes;
        else
        {
        	Debug::trace( "CanManager.Error", "Send Error: " + Integer::toString(nbytes));
			Debug::trace( "CanManager.Error", "Frame ID: 0x" + Hex::toString(pCanFrame->can_id));
        	PAL_absclass::PAL_sleep( 10 );

        	if ( 3 == ++retry )
        		break;
        }

    } while ( totalbytes <= 0 );

	// rate limit
	//PAL_absclass::PAL_sleep( 2 );

	return totalbytes;
}

#endif // __linux__ 
