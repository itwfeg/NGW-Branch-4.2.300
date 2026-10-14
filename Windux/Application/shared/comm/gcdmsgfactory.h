/**
 * @file gcdmsgfactory.h
 * GCD message factory.
 *
 * Factory class for @c gcdMsg allocation and destruction.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef GCDMSGFACTORY_H
#define GCDMSGFACTORY_H

#include "gcdmsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD Message factory class.
/**
 * This class will parse raw data and allocate the appropriate @c gcdMsg class for that data.
 *
 * For example, assuming you have raw data already:
 * @code
 *   unsigned int rawDataLen;
 *   gcdMsg *m = gcdMsgFactory::allocMsgRequest( rawData, rawDataLen );
 *   if ( m ) {
 *       // do something with m
 *       gcdMsgFactory::freeMsgRequest( m );
 *   }
 * @endcode
 */
class gcdMsgFactory
{
    typedef gcdMsgFactory _Myt;

public:

    /// Message type this factory manages.
    typedef gcdMsg msgType;

    /// Allocate message from raw stream.
    /**
     * Attempts to parse raw data within @p data.  If data contains a valid message, @p dataLen
     * will be set to the message length and a pointer to a freshly allocated @c gcdMsg will
     * be returned to the caller.
     *
     * @param[in] data  raw data from socket
     * @param[out] dataLen  length of message
     * @return  pointer to @c gcdMsg if data is valid, @c NULL otherwise
     */
    static msgRequest *allocMsgRequest( const QByteArray& data, unsigned int& dataLen );

    /// Free a message allocated by this factory.
    /**
     * @param[in] m  message to free
     */
    static void freeMsgRequest( msgRequest *m );

private:

    // not implemented
    gcdMsgFactory();

    // not implemented
    gcdMsgFactory( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GCDMSGFACTORY_H
