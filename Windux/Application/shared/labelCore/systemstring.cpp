/**
 * @file systemstring.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "systemstring.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
systemString::systemString() :
    _Mybase(),
    id_( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
systemString::systemString( int id, const QString& text ) :
    _Mybase(),
    id_( id ),
    text_( text )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
systemString::systemString( const _Myt& rhs ) :
    _Mybase()
{
    copy( rhs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
systemString::~systemString()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
systemString::_Myt& systemString::operator = ( const _Myt& rhs )
{
    copy( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
systemString::list_type systemString::toValueList( const tagDataList& r )
{
    LOG_CLASS_INFO( "systemString", "toValueList" );

    LOG_DEBUG( "processing tag data list of size %d", r.size() );
    list_type values;

    _Myt temp;

    for ( tagDataList::const_iterator i = r.constBegin(); i != r.constEnd(); ++i )
    {
        switch ( i->first )
        {
        case tagDataFunctionCode::SYSTEM_STRING_NUMBER:
            temp.setId( i->second.toInt() );
            break;
        case tagDataFunctionCode::SYSTEM_STRING:
            temp.setText( i->second );

            // add to list
            if ( 0 >= temp.id() )
            {
                LOG_WARN( "bad system string id %d", temp.id() );
                temp = _Myt();
            }
            else
            {
                LOG_DEBUG( "have system string %d '%s'",
                    temp.id(),
                    temp.text().toLatin1().constData() );

                values.push_back( temp );
                temp = _Myt();
            }

            break;
        default:
            LOG_WARN( "unhandled function code %d", i->first );
            break;
        }
    }

    LOG_INFO( "have %d system strings", values.size() );
    return values;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecordList systemString::toTagDataRecordList( const list_type& values )
{
    LOG_CLASS_INFO( "systemString", "toTagDataRecordList" );

    LOG_INFO( "processing value list of size %d", values.size() );
    tagDataRecordList rl;

    tagDataRecord r( tagDataCommand::toRecordType<systemString, tagDataCommand::WRITE>::TYPE );

    for ( list_type::const_iterator i = values.constBegin(); i != values.constEnd(); ++i )
    {
        if ( 0 >= i->id() )
            LOG_WARN( "bad object id %d", i->id() );
        else
        {
            static const QString format( "%1" );

            LOG_DEBUG( "have system string %d '%s'",
                i->id(),
                i->text().toLatin1().constData() );

            r.second.push_back( tagData( tagDataFunctionCode::SYSTEM_STRING_NUMBER, format.arg( i->id() ) ) );
            r.second.push_back( tagData( tagDataFunctionCode::SYSTEM_STRING, i->text() ) );
        }
    }

    if ( r.second.size() )
        rl.push_back( r );

    LOG_DEBUG( "have %d records", rl.size() );
    return rl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void systemString::copy( const _Myt& rhs )
{
    id_ = rhs.id_;
    text_ = rhs.text_;
}
