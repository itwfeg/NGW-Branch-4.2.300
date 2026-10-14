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

//#include <Debug.h>

#include <e_string/utilstring.h>

//#include <bootParameters.h>

//#include <CfgInternal.h>
//#include <Configuration.h>

//#include <FpmManager.h>

//#include <TimerManager.h>

//#include <VersionInfo.h>
//#include <VersionManager.h>

#if defined( _MSC_VER )
#if defined ( WIN32 )
#define __func__ __FUNCTION__
#endif
#elif defined( __GNUC__ )
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#endif

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DiagnosticsEdgeDevice::dataType_ = 0;
std::string DiagnosticsEdgeDevice::dataTypeName_( "rtc.diagnosticsmanager.diagnosticsedgedevice" );
#endif

std::string DiagnosticsEdgeDevice::category_( "DiagnosticsManager.edgedevice" );

//#define TRACE( logline ) Debug::trace( category_, (logline) )
//#define TRACEFUNC( logline ) if (Debug::isTracing( category_ + "." + __func__ )) TRACE( (logline) )
#define TRACE( logline ) std::cout << category_ << "  " << (logline) << std::endl
#define TRACEFUNC( logline ) std::cout << category_ + "." + __func__ << "  " << (logline) << std::endl

extern std::string CfgDiagGatewayDeviceAddress;
extern std::string CfgSerial;
extern std::string CfgModel;
extern std::string CfgAddress;
extern std::string CfgHostname;

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsEdgeDevice::DiagnosticsEdgeDevice()
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

    address_ = CfgDiagGatewayDeviceAddress; //Configuration::getInstance()->toStringCfg( fieldOffsetCfgDiagGatewayDeviceAddress );
	port_ = EDGE_CLIENT_PORT;

	if (( address_.empty() ) && ( discoveredAddress_.size() ))
	{
		address_ = discoveredAddress_;

		if ( discoveredPort_ )
			port_ = discoveredPort_;
	}

	if ( address_.empty() )
		return false;

    logLevel_ = 1;//Configuration::getInstance()->toIntCfg( fieldOffsetCfgDiagGatewayDeviceStatusMsgLogLevel );

	return true;
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

				TRACEFUNC( "discovery buffer:\n" + discoveryBuffer_);
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
            //m_.acquire();
            m_.lock();

			if ( messages_.size() )
			{
				TRACE( "sending client data... [" + util_string::inttostring( messages_.size() ) + "]" );
				const std::string message( messages_.front() );

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

            //m_.release();
            m_.unlock();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsEdgeDevice::checkOnline()
{
	// nothing to do
	if ( !enabled() )
	{
		TRACE( "gateway not enabled" );
	}

	// connect to gateway server
	else if ( !clientSocket_ )
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
			TRACE( "CONNECTED!" );
		}
	}
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

//    MtdManufacturingData *manfData( FpmManager::getInstance()->readMfgScaleData() );

	// ========
	//  HEADER
	// ========

	edgecomm::powerUpMsg_v1 msg( serial_, model_ );

	uint16_t peripheralStatus( 0 );

    // manufacture date
//    if ( manfData )
//    {
        std::string dof( "01/02/2003" /*manfData->getDateOfManufacturing()*/ );
        replaceAll( dof, "\"", "" );

        util::Timestamp manufactureDate;
        manufactureDate.fromDateTime( dof + " 08:00" );

        util::NetworkVal::setValueInt( msg.manufactureDate_, manufactureDate.toUnixEpochTime() );
//    }

	// ip address
//	std::string ip;
//	std::string gateway;

//	VersionInfo::getIpAndGatewayAddress( ip, gateway );
//    ip = "127.0.0.1";

    util::NetworkVal::setValueString( msg.machineAddress_, sizeof(msg.machineAddress_), CfgAddress );

	// hostname
    //util::NetworkVal::setValueString( msg.machineHostname_, sizeof(msg.machineHostname_), bootParameters::getDNSHostName() );
    util::NetworkVal::setValueString( msg.machineHostname_, sizeof(msg.machineHostname_), CfgHostname );

	// cycle count
	util::NetworkVal::setValueInt( msg.cycleCount_, machineCycleCount() );

	// ==============
	//  VERSION INFO
	// ==============

    edgecomm::powerUpMsg_v1_firmware firmware[64];
	size_t firmwareCount( 0 );

    // system controller version
//    if ( manfData )
//    {
        std::string rev( "2.0B" /*manfData->getBoardRevision()*/ );
    	replaceAll( rev, "\"", "" );

		appendFirmware_v1( firmware, firmwareCount,
			edgecomm::powerUpMsg::SYSTEM_CONTROLLER_VERSION,
			rev );
//    }

    // application version
    appendFirmware_v1( firmware, firmwareCount,
    	edgecomm::powerUpMsg::APPLICATION_VERSION,
        "2.5.301" /*VersionInfo::getApplicationVersion()*/ );

    // os version
    appendFirmware_v1( firmware, firmwareCount,
    	edgecomm::powerUpMsg::OS_VERSION,
        "3.10.17.1.0.0-" /*VersionManager::getInstance()->getOSVersionAsString()*/ );

    // loader version
    appendFirmware_v1( firmware, firmwareCount,
    	edgecomm::powerUpMsg::LOADER_VERSION,
        "2.1.0" /*FpmManager::getInstance()->getLoaderVersion()*/ );

    // backend version
    appendFirmware_v1( firmware, firmwareCount,
    	edgecomm::powerUpMsg::SM_BACKEND_VERSION,
        "2.0.17" /*VersionInfo::getSMBackendVersion()*/ );

    // wrapper node app versions
	// ignore node zero (0), use 1-N
	for ( size_t i( 1 ); i < wrapperNodeAppVersions_.size(); ++i )
		if ( wrapperNodeAppVersions_[i].length() )
		{
			appendFirmware_v1( firmware, firmwareCount,
				edgecomm::powerUpMsg::WRAPPER_NODE1_APP_VERSION + i - 1,
				wrapperNodeAppVersions_[i] );
		}

    // wrapper node data versions
	// ignore node zero (0), use 1-N
	for ( size_t i( 1 ); i < wrapperNodeDataVersions_.size(); ++i )
		if ( wrapperNodeDataVersions_[i].length() )
		{
			appendFirmware_v1( firmware, firmwareCount,
				edgecomm::powerUpMsg::WRAPPER_NODE1_DATA_VERSION + i - 1,
				wrapperNodeDataVersions_[i] );
		}

    // wrapper node boot versions
	// ignore node zero (0), use 1-N
	for ( size_t i( 1 ); i < wrapperNodeBootVersions_.size(); ++i )
		if ( wrapperNodeBootVersions_[i].length() )
		{
			appendFirmware_v1( firmware, firmwareCount,
				edgecomm::powerUpMsg::WRAPPER_NODE1_BOOT_VERSION + i - 1,
				wrapperNodeBootVersions_[i] );
		}

    // wrapper node checksums
	// ignore node zero (0), use 1-N
	for ( size_t i( 1 ); i < wrapperNodeChecksums_.size(); ++i )
		if ( wrapperNodeChecksums_[i].length() )
		{
			appendFirmware_v1( firmware, firmwareCount,
				edgecomm::powerUpMsg::WRAPPER_NODE1_CHECKSUM + i - 1,
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
            "08:00 am" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnMon )*/,
            "04:30 pm" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffMon )*/,
			wrapperInfo.sealerBeltOnOff_[0] );

		parseBeltTimes(
            "08:00 am" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnTue )*/,
            "04:30 pm" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffTue)*/,
			wrapperInfo.sealerBeltOnOff_[1] );

		parseBeltTimes(
            "08:00 am" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnWed)*/,
            "04:30 pm" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffWed)*/,
			wrapperInfo.sealerBeltOnOff_[2] );

		parseBeltTimes(
            "08:00 am" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnThu)*/,
            "04:30 pm" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffThu)*/,
			wrapperInfo.sealerBeltOnOff_[3] );

		parseBeltTimes(
            "08:00 am" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnFri )*/,
            "04:30 pm" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffFri )*/,
			wrapperInfo.sealerBeltOnOff_[4] );

		parseBeltTimes(
            "09:30 am" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnSat )*/,
            "04:00 pm" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffSat )*/,
			wrapperInfo.sealerBeltOnOff_[5] );

		parseBeltTimes(
            "09:30 am" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOnSun )*/,
            "04:00 pm" /*Configuration::getInstance()->toStringCfg( fieldOffsetCfgWrSealerBeltTimeOffSun )*/,
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

	util::NetworkVal::setValueInt( msg.peripheralStatusFlags_, peripheralStatus );

	// update packet with new data
	packet.update( 0, msg );

	powerUpComplete_ = true;

	powerUpTime_ = (long long) now;

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
    m_.lock(); //.acquire();
	messages_.push_back( std::string( (char*) b, bs ) );
    m_.unlock(); //.release();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsEdgeDevice::initialize()
{
//    MtdManufacturingData *manfData( FpmManager::getInstance()->readMfgScaleData() );

    // save off serial and model number
//    if ( manfData )
//    {
        serial_ = CfgSerial; //manfData->getSerialNumber();
		replaceAll( serial_, "\"", "" );

        model_ = CfgModel; //manfData->getScaleStyle();
		replaceAll( model_, "\"", "" );

		TRACE( "serial: " + serial_ );
		TRACE( "model: " + model_ );
//    }

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

	enabled_ = false;
	powerUpComplete_ = false;

	// FPC
	fpcCycleCount_ = 0;

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
	if ( clientSocket_ )
		delete clientSocket_;

	if ( discoverySocket_ )
		delete discoverySocket_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int DiagnosticsEdgeDevice::machineCycleCount() const
{
	if ( fpcCycleCount_ )
		return fpcCycleCount_;

	return wrapperCycleCount_;
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

	edgecomm::powerUpMsg_v1_firmware *block( &p[pi++] );

    util::NetworkVal::setValueInt( block->firmwareType_, type );
    util::NetworkVal::setValueString( block->firmwareVersion_, sizeof(block->firmwareVersion_), version );
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
