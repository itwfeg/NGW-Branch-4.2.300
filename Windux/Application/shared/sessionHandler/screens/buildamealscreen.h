/**
 * @file buildamealscreen.h
 * Screen for build-a-meal.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef BUILDAMEALSCREEN_H
#define BUILDAMEALSCREEN_H

#include "screenwidget.h"

class OperateMode;

namespace hobart {
class mealBuilderWidget;
class softkeysWidget;
}

class QStackedWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Build-A-Meal meal builder.
class MealBuilderScreen : public ScreenWidget
{
    Q_OBJECT

    typedef MealBuilderScreen _Myt;
    typedef ScreenWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit MealBuilderScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~MealBuilderScreen();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

    /// Reset flashkeys to default (initial) group.
    /**
     * @param[in] force  @c true to force reset all flashkey banks, @c false otherwise
     */
    virtual void resetFlashkeys( bool force = false );

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Screen that gets displayed when swiping (gesturing) the screen down.
    /**
     * @returns  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenDown() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for screen changed.
    void onScreenChanged();

    /// Slot for softkey button click.
    void onSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature );

private:

    OperateMode *operateMode_;
    int weighMode_;

    QStackedWidget *pages_;

    QWidget *mealPage_;
    hobart::softkeysWidget *meals_;

    QWidget *sideItemPage_;
    hobart::softkeysWidget *sideItems_;
    hobart::mealBuilderWidget *editor_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // BUILDAMEALSCREEN_H
