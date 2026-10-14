/**
 * @file upgradeAccessWrapper.h
 * @brief This is the screen that shows you what is upgrading on the Wrapper and its progress
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala
 * @addtogroup upgradeAccessWrapperCsu
 * @{
*/
#ifndef UPGRADEACCESSWRAPPER_H
#define UPGRADEACCESSWRAPPER_H

#include "screenwidget.h"

class QLabel;

namespace Ui {
class upgradeAccessWrapper;
}

/**
* @class upgradeAccessWrapper
* @brief Upgrades and displays progress in the table and bytes sent
*/

class upgradeAccessWrapper  : public ScreenWidget
{
    Q_OBJECT

    typedef upgradeAccessWrapper _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
      */
    explicit upgradeAccessWrapper(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~upgradeAccessWrapper();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    typedef enum WrapperTypes_t
    {
        WRMasterNVDataSW    = 26,   //AWS
        WRMasterBootSW      = 27,
        WRMasterAppSW       = 28,
        WRSlave1NVDataSW    = 29,
        WRSlave1BootSW      = 30,
        WRSlave1AppSW       = 31,
        WRSlave3NVDataSW    = 32,
        WRSlave3BootSW      = 33,
        WRSlave3AppSW       = 34,
        WRSlave4NVDataSW    = 35,
        WRSlave4BootSW      = 36,
        WRSlave4AppSW       = 37,
        WRSlave5NVDataSW    = 38,
        WRSlave5BootSW      = 39,
        WRSlave5AppSW       = 40,
        WRSlave6NVDataSW    = 41,
        WRSlave6BootSW      = 42,
        WRSlave6AppSW       = 43,
        WREMasterNVDataSW   = 52,    //EWS
        WREMasterBootSW     = 53,
        WREMasterAppSW      = 54,
        WRESlave1NVDataSW   = 55,
        WRESlave1BootSW     = 56,
        WRESlave1AppSW      = 57,
        WRESlave3NVDataSW   = 58,
        WRESlave3BootSW     = 59,
        WRESlave3AppSW      = 60,
        WRESlave4NVDataSW   = 61,
        WRESlave4BootSW     = 62,
        WRESlave4AppSW      = 63,
        WRESlave5NVDataSW   = 64,
        WRESlave5BootSW     = 65,
        WRESlave5AppSW      = 66
    } WrapperTypes;

private slots:

    /**
     * SLOT method that returns to the previous location after the upgrade is done
     *
     * @param[in] none
     * @returns  none
     */
    void finish();

    /**
     * SLOT method that tells the user to reset the wrapper after an upgrade
     *
     * @param[in] none
     * @returns  none
     */
    void okPressed();

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
    Ui::upgradeAccessWrapper *ui;
    bool upgradeInProgress;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * @brief updateBytesSent updates the bytes sent label
     * @param bytes the number of bytes
     */
    void updateBytesSent(int bytes);

    /**
     * @brief updateTotalBytes updates the total bytes label
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
     * @param status the status of the node to be upgraded
     */
    void updateNodeState(QLabel* label, int status);

    /**
     * @brief updateNodeState wrapper function for updateNodeState
     * @param node the node to be upgraded
     * @param status the status of the node to be upgraded
     */
    void updateNodeState(int node, int status);

    /**
     * @brief enableDoneButton Enables the done button
     */
    void enableDoneButton();

    /**
     * @brief disableDoneButton Disables the done button
     */
    void disableDoneButton();

    /**
     * @brief showDialog shows the dialog box
     */
    void showDialog();

    /**
     * @brief hideDialog Hides the dialog box
     */
    void hideDialog();

    /**
     * @brief refreshUpgradeStates displays upgrade states in the new language
     */
    void refreshUpgradeStates();

    /**
     * @brief initializeWrapperUpgradeStatus initializes wrapperUpgradeStatus
     */
    void initializeWrapperUpgradeStatus();

    typedef enum nodeState_t
    {
        MT_START    = 0,
        MT_FAILED   = 1,
        MT_SUCCESS  = 2,
        MT_PENDING  = 3,
        MT_ABORTED  = 4
    } nodeState;

    typedef struct UpgradeStatus_t {
        int masterAppStatus;
        int masterDataStatus;
        int masterBootStatus;

        int slave1AppStatus;
        int slave1DataStatus;
        int slave1BootStatus;

        int slave3AppStatus;
        int slave3DataStatus;
        int slave3BootStatus;

        int slave4AppStatus;
        int slave4DataStatus;
        int slave4BootStatus;

        int slave5AppStatus;
        int slave5DataStatus;
        int slave5BootStatus;

        int slave6AppStatus;
        int slave6DataStatus;
        int slave6BootStatus;
    } UpgradeStatus;

    UpgradeStatus wrapperUpgradeStatus;

};
#endif // UPGRADEACCESSWRAPPER_H
