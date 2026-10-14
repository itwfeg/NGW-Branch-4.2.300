/**
 * @file devicelist.h
 * HFPD Device List Message.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HFPD_DEVICEOPNAMELIST_H
#define HFPD_DEVICEOPNAMELIST_H

#include "message.h"

#include <algorithm>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFPD Device List Message.
class deviceOpNameList : public message
{
    typedef deviceOpNameList _Myt;
    typedef message _Mybase;

public:

    commandResult result_;                          ///< Command result.

    std::vector<std::string> address_;              ///< Device address.
    std::vector<std::string> opName_;               ///< Device operator name.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
	deviceOpNameList() :
        _Mybase( HFPD_DEV_OPNAME_LIST, 1 ),
        result_( HFPD_ERROR_SUCCESS ) {}

    /// Constructor.
    /**
     * @param[in] other  message
     */
	deviceOpNameList( const _Myt& other ) :
        _Mybase( other ),
        result_( other.result_ ),
        address_( other.address_ ),
        opName_( other.opName_ ) {}

    /// Destructor.
    virtual ~deviceOpNameList() {}

    // ========================================================================
    // Properties
    // ========================================================================

    virtual size_t size() const
    {
        size_t count( 0 );
        count = (std::max)( count, address_.size() );
        count = (std::max)( count, opName_.size() );

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

        // number of devices
        size_t count;
        if ( !parseValue( &s[idx], n-idx, count, MSG_COUNT_LEN ) )
            return false;
        idx += MSG_COUNT_LEN;

        address_.clear();
		opName_.clear();

        while ( count-- )
        {
            std::string str;

            // address
            if ( !parseString( &s[idx], n-idx, str ) )
                return false;
            address_.push_back( str );
            idx += str.length()+1;

            // opName
            if ( !parseString( &s[idx], n - idx, str ) )
				return false;
            opName_.push_back( str );
			idx += str.length() + 1;
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
        serializeValue( msg, size(), MSG_COUNT_LEN );

        for ( size_t i( 0 ); i < size(); ++i )
        {
            serializeString( msg, address_[i] );
			serializeString( msg, opName_[i] );
        }

        return msg;
    }

private:

    enum {MSG_RESULT_LEN = 2};
    enum {MSG_COUNT_LEN = 2};

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFPD_DEVICEOPNAMELIST_H

