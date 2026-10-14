/**
 * @file event.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "event.h"

#include <QDebug>

///////////////////////////////////////////////////////////////////////////////////////////////////
Event::Event( const QVector<uint>& dataLen ) :
    device_( DeviceType::Invalid ),
    event_( 0 ),
    dataLen_( dataLen )
{
    // data
    data_.resize( dataLen_.size() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Event::Event( DeviceType device, uint event, const QVector<uint>& dataLen ) :
    device_( device ),
    event_( event ),
    dataLen_( dataLen )
{
    // data
    data_.resize( dataLen_.size() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Event::Event( const QByteArray& a )
{
    uint pos( 0 );

    if ( !deserialize( a, pos ) )
    {
        device_ = DeviceType::Invalid;
        event_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Event::Event( const _Myt& other )
{
    (*this) = other;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Event::~Event()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Event::operator == ( const _Myt& rhs ) const
{
    return (( device() == rhs.device() ) && ( event() == rhs.event() ) &&
        ( senderService() == rhs.senderService() ) && ( senderObjectPath() == rhs.senderObjectPath() ) &&
        ( dataLen_ == rhs.dataLen_ ) && ( data_ == rhs.data_ ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Event::operator != ( const _Myt& rhs ) const
{
    return !((*this) == rhs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Event& Event::operator = ( const _Myt& rhs )
{
    device_ = rhs.device();
    event_ = rhs.event();

    senderService_ = rhs.senderService();
    senderObjectPath_ = rhs.senderObjectPath();

    dataLen_ = rhs.dataLen_;
    data_ = rhs.data_;

    return (*this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Event::isValid() const
{
    return (( DeviceType::Invalid != device_ ) && ( 0 < event_ ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString Event::senderService() const
{
    return senderService_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Event::setSenderService( const QString& value )
{
    senderService_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString Event::senderObjectPath() const
{
    return senderObjectPath_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Event::setSenderObjectPath( const QString& value )
{
    senderObjectPath_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DeviceType Event::device() const
{
    return device_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Event::setDevice( DeviceType value )
{
    device_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint Event::event() const
{
    return event_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Event::setEvent( uint value )
{
    event_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Event::setData( uint offset, const char *value )
{
    setData( offset, QByteArray(  value ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray Event::serialize() const
{
    QByteArray a;

    // event info
    write( a, (uint) device_ );
    write( a, event_ );

    // sender info
    write( a, senderService_.toLatin1() );
    write( a, senderObjectPath_.toLatin1() );

    // data length
    const uint dataSize( qMin( dataLen_.size(), data_.size() ) );

    write( a, dataSize );

    for ( uint i( 0 ); i < dataSize; ++i )
        write( a, dataLen_[i] );

    // data
    for ( uint i( 0 ); i < dataSize; ++i )
    {
        // missing data length specification
        if ( (unsigned) dataLen_.size() <= i )
        {
            qWarning() << "have data but no data length" << i;
            break;
        }

        // when provided a data length use it
        //   0 = variable data length (prefix length before data)
        //  >0 = fixed data length
        if ( dataLen_[i] )
        {
            if ( (uint) data_[i].size() != dataLen_[i] )
                qWarning() << "data size different from expected";
            else
            {
                a.append( data_[i] );
            }
        }

        // no data length spec
        // prefix length before data
        else
        {
            write( a, data_[i] );
        }
    }

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Event::deserialize( const QByteArray& a, uint& pos )
{
    QByteArray va;
    uint v;

    // event info
    if ( !read( a, pos, v ) )
        return false;

    device_ = (DeviceType) v;

    if ( !read( a, pos, v ) )
        return false;

    event_ = v;

    // sender info
    senderService_.clear();
    senderObjectPath_.clear();

    if ( !read( a, pos, va ) )
        return false;

    senderService_ = va;

    if ( !read( a, pos, va ) )
        return false;

    senderObjectPath_ = va;

    // data len
    if ( !read( a, pos, v ) )
        return false;

    dataLen_.resize( v );

    for ( int i( 0 ); i < dataLen_.size(); ++i )
    {
        if ( !read( a, pos, v ) )
            return false;

        dataLen_[i] = v;
    }

    // data
    data_.resize( dataLen_.size() );

    for ( int i( 0 ); i < dataLen_.size(); ++i )
    {
        if ( !read( a, pos, va, dataLen_[i] ) )
            return false;

        data_[i] = va;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Event::write( QByteArray& a, uint value )
{
    a.append( (const char*) &value, sizeof(value) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Event::write( QByteArray& a, const QByteArray& value )
{
    write( a, value.size() );

    if ( value.size() )
        a.append( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Event::read( const QByteArray& a, uint& pos, uint& v )
{
    if ( (unsigned) a.size() < (pos + sizeof(uint)) )
    {
        qWarning() << "data missing from payload";
        return false;
    }

    const char *data( a.constData() );

    memcpy( &v, &data[pos], sizeof(uint) );
    pos += sizeof(uint);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Event::read( const QByteArray& a, uint& pos, QByteArray& value, uint len )
{
    // read length
    if (( !len ) && ( !read( a, pos, len ) ))
        return false;

    // read data
    value.clear();

    if ( len )
    {
        if ( (unsigned) a.size() < (pos + len) )
        {
            qWarning() << "data missing from payload";
            return false;
        }

        value = a.mid( pos, len );
        pos += len;
    }

    return true;
}
