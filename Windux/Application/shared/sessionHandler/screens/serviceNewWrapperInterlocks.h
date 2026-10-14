/**
 * @file ServiceNewWrapperInterlocks.h
 * @brief Widget to view a wrapper's interlocks.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Alex Zavala
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/
#ifndef SERVICENEWWRAPPERINTERLOCKS_H
#define SERVICENEWWRAPPERINTERLOCKS_H

#include "screenwidget.h"

class DomCurrentPLU;
class DomEvolWrapperSensors;
class QLabel;


namespace Ui {
class serviceNewWrapperInterlocks;
}

/**
* @class ServiceNewWrapperInterlocks
* @brief  Updates the Wrapper's Interlocks
*
*/

class serviceNewWrapperInterlocks  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperInterlocks _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperInterlocks(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperInterlocks();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets

//No slots needed for this screen

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
    Ui::serviceNewWrapperInterlocks *ui;

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
     * @param[in] none
     * @returns none
     * @brief setSensorState
     */
    void setSensorState(QLabel* label, bool open);

    /**
     * @brief updateWrapperSensors updates the sensors to their current states
     */
    void updateWrapperSensors();

    DomEvolWrapperSensors *wrapperSensors;
    bool wrapperSensorsMsgReceived;
};

#endif // SERVICENEWWRAPPERINTERLOCKS_H
