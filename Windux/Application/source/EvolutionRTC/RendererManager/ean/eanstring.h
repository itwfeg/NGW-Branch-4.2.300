/**
 * @file eanstring.h
 * EAN String.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef EANSTRING_H
#define EANSTRING_H

#include "eantype.h"

#include <bitstring.h>

#include <string>

class bitmap;

/// EAN Barcode Namespace.
namespace ean
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// EAN String.
class string
{
    typedef string _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    string();

    /// Constructor.
    /**
     * @param[in] value  data to set
     * @param[in] t  ean type
     */
    string( const std::string &value, type t );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    string( const _Myt &other );

    /// Destructor.
    virtual ~string();

    // ========================================================================
    // Operators
    // ========================================================================

    /// Assignment operator.
    /**
     * @param[in] rhs  object to copy
     * @return  reference to this
     */
    _Myt &operator = ( const _Myt &rhs );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve encoded length.
    /**
     * @return  length
     */
    virtual size_t length() const {return s_.length();}

    /// Set data.
    /**
     * @param[in] value  data to set
     * @param[in] t  ean type
     */
    virtual void setData( const std::string &value, type t );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Assignment.
    /**
     * @param[in] other  object to copy
     */
    virtual void assign( const _Myt &other );

    /// Print matrix.
    virtual void print() const;

    /// Render string.
    /**
     * @param[in,out] b  bitmap to modify
     * @param[in] x  x coordinate
     * @param[in] y  y coordinate
     * @param[in] width  width of rectangle
     * @param[in] height  height of rectangle
     */
    virtual void render( bitmap &b, size_t x, size_t y, size_t width, size_t height ) const;

protected:

    bitstring s_;                                   ///< Encoded bitstring.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Generate bitstring from data.
    /**
     * @param[in] value  data to generate bitstring from
     * @param[in] t  ean barcode type
     */
    virtual void generate( const std::string &value, type t );

private:

    enum parity
    {
        leftEven,
        leftOdd,
        right
    };

    static const unsigned int leftEven_[10];
    static const unsigned int leftOdd_[10];
    static const unsigned int right_[10];

    static const parity parity_[10][6];

    static const unsigned int ean8_checkDigitWeight_[7];
    static const unsigned int ean13_checkDigitWeight_[12];

    /// Calculate check digit.
    static unsigned char calcCheckDigit( const std::string &value, type t );

    /// Encode EAN string.
    static bitstring encode( const std::string &value, type t );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // EAN13STRING_H

