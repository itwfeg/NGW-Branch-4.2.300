/**
 * @file usbdev.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "candev.h"
#include "cansocket.h"

#include <iomanip>
#include <iostream>

#include <QtGlobal>

///////////////////////////////////////////////////////////////////////////////////////////////////
canDev::canDev( canSocket *s ) :
    _Mybase(),
    s_( s )
{
    // connect slots
    if ( !s_ )
        std::cerr << "bad socket" << std::endl;
    else
    {
        connect( s_, SIGNAL(destroyed(QObject*)), SLOT(onDeviceDestroyed(QObject*)) );
        connect( s_, SIGNAL(recvFrame(const can_frame*)), SLOT(onRecvFrame(const can_frame*)), Qt::DirectConnection );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
canDev::~canDev()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void canDev::addDeviceRange( const rangeType &value )
{
    //const unsigned int max( value.first + value.second );

    QMutexLocker guard( &m_ );
    deviceRanges_.append( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void canDev::addDeviceRange( const rangeTypeList &value )
{
    foreach ( const rangeType &range, value )
        addDeviceRange( range );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
canDev::rangeTypeList canDev::deviceRanges() const
{
    QMutexLocker guard( &m_ );
    return deviceRanges_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t canDev::framesAvailable() const
{
    QMutexLocker guard( &m_ );

    return buffer_.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
can_frame canDev::peek() const
{
    can_frame frame;

    QMutexLocker guard( &m_ );

    if ( buffer_.size() )
        frame = buffer_.first();

    return frame;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<can_frame> canDev::peek( size_t maxSize ) const
{
    QList<can_frame> frames;

    QMutexLocker guard( &m_ );

    maxSize = qMin( maxSize, (size_t) buffer_.size() );

    for ( size_t i( 0 ); i < maxSize; ++i )
        frames.push_back( buffer_[i] );

    return frames;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
can_frame canDev::read()
{

    std::cout << "read() " << std::endl;
    can_frame frame;

    QMutexLocker guard( &m_ );

    if ( buffer_.size() )
    {
        frame = buffer_.first();
        buffer_.pop_front();
    }

    return frame;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<can_frame> canDev::read( size_t maxSize )
{
    QList<can_frame> frames;

    QMutexLocker guard( &m_ );

    maxSize = qMin( maxSize, (size_t) buffer_.size() );

    while ( maxSize-- )
    {
        frames.push_back( buffer_.first() );
        buffer_.pop_front();
    }

    return frames;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<can_frame> canDev::readAll()
{
    QMutexLocker guard( &m_ );

    const QList<can_frame> frames( buffer_ );

    buffer_.clear();

    return frames;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool canDev::write( const can_frame &frame )
{
    QMutexLocker guard( &m_ );

    return ( s_ ? s_->sendFrame( frame ) : false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool canDev::write( const QList<can_frame> &frames )
{
    QMutexLocker guard( &m_ );

    return ( s_ ? s_->sendFrames( frames ) : false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool canDev::waitForReadyRead( int msecs )
{
    do
    {
        if ( framesAvailable() )
            return true;

        QThread::msleep( 50 );
        msecs -= 50;

    } while ( 0 < msecs );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void canDev::onDeviceDestroyed( QObject *d )
{
    QMutexLocker guard( &m_ );

    if ( d == s_ )
        s_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void canDev::onRecvFrame( const can_frame *frame )
{
    QMutexLocker guard( &m_ );

    if ( !validateRange( frame->can_id, deviceRanges_ ) )
    {
        std::cout << "RX STEP 2 error - onRecvFrame() frame not in device range " << std::uppercase << std::setfill('0') << std::setw( 4 ) << std::hex << (unsigned int) frame->can_id << std::dec << std::endl;
    }
    else
    {
        //unsigned int myid = frame->can_id;
        //if( myid >= 1056 && myid <= 1133 )
        //    std::cout << "RX STEP 2 candev::onRecvFrame() success. " << std::uppercase << std::setfill('0') << std::setw( 4 ) << std::hex << (unsigned int) frame->can_id << std::dec << std::endl;

        buffer_.push_back( *frame );

        Q_EMIT readyRead();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//void canDev::onDoNothing()
//{
//    std::cout << "\nRX STEP 2 candev::onDoNothing()" << std::endl;
//}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool canDev::validateRange( unsigned int id, const rangeType &range )
{
    const unsigned int max( range.first + range.second );

    return (( range.first <= id ) && ( id < max ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool canDev::validateRange( unsigned int id, const rangeTypeList &ranges )
{
    // enumerate ranges
    foreach ( const rangeType &range, ranges )
        if ( validateRange( id, range ) )
            return true;

    return false;
}
