/**
 * @file serviceNewWrapperTestIntake.h
 * @brief Widget to view Wrapper Test Intake.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SERVICENEWWRAPPERTESTINTAKE_H
#define SERVICENEWWRAPPERTESTINTAKE_H

#include "screenwidget.h"
#include "deviceTests.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

class QIntValidator;
class QLabel;
class QTimer;

namespace Ui {
class serviceNewWrapperTestIntake;
}

/**
* @class serviceNewWrapperTestIntake
* @brief  Shows wrapper test intake
*
*/

class serviceNewWrapperTestIntake : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestIntake _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperTestIntake(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperTestIntake();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets


    /**
    * SLOT method called when product or class change to signal a save on close of form
    *
    * @param[in] none
    * @returns  none
    */
    void doSave();

    void butPrimaryMotorCycleClicked();
    void butSecondaryMotorCycleClicked();
    void butCycleIntakeClicked();
    void butCycleWrapperClicked();
    void butHeightSensorsOffClicked();
    void butHeightSensorsOnClicked();
    void butShifterHomeClicked();
    void butShifterLeftClicked();
    void butShifterRightClicked();

    /// Slot to request camera tracking.
    void requestCameraTracking();

protected:
    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /**
     * Check for specific error message is handled by screen.
     *
     * Session will call this method upon receiving an error message. The default error handling
     * (i.e. popup an error dialog) shall be invoked unless @c true is returned from
     * this property.
     *
     * @returns  @c true to use custom error handler, @c false otherwise
     */
    virtual bool isErrorMessageHandler( const SessionMessage& errorMsg ) const;

    virtual bool eventFilter( QObject *obj, QEvent *event );
private:
    Ui::serviceNewWrapperTestIntake *ui;
    MadpInternalConfig *internalConfigData;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();
    void updateEvolWrapperSensors(const SessionMessage& msg);
    void updateEvolWrapperPkgDescription(const SessionMessage& msg);
    void updateCameraHeightScan(const SessionMessage& msg);
    void redTheLabel(QLabel *lbl);
    void greenTheLabel(QLabel *lbl);
    void greyTheLabel(QLabel *lbl);
    void displaySensorState(QLabel *lbl, const QString &state);
    void saveDataOnExit();
    void updateSystemInfo(const SessionMessage& msg);

    int makeValidatorTop(int digits);

    MadpScaleConfig *scaleConfigData;
    DomEvolWrapperConfig *evolWrapperConfigData;
    DomEvolWrapperSystemInformation *evolWrapperSystemInformation;

    QTimer *requestCameraTracking_;

    QPalette normalBackground_;
    QPalette greenBackground_;
    QPalette redBackground_;

    DomEvolWrapperCameraHeightScan prevScan_;

    bool hasChanged;
	bool heightSensorsOn;

    QString scaleModel;
    QString isScaleMetric;
    deviceTests* deviceTest;

};

#endif // SERVICENEWWRAPPERTESTINTAKE_H
