/**
 * @file scanneragent.h
 * Scanner agent class.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCANNERAGENT
#define SCANNERAGENT

#include <QBluetoothDeviceInfo>
#include <QObject>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scanner agent device info.
struct scannerAgentDeviceInfo
{
    QBluetoothDeviceInfo info;                      ///< Device information.
    int type;                                       ///< Device type.

    bool lowPowerAdvert;                            ///< Flag @c true when low power advert
    bool highPowerAdvert;                           ///< Flag @c true when high power advert

    int transmitPowerLevel;                         ///< TX Power level.
    int firmwareVersion;                            ///< Firmware version.
    int batteryLevel;                               ///< Battery level.
    int messages;                                   ///< Unread message count.

    // Constructor.
    scannerAgentDeviceInfo() :
        type( 0 ),
        lowPowerAdvert( false ),
        highPowerAdvert( false ),
        transmitPowerLevel( 0 ),
        firmwareVersion( 0 ),
        batteryLevel( 0 ),
        messages( 0 ) {}

};

Q_DECLARE_METATYPE( scannerAgentDeviceInfo )

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scanner agent for bluetooth.
class scannerAgent : public QObject
{
    Q_OBJECT

    typedef scannerAgent _Myt;
    typedef QObject _Mybase;

signals:

    /// Signal for when device discovered.
    /**
     * @param[in] info  device info
     */
    void deviceDiscovered( const scannerAgentDeviceInfo& info );

    /// Signal for when device discovered in OTA mode.
    /**
     * @param[in] info  device info
     */
    void deviceDiscoveredOTA( const scannerAgentDeviceInfo& info );

    /// Signal for scanning started.
    void scanningStarted();

    /// Signal for scanning stopped.
    void scanningStopped();

public:

    /// Scan types.
    enum scanType
    {
        SCAN_TYPE_DEFAULT,                          ///< Default BLE scan.
        SCAN_TYPE_ACTIVE,                           ///< Active BLE scan.
        SCAN_TYPE_PASSIVE                           ///< Passive BLE scan.
    };

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Destructor.
    virtual ~scannerAgent();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve error string.
    /**
     * @return  error string
     */
    virtual QString errorString() const;

    /// Retrieve if scanning is active.
    /**
     * @return  @c true if scanning active, @c false otherwise
     */
    virtual bool isScanningActive() const = 0;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Reset device.
    virtual void reset() = 0;

public slots:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Start scanning.
    /**
     * @param[in] type  scan type
     */
    virtual void startScanning( scanType type = SCAN_TYPE_DEFAULT ) = 0;

    /// Stop scanning.
    virtual void stopScanning() = 0;

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    scannerAgent( QObject *parent = 0 );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set error string.
    /**
     * @param[in] value  error string
     */
    virtual void setErrorString( const QString& value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Reset error string.
    virtual void resetErrorString();

private:

    bool error_;

    QString errorMessage_;

    // not implemented
    scannerAgent( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCANNERAGENT

