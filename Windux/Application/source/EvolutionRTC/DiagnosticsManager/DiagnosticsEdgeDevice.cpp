/**
 * @file DiagnosticsEdgeDevice.cpp
 * @brief Interface to gateway edge device.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "DiagnosticsEdgeDevice.h"

#include "EdgeComm/DiagnosticsMsg.h"
#include "EdgeComm/EdgePacket.h"
#include "EdgeComm/EventMsg.h"
#include "EdgeComm/HealthInfoMsg.h"
#include "EdgeComm/PowerUpMsg.h"
#include "EdgeComm/StatusCheckMsg.h"

#include "Util/NetworkVal.h"

#include <Debug.h>
#include <Hex.h>

#include <e_string/utilstring.h>

#include <bootParameters.h>

#include <CfgInternal.h>
#include <Configuration.h>

#include <FpmManager.h>

#include <TimerManager.h>

#include <VersionInfo.h>
#include <VersionManager.h>

#if defined( _MSC_VER )
#define __func__ __FUNCTION__

#elif defined( __GNUC__ )
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <termios.h>
#include <unistd.h>

#endif

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DiagnosticsEdgeDevice::dataType_ = 0;
std::string DiagnosticsEdgeDevice::dataTypeName_( "rtc.diagnosticsmanager.diagnosticsedgedevice" );
#endif

std::string DiagnosticsEdgeDevice::category_( "DiagnosticsManager.edgedevice" );

#define TRACE( logline ) Debug::trace( category_, (logline) )
#define TRACEFUNC( logline ) if (Debug::isTracing( category_ + "." + __func__ )) TRACE( (logline) )

#define EDGE_SERIAL_DEVICE "/dev/ttyHobartDiag"

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsEdgeDevice::DiagnosticsEdgeDevice() :
	isScale_( false ),
	isWrapper_( false ),
	enabled_( false )
{
#ifdef USE_ACCOUNTANT
	if ( !dataType )
		dataType_ = util_acc::registerDataType( dataTypeName_ );

	util_acc::addInstance( dataType_ );
#endif

	TRACE( "CTOR" );
	initialize();

	TRACE( "CTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsEdgeDevice::~DiagnosticsEdgeDevice()
{
	TRACE( "DTOR" );
	finalize();

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsEdgeDevice::enabled() const
{
	if ( !enabled_ )
		return false;

	if ( serial_.empty() )
		return false;

	if ( !machineCycleCount() )
		return false;

	// check for server to connect to
	address_ = Configuration::getInstance()->toStringCfg( fieldOffsetCfgDiagGatewayDeviceAddress );
	port_ = EDGE_CLIENT_PORT;

	if (( address_.empty() ) && ( discoveredAddress_.size() ))
	{
		address_ = discoveredAddress_;

		if ( discoveredPort_ )
			port_ = discoveredPort_;
	}

	// no server to connect to
	//  -AND-
	// no serial device to use
	if (( address_.empty() ) && ( !serialDeviceExists() ))
		return false;

	logLevel_ = Configuration::getInstance()->toIntCfg( fieldOffsetCfgDiagGatewayDeviceStatusMsgLogLevel );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int DiagnosticsEdgeDevice::offlineMessages() const
{
	unsigned int result( 0 );

	m_.acquire();
	result = (unsigned int) messages_.size();
	m_.release();

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsEdgeDevice::online( std::string &gateway, bool &discovered ) const
{
	if ( enabled_ )
	{
		if ( 0 <= clientSerialPort_ )
		{
			gateway = "UART";
			discovered = false;
			return true;
		}
		else if (( clientSocket_ ) && ( clientSocket_->isValid() ))
		{
			gateway = address_;
			discovered = (address_ == discoveredAddress_);
			return true;
		}
	}

	gateway.clear();
	discovered = false;
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsEdgeDevice::setHealthInfo( const dict_type &value )
{
    bool sentPowerUpMessage( false );

	if (( enabled() ) && ( powerUpComplete_ ))
	{
        const dict_type healthInfoCopy( healthInfo_ );

		// send new health info
		for ( dict_type::const_iterator i( value.begin() ); i != value.end(); ++i )
		{
			bool found( false );

            for ( dict_type::const_iterator j( healthInfoCopy.begin() ); j != healthInfoCopy.end(); ++j )
				if ( (found = (i->at( "errorCode" ) == j->at( "errorCode" ))) )
					break;

			if ( !found )
			{
				unsigned int flags( 0 );
				flags |= edgecomm::healthInfoMsg::addToLog;

				if ( i->at( "timestamp" ) == i->at( "latestDiag" ) )
					flags |= edgecomm::healthInfoMsg::generatedByDiagnostics;

				edgecomm::healthInfoMsg_v1 msg( serial_, model_ );
				util::NetworkVal::setValueInt( msg.errorCode_, util_string::stringtolong( i->at( "errorCode" ) ) );
				util::NetworkVal::setValueInt( msg.severity_, util_string::stringtolong( i->at( "severity" ) ) );
				util::NetworkVal::setValueInt( msg.flags_, flags );

				edgecomm::EdgePacket packet;
				packet.append( msg );

				// send!
                if ( !sentPowerUpMessage )
                {
                    const util::DictionaryList temp( healthInfo_ );

                    healthInfo_ = value;
                    sendPowerUp();
                    healthInfo_ = temp;

                    sentPowerUpMessage = true;
                }

				TRACE( "sending health info packet..." );
				sendMsg( packet.transmitBuffer(), packet.transmitSize() );
			}
		}

		// make list of removed health info
        for ( dict_type::const_iterator i( healthInfoCopy.begin() ); i != healthInfoCopy.end(); ++i )
		{
			bool found( false );

			for ( dict_type::const_iterator j( value.begin() ); j != value.end(); ++j )
				if ( (found = (i->at( "errorCode" ) == j->at( "errorCode" ))) )
					break;

			if ( !found )
			{
				unsigned int flags( 0 );
				flags |= edgecomm::healthInfoMsg::removeFromLog;

				edgecomm::healthInfoMsg_v1 msg( serial_, model_ );
				util::NetworkVal::setValueInt( msg.errorCode_, util_string::stringtolong( i->at( "errorCode" ) ) );
				util::NetworkVal::setValueInt( msg.severity_, util_string::stringtolong( i->at( "severity" ) ) );
				util::NetworkVal::setValueInt( msg.flags_, flags );

				util::NetworkVal::setValueInt( msg.serviceActions_[0][0], util_string::stringtolong( i->at( "serviceMethod1" ) ) );
				util::NetworkVal::setValueInt( msg.serviceActions_[0][1], util_string::stringtolong( i->at( "servicePart1" ) ) );

				util::NetworkVal::setValueInt( msg.serviceActions_[1][0], util_string::stringtolong( i->at( "serviceMethod2" ) ) );
				util::NetworkVal::setValueInt( msg.serviceActions_[1][1], util_string::stringtolong( i->at( "servicePart2" ) ) );

				util::NetworkVal::setValueInt( msg.serviceActions_[2][0], util_string::stringtolong( i->at( "serviceMethod3" ) ) );
				util::NetworkVal::setValueInt( msg.serviceActions_[2][1], util_string::stringtolong( i->at( "servicePart3" ) ) );

				util::NetworkVal::setValueInt( msg.serviceActions_[3][0], util_string::stringtolong( i->at( "serviceMethod4" ) ) );
				util::NetworkVal::setValueInt( msg.serviceActions_[3][1], util_string::stringtolong( i->at( "servicePart4" ) ) );

				edgecomm::EdgePacket packet;
				packet.append( msg );

				// append service notes
				const std::string serviceNotes( i->at( "serviceNotes" ) );

				if ( serviceNotes.length() )
				{
					util::NetworkVal::setValueInt( msg.serviceNotesOffset_, packet.size() );
					util::NetworkVal::setValueInt( msg.serviceNotesLength_, serviceNotes.length() );

					packet.update( 0, msg );
					packet.append( serviceNotes.c_str(), serviceNotes.length() );
				}

				// send!
                if ( !sentPowerUpMessage )
                {
                    const util::DictionaryList temp( healthInfo_ );

                    healthInfo_ = value;
                    sendPowerUp();
                    healthInfo_ = temp;

                    sentPowerUpMessage = true;
                }

                TRACE( "sending resolved health info..." );
				sendMsg( packet.transmitBuffer(), packet.transmitSize() );
			}
		}
	}

	// set health info
	healthInfo_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsEdgeDevice::checkComms()
{
#if defined ( __linux__ )
	int nfds( 0 );

    char buffer[1024];
    int recvlen;

    // setup socket read
    fd_set sockRead;
    FD_ZERO( &sockRead );

    // setup socket except
    fd_set sockExcept;
    FD_ZERO( &sockExcept );

    if (( discoverySocket_ ) && ( discoverySocket_->isValid() ))
	{
		FD_SET( discoverySocket_->fd(), &sockRead );

		// dont look for exceptions on discovery socket
		//FD_SET( discoverySocket_->fd(), &sockExcept );

#if defined( _MSC_VER )
        nfds = max( nfds, discoverySocket_->fd() );
#elif defined( __GNUC__ )
        nfds = std::max( nfds, discoverySocket_->fd() );
#endif
	}

	if ( 0 <= clientSerialPort_ )
	{
		FD_SET( clientSerialPort_, &sockRead );

		// dont look for exceptions on client socket
		//FD_SET( clientSerialPort_, &sockExcept );

#if defined( _MSC_VER )
        nfds = max( nfds, clientSerialPort_ );
#elif defined( __GNUC__ )
        nfds = std::max( nfds, clientSerialPort_ );
#endif
	}

    if (( clientSocket_ ) && ( clientSocket_->isValid() ))
	{
		FD_SET( clientSocket_->fd(), &sockRead );

		// dont look for exceptions on client socket
		//FD_SET( clientSocket_->fd(), &sockExcept );

#if defined( _MSC_VER )
        nfds = max( nfds, clientSocket_->fd() );
#elif defined( __GNUC__ )
        nfds = std::max( nfds, clientSocket_->fd() );
#endif
	}

    // nothing to do
    if ( !nfds )
    	return;

    // setup timeout
    struct timeval timeout;
    std::memset( &timeout, 0, sizeof(timeout) );
    timeout.tv_usec = 250000;

    // wait
    if ( SOCKET_ERROR == select( nfds+1, &sockRead, NULL, &sockExcept, &timeout ) )
    {
		TRACE( "error waiting for edge comms: " + errno );
        return;
    }

    // process discovery socket
    if (( discoverySocket_ ) && ( discoverySocket_->isValid() ))
    {
	    if ( FD_ISSET( discoverySocket_->fd(), &sockExcept ) )
	    {
			TRACE( "discovery socket in exception state!" );

	    	delete discoverySocket_;
	    	discoverySocket_ = NULL;

	    	discoveryBuffer_.clear();
	    }
	    else if ( FD_ISSET( discoverySocket_->fd(), &sockRead ) )
	    {
			TRACEFUNC( "reading discovery socket..." );

			if ( 0 < (recvlen = discoverySocket_->recv( buffer, sizeof(buffer), 0 )) )
			{
				discoveryBuffer_.append( std::string( buffer, recvlen ) );

				TRACEFUNC( "discovery buffer:\n" + discoveryBuffer_ );
			}
			else
			{
				TRACE( "discovery socket recv error (" + util_string::inttostring( recvlen ) + "): " + util_string::inttostring( errno ) );
			}
	    }

		// process discovery messages
		if ( discoveryBuffer_.length() )
		{
			std::string::size_type pos;

			while ( std::string::npos != (pos = discoveryBuffer_.find( '\n' )) )
			{
				std::stringstream ss( discoveryBuffer_.substr( 0, pos ) );
				discoveryBuffer_.erase( 0, pos+1 );

				std::vector<std::string> fields;
				std::string field;

				while ( std::getline( ss, field, '|' ) )
					fields.push_back( field );

				if (( 3 <= fields.size() ) && ( "hobartGateway" == fields[0] ))
				{
					discoveredAddress_ = fields[1];

					TRACE( "FOUND GATEWAY: " + discoveredAddress_ );
				}
			}

			// attempt to correct bad buffer...
			if ( 1024 <= discoveryBuffer_.length() )
				discoveryBuffer_.clear();
		}
    }

	// process serial device
	if ( 0 <= clientSerialPort_ )
	{
        if ( FD_ISSET( clientSerialPort_, &sockExcept ) )
        {
			TRACE( "serial port in exception state!" );

        	::close( clientSerialPort_ );
        	clientSerialPort_ = -1;

    		clientBuffer_.clear();
        }
        else if ( FD_ISSET( clientSerialPort_, &sockRead ) )
		{
			TRACEFUNC( "reading serial port..." );

			if ( 0 < (recvlen = ::read( clientSerialPort_, buffer, sizeof(buffer) )) )
				clientBuffer_.append( std::string( buffer, recvlen ) );
			else
			{
				TRACE( "serial port read error (" + util_string::inttostring( recvlen ) + "): " + util_string::inttostring( errno ) );

				::close( clientSerialPort_ );
				clientSerialPort_ = -1;

	    		clientBuffer_.clear();
			}
		}

    	// drop any client socket data (at least in this phase 1)
    	if ( clientBuffer_.length() )
    		clientBuffer_.clear();

    	// send messages
        if ( 0 <= clientSerialPort_ )
        {
			m_.acquire();

			if ( messages_.size() )
			{
				TRACE( "sending client data SERIAL... [" + util_string::inttostring( messages_.size() ) + "]" );
				const std::string message( messages_.front() );

				TRACEFUNC( Hex::toString( message.c_str(), message.length() ) );

				if ( message.empty() )
				{
					TRACE( "removing zero length message" );
					messages_.pop_front();
				}
				else if ( ::write( clientSerialPort_, message.c_str(), message.length() ) )
				{
					messages_.pop_front();
				}
				else
				{
					TRACE( "serial port send error!" );

					::close( clientSerialPort_ );
					clientSerialPort_ = -1;

					clientBuffer_.clear();
				}
			}

			m_.release();
        }
	}

    // process client socket
    if (( clientSocket_ ) && ( clientSocket_->isValid() ))
    {
        if ( FD_ISSET( clientSocket_->fd(), &sockExcept ) )
        {
			TRACE( "client socket in exception state!" );

        	delete clientSocket_;
        	clientSocket_ = NULL;

    		clientBuffer_.clear();
        }
        else if ( FD_ISSET( clientSocket_->fd(), &sockRead ) )
		{
			TRACEFUNC( "reading client socket..." );

			if ( 0 < (recvlen = clientSocket_->recv( buffer, sizeof(buffer), 0 )) )
				clientBuffer_.append( std::string( buffer, recvlen ) );
			else
			{
				TRACE( "client socket recv error (" + util_string::inttostring( recvlen ) + "): " + util_string::inttostring( errno ) );

	        	delete clientSocket_;
	        	clientSocket_ = NULL;

	    		clientBuffer_.clear();
			}
		}

    	// drop any client socket data (at least in this phase 1)
    	if ( clientBuffer_.length() )
    		clientBuffer_.clear();

    	// send messages
        if (( clientSocket_ ) && ( clientSocket_->isValid() ))
        {
			m_.acquire();

			if ( messages_.size() )
			{
				TRACE( "sending client data SOCKET... [" + util_string::inttostring( messages_.size() ) + "]" );
				const std::string message( messages_.front() );

				TRACEFUNC( Hex::toString( message.c_str(), message.length() ) );

				if ( message.empty() )
				{
					TRACE( "removing zero length message" );
					messages_.pop_front();
				}
				else if ( clientSocket_->send( message ) )
				{
					messages_.pop_front();
				}
				else
				{
					TRACE( "client socket send error!" );

					delete clientSocket_;
					clientSocket_ = NULL;

					clientBuffer_.clear();
				}
			}

			m_.release();
        }
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsEdgeDevice::checkOnline()
{
#if defined ( __linux__ )
	// nothing to do
	if ( !enabled() )
	{
		TRACE( "gateway not enabled" );
	}

	// connect to diagnostics port
	else if (( clientSerialPort_ < 0 ) && ( serialDeviceExists() ))
	{
		// open serial port
		if ( (clientSerialPort_ = ::open( EDGE_SERIAL_DEVICE, O_RDWR | O_NOCTTY | O_SYNC )) < 0 )
			TRACE( "failed to create serial port!" );

		// set interface attributes
		else if ( !setInterfaceAttrib( clientSerialPort_, EDGE_SERIAL_DEVICE_BAUD_RATE, EDGE_SERIAL_DEVICE_PARITY ) )
			TRACE( "failed to set interface attributes!" );

		// set blocking
		else if ( !setBlocking( clientSerialPort_, false ) )
			TRACE( "failed to set blocking!" );

		else
		{
			TRACE( "CONNECTED SERIAL!" );
		}
	}

	// connect to gateway server
	else if (( !clientSocket_ ) && ( address_.length() ) && ( 0 < port_ ))
	{
		clientSocket_ = new util::BasicSocket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

		TRACE( "connecting to gateway device " + address_ + ":" + util_string::inttostring( port_ ) + "..." );

		if ( !clientSocket_->isValid() )
		{
			TRACE( "bad socket!" );

			delete clientSocket_;
			clientSocket_ = NULL;
		}
		else if ( !clientSocket_->connect( AF_INET, address_, port_ ) )
		{
			TRACE( "error connecting to gateway!" );

			delete clientSocket_;
			clientSocket_ = NULL;
		}
		else
		{
			TRACE( "CONNECTED SOCKET!" );
		}
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsEdgeDevice::sendDiagnostics( unsigned int code, diagResult result, int errorCode, int routine, int routineExpectedResult, int routineActualResult, bool execOper, bool execService )
{
	if ( !enabled() )
		return false;

	unsigned int flags( 0 );

	if ( Pass == result )
		flags |= edgecomm::diagnosticsMsg::passed;
	else if ( Fail == result )
		flags |= edgecomm::diagnosticsMsg::failed;
	else if ( FailTimeout == result )
		flags |= (edgecomm::diagnosticsMsg::failed | edgecomm::diagnosticsMsg::timeout);

	if ( execOper )
		flags |= edgecomm::diagnosticsMsg::runByOperator;

	if ( execService )
		flags |= edgecomm::diagnosticsMsg::runByService;

	edgecomm::diagnosticsMsg_v1 msg( serial_, model_ );
	util::NetworkVal::setValueInt( msg.errorCode_, code );
	util::NetworkVal::setValueInt( msg.resultCode, errorCode );
	util::NetworkVal::setValueInt( msg.diagRoutine_, routine );
	util::NetworkVal::setValueInt( msg.diagRoutineExpected, routineExpectedResult );
	util::NetworkVal::setValueInt( msg.diagRoutineActual, routineActualResult );
	util::NetworkVal::setValueInt( msg.flags_, flags );

	edgecomm::EdgePacket packet;
	packet.append( msg );

	// send!
	TRACE( "sending diagnostics packet..." );
	return sendMsg( packet.transmitBuffer(), packet.transmitSize() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsEdgeDevice::sendEvent( unsigned int code, unsigned int severity, bool diag, bool diagForced )
{
	if ( !enabled() )
		return false;
	else if ( !logLevel_ )
		return false;
	else if ( severity < logLevel_ )
		return false;

	unsigned int flags( 0 );

	if ( diag )
		flags |= edgecomm::eventMsg::diagnosticsOffered;

	if ( diagForced )
		flags |= edgecomm::eventMsg::diagnosticsForced;

	edgecomm::eventMsg_v1 msg( serial_, model_ );
	util::NetworkVal::setValueInt( msg.cycleCount_, machineCycleCount() );
	util::NetworkVal::setValueInt( msg.errorCode_, code );
	util::NetworkVal::setValueInt( msg.severity_, severity );
	util::NetworkVal::setValueInt( msg.flags_, flags );

	edgecomm::EdgePacket packet;
	packet.append( msg );

	// send!
	TRACE( "sending event packet..." );
	return sendMsg( packet.transmitBuffer(), packet.transmitSize() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsEdgeDevice::sendPowerUp()
{
	if ( !enabled() )
		return false;

    MtdManufacturingData *manfData( FpmManager::getInstance()->readMfgScaleData() );

	// ========
	//  HEADER
	// ========

	edgecomm::powerUpMsg_v1 msg( serial_, model_ );

	uint16_t peripheralStatus( 0 );

    // manufacture date
    if ( manfData )
    {
    	std::string dof( manfData->getDateOfManufacturing() );
    	replaceAll( dof, "\"", "" );

    	util::Timestamp manufactureDate;
    	manufactureDate.fromDateTime( dof + " 08:00" );

    	util::NetworkVal::setValueInt( msg.manufactureDate_, manufactureDate.toUnixEpochTime() );
    }

	// ip address
	std::string ip;
	std::string gateway;

	VersionInfo::getIpAndGatewayAddress( ip, gateway );

	util::NetworkVal::setValueString( msg.machineAddress_, sizeof(msg.machineAddress_), ip );

	// hostname
	util::NetworkVal::setValueString( msg.machineHostname_, sizeof(msg.machineHostname_), bootParameters::getDNSHostName() );

	// cycle count
	util::NetworkVal::setValueInt( msg.cycleCount_, machineCycleCount() );

	// ==============
	//  VERSION INFO
	// ==============

	edgecomm::powerUpMsg_v1_firmware firmware[64];
	size_t firmwareCount( 0 );

    // system controller version
    if ( manfData )
    {
    	std::string rev( manfData->getBoardRevision() );
    	replaceAll( rev, "\"", "" );

		appendFirmware_v1( firmware, firmwareCount,
			edgecomm::powerUpMsg::SYSTEM_CONTROLLER_VERSION,
			rev );
    }

    // application version
    appendFirmware_v1( firmware, firmwareCount,
    	edgecomm::powerUpMsg::APPLICATION_VERSION,
		VersionInfo::getApplicationVersion() );

    // os version
    appendFirmware_v1( firmware, firmwareCount,
    	edgecomm::powerUpMsg::OS_VERSION,
		VersionManager::getInstance()->getOSVersionAsString() );

    // loader version
    appendFirmware_v1( firmware, firmwareCount,
    	edgecomm::powerUpMsg::LOADER_VERSION,
		FpmManager::getInstance()->getLoaderVersion() );

    // backend version
    appendFirmware_v1( firmware, firmwareCount,
    	edgecomm::powerUpMsg::SM_BACKEND_VERSION,
		VersionInfo::getSMBackendVersion() );

    // wrapper node app versions
	// ignore node zero (0), use 1-N
	for ( size_t i( 1 ); i < wrapperNodeAppVersions_.size(); ++i )
		if ( wrapperNodeAppVersions_[i].length() )
		{
			appendFirmware_v1( firmware, firmwareCount,
				(int) (edgecomm::powerUpMsg::WRAPPER_NODE1_APP_VERSION + i - 1),
				wrapperNodeAppVersions_[i] );
		}

    // wrapper node data versions
	// ignore node zero (0), use 1-N
	for ( size_t i( 1 ); i < wrapperNodeDataVersions_.size(); ++i )
		if ( wrapperNodeDataVersions_[i].length() )
		{
			appendFirmware_v1( firmware, firmwareCount,
				(int) (edgecomm::powerUpMsg::WRAPPER_NODE1_DATA_VERSION + i - 1),
				wrapperNodeDataVersions_[i] );
		}

    // wrapper node boot versions
	// ignore node zero (0), use 1-N
	for ( size_t i( 1 ); i < wrapperNodeBootVersions_.size(); ++i )
		if ( wrapperNodeBootVersions_[i].length() )
		{
			appendFirmware_v1( firmware, firmwareCount,
				(int) (edgecomm::powerUpMsg::WRAPPER_NODE1_BOOT_VERSION + i - 1),
				wrapperNodeBootVersions_[i] );
		}

    // wrapper node checksums
	// ignore node zero (0), use 1-N
	for ( size_t i( 1 ); i < wrapperNodeChecksums_.size(); ++i )
		if ( wrapperNodeChecksums_[i].length() )
		{
			appendFirmware_v1( firmware, firmwareCount,
				(int) (edgecomm::powerUpMsg::WRAPPER_NODE1_CHECKSUM + i - 1),
				wrapperNodeChecksums_[i] );
		}

	// weigher software
	if ( weigherSoftwareVersion_.length() )
		appendFirmware_v1( firmware, firmwareCount,
			edgecomm::powerUpMsg::WEIGHER_SOFTWARE_VERSION,
			weigherSoftwareVersion_ );

	// weigher hardware
	if ( weigherHardwareVersion_.length() )
		appendFirmware_v1( firmware, firmwareCount,
			edgecomm::powerUpMsg::WEIGHER_HARDWARE_VERSION,
			weigherHardwareVersion_ );

	// weigher checksum
	if ( weigherChecksum_.length() )
		appendFirmware_v1( firmware, firmwareCount,
			edgecomm::powerUpMsg::WEIGHER_CHECKSUM,
			weigherChecksum_ );

	// printers
	for ( unsigned int i( 0 ); i < 3; ++i )
	{
		// printer software
		if ( printerSoftwareVersion_[i].length() )
			appendFirmware_v1( firmware, firmwareCount,
				edgecomm::powerUpMsg::PRINTER1_SOFTWARE_VERSION + (3 * i),
				weigherSoftwareVersion_ );

		// printer hardware
		if ( printerHardwareVersion_[i].length() )
			appendFirmware_v1( firmware, firmwareCount,
				edgecomm::powerUpMsg::PRINTER1_HARDWARE_VERSION + (3 * i),
				printerHardwareVersion_[i] );

		// printer checksum
		if ( printerChecksum_[i].length() )
			appendFirmware_v1( firmware, firmwareCount,
				edgecomm::powerUpMsg::PRINTER1_CHECKSUM + (3 * i),
				printerChecksum_[i] );
	}

    // =============
    //  HEALTH INFO
	// =============

	edgecomm::powerUpMsg_v1_healthInfo healthInfo[128];
	size_t healthInfoCount( 0 );

	for ( dict_type::const_iterator i( healthInfo_.begin() ); i != healthInfo_.end(); ++i )
	{
		if ( 128 == healthInfoCount )
			break;

		util::Timestamp timestamp( i->at( "timestamp" ) );
		util::Timestamp latestOccurrence( i->at( "latestOccurrence" ) );

		appendHealthInfo_v1( healthInfo, healthInfoCount,
			util_string::stringtolong( i->at( "errorCode" ) ),
			timestamp.toUnixEpochTime(),
			util_string::stringtolong( i->at( "count" ) ),
			latestOccurrence.toUnixEpochTime(),
			util_string::stringtolong( i->at( "severity" ) ) );
	}

	// ========================================================================

	edgecomm::EdgePacket packet;
	packet.append( msg );

	// update with firmware offsets
	util::NetworkVal::setValueInt( msg.firmwareInfoOffset_, packet.size() );
	util::NetworkVal::setValueInt( msg.firmwareInfoCount_, firmwareCount );

	for ( size_t i( 0 ); i < firmwareCount; ++i )
		packet.append( firmware[i] );

	// update with health info
	if ( healthInfoCount )
	{
		util::NetworkVal::setValueInt( msg.healthInfoOffset_, packet.size() );
		util::NetworkVal::setValueInt( msg.healthInfoCount_, healthInfoCount );

		for ( size_t i( 0 ); i < healthInfoCount; ++i )
			packet.append( healthInfo[i] );
	}

	// =========
	//  WRAPPER
	// =========

	if ( wrapperSerial_.length() )
	{
		edgecomm::powerUpMsgWrapperInfo_v1 wrapperInfo;

		util::NetworkVal::setValueInt( msg.wrapperInfoOffset_, packet.size() );
		util::NetworkVal::setValueInt( msg.wrapperInfoLength_, edgecomm::EdgePacket::sizeofMsg( wrapperInfo ) );
		peripheralStatus |= edgecomm::powerUpMsg::wrapperOnline;

		util::NetworkVal::setValueString( wrapperInfo.wrapperSerialNumber_, sizeof(wrapperInfo.wrapperSerialNumber_), wrapperSerial_ );
		util::NetworkVal::setValueString( wrapperInfo.wrapperModelNumber_, sizeof(wrapperInfo.wrapperModelNumber_), wrapperModel_ );

		util::NetworkVal::setValueInt( wrapperInfo.upperFilmWidth_, wrapperUpperFilmWidth_ );
		util::NetworkVal::setValueInt( wrapperInfo.lowerFilmWidth_, wrapperLowerFilmWidth_ );
		util::NetworkVal::setValueInt( wrapperInfo.upperFilmUsage_, wrapperUpperFilmUsage_ );
		util::NetworkVal::setValueInt( wrapperInfo.lowerFilmUsage_, wrapperLowerFilmUsage_ );

		util::NetworkVal::setValueInt( wrapperInfo.sealerBeltState_, wrapperBeltEnabled_ );
		util::NetworkVal::setValueInt( wrapperInfo.sealerBeltTempExpected_, wrapperBeltTempSetting_ );
		util::NetworkVal::setValueInt( wrapperInfo.sealerBeltTempActual_, wrapperBeltTempActual_ );

		parseBeltTimes(
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnMon ),
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffMon ),
			wrapperInfo.sealerBeltOnOff_[0] );

		parseBeltTimes(
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnTue ),
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffTue ),
			wrapperInfo.sealerBeltOnOff_[1] );

		parseBeltTimes(
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnWed ),
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffWed ),
			wrapperInfo.sealerBeltOnOff_[2] );

		parseBeltTimes(
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnThu ),
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffThu ),
			wrapperInfo.sealerBeltOnOff_[3] );

		parseBeltTimes(
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnFri ),
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffFri ),
			wrapperInfo.sealerBeltOnOff_[4] );

		parseBeltTimes(
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnSat ),
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffSat ),
			wrapperInfo.sealerBeltOnOff_[5] );

		parseBeltTimes(
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnSun ),
			Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffSun ),
			wrapperInfo.sealerBeltOnOff_[6] );

		util::NetworkVal::setValueInt( wrapperInfo.compressorLine_, wrapperCompressorLine_ );
		util::NetworkVal::setValueInt( wrapperInfo.compressorRegulated_, wrapperCompressorRegulated_ );
		util::NetworkVal::setValueInt( wrapperInfo.compressorRunTime_, wrapperCompressorRunTime_ );

		util::NetworkVal::setValueInt( wrapperInfo.vacuumPumpLine_, wrapperVacuumLine_ );
		util::NetworkVal::setValueInt( wrapperInfo.vacuumPumpRunTime_, wrapperVacuumRunTime_ );

		packet.append( wrapperInfo );
	}

	// =========
	//  WEIGHER
	// =========

	if ( weigherSoftwareVersion_.length() )
	{
		peripheralStatus |= edgecomm::powerUpMsg::weigherOnline;
	}

	// =========
	//  PRINTER
	// =========

	if ( printerSoftwareVersion_[0].length() )
	{
		peripheralStatus |= edgecomm::powerUpMsg::printerOneOnline;
	}

	if ( printerSoftwareVersion_[1].length() )
	{
		peripheralStatus |= edgecomm::powerUpMsg::printerTwoOnline;
	}

	if ( printerSoftwareVersion_[2].length() )
	{
		peripheralStatus |= edgecomm::powerUpMsg::printerThreeOnline;
	}

	// ========================================================================

	if ( !powerUpComplete_ )
	{
		const util::Timestamp now( util::Timestamp::now() );

		powerUpComplete_ = true;
		powerUpTime_ = now.toUnixEpochTime();
	}

	util::NetworkVal::setValueInt( msg.peripheralStatusFlags_, peripheralStatus );
	util::NetworkVal::setValueInt( msg.powerUpTime_, powerUpTime_ );

	// update packet with new data
	packet.update( 0, msg );


	// send!
	TRACE( "sending power up packet..." );
	return sendMsg( packet.transmitBuffer(), packet.transmitSize() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsEdgeDevice::sendStatusCheck()
{
	// for the 'ping' packet only send when we are really online
	if ( !enabled() )
		return false;
	else if (( !clientSocket_ ) || ( !clientSocket_->isValid() ))
		return false;

	edgecomm::statusCheckMsg_v1 msg( serial_, model_ );
	util::NetworkVal::setValueInt( msg.cycleCount_, machineCycleCount() );
	util::NetworkVal::setValueInt( msg.healthInfoCount_, healthInfo_.size() );

	edgecomm::EdgePacket packet;
	packet.append( msg );

	// send!
	TRACE( "sending status check packet..." );
	return sendMsg( packet.transmitBuffer(), packet.transmitSize() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsEdgeDevice::sendMsg( const void *b, size_t bs )
{
	m_.acquire();
	messages_.push_back( std::string( (char*) b, bs ) );
	m_.release();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsEdgeDevice::initialize()
{
    MtdManufacturingData *manfData( FpmManager::getInstance()->readMfgScaleData() );

    // save off serial and model number
    if ( manfData )
    {
		serial_ = manfData->getSerialNumber();
		replaceAll( serial_, "\"", "" );

		model_ = manfData->getScaleStyle();
		replaceAll( model_, "\"", "" );

		TRACE( "serial: " + serial_ );
		TRACE( "model: " + model_ );

		if ( 2 <= model_.length() )
		{
			// scale
			if (( 'H' == model_[0] ) && ( 'T' == model_[1] )) // HTx, HTi, HTs
				isScale_ = true;

			// wrapper
			else if (( 'N' == model_[0] ) && ( 'G' == model_[1] )) // NGW, NGS
				isWrapper_ = true;
			else if (( 'A' == model_[0] ) && ( 'W' == model_[1] )) // AWS3
				isWrapper_ = true;
			else if (( 'E' == model_[0] ) && ( 'P' == model_[1] )) // EPCP
				isWrapper_ = true;
		}

		TRACE( "scale/wrapper: " + util_string::inttostring( isScale_ ) + " " + util_string::inttostring( isWrapper_ ) );
    }

    port_ = 0;
	logLevel_ = 0;

	discoveredPort_ = 0;

	// start listening for discovery packets
	discoverySocket_ = new util::BasicSocket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	if ( !discoverySocket_->isValid() )
		TRACE( "error creating discovery socket!" );
	else if (( discoverySocket_->setBlocking( false ) ) && ( discoverySocket_->bind( AF_INET, INADDR_ANY, EDGE_DISCOVERY_PORT ) ))
	{
		// success
	}
	else
	{
		TRACE( "error binding discovery socket!" );
		discoverySocket_->close();
	}

	clientSocket_ = NULL;

	clientSerialPort_ = -1;

	enabled_ = false;
	powerUpComplete_ = false;

	// PRINTER
	printerCounts_[0] = printerCounts_[1] = printerCounts_[2] = 0;

	// WRAPPER
	wrapperBeltEnabled_ = false;
	wrapperBeltTempActual_ = 0;
	wrapperBeltTempSetting_ = 0;
	wrapperCompressorLine_ = 0;
	wrapperCompressorRegulated_ = 0;
	wrapperCompressorRunTime_ = 0;
	wrapperCycleCount_ = 0;
	wrapperLowerFilmUsage_ = 0;
	wrapperLowerFilmWidth_ = 0;
	wrapperUpperFilmUsage_ = 0;
	wrapperUpperFilmWidth_ = 0;
	wrapperVacuumLine_ = 0;
	wrapperVacuumRunTime_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsEdgeDevice::finalize()
{
#if defined ( __linux__ )
	if ( clientSocket_ )
		delete clientSocket_;

	if ( 0 <= clientSerialPort_ )
		::close( clientSerialPort_ );

	if ( discoverySocket_ )
		delete discoverySocket_;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int DiagnosticsEdgeDevice::machineCycleCount() const
{
	// scale
	if ( isScale_ )
		return printerCounts_[0];

	// wrapper
	else if ( isWrapper_ )
		return wrapperCycleCount_;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsEdgeDevice::replaceAll( std::string &s, const std::string &from, const std::string &to )
{
	const size_t len( from.length() );

	size_t pos( std::string::npos );

	while ( std::string::npos != (pos = s.find_last_of( from, pos )) )
		s.replace( pos, len, to );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsEdgeDevice::parseBeltTimes( const std::string &on, const std::string &off, unsigned char *buffer )
{
	buffer[0] = buffer[1] = buffer[2] = buffer[3] = 0;

	if (( 8 <= on.length() ) && ( ':' == on[2] ) && ( ' ' == on[5] ))
	{
		unsigned char hour( util_string::stringtoint( on.substr( 0, 2 ) ) );
        unsigned char minute( util_string::stringtoint( on.substr( 3, 2 ) ) );

		if (( 'P' == on[6] ) || ( 'p' == on[6] ))
			hour += 12;

		buffer[0] = hour;
        buffer[1] = minute;
	}

	if (( 8 <= off.length() ) && ( ':' == off[2] ) && ( ' ' == off[5] ))
	{
		unsigned char hour( util_string::stringtoint( off.substr( 0, 2 ) ) );
        unsigned char minute( util_string::stringtoint( off.substr( 3, 2 ) ) );

		if (( 'P' == off[6] ) || ( 'p' == off[6] ))
			hour += 12;

		buffer[2] = hour;
        buffer[3] = minute;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsEdgeDevice::appendFirmware_v1( edgecomm::powerUpMsg_v1_firmware *p, size_t &pi, int type, const std::string version )
{
	if ( version.empty() )
		return;

	std::string versionTrimmed( version );
	util_string::trim( versionTrimmed );

	edgecomm::powerUpMsg_v1_firmware *block( &p[pi++] );

    util::NetworkVal::setValueInt( block->firmwareType_, type );
    util::NetworkVal::setValueString( block->firmwareVersion_, sizeof(block->firmwareVersion_), versionTrimmed );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsEdgeDevice::appendHealthInfo_v1( edgecomm::powerUpMsg_v1_healthInfo *p, size_t &pi, unsigned int errorCode, unsigned int date, unsigned int count, unsigned int lastOccurrence, unsigned int severity )
{
	if ( !errorCode )
		return;

	edgecomm::powerUpMsg_v1_healthInfo *block( &p[pi++] );

    util::NetworkVal::setValueInt( block->errorCode_, errorCode );
    util::NetworkVal::setValueInt( block->date_, date );
    util::NetworkVal::setValueInt( block->numberOccurrences_, count );
    util::NetworkVal::setValueInt( block->lastOccurrenceDate_, lastOccurrence );
    util::NetworkVal::setValueInt( block->severity_, severity );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsEdgeDevice::setInterfaceAttrib( int fd, unsigned int baudRate, unsigned char parity )
{
#if defined ( __linux__ )
	struct termios tty;
    memset( &tty, 0, sizeof(tty) );

    // get attributes
    if ( 0 != tcgetattr( fd, &tty ) )
    {
        TRACE( "failed to get com attributes " + util_string::inttostring( errno ) );
        return false;
    }

    // determine speed
    speed_t speed;

    switch ( baudRate )
    {
    case 50: speed = B50; break;
    case 75: speed = B75; break;
    case 110: speed = B110; break;
    case 134: speed = B134; break;
    case 150: speed = B150; break;
    case 200: speed = B200; break;
    case 300: speed = B300; break;
    case 600: speed = B600; break;
    case 1200: speed = B1200; break;
    case 1800: speed = B1800; break;
    case 2400: speed = B2400; break;
    case 4800: speed = B4800; break;
    case 9600: speed = B9600; break;
    case 19200: speed = B19200; break;
    case 38400: speed = B38400; break;
    case 57600: speed = B57600; break;
    case 115200: speed = B115200; break;
    case 230400: speed = B230400; break;
    case 460800: speed = B460800; break;
    case 500000: speed = B500000; break;
    case 576000: speed = B576000; break;
    case 921600: speed = B921600; break;
    case 1000000: speed = B1000000; break;
    case 1152000: speed = B1152000; break;
    case 1500000: speed = B1500000; break;
    case 2000000: speed = B2000000; break;
    case 2500000: speed = B2500000; break;
    case 3000000: speed = B3000000; break;
    case 3500000: speed = B3500000; break;
    case 4000000: speed = B4000000; break;
    default:
        TRACE( "invalid baud rate " + util_string::inttostring( baudRate ) );
        return false;
    }

    cfsetospeed( &tty, speed );
    cfsetispeed( &tty, speed );

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars

    // disable IGNBRK for mismatched speed tests; otherwise receive break as \000 chars
    tty.c_iflag &= ~IGNBRK;                         // disable break processing
    tty.c_iflag &= ~(INLCR | ICRNL);                // do not convert carriage returns or new lines
    tty.c_lflag = 0;                                // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;                                // no remapping, no delays
    tty.c_cc[VMIN] = 0;                             // read doesn't block
    tty.c_cc[VTIME] = 5;                            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);                // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);              // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    // set attributes
    if ( 0 != tcsetattr( fd, TCSANOW, &tty ) )
    {
        TRACE( "error setting comm attributes " + util_string::inttostring( errno ) );
        return false;
    }

#endif
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsEdgeDevice::setBlocking( int fd, bool shouldBlock )
{
#if defined ( __linux__ )
	struct termios tty;
    memset( &tty, 0, sizeof(tty) );

    // get attributes
    if ( 0 != tcgetattr( fd, &tty ) )
    {
        TRACE( "failed to get com attributes " + util_string::inttostring( errno ) );
        return false;
    }

    // set blocking and read timeout
    tty.c_cc[VMIN] = (shouldBlock ? 1 : 0);
    tty.c_cc[VTIME] = 5;                        // 0.5 seconds read timeout

    // set attributes
    if ( 0 != tcsetattr( fd, TCSANOW, &tty ) )
    {
        TRACE( "error setting comm attributes " + util_string::inttostring( errno ) );
        return false;
    }
#endif
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsEdgeDevice::serialDeviceExists()
{
#if defined ( __linux__ )
	return ( 0 == ::access( EDGE_SERIAL_DEVICE, F_OK ) );
#else
	return false;
#endif
}
