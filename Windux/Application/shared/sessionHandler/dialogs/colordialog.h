/**
 * @file colordialog.h
 * Dialog wrapper for QColorDialog.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include "../sessiondialog.h"

#include <QColorDialog>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog wrapper for QColorDialog.
class ColorDialog : public SessionDialog
{
    Q_OBJECT

    typedef ColorDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit ColorDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~ColorDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve current color.
    /**
     * @return  color
     */
    virtual QColor currentColor() const {return d_->currentColor();}

    /// Set current color.
    /**
     * @param[in] value  color
     */
    virtual void setCurrentColor( const QColor& value ) {d_->setCurrentColor( value );}

    /// Retrieve selected color.
    /**
     * @return  color
     */
    virtual QColor selectedColor() const {return d_->selectedColor();}

    /// Retrieve options
    /**
     * @return  options
     */
    virtual QColorDialog::ColorDialogOptions options() const {return d_->options();}

    /// Set options
    /**
     * @param[in] value  options
     */
    virtual void setOptions( QColorDialog::ColorDialogOptions value );

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Hide event.
    /**
     * @param[in,out] e  hide event
     */
    virtual void hideEvent( QHideEvent *e );

private:

    QColorDialog *d_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // COLORDIALOG_H
