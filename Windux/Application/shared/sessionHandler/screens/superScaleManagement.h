/**
  * @file superScaleManagement.h
  * @brief Allows all performed transactions to be cleared when a clear totals command is sent through a communcations package.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup superScaleManagementCsu
  * @{
  */

#ifndef SUPERSCALEMANAGEMENT_H
#define SUPERSCALEMANAGEMENT_H

#include "screenwidget.h"

namespace Ui {
class superScaleManagement;
}

/**
 * @class superScaleManagement
 * @brief Set how to handle old labels or what can change in a product during a comm session.
 *
 * Settings include:
 *
 * Ignore Ultima/Quantum labels
 * Don't change Current COOL in Products
 */

class superScaleManagement : public ScreenWidget
{
    Q_OBJECT

    typedef superScaleManagement _Myt;
    typedef ScreenWidget _Mybase;

    
public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit superScaleManagement(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superScaleManagement();

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
    Ui::superScaleManagement *ui;

    bool bSetupChanged;

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveDataOnExit();

};

#endif // SUPERSCALEMANAGEMENT_H
