/**
 * @file abstractbutton.h
 * Abstract button class with more control than the standard one.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef ABSTRACTBUTTON_H
#define ABSTRACTBUTTON_H

#include "backgroundwidget.h"

#include <pixmap.h>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class abstractButton
 * @brief Abstract button class with more control than the standard one.
 *
 * @tparam _Button  base button class
 */
template <class _Button>
class abstractButton : public backgroundWidget<_Button>
{
    typedef abstractButton _Myt;
    typedef backgroundWidget<_Button> _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     * @param[in] imagePressed  background image for the widget when pressed
     */
    explicit abstractButton(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap(),
        const QPixmap& imagePressed = QPixmap() );

    /**
     * Constructor.
     *
     * @param[in] text  button text
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     * @param[in] imagePressed  background image for the widget when pressed
     */
    explicit abstractButton(
        const QString& text,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap(),
        const QPixmap& imagePressed = QPixmap() );

    /**
     * Constructor.
     *
     * @param[in] icon  button icon
     * @param[in] text  button text
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     * @param[in] imagePressed  background image for the widget when pressed
     */
    explicit abstractButton(
        const QIcon& icon,
        const QString& text,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap(),
        const QPixmap& imagePressed = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~abstractButton();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve background for pressed state.
     *
     * @returns  background pixmap
     */
    virtual QPixmap backgroundPressed() const {return backgroundPressed_;}

    /**
     * Set the background for pressed state.
     *
     * @param[in] value  background pixmap
     */
    virtual void setBackgroundPressed( const QPixmap& value );

    /**
     * Set the button down state.
     *
     * @param[in] value  @c true to set down state, @c false otherwise
     */
    virtual void setDown( bool value );

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Set the button appearance.
     *
     * This method is called when the button is pressed, released, or toggled. You may override
     * this method to apply your own styling. This base class method sets the background palette.
     *
     * @param[in] pressed  @c true when button in pressed state, @c false otherwise
     */
    virtual void setButtonAppearance( bool pressed );

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Method for when button is pressed.
     */
    virtual void onPressed();

    /**
     * Method for when button is released.
     */
    virtual void onReleased();

    /**
     * Method for when button is toggled.
     *
     * @param[in] value  @c true when button is down, @c false otherwise
     */
    virtual void onToggled( bool value );

private:

    QPixmap backgroundPressed_;

    /**
     * Initialize.
     */
    void initialize( const QPixmap& imagePressed );

};

template <class _Button>
abstractButton<_Button>::abstractButton( QWidget *parent, const QPixmap& image, const QPixmap& imagePressed ) :
    _Mybase( image, parent )
{
    // initialize
    initialize( imagePressed );
}

template <class _Button>
abstractButton<_Button>::abstractButton( const QString& text, QWidget *parent, const QPixmap& image, const QPixmap& imagePressed ) :
    _Mybase( image, parent )
{
    // initialize
    initialize( imagePressed );

    // set properties
    _Mybase::setText( text );
}

template <class _Button>
abstractButton<_Button>::abstractButton( const QIcon& icon, const QString& text, QWidget *parent, const QPixmap& image, const QPixmap& imagePressed ) :
    _Mybase( image, parent )
{
    // initialize
    initialize( imagePressed );

    // set properties
    _Mybase::setIcon( icon );
    _Mybase::setText( text );
}

template <class _Button>
abstractButton<_Button>::~abstractButton()
{
}

template <class _Button>
void abstractButton<_Button>::setBackgroundPressed( const QPixmap& value )
{
    // set background
    initialize( value );
    _Mybase::update();
}

template <class _Button>
void abstractButton<_Button>::setDown( bool value )
{
    // set appearance
    if ( !_Mybase::isCheckable() )
        setButtonAppearance( value );

    _Mybase::setDown( value );
}

template <class _Button>
void abstractButton<_Button>::setButtonAppearance( bool pressed )
{
    QPixmap image = (pressed ? backgroundPressed() : _Mybase::background());

    // set the background
    if ( !image.isNull() )
    {
        _Mybase::setBackgroundPalette( image );
        _Mybase::update();
    }
}

template <class _Button>
void abstractButton<_Button>::onPressed()
{
    if ( _Mybase::isCheckable() )
        return;

    // set appearance
    setButtonAppearance( true );
}

template <class _Button>
void abstractButton<_Button>::onReleased()
{
    if ( _Mybase::isCheckable() )
        return;

    // set appearance
    setButtonAppearance( false );
}

template <class _Button>
void abstractButton<_Button>::onToggled( bool value )
{
    if ( !_Mybase::isCheckable() )
        return;

    // set appearance
    setButtonAppearance( value );
}

template <class _Button>
void abstractButton<_Button>::initialize( const QPixmap& imagePressed )
{
    // set background for pressed state
    backgroundPressed_ = imagePressed;

    // error loading the image... use inversed image instead...
    if ( backgroundPressed_.isNull() )
    {
        invertPixmap inverter;
        QPixmap image = _Mybase::background();

        // invert!
        if ( !image.isNull() )
            backgroundPressed_ = inverter( image );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // ABSTRACTBUTTON_H
