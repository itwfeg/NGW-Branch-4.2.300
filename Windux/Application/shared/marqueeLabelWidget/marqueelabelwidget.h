/**
 * @file marqueelabelwidget.h
 * Qt Widget class that represents a scrolling marquee.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup marqueeEditorCsu
 * @{
 */

#ifndef _MARQUEELABELWIDGET_H_
#define _MARQUEELABELWIDGET_H_

#include "marqueetextlist.h"

#include <QLabel>
#include <QTimer>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class MarqueeLabelWidget
 * @brief Label widget that scrolls.
 */
class MarqueeLabelWidget : public QLabel
{
    Q_OBJECT

    Q_PROPERTY( Direction direction READ direction WRITE setDirection )
    Q_PROPERTY( ScrollType scrollType READ scrollType WRITE setScrollType )
    Q_PROPERTY( int scrollSpeed READ scrollSpeed WRITE setScrollSpeed )
    Q_PROPERTY( QPoint position READ position RESET resetPosition )

    typedef MarqueeLabelWidget _Myt;
    typedef QLabel _Mybase;

public:

    /**
     * Scroll types.
     */
    enum ScrollType
    {
        Always,                             ///< text will always scroll
        OnlyWhenLarger                      ///< text will only scroll when width larger than control
    };

    /**
     * Scroll directions.
     */
    enum Direction
    {
        RightToLeft,                        ///< text scrolls from right side of screen to left
        LeftToRight,                        ///< text scrolls from left side of screen to right
        BottomToTop,                        ///< text scrolls from above to below
        TopToBottom                         ///< text scrolls from below to above
    };

    /**
     * Constructor.
     *
     * @param[in] parent  parent @c QWidget object
     */
    MarqueeLabelWidget( QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~MarqueeLabelWidget();

    /**
     * Get scroll type.
     *
     * @returns  scroll type
     */
    virtual ScrollType scrollType() const {return scrollType_;}

    /**
     * Get marquee direction.
     *
     * @returns  direction marquee travels
     */
    virtual Direction direction() const {return direction_;}

    /**
     * Get scroll speed in pixels per update.
     *
     * @returns  speed in pixels
     */
    virtual int scrollSpeed() const {return scrollSpeed_;}

    /**
     * Get scroll position.
     *
     * @returns  position of text within control
     */
    virtual QPoint position() const {return position_;}

public Q_SLOTS:

    /**
     * Set text alignment within label.
     *
     * @param[in] value  text alignment
     */
    virtual void setAlignment( Qt::Alignment value );

    /**
     * Set text font.
     *
     * @param[in] value  font
     */
    virtual void setFont( const QFont& value );

    /**
     * Set widget text.
     *
     * @param[in] value  widget text
     */
    virtual void setText( const QString& value );

    /**
     * Set scroll type.
     *
     * @param[in] value  scroll type
     */
    virtual void setScrollType( ScrollType value );

    /**
     * Set scroll direction.
     *
     * @param[in] value  direction marquee should travel
     */
    virtual void setDirection( Direction value );

    /**
     * Set scroll speed.
     *
     * @param[in] value  scroll speed in pixels per update
     */
    virtual void setScrollSpeed( int value );

    /**
     * Reset position of marquee text to start position.
     */
    virtual void resetPosition();

protected:

    /**
     * Paint widget event.
     *
     * @param[in,out] evt  event
     */
    virtual void paintEvent( QPaintEvent *evt );

    /**
     * Resize widget event.
     *
     * @param[in,out] evt  event
     */
    virtual void resizeEvent( QResizeEvent *evt );

    /**
     * Get begin location for current text.
     *
     * @returns  starting point
     */
    virtual QPoint beginLocation() const {return beginLocation_;}

    /**
     * Get hold location for current text.
     *
     * @returns  hold point
     */
    virtual QPoint frameLocation() const {return frameLocation_;}

    /**
     * Get end location for current text.
     *
     * @returns  ending point
     */
    virtual QPoint endLocation() const {return endLocation_;}

private:

    enum {TIMEOUT = 100};                   // timer timeout in milliseconds
    enum {BLINK_TIME = 500};                // blink time in milliseconds
    enum {HOLD_TIME = 1000};                // hold time in milliseconds

    enum {DEFAULT_DIRECTION = RightToLeft};
    enum {DEFAULT_SCROLL_TYPE = Always};

    enum ScrollState
    {
        Entry,
        Exit,
        DoneHide,
        Done
    };

    QString textWithoutMarkup_;

    marqueeTextList textList_;
    int textListIndex_;

    marqueeText text_;
    QPoint beginLocation_;
    QPoint frameLocation_;
    QPoint endLocation_;

    Direction direction_;
    ScrollType scrollType_;
    ScrollState scrollState_;
    int scrollSpeed_;

    QTimer timer_;
    QPoint position_;

    int blinkTimer_;
    int holdTimer_;

    // not implemented
    MarqueeLabelWidget( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /**
     * Check if blinking text is visible.
     */
    bool blinkTextVisible() const {return blinkTimer_ >= 0;}

    /**
     * Display next marquee text from list.
     */
    void displayNextMarquee();

    /**
     * Set widget marquee.
     */
    void setMarquee( const marqueeText& value );

    /**
     * Compute begin location for particular settings.
     */
    template <ScrollType _Type, Direction _Direction>
    QPoint calculateBeginLocationT() const;

    /**
     * Compute begin location for current text.
     */
    QPoint calculateBeginLocation() const;

    /**
     * Compute hold location for current text.
     */
    QPoint calculateFrameLocation() const;

    /**
     * Compute end location for current text.
     */
    QPoint calculateEndLocation() const;

    /**
     * Check current position if it has passed a set point.
     */
    bool checkPositionPast( const QPoint& point ) const;

private Q_SLOTS:

    /**
     * Timer timeout.
     */
    void onTimeout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif /*_MARQUEELABELWIDGET_H_*/
