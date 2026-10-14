/**
 * @file motorolassiscanner.h
 * Motorola SSI scanner driver.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef MOTOROLASSISCANNER
#define MOTOROLASSISCANNER

#include <map>
#include <queue>
#include <string>

class serialComm;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Serial communication class.
class motorolaScanner
{
    typedef motorolaScanner _Myt;

public:

    /// Barcode types.
    enum barcodeType
    {
        BOOKLAND = 0x16,
        CODABAR = 0x02,
        CODE_11 = 0x0C,
        CODE_128 = 0x03,
        CODE_32 = 0x20,
        CODE_39 = 0x01,
        CODE_39_FULL_ASCII = 0x13,
        CODE_93 = 0x07,
        COMPOSITE_CCA_EAN128 = 0x51,
        COMPOSITE_CCA_EAN13 = 0x52,
        COMPOSITE_CCA_EAN8 = 0x53,
        COMPOSITE_CCA_RSS_EXPANDED = 0x54,
        COMPOSITE_CCA_RSS_LIMITED = 0x55,
        COMPOSITE_CCA_RSS14 = 0x56,
        COMPOSITE_CCA_UPCA = 0x57,
        COMPOSITE_CCA_UPCE = 0x58,
        COMPOSITE_CCB_EAN128 = 0x61,
        COMPOSITE_CCB_EAN13 = 0x62,
        COMPOSITE_CCB_EAN8 = 0x63,
        COMPOSITE_CCB_RSS_EXPANDED = 0x64,
        COMPOSITE_CCB_RSS_LIMITED = 0x65,
        COMPOSITE_CCB_RSS14 = 0x66,
        COMPOSITE_CCB_UPCA = 0x67,
        COMPOSITE_CCB_UPCE = 0x68,
        COMPOSITE_CCC_EAN128 = 0x59,
        COUPON_CODE = 0x17,
        CUE_CAT_CODE = 0x38,
        D25 = 0x04,
        DATA_MATRIX = 0x1B,
        EAN128 = 0x0F,
        EAN13 = 0x0B,
        EAN13_P2 = 0x4B,
        EAN13_P5 = 0x8B,
        EAN8 = 0x0A,
        EAN8_P2 = 0x4A,
        EAN8_P5 = 0x8A,
        IATA = 0x05,
        ISBT128 = 0x19,
        ISBT128_CONCAT = 0x21,
        ITF = 0x06,
        MACRO_MICRO_PDF = 0x9A,
        MACRO_PDF417 = 0x28,
        MAXICODE = 0x25,
        MICRO_PDF = 0x1A,
        MSI = 0x0E,
        PARAMETER_FNC3 = 0x33,
        PDF417 = 0x11,
        PLANET_US = 0x1F,
        POSTAL_AUSTRALIA = 0x23,
        POSTAL_DUTCH = 0x24,
        POSTAL_JAPAN = 0x22,
        POSTAL_UK = 0x27,
        POSTBAR_CA = 0x26,
        POSTNET_US = 0x1E,
        QR_CODE = 0x1C,
        RSS_EXPANDED = 0x32,
        RSS_LIMITED = 0x31,
        RSS14 = 0x30,
        SCANLET_WEBCODE = 0x37,
        TLC39 = 0x5A,
        TRIOPTIC = 0x15,
        UPCA = 0x08,
        UPCA_P2 = 0x48,
        UPCA_P5 = 0x88,
        UPCE = 0x09,
        UPCE_P2 = 0x49,
        UPCE_P5 = 0x89,
        UPCE1 = 0x10,
        UPCE1_P2 = 0x50,
        UPCE1_P5 = 0x90
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] portName  port name
     */
    motorolaScanner( const std::string &portName );

    /// Destructor.
    virtual ~motorolaScanner();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve continuous scan.
    /**
     * When @c true, scanner will not timeout.
     *
     * @return  @c true for continuous scanning, @c false otherwise
     */
    virtual bool continuousScanning() const;

    /// Disable scanner after valid scan.
    /**
     * When @c true, scanner will automaically disable after scanning a barcode.
     *
     * @return  @c true when enabled, @c false otherwise
     */
    virtual bool disableAfterScan() const {return autoDisable_;}

    /// Check enabled.
    /**
     * @return  @c true if enabled, @c false otherwise
     */
    virtual bool isEnabled() const {return enabled_;}

    /// Check validity.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const;

    /// Set enabled.
    /**
     * @param[in] value  @c true to enable, @c false otherwise
     */
    virtual void setEnabled( bool value );

    /// Set disable scanner after valid scan.
    /**
     * @param[in]  @c true to enabled, @c false otherwise
     */
    virtual void setDisableAfterScan( bool value ) {autoDisable_ = value;}

    /// Turn on/off LED lights when scanning.
    /**
     * @param[in] value  @c true to enable, @c false otherwise
     */
    virtual void setLedEnabled( bool value );

    /// Retrieve decoder timeout.
    /**
     * This is the amount of time scanner stays active when enabling.
     *
     * @return  timeout (milliseconds)
     */
    virtual unsigned int timeout() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Clear scanner buffer.
    virtual void clear();

    /// Read scanner buffer.
    /**
     * @param[out] type  barcode type
     * @param[out] data  barcode data
     * @return  @c true if barcode present, @c false otherwise
     */
    virtual bool read( barcodeType &type, std::string &data );

protected:

    /// Scanned barcode type (contains type info and data).
    typedef std::pair<barcodeType, std::string> barcode;

    std::queue<barcode> barcodes_;                  ///< Scanner buffer.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Setup device.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool setupDevice();

private:

    enum {MULTIPACKET_FORMAT = 0x99};

    enum ssiOpcode
    {
        AIM_OFF = 0xC4,
        AIM_ON = 0xC5,
        BEEP = 0xE6,
        CMD_ACK = 0xD0,
        CMD_NAK = 0xD1,
        DECODE_DATA = 0xF3,
        IMAGER_MODE = 0xF7,
        LED_OFF = 0xE8,
        LED_ON = 0xE7,
        PARAM_DEFAULTS = 0xC8,
        PARAM_REQUEST = 0xC7,
        PARAM_SEND = 0xC6,
        REPLY_REVISION = 0xA4,
        REQUEST_REVISION = 0xA3,
        SCAN_DISABLE = 0xEA,
        SCAN_ENABLE = 0xE9,
        START_SESSION = 0xE4,
        STOP_SESSION = 0xE5,

        _ALL_OPCODES = 0xFF
    };

    enum imagerMode
    {
        MODE_DECODE = 0,
        MODE_IMAGE_CAPTURE = 1,
        MODE_VIDEO = 2
    };

    serialComm *device_;

    std::map<size_t, size_t> paramSize_;
    std::map<size_t, size_t> params_;

    bool autoDisable_;
    bool enabled_;

    std::queue<std::string> packets_;
    std::string buffer_;

    /// Read from device.
    void readDevice();

    /// Read from device.
    bool readDeviceReply( ssiOpcode opcode, std::string &payload );

    /// Write to device.
    void writeDevice( const std::string &packet );

    /// Process device packet.
    ssiOpcode processDevicePacket( const std::string &packet, unsigned char &status, std::string &payload );

    /// Create packet.
    static std::string createHostPacket( ssiOpcode opcode, char status = 0 );

    /// Create packet.
    static std::string createHostPacket( ssiOpcode opcode, const std::string &payload, char status = 0 );

    /// Sleep.
    static void sleep( unsigned int milliseconds );

    // not implemented
    motorolaScanner( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MOTOROLASSISCANNER
