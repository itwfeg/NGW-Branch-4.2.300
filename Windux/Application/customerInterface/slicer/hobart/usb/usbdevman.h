/**
 * @file usbdevman.h
 * USB Device Manager Singleton.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef USBDEVMAN_H
#define USBDEVMAN_H

#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QThread>

#include <libusb.h>

class usbDev;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// USB Device Manager Singleton.
class usbDevManager : public QThread
{
    Q_OBJECT

    typedef usbDevManager _Myt;
    typedef QThread _Mybase;

    friend int libusb_hotplug_callback( libusb_context *ctx, libusb_device *device, libusb_hotplug_event event, void *user_data );

Q_SIGNALS:

    /// Device arrived signal.
    /**
     * @param d[in,out]  device
     */
    void deviceArrived( usbDev *d );

    /// Device left signal.
    /**
     * Called before device destroyed.
     *
     * @param d[in,out]  device
     */
    void deviceLeft( usbDev *d );

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    ~usbDevManager();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve context.
    /**
     * @return  context
     */
    libusb_context *context() const {return context_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Retrieve attached devices of type @p T.
    /**
     * @return  list of attached devices
     */
    template <class T>
    QList<usbDev*> attachedDevices() const
    {
        QList<usbDev*> devices;
        T *d;

        QMutexLocker guard( &instanceMutex_ );

        // iterate devices
        for ( deviceMap::const_iterator i( devices_.begin() ); i != devices_.end(); i++ )
            if ( (d = dynamic_cast<T*>( i.value() )) )
                devices.push_back( d );

        return devices;
    }

    /// Retrieve attached devices.
    /**
     * @return  list of attached devices
     */
    QList<usbDev*> attachedDevices() const;

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

    typedef QMap<libusb_device*, usbDev*> deviceMap;

    static QMutex instanceMutex_;
    static usbDevManager *instance_;

    libusb_context *context_;
    libusb_hotplug_callback_handle callbackHandle_;

    deviceMap devices_;

    bool terminate_;

    /// Constructor.
    usbDevManager();

    /// Device arrived event.
    void processDeviceArrived( libusb_device *device );

    /// Device left event.
    void processDeviceLeft( libusb_device *device );

    // not implemented
    usbDevManager( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // USBDEVMAN_H
