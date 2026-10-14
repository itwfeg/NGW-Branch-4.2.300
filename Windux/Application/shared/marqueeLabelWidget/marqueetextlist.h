/**
 * @file marqueetextlist.h
 * List of scrolling marquee text objects.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup marqueeEditorCsu
 * @{
 */

#ifndef _MARQUEETEXTLIST_H_
#define _MARQUEETEXTLIST_H_

#include "marqueetext.h"

#include <QFontMetrics>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class marqueeTextList
 * @brief Container of @c marqueeText objects.
 *
 * This class offers a parser that will create the list automatically.
 */
class marqueeTextList : public QList<marqueeText>
{
    typedef marqueeTextList _Myt;
    typedef QList<marqueeText> _Mybase;

public:

    /**
     * Constructor.
     */
    marqueeTextList() : _Mybase() {}

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    marqueeTextList( const _Myt& rhs ) : _Mybase( rhs ) {}

    /**
     * Destructor.
     */
    virtual ~marqueeTextList() {}

    /**
     * Create marquee text list from string.
     *
     * This function will parse out marquee text and markup from a single string.
     *
     * @param[in] value  marquee to parse
     * @param[in] m  font metrics
     * @returns  list of marquee text objects
     */
    static _Myt create( const QString& value, const QFontMetrics& m );

private:

    /**
     * Check for tag at offset @p index and remove from string if present.
     */
    static bool validateRemoveTag( QString& text, int index, const QString& tag );

    /**
     * Convert string from one format to another.
     */
    static QString convertMarqueeText( const QString& value );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif /*_MARQUEETEXTLIST_H_*/
