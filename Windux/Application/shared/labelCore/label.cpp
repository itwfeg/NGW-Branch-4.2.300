/**
 * @file label.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "label.h"
#include "labelformtype.h"

const QSize label::defaultSize_( 240, 240 );

///////////////////////////////////////////////////////////////////////////////////////////////////
label::label() :
    _Mybase(),
    versionMajor_( 1 ),
    versionMinor_( 0 ),
    id_( 0 ),
    loaded_( true ),
    size_( defaultSize_ ),
    rotation_( 0 ),
    skew_( 0 ),
    customerVersion_("N/A")
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
label::label( int id ) :
    _Mybase(),
    versionMajor_( 1 ),
    versionMinor_( 0 ),
    id_( id ),
    loaded_( true ),
    size_( defaultSize_ ),
    rotation_( 0 ),
    skew_( 0 ),
    customerVersion_("N/A")
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
label::label( const QString& data, base16_encoding_t ) :
    _Mybase()
{
    parse( data, base16_encoding );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
label::label( const QString& data, base64_encoding_t ) :
    _Mybase()
{
    parse( data, base64_encoding );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
label::label( QDataStream& is ) :
    _Mybase()
{
    parse( is );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
label::label( const _Myt& rhs ) :
    _Mybase()
{
    copy( rhs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
label::~label()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
label::_Myt& label::operator = ( const _Myt& rhs )
{
    copy( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString label::customerVersion() const
{
    foreach ( const labelForm& form, forms_ )
        if ( labelFormType::CUSTOMER_LABEL_VERSION_NUM == form.type() )
            return form.data();

    return "N/A";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize label::physicalSize() const
{
    if (( 90 == rotation() ) || ( 270 == rotation() ))
        return QSize( size().height(), size().width() );

    return size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString label::serialize( base16_encoding_t ) const
{
    QByteArray a;
    QDataStream os( &a, QIODevice::WriteOnly );

    os.setByteOrder( QDataStream::BigEndian );
    serialize( os );

    return a.toHex();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString label::serialize( base64_encoding_t ) const
{
    QByteArray a;
    QDataStream os( &a, QIODevice::WriteOnly );

    os.setByteOrder( QDataStream::BigEndian );
    serialize( os );

    return a.toBase64();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void label::serialize( QDataStream& os ) const
{
    LOG_CLASS_INFO( "label", "serialize" );

    // must have attached device
    if ( !os.device() )
    {
        LOG_WARN( "no device attached to stream!", NULL );
        return;
    }
    // device must be writable
    else if ( !os.device()->isWritable() )
    {
        LOG_WARN( "stream is not writable!", NULL );
        return;
    }
    // device must be random access
    else if ( os.device()->isSequential() )
    {
        LOG_WARN( "stream is not random access!", NULL );
        return;
    }

    qint8 i8;
    qint16 i16;
    qint32 i32;

    QByteArray str;

    // save starting offset
    int offset = os.device()->pos();

    LOG_DEBUG( "serializing label...", NULL );

    // major version
    i8 = versionMajor_;
    os << i8;

    // minor version
    i8 = versionMinor_;
    os << i8;

    // type
    str = "LABL";
    os.writeRawData( str.constData(), 4 );

    // id
    i16 = id_;
    os << i16;

    // loaded flag
    i8 = (loaded_ ? 1 : 0);
    os << i8;

    // padding
    i8 = 0;
    os << i8;

    int offsetLength = os.device()->pos();

    // length (filled in later)
    i16 = 0;
    os << i16;

    // name
    str = name_.toLatin1();

    while ( 32 > str.size() )
        str.append( '\0' );

    os.writeRawData( str.constData(), 32 );

    // width
    i16 = size_.width();
    os << i16;

    // height
    i16 = size_.height();
    os << i16;

    // rotation
    i16 = rotation_;
    os << i16;

    // skew
    i16 = skew_;
    os << i16;

    int offsetFormPtrs = os.device()->pos();

    // form offsets (filled in later)
    for ( int i = 0; i <= forms_.size(); ++i )
    {
        i32 = 0;
        os << i32;
    }

    int offsetForms = os.device()->pos();

    // form data (filled in later)
    if ( forms_.size() )
    {
        QByteArray buffer( forms_.size() * labelForm::BINARY_IMAGE_SIZE, '\0' );
        os.writeRawData( buffer.constData(), buffer.size() );
    }

    // forms
    foreach ( const labelForm& form, forms_ )
    {
        // write offset of this form
        os.device()->seek( offsetFormPtrs );
        i32 = offsetForms;
        os << i32;
        os.device()->seek( offsetForms );

        offsetFormPtrs += sizeof(i32);
        offsetForms += labelForm::BINARY_IMAGE_SIZE;

        // write form data
        os << form;
    }

    // write length
    i16 = os.device()->size() - offset;
    os.device()->seek( offsetLength );
    os << i16;

    // seek to end of label
    os.device()->seek( os.device()->size() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void label::parse( const QString& data, base16_encoding_t )
{
    QByteArray a( QByteArray::fromHex( data.toLatin1() ) );
    QDataStream is( a );

    is.setByteOrder( QDataStream::BigEndian );
    parse( is );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void label::parse( const QString& data, base64_encoding_t )
{
    QByteArray a( QByteArray::fromBase64( data.toLatin1() ) );
    QDataStream is( a );

    is.setByteOrder( QDataStream::BigEndian );
    parse( is );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void label::parse( QDataStream& is )
{
    LOG_CLASS_INFO( "label", "parse" );

    // must have attached device
    if ( !is.device() )
    {
        LOG_WARN( "no device attached to stream!", NULL );
        return;
    }
    // device must be readable
    else if ( !is.device()->isReadable() )
    {
        LOG_WARN( "stream is not readable!", NULL );
        return;
    }
    // device must be random access
    else if ( is.device()->isSequential() )
    {
        LOG_WARN( "stream is not random access!", NULL );
        return;
    }

    qint8 i8;
    qint16 i16;
    qint32 i32;

    char str[64];

    // save starting offset
    int offset = is.device()->pos();

    LOG_DEBUG( "parsing label...", NULL );

    // major version
    is >> i8;
    versionMajor_ = i8;

    // minor version
    is >> i8;
    versionMinor_ = i8;

    LOG_DEBUG( "version major %d minor %d", versionMajor_, versionMinor_ );

    if (( 1 != versionMajor_ ) || ( 0 != versionMinor_ ))
    {
        LOG_ERROR( "bad label version %d.%d!", versionMajor_, versionMinor_ );
        return;
    }

    // type
    memset( str, 0, sizeof(str) );
    if ( 4 == is.readRawData( str, 4 ) )
        if ( "LABL" != QString( str ) )
        {
            LOG_ERROR( "bad resource type '%s'", str );
            return;
        }

    // id
    is >> i16;
    id_ = i16;

    // loaded flag
    is >> i8;
    loaded_ = (i8 ? true : false);

    // padding
    is.skipRawData( 1 );

    // length
    is >> i16;
    int length = i16;

    // name
    memset( str, 0, sizeof(str) );
    if ( 32 == is.readRawData( str, 32 ) )
        name_ = str;

    LOG_DEBUG( "id %d name '%s' loaded %d length %d", id_, name_.toLatin1().constData(), loaded_, length );

    // width
    is >> i16;
    size_.setWidth( i16 );

    // height
    is >> i16;
    size_.setHeight( i16 );

    // rotation
    is >> i16;
    rotation_ = i16;

    // skew
    is >> i16;
    skew_ = i16;

    QList<int> offsets;

    // form offsets
    while ( !is.atEnd() )
    {
        is >> i32;

        if ( 0 == i32 )
            break;

        offsets.push_back( i32 );
    }

    LOG_DEBUG( "have %d forms to process...", offsets.size() );

    // forms
    while ( offsets.size() )
    {
        is.device()->seek( offsets.front() );
        offsets.pop_front();

        forms_.push_back( labelForm( is ) );
    }

    // seek to end of label
    is.device()->seek( offset + length );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
label::key_list_type label::toKeyList( const tagDataList& r )
{
    LOG_CLASS_INFO( "label", "toKeyList" );
    key_list_type keys;

    // process all tags
    for ( tagDataList::const_iterator i = r.constBegin(); i != r.constEnd(); ++i )
    {
        switch ( i->first )
        {
        case tagDataFunctionCode::RESOURCE_ID:
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
label::list_type label::toValueList( const tagDataList& r )
{
    LOG_CLASS_INFO( "label", "toValueList" );

    LOG_DEBUG( "processing tag data list of size %d", r.size() );
    list_type values;

    _Myt temp;

    for ( tagDataList::const_iterator i = r.constBegin(); i != r.constEnd(); ++i )
    {
        switch ( i->first )
        {
        case tagDataFunctionCode::RESOURCE_ID:
            // ignore resource id, it is embedded within the label data
            break;
        case tagDataFunctionCode::RESOURCE:
            temp.parse( i->second, base16_encoding );

            // add to list
            if ( !temp.id() )
            {
                LOG_WARN( "have bad label %d", temp.id() );
                temp = _Myt();
            }
            else
            {
                LOG_DEBUG( "have label %d of dimensions %d,%d with %d forms",
                    temp.id(),
                    temp.size().width(),
                    temp.size().height(),
                    temp.forms().size() );

                values.push_back( temp );
                temp = _Myt();
            }

            break;
        default:
            LOG_WARN( "unhandled function code %d", i->first );
            break;
        }
    }

    LOG_INFO( "have %d labels", values.size() );
    return values;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecordList label::toTagDataRecordList( const list_type& values )
{
    LOG_CLASS_INFO( "label", "toTagDataRecordList" );

    LOG_INFO( "processing value list of size %d", values.size() );
    tagDataRecordList rl;

    for ( list_type::const_iterator i = values.constBegin(); i != values.constEnd(); ++i )
    {
        if ( !i->id() )
            LOG_WARN( "have bad label %d", i->id() );
        else
        {
            static const QString format( "%1" );
            tagDataRecord r( tagDataCommand::toRecordType<label, tagDataCommand::WRITE>::TYPE );

            LOG_DEBUG( "have label %d of dimensions %d,%d with %d forms",
                i->id(),
                i->size().width(),
                i->size().height(),
                i->forms().size() );

            r.second.push_back( tagData( tagDataFunctionCode::RESOURCE_ID, format.arg( i->id() ) ) );
            r.second.push_back( tagData( tagDataFunctionCode::RESOURCE, i->serialize( base16_encoding ) ) );
            rl.push_back( r );
        }
    }

    LOG_DEBUG( "have %d records", rl.size() );
    return rl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void label::copy( const _Myt& rhs )
{
    versionMajor_ = rhs.versionMajor_;
    versionMinor_ = rhs.versionMinor_;

    id_ = rhs.id_;
    name_ = rhs.name_;

    loaded_ = rhs.loaded_;

    size_ = rhs.size_;
    rotation_ = rhs.rotation_;
    skew_ = rhs.skew_;

    forms_ = rhs.forms_;
}
