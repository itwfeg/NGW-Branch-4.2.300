/**
 * @file devicemsg.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "devicemsg.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
bool deviceMsg::valid( const QByteArray& data, deviceMsgType::type& device, unsigned int& event )
{
    bool okay = false;

    // get data length out to two fields
    unsigned int dataLen = calculateLength( data );

    if ( !dataLen )
        return false;

    // parse out fields
    QList<QByteArray> tokens = split( data.mid( PREFIX_LEN, dataLen - PREFIX_LEN ) );

    if ( tokens.size() < 2 )
        return false;

    // get device type
    device = (deviceMsgType::type) tokens[0].toUInt( &okay );

    if ( !okay )
        return false;

    // get event type
    event = tokens[1].toUInt( &okay );

    if ( !okay )
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int deviceMsg::calculateLength( const QByteArray& data )
{
    // must be more then prefix length
    if ( PREFIX_LEN < data.length() )
    {
        bool okay = false;

        // convert to int
        unsigned int len = data.left( PREFIX_LEN ).toUInt( &okay );

        // validate
        if (( okay ) && ( PREFIX_LEN < len ))
            return len;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString deviceMsg::decorateStringField( const QString& f )
{
    QString result = f;

    // change all " to ""...
    result.replace( "\"", "\"\"" );

    return ( "\"" + result + "\"" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString deviceMsg::undecorateStringField( const QString& f )
{
    QString result = f;

    unsigned int len = result.length();

    // remove leading and trailing indicator
    if ( len >= 2 )
        if (( TEXT_FIELD_INDICATOR == result[0] ) && ( TEXT_FIELD_INDICATOR == result[len-1] ))
            result = result.mid( 1, len-2 );

    // change all "" to "...
    result.replace( "\"\"", "\"" );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<QByteArray> deviceMsg::split( const QByteArray& data )
{
    const char delimiter = 255;

    bool withinTextField = false;

    QByteArray temp = data;

    // count delimiters until we reach field count
    for ( unsigned int i = 0; i < (unsigned int) temp.size(); ++i )
    {
        // do not count delimiters within a text field
        if ( TEXT_FIELD_INDICATOR == temp[i] )
            withinTextField = !withinTextField;
        else if ( !withinTextField )
        {
            if ( DELIMITER == temp[i] )
                temp[i] = delimiter;
        }
    }

    return temp.split( delimiter );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void deviceMsg::copy( const _Myt& rhs )
{
    // empty list
    // do _not_ call clear() here, results in segment fault! more mingw32 workarounds...
    while ( !empty() )
        pop_front();

    // copy list
    // do _not_ use a "foreach" loop here, results in segment fault! more mingw32 workarounds...
    for ( const_iterator i = rhs.begin(); i != rhs.end(); ++i )
        push_back( *i );
}
