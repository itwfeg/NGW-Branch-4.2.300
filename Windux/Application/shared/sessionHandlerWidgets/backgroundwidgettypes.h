/**
 * @file backgroundwidgettypes.h
 * Background rendering for background widgets.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef BACKGROUNDWIDGETTYPES_H
#define BACKGROUNDWIDGETTYPES_H

#include "backgroundwidgetrenderer.h"
#include "backgroundwidgetstyles.h"

#include <QPainterPath>
#include <QSize>

/// Hobart namespace.
namespace hobart {

// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// ================================================================================================
//
// Base classes for Widget Appearance
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

/// Widget drawing base class.
class backgroundWidgetBase
{
    typedef backgroundWidgetBase _Myt;

public:

    /// Renderer types.
   enum rendererType
   {
       PainterPath,                                 ///< Render using painter path.
       Rectangle,                                   ///< Render using rectangle.
       RoundedRect                                  ///< Render using rounded rectangle.
   };

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~backgroundWidgetBase() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Renderer type.
    /**
     * @return  renderer type
     */
    virtual rendererType type() const {return type_;}

   /// Retrieve widget painter path.
   /**
    * @param[in] s  size
    * @return  painter path
    */
   virtual QPainterPath painterPath( const QSize& /*s*/ ) const {return QPainterPath();}

    /// Retrieve x radius.
    /**
     * @return  radius
     */
    virtual qreal xRadius() const {return 0.0;}

    /// Retrieve y radius.
    /**
     * @return  radius
     */
    virtual qreal yRadius() const {return 0.0;}

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
   /**
    * @param[in] t  renderer type
    */
    backgroundWidgetBase( rendererType t ) : type_( t ) {}

private:

    rendererType type_;

};

// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// ================================================================================================
//
// Widget Drawing Traits
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

/// Widget drawing details for a capacity bar.
class capacityBarBackgroundWidget : public backgroundWidgetBase
{
    typedef capacityBarBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 1024;          ///< Default width.
    static const int defaultHeight_ = 44;           ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    capacityBarBackgroundWidget() : _Mybase( PainterPath ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @param[in] s  size
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        qreal x( s.width() );
        x /= defaultWidth_;

        qreal y( s.height() );
        y /= defaultHeight_;

        // draw path counter-clockwise
        QPainterPath path;
        path.moveTo( 0.0, 0.0 );
        path.lineTo( 0.0, y * 27.0 );
        path.lineTo( x * 416.0, y * 27.0 );
        path.lineTo( x * 418.0, y * 29.0 );
        path.lineTo( x * 425.0, y * 41.0 );
        path.lineTo( x * 428.0, y * 43.0 );
        path.lineTo( x * 596.0, y * 43.0 );
        path.lineTo( x * 598.0, y * 41.0 );
        path.lineTo( x * 605.0, y * 29.0 );
        path.lineTo( x * 607.0, y * 27.0 );
        path.lineTo( s.width(), y * 27.0 );
        path.lineTo( s.width(), 0.0 );
        path.closeSubpath();

        return path;
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget drawing details for a left slanted field on the weight bar (aka weight).
class leftSlantedWeightBarBackgroundWidget : public backgroundWidgetBase
{
    typedef leftSlantedWeightBarBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 270;           ///< Default width.
    static const int defaultHeight_ = 90;           ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    leftSlantedWeightBarBackgroundWidget() : _Mybase( PainterPath ) {}

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @param[in] s  size
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        qreal x( s.width() );
        x /= defaultWidth_;

        // draw path counter-clockwise
        QPainterPath path;
        path.moveTo( s.width() - 5.0, s.height() - 5.0 );
        path.lineTo( x * 250.0, 5.0 );
        path.lineTo( 5.0, 5.0 );
        path.lineTo( 5.0, s.height() - 5.0 );
        path.closeSubpath();

        return path;
    }

};

/// Widget drawing details for a left notched field on the weight bar (aka tare).
class leftNotchedWeightBarBackgroundWidget : public backgroundWidgetBase
{
    typedef leftNotchedWeightBarBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 250;           ///< Default width.
    static const int defaultHeight_ = 90;           ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    leftNotchedWeightBarBackgroundWidget() : _Mybase( PainterPath ) {}

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @param[in] s  size
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        qreal x( s.width() );
        x /= defaultWidth_;

        qreal y( s.height() );
        y /= defaultHeight_;

        // draw path counter-clockwise
        QPainterPath path;
        path.moveTo( s.width() - 5.0, s.height() - 5.0 );
        path.lineTo( s.width() - 5.0, y * 20.0 );
        path.lineTo( x * 160.0, y * 20.0 );
        path.lineTo( x * 150.0, 5.0 );
        path.lineTo( 5.0, 5.0 );
        path.lineTo( x * 20.0, s.height() - 5.0 );
        path.closeSubpath();
        
        //path.moveTo( 245.0, 85.0 );
        //path.lineTo( 245.0, 20.0 );
        //path.lineTo( 160.0, 20.0 );
        //path.lineTo( 150.0, 5.0 );
        //path.lineTo( 5.0, 5.0 );
        //path.lineTo( 20.0, 85.0 );

        return path;
    }

};

/// Widget drawing details for a left notched field on the weight bar (aka tare).
class squareWeightBarBackgroundWidget : public backgroundWidgetBase
{
    typedef squareWeightBarBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 104;           ///< Default width.
    static const int defaultHeight_ = 90;           ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    squareWeightBarBackgroundWidget() : _Mybase( PainterPath ) {}

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @param[in] s  size
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        qreal x( s.width() );
        x /= defaultWidth_;

        qreal y( s.height() );
        y /= defaultHeight_;

        // draw path counter-clockwise
        QPainterPath path;
        int sw = s.width();
        int sh = s.height();
        path.moveTo( sw - 5.0, sh - 5.0 );  // 99,85  bottom-right
        path.lineTo( sw - 5.0, y * 5.0 );   // 99,5   top-right
        path.lineTo( 5.0, 5.0 );            // 5,5    top-left
        path.lineTo( x * 20.0, sh - 5.0 );  // 20,85  bottom-left
        path.closeSubpath();

        return path;
    }
};

/// Widget drawing details for a left notched field on the weight bar (aka tare).
class halfLeftNotchedWeightBarBackgroundWidget : public backgroundWidgetBase
{
    typedef halfLeftNotchedWeightBarBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 125;           ///< Default width.
    static const int defaultHeight_ = 90;           ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    halfLeftNotchedWeightBarBackgroundWidget() : _Mybase( PainterPath ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        qreal x( s.width() );
        x /= defaultWidth_;

        qreal y( s.height() );
        y /= defaultHeight_;

        QPainterPath path;

        // draw path counter-clockwise
        path.moveTo( s.width() - 5.0, s.height() - 5.0 );
        path.lineTo( s.width() - 5.0, y * 20.0 );
        path.lineTo( x * 35.0,  y * 20.0 );     // 0=(160,20) Little slope at the top, so keep it
        path.lineTo( x * 25.0,  5.0 );      // 0=(150,5)
        path.lineTo( 5.0,   5.0 );      // 0=(5,5)  line across the top to 0,0  // should be 120
        path.lineTo( x * 5.0,   s.height() - 5.0 );     // 0=(20,85) far left vert edge 20,85y=HeightOfBox(keep)

        path.closeSubpath();

        return path;
    }
};

/// Widget drawing details for a left notched field on the weight bar (aka tare).
class halfMoreLeftNotchedWeightBarBackgroundWidget : public backgroundWidgetBase
{
    typedef halfMoreLeftNotchedWeightBarBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 125;           /// 125 < Default width.
    static const int defaultHeight_ = 90;           ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    halfMoreLeftNotchedWeightBarBackgroundWidget() : _Mybase( PainterPath ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        qreal x( s.width() );               // 168
        x /= defaultWidth_;                 // 1.264

        qreal y( s.height() );              // 90
        y /= defaultHeight_;                // 1

        QPainterPath path;

        // draw path counter-clockwise
        path.moveTo( s.width() - 5.0, s.height() - 5.0 );   // 158-5,90-5       152,85  moveTo BottomRight corner
        path.lineTo( s.width() - 5.0, y * 20.0 );           // 158-5,1*20=20    152,20  lineTo TopRight corner
        path.lineTo( x * 53.0,  y * 20.0 );                 // 1.264*35,1*20     44,20  lineTo Bottom of little slope,
        path.lineTo( x * 44.0,  5.0 );                      // 1.264*25,5        31,5
        path.lineTo( 5.0,   5.0 );                          // 5,5                5,5   line across the top to 0,0
        path.lineTo( x * 5.0,   s.height() - 5.0 );       // 1.264*5, 90-5      6,85   far left vert edge 20,

        path.closeSubpath();                                //                  152,85

        return path;
    }
};

/// Widget drawing details for a left notched field on the weight bar (aka tare).
class verticalBarBackgroundWidget : public backgroundWidgetBase
{
    typedef verticalBarBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 25;           ///< Default width.
    static const int defaultHeight_ = 90;           ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    verticalBarBackgroundWidget() : _Mybase( PainterPath ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @return  painter path
     */
    virtual QPainterPath painterPath() const
    {
        QPainterPath path;

        path.moveTo( 5.0, 6.0  );
        path.lineTo( 5.0, 85.0 );
        path.lineTo(15.0, 85.0 );
        path.lineTo(15.0, 6.0  );
        path.closeSubpath();

        return path;
    }
};

/// Widget drawing details for a rectangular field.
class rectangleBackgroundWidget : public backgroundWidgetBase
{
    typedef rectangleBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    rectangleBackgroundWidget() : _Mybase( PainterPath ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @param[in] s  size
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        // draw path counter-clockwise
        QPainterPath path;
        path.moveTo( 5.0, 5.0 );
        path.lineTo( s.width() - 5.0, 5.0 );
        path.lineTo( s.width() - 5.0, s.height() - 5.0 );
        path.lineTo( 5.0, s.height() - 5.0 );
        path.closeSubpath();

        return path;
    }

};

/// Widget drawing details for the operate mode panel field.
class operateModePanelBackgroundWidget : public backgroundWidgetBase
{
    typedef operateModePanelBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 420;           ///< Default width.
    static const int defaultHeight_ = 490;          ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    operateModePanelBackgroundWidget() : _Mybase( PainterPath ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @param[in] s  size
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        qreal x( s.width() );
        x /= defaultWidth_;

        qreal y( s.height() );
        y /= defaultHeight_;

        // draw path counter-clockwise
        QPainterPath path;
        path.moveTo( 10.0, y * 110.0 );
        path.lineTo( 10.0, s.height() - 10.0 );
        path.lineTo( s.width() - 10.0, s.height() - 10.0 );
        path.lineTo( s.width() - 10.0, 10.0 );
        path.lineTo( x * 115.0, 10.0 );
        path.lineTo( x * 100.0, y * 110.0 );
        path.closeSubpath();

        return path;
    }

};

/// Widget drawing details for the operate mode extended panel field.
class operateModeExPanelBackgroundWidget : public backgroundWidgetBase
{
    typedef operateModeExPanelBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 465;           ///< Default width.
    static const int defaultHeight_ = 490;          ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    operateModeExPanelBackgroundWidget() : _Mybase( PainterPath ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @param[in] size  size
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        // draw path counter-clockwise
        QPainterPath path;
        path.moveTo( 2.0, 10.0 );
        path.lineTo( s.width() - 10.0, 10.0 );
        path.lineTo( s.width() - 10.0, s.height() - 10.0 );
        path.lineTo( 2.0, s.height() - 10.0 );
        path.closeSubpath();

        return path;
    }

};

/// Widget drawing details for the supervisor mode panel field.
class supervisorModePanelBackgroundWidget : public backgroundWidgetBase
{
    typedef supervisorModePanelBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 885;           ///< Default width.
    static const int defaultHeight_ = 490;          ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    supervisorModePanelBackgroundWidget() : _Mybase( PainterPath ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @param[in] s  size
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        qreal x( s.width() );
        x /= defaultWidth_;

        qreal y( s.height() );
        y /= defaultHeight_;

        // draw path counter-clockwise
        QPainterPath path;
        path.moveTo( 10.0, y * 110.0 );
        path.lineTo( 10.0, s.height() - 10.0 );
        path.lineTo( s.width() - 10.0, s.height() - 10.0 );
        path.lineTo( s.width() - 10.0, 10.0 );
        path.lineTo( x * 115.0, 10.0 );
        path.lineTo( x * 100.0, y * 110.0 );
        path.closeSubpath();

        return path;
    }

};

/// Widget drawing details for the supervisor mode compact panel field.
class supervisorModeCompactPanelBackgroundWidget : public backgroundWidgetBase
{
    typedef supervisorModeCompactPanelBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 550;           ///< Default width.
    static const int defaultHeight_ = 490;          ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    supervisorModeCompactPanelBackgroundWidget() : _Mybase( PainterPath ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @param[in] s  size
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        qreal x( s.width() );
        x /= defaultWidth_;

        qreal y( s.height() );
        y /= defaultHeight_;

        // draw path counter-clockwise
        QPainterPath path;
        path.moveTo( 10.0, y * 110.0 );
        path.lineTo( 10.0, s.height() - 10.0 );
        path.lineTo( s.width() - 5.0, s.height() - 10.0 );
        path.lineTo( s.width() - 5.0, 10.0 );
        path.lineTo( x * 115.0, 10.0 );
        path.lineTo( x * 100.0, y * 110.0 );
        path.closeSubpath();

        return path;
    }

};

/// Widget drawing details for the service mode panel field.
class serviceModePanelBackgroundWidget : public backgroundWidgetBase
{
    typedef serviceModePanelBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 700;           ///< Default width.
    static const int defaultHeight_ = 490;          ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    serviceModePanelBackgroundWidget() : _Mybase( PainterPath ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @param[in] s  size
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        qreal x( s.width() );
        x /= defaultWidth_;

        qreal y( s.height() );
        y /= defaultHeight_;

        // draw path counter-clockwise
        QPainterPath path;
        path.moveTo( 10.0, y * 110.0 );
        path.lineTo( 10.0, s.height() - 10.0 );
        path.lineTo( s.width() - 5.0, s.height() - 10.0 );
        path.lineTo( s.width() - 5.0, 10.0 );
        path.lineTo( x * 115.0, 10.0 );
        path.lineTo( x * 100.0, y * 110.0 );
        path.closeSubpath();

        return path;
    }

};

/// Widget drawing details for the mode login panel field.
/**
 * This login panel has less width to accommodate the keypad.
 */
class modeLoginPanelBackgroundWidget : public backgroundWidgetBase
{
    typedef modeLoginPanelBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    static const int defaultWidth_ = 500;           ///< Default width.
    static const int defaultHeight_ = 490;          ///< Default height.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    modeLoginPanelBackgroundWidget() : _Mybase( PainterPath ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve widget painter path.
    /**
     * @param[in] s  size
     * @return  painter path
     */
    virtual QPainterPath painterPath( const QSize &s ) const
    {
        qreal x( s.width() );
        x /= defaultWidth_;

        qreal y( s.height() );
        y /= defaultHeight_;

        // draw path counter-clockwise
        QPainterPath path;
        path.moveTo( 10.0, y * 110.0 );
        path.lineTo( 10.0, s.height() - 10.0 );
        path.lineTo( s.width() - 5.0, s.height() - 10.0 );
        path.lineTo( s.width() - 5.0, 10.0 );
        path.lineTo( x * 115.0, 10.0 );
        path.lineTo( x * 100.0, y * 110.0 );
        path.closeSubpath();

        return path;
    }

};

/// Widget drawing details for a panel button.
class panelButtonBackgroundWidget : public backgroundWidgetBase
{
    typedef panelButtonBackgroundWidget _Myt;
    typedef backgroundWidgetBase _Mybase;

public:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] r  corner radius
     */
    panelButtonBackgroundWidget( int r ) : _Mybase( RoundedRect ), borderRadius_( r ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve x radius.
    /**
     * @return  radius
     */
    virtual qreal xRadius() const {return borderRadius_;}

    /// Retrieve y radius.
    /**
     * @return  radius
     */
    virtual qreal yRadius() const {return borderRadius_;}

protected:

    int borderRadius_;                              ///< Corner radius.

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // BACKGROUNDWIDGETTYPES_H
