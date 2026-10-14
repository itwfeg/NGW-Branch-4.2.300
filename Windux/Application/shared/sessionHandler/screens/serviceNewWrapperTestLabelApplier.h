/**
 * @file serviceNewWrapperTestLabelApplier.h
 * @brief Widget to test the wrapper's label applier.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Alex Zavala
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/
#ifndef SERVICENEWWRAPPERTESTLABELAPPLIER_H
#define SERVICENEWWRAPPERTESTLABELAPPLIER_H

#include "screenwidget.h"
#include "deviceTests.h"
#include <QTimer>

class DomCurrentPLU;
class DomEvolWrapperSensors;
class DomEvolWrapperSystemInformation;
class DomEvolWrapperConfig;
class QLabel;

namespace Ui {
class serviceNewWrapperTestLabelApplier;
}

/**
* @class serviceNewWrapperTestLabelApplier
* @brief  Class to test the label applier on the wrapper
*
*/

class serviceNewWrapperTestLabelApplier  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestLabelApplier _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperTestLabelApplier(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperTestLabelApplier();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets

    /**
    * SLOT method called when btnWandUp is pressed
    *
    * @param[in] none
    * @returns  none
    * @brief wandUp
    */
    void wandUp();

    /**
     * SLOT method called when btnWandDown is pressed
     *
     * @param[in] none
     * @returns none
     * @brief wandDown
     */
    void wandDown();

    /**
     * SLOT method called when btnApplierUp is pressed
     *
     * @param[in] none
     * @returns none
     * @brief applierUp
     */
    void applierUp();

    /**
     * SLOT method called when btnApplierDown is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief applierDown
     */
    void applierDown();

    /**
     * SLOT method called when btnVacuumOn is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief vacuumOn
     */
    void vacuumOn();

    /**
     * SLOT method called when btnVacuumOff is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief vacuumOff
     */
    void vacuumOff();

    /**
     * SLOT method called when btnPrintLabel is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief print
     */
    void print();

    /**
     * SLOT method called when btnPrintAndApplyLabel is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief printAndApply
     */
    void printAndApply();

    /**
     * SLOT method called when Slide Applier Out is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief sldieApplierOut
     */
    void slideApplierOut();

    /**
     * SLOT method called when Slide Applier Home is pressed
     *
     * @param[in] none
     * @returns  none
     * @brief sldieApplierHome
     */
    void slideApplierHome();

    /**
     * SLOT method called when a selection is made in cboxLabelRotation
     *
     * @brief updateLabelRotation
     * @returns none
     */
    void updateLabelRotation();

    /**
     * SLOT method called to change between primary and secondary label appliers
     *
     * @brief labelApplierType
     * @param index the selection in cboxLabelApplier
     */
    void labelApplierType(int index);

    /**
     * SLOT method called when the 2nd label applier returns home
     *
     * @brief onSlideHomeTimeout
     * @returns none
     */
    void onSlideHomeTimeout();

    /**
     * SLOT method called when the 2nd label applier slides out
     *
     * @brief onSlideOutTimeout
     * @returns none
     */
    void onSlideOutTimeout();

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
    Ui::serviceNewWrapperTestLabelApplier *ui;

    enum LA_TEST_PARAMETER
    {
          WAND_UP,
          WAND_DOWN,
          APPLIER_UP,
          APPLIER_DOWN,
          VACCUM_ON,
          VACCUM_OFF,
          LA_CYCLE,
          CYCLE_2ND_APPLIER_MOTOR_OUT,          /* moves worm gear motor out 8" */
          CYCLE_2ND_APPLIER_MOTOR_IN            /* moves worm gear motor to home position */
    };

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
     * Method that sets the label applier type and cbox
     *
     * @brief setLabelApplierType
     * @param labelApplierType 0:Primary, 1:Secondary
     */
    void setLabelApplierType(int labelApplierType);

    /**
     * Method that enables the ability to change the label rotation
     *
     * @param[in] none
     * @returns  none
     * @brief enableRotation
     */
    void enableRotation();

    /**
     * Method that disables the ability to change the label rotation
     *
     * @param[in] none
     * @returns  none
     * @brief disableRotation
     */
    void disableRotation();

    /**
     * Method that enables the ability to change between primary and secondary label applier
     *
     * @param[in] none
     * @returns  none
     * @brief enableLabelApplierType
     */
    void enableLabelApplierType();

    /**
     * Method that disables the ability to change between primary and secondary label applier
     *
     * @param[in] none
     * @returns  none
     * @brief disableLabelApplierType
     */
    void disableLabelApplierType();

    //The variable to send test messages to the wrapper
    deviceTests* deviceTest;

    //Boolean to check which label applier is being tested
    bool isPrimaryLabelApplier;

    QTimer *slideOutTimer_;
    QTimer *slideHomeTimer_;
};
#endif // SERVICENEWWRAPPERTESTLABELAPPLIER_H
