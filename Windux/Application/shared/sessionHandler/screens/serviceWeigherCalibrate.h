/**
  * @file serviceWeigherCalibrate.h
  * @brief Screen with Weigher Calibrate widget in it.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup serviceWeigherCalibrateCsu
  * @{
  */


#ifndef SERVICEWEIGHERCALIBRATE_H
#define SERVICEWEIGHERCALIBRATE_H

#include "screenwidget.h"

class SessionMainWindow;
class MadpInternalConfig;

namespace hobart {
class weigherCalibrate;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class serviceWeigherCalibrate
 * @brief Screen with Weigher Calibrate widget in it.
 */

class serviceWeigherCalibrate : public ScreenWidget
{
    Q_OBJECT

    typedef serviceWeigherCalibrate _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceWeigherCalibrate( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );


    /**
     * Destructor.
     */
    virtual ~serviceWeigherCalibrate();


    /**
     * Translate the user interface to current language.
     */
    // FT-695
    virtual void translate();


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

    hobart::weigherCalibrate *weigherCalibrateView_;

    //Session *session_;
    //SessionMainWindow *mainWindow_;
    MadpInternalConfig *internalConfig_;

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICEWEIGHERCALIBRATE_H
