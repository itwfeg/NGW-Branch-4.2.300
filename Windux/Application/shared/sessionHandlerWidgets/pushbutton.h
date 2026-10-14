/**
 * @file pushbutton.h
 * Push button class with more control than the standard one.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include "abstractbutton.h"

#include <QPushButton>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class pushButton
 * @brief Push button class with more control than the standard one.
 */
class pushButton : public abstractButton<QPushButton>
{
    Q_OBJECT

    typedef pushButton _Myt;
    typedef abstractButton<QPushButton> _Mybase;

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
    explicit pushButton(
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
    explicit pushButton(
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
    explicit pushButton(
        const QIcon& icon,
        const QString& text,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap(),
        const QPixmap& imagePressed = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~pushButton();

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

#endif // PUSHBUTTON_H
