/**
 * @file dfudevice.h
 * Nordic DFU (Device Firmware Update) Device.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DFUDEVICE
#define DFUDEVICE

#include "bledevice.h"

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Nordic DFU (Device Firmware Update) Device.
class dfuDevice : public bleDevice
{
    Q_OBJECT

    typedef dfuDevice _Myt;
    typedef bleDevice _Mybase;

public:

    /// DFU Error Codes.
    enum errorCode
    {
        SUCCESS = 1,
        INVALID_STATE = 2,
        NOT_SUPPORTED = 3,
        DATA_SIZE_EXCEEDS_LIMIT = 4,
        CRC_ERROR = 5,
        OPERATION_FAILED = 6,
    };

    Q_ENUM( errorCode )

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] info  device info
     * @param[in] parent  parent object
     */
    dfuDevice( const QBluetoothDeviceInfo& info, QObject *parent = 0 );

    /// Destructor.
    virtual ~dfuDevice();

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

    // ========================================================================
    // Methods
    // ========================================================================

    /// Activate firmware.
    virtual void activate();

    /// Request reveived image size.
    virtual void firmwareSize();

    /// Initialize upgrade.
    /**
     * @param[in] appVersion  app version
     * @param[in] crc  firmware checksum
     */
    virtual void init( size_t appVersion, size_t crc );

    /// Send firmware image.
    /**
     * @param[in] image  firmware image
     * @param[in] idx  index to begin transmission
     */
    virtual void sendFirmware( const QByteArray& image, size_t idx = 0 );

    /// Start application upgrade.
    /**
     * @param[in] appSize  size of app code image
     */
    virtual void start( size_t appSize );

    /// Validate firmware.
    virtual void validate();

signals:

    /// Signal for download progress.
    /**
     * @param[in] percent  progress
     */
    void downloadProgress( qreal percent );

    /// Signal for firmware size result.
    /**
     * @param[in] result  result code
     * @param[in] size  image size
     */
    void firmwareSizeResult( errorCode result, size_t size = 0 );

    /// Signal for init result.
    /**
     * @param[in] result  result code
     */
    void initResult( errorCode result );

    /// Signal for send firmware result.
    /**
     * @param[in] result  result code
     */
    void sendFirmwareResult( errorCode result );

    /// Signal for start result.
    /**
     * @param[in] result  result code
     */
    void startResult( errorCode result );

    /// Signal for validate result.
    /**
     * @param[in] result  result code
     */
    void validateResult( errorCode result );

protected:

    /// DFU Image Types.
    enum imageType
    {
        SOFTDEVICE = 1,
        BOOTLOADER = 2,
        SD_BL = 3,
        APPLICATION = 4,
    };

    /// DFU Opcodes.
    enum opcode
    {
        INVALID_OPCODE = 0,
        START_DFU = 1,
        INITIALIZE_DFU = 2,
        RECEIVE_FIRMWARE_IMAGE = 3,
        VALIDATE_FIRMWARE_IMAGE = 4,
        ACTIVATE_FIRMWARE_AND_RESET = 5,
        SYSTEM_RESET = 6,
        REPORT_RECEIVED_IMAGE_SIZE = 7,
        REQ_PKT_RCPT_NOTIFICATION = 8,
        RESPONSE = 16,
        PKT_RCPT_NOTIF = 17
    };

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

    // ========================================================================
    // Methods
    // ========================================================================

    /// Send control data.
    /**
     * @param[in] op  opcode
     * @param[in] data  data
     */
    virtual void sendControlData( opcode op, const QByteArray& data = QByteArray() );

    /// Send packet data.
    /**
     * @param[in] data  data
     */
    virtual void sendPacketData( const QByteArray& data );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Create image size packet.
    /**
     * @param[in] softdeviceSize  size of sofdevice code image
     * @param[in] bootloaderSize  size of bootloader code image
     * @param[in] appSize  size of app code image
     * @return  image size packet
     */
    static QByteArray createImageSizePacket( uint32_t softdeviceSize, uint32_t bootloaderSize, uint32_t appSize );

    /// Create init packet.
    /**
     * @param[in] dt  device type
     * @param[in] drev  device revision
     * @param[in] appVersion  app version
     * @param[in] softDevices  list of soft devices supported
     * @param[in] crc  firmware checksum
     * @return  init packet
     */
    static QByteArray createInitPacket( uint16_t dt, uint16_t drev, uint32_t appVersion, const QList<uint16_t>& softDevices, uint16_t crc );

private slots:

    /// Slot for when characteristic value changes.
    void onValueChanged( const QBluetoothUuid& uuid, const QByteArray& value );

private:

    enum {FIRMARE_IMAGE_PACKET_SIZE = 20};

    QMap<QBluetoothUuid, QString> names_;

    QByteArray imagePending_;
    size_t imagePendingIndex_;

    /// Send firmware packet.
    void sendFirmwarePacket();

    /// Convert value to byte array.
    template <typename N>
    static QByteArray toByteArray( N value )
    {
        static const N mask( 0xFF );

        size_t loops( sizeof(N) );
        QByteArray a;

        do
        {
            a.append( value & mask );
            value >>= 8;

        } while ( --loops );

        return a;
    }

    // not implemented
    dfuDevice( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DFUDEVICE
