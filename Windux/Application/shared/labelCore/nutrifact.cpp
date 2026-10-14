/**
 * @file nutrifact.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "nutrifact.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
nutrifact::nutrifact() :
    _Mybase(),
    id_( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
nutrifact::nutrifact( int id ) :
    _Mybase(),
    id_( id )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
nutrifact::nutrifact( const _Myt& rhs ) :
    _Mybase()
{
    copy( rhs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
nutrifact::~nutrifact()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
nutrifact::_Myt& nutrifact::operator = ( const _Myt& rhs )
{
    copy( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
nutrifact::key_list_type nutrifact::toKeyList( const tagDataList& r )
{
    LOG_CLASS_INFO( "nutrifact", "toKeyList" );
    key_list_type keys;

    // process all tags
    for ( tagDataList::const_iterator i = r.constBegin(); i != r.constEnd(); ++i )
    {
        switch ( i->first )
        {
        case tagDataFunctionCode::NUTRIFACT_NUMBER:
            keys.push_back( i->second.toInt() );
            break;
        default:
            LOG_WARN( "unhandled function code %d", i->first );
            break;
        }
    }

    LOG_INFO( "have %d keys", keys.size() );
    return keys;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
nutrifact::list_type nutrifact::toValueList( const tagDataList& r )
{
    LOG_CLASS_INFO( "nutrifact", "toValueList" );

    LOG_DEBUG( "processing tag data list of size %d", r.size() );
    list_type values;

    _Myt temp;
    bool first = true;

    for ( tagDataList::const_iterator i = r.constBegin(); i != r.constEnd(); ++i )
    {
        switch ( i->first )
        {
        case tagDataFunctionCode::NUTRIFACT_NUMBER:

            // add to list
            if ( first )
                first = false;
            else if ( 0 >= temp.id() )
            {
                LOG_WARN( "have bad nutrifact %d", temp.id() );
                temp = _Myt();
            }
            else
            {
                LOG_DEBUG( "have nutrifact %d with %d columns",
                    temp.id(),
                    temp.columns().size() );

                values.push_back( temp );
                temp = _Myt();
            }

            temp.setId( i->second.toInt() );
            break;
        case tagDataFunctionCode::NUTRITION_COLUMN1:
            temp.setColumn( 0, nutrifactColumn( i->second ) );
            break;
        case tagDataFunctionCode::NUTRITION_COLUMN2:
            temp.setColumn( 1, nutrifactColumn( i->second ) );
            break;
        case tagDataFunctionCode::NUTRITION_COLUMN3:
            temp.setColumn( 2, nutrifactColumn( i->second ) );
            break;
        case tagDataFunctionCode::NUTRITION_COLUMN4:
            temp.setColumn( 3, nutrifactColumn( i->second ) );
            break;
        default:
            LOG_WARN( "unhandled function code %d", i->first );
            break;
        }
    }

    // add to list
    if ( 0 >= temp.id() )
        LOG_WARN( "have bad nutrifact %d", temp.id() );
    else
    {
        LOG_DEBUG( "have nutrifact %d with %d columns",
            temp.id(),
            temp.columns().size() );

        values.push_back( temp );
    }

    LOG_INFO( "have %d nutrifacts", values.size() );
    return values;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecordList nutrifact::toTagDataRecordList( const list_type& values )
{
    LOG_CLASS_INFO( "nutrifact", "toTagDataRecordList" );

    LOG_INFO( "processing value list of size %d", values.size() );
    tagDataRecordList rl;

    for ( list_type::const_iterator i = values.constBegin(); i != values.constEnd(); ++i )
    {
        if ( 0 >= i->id() )
            LOG_WARN( "have bad nutrifact %d", i->id() );
        else
        {
            tagDataRecord r( tagDataCommand::toRecordType<nutrifact, tagDataCommand::WRITE>::TYPE );
            static const QString format( "%1" );

            LOG_DEBUG( "have nutrifact %d with %d columns",
                i->id(),
                i->columns().size() );

            r.second.push_back( tagData( tagDataFunctionCode::NUTRIFACT_NUMBER, format.arg( i->id() ) ) );

            if ( i->columns().contains( 0 ) )
                r.second.push_back( tagData( tagDataFunctionCode::NUTRITION_COLUMN1, i->column( 0 ).data() ) );
            if ( i->columns().contains( 1 ) )
                r.second.push_back( tagData( tagDataFunctionCode::NUTRITION_COLUMN2, i->column( 1 ).data() ) );
            if ( i->columns().contains( 2 ) )
                r.second.push_back( tagData( tagDataFunctionCode::NUTRITION_COLUMN3, i->column( 2 ).data() ) );
            if ( i->columns().contains( 3 ) )
                r.second.push_back( tagData( tagDataFunctionCode::NUTRITION_COLUMN4, i->column( 3 ).data() ) );

            rl.push_back( r );
        }
    }

    LOG_DEBUG( "have %d records", rl.size() );
    return rl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void nutrifact::copy( const _Myt& rhs )
{
    id_ = rhs.id_;
    columns_ = rhs.columns_;
}

