/**
 * @file poly.h
 * Reed Solomon polynomial.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef POLY_H
#define POLY_H

#include <buffer.h>

/// QR Barcode Namespace
namespace qr
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Reed Solomon polynomial.
class poly : public buffer
{
    typedef poly _Myt;
    typedef buffer _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    poly();

    /// Constructor.
    /**
     * @param[in] n  size of polynomial
     * @param[in] val  initialization value
     */
    poly( size_t n, value_type val );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    poly( const _Mybase &other );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    poly( const _Myt &other );

    /// Constructor.
    /**
     * @param[in] s  bitstring
     * @param[in] len  minimum length (padding added if required)
     */
    poly( const bitstring &s, size_t len = 0 );

    /// Destructor.
    virtual ~poly();

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Addition of two polynomials.
    /**
     * @param[in] p  right operand polynomial
     * @param[in] q  left operand polynomial
     * @return  polynomial
     */
    static _Myt add( const _Myt &p, const _Myt &q );

    /// Division of two polynomials.
    /**
     * @param[in] p  right operand polynomial
     * @param[in] q  left operand polynomial
     * @return  polynomial
     */
    static _Myt div( const _Myt &p, const _Myt &q );

    /// Evaluation of polynomial in x.
    /**
     * @param[in] p  polynomial to evaluate
     * @param[in] x  evaluation point
     * @return  result
     */
    static int8_t eval( const _Myt &p, uint16_t x );

    /// Multiplication of two polynomials.
    /**
     * @param[in] p  right operand polynomial
     * @param[in] q  left operand polynomial
     * @return  polynomial
     */
    static _Myt mul( const _Myt &p, const _Myt &q );

    /// Multiplication of polynomial by scalar.
    /**
     * @param[in] p  polynomial
     * @param[in] x  scalar
     * @return  polynomial
     */
    static _Myt scale( const _Myt &p, uint16_t x );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Addition in galois field
    /**
     * @param[in] x  right operand
     * @param[in] y  left operand
     * @return  x+y
     */
    static uint8_t add( uint8_t x, uint8_t y );

    /// Division in galois field
    /**
     * @param[in] x  right operand
     * @param[in] y  left operand
     * @return  x/y
     */
    static uint8_t div( uint8_t x, uint8_t y );

    /// Inverse.
    /**
     * @param[in] x  number
     * @return  inversion of x
     */
    static uint8_t inverse( uint8_t x );

    /// Multiplication in galois field
    /**
     * @param[in] x  right operand
     * @param[in] y  left operand
     * @return  x*y
     */
    static uint8_t mul( uint16_t x, uint16_t y );

    /// Power.
    /**
     * @param[in] x  operand
     * @param[in] power  power
     * @return  x^power
     */
    static uint8_t pow( uint8_t x, intmax_t power );

    /// Subtraction in galois field
    /**
     * @param[in] x  right operand
     * @param[in] y  left operand
     * @return  x-y
     */
    static uint8_t sub( uint8_t x, uint8_t y );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Generate Reed Solomon poly.
    /**
     * @param[in] nsym  number symbols
     * @return  reed solomon generator poly
     */
    static _Myt reedSolomonGenerator( size_t nsym );

private:

    static const uint8_t exp_[];
    static const uint8_t log_[];

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // POLY_H

