/**
 * @file dmmode.h
 * Data Matrix Encodation Schemes.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DMMODE_H
#define DMMODE_H

#include <string>

/// Data Matrix Barcode Namespace
namespace dm
{

class modeData;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Mode.
class mode
{
    typedef mode _Myt;

public:

    /// Encodation schemes.
    enum scheme
    {
        Padding = 129,                              ///< Padding.
        C40 = 230,                                  ///< C40 encodation.
        Base256 = 231,                              ///< Base 256 encodation.
        FNC1 = 232,                                 ///< FNC1 Function Code.
        StructuredAppend = 233,                     ///< Structured append mode.
        X12 = 238,                                  ///< ANSI X12 encodation.
        Text = 239,                                 ///< Text endodation.
        EDIFACT = 240,                              ///< EDIFACT encodation.
        ECI = 241,                                  ///< Extended Channel Interpretation.
        ASCII = 800,                                ///< ASCII data.
        Numeric = 801,                              ///< Numeric value (2-digit data 00-99).
        Auto = 900                                  ///< Automatically choose best encoding.
    };

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Create encoded data.
    /**
     * @param[in] s  encodation scheme
     * @return  encoded data
     */
    static modeData *create( scheme s );

    /// Create encoded data.
    /**
     * @param[in] s  encodation scheme
     * @param[in] value  value
     * @return  encoded data
     */
    static modeData *create( scheme s, unsigned int value );

    /// Create encoded data.
    /**
     * @param[in] s  encodation scheme
     * @param[in] value  value
     * @return  encoded data
     */
    static modeData *create( scheme s, const std::string &value );

    /// Execute test cases.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    static bool test();

private:

    // not implemented
    mode();

    // not implemented
    _Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DMMODE_H

