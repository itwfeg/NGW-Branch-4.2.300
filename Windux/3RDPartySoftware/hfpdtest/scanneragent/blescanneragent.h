/**
 * @file blescanneragent.h
 * BLE Discovery Agent.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef BLESCANNERAGENT_H
#define BLESCANNERAGENT_H

#include "scanneragent.h"

#include <QBluetoothAddress>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QMap>
#include <QObject>

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Bluetooth device discovery agent.
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
     * @param[in] parent  parent
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
    void onScanDeviceDiscovered( const QBluetoothDeviceInfo &info );

    /// Slot for scan error.
    void onScanError( QBluetoothDeviceDiscoveryAgent::Error error );

    /// Slot for scan finished.
    void onScanFinished();

private:

    enum {SCAN_TIMEOUT = 2500};

    unsigned int errors_;

    bool active_;
    bool activeEmitted_;

    QTimer *scanTimer_;

    QBluetoothDeviceDiscoveryAgent *scan_;
    QBluetoothLocalDevice *localDevice_;

    // not implemented
    bleScannerAgent( const _Myt &other );

    // not implemented
    _Myt & operator = ( const _Myt &other );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BLEAGENT_H
