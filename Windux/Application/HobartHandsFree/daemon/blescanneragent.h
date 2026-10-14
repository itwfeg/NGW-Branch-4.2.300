/**
 * @file blescanneragent.h
 * BLE Discovery Agent.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef BLESCANNERAGENT
#define BLESCANNERAGENT

#include "scanneragent.h"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>

class QBluetoothDeviceInfo;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// BLE Discovery Agent.
/**
 * @note
 * If the bluez daemon is not up this device class will not work. You may bring it up with
 * the following commands:
 * $ modprobe ath3k
 * $ hciconfig hci0 up
 * $ /usr/lib/bluetooth/bluetoothd
 */
class bleScannerAgent : public scannerAgent
{
    Q_OBJECT

    typedef bleScannerAgent _Myt;
    typedef scannerAgent _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    bleScannerAgent( QObject *parent = 0 );

    /// Destructor.
    virtual ~bleScannerAgent();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve if scanning is active.
    /**
     * @return  @c true if scanning active, @c false otherwise
     */
    virtual bool isScanningActive() const {return active_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Reset device.
    virtual void reset();

public slots:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Start scanning.
    /**
     * @param[in] type  scan type
     */
    virtual void startScanning( scanType type = SCAN_TYPE_DEFAULT );

    /// Stop scanning.
    virtual void stopScanning();

protected:

    /// Create scanner.
    void createScanner();

    /// Destroy scanner.
    void destroyScanner();

private slots:

    /// Slot for local device error.
    void onLocalDeviceError( QBluetoothLocalDevice::Error error );

    /// Slot for local device host mode changed.
    void onLocalDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode state );

    /// Slot for scan canceled.
    void onScanCanceled();

    /// Slot for scan device discovered.
    void onScanDeviceDiscovered( const QBluetoothDeviceInfo& info );

    /// Slot for scan error.
    void onScanError( QBluetoothDeviceDiscoveryAgent::Error error );

    /// Slot for scan finished.
    void onScanFinished();

private:

    enum {SCAN_TIMEOUT = 750};

    unsigned int errors_;

    bool active_;
    bool activeEmitted_;

    QTimer *scanTimer_;

    QBluetoothDeviceDiscoveryAgent *scan_;
    QBluetoothLocalDevice *localDevice_;

    // not implemented
    bleScannerAgent( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BLESCANNERAGENT
