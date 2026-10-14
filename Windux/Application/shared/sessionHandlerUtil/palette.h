/**
 * @file palette.h
 * Utility methods for QPalette handling
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PALETTE_H
#define PALETTE_H

#include <QApplication>
#include <QPalette>
#include <QPixmap>
#include <QWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class modifyPaletteBackground
 * @brief Functors that modify the palette background role.
 */
class modifyPaletteBackground
{
public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /// Constructor.
    modifyPaletteBackground() {}

    /**
     * Constructor.
     *
     * @param[in] brush  background brush
     */
    modifyPaletteBackground( const QBrush& brush ) : brush_( brush ) {}

    /**
     * Constructor.
     *
     * @param[in] image  background image
     */
    modifyPaletteBackground( const QPixmap& image ) : brush_( QBrush( image ) ) {}

    /// Destructor.
    virtual ~modifyPaletteBackground() {}

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     */
    void operator () ( QWidget *widget ) const {setBackgroundRole( widget, brush_ );}

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     * @param[in] brush  background brush
     */
    void operator () ( QWidget *widget, const QBrush& brush ) const {setBackgroundRole( widget, brush );}

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     * @param[in] image  background image
     */
    void operator () ( QWidget *widget, const QPixmap& image ) const {setBackgroundRole( widget, QBrush( image ) );}

private:

    QBrush brush_;

    /**
     * Set the background palette.
     */
    static void setBackgroundRole( QWidget *widget, const QBrush& brush )
    {
        QPalette p( widget->palette() );
        p.setBrush( widget->backgroundRole(), brush );
        widget->setPalette( p );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class modifyPaletteForeground
 * @brief Functors that modify the palette foreground roles.
 */
class modifyPaletteForeground
{
public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] palette  palette to set foreground from
     */
    modifyPaletteForeground( const QPalette& palette = QApplication::palette() ) : palette_( palette ) {}

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     */
    void operator () ( QWidget *widget ) const {setForegroundRole( widget, palette_ );}

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     * @param[in] palette  palette to set foreground from
     */
    void operator () ( QWidget *widget, const QPalette& palette ) const {setForegroundRole( widget, palette );}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Create foreground palette using specific color.
     *
     * @param[in] color  color to create palette from
     * @returns  created palette
     */
    static QPalette createPalette( const QColor& color )
    {
        QPalette p( QApplication::palette() );

        p.setColor( QPalette::Active, QPalette::ButtonText, color );
        p.setColor( QPalette::Active, QPalette::BrightText, invertColor( color ) );
        p.setColor( QPalette::Active, QPalette::Text, color );
        p.setColor( QPalette::Active, QPalette::ToolTipText, color );
        p.setColor( QPalette::Active, QPalette::WindowText, color );

        p.setColor( QPalette::Inactive, QPalette::ButtonText, color );
        p.setColor( QPalette::Inactive, QPalette::BrightText, invertColor( color ) );
        p.setColor( QPalette::Inactive, QPalette::Text, color );
        p.setColor( QPalette::Inactive, QPalette::ToolTipText, color );
        p.setColor( QPalette::Inactive, QPalette::WindowText, color );

        return p;
    }

    /**
     * Create foreground palette using specific colors.
     *
     * @param[in] color  color to create palette from
     * @param[in] disabledColor  color to create disabled palette from
     * @returns  created palette
     */
    static QPalette createPalette( const QColor& color, const QColor& disabledColor )
    {
        QPalette p( createPalette( color ) );

        p.setColor( QPalette::Disabled, QPalette::ButtonText, disabledColor );
        p.setColor( QPalette::Disabled, QPalette::BrightText, invertColor( color ) );
        p.setColor( QPalette::Disabled, QPalette::Text, disabledColor );
        p.setColor( QPalette::Disabled, QPalette::ToolTipText, disabledColor );
        p.setColor( QPalette::Disabled, QPalette::WindowText, disabledColor );

        return p;
    }

    /**
     * Inverts color passed in.
     *
     * @param[in] color  color to invert
     * @returns  inverted color
     */
    static QColor invertColor( const QColor& color )
    {
        return QColor( 255 - color.red(), 255 - color.green(), 255 - color.blue(), color.alpha() );
    }

private:

    QPalette palette_;

    /**
     * Set palette color from another palette.
     */
    static void setColor( QPalette& p, const QPalette& from, QPalette::ColorGroup cg, QPalette::ColorRole cr )
    {
        p.setColor( cg, cr, from.color( cg, cr ) );
    }

    /**
     * Set palette foreground color group from another palette.
     */
    static void setColorGroup( QPalette& p, const QPalette& from, QPalette::ColorGroup cg )
    {
        setColor( p, from, cg, QPalette::ButtonText );
        setColor( p, from, cg, QPalette::BrightText );
        setColor( p, from, cg, QPalette::Text );
        setColor( p, from, cg, QPalette::ToolTipText );
        setColor( p, from, cg, QPalette::WindowText );
    }

    /**
     * Set palette foreground colors from another palette.
     */
    static void setForegroundRole( QWidget *widget, const QPalette& palette )
    {
        QPalette p( widget->palette() );

        setColorGroup( p, palette, QPalette::Active );
        setColorGroup( p, palette, QPalette::Disabled );
        setColorGroup( p, palette, QPalette::Inactive );

        widget->setPalette( p );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class modifyPaletteColorRole
 * @brief Functors that modify the @p ROLE palette color role.
 */
template <QPalette::ColorRole ROLE>
class modifyPaletteColorRole
{
public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /// Constructor.
    modifyPaletteColorRole() {}

    /**
     * Constructor.
     *
     * @param[in] brush  background brush
     */
    modifyPaletteColorRole( const QBrush& brush ) : brush_( brush ) {}

    /**
     * Constructor.
     *
     * @param[in] image  background image
     */
    modifyPaletteColorRole( const QPixmap& image ) : brush_( QBrush( image ) ) {}

    /// Destructor.
    virtual ~modifyPaletteColorRole() {}

    ///////////////////////////////////////////////////////////////////////////
    // Operators

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     */
    void operator () ( QWidget *widget ) const {setRole( widget, brush_ );}

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     * @param[in] brush  background brush
     */
    void operator () ( QWidget *widget, const QBrush& brush ) const {setRole( widget, brush );}

    /**
     * Function operator.
     *
     * @param[in] widget  widget to modify
     * @param[in] image  background image
     */
    void operator () ( QWidget *widget, const QPixmap& image ) const {setRole( widget, QBrush( image ) );}

private:

    QBrush brush_;

    /**
     * Set the background palette.
     */
    static void setRole( QWidget *widget, const QBrush& brush )
    {
        QPalette p( widget->palette() );
        p.setBrush( ROLE, brush );
        widget->setPalette( p );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class modifyPaletteHighlight
 * @brief Functors that modify the palette highlight role.
 */
typedef modifyPaletteColorRole<QPalette::Base> modifyPaletteBase;

/**
 * @class modifyPaletteButton
 * @brief Functors that modify the palette button role.
 */
typedef modifyPaletteColorRole<QPalette::Button> modifyPaletteButton;

/**
 * @class modifyPaletteHighlight
 * @brief Functors that modify the palette highlight role.
 */
typedef modifyPaletteColorRole<QPalette::Highlight> modifyPaletteHighlight;

/**
 * @class modifyPaletteHighlightText
 * @brief Functors that modify the palette highlight text role.
 */
typedef modifyPaletteColorRole<QPalette::HighlightedText> modifyPaletteHighlightText;

/**
 * @class modifyPaletteWindow
 * @brief Functors that modify the palette window role.
 */
typedef modifyPaletteColorRole<QPalette::Window> modifyPaletteWindow;

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PALETTE_H
