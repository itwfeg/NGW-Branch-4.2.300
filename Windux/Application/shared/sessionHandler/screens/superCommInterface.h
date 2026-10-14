/**
  * @file superCommInterface.h
  * @brief Allows all performed transactions to be cleared when a clear totals command is sent through a communcations package.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup superCommInterfaceCsu
  * @{
  */

#ifndef SUPERCOMMINTERFACE_H
#define SUPERCOMMINTERFACE_H

#include "screenwidget.h"

namespace Ui {
class superCommInterface;
}

/**
 * @class superCommInterface
 * @brief Allows all performed transactions to be cleared when a clear totals command is sent through a communcations package.
 *
 * Settings include:
 *
 * Clear operator totals
 * Clear operator changes
 * Clear hourly totals
 * Clear product and class totals
 * Clear COOL totals
 * Clear Ignore Ultima Labels
 */

class superCommInterface : public ScreenWidget
{
    Q_OBJECT

    typedef superCommInterface _Myt;
    typedef ScreenWidget _Mybase;

    
public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit superCommInterface(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superCommInterface();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when a checkbox is checked/un-checked.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChanged();

    /**
     * Slot for when a specific COOL checkbox is checked/un-checked.
     * Sets property indicating that the COOL setup needs to be saved
     * when exiting.
     */
    void CoolSetupChanged();

    /**
     * Slot for detecting Ignore Ultima Labels is checked/un-checked.
     * Sets property indicating that the Ignore Ultima Labels needs to be saved
     * when exiting.
     */
    void IgnoreUltimaLabelsChanged();

    /**
     * SLOT toggle schedule use
     * */
    void toggleUseAutomatic( bool isChecked );

    void manageStartTimeBox( bool isChecked );
    void manageNoStartTimeBox( bool isChecked );

    /**
    * SLOT method called when "Scheduled Start AM/PM button is clicked to toggle text.
    *
      * @param[in] none
      * @returns  none
    */
    void toggleScheduledStartAmPm();

    /**
    * SLOT method called when "Automatically update using the schedule below" checkbox is selected or deselected,
    * or when the chkDaily radio button for choosing a specific time of day is clicked or un-clicked.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedEnableTimeOfDay( int state );

    /**
    * SLOT method called when the "Merge Now" button is clicked.
    * @param[in] none
    * @returns  none
    */
    void mergeNowClicked();

    /**
    * SLOT method called when the "View Files" button is clicked.
    * @param[in] none
    * @returns  none
    */
    void viewFilesButtonClicked();


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
    Ui::superCommInterface *ui;

    bool bSetupChanged;
    bool bCoolSetupChanged;
    bool bIgnoreUltimaLabels;

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveDataOnExit();

    QIntValidator *scheduledStartHourValidator_;
    QIntValidator *scheduledStartMinuteValidator_;
    void setupValidators();

    bool isScheduledStartAmSelected;
};

#endif // SUPERCOMMINTERFACE_H
