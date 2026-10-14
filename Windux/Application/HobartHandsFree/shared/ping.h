/**
 * @file ping.h
 * HFPD Ping Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_PING_H
#define HFPD_PING_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Ping Message.
class ping : public message
{
    typedef ping _Myt;
    typedef message _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    ping() :
        _Mybase( HFPD_PING, 1 ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    ping( const _Myt& other ) :
        _Mybase( other ) {}

    /// Destructor.
    virtual ~ping() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_PING_H

