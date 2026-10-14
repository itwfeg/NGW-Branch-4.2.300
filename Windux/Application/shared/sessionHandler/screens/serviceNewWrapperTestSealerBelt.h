/**
 * @file serviceNewWrapperTestSealerBelt.h
 * @brief Widget to test the wrapper's sealer belt.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Alex Zavala
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/
#ifndef SERVICENEWWRAPPERTESTSEALERBELT_H
#define SERVICENEWWRAPPERTESTSEALERBELT_H

#include "screenwidget.h"
#include "deviceTests.h"
#include <QtGui>

class DomCurrentPLU;
class DomEvolWrapperConfig;
class DomEvolWrapperStatus;
class MadpScaleConfig;

namespace Ui {
class serviceNewWrapperTestSealerBelt;
}

/**
* @class serviceNewWrapperTestSealerBelt
* @brief  Class to test the sealer belt on the wrapper
*
*/

class serviceNewWrapperTestSealerBelt  : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperTestSealerBelt _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit serviceNewWrapperTestSealerBelt(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceNewWrapperTestSealerBelt();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets

    /**
    * SLOT method called when btnCycleSealerBelt is pressed
    *
    * @param[in] none
    * @returns  none
    * @brief cycleSealerBelt
    */
    void cycleSealerBelt();

    /**
     * SLOT method called when cboxSealerBeltPower is toggled
     *
     * @brief sealerBeltPower
     * @param index
     * @returns none
     */
    void sealerBeltPower(int index);

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

    /// Retrieve temperature scale.
    /**
     * @return  temperature scale
     */
    virtual QString temperatureScale() const;

    /// Retrieve temperature scale.
    /**
     * @return  temperature given metricness of scale
     */
    virtual QString temperatureValue(int val) const;

private:
    Ui::serviceNewWrapperTestSealerBelt *ui;

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
     * Method for updating the sealer belt power
     *
     * @brief setBeltPower
     * @param power 0/on, 1/off
     */
    void setBeltPower(int power);

    /**
     * Method for updating the sealer belt setting
     *
     * @brief setBeltSetting
     * @param setting temperature setting
     */
    void setBeltSetting(int setting);

    /**
     * Method for updating the sealer belt temperature
     *
     * @brief setBeltTemperature
     * @param temp temperature
     */
    void setBeltTemperature(int temp);

    //The variable to send test messages to the wrapper
    deviceTests* deviceTest;

    //true/false when the filmSensorTest is on/off
    bool sensorTestIsRunning;

    //Access ScaleConfig properties
    MadpScaleConfig *scaleConfigData;
};
#endif // SERVICENEWWRAPPERTESTSEALERBELT_H
