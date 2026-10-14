/**
 * @file hciscanneragentthread.h
 * HCI Bluetooth device read thread.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HCISCANNERAGENTTHREAD
#define HCISCANNERAGENTTHREAD

#include "leadvertinfo.h"

#include <QThread>

class hciScannerAgent;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HCI Bluetooth device read thread.
class hciScannerAgentThread : public QThread
{
    typedef hciScannerAgentThread _Myt;
    typedef QThread _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    hciScannerAgentThread( QObject *parent = 0 );

    /// Destructor.
    virtual ~hciScannerAgentThread();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Stop thread.
    virtual void stop();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Process event.
    /**
     * @param[in] e  event
     * @param[in] len  event length
     */
    virtual void processEvent( const evt_le_meta_event *e, size_t len );

    /// Thread run method.
    virtual void run();

private:

    hciScannerAgent *dev_;

    int fd_;

    /// Retrieve RSSI.
    bool readRSSI( const bdaddr_t *bdaddr, int& rssi );

    // not implemented
    hciScannerAgentThread( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HCISCANNERAGENTTHREAD

