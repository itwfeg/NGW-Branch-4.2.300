/**
 * @file commserver.h
 * TCP server base class based on Qt.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup communicationsCsu
 * @{
 */

#ifndef COMMSERVER_H
#define COMMSERVER_H

#include <QObject>

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QNetworkSession>
#include <QtNetwork/QTcpSocket>

#include <QByteArray>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Communication server class.
/**
 * This base class opens a server socket and listens for connections and data.  Data is in raw
 * binary format.
 *
 * Derived classes must implement the read() function for message handling.  This function is
 * called when data arrives on the socket.
 *
 * Derived classes must implement the disconnect() function for when sockets disconnect.
 */
class commServer : public QObject
{
    Q_OBJECT

    typedef commServer _Myt;
    
public:

    /// Destructor.
    virtual ~commServer();

    /// Send message over client socket.
    /**
     * @param[in] data  data to send over the connection
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool write( const QByteArray& data ) const;

protected:

    /// Constructor.
    /**
     * @param[in] port  port to listen on
     */
    commServer( unsigned int port );

    /// Constructor.
    /**
     * @param[in] address  address to listen on
     * @param[in] port  port to listen on
     */
    commServer( const QHostAddress& address, unsigned int port );

    /// Handle message from client socket.
    /**
     * This function is called when data is read from the socket.  Callee must determine if data
     * is a valid message or not.  If it is, @c true must me returned and @p dataLen must be set to
     * the message length.
     *
     * @param[in] data  raw data from socket
     * @param[out] dataLen  length of message
     * @return  @c true if data is a valid message, @c false otherwise
     */
    virtual bool read( const QByteArray& data, unsigned int& dataLen ) = 0;

    /// Handle client socket disconnect.
    virtual void disconnect() = 0;

private Q_SLOTS:

    /// Slot called when session opened.
    void sessionOpened();

    /// Slot called to handle connection.
    void sessionConnect();

    /// Slot to read data from client socket.
    void clientRead();

    /// Slot to handle client socket error.
    /**
     * @param[in] socketError  socket error that occured.
     */
    void clientError( QAbstractSocket::SocketError socketError );

    /// Slot to handle client disconnect.
    void clientDisconnected();

private:

    enum {READ_ERROR_LIMIT = 3};

    QHostAddress address_;
    unsigned int port_;

    QByteArray buffer_;

    QTcpServer *tcpServer_;
    QNetworkSession *session_;

    QTcpSocket *client_;
    unsigned int readErrors_;

    // not implemented
    commServer( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // COMMSERVER_H
