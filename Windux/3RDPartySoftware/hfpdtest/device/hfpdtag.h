/**
 * @file hfpdtag.h
 * Vantron Tag.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPDTAG_H
#define HFPDTAG_H

#include "bledevice.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Vantron HFPD Tag.
class hfpdTag : public bleDevice
{
    Q_OBJECT

    typedef hfpdTag _Myt;
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
    hfpdTag( const QBluetoothDeviceInfo &info, QObject *parent = 0 );

    /// Destructor.
    virtual ~hfpdTag();

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

    /// Retrieve list of services.
    /**
     * @return  list of services
     */
    virtual uuidList services() const;

private:

    QMap<QBluetoothUuid, QString> names_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPDTAG_H
