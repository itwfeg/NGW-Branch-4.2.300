/**
  * @file servicePricingMode.h
  * @brief Allows for allowing Pricing Mode to be set.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup servicePricingModeCsu
  * @{
  */

#ifndef SERVICEPRICINGMODE_H
#define SERVICEPRICINGMODE_H

#include "screenwidget.h"

namespace Ui {
class servicePricingMode;
}

/**
 * @class servicePricingMode
 * @brief Allows for allowing Pricing Mode to be set.
 *
 * Selections are:  Allow Pounds for Pricing and Allow Fractional Pricing.
 *
 *                  When the scale is set up for Pounds, both
 *                  "Pounds for Pricing" and "Allow Fractional Pricing"
 *                  options are visible.  Only one checkbox is allowed to
 *                  be checked at a time.
 *
 *                  When the scale is set up for Kilograms (i.e. metric),
 *                  only the "Allow Fractional Pricing" checkbox is visible.
*/



class servicePricingMode : public ScreenWidget
{
    Q_OBJECT

    typedef servicePricingMode _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit servicePricingMode(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~servicePricingMode();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when a field is changed.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChanged();

    /**
     * Slot for when "Allow Pounds for Pricing" checkbox is changed.
     * Need to enable/disable (and clear) "Allow Fractional Pricing"
     */
    void PoundsForPricingChanged();

    /**
    * Slot for when "Allow Fractional Pricing" is changed.
     * Need to enable/disable (and clear) "Allow Pounds for Pricing"
    */
    void FractionalPricingChanged();

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

private:
    Ui::servicePricingMode *ui;

    bool bSetupChanged;

    bool bIsMetricScale;


    /**
     * Set the checkboxes based on the values for poundsForPricingOn and fractionalPricingOn in MadpScaleConfig.
     *
     */
    void initialize();

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveDataOnExit();

};

#endif // SERVICEPRICINGMODE_H

