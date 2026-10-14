/**
 * @file bitmap.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include "rbitmap.h"

#ifdef QT_CORE_LIB
#include <QBuffer>
#include <QByteArray>
#ifdef QT_GUI_LIB
#include <QColor>
#include <QImage>
#endif
#endif

const uint32_t bitmap::mask_[] = {
    0x0000,
    0x0001,
    0x0003,
    0x0000,
    0x000f,
    0x0000,
    0x0000,
    0x0000,
    0x00ff,
};

const uint32_t bitmap::shift_[][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 0
    { 7, 6, 5, 4, 3, 2, 1, 0 }, // 1*
    { 6, 4, 2, 0, 0, 0, 0, 0 }, // 2*
    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 3
    { 4, 0, 0, 0, 0, 0, 0, 0 }, // 4*
    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 5
    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 6
    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 7
    { 0, 0, 0, 0, 0, 0, 0, 0 }, // 8*
};

const uint32_t bitmap::alignment_( 16 );

///////////////////////////////////////////////////////////////////////////////////////////////////
bitmap::bitmap() :
    size_( dimensions_t( 0, 0 ) ),
    bpp_( 1 )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitmap::bitmap( size_t width, size_t height, uint32_t bpp ) :
    size_( dimensions_t( width, height ) ),
    bpp_( bpp )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitmap::bitmap( const dimensions_t &size, uint32_t bpp ) :
    size_( size ),
    bpp_( bpp )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitmap::bitmap( const _Myt &other )
{
    assign( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitmap::~bitmap()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bitmap &bitmap::operator = ( const _Myt &rhs )
{
    assign( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool bitmap::operator == ( const _Myt &rhs ) const
{
    if (( size_ != rhs.size_ ) || ( bits_.size() != rhs.bits_.size() ))
        return false;

    return (bits_ == rhs.bits_);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t bitmap::black() const
{
    return (bpp_ <= 8) ? 0xffffffff : 0xff000000;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t bitmap::bpp() const
{
    return bpp_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t bitmap::height() const
{
    return size_.second;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <>
uint32_t bitmap::read<0>( size_t x, size_t y ) const
{
    uint32_t result( bits_[offset( x, y )] );
    result >>= shift_[bpp_][x % m_];
    result &= mask_[bpp_];

    return result;
}

template <>
uint32_t bitmap::read<1>( size_t x, size_t y ) const
{
    const size_t base( offset( x, y ) );

    uint32_t result( 0 );

    for ( size_t i( 0 ); i < m_; ++i )
    {
        result <<= 8;
        result |= bits_[base+i];
    }

    return result;
}

uint32_t bitmap::pixel( size_t x, size_t y ) const
{
    return ( ver_ ? read<1>( x, y ) : read<0>( x, y ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t bitmap::pixel( const point_t &p ) const
{
    return pixel( p.first, p.second );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template <>
void bitmap::write<0>( size_t x, size_t y, uint32_t value )
{
    const size_t index( offset( x, y ) );
    const size_t shift( shift_[bpp_][x % m_] );

    const uint8_t mask( mask_[bpp_] << shift );

    bits_[index] &= (~mask);
    bits_[index] |= (mask & (value << shift));
}

template <>
void bitmap::write<1>( size_t x, size_t y, uint32_t value )
{
    size_t len( m_ );
    size_t index( offset( x, y ) + len );

    while ( len-- )
    {
        bits_[--index] = (value & 0xff);
        value >>= 8;
    }
}

void bitmap::setPixel( size_t x, size_t y, uint32_t value )
{
    if ( ver_ )
        write<1>( x, y, value );
    else
        write<0>( x, y, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bitmap::setPixel( const point_t &p, uint32_t value )
{
    setPixel( p.first, p.second, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dimensions_t bitmap::size() const
{
    return size_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool bitmap::valid() const
{
    return (bits_.length() ? true : false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t bitmap::white() const
{
    return (bpp_ <= 8) ? 0xff000000 : 0xffffffff;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t bitmap::width() const
{
    return size_.first;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bitmap::assign( const _Myt &other )
{
    size_ = other.size_;
    bpp_ = other.bpp_;

    bits_ = other.bits_;

    ver_ = other.ver_;

    scanLineWidth_ = other.scanLineWidth_;
    m_ = other.m_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bitmap::drawRect( const point_t &p, const dimensions_t &size, uint32_t value )
{
    drawRect( p.first, p.second, size.first, size.second, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bitmap::drawRect( size_t x, size_t y, size_t width, size_t height, uint32_t value )
{
    while ( height-- )
    {
        size_t w( width );

        while ( w-- )
            setPixel( x+w, y+height, value );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bitmap::fill( uint32_t value )
{
    for ( size_t y( size_.second ); --y; )
        for ( size_t x( size_.first ); --x; )
            setPixel( x, y, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
buffer bitmap::toImage( const std::string &format ) const
{
#if !defined( QT_CORE_LIB )
    return buffer();
#elif !defined( QT_GUI_LIB )
    return buffer();
#else
    QByteArray a;
    QBuffer dev( &a );

    // monochrome
    if ( 1 == bpp_ )
    {
        // generate image
        QImage image( size_.first, size_.second, QImage::Format_Mono );

        for ( size_t y( size_.second ); y--; )
        {
            size_t index( y * scanLineWidth() );
            size_t x( 0 );

            do
            {
                const uint8_t b( bits_[index++] );

                uint8_t mask( 0x80 );

                do
                {
                    image.setPixel( x++, y, ((b & mask) ? 0 : 1) );

                } while ( mask >>= 1 );
            } while ( x < size_.first );
        }

        // save image
        image.save( &dev, format.c_str() );
    }

    // color
    else if ( 8 < bpp_ )
    {
        QImage::Format f( QImage::Format_RGB16 );

        if ( 32 == bpp_ )
            f = QImage::Format_ARGB32;

        // generate image
        QImage image( size_.first, size_.second, f );

        for ( size_t y( size_.second ); y--; )
        {
            size_t index( y * scanLineWidth() );
            size_t x( 0 );

            do
            {
                uint32_t rgb( 0 );

                for ( size_t m( m_ ); m--; )
                {
                    rgb <<= 8;
                    rgb |= bits_[index++];
                }

                // set pixel
                if ( 32 == bpp_ )
                    image.setPixel( x++, y, rgb );
                else
                {
                    // convert rgb565
                    QColor c( (rgb & 0xf800) >> 8, (rgb & 0x07e0) >> 3, (rgb & 0x001f) << 3 );

                    if ( 0xf8 == c.red() )
                        c.setRed( 0xff );
                    if ( 0xfc == c.green() )
                        c.setGreen( 0xff );
                    if ( 0xf8 == c.red() )
                        c.setRed( 0xff );

                    image.setPixel( x++, y, c.rgb() );
                }

            } while ( x < size_.first );
        }

        // save image
        image.save( &dev, format.c_str() );
    }

    // grayscale
    else
    {
        // generate image
        QImage image( size_.first, size_.second, QImage::Format_RGB32 );

        for ( size_t y( size_.second ); y--; )
        {
            const uint8_t startingMask( mask_[bpp_] << shift_[bpp_][0] );

            size_t index( y * scanLineWidth() );
            size_t x( 0 );

            do
            {
                const uint8_t b( bits_[index++] );
                uint8_t mask( startingMask );

                do
                {
                    uint8_t value( b & mask );

                    if ( value == mask )
                        value = 0xff;
                    else
                        value <<= (8 - (bpp_ + shift_[bpp_][x % m_]));

                    // flip value
                    value = 0xff - value;

                    // set pixel
                    const QColor c( value, value, value );

                    image.setPixel( x++, y, c.rgb() );

                } while ( mask >>= bpp_ );
            } while ( x < size_.first );
        }

        // save image
        image.save( &dev, format.c_str() );
    }

    // give data to caller
    return buffer( (uint8_t*) a.constData(), a.size() );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t bitmap::scanLineWidth() const
{
    return scanLineWidth_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void bitmap::initialize()
{
    // set version and multiplier
    if ( bpp_ < 8 )
    {
        ver_ = 0;
        m_ = (8 / bpp_);
    }
    else
    {
        ver_ = 1;
        m_ = (bpp_ / 8);
    }

    // calculate scan line width
    const size_t bits( bpp_ * size_.first );

    scanLineWidth_ = bits / alignment_;
    scanLineWidth_ += (bits % alignment_) ? 1 : 0;
    scanLineWidth_ *= (alignment_ / 8);

    // create buffer
    bits_.assign( size_.second * scanLineWidth_, white() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t bitmap::offset( size_t x, size_t y ) const
{
    static const size_t data[] = {
        0, 8, 4, 0, 2, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0,
        2, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        4, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
    };

    return (y * scanLineWidth()) + (ver_ ? (x * data[bpp_]) : (x / data[bpp_]));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t bitmap::offset( const point_t &p ) const
{
    return offset( p.first, p.second );
}
