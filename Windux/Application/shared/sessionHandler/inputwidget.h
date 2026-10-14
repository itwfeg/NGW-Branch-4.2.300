/**
 * @file inputwidget.h
 * Base input widget class for the virtual keyboard and numberpad widget.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QWidget>

class SessionMainWindow;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Base input widget class for the virtual keyboard and numberpad widget.
class InputWidget : public QWidget
{
    Q_OBJECT

    typedef InputWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// This signal is emitted when a key is pressed.
    /**
     * @param[in] key  what key was pressed
     * @param[in] modifiers  keyboard modifiers
     * @param[in] text  text that the key generated
     */
    void characterGenerated( int key, Qt::KeyboardModifiers modifiers, const QString& text = QString() );

    /// This signal is emitted when the input widget is shown or hidden.
    /**
     * @param[in] value  @c true when input widget is shown, @c false when hidden
     */
    void visible( bool value );

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~InputWidget();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Send an enter or tab key to the focused input widget.
    /**
     * @param[in] print  accepting due to print key press
     */
    virtual void acceptFocusWidget( bool print = false );

    /// Clear the text/number from the focused input widget.
    virtual void clearFocusWidget();

    /// Dismiss (hide) the input widget.
    virtual void dismiss();

protected:

    static const int keyAutoRepeatDelay_;                   ///< Initial delay for keys that auto repeat.
    static const int keyAutoRepeatInterval_;                ///< Repeat interval for keys that auto repeat.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit InputWidget( QWidget *parent = 0 );

    // ========================================================================
    // Events
    // ========================================================================

    /// Hide event.
    /**
     * @param[in,out] e  hide event
     */
    virtual void hideEvent( QHideEvent *e );

    /// Show event.
    /**
     * @param[in,out] e  show event
     */
    virtual void showEvent( QShowEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event hook.
    /**
     * This virtual function receives events to an object and should return true if the event
     * @p e was recognized and processed.
     *
     * The event() function can be reimplemented to customize the behavior of an object.
     *
     * @param[in,out] e  event to handle
     * @returns  @c true if event was handled, @c false otherwise
     *
     * @sa installEventFilter()
     * @sa timerEvent()
     * @sa QApplication::sendEvent()
     * @sa QApplication::postEvent()
     * @sa QWidget::event()
     */
    virtual bool event( QEvent *e );

private Q_SLOTS:

    /// Slot to handle focus change.
    void onFocusChanged( QWidget *oldFocus, QWidget *newFocus );

private:

    SessionMainWindow *mainWindow_;

    QWidget *lastFocusedWidget_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // INPUTWIDGET_H
