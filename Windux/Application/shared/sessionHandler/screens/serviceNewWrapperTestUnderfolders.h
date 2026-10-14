/**
 * @file serviceNewWrapperTestUnderfolders.h
 * @brief Service screen for testing wrapper underfolders.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup Supervisor and Service Mode CSU
 * @{
*/

#ifndef SERVICENEWWRAPPERTESTUNDERFOLDERS_H
#define SERVICENEWWRAPPERTESTUNDERFOLDERS_H

#include "screenwidget.h"

class QLabel;
class QPushButton;
class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Service screen for testing wrapper underfolders.
class serviceNewWrapperTestUnderfolders : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestUnderfolders _Myt;
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
    explicit serviceNewWrapperTestUnderfolders( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~serviceNewWrapperTestUnderfolders();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle screen changing.
    /**
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for push button clicked.
    void onPushButtonClicked( QWidget *widget );

private:

    QLabel *sensorsLabel_;
    QLabel *sideHomeLabel_;
    QLabel *sideHome_;
    QLabel *frontHomeLabel_;
    QLabel *frontHome_;
    QLabel *rearHomeLabel_;
    QLabel *rearHome_;

    QLabel *testLabel_;
    QPushButton *cycleSide_;
    QPushButton *cycleFront_;
    QPushButton *cycleRear_;

    QSignalMapper *pushButtonClicked_;
    QPalette pushButtonPalette_;

    bool testActive_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICENEWWRAPPERTESTUNDERFOLDERS_H
