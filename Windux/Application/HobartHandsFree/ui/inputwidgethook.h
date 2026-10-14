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

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Activate input widget.
    /**
     * @param[in] widget  focus widget
     */
    virtual void activate( QWidget *widget = NULL );

    /// Dismiss input widget
    virtual void dismiss();

private Q_SLOTS:

    /// Slot to handle character generation.
    void onCharacterGenerated( int key, Qt::KeyboardModifiers modifiers, const QString &text );

    /// Slot to handle focus change.
    void onFocusChanged( QWidget *oldFocus, QWidget *newFocus );

private:

    QWidget *focusWidget_;

    inputWidget *keyboard_;

    /// Initialize.
    void initialize();

    /// Update focus widget.
    void updateFocusWidget( QWidget *widget );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // INPUTWIDGETHOOK_H
