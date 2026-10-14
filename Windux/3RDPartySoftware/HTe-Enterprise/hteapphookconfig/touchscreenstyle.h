/**
 * @file touchscreenstyle.h
 * Style for touch screens.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef TOUCHSCREENSTYLE_H
#define TOUCHSCREENSTYLE_H

#include <QProxyStyle>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Style for touch screens.
class touchScreenStyle : public QProxyStyle
{
    typedef touchScreenStyle _Myt;
    typedef QProxyStyle _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    touchScreenStyle();

    /// Destructor.
    virtual ~touchScreenStyle();

    // ========================================================================
    // Properties
    // ========================================================================

    /**
     * Returns the value of the given pixel @p metric.
     *
     * @param[in] metric  metric to retrieve
     * @param[in] option  style option
     * @param[in] widget  widget for metric
     * @returns  pixel metric
     */
    virtual int pixelMetric( PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0 ) const;

    /**
     * Returns the rectangle containing the specified @p subControl of the given complex @p control
     * (with the style specified by @p option). The rectangle is defined in screen coordinates.
     *
     * @param[in] control  complex control type
     * @param[in] option  style option
     * @param[in] subControl  sub control type
     * @param[in] widget  widget to style
     */
    virtual QRect subControlRect( ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget = 0 ) const;

    /**
     * Returns an integer representing the specified style @p hint for the given @p widget
     * described by the provided style @p option.
     *
     * @param[in] hint  style hint
     * @param[in] option  style option
     * @param[in] widget  widget
     * @param[out] returnData  style hint return data
     * @returns  style hint
     */
    virtual int	styleHint( StyleHint hint, const QStyleOption *option = 0, const QWidget *widget = 0, QStyleHintReturn *returnData = 0 ) const;

    // ========================================================================
    // Methods
    // ========================================================================

    /**
     * Draws the given @p control using the provided @p painter with the style options specified
     * by @p option.
     *
     * The @p widget argument is optional and can be used as aid in drawing the control.
     *
     * The @p option parameter is a pointer to a @c QStyleOptionComplex object that can be cast to
     * the correct subclass using the qstyleoption_cast() function. Note that the @c rect member
     * of the specified @p option must be in logical coordinates. Reimplementations of this
     * function should use visualRect() to change the logical coordinates into screen coordinates
     * before calling the drawPrimitive() or drawControl() function.
     *
     * @param[in] control  control to draw
     * @param[in] option  style options
     * @param[in] painter  painter to draw with
     * @param[in] widget  widget being drawn
     *
     * @sa drawPrimitive()
     * @sa drawControl()
     */
    virtual void drawComplexControl( ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = 0 ) const;

    /**
     * Draws the given primitive @p element with the provided painter using the style options
     * specified by @p option.
     *
     * The @p widget argument is optional and may contain a widget that may aid in drawing the
     * primitive @p element.
     *
     * @param[in] element  primitive to draw
     * @param[in] option  style options
     * @param[in] painter  painter to draw with
     * @param[in] widget  widget being drawn
     *
     * @sa drawComplexControl()
     * @sa drawControl()
     */
    virtual void drawPrimitive( PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0 ) const;

    /**
     * Initializes the appearance of the given @p widget.
     *
     * This function is called for every widget at some point after it has been fully created but
     * just before it is shown for the very first time.
     *
     * @param[in] widget  widget to polish
     *
     * @sa unpolish()
     */
    virtual void polish( QWidget *widget );

private:

    enum Direction
    {
        TopDown,
        FromLeft,
        BottomUp,
        FromRight
    };

    static const int minimumThickness_;

    static const QString baseStyle_;

    /// Draw a gradient.
    static void drawGradient( QPainter *painter, const QRect& rect, const QColor& gradientStart, const QColor& gradientStop, Direction direction = TopDown, QBrush bgBrush = QBrush() );

    /// Merge colors.
    static QColor mergedColors( const QColor& colorA, const QColor& colorB, int factor = 50 );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TOUCHSCREENSTYLE_H
