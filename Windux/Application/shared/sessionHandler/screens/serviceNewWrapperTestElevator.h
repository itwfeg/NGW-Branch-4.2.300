/**
 * @file serviceNewWrapperTestElevator.h
 * @brief Widget to test the wrapper's elevator.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Alex Zavala
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/
#ifndef SERVICENEWWRAPPERTESTELEVATOR_H
#define SERVICENEWWRAPPERTESTELEVATOR_H

#include "screenwidget.h"
#include "deviceTests.h"

class DomCurrentPLU;
class DomEvolWrapperSensors;
class DomEvolWrapperSystemInformation;
class QLabel;


namespace Ui {
class serviceNewWrapperTestElevator;
}

/**
* @class serviceNewWrapperTestElevator
* @brief  Class to test the elevator on the wrapper
*
*/

class serviceNewWrapperTestElevator  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestElevator _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperTestElevator(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperTestElevator();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets

    /**
    * SLOT method called when btnElevatorCycle is pressed
    *
    * @param[in] none
    * @returns  none
    */
    void cycleElevator();

protected:
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

private:
    Ui::serviceNewWrapperTestElevator *ui;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to reload widget's fields with new values.
     *
     * @param[in] none
     * @returns  none
     */
    void reloadFields();

    /**
      Method that sets/clears the elevator sensor state
     * @brief setElevatorSensorState
     * @param label the label whose state is changing
     * @param open 1, if sensor is blocked, 0 if it isn't
     */
    void setSensorState(QLabel* label, bool open);

    //The variable to send test messages to the wrapper
    deviceTests* deviceTest;
};
#endif // SERVICENEWWRAPPERTESTELEVATOR_H
