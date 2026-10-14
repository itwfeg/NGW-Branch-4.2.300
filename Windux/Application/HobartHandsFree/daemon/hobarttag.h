/**
 * @file hobarttag.h
 * Hobart Tag.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTTAG
#define HOBARTTAG

#include "bledevice.h"

#include <QDateTime>
#include <QMap>
#include <QObject>
#include <QString>

struct scannerAgentDeviceInfo;

class QByteArray;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Hobart Tag.
class hobartTag : public bleDevice
{
    Q_OBJECT

    typedef hobartTag _Myt;
    typedef bleDevice _Mybase;

signals:

    /// Signal for when config value changes.
    /**
     * @param[in] conf  config type
     * @param[in] value  new value
     */
    void configValueChanged( int conf, const QByteArray& value );

public:

    /// Advert Power Levels.
    enum powerLevel
    {
        POWER_LEVEL_HIGH                = 0,        ///< High power level.
        POWER_LEVEL_LOW                 = 1,        ///< Low power level.
    };

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~hobartTag();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve battery level.
    /**
     * @return  battery level
     */
    virtual int batteryLevel() const;

    /// Retrieve created date/time.
    /**
     * @return  creation date/time
     */
    virtual QDateTime created() const;

    /// Retrieve firmware version.
    /**
     * @return  firmware version
     */
    virtual int firmwareVersion() const;

    /// Retrieve last advertisement date/time.
    /**
     * @param[in] txpower  power level
     * @return  last advertisement date/time
     */
    virtual QDateTime lastAdvert( powerLevel txpower ) const;

    /// Retrieve last communication date/time.
    /**
     * @return  last communication date/time
     */
    virtual QDateTime lastComm() const;

    /// Retrieve number of unread messages.
    /**
     * @return  unread messages
     */
    virtual int messages() const;

    /// Retrieve configured operator name.
    /**
     * @return  operator name
     */
    virtual QString operatorName() const;

    /// Retrieve rssi.
    /**
     * @param[in] txpower  power level
     * @return  device rssi.
     */
    virtual int rssi( powerLevel txpower ) const;

    /// Set configured operator name.
    /**
     * @param[in] value  operator name
     */
    virtual void setOperatorName( const QString& value );

    /// Retrieve transmit power level.
    /**
     * @param[in] txpower  power level
     * @return  power level
     */
    virtual int transmitPowerLevel( powerLevel txpower ) const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Clear all messages on device.
    virtual void clearMessages() = 0;

    /// Enter OTA mode (if supported).
    virtual void otaMode();

    /// Read all configuration values.
    virtual void readAllConfig() = 0;

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
    virtual void sendMessage( unsigned char id, const QTime& time, const QByteArray& msg ) = 0;

    /// Update device with scanner information.
    /**
     * @param[in] info  scanner information
     */
    virtual void update( const scannerAgentDeviceInfo& info );

    /// Write config value to device.
    /**
     * @param[in] conf  config type
     * @param[in] value  value
     * @return  @c true if config written, @c false otherwise
     */
    virtual bool writeConfig( int conf, const QByteArray& value );

protected:

    /// Map of date/time values.
    typedef QMap<powerLevel, QDateTime> dateTimeMap;

    /// Map of transmit power level values.
    typedef QMap<powerLevel, int> transmitPowerLevelMap;

    QDateTime lastComm_;                            ///< Last communication time.

    int batteryLevel_;                              ///< Battery level.
    int firmwareVersion_;                           ///< Firmware version.
    int messages_;                                  ///< Unread message count.

    dateTimeMap lastAdvert_;                        ///< Last advert time.
    transmitPowerLevelMap powerLevel_;              ///< Transmit power level.

    int rssiHighPower_;                             ///< RSSI (high power advert).

    QMap<int, QByteArray> configValue_;             ///< Configuration values.

    QString operatorName_;                          ///< Configured operator name.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] info  device info
     * @param[in] parent  parent object
     */
    hobartTag( const QBluetoothDeviceInfo& info, QObject *parent = 0 );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Calculate filtered low power rssi value.
    /**
     * @return  filtered rssi value
     */
    virtual int rssiLowPowerFiltered() const;

private slots:

    /// Slot for value changed
    void onValueChanged( const QBluetoothUuid& uuid, const QByteArray& value );

private:

    enum {HISTORY_MIN_SIZE = 4};                    ///< Minimum history to keep.
    enum {HISTORY_MAX_SIZE = 16};                   ///< Maximum history to keep.
    enum {HISTORY_AMOUNT_MS = 4700};                ///< Amount of history to keep in milliseconds. increased from 2700 

    /// RSSI value at point in time.
    typedef QPair<QDateTime, int> rssiDataPoint;

    /// List of transmit power over time.
    typedef QList<rssiDataPoint> rssiHistoryList;

    rssiHistoryList history_;                       ///< RSSI history for low power adverts.

    // not implemented
    hobartTag( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTTAG
