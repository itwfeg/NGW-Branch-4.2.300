/**
 * @file motorolassiscanner.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "motorolassiscanner.h"
#include "serialiolinux.h"
#include "serialiowin.h"

#include <iostream>
#include <fstream>

#ifdef __linux__
#include <unistd.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
motorolaScanner::motorolaScanner( const std::string &portName ) :
    device_( NULL ),
    autoDisable_( false ),
    enabled_( false )
{
    bool okay( false );

    // create device
    device_ = new serialComm();

    // open device
    if ( !device_->open( portName ) )
        std::cerr << "could not open scanner device on port " << portName << std::endl;

    // setup scanner
    else if ( !setupDevice() )
        std::cerr << "error setting up device" << std::endl;

    else
    {
        okay = true;
    }

    // scanner did not setup properly, delete it
    if ( !okay )
    {
        delete device_;
        device_ = NULL;
    }

    // setup param size mapping
    paramSize_[0x71] = 2;
    paramSize_[0x78] = 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
motorolaScanner::~motorolaScanner()
{
    if ( device_ )
    {
        delete device_;
        device_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool motorolaScanner::continuousScanning() const
{
    // always true, we turn this on in setupDevice().
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool motorolaScanner::isValid() const
{
    return (( device_ ) && ( device_->isValid() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void motorolaScanner::setEnabled( bool value )
{
    std::string data;

    // nothing to do
    if ( enabled_ == value )
        return;

    // start/stop decoding
    writeDevice( createHostPacket( value ? START_SESSION : STOP_SESSION ) );

    // wait for reply
    if ( !readDeviceReply( CMD_ACK, data ) )
    {
        std::cerr << "error starting session" << std::endl;
        return;
    }

    // set value
    enabled_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void motorolaScanner::setLedEnabled( bool value )
{
    std::string data;
    data.push_back( (unsigned char) 0xFF );

    // turn on/off led
    writeDevice( createHostPacket( value ? LED_ON : LED_OFF, data ) );

    // wait for reply
    if ( !readDeviceReply( CMD_ACK, data ) )
        std::cerr << "error setting led values" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int motorolaScanner::timeout() const
{
    // Decode Session Timeout (Parameter # 88h)
    std::map<size_t, size_t>::const_iterator value( params_.find( 0x88 ) );

    if ( params_.end() == value )
        return 0;

    return (100 * value->second);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void motorolaScanner::clear()
{
    while ( !barcodes_.empty() )
        barcodes_.pop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool motorolaScanner::read( barcodeType &type, std::string &data )
{
    // process device
    readDevice();

    // check buffer
    if ( barcodes_.empty() )
        return false;

    barcode b( barcodes_.front() );
    barcodes_.pop();

    type = b.first;
    data = b.second;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool motorolaScanner::setupDevice()
{
    std::string data;

    // request version
    writeDevice( createHostPacket( REQUEST_REVISION ) );

    // wait for reply
    if ( !readDeviceReply( REPLY_REVISION, data ) )
    {
        std::cerr << "device did return revision data!" << std::endl;
        return false;
    }

    std::cout << data << std::endl;

    // set decoder mode
    data.clear();
    data.push_back( (unsigned char) MODE_DECODE );

    writeDevice( createHostPacket( IMAGER_MODE, data ) );

    // wait for reply
    if ( !readDeviceReply( CMD_ACK, data ) )
    {
        std::cerr << "error setting decode mode" << std::endl;
        return false;
    }

    // set scanner parameters
    data.clear();
    data.push_back( (unsigned char) 0xFF ); // beep not required
    data.push_back( (unsigned char) 0xEE ); // "Decode Data Packet Format" parameter
    data.push_back( (unsigned char) 0x01 ); // enable
    data.push_back( (unsigned char) 0xF1 ); // "Continuous Bar Code Read" parameter
    data.push_back( (unsigned char) 0x89 );
    data.push_back( (unsigned char) 0x01 ); // enable

    writeDevice( createHostPacket( PARAM_SEND, data, 0x08 ) );

    // wait for reply
    if ( !readDeviceReply( CMD_ACK, data ) )
    {
        std::cerr << "error setting decode mode" << std::endl;
        return false;
    }

    // request all parameters
    data.clear();
    data.push_back( (unsigned char) 0xFE );

    writeDevice( createHostPacket( PARAM_REQUEST, data ) );

    // wait for reply
    if ( !readDeviceReply( PARAM_SEND, data ) )
    {
        std::cerr << "error setting decode mode" << std::endl;
        return false;
    }
    else
    {
        std::ofstream params( "param.bin", std::ios_base::out | std::ios_base::binary );
        params << data;

        // store params
        size_t index( 0 );

        while ( index < data.length() )
        {
            // retrieve value
            size_t param( (unsigned char) data[index++] );

            // check for extended param
            if ( 0xF0 <= param )
            {
                param <<= 8;
                param += (unsigned char) data[index++];
            }

            // Note:
            // Skip extended param processing for now...
            if ( 0xF0 <= param )
                break;

            // calc param length
            size_t paramLen( 1 );

            std::map<size_t, size_t>::const_iterator i( paramSize_.find( param ) );

            if ( i != paramSize_.end() )
                paramLen = i->second;

            // retrieve value
            size_t value( 0 );

            while ( paramLen-- )
            {
                value <<= 8;
                value += (unsigned char) data[index++];
            }

            // insert
            params_[param] = value;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void motorolaScanner::readDevice()
{
    std::string payload;

    // read all opcodes
    readDeviceReply( _ALL_OPCODES, payload );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool motorolaScanner::readDeviceReply( ssiOpcode opcode, std::string &payload )
{
    static const size_t sleepTime( 250 );

    // clear payload
    payload.clear();

    // process packets until we find desired opcode or timeout expires
    for ( size_t maxWaitTime( 2500 ); 0 < maxWaitTime; maxWaitTime -= sleepTime )
    {
        // read from device
        buffer_.append( device_->read() );

        // check for messages
        while ( !buffer_.empty() )
        {
            size_t len( (unsigned char) buffer_[0] );
            len += 2;

            // no message or partial message
            if ( buffer_.length() < len )
                break;

            // extract message
            std::string packet( buffer_.substr( 0, len ) );

            buffer_.erase( 0, len );

            // sum packet
            unsigned short checksum( 0 );

            for ( size_t i( 0 ); i < (len-2); ++i )
                checksum += (unsigned char) packet[i];

            // take two's complement
            checksum = (~checksum + 1);

            // validate
            if (( (unsigned char) ((checksum >> 8) & 0xFF) == (unsigned char) packet[len-2] ) &&
                ( (unsigned char) (checksum & 0xFF) == (unsigned char) packet[len-1] ))
                packets_.push( packet );
            else
            {
                std::cerr << "bad packet checksum!" << std::endl;
            }
        }

        // packets to process
        while ( packets_.size() )
        {
            // pull packet from stack
            const std::string packet( packets_.front() );
            packets_.pop();

            // process packet
            unsigned char status;

            std::string packetPayload;
            ssiOpcode packetOpcode( processDevicePacket( packet, status, packetPayload ) );

            // update payload
            payload.append( packetPayload );

            // check for opcode match and last packet of multipacket transmission
            if ( opcode == packetOpcode )
                if ( !(0x2 & status) )
                    return true;
        }

        // not waiting for specific opcode
        if ( _ALL_OPCODES == opcode )
            return true;

        sleep( sleepTime );
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void motorolaScanner::writeDevice( const std::string &packet )
{
    device_->write( packet );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
motorolaScanner::ssiOpcode motorolaScanner::processDevicePacket( const std::string &packet, unsigned char &status, std::string &payload )
{
    const ssiOpcode opcode( (ssiOpcode) ((unsigned char) packet[1]) );

    // set status
    status = (unsigned char) packet[3];

    // check for payload
    if ( 6 < packet.size() )
        payload = packet.substr( 4, packet.size() - 6 );

    switch ( opcode )
    {
    case DECODE_DATA:

        // send ack
        writeDevice( createHostPacket( CMD_ACK ) );

        // validate payload length
        if ( 1 < payload.length() )
        {
            const barcodeType type( (barcodeType) ((unsigned char) payload[0]) );

            // we don't handle multipacket formats
            if ( (barcodeType) MULTIPACKET_FORMAT != type )
            {
                // queue barcode
                barcodes_.push( barcode( type, payload.substr( 1 ) ) );
            }
        }

        // disable scanner
        if ( autoDisable_ )
            setEnabled( false );

        break;

    case PARAM_SEND:

        // we do not care about beep code on this message, strip it out
        payload = payload.substr( 1 );
        break;

    default:
        break;
    }

    return opcode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string motorolaScanner::createHostPacket( ssiOpcode opcode, char status )
{
    return createHostPacket( opcode, std::string(), status );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string motorolaScanner::createHostPacket( ssiOpcode opcode, const std::string &payload, char status )
{
    unsigned short checksum( 0 );

    // create packet
    std::string packet;
    packet.push_back( 4 + payload.length() );               // length
    packet.push_back( (char) opcode );                      // opcode
    packet.push_back( 4 );                                  // message source (host)
    packet.push_back( status );                             // status
    packet.append( payload );                               // payload

    // sum packet
    for ( std::string::const_iterator i( packet.begin() ); i != packet.end(); ++i )
        checksum += (unsigned char) *i;

    // take two's complement
    checksum = (~checksum + 1);

    packet.push_back( (unsigned char) ((checksum >> 8) & 0xFF) );    // checksum high
    packet.push_back( (unsigned char) (checksum & 0xFF) );           // checksum low

    return packet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void motorolaScanner::sleep( unsigned int milliseconds )
{
#ifdef __linux__
    usleep( milliseconds * 1000 );
#else
    Sleep( milliseconds );
#endif
}
