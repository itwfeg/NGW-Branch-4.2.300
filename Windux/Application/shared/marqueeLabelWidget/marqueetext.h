/**
 * @file marqueetext.h
 * Scrolling marquee text.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup marqueeEditorCsu
 * @{
 */

#ifndef _MARQUEETEXT_H_
#define _MARQUEETEXT_H_

#include "marqueetextfragment.h"

#include <QPoint>

class QPainter;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class marqueeText
 * @brief Text object for @c MarqueeLabelWidget.
 *
 * Marquee text enters and exits a marquee label in predefined ways.  It may also do special
 * actions while inside the marquee.  This class holds marquee text and all these behaviors
 * associated with the text.
 *
 * Behaviors are embedded within the text via tags.  Here are some of the possible tags:
 * @arg <SCROLL> text will scroll onto the screen from the side
 * @arg <FRAME> text will just appear on the screen
 * @arg <ROLL> text will roll off the screen
 * @arg <CLEAR> text will disappear from the screen
 * @arg <HOLD> pause the text for one second
 *
 * As an example:
 * @code
 * <SCROLL>Widget Marquee Labels<HOLD><HOLD><CLEAR><FRAME>ROCK!<HOLD><ROLL>
 * @endcode
 *
 * Will cause the text "Widget Marquee Labels" to scroll into the marquee, pause for 2 seconds,
 * and then disappear.  Then the text "ROCK!" will appear in the marquee, pause for a second, and
 * roll off the screen.
 */
class marqueeText : public QList<marqueeTextFragment>
{
    typedef marqueeText _Myt;
    typedef QList<marqueeTextFragment> _Mybase;

public:

    /**
     * Text entry behavior.
     *
     * This enumeration controls how text comes onto the marquee.
     */
    enum EntryBehavior
    {
        Scroll,                                         ///< scrolls onto the screen
        Frame                                           ///< appear on screen in the middle and hold
    };

    /**
     * Text exit behavior.
     *
     * This enumeration controls how text exits the marquee.
     */
    enum ExitBehavior
    {
        Roll,                                           ///< rolls off screen
        Clear                                           ///< disappears
    };

    /**
     * Marquee scrolling speeds.
     *
     * These are example values, you may use any value you wish.
     */
    enum
    {
        SCROLL_SPEED_SLOW = 4,                          ///< slow
        SCROLL_SPEED_NORMAL = 8,                        ///< normal
        SCROLL_SPEED_QUICK = 12,                        ///< quick
        SCROLL_SPEED_EXTRA_QUICK = 24                   ///< extra quick
    };

    /**
     * Here are the locations where hold time may occur.
     */
    enum HoldLocation
    {
        BeforeEntry = 0,                                ///< hold before entry behavior is processed
        DuringFrame,                                    ///< hold at the frame location
        AfterExit                                       ///< hold after exit behavior is processed
    };

    /**
     * Constructor.
     */
    marqueeText() :
        _Mybase(),
        entryBehavior_( (EntryBehavior) DEFAULT_ENTRY_BEHAVIOR ),
        exitBehavior_( (ExitBehavior) DEFAULT_EXIT_BEHAVIOR ),
        scrollSpeed_( DEFAULT_SCROLL_SPEED ) {
        resetHoldTimes();
    }

    /**
     * Constructor.
     *
     * @param[in] rhs  object to copy
     */
    marqueeText( const _Myt& rhs ) :
        _Mybase() {
        copy( rhs );
    }

    /**
     * Destructor.
     */
    virtual ~marqueeText() {}

    /**
     * Assignment operator.
     *
     * @param[in] rhs  value to assign from
     * @returns  reference to @c this
     */
    _Myt& operator = ( const _Myt& rhs ) {
        copy( rhs );
        return *this;
    }

    /**
     * Get text.
     *
     * @returns  text
     */
    virtual QString text() const;

    /**
     * Set text.
     *
     * @param[in] value  text
     * @param[in] m  font metrics
     */
    virtual void setText( const QString& value, const QFontMetrics& m );

    /**
     * Retrieve size of text in pixels.
     *
     * @returns  size of text in pixels
     */
    virtual QSize textSize() const {return textSize_;}

    /**
     * Retrieve how text should come into the marquee.
     *
     * @returns  marquee entry behavior
     */
    virtual EntryBehavior entryBehavior() const {return entryBehavior_;}

    /**
     * Set how text should enter the marquee.
     *
     * @param[in] value  marquee entry behavior
     */
    virtual void setEntryBehavior( EntryBehavior value ) {entryBehavior_ = value;}

    /**
     * Retrieve how text should exit the marquee.
     *
     * @returns  marquee exit behavior
     */
    virtual ExitBehavior exitBehavior() const {return exitBehavior_;}

    /**
     * Set how text should exit the marquee.
     *
     * @param[in] value  marquee exit behavior
     */
    virtual void setExitBehavior( ExitBehavior value ) {exitBehavior_ = value;}

    /**
     * Get marquee text scroll speed.
     *
     * @returns  speed in pixels per update
     */
    virtual int scrollSpeed() const {return scrollSpeed_;}

    /**
     * Set marquee text scroll speed.
     *
     * @param[in] value  speed in pixes per update
     */
    virtual void setScrollSpeed( int value ) {scrollSpeed_ = value;}

    /**
     * Get the text hold time.
     *
     * @param[in] where  which hold location to retrieve
     * @returns  hold time in seconds
     */
    virtual int holdTime( HoldLocation where ) const;

    /**
     * Set the text hold time.
     *
     * @param[in] where  which hold location to retrieve
     * @param[in] value  hold time in seconds
     */
    virtual void setHoldTime( HoldLocation where, int value ) {holdTimes_[where] = value;}

    /**
     * Increment a hold time by one.
     *
     * @param[in] where  which hold location to retrieve
     */
    virtual void incrementHoldTime( HoldLocation where ) {++holdTimes_[where];}

    /**
     * Reset all hold times to zero.
     */
    virtual void resetHoldTimes() {
        for ( int i = 0; i < HOLD_TIMES; ++i )
            holdTimes_[i] = 0;
    }

    /**
     * Paint text.
     *
     * @param[in] painter  painter to paint with
     * @param[in] pos  position to paint text
     * @param[in] blinkTextVisible  @c true if blink text should be visible, @c false otherwise
     */
    virtual void paint( QPainter *painter, const QPoint& pos, bool blinkTextVisible ) const;

private:

    enum {DEFAULT_ENTRY_BEHAVIOR = Scroll};
    enum {DEFAULT_EXIT_BEHAVIOR = Roll};
    enum {DEFAULT_SCROLL_SPEED = SCROLL_SPEED_NORMAL};

    enum {HOLD_TIMES = 3};

    QSize textSize_;

    EntryBehavior entryBehavior_;
    ExitBehavior exitBehavior_;
    int scrollSpeed_;

    int holdTimes_[HOLD_TIMES];

    /**
     * Copy value from passed in object.
     */
    void copy( const _Myt& rhs );

    /**
     * Calculate text size.
     */
    QSize calculateTextSize() const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif /*_MARQUEETEXT_H_*/
