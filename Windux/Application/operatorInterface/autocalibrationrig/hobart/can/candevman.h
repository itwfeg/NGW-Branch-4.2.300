/**
 * @file candevman.h
 * CAN Device Manager Singleton.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CANDEVMAN_H
#define CANDEVMAN_H

#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>

class canDev;
class canSocket;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// CAN Device Manager Singleton.
class canDevManager : public QObject
{
    Q_OBJECT

    typedef canDevManager _Myt;
    typedef QObject _Mybase;

Q_SIGNALS:

    /// Device arrived signal.
    /**
     * @param d[in,out]  device
     */
    void deviceArrived( canDev *d );

    /// Device left signal.
    /**
     * Called before device destroyed.
     *
     * @param d[in,out]  device
     */
    void deviceLeft( canDev *d );

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    ~canDevManager();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve list of CAN interfaces.
    /**
     * @return  list of CAN interfaces
     */
    QStringList interfaces() const;

    /// Set CAN interfaces.
    /**
     * @param[in] value  list of CAN interfaces
     */
    void setInterfaces( const QStringList &value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Retrieve attached devices of type @p T.
    /**
     * @return  list of attached devices
     */
    template <class T>
    QList<canDev*> attachedDevices() const
    {
        QList<canDev*> devices;
        T *d;

        QMutexLocker guard( &instanceMutex_ );

        // iterate devices
        foreach ( canDev *dev, devices_ )
            if ( (d = dynamic_cast<T*>( dev )) )
                devices.push_back( d );

        return devices;
    }

    /// Retrieve attached devices.
    /**
     * @return  list of attached devices
     */
    QList<canDev*> attachedDevices() const;

    /// Create device.
    /**
     * @param[in] interface  interface device resides on
     * @return  device pointer
     */
    template <class T>
    T *createDevice( const QString &interface )
    {
        if ( !interfaces_.contains( interface ) )
            return NULL;

        // create device
        T *d( new T( interfaces_[interface] ) );
        devices_.append( d );

        connect( d, SIGNAL(destroyed(QObject*)), SLOT(onDeviceDestroyed(QObject*)) );

        Q_EMIT deviceArrived( d );

        return d;
    }

    /// Lock events.
    void lockEvents();

    /// Unlock events.
    void unlockEvents();

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve object singleton instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Thread run method.
    void run();

private Q_SLOTS:

    /// Slot for device destroyed.
    void onDeviceDestroyed( QObject *d );

private:

    typedef QMap<QString, canSocket*> interfaceMap;

    typedef QList<canDev*> deviceList;

    static QMutex instanceMutex_;
    static canDevManager *instance_;

    interfaceMap interfaces_;
    deviceList devices_;

    /// Constructor.
    canDevManager();

    // not implemented
    canDevManager( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CANDEVMAN_H
