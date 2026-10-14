/**
 * @file hobartprinter.h
 * Hobart Printer Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTPRINTER_H
#define HOBARTPRINTER_H

#include <cstddef>

#include <QMetaType>

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart Printer Device.
class hobartPrinter
{
    typedef hobartPrinter _Myt;

public:

    /// Printer types.
    enum printerType
    {
        first = 0,                                  ///< First printer.
        second,                                     ///< Second printer.
        third                                       ///< Third printer.
    };

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~hobartPrinter();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve printer type.
    /**
     * @return  type
     */
    virtual printerType type() const {return t_;}

protected:

    printerType t_;                                 ///< Printer type.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] t  printer type
     */
    hobartPrinter( printerType t );

private:

    // not implemented
    hobartPrinter( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTPRINTER_H
