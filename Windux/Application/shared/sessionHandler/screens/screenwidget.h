/**
 * @file screenwidget.h
 * Base class for all screen widget objects.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

#include <Session.h>

#include <QWidget>
#include <QMargins>
#include <QComboBox>
#include <QEasingCurve>

class SessionMainWindow;
class ModeWidget;
class QTabWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Base class for all screen widget objects.
/**
 * Classes that derive this class should implement the following methods:
 * @arg helpText() - retrieve help about the screen
 * @arg translate() - translate the screen to a new language
 * @arg swipeScreenLeft() - screen that should appear when swiping left (optional)
 * @arg swipeScreenRight() - screen that should appear when swiping right (optional)
 * @arg onScreenChanging() - disallow unload when operator changes screens (optional)
 * @arg onSessionMessage() - process session messages that the screen cares about (optional)
 */
class ScreenWidget : public QWidget
{
    Q_OBJECT

    friend class ModeWidget;

    typedef ScreenWidget _Myt;
    typedef QWidget _Mybase;

public:

    /// Swipe directions.
    enum SwipeDirection
    {
        Down,                                   ///< Swipe the screen down.
        Left,                                   ///< Swipe the screen left.
        Right,                                  ///< Swipe the screen right.
        Up                                      ///< Swipe the screen up.
    };

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~ScreenWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve screen help text.
    /**
     * When help is activated on a screen this property returns the screen title and help text.
     *
     * @param[out] title  screen title
     * @param[out] description  screen help description
     * @return  @c true if help exists for the screen, @c false otherwise
     */
    virtual bool helpText( QString& title, QString& description ) const;

    /// Retrieve margins.
    /**
     * Some screens add widgets to the top and/or bottom. This property returns how much space
     * the widgets are taking up. This property is useful because we may want to offset the
     * numeric keypad or other widgets.
     *
     * @return  margins
     */
    virtual QMargins margins() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

public Q_SLOTS:

    /// Slot for handling a swipe.
    /**
     * @param[in] direction  swipe direction
     * @return  @c true if swipe available and handled, @c false otherwise
     */
    virtual bool handleSwipe( SwipeDirection direction );

protected:

    SessionMainWindow *mainWindow_;             ///< Pointer to the main window.
    ModeWidget *mode_;                          ///< Pointer to the current mode.

    Session *session_;                          ///< Pointer to the session.

    static const int marginSpacing_;            ///< Margin spacing for screens that require it (i.e. supervisor and service screens)

    static int scalingTableX_[2048];            ///< Protected so that all child widgets can reference table lookups for screen-specific scaling
    static int scalingTableY_[1024];            ///  which may require more fine tuning than scaleWidget()'s recursion allows

    bool scaling_;                              ///< Protected for the same reasons as scalingTables

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit ScreenWidget( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if an animation (i.e. swipe) is being performed.
    /**
     * @return  @c true if animating, @c false otherwise
     */
    static bool isAnimating() {return animating_;}

    /// Check for specific error message is handled by screen.
    /**
     * Session will call this method upon receiving an error message. The default error handling
     * (i.e. popup an error dialog) shall be invoked unless @c true is returned from
     * this property.
     *
     * @return  @c true to use custom error handler, @c false otherwise
     */
    virtual bool isErrorMessageHandler( const SessionMessage& /*errorMsg*/ ) const {return false;}

    /// Screen that gets displayed when swiping (gesturing) the screen down.
    /**
     * @return  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenDown() const {return NULL;}

    /// Screen that gets displayed when swiping (gesturing) the screen left.
    /**
     * @return  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenLeft() const {return NULL;}

    /// Screen that gets displayed when swiping (gesturing) the screen right.
    /**
     * @return  previous screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenRight() const {return NULL;}

    /// Screen that gets displayed when swiping (gesturing) the screen up.
    /**
     * @return  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenUp() const {return NULL;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event filter.
    /**
     * @param[in,out] obj  object
     * @param[in,out] e  event
     * @return  @c true to filter, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /// Handle screen changing.
    /**
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @return  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /// Scale screen to monitor resolution.
    virtual void scaleScreen();

    /// Scale for screen.
    /**
     * @param[in] value  value to scale
     * @return  scaled value
     */
    virtual int scaleFontPointSizeForScreen( int value ) const {return scalingTableY_[value];}

    ///Scale tabs evenly across container width for any screen
    /**
     * @param tabWidget - widget whose tabs we are scaling
     */
    virtual void expandTabs(QTabWidget *tabWidget);

    ///Set visibility to 0 for component and children
    /**
     * @param widget - widget to hide
     */
    virtual void hideAll(QWidget *widget);

    ///Scale the combo box and fix size of dropdown (images scale in stylesheet)
    /**
     * @param cboTemp - combo box to scale
     */
    virtual void fixComboBoxStyleSheet(QComboBox *cboTemp);

    /// Scale for screen.
    /**
     * @param[in] value  value to scale
     * @return  scaled value
     */
    virtual QPoint scalePointForScreen( const QPoint &value ) const;

    /// Scale for screen.
    /**
     * @param[in] value  value to scale
     * @return  scaled value
     */
    virtual QRect scaleRectForScreen( const QRect &value ) const;

    /// Scale for screen.
    /**
     * @param[in] value  value to scale
     * @return  scaled value
     */
    virtual QSize scaleSizeForScreen( const QSize &value ) const;

    /// Scale widget to screen size.
    void scaleWidget( QWidget *w );

    /// Scale for screen.
    /**
     * @param[in] value  value to scale
     * @return  scaled value
     */
    virtual int scaleWidthForScreen( int value ) const {return scalingTableX_[value];}

    /// Scale for screen.
    /**
     * @param[in] value  value to scale
     * @return  scaled value
     */
    virtual int scaleHeightForScreen( int value ) const {return scalingTableY_[value];}

private Q_SLOTS:

    /// Slot to handle session message.
    void onSessionMessageReceive( const SessionMessage& msg );

    /// Slot to handle animation finished.
    void onAnimationFinished();

private:

    static bool animating_;
    static bool scalingTableInit_;

    ScreenWidget *screen_;



    static const int animationSpeed_;
    static const QEasingCurve::Type animationType_;



};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SCREENWIDGET_H
