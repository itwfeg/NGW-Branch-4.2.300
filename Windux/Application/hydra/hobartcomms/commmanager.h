/**
 * @file commmanager.h
 * @brief Communication manager for Hobart services.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef COMMMANAGER_H
#define COMMMANAGER_H

#include <messagecenterclient.h>

#include <QAbstractSocket>
#include <QMutex>

class QTcpServer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Communication Manager.
class CommManager : public MessageCenterClient
{
    Q_OBJECT

    using _Myt = CommManager;
    using _Mybase = MessageCenterClient;

public slots:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve number of connections.
    /**
     * @return  connection count
     */
    uint count() const;

    /// Check if manager is open.
    /**
     * @return  @c true if open, @c false if closed
     */
    bool isOpen() const;

    /// Retrieve server port.
    /**
     * @return  port
     */
    uint serverPort() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Close server.
    void close();

    /// Open server.
    /**
     * @return  @c true if server open, @c false otherwise
     */
    bool open();

    /// Terminate connection.
    /**
     * @param[in] name  connetion name
     */
    void terminateConnection( const QString& name );

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve global instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

protected:

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

    /// Slot for accept errors.
    void onAcceptError( QAbstractSocket::SocketError socketError );

    /// Slot for new connections.
    void onNewConnection();

private:

    enum {SERVER_PORT = 6000};

    static _Myt *instance_;
    static QMutex instanceMutex_;

    QTcpServer *s_;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    CommManager();

    // not implemented
    CommManager( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // COMMMANAGER_H
