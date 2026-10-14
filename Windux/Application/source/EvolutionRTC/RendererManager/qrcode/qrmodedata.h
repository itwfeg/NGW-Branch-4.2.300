/**
 * @file qrmodedata.h
 * QR mode data.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef QRMODEDATA_H
#define QRMODEDATA_H

#include "qrmode.h"
#include "qrversion.h"

#include <bitstring.h>

/// QR Barcode Namespace
namespace qr
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
     * @param[in] m  mode type
     */
    modeData( mode::dataType mdt );

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

    /// Retrieve max length.
    /**
     * @return  max length
     */
    virtual size_t maxLength() const;

    /// Retrieve mode data type.
    /**
     * @return  mode data type
     */
    virtual mode::dataType modeDataType() const;

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
     * @param[in] v  version type
     * @return  bitstring
     */
    virtual bitstring generate( version ver ) const;

protected:

    mode::dataType type_;                           ///< Mode data type.

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
    // Properties
    // ========================================================================

    /// Retrieve max length.
    /**
     * @return  max length
     */
    virtual size_t maxLength() const;

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
     * @param[in] v  version type
     * @return  bitstring
     */
    virtual bitstring generate(  version ver ) const;

protected:

    unsigned int eci_;                              ///< ECI.

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
     * @param[in] m  mode type
     */
    encodedModeData( mode::dataType mdt );

    /// Constructor.
    /**
     * @param[in] m  mode type
     * @param[in] data  data to encode.
     */
    encodedModeData( mode::dataType mdt, const std::string &data );

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

    /// Retrieve character count length.
    /**
     * @param[in] v  version type
     * @return  length (in bits)
     */
    virtual size_t characterCountLen( version ver ) const;

    /// Retrieve max length.
    /**
     * @return  max length
     */
    virtual size_t maxLength() const;

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
     * @param[in] v  version type
     * @return  bitstring
     */
    virtual bitstring generate( version ver ) const;

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Check if data is alphanumeric.
    /**
     * @param[in] data  data to check
     * @return  @c true upon success, @c false otherwise
     */
    static bool isAlphanumeric( const std::string &data );

    /// Check if data is numeric.
    /**
     * @param[in] data  data to check
     * @return  @c true upon success, @c false otherwise
     */
    static bool isNumeric( const std::string &data );

protected:

    unsigned int characterCount_;                   ///< Number of characters encoded.
    bitstring data_;                                ///< Encoded data stream.

    /// Encode data using mode.
    /**
     * @param[in] data  data to encode
     */
    template <mode::dataType>
    void encode( const std::string &data );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // QRMODEDATA_H

