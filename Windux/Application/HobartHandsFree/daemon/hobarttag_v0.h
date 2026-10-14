/**
 * @file hobarttag_v0.h
 * Vantron Tag.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTTAG_V0
#define HOBARTTAG_V0

#include "hobarttag.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Vantron HFPD Tag.
class hobartTag_V0 : public hobartTag
{
    Q_OBJECT

    typedef hobartTag_V0 _Myt;
    typedef hobartTag _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] info  device info
     * @param[in] parent  parent object
     */
    hobartTag_V0( const QBluetoothDeviceInfo& info, QObject *parent = 0 );

    /// Destructor.
    virtual ~hobartTag_V0();

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

    /// Clear all messages on device.
    virtual void clearMessages();

    /// Read all configuration values.
    virtual void readAllConfig();

    /// Send message to device.
    /**
     * @param[in] id  message id
     * @param[in] time  timestamp
     * @param[in] msg  message
     */
    virtual void sendMessage( unsigned char id, const QTime& time, const QByteArray& msg );

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

    /// Retrieve list of services.
    /**
     * @return  list of services
     */
    virtual uuidList services() const;

private:

    QMap<QBluetoothUuid, QString> names_;

    // not implemented
    hobartTag_V0( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTTAG_V0
