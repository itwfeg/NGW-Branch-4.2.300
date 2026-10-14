/**
 * @file gcdcommserver.h
 * GCD backend based on a TCP server handling GCD messages.
 *
 * This is a GCD backend used for controlling the GCD frontend.  The backend and frontend are tied
 * together using the Qt signal/slot paradigm.
 *
 * The @c gcdCommServer mostly handle GCD messages however in some configurations device messages
 * will also be receieved.  After messages are parsed the @c updateUserInterface() or
 * @c controlMessage() signal will emitted depending upon message type.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup metrologyVideosCsu
 * @{
 */

#ifndef GCDCOMMSERVER_H
#define GCDCOMMSERVER_H

#include <commserver.h>
#include <msgdispatcher.h>

#include <devicemsgfactory.h>
#include <gcdmsgfactory.h>

#include <process.h>

#include "gcdsignal.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// GCD Communication server class.
/**
 * This class will open up a server on the GCD port and handle gcd and device messages that
 * arrive.  UI oriented messages will be emitted to the front end.
 */
class gcdCommServer : public commServer, public msgDispatcher<deviceMsgFactory>, public msgDispatcher<gcdMsgFactory>
{
    Q_OBJECT

    typedef gcdCommServer _Myt;

    typedef commServer _Mybase;

    typedef msgDispatcher<deviceMsgFactory> _DeviceMsgDispatcher;
    typedef msgDispatcher<gcdMsgFactory> _GcdMsgDispatcher;

Q_SIGNALS:

    /// Update user interface.
    /**
     * Signal issued to update graphical fields.
     *
     * @param[in] type  ui field type
     * @param[in] value  field value
     */
    void updateUserInterface( gcdUpdateUserInterfaceType::type type, const QString& value );

    /// Control message.
    /**
     * Signal issued to front end in order to perform functionality (i.e. parameter changes,
     * special commands).
     *
     * @param[in] type  control message type
     * @param[in] value  control message data
     */
    void controlMessage( gcdControlMessageType::type type, const QString& value );

public:

    /// This is the default GCD port.
    enum {PORT = 27015};

    /// Constructor.
    /**
     * @param[in] port  tcp port to open the gcd server on
     */
    gcdCommServer( unsigned int port = PORT );

    /// Destructor.
    virtual ~gcdCommServer();

public Q_SLOTS:

    /// Set status information.
    /**
     * Slot that receives status updates from the gcd frontend.
     *
     * @param[in] versionMajor  major version of the gcd application
     * @param[in] versionMinor  minor version of the gcd application
     * @param[in] language  language the gcd application is currently using
     * @param[in] weighMode  weigh mode the gcd application is currently in
     */
    void onUpdateStatus(
        unsigned int versionMajor,
        unsigned int versionMinor,
        const QString& language,
        const QString& weighMode );

    /// Set playlist error.
    /**
     * Slot that receives playlist error from gcd frontend and sends event to backend.
     *
     * @param[in] file  file in error
     * @param[in] errorDescription  description of error
     */
    void onPlaylistError(
        const QString& file,
        const QString& errorDescription );

protected:

    /// Handle message from client socket.
    /**
     * Override of @c commServer abstract function.
     *
     * This function is called when data is read from the socket.  Callee must determine if data
     * is a valid message or not.  If it is, @c true must me returned and @p dataLen must be set to
     * the message length.
     *
     * @param[in] data  raw data from socket
     * @param[out] dataLen  length of message
     * @return  @c true if data is a valid message, @c false otherwise
     */
    virtual bool read( const QByteArray& data, unsigned int& dataLen );

    /// Send message over client socket.
    /**
     * Override of @c commServer function.
     *
     * Serializes message and sends raw data over socket.
     *
     * @param[in] m  message to send over the connection
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool write( const msgResponse& m ) const;

    /// Handle client socket disconnect.
    virtual void disconnect();

    /// Message handler.
    /**
     * Override of @c msgDispatcher<deviceMsgFactory> abstract function.
     *
     * This function should be subclassed and implemented to handle incoming messages.  The
     * message @p m will be deleted automatically by the dispatcher.
     *
     * @param[in] m  message to handle
     */
    virtual void handleMsgRequest( const deviceMsg *m );

    /// Message handler.
    /**
     * Override of @c msgDispatcher<gcdMsgFactory> abstract function.
     *
     * This function should be subclassed and implemented to handle incoming messages.  The
     * message @p m will be deleted automatically by the dispatcher.
     *
     * @param[in] m  message to handle
     */
    virtual void handleMsgRequest( const gcdMsg *m );

private:

    unsigned int versionMajor_;
    unsigned int versionMinor_;

    QString language_;
    QString weighMode_;

    QString storeMarquee_;

    // not implemented
    gcdCommServer( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /// Implementation of specific messages.
    template <class T>
    void handleMsgRequestT( const T *m );

    /// Strip old version qualifiers from filename.
    static QString stripPreviousVersionQualifiers( const QString& filename );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GCDCOMMSERVER_H
