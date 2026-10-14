/**
  * @file serviceSupervisorOsAccess.h
  * @brief Screen for enabling Supervisor to access Operating System in order to configure it.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup serviceSupervisorOsAccessCsu
  * @{
  */

#ifndef SERVICESUPERVISOROSACCESS_H
#define SERVICESUPERVISOROSACCESS_H

#include "screenwidget.h"

namespace Ui {
class serviceSupervisorOsAccess;
}

/**
 * @class serviceSupervisorOsAccess
 * @brief Screen for enabling Supervisor to access Operating System in order to configure it.
 */

class serviceSupervisorOsAccess : public ScreenWidget
{
    Q_OBJECT

    typedef serviceSupervisorOsAccess _Myt;
    typedef ScreenWidget _Mybase;
    
public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceSupervisorOsAccess(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceSupervisorOsAccess();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when the checkbox is changed.
     * This setting is saved immediately, not when exiting the screen.
     */
    void SaveCheckboxSetting();

private:

    Ui::serviceSupervisorOsAccess *ui;

    /**
     * Set the checkbox based on the value of rebootUnprotectedAllowed in MadpScaleConfig.
     *
     */
    void initialize();

};

#endif // SERVICESUPERVISOROSACCESS_H
