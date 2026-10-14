/**
 * @file hobartweigher.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobartweigher.h"

#include <iostream>
#include <iomanip>

#include <memory.h>
#include <stdint.h>

#define CONFIGMSGSIZE 64

// Weigher status bits.


///////////////////////////////////////////////////////////////////////////////////////////////////
hobartWeigher::hobartWeigher()
{
    qRegisterMetaType<hobartWeigher::weigherState>("hobartWeigher::weigherState");
    isAvoir = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartWeigher::~hobartWeigher()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::setWeigherMode( unsigned int mode )
{
    static const size_t msgLen( 4 );

    unsigned char msg[16];
    msg[0] = wgMode;
    msg[1] = 0;
    msg[2] = mode;
    msg[3] = 0;

    if ( msgLen != sendWeigherMessage( msg, msgLen, XFER_TIMEOUT ) )
        std::cout << "failed to set weigher mode!" << std::endl;
}


void hobartWeigher::setReqConfigMode(unsigned int mode)
{
    static const size_t msgLen( 2 );

    unsigned char msg[16];
    msg[0] = mode;
    msg[1] = 0;

    if ( msgLen != sendWeigherMessage( msg, msgLen, XFER_TIMEOUT ) )
        std::cout << "failed to set weigher mode!" << std::endl;
}

void hobartWeigher::setReqStatus(unsigned int mode)
{
    static const size_t msgLen( 2 );

    unsigned char msg[16];
    msg[0] = mode;
    msg[1] = 0;

    if ( msgLen != sendWeigherMessage( msg, msgLen, XFER_TIMEOUT ) )
        std::cout << "failed to set weigher mode!" << std::endl;
}


void hobartWeigher::setWriteConfig(int state) {
    static const size_t msgLen( 106 );

    switch ( state )
    {
    case 1:
            if(state>0) {
            const unsigned char buffer1[] ={0x01,0x00,0x00,0x01,0x57,0x47,0x00,0x00,
                       0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                       0x00,0x05,0x00,0x04,0x97,0x64,0x00,0x0F,
                       0x42,0x40,0x00,0x00,0x00,0x00,0x00,0x00,
                       0x00,0x00,0x00,0x0F,0x00,0x2D,0x00,0x05,
                       0x00,0x01,0x00,0x03,0x00,0x3C,0x00,0x3C,
                       0x00,0x3C,0x00,0x0F,0x00,0x64,0x00,0x24,
                       0x00,0x08,0x00,0x18,0x00,0x01,0x00,0x00,
                       0x75,0x30,0x17,0x70,0x02,0x00,0x58,0x00,
                       0xEC,0xFD,0x00,0x02,0x58,0x88,0xBE,0x83,
                       0x00,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,
                       0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                       0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
                       0x00,0x0A };
            if ( msgLen != sendWeigherMessage( buffer1, msgLen, XFER_TIMEOUT ) )
                std::cout << "failed to set weigher mode!" << std::endl;
        }
        break;
    case 2:
            if(state>0) {
            const unsigned char buffer2[] ={0x01,0x00,0x80,0x00,0x57,0x47,0x00,0x00,
                               0x00,0x00,0x00,0x1A,0xEE,0x12,0x00,0x00,
                               0x00,0x05,0x00,0x04,0x97,0x64,0x00,0x11,
                               0x08,0xAC,0x00,0x00,0x00,0x00,0x00,0x00,
                               0x00,0x00,0x00,0x0F,0x00,0x2D,0x00,0x05,
                               0x00,0x01,0x00,0x03,0x00,0x3C,0x00,0x3C,
                               0x00,0x3C,0x00,0x0F,0x00,0x64,0x00,0x24,
                               0x00,0x08,0x00,0x18,0x00,0x01,0x00,0x00,
                               0x75,0x30,0x17,0x70,0x02,0x00,0x58,0x00,
                               0xEC,0xFD,0x00,0x02,0x58,0x88,0xBE,0x83,
                               0x00,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,
                               0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                               0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
                               0x00,0x0A };
            if ( msgLen != sendWeigherMessage( buffer2, msgLen, XFER_TIMEOUT ) )
                std::cout << "failed to set weigher mode!" << std::endl;
        }
        break;
    case 3:
        if(state>0) {
        const unsigned char buffer3[] ={0x01,0x00,0x80,0x00,0x57,0x47,0x00,0x00,
                           0x00,0x00,0x00,0x1A,0xEE,0x12,0x00,0x00,
                           0x00,0x05,0x00,0x04,0x97,0x64,0x00,0x11,
                           0x08,0xAC,0x00,0x00,0x00,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x0F,0x00,0x2D,0x00,0x05,
                           0x00,0x01,0x00,0x03,0x00,0x3C,0x00,0x3C,
                           0x00,0x3C,0x00,0x0F,0x00,0x64,0x00,0x24,
                           0x00,0x08,0x00,0x18,0x00,0x01,0x00,0x00,
                           0x75,0x30,0x17,0x70,0x02,0x00,0x59,0xE9,
                           0xFF,0x0D,0x00,0x03,0x58,0x88,0xBE,0x83,
                           0x00,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                           0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
                           0x00,0x0A };
        if ( msgLen != sendWeigherMessage( buffer3, msgLen, XFER_TIMEOUT ) )
            std::cout << "failed to set weigher mode!" << std::endl;
        }
        break;

    default:
        break;

    }

}

void hobartWeigher::sendColdReset(){
    if ( !sendWeigherMessageBasic( wgColdReset ) )
        std::cout << "failed to cold reset weigher!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::weigherEnable()
{
    if ( !sendWeigherMessageBasic( wgEnable ) )
        std::cout << "failed to enable weigher!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::weigherWakeup()
{
    if ( !sendWeigherMessageBasic( wgWakeup) )
        std::cout << "failed to wakeup weigher!" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::weigherZero()
{
    if ( !sendWeigherMessageBasic( wgRezero ) )
        std::cout << "failed to zero weigher!" << std::endl;
}


void hobartWeigher::sendFactoryReset(){
    if(!sendWeigherMessageBasic(wgResetFactoryDefaults))
        std::cout << "failed reset weigher!" << std::endl;
}

void hobartWeigher::reqWeigherConfig(){
    if(!sendWeigherMessageBasic(wgReqConfig))
        std::cout << "failed to send get config!" << std::endl;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartWeigher::recvWeigherMessage( const unsigned char *buffer, size_t bufferLen )
{
    if ( bufferLen < 2 )
        return;

    const weigherMessage msgType( (weigherMessage) buffer[0] );

    //std::cout << "recv weigher message " << msgType << std::endl;

    // handle message
    switch ( msgType )
    {
    case wgWakeup:
        //qDebug()<<"Wakeup Bufferlen:"<<bufferLen<<"Buffer:"<<buffer;
        ba = QByteArray::fromRawData((const char*)buffer,bufferLen);
        //qDebug()<<"Bytes: "<<ba;

    case wgStatus:
        if ( 12 <= bufferLen )
        {

            baWriteCfg.clear();
            baWriteCfg = QByteArray::fromRawData((const char*)buffer,bufferLen);
            Q_EMIT sig_WeigherStatus(buffer, bufferLen);
            const uint16_t flags( be16toh( *reinterpret_cast<const uint16_t*>( &buffer[2] ) ) );
            uint32_t counts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[4] ) ) );
            const uint32_t rawCounts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[8] ) ) );
            const uint32_t filteredUncalibratedCounts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[12] ) ) );
            const uint32_t ADCounts( be32toh( *reinterpret_cast<const uint32_t*>( &buffer[16] ) ) );

            //qDebug() << baWriteCfg;
#if 1
            std::cout << "status: flags " << std::uppercase << std::setfill('0') << std::setw( 4 ) << std::hex << flags << std::dec <<
                ", counts " << counts <<
                ", rawCounts " << rawCounts <<
                ", filteredUncalibratedCounts " << filteredUncalibratedCounts <<
                ", ADCounts " << ADCounts << std::endl;
#endif
            //if metric mode device counts in half
            if(!isAvoir){
                std::cout << "Half counts" << counts << std::endl;
                counts /= 2;
                std::cout << "Half counts" << counts << std::endl;
            }else{
                std::cout << "full counts" << std::endl;
            }
            if(flags & WEIGHER_KEY_PRESSED){
                 std::cout << "weigher key pressed" << std::endl;
                 Q_EMIT sig_WeigherKeyPressed();
            }

            // error conditions
            if (( SCALE_WARMUP & flags ) || ( WEIGHER_DISABLED & flags )){
                std::cout << "weigher in warmup or disbled" << std::endl;
                break;
            }
            else if (( WEIGHER_CALIBRATION_FAILED & flags ) || ( WEIGHER_EEP_READ_FAILED & flags ))
                Q_EMIT weigherStateChanged( error, 0.0 );
            else if ( ZERO_OUT_OF_MAINTENANCE_ZONE & flags )
                Q_EMIT weigherStateChanged( outOfMaint, 0.0 );

            // bad or unstable weight
            else if ( LARGE_MOTION & flags )
                Q_EMIT weigherStateChanged( unstable, 0.0 );
            else if ( NEGATIVE_GROSS_WEIGHT & flags )
                Q_EMIT weigherStateChanged( underWeight, 0.0 );
            else if ( OVER_GROSS_WEIGHT_LIMIT & flags )
                Q_EMIT weigherStateChanged( overWeight, 0.0 );

            // stable weight
            else if (( WITHIN_QUARTER_GRADUATION_OF_ZERO & flags ) || ( NEW_ZERO & flags ))
                Q_EMIT weigherStateChanged( zero, 0.0 );
            else
            {
                //round counts to the nearest 50ths;
                int newCount = counts;
                newCount = qRound(newCount/50.0) * 50;

                // round up
                float rawWeight( newCount );
                rawWeight /= 100.0;

                float weight( rawWeight  );
                weight /= 100.0;
                Q_EMIT weigherStateChanged( stable, weight );
            }
        }

        break;
    case wgReqConfig:
        //qDebug()<<"pk Bufferlen:"<<bufferLen<<"Buffer:"<<buffer;

        ba = QByteArray::fromRawData((const char*)buffer,bufferLen);
        //std::cout <<"Bytes: " << std::hex << ba.toHex().toStdString();
        //std::cout << std::endl;



        if(100 < bufferLen) {
            WeigherConfiguration config(buffer);
            isAvoir = ((config.getFlags() & 0x01) == 0x00);
            Q_EMIT weigherConfigChanged(config);

        }

        break;

      case wgColdReset:
        baWriteCfg = QByteArray::fromRawData((const char*)buffer,bufferLen);
        break;

    default:
        break;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
size_t hobartWeigher::sendWeigherMessage( const unsigned char *buffer, size_t bufferLen, unsigned int timeout )
{
    return sendData( weigherInterruptOut(), logicalAddrWeigher(), buffer, bufferLen, timeout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartWeigher::sendWeigherMessageBasic( weigherMessage msgType )
{
    static const size_t msgLen( 2 );

    unsigned char msg[16];
    msg[0] = msgType;
    msg[1] = 0;

    return (msgLen == sendWeigherMessage( msg, msgLen, XFER_TIMEOUT ));
}
