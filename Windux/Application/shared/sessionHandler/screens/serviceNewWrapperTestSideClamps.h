/**
 * @file serviceNewWrapperTestSideClamps.h
 * @brief Service screen for testing wrapper side clamps.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup Supervisor and Service Mode CSU
 * @{
*/

#ifndef SERVICENEWWRAPPERTESTSIDECLAMPS_H
#define SERVICENEWWRAPPERTESTSIDECLAMPS_H

#include "screenwidget.h"

class DomEvolWrapperSystemInformation;

class QLabel;
class QPushButton;
class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Service screen for testing wrapper side clamps.
class serviceNewWrapperTestSideClamps : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestSideClamps _Myt;
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
    explicit serviceNewWrapperTestSideClamps( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~serviceNewWrapperTestSideClamps();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

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
    void onPushButtonClicked();

private:

    DomEvolWrapperSystemInformation *wrapperSystemInformation_;


    QLabel *title_;

    QLabel *sideClampsLabel_;
    QPushButton *clampsOn_;
    QPushButton *clampsOff_;

    QPushButton *cycleLowerFilmButton_;
    QPushButton *finishButton_;
    QPushButton *cycleUpperFilmButton_;

    QLabel *sensorsTitleSpacer_;
    QLabel *sensorsTitle_;
    QLabel *homeSensorLabel_;
    QLabel *homeSensor_;
    QLabel *rightClampSensorLabel_;
    QLabel *rightClampSensor_;
    QLabel *leftClampSensorLabel_;
    QLabel *leftClampSensor_;
    QLabel *regulatedPressureLabel_;
    QLabel *regulatedPressure_;

    QLabel *actuationTitle_;
    QLabel *actuationOpenTitle_;
    QLabel *actuationCloseTitle_;
    QLabel *clampOpenSpacer_;
    QLabel *clampCloseSpacer_;
    QLabel *rightClampOpen_;
    QLabel *rightClampClose_;
    QLabel *leftClampOpen_;
    QLabel *leftClampClose_;


    QPalette pushButtonPalette_;

    bool testActive_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICENEWWRAPPERTESTSIDECLAMPS_H
