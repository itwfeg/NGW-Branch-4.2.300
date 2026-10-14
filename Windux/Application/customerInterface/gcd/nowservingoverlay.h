/**
 * @file nowservingoverlay.h
 * Overlay that displays now serving number to customer.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef NOWSERVINGOVERLAY_H
#define NOWSERVINGOVERLAY_H

#include "overlay.h"

#include <QString>
#include <QWidget>

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Overlay that displays now serving number to customer.
class nowServingOverlay : public QWidget, public overlay
{
    Q_OBJECT

    typedef nowServingOverlay _Myt;
    typedef QWidget _Mybase;

    typedef overlay _Myimpl;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] customer  now serving customer number
     * @param[in] parent  parent widget
     */
    explicit nowServingOverlay( const QString& customer, QWidget *parent = 0 );

    /// Destructor.
    virtual ~nowServingOverlay();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Overlay location.
    /**
     * @return  rect
     */
    virtual QRect location() const {return location_;}

    /// Set overlay location.
    /**
     * @param[in] value  rect
     */
    virtual void setLocation( const QRect& value ) {location_ = value;}

    /// Size hint.
    /**
     * @return  size
     */
    virtual QSize sizeHint() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Initialize overlay.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool initializeOverlay();

    /// Show overlay.
    virtual void showOverlay();

    /// Hide overlay.
    virtual void hideOverlay();

protected:

    /// Paint event.
    /**
     * @param[in,out] e  event
     */
    virtual void paintEvent( QPaintEvent* e );

private Q_SLOTS:

    /// Overlay complete.
    void overlayComplete();

    /// Speak now serving number.
    void speakServingNumber();

    /// Update overlay state.
    void updateOverlay();

private:

    QString customer_;

    unsigned int state_;

    QRect location_;

    QTimer *overlayCompleteTimer_;
    QTimer *updateOverlayTimer_;

    QTimer *speakTimer_;

    /// Initialize.
    void initialize();

    /// Calc font size for rectangle.
    int calcFontPointSize( int size, const QFont& font, const QRect& rect, const QString& text );

    /// Speak now serving number.
    static void speakServingNumberInternal( const QString& number );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // NOWSERVINGOVERLAY_H
