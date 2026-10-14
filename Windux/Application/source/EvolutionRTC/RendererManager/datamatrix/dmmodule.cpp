/**
 * @file dmmodule.cpp
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "dmmetrics.h"
#include "dmmodule.h"

#include <rbitmap.h>
#include "buffer.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>

/// Data Matrix Barcode Namespace
namespace dm
{

///////////////////////////////////////////////////////////////////////////////////////////////////
module::module() :
    type_( Reserved ),
    value_( false ),
    next_( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
module::~module()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
moduleMatrix::moduleMatrix( symbolSize s ) :
    size_( s )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
moduleMatrix::moduleMatrix( symbolSize s, const buffer &value ) :
    size_( s )
{
    initialize();
    setData( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
moduleMatrix::moduleMatrix( const _Myt &other ) :
    size_( other.size_ )
{
    initialize();
    assign( other );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
moduleMatrix::~moduleMatrix()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
moduleMatrix &moduleMatrix::operator = ( const _Myt &rhs )
{
    assign( rhs );
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const module &moduleMatrix::at( size_t x, size_t y ) const
{
    return modules_[width_*y + x];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
module &moduleMatrix::at( size_t x, size_t y )
{
    return modules_[width_*y + x];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t moduleMatrix::height() const
{
    return height_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::setData( const buffer &value )
{
    static const uint8_t mask( 0x80 );

    module *m( data_ );

    // validate buffer length matches number of modules
    size_t moduleCount( 0 );
    module *mod( m );

    while ( mod )
    {
        ++moduleCount;
        mod = mod->next_;
    }

    if ( moduleCount != (8 * value.size()) )
        return;

    // set module values
    for ( buffer::const_iterator i( value.begin() ); i != value.end(); ++i )
    {
        uint8_t value( *i );
        size_t iter( 8 );

        do
        {
            m->value_ = (mask & value) ? true : false;

            value <<= 1;
            m = m->next_;

        } while ( --iter );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
symbolSize moduleMatrix::size() const
{
    return size_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t moduleMatrix::width() const
{
    return width_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::assign( const _Myt &other )
{
    // initialize
    if ( size_ != other.size_ )
    {
        size_ = other.size_;
        initialize();
    }

    // copy module values
    for ( size_t i( modules_.size() ); i--; )
        modules_[i].value_ = other.modules_[i].value_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::print() const
{
    for ( size_t y( 0 ); y < height_; ++y )
    {
        for ( size_t x( 0 ); x < width_; ++x )
            std::cout << (at( x, y ).value_ ? "X" : " ");

        std::cout << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::printModuleTypes() const
{
    for ( size_t y( 0 ); y < height_; ++y )
    {
        for ( size_t x( 0 ); x < width_; ++x )
        {
            switch ( at( x, y ).type_ )
            {
            case module::FinderPattern:
                std::cout << "=";
                break;
            case module::Data:
                std::cout << " ";
                break;
            case module::Reserved:
                std::cout << "R";
                break;
            default:
                std::cout << "?";
                break;
            }
        }

        std::cout << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::render( bitmap &canvas, size_t bx, size_t by, size_t width, size_t height ) const
{
    //
    // calculate module length
    //

    size_t moduleLen( 0 );
    size_t h( height_ );
    size_t w( width_ );

    while (( w <= width ) && ( h <= height ))
    {
        ++moduleLen;
        h += height_;
        w += width_;
    }

    if ( !moduleLen )
        return;

    h -= height_;
    w -= width_;

    //
    // center matrix in provided rectangle
    //

    bx += ((width - w) / 2 );

    by += ((height - h) / 2 );
    by += h;

    //
    // draw matrix
    //

    for ( size_t y( height_ ); y--; )
    {
        by -= moduleLen;

        for ( size_t x( width_ ); x--; )
        {
            const uint32_t p( at( x, y ).value_ ? canvas.black() : canvas.white() );

            canvas.drawRect( bx + (moduleLen * x), by, moduleLen, moduleLen, p );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::setModuleValue( module *m, size_t value )
{
    while (( value ) && ( m ))
    {
        m->value_ = (value & 0x01) ? true : false;

        value >>= 1;
        m = m->next_;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::initialize()
{
    static const size_t widthData[] = {
        10, 12, 14, 16, 18, 20,
        22, 24, 26, 32, 36, 40,
        44, 48, 52, 64, 72, 80,
        88, 96, 104, 120, 132, 144,
        18, 32, 26, 36, 36, 48
    };

    static const size_t heightData[] = {
        10, 12, 14, 16, 18, 20,
        22, 24, 26, 32, 36, 40,
        44, 48, 52, 64, 72, 80,
        88, 96, 104, 120, 132, 144,
        8, 8, 12, 12, 16, 16
    };

    // lookup symbology
    const symbolInfo info( metrics::symbology( size_ ) );

    // setup modules
    width_ = widthData[size_];
    height_ = heightData[size_];

    modules_.resize( width_ * height_ );

    mappingWidth_ = info.mappingMatrixWidth;
    mappingHeight_ = info.mappingMatrixHeight;

    // generate mapping
    coordinateList list;

    int x( 0 );
    int y( 4 );

    do
    {
        // check for one of the special corner cases
        if (( y == mappingHeight_ ) && ( x == 0 ))
            corner1( list );

        if (( y == mappingHeight_-2 ) && ( x == 0 ) && ( mappingWidth_%4 ))
            corner2( list );

        if (( y == mappingHeight_-2 ) && ( x == 0 ) && ( mappingWidth_%8 == 4 ))
            corner3( list );

        if (( y == mappingHeight_+4 ) && ( x == 2 ) && ( !(mappingWidth_%8) ))
            corner4( list );

        // sweep upward diagonally, inserting successive bits
        do
        {
            if (( y < (int) mappingHeight_ ) && ( x >= 0 ))
                if ( list.end() == std::find( list.begin(), list.end(), coordinate( x, y ) ) )
                    utah( list, x, y );

            x += 2;
            y -= 2;

        } while (( y >= 0 ) && ( x < (int) mappingWidth_ ));

        x += 3;
        y += 1;

        // sweep downward diagonally, inserting successive bits
        do
        {
            if (( y >= 0 ) && ( x < (int) mappingWidth_ ))
                if ( list.end() == std::find( list.begin(), list.end(), coordinate( x, y ) ) )
                    utah( list, x, y );

            x -= 2;
            y += 2;

        } while (( y < (int) mappingHeight_ ) && ( x >= 0 ));

        x += 1;
        y += 3;

        // loop until the entire array is scanned

    } while (( y < (int) mappingHeight_ ) || ( x < (int) mappingWidth_ ));

    assert( 0 == list.size()%8 );

    // create matrix
    size_t regions( info.numDataRegions );

    size_t bx( 0 );
    size_t by( 0 );

    do
    {
        // create finder pattern
        createFinderPattern( bx, by, info.dataRegionWidth+2, info.dataRegionHeight+2 );

        // process next data region
        bx += info.dataRegionWidth + 2;

        if ( width_ <= bx )
        {
            bx = 0;
            by += info.dataRegionHeight + 2;
        }

    } while ( --regions );

    // create data
    data_ = linkModules( list, module::Data );

    // create reserved
    if ( module::Reserved == at( width_-2, height_-2 ).type_ )
    {
        at( width_-2, height_-2 ).value_ = true;
        at( width_-3, height_-3 ).value_ = true;

        at( width_-3, height_-2 ).value_ = false;
        at( width_-2, height_-3 ).value_ = false;
    }

    // validation
    size_t finderPattern( 0 );
    size_t data( 0 );
    size_t reserved( 0 );

    size_t value( 0 );

    for ( size_t x( width_ ); x--; )
        for ( size_t y( height_ ); y --; )
        {
            switch ( at( x, y ).type_ )
            {
            case module::FinderPattern:
                ++finderPattern;
                break;
            case module::Data:
                ++data;
                break;
            case module::Reserved:
                ++reserved;
                break;
            default:
                break;
            }

            if ( at( x, y ).value_ )
                ++value;
        }

    const size_t regionWidth( info.dataRegionWidth + 2 );
    const size_t regionHeight( info.dataRegionHeight + 2 );

    assert( 0 == data%8 );
    assert( data + reserved == mappingWidth_ * mappingHeight_ );
    assert( finderPattern == info.numDataRegions * ((2 * regionWidth) + (2 * regionHeight) - 4) );

    assert( value == reserved/2 + info.numDataRegions * (regionWidth + regionWidth/2 + regionHeight + regionHeight/2 - 3) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::createFinderPattern( size_t bx, size_t by, size_t w, size_t h )
{
    const size_t b( h-1 ); // bottom index
    const size_t r( w-1 ); // right index

    for ( size_t x( w ); x--; )
    {
        // top
        at( bx+x, by ).type_ = module::FinderPattern;
        at( bx+x, by ).value_ = (0 == x%2);

        // bottom
        at( bx+x, by+b ).type_ = module::FinderPattern;
        at( bx+x, by+b ).value_ = true;
    }

    for ( size_t y( h ); y--; )
    {
        // left
        at( bx, by+y ).type_ = module::FinderPattern;
        at( bx, by+y ).value_ = true;

        // right
        at( bx+r, by+y ).type_ = module::FinderPattern;
        at( bx+r, by+y ).value_ = (0 == (b-y)%2);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
module *moduleMatrix::linkModules( const coordinateList &list, module::type t )
{
    module *head( NULL );
    module *prev( NULL );

    for ( coordinateList::const_iterator l( list.begin() ); l != list.end(); ++l )
    {
        // translate coordinate
        size_t mx( l->first );
        size_t my( l->second );

        size_t x( 0 );
        size_t y( 0 );

        for ( ;; ++x )
            if ( module::FinderPattern != at( x, 1 ).type_ )
            {
                if ( !mx-- )
                    break;
            }

        for ( ;; ++y )
            if ( module::FinderPattern != at( 1, y ).type_ )
            {
                if ( !my-- )
                    break;
            }

        // link module
        module *m( &at( x, y ) );
        m->type_ = t;

        if ( prev )
            prev->next_ = m;
        else
        {
            head = m;
        }

        prev = m;
    }

    return head;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::appendModule( coordinateList &list, int x, int y ) const
{
    if ( y < 0 )
    {
        y += (int)mappingHeight_;
        x += 4 - (int)((mappingHeight_+4)%8);
    }

    if ( x < 0 )
    {
        x += (int)mappingWidth_;
        y += 4 - (int)((mappingWidth_+4)%8);
    }

    list.push_back( coordinate( x, y ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::utah( coordinateList &list, int x, int y ) const
{
    appendModule( list, x-2, y-2 );
    appendModule( list, x-1, y-2 );
    appendModule( list, x-2, y-1 );
    appendModule( list, x-1, y-1 );
    appendModule( list, x, y-1 );
    appendModule( list, x-2, y );
    appendModule( list, x-1, y );
    appendModule( list, x, y );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::corner1( coordinateList &list ) const
{
    appendModule( list, 0, (int) mappingHeight_-1 );
    appendModule( list, 1, (int)mappingHeight_-1 );
    appendModule( list, 2, (int)mappingHeight_-1 );
    appendModule( list, (int)mappingWidth_-2, 0 );
    appendModule( list, (int)mappingWidth_-1, 0 );
    appendModule( list, (int)mappingWidth_-1, 1 );
    appendModule( list, (int)mappingWidth_-1, 2 );
    appendModule( list, (int)mappingWidth_-1, 3 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::corner2( coordinateList &list ) const
{
    appendModule( list, 0, (int)mappingHeight_-3 );
    appendModule( list, 0, (int)mappingHeight_-2 );
    appendModule( list, 0, (int)mappingHeight_-1 );
    appendModule( list, (int)mappingWidth_-4, 0 );
    appendModule( list, (int)mappingWidth_-3, 0 );
    appendModule( list, (int)mappingWidth_-2, 0 );
    appendModule( list, (int)mappingWidth_-1, 0 );
    appendModule( list, (int)mappingWidth_-1, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::corner3( coordinateList &list ) const
{
    appendModule( list, 0, (int)mappingHeight_-3 );
    appendModule( list, 0, (int)mappingHeight_-2 );
    appendModule( list, 0, (int)mappingHeight_-1 );
    appendModule( list, (int)mappingWidth_-2, 0 );
    appendModule( list, (int)mappingWidth_-1, 0 );
    appendModule( list, (int)mappingWidth_-1, 1 );
    appendModule( list, (int)mappingWidth_-1, 2 );
    appendModule( list, (int)mappingWidth_-1, 3 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::corner4( coordinateList &list ) const
{
    appendModule( list, 0, (int)mappingHeight_-1 );
    appendModule( list, (int)mappingWidth_-1, (int)mappingHeight_-1 );
    appendModule( list, (int)mappingWidth_-3, 0 );
    appendModule( list, (int)mappingWidth_-2, 0 );
    appendModule( list, (int)mappingWidth_-1, 0 );
    appendModule( list, (int)mappingWidth_-3, 1 );
    appendModule( list, (int)mappingWidth_-2, 1 );
    appendModule( list, (int)mappingWidth_-1, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace
