/**
 * @file BackupRestore.h
 * @brief BackupRestore performs the fuunctions Backup, Restore, Merge, and Import.
 * In previous versions of the counter scale we also performed record scans
 * However, moving forward record scans will not be done in this widget.
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperBackupRestoreCsu
 * @{
*/

#ifndef BACKUPRESTORE_H
#define BACKUPRESTORE_H

#include "screenwidget.h"

#include <Messages.h>

#include <QDir>
#include <QVector>
#include "python.h"

class QLabel;
class QCheckBox;
class QPushButton;

class ScaleConfiguration;
class MadpScaleConfig;
class MadpInternalConfig;

namespace Ui {
class BackupRestore;
}

/**
* @class Backup and Restore
* @brief Performs Backup, Restore, Merge, and Import functionality for Hobart Scales
*/
class BackupRestore : public ScreenWidget
{
    Q_OBJECT

    typedef BackupRestore _Myt;
    typedef ScreenWidget _Mybase;


    
public:
    /**
     * Constructor.
     */
    explicit BackupRestore(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~BackupRestore();

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
     * Slot that begins the initial process of setting up the widget for the backup
     * process. Unused fields are hidden (from merge/restore) the status bars are
     * shown, and the initial text titles are displayed.
     *
     * @param[in] none
     * @returns  none
     */
    void backupFile();

    /**
     * Slot that begins the initial process of setting up the widget for the backup
     * process. Unused fields are hidden (for merge/restore) the status bars are
     * hidden, and the initial text titles are displayed.
     *
     * @param[in] none
     * @returns  none
     */
    void restoreFile();

    /**
     * Slot that begins the initial process of setting up the widget for the backup
     * process. Unused fields are hidden (for merge/restore) the status bars are
     * hidden, and the initial text titles are displayed.
     *
     * @param[in] none
     * @returns  none
     */
    void mergeFile();

    /**
     * Slot begins the process of performing the import function
     *
     * @param[in] none
     * @returns  none
     */
    void importFile();

    /**
     * Slot begins the process of performing the export function
     *
     * @param[in] none
     * @returns  none
     */
    void exportFile();

    /**
     * Slot sends a CANCEL command to the Application to Stop/Cancel the current operation
     *
     * @param[in] none
     * @returns  none
     */
    void cancelOperations();

    /**
     * Method gives time to the OS to write the backup and restore file out.
     * @param[in] none
     * @returns  none
     */
    void setFinishedIsDone();

    /**
     * Slot begins the process of performing the import PLU Product Graphics
     *
     * @param[in] none
     * @returns  none
     *
     */
    //void importProducts();

    /**
     * Slot begins the process of performing the import PLU Category Graphics for flashkeys
     *
     * @param[in] none
     * @returns  none
     *
     */
    //void importCategories();

    /**
     * Slot begins the process of performing the import PLU and Flashkey Graphics
     *
     * @param[in] none
     * @returns  none
     *
     */
    //void importFlaskeys();

    /**
     * Slot begins the process of performing the import PLU Label Graphics
     *
     * @param[in] none
     * @returns  none
     */
    //void importGraphics();

    /**
     * Slot begins the process of performing the import user Videos
     *
     * @param[in] none
     * @returns  none
     */
    //void importVideos();

    /**
     * Slot begins the process of performing the import GSD Store Logo
     *
     * @param[in] none
     * @returns  none
     */
    //void importLogo();

    /**
     * The Slot method called from from the current form when the
     * Select All checkbox is selected.
     *
     * @param[in] none
     * @returns  none
     */
    void selectAllBoxes();

    /**
     * The Slot method called from from the current form when the
     * Unselect All checkbox is selected.
     *
     * @param[in] none
     * @returns  none
     */
    void deselectAllBoxes();

    /**
     * The Slot method called from from the current form when the
     * Plu Data checkbox is selected.
     *
     * @param[in] none
     * @returns  none
     */
    void selectPluData(int checkState);

    /**
     * The Slot method called from from the current form when the
     * Label Data checkbox is selected.
     *
     * @param[in] checkState  checkState indicates if the checkbox is selected or not
     * @returns  none
     */
    void selectLabelData(int checkState);

    /**
     * The Slot method called from from the current form when the
     * Operate Data checkbox is selected.
     *
     * @param[in] checkState  checkState indicates if the checkbox is selected or not
     * @returns  none
     */
    void selectOperatorData(int checkState);

    /**
     * The Slot method called from from the current form when the
     * Softkey Data checkbox is selected.
     *
     * @param[in] checkState  checkState indicates if the checkbox is selected or not
     * @returns  none
     */
    void selectSoftkeyData(int checkState);

    /**
     * The Slot method called from from the current form when the
     * Store Data checkbox is selected.
     *
     * @param[in] checkState  checkState indicates if the checkbox is selected or not
     * @returns  none
     */
    void selectStoreData(int checkState);

    /**
     * The Slot method called from from the current form when the
     * Scale Cofiguration checkbox is selected.
     *
     * @param[in] checkState  checkState indicates if the checkbox is selected or not
     * @returns  none
     */
    void selectScaleConfigData(int checkState);

    /**
     * The Slot method called from from the current form when the
     * Synchronize Data checkbox is selected.
     *
     * @param[in] checkState  checkState indicates if the checkbox is selected or not
     * @returns none
     */
    void selectSyncConfigData(int checkState);

    /**
     * The Slot method called from from the current form when the
     * Wrapper Settings checkbox is selected.
     *
     * @param[in] checkState  checkState indicates if the checkbox is selected or not
     * @returns none
     */
    void selectWrapperSettings(int checkState);

    /**
     * The Slot method called when a backup or restore action is going to
     * occur. The file at this point is valid and data has been selected. This method
     * goes through list of data that can be acted upon and builds a string to send
     * to the application indicating the data records to be backed or restored.
     *
     * @param[in] none
     * @returns  @c QString of all of the possible items that can be backed up
     * or restored to send to the application. @c 1 indicates yes, @c 0 indicates no.
     */
    QString getDataToBackupRestore();

    /**
     * The Slot method called when an operator chooses to change directories.
     * At this time this is a legacy method and is still being determined if
     * this method needs to be implemented in the current system.
     *
     * @param[in] none
     * @returns  none
     */
    void changeDirectory();

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
    Ui::BackupRestore *ui;                                          // automatic ui form pointer

    DomBackupRestoreData brData;                                    // method #1 - experiment with differnt ways of access

    MadpInternalConfig *internalConfig;
    MadpScaleConfig *scaleConfig_;


    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to perform any actions that need to occur when we leave the form.
     * Used as a compatability method to the older legacy systems.
     * @param[in] none
     * @returns  none
     */
    void saveDataOnExit();

    /**
     * Method used to check to determine what data the user has selected, if any, so it can be acted upon.
     * The validity of the file is checked, if one was even entered. If the file is valid and data was selected
     * the selected action, backup, restore, import, or merge, is fired off to begin.
     * @param[in] actionType  actionType is the action take, backup, merge, restore, or import.
     * @param[in] fileName  fileName is the name of the backup and restore file written to or read from.
     * @returns  none
     */
    void performScaleAction(int scaleAction, QString fileName);

    /**
     * Method to reset the progress bars, determines what OS (starting dir), and lets pops a file or
     * directory dialog box, and lets the user choose or enter a new file or directory for backing up
     * restore, importing, or merging.
     *
     * @param[in] actionType  actionType is the action take, backup, merge, restore, or import.
     * @returns  none
     */
    void setFileName(int);

    /**
     * Method to reset the progress bars, determines what OS (starting dir), and lets pops a file or
     * directory dialog box, and lets the user choose or enter a new file or directory for backing up
     * restore, importing, or merging.
     *
     * @param[in] importWhat  importWhat is the flag indicating what to import such as flashkeys, graphics, logos, etc.
     * @returns  none
     */
    // TODO - TO BE USED WHEN/IF we go to individual buttons
    //void setImportFileName(int importWhat);

    /**
     * Method enables and disables some objects when done.
     * @param[in] none
     * @returns  none
     */
    void simpleFinishedIsDone();

    /**
     * Method is called to either select all checkboxes, or deselect all checkboxes.
     * Also depending on the state passed in, all struct elements are updated as well indicating their value.
     *
     * @param[in] state  state is @c true if checkboxes are enabled, @c false if disabled
     * @param[in] dataValue  dataValue is the data value to update all of the struct elements
     * @returns  none
     */
    void setAllCheckboxes(bool state, QString dataValue);

    /**
     * Method used to sends to the application any error encountered while
     * selecting a file or entering a file name
     *
     * @param[in] msg  msg is the message number sent to the application indicating the error
     * @returns  none
     */
    void sendErrMsg(QString msg);

    /**
     * Method used to sends to the application any error encountered while
     * selecting a file or entering a file name
     *
     * @param[in] msg  msg is the message number sent to the application indicating the error
     * @returns @c true if the user has selected data to be backed up or restored, @c false otherwise
     */
    bool checkDataSelected();

    /**
     * The method checks to determine if a valid file was entered. If the filename
     * is an empty string, or one of a number of illegal characters, a warning will
     * be displayed to the user indicating the problem.
     *
     * @param[in] myfile  myfile is the name of the file the user entered
     * @param[in] fileOrFolder  fileOrFolder indicates if a filename or foldername was entered
     * @returns @c true if the user has selected data to be backed up or restored, @c false otherwise
     */
    bool checkFileName(QString &myfile, int fileOrFolder);

    /**
     * Method determines if a filename was entered with an extension. The extension
     * is displayed in the file type combo box, and is automatically append the extension
     * selected by default. However, without question some user will add the extension
     * while entering the file name itself. This method fixes any issues that may be
     * encountered if the user adds the entension themselves.
     *
     * @param[in] myFile  myFile is the name of the file the user entered.
     * @returns QString with the valid file name with the extension properly applied
     */
    QString getFileName(QString myfile);

    /**
     * The method performs the final checks before any action occurs. At this time
     * some backup/restore method is goingt to be performed. Build the function string to
     * send to the application and set some flags that will have to be handled at some later
     * time depending on the action selected. Finally fire off the action message to the app.
     *
     * @param[in] argCommand  argCommand indicates the action to occur, backup, restore, merge, import
     * @param[in] argFile  argFile is the name of the file the user entered
     * @param[in] argFormat  argFormat indicates merge and restore options are very similar with restore
                             deleting all files first. The format indicates if the delete occurs.
     * @returns none
     */
    void performScaleBackup(int argCommand, QString argFile, QString argFormat);


    /**
     * The method takes all of the valid input data and sends it to the application
     * so the application can perform either backup, restore, merge, or import.
     *
     * @param[in] argCommand  argCommand indicates the action to occur, backup, restore, merge, import
     * @param[in] argFile  argFile is the name of the file the user entered
     * @param[in] argFormat  argFormat indicates merge and restore options are very similar with restore
     *                       deleting all files first. The format indicates if the delete occurs.
     * @param[in] dataSelected  dataSelected is the comma separated list of the data selected in the check boxes.
     * @returns none
     */
    void sendBackupAndRestoreMsg(QString argFile, QString argFormat, QString dataSelected, int argCommand);

    /**
     * Method determines if BACKUP action is being performed, the status bars are updated based
     * upon the percentage complete for the given record and update the overall percentage based
     * upon the records backuped up and the records remaining. If an IMPORT, MERGE\RESTORE is
     * being performed, the current record counts are updated on the screen.
     * @param[in] none
     * @returns  none
     */
    void updateStatusBar(const SessionMessage& msg);

    /**
     * Method sets all status bars to 100% complete done, displayes the
     * FINISHED label on the form to indicate to the user the task is complete, and
     * enables all of the action buttons to their default enabled state.
     * @param[in] msg  msg is the incoming message from the scale application
     * @returns  none
     */
    void statusBarFinished();

    /**
     * The method determines if we are running on LINUX or WINDOWS to set initial directories
     *
     * @param[in] none
     * @returns  @c true if system OS is a LINUX based system, @c else false which indicates Windows
     */
    static bool isLinux();

    /**
     * Method is called when one of the backup or restore actions begin. All of the
     * action buttons are disabled so they cannot be pressed again while a backup or restore
     * is currently in process. When the process is complete, all buttons are made enabled again.
     *
     * @param[in] isEnabled  isEnabled indicates if the buttons are to be set enabled or disabled
     */
    void setButtonsEnabled(bool isEnabled);

    /**
     * Method is called when one of the backup or restore actions begin. Some of the
     * text labels that are not needed for a particular action are hidden
     *
     * @param[in] isEnabled  isEnabled indicates if the buttons are to be set enabled or disabled
     */
    void setLabelsVisible(bool isLabelEnabled);

    QDir directory;                                                 // not currently used - legacy bound, but may need it.
    int howManyChecked;                                             // how many checkboxes are clicked
    int actionTask;                                                 // The action to occur, backup, restore, merge, import
    int waitCtr;                                                    // a factor to determine how long we wait to re-enable stuff
    QVector<QString> groupIndicatorArr;                             // vector of strings indicating all of the records being backed up
    QVector<QString> fileIndicatorArr;                              // ditto
    python *pythonProcess;
    bool performingAction;

};

/** @} */

#endif // BACKUPRESTORE_H
