/**
 * @file msgdispatcher.h
 * Template class that uses factory methods to parse and dispatch a message request.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef MSGDISPATCHER_H
#define MSGDISPATCHER_H

#include "msg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Message dispatcher class.
/**
 * This class is intended to be derived from.  When you recieve raw data you should call the
 * @c parse() method.  If that raw data is parsable a message will created and dispatched to the
 * subclassed @c handleMsgRequest() function.
 *
 * For example, a class that writes messages to the screen may look like:
 * @code
 *   class myMessageWriter : public msgDispatcher<myMessageFactory> {
 *   public:
 *       void onGotData( const QByteArray& data ) {
 *           unsigned int dataLen;
 *           parse( data, dataLen );
 *       }
 *   protected:
 *       void handleMsgRequest( const msgType *m ) {
 *           std::cout << (*m) << std::endl;
 *       }
 *   };
 * @endcode
 *
 * @tparam _Factory  factory class that parses raw data
 */
template <class _Factory>
class msgDispatcher
{
    typedef msgDispatcher<_Factory> _Myt;

public:

    /// Message type factory dispatches.
    typedef typename _Factory::msgType msgType;

    /// Destructor
    virtual ~msgDispatcher() {}

    /// Parse and dispatch message.
    /**
     * Attempts to parse raw data within @p data.  If data contains a valid message, @p dataLen
     * will be set to the message length and @c true will be returned.
     *
     * Valid messages will be dispatched to the subclassed @c handleMsgRequest() function.
     *
     * @param[in] data  raw data from socket
     * @param[out] dataLen  length of message
     * @return  @c true if data is a valid message, @c false otherwise
     */
    virtual bool parse( const QByteArray& data, unsigned int& dataLen )
    {
        msgRequest *m = _Factory::allocMsgRequest( data, dataLen );

        if ( 0 < dataLen )
        {
            if ( m )
            {
                msgType *temp = dynamic_cast<msgType*>( m );

                if ( temp )
                    handleMsgRequest( temp );

                _Factory::freeMsgRequest( m );
            }

            return true;
        }

        return false;
    }

protected:

    /// Constructor.
    msgDispatcher() {}

    /// Message handler.
    /**
     * This function should be subclassed and implemented to handle incoming messages.  The
     * message @p m will be deleted automatically by the dispatcher.
     *
     * @param[in] m  message to handle
     */
    virtual void handleMsgRequest( const msgType *m ) = 0;

private:

    // not implemented
    msgDispatcher( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MSGDISPATCHER_H
