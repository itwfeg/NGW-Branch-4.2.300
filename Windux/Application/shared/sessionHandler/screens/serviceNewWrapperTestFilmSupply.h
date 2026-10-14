
/**
 * @file serviceNewWrapperTestFilmSupply.h
 * @brief Widget to test the wrapper's film supply.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Alex Zavala
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/
#ifndef SERVICENEWWRAPPERTESTFILMSUPPLY_H
#define SERVICENEWWRAPPERTESTFILMSUPPLY_H
#include "screenwidget.h"
#include "deviceTests.h"

class DomCurrentPLU;
class DomEvolWrapperSensors;
class DomEvolWrapperSystemInformation;
class QLabel;

namespace Ui {
class serviceNewWrapperTestFilmSupply;
}

/**
* @class serviceNewWrapperTestFilmSupply
* @brief  Class to test the film supply on the wrapper
*
*/

class serviceNewWrapperTestFilmSupply  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestFilmSupply _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperTestFilmSupply(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperTestFilmSupply();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets

    /**
    * SLOT method called when btnSelectorPositionUpper is pressed
    *
    * @param[in] none
    * @returns  none
    * @brief selectorPositionUpper
    */
    void selectorPositionUpper();

    /**
     * SLOT method called when btnSelectorPositionLower is pressed
     *
     * @param[in] none
     * @returns none
     * @brief selectorPositionLower
     */
    void selectorPositionLower();

    /**
     * SLOT method called when btnUpperBrakeOn is pressed
     *
     * @param[in] none
     * @returns none
     * @brief upperBrakeOn
     */
    void upperBrakeOn();

    /**
     * SLOT method called when btnUpperBrakeOff is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief upperBrakeOff
     */
    void upperBrakeOff();

    /**
     * SLOT method called when btnLowerBrakeOn is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief lowerBrakeOn
     */
    void lowerBrakeOn();

    /**
     * SLOT method called when btnLowerBrakeOff is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief lowerBrakeOff
     */
    void lowerBrakeOff();

    /**
     * SLOT method called when btnFilmSensorTestOff is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief filmSensorTestOn
     */
    void filmSensorTestOn();

    /**
     * SLOT method called when btnFilmSensorTestOn is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief filmSensorTestOff
     */
    void filmSensorTestOff();

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

private:
    Ui::serviceNewWrapperTestFilmSupply *ui;

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
     * Method that sets/clears the sensor state
     *
     * @brief setSensorState
     * @param label the label whose state is changing
     * @param open 1, if sensor is open and 0 if it is closed
     */
    void setSensorState(QLabel* label, bool open);

    /**
     * Method that disables the Wide Pegs buttons
     *
     * @param[in] none
     * @returns  none
     * @brief disableLowerBrakes
     */
    void disableLowerBrakes();

    /**
     * Method that enables the Wide Pegs buttons
     *
     * @param[in] none
     * @returns  none
     * @brief enableLowerBrakes
     */
    void enableLowerBrakes();

    /**
     * Method that disables the Selector Position buttons
     *
     * @param[in] none
     * @returns  none
     * @brief disableSelectorPosition
     */
    void disableSelectorPosition();

    /**
     * Method that enables the Selector Position buttons
     *
     * @param[in] none
     * @returns  none
     * @brief enableSelectorPosition
     */
    void enableSelectorPosition();

    //The variable to send test messages to the wrapper
    deviceTests* deviceTest;

    //true/false when the filmSensorTest is on/off
    bool sensorTestIsRunning;

};

#endif // SERVICENEWWRAPPERTESTFILMSUPPLY_H
