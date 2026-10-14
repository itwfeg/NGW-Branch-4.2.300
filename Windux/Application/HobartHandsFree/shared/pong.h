/**
 * @file pong.h
 * HFPD Pong Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_PONG_H
#define HFPD_PONG_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Pong Message.
class pong : public message
{
    typedef pong _Myt;
    typedef message _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    pong() :
        _Mybase( HFPD_PONG, 1 ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    pong( const _Myt& other ) :
        _Mybase( other ) {}

    /// Destructor.
    virtual ~pong() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_PONG_H

