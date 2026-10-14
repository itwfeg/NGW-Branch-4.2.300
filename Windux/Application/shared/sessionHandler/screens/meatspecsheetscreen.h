/**
 * @file meatspecsheetscreen.h
 * Screen for Cutting Test Meat Specification Sheets.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Job E. Crew
 * @inspired by Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef MEATSPECSHEETSCREEN_H
#define MEATSPECSHEETSCREEN_H

#include "screenwidget.h"

class OperateMode;
class DomCurrentPLU;

namespace hobart {
class meatSheetWidget;
class softkeysWidget;
}

class QStackedWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Meat Specification Sheet builder.
class MeatSheetScreen : public ScreenWidget
{
    Q_OBJECT

    typedef MeatSheetScreen _Myt;
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
    explicit MeatSheetScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~MeatSheetScreen();

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

    /// clear flashkeys to default groups on main screen and meat spec sheet screen.
    /**
     * @param[in] force  @c true to force reset all flashkey banks, @c false otherwise
     */
    virtual void clearFlashkeys();

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

    QWidget *meatPage_;                      // *mealPage_;
    hobart::softkeysWidget *primals_;        // *meals_;

    QWidget *subPrimalPage_;                 // *sideItemPage_;
    hobart::softkeysWidget *subPrimals_;     // *sideItems_;
    hobart::meatSheetWidget *editor_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MEATSPECSHEETSCREEN_H
