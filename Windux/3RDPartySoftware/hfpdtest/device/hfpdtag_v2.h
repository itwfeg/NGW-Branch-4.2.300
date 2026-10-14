/**
 * @file hfpdtag.h
 * Vantron Tag.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPDTAGV2_H
#define HFPDTAGV2_H

#include "bledevice.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Vantron HFPD Tag.
class hfpdTagV2 : public bleDevice
{
    Q_OBJECT

    typedef hfpdTagV2 _Myt;
    typedef bleDevice _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] info  device info
     * @param[in] parent  parent object
     */
    hfpdTagV2( const QBluetoothDeviceInfo &info, QObject *parent = 0 );

    /// Destructor.
    virtual ~hfpdTagV2();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve characteristic name.
    /**
     * @param[in] uuid  characteristic
     * @return  name
     */
    virtual QString characteristicName( const QBluetoothUuid &uuid ) const;

    /// Check for read only characteristic.
    /**
     * @param[in] uuid  characteristic to check
     * @return  @c true if write only, @c false otherwise
     */
    virtual bool isReadOnly( const QBluetoothUuid &uuid ) const;

    /// Check for write only characteristic.
    /**
     * @param[in] uuid  characteristic to check
     * @return  @c true if write only, @c false otherwise
     */
    virtual bool isWriteOnly( const QBluetoothUuid &uuid ) const;

    /// Set characteristic value.
    /**
     * @note This is an asyncronous call.
     *
     * @param[in] uuid  characteristic to set
     * @param[in] value  new value
     */
    virtual void setValue( const QBluetoothUuid &uuid, const QByteArray &value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Read firmware version.
    virtual void firmware();

    /// Write message to device.
    /**
     * @param[in] id  message id
     * @param[in] time  timestamp
     * @param[in] msg  message
     */
    virtual void writeMessage( unsigned char id, const QTime &time, const QByteArray &msg );

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve list of characteristics for specific service.
    /**
     * @param[in] uuid  service uuid
     * @return  list of characteristics
     */
    virtual uuidList characteristics( const QBluetoothUuid &uuid ) const;

    /// Check for characteristic that enables notify.
    /**
     * @param[in] uuid  characteristic to check
     * @return  @c true if notify only, @c false otherwise
     */
    virtual bool isNotifyEnabled( const QBluetoothUuid &uuid ) const;

    /// Retrieve list of services.
    /**
     * @return  list of services
     */
    virtual uuidList services() const;

private:

    QMap<QBluetoothUuid, QString> names_;

    ///



};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPDTAGV2_H
