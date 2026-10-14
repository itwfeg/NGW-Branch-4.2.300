/**
 * @file graphic.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "graphic.h"

#include <QFile>
#include <QDir>

///////////////////////////////////////////////////////////////////////////////////////////////////
graphic::graphic() :
    _Mybase()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
graphic::graphic( const QString& name ) :
    _Mybase(),
    name_( name )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
graphic::graphic( const QString& name, const QImage& image ) :
    _Mybase(),
    name_( name ),
    image_( image )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
graphic::graphic( const QString& name, const QString& data, base16_encoding_t ) :
    _Mybase(),
    name_( name )
{
    parseImage( data, base16_encoding );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
graphic::graphic( const QString& name, const QString& data, base64_encoding_t ) :
    _Mybase(),
    name_( name )
{
    parseImage( data, base64_encoding );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
graphic::graphic( const _Myt& rhs ) :
    _Mybase()
{
    copy( rhs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
graphic::~graphic()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
graphic::_Myt& graphic::operator = ( const _Myt& rhs )
{
    copy( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString graphic::objectKey() const
{
    int index;

    // use graphic name as the object key, but remove any file extension
    if ( 0 < (index = name_.lastIndexOf( "." )) )
        return name_.left( index );

    return name_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString graphic::serializeImage( base16_encoding_t ) const
{
    return imageData().toHex();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString graphic::serializeImage( base64_encoding_t ) const
{
    return imageData().toBase64();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void graphic::parseImage( const QString& data, base16_encoding_t )
{
    setImageData( QByteArray::fromHex( data.toLatin1() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void graphic::parseImage( const QString& data, base64_encoding_t )
{
    setImageData( QByteArray::fromBase64( data.toLatin1() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
graphic::key_list_type graphic::toKeyList( const tagDataList& r )
{
    LOG_CLASS_INFO( "graphic", "toKeyList" );
    key_list_type keys;

    // process all tags
    for ( tagDataList::const_iterator i = r.constBegin(); i != r.constEnd(); ++i )
    {
        switch ( i->first )
        {
        case tagDataFunctionCode::FILENAME:
            keys.push_back( i->second );
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
graphic::list_type graphic::toValueList( const tagDataList& r )
{
    LOG_CLASS_INFO( "graphic", "toValueList" );

    LOG_DEBUG( "processing tag data list of size %d", r.size() );
    list_type values;

    _Myt temp;

    for ( tagDataList::const_iterator i = r.constBegin(); i != r.constEnd(); ++i )
    {
        switch ( i->first )
        {
        case tagDataFunctionCode::FILENAME:
            temp.setName( i->second );
            break;
        case tagDataFunctionCode::RESOURCE:
            temp.parseImage( i->second, base16_encoding );

            // add to list
            if (( !temp.name().length() ) || ( temp.image().isNull() ))
            {
                LOG_WARN( "have bad graphic name '%s'", temp.name().toLatin1().constData() );
                temp = _Myt();
            }
            else
            {
                LOG_DEBUG( "have graphic '%s' of dimensions %d,%d",
                    temp.name().toLatin1().constData(),
                    temp.size().width(),
                    temp.size().height() );

                values.push_back( temp );
                temp = _Myt();
            }

            break;
        default:
            LOG_WARN( "unhandled function code %d", i->first );
            break;
        }
    }

    LOG_INFO( "have %d graphics", values.size() );
    return values;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecordList graphic::toTagDataRecordList( const list_type& values )
{
    LOG_CLASS_INFO( "graphic", "toTagDataRecordList" );

    LOG_INFO( "processing value list of size %d", values.size() );
    tagDataRecordList rl;

    for ( list_type::const_iterator i = values.constBegin(); i != values.constEnd(); ++i )
    {
        if (( !i->name().length() ) || ( i->image().isNull() ))
            LOG_WARN( "have bad graphic name '%s'", i->name().toLatin1().constData() );
        else
        {
            tagDataRecord r( tagDataCommand::toRecordType<graphic, tagDataCommand::WRITE>::TYPE );

            LOG_DEBUG( "have graphic '%s' of dimensions %d,%d",
                i->name().toLatin1().constData(),
                i->size().width(),
                i->size().height() );

            r.second.push_back( tagData( tagDataFunctionCode::FILENAME, i->name() ) );
            r.second.push_back( tagData( tagDataFunctionCode::RESOURCE, i->serializeImage( base16_encoding ) ) );
            rl.push_back( r );
        }
    }

    LOG_DEBUG( "have %d records", rl.size() );
    return rl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void graphic::copy( const _Myt& rhs )
{
    name_ = rhs.name_;
    image_ = rhs.image_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray graphic::imageData() const
{
    LOG_CLASS_INFO( "graphic", "imageData" );

    QString extension( "png" );
    int index;

    LOG_TRACE( "looking for graphic type...", NULL );

    // check for graphic name extension
    if ( 0 < (index = name_.lastIndexOf( "." )) )
    {
        int length = name_.length() - (index+1);

        if ( 3 <= length )
            extension = name_.right( length );
    }

    LOG_DEBUG( "saving as type '%s'", extension.toLatin1().constData() );

    QByteArray a;
    QDataStream os( &a, QIODevice::WriteOnly );

    if ( !image_.save( os.device(), extension.toLatin1().constData() ) )
        LOG_WARN( "could not save image!", NULL );

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void graphic::setImageData( const QByteArray& data )
{
    LOG_CLASS_INFO( "graphic", "setImageData" );

    if ( !image_.loadFromData( data ) )
        LOG_WARN( "could not load image!", NULL );
    else
    {
        LOG_DEBUG( "have graphic '%s' of width %d height %d",
            name_.toLatin1().constData(),
            image_.size().width(),
            image_.size().height() );
    }
}
