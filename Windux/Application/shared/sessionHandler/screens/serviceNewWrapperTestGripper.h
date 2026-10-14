
/**
 * @file serviceNewWrapperTestGripper.h
 * @brief Widget to test the wrapper's gripper.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/
#ifndef SERVICENEWWRAPPERTESTGRIPPER_H
#define SERVICENEWWRAPPERTESTGRIPPER_H
#include "screenwidget.h"
#include "deviceTests.h"

class DomCurrentPLU;
class DomEvolWrapperSensors;
class DomEvolWrapperSystemInformation;
class QLabel;

namespace Ui {
class serviceNewWrapperTestGripper;
}

/**
* @class serviceNewWrapperTestGripper
* @brief  Class to test the gripper on the wrapper
*
*/

class serviceNewWrapperTestGripper  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestGripper _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperTestGripper(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperTestGripper();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets

    /**
     * SLOT method to move the gripper in
     *
     * @brief gripperIn
     */
    void gripperIn();

    /**
     * SLOT method to home the gripper
     *
     * @brief gripperHome
     */
    void gripperHome();

    /**
     * SLOT method to move the gripper out
     *
     * @brief gripperOut
     */
    void gripperOut();

    /**
     * SLOT method to open the narrow gripper
     *
     * @brief narrowGripperOpen
     */
    void narrowGripperOpen();

    /**
     * SLOT method to close the narrow gripper
     *
     * @brief narrowGripperClose
     */
    void narrowGripperClose();

    /**
     * SLOT method to open the wide gripper
     *
     * @brief wideGripperOpen
     */
    void wideGripperOpen();

    /**
     * SLOT method to close the wide gripper
     *
     * @brief wideGripperClose
     */
    void wideGripperClose();

    /**
     * SLOT method to test the upper film selector
     *
     * @brief filmSelectTestUpper
     */
    void filmSelectTestUpper();

    /**
     * SLOT method to test the lower film selector
     *
     * @brief filmSelectTestLower
     */
    void filmSelectTestLower();

    /**
     * SLOT method to cycle the gripper
     *
     * @brief cycleGripper
     */
    void cycleGripper();

    /**
     * SLOT method to finish cycling the gripper
     *
     * @brief finish
     */
    void finish();

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
    Ui::serviceNewWrapperTestGripper *ui;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method that sets/clears the sensor state
     *
     * @brief setSensorState
     * @param label the label whose state is changing
     * @param open 1, if sensor is open and 0 if it is closed
     */
    void setSensorState(QLabel* label, bool open);

    /**
     * Method that disables the finish button
     *
     * @brief disableFinish
     */
    void disableFinish();

    /**
     * Method that enables the finish button
     *
     * @brief enableFinish
     */
    void enableFinish();

    /**
     * Method to disable the lower film tests
     *
     * @brief disableLowerFilm
     */
    void disableLowerFilm();

    /**
     * Method to enable the lower film tests
     *
     * @brief enableLowerFilm
     */
    void enableLowerFilm();

    //The variable to send test messages to the wrapper
    deviceTests* deviceTest;

    //true/false when the gripper is being cycled
    bool gripperIsCycling;
};
#endif // SERVICENEWWRAPPERTESTGRIPPER_H
