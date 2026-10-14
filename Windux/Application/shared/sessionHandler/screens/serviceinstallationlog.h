/**
 * @file ServiceInstallationLog.h
 * @brief Service Installation Log
 * Allows technicians to add notes when changing, adding, removing, replacing, or fixing a hardware device
 * on the scale such as a printer, weigher, or console.
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup ServiceInstallationLogCsu
 * @{
 */

#ifndef SERVICEINSTALLATIONLOG_H
#define SERVICEINSTALLATIONLOG_H

#include "screenwidget.h"

class DomInstallationLogDeviceData;

namespace Ui {
class ServiceInstallationLog;
}

/**
* @class ServiceInstallationLog
* @brief  
*  
*/

class ServiceInstallationLog  : public ScreenWidget
{
    Q_OBJECT

    typedef ServiceInstallationLog _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
      */
    explicit ServiceInstallationLog(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~ServiceInstallationLog();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve screen help text.
     *
     * When help is activated on a screen this property returns the screen title and help text.
     *
     * @param[out] title  screen title
     * @param[out] description  screen help description
     * @returns  @c true if help exists for the screen, @c false otherwise
     */
    virtual bool helpText( QString& title, QString& description ) const;

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
     * SLOT Method used to set up the device sub id depending upon the device selected
     *
     * @param[in] index  index is the selected index of the devices combo box
     * @returns  none
     */
    void setupSubIdList(int index);

    /**
     * SLOT method used to request device information when a device or subdevice is selected
     *
     * @param[in] none
     * @returns  none
     */
    void requestDeviceInfo();


    /**
     * SLOT method used to add a new note when the new note button is selected
     *
     * @param[in] index  index is the selected index of the devices combo box
     * @returns  the device ID
     */
    void addNote();

    /**
     * SLOT method to handle masking of date fields
     *
     * @param[in] key  key is the key entered on the keypad
     * returns  none
     */
    void performKeyMasking(QString key);



protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /**
     * Handle a CLEAR press on the keyboard locally.
     *
     * @param[in] none
     * @param[out] none
     */
    virtual void clearFocusWidget();

private:
    Ui::ServiceInstallationLog *ui;
    DomInstallationLogDeviceData *installationLogDeviceData_;

    QStringList noneList;
    QStringList twoList;
    QStringList printerList;
    QString dateMask;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method used to send the device information and note data
     *
     * @param[in] none
     * @returns  none
     */
    void sendDeviceInfo();

    /**
     * Method to update the device information when a device is selected from the
     * device combo box
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateDeviceInfo(const SessionMessage &msg);


};

#endif // SERVICEINSTALLATIONLOG
