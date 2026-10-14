/* @file BackupRestore.cpp
 *  BackupRestore performs the fuunctions Backup, Restore, Merge, and Import.
 *  In previous versions of the counter scale we also performed record scans
 *  However, moving forward record scans will not be done in this widget.
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Joe E. Crew
 */

#include <QtGui>
#include <QtCore>
#include <QVector>
#include <QTimer>

#include "ui_BackupRestore.h"
#include "BackupRestore.h"

#include "../sessionmainwindow.h"

#include "../dialogs/filedialog.h"
#include "../dialogs/messageboxdialog.h"

#include <ScaleConfiguration.h>
#include "python.h"
#include "filesystem.h"
#include <logmanager.h>
#include <UiUtilities.h>

static const int IMPORT_PRODUT_GRAPHICS = 1;
static const int IMPORT_CATEGORY_GRAPHICS = 2;
static const int IMPORT_FLASHKEY_GRAPHICS = 3;
static const int IMPORT_LABEL_GRAPHICS = 4;
static const int IMPORT_VIDEOS = 5;
static const int IMPORT_STORE_LOGO = 6;

////////////////////////////////////////////////////////////
//
//        C O N S T R U C T O R S  &  D E S T R U C T O R S
//
///////////////////////////////////////////////////////////
BackupRestore::BackupRestore(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::BackupRestore),
    internalConfig( NULL ),
    scaleConfig_( NULL ),
    performingAction( false )
{
    ui->setupUi(this);
    scaleScreen();

    scaleConfig_    = &session_->scaleConfig->scaleConfig;
    internalConfig = &session_->scaleConfig->internalConfig;

    initialize();
}

BackupRestore::~BackupRestore()
{
    delete ui;
    pythonProcess->cleanup();
}

/** initialize()
 * 
 *  The initPage() initializes all variables, sets components visible or hidden, and creates all signals and slots
 *  \param: None
 *  \return: void
 *  \Bugs\Issues: None known
 *  \sa Version 1
 */
void BackupRestore::initialize()
{
    ui->dlgImportExport->setVisible(false);
    ui->labFinished->setVisible(false);

    setLabelsVisible(false);

    howManyChecked = 0;
    actionTask = 0;
    waitCtr = 0;                                                    // a factor to determine how long we wait to re-enable stuff

    /* IMPORTANT NOTE
     * The following list has to match enum for enum, text item to text item, enum to text item
     * with the enum list named BRGroupIndicator located in Application/source/HFC/BackupRestore.h
     * Anytime a new item is being backed up, an enum must be added to this list and to BRGroupIndicator.
     */
     // Current Count = 11, 12 if you count the 0 item Unknown
    groupIndicatorArr << " " << "Product Data" << "Resources" << "Operator Data" << "Store Data" << "Scale Configuration" << "Wrapper Settings" << "Flashkey Data" << "COOL Data" << "Self-Serve Data" << "Totals" << "Synchronization Config";

    /* IMPORTANT NOTE
     * The following list has to match enum for enum, text item to text item, enum to text item
     * with the enum list named BRFileIndicator located in Application/source/HFC/BackupRestore.h
     * Anytime a new item is being backed up, an enum must be added to this list and to BRFileIndicator.
     */
     // Current Count = 61, 62 if you count the 0 item UnKnown
    fileIndicatorArr  << " " << "Products"     << "Classes"   << "Nutrifacts" << "Expanded Text" << "Special Messages" << "Product Notes" << "Marquees" << "Cooking Instructions" << "Recipes" << "Expanded Text" << "Suggestive Selling" << "Allergens" << "Transaction Type Names" << "Fonts" << "Label Graphics"
                      << "Label Types"         << "Label Text Names" << "Store Data" << "Operator Data" << "Scale Configuration" << "Wrapper Settings" << "Category Data" << "Presentation Methods" << "Specials Data" << "Streaming Graphics Data"
                      << "Product Statistics"  << "Operator Notes" << "Operator Changes" << "Product Defaults" << "Product Totals" << "Class Totals" << "Operator Totals" << "Hourly Totals" << "Transactions" << "Product Graphics"
                      << "Category Graphics"   << "Specials Graphics" << "Streaming Graphics" << "Softkey Graphics" << "Email Alert Data" << "COOL Categories" << "COOL Countries" << "COOL Production Steps" << "COOL Pretext" << "COOL Text"
                      << "COOL Shortlist Data" << "COOL Totals" << "Softkey Styles" << "Softkey Features" << "Softkeys" << "Softkey Groups" << "COOL Category/Country info" << "COOL Category/Production Step info" << "COOL Category/Pretext info" << "Expiration Label Data" << "Synchronization Config"
                      << "Recalls"             << "Devices"     << "Product Text Config" << "GS1 Scanned Barcodes" << "Cut Tests" << "Scale Departments";

    if ( session_->isPrepack() )
        ui->synchronizeConfig->hide();

    if ( session_->isServiceCounter() )
        ui->wrapperSettings->hide();

    // PJR-145: Wrap-only
    if( ( session_->uiUtilities->isWrapOnly() ) )
    {
        ui->pluData->hide();
        ui->labelData->hide();
        ui->softkeyData->hide();
        ui->operatorData->hide();
        ui->storeName->hide();
    }

    // create all of the checkbox signals and slots
    connect(ui->selectAll,         SIGNAL(clicked()), this,         SLOT(selectAllBoxes()  ) );
    connect(ui->deselectAll,       SIGNAL(clicked()), this,         SLOT(deselectAllBoxes()) );

    // PJR-145: Wrap-only
    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        connect(ui->pluData,           SIGNAL(stateChanged(int)), this, SLOT(selectPluData(int)) );
        connect(ui->labelData,         SIGNAL(stateChanged(int)), this, SLOT(selectLabelData(int)) );
        connect(ui->operatorData,      SIGNAL(stateChanged(int)), this, SLOT(selectOperatorData(int)) );
        connect(ui->softkeyData,       SIGNAL(stateChanged(int)), this, SLOT(selectSoftkeyData(int)) );
        connect(ui->storeName,         SIGNAL(stateChanged(int)), this, SLOT(selectStoreData(int)) );
    }

    connect(ui->scaleConfig,       SIGNAL(stateChanged(int)), this, SLOT(selectScaleConfigData(int)) );

    if ( session_->isServiceCounter() )
        connect(ui->synchronizeConfig, SIGNAL(stateChanged(int)), this, SLOT(selectSyncConfigData(int)) );

    if ( session_->isPrepack() )
        connect(ui->wrapperSettings,   SIGNAL(stateChanged(int)), this, SLOT(selectWrapperSettings(int)) );

    // create all of the button signals and slots
    connect(ui->btnBackup,  SIGNAL(clicked()), this, SLOT(backupFile()));
    connect(ui->btnRestore, SIGNAL(clicked()), this, SLOT(restoreFile()));
    connect(ui->btnMerge,   SIGNAL(clicked()), this, SLOT(mergeFile()));
    connect(ui->btnImport,  SIGNAL(clicked()), this, SLOT(importFile()));
    connect(ui->btnExport,  SIGNAL(clicked()), this, SLOT(exportFile()));
    connect(ui->btnCancel,  SIGNAL(clicked()), this, SLOT(cancelOperations()));

    // TODO - TO BE USED WHEN/IF WE GO TO INDIVIDUAL BUTTONS
    /*
    connect(ui->btnImportProdGraphics,      SIGNAL(clicked()), this, SLOT(importProducts()));
    connect(ui->btnImportCatGraphis,        SIGNAL(clicked()), this, SLOT(importCategories()));
    connect(ui->btnImportFlashGraphics,     SIGNAL(clicked()), this, SLOT(importFlaskeys()));
    connect(ui->btnImportLabelGraphics,     SIGNAL(clicked()), this, SLOT(importGraphics()));
    connect(ui->btnImportTrainingVideos,    SIGNAL(clicked()), this, SLOT(importVideos()));
    connect(ui->btnImportGcdGraphics,       SIGNAL(clicked()), this, SLOT(importLogo()));
    */

    ui->btnCancel->setEnabled( false );

    pythonProcess = new python();

    // connect(ui->actionChange_directory, SIGNAL(triggered()), this, SLOT(changeDirectory()));
    // connect(ui->pluData, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int )));

    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool BackupRestore::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BackupRestore::translate()
{	
	ui->retranslateUi( this );

    update();
}


/////////////////////////////////////////////////////////////
//
//     S L O T S   F R O M   C L I C K I N G   B U T T O N S
//
////////////////////////////////////////////////////////////

/** backupFile()
 * 
 *  backupFile begins the initial process of setting up the widget for the backup
 *  process. Unused fields are hidden (from merge/restore) the status bars are
 *  shown, and the initial text titles are displayed.
 *  \param: None
 *  \return: None
 *  \Bugs\Issues: None known
 *  \sa Version 1
 */
void BackupRestore::backupFile()
{
    setLabelsVisible( false );

    ui->taskBar->setVisible(true);
    ui->fileBar->setVisible(true);
    ui->totalCompleteBar->setVisible(true);

    ui->labFileIndicator->setText(tr("Sub group (file)"));
    ui->labOverall->setText(tr("Overall progress"));

     setFileName(1);                                                 // The 1 represents BACKUP, and sets the style essentially
}

/** restoreFile()
 * 
 *  restoreFile begins the initial process of setting up the widget for the backup
 *  process. Unused fields are hidden (for merge/restore) the status bars are
 *  hidden, and the initial text titles are displayed.
 *  \param: None
 *  \return: None
 *  \Bugs\Issues: None known
 *  \sa Version 1
 */
void BackupRestore::restoreFile()
{
    setLabelsVisible( true );

    ui->taskBar->setVisible(false);
    ui->fileBar->setVisible(false);
    ui->totalCompleteBar->setVisible(false);

    ui->labFileIndicator->setText(tr("Ignored"));
    ui->labOverall->setText(tr("Total records"));

    setFileName(2);
}

/** mergeFile()
 * 
 *  mergeFile begins the initial process of setting up the widget for the backup
 *  process. Unused fields are hidden (for merge/restore) the status bars are
 *  hidden, and the initial text titles are displayed.
 *  \param: None
 *  \return: None
 *  \Bugs\Issues: None known
 *  \sa Version 1
 */
void BackupRestore::mergeFile()
{
    setLabelsVisible( true );

    ui->taskBar->setVisible(false);
    ui->fileBar->setVisible(false);
    ui->totalCompleteBar->setVisible(false);

    ui->labFileIndicator->setText(tr("Ignored"));
    ui->labOverall->setText(tr("Total records"));

    setFileName(3);
}

/** importFile()
 * 
 *  importFile begins the process of performing the import function
 *  \param: None
 *  \return: None
 *  \Bugs\Issues: None known
 *  \sa Version 1
 */
void BackupRestore::importFile()
{
    /* TODO: I need to add more functionality here to enable and disable buttons and fields */

    setFileName(4);
}

/** exportFile()
 *
 *  exportFile begins the process of performing the export function
 *  \param: None
 *  \return: None
 *  \Bugs\Issues: None known
 *  \sa Version 1
 */
void BackupRestore::exportFile()
{
    /* TODO: I need to add more functionality here to enable and disable buttons and fields */

    setFileName(5);
}

/** cancelOperations()
 *
 *  cancelOperations() stops/cancels the current operation
 *  \param: None
 *  \return: None
 *  \Bugs\Issues: None known
 *  \sa Version 1
 */
void BackupRestore::cancelOperations()
{
    session_->send(BackupRestoreDev, BRCancel);                     // 109,5

    // reset some stff
    ui->taskBar->setValue(0);
    ui->fileBar->setValue(0);
    ui->totalCompleteBar->setValue(0);
    ui->labFinished->setVisible(true);

    ui->btnCancel->setEnabled( false );                        // always the opposite of the next line
    setButtonsEnabled(true);
	performingAction = false;
}

// TODO - Slot begins the process of performing the import PLU Product Graphics
/*
void BackupRestore::importProducts()
{
    setImportFileName(IMPORT_PRODUT_GRAPHICS);
}

// Slot begins the process of performing the import PLU Category Graphics for flashkeys
void BackupRestore::importCategories()
{
    setImportFileName(IMPORT_CATEGORY_GRAPHICS);
}

// Slot begins the process of performing the import PLU and Flashkey Graphics
void BackupRestore::importFlaskeys()
{
    setImportFileName(IMPORT_FLASHKEY_GRAPHICS);
}

// Slot begins the process of performing the import PLU Label Graphics
void BackupRestore::importGraphics()
{
    setImportFileName(IMPORT_LABEL_GRAPHICS);
}

 // Slot begins the process of performing the import user Videos
void BackupRestore::importVideos()
{
    setImportFileName(IMPORT_VIDEOS);
}

// Slot begins the process of performing the import GSD Store Logp
void BackupRestore::importLogo()
{
    setImportFileName(IMPORT_STORE_LOGO);
}
*/

/////////////////////////////////////////////////////////////
//
//     A C T I O N S   D O N E   B Y   C L I C K I N G
//     C A L L E D   B Y   S L O T S
//
////////////////////////////////////////////////////////////

/** setFileName()
 * 
 *  setFileName resets the progress bars, determines what OS (starting dir), and lets pops a file or
 *  directory dialog box, and lets the user choose or enter a new file or directory for backing up
 *  restore, importing, or merging.
 *  \param: actionType - The action take, backup, merge, restore, or import.
 *  \return: None
 *  \Bugs\Issues: Possibly the LINUX home directory.
 *  \sa Version 1
 */
void BackupRestore::setFileName(int actionType)
{
    LOG_CLASS_INFO( "BackupRestore", "setFileName" );

    // pjr383 - drop wt req - its really ust based on if flashkeys are active
    //if ( scaleConfig_->SelfServeWeightRequiredToSelectItem() || scaleConfig_->selfServeCombineSearchKeypadFlashkeys() )
    if( scaleConfig_->selfServeItemEntryScreenState() < SelfServicePrintOption::SelfServOption_LookupKeypadOnly )
        internalConfig->updateSelfServiceFlashkeys = true;           // reload flashkeys in self service mode
    
    // reset some values before next action
    ui->taskBar->setValue( 0 );
    ui->fileBar->setValue( 0 );
    ui->totalCompleteBar->setValue( 0 );
    ui->labFinished->setVisible( false );

    QString startDirStr;
    QString formatStr( tr( "HT (*.ht);;HLX (*.hlx);;Quantum (*.dat);;Access (*)" ) );
    if(session_->isPrepack())
        formatStr = tr( "Access (*);;HT (*.ht);;HLX (*.hlx);;Quantum (*.dat)" );


    FileDialog dialog( mainWindow_ );
    dialog.setFilter( formatStr );

    if (( 4 == actionType ) || ( 5 == actionType ))             // Import or Export
    {
        QStringList folders;

#if defined( Q_OS_LINUX )
        folders.push_back( "/media/" );
        folders.push_back( "/home/" );
#elif defined( Q_OS_WIN )
        folders.push_back( "E:/backup/" );
        folders.push_back( "D:\\" );
        folders.push_back( "C:\\" );
#endif

        // find existing folder
        foreach ( const QString& f, folders )
            if ( QDir( startDirStr = f ).exists() )
                break;

        dialog.setAcceptMode( QFileDialog::AcceptOpen );
        dialog.setDirectory( startDirStr );
        dialog.setFileMode( QFileDialog::DirectoryOnly );
        dialog.setOptions( QFileDialog::ShowDirsOnly );

        if (4 == actionType)
        {
            dialog.setWindowTitle( tr( "Import" ) );
            ui->lblImportExportMsg->setText( tr("Importing files.\n\nPlease wait...") );
        }
        else if (5 == actionType)
        {
            dialog.setWindowTitle( tr( "Export" ) );
            ui->lblImportExportMsg->setText( tr("Exporting files.\n\nPlease wait...") );
        }

        if ( QDialog::Accepted == dialog.exec() )
        {
            ui->dlgImportExport->setVisible(true);

            qApp->processEvents();

            QStringList selected( dialog.selectedFiles() );
            QString folderName;

            if ( 1 == selected.count() )
                folderName = selected[0];

            int checkForFolder = 2;                                 // 1 = file, 2 = folder, here we are checking a folder
            bool hasGoodFileName = checkFileName( folderName, checkForFolder );

            if ( hasGoodFileName )
            {
                // change the file separator slashes
                if ( !isLinux() )
                    folderName.replace( "/", "\\\\" );

                if (4 == actionType)
                    session_->send( ImportDev, ImportStart, folderName );       // 144,1, newMsg

                // Import/Export all GCD graphic files which is merely a copy from the
                // selected device (thumb drive)/import to root folder on scale
                QStringList args;

                if (4 == actionType)
                {
                    QString appFolder = filesystem::folder( folderType::RootFolder, false);
                    if ( !isLinux() )
                    {
                        appFolder.replace( "/", "\\\\" );
                        appFolder = "\"" + appFolder + "\"";
                    }


                    args.append(folderName);                        //folder selected by user
                    args.append(appFolder);                         // get root folder on scale

                    // execute python import files command to copy files from user selected device/folder
                    if (pythonProcess->execCommand("importFiles", args))
                    {
                        // check exit code of python command
                        if (pythonProcess->getExitCode() == 0)
                        {
                            LOG_DEBUG("gcd files import from device", NULL);
                        }
                        else
                        {
                            // TODO:
                            // what do we do here?
                            LOG_DEBUG("exit code = %d", pythonProcess->getExitCode());
                        }
                    }
                    if( isLinux() )
                        system("sync");         // flush the file to disc

                }
                else if (5 == actionType)
                {
                    // For some reason, the importFiles Python script needs to have an additional
                    // set of double quotes around the Program Files path due to the space in it.
                    // HOWEVER, the exportFiles Python script WILL NOT WORK with an extra set of
                    // double quotes around the Program Files source path.  Ummmm huh?
                    // No I can't explain it
                    QString appFolder = filesystem::folder( folderType::RootFolder, false);
                    if ( !isLinux() )
                        appFolder.replace( "/", "\\\\" );

                    args.append(appFolder); // get root folder on scale
                    args.append(folderName);

                    // execute python export files command to copy files from root location to user selected device/folder
                    if (pythonProcess->execCommand("exportFiles", args))
                    {
                        // check exit code of python command
                        if (pythonProcess->getExitCode() == 0)
                        {
                            LOG_DEBUG("gcd files exported to device", NULL);
                        }
                        else
                        {
                            // TODO:
                            // what do we do here?
                            LOG_DEBUG("exit code = %d", pythonProcess->getExitCode());
                        }
                    }
                    if( isLinux() )
                        system("sync");         // flush the file to disc

                   session_->send(Error,ImportDev, "144008,5");        // 103,144,144008
                }
            }
            ui->dlgImportExport->setVisible(false);
        }

        simpleFinishedIsDone();
    }
    else
    {
        QStringList folders;

#if defined( Q_OS_LINUX )
        folders.push_back( "/media/thumb" );
        folders.push_back( "/media/" );
        folders.push_back( "/home/" );
#elif defined( Q_OS_WIN )
        folders.push_back( "E:/backup" );
        folders.push_back( "D:\\" );
        folders.push_back( "C:\\" );
#endif

        // find existing folder
        foreach ( const QString& f, folders )
            if ( QDir( startDirStr = f ).exists() )
                break;

        if ( 1 == actionType )                                      // Backup
        {
            dialog.setAcceptMode( QFileDialog::AcceptSave );
            dialog.setWindowTitle( tr( "Backup File" ) );
        }
        else if ( 2 == actionType )                                 // Restore
        {
            dialog.setAcceptMode( QFileDialog::AcceptOpen );
            dialog.setFileMode( QFileDialog::ExistingFile );
            dialog.setWindowTitle( tr( "Restore File" ) );
        }
        else if ( 3 == actionType )                                 // Merge
        {
            dialog.setAcceptMode( QFileDialog::AcceptOpen );
            dialog.setFileMode( QFileDialog::ExistingFile );
            dialog.setWindowTitle( tr( "Merge File" ) );
        }

        dialog.setDirectory( startDirStr );
        dialog.setFilter( formatStr );
        dialog.setOptions( QFileDialog::DontConfirmOverwrite );

        if ( QDialog::Accepted == dialog.exec() )
        {
            QStringList selected( dialog.selectedFiles() );

            if ( 1 == selected.count() )
                performScaleAction( actionType, selected[0] );
        }
    }
}


/* TODO - USE THIS WHEN WE WANT INDIVIDUAL IMPORT BUTTONS
void BackupRestore::setImportFileName(int importWhat)
{
    // Some of this code is being left in here so we can in the future use sepearate buttons for each import function.
    // In the Application see ImportManager::handleImportAllGraphics -> GraphicManager::importGraphicTypes()
    // to see how it is implemented. Pretty much, everything that is done on the thumbdrive will need to be
    // transferred to the application so the thumbdrive can have just a dir and the app does the heavy lifting

    // reset some values before next action
    ui->taskBar->setValue(0);
    ui->fileBar->setValue(0);
    ui->totalCompleteBar->setValue(0);
    ui->labFinished->setVisible(false);

    QString startDirStr = "";

    if( isLinux() )                                                 // Will have to modify if IMPORTING, but we use this 95% of the time
        startDirStr = "/home/backup";
    else
        startDirStr = "C:/backup/";                                 // If windows

    QString folderName = QFileDialog::getExistingDirectory ( this, tr("Import"), startDirStr, QFileDialog::ShowDirsOnly );

    int checkForFolder = 2;                                         // 1 = file, 2 = folder, here we are checking a folder
    bool hasGoodFileName = checkFileName(folderName, checkForFolder);

    if(hasGoodFileName)
    {
        session_->send(ImportDev, ImportStart, folderName);           // 144,1, newMsg, (add importWhat once i get app up to pace)
    }
}
*/

/** isLinux()
 * 
 *  The isLinux() determines if we are running on LINUX or WINDOWS to set initial directories
 *  \param: None
 *  \return: either true if LINUX, else false, which means we are on Windows.
 *  \Bugs\Issues: None known
 *  \sa Version 1
 */
bool BackupRestore::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}

/////////////////////////////////////////////////////////////
//
//     C H E C K I N G   F O R   V A L I D   I N P U T
//
////////////////////////////////////////////////////////////

/** performScaleAction()
 * 
 *  performScaleAction checks to determine what data the user has selected, if any, so it can be acted upon.
 *  The validity of the file is checked, if one was even entered. If the file is valid and data was selected
 *  the selected action, backup, restore, import, or merge, is fired off to begin.
 *  \param: actionType - The action take, backup, merge, restore, or import.
 *  \return: None
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::performScaleAction(int scaleAction, QString fileName)
{
     // First remove the virtual keyboard
     //manuallyHideVirtualKeyboard();                                // added in case it was launched and field not edited
     actionTask = scaleAction;                                       // need it later for updates
     int checkFile = 1;                                             // 1 = file, 2 = folder, here we are checking a file

     //CHANGE THIS TO BY REFERENCE SO IT DOES NOT HAVE TO BE GLOBAL
     bool hasDataSelected = checkDataSelected();
     bool hasGoodFileName = checkFileName(fileName, checkFile);     // pass in the filename and either a file(1) or folder(2) action

     if((hasGoodFileName) && (hasDataSelected))
     {
         setButtonsEnabled(false);
         ui->btnCancel->setEnabled( true );                        // always the opposite of the previous line

         // Have to do some Windows .vs LINUX stuff here with file separators
         if( !isLinux() )
         {
             // change the file separator slashes
             QString tmpFileStr = fileName;
             tmpFileStr = tmpFileStr.replace("/", "\\\\");
             fileName = tmpFileStr;
         }

         performScaleBackup(scaleAction, fileName, "1");
     }
     else if(hasDataSelected == 0)
        sendErrMsg("109013");
}

/** performScaleBackup()
 * 
 *  performScaleBackup performs the final checks before any action occurs. At this time
 *  some backup/restore method is goingt to be performed. Build the function string to
 *  send to the application and set some flags that will have to be handled at some later
 *  time depending on the action selected. Finally fire off the action message to the app.
 *  \param: argCommand - the action to occur, backup, restore, merge, import
 *          argFile    - the file the action will occur against to backup, restore, merge, import
 *          argFormat  - merge and restore are very similar with restore deleting all files first.
 *                       The format indicates if the delete occurs.
 *  \return: None
 *  \Bugs\Issues: None
 *  \sa Version 1
*/
void BackupRestore::performScaleBackup(int argCommand, QString argFile, QString argFormat)
{
    // 0 = none, 1 = BACKUP, 2 = RESTORE, 3 = MERGE, 4 = IMPORT, 5 = SCAN

    QString dataSel = getDataToBackupRestore();

    internalConfig->brDatastream = dataSel;
    internalConfig->brFilename = argFile;

    if (((argCommand == 2) || (argCommand == 3)) && ( ui->labelData->isChecked() ))
        internalConfig->updateOperateLabelData = true;

    if (((argCommand == 2) || (argCommand == 3)) && (ui->softkeyData->isChecked() ))
        internalConfig->updateOperateFlashkeyData = true;           // this is so we can update programmable keys after a restore

    if(argCommand == 2)
        internalConfig->insyncSoftKeyCtr = false;

    sendBackupAndRestoreMsg(argFile, argFormat, dataSel, argCommand);
}

/////////////////////////////////////////////////////////////
//
//     C H E C K I N G   V A L I D   F I L E   I N P U T
//
////////////////////////////////////////////////////////////

/** checkFileName()
 * 
 *  checkFileName checks to determine if a valid file was entered. If the filename
 *  is an empty string, or one of a number of illegal characters, a warning will
 *  be displayed to the user indicating the problem.
 *  \param: myfile       - the file name the user entered
 *          fileOrFolder - indicates if a filename or foldername was entered
 *  \return: bool - indicating if a valid file was entered, or not.
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
bool BackupRestore::checkFileName(QString &myfile, int fileOrFolder)
{
    // fileOrFolder = 1 if checking a file, fileOrFolder = 2 if checking a folder

    bool validFile = false;
    QString errStr = "";

    if(myfile == "")
        sendErrMsg("109001");                                       // This is the string part of the error message,
    else
    {
        int filelen = myfile.length();

        if(filelen == 0)
            errStr = "109001";
        else if(myfile.at(filelen - 1) == ('/'))
            errStr = "109005";
        else
        {
            if(fileOrFolder == 1)
                myfile = getFileName(myfile);				//checking a file
            else if(fileOrFolder == 2)
                myfile = myfile;							// checking a folder

            if(filelen <= 0)
                errStr = "109002";
            else
            {
                if(fileOrFolder == 1 && myfile.indexOf("\\") > 0)
                    errStr = "109003";
                else if(myfile.indexOf(";") > 0)
                    errStr = "109004";
                else if(myfile.indexOf("*") > 0)
                    errStr = "109006";
                else if(myfile.indexOf("?") > 0)
                    errStr = "109007";
                else if((myfile.indexOf("\"") > 0) || (myfile.indexOf("\'") > 0))
                    errStr = "109008";
                else if((myfile.indexOf("<") > 0) || (myfile.indexOf(">") > 0))
                    errStr = "109009";
                else if((myfile.indexOf("+") > 0) || (myfile.indexOf("-") > 0))
                    errStr = "109010";
                else if(myfile.indexOf("|") > 0)
                    errStr = "109011";
                else
                    validFile = true;
            }
        }

        if (!validFile)
            sendErrMsg(errStr);
    }

    return validFile;
}

/** getFileName()
 * 
 *  getFileName determines if a filename was entered with an extension. The extension
 *  is displayed in the file type combo box, and is automatically append the extension
 *  selected by default. However, without question some user will add the extension
 *  while entering the file name itself. This method fixes any issues that may be
 *  encountered if the user adds the entension themselves.
 *  \param: myfile       - the file name the user entered
 *  \return: String - the valid file name with the extension properly appended.
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
// Need to figure out, what this is trying to figure out.
QString BackupRestore::getFileName(QString myfile)
{
    //var fileObj;
    QString strippedFileName = "";
    QString retFile = "";

   // FT-101 Backing up a database results in a file with no extension
   // Still may have to make some more additions for LINUX. Needs seriously tested

    bool doesFileAlreadyExist = QFile::exists(myfile);

    if( doesFileAlreadyExist )
    {
        retFile = myfile;                                           // then file name already exists and we can return it as is
    }
    else                                                            // we have to create a new one then
    {
        if(myfile.indexOf(".ht") > 0 || myfile.indexOf(".hlx") > 0 || myfile.indexOf(".dat") > 0)
        {
            // we dont have a file, so we need to create one. Do not include the entire path,
            // but do include the entension if the extension is .HT, .HLX or .DAT
            strippedFileName = myfile;                              // then we are good to go
        }
        else if(myfile.indexOf(".") > 0)
        {
            // we dont have a file, so we need to create one. Do not include the entire path, just the file.
            // An extension was added to the file, but we don't recogonize it, but include it.
            int getDot = myfile.indexOf(".") + 1;
            if (myfile.length() == getDot + 3)
                strippedFileName = myfile;
        }
        else
        {
            // we don't have a file, so create one. Do not include the entire path of the file.
            // No extension was given for the file, so default to the file to an HT file ext.
            //strippedFileName = myfile.mid(myfile.lastIndexOf("/") + 1, myfile.length());
            strippedFileName = myfile + ".ht";            // .ht by default now
        }

        retFile = strippedFileName;
    }
    return retFile;
}

/////////////////////////////////////////////////////////////////////////
//
//     M E S S A G I N G   T O   A N D   F R O M   A P P L I C A T I O N
//
/////////////////////////////////////////////////////////////////////////

/** sendBackupAndRestoreMsg()
 * 
 *  sendBackupAndRestoreMsg takes all of the valid input data and sends it to the application
 *  so the application can perform either backup, restore, merge, or import.
 *  \param: argCommand   - the action to occur, backup, restore, merge, import
 *          argFile      - the file the action will occur against to backup, restore, merge, import
 *          argFormat    - merge and restore are very similar with restore deleting all files first.
 *                         The format indicates if the delete occurs.
 *          dataSelected - The comma separated list of the data selected in the check boxes.
 *  \return: None
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::sendBackupAndRestoreMsg(QString argFile, QString argFormat, QString dataSelected, int argCommand)
{
    // 0 = none, 1 = BACKUP, 2 = RESTORE, 3 = MERGE, 4 = IMPORT, 5 = SCAN

    switch(argCommand)
    {
        case 0:
            break;
        case 1:
            session_->send(BackupRestoreDev, Backup, argFile + ",1," + argFormat + "," + dataSelected);
            performingAction = true;
            break;
        case 2:
            session_->send(BackupRestoreDev, Restore, argFile + ",2," +  argFormat + "," + dataSelected);
            performingAction = true;
            break;
        case 3:
            session_->send(BackupRestoreDev, Restore, argFile + ",1," +  argFormat + "," + dataSelected);
            performingAction = true;
            break;
        default:
            break;
    }
}

/** sendErrMsg()
 * 
 *  sendErrMsg - Sends to the application any error encountered while
 *  selecting a file or entering a file name
 *  \param: msg - The message number sent to the application indicating the error
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::sendErrMsg(QString msg)
{
    QString newMsg  = msg + ",5";
    session_->send(Error,BackupRestoreDev, newMsg);        // 103,109
}

/** updateStatusBar()
 * 
 *  updateStatusBar - if BACKUP action is being performed, the status bars are updated based
 *  upon the percentage complete for the given record and update the overall percentage based
 *  upon the records backuped up and the records remaining. If an IMPORT, MERGE\RESTORE is
 *  being performed, the current record counts are updated on the screen.
 *  \param: msg - None
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::updateStatusBar(const SessionMessage& msg)
{
    DomBackupRestoreStatus m( msg );

    int groupIndex = m.groupIndicator().toInt();
    int fileIndex = m.fileIndicator().toInt();

    ui->labGroupIndicator->setText(groupIndicatorArr[groupIndex]);          // only if style == 1
    ui->errorCount->setText(m.errorCount() + tr(" errors") );

    // Restore and Imports
    if(actionTask == 2 || actionTask == 3)
    {
        ui->txtRecordCtr->setText( m.fileRecordCnt() );
        ui->txtTotalRecordCtr->setText( m.taskRecordCnt() );
        ui->txtRecordIgnoreCtr->setText( m.filesIgnoredRecordCnt() );          // txtRecIgCt.setText((String)victor.elementAt(8));
        ui->txtTotalRecordIgnoreCtr->setText( m.totFilesIgnoredRecCnt() );
    }
    else
    {
        ui->labFileIndicator->setText( fileIndicatorArr[fileIndex]  );

        float fileTotals   = m.totFileRecordCnt().toFloat();
        float taskTotal    = m.totTaskRecordCnt().toFloat();
        float fileProgress = m.fileRecordCnt().toFloat();
        float taskProgress = m.taskRecordCnt().toFloat();
        float errorTotal   = m.errorCount().toFloat();

        float filePercentComplete    = 0;
        float taskPercentComplete    = 0;
        float overallPercentComplete = 0;

        if ((fileTotals > 0) && (taskTotal > 0))
        {
            filePercentComplete    =  (fileProgress / fileTotals);
            taskPercentComplete    =  (taskProgress / taskTotal );
            overallPercentComplete =  (taskProgress /(taskTotal + errorTotal) );
            filePercentComplete    = filePercentComplete    * 100;
        }
        else
        {
            taskPercentComplete    =  (taskProgress / taskTotal );
            overallPercentComplete =  (taskProgress /(taskTotal + errorTotal) );
        }

        taskPercentComplete    = taskPercentComplete    * 100;
        overallPercentComplete = overallPercentComplete * 100;

        ui->fileBar->setValue((int)filePercentComplete);
        ui->taskBar->setValue((int)taskPercentComplete);
        ui->totalCompleteBar->setValue((int)overallPercentComplete);
    }
}

/** statusBarFinished()
 * 
 *  statusBarFinished - sets all status bars to 100% complete done, displayes the
 *  FINISHED label on the form to indicate to the user the task is complete, and
 *  enables all of the action buttons to their default enabled state.
 *  \param: msg - None
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::statusBarFinished()
{
    // All done now so update all status bars and messages to complete
    // moved all of this next door to setFinishedIsDone()

    // I don't know yet what these should be, I'll experiment with them.
    // Adding the new sync code, I may be able to remove these completely

    int timerFactor = 2;
    if(waitCtr > 50 && waitCtr < 250)
        timerFactor = 3;
    else
        timerFactor = 4;

    if( isLinux() )
        system("sync");         // flush the file to disc

    QTimer::singleShot(timerFactor * 1000, this, SLOT(setFinishedIsDone()));
}

void BackupRestore::setFinishedIsDone()
{
    ui->taskBar->setValue(100);
    ui->fileBar->setValue(100);
    ui->totalCompleteBar->setValue(100);
    ui->labFinished->setVisible(true);

    ui->btnCancel->setEnabled( false );                        // always the opposite of the next line
    setButtonsEnabled(true);
}

void BackupRestore::simpleFinishedIsDone()
{
    ui->labFinished->setVisible(true);

    ui->btnCancel->setEnabled( false );                        // always the opposite of the next line
    setButtonsEnabled(true);
}

////////////////////////////////////////////////////////////////////
//
//     S L O T S   C A L L E D   B Y   S E S S I O N    A F T E R
//     R E C E I V I N G   M E S S A G E S   F R O M   A P P
//
///////////////////////////////////////////////////////////////////


bool BackupRestore::onScreenChanging()
{
    // do not allow screen change when processing backup/restore...
    if ( performingAction )
    {
        MessageBoxDialog dialog( mainWindow_ );

        dialog.setText( tr( "Please wait for the current process to complete!" ) );
        dialog.setStandardButtons( QDialogButtonBox::Ok );

        dialog.exec();

        return false;
    }

    session_->send( TransactionTypeDev, TransactionTypeWriteCurrentType, "1" );       // Production

    return _Mybase::onScreenChanging();
}

/** onSessionMessage()
 *
 *  onSessionMessage - recieves a message from the application where the data in the
 *  message is passed into this method in order to update the data on this form.
 *  onSessionMessage is a SLOT called from Session.handleMessage()
 *  \param: msg - The message from the application.
 *  \return: void
 *  \sa Version 1
 */
void BackupRestore::onSessionMessage( const SessionMessage& msg )
{
    if ( BackupRestoreDev == msg[0].toInt() )
    {
        if( BRCancel == msg[1].toInt() )
        {
            session_->send(BackupRestoreDev, BRCancel);                     // 109,5
			performingAction = false;
        }
        else if ( Terminate == msg[1].toInt() )
        {
            // backup and restore complete message
            statusBarFinished();
            update();

            performingAction = false;
        }
        else if ( Progress == msg[1].toInt() )
        {
            // backup and restore progress message
            waitCtr++;
            updateStatusBar(msg);
            update();
        }
    }
}

////////////////////////////////////////////////////////////////////
//
//     S L O T S   F R O M   C H E C K B O X E S   C L I C K I N G
//
///////////////////////////////////////////////////////////////////

/** selectAllBoxes()
 * 
 *  selectAllBoxes - is a method called from from the current form when the
 *  Select All checkbox is selected. This is the Select All SLOT
 *  \param: None
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::selectAllBoxes()
{
    ui->deselectAll->setChecked(false);
    setAllCheckboxes(true, "1");
}

/** deselectAllBoxes()
 * 
 *  deselectAllBoxes - is a method called from from the current form when the
 *  Unselect All checkbox is selected. This is the Deselect All SLOT
 *  \param: None
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::deselectAllBoxes()
{
    setAllCheckboxes(false, "0");
}

/** selectPluData()
 * 
 *  selectPluData - is a method called from from the current form when the
 *  Plu Data checkbox is selected. This is the Plu Data SLOT
 *  \param: checkState - indicates if the checkbox is selected or not
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::selectPluData(int checkState)
{
    if(checkState == Qt::Checked)
    {
        howManyChecked += 1;
        brData.pluData = "1";
    }
    else
    {
        howManyChecked -= 1;
        brData.pluData = "0";
        ui->selectAll->setChecked(false);
    }
}

/** selectLabelData()
 * 
 *  selectLabelData - is a method called from from the current form when the
 *  Label Data checkbox is selected. This is the Label Data SLOT
 *  \param: checkState - indicates if the checkbox is selected or not
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::selectLabelData(int checkState)
{
    if(checkState == Qt::Checked)
    {
        howManyChecked += 1;
        brData.labelData = "1";
    }
    else
    {
        howManyChecked -= 1;
        brData.labelData = "0";
        ui->selectAll->setChecked(false);
    }
}

/** selectOperatorData()
 * 
 *  selectOperatorData - is a method called from from the current form when the
 *  Operator Data checkbox is selected. This is the Operator Data SLOT
 *  \param: checkState - indicates if the checkbox is selected or not
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::selectOperatorData(int checkState)
{
    if(checkState == Qt::Checked)
    {
        howManyChecked += 1;
        brData.operatorData = "1";
    }
    else
    {
        howManyChecked -= 1;
        brData.operatorData = "0";
        ui->selectAll->setChecked(false);
    }
}

/** selectSoftkeyData()
 * 
 *  selectSoftkeyData - is a method called from from the current form when the
 *  Softkey Data checkbox is selected. This is the Softkey Data SLOT
 *  \param: checkState - indicates if the checkbox is selected or not
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::selectSoftkeyData(int checkState)
{
    if(checkState == Qt::Checked)
    {
        howManyChecked += 1;
        brData.softkeyData = "1";
    }
    else
    {
        howManyChecked -= 1;
        brData.softkeyData = "0";
        ui->selectAll->setChecked(false);
    }
}

/** selectStoreData()
 * 
 *  selectStoreData - is a method called from from the current form when the
 *  Store Data checkbox is selected. This is the Store Data SLOT
 *  \param: checkState - indicates if the checkbox is selected or not
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::selectStoreData(int checkState)
{
    if(checkState == Qt::Checked)
    {
        howManyChecked += 1;
        brData.storeName = "1";
    }
    else
    {
        howManyChecked -= 1;
        brData.storeName = "0";
        ui->selectAll->setChecked(false);
    }
}

/** selectScaleConfigData()
 * 
 *  selectScaleConfigData - is a method called from from the current form when the
 *  Configuration Data checkbox is selected. This is the Configuration Data SLOT
 *  \param: checkState - indicates if the checkbox is selected or not
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::selectScaleConfigData(int checkState)
{
    if(checkState == Qt::Checked)
    {
        howManyChecked += 1;
        brData.scaleConfig = "1";
    }
    else
    {
        howManyChecked -= 1;
        brData.scaleConfig = "0";
        ui->selectAll->setChecked(false);
    }
}

/** selectSyncConfigData()
 *
 *  selectSyncConfigData - is a method called from from the current form when the
 *  Synchronization Data checkbox is selected. This is the Synchronization Data SLOT
 *  \param: checkState - indicates if the checkbox is selected or not
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::selectSyncConfigData(int checkState)
{
    if(checkState == Qt::Checked)
    {
        howManyChecked += 1;
        brData.synchronizeConfig = "1";
    }
    else
    {
        howManyChecked -= 1;
        brData.synchronizeConfig = "0";
        ui->selectAll->setChecked(false);
    }
}

/** selectWrapperSettings()
 *
 *  selectWrapperSettings - is a method called from from the current form when the
 *  Wrapper Settings checkbox is selected.
 *  \param: checkState - indicates if the checkbox is selected or not
 *  \return: void
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::selectWrapperSettings(int checkState)
{
    if(checkState == Qt::Checked)
    {
        howManyChecked += 1;
        brData.wrapperSettings = "1";
    }
    else
    {
        howManyChecked -= 1;
        brData.wrapperSettings = "0";
        ui->selectAll->setChecked(false);
    }
}

/** checkDataSelected()
 * 
 *  checkDataSelected - is a method called when one of the backup and restore
 *  action buttons is pressed. It determines if the user has selected any data.
 *  \param: None
 *  \return: bool - true if the user has selected data to be backed up or restored
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
bool BackupRestore::checkDataSelected()
{
    bool dataOk = false;

    if(howManyChecked >= 1)
        dataOk = true;
    else
        dataOk = false;

    return dataOk;
}

/** getDataToBackupRestore()
 * 
 *  getDataToBackupRestore - is called when a backup or restore action is going to
 *  occur. The file at this point is valid and data has been selected. This method
 *  goes through list of data that can be acted upon and builds a string to send
 *  to the application indicating the data recrods to be backed or restored.
 *  \param: None
 *  \return: QString - A string of all of the possible items that can be backed up
 *  or restored to send to the application. 1 indicates yes, 0 indicates no.
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
QString BackupRestore::getDataToBackupRestore()
{
    QString retData = "";

    if ( ui->pluData->isChecked() )
        retData += "1,";
    else
        retData += "0,";

    if ( ui->labelData->isChecked() )
        retData += "1,";
    else
        retData += "0,";

    if ( ui->operatorData->isChecked() )
        retData += "1,";
    else
        retData += "0,";

    if ( ui->storeName->isChecked() )
        retData += "1,";
    else
        retData += "0,";

    if ( ui->scaleConfig->isChecked() )
        retData += "1,";
    else
        retData += "0,";

    if ( ui->wrapperSettings->isChecked() )			//	HA-59, HA-61
        retData += "1,";
    else
        retData += "0,";

    if ( ui->softkeyData->isChecked() )
        retData += "1,";
    else
        retData += "0,";

    if ( ui->pluData->isChecked() )
        retData += "1,";                                            // COOL data is included with pluData, but is a separate selection for the Application
    else
        retData += "0,";

    if ( ui->pluData->isChecked() )
        retData += "1,";                                            // Self-serve data is included with pluData, but is a separate selection for the Application
    else
        retData += "0,";

    if ( ui->synchronizeConfig->isChecked() )
        retData += "1";
    else
        retData += "0";

    return retData;
}

/////////////////////////////////////////////////////////////
//
//     A C T I O N S   D O N E   B Y   C H E C K B O X I N G
//     C A L L E D   B Y   S L O T S
//
////////////////////////////////////////////////////////////

/** setAllCheckboxes()
 * 
 *  setAllCheckboxes - is called to either select all checkboxes, or deselect all checkboxes.
 *  Also depending on the state passed in, all struct elements are updated as well indicating their value.
 *  \param: state - enabled or disabled
 *          dataValue - the data value to update all of the struct elements
 *  \return: None
 *  \Bugs\Issues: None
 *  \sa Version 1
 */

void BackupRestore::setAllCheckboxes(bool state, QString dataValue)
{
    // PJR-145: Wrap-only
    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        ui->pluData->setChecked(state);
        ui->labelData->setChecked(state);
        ui->softkeyData->setChecked(state);
        ui->operatorData->setChecked(state);
        ui->storeName->setChecked(state);
    }

    ui->scaleConfig->setChecked(state);

    if ( session_->isServiceCounter() )
	    ui->synchronizeConfig->setChecked(state);

    if ( session_->isPrepack() )
	    ui->wrapperSettings->setChecked(state);		//	HA-11

    // PJR-145: Wrap-only
    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        brData.pluData = dataValue;
        brData.storeName = dataValue;
        brData.labelData = dataValue;
        brData.operatorData = dataValue;
        brData.softkeyData = dataValue;
    }

    brData.scaleConfig = dataValue;

    if ( session_->isServiceCounter() )
	    brData.synchronizeConfig = dataValue;

    if ( session_->isPrepack() )
	    brData.wrapperSettings = dataValue;			//	HA-11
}


/////////////////////////////////////////////////////////////
//
//     A C T I O N S   T A K E N   D U R I N G   B A C K U P
//
////////////////////////////////////////////////////////////

/** setButtonsEnabled()
 * 
 *  setButtonsEnabled - is called when one of the backup or restore actions begin. All of the
 *  action buttons are disabled so they cannot be pressed again while a backup or restore
 *  is currently in process. When the process is complete, all buttons are made enabled again.
 *  \param: isEnabled - enabled or disabled
 *  \return: None
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::setButtonsEnabled(bool isEnabled)
{
    ui->btnBackup->setEnabled(isEnabled);                              //Session::getInstance()->send(SessionDev,SnEnableInputEvent,"");  // 100,56 - caused issues
    ui->btnRestore->setEnabled(isEnabled);                             //Session::getInstance()->send(SessionDev,SnShutdownTSInput,"");   // 100,57 - caused issues
    ui->btnMerge->setEnabled(isEnabled);
    ui->btnImport->setEnabled(isEnabled);
    ui->btnExport->setEnabled(isEnabled);

    // TODO - TO BE USED IF WE GO TO SINGLE IMPORT BUTTONS
    /*
    ui->btnImportProdGraphics->setEnabled(isEnabled);
    ui->btnImportFlashGraphics->setEnabled(isEnabled);
    ui->btnImportCatGraphis->setEnabled(isEnabled);
    ui->btnImportGcdGraphics->setEnabled(isEnabled);
    ui->btnImportLabelGraphics->setEnabled(isEnabled);
    ui->btnImportTrainingVideos->setEnabled(isEnabled);
    */
}

/** setLabelsVisible()
 *
 *  setLabelsVisible - is called when one of the backup or restore actions begin. When an
 * action begins the labels that are not needed are hidden, and those needed are shown.
 *  \param: isEnabled - enabled or disabled
 *  \return: None
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::setLabelsVisible(bool isLabelEnabled)
{
    ui->labTotalIgnored->setVisible(isLabelEnabled);

    ui->txtRecordIgnoreCtr->setVisible(isLabelEnabled);
    ui->txtRecordIgnoreCtrLabel->setVisible(isLabelEnabled);

    ui->txtTotalRecordIgnoreCtr->setVisible(isLabelEnabled);
    ui->txtTotalRecordIgnoreCtrLabel->setVisible(isLabelEnabled);

    ui->txtRecordCtr->setVisible(isLabelEnabled);
    ui->txtRecordCtrLabel->setVisible( isLabelEnabled);

    ui->txtTotalRecordCtr->setVisible(isLabelEnabled);
    ui->txtTotalRecordCtrLabel->setVisible(isLabelEnabled);
}

/////////////////////////////////////////////////////////////
//
//    U N U S E D   M E T H O D S
//
////////////////////////////////////////////////////////////

/* This is not used yet at this time. It was a leftover from the previous backup and restore, not sure
   yet at this point if we need it or not. Had to use it on LINUX previous to this project. Test
   to see if we still need this action moving forward. I'm thinking not, but check anyway.
*/

/** changeDirectory()
 * 
 *  changeDirectory - was used in previous projects. Still trying to determine if it's still required.
 *  \param: isEnabled - enabled or disabled
 *  \return: None
 *  \Bugs\Issues: None - just needs tested to determine it use.
 *  \sa Version 1
 */
void BackupRestore::changeDirectory()
{
    /* select a directory using file dialog */
    FileDialog dialog( mainWindow_ );
    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setDirectory( directory.path() );
    dialog.setFileMode( QFileDialog::DirectoryOnly );
    dialog.setOptions( QFileDialog::ShowDirsOnly );
    dialog.setWindowTitle( tr( "Directory" ) );

    if ( QDialog::Accepted == dialog.exec() )
    {
        QStringList selected( dialog.selectedFiles() );
        QString folderName;

        if ( 1 == selected.count() )
            folderName = selected[0];

        if ( !folderName.isEmpty() )
            directory.setPath( folderName );
    }
}


/////////////////////////////////////////////////////////////
//
//     A C T I O N S   T A K E N   W H E N   A L L   D O N E
//
////////////////////////////////////////////////////////////

/** saveDataOnExit()
 * 
 *  saveDataOnExit - performs any actions that need to occur when we leave the form.
 *  \param: isEnabled - enabled or disabled
 *  \return: None
 *  \Bugs\Issues: None
 *  \sa Version 1
 */
void BackupRestore::saveDataOnExit()
{
    brData.pluData = "0";
    brData.storeName = "0";
    brData.labelData = "0";
    brData.scaleConfig = "0";
    brData.operatorData = "0";
    brData.wrapperSettings = "0";
    brData.softkeyData = "0";
    brData.synchronizeConfig = "0";
}
