/**
 * @file otaupgrademanager.h
 * OTA (Over the Air) Upgrade Manager.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef OTAUPGRADEMANAGER
#define OTAUPGRADEMANAGER

#include "dfudevice.h"

#include <QDateTime>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QSharedPointer>

struct scannerAgentDeviceInfo;

class QBluetoothAddress;
class QBluetoothDeviceInfo;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// OTA Upgrade Manager.
class otaUpgradeManager : public QObject
{
    Q_OBJECT

    typedef otaUpgradeManager _Myt;
    typedef QObject _Mybase;

public:

    /// Device type.
    typedef dfuDevice device_t;

    /// Device pointer.
    typedef QSharedPointer<device_t> device_ptr_t;

    /// Device pointer list.
    typedef QList<device_ptr_t> device_ptr_list_t;

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve number of active connections.
    /**
     * @return  acttive connections
     */
    int activeConnections() const {return active_;}

    /// Retrieve list of devices.
    /**
     * @return  list of devices
     */
    device_ptr_list_t devices() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Prepare device for upgrade.
    /**
     * @param[in] device  device address
     * @param[in] filename  firmware
     * @param[in] id  ota identifier
     * @return  @c true upon success, @c false otherwise
     */
    bool prepareUpgrade( const QBluetoothAddress& device, const QString& filename, unsigned int id = 0 );

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

public slots:

    /// Update device in OTA mode.
    /**
     * @param[in] info  device info
     */
    void updateDevice( const scannerAgentDeviceInfo& info );

signals:

    /// Signal for device requesting firmware.
    /**
     * @param[in] address  device address
     */
    void firmwareRequested( const QBluetoothAddress& address );

    /// Signal for active connections changing.
    /**
     * @param[in] active  number of active connections
     */
    void updateActiveConnections( int active );

    /// Signal for firmware upgrade progress.
    /**
     * @param[in] address  device address
     * @param[in] start  @c true if starting upgrade
     * @param[in] finish  @c true if completing upgrade
     * @param[in] progress  upgrade progress percentage
     */
    void updateProgress( const QBluetoothAddress& address, bool start, bool finish, qreal progress );

protected:

    mutable QReadWriteLock m_;                      ///< Mutex for device access.

    /// Device mapping.
    typedef QMap<QBluetoothAddress, device_ptr_t> deviceMap;

    deviceMap devices_;                             ///< Devices.

private slots:

    /// Slot for device connecting.
    void onDeviceConnecting();

    /// Slot for device disconnected.
    void onDeviceDisconnected();

    /// SLot for device download progress.
    void onDeviceDownloadProgress( qreal percent );

    /// Slot for device firmware size result.
    void onDeviceFirmwareSizeResult( dfuDevice::errorCode value, size_t size );

    /// Slot for device ready.
    void onDeviceReady();

    /// Slot for device result.
    void onDeviceResult( dfuDevice::errorCode value );

    /// Slot for timeout.
    void onTimeout();

private:

    enum {TIMEOUT_TIME = 250};

    enum {RECONNECT_TIMEOUT = 5000};
    enum {PENDING_TIMEOUT = 90000};
    enum {UPGRADE_TIMEOUT = 300000};

    static QMutex instanceLock_;
    static _Myt *instance_;

    int active_;

    /// Update state.
    enum updateState
    {
        PENDING,
        START,
        INIT,
        UPDATE,
        UPDATE_RECOVERY,
        VALIDATE,
        ACTIVATE,
        COMPLETE
    };

    /// Firmware update info.
    struct firmwareUpdateInfo
    {
        updateState state;
        bool processed;
        bool timeout;

        QDateTime pending;
        QDateTime start;
        QDateTime finish;

        QByteArray firmware;
        size_t firmwareIndex;
        size_t version;
        size_t crc;
    };

    /// Firmware update info mapping.
    typedef QMap<QBluetoothAddress, firmwareUpdateInfo> firmwareUpdateInfoMap;

    firmwareUpdateInfoMap deviceInfo_;              ///< Device info.

    QTimer *t_;                                     ///< Timer.

    /// State machine mapping.
    typedef QMap<updateState, updateState> updateStateMap;

    updateStateMap nextState_;                      ///< Next state map.

    /// Constructor.
    otaUpgradeManager();

    /// Destructor.
    ~otaUpgradeManager();

    /// Restart firmware update.
    void restart( device_t *device );

    /// Process next state.
    void processNextState( device_t *device );

    /// Process current state.
    void processCurrentState( device_t *device );

    // not implemented
    otaUpgradeManager( const _Myt & );

    // not implemented
    _Myt& operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // OTAUPGRADEMANAGER

