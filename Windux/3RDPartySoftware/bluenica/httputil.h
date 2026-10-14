/**
 * @file httputil.h
 * HTTP utility methods.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HTTPUTIL
#define HTTPUTIL

#include <list>
#include <map>
#include <string>

struct hostent;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HTTP utility methods.
class httpUtil
{
    typedef httpUtil _Myt;

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Perform HTTP GET to host.
    /**
     * @param[in] host  hostname
     * @param[in] port  port
     * @param[in] msg  message
     * @param[out] result  result
     * @return  @c true if successful, @c false otherwise
     */
    static bool get( const std::string &host, unsigned short port, const std::string &msg, std::string &result );

    /// Perform HTTP POST to host.
    /**
     * @param[in] host  hostname
     * @param[in] port  port
     * @param[in] msg  message
     * @param[in] body  body
     * @param[out] result  result
     * @return  @c true if successful, @c false otherwise
     */
    static bool post( const std::string &host, unsigned short port, const std::string &msg, const std::string &body, std::string &result );

private:

    typedef std::pair<std::string, std::string> headerEntry;
    typedef std::list<headerEntry> headerEntryList;

    // not implemented
    httpUtil();

    // not implemented
    _Myt& operator = ( const _Myt &rhs );

    /// Send and receive data.
    static size_t sendRecv( struct hostent *server, unsigned short port, const char *msg, size_t msgLen, char *resp, size_t respLen );

    /// Parse message with header.
    static bool parse( const std::string &msg, headerEntryList &header, std::string &data );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HTTPUTIL

