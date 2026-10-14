/**
 * @file superUpgradeFirmwareViaSFTP.h
 * @brief Widget to set the information required to upgrade the scale's firmware/software to a version that is not the same as the current version.
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SUPERUPGRADEFIRMWAREVIASFTP_H
#define SUPERUPGRADEFIRMWAREVIASFTP_H

#include "screenwidget.h"
#include <Messages.h>
#include <QProcess>
#include <QFileSystemWatcher>

class QIntValidator;

namespace Ui {
class superUpgradeFirmwareViaSFTP;
}

/**
* @class superUpgradeFirmwareViaSFTP
* @brief  Changes a product's reporting class
*
*/

class superUpgradeFirmwareViaSFTP  : public ScreenWidget
{
    Q_OBJECT

    typedef superUpgradeFirmwareViaSFTP _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit superUpgradeFirmwareViaSFTP(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superUpgradeFirmwareViaSFTP();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
     * SLOT to improve existing HLX code by preventing spaces in hostnames
     *
     * @param[in] none
     * @returns  none
     */
     void preventSpacesInHostnames(const QString&);

     /**
      * SLOT to catch a button click to switch between hostname and IP mail server
      *
      * @param[in] none
      * @returns  none
      */
     void switchIPHostnameButClicked();

     /**
      * SLOT to catch a button click to switch between Anti-meridian and Post-meridian
      *
      * @param[in] none
      * @returns  none
      */
     void switchAMPMButClicked();

     /**
      * SLOT to perform an upgrade now when Upgrade now button clicked.
      *
      * @param[in] none
      * @returns  none
      */
     void upgradeNow();

     /**
      * SLOT to install upgrade files in the upgrade folder
      *
      * @param[in] none
      * @returns  none
      */
     void installUpgradesNow();

     /**
      * SLOT to show/hide firmware alert if checked or not
      *
      * @param[in] bool
      * @returns  none
      */
     void manageFirmwareAlert(bool isChecked);

     /**
      * SLOT to show/hide firmware alert if checked or not
      *
      * @param[in] bool
      * @returns  none
      */
     void manageFirmwareSendToDeptAlert(bool isChecked);

     /**
      * SLOT to ensure frequecy is within the range
      *  5 minutes to a week
      */
     void manageFrequencyControls();

     /**
      * SLOT to view upgrade filenames
      */

     void viewUpgradesButClicked();

     /**
      * SLOT to close upgrade filenames
      */
     void closeUpgradeViewButClicked();

     /**
      * SLOT to start downloading
      */
     void downloadNowButClicked();

     /**
       * SLOT to update
       */
     void butUpdateNowClicked();

     /**
      * SLOT toggle schedule use
      * */
     void toggleUseAutomatic(bool isChecked);

     /**
      * @brief directoryChanged
      * @param path
      */
     void directoryChanged(const QString & path);

     /**
      * @brief fileChanged
      * @param path
      */
     void fileChanged(const QString & path);

     void noUpdateNowClicked();
     void yesUpdateNowClicked();
     void updateNowComplete();
     void closeUpdateNowFrame();

     void manageStartTimeBox(bool isChecked);
     void manageNoStartTimeBox(bool isChecked);

     void manageUpdateNowDownloadOnly();
     void manageUpdateNowDownloadAndInstall();

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
    Ui::superUpgradeFirmwareViaSFTP *ui;
    QProcess *updateNowProcess;

    QFileSystemWatcher * watcher;
    /**
     * Method to save the SFTP server info
     *
     * @param[in] none
     * @returns  none
     */

    void saveAllConfigsForUpdateNow();

    bool updateUIProgressMessages;
    bool isIPSelected;
    bool isAMSelected;
    bool prelimTesting;
    bool loading;
    bool processingAnUpgrade;
    int localScaleType_;                                            // stndalone=0,primary=1,secondary=2
    bool keepModal;
    bool waitForEmptyInstallOnly;
    bool doneADownload;
    bool clickedInstallUpdates;
    bool hadLostConnectionError;

    QString buttonHit;
    QIntValidator *ipFirstOctet_;
    QIntValidator *ipNonFirstOctet_;
    QIntValidator *hoursValidator_;
    QIntValidator *minutesValidator_;

    QString alertMsg;
    QStringList alertList;
    QStringList alertCBList;
    QStringList notifyCBList;
    QStringList delayList;
    void manageInstallUpdatesButton();
    QString generateUserConfigMessagePart();
    QString prepareDownloadRoutineCallString();
    QString generateScheduleMessagePart();
    QString prepareInstallRoutineCallString();
    void updateScaleSynchronizationType(const SessionMessage &msg);
    void manageDownloadUpdatesButton();
    DomAutomaticDataTypes *automaticDataTypes_;                     // fields being synchronized
    DomAutomaticUpdate *automaticUpdate_;                           // type of scale being update (primary, secondary, standalone)
    MadpInternalConfig *internalConfigData;

    // SOF-2779
    bool onlyInstallingFirmware;
};

#endif // SUPERUPGRADEFIRMWAREVIASFTP
