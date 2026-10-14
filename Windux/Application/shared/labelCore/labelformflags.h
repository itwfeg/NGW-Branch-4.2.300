/**
 * @file labelformflags.h
 * Label form flags.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
* @addtogroup labelViewerCsu
 * @{
 */

#ifndef LABELFORMFLAGS_H
#define LABELFORMFLAGS_H

#include <QString>
#include <QTextStream>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labelFormFlags
 * @brief Label form flags.
 *
 * Enumeration class of label form flags.
 *
 * @sa labelForm
 */
class labelFormFlags
{
public:

    /**
     * Label form flags.
     *
     * @note
     * When adding new type you will need to add a template function for conversion into string
     * form at the bottom of this header.  You will also then need to modify fromString() and
     * toString() in the cpp file.
     */
    enum type
    {
        TEXT_ENTITY                         = 0x0001,           ///< Text form

        BARCODE_ENTITY                      = 0x0002,           ///< Barcode form

        GRAPHIC_ENTITY                      = 0x0004,           ///< Graphic form

        LINE_ENTITY                         = 0x0008,           ///< Line form

        PRE_IMAGE                           = 0x0010,           ///< Form will be pre-imaged

        POST_IMAGE                          = 0x0020,           ///< Form will be post-imaged

        FIXED                               = 0x0100,           ///< Form stays where specified (location and size)

        FLOATING                            = 0x0200,           ///< Forms can float upwards (size stays the same) on continuous label
                                                                ///< stock only, based on the space left available by @c VARIABLE forms
                                                                ///< above it.

        STRETCH                             = 0x0400,           ///< Forms can stretch upwards (top moves up, bottom stays the same) based
                                                                ///< on the space left available by @c VARIABLE forms above it.

        VARIABLE                            = 0x0800,           ///< Form size can vary based on the amount of text to render inside the
                                                                ///< form (only for pre-image, text forms).

        RECT_BORDER                         = 0x1000,           ///< Form is drawn with a border around it.

        INVERTED                            = 0x2000,            ///< Print White text on Black background.

        DIGIMARC_KEEP_OUT                   = 0x4000,            ///< Do not perform Digimarc watermarking in the area of this field.

        CODE_CHECKER_DISABLED               = 0x8000,            ///< Disable code checker for this field (i.e. print all dots in this
                                                                 ///< field regardless of printer head state).
    };

    /**
     * Convert from string.
     *
     * @param[in] flags  label form flag(s) in string form
     * @returns  label form flag
     */
    static type fromString( const QString& flags );

    /**
     * Convert to string.
     *
     * @returns  label form flag in string form
     *
     * @tparam _Type  flag to convert
     */
    template <type _Type>
    static QString toString();

    /**
     * Convert to string.
     *
     * @param[in] flags  label form flag(s)
     * @returns  label form flag(s) in string form
     */
    static QString toString( type flags );

    /**
     * Check if particular flag is set.
     *
     * @param[in] flags  flag(s) value to check
     * @returns  @c true if @p _Flag is set within @p flags, @c false otherwise
     *
     * @tparam _Flag  flag to look for
     */
    template <type _Flag>
    static bool isSet( type flags ) {
        return (_Flag == (flags & _Flag));
    }

private:

    /**
     * Check if text contains flag and set.
     */
    template <type _Flag>
    static void checkAppendFlag( const QString& flags, type result ) {
        if ( flags.contains( toString<_Flag>() ) )
            result = (type) (result | _Flag);
    }

    /**
     * Check if flag is set and append text.
     */
    template <type _Flag>
    static void checkAppendFlag( type flags, QTextStream& os ) {
        if ( isSet<_Flag>( flags ) )
            os << toString<_Flag>() << "|";
    }

};

/**
 * This override handles converting @c TEXT_ENTITY into string form.
 */
template <>
inline QString labelFormFlags::toString<labelFormFlags::TEXT_ENTITY>() {
    return "TEXT";
}

/**
 * This override handles converting @c BARCODE_ENTITY into string form.
 */
template <>
inline QString labelFormFlags::toString<labelFormFlags::BARCODE_ENTITY>() {
    return "BARCODE";
}

/**
 * This override handles converting @c GRAPHIC_ENTITY into string form.
 */
template <>
inline QString labelFormFlags::toString<labelFormFlags::GRAPHIC_ENTITY>() {
    return "GRAPHIC";
}

/**
 * This override handles converting @c LINE_ENTITY into string form.
 */
template <>
inline QString labelFormFlags::toString<labelFormFlags::LINE_ENTITY>() {
    return "LINE";
}

/**
 * This override handles converting @c PRE_IMAGE into string form.
 */
template <>
inline QString labelFormFlags::toString<labelFormFlags::PRE_IMAGE>() {
    return "PRE_IMAGE";
}

/**
 * This override handles converting @c POST_IMAGE into string form.
 */
template <>
inline QString labelFormFlags::toString<labelFormFlags::POST_IMAGE>() {
    return "POST_IMAGE";
}

/**
 * This override handles converting @c FIXED into string form.
 */
template <>
inline QString labelFormFlags::toString<labelFormFlags::FIXED>() {
    return "FIXED";
}

/**
 * This override handles converting @c FLOATING into string form.
 */
template <>
inline QString labelFormFlags::toString<labelFormFlags::FLOATING>() {
    return "FLOATING";
}

/**
 * This override handles converting @c STRETCH into string form.
 */
template <>
inline QString labelFormFlags::toString<labelFormFlags::STRETCH>() {
    return "STRETCH";
}

/**
 * This override handles converting @c VARIABLE into string form.
 */
template <>
inline QString labelFormFlags::toString<labelFormFlags::VARIABLE>() {
    return "VARIABLE";
}

/**
 * This override handles converting @c RECT_BORDER into string form.
 */
template <>
inline QString labelFormFlags::toString<labelFormFlags::RECT_BORDER>() {
    return "BORDER";
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LABELFORMFLAG_H
