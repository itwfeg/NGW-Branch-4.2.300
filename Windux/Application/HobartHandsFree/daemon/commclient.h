/**
 * @file commclient.h
 * HFPD Comm Client.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef COMMCLIENT
#define COMMCLIENT

#include "hobarttagmanager.h"
#include "otaupgrademanager.h"

#include <QByteArray>
#include <QList>
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
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] sock  client socket
     * @param[in] parent  parent object
     */
    commClient( QTcpSocket *sock, QObject *parent = 0 );

    /// Destructor.
    virtual ~commClient();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Close client socket.
    virtual void close();

protected:

    /// Device type.
    typedef typename hobartTagManager::device_t device_t;

    /// Device pointer.
    typedef typename hobartTagManager::device_ptr_t device_ptr_t;

    /// Device pointer list.
    typedef typename hobartTagManager::device_ptr_list_t device_ptr_list_t;

    device_ptr_list_t connections_;                 ///< List of active connections.

private slots:

    /// Slot for device config value changed.
    void onDeviceConfigValueChanged( int conf, const QByteArray& value );

    /// Slot for device connected.
    void onDeviceConnected();

    /// Slot for connecting to device.
    void onDeviceConnecting();

    /// Slot for device disconnected.
    void onDeviceDisconnected();

    /// Slot for device error.
    void onDeviceError( int num, const QString& description );

    /// Slot for device ready.
    void onDeviceReady();

    /// Slot for socket disconnected.
    void onDisconnected();

    /// Slot for socket error.
    void onError( QAbstractSocket::SocketError socketError );

    /// Slot for firmware requested.
    void onFirmwareRequested( const QBluetoothAddress& address );

    /// Slot for socket ready read.
    void onReadyRead();

    /// Slot for socket state changed.
    void onStateChanged( QAbstractSocket::SocketState socketState );

    /// Slot for timeout.
    void onTimeout();

    /// Slot for firmware upgrade progress.
    void onUpdateProgress( const QBluetoothAddress& address, bool start, bool finish, qreal progress );

private:

    enum {KEEP_ALIVE_TIMEOUT = 5000};

    hobartTagManager *tagManager_;
    otaUpgradeManager *upgradeManager_;

    QTcpSocket *sock_;
    QByteArray a_;

    QTimer *keepAlive_;

    int timeout_;

    QList<QBluetoothAddress> pendingFirmware_;

    /// Receive message.
    void recvMessage( const message *msg );

    /// Send message.
    void sendMessage( const message *msg ) const;

    /// Handle device message.
    template <class T>
    void handleDeviceMessage( const T *msg );

    /// Find device by address.
    static device_ptr_t findDeviceByAddress( const device_ptr_list_t& devices, const QBluetoothAddress& address );

    // not implemented
    commClient( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // COMMCLIENT
