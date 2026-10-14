/**
 * @file BasicSocket.h
 * @brief Basic socket wrapper class.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef BASICSOCKET_H
#define BASICSOCKET_H

#if defined( _MSC_VER )
#include <winsock2.h>
#endif

#include <string>

/// Util objects namespace.
namespace util
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Basic socket wrapper class.
class BasicSocket
{
	typedef BasicSocket _Myt;

public:

#if defined( _MSC_VER )
	typedef SOCKET fd_type;							///< File descriptor type.
#elif defined( __GNUC__ )
	typedef int fd_type;							///< File descriptor type.
#endif

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] family  family
	 * @param[in] type  type
	 * @param[in] protocol  protocol
	 */
	BasicSocket( int family, int type, int protocol );

	/// Destructor.
	virtual ~BasicSocket();

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve blocking.
	/**
	 * @return  @c true if blocking, @c false otherwise
	 */
	virtual bool isBlocking() const {return blocking_;}

	/// Set blocking.
	/**
	 * @param[in] value  @c true if blocking, @c false otherwise
	 * @return  @c true if set successfully, @c false otherwise
	 */
	virtual bool setBlocking( bool value );

	/// Retrieve file descriptor.
    /**
     * @return  file descriptor
     */
	virtual fd_type fd() const {return fd_;}

	/// Check socket valid.
	/**
	 * @return  @c true if valid, @c false otherwise
	 */
	virtual bool isValid() const {return (INVALID_SOCKET != fd_);}

	// ========================================================================
	// Methods
	// ========================================================================

	/// Bind socket.
	/**
	 * @param[in] family  family
	 * @param[in] addr  address
	 * @param[in] port  port
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool bind( unsigned short family, unsigned long addr, unsigned short port );

	/// Connect socket.
	/**
	 * @param[in] family  family
	 * @param[in] host  host name
	 * @param[in] port  port
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool connect( unsigned short family, const std::string &host, unsigned short port );

	/// Send data.
	/**
	 * @param[in] buffer  buffer to send
	 * @param[in] flags  flags
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool send( const std::string &buffer, int flags = 0 );

	/// Receive data.
	/**
	 * @param[out] buffer  buffer for data
	 * @param[in] len  buffer length
	 * @param[in] flags  flags
	 * @return  amount of data read, or negative value upon failure
	 */
	virtual int recv( void *buffer, unsigned int len, int flags = 0 );

	/// Close socket.
	virtual void close();

protected:

#if defined( __GNUC__ )
	static const fd_type INVALID_SOCKET = -1;		///< Invalid socket.
#endif

	fd_type fd_;									///< Socket.

	bool blocking_;									///< Blocking flag.

private:

	// not implemented
	BasicSocket( const _Myt& );

	// not implemented
	_Myt & operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // BASICSOCKET_H
