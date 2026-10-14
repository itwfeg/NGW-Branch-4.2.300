/**
 * @file buffer.h
 * Generic buffer class.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef BUFFER_H
#define BUFFER_H

#include "bitstring.h"

#include <cstdint>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Buffer.
class buffer : public std::basic_string<uint8_t, std::char_traits<uint8_t>, std::allocator<uint8_t> >
{
    typedef buffer _Myt;
    typedef std::basic_string<uint8_t, std::char_traits<uint8_t>, std::allocator<uint8_t> > _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    buffer();

    /// Constructor.
    /**
     * @param[in] b  buffer
     * @param[in] bs  buffer size
     */
    buffer( const value_type *b, size_t bs );

    /// Constructor.
    /**
     * @param[in] n  size of buffer
     * @param[in] val  initialization value
     */
    buffer( size_t n, value_type val );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    buffer( const _Mybase &other );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    buffer( const _Myt &other );

    /// Constructor.
    /**
     * @param[in] s  bitstring
     * @param[in] len  minimum length (padding added if required)
     */
    buffer( const bitstring &s, size_t len = 0 );

    /// Destructor.
    virtual ~buffer();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Save buffer to file.
    /**
     * @param[in] filename  filename
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool save( const std::string &filename ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BUFFER_H

