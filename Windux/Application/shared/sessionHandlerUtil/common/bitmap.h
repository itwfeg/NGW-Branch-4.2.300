/**
 * @file bitmap.h
 * Bitmap class.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef BITMAP_H
#define BITMAP_H

#include "buffer.h"

#include <map>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dimensions type.
typedef std::pair<size_t, size_t> dimensions_t; // width, height

/// Point type.
typedef std::pair<size_t, size_t> point_t; // x, y

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Bitmap class.
///
/// Multiple bits per pixel are supported:
///  1 = monochrome; 0 = white, 1 = black
///  2 = grayscale
///  4 = grayscale
///  8 = grayscale
/// 16 = color; rgb565 format
/// 32 = color; argb format
class bitmap
{
    typedef bitmap _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    bitmap();

    /// Constructor.
    /**
     * @param[in] width  bitmap width
     * @param[in] height  bitmap height
     * @param[in] bpp  bits per pixel
     */
    bitmap( size_t width, size_t height, uint32_t bpp = 1 );

    /// Constructor.
    /**
     * @param[in] size  size of bitmap
     * @param[in] bpp  bits per pixel
     */
    bitmap( const dimensions_t &size, uint32_t bpp = 1 );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    bitmap( const _Myt &other );

    /// Destructor.
    virtual ~bitmap();

    // ========================================================================
    // Operators
    // ========================================================================

    /// Assignment operator.
    /**
     * @param[in] rhs  object to copy
     * @return  reference to this
     */
    _Myt &operator = ( const _Myt &rhs );

    /// Equality operator.
    /**
     * @param[in] rhs  object to compare
     * @return  @c true if equal, @c false otherwise
     */
    bool operator == ( const _Myt &rhs ) const;

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve black pixel color.
    /**
     * @return  black pixel
     */
    virtual uint32_t black() const;

    /// Retrieve bits per pixel.
    /**
     * @return  bits per pixel
     */
    virtual uint32_t bpp() const;

    /// Retrieve height.
    /**
     * @return  height
     */
    virtual size_t height() const;

    /// Retrieve pixel value.
    /**
     * @param[in] x  x coordinate
     * @param[in] y  y coordinate
     * @return  pixel value
     */
    virtual uint32_t pixel( size_t x, size_t y ) const;

    /// Retrieve pixel value.
    /**
     * @param[in] p  point
     * @return  pixel value
     */
    virtual uint32_t pixel( const point_t &p ) const;

    /// Set pixel value.
    /**
     * @param[in] x  x coordinate
     * @param[in] y  y coordinate
     * @param[in] value  pixel value
     */
    virtual void setPixel( size_t x, size_t y, uint32_t value );

    /// Set pixel value.
    /**
     * @param[in] p  point
     * @param[in] value  pixel value
     */
    virtual void setPixel( const point_t &p, uint32_t value );

    /// Retrieve size.
    /**
     * @return  size
     */
    virtual dimensions_t size() const;

    /// Check if bitmap is valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool valid() const;

    /// Retrieve white pixel color.
    /**
     * @return  black pixel
     */
    virtual uint32_t white() const;

    /// Retrieve width.
    /**
     * @return  width
     */
    virtual size_t width() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Assignment.
    /**
     * @param[in] other  object to copy
     */
    virtual void assign( const _Myt &other );

    /// Draw rectangle.
    /**
     * @param[in] p  point
     * @param[in] size  size or rectangle
     * @param[in] value  fill value
     */
    virtual void drawRect( const point_t &p, const dimensions_t &size, uint32_t value );

    /// Draw rectangle.
    /**
     * @param[in] x  x coordinate
     * @param[in] y  y coordinate
     * @param[in] width  bitmap width
     * @param[in] height  bitmap height
     * @param[in] value  fill value
     */
    virtual void drawRect( size_t x, size_t y, size_t width, size_t height, uint32_t value );

    /// Fill bitmap with value.
    /**
     * @param[in] value  fill value
     */
    virtual void fill( uint32_t value );

    /// Convert to image.
    /**
     * @param[in] format  image format (i.e. png, jpeg)
     * @return  image buffer
     */
    virtual buffer toImage( const std::string &format ) const;

protected:

    dimensions_t size_;                             ///< Size.
    uint32_t bpp_;                                  ///< Bits per pixel.

    buffer bits_;                                   ///< Raw bits.

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve scan line width.
    /**
     * @return  width of scan line (bytes)
     */
    virtual size_t scanLineWidth() const;

    /// Retrieve offset of point.
    /**
     * @param[in] x  x coordinate
     * @param[in] y  y coordinate
     * @return  offset
     */
    virtual size_t offset( size_t x, size_t y ) const;

    /// Retrieve offset of point.
    /**
     * @param[in] p  point
     * @return  offset
     */
    virtual size_t offset( const point_t &p ) const;

private:

    uint32_t ver_;

    size_t scanLineWidth_;
    size_t m_;

    static const uint32_t mask_[];
    static const uint32_t shift_[][8];

    static const uint32_t alignment_;

    /// Initialize.
    void initialize();

    /// Read pixel.
    template <uint32_t V>
    uint32_t read( size_t x, size_t y ) const;

    /// Write pixel.
    template <uint32_t V>
    void write( size_t x, size_t y, uint32_t value );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BITMAP_H

