/**
 * @file hcidevthread.h
 * HCI Bluetooth device read thread.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HCIDEVTHREAD
#define HCIDEVTHREAD

#include "leadvertinfo.h"

#include <pthread.h>

#include <list>

class hcidev;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HCI Bluetooth device read thread.
class hcidevThread
{
    typedef hcidevThread _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] dev  device to read
     */
    hcidevThread( hcidev *dev );

    /// Destructor.
    virtual ~hcidevThread();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve event count.
    /**
     * @return  number of events queued
     */
    virtual size_t eventCount() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Pull event from queue.
    /**
     * @return  queued event
     */
    virtual leAdvertInfo event();

    /// Read loop.
    virtual void readLoop();

    /// Start thread.
    /**
     * @return  @c true if started succesfully, @c false otherwise
     */
    virtual bool start();

    /// Stop thread.
    virtual void stop();

protected:

    hcidev *dev_;                                   ///< HCI Device.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Process event.
    /**
     * @param[in] e  event
     * @param[in] len  event length
     */
    virtual void processEvent( const evt_le_meta_event *e, size_t len );

private:

    mutable pthread_mutex_t m_;

    std::list<leAdvertInfo> le_advert_info_;

    pthread_t thread_;

    bool done_;
    bool error_;
    bool running_;

    /// Retrieve RSSI.
    bool readRSSI( const bdaddr_t *bdaddr, int &rssi );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HCIDEVTHREAD

