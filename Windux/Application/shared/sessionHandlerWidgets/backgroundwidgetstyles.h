/**
 * @file backgroundwidgetstyles.h
 * Background widget rendering styles.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef BACKGROUNDWIDGETSTYLES_H
#define BACKGROUNDWIDGETSTYLES_H

#include "backgroundwidgetrenderer.h"

#include <QBrush>
#include <QColor>
#include <QPen>
#include <QSize>

#include <qmath.h>

/// Hobart namespace.
namespace hobart {

// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// ================================================================================================
//
// Color Palette
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

/// Hobart color palette.
namespace colorPalette {

static const QColor black( 0, 0, 0 );                               /// Black text.
static const QColor blue( 0, 176, 240 );                            /// Blue (selected) border.
static const QColor lightBlue( 100, 246, 255 );                     /// Department Blue
static const QColor darkGray( 32, 32, 32 );                         /// Dark gray.
static const QColor gray( 64, 64, 64 );                             /// Gray (panel button) border.
static const QColor green( 144, 208, 64 );                          /// Green (selectable) border.
static const QColor lightGray( 128, 128, 128 );                     /// Light gray.
static const QColor red( 232, 32, 32 );                             /// Red (void) border.
static const QColor white( 255, 255, 255 );                         /// White (read only) border.
static const QColor yellow( 255, 247, 79 );                         /// Yellow.
static const QColor pluNumGrey( 121, 124, 134 );                    /// Plu Number on Flashkeys for Gen2 UI
static const QColor pluDescDark( 25, 34, 46 );                      /// Plu Description on Flashkeys for Gen2 UI
static const QColor backgroundGrey( 244, 247, 250 );
}

/// Gradient colors
namespace gradientColors {

static const QColor lightOperatorField_L0( 255, 255, 255 );         /// Light operator field.
static const QColor lightOperatorField_L1( 208, 208, 208 );         /// Light operator field.

static const QColor darkOperatorField_L0( 32, 32, 32 );             /// Dark operator field.
static const QColor darkOperatorField_L1( 64, 64, 64 );             /// Dark operator field.

static const QColor forcedOperatorField_L0( 232, 32, 32 );          /// Forced operator field.
static const QColor forcedOperatorField_L1( 200, 0, 0 );            /// Forced operator field.

static const QColor greenOperatorField_L0( 80, 144, 0 );            /// Green operator field.
static const QColor greenOperatorField_L1( 144, 208, 64 );          /// Green operator field.

static const QColor yellowOperatorField_L0( 191, 183, 15 );         /// Yellow operator field.
static const QColor yellowOperatorField_L1( 255, 247, 79 );         /// Yellow operator field.

// was 32,178,170
static const QColor greenP5OperatorField_L0( 0, 255, 127 );        /// Pick 5 Green operator field (dark).
static const QColor greenP5OperatorField_L1( 0, 255, 127 );        /// Pick 5 Green operator field (light).

static const QColor lightButton_L0( 255, 255, 255 );                /// Light button.
static const QColor lightButton_L1( 208, 208, 208 );                /// Light button.

static const QColor darkButton_L0( 192, 192, 192 );                 /// Dark button.
static const QColor darkButton_L1( 96, 96, 96 );                    /// Dark button.
static const QColor darkButton_L2( 64, 64, 64 );                    /// Dark button.
static const QColor darkButton_L3( 32, 32, 32 );                    /// Dark button.
static const QColor darkButton_L4( 64, 64, 64 );                    /// Dark button.

// Note:
// I tried using the green from above but button looks flat... added more green...
static const QColor greenButton_L0( 144, 232, 64 );                 /// Green button.
static const QColor greenButton_L1( 0, 0, 0 );                      /// Green button.

static const QColor redButton_L0( 232, 32, 32 );                    /// Red button.
static const QColor redButton_L1( 0, 0, 0 );                        /// Red button.
}

// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// ================================================================================================
//
// Base classes for Widget Appearance
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

/// Widget renderer base class.
class widgetRendererAppearanceBase
{
    typedef widgetRendererAppearanceBase _Myt;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~widgetRendererAppearanceBase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& /*size*/ ) const {return QBrush( Qt::transparent );}

    /// Retrieve pen.
    /**
     * @param[in] type  widget state
     * @return  pen
     */
    virtual QPen pen( rendererState /*type*/ ) const {return QPen( Qt::transparent );}

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    widgetRendererAppearanceBase() {}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create brush from linear gradient.
    /**
     * @param[in] size  field size
     * @param[in] L0  color
     * @param[in] L1  color
     * @return  brush
     */
    virtual QBrush makeLinearGradient( const QSize& size, const QColor& L0, const QColor& L1 ) const
    {
        QLinearGradient g( 0, 0, 0, size.height() );

        g.setColorAt( 0.0, L0 );
        g.setColorAt( 1.0, L1 );

        return QBrush( g );
    }

    /// Create brush from radial gradient.
    /**
     * @param[in] size  field size
     * @param[in] L0  color
     * @param[in] L1  color
     * @return  brush
     */
    virtual QBrush makeRadialGradient( const QSize& size, const QColor& L0, const QColor& L1 ) const
    {
        qreal w( size.width() / 2.0 );
        qreal h( size.height() / 2.0 );
        qreal r( qSqrt( (w * w) + (h * h) ) );

        QRadialGradient g( w, h, (1.5 * r) );

        g.setColorAt( 0.0, L0 );
        g.setColorAt( 1.0, L1 );

        return QBrush( g );
    }

};

/// Appearance for input field style widgets.
class inputFieldWidgetRendererAppearanceBase : public widgetRendererAppearanceBase
{
    typedef inputFieldWidgetRendererAppearanceBase _Myt;
    typedef widgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve pen.
    /**
     * @param[in] type  widget state
     * @return  pen
     */
    virtual QPen pen( rendererState type ) const
    {
        QPen pen;

        if ( Normal == type )
        {
            pen.setColor( colorPalette::green );
            pen.setWidth( 4 );
        }
        else if ( ReadOnly == type )
        {
            pen.setColor( colorPalette::white );
            pen.setWidth( 4 );
        }
        else if ( Focused == type )
        {
            pen.setColor( colorPalette::blue );
            pen.setWidth( 6 );
        }

        return pen;
    }

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    inputFieldWidgetRendererAppearanceBase() : _Mybase() {}

};

/// Appearance for input field style widgets.
class neverInputFieldWidgetRendererAppearanceBase : public widgetRendererAppearanceBase
{
    typedef neverInputFieldWidgetRendererAppearanceBase _Myt;
    typedef widgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve pen.
    /**
     * @param[in] type  widget state
     * @return  pen
     */
    virtual QPen pen( rendererState type ) const
    {
        QPen pen;

        if ( Normal == type )
        {
            pen.setColor( colorPalette::white );
            pen.setWidth( 4 );      // 4
        }
        else if ( ReadOnly == type )
        {
            pen.setColor( colorPalette::white );
            pen.setWidth( 4 );      // 4
        }
        else if ( Focused == type )
        {
            pen.setColor( colorPalette::white );
            pen.setWidth( 4 );
        }

        return pen;
    }

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    neverInputFieldWidgetRendererAppearanceBase() : _Mybase() {}

};

/// Appearance for button style widgets.
class buttonWidgetRendererAppearanceBase : public widgetRendererAppearanceBase
{
    typedef buttonWidgetRendererAppearanceBase _Myt;
    typedef widgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve pen.
    /**
     * @param[in] type  widget state
     * @return  pen
     */
    virtual QPen pen( rendererState type ) const
    {
        QPen pen;

        if ( Focused == type )
        {
            pen.setColor( colorPalette::blue );
            pen.setWidth( 4 );
        }
        else if ( Normal == type )
        {
            pen.setColor( colorPalette::green );
            pen.setWidth( 4 );
        }
        else
        {
            pen.setColor( colorPalette::white );
            pen.setWidth( 2 );
        }

        return pen;
    }

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    buttonWidgetRendererAppearanceBase() : _Mybase() {}

};

// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// ================================================================================================
//
// Widget Appearances
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

/// Appearance for a capacity bar background widget.
class capacityBarBackgroundWidgetRendererAppearance : public widgetRendererAppearanceBase
{
    typedef capacityBarBackgroundWidgetRendererAppearance _Myt;
    typedef widgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    capacityBarBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& /*size*/ ) const {return QBrush( colorPalette::gray );}

    /// Retrieve pen.
    /**
     * @param[in] type  widget state
     * @return  pen
     */
    virtual QPen pen( rendererState /*type*/ ) const {return QPen( colorPalette::gray );}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a light operator widget.
class lightOperatorBackgroundWidgetRendererAppearance : public inputFieldWidgetRendererAppearanceBase
{
    typedef lightOperatorBackgroundWidgetRendererAppearance _Myt;
    typedef inputFieldWidgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    lightOperatorBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeLinearGradient(
            size,
            gradientColors::lightOperatorField_L0,
            gradientColors::lightOperatorField_L1 );
    }

};

/// Appearance for an uneditable light operator widget.
class lightNeverOperatorBackgroundWidgetRendererAppearance : public neverInputFieldWidgetRendererAppearanceBase
{
    typedef lightNeverOperatorBackgroundWidgetRendererAppearance _Myt;
    typedef neverInputFieldWidgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    lightNeverOperatorBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeLinearGradient(
            size,
            gradientColors::lightOperatorField_L0,
            gradientColors::lightOperatorField_L1 );
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a dark operator widget.
class darkOperatorBackgroundWidgetRendererAppearance : public inputFieldWidgetRendererAppearanceBase
{
    typedef darkOperatorBackgroundWidgetRendererAppearance _Myt;
    typedef inputFieldWidgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    darkOperatorBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeLinearGradient(
            size,
            gradientColors::darkOperatorField_L0,
            gradientColors::darkOperatorField_L1 );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a forced operator widget.
/**
 * tparam Type  0 for darker top, 1 for lighter top
 */
template <int Type>
class forcedOperatorBackgroundWidgetRendererAppearance : public inputFieldWidgetRendererAppearanceBase
{
    typedef forcedOperatorBackgroundWidgetRendererAppearance<Type> _Myt;
    typedef inputFieldWidgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    forcedOperatorBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeLinearGradient(
            size,
            (1 == Type) ? gradientColors::forcedOperatorField_L0 : gradientColors::forcedOperatorField_L1,
            (1 == Type) ? gradientColors::forcedOperatorField_L1 : gradientColors::forcedOperatorField_L0 );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a green operator widget.
class greenOperatorBackgroundWidgetRendererAppearance : public inputFieldWidgetRendererAppearanceBase
{
    typedef greenOperatorBackgroundWidgetRendererAppearance _Myt;
    typedef inputFieldWidgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    greenOperatorBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeLinearGradient(
            size,
            gradientColors::greenOperatorField_L0,
            gradientColors::greenOperatorField_L1 );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a yellow operator widget.
class yellowOperatorBackgroundWidgetRendererAppearance : public inputFieldWidgetRendererAppearanceBase
{
    typedef yellowOperatorBackgroundWidgetRendererAppearance _Myt;
    typedef inputFieldWidgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    yellowOperatorBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeLinearGradient(
            size,
            gradientColors::yellowOperatorField_L0,
            gradientColors::yellowOperatorField_L1 );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a yellow operator widget.
class greenP5OperatorBackgroundWidgetRendererAppearance : public inputFieldWidgetRendererAppearanceBase
{
    typedef greenP5OperatorBackgroundWidgetRendererAppearance _Myt;
    typedef inputFieldWidgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    greenP5OperatorBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeLinearGradient(
            size,
            gradientColors::greenP5OperatorField_L0,
            gradientColors::greenP5OperatorField_L1 );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a panel widget.
class panelBackgroundWidgetRendererAppearance : public widgetRendererAppearanceBase
{
    typedef panelBackgroundWidgetRendererAppearance _Myt;
    typedef widgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    panelBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeLinearGradient(
            size,
            gradientColors::darkOperatorField_L0,
            gradientColors::darkOperatorField_L1 );
    }

    /// Retrieve pen.
    /**
     * @param[in] type  widget state
     * @return  pen
     */
    virtual QPen pen( rendererState /*type*/ ) const
    {
        return QPen( colorPalette::white, 4 );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a panel button widget.
class panelButtonBackgroundWidgetRendererAppearance : public widgetRendererAppearanceBase
{
    typedef panelButtonBackgroundWidgetRendererAppearance _Myt;
    typedef widgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    panelButtonBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& /*size*/ ) const
    {
        return QBrush( QColor( 96, 96, 96 ) );
    }

    /// Retrieve pen.
    /**
     * @param[in] type  widget state
     * @return  pen
     */
    virtual QPen pen( rendererState type ) const
    {
        QPen pen( QColor( 128, 128, 128 ), 2 );

        if ( Focused == type )
            pen.setColor( colorPalette::blue );

        return pen;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a light button widget.
class lightButtonBackgroundWidgetRendererAppearance : public buttonWidgetRendererAppearanceBase
{
    typedef lightButtonBackgroundWidgetRendererAppearance _Myt;
    typedef buttonWidgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    lightButtonBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeLinearGradient(
            size,
            gradientColors::lightButton_L0,
            gradientColors::lightButton_L1 );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a dark button widget.
class darkButtonBackgroundWidgetRendererAppearance : public buttonWidgetRendererAppearanceBase
{
    typedef darkButtonBackgroundWidgetRendererAppearance _Myt;
    typedef buttonWidgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    darkButtonBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        QLinearGradient g( 0, 0, 0, size.height() );

        g.setColorAt( 0.0, gradientColors::darkButton_L0 );
        g.setColorAt( 0.2121, gradientColors::darkButton_L1 );
        g.setColorAt( 0.5091, gradientColors::darkButton_L2 );
        g.setColorAt( 0.5333, gradientColors::darkButton_L3 );
        g.setColorAt( 1.0, gradientColors::darkButton_L4 );

        return QBrush( g );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a green button widget.
class greenButtonBackgroundWidgetRendererAppearance : public buttonWidgetRendererAppearanceBase
{
    typedef greenButtonBackgroundWidgetRendererAppearance _Myt;
    typedef buttonWidgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    greenButtonBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeRadialGradient(
            size,
            gradientColors::greenButton_L0,
            gradientColors::greenButton_L1 );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a red button widget.
class redButtonBackgroundWidgetRendererAppearance : public buttonWidgetRendererAppearanceBase
{
    typedef redButtonBackgroundWidgetRendererAppearance _Myt;
    typedef buttonWidgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    redButtonBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeRadialGradient(
            size,
            gradientColors::redButton_L0,
            gradientColors::redButton_L1 );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for an input widget (i.e. keyboard, number pad).
class inputBackgroundWidgetRendererAppearance : public widgetRendererAppearanceBase
{
    typedef inputBackgroundWidgetRendererAppearance _Myt;
    typedef widgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    inputBackgroundWidgetRendererAppearance() : _Mybase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& size ) const
    {
        return makeLinearGradient(
            size,
            gradientColors::darkOperatorField_L0,
            gradientColors::darkOperatorField_L1 );
    }

    /// Retrieve pen.
    /**
     * @param[in] type  widget state
     * @return  pen
     */
    virtual QPen pen( rendererState /*type*/ ) const
    {
        return QPen( colorPalette::gray, 2 );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for a transparent background widget.
class transparentBackgroundWidgetRendererAppearance : public widgetRendererAppearanceBase
{
    typedef transparentBackgroundWidgetRendererAppearance _Myt;
    typedef widgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    transparentBackgroundWidgetRendererAppearance() : _Mybase() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Appearance for an outline background widget.
class outlineBackgroundWidgetRendererAppearance : public widgetRendererAppearanceBase
{
    typedef outlineBackgroundWidgetRendererAppearance _Myt;
    typedef widgetRendererAppearanceBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    outlineBackgroundWidgetRendererAppearance( const QColor& c = colorPalette::gray ) : _Mybase(), c_( c ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve brush.
    /**
     * @param[in] type  widget state
     * @param[in] size  widget size
     * @return  brush
     */
    virtual QBrush brush( rendererState /*type*/, const QSize& /*size*/ ) const
    {
        return QBrush( Qt::transparent );
    }

    /// Retrieve pen.
    /**
     * @param[in] type  widget state
     * @return  pen
     */
    virtual QPen pen( rendererState /*type*/ ) const
    {
        return QPen( c_, 3 );
    }

protected:

    QColor c_;                                      ///< Pen color.

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // BACKGROUNDWIDGETSTYLES_H
