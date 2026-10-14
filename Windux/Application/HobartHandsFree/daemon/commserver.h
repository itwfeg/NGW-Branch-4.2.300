/**
 * @file commserver.h
 * HFPD Comm Server.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef COMMSERVER
#define COMMSERVER

#include <QObject>
#include <QTcpServer>

class hobartTagManager;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Comm Server.
class commServer : public QObject
{
    Q_OBJECT

    typedef commServer _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    commServer( QObject *parent = 0 );

    /// Destructor.
    virtual ~commServer();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if server listening for connections.
    /**
     * @return  @c true if listening, @c false otherwise
     */
    virtual bool isListening() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Close server.
    virtual void close();

    /// Disconnect all clients.
    virtual void disconnectAll();

    /// Listen for connections.
    /**
     * @param[in] address  address to listen on
     * @param[in] port  port to listen on
     * @return  @c true if listening, @c false otherwise
     */
    virtual bool listen( const QHostAddress& address = QHostAddress::Any, quint16 port = 0 );

private slots:

    /// Slot for accept error.
    void onAcceptError( QAbstractSocket::SocketError socketError );

    /// Slot for client destroyed.
    void onClientDestroyed( QObject *obj );

    /// Slot for new connection.
    void onNewConnection();

private:

    hobartTagManager *tagManager_;

    QTcpServer *server_;

    // not implemented
    commServer( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // COMMSERVER
