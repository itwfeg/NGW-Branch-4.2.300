/**
 * @file qrmodule.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include "poly.h"
#include "qrmetrics.h"
#include "qrmodule.h"

#include <bitmap.h>

#include <iostream>
#include <limits>
#if !defined( __linux__)
#include <algorithm>
#endif

/// QR Barcode Namespace
namespace qr
{

///////////////////////////////////////////////////////////////////////////////////////////////////
module::module() :
    type_( Data ),
    value_( false ),
    next_( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
module::~module()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
moduleMatrix::moduleMatrix( version ver ) :
    ver_( ver )
{
    initialize();
    generate( LevelL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
moduleMatrix::moduleMatrix( version ver, eccLevel level, const poly &value ) :
    ver_( ver )
{
    initialize();
    setData( level, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
moduleMatrix::moduleMatrix( const _Myt &other ) :
    ver_( other.ver_ )
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
    return modules_[modulesPerSide_*x + y];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
module &moduleMatrix::at( size_t x, size_t y )
{
    return modules_[modulesPerSide_*x + y];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
size_t moduleMatrix::modulesPerSide() const
{
    return modulesPerSide_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::setData( eccLevel level, const poly &value )
{
    static const uint8_t mask( 0x80 );

    module *m( data_ );

    // set module values
    for ( poly::const_iterator i( value.begin() ); i != value.end(); ++i )
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

    // generate!
    generate( level );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
version moduleMatrix::ver() const
{
    return ver_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::assign( const _Myt &other )
{
    // initialize
    if ( ver_ != other.ver_ )
    {
        ver_ = other.ver_;
        initialize();
    }

    // copy module values
    for ( size_t i( modules_.size() ); i--; )
        modules_[i].value_ = other.modules_[i].value_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::print() const
{
    for ( size_t y( 0 ); y < modulesPerSide_; ++y )
    {
        for ( size_t x( 0 ); x < modulesPerSide_; ++x )
            std::cout << (at( x, y ).value_ ? "X" : " ");

        std::cout << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::printModuleTypes() const
{
    for ( size_t y( 0 ); y < modulesPerSide_; ++y )
    {
        for ( size_t x( 0 ); x < modulesPerSide_; ++x )
        {
            switch ( at( x, y ).type_ )
            {
            case module::AlignmentPattern:
                std::cout << "+";
                break;
            case module::Data:
                std::cout << " ";
                break;
            case module::FinderPattern:
                std::cout << "=";
                break;
            case module::FormatInfo:
                std::cout << "f";
                break;
            case module::TimingPattern:
                std::cout << "t";
                break;
            case module::Separator:
                std::cout << "-";
                break;
            case module::VersionInfo:
                std::cout << "v";
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
    size_t len( modulesPerSide() );

    while (( len <= width ) && ( len <= height ))
    {
        ++moduleLen;
        len += modulesPerSide();
    }

    if ( !moduleLen )
        return;

    len -= modulesPerSide();

    //
    // center matrix in provided rectangle
    //

    bx += ((width - len) / 2 );

    by += ((height - len) / 2 );
    by += len;

    //
    // draw matrix
    //

    for ( size_t y( modulesPerSide() ); y--; )
    {
        by -= moduleLen;

        for ( size_t x( modulesPerSide() ); x--; )
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
size_t moduleMatrix::mask( size_t num )
{
    const size_t end( modulesPerSide_ - 1 );

    //
    // mask data modules
    //

    size_t blockCount( 0 );                         // how many 2x2 blocks
    size_t finderPatternCount( 0 );                 // how many finder patterns found
    size_t darkModuleCount( 0 );                    // how many dark modules

    // mask data bits
    for ( size_t y( modulesPerSide_ ); y--; )
        for ( size_t x( modulesPerSide_ ); x--; )
        {
            module &m( at( x, y ) );

            // mask bit
            if ( module::Data == m.type_ )
            {
                bool value( false );

                switch ( num )
                {
                case 0:
                    value = (0 == (y+x)%2);
                    break;
                case 1:
                    value = (0 == y%2);
                    break;
                case 2:
                    value = (0 == x%3);
                    break;
                case 3:
                    value = (0 == (y+x)%3);
                    break;
                case 4:
                    value = (0 == ((y/2)+(x/3)) % 2);
                    break;
                case 5:
                    value = (0 == ((y*x)%2 + (y*x)%3));
                    break;
                case 6:
                    value = (0 == (((y*x)%2 + (y*x)%3) % 2));
                    break;
                case 7:
                    value = (0 == (((y+x)%2 + (y*x)%3) % 2));
                    break;
                }

                m.value_ ^= value;
            }

            // track large blocks
            if (( x < end ) && ( y < end ))
            {
                if (( m.value_ == at( x, y+1 ).value_ ) &&
                    ( m.value_ == at( x+1, y ).value_ ) &&
                    ( m.value_ == at( x+1, y+1 ).value_ ))
                {
                    ++blockCount;
                }
            }

            // increment dark blocks
            if ( m.value_ )
                ++darkModuleCount;
        }

    //
    // score result barcode
    //

    // (1)
    // calculate number adjacent modules 5 or greater
    // penalties:
    // 5 = 3
    // 6 = 4
    // 7 = 5
    // ...
    size_t score1( 0 );

    for ( size_t y( 0 ); y < modulesPerSide_; ++y )
    {
        size_t adj( 1 );
        size_t value( 0 );

        for ( size_t x( 0 ); x < modulesPerSide_; ++x )
        {
            value <<= 1;
            value |= (at( x, y ).value_ ? 0x01 : 0x00);

            // count adj bits
            if ( 0 < x )
            {
                const size_t pattern( value & 0x03 );

                if (( 0x00 == pattern ) || ( 0x03 == pattern ))
                    ++adj;
                else
                {
                    if ( 5 <= adj )
                        score1 += 3 + (adj-5);

                    adj = 1;
                }
            }

            // check for finder pattern
            if ( 10 < x )
            {
                const size_t pattern( value & 0x07ff );

                if (( 0x05d0 == pattern ) || ( 0x005d == pattern ))
                    ++finderPatternCount;
            }
        }

        if ( 5 <= adj )
            score1 += 3 + (adj-5);
    }

    for ( size_t x( 0 ); x < modulesPerSide_; ++x )
    {
        size_t adj( 1 );
        size_t value( 0 );

        for ( size_t y( 0 ); y < modulesPerSide_; ++y )
        {
            value <<= 1;
            value |= (at( x, y ).value_ ? 0x01 : 0x00);

            // count adj bits
            if ( 0 < y )
            {
                const size_t pattern( value & 0x03 );

                if (( 0x00 == pattern ) || ( 0x03 == pattern ))
                    ++adj;
                else
                {
                    if ( 5 <= adj )
                        score1 += 3 + (adj-5);

                    adj = 1;
                }
            }

            // check for finder pattern
            if ( 10 < y )
            {
                const size_t pattern( value & 0x07ff );

                if (( 0x05d0 == pattern ) || ( 0x005d == pattern ))
                    ++finderPatternCount;
            }
        }

        if ( 5 <= adj )
            score1 += 3 + (adj-5);
    }

    // (2)
    // check for existance of 2x2 blocks
    // penalty:
    // 3 per occurence
    size_t score2( 3 * blockCount );

    // (3)
    // check for finder pattern
    // penalty:
    // 40 per occurence
    size_t score3( 40 * finderPatternCount );

    // (4)
    // calculate ratio of light to dark modules
    // penalty:
    // 10 per 5% deviation off 50%
    size_t ratio( (darkModuleCount * 100) / modules_.size() );

    if ( ratio < 50 )
        ratio = 50 - ratio;
    else
        ratio -= 50;

    size_t score4( 10 * (ratio/5) );

    // give total score to caller
    return (score1 + score2 + score3 + score4);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::generate( eccLevel level )
{
    static const size_t data[] = {
        0x5412, 0x5125, 0x5E7C, 0x5B4B, 0x45F9, 0x40CE, 0x4F97, 0x4AA0,
        0x77C4, 0x72F3, 0x7DAA, 0x789D, 0x662F, 0x6318, 0x6C41, 0x6976,
        0x1689, 0x13BE, 0x1CE7, 0x19D0, 0x0762, 0x0255, 0x0D0C, 0x083B,
        0x355F, 0x3068, 0x3F31, 0x3A06, 0x24B4, 0x2183, 0x2EDA, 0x2BED,
    };

    static const size_t formatValue[] = {
        0x0008, // L
        0x0000, // M
        0x0018, // Q
        0x0010, // H
    };

    const size_t format( formatValue[level] );

    size_t bestScore( std::numeric_limits<size_t>::max() );
    size_t index( 0 );

    // generate masks
    std::vector<moduleMatrix> masks;

    for ( size_t i( 0 ); i < 8; ++i )
    {
        masks.push_back( moduleMatrix( *this ) );

        // create format information
        for ( moduleList::const_iterator f( masks[i].formatting_.begin() ); f != masks[i].formatting_.end(); ++f )
            masks[i].setModuleValue( *f, data[format+i] );

        // calculate score
        const size_t score( masks[i].mask( i ) );

        // keep best score
        if ( score < bestScore )
        {
            index = i;
            bestScore = score;
        }
    }

    // use best mask
    assign( masks[index] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::initialize()
{
    const alignmentInfo ai( metrics::alignment( ver_ ) );
    const dataCapacityInfo dci( metrics::dataCapacity( ver_ ) );

    modulesPerSide_ = dci.numModulesPerSide;
    modules_.resize( modulesPerSide_ * modulesPerSide_ );

    createFinderPattern( 3, 3 );
    createFinderPattern( 3, modulesPerSide_ - 4 );
    createFinderPattern( modulesPerSide_ - 4, 3 );

    createTimingPattern();

    for ( size_t x( 0 ); x < 7; ++x )
        for ( size_t y( 0 ); y < 7; ++y )
            createAlignmentPattern( ai.location[x], ai.location[y] );

    createFormatInfo();
    createVersionInfo();

    // generate data module list
    coordinateList data;

    size_t x( modulesPerSide_ + 1 );
    size_t y;

    do
    {
        // upward
        x -= 2;
        y = modulesPerSide_;

        while ( y-- )
        {
            if ( module::Data == at( x, y ).type_ )
                data.push_back( coordinate( x, y ) );

            if ( module::Data == at( x-1, y ).type_ )
                data.push_back( coordinate( x-1, y ) );
        }

        // downward
        x -= 2;
        y = 0;

        if ( 6 == x )
            --x;

        do
        {
            if ( module::Data == at( x, y ).type_ )
                data.push_back( coordinate( x, y ) );

            if ( module::Data == at( x-1, y ).type_ )
                data.push_back( coordinate( x-1, y ) );

        } while ( ++y < modulesPerSide_ );

    } while ( 3 < x  );

    data_ = linkModules( data, module::Data );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::createAlignmentPattern( size_t x0, size_t y0 )
{
    // validate coordinates
    if (( !x0 ) || ( !y0 ))
        return;

    // do not create alignment pattern on top of finder pattern
    module &origin( at( x0, y0 ) );

    if ( module::FinderPattern == origin.type_ )
        return;

    // create pattern
    const size_t xbegin( x0 - 2 );
    const size_t xend( x0 + 3 );

    const size_t ybegin( y0 - 2 );
    const size_t yend( y0 + 3 );

    for ( size_t x( xbegin ); x < xend; ++x )
        for ( size_t y( ybegin ); y < yend; ++y )
        {
            const size_t dx( (x < x0) ? x0 - x : x - x0 );
            const size_t dy( (y < y0) ? y0 - y : y - y0 );

            module &m( at( x, y ) );

            m.type_ = module::AlignmentPattern;

            switch ( std::max( dx, dy ) )
            {
            case 0:
            case 2:
                m.value_ = true;
                break;
            default:
                m.value_ = false;
                break;
            }
        }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::createFinderPattern( size_t x0, size_t y0 )
{
    const size_t xbegin( (x0 < 4) ? 0 : x0 - 4 );
    const size_t xend( std::min( modulesPerSide_, x0 + 5 ) );

    const size_t ybegin( (y0 < 4) ? 0 : y0 - 4 );
    const size_t yend( std::min( modulesPerSide_, y0 + 5 ) );

    for ( size_t x( xbegin ); x < xend; ++x )
        for ( size_t y( ybegin ); y < yend; ++y )
        {
            const size_t dx( (x < x0) ? x0 - x : x - x0 );
            const size_t dy( (y < y0) ? y0 - y : y - y0 );

            module &m( at( x, y ) );
            m.type_ = module::FinderPattern;

            switch ( std::max( dx, dy ) )
            {
            case 0:
            case 1:
            case 3:
                m.value_ = true;
                break;
            case 4:
                m.type_ = module::Separator;
                break;
            default:
                break;
            }
        }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::createFormatInfo()
{
    //
    // generate lists for formatting info
    //

    coordinateList l0;
    l0.push_back( coordinate( modulesPerSide_ - 1, 8 ) );
    l0.push_back( coordinate( modulesPerSide_ - 2, 8 ) );
    l0.push_back( coordinate( modulesPerSide_ - 3, 8 ) );
    l0.push_back( coordinate( modulesPerSide_ - 4, 8 ) );
    l0.push_back( coordinate( modulesPerSide_ - 5, 8 ) );
    l0.push_back( coordinate( modulesPerSide_ - 6, 8 ) );
    l0.push_back( coordinate( modulesPerSide_ - 7, 8 ) );
    l0.push_back( coordinate( modulesPerSide_ - 8, 8 ) );
    l0.push_back( coordinate( 8, modulesPerSide_ - 7 ) );
    l0.push_back( coordinate( 8, modulesPerSide_ - 6 ) );
    l0.push_back( coordinate( 8, modulesPerSide_ - 5 ) );
    l0.push_back( coordinate( 8, modulesPerSide_ - 4 ) );
    l0.push_back( coordinate( 8, modulesPerSide_ - 3 ) );
    l0.push_back( coordinate( 8, modulesPerSide_ - 2 ) );
    l0.push_back( coordinate( 8, modulesPerSide_ - 1 ) );

    coordinateList l1;
    l1.push_back( coordinate( 8, 0 ) );
    l1.push_back( coordinate( 8, 1 ) );
    l1.push_back( coordinate( 8, 2 ) );
    l1.push_back( coordinate( 8, 3 ) );
    l1.push_back( coordinate( 8, 4 ) );
    l1.push_back( coordinate( 8, 5 ) );
    l1.push_back( coordinate( 8, 7 ) );
    l1.push_back( coordinate( 8, 8 ) );
    l1.push_back( coordinate( 7, 8 ) );
    l1.push_back( coordinate( 5, 8 ) );
    l1.push_back( coordinate( 4, 8 ) );
    l1.push_back( coordinate( 3, 8 ) );
    l1.push_back( coordinate( 2, 8 ) );
    l1.push_back( coordinate( 1, 8 ) );
    l1.push_back( coordinate( 0, 8 ) );

    //
    // update matrix with version info
    //

    formatting_.clear();
    formatting_.push_back( linkModules( l0, module::FormatInfo ) );
    formatting_.push_back( linkModules( l1, module::FormatInfo ) );

    // set the special format module
    module &m( at( 8, modulesPerSide_ - 8 ));
    m.type_ = module::FormatInfo;
    m.value_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::createTimingPattern()
{
    const size_t end( modulesPerSide_ - 8 );

    for ( size_t i( 8 ); i < end; ++i )
    {
        module &x( at( i, 6 ) );
        module &y( at( 6, i ) );

        x.type_ = y.type_ = module::TimingPattern;
        x.value_ = y.value_ = (i % 2) ? false : true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void moduleMatrix::createVersionInfo()
{
    if ( ver_ < Ver7 )
        return;

    //
    // generate lists for version info
    //

    coordinateList l0;
    l0.push_back( coordinate( modulesPerSide_ - 11, 0 ) );
    l0.push_back( coordinate( modulesPerSide_ - 10, 0 ) );
    l0.push_back( coordinate( modulesPerSide_ -  9, 0 ) );
    l0.push_back( coordinate( modulesPerSide_ - 11, 1 ) );
    l0.push_back( coordinate( modulesPerSide_ - 10, 1 ) );
    l0.push_back( coordinate( modulesPerSide_ -  9, 1 ) );
    l0.push_back( coordinate( modulesPerSide_ - 11, 2 ) );
    l0.push_back( coordinate( modulesPerSide_ - 10, 2 ) );
    l0.push_back( coordinate( modulesPerSide_ -  9, 2 ) );
    l0.push_back( coordinate( modulesPerSide_ - 11, 3 ) );
    l0.push_back( coordinate( modulesPerSide_ - 10, 3 ) );
    l0.push_back( coordinate( modulesPerSide_ -  9, 3 ) );
    l0.push_back( coordinate( modulesPerSide_ - 11, 4 ) );
    l0.push_back( coordinate( modulesPerSide_ - 10, 4 ) );
    l0.push_back( coordinate( modulesPerSide_ -  9, 4 ) );
    l0.push_back( coordinate( modulesPerSide_ - 11, 5 ) );
    l0.push_back( coordinate( modulesPerSide_ - 10, 5 ) );
    l0.push_back( coordinate( modulesPerSide_ -  9, 5 ) );

    coordinateList l1;
    l1.push_back( coordinate( 0, modulesPerSide_ - 11 ) );
    l1.push_back( coordinate( 0, modulesPerSide_ - 10 ) );
    l1.push_back( coordinate( 0, modulesPerSide_ -  9 ) );
    l1.push_back( coordinate( 1, modulesPerSide_ - 11 ) );
    l1.push_back( coordinate( 1, modulesPerSide_ - 10 ) );
    l1.push_back( coordinate( 1, modulesPerSide_ -  9 ) );
    l1.push_back( coordinate( 2, modulesPerSide_ - 11 ) );
    l1.push_back( coordinate( 2, modulesPerSide_ - 10 ) );
    l1.push_back( coordinate( 2, modulesPerSide_ -  9 ) );
    l1.push_back( coordinate( 3, modulesPerSide_ - 11 ) );
    l1.push_back( coordinate( 3, modulesPerSide_ - 10 ) );
    l1.push_back( coordinate( 3, modulesPerSide_ -  9 ) );
    l1.push_back( coordinate( 4, modulesPerSide_ - 11 ) );
    l1.push_back( coordinate( 4, modulesPerSide_ - 10 ) );
    l1.push_back( coordinate( 4, modulesPerSide_ -  9 ) );
    l1.push_back( coordinate( 5, modulesPerSide_ - 11 ) );
    l1.push_back( coordinate( 5, modulesPerSide_ - 10 ) );
    l1.push_back( coordinate( 5, modulesPerSide_ -  9 ) );

    //
    // set version info
    //

    static const size_t data[] = {
                  0x007C94, 0x0085BC, 0x009A99, 0x00A4D3,
        0x00BBF6, 0x00C762, 0x00D847, 0x00E60D, 0x00F928,
        0x010B78, 0x01145D, 0x012A17, 0x013532, 0x0149A6,
        0x015683, 0x0168C9, 0x0177EC, 0x018EC4, 0x0191E1,
        0x01AFAB, 0x01B08E, 0x01CC1A, 0x01D33F, 0x01ED75,
        0x01F250, 0x0209D5, 0x0216F0, 0x0228BA, 0x02379F,
        0x024B0B, 0x02542E, 0x026A64, 0x027541, 0x028C69,
    };

    setModuleValue( linkModules( l0, module::VersionInfo ), data[ver_ - Ver7] );
    setModuleValue( linkModules( l1, module::VersionInfo ), data[ver_ - Ver7] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
module *moduleMatrix::linkModules( const coordinateList &list, module::type t )
{
    module *head( NULL );
    module *prev( NULL );

    for ( coordinateList::const_iterator l( list.begin() ); l != list.end(); ++l )
    {
        module *m( &at( l->first, l->second ) );
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

} // namespace
