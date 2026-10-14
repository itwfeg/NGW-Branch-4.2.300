/**
 * @file connection.h
 * @brief Socket connection object.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <messagecenterclient.h>

#include <QAbstractSocket>
#include <QByteArray>
#include <QMutex>

class EventAdapter;

class QTcpSocket;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Connection object.
class Connection : public MessageCenterClient
{
    Q_OBJECT

    using _Myt = Connection ;
    using _Mybase = MessageCenterClient;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] s  socket
     * @param[in,out] parent  parent object
     */
    Connection( QTcpSocket *s, QObject *parent = nullptr );

    /// Destructor.
    ~Connection();

public slots:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve connection id.
    /**
     * @return  id
     */
    QString id() const;

    /// Retrieve total bytes sent.
    /**
     * @return  bytes sent
     */
    qulonglong bytesSent() const;

    /// Retrieve total bytes received.
    /**
     * @return  bytes sent
     */
    qulonglong bytesReceived() const;

protected:

    QString id_;                                    ///< Connection identifier.

    qulonglong sent_;                               ///< Bytes sent.
    qulonglong recv_;                               ///< Bytes received.

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if session is persistent.
    /**
     * @return  @c true if peristent, @c false otherwise
     */
    bool isSessionPersistent() const;

    /// Set persistent session.
    /**
     * @param[in]  @c true for peristent, @c false otherwise
     */
    void setSessionPersistent( bool value );

    // ========================================================================
    // Events
    // ========================================================================

    /// Process event message signal.
    /**
     * @param[in] e  event
     * @param[in,out] destroy  set to @c true if event should be destroyed, @c false if ownership taken
     */
    virtual void onProcessEvent( const Event *e, bool& destroy ) override;

private slots:

    /// Slot for socket disconnected.
    void onDisconnected();

    /// Slot for socket error.
    void onErrorOccurred( QAbstractSocket::SocketError socketError );

    /// Slot for data ready.
    void onReadyRead();

    /// Slot for timer timeout.
    void onTimeout();

private:

    enum {PEEK_SIZE = 256};
    enum {PEEK_TIME = 15 * 1000}; // 15s

    enum {IDLE_TIME = 15 * 60 * 1000}; // 15m

    enum {MAX_ERRORS = 8};

    static QMutex m_;
    static uint nextConnection_;

    QTimer *peekTimer_;
    QTimer *idleTimer_;

    QTcpSocket *s_;

    uint sendErrors_;
    uint recvErrors_;

    QByteArray buffer_;

    EventAdapter *adapter_;

    uint sessionType_;
    QString sessionPath_;

    bool sessionPersistent_;

    /// Create adapter based on data in buffer.
    void createAdapter();

    /// Create session based on adapter and data in buffer.
    void createSession();

    /// Retrieve next connection identifier.
    static QString nextConnectionId();

    // not implemented
    Connection( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CONNECTION_H
