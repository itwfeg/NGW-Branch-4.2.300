/**
 * @file hobarttag_v1.h
 * Vantron Tag.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTTAG_V1
#define HOBARTTAG_V1

#include "hobarttag.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Vantron HFPD Tag.
class hobartTag_V1 : public hobartTag
{
    Q_OBJECT

    typedef hobartTag_V1 _Myt;
    typedef hobartTag _Mybase;

public:

    /// Tag configuration values.
    enum configType
    {
        FIRMWARE_VER                        = 0,    ///< RO Firmware Version.
        DEVICE_TYPE                         = 1,    ///< RO Device Type.
        OTA_ID                              = 2,    ///< RO OTA Number (for firmware upgrade).

        MSG_ACTIVE_TOP                      = 10,   ///< RW Top Line Message when Active.
        MSG_ACTIVE_BOTTOM                   = 11,   ///< RW Bottom Line Message when Active.
        MSG_INACTIVE_TOP                    = 12,   ///< RW Top Line Message when Inactive (off shift).
        MSG_INACTIVE_BOTTOM                 = 13,   ///< RW Bottom Line Message when Inactive (off shift).

        USER_NAME                           = 20,   ///< RW Operator user name.
        SHIFT_DURATION                      = 21,   ///< RW Operator shift duration in hours: [0-36], 0 to disable

        ADVERT_TX_POWER_LEVEL               = 50,   ///< RW Low power advert tx strength (-40, -20, -16, -12, -8, -4, 0, 4).
        ADVERT_TX_COUNT                     = 51,   ///< RW Low power advert count before high power: [0-255], 0 to disable
        ADVERT_TX_DELAY                     = 52,   ///< RW Delay between adverts in milliseconds: [100-10000]

        BEEP_TIME                           = 60,   ///< RW Beep time in milliseconds: [0-255], 0 to disable

        LOGO                                = 80,   ///< WO Logo bitmap.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] info  device info
     * @param[in] parent  parent object
     */
    hobartTag_V1( const QBluetoothDeviceInfo& info, QObject *parent = 0 );

    /// Destructor.
    virtual ~hobartTag_V1();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve characteristic name.
    /**
     * @param[in] uuid  characteristic
     * @return  name
     */
    virtual QString characteristicName( const QBluetoothUuid& uuid ) const;

    /// Check for read only characteristic.
    /**
     * @param[in] uuid  characteristic to check
     * @return  @c true if write only, @c false otherwise
     */
    virtual bool isReadOnly( const QBluetoothUuid& uuid ) const;

    /// Check for write only characteristic.
    /**
     * @param[in] uuid  characteristic to check
     * @return  @c true if write only, @c false otherwise
     */
    virtual bool isWriteOnly( const QBluetoothUuid& uuid ) const;

    /// Set characteristic value.
    /**
     * @note This is an asyncronous call.
     * @param[in] uuid  characteristic to set
     * @param[in] value  new value
     * @param[in] respUuid  when set, wait for characteristic @p respUuid before proceeding to next write
     */
    virtual void setValue( const QBluetoothUuid& uuid, const QByteArray& value, const QBluetoothUuid& respUuid = QBluetoothUuid() );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Clear all messages on device.
    virtual void clearMessages();

    /// Enter OTA mode (if supported).
    virtual void otaMode();

    /// Read all configuration values.
    virtual void readAllConfig();

    /// Read config value.
    /**
     * @param[in] conf  config type
     * @param[out] value  value
     * @param[in] useCached  @c true to use cached value, @c false to read device
     * @return  @c true if @p value populated, @c false otherwise
     */
    virtual bool readConfig( int conf, QByteArray& value, bool useCached = true );

    /// Send message to device.
    /**
     * @param[in] id  message id
     * @param[in] time  timestamp
     * @param[in] msg  message
     */
    virtual void sendMessage( unsigned char id, const QTime& time, const QByteArray& msg );

    /// Write config value to device.
    /**
     * @param[in] conf  config type
     * @param[in] value  value
     * @return  @c true if config written, @c false otherwise
     */
    virtual bool writeConfig( int conf, const QByteArray& value );

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve list of characteristics for specific service.
    /**
     * @param[in] uuid  service uuid
     * @return  list of characteristics
     */
    virtual uuidList characteristics( const QBluetoothUuid& uuid ) const;

    /// Check for characteristic that enables notify.
    /**
     * @param[in] uuid  characteristic to check
     * @return  @c true if notify only, @c false otherwise
     */
    virtual bool isNotifyEnabled( const QBluetoothUuid& uuid ) const;

    /// Retrieve list of services.
    /**
     * @return  list of services
     */
    virtual uuidList services() const;

private slots:

    /// Slot for value changed
    void onValueChanged( const QBluetoothUuid& uuid, const QByteArray& value );

private:

    enum commandType
    {
        CMD_GET_FW_VERSION                  = 0x01,
        CMD_GET_OTA_NUMBER                  = 0x02,
        CMD_GO_TO_OTA_MODE                  = 0x03,
        CMD_RCV_MESSAGE                     = 0x04,
        CMD_SET_IP_AND_TIME                 = 0x05,
        CMD_CLEAR_MESSAGES                  = 0x06,
        CMD_SET_ACTV_MSG_TOP                = 0x07,
        CMD_SET_ACTV_MSG_BOT                = 0x08,
        CMD_SET_SLEP_MSG_TOP                = 0x09,
        CMD_SET_SLEP_MSG_BOT                = 0x0A,
        CMD_SET_USR_NAME                    = 0x0B,
        CMD_SET_TX_PWR_LVL                  = 0x0C,
        CMD_SET_SHIFT_DURATION              = 0x0D,
        CMD_SET_LOW_POWER_COUNTS            = 0x0E,
        CMD_SET_ADV_DELAY                   = 0x0F,
        CMD_SET_BEEP_TIME                   = 0x10,
        CMD_GET_BEEPER_CONFIG               = 0x11,
        CMD_GET_ACTV_MSG_TOP                = 0x12,
        CMD_GET_ACTV_MSG_BOT                = 0x13,
        CMD_GET_SLEP_MSG_TOP                = 0x14,
        CMD_GET_SLEP_MSG_BOT                = 0x15,
        CMD_GET_USR_NAME                    = 0x16,

        CMD_LOGO_UPDATE                     = 0x50,
        CMD_LOGO_UPDATE_END                 = 0x80,
    };

    QMap<QBluetoothUuid, QString> names_;

    /// Pad byte array value.
    static QByteArray padValue( const QByteArray& value, int len );

    // not implemented
    hobartTag_V1( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTTAG_V1
