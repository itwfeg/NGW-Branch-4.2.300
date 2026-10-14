/**
  * @file serviceRemoveLabels.h
  * @brief Allows for removing selected labels from use.
  *
  * @copyright Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
  * @author Joe Crew
  * @addtogroup serviceRemoveLabelsCsu
  * @{
  */

#ifndef SERVICEREMOVELABELS_H
#define SERVICEREMOVELABELS_H

#include "screenwidget.h"

namespace Ui {
class serviceRemoveLabels;
}

/**
 * @class serviceRemoveLabels
 * @brief Allows for removing labels from use on a scale due to sizing issues..
 *
 * Selections are:  Each size of label installed on the scale.
 *
*/



class serviceRemoveLabels : public ScreenWidget
{
    Q_OBJECT

    typedef serviceRemoveLabels _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceRemoveLabels(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceRemoveLabels();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

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
    Ui::serviceRemoveLabels *ui;

    /**
     * Set the checkboxes based on the values for poundsForPricingOn and fractionalPricingOn in MadpScaleConfig.
     *
     */
    void updateCheckboxes(const SessionMessage &msg);

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void saveDataOnExit();

    bool isUpdating;
};

#endif // SERVICEPRICINGMODE_H
