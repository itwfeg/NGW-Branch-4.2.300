/**
 * @file devicediscovery.h
 * Edgify/Tiliter Device Discovery.
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DEVICEDISCOVERY_H
#define DEVICEDISCOVERY_H

#include <QByteArray>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QStringList>

class QUdpSocket;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Edgify/Tiliter Device Discovery.
class DeviceDiscovery : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QStringList devices READ devices NOTIFY devicesChanged )

    typedef DeviceDiscovery _Myt;
    typedef QObject _Mybase;

signals:

    /// Signal for devices changes.
    void devicesChanged();

public:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve list of devices.
    /**
     * @return  device listing
     */
    QStringList devices() const {return devices_;}

    /// Retrieve device hostname.
    /**
     * @param[in] device  device
     * @return  hostname
     */
    QString hostname( const QString& device ) const;

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve singleton instance.
    /**
     * @return  instance pointer
     */
    static _Myt *instance();

private slots:

    /// Slot for reading pending datagram packets.
    void readPendingDatagrams();

private:

    enum {UDP_PORT = 5091};
    enum {SM_BACKEND_PORT = 5076};

    static QMutex instanceMutex_;
    static _Myt *instance_;

    QUdpSocket *sock_;

    QStringList devices_;
    QStringList others_;

    QMap<QString, QString> hostnames_;

    /// Constructor.
    DeviceDiscovery();

    // not implemented
    DeviceDiscovery( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DEVICEDISCOVERY_H
