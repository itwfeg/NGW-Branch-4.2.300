/**
 * @file touchscreenstyle.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "touchscreenstyle.h"

#include <QPushButton>
#include <QToolButton>

#include <QPainter>
#include <QStyleFactory>
#include <QStyleOptionComplex>

const int TouchScreenStyle::minimumThickness_ = 20;

const QString TouchScreenStyle::baseStyle_( "fusion" );

static const char * const qt_cleanlooks_radiobutton[] = {
    "32 32 9 1",
    " 	c None",
    ".	c #ABA094",
    "+	c #B7ADA0",
    "@	c #C4BBB2",
    "#	c #DDD4CD",
    "$	c #E7E1E0",
    "%	c #F4EFED",
    "&	c #FFFAF9",
    "*	c #FCFEFB",
    "            @@@@@@@@            ",
    "         @@@********@@@         ",
    "        @**************@        ",
    "      @@****************@@      ",
    "     @********************@     ",
    "    @**********************@    ",
    "   @************************@   ",
    "   @************************@   ",
    "  @**************************@  ",
    " @****************************@ ",
    " @****************************@ ",
    " @****************************@ ",
    "@******************************@",
    "@******************************@",
    "@******************************@",
    "@******************************@",
    "@******************************@",
    "@******************************@",
    "@******************************@",
    "@******************************@",
    " @****************************@ ",
    " @****************************@ ",
    " @****************************@ ",
    "  @**************************@  ",
    "   @************************@   ",
    "   @************************@   ",
    "    @**********************@    ",
    "     @********************@     ",
    "      @@****************@@      ",
    "        @**************@        ",
    "         @@@********@@@         ",
    "            @@@@@@@@            "};

static const char * const qt_cleanlooks_radiobutton_checked[] = {
    "32 32 20 1",
    " 	c None",
    ".	c #272D33", // #A8ABAE
    "+	c #596066",
    "@	c #283138",
    "#	c #A9ACAF",
    "$	c #A6A9AB",
    "%	c #6B7378",
    "&	c #8C9296",
    "*	c #A2A6AA",
    "=	c #61696F",
    "-	c #596065",
    ";	c #93989C",
    ">	c #777E83",
    ",	c #60686E",
    "'	c #252D33",
    ")	c #535B62",
    "!	c #21292E",
    "~	c #242B31",
    "{	c #1F262B",
    "]	c #41484E",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "             ......             ",
    "           ..........           ",
    "         ..............         ",
    "        ................        ",
    "       ..................       ",
    "       ..................       ",
    "      ....................      ",
    "      ....................      ",
    "     ......................     ",
    "     ......................     ",
    "     ......................     ",
    "     ......................     ",
    "     ......................     ",
    "     ......................     ",
    "      ....................      ",
    "      ....................      ",
    "       ..................       ",
    "       ..................       ",
    "        ................        ",
    "          ............          ",
    "           ..........           ",
    "             ......             ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                "};

static const char * const qt_scrollbar_button_left[] = {
    "32 32 6 1",
    "   c None",
    ".  c #BFBFBF",
    "+  c #979797",
    "#  c #FAFAFA",
    "<  c #FAFAFA",
    "*  c #FAFAFA",
    " .++++++++++++++++++++++++++++++",
    ".+#############################+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    ".+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<+",
    " .++++++++++++++++++++++++++++++"};

static const char * const qt_scrollbar_button_right[] = {
    "32 32 6 1",
    "   c None",
    ".  c #BFBFBF",
    "+  c #979797",
    "#  c #FAFAFA",
    "<  c #FAFAFA",
    "*  c #FAFAFA",
    "++++++++++++++++++++++++++++++. ",
    "+#############################+.",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<+.",
    "++++++++++++++++++++++++++++++. "};

static const char * const qt_scrollbar_button_up[] = {
    "32 32 6 1",
    "   c None",
    ".  c #BFBFBF",
    "+  c #979797",
    "#  c #FAFAFA",
    "<  c #FAFAFA",
    "*  c #FAFAFA",
    " .++++++++++++++++++++++++++++. ",
    ".+############################+.",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<+",
    "++++++++++++++++++++++++++++++++"};

static const char * const qt_scrollbar_button_down[] = {
    "32 32 6 1",
    "   c None",
    ".  c #BFBFBF",
    "+  c #979797",
    "#  c #FAFAFA",
    "<  c #FAFAFA",
    "*  c #FAFAFA",
    "++++++++++++++++++++++++++++++++",
    "+##############################+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    "+#                            <+",
    ".+<<<<<<<<<<<<<<<<<<<<<<<<<<<<+.",
    " .++++++++++++++++++++++++++++. "};

static const char * const qt_cleanlooks_checkbox_checked[] = {
    "32 32 3 1",
    " 	c None",
    ".	c #272D33",
    "%	c #666666",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                         .      ",
    "                        ...     ",
    "                       .....    ",
    "                      .....     ",
    "                     .....      ",
    "                    .....       ",
    "                   .....        ",
    "                  .....         ",
    "                 .....          ",
    "                .....           ",
    "               .....            ",
    "      .       .....             ",
    "     ...     .....              ",
    "    .....   .....               ",
    "     ..... .....                ",
    "      .........                 ",
    "       .......                  ",
    "        .....                   ",
    "         ...                    ",
    "          .                     ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                "};

///////////////////////////////////////////////////////////////////////////////////////////////////
TouchScreenStyle::TouchScreenStyle() :
    _Mybase( QStyleFactory::create( baseStyle_ ) )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
TouchScreenStyle::~TouchScreenStyle()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int TouchScreenStyle::pixelMetric( PixelMetric metric, const QStyleOption *option, const QWidget *widget ) const
{
    // check box size
    if (( PM_IndicatorWidth == metric ) || ( PM_IndicatorHeight == metric ))
        return minimumThickness_;

    // radio button size
    if (( PM_ExclusiveIndicatorWidth == metric ) || ( PM_ExclusiveIndicatorHeight == metric ))
        return minimumThickness_;

    // scroll bar thickness and min length
    if (( PM_ScrollBarExtent == metric ) || ( PM_ScrollBarSliderMin == metric ))
        return minimumThickness_;

    // slider thickness
    if (( PM_SliderControlThickness == metric ) || ( PM_SliderThickness == metric ))
        return minimumThickness_;
    // slider length
    else if ( PM_SliderLength == metric )
        return qMax( minimumThickness_, _Mybase::pixelMetric( PM_SliderLength, option, widget ) );

    return _Mybase::pixelMetric( metric, option, widget );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QRect TouchScreenStyle::subControlRect( ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget ) const
{
    QRect rect( _Mybase::subControlRect( control, option, subControl, widget ) );

    // adjust combo box arrow button width
    if (( CC_ComboBox == control ) && ( rect.isValid() ))
    {
        // adjust button size
        if ( SC_ComboBoxArrow == subControl )
            rect.adjust( rect.width() - qMax( minimumThickness_, rect.width() ), 0, 0, 0 );

        // adjust edit field size
        else if ( SC_ComboBoxEditField == subControl )
        {
            QRect comboBoxArrowButtonRect( _Mybase::subControlRect( CC_ComboBox, option, SC_ComboBoxArrow, widget ) );

            if ( comboBoxArrowButtonRect.isValid() )
                rect.adjust( 0, 0, comboBoxArrowButtonRect.width() - qMax( minimumThickness_, comboBoxArrowButtonRect.width() ), 0 );
        }
    }

    // adjust spin box button width
    else if (( CC_SpinBox == control ) && ( rect.isValid() ))
    {
        // adjust button size
        if (( SC_SpinBoxUp == subControl ) || ( SC_SpinBoxDown == subControl ))
            rect.adjust( rect.width() - qMax( minimumThickness_, rect.width() ), 0, 0, 0 );

        // adjust edit field size
        else if ( SC_SpinBoxEditField == subControl )
        {
            QRect spinBoxUpButtonRect( _Mybase::subControlRect( CC_SpinBox, option, SC_SpinBoxUp, widget ) );

            if ( spinBoxUpButtonRect.isValid() )
                rect.adjust( 0, 0, spinBoxUpButtonRect.width() - qMax( minimumThickness_, spinBoxUpButtonRect.width() ), 0 );
        }
    }

    return rect;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int	TouchScreenStyle::styleHint( StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData ) const
{
    if ( SH_ComboBox_Popup == hint )
        return 0;

    return _Mybase::styleHint( hint, option, widget, returnData );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TouchScreenStyle::drawComplexControl( ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget ) const
{
    // Note:
    // The code below was taken straight from the qt source code and trimmed down to include only
    // the control(s) we are interested in.

    QColor button = option->palette.button().color();
    QColor dark;
    QColor grooveColor;
    QColor darkOutline;
    dark.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*1.9)),
                qMin(255, (int)(button.value()*0.7)));
    grooveColor.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*2.6)),
                qMin(255, (int)(button.value()*0.9)));
    darkOutline.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*3.0)),
                qMin(255, (int)(button.value()*0.6)));

    QColor alphaCornerColor;
    if (widget) {
        // ### backgroundrole/foregroundrole should be part of the style option
        alphaCornerColor = mergedColors(option->palette.color(widget->backgroundRole()), darkOutline);
    } else {
        alphaCornerColor = mergedColors(option->palette.background().color(), darkOutline);
    }
    QColor gripShadow = grooveColor.darker(110);
    //QColor buttonShadow = option->palette.button().color().darker(110);

    QColor gradientStartColor = option->palette.button().color().lighter(108);
    QColor gradientStopColor = mergedColors(option->palette.button().color().darker(108), dark.lighter(150), 70);

    //QColor highlightedGradientStartColor = option->palette.button().color();
    //QColor highlightedGradientStopColor = mergedColors(option->palette.button().color(), option->palette.highlight().color(), 85);

    //QColor highlightedDarkInnerBorderColor = mergedColors(option->palette.button().color(), option->palette.highlight().color(), 35);
    //QColor highlightedLightInnerBorderColor = mergedColors(option->palette.button().color(), option->palette.highlight().color(), 58);

    //QColor buttonShadowAlpha = option->palette.background().color().darker(105);

    QPalette palette = option->palette;

    switch ( control )
    {
    case CC_ScrollBar:
        painter->save();
        if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            bool isEnabled = scrollBar->state & State_Enabled;
            bool reverse = scrollBar->direction == Qt::RightToLeft;
            bool horizontal = scrollBar->orientation == Qt::Horizontal;
            bool sunken = scrollBar->state & State_Sunken;

            painter->fillRect(option->rect, option->palette.background());

            QRect scrollBarSubLine = proxy()->subControlRect(control, scrollBar, SC_ScrollBarSubLine, widget);
            QRect scrollBarAddLine = proxy()->subControlRect(control, scrollBar, SC_ScrollBarAddLine, widget);
            QRect scrollBarSlider = proxy()->subControlRect(control, scrollBar, SC_ScrollBarSlider, widget);
            QRect grooveRect = proxy()->subControlRect(control, scrollBar, SC_ScrollBarGroove, widget);

            // paint groove
            if (scrollBar->subControls & SC_ScrollBarGroove) {
                painter->setBrush(grooveColor);
                painter->setPen(Qt::NoPen);
                if (horizontal) {
                    painter->drawRect(grooveRect);
                    painter->setPen(darkOutline);
                    painter->drawLine(grooveRect.topLeft(), grooveRect.topRight());
                    painter->drawLine(grooveRect.bottomLeft(), grooveRect.bottomRight());
                } else {
                    painter->drawRect(grooveRect);
                    painter->setPen(darkOutline);
                    painter->drawLine(grooveRect.topLeft(), grooveRect.bottomLeft());
                    painter->drawLine(grooveRect.topRight(), grooveRect.bottomRight());
                }
            }
            //paint slider
            if (scrollBar->subControls & SC_ScrollBarSlider) {
                QRect pixmapRect = scrollBarSlider;
                if (horizontal)
                    pixmapRect.adjust(-1, 0, 0, -1);
                else
                    pixmapRect.adjust(0, -1, -1, 0);

                if (isEnabled) {
                    QLinearGradient gradient(pixmapRect.center().x(), pixmapRect.top(),
                                             pixmapRect.center().x(), pixmapRect.bottom());
                    if (!horizontal)
                        gradient = QLinearGradient(pixmapRect.left(), pixmapRect.center().y(),
                                                   pixmapRect.right(), pixmapRect.center().y());

                    if (option->palette.button().gradient()) {
                        gradient.setStops(option->palette.button().gradient()->stops());
                    } else {
                        if (sunken || (option->state & State_MouseOver &&
                            (scrollBar->activeSubControls & SC_ScrollBarSlider))) {
                            gradient.setColorAt(0, gradientStartColor.lighter(110));
                            gradient.setColorAt(1, gradientStopColor.lighter(110));
                        } else {
                            gradient.setColorAt(0, gradientStartColor);
                            gradient.setColorAt(1, gradientStopColor);
                        }
                    }
                    painter->setPen(QPen(darkOutline, 0));
                    painter->setBrush(gradient);
                    painter->drawRect(pixmapRect);


                    //calculate offsets used by highlight and shadow
                    int yoffset, xoffset;
                    if (option->state & State_Horizontal) {
                        xoffset = 0;
                        yoffset = 1;
                    } else {
                        xoffset = 1;
                        yoffset = 0;
                    }
                    //draw slider highlights
                    painter->setPen(QPen(gradientStopColor, 0));
                    painter->drawLine(scrollBarSlider.left() + xoffset,
                                      scrollBarSlider.bottom() - yoffset,
                                      scrollBarSlider.right() - xoffset,
                                      scrollBarSlider.bottom() - yoffset);
                    painter->drawLine(scrollBarSlider.right() - xoffset,
                                      scrollBarSlider.top() + yoffset,
                                      scrollBarSlider.right() - xoffset,
                                      scrollBarSlider.bottom() - yoffset);

                    //draw slider shadow
                    painter->setPen(QPen(gradientStartColor, 0));
                    painter->drawLine(scrollBarSlider.left() + xoffset,
                                      scrollBarSlider.top() + yoffset,
                                      scrollBarSlider.right() - xoffset,
                                      scrollBarSlider.top() + yoffset);
                    painter->drawLine(scrollBarSlider.left() + xoffset,
                                      scrollBarSlider.top() + yoffset,
                                      scrollBarSlider.left() + xoffset,
                                      scrollBarSlider.bottom() - yoffset);
                } else {
                    QLinearGradient gradient(pixmapRect.center().x(), pixmapRect.top(),
                                             pixmapRect.center().x(), pixmapRect.bottom());
                    if (!horizontal) {
                        gradient = QLinearGradient(pixmapRect.left(), pixmapRect.center().y(),
                                                   pixmapRect.right(), pixmapRect.center().y());
                    }
                    if (sunken) {
                        gradient.setColorAt(0, gradientStartColor.lighter(110));
                        gradient.setColorAt(1, gradientStopColor.lighter(110));
                    } else {
                        gradient.setColorAt(0, gradientStartColor);
                        gradient.setColorAt(1, gradientStopColor);
                    }
                    painter->setPen(darkOutline);
                    painter->setBrush(gradient);
                    painter->drawRect(pixmapRect);
                }
                int gripMargin = 4;
                //draw grips
                if (horizontal) {
                    for (int i = -3; i< 6 ; i += 3) {
                        painter->setPen(QPen(gripShadow, 1));
                        painter->drawLine(
                            QPoint(scrollBarSlider.center().x() + i ,
                                   scrollBarSlider.top() + gripMargin),
                            QPoint(scrollBarSlider.center().x() + i,
                                   scrollBarSlider.bottom() - gripMargin));
                        painter->setPen(QPen(palette.light(), 1));
                        painter->drawLine(
                            QPoint(scrollBarSlider.center().x() + i + 1,
                                   scrollBarSlider.top() + gripMargin  ),
                            QPoint(scrollBarSlider.center().x() + i + 1,
                                   scrollBarSlider.bottom() - gripMargin));
                    }
                } else {
                    for (int i = -3; i < 6 ; i += 3) {
                        painter->setPen(QPen(gripShadow, 1));
                        painter->drawLine(
                            QPoint(scrollBarSlider.left() + gripMargin ,
                                   scrollBarSlider.center().y()+ i),
                            QPoint(scrollBarSlider.right() - gripMargin,
                                   scrollBarSlider.center().y()+ i));
                        painter->setPen(QPen(palette.light(), 1));
                        painter->drawLine(
                            QPoint(scrollBarSlider.left() + gripMargin,
                                   scrollBarSlider.center().y() + 1 + i),
                            QPoint(scrollBarSlider.right() - gripMargin,
                                   scrollBarSlider.center().y() + 1 + i));
                    }
                }
            }

            // The SubLine (up/left) buttons
            if (scrollBar->subControls & SC_ScrollBarSubLine) {
                //int scrollBarExtent = proxy()->pixelMetric(PM_ScrollBarExtent, option, widget);
                QRect pixmapRect = scrollBarSubLine;
                if (isEnabled ) {
                    QRect fillRect = pixmapRect.adjusted(1, 1, -1, -1);
                    // Gradients
                    if ((scrollBar->activeSubControls & SC_ScrollBarSubLine) && sunken) {
                        drawGradient(painter,
                                                    QRect(fillRect),
                                                    gradientStopColor.darker(120),
                                                    gradientStopColor.darker(120),
                                                    horizontal ? TopDown : FromLeft, option->palette.button());
                    } else {
                        drawGradient(painter,
                                                    QRect(fillRect),
                                                    gradientStartColor.lighter(105),
                                                    gradientStopColor,
                                                    horizontal ? TopDown : FromLeft, option->palette.button());
                    }
                }
                // Details
                QImage subButton;
                if (horizontal) {
                    subButton = QImage(reverse ? qt_scrollbar_button_right : qt_scrollbar_button_left);
                } else {
                    subButton = QImage(qt_scrollbar_button_up);
                }
                subButton.setColor(1, alphaCornerColor.rgba());
                subButton.setColor(2, darkOutline.rgba());
                if ((scrollBar->activeSubControls & SC_ScrollBarSubLine) && sunken) {
                    subButton.setColor(3, gradientStopColor.darker(140).rgba());
                    subButton.setColor(4, gradientStopColor.darker(120).rgba());
                } else {
                    subButton.setColor(3, gradientStartColor.lighter(105).rgba());
                    subButton.setColor(4, gradientStopColor.rgba());
                }
                subButton.setColor(5, scrollBar->palette.text().color().rgba());
                painter->drawImage(pixmapRect, subButton);

                // Arrows
                PrimitiveElement arrow;
                if (option->state & State_Horizontal)
                    arrow = option->direction == Qt::LeftToRight ? PE_IndicatorArrowLeft: PE_IndicatorArrowRight;
                else
                    arrow = PE_IndicatorArrowUp;
                QStyleOption arrowOpt = *option;
                arrowOpt.rect = scrollBarSubLine.adjusted(3, 3, -2, -2);
                proxy()->drawPrimitive(arrow, &arrowOpt, painter, widget);


                // The AddLine (down/right) button
                if (scrollBar->subControls & SC_ScrollBarAddLine) {
                    //QString addLinePixmapName = QStyleHelper::uniqueName(QLatin1String("scrollbar_addline"), option, QSize(16, 16));
                    QRect pixmapRect = scrollBarAddLine;
                    if (isEnabled) {
                        QRect fillRect = pixmapRect.adjusted(1, 1, -1, -1);
                        // Gradients
                        if ((scrollBar->activeSubControls & SC_ScrollBarAddLine) && sunken) {
                            drawGradient(painter,
                                                        fillRect,
                                                        gradientStopColor.darker(120),
                                                        gradientStopColor.darker(120),
                                                        horizontal ? TopDown: FromLeft, option->palette.button());
                        } else {
                            drawGradient(painter,
                                                        fillRect,
                                                        gradientStartColor.lighter(105),
                                                        gradientStopColor,
                                                        horizontal ? TopDown : FromLeft, option->palette.button());
                        }
                    }
                    // Details
                    QImage addButton;
                    if (horizontal) {
                        addButton = QImage(reverse ? qt_scrollbar_button_left : qt_scrollbar_button_right);
                    } else {
                        addButton = QImage(qt_scrollbar_button_down);
                    }
                    addButton.setColor(1, alphaCornerColor.rgba());
                    addButton.setColor(2, darkOutline.rgba());
                    if ((scrollBar->activeSubControls & SC_ScrollBarAddLine) && sunken) {
                        addButton.setColor(3, gradientStopColor.darker(140).rgba());
                        addButton.setColor(4, gradientStopColor.darker(120).rgba());
                    } else {
                        addButton.setColor(3, gradientStartColor.lighter(105).rgba());
                        addButton.setColor(4, gradientStopColor.rgba());
                    }
                    addButton.setColor(5, scrollBar->palette.text().color().rgba());
                    painter->drawImage(pixmapRect, addButton);

                    PrimitiveElement arrow;
                    if (option->state & State_Horizontal)
                        arrow = option->direction == Qt::LeftToRight ? PE_IndicatorArrowRight : PE_IndicatorArrowLeft;
                    else
                        arrow = PE_IndicatorArrowDown;

                    QStyleOption arrowOpt = *option;
                    arrowOpt.rect = scrollBarAddLine.adjusted(3, 3, -2, -2);
                    proxy()->drawPrimitive(arrow, &arrowOpt, painter, widget);
                }
            }
        }
        painter->restore();
        break;

    default:
        _Mybase::drawComplexControl( control, option, painter, widget );
        break;

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TouchScreenStyle::drawPrimitive( PrimitiveElement elem, const QStyleOption *option, QPainter *painter, const QWidget *widget ) const
{
    // Note:
    // The code below was taken straight from the qt source code and trimmed down to include only
    // the control(s) we are interested in.

    QRect rect = option->rect;
    int state = option->state;

    QColor button = option->palette.button().color();
    QColor buttonShadow = option->palette.button().color().darker(110);
    QColor buttonShadowAlpha = buttonShadow;

    buttonShadowAlpha.setAlpha(128);

    QColor darkOutline;
    QColor dark;

    darkOutline.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*3.0)),
                qMin(255, (int)(button.value()*0.6)));

    dark.setHsv(button.hue(),
                qMin(255, (int)(button.saturation()*1.9)),
                qMin(255, (int)(button.value()*0.7)));

    //QColor tabFrameColor = mergedColors(option->palette.background().color(),
    //                                            dark.lighter(135), 60);

    switch(elem)
    {
    case PE_IndicatorCheckBox:
        painter->save();
        if (const QStyleOptionButton *checkbox = qstyleoption_cast<const QStyleOptionButton*>(option)) {
            QRect checkRect;
            checkRect.setX(rect.left() );
            checkRect.setY(rect.top() );
            checkRect.setWidth(rect.width() - 1);
            checkRect.setHeight(rect.height() - 1);
            if (state & State_Sunken)
                painter->setBrush(dark.lighter(130));
            else
                painter->setBrush(option->palette.base());
            painter->setPen(QPen(dark.lighter(110), 0));
            painter->drawRect(checkRect);
            if (checkbox->state & (State_On | State_Sunken  | State_NoChange)) {
                QImage image(qt_cleanlooks_checkbox_checked);
                QColor fillColor = option->palette.text().color();
                image.setColor(1, fillColor.rgba());
                fillColor.setAlpha(100);
                image.setColor(2, fillColor.rgba());
                painter->drawImage(rect, image);
                if (checkbox->state & State_NoChange) {
                    QColor bgc = option->palette.background().color();
                    bgc.setAlpha(127);
                    painter->fillRect(checkRect.adjusted(1, 1, -1, -1), bgc);
                }
            }
        }
        painter->restore();
        break;

    case PE_IndicatorRadioButton:
        painter->save();
        {
            painter->setRenderHint(QPainter::SmoothPixmapTransform);
            QRect checkRect = rect.adjusted(0, 0, 0, 0);
            if (state & (State_On )) {
                painter->drawImage(rect, QImage(qt_cleanlooks_radiobutton));
                painter->drawImage(checkRect, QImage(qt_cleanlooks_radiobutton_checked));
            }
            else if (state & State_Sunken) {
                painter->drawImage(rect, QImage(qt_cleanlooks_radiobutton));
                QColor bgc = buttonShadow;
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setBrush(bgc);
                painter->setPen(Qt::NoPen);
                painter->drawEllipse(rect.adjusted(1, 1, -1, -1));                }
            else {
                painter->drawImage(rect, QImage(qt_cleanlooks_radiobutton));
            }
        }
        painter->restore();
        break;

    default:
        _Mybase::drawPrimitive(elem, option, painter, widget);
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TouchScreenStyle::polish( QWidget *widget )
{
    static const QSize zeroMinimumSize( 0, 0 );

    // base class polish!
    _Mybase::polish( widget );

    // widgets should have no context menu
    widget->setContextMenuPolicy( Qt::NoContextMenu );

    // widget has minimum size set, no need to polish
    if ( zeroMinimumSize != widget->minimumSize() )
        return;

    QPushButton *pushButton( NULL );
    QToolButton *toolButton( NULL );

    // set minimum width and height
    if (( pushButton = dynamic_cast<QPushButton*>( widget ) ) ||
        ( toolButton = dynamic_cast<QToolButton*>( widget ) ))
    {
        widget->setMinimumWidth( 2 * minimumThickness_ );
        widget->setMinimumHeight( 2 * minimumThickness_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TouchScreenStyle::drawGradient(
    QPainter *painter,
    const QRect& rect,
    const QColor& gradientStart,
    const QColor& gradientStop,
    Direction direction,
    QBrush bgBrush )
{
    int x = rect.center().x();
    int y = rect.center().y();

    QLinearGradient *gradient;

    switch( direction )
    {
    case FromLeft:
        gradient = new QLinearGradient( rect.left(), y, rect.right(), y );
        break;
    case FromRight:
        gradient = new QLinearGradient( rect.right(), y, rect.left(), y );
        break;
    case BottomUp:
        gradient = new QLinearGradient( x, rect.bottom(), x, rect.top() );
        break;
    case TopDown:
    default:
        gradient = new QLinearGradient( x, rect.top(), x, rect.bottom() );
        break;
    }

    if ( bgBrush.gradient() )
        gradient->setStops(bgBrush.gradient()->stops());
    else
    {
        gradient->setColorAt( 0, gradientStart );
        gradient->setColorAt( 1, gradientStop );
    }

    painter->fillRect( rect, *gradient );

    delete gradient;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QColor TouchScreenStyle::mergedColors( const QColor& colorA, const QColor& colorB, int factor )
{
    const int maxFactor = 100;

    QColor tmp = colorA;

    tmp.setRed( (tmp.red() * factor) / maxFactor + (colorB.red() * (maxFactor - factor)) / maxFactor );
    tmp.setGreen( (tmp.green() * factor) / maxFactor + (colorB.green() * (maxFactor - factor)) / maxFactor );
    tmp.setBlue( (tmp.blue() * factor) / maxFactor + (colorB.blue() * (maxFactor - factor)) / maxFactor );

    return tmp;
}
