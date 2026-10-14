/**
 * @file cuttingTestDetails.cpp
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 * @{
 *
 */

#include <logmanager.h>

#include <QtGui>
#include <QtCore>

#include "cuttingTestDetails.h"
#include "ui_cuttingTestDetails.h"

#include "../sessionmainwindow.h"
#include "../modes/operatemode.h"
#include "../dialogs/filedialog.h"
#include "filesystem.h"
#include "plumainscreen.h"
#include "python.h"

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>
#include <QVector>
#include <QStringList>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <ScaleClasses.h>

static const int DEFAULT_ROWS      = 6;                    ///< How many rows we will make by default
static const int GRAND_TOTALS      = 2;                    ///< How many rows we need to display grand totals
static const int DEFAULT_COLS      = 7;                    ///< How many columns we will ever need

static const int CHECKING_FILE              = 1;           ///< When checking if user selected  a file or folder to save to usb
static const int CHECKING_FOLDER            = 2;           ///< When checking if user selected  a file or folder to save to usb
static const int REPORT_DETAILS                  = 1;
static const int REPORT_COMMENTS                 = 2;
static const int REPORT_FINAL_COMMENTS           = 3;
static const QString REPORT_TYPE_DETAILS        = "1";     ///< When sending the report type
static const QString REPORT_TYPE_COMMENTS       = "2";     ///< When sending the report type
static const QString REPORT_TYPE_FINAL_COMMENTS = "3";     ///< When sending the report type

CuttingTestDetails::CuttingTestDetails(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::CuttingTestDetails),
    numOfItems_( 0 ),
    reportNumber_( 1 ),
    test_num_( "" ),
    csvFileData( "" ),
    csvFileSubData( "" ),
    pythonProcess( NULL ),
    numOfTotalRows_( 0 ),                               ///< DEFAULT_ROWS+GRAND_TOTALS or numOfItems+GRAND_TOTALS if numOfItems>DEFAULT_ROWS
    selectedTest_( -1 ),
    returnHomeToWhere_( 1 ),
    currentSelectedRow_(-1)
{
    ui->setupUi(this);

    // check if in operate mode
    if ( (operateMode_ = dynamic_cast<OperateMode*>( parent )) )
    {
        // connect slots
        connect( operateMode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );
    }

    initialize();
    translate();
}

CuttingTestDetails::~CuttingTestDetails()
{
    ScaleConfiguration *scaleConfig( session_->scaleConfig );

    // turn dialogs back on for cut test
    if ( scaleConfig->setAreDialogsTurnedOff( false ) )
        session_->send( SessionDev, SnRestartDialogs );

    delete ui;
}

void CuttingTestDetails::translate()
{
    ui->retranslateUi(this);

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CuttingTestDetails::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

void CuttingTestDetails::initialize()
{
    if ( session_->scaleConfig->setAreDialogsTurnedOff( true ) )
        session_->send( SessionDev, SnStopDialogs );

    /// get and set the operator
    operatorAccess_ = session_->scaleConfig->getLoggedOperatorAccess();

    connect(ui->butPrint,      SIGNAL( clicked() ),              this,   SLOT( onPrintButtonClicked() )    );
    connect(ui->butFinished,   SIGNAL( clicked() ),              this,   SLOT( onFinishedButtonClicked() ) );
    connect(ui->butSave,       SIGNAL( clicked()  ),             this,   SLOT( onSaveButtonClicked()) );
    connect(ui->tblDetails,    SIGNAL( cellClicked(int,int)),    this,   SLOT( selectTheRow(int,int)) );             // click cell

    numOfTotalRows_ = DEFAULT_ROWS + GRAND_TOTALS;               ///< DEFAULT_ROWS+GRAND_TOTALS or numOfItems+GRAND_TOTALS if numOfItems>DEFAULT_ROWS

    ui->tblDetails->clear();
    ui->tblDetails->setRowCount( numOfTotalRows_ );
    ui->tblDetails->setColumnCount( DEFAULT_COLS );

    calcItemColumnWidths();

    /// Set the initial height of the rows
    QHeaderView *verticalHeader = ui->tblDetails->verticalHeader();
    verticalHeader->sectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(33);

    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDetails::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    calcItemColumnWidths();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDetails::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    // hide any weight bar
    if ( operateMode_ )
        operateMode_->hideWeightBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDetails::onPrintButtonClicked()
{
    // user clicked the Print button
    // I added this Cut Test Event but see below
    // session_->send( CutTestDev, PrintCuttingTest, test_num_ );

    // I found this flashkey action which looks like printing a details report
    // This is from operatemode.cpp Action_PrintRunTotals
    session_->send( ReportManagerDev, StartReport, "12,0,0," + test_num_ + "," + REPORT_TYPE_DETAILS );        //	ReportManagerDev, StartReport, RunTotalsReportType, FirstPrinter
    reportNumber_ = REPORT_COMMENTS;        // The next report to be printed
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDetails::onFinishedButtonClicked()
{
    // the user clicked the Finish button

    session_->send( CutTestDev, CancelCuttingTest, test_num_ );

    // Now I need to return to who launched me.
    // 1. Cutting Test History if we got here by clicking a row and then Details in the History Screen
    // 2. Start Screen if gotten here from Save button on Final Comments

    if ( returnHomeToWhere_ == OriginalScreen::CUT_TEST_HISTORY_SCREEN )
    {
        // return to the Cutting Test History Screen bc we clicked Details there
        mainWindow_->launchCuttingTestHistory();
    }
    else if ( returnHomeToWhere_ == OriginalScreen::CUT_TEST_COMMENT_SCREEN )
    {
        // return to the Start Screen because we got here clicking Save on Final Comments
        mainWindow_->navigateHome();
        // reset the screen of the current PLU
        session_->send( ProductDev, ProductRead, "0" );

        //PluMainScreen *pluMainScreen( dynamic_cast<PluMainScreen*>( screen() ) );

        // Reset the Main Screen Flashkeys to 999003
        //if ( pluMainScreen )
        //    pluMainScreen->setSpecialFlasheyWidget( false );  // hide the cut test run screen
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDetails::onSaveButtonClicked()
{
    LOG_CLASS_INFO( "cuttingTestDetails", "onSaveButtonClicked" );

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

        LOG_DEBUG( "file is: %s", fileName.toLatin1().constData() );

        bool hasGoodFileName = checkFileName( fileName, 1 );  // the 2nd arg, 1 = file, 2 = folder, here we are checking a file

        if ( hasGoodFileName )
        {
#if defined( Q_OS_WIN )
            fileName.replace( "/", "\\\\" );
#endif

            QString args = test_num_ + ",0," + fileName;
            ui->lblUserInfo->setText( tr("Exporting, please wait ...") );

            session_->send( CutTestDev, SaveCuttingTestToUsb, args );

/*
            QStringList argsList;
            argsList.append(fileName);
            argsList.append(csvFileData);
            argsList.append(csvFileSubData);
            python pythonProcess;

            if (pythonProcess.execCommand("outputCutTestInfo", argsList))
            {
                if (pythonProcess.getExitCode() == 0)
                {
                    LOG_DEBUG("outputCutTestInfo called", NULL);
                }
                else
                {
                    LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
                }
            }
            else
            {
                LOG_DEBUG("exit code = %d", pythonProcess.getExitCode());
            }

            ui->lblUserInfo->setText( tr("File saved to USB") );
*/
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDetails::selectTheRow(int row, int col)
{
    Q_UNUSED( col );

    QFont font;

    if ( 0 <= row && row < numOfItems_ )
    {
        // means we have a row already selected
        if ( row == currentSelectedRow_ )
        {
            // then we clicked the same row already selected
            font.setBold( false );
            for ( int j( 0 ); j < DEFAULT_COLS; j++ )
                ui->tblDetails->item( row, j)->setFont(font);
            ui->tblDetails->clearSelection();
            currentSelectedRow_ = -1;
            selectedTest_ = -1;
        }
        else
        {
            // Let QTTable do its normal behavior - switch rows
            // we have to unbold the previous row
            if( 0 <= currentSelectedRow_ )
            {
                font.setBold( false );
                for ( int j( 0 ); j < DEFAULT_COLS; j++ )
                    ui->tblDetails->item( currentSelectedRow_, j)->setFont(font);
            }

            // and now bold the current row
            font.setBold( true );
            for ( int j( 0 ); j < DEFAULT_COLS; j++ )
                ui->tblDetails->item( row, j)->setFont(font);

            ui->tblDetails->selectRow( row );
            currentSelectedRow_ = row;
            QTableWidgetItem* item = ui->tblDetails->item( row, 0 );
            selectedTest_ = item->text().toInt();
        }
    }
    else
    {
        // This section only gets called is when we have less cut tests than the numOfTotalRows_ count
        // which is 8. If we have < 6 items (+2 totals rows), then we have empty rows at the bottom of
        // the table and if the user clicks in those rows, this code here handles it. Basically clears stuff.

        // unbold all columns of the currently selected row.
        if( currentSelectedRow_ >= 0)
        {
            font.setBold( false );
            for ( int j( 0 ); j < DEFAULT_COLS; j++ )
                ui->tblDetails->item( currentSelectedRow_, j)->setFont(font);
        }

        currentSelectedRow_ = -1;
        selectedTest_ = -1;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDetails::calcItemColumnWidths()
{   
    int width = ui->tblDetails->width();

    ui->tblDetails->setColumnWidth( 0, (width *  8) / 100 ); // product number
    ui->tblDetails->setColumnWidth( 1, (width * 25) / 100 ); // description
    ui->tblDetails->setColumnWidth( 2, (width * 11) / 100 ); // packages
    ui->tblDetails->setColumnWidth( 3, (width * 13) / 100 ); // weights
    ui->tblDetails->setColumnWidth( 4, (width * 13) / 100 ); // price per lb\kg
    ui->tblDetails->setColumnWidth( 5, (width * 13) / 100 ); // sale amount
    ui->tblDetails->setColumnWidth( 6, (width * 17) / 100 ); // participation
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CuttingTestDetails::checkFileName(QString &myfile, int fileOrFolder)
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
QString CuttingTestDetails::getFileName(QString myfile)
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

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDetails::setReturnHomeLocation(int homeLocation)
{
    returnHomeToWhere_ = homeLocation;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDetails::updateReportData( const DomCuttingTestTotals& value )
{
    LOG_CLASS_INFO( "CuttingTestDetails", "updateReportData" );

    UiUtilities *util = session_->uiUtilities;
    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );

    MadpScaleConfig *scaleConfig( &scaleConfiguration->scaleConfig );
    MadpCurrencySettings *currencySettings( &scaleConfiguration->currencySettings );

    const QString unit( scaleConfig->isScaleMetric() ? " kg" : " lb" );
    const QString perunit( scaleConfig->isScaleMetric() ? " /kg" : " /lb" );
    const QString currSymbol( currencySettings->priCurrSymbol() );

    DomCuttingTestTotals *tests( const_cast<DomCuttingTestTotals*>( &value ) );
    test_num_ = tests->testNum();

    csvFileData = "";
    csvFileSubData = "";

    /// status
    QStringList status_list;
    status_list << tr("Unknown") << tr("Initial") << tr("InProcess") << tr("Completed") << tr("Suspended") << tr("Deleted");

    ui->tblDetails->clear();

    /// Update the top header area
    if( ( 0 == tests->operatorName().length() ) ||  ( 0 == tests->testNum().length() ) || ( 0 == tests->itemNum().length() ) )
    {
        // SOF-3921 Cut Test Stop Button
        // excuses to bail on filling out screen if a user got here accidentally.
        // if we dont have this basic information we have to not display anything.
        // Bad blank data will crash if it's tried to be processed
        return;
    }

    ui->lblCutTestNumber->setText( tests->testNum() );
    ui->lblProductNumber->setText( tests->itemNum() );
    ui->lblSupplierNumber->setText( tests->vendorNum() );
    ui->lblProductCost->setText( currSymbol + tests->itemCost() );
    ui->lblProductWeight->setText( tests->itemWeight() + unit );
    ui->lblLotNumber->setText( tests->itemLotNumber() );
    int status_index = tests->testStatus().toInt();

    // SOF-3921 Cut Test Stop Button - prevent a possible out of index bounds error - but we will likely already returned by now.
    QString cut_status_text( status_list.at(0) );
    if( ( status_index >= 0  ) && ( status_index  < status_list.count() ) )
        cut_status_text = status_list.at(status_index);

    ui->lblStatus->setText( cut_status_text );
    ui->lblOperator->setText( tests->operatorName() );
    ui->lblPackingDate->setText( tests->itemPackingDate() );

    csvFileData = tests->testNum() + ",";

    if( tests->itemNum().length() > 0 )
        csvFileData += tests->itemNum() + ",";
    else
        csvFileData += tr("blank") + ",";

    if( tests->vendorNum().length() > 0 )
        csvFileData += tests->vendorNum() + ",";
    else
        csvFileData += tr("blank") + ",";

    if( tests->itemCost().length() > 0 )
        csvFileData += currSymbol + tests->itemCost() + ",";
    else
        csvFileData += tr("blank") + ",";

    if( tests->itemWeight().length() > 0 )
        csvFileData += tests->itemWeight() + unit + ",";
    else
        csvFileData += tr("blank") + ",";

    if( tests->itemLotNumber().length() > 0 )
        csvFileData += tests->itemLotNumber() + ",";
    else
        csvFileData += tr("blank") + ",";

    if( cut_status_text.length() > 0 )
        csvFileData += cut_status_text + ",";
    else
        csvFileData += tr("blank") + ",";

    if( tests->operatorName().length() > 0 )
        csvFileData += tests->operatorName() + ",";
    else
        csvFileData += tr("blank") + ",";

    if( tests->itemPackingDate().length() > 0 )
        csvFileData += tests->itemPackingDate() + ",";
    else
        csvFileData += tr("blank") + ",";

    if( tests->startDate().length() > 0 )
        csvFileData += tests->startDate() + ",";
    else
        csvFileData += tr("blank") + ",";

    if( tests->endDate().length() > 0 )
        csvFileData += tests->endDate() + ",";
    else
        csvFileData += tr("blank") + ",";

    /// get the size of everything from the message class
    int numItems = tests->numOfItems();
    int numDescs = tests->num2();
    int numPckgs = tests->num7();
    int numWghts = tests->num3();
    int numUntPr = tests->num5();
    int numTotPr = tests->num6();
    int numParti = tests->num4();           // in old cut test this is yield list

    /// get the lists from the message class
    QStringList      cut_pluno = tests->itemsList().split(",");
    QStringList      cut_descs = tests->descList().split(",");
    QStringList      cut_pckgs = tests->packagesList().split(",");
    QVector<QString> cut_wghts = TokenParser::splitStringTokens( tests->weightList() );
    QVector<QString> cut_untPr = TokenParser::splitStringTokens( tests->unitPriceList() );
    QVector<QString> cut_totPr = TokenParser::splitStringTokens( tests->totalPriceList() );
    QStringList      cut_parti = tests->yieldList().split(",");

    /// get the grand totals information
    QString total_wasted = tests->shrinkWeight() + unit;
    QString total_weight = tests->itemWeight() + unit;
    QString total_salepr = currSymbol + tests->sellPrice();
    QString total_wasteP = tests->shrinkYield() + "%";
    QString total_partic = tests->itemYield() + "%";

    if( total_wasted.length() > 0 )
        csvFileData += total_wasted + ",";
    else
        csvFileData += tr("blank") + ",";

    if( total_weight.length() > 0 )
        csvFileData += total_weight + ",";
    else
        csvFileData += tr("blank") + ",";

    if( total_salepr.length() > 0 )
        csvFileData += total_salepr + ",";
    else
        csvFileData += tr("blank") + ",";

    if( total_wasteP.length() > 0 )
        csvFileData += total_wasteP + ",";
    else
        csvFileData += tr("blank") + ",";

    if( total_partic.length() > 0 )
        csvFileData += total_partic;
    else
        csvFileData += tr("blank");


    // check the sise of everything, does it match?
    if ( numItems != cut_pluno.size() )
        LOG_WARN( "mismatch in products  list %d %d", numItems, cut_pluno.size() );

    if ( numDescs != cut_descs.size() )
        LOG_WARN( "mismatch in description list %d %d", numDescs, cut_descs.size() );

    if ( numPckgs != cut_pckgs.size() )
        LOG_WARN( "mismatch in  packages %d %d", numPckgs, cut_pckgs.size() );

    if ( numWghts != cut_wghts.size() )
        LOG_WARN( "mismatch in weights list %d %d", numWghts, cut_wghts.size() );

    if ( numUntPr != cut_untPr.size() )
        LOG_WARN( "mismatch in unit price list %d %d", numUntPr, cut_untPr.size() );

    if ( numTotPr != cut_totPr.size() )
        LOG_WARN( "mismatch in total price list %d %d", numTotPr, cut_totPr.size() );

    if ( numWghts != cut_parti.size() )
        LOG_WARN( "mismatch in participation list %d %d", numParti, cut_parti.size() );

    /// The following should never happen, but we are going to guard against it because a mismatch in counts
    /// will immediately crash the scale, so reduce the number of rows to the least possible number of items.
    /// If there are some on the end, they may get missed, but this only happens on an Application Error/Problem

    int count = numItems;
    count = qMin( count, numDescs );
    count = qMin( count, numPckgs );
    count = qMin( count, numWghts );
    count = qMin( count, numUntPr );
    count = qMin( count, numTotPr );
    count = qMin( count, numParti );

    numOfItems_ = count;                                         ///< so how many items are there?

    numOfTotalRows_ = numOfItems_ + GRAND_TOTALS;                ///< numOfItems+GRAND_TOTALS if numOfItems>DEFAULT_ROWS

    ui->tblDetails->setRowCount( numOfTotalRows_ );

    ui->tblDetails->setColumnCount( DEFAULT_COLS );

    QString price_per = tr("Price Per").append(" ").append(unit);

    QStringList header_list;
    header_list << tr("PLU") << tr("Description") << tr("Packages") << tr("Weight") << price_per << tr("Sale Amount") << tr("Participation");
    ui->tblDetails->setHorizontalHeaderLabels( header_list );

    // body
    if ( count )
    {
        // process items
        int curCol;
        QString tmpStr = "";
        QString teststr = "";

        for ( int j( 0 ); j < count; j++ )
        {
            curCol = 0;                     // always reset to 0 each time around
            csvFileSubData += "\n";            // add a newline for each time around for the csv file

            // Plu numbers
            QTableWidgetItem *testPluItem = new QTableWidgetItem;
            teststr = cut_pluno.at(j);
            testPluItem->setText( teststr );
            ui->tblDetails->setItem( j, curCol++, testPluItem);
            if( total_partic.length() > 0 )
                csvFileSubData += teststr + ",";
            else
                csvFileSubData += tr("blank") + ",";


            // Descriptions
            QTableWidgetItem *testDescriptionItem = new QTableWidgetItem;
            teststr = cut_descs.at(j);
            teststr = teststr.replace("\"", "");
            testDescriptionItem->setText( teststr );
            ui->tblDetails->setItem( j, curCol++, testDescriptionItem );
            if( total_partic.length() > 0 )
                csvFileSubData += teststr + ",";
            else
                csvFileSubData += tr("blank") + ",";

            // Packages
            QTableWidgetItem *testPackagesItem = new QTableWidgetItem;
            teststr = cut_pckgs.at(j);
            testPackagesItem->setText( teststr );
            ui->tblDetails->setItem( j, curCol++, testPackagesItem );
            if( total_partic.length() > 0 )
                csvFileSubData += teststr + ",";
            else
                csvFileSubData += tr("blank") + ",";

            // Weights
            QTableWidgetItem *testWeightsItem = new QTableWidgetItem;
            tmpStr = cut_wghts.at(j);
            util->youGottaSetTheSeparator( tmpStr );
            teststr = tmpStr + unit;
            testWeightsItem->setText( teststr );
            ui->tblDetails->setItem( j, curCol++, testWeightsItem );
            if( total_partic.length() > 0 )
                csvFileSubData += teststr + ",";
            else
                csvFileSubData += tr("blank") + ",";

            // Unit prices
            QTableWidgetItem *testUnitPriceItem = new QTableWidgetItem;
            tmpStr = cut_untPr.at(j);
            tmpStr = tmpStr.replace("\"", "");
            util->youGottaSetTheSeparator( tmpStr );
            teststr = currSymbol + tmpStr + perunit;
            testUnitPriceItem->setText( teststr );
            ui->tblDetails->setItem( j, curCol++, testUnitPriceItem );
            if( total_partic.length() > 0 )
                csvFileSubData += teststr + ",";
            else
                csvFileSubData += tr("blank") + ",";

            // Total prices
            QTableWidgetItem *testTotalPriceItem = new QTableWidgetItem;
            tmpStr = cut_totPr.at(j);
            util->youGottaSetTheSeparator( tmpStr );
            teststr = currSymbol + tmpStr;
            testTotalPriceItem->setText( teststr );
            ui->tblDetails->setItem( j, curCol++, testTotalPriceItem );
            if( total_partic.length() > 0 )
                csvFileSubData += teststr + ",";
            else
                csvFileSubData += tr("blank") + ",";

            // Participation
            QTableWidgetItem *testParticipationItem = new QTableWidgetItem;
            teststr = cut_parti.at(j) + "%";
            testParticipationItem->setText( teststr );
            ui->tblDetails->setItem( j, curCol++, testParticipationItem );
            if( total_partic.length() > 0 )
                csvFileSubData += teststr + ",";
            else
                csvFileSubData += tr("blank") + ",";
        }
    }
    else
    {
        csvFileSubData += "\n";
        csvFileSubData += tr("Product data does not exist") + ",";
    }

    // remove the last trailing comma
    if( csvFileSubData.length() )
        csvFileSubData.chop(1);

    // totals
    QFont font;
    font.setBold( true );

    if( count )
    {
        // determine what row we start putting these. Right after the last item
        int start_row = numOfItems_;
        int last_row = start_row + 1;

        QTableWidgetItem *wasteTitle = new QTableWidgetItem;
        wasteTitle->setText( tr("Waste") );
        ui->tblDetails->setItem( start_row, 1, wasteTitle);
        ui->tblDetails->item( start_row, 1)->setFont(font);

        QTableWidgetItem *totalTitle = new QTableWidgetItem;
        totalTitle->setText( tr("Total") );
        ui->tblDetails->setItem( last_row, 1, totalTitle);
        ui->tblDetails->item( last_row, 1)->setFont(font);

        QTableWidgetItem *wasteTotal = new QTableWidgetItem;
        wasteTotal->setText( total_wasted );
        ui->tblDetails->setItem( start_row, 3, wasteTotal);
        ui->tblDetails->item( start_row, 3)->setFont(font);

        QTableWidgetItem *weightTotal = new QTableWidgetItem;
        weightTotal->setText( total_weight );
        ui->tblDetails->setItem( last_row, 3, weightTotal);
        ui->tblDetails->item( last_row, 3)->setFont(font);

        QTableWidgetItem *priceTotal = new QTableWidgetItem;
        priceTotal->setText( total_salepr );
        ui->tblDetails->setItem( last_row, 5, priceTotal);
        ui->tblDetails->item( last_row, 5)->setFont(font);

        QTableWidgetItem *wasteParticipationTotal = new QTableWidgetItem;
        wasteParticipationTotal->setText( total_wasteP );
        ui->tblDetails->setItem( start_row, 6, wasteParticipationTotal);
        ui->tblDetails->item( start_row, 6)->setFont(font);

        QTableWidgetItem *totalParticipationTotal = new QTableWidgetItem;
        totalParticipationTotal->setText( total_partic );
        ui->tblDetails->setItem( last_row, 6, totalParticipationTotal);
        ui->tblDetails->item( last_row, 6)->setFont(font);
    }
}

bool CuttingTestDetails::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestDetails::onSessionMessage( const SessionMessage& msg )
{
    int device = msg[0].toInt();
    int event = msg[1].toInt();

    if ( ( ReportManagerDev == device ) && ( TerminateReport == event ) )
    {
        if( REPORT_COMMENTS == reportNumber_ )
        {
            // Print the 2nd report
            session_->send( ReportManagerDev, StartReport, "12,0,0," + test_num_ + "," + REPORT_TYPE_COMMENTS );       //	ReportManagerDev, StartReport, RunTotalsReportType, FirstPrinter
            reportNumber_ = REPORT_FINAL_COMMENTS;      // set to last report
        }
        else if( REPORT_FINAL_COMMENTS == reportNumber_ )
        {
            // Print the last report
            session_->send( ReportManagerDev, StartReport, "12,0,0," + test_num_ + "," + REPORT_TYPE_FINAL_COMMENTS ); //	ReportManagerDev, StartReport, RunTotalsReportType, FirstPrinter
            reportNumber_ = REPORT_DETAILS;             // reset to the first report
        }
        else
        {
            // something went wrong, so reset to the first report so they can print again
            reportNumber_ = REPORT_DETAILS;             // reset to the first report
        }
    }
    else if ( ( CutTestDev == device ) && ( CuttingTestSaveToUSBFinished == event ) )
    {
        ui->lblUserInfo->setText( tr("File saved") );
    }
}
