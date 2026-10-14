/**
 * @file hobartcandev.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "hobartcandev.h"

#include <linux/can.h>

#include <iomanip>
#include <iostream>

#include <QtGlobal>
static const bool DISPLAY_DEBUG = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartCanDev::hobartCanDev( canSocket *s ) :
    _Mybase( s )
{
    connect( this, SIGNAL(readyRead()), SLOT(onReadyRead()), Qt::QueuedConnection );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobartCanDev::~hobartCanDev()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanDev::addMessage( const rangeType &range, const messageSizeList &sizes )
{
    // validate sizes
    if ( range.second != (unsigned int) sizes.size() )
    {
        std::cout << "addMessage() - sizes didnt match. range.second: " << range.second << "    sizes.size: " << (unsigned int) sizes.size() << std::endl;
        return;
    }

    // update message info mapping
    for ( unsigned int i( 0 ); i < range.second; ++i )
    {
        unsigned int id( range.first + i );

        // message size must be in range (1, CAN_MAX_DLEN)
        messageInfo_[id] = messageInfo( range, qMax( (unsigned int) 1, (unsigned int) qMin( (unsigned int) CAN_MAX_DLEN, sizes[i] ) ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hobartCanDev::sendData( unsigned int id, const unsigned char *buffer, size_t bufferLen )
{
    QList<can_frame> frames;

    std::cerr << "HCDev::sendData() START " << std::endl;

    int framectr = 1;
    // generate frames
    while ( bufferLen )
    {
        if ( !messageInfo_.contains( id ) )
        {
            if( id >= 1056 && id <= 1133 )
                std::cerr << "error hobartcandev::sendData() unknown frame " << id << std::endl;
            break;
        }

        const size_t frameLen( messageInfo_[id].second );

        if ( bufferLen < frameLen )
        {
            std::cerr << "HCDev::sendData()1 Err::length mismatch frame " << id << std::endl;
            std::cerr << "HCDev::sendData()2 Err::bufferLen " << bufferLen << std::endl;
            std::cerr << "HCDev::sendData()3 Err::frameLen " << frameLen << std::endl;
            break;
        }
        else
        {
            if( DISPLAY_DEBUG )
            {
                std::cerr << "HCDev::sendData() frame " << framectr << " bufferLen " << bufferLen << std::endl;
                std::cerr << "HCDev::sendData() frame " << framectr << " frameLen " << frameLen << std::endl;
            }
        }

        std::cout << "HCDev::sendData() frame " << framectr << " send message " << std::uppercase << std::setfill('0') << std::setw( 4 ) << std::hex << id << std::dec <<
           " length " << frameLen << std::endl;

        can_frame frame;
        memset( &frame, 0, sizeof(frame) );

        frame.can_id = id++;
        frame.can_dlc = (unsigned char) frameLen;
        memcpy( &frame.data, buffer, frameLen );

        std::cerr << "HCDev::sendData() frame " << framectr << " frame.can_id: " << frame.can_id << std::endl;

        frames.push_back( frame );

        buffer = &buffer[frameLen];
        bufferLen -= frameLen;
    }

    std::cout << "\nHCDev::sendData() CALL canDev::write() " << std::endl;

    // send
    return write( frames );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hobartCanDev::onReadyRead()
{
    // read all frames
    const QList<can_frame> frames( readAll() );

    // enumerate frames
    foreach ( const can_frame &frame, frames )
    {
        const unsigned int id( frame.can_id );

        if ( !messageInfo_.contains( id ) )
        {
            if( id >= 1056 && id <= 1133 )
                std::cerr << "error HCDev::onReadyRead() unknown frame " << id << std::endl;
            continue;
        }

        const size_t frameLen( messageInfo_[id].second );

        if ( frameLen != frame.can_dlc )
        {
            if( id >= 1056 && id <= 1133 )
                std::cerr << "error HCDev::onReadyRead() bad frame length " << id << std::endl;
            continue;
        }

        // save
        messages_[id] = QByteArray( (char*) frame.data, frameLen );

        // check for complete messages
        const rangeType range( messageInfo_[id].first );
        const unsigned int min( range.first );
        const unsigned int max( range.first + range.second );

        for ( unsigned int i( min ); i < max; ++i )
            if ( !messages_.contains( i ) )
                continue;

        // reconstruct message
        QByteArray a;            

        for ( unsigned int i( min ); i < max; ++i )
        {
            a.append( messages_[i] );
            messages_.remove( i );
        }

        if( id >= 1056 && id <= 1133 )
            std::cout << "RX STEP 3: HCDev::onReadyRead() read message id: " << std::uppercase << std::setfill('0') << std::setw( 4 ) << std::hex << id << std::dec << "  length: " << frameLen << std::endl;

        recvData( min, (unsigned char*) a.constData(), a.size() );
    }
}
