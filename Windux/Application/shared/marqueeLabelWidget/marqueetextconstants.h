/**
 * @file marqueetextconstants.h
 * Constants relating to scrolling marquee text.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup marqueeEditorCsu
 * @{
 */

#ifndef MARQUEETEXTCONSTANTS_H
#define MARQUEETEXTCONSTANTS_H

#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Tag for text to scroll onto screen from right side.
 */
static const QString SCROLL( "<SCROLL>" );

/**
 * Tag for text to appear in middle of marquee.
 */
static const QString FRAME( "<FRAME>" );

/**
 * Tag for text to roll off marquee to left side.
 */
static const QString ROLL( "<ROLL>" );

/**
 * Tag for text to disappear from marquee.
 */
static const QString CLEAR( "<CLEAR>" );

/**
 * Tag for text to pause for 1 second.
 */
static const QString HOLD( "<HOLD>" );

/**
 * Tag for text to move slowly.
 */
static const QString SLOW( "<SLOW>" );

/**
 * Tag for text to move normally.
 */
static const QString NORMAL( "<NORMAL>" );

/**
 * Tag for text to move quickly.
 */
static const QString QUICK( "<QUICK>" );

/**
 * Tag for text to move extra quick.
 */
static const QString EXTRA_QUICK( "<X-QUICK>" );

/**
 * Tag to make text blink.
 */
static const QString BLINK_PREFIX( "<BLINKBEGIN>" );

/**
 * Tag to make text not blink.
 */
static const QString BLINK_SUFFIX( "<BLINKEND>" );

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MARQUEETEXTCONSTANTS_H
