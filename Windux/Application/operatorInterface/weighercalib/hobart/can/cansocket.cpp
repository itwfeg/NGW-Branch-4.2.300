/**
 * @file cansocket.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "cansocket.h"

#include <iomanip>
#include <iostream>

#include <errno.h>
#include <unistd.h>

#include <net/if.h>

#include <linux/can/raw.h>

#include <sys/ioctl.h>

#include <QCoreApplication>

///////////////////////////////////////////////////////////////////////////////////////////////////
canSocket::canSocket( const QString &iface, QObject *parent ) :
    _Mybase( parent ),
    terminate_( false )
{
    // create socket
    if ( 0 <= (fd_ = socket( PF_CAN, SOCK_RAW, CAN_RAW )) )
    {
         struct ifreq ifr;

        strcpy( ifr.ifr_name, iface.toLatin1().constData() );
        ioctl( fd_, SIOCGIFINDEX, &ifr );

        addr_.can_family = AF_CAN;
        addr_.can_ifindex = ifr.ifr_ifindex;

        // bind socket and start event loop
        if ( 0 == bind( fd_, (sockaddr*) &addr_, sizeof(addr_) ) )
        {
            start();
        }
        else
        {
            std::cerr << "error binding to CAN " << iface.toStdString() << ": " << errno << std::endl;

            close( fd_ );
            fd_ = -1;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
canSocket::~canSocket()
{
    std::cout << "Destructor() set terminate = true" << std::endl;

    // stop event loop
    terminate_ = true;

    do
    {
        msleep( 25 );

    } while ( isRunning() );

    // close file descriptor
    if ( 0 <= fd_ )
    {
        close( fd_ );
        fd_ = -1;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool canSocket::sendFrame( const can_frame &frame )
{
    std::cout << "\ncansocket::sendFrame() can socket write frame " << std::uppercase  << std::setfill('0') << std::setw( 4 ) << std::hex << (unsigned int) frame.can_id << std::dec <<
        " length " << (unsigned int) frame.can_dlc << std::endl;

    const size_t n( write( fd_, &frame, sizeof(can_frame) ) );          // JHC - FAILS HERE

    if ( n != sizeof(can_frame) )
    {
        std::cerr << "canSocket::sendFrame() frame.can_id: " << frame.can_id << std::endl;
        std::cerr << "canSocket::sendFrame() frame.data: " << frame.data << std::endl;
        std::cerr << "canSocket()::sendFrame() ERROR sizeof n: " << n << std::endl;
        std::cerr << "canSocket()::sendFrame() ERROR sizeof(can_frame): " << sizeof(can_frame) << std::endl;
        std::cerr << "canSocket()::sendFrame() ERROR error writing to CAN: " << errno << std::endl;
        return false;
    }
    else
    {
        std::cerr << "canSocket::sendFrame() frame.can_id: " << frame.can_id << std::endl;
        std::cerr << "canSocket::sendFrame() frame.data: " << frame.data << std::endl;
        std::cout << "canSocket()::sendFrame() GOOD sizeof n: " << n << std::endl;
        std::cout << "canSocket()::sendFrame() GOOD sizeof(can_frame): " << sizeof(can_frame) << std::endl;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool canSocket::sendFrames( const QList<can_frame> &frames )
{
    std::cerr << "canSocket::sendFrames() GET READY TO LOOP to send QList of frames: " << std::endl;

    int ctr = 1;
    foreach ( const can_frame &frame, frames )
        if ( !sendFrame( frame ) )
            return false;
        else
            std::cerr << "canSocket::sendFrames() LOOP " << ctr++ << " frame frame.can_id: " << frame.can_id << std::endl;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void canSocket::run()
{
    // 50ms timeout value
    // struct timeval tv;
    // tv.tv_sec = 60 * 5;      // 2 min second timeout - This was 0
    // tv.tv_usec = 0;          // milli - was 50000

    fd_set fds;

    FD_ZERO( &fds );
    FD_SET( fd_, &fds );

    do
    {
        // wait for read

        FD_ZERO( &fds );
        FD_SET( fd_, &fds );        // needs reset each time through the loop

        int rc;
        rc = select( fd_+1, &fds, NULL, NULL, NULL );           // (rc = select( fd_+1, &fds, NULL, NULL, &tv )) < 0

        if ( rc < 0 )
        {
            std::cerr << "\nRX STEP 1: error during CAN select: " << errno << std::endl;
            break;
        }
        else if ( rc > 0 )
        {
            if ( FD_ISSET( fd_, &fds ) )
            {
                struct can_frame frame;

                const size_t n( read( fd_, &frame, sizeof(can_frame) ) );

                if ( n != sizeof(can_frame) )
                {

                    std::cerr << "RX STEP 1: In run() error writing to CAN: " << errno << std::endl;
                }
                else
                {
                    //std::cerr << "\nRX STEP 1: In cansocket::run() EMIT recvFrame() " << std::uppercase << std::setfill('0') << std::setw( 4 ) << std::hex << (unsigned int) frame.can_id << std::dec << " length " << (unsigned int) frame.can_dlc << std::endl;
                    Q_EMIT recvFrame( &frame );
                }

                FD_CLR( fd_, &fds );
            }
            else
                std::cerr << "RX STEP1: else FD_ISSET( fd_, &fds ) error writing to CAN: " << errno << std::endl;
        }
        //else if( 0 == rc)
        //{
        //    // we are getting stuck in here and not coming out
        //    std::cerr << "\nRX STEP1: 2 minute timeout: " << std::endl;
        //}

        // handle qt events
        QCoreApplication::processEvents();

    } while ( !terminate_ );


    std::cout << "exit can socket event loop" << std::endl;
}
