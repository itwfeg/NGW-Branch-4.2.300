/**
 * @file qrmode.h
 * QR encoding mode.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef QRMODE_H
#define QRMODE_H

#include <string>

/// QR Barcode Namespace
namespace qr
{

class modeData;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Mode.
class mode
{
    typedef mode _Myt;

public:

    /// Mode data types.
    enum dataType
    {
        Terminator = 0,                             ///< Terminator.
        Numeric = 1,                                ///< Numeric decimal digits only (0-9).
        Alphanumeric = 2,                           ///< Alpha (Uppercase A-Z), Numeric (0-9), and special characters only (' ', $, %, *, +, -, ., /, :)
        StructuredAppend = 3,                       ///< Structured append mode.
        Byte = 4,                                   ///< Any 8-bit byte.
        FNC1_FirstPosition = 5,                     ///< FNC1.
        ECI = 7,                                    ///< Extended Channel Interpretation.
        Kanji = 8,                                  ///< Shift JIS system based on JIXS X 0208.
        FNC1_SecondPosition = 9                     ///< FNC1.
    };

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Create mode data.
    /**
     * @param[in] mdt  mode data type
     * @return  mode data
     */
    static modeData *create( dataType mdt );

    /// Create mode data.
    /**
     * @param[in] mdt  mode data type
     * @param[in] value  value
     * @return  mode data
     */
    static modeData *create( dataType mdt, unsigned int value );

    /// Create mode data.
    /**
     * @param[in] mdt  mode data type
     * @param[in] value  value
     * @return  mode data
     */
    static modeData *create( dataType mdt, const std::string &value );

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

#endif // QRMODE_H

