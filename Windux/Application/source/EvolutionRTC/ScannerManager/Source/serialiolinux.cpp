/**
 * @file serialiolinux.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "serialiolinux.h"

#ifdef __linux__

#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <termios.h>
#include <unistd.h>

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
void *_SerialCommThreadFn( void *param )
{
    serialComm *obj( reinterpret_cast<serialComm*>( param ) );

    obj->threadStarted_ = true;
    obj->run();
    obj->threadStarted_ = false;

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serialComm::serialComm() :
    comm_( -1 ),
    thread_( pthread_self() ),
    threadStarted_( false ),
    threadTerminated_( false )
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_NORMAL );

    pthread_mutex_init( &m_, &attr );

    pthread_mutexattr_destroy( &attr );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serialComm::~serialComm()
{
    // close device
    close();

    pthread_mutex_destroy( &m_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::isValid() const
{
    return (0 <= comm_);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serialComm::close()
{
    stop();

    ::close( comm_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::open( const std::string &portName, unsigned int baudRate, unsigned char parity, unsigned char /*stopBits*/, unsigned char /*byteSize*/ )
{
    // already running
    if ( isValid() )
        return true;

    // reset events
    threadStarted_ = false;
    threadTerminated_ = false;

    // open COM port
    if ( (comm_ = ::open( portName.c_str(), O_RDWR | O_NOCTTY | O_SYNC )) < 0 )
    {
        std::cerr << "failed to create COM port!" << std::endl;
        goto error;
    }

    // set interface attributes
    if ( !setInterfaceAttrib( comm_, baudRate, parity ) )
    {
        std::cerr << "failed to set blocking!" << std::endl;
        goto error;
    }

    // set blocking
    if ( !setBlocking( comm_, false ) )
    {
        std::cerr << "failed to set blocking!" << std::endl;
        goto error;
    }

    // go!
    if ( start() )
        return true;

error:

    ::close( comm_ );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string serialComm::read()
{
    std::string result;

    // grab buffered data
    if ( 0 == pthread_mutex_lock( &m_ ) )
    {
        result = buffer_;
        buffer_.clear();

        pthread_mutex_unlock( &m_ );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::write( const std::string &buffer )
{
    bool result( false );

    // make sure valid
    if ( isValid() )
    {
        const ssize_t bytesWritten( ::write( comm_, buffer.c_str(), buffer.length() ) );

        // validate written
        result = ((ssize_t) buffer.length() == bytesWritten);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::isRunning() const
{
    return threadStarted_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serialComm::run()
{
    do
    {
        // read
        char buffer[1024];
        ssize_t bytesRead( ::read( comm_, buffer, sizeof(buffer) ) );

        // have data
        if ( bytesRead )
            if ( 0 == pthread_mutex_lock( &m_ ) )
            {
                buffer_.append( buffer, bytesRead );

                pthread_mutex_unlock( &m_ );
            }

        //usleep( SLEEP_TIME );

    } while ( !threadTerminated_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::start()
{
    bool result( false );

    // already running
    if ( isRunning() )
        return true;

    pthread_attr_t attr;
    pthread_attr_init( &attr );

    // create thread
    if ( 0 == pthread_create( &thread_, &attr, &_SerialCommThreadFn, this ) )
    {
        // wait for started
        do
        {
            usleep( SLEEP_TIME );

        } while ( !threadStarted_ );

        result = true;
    }

    pthread_attr_destroy( &attr );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serialComm::stop()
{
    // stop thread
    if ( isRunning() )
    {
        threadTerminated_ = true;

        pthread_join( thread_, NULL );
    }

    thread_ = pthread_self();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::setInterfaceAttrib( int fd, unsigned int baudRate, unsigned char parity )
{
    struct termios tty;
    memset( &tty, 0, sizeof(tty) );

    // get attributes
    if ( 0 != tcgetattr( fd, &tty ) )
    {
        std::cerr << "failed to get com attributes " << errno << std::endl;
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
        std::cerr << "invalid baud rate " << baudRate << std::endl;
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
        std::cerr << "error setting comm attributes " << errno << std::endl;
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serialComm::setBlocking( int fd, bool shouldBlock )
{
    struct termios tty;
    memset( &tty, 0, sizeof(tty) );

    // get attributes
    if ( 0 != tcgetattr( fd, &tty ) )
    {
        std::cerr << "failed to get com attributes " << errno << std::endl;
        return false;
    }

    // set blocking and read timeout
    tty.c_cc[VMIN] = (shouldBlock ? 1 : 0);
    tty.c_cc[VTIME] = 5;                        // 0.5 seconds read timeout

    // set attributes
    if ( 0 != tcsetattr( fd, TCSANOW, &tty ) )
    {
        std::cerr << "error setting comm attributes " << errno << std::endl;
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // __linux__
