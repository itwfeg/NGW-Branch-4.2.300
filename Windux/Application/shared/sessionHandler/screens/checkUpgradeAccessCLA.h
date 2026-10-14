/**
 * @file checkUpgradeAccessCLA.h
 * @brief This is the screen that first pops up when there is a discrepancy
 *  between what firmware is available on the Access and what is on the CLA.
 * This checks whether you want to upgrade to what is installed or not.
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala
 * @addtogroup checkUpgradeAccessCLACsu
 * @{
*/
#ifndef CHECKUPGRADEACCESSCLA_H
#define CHECKUPGRADEACCESSCLA_H

#include "screenwidget.h"

namespace Ui {
class checkUpgradeAccessCLA;
}

/**
* @class checkUpgradeAccessCLA
* @brief checks for upgrades and displays the versions in a table
*/

class checkUpgradeAccessCLA  : public ScreenWidget
{
    Q_OBJECT

    typedef checkUpgradeAccessCLA _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
      */
    explicit checkUpgradeAccessCLA(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~checkUpgradeAccessCLA();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
    * SLOT method to start the upgrade
    *
    * @param[in] none
    * @returns  none
    */
    void beginUpgrade();

    /**
    * SLOT method to cancel the upgrade
    *
    * @param[in] none
    * @returns  none
    */
    void cancelUpgrade();

    /**
     * Slot for when screen changes.
     */
    void onScreenChanged();

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
    Ui::checkUpgradeAccessCLA *ui;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * @brief updateTable updates the version table
     * @param components a csv QString of components
     * @param currentVersions a csv QString of current versions of the components
     * @param newVersions a csv QString of the new versions of the components
     * @param pending a csv QString of the status of the components
     * @param numOfItems the number of components in the list
     */
    void updateTable(QString components, QString currentVersions, QString newVersions, QString pending, int numOfItems);

    /**
     * @brief loadUpgradePage loads the upgrade page for the CLA
     */
    void loadUpgradePage();

    /**
     * @brief parseList returns a list of the comma separated values in the QString
     * @param list an unparsed QString of comma separated values
     * @return a list of QStrings that were comma separated
     */
    QStringList parseList(QString list);

    typedef enum versionType_t
    {
        App     = 0,
        Data    = 1,
        Boot    = 2
    } version_t;

    /**
      * Method to request data for dialog box such as the date range
      *
      * @param[in] none
      * @returns none
      */
    void initDialog();

    //Keeps track of whether your leaving screen to start the upgrade or via the menu
    bool switchScreen;
};
#endif // CHECKUPGRADEACCESSCLA_H
