/**
 * @file devicelist.h
 * HFPD Device List Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICELIST_H
#define HFPD_DEVICELIST_H

#include "message.h"

#include <algorithm>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device List Message.
class deviceList : public message
{
    typedef deviceList _Myt;
    typedef message _Mybase;

public:

    commandResult result_;                          ///< Command result.
    int strength_;                                  ///< Advert tx strength.

    std::vector<std::string> address_;              ///< Device addresses.
    std::vector<int> rssi_;                         ///< Device rssi values.

    std::vector<int> batteryLevel_;                 ///< Device battery levels.
    std::vector<int> firmwareVersion_;              ///< Device firmware versions.
    std::vector<int> messages_;                     ///< Device number of unread messages.

    std::vector<std::string> operatorName_;         ///< Configured operator name.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    deviceList() :
        _Mybase( HFPD_DEV_LIST, 1 ),
        result_( HFPD_ERROR_SUCCESS ),
        strength_( HFPD_HIGH_POWER_ADVERT ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
    deviceList( const _Myt& other ) :
        _Mybase( other ),
        result_( other.result_ ),
        strength_( other.strength_ ),
        address_( other.address_ ),
        rssi_( other.rssi_ ),
        batteryLevel_( other.batteryLevel_ ),
        firmwareVersion_( other.firmwareVersion_ ),
        messages_( other.messages_ ),
        operatorName_( other.operatorName_ ) {}

    /// Destructor.
    virtual ~deviceList() {}

    // ========================================================================
    // Properties
    // ========================================================================

    virtual size_t size() const
    {
        size_t count( 0 );
        count = (std::max)( count, address_.size() );
        count = (std::min)( count, rssi_.size() );
        count = (std::min)( count, batteryLevel_.size() );
        count = (std::min)( count, firmwareVersion_.size() );
        count = (std::min)( count, messages_.size() );
        count = (std::min)( count, operatorName_.size() );

        return count;
    }

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

        // result
        if ( !parseValue( &s[idx], n-idx, result_, MSG_RESULT_LEN ) )
            return false;
        idx += MSG_RESULT_LEN;

        // result
        if ( !parseValue( &s[idx], n-idx, strength_, MSG_STRENGTH_LEN ) )
            return false;
        idx += MSG_STRENGTH_LEN;

        // number of devices
        size_t count;
        if ( !parseValue( &s[idx], n-idx, count, MSG_COUNT_LEN ) )
            return false;
        idx += MSG_COUNT_LEN;

        address_.clear();
        rssi_.clear();
        batteryLevel_.clear();
        firmwareVersion_.clear();
        messages_.clear();
        operatorName_.clear();

        while ( count-- )
        {
            std::string str;
            int val;

            // address
            if ( !parseString( &s[idx], n-idx, str ) )
                return false;
            address_.push_back( str );
            idx += str.length()+1;

            // rssi
            if ( !parseValue( &s[idx], n-idx, val, MSG_RSSI_LEN ) )
                return false;
            rssi_.push_back( val );
            idx += MSG_RSSI_LEN;

            // battery level
            if ( !parseValue( &s[idx], n-idx, val, MSG_BATTERY_LEVEL_LEN ) )
                return false;
            batteryLevel_.push_back( val );
            idx += MSG_BATTERY_LEVEL_LEN;

            // firmware version
            if ( !parseValue( &s[idx], n-idx, val, MSG_FIRMWARE_VER_LEN ) )
                return false;
            firmwareVersion_.push_back( val );
            idx += MSG_FIRMWARE_VER_LEN;

            // messages
            if ( !parseValue( &s[idx], n-idx, val, MSG_MESSAGES_LEN ) )
                return false;
            messages_.push_back( val );
            idx += MSG_MESSAGES_LEN;

            // operator name
            if ( !parseString( &s[idx], n-idx, str ) )
                return false;
            operatorName_.push_back( str );
            idx += str.length()+1;
        }

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
        serializeValue( msg, result_, MSG_RESULT_LEN );
        serializeValue( msg, strength_, MSG_STRENGTH_LEN );
        serializeValue( msg, size(), MSG_COUNT_LEN );

        for ( size_t i( 0 ); i < size(); ++i )
        {
            serializeString( msg, address_[i] );
            serializeValue( msg, rssi_[i], MSG_RSSI_LEN );
            serializeValue( msg, batteryLevel_[i], MSG_BATTERY_LEVEL_LEN );
            serializeValue( msg, firmwareVersion_[i], MSG_FIRMWARE_VER_LEN );
            serializeValue( msg, messages_[i], MSG_MESSAGES_LEN );
            serializeString( msg, operatorName_[i] );
        }

        return msg;
    }

private:

    enum {MSG_RESULT_LEN = 2};
    enum {MSG_STRENGTH_LEN = 2};
    enum {MSG_COUNT_LEN = 2};

    enum {MSG_RSSI_LEN = 1};
    enum {MSG_BATTERY_LEVEL_LEN = 1};
    enum {MSG_FIRMWARE_VER_LEN = 2};
    enum {MSG_MESSAGES_LEN = 1};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICELIST_H

