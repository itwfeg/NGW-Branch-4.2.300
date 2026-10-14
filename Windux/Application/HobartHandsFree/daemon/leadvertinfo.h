/**
 * @file leadvertinfo.h
 * Bluetooth LE advertisement info.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef LEADVERTINFO
#define LEADVERTINFO

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#include <list>
#include <string>

#define GAP_ADTYPE_FLAGS                        0x01    ///< Discovery Mode: GAP ADTYPE Flags Discovery Modes.
#define GAP_ADTYPE_16BIT_MORE                   0x02    ///< Service: More 16-bit UUIDs available.
#define GAP_ADTYPE_16BIT_COMPLETE               0x03    ///< Service: Complete list of 16-bit UUIDs.
#define GAP_ADTYPE_32BIT_MORE                   0x04    ///< Service: More 32-bit UUIDs available.
#define GAP_ADTYPE_32BIT_COMPLETE               0x05    ///< Service: Complete list of 32-bit UUIDs.
#define GAP_ADTYPE_128BIT_MORE                  0x06    ///< Service: More 128-bit UUIDs available.
#define GAP_ADTYPE_128BIT_COMPLETE              0x07    ///< Service: Complete list of 128-bit UUIDs.
#define GAP_ADTYPE_LOCAL_NAME_SHORT             0x08    ///< Shortened local name.
#define GAP_ADTYPE_LOCAL_NAME_COMPLETE          0x09    ///< Complete local name.
#define GAP_ADTYPE_POWER_LEVEL                  0x0A    ///< TX Power Level: 0xXX: -127 to +127 dBm.
#define GAP_ADTYPE_OOB_CLASS_OF_DEVICE          0x0D    ///< Simple Pairing OOB Tag: Class of device (3 octets).
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC     0x0E    ///< Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets).
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR     0x0F    ///< Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets).
#define GAP_ADTYPE_SM_TK                        0x10    ///< Security Manager TK Value.
#define GAP_ADTYPE_SM_OOB_FLAG                  0x11    ///< Secutiry Manager OOB Flags.
#define GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE    0x12    ///< Min and Max values of the connection interval (2 octets Min, 2 octets Max) (0xFFFF indicates no conn interval min or max).
#define GAP_ADTYPE_SIGNED_DATA                  0x13    ///< Signed Data field.
#define GAP_ADTYPE_SERVICES_LIST_16BIT          0x14    ///< Service Solicitation: list of 16-bit Service UUIDs.
#define GAP_ADTYPE_SERVICES_LIST_128BIT         0x15    ///< Service Solicitation: list of 128-bit Service UUIDs.
#define GAP_ADTYPE_SERVICE_DATA                 0x16    ///< Service Data.
#define GAP_ADTYPE_APPEARANCE                   0x19    ///< Appearance.
#define GAP_ADTYPE_MANUFACTURER_SPECIFIC        0xFF    ///< Manufacturer Specific Data: first 2 octets contain the Company Identifier Code followed by the additional manufacturer specific data.

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Bluetooth LE advertisement info.
class leAdvertInfo
{
    typedef leAdvertInfo _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] buffer  raw packet
     */
    leAdvertInfo( const std::string& buffer = std::string() );

    /// Constructor.
    /**
     * @param[in] rhs  object to copy
     */
    leAdvertInfo( const _Myt& other );

    /// Destructor.
    virtual ~leAdvertInfo();

    // ========================================================================
    // Operators
    // ========================================================================

    /// Assignment operator.
    /**
     * @param[in] rhs  object to copy
     * @return  reference to this
     */
    _Myt& operator = ( const _Myt& rhs );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve device address.
    /**
     * @return  device address
     */
    virtual std::string address() const;

    /// Retrieve eir.
    /**
     * @param[in] type  which eir
     * @return  eir
     */
    virtual std::string eir( uint8_t type ) const;

    /// Retrieve eirs.
    /**
     * @return  list of eirs
     */
    virtual std::list<std::string> eirs() const {return eirs_;}

    /// Check if valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const {return !raw_.empty();}

    /// Retrieve rssi.
    /**
     * @return  rssi
     */
    virtual int rssi() const {return rssi_;}

    /// Retrieve timestamp.
    /**
     * @return  timestamp
     */
    virtual time_t timestamp() const {return stamp_.tv_sec;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Compare event to another event.
    /**
     * @param[in] rhs  object to compare to
     * @param[in] checkStamp  check timestamp
     * @return  @c true if equal, @c false otherwise
     */
    virtual bool compare( const _Myt& rhs, bool checkStamp = true ) const;

    /// Print object data.
    virtual void print() const;

    /// Read object from stream.
    /**
     * @param[in,out] is  stream
     */
    virtual void read( std::istream& is );

    /// Write object to stream.
    /**
     * @param[in,out] os  stream
     */
    virtual void write( std::ostream& os ) const;

protected:

    std::string raw_;                               ///< Raw packet data.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Parse raw packet.
    /**
     * @param[in] buffer
     * @return  @c true on success, @c false otherwise
     */
    virtual bool parse( const std::string& buffer );

private:

    timeval stamp_;

    uint8_t	bdaddr_type_;
    bdaddr_t bdaddr_;

    uint8_t reports_count_;
    int8_t rssi_;

    std::list<std::string> eirs_;

    /// Copy object.
    void copy( const _Myt& other );

    /// Reset object.
    void reset();

};

/// Comparison operator.
/**
 * @param[in] lhs  object to compare
 * @param[in] rhs  object to compare
 * @return  @c true if equal, @c false otherwise
 */
inline bool operator == ( const leAdvertInfo& lhs, const leAdvertInfo& rhs )
{
    return (0 == lhs.compare( rhs ));
}

/// Insertion operator.
/**
 * @param[in,out] lhs  stream
 * @param[in] rhs  object
 * @return  reference to lhs
 */
inline std::ostream& operator << ( std::ostream& lhs, const leAdvertInfo& rhs )
{
    rhs.write( lhs );
    return lhs;
}

/// Extraction operator.
/**
 * @param[in,out] lhs  stream
 * @param[in] rhs  object
 * @return  reference to lhs
 */
inline std::istream& operator >> ( std::istream& lhs, leAdvertInfo& rhs )
{
    rhs.read( lhs );
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // LEADVERTINFO

