/**
 * @file serviceNewWrapperTestKnife.h
 * @brief Service screen for testing wrapper knife.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup Supervisor and Service Mode CSU
 * @{
*/

#ifndef SERVICENEWWRAPPERTESTKNIFE_H
#define SERVICENEWWRAPPERTESTKNIFE_H

#include "screenwidget.h"

class QLabel;
class QPushButton;
class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Service screen for testing wrapper knife.
class serviceNewWrapperTestKnife : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestKnife _Myt;
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
    explicit serviceNewWrapperTestKnife( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~serviceNewWrapperTestKnife();

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

    QLabel *title_;

    QLabel *knifeUpDownLabel_;
    QPushButton *knifeUp_;
    QPushButton *knifeDown_;

    QLabel *minLabel_;
    QLabel *installLabel_;
    QLabel *maxLabel_;
    QLabel *currentLabel_;

    QLabel *knifeUpTimeLabel_;
    QLabel *knifeUpTimeMin_;
    QLabel *knifeUpTimeSetup_;
    QLabel *knifeUpTimeMax_;
    QLabel *eventUpTime_;

    QLabel *dateUpLabel_;
    QLabel *dateUpMin_;
    QLabel *dateUpSetup_;
    QLabel *dateUpMax_;

    QLabel *knifeDownTimeLabel_;
    QLabel *knifeDownTimeMin_;
    QLabel *knifeDownTimeSetup_;
    QLabel *knifeDownTimeMax_;
    QLabel *eventDownTime_;

    QLabel *dateDownLabel_;
    QLabel *dateDownMin_;
    QLabel *dateDownSetup_;
    QLabel *dateDownMax_;

    QLabel *sensorsTitle_;
    QLabel *knifeUpSensorLabel_;
    QLabel *knifeUpSensor_;
    QLabel *knifeDownSensorLabel_;
    QLabel *knifeDownSensor_;
    QLabel *regulatedPressureLabel_;
    QLabel *regulatedPressure_;

    QPushButton *clearInstall_;
    QPushButton *cycleWrapperButton_;

    QPalette pushButtonPalette_;

    bool testActive_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICENEWWRAPPERTESTKNIFE_H
