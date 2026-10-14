/**
 * @file labelform.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "labelform.h"

const QSize labelForm::defaultSize_( 100, 15 );

///////////////////////////////////////////////////////////////////////////////////////////////////
labelForm::labelForm() :
    rect_( QPoint( 0, 0 ), defaultSize_ ),
    type_( labelFormType::CONSTANT_TEXT ),
    flags_( (labelFormFlags::type) 0 ),
    alignment_( Qt::AlignLeft ),
    verticalJustification_( 0 ),
    subType_( 0 ),
    rotation_( 0 ),
    fontPointSize_( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelForm::labelForm( labelFormType::type type ) :
    rect_( QPoint( 0, 0 ), defaultSize_ ),
    type_( type ),
    flags_( (labelFormFlags::type) 0 ),
    alignment_( Qt::AlignLeft ),
    verticalJustification_( 0 ),
    subType_( 0 ),
    rotation_( 0 ),
    fontPointSize_( 0 )
{
    // special case for line types... default to horizontal line
    if (( labelFormType::USER_LINE == type_ ) || ( labelFormType::DIVIDING_LINE == type_ ))
    {
        rect_.setHeight( 0 );
        subType_ = 1;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelForm::labelForm( QDataStream& is )
{
    parse( is );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelForm::labelForm( const _Myt& rhs )
{
    copy( rhs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelForm::~labelForm()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelForm::_Myt& labelForm::operator = ( const _Myt& rhs )
{
    copy( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString labelForm::typeText() const
{
    if ( labelFormType::BARCODE == type() )
    {
        switch ( subType() )
        {
        case 0:
            return "UPC-A Barcode";
        case 1:
            return "UPC-D3 Barcode";
        case 2:
            return "UPC-E Barcode";
        case 3:
            return "EAN13 Barcode";
        case 4:
            return "EAN128C Barcode";
        case 5:
            return "RSS14E Barcode";
        case 6:
            return "RSS14E GTIN 13 Barcode";
        case 7:
            return "RSS14E GTIN 12 Barcode";
        case 8:
            return "RSS14E GTIN 8 Barcode";
        case 9:
            return "QR LEVEL M Barcode";
        case 10:
            return "QR LEVEL L Barcode";
        case 11:
            return "QR LEVEL Q Barcode";
        case 12:
            return "QR LEVEL H Barcode";
        case 13:
            return "DATA MATRIX Barcode";
        default:
            break;
        }
    }
    else if ( labelFormType::NUTRI_COLUMN_1 == type() )
        return tag_ + "-NDC1";
    else if ( labelFormType::NUTRI_COLUMN_2 == type() )
        return tag_ + "-NDC2";
    else if ( labelFormType::NUTRI_COLUMN_3 == type() )
        return tag_ + "-NDC3";
    else if ( labelFormType::NUTRI_COLUMN_4 == type() )
        return tag_ + "-NDC4";

    return labelFormType::toString( type() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelForm::serialize( QDataStream& os ) const
{
    LOG_CLASS_INFO( "labelForm", "serialize" );

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

    LOG_DEBUG( "serializing label form...", NULL );

    // x
    i16 = rect_.left();
    os << i16;

    // y
    i16 = rect_.top();
    os << i16;

    // width
    i16 = rect_.width();
    os << i16;

    // height
    i16 = rect_.height();
    os << i16;

    // flags
    i16 = flags_;
    os << i16;

    // type
    i8 = type_;
    os << i8;

    // horizontal alignment (0 left, 1 center, 2 right)
    if ( Qt::AlignHCenter & alignment_ )
        i8 = 1;
    else if ( Qt::AlignRight & alignment_ )
        i8 = 2;
    else // Qt::AlignLeft
        i8 = 0;

    os << i8;

    // vertical justification
    i16 = verticalJustification_;
    os << i16;

    // sub type
    i8 = subType_;
    os << i8;

    // padding
    i8 = 0;
    os << i8;

    // rotation
    i16 = rotation_;
    os << i16;

    // font name
    str = fontFamilyName_.toLatin1();

    while ( 40 > str.size() )
        str.append( '\0' );

    os.writeRawData( str.constData(), 40 );

    // font size
    i16 = fontPointSize_;
    os << i16;

    // tag
    str = tag_.toLatin1();

    while ( 6 > str.size() )
        str.append( '\0' );

    os.writeRawData( str.constData(), 6 );

    // data offset
    i32 = 0;

    if ( data_.length() )
        i32 = os.device()->size();

    os << i32;

    // data
    if ( data_.length() )
    {
        int pos = os.device()->pos();

        // write data and NULL terminator
        os.device()->seek( i32 );
        os.writeRawData( data_.toLatin1().constData(), data_.length() + 1 );

        os.device()->seek( pos );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelForm::parse( QDataStream& is )
{
    LOG_CLASS_INFO( "labelForm", "parse" );

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

    quint8 i8;
    qint16 i16;
    qint32 i32;

    char str[64];

    LOG_DEBUG( "parsing label form...", NULL );

    // x
    is >> i16;
    rect_.setLeft( i16 );

    // y
    is >> i16;
    rect_.setTop( i16 );

    // width
    is >> i16;
    rect_.setWidth( i16 );

    // height
    is >> i16;
    rect_.setHeight( i16 );

    LOG_DEBUG( "left %d top %d width %d height %d", rect_.left(), rect_.top(), rect_.width(), rect_.height() );

    // flags
    is >> i16;
    flags_ = (labelFormFlags::type) i16;

    // type
    is >> i8;
    type_ = (labelFormType::type) i8;

    // horizontal alignment (0 left, 1 center, 2 right)
    is >> i8;

    if ( 1 == i8 )
        alignment_ = Qt::AlignHCenter;
    else if ( 2 == i8 )
        alignment_ = Qt::AlignRight;
    else // 0
        alignment_ = Qt::AlignLeft;

    // vertical justification
    is >> i16;
    verticalJustification_ = i16;

    // sub type
    is >> i8;
    subType_ = i8;

    LOG_DEBUG( "type %d '%s' subtype %d", type_, labelFormType::toString( type_ ).toLatin1().constData(), subType_ );
    LOG_DEBUG( "flags 0x%08x alignment %d vertical justification %d", flags_, alignment_, verticalJustification_ );

    // padding
    is.skipRawData( 1 );

    // rotation
    is >> i16;
    rotation_ = i16;

    // font name
    memset( str, 0, sizeof(str) );
    if ( 40 == is.readRawData( str, 40 ) )
        fontFamilyName_ = QString( str ).trimmed();

    // font size
    is >> i16;
    fontPointSize_ = i16;

    LOG_DEBUG( "font '%s' font size %d rotation %d", fontFamilyName_.toLatin1().constData(), fontPointSize_, rotation_ );

    // tag
    memset( str, 0, sizeof(str) );
    if ( 6 == is.readRawData( str, 6 ) )
        tag_ = QString( str ).trimmed();

    // data offset
    is >> i32;
    data_.clear();

    // data
    if ( i32 )
    {
        int offset = is.device()->pos();

        is.device()->seek( i32 );

        // read until at end of stream or NULL terminator hit
        while ( !is.atEnd() )
        {
            is >> i8;

            if ( '\0' == i8 )
                break;

            data_ += i8;
        }

        is.device()->seek( offset );
    }

    LOG_DEBUG( "tag '%s' data '%s'", tag_.toLatin1().constData(), data_.toLatin1().constData() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelForm::copy( const _Myt& rhs )
{
    rect_ = rhs.rect_;

    type_ = rhs.type_;
    flags_ = rhs.flags_;

    alignment_ = rhs.alignment_;
    verticalJustification_ = rhs.verticalJustification_;

    subType_ = rhs.subType_;
    rotation_ = rhs.rotation_;

    fontFamilyName_ = rhs.fontFamilyName_;
    fontPointSize_ = rhs.fontPointSize_;

    tag_ = rhs.tag_;
    data_ = rhs.data_;
}
