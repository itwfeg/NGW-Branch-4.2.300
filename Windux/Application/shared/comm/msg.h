/**
 * @file msg.h
 * Base message classes for any message that is parsable and/or serializable.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef MSG_H
#define MSG_H

#include <QByteArray>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Base message class.
class msg
{
    typedef msg _Myt;

public:

    /// Destructor.
    virtual ~msg() {}

    /// Assignment operator.
    /**
     * @param[in] rhs  value to assign from
     * @return  reference to @c this
     */
    _Myt& operator = ( const _Myt& /*rhs*/ ) {return *this;}

protected:

    /// Constructor.
    msg() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    msg( const _Myt& /*rhs*/ ) {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Message request class.
/**
 * This class represents a received message and implements parsing functionality.
 */
class msgRequest : public msg
{
    typedef msgRequest _Myt;
    typedef msg _Mybase;

public:

    /// Parse message.
    /**
     * Attempts to parse raw data within @p data.  If data contains a valid message, @p dataLen
     * will be set to the message length and @c true will be returned.
     *
     * @param[in] data  raw data from socket
     * @param[out] dataLen  length of message
     * @return  @c true if data is a valid message, @c false otherwise
     */
    virtual bool parse( const QByteArray& data, unsigned int& dataLen ) = 0;

protected:

    /// Constructor.
    msgRequest() : _Mybase() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    msgRequest( const _Myt& rhs ) : msg( rhs ) {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Message response class.
/**
 * This class represents a sendable message and implements a serialization routine.
 */
class msgResponse : public msg
{
    typedef msgResponse _Myt;
    typedef msg _Mybase;

public:

    /// Serialize message into raw data suitable for sending.
    /**
     * @return  raw data that is sendable over a socket (or otherwise).
     */
    virtual QByteArray serialize() const = 0;

protected:

    /// Constructor.
    msgResponse() : _Mybase() {}

    /// Constructor.
    /**
     * @param[in] rhs  message to copy
     */
    msgResponse( const _Myt& rhs ) : msg( rhs ) {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MSG_H
