/**
 * @file transformresource.h
 * @brief Conversion to and from file resource event objects.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef TRANSFORMRESOURCE_H
#define TRANSFORMRESOURCE_H

#include <QString>

class Event;
class FileResourceEvent;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Conversion to and from file resource event objects.
class TransformResource
{
    using _Myt = TransformResource;

public:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Convert from @c FileResourceEvent.
    /**
     * Convert graphic and font file resource events.
     * @param[in] e  event to convert
     * @param[out] tag  event tag
     * @return  event
     */
    static Event *convertFromEvent( const FileResourceEvent *e, QString& tag );

    /// Convert to @c FileResourceEvent.
    /**
     * Convert graphic and font file resource events.
     * @param[in] e  event to convert
     * @param[in] tag  event tag
     * @return  event
     */
    static FileResourceEvent *convertToEvent( const Event *e, const QString& tag );

private:

    // not implemented
    TransformResource() = delete;

    // not implemented
    TransformResource( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TRANSFORMRESOURCE_H
