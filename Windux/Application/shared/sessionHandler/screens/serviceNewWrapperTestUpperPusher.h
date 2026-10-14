/**
 * @file serviceNewWrapperTestUpperPusher.h
 * @brief Widget to test the wrapper's upper pusher.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Alex Zavala
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/
#ifndef SERVICENEWWRAPPERTESTUPPERPUSHER_H
#define SERVICENEWWRAPPERTESTUPPERPUSHER_H

#include "screenwidget.h"
#include "deviceTests.h"

class DomCurrentPLU;
class DomEvolWrapperSensors;
class DomEvolWrapperSystemInformation;
class QLabel;

namespace Ui {
class serviceNewWrapperTestUpperPusher;
}

/**
* @class serviceNewWrapperTestUpperPusher
* @brief  Class to test the upper pusher on the wrapper
*
*/

class serviceNewWrapperTestUpperPusher  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestUpperPusher _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperTestUpperPusher(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperTestUpperPusher();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets

    /**
    * SLOT method called when btnCycleUpperPusher is pressed
    *
    * @param[in] none
    * @returns  none
    * @brief cycleUpperPusher
    */
    void cycleUpperPusher();

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
    Ui::serviceNewWrapperTestUpperPusher *ui;

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

    //The variable to send test messages to the wrapper
    deviceTests* deviceTest;
};
#endif // SERVICENEWWRAPPERTESTUPPERPUSHER_H
