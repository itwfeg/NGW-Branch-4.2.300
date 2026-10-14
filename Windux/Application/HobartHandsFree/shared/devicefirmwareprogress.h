/**
 * @file devicefirmwareprogress.h
 * HFPD Device Firmware Progress.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICEFIRMWAREPROGRESS_H
#define HFPD_DEVICEFIRMWAREPROGRESS_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device Firmware Progress.
class deviceFirmwareProgress : public message
{
    typedef deviceFirmwareProgress _Myt;
    typedef message _Mybase;

public:

    std::string address_;                           ///< Device address.

    bool start_;                                    ///< Firmware upgrade starting.
    bool finish_;                                   ///< Firmware upgrade finished.

    unsigned int progress_;                         ///< Firmware upgrade progress.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceFirmwareProgress() :
        _Mybase( HFPD_DEV_FIRMWARE_PROGRESS, 1 ),
        start_( false ),
        finish_( false ),
        progress_( 0 ) {}

    /// Constructor.
    /**
     * @param[in] address  device address
     */
    deviceFirmwareProgress( const std::string& address, bool start, bool finish, unsigned int progress ) :
        _Mybase( HFPD_DEV_FIRMWARE_PROGRESS, 1 ),
        address_( address ),
        start_( start ),
        finish_( finish ),
        progress_( progress ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceFirmwareProgress( const _Myt& other ) :
        _Mybase( other ),
        address_( other.address_ ),
        start_( other.start_ ),
        finish_( other.finish_ ),
        progress_( other.progress_ ) {}

    /// Destructor.
    virtual ~deviceFirmwareProgress() {}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Parse message.
    /**
     * @param[in] s  pointer to array of characters
     * @param[in] n  number of characters in array
     * @param[out] len  characters used
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool parse( const char *s, size_t n, size_t *len )
    {
        size_t idx( 0 );

        if ( !_Mybase::parse( s, n, &idx ) )
            return false;

        int val;

        // address
        if ( !parseString( &s[idx], n-idx, address_ ) )
            return false;
        idx += address_.length()+1;

        // start
        if ( !parseValue( &s[idx], n-idx, val, MSG_START_LEN ) )
            return false;
        start_ = val ? true : false;
        idx += MSG_START_LEN;

        // finish
        if ( !parseValue( &s[idx], n-idx, val, MSG_FINISH_LEN ) )
            return false;
        finish_ = val ? true : false;
        idx += MSG_FINISH_LEN;

        // progress
        if ( !parseValue( &s[idx], n-idx, progress_, MSG_PROGRESS_LEN ) )
            return false;
        idx += MSG_PROGRESS_LEN;

        (*len) = idx;
        return true;
    }

    /// Serialize message.
    /**
     * @return  serialized message
     */
    virtual std::string serialize() const
    {
        std::string msg( _Mybase::serialize() );
        serializeString( msg, address_ );
        serializeValue( msg, start_ ? 1 : 0, MSG_START_LEN );
        serializeValue( msg, finish_ ? 1 : 0, MSG_FINISH_LEN );
        serializeValue( msg, progress_, MSG_PROGRESS_LEN );

        return msg;
    }

private:

    enum {MSG_START_LEN = 1};
    enum {MSG_FINISH_LEN = 1};
    enum {MSG_PROGRESS_LEN = 2};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICEFIRMWAREPROGRESS_H

