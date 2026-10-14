/**
 * @file serviceNewWrapperCalibration.h
 * @brief Service screen for wrapper calibration.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup Supervisor and Service Mode CSU
 * @{
*/

#ifndef SERVICENEWWRAPPERCALIBRATION_H
#define SERVICENEWWRAPPERCALIBRATION_H

#include "screenwidget.h"

class DomEvolWrapperSystemInformation;

class QLabel;
class QPushButton;
class QRadioButton;
class QSignalMapper;

namespace hobart {
class numericLineEdit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Service screen for wrapper calibration.
class serviceNewWrapperCalibration : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperCalibration _Myt;
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
    explicit serviceNewWrapperCalibration( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~serviceNewWrapperCalibration();

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

    /// Slot for calibrate clicked.
    void onCalibrateClicked();

    /// Slot for radio button clicked.
    void onRadioButtonClicked( QWidget *widget );

private:

    QLabel *title_;

    QRadioButton *allMechanisms_;
    QLabel *mechValue_;
    QRadioButton *primary_;
    QRadioButton *shifter_;
    QRadioButton *gripper_;
    QRadioButton *sideClamps_;
    QRadioButton *pusher_;
    QRadioButton *frontUnderfolder_;
    QRadioButton *rearUnderfolder_;
    QRadioButton *sideUnderfolders_;
    QRadioButton *actuators_;

    QLabel *trayLengthLabel_;
    hobart::numericLineEdit *trayLength_;
    QLabel *maxLength_;
    QPushButton *calibrate_;

    QSignalMapper *radioButtonClicked_;

    QMap<QWidget*, int> mechanisms_;

    bool testActive_;
    QPalette calibratePalette_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICENEWWRAPPERCALIBRATION_H
