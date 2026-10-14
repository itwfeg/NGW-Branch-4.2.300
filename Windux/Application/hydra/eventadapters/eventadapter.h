/**
 * @file eventadapter.h
 * @brief Abstract event adapter for socket comms.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef EVENTADAPTER_H
#define EVENTADAPTER_H

#include "eventadapters_global.h"

#include <QList>

class Event;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract adapter for socket comms.
class EVENTADAPTERS_EXPORT EventAdapter
{
    using _Myt = EventAdapter;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~EventAdapter();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Destroy allocated events.
    /**
     * @param[in] doomed  list of events
     */
    virtual void destroyEvents( const QList<Event*>& doomed ) = 0;

    /// Translate comm data into events.
    /**
     * @param[in,out] buffer  comm data
     * @param[out] valid  translation result
     * @return  list of events
     */
    virtual QList<Event*> translateComm( QByteArray& buffer, bool *valid = nullptr ) = 0;

    /// Translate event into comm data.
    /**
     * @param[in] e  event
     * @param[out] valid  translation result
     * @return  comm data
     */
    virtual QByteArray translateEvent( const Event *e, bool *valid = nullptr ) = 0;

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    EventAdapter();

private:

    // not implemented
    EventAdapter( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // EVENTADAPTER_H
