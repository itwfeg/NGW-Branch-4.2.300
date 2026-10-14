/**
 * @file serviceCurrencySetup.h
 * Widget for allowing service to modify Currency Settings.
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
  */

#ifndef SERVICECURRENCYSETUP_H
#define SERVICECURRENCYSETUP_H

#include "screenwidget.h"

namespace Ui {
class serviceCurrencySetup;
}

/**
* @class serviceCurrencySetup
* @brief Change currency display, rounding and precision settings
*  
*/

class serviceCurrencySetup  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceCurrencySetup _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceCurrencySetup(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     * Saves on exit
     */
    virtual ~serviceCurrencySetup();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets


    /**
    * SLOT method called when UI has User interaction and has changed the current values.
    *
    * @param[in] none
    * @returns  none
    */
    void SetupChanged();

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

    Ui::serviceCurrencySetup *ui;
    bool serviceCurrencySetupChanged;
    int weigherSwitchPressed;
    bool allAreDisabled;
    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
    * Method to load UI widgets with currency content from backend.
    *
    * @param[in] msg, The message's content
    * @returns  none
    */
    void setUICurrencySettings(const SessionMessage &msg);

    /**
    * Method to set Weigher Switch var locally.
    *
    * @param[in] none
    * @returns none
    */
    void setSwitchToPressed();

    /**
    * Method to enable all widgets on screen.
    *
    * @param[in] none
    * @returns none
    */
    void enableAll();

    /**
    * Method to decide from backend if Weigher Switch was pressed.
    *
    * @param[in] none
    * @returns  bool
    */
    bool wasWeigherSwitchPressed();

    /**
    * Method to save UI content if UI has been clicked.
    *
    * @param[in] none
    * @returns  none
    */
    void saveOnExit();

};

#endif // SERVICECURRENCYSETUP
