/**
 * @file serviceLabelApplierTests.h
 * @brief Widget to test the standalone label applier.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Alex Zavala
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/
#ifndef SERVICELABELAPPLIERTESTS_H
#define SERVICELABELAPPLIERTESTS_H
#include "screenwidget.h"
#include "deviceTests.h"
#include <QtGui>

class DomCurrentPLU;

namespace Ui {
class serviceLabelApplierTests;
}

/**
* @class serviceLabelApplierTests
* @brief  Class to test the standalone label applier
*
*/

class serviceLabelApplierTests  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceLabelApplierTests _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceLabelApplierTests(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceLabelApplierTests();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets

    /**
    * SLOT method that cycles the motors
    *
    * @param[in] none
    * @returns  none
    * @brief cycleMotor
    */
    void cycleMotor();

    /**
     * SLOT method that puts the label applier belts up
     *
     * @param[in] none
     * @returns none
     * @brief beltsUp
     */
    void beltsUp();

    /**
     * SLOT method that puts the label applier belts down
     *
     * @param[in] none
     * @returns none
     * @brief beltsDown
     */
    void beltsDown();

    /**
     * SLOT method that puts the wand up
     *
     * @param[in] none
     * @returns  none
     * @brief wandUp
     */
    void wandUp();

    /**
     * SLOT method that puts the wand down
     *
     * @param[in] none
     * @returns  none
     * @brief wandDown
     */
    void wandDown();

    /**
     * SLOT method that puts the applier up
     *
     * @param[in] none
     * @returns  none
     * @brief applierUp
     */
    void applierUp();

    /**
     * SLOT method that puts the applier down
     *
     * @param[in] none
     * @returns  none
     * @brief applierDown
     */
    void applierDown();

    /**
     * SLOT method that puts the gate up
     *
     * @param[in] none
     * @returns  none
     * @brief gateUp
     */
    void gateUp();

    /**
     * SLOT method that puts the gate down
     *
     * @param[in] none
     * @returns  none
     * @brief gateDown
     */
    void gateDown();

    /**
     * SLOT method that turns the compressor on
     *
     * @param[in] none
     * @returns  none
     * @brief compressorOn
     */
    void compressorOn();

    /**
     * SLOT method that turns the compressor off
     *
     * @param[in] none
     * @returns  none
     * @brief compressorOff
     */
    void compressorOff();

    /**
     * SLOT method that turns the wand vacuum on
     *
     * @param[in] none
     * @returns  none
     * @brief vacuumOn
     */
    void vacuumOn();

    /**
     * SLOT method that turns the wand vacuum off
     *
     * @param[in] none
     * @returns  none
     * @brief vacuumOff
     */
    void vacuumOff();

    /**
     * SLOT method that puts the label applier in mfg. run in
     *
     * @param[in] none
     * @returns  none
     * @brief runInOn
     */
    void runInOn();

    /**
     * SLOT method that turns off label applier mfg. run in
     *
     * @param[in] none
     * @returns  none
     * @brief runInOff
     */
    void runInOff();

    /**
     * SLOT method for changing the label applier type
     *
     * @brief labelApplierType
     * @param index 0/Primary Label Applier, 1/Weigher Conveyor
     * @returns  none
     */
    void labelApplierType(int index);

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
    Ui::serviceLabelApplierTests *ui;

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
     * Method that loads the weigher conveyor tests
     *
     * @param[in] none
     * @returns  none
     * @brief loadWeigherConveyorTests
     */
    void loadWeigherConveyorTests();

    /**
     * Method that loads that label applier tests
     *
     * @param[in] none
     * @returns  none
     * @brief loadLabelApplierTests
     */
    void loadLabelApplierTests();

    /**
     * Method that makes the label applier test names visible
     *
     * @param[in] none
     * @returns  none
     * @brief showLabelApplierTestLabels
     */
    void showLabelApplierTestLabels();

    /**
     * Method that makes the label applier test buttons visible
     *
     * @param[in] none
     * @returns  none
     * @brief showLabelApplierTestButtons
     */
    void showLabelApplierTestButtons();

    /**
     * Method that enables the label applier test buttons
     *
     * @param[in] none
     * @returns  none
     * @brief enableLabelApplierTestButtons
     */
    void enableLabelApplierTestButtons();

    /**
     * Method that hides the label applier's test names
     *
     * @param[in] none
     * @returns  none
     * @brief hideLabelApplierTestLabels
     */
    void hideLabelApplierTestLabels();

    /**
     * Method that hides the label applier's test buttons
     *
     * @param[in] none
     * @returns  none
     * @brief hideLabelApplierTestButtons
     */
    void hideLabelApplierTestButtons();

    /**
     * Method that disables the label applier test buttons
     *
     * @param[in] none
     * @returns  none
     * @brief disableLabelApplierTestButtons
     */
    void disableLabelApplierTestButtons();

    int getCurrentStation();

    //Enum for the stations allowed in this test(Secondary Applier not supported)
    typedef enum station
    {
        WeigherConveyor         = 0,
        PrimaryLabelApplier     = 1,
        SecondaryLabelApplier   = 2
    } station_et;

    //The variable to send test messages to the wrapper
    deviceTests* deviceTest;

    //true/false when the cboxLabelApplier's index is 0/1
    bool isLabelApplier;

    //true/false if we are in mfg. run in
    bool inMfgRunIn;
};
#endif // SERVICELABELAPPLIERTESTS_H
