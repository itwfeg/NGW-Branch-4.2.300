/**
 * @file icons.h
 * Icon widget base classes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef ICONS_H
#define ICONS_H

#include <QWidget>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Base icon widget class.
class icon : public QWidget
{
    Q_OBJECT

    typedef icon _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Signal for when icon is clicked.
    void clicked();

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~icon();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Holds the recommended size for the widget.
    /**
     * If the value of this property is an invalid size, no size is recommended.
     *
     * @return  size
     */
    virtual QSize sizeHint() const;

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit icon( QWidget *parent = 0 );

    // ========================================================================
    // Events
    // ========================================================================

    /// Mouse release event.
    /**
     * @param[in,out] event  mouse event
     */
    virtual void mouseReleaseEvent( QMouseEvent *event );

    /// Paint event.
    /**
     * @param[in,out] event  paint event
     */
    virtual void paintEvent( QPaintEvent *event );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve icon pixmap.
    /**
     * @return  pixmap
     */
    virtual QPixmap pixmap() const {return icon_;}

    /// Set icon pixmap.
    /**
     * @param[in] value  pixmap
     */
    virtual void setPixmap( const QPixmap& value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Scale pixmap to widget size.
    /**
     * @param[in] pixmap  pixmap to scale
     * @return  scaled pixmap
     */
    virtual QPixmap scalePixmap( const QPixmap &pixmap ) const;

private:

    QPixmap icon_;                                  ///< Icon for normal (non-alert) state.

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Base class for alertable icons.
class alertableIcon : public icon
{
    Q_OBJECT

    typedef alertableIcon _Myt;
    typedef icon _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~alertableIcon();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if icon is in an alert state.
    /**
     * @return  @c true if icon is in alert state, @c false otherwise
     */
    virtual bool alertState() const {return alertState_;}

    /// Set icon alert state.
    /**
     * @param[in]  @c true if icon is in alert state, @c false otherwise
     */
    virtual void setAlertState( bool value );

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit alertableIcon( QWidget *parent = 0 );

    // ========================================================================
    // Events
    // ========================================================================

    /// Paint event.
    /**
     * @param[in,out] event  paint event
     */
    virtual void paintEvent( QPaintEvent *event );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve the always visible property.
    /**
     * Normally icons are only visible in the alert state. When this property is set the icon
     * will be visible all the time.
     *
     * @note
     * As per Scott and Jill this is visible all the time now...
     *
     * @return  @c true if visible all the time, @c false otherwise
     */
    virtual bool alwaysVisible() const {return true;}

    /// Retrieve alert icon pixmap.
    /**
     * @return  pixmap
     */
    virtual QPixmap alertPixmap() const {return alertIcon_;}

    /// Set alert icon pixmap.
    /**
     * @param[in] value  pixmap
     */
    virtual void setAlertPixmap( const QPixmap& value );

private Q_SLOTS:

    /// Slot for timer timeout.
    void onAlertTimerTimeout();

private:

    QPixmap alertIcon_;                             ///< Icon for alert state.

    bool alertState_;
    bool blinking_;

    static QTimer *alertTimer_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // ICONS_H
