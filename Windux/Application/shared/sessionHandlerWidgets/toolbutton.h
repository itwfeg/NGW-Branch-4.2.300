/**
 * @file toolbutton.h
 * Tool button class with more control than the standard one.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QToolButton>

#include "abstractbutton.h"

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class toolButton
 * @brief Tool button class with more control than the standard one.
 *
 * @warning
 * This implementation only handles the basic tool button with icon and text. If you want the
 * advanced features (i.e. dropdown menu) this class is not what you are looking for.
 */
class toolButton : public abstractButton<QToolButton>
{
    Q_OBJECT

    typedef toolButton _Myt;
    typedef abstractButton<QToolButton> _Mybase;

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
    explicit toolButton(
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
    explicit toolButton(
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
    explicit toolButton(
        const QIcon& icon,
        const QString& text,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap(),
        const QPixmap& imagePressed = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~toolButton();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Set the button down state.
     *
     * @param[in] value  @c true to set down state, @c false otherwise
     */
    virtual void setDown( bool value );

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Paint event.
     *
     * @param[in,out] e  paint event
     */
    virtual void paintEvent( QPaintEvent *e );

private Q_SLOTS:

    /**
     * Slot for when button is pressed.
     */
    void onPressed() {_Mybase::onPressed();}

    /**
     * Slot for when button is released.
     */
    void onReleased() {_Mybase::onReleased();}

    /**
     * Slot for when button is toggled.
     */
    void onToggled( bool value ) {_Mybase::onToggled( value );}

private:

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // TOOLBUTTON_H
