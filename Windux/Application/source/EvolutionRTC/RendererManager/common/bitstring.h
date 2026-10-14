/**
 * @file bitstring.h
 * String of bits.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef BITSTRING_H
#define BITSTRING_H

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Bit string.
class bitstring : public std::basic_string<bool, std::char_traits<bool>, std::allocator<bool> >
{
    typedef bitstring _Myt;
    typedef std::basic_string<bool, std::char_traits<bool>, std::allocator<bool> > _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    bitstring();

    /// Constructor.
    /**
     * @param[in] n  size of string
     * @param[in] val  initialization value
     */
    bitstring( size_t n, value_type val );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    bitstring( const _Mybase &other );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    bitstring( const _Myt &other );

    /// Destructor.
    virtual ~bitstring();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Append bits to string.
    /**
     * @param[in] value  value to append
     * @param[in] bits  number of bits
     */
    virtual void append( unsigned int value, size_t bits );

    /// Append bits to string.
    /**
     * @param[in] value  value to append
     */
    virtual void append( const std::string &data );

    /// Append bits to string.
    /**
     * @param[in] value  value to append
     */
    virtual void append( const _Myt &data );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BITSTRING_H
