/**
 * @file devicemsgfactory.h
 * Device message factory.
 *
 * Factory class for @c deviceMsg allocation and destruction.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef DEVICEMSGFACTORY_H
#define DEVICEMSGFACTORY_H

#include "devicemsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Device Message factory class.
/**
 * This class will parse raw data and allocate the appropriate @c deviceMsg class for that data.
 *
 * For example, assuming you have raw data already:
 * @code
 *   unsigned int rawDataLen;
 *   deviceMsg *m = deviceMsgFactory::allocMsgRequest( rawData, rawDataLen );
 *   if ( m ) {
 *       // do something with m
 *       deviceMsgFactory::freeMsgRequest( m );
 *   }
 * @endcode
 */
class deviceMsgFactory
{
    typedef deviceMsgFactory _Myt;

public:

    /// Message type this factory manages.
    typedef deviceMsg msgType;

    /// Allocate message from raw stream.
    /**
     * Attempts to parse raw data within @p data.  If data contains a valid message, @p dataLen
     * will be set to the message length and a pointer to a freshly allocated @c deviceMsg will
     * be returned to the caller.
     *
     * @param[in] data  raw data from socket
     * @param[out] dataLen  length of message
     * @return  pointer to @c deviceMsg if data is valid, @c NULL otherwise
     */
    static msgRequest *allocMsgRequest( const QByteArray& data, unsigned int& dataLen );

    /// Free a message allocated by this factory.
    /**
     * @param[in] m  message to free
     */
    static void freeMsgRequest( msgRequest *m );

private:

    // not implemented
    deviceMsgFactory();

    // not implemented
    deviceMsgFactory( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DEVICEMSGFACTORY_H
