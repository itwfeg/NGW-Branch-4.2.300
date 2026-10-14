/**
 * @file hobarttagmanager.h
 * Hobart Operator Tag Manager.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTTAGMANAGER
#define HOBARTTAGMANAGER

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QSharedPointer>
#include <QTimer>

class hobartTag;
struct scannerAgentDeviceInfo;

class QBluetoothAddress;
class QBluetoothDeviceInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Hobart Operator Tag Manager.
class hobartTagManager : public QObject
{
    Q_OBJECT

    typedef hobartTagManager _Myt;
    typedef QObject _Mybase;

public:

    /// Device type.
    typedef hobartTag device_t;

    /// Device pointer.
    typedef QSharedPointer<device_t> device_ptr_t;

    /// Device pointer list.
    typedef QList<device_ptr_t> device_ptr_list_t;

    /// Mapping of operator names.
    typedef QMap<QBluetoothAddress, QString> operator_name_map_t;

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

    /// Set operator names.
    /**
     * @param[in] value  map of operator names
     */
    void setOperatorNames( const operator_name_map_t& value );

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

    /// Update device info.
    /**
     * @param[in] info  device info
     */
    void updateDevice( const scannerAgentDeviceInfo& info );

signals:

    /// Signal for active connections changing.
    void updateActiveConnections( int active );

protected:

    mutable QReadWriteLock m_;                      ///< Mutex for device access.

    /// Device mapping.
    typedef QMap<QBluetoothAddress, device_ptr_t> deviceMap;

    deviceMap devices_;                             ///< Devices.

    operator_name_map_t operatorNames_;             ///< Operator names.

private slots:

    /// Slot for device connecting.
    void onDeviceConnecting();

    /// Slot for device disconnected.
    void onDeviceDisconnected();

private:

    static QMutex instanceLock_;
    static _Myt *instance_;

    int active_;

    /// Constructor.
    hobartTagManager();

    /// Destructor.
    ~hobartTagManager();

    // not implemented
    hobartTagManager( const _Myt & );

    // not implemented
    _Myt& operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTTAGMANAGER

