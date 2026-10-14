/**
 * @file labelparser.h
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "labelparser.h"

#include <QByteArray>
#include <QDataStream>
#include <QRect>

#include <clio.h>

#define CUSTOMER_LABEL_VERSION_NUM 125

///////////////////////////////////////////////////////////////////////////////////////////////////
labelParser::labelParser( const QString& bin ) :
    valid_( false ),
    customerVersion_( "N/A" )
{
    QByteArray a( QByteArray::fromHex( bin.toLatin1() ) );
    QDataStream is( a );

    is.setByteOrder( QDataStream::BigEndian );
    parse( is );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labelParser::~labelParser()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelParser::parse( QDataStream &is )
{
    // must have attached device
    if ( !is.device() )
    {
        LOG_WARN << "no device attached to stream!";
        return;
    }
    // device must be readable
    else if ( !is.device()->isReadable() )
    {
        LOG_WARN << "stream is not readable!";
        return;
    }
    // device must be random access
    else if ( is.device()->isSequential() )
    {
        LOG_WARN << "stream is not random access!";
        return;
    }

    qint8 i8;
    qint16 i16;
    qint32 i32;

    char str[64];

    // save starting offset
    int offset = is.device()->pos();

    LOG_DEBUG << "parsing label...";

    // major version
    is >> i8;
    versionMajor_ = i8;

    // minor version
    is >> i8;
    versionMinor_ = i8;

    LOG_DEBUG << "version major " << versionMajor_ << " minor " << versionMinor_;

    if (( 1 != versionMajor_ ) || ( 0 != versionMinor_ ))
    {
        LOG_WARN << "bad label version " << versionMajor_ << "." << versionMinor_;
        return;
    }

    // type
    memset( str, 0, sizeof(str) );
    if ( 4 == is.readRawData( str, 4 ) )
        if ( "LABL" != QString( str ) )
        {
            LOG_WARN << "bad resource type '" << str << "'";
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

    LOG_TRACE << "id " << id_ << " name '" << name_.toStdString() << "' loaded " << loaded_ << " length " << length;

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

    LOG_DEBUG << "have " << offsets.size() << " forms to process...";

    // forms
    while ( offsets.size() )
    {
        is.device()->seek( offsets.front() );
        offsets.pop_front();

        parseForm( is );
    }

    // seek to end of label
    is.device()->seek( offset + length );

    // it would appear we got a valid label
    valid_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labelParser::parseForm( QDataStream &is )
{
    QRect rect;

    int type;
    int flags;

    Qt::AlignmentFlag alignment;
    int verticalJustification;

    int subType;
    int rotation;

    QString fontFamilyName;
    int fontPointSize;

    QString tag;
    QString data;

    // must have attached device
    if ( !is.device() )
    {
        LOG_WARN << "no device attached to stream!";
        return;
    }
    // device must be readable
    else if ( !is.device()->isReadable() )
    {
        LOG_WARN << "stream is not readable!";
        return;
    }
    // device must be random access
    else if ( is.device()->isSequential() )
    {
        LOG_WARN << "stream is not random access!";
        return;
    }

    qint8 i8;
    qint16 i16;
    qint32 i32;

    char str[64];

    LOG_DEBUG << "parsing label form...";

    // x
    is >> i16;
    rect.setLeft( i16 );

    // y
    is >> i16;
    rect.setTop( i16 );

    // width
    is >> i16;
    rect.setWidth( i16 );

    // height
    is >> i16;
    rect.setHeight( i16 );

    LOG_TRACE << "left " << rect.left() <<
        " top " << rect.top() <<
        " width " << rect.width() <<
        " height " << rect.height();

    // flags
    is >> i16;
    flags = i16;

    // type
    is >> i8;
    type = i8;

    // horizontal alignment (0 left, 1 center, 2 right)
    is >> i8;

    if ( 1 == i8 )
        alignment = Qt::AlignHCenter;
    else if ( 2 == i8 )
        alignment = Qt::AlignRight;
    else // 0
        alignment = Qt::AlignLeft;

    // vertical justification
    is >> i16;
    verticalJustification = i16;

    // sub type
    is >> i8;
    subType = i8;

    LOG_TRACE << "type " << type << " subtype " << subType;
    LOG_TRACE << "flags " << flags << " alignment " << alignment << " vertical justification " << verticalJustification;

    // padding
    is.skipRawData( 1 );

    // rotation
    is >> i16;
    rotation_ = i16;

    // font name
    memset( str, 0, sizeof(str) );
    if ( 40 == is.readRawData( str, 40 ) )
        fontFamilyName = QString( str ).trimmed();

    // font size
    is >> i16;
    fontPointSize = i16;

    LOG_TRACE << "font '" << fontFamilyName.toStdString() <<
        "' font size " << fontPointSize <<
        " rotation "<< rotation;

    // tag
    memset( str, 0, sizeof(str) );
    if ( 6 == is.readRawData( str, 6 ) )
        tag = QString( str ).trimmed();

    // data offset
    is >> i32;
    data.clear();

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

            data += i8;
        }

        is.device()->seek( offset );
    }

    LOG_TRACE << "tag '" << tag.toStdString() << "' data '" << data.toStdString() << "'";

    // customer label version
    if ( CUSTOMER_LABEL_VERSION_NUM == type )
    {
        customerVersion_ = data;

        LOG_DEBUG << "found label version " << customerVersion_.toStdString();
    }
}
