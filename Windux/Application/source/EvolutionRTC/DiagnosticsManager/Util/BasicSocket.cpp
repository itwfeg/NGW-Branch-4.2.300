/**
 * @file BasicSocket.cpp
 * @brief Basic socket wrapper class.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "BasicSocket.h"

#include <cstring>

#if defined( _MSC_VER )
#include <ws2tcpip.h>
#elif defined( __GNUC__ )
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

/// Util objects namespace.
namespace util
{

///////////////////////////////////////////////////////////////////////////////////////////////////
BasicSocket::BasicSocket( int family, int type, int protocol )
{
	fd_ = socket( family, type, protocol );

#if defined( _MSC_VER )
	blocking_ = true;
#elif defined( __GNUC__ )
	const int flags( fcntl( fd_, F_GETFL, 0 ) );

	blocking_ = ((O_NONBLOCK & flags) ? false : true);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BasicSocket::~BasicSocket()
{
	close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool BasicSocket::setBlocking( bool value )
{
#if defined( _MSC_VER )
    unsigned long mode( value ? 0 : 1 );

    if ( 0 != ioctlsocket( fd_, FIONBIO, &mode ) )
    	return false;
#elif defined( __GNUC__ )
    int flags( fcntl( fd_, F_GETFL, 0 ) );

    if ( flags < 0 )
    	return false;

    if ( value )
    	flags &= ~O_NONBLOCK;
    else
    	flags |= O_NONBLOCK;

    if ( 0 != fcntl( fd_, F_SETFL, flags ) )
    	return false;
#endif

    blocking_ = value;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool BasicSocket::bind( unsigned short family, unsigned long addr, unsigned short port )
{
    struct sockaddr_in servaddr;
    std::memset( &servaddr, 0, sizeof(servaddr) );
    servaddr.sin_family = family;
    servaddr.sin_addr.s_addr = htonl( addr );
    servaddr.sin_port = htons( port );

    // bind socket to port
    return ( 0 == ::bind( fd_, (sockaddr*) &servaddr, sizeof(servaddr) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool BasicSocket::connect( unsigned short family, const std::string &host, unsigned short port )
{
    struct sockaddr_in servaddr;
    std::memset( &servaddr, 0, sizeof(servaddr) );
    servaddr.sin_family = family;
    servaddr.sin_port = htons( port );

#if defined( _MSC_VER )
    InetPtonA( family, host.c_str(), &servaddr.sin_addr );
#elif defined( __GNUC__ )
    inet_pton( family, host.c_str(), &servaddr.sin_addr );
#endif

    // connect
    return ( 0 == ::connect( fd_, (sockaddr*) &servaddr, sizeof(servaddr) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool BasicSocket::send( const std::string &buffer, int flags )
{
	const int sendlen( ::send( fd_, buffer.c_str(), (int) buffer.size(), flags ) );

	if ( sendlen <= 0 )
		return false;

	return ( buffer.size() == (size_t) sendlen );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int BasicSocket::recv( void *buffer, unsigned int len, int flags )
{
    return ::recv( fd_, (char*) buffer, len, flags );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BasicSocket::close()
{
#if defined( _MSC_VER )
	closesocket( fd_ );
#elif defined( __GNUC__ )
	::close( fd_ );
#endif

	fd_ = INVALID_SOCKET;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace
