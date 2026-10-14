/**
 * @file bledevice.h
 * BLE Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef BLEDEVICE_H
#define BLEDEVICE_H

#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QDateTime>
#include <QList>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QMap>
#include <QObject>

class QBluetoothDeviceInfo;
class QByteArray;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Bluetooth device.
class bleDevice : public QObject
{
    Q_OBJECT

    typedef bleDevice _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~bleDevice();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve device address.
    /**
     * @return  device address
     */
    virtual QBluetoothAddress address() const;

    /// Retrieve characteristic name.
    /**
     * @param[in] uuid  characteristic
     * @return  name
     */
    virtual QString characteristicName( const QBluetoothUuid &uuid ) const;

    /// Check for read only characteristic.
    /**
     * @param[in] uuid  characteristic to check
     * @return  @c true if write only, @c false otherwise
     */
    virtual bool isReadOnly( const QBluetoothUuid &uuid ) const;

    /// Check for write only characteristic.
    /**
     * @param[in] uuid  characteristic to check
     * @return  @c true if write only, @c false otherwise
     */
    virtual bool isWriteOnly( const QBluetoothUuid &uuid ) const;

    /// Retrieve rssi.
    /**
     * @return  device rssi.
     */
    virtual int rssi() const {return rssi_;}

    /// Set characteristic value.
    /**
     * @note This is an asyncronous call.
     *
     * @param[in] uuid  characteristic to set
     * @param[in] value  new value
     */
    virtual void setValue( const QBluetoothUuid &uuid, const QByteArray &value );

    /// Retrieve characteristic value.
    /**
     * @param[in] uuid  characteristic to retrieve
     * @return  value upon success, empty array if value not ready
     */
    virtual QByteArray value( const QBluetoothUuid &uuid ) const;

signals:

    /// Signal for when characteristic value changes.
    /**
     * @param[in] uuid  characteristic
     * @param[in] value  new value
     */
    void valueChanged( const QBluetoothUuid &uuid, const QByteArray &value );

protected:

    /// Bluetooth uuid list.
    typedef QList<QBluetoothUuid> uuidList;

    QLowEnergyController *controller_;              ///< Controller.

    QDateTime created_;                             ///< Creation time.
    int rssi_;                                      ///< RSSI.

    /// Map of discovered characteristics by uuid.
    QMap<QBluetoothUuid, QLowEnergyCharacteristic> characteristic_;

    /// Map of characteristic values by uuid.
    QMap<QBluetoothUuid, QByteArray> values_;

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] info  device info
     * @param[in] parent  parent object
     */
    bleDevice( const QBluetoothDeviceInfo &info, QObject *parent = 0 );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve list of characteristics for specific service.
    /**
     * @param[in] uuid  service uuid
     * @return  list of characteristics
     */
    virtual uuidList characteristics( const QBluetoothUuid &uuid ) const = 0;

    /// Check for characteristic that enables notify.
    /**
     * @param[in] uuid  characteristic to check
     * @return  @c true if notify only, @c false otherwise
     */
    virtual bool isNotifyEnabled( const QBluetoothUuid &uuid ) const;

    /// Retrieve list of services.
    /**
     * @return  list of services
     */
    virtual uuidList services() const = 0;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Read all characteristics.
    virtual void readAllCharacteristics();

    /// Read characteristic.
    /**
     * @param[in] ch  characteristic to retrieve
     */
    virtual void readCharacteristic( const QBluetoothUuid &uuid );

    /// Write characteristic.
    /**
     * @param[in] uuid  characteristic to set
     * @param[in] value  new value
     */
    virtual void writeCharacteristic( const QBluetoothUuid &uuid, const QByteArray &value );

private slots:

    /// Slot for controller error.
    void onControllerError( QLowEnergyController::Error newError );

    /// Slot for controller state changed.
    void onControllerStateChanged( QLowEnergyController::ControllerState state );

    /// Slot for service characteristic updated.
    void onServiceCharacteristicUpdated( const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue );

    /// Slot for service error.
    void onServiceError( QLowEnergyService::ServiceError newError );

    /// Slot for service state changed.
    void onServiceStateChanged( QLowEnergyService::ServiceState state );

    /// Slot for timer timeout.
    void onTimeout();

private:

    typedef QList<QLowEnergyCharacteristic> characteristicList;

    struct commOperation
    {
        commOperation( const QBluetoothUuid &id ) : id_( id ) {}
        commOperation( const QBluetoothUuid &id, const QByteArray &v ) : id_( id ), value_( v ) {}

        QBluetoothUuid id_;
        QByteArray value_;
    };

    enum {CONNECT_TIMEOUT = 30000};
    enum {DISCOVER_SERVICES_TIMEOUT = 10000};
    enum {READ_TIMEOUT = 10000};
    enum {WRITE_TIMEOUT = 10000};

    QList<commOperation> comm_;

    QTimer *reconnectTimer_;
    QTimer *processCommTimer_;
    QTimer *timeoutTimer_;
    QTimer *uuidTimer_;

    QMap<QBluetoothUuid, QBluetoothUuid> uuid_;

    /// Connect to device.
    void connectToDevice();

    /// Process uuid.
    void processUuid();

    /// Process characteristics.
    void processCharacteristics( const characteristicList &chs );

    /// Process services.
    void processServices( const uuidList &ids );

    /// Process comms.
    void processComm();

    // not implemented
    bleDevice( const _Myt &other );

    // not implemented
    _Myt & operator = ( const _Myt &other );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BLEDEVICE_H
