/**
 * @file gradientpushbutton.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef GRADIENTPUSHBUTTON_H
#define GRADIENTPUSHBUTTON_H

#include <QGradient>
#include <QPushButton>

class QString;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Push button that is rendered with a gradient.
class gradientPushButton : public QPushButton
{
    Q_OBJECT

    typedef gradientPushButton _Myt;
    typedef QPushButton _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    gradientPushButton( QWidget *parent = 0 );

    /// Constructor.
    /**
     * @param[in] text  button text
     * @param[in] parent  parent widget
     */
    gradientPushButton( const QString& text, QWidget *parent = 0 );

    /// Destructor.
    virtual ~gradientPushButton();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /// Paint event.
    /**
     * @param[in,out] e  event
     */
    virtual void paintEvent( QPaintEvent *e );

private:

    QLinearGradient gradient_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // GRADIENTPUSHBUTTON_H
