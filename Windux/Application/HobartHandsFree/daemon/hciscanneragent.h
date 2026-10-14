/**
 * @file hciscanneragent.h
 * HCI Bluetooth device.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HCISCANNERAGENT
#define HCISCANNERAGENT

#include "leadvertinfo.h"
#include "scanneragent.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

class hciScannerAgentThread;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HCI Bluetooth device.
/**
 * @note
 * If the bluetooth interface is not up this device class will not work. You may bring it up with
 * the following commands:
 * $ modprobe ath3k
 * $ hciconfig hci0 up
 *
 */
class hciScannerAgent : public scannerAgent
{
    Q_OBJECT

    typedef hciScannerAgent _Myt;
    typedef scannerAgent _Mybase;

    friend class hciScannerAgentThread;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    hciScannerAgent( QObject *parent = 0 );

    /// Destructor.
    virtual ~hciScannerAgent();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve if scanning is active.
    /**
     * @return  @c true if scanning active, @c false otherwise
     */
    virtual bool isScanningActive() const;

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

    /// Device states.
    enum deviceState
    {
        HCI_STATE_NONE = 0,                         ///< Base state (unopened).
        HCI_STATE_OPEN = 2,                         ///< Open.
        HCI_STATE_SCANNING = 3,                     ///< Scanning.
        HCI_STATE_FILTERING = 4                     ///< Filtering.
    };

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve adapter address.
    /**
     * @param[out] bdaddr  address
     * @return  @c true on success, @c false otherwise
     */
    virtual bool adapterAddress( bdaddr_t *bdaddr ) const;

    /// Retrieve device state.
    /**
     * @return  state
     */
    virtual deviceState state() const {return state_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Process BLE advert.
    /**
     * @param[in] info  ble advert
     */
    virtual void processAdvert( const leAdvertInfo *info );

private:

    int device_id_;
    int device_handle_;

    struct hci_filter original_filter_;

    deviceState state_;

    hciScannerAgentThread *t_;

    /// Open default hci device.
    void openDefaultDevice();

    /// Close device.
    void close();

    // not implemented
    hciScannerAgent( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HCISCANNERAGENT

