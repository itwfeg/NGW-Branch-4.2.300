/**
 * @file hcidev.h
 * HCI Bluetooth device.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HCIDEV
#define HCIDEV

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#include <string>

class hcidevThread;

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
class hcidev
{
    typedef hcidev _Myt;

    friend class hcidevThread;

public:

    /// Device states.
    enum deviceState
    {
        HCI_STATE_NONE = 0,                         ///< Base state (unopened).
        HCI_STATE_OPEN = 2,                         ///< Open.
        HCI_STATE_SCANNING = 3,                     ///< Scanning.
        HCI_STATE_FILTERING = 4                     ///< Filtering.
    };

    /// Scan types.
    enum scanType
    {
        SCAN_TYPE_ACTIVE,                           ///< Active BLE scan.
        SCAN_TYPE_PASSIVE                           ///< Passive BLE scan.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    hcidev();

    /// Constructor.
    /**
     * @param[in] rhs  object to copy
     */
    hcidev( const _Myt &rhs );

    /// Destructor.
    virtual ~hcidev();

    // ========================================================================
    // Operators
    // ========================================================================

    /// Assignment operator.
    /**
     * @param[in] rhs  object to copy
     * @return  reference to this
     */
    _Myt& operator = ( const _Myt &rhs );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve adapter address.
    /**
     * @param[out] bdaddr  address
     * @return  @c true on success, @c false otherwise
     */
    virtual bool adapterAddress( bdaddr_t *bdaddr ) const;

    /// Retrieve error string.
    /**
     * @return  error string
     */
    virtual std::string errorString() const {return error_message_;}

    /// Check if device is valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const {return (HCI_STATE_NONE != state());}

    /// Retrieve device state.
    /**
     * @return  state
     */
    virtual deviceState state() const {return state_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Start scanning.
    /**
     * @param[in] type  scan type
     * @return  @c true on success, @c false otherwise
     */
    virtual bool startScanning( scanType type = SCAN_TYPE_PASSIVE );

    /// Stop scanning.
    /**
     * @return  @c true on success, @c false otherwise
     */
    virtual bool stopScanning();

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Open default hci device.
    /**
     * @return  device
     */
    static _Myt openDefaultDevice();

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set error string.
    /**
     * @param[in] value  error string
     */
    virtual void setErrorString( const std::string &value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Close device.
    virtual void close();

    /// Reset error string.
    virtual void resetErrorString();

private:

    int device_id_;
    int device_handle_;

    struct hci_filter original_filter_;

    deviceState state_;

    bool has_error_;
    std::string error_message_;

    /// Copy object.
    void copy( const _Myt &rhs );

    /// Reset object.
    void reset();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HCIDEV

