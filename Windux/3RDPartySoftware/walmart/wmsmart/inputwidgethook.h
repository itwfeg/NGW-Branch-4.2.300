/**
 * @file inputwidgethook.h
 * Hook into Qt for poping virtual input devices.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef INPUTWIDGETHOOK_H
#define INPUTWIDGETHOOK_H

#include <QObject>

class inputWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Hook into Qt for poping virtual input devices.
class inputWidgetHook : public QObject
{
    Q_OBJECT

    typedef inputWidgetHook _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    explicit inputWidgetHook( QObject *parent = 0 );

    /// Destructor.
    virtual ~inputWidgetHook();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve if hook disabled.
    /**
     * @return  @c true if disabled, @c false otherwise
     */
    virtual bool disabled() const {return disabled_;}

    /// Set hook disabled.
    /**
     * @param[in] value  @c true to disable, @c false otherwise
     */
    virtual void setDisabled( bool value ) {disabled_ = value;}

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Dismiss input widget
    virtual void dismiss();

    /// Activate input widget.
    /**
     * @param[in] widget  focus widget
     * @param[in] altLocation  use alternate location, if possible (i.e. top/bottom of screen)
     */
    virtual void invoke( QWidget *widget = NULL, bool altLocation = false );

private Q_SLOTS:

    /// Slot to handle character generation.
    void onCharacterGenerated( int key, Qt::KeyboardModifiers modifiers, const QString &text );

    /// Slot to handle focus change.
    void onFocusChanged( QWidget *oldFocus, QWidget *newFocus );

private:

    bool disabled_;

    QWidget *focusWidget_;

    inputWidget *keyboard_;

    /// Initialize.
    void initialize();

    /// Update focus widget.
    void updateFocusWidget( QWidget *widget, bool altLocation = false );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // INPUTWIDGETHOOK_H
