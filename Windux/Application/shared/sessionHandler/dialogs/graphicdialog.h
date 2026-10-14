/**
 * @file graphicdialog.h
 * Dialog that displays a graphic to the operator (i.e. planograms).
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef GRAPHICDIALOG_H
#define GRAPHICDIALOG_H

#include "../sessiondialog.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog that displays a graphic to the operator (i.e. planograms).
class GraphicDialog : public SessionDialog
{
    Q_OBJECT

    Q_PROPERTY( bool valid READ isValid )

    typedef GraphicDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] pixmap  graphic to display
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit GraphicDialog( const QPixmap& filename, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Constructor.
    /**
     * @param[in] filename  graphic to display
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit GraphicDialog( const QString& filename, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~GraphicDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if graphic is valid or not.
    /**
     * @return  @c true if graphic valid, @c false otherwise
     */
    virtual bool isValid() const {return !pixmap_.isNull();}

    /// Set whether to show on customer display.
    /**
     * @param[in] value  @c true to show on gcd, @c false otherwise
     */
    virtual void setShowCustomerDisplay( bool value ) {showCustomerDisplay_ = value;}

    /// Retrieve whether to show on customer display.
    /**
     * @return  @c true to show on gcd, @c false otherwise
     */
    virtual bool showCustomerDisplay() const {return showCustomerDisplay_;}

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Hide event.
    /**
     * @param[in,out] e  event
     */
    virtual void hideEvent( QHideEvent *e );

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    /// Show event.
    /**
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

private:

    bool showCustomerDisplay_;
    bool customerDisplayVisible_;

    QString graphicFilename_;
    QWidget *graphic_;

    QPixmap pixmap_;

    /// Initialize.
    void initialize();

    /// Update graphic with pixmap.
    void updateGraphic( const QPixmap& value );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // GRAPHICDIALOG_H
