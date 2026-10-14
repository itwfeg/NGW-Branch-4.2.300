/**
 * @file margins.h
 * Methods that extend the QMargins object.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef MARGINS_H
#define MARGINS_H

// Note:
// Operators contained herewithin have been added to Qt5
#if ( QT_VERSION < 0x050000 )

#include <QMargins>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Addition Operator.
 *
 * Adds together to @c QMargins objects and returns the result.
 *
 * @param[in] lhs  margins object to add to
 * @param[in] rhs  margins object to add
 * @returns  result of addition
 */
inline QMargins& operator += ( QMargins& lhs, const QMargins& rhs )
{
    lhs.setLeft( lhs.left() + rhs.left() );
    lhs.setTop( lhs.top() + rhs.top() );
    lhs.setRight( lhs.right() + rhs.right() );
    lhs.setBottom( lhs.bottom() + rhs.bottom() );

    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // QT_VERSION < 0x050000

#endif // MARGINS_H
