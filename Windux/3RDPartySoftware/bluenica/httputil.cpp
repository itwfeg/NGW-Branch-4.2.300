/**
 * @file httputil.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "httputil.h"

#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>

#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////////////////
bool httpUtil::get( const std::string &host, unsigned short port, const std::string &msg, std::string &result )
{
    struct hostent *server;

    // resolve host
    server = gethostbyname( host.c_str() );

    if ( !server )
    {
        std::cerr << "Could not resolve host: " << host << std::endl;
        return false;
    }

    // format message
    char message[1024];
    int messageLen( snprintf( message, sizeof(message), "GET /%s HTTP/1.0\r\n\r\n", msg.c_str() ) );

    if ( messageLen <= 0 )
    {
        std::cerr << "Could not format message" << std::endl;
        return false;
    }

#ifdef _DEBUG_HTTP
    std::ofstream ofs( "/tmp/get.txt", std::ofstream::out );
    ofs << std::string( message, messageLen );
    ofs.close();
#endif

    // send/receive
    char resp[8192];
    size_t respLen;

    if ( 0 == (respLen = sendRecv( server, port, message, messageLen, resp, sizeof(resp) )) )
        return false;

#ifdef _DEBUG_HTTP
    std::ofstream ofr( "/tmp/getresp.txt", std::ofstream::out );
    ofr << std::string( resp, respLen );
    ofr.close();
#endif

    // parse response
    headerEntryList header;

    return parse( std::string( resp, respLen ), header, result );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool httpUtil::post( const std::string &host, unsigned short port, const std::string &msg, const std::string &body, std::string &result )
{
    struct hostent *server;

    // resolve host
    server = gethostbyname( host.c_str() );

    if ( !server )
    {
        std::cerr << "Could not resolve host: " << host << std::endl;
        return false;
    }

    // format message
    char message[1024];
    int messageLen( snprintf( message, sizeof(message), "POST /%s HTTP/1.0\r\nContent-Type: application/octet-stream\r\nContent-Length: %d\r\n\r\n", msg.c_str(), body.length() ) );

    if ( messageLen <= 0 )
    {
        std::cerr << "Could not format message" << std::endl;
        return false;
    }

    std::string buffer;
    buffer.append( std::string( message, messageLen ) );
    buffer.append( body );

#ifdef _DEBUG_HTTP
    std::ofstream ofs( "/tmp/post.txt", std::ofstream::out );
    ofs << buffer;
    ofs.close();
#endif

    // send/receive
    char resp[8192];
    size_t respLen;

    if ( 0 == (respLen = sendRecv( server, port, buffer.c_str(), buffer.length(), resp, sizeof(resp) )) )
        return false;

#ifdef _DEBUG_HTTP
    std::ofstream ofr( "/tmp/postresp.txt", std::ofstream::out );
    ofr << std::string( resp, respLen );
    ofr.close();
#endif

    // parse response
    headerEntryList header;

    return parse( std::string( resp, respLen ), header, result );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t httpUtil::sendRecv( struct hostent *server, unsigned short port, const char *msg, size_t msgLen, char *resp, size_t respLen )
{
    size_t total;

    size_t sent( 0 );
    size_t received( 0 );

    // setup socket address
    struct sockaddr_in serv_addr;

    memset( &serv_addr, 0, sizeof(serv_addr) );
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons( port );
    memcpy( &serv_addr.sin_addr.s_addr, server->h_addr, server->h_length );

    // create the socket
    int sockfd( socket( AF_INET, SOCK_STREAM, 0 ) );

    if ( sockfd < 0 )
    {
        std::cerr << "Error opening socket" << std::endl;
        goto done;
    }

    // connect
    else if ( connect( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0 )
    {
        std::cerr << "Error connecting to host" << std::endl;
        goto done;
    }

    // send the request
    total = msgLen;

    do
    {
        ssize_t bytes( write( sockfd, &msg[sent], total - sent ) );

        if ( !bytes )
            break;
        else if ( bytes < 0 )
        {
            std::cerr << "Error writing message to socket: " << bytes << std::endl;
            goto done;
        }

        sent += bytes;

    } while ( sent < total );

    // receive the response
    total = respLen;

    do
    {
        ssize_t bytes( read( sockfd, &resp[received], total - received ) );

        if ( !bytes )
            break;
        else if ( bytes < 0 )
        {
            std::cerr << "Error reading response from socket: " << bytes << std::endl;

            received = 0;
            goto done;
        }

        received += bytes;

    } while ( received < total );

    if ( received == total )
    {
        std::cerr << "Error storing complete response from socket" << std::endl;

        received = 0;
        goto done;
    }

done:

    // close socket
    if ( 0 <= sockfd )
        close( sockfd );

    return received;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool httpUtil::parse( const std::string &msg, headerEntryList &header, std::string &data )
{
    static const char delim( '\n' );

    // validate message... we need to find two delim's in a row
    unsigned int delims( 0 );
    unsigned int dataPtr( 0 );

    for ( size_t i( 0 ); i < msg.length(); ++i )
    {
        if ( '\r' == msg[i] )
            ; // ignore cr's
        else if ( delim != msg[i] )
            delims = 0;
        else if ( 2 == ++delims )
        {
            dataPtr = i + 1;
            break;
        }
    }

    if ( !dataPtr )
        return false;

    // copy data
    if ( dataPtr < msg.length() )
        data = msg.substr( dataPtr );

    // parse header
    std::list<std::string> lines;

    const char *ptr( msg.c_str() );
    const char *s( ptr );

    for ( unsigned int i( 0 ); i < dataPtr; ++i )
        if ( delim == ptr[i] )
        {
            lines.push_back( std::string( s, &ptr[i] - s ) );
            s = &ptr[i + 1];
        }

    // process header
    std::string result( lines.front() );
    size_t pos;

    if ( std::string::npos == (pos = result.find( "200 OK" )) )
        return false;
    else
    {
        lines.pop_front();
    }

    for ( std::list<std::string>::iterator i( lines.begin() ); i != lines.end(); ++i )
        if ( std::string::npos != (pos = i->find( ": " )) )
        {
            std::string key( i->c_str(), pos );
            std::string value;

            if ( (pos + 2) < i->length() )
                value = i->substr( pos + 2 );

            header.push_back( headerEntry( key, value ) );

            // validate content length
            if ( std::string::npos != (pos = i->find( "Content-Length" )) )
                if ( data.length() != (size_t) atoi( value.c_str() ) )
                {
                    std::cerr << "Content length mismatch" << std::endl;
                    return false;
                }
        }

    return true;
}

