/**
 * @file serviceWeigherAuditLog.cpp
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Job E. Crew
 */

#include "serviceWeigherAuditLog.h"
#include "ui_serviceWeigherAuditLog.h"

#include "../dialogs/filedialog.h"

#include <Messages.h>
#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"
#include <UiUtilities.h>

#include "../sessionmainwindow.h"
#include "TokenParser.h"

static const int DEFAULT_COLS = 7;
static const int DEFAULT_ROWS = 8;

serviceWeigherAuditLog::serviceWeigherAuditLog(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::serviceWeigherAuditLog)
{
    ui->setupUi(this);

    initialize();

    translate();

    QString initialText = "Audit trail information...";
    ui->lblUserInfo->setText( initialText );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceWeigherAuditLog::~serviceWeigherAuditLog()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceWeigherAuditLog::translate()
{
    ui->retranslateUi(this);

    QStringList header_list;
    //header_list << tr("Record") << tr("Audit Type") << tr("Audit Event") << tr("Parameter") << tr("Date/Time") << tr("New Value") << tr("Old Value");
    header_list << tr("Record") << tr("") << tr("Audit Event") << tr("Parameter") << tr("Date/Time") << tr("New Value") << tr("");

    ui->tblAuditLog->setHorizontalHeaderLabels( header_list );

    calcItemColumnWidths();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceWeigherAuditLog::initialize()
{
    connect(ui->butPrint,          SIGNAL( clicked() ),      this,   SLOT( onPrintButtonClicked() ) );
    connect(ui->butSave,           SIGNAL( clicked() ),      this,   SLOT( onSaveButtonClicked() ) );

    ui->lblUserInfo->setText("");

    ui->butPrint->setVisible( false );

    //------------------------------------------
    // Request the Weigher Audit Log
    //------------------------------------------
    session_->send( Weigher, WgReadAuditTrail );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceWeigherAuditLog::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    calcItemColumnWidths();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceWeigherAuditLog::calcItemColumnWidths()
{
    int width = ui->tblAuditLog->width();

    ui->tblAuditLog->setColumnWidth( 0, (width *  6) / 100 ); // audit item number
    ui->tblAuditLog->setColumnWidth( 1, (width * 0) / 100 ); // audit type
    ui->tblAuditLog->setColumnWidth( 2, (width * 15) / 100 ); // audit event
    ui->tblAuditLog->setColumnWidth( 3, (width * 34) / 100 ); // parameter
    ui->tblAuditLog->setColumnWidth( 4, (width * 21) / 100 ); // date/time
    ui->tblAuditLog->setColumnWidth( 5, (width * 18) / 100 );  // new value
    ui->tblAuditLog->setColumnWidth( 6, (width * 0) / 100 );  // old value
    //                                   -------------------
    //                                           94%             because of scrollbar width

    //ui->tblAuditLog->setColumnWidth( 1, (width * 10) / 100 ); // audit type
    //ui->tblAuditLog->setColumnWidth( 3, (width * 22) / 100 ); // parameter
    //ui->tblAuditLog->setColumnWidth( 5, (width * 8) / 100 );  // new value
    //ui->tblAuditLog->setColumnWidth( 6, (width * 8) / 100 );  // old value

}


///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceWeigherAuditLog::onPrintButtonClicked()
{
    //------------------------------------------------
    // Send the command to print the Weigher Audit Log
    //------------------------------------------------
    session_->send( Weigher, WgPrintAuditTrail );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceWeigherAuditLog::onSaveButtonClicked()
{
    // Send a message to the application to save the log

    // the user clicked the Save button

    QString startDirStr = "";
    QString formatStr( "CSV (*.csv);;*.* (*)" );

    FileDialog filepicker( mainWindow_ );
    filepicker.setFilter( formatStr );
    filepicker.setOptions( QFileDialog::DontConfirmOverwrite );

    QStringList folders;

#if defined( Q_OS_LINUX )
        folders.push_back( "/media/thumb" );
        folders.push_back( "/media/" );
        folders.push_back( "/home/" );
#elif defined( Q_OS_WIN )
        folders.push_back( "D:\\" );
        folders.push_back( "E:\\" );
        folders.push_back( "C:\\" );
#endif

    // find existing folder
    foreach ( const QString& f, folders )
        if ( QDir( startDirStr = f ).exists() )
            break;

    filepicker.setAcceptMode( QFileDialog::AcceptOpen );
    filepicker.setDirectory( startDirStr );
    filepicker.setWindowTitle( tr( "Save to USB" ) );

    if ( QDialog::Accepted == filepicker.exec() )
    {
        qApp->processEvents();

        QStringList selected( filepicker.selectedFiles() );
        QString fileName;

        if ( 1 == selected.count() )
            fileName = selected[0];

        bool hasGoodFileName = checkFileName( fileName, 1 );  // the 2nd arg, 1 = file, 2 = folder, here we are checking a file

        if ( hasGoodFileName )
        {
#if defined( Q_OS_WIN )
            fileName.replace( "/", "\\\\" );
#endif
            QString args = fileName;
            ui->lblUserInfo->setText( tr("Saving log, please wait ...") );

            session_->send( Weigher, WgSaveAuditTrailToDisc, args );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceWeigherAuditLog::checkFileName(QString &myfile, int fileOrFolder)
{
    // fileOrFolder = 1 if checking a file, fileOrFolder = 2 if checking a folder

    bool validFile = false;
    QString errStr = "";

    if(myfile == "")
        errStr = tr( "A file was not selected. Please select a valid file to continue." );                                       // This is the string part of the error message,
    else
    {
        int filelen = myfile.length();

        if(filelen == 0)
            errStr = tr( "A file was not selected. Please select a valid file to continue." );
        else if(myfile.at(filelen - 1) == ('/'))
            errStr = tr( "The filename ends in a slash [/].  Please enter a valid file name." );
        else
        {
            if(fileOrFolder == 1)
                myfile = getFileName(myfile);				//checking a file
            else if(fileOrFolder == 2)
                myfile = myfile;							// checking a folder

            if(filelen <= 0)
                errStr = tr( "A valid file name has not been given.  The file name has no length." );
            else
            {
                if(fileOrFolder == 1 && myfile.indexOf("\\") > 0)
                    errStr = tr( "The \\ [slash] character is not allowed in the filename." );
                else if(myfile.indexOf(";") > 0)
                    errStr = tr( "The ; [semi-colon] character is not allowed in the filename." );
                else if(myfile.indexOf("*") > 0)
                    errStr = tr( "The * [asterisk] character is not allowed in the filename." );
                else if(myfile.indexOf("?") > 0)
                    errStr = tr( "The ? [question mark] character is not allowed in the filename." );
                else if((myfile.indexOf("\"") > 0) || (myfile.indexOf("\'") > 0))
                    errStr = tr( "The \" or \' [single or double quotes] character is not allowed in the filename." );
                else if((myfile.indexOf("<") > 0) || (myfile.indexOf(">") > 0))
                    errStr = tr( "The < or > [qreater or less than] character is not allowed in the filename." );
                else if((myfile.indexOf("+") > 0) || (myfile.indexOf("-") > 0))
                    errStr = tr( "The + or - [plus or minus] character is not allowed in the filename." );
                else if(myfile.indexOf("|") > 0)
                    errStr = tr( "The | [separator bar] character is not allowed in the filename." );
                //else if( myfile.indexOf("C:") > 0 )     // cannot contain C:
                //    errStr = tr("The file cannot be saved to the root file system");
                //else if( isLinux() && myfile.indexOf("media") < 0 )  // has to contain media (a path to a thumb)
                //    errStr = tr("The file cannot be saved to the root file system");
                else
                    validFile = true;
            }
        }
    }

    if ( validFile )
        ui->lblUserInfo->setText( tr("Valid file name") );
    else
        ui->lblUserInfo->setText( errStr );

    return validFile;
}

// Need to figure out, what this is trying to figure out.
QString serviceWeigherAuditLog::getFileName(QString myfile)
{
    //var fileObj;
    QString strippedFileName = "";
    QString retFile = "";

    bool doesFileAlreadyExist = QFile::exists(myfile);

    if( doesFileAlreadyExist )
    {
        retFile = myfile;                                           // then file name already exists and we can return it as is
    }
    else                                                            // we have to create a new one then
    {
        if( myfile.indexOf(".csv") > 0 )
        {
            // we dont have a file, so we need to create one. Do not include the entire path,
            // but do include the entension if the extension is .csv
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
            // No extension was given for the file, so default to the file to an csv file ext.
            strippedFileName = myfile + ".csv";
        }

        retFile = strippedFileName;
    }
    return retFile;
}

bool serviceWeigherAuditLog::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceWeigherAuditLog::updateAuditLogList( const SessionMessage& msg )
{
    DomAuditLogInformation m(msg);

    int numEvents  = m.numOfEvents();
    int numTypes   = m.numOfTypes();
    int numParams  = m.numOfParams();
    int numDates   = m.numOfDates();
    int numNewVals = m.numOfNewVals();
    int numOldVals = m.numOfOldVals();

    QStringList audit_events = m.auditEventList().split(",");
    QStringList audit_types  = m.auditTypeList().split(",");
    QStringList audit_params = m.parameterIdList().split(",");
    QStringList audit_dates  = m.dateTimeList().split(",");
    
    QString tmp1 = m.newValueList();
    QVector<QString>  vecNewNames;
    vecNewNames = TokenParser::splitStringTokens( tmp1 );
    //QStringList audit_newval = m.newValueList().split(",");

    QString tmp2 = m.previousValueList();
    QVector<QString>  vecOldNames;
    vecOldNames = TokenParser::splitStringTokens( tmp2 );
    //QStringList audit_oldval = m.previousValueList().split(",");

    // all of these numbers should always match.  But in case they dont take the lowest numbered value.
    int count = numEvents;
    count = qMin( count, numTypes );
    count = qMin( count, numParams );
    count = qMin( count, numDates );
    count = qMin( count, numNewVals );
    count = qMin( count, numOldVals );

    ui->tblAuditLog->clear();
    if( count < DEFAULT_ROWS )
        ui->tblAuditLog->setRowCount( DEFAULT_ROWS );
    else
        ui->tblAuditLog->setRowCount( count );
    ui->tblAuditLog->setColumnCount( DEFAULT_COLS );

    calcItemColumnWidths();

    QStringList header_list;;
    //header_list << tr("Record") << tr("Category") << tr("Old Value") << tr("New Value") << tr("Date/Time") << tr("Event") << tr("Audit Type");
    //header_list << tr("Record") << tr("Audit Type") << tr("Audit Event") << tr("Parameter") << tr("Date/Time") << tr("New Value") << tr("Old Value");
    header_list << tr("Record") << tr("") << tr("Audit Event") << tr("Parameter") << tr("Date/Time") << tr("New Value") << tr("");

    ui->tblAuditLog->setHorizontalHeaderLabels( header_list );

    /// Received enums to text

    /// For the Audit Type column
    QStringList audittype_list;
    audittype_list << tr("Locally") << tr("Remote") << tr("Unknown");
    //int unknownType = 2;

    /// For the Audit Event column
    QStringList auditevent_list;
    auditevent_list << tr("Calibration") << tr("Configuration") << tr("Upgrade") << tr("Unknown");
    int unknownEvent = 3;

    /// For the Audit Parameter column
    QStringList auditparam_list;
    auditparam_list << tr("Center of maintenance zone") << tr("Scale factor") << tr("Max weight") << tr("Gain factor") << tr("Zero reference") << tr("Cal gain coefficient") << tr("Cal offset coefficient") << tr("Filter speed") << tr("Min weight to print") << tr("Weigher model") << tr("Division size") << tr("Software update") << tr("Gravity constant") << tr("Fractional pricing") << tr("Pounds for multiplier") << tr("Service switch pressed") << tr("Weigh Mode") \
    << tr("Decimal Separator") << tr("Currency Symbol Placement") << tr("Total Price Digits") << tr("Unit Price Digits") << tr("Rounding Factor") \
    << tr("Monetary Rounding Method") << tr("Primary Monetary Symbol") << tr("Primary Precision Digits") << tr("Secondary Monetary Symbol") \
    << tr("Secondary Monetary Decimal")<< tr("Unknown");

    int unknownParameter = 27;

   if ( count )
   {
       QString curStr    = "";
       int     curCol    = 0;
       int     curIndex  = 0;
       int     curEnum   = 0;
       int     numrows   = count - 1;

       for( int j = numrows; j >= 0; j-- )
       {
           curCol  = 0;                      // always reset to 0 each time around
           curStr  = "";                     // always blank the string each time around
           curEnum = 0;

           // Record number
           QTableWidgetItem *auditNumItem = new QTableWidgetItem;
           curStr = QString::number(curIndex);
           auditNumItem->setText( curStr );
           ui->tblAuditLog->setItem( j, curCol++, auditNumItem );

           // Audit Type
           QTableWidgetItem *auditTypeItem = new QTableWidgetItem;
           curEnum = audit_types[curIndex].toInt();
           //if (curEnum >= unknownType )
           //   auditTypeItem->setText( "???" + QString::number(curEnum) + "???");
           //else
           //{
           //    curStr = audittype_list[curEnum];
           //    auditTypeItem->setText( curStr );
           //}
           auditTypeItem->setText( "" );
           ui->tblAuditLog->setItem( j, curCol++, auditTypeItem );

           // Audit Event
           QTableWidgetItem *auditEventItem = new QTableWidgetItem;
           curEnum = audit_events[curIndex].toInt();
           if (curEnum >= unknownEvent )
              auditEventItem->setText( "???" + QString::number(curEnum) + "???" );
           else
           {
               curStr = auditevent_list[curEnum];
               auditEventItem->setText( curStr );
           }
           ui->tblAuditLog->setItem( j, curCol++, auditEventItem );

           // Audit Parameter/Category
           QTableWidgetItem *auditParamItem = new QTableWidgetItem;
           curEnum = audit_params[curIndex].toInt();
           if (curEnum >= unknownParameter )
              auditParamItem->setText( "???" + QString::number(curEnum) + "???");
           else
           {
               curStr = auditparam_list[curEnum];
               auditParamItem->setText( curStr );
           }
           ui->tblAuditLog->setItem( j, curCol++, auditParamItem );

           // Date/time value
           QTableWidgetItem *auditDateItem = new QTableWidgetItem;
           curStr = audit_dates[curIndex];
           auditDateItem->setText( curStr );
           ui->tblAuditLog->setItem( j, curCol++, auditDateItem );

           // New value
           QTableWidgetItem *auditNewItem = new QTableWidgetItem;
           curStr = vecNewNames.at(curIndex);      // to access vector items
           //curStr = audit_newval[curIndex];
           auditNewItem->setText( curStr );
           ui->tblAuditLog->setItem( j, curCol++, auditNewItem );

           // Old value (NO LONGER DISPLAYED)
           QTableWidgetItem *auditOldItem = new QTableWidgetItem;
           curStr = " ";
           //curStr = audit_oldval[curIndex];
           //auditOldItem->setText( curStr );
           auditOldItem->setText( curStr );
           ui->tblAuditLog->setItem( j, curCol++, auditOldItem );

           curIndex++;
       }

       ui->butPrint->setVisible( true ); 
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceWeigherAuditLog::onSessionMessage( const SessionMessage& msg )
{
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString weigherAttached = settings->weigherAttached();

    //--------------------------------------------------------------------------
    // Update Weigher Audit Log Information
    //--------------------------------------------------------------------------
    if ((Weigher == msg[0].toInt() ) && ( WgWriteAuditTrail == msg[1].toInt() ))
    {
        updateAuditLogList( msg );
    }
    else if ( ( Weigher == msg[0].toInt() ) && ( WgSaveAuditTrailToDiskFinished == msg[1].toInt() ) )
    {
        ui->lblUserInfo->setText( tr("File saved") );
    }
}
