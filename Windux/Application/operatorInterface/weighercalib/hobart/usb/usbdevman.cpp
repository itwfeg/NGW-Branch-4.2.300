/**
 * @file usbdevman.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "usbdev.h"
#include "usbdevfactory.h"
#include "usbdevman.h"

#include <iostream>
#include <iomanip>

#include <QCoreApplication>

QMutex usbDevManager::instanceMutex_( QMutex::Recursive );
usbDevManager *usbDevManager::instance_( NULL );

///////////////////////////////////////////////////////////////////////////////////////////////////
int libusb_hotplug_callback( libusb_context *ctx, libusb_device *device, libusb_hotplug_event event, void *user_data )
{
    usbDevManager *instance( (usbDevManager*) user_data );

    // validate instance
    if ( !instance )
        return 1;
    else if ( ctx != instance->context() )
        return 0;

    std::cout << "process hotplug event..." << std::endl;

    // handle event
    switch ( event )
    {
    case LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED:
        instance->processDeviceArrived( device );
        break;

    case LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT:
        instance->processDeviceLeft( device );
        break;

    default:
        break;
    }

    std::cout << "hotplug event complete" << std::endl;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
usbDevManager::usbDevManager() :
    _Mybase(),
    context_( NULL ),
    callbackHandle_( 0 ),
    terminate_( false )
{
    const libusb_hotplug_event hotplug_events( (libusb_hotplug_event) (
        LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
        LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT ) );

    int result;

    std::cout << "usbDevManager CTOR..." << std::endl;

    // init libusb
    result = libusb_init( &context_ );

    if (( LIBUSB_SUCCESS != result ) || ( !context_ ))
    {
        std::cerr << "libusb_init failed rc=" << result << std::endl;
        return;
    }

    // setup debug level
    libusb_set_debug( context_, LIBUSB_LOG_LEVEL_WARNING );

    // register hotplug callback
    result = libusb_hotplug_register_callback(
        context_,
        hotplug_events,
        LIBUSB_HOTPLUG_ENUMERATE,
        LIBUSB_HOTPLUG_MATCH_ANY,
        LIBUSB_HOTPLUG_MATCH_ANY,
        LIBUSB_HOTPLUG_MATCH_ANY,
        libusb_hotplug_callback,
        this,
        &callbackHandle_ );

    if ( LIBUSB_SUCCESS != result )
    {
        libusb_exit( context_ );
        context_ = NULL;

        std::cerr << "libusb_hotplug_register_callback failed rc=" << result << std::endl;
        return;
    }

    // start event loop
    start();

    std::cout << "usbDevManager CTOR complete" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
usbDevManager::~usbDevManager()
{
    std::cout << "usbDevManager DTOR..." << std::endl;

    // stop event loop
    terminate_ = true;

    do
    {
        msleep( 25 );

    } while ( isRunning() );

    // deregister callback
    libusb_hotplug_deregister_callback( context_, callbackHandle_ );

    QMutexLocker guard( &instanceMutex_ );

    // destroy all devices
    std::cout << "destroying all devices..." << std::endl;

    QList<libusb_device*> devices( devices_.keys() );

    foreach ( libusb_device *device, devices )
        processDeviceLeft( device );

    // exit libusb
    libusb_exit( context_ );
    context_ = NULL;

    std::cout << "usbDevManager DTOR complete" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<usbDev*> usbDevManager::attachedDevices() const
{
    QMutexLocker guard( &instanceMutex_ );
    return devices_.values();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void usbDevManager::lockEvents()
{
    libusb_lock_events( context_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void usbDevManager::unlockEvents()
{
    libusb_unlock_events( context_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
usbDevManager *usbDevManager::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
        {
            std::cout << "creating usbDevManager..." << std::endl;
            instance_ = new usbDevManager();
        }
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void usbDevManager::run()
{
    int result;

    // 50ms timeout value
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 50000;

    do
    {
        // handle libusb events
        if ( LIBUSB_SUCCESS != (result = libusb_handle_events_timeout_completed( context_, &tv, NULL )) )
        {
            std::cerr << "libusb_handle_events failed rc=" << result << std::endl;
            break;
        }

        // handle qt events
        QCoreApplication::processEvents();

    } while ( !terminate_ );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void usbDevManager::onDeviceDestroyed( QObject *d )
{
    QMutexLocker guard( &instanceMutex_ );

    std::cout << "remove device 0x" << std::uppercase << std::setfill('0') << std::setw( 8 ) << std::hex << d << " from map" << std::endl << std::dec;

    // remove device from map
    for ( deviceMap::iterator i( devices_.begin() ); i != devices_.end(); i++ )
        if ( d == i.value() )
        {            
            devices_.remove( i.key() );

            std::cout << "device removed" << std::endl;
            break;
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void usbDevManager::processDeviceArrived( libusb_device *device )
{
    usbDev *d;

    std::cout << "have new device..." << std::endl;

    // create object
    if ( (d = usbDevFactory::create( device )) )
    {
        QMutexLocker guard( &instanceMutex_ );

        std::cout << "adding device 0x" << std::uppercase << std::setfill('0') << std::setw( 8 ) << std::hex << d << " to map" << std::endl << std::dec;
        devices_[device] = d;

        connect( d, SIGNAL(destroyed(QObject*)), SLOT(onDeviceDestroyed(QObject*)) );

        std::cout << "emit arrived signal..." << std::endl;
        Q_EMIT deviceArrived( d );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void usbDevManager::processDeviceLeft( libusb_device *device )
{
    QMutexLocker guard( &instanceMutex_ );

    std::cout << "device left..." << std::endl;

    // destroy object
    if ( devices_.contains( device ) )
    {
        usbDev *d;

        // destroy device
        if ( (d = devices_[device]) )
        {
            std::cout << "emit left signal..." << std::endl;
            Q_EMIT deviceLeft( d );

            std::cout << "destroy device 0x" << std::uppercase << std::setfill('0') << std::setw( 8 ) << std::hex << d << "..." << std::endl << std::dec;
            delete d;
        }
    }
}
