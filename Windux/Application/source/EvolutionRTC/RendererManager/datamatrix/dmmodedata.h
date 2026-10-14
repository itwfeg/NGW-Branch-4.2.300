/**
 * @file dmmodedata.h
 * Data Matrix encoded data.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DMMODEDATA_H
#define DMMODEDATA_H

#include "dmmode.h"

#include <bitstring.h>

/// Data Matrix Barcode Namespace
namespace dm
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Mode data.
class modeData
{
    typedef modeData _Myt;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] s  encodation scheme
     */
    modeData( mode::scheme s );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    modeData( const _Myt &other );

    /// Destructor.
    virtual ~modeData();

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

    /// Retrieve mode scheme.
    /**
     * @return  mode scheme
     */
    virtual mode::scheme modeScheme() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Assignment.
    /**
     * @param[in] other  object to copy
     */
    virtual void assign( const _Myt &other );

    /// Generate data bitstring.
    /**
     * @return  bitstring
     */
    virtual bitstring generate() const;

protected:

    mode::scheme scheme_;                           ///< Mode scheme.

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// ECI mode data.
class eciModeData : public modeData
{
    typedef eciModeData _Myt;
    typedef modeData _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] eci  ECI
     */
    eciModeData( unsigned int eci );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    eciModeData( const _Myt &other );

    /// Destructor.
    virtual ~eciModeData();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Assignment.
    /**
     * @param[in] other  object to copy
     */
    virtual void assign( const _Myt &other );

    /// Generate data bitstring.
    /**
     * @return  bitstring
     */
    virtual bitstring generate() const;

protected:

    unsigned int eci_;                              ///< ECI.

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Padding mode data.
class paddingModeData : public modeData
{
    typedef paddingModeData _Myt;
    typedef modeData _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] pos  padding position
     */
    paddingModeData( unsigned int pos );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    paddingModeData( const _Myt &other );

    /// Destructor.
    virtual ~paddingModeData();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Assignment.
    /**
     * @param[in] other  object to copy
     */
    virtual void assign( const _Myt &other );

    /// Generate data bitstring.
    /**
     * @return  bitstring
     */
    virtual bitstring generate() const;

protected:

    unsigned int pos_;                              ///< Position.

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Encoded mode data.
class encodedModeData : public modeData
{
    typedef encodedModeData _Myt;
    typedef modeData _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] s  mode scheme
     */
    encodedModeData( mode::scheme s );

    /// Constructor.
    /**
     * @param[in] s  mode scheme
     * @param[in] data  data to encode.
     */
    encodedModeData( mode::scheme s, const std::string &data );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    encodedModeData( const _Myt &other );

    /// Destructor.
    virtual ~encodedModeData();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set data.
    /**
     * Encode data using current mode.
     * @param[in] data  data to encode
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool setData( const std::string &data );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Assignment.
    /**
     * @param[in] other  object to copy
     */
    virtual void assign( const _Myt &other );

    /// Generate data bitstring.
    /**
     * @return  bitstring
     */
    virtual bitstring generate() const;

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Check if data is EDIFACT.
    /**
     * @param[in] data  data to check
     * @return  @c true upon success, @c false otherwise
     */
    static bool isEDIFACT( const std::string &data );

    /// Check if data is numeric.
    /**
     * @param[in] data  data to check
     * @return  @c true upon success, @c false otherwise
     */
    static bool isNumeric( const std::string &data );

    /// Check if data is EDIFACT.
    /**
     * @param[in] data  data to check
     * @return  @c true upon success, @c false otherwise
     */
    static bool isX12( const std::string &data );

protected:

    bitstring data_;                                ///< Encoded data stream.

    /// Encode data using mode.
    /**
     * @param[in] data  data to encode
     * @return  encoded bitstring
     */
    template <mode::scheme>
    bitstring encode( const std::string &data ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DMMODEDATA_H

