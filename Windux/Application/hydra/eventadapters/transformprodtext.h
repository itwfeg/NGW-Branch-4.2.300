/**
 * @file transformprodtext.h
 * @brief Conversion to and from product text event objects.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef TRANSFORMPRODTEXT_H
#define TRANSFORMPRODTEXT_H

#include <QString>

class Event;
class JsonDocumentEvent;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Conversion to and from product text event objects.
class TransformProductText
{
    using _Myt = TransformProductText;

public:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Convert from @c JsonDocumentEvent.
    /**
     * @param[in] e  event to convert
     * @param[out] tag  event tag
     * @return  event
     */
    static Event *convertFromEvent( const JsonDocumentEvent *e, QString& tag );

    /// Convert to @c JsonDocumentEvent.
    /**
     * @param[in] e  event to convert
     * @param[in] tag  event tag
     * @return  event
     */
    static JsonDocumentEvent *convertToEvent( const Event *e, const QString& tag );

private:

    // not implemented
    TransformProductText() = delete;

    // not implemented
    TransformProductText( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TRANSFORMPRODTEXT_H
