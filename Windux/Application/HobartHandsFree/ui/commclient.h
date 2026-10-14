/**
 * @file commclient.h
 * HFPD Comm Client.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef COMMCLIENT
#define COMMCLIENT

#include <QByteArray>
#include <QMutex>
#include <QObject>
#include <QTcpSocket>

class message;

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Comm Client.
class commClient : public QObject
{
    Q_OBJECT

    typedef commClient _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve error.
    /**
     * @return  error
     */
    QAbstractSocket::SocketError error() const;

    /// Retrieve error string.
    /**
     * @return  error string.
     */
    QString errorString() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Connect to host.
    /**
     * @param[in] hostName  host name
     * @param[in] port  port
     */
    void connectToHost( const QString& hostName, quint16 port );

    /// Send message.
    /**
     * @param[in] msg  pointer to message
     */
    void sendMessage( const message *msg ) const;

    /// Wait for connection.
    /**
     * @param[in] msecs  how long to wait
     * @return  @c true if connected, @c false otherwise
     */
    bool waitForConnected( int msecs );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve singleton instance.
    /**
     * @return  instance
     */
    static _Myt *instance();

    /// Destroy singleton instance.
    static void destroy();

Q_SIGNALS:

    /// Signal for socket connected.
    void connected();

    /// Signal for socket disconnected.
    void disconnected();

    /// Signal for socket error.
    /**
     * @param[in] socketError  error
     */
    void error( QAbstractSocket::SocketError socketError );

    /// Signal for receive message.
    /**
     * @param[in] msg  pointer to message
     */
    void recvMessage( const message* msg );

private Q_SLOTS:

    /// Slot for socket connected.
    void onConnected();

    /// Slot for socket disconnected.
    void onDisconnected();

    /// Slot for socket error.
    void onError( QAbstractSocket::SocketError socketError );

    /// Slot for socket ready read.
    void onReadyRead();

    /// Slot for socket state changed.
    void onStateChanged( QAbstractSocket::SocketState socketState );

    /// Slot for timeout.
    void onTimeout();

private:

    enum {KEEP_ALIVE_TIMEOUT = 5000};

    static QMutex instanceLock_;
    static _Myt *instance_;

    QTcpSocket *sock_;
    QByteArray a_;

    QTimer *keepAlive_;

    int timeout_;

    /// Constructor.
    commClient();

    /// Destructor.
    ~commClient();

    /// Process message.
    void processMessage( const message *msg );

    // not implemented
    commClient( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // COMMCLIENT
