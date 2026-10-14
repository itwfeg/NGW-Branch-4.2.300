/**
 * @file tagdatarecord.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "tagdatarecord.h"
#include "tagdatastream.h"

#include <QtEndian>

const qint8 tagDataRecord::RS = 0x1E;
const qint8 tagDataRecord::US = 0x1F;

static const tagDataRecordType HEADER_RECORD_TYPE = 'RT01';
static const tagDataRecordType ERROR_RECORD_TYPE = 'RTER';
static const tagDataRecordType FOOTER_RECORD_TYPE = 'RTFF';

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecord::tagDataRecord() :
    _Mybase()
{
    first = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecord::tagDataRecord( tagDataRecordType rt ) :
    _Mybase()
{
    first = rt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecord::tagDataRecord( tagDataRecordType rt, const tagDataList& tags ) :
    _Mybase()
{
    first = rt;
    second = tags;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecord::tagDataRecord( const _Myt& rhs ) :
    _Mybase( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecord::~tagDataRecord()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tagDataRecord::isValid() const
{
    return (( 0 < first ) && ( 'R' == (first >> 24) ) && ( 'T' == ((first >> 16) & 0xFF) ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tagDataRecord::isHeaderRecordType() const
{
    return (HEADER_RECORD_TYPE == first);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tagDataRecord::isErrorRecordType() const
{
    return (ERROR_RECORD_TYPE == first);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool tagDataRecord::isFooterRecordType() const
{
    return (FOOTER_RECORD_TYPE == first);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tagDataRecord::serialize( tagDataStream& os ) const
{
    LOG_CLASS_INFO( "tagDataRecord", "serialize" );

    // validate stream
    if (( !os.device() ) || ( !os.device()->isWritable() ))
    {
        LOG_ERROR( "bad device or device not writable", NULL );
        return;
    }

    // validate record type
    if ( !isValid() )
    {
        LOG_ERROR( "invalid record type", NULL );
        return;
    }

    // record type
    os << first;

    // write all tag data to stream
    for ( tagDataList::const_iterator i = second.constBegin(); i != second.constEnd(); ++i )
    {
        os << US;

        // tag data function code
        qint16 fc = i->first;
        os << fc;

        // tag data value
        os.writeRawData( i->second.toLatin1().constData(), i->second.length() );
    }

    // end of record
    os << RS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tagDataRecord::parse( tagDataStream& is )
{
    LOG_CLASS_INFO( "tagDataRecord", "parse" );

    QIODevice *device = is.device();
    qint8 c;

    // clear record type and tag data
    first = 0;
    second.clear();

    // validate stream
    if (( !device ) || ( !device->isReadable() ))
    {
        LOG_ERROR( "bad device or device not readable", NULL );
        return;
    }

    static const int peekLen = 32 * 1024;
    bool valid = false;

    // check the device data for a record seperator
    for ( qint64 len = peekLen; !valid; len += peekLen )
    {
        QByteArray a = device->peek( qMin( len, device->bytesAvailable() ) );

        for ( int i = len - peekLen; i < a.size(); ++i )
            if ( RS == a[i] )
            {
                LOG_TRACE_DUMP( a.constData(), i );
                valid = true;
                break;
            }

        if ( a.size() < len )
            break;
    }

    if ( !valid )
    {
        LOG_ERROR( "no record seperator found in stream", NULL );
        return;
    }

    // record type
    is >> first;
    is >> c;

    // there should be some type of seperator after the record type
    if ( RS == c )
        return;
    else if ( US != c )
    {
        LOG_ERROR( "record type invalid format", NULL );
        return;
    }

    QByteArray data;

    // read all tag data
    while ( !is.atEnd() )
    {
        is >> c;

        bool done = (( RS == c ) || ( is.atEnd() ));
        bool process = (( done ) || ( US == c ));

        // at end of stream, but no seperator
        if (( done ) && ( RS != c ) && ( US != c ))
            data += c;

        if ( !process )
            data += c;
        else
        {
            if ( 2 > data.size() )
                LOG_WARN( "invalid tag data '%s'", data.constData() );
            else
            {
                qint16 fc = qToBigEndian( *reinterpret_cast<const qint16*>( data.constData() ) );
                QString value = data.right( data.size() - sizeof(qint16) );

                LOG_TRACE( "inserting tag data [%d,'%s']", fc, value.toLatin1().constData() );
                second.push_back( tagData( (tagDataFunctionCode::type) fc, value ) );
            }

            // clear buffer for next tag data
            data.clear();
        }

        if ( done )
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecord tagDataRecord::header( const QString& scaleId )
{
    tagDataRecord r( HEADER_RECORD_TYPE );

    static const QString dataType( "1" );
    r.second.push_back( tagData( tagDataFunctionCode::DATA_TYPE, dataType ) );

    if ( scaleId.length() )
        r.second.push_back( tagData( tagDataFunctionCode::SCALE_ID, scaleId ) );

    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecord tagDataRecord::error( const QString& errorCode,
    const QString& keyValue,
    const QString& failedTag,
    const QString& errorRecord )
{
    tagDataRecord r( ERROR_RECORD_TYPE );

    r.second.push_back( tagData( tagDataFunctionCode::ERROR_CODE, errorCode ) );

    if ( keyValue.length() )
        r.second.push_back( tagData( tagDataFunctionCode::KEY_VALUE, keyValue ) );

    if ( failedTag.length() )
        r.second.push_back( tagData( tagDataFunctionCode::FAILED_TAG, failedTag ) );

    if ( errorRecord.length() )
        r.second.push_back( tagData( tagDataFunctionCode::ERROR_RECORD, errorRecord ) );

    return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataRecord tagDataRecord::footer()
{
    return tagDataRecord( FOOTER_RECORD_TYPE );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataStream& operator << ( tagDataStream& lhs, const tagDataRecord& rhs )
{
    rhs.serialize( lhs );
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
tagDataStream& operator >> ( tagDataStream& lhs, tagDataRecord& rhs )
{
    rhs.parse( lhs );
    return lhs;
}
