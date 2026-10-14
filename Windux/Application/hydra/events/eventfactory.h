/**
 * @file eventfactory.h
 * @brief Event object factory creation/destruction.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef EVENTFACTORY_H
#define EVENTFACTORY_H

#include "events_global.h"

class Event;

class QByteArray;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Event object factory creation/destruction.
class EVENTS_EXPORT EventFactory
{
    using _Myt = EventFactory;

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Event factory creation.
    /**
     * @param[in] a  event data
     * @return  pointer to @c Event object, or @c nullptr on failure
     */
    static Event *create( const QByteArray& a );

    /// Event factory destruction.
    /**
     * @param[in] doomed  event to destroy
     */
    static void destroy( const Event *doomed );

private:

    // not implemented
    EventFactory() = delete;

    // not implemented
    EventFactory( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // EVENTFACTORY_H
