/**
 * @file upgradeAccessCLA.h
 * @brief This is the screen that shows you what is upgrading on the CLA and its progress
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala
 * @addtogroup upgradeAccessCLACsu
 * @{
*/
#ifndef UPGRADEACCESSCLA_H
#define UPGRADEACCESSCLA_H

#include "screenwidget.h"

class QLabel;

namespace Ui {
class upgradeAccessCLA;
}

/**
* @class upgradeAccessCLA
* @brief Upgrades and displays progress in the table and bytes sent
*/

class upgradeAccessCLA  : public ScreenWidget
{
    Q_OBJECT

    typedef upgradeAccessCLA _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
      */
    explicit upgradeAccessCLA(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~upgradeAccessCLA();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    typedef enum CLATypes_t
    {
        CLAAppSW    = 71,
        CLADataSW   = 72,
        CLABootSW   = 73
    } CLATypes;

private slots:

    /**
     * SLOT method that returns to the previous location after the upgrade is done
     *
     * @param[in] none
     * @returns  none
     */
    void finish();

    /**
     * Slot for when screen changes.
     */
    void onScreenChanged();

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
    Ui::upgradeAccessCLA *ui;
    bool upgradeInProgress;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * @brief updateBytesSent updates the bytes sent label in the ui
     * @param bytes the number of bytes
     */
    void updateBytesSent(int bytes);

    /**
     * @brief updateTotalBytes updates the total bytes label in the ui
     * @param bytes the number of bytes
     */
    void updateTotalBytes(int bytes);

    /**
     * @brief returnToHome returns the console to the home screen
     */
    void returnToHome();

    /**
     * @brief updateNodeState updates whether the node upgrade is in progress, pending, etc.
     * @param label the node's label in the ui
     * @param status the status of the node
     */
    void updateNodeState(QLabel* label, int status);

    /**
     * @brief updateNodeState wrapper function of updateNodeState
     * @param node the node to be upgraded
     * @param status the status of the node to be upgraded
     */
    void updateNodeState(int node, int status);

    /**
     * @brief enableDoneButton enables the done button
     */
    void enableDoneButton();

    /**
     * @brief disableDoneButton disables the done button
     */
    void disableDoneButton();

    /**
     * @brief refreshUpgradeStates refreshes the labels to their current states
     */
    void refreshUpgradeStates();

    typedef enum nodeState_t
    {
        MT_START    = 0,
        MT_FAILED   = 1,
        MT_SUCCESS  = 2,
        MT_PENDING  = 3,
        MT_ABORTED  = 4
    } nodeState;

    //Variables to keep track of the cla upgrade status for each node
    int appStatus;
    int dataStatus;
    int bootStatus;

};
#endif // UPGRADEACCESSCLA_H
