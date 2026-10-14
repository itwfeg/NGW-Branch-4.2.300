/**
 * @file cuttingTestHistory.cpp
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 * @{
 *
 */

#include <logmanager.h>

#include "cuttingTestHistory.h"
#include "ui_cuttingTestHistory.h"

#include "../sessionmainwindow.h"
#include "../modes/operatemode.h"
#include "../dialogs/filedialog.h"
#include "cuttingTestDetails.h"

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>
#include <QVector>
#include <QStringList>
#include <QTableWidgetItem>
#include <softkeyswidget.h>
#include <QFileDialog>
#include <ScaleClasses.h>

static const int DEFAULT_ROWS   = 10;
static const int DEFAULT_COLS   = 9;

CuttingTestHistory::CuttingTestHistory(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::CuttingTestHistory),
    numOfCutTests_( 0 ),
    isValidResumePlu( false )
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

CuttingTestHistory::~CuttingTestHistory()
{
    ScaleConfiguration *scaleConfig( session_->scaleConfig );

    scaleConfig->cuttingTestHistoryActive = false;

    // turn dialogs back on for cut test
    if ( scaleConfig->setAreDialogsTurnedOff( false ) )
        session_->send( SessionDev, SnRestartDialogs );

    delete ui;
}

void CuttingTestHistory::translate()
{
    ui->retranslateUi(this);

    QStringList header_list;
    header_list << tr("Test") << tr("Product\nnumber") << tr("Operator\nname") << tr("Status") << tr("Supplier\nnumber") << tr("Product\ncost") << tr("Product\nweight") << tr("Lot number") << tr("Packing data");
    ui->tblHistory->setHorizontalHeaderLabels( header_list );

    int num_rows = ui->tblHistory->rowCount() - 1;
    for( int j = 0; j < num_rows; j++)
    {
        QTableWidgetItem* item = ui->tblHistory->item( j, 3 );
        if( item )
        {
            QString status = item->text();
            QString new_status = "";

            if( status == "Initial" || status == "Inicial" || status == "Initial" )
                new_status = tr("Initial");
            else if( status == "InProcess" || status == "EnProceso"  || status == "InProcess" )
                new_status = tr("InProcess");
            else if( status == "Completed" || status == "Completado" || status == "Completed" )
                new_status = tr("Completed");
            else if( status == "Suspended" || status == "Suspendido" || status == "Suspended")
                new_status = tr("Suspended");
            else if( status == "Deleted"   || status == "Borrado"    || status == "Deleted" )
                new_status = tr("Deleted");
            else
                new_status == tr("Unknown");

            item->setText( new_status );
        }
    }

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CuttingTestHistory::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

void CuttingTestHistory::initialize()
{
    selectedTest_ = -1;
    selectedPlu_ = -1;
    meatSpecSheet_ = -1;
    currentSelectedRow_ = -1;

    if ( session_->scaleConfig->setAreDialogsTurnedOff( true ) )
        session_->send( SessionDev, SnStopDialogs );

    /// get and set the operator
    operatorAccess_ = session_->scaleConfig->getLoggedOperatorAccess();
    operatorName_   = session_->scaleConfig->getLoggedOperatorName();
    operatorNumber_ = session_->scaleConfig->getLoggedOperatorId();

    connect(ui->butHome,            SIGNAL( clicked() ),      this,   SLOT( onHomeButtonClicked() ) );
    connect(ui->butDetail,          SIGNAL( clicked() ),      this,   SLOT( onDetailsButtonClicked() ) );
    connect(ui->butResume,          SIGNAL( clicked() ),      this,   SLOT( onResumeButtonClicked() ) );
    connect(ui->butDelete,          SIGNAL( clicked()  ),     this,   SLOT( onDeleteButtonClicked()) );
    connect(ui->butExportHistory,   SIGNAL( clicked()  ),     this,   SLOT( onExportHistoryButtonClicked()) );

    connect(ui->tblHistory,   SIGNAL(cellClicked(int,int)),     this,   SLOT( selectTheRow(int,int)));             // click cell
    connect(operateMode_,     SIGNAL( currentProductChanged(bool) ),    SLOT( onCurrentProductChanged(bool) ) );

    /// Hide some stuff we dont need until we select a row
    if ( 5 <= operatorAccess_ )
        ui->butDetail->setVisible( true );
    else
        ui->butDetail->setVisible( false );

    ui->butDetail->setVisible( false );
    ui->butResume->setVisible( false );
    ui->butDelete->setVisible( false );

    ui->tblHistory->clear();
    ui->tblHistory->setRowCount( DEFAULT_ROWS );
    ui->tblHistory->setColumnCount( DEFAULT_COLS );

    calcItemColumnWidths();

    /// Set the initial height of the rows
    QHeaderView *verticalHeader = ui->tblHistory->verticalHeader();
    verticalHeader->sectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(33);

    translate();

    ui->lblUserInfo->setText("");
    ui->lblOperator->setText( operatorName_ );
    ui->lblId->setText( operatorNumber_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestHistory::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    calcItemColumnWidths();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CuttingTestHistory::isDataValid()
{
    bool isValid = true;        // just assume we are ok

    // Get the plu from the selected row
    QTableWidgetItem* pluitem = ui->tblHistory->item( currentSelectedRow_, 1 );
    QTableWidgetItem* plucost = ui->tblHistory->item( currentSelectedRow_, 5 );
    QTableWidgetItem* pluwght = ui->tblHistory->item( currentSelectedRow_, 6 );
    QTableWidgetItem* pludate = ui->tblHistory->item( currentSelectedRow_, 6 );

    QString plu_numb = pluitem->text();
    QString plu_cost = plucost->text();
    QString plu_wght = pluwght->text();
    QString plu_date = pludate->text();

    if( plu_numb.isEmpty() || plu_numb == "0"  )
    {
        isValid =  false;
    }
    else if( plu_cost.isEmpty() || plu_cost == "0" || plu_cost == "$0.00" )
    {
        isValid =  false;
    }
    else if( plu_wght.isEmpty() || plu_wght == "0" || plu_wght == "0.00 lb" || plu_wght == "0.000 lb" || plu_wght == "0.000 kg" )
    {
        isValid =  false;
    }
    else if ( plu_date.isEmpty() )
    {
        isValid =  false;
    }

    return isValid;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestHistory::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    // hide any weight bar
    if ( operateMode_ )
        operateMode_->hideWeightBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestHistory::onHomeButtonClicked()
{
    session_->send( ProductDev, ProductRead, "0" );
    mainWindow_->navigateHome();         // transition to the start a run screen
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestHistory::onDetailsButtonClicked()
{
    // a row is selected and the user clicked the Details button

    // Tell the UI who made us go to the Details screen so we can return here later on when done
    session_->scaleConfig->returnHomeLocation = OriginalScreen::CUT_TEST_HISTORY_SCREEN;

    // Send a request to see the Details.  The reply will pop the details screen
    session_->send( CutTestDev, CuttingTestDetailsRead, QString::number(selectedTest_) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestHistory::onResumeButtonClicked()
{
    // a row is selected and the user clicked the Resume button
    if( selectedTest_ >= 0 )
    {
        ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
        MadpCutTestPlu *cutTestPlu( &scaleConfiguration->cutTestPlu );
        DomCurrentPLU *product( currentProduct() );


        if ( isDataValid() || !product )
        {
            if( selectedPlu_ >= 1 )
            {       
                cutTestPlu->pluNumber = product->itemNumber().toInt();
                cutTestPlu->meatSpecSheet = product->meatSpecificationSheet().toInt();
                cutTestPlu->cutTestNumber = selectedPlu_;

                session_->send( CutTestDev, ResumeCuttingTest, QString::number(selectedTest_) );

                if( meatSpecSheet_ > 0)
                {
                    // load the selected spec sheet and go there
                    QString group   = QString::number( meatSpecSheet_ );
                    QString feature = QString::number( hobart::softkeysWidget::MeatSpecSheetScreeenButtons );
                    session_->send( SoftkeyDev, LoadSoftkeyGroup, feature + "," + group );       // (136,93)

                    mainWindow_->toggleCuttingTest( true );
                    //mainWindow_->switchToModeScreen<OperateMode, MeatSheetScreen>(); // MeatSheetScreen
                    session_->send( ProductDev, ProductRead, "0" );
                }
                else
                {
                    // go to the run screen so they can enter items manually
                    session_->send( ProductDev, ProductRead, "0" );
                    mainWindow_->toggleCuttingTest( true );
                    mainWindow_->navigateHome();         // transition to the run screen
                }
            }
        }
        else
        {
            // then there is data that wasn't originally filled out, or we canceled from the screen, or hit the
            // menu button, or we suspended a test we were setting up or we hit the home button
            scaleConfiguration->resumeIncompleteTest = true;
            session_->send( CutTestDev, CuttingTestDtRead, QString::number(selectedTest_) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestHistory::onDeleteButtonClicked()
{
    // A row is selected and the user clicked the Delete button
    if( selectedTest_ >= 0 )
    {
        // Send a message to the application to delete the test
        session_->send( CutTestDev, DeleteCuttingTest, QString::number(selectedTest_) );

        // Now mark it as deleted
        QTableWidgetItem *deletedItem = new QTableWidgetItem;
        deletedItem->setText( tr("Deleted") );
        ui->tblHistory->setItem( currentSelectedRow_, 3, deletedItem );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestHistory::onExportHistoryButtonClicked()
{
    // Send a message to the application to export the history
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
            QString args = "0," + operatorNumber_  + "," + fileName;
            ui->lblUserInfo->setText( tr("Exporting, please wait ...") );

            session_->send( CutTestDev, SaveCuttingTestToUsb, args );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool CuttingTestHistory::checkFileName(QString &myfile, int fileOrFolder)
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
QString CuttingTestHistory::getFileName(QString myfile)
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
void CuttingTestHistory::selectTheRow(int row, int col)
{
    Q_UNUSED( col );

    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );

    isValidResumePlu = false;
    bool enableButtons = false;
    QFont font;

    if ( 0 <= row && row < numOfCutTests_ )
    {
        // means we have a row already selected
        if ( row == currentSelectedRow_ )
        {
            // then we clicked the same row already selected
            font.setBold( false );
            for ( int j( 0 ); j < DEFAULT_COLS; j++ )
                ui->tblHistory->item( row, j)->setFont(font);
            ui->tblHistory->clearSelection();
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
                    ui->tblHistory->item( currentSelectedRow_, j)->setFont(font);
            }
            // and now bold the current row
            font.setBold( true );
            for ( int j( 0 ); j < DEFAULT_COLS; j++ )
                ui->tblHistory->item( row, j)->setFont(font);

            ui->tblHistory->selectRow( row );
            currentSelectedRow_ = row;
            enableButtons = true;

            // Get the selected test
            QTableWidgetItem* item = ui->tblHistory->item( row, 0 );
            selectedTest_ = item->text().toInt();

            // Get the plu from the selected row
            QTableWidgetItem* pluitem = ui->tblHistory->item( row, 1 );
            selectedPlu_ = pluitem->text().toInt();

            if( selectedPlu_ > 0 )
            {
                QTableWidgetItem* statusItem = ui->tblHistory->item( row, 3 );
                QString status = statusItem->text();

                if( status.contains("Suspend") )
                {
                    if( pluitem->text().length() > 0 && pluitem->text() != "0" )
                        session_->send( ProductDev, ProductRead, pluitem->text() );
                }
            }
        }
    }
    else
    {
        // This section only gets called is when we have less cut tests than the DEFAULT_ROWS count
        // which is 11. If we have , 11 rows, then we have empty rows at the bottom of the table and
        // if the user clicks in those rows, this code here handles it. Basically clears stuff.

        // unselect any rows that may be selected.
        if( 0 <= currentSelectedRow_ )
        {
            font.setBold( false );
            for ( int j( 0 ); j < DEFAULT_COLS; j++ )
                ui->tblHistory->item( currentSelectedRow_, j)->setFont(font);
        }

        currentSelectedRow_ = -1;
        selectedTest_ = -1;
    }

    if( CutTestOption::CutTestOption_Default == scaleConfiguration->scaleConfig.cuttingTestType() )     // Default
        return;             // if default we dont enable any additional buttons, period

    /// Hide or display some stuff we need if we select a row
    if ( operatorAccess_ < 5 )
    {
        QTableWidgetItem* statusItem = ui->tblHistory->item( row, 3 );
        if (statusItem)
        {
            QString status = statusItem->text();

            bool canResume( false );
            if( status.contains("Suspend") )
                canResume = true;

            ui->butResume->setVisible( canResume );

            ui->butDetail->setVisible( true );
            ui->butDelete->setVisible( false );
        }
    }
    else
    {
        // Also I can only enable this button if the status field indicates its suspended
        QTableWidgetItem* item = ui->tblHistory->item( row, 3 );
        if (item)
        {
            QString status = item->text();

            if( enableButtons )
            {
                // If a row is selected, always enable details
                ui->butDetail->setVisible( true );

                if( status.contains("Suspend") )
                {
                    ui->butDelete->setVisible( true );
                    ui->butResume->setVisible( true );
                }
                else
                {
                    ui->butDelete->setVisible( false );
                    ui->butResume->setVisible( false );
                }
            }
            else
            {
                ui->butDetail->setVisible( enableButtons );
                ui->butDelete->setVisible( enableButtons );

                if( status.contains("Suspend") )
                    ui->butResume->setVisible( enableButtons );
                else
                    ui->butResume->setVisible( false );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DomCurrentPLU *CuttingTestHistory::currentProduct() const
{
    DomCurrentPLU *product = (operateMode_ ? operateMode_->currentProduct() : NULL);

    if ( product )
    {
        QString itemNumber( product->itemNumber() );

        if ( itemNumber.isEmpty() )
            product = NULL;
    }

    return product;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestHistory::onCurrentProductChanged( bool nof )
{
    meatSpecSheet_ = -1;

    if ( nof )
    {
        if( !isValidResumePlu )
        {
            // if we are trying to resume a test, we have a valid PLU. If isValidResumePlu is true
            // that means we are leaving this screen and we received a clearPLU to clear the PLU
            // on the main screen, and in that case we dont want to see this error.

            //session_->send( Error, ErrSoftkeyDevice, "159004,5" );
            return;
        }
        else
            return;
    }
    else if ( !operateMode_ )
        return;


    DomCurrentPLU *product( currentProduct() );

    if( !product )
    {
        if( !isValidResumePlu )
        {
            // see comment up above
            //session_->send( Error, ErrSoftkeyDevice, "159004,5" );
            return;
        }
        else
            return;
    }

    if( product->itemNumber().toInt() )
    {
        meatSpecSheet_ = product->meatSpecificationSheet().toInt();
        isValidResumePlu = true;
    }
    //else
    //{
    //    session_->send( Error, ErrSoftkeyDevice, "159004,5" );
    //}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestHistory::calcItemColumnWidths()
{   
    int width = ui->tblHistory->width();

    ui->tblHistory->setColumnWidth( 0, (width *  4) / 100 ); // cut test number
    ui->tblHistory->setColumnWidth( 1, (width *  8) / 100 ); // product number
    ui->tblHistory->setColumnWidth( 2, (width * 16) / 100 ); // operator name
    ui->tblHistory->setColumnWidth( 3, (width * 10) / 100 ); // status
    ui->tblHistory->setColumnWidth( 4, (width * 12) / 100 ); // supplier
    ui->tblHistory->setColumnWidth( 5, (width *  9) / 100 ); // product cost
    ui->tblHistory->setColumnWidth( 6, (width *  9) / 100 ); // product weight
    ui->tblHistory->setColumnWidth( 7, (width * 11) / 100 ); // lot number
    ui->tblHistory->setColumnWidth( 8, (width * 15) / 100 ); // packing date
}

void CuttingTestHistory::updateReportData( const DomCuttingTestHistoryTotals& value )
{
    LOG_CLASS_INFO( "CuttingTestHistory", "updateReportData" );

    isJumpingToInitInfo = false;

    DomCuttingTestHistoryTotals *tests( const_cast<DomCuttingTestHistoryTotals*>( &value ) );

    UiUtilities          *util = session_->uiUtilities;
    ScaleConfiguration   *scaleConfiguration( session_->scaleConfig );
    MadpScaleConfig      *scaleConfig( &scaleConfiguration->scaleConfig );
    MadpCurrencySettings *currencySettings( &scaleConfiguration->currencySettings );

    const QString unit( scaleConfig->isScaleMetric() ? " kg" : " lb" );
    const QString currSymbol( currencySettings->priCurrSymbol() );

    ui->tblHistory->clear();

    /// get the data from the message class
    int numTests = tests->numOfTests();
    numOfCutTests_ = numTests;
    int numItems = tests->numOfItems();
    int numOpers = tests->numOfOps();
    int numStats = tests->numOfstati();
    int numSupps = tests->numOfSups();
    int numCosts = tests->numOfPluCosts();
    int numWghts = tests->numOfPluWt();
    int numLots  = tests->numOfLotNums();
    int numDates = tests->numOfpackDates();


    QStringList cut_tests = tests->testNumList().split(",");
    QStringList cut_items = tests->itemNumList().split(",");
    QStringList cut_opers = tests->operatorIDList().split(",");
    QStringList cut_stats = tests->testStatusList().split(",");
    QStringList cut_supps = tests->supplierNumList().split(",");
    QStringList cut_costs = tests->prodCostList().split(",");
    QStringList cut_wghts = tests->prodWeightList().split(",");
    QStringList cut_lots = tests->lotNumList().split(",");
    QStringList cut_dates = tests->packDateList().split(",");

    if ( numTests != cut_tests.size() )
        LOG_WARN( "mismatch in test numbers list %d %d", numTests, cut_tests.size() );

    if ( numItems != cut_items.size() )
        LOG_WARN( "mismatch in plu list %d %d", numItems, cut_items.size() );

    if ( numOpers != cut_opers.size() )
        LOG_WARN( "mismatch in operators list %d %d", numOpers, cut_opers.size() );

    if ( numStats != cut_stats.size() )
        LOG_WARN( "mismatch in status list %d %d", numStats, cut_stats.size() );

    if ( numSupps != cut_supps.size() )
        LOG_WARN( "mismatch in suppliers price list %d %d", numSupps, cut_supps.size() );

    if ( numCosts != cut_costs.size() )
        LOG_WARN( "mismatch in price list %d %d", numCosts, cut_costs.size() );

    if ( numWghts != cut_wghts.size() )
        LOG_WARN( "mismatch in weight list %d %d", numWghts, cut_wghts.size() );

    if ( numLots != cut_lots.size() )
        LOG_WARN( "mismatch in lot number list %d %d", numLots, cut_lots.size() );

    if ( numDates != cut_dates.size() )
        LOG_WARN( "mismatch in packing date list %d %d", numDates, cut_dates.size() );

    /// The following should never happen, but we are going to guard against it because a mismatch in counts
    /// will immediately crash the scale, so reduce the number of rows to the least possible number of items.
    /// If there are some on the end, they may get missed, but this only happens on an Application Error/Problem

    int count = numTests;
    count = qMin( count, numItems );
    count = qMin( count, numOpers );
    count = qMin( count, numStats );
    count = qMin( count, numSupps );
    count = qMin( count, numCosts );
    count = qMin( count, numWghts );
    count = qMin( count, numLots );
    count = qMin( count, numDates );

    if ( count < DEFAULT_ROWS )
        ui->tblHistory->setRowCount( DEFAULT_ROWS );
    else
        ui->tblHistory->setRowCount( count );

    ui->tblHistory->setColumnCount( DEFAULT_COLS );

    /// header
    QStringList header_list;
    header_list << tr("Test") << tr("Product\nnumber") << tr("Operator\nname") << tr("Status") << tr("Supplier\nnumber") << tr("Product\ncost") << tr("Product\nweight") << tr("Lot number") << tr("Packing data");
    ui->tblHistory->setHorizontalHeaderLabels( header_list );

    /// status
    QStringList status_list;
    status_list << tr("Unknown") << tr("Initial") << tr("InProcess") << tr("Completed") << tr("Suspended") << tr("Deleted");

    /// body
    if ( count )
    {
        // process items
        int curCol;
        QString tmpStr = "";

        for ( int j( 0 ); j < count; j++ )
        {
            curCol = 0;                     // always reset to 0 each time around

            // Test number
            QTableWidgetItem *testNumItem = new QTableWidgetItem;
            testNumItem->setText( cut_tests.at(j) );
            ui->tblHistory->setItem( j, curCol++, testNumItem );

            // Plu numbers
            QTableWidgetItem *testPluItem = new QTableWidgetItem;
            testPluItem->setText( cut_items.at(j) );
            ui->tblHistory->setItem( j, curCol++, testPluItem);

            // Operators
            QTableWidgetItem *testOperatorItem = new QTableWidgetItem;
            QString tmpstr = cut_opers.at(j);
            tmpstr = tmpstr.replace("\"", "");
            testOperatorItem->setText(tmpstr );
            ui->tblHistory->setItem( j, curCol++, testOperatorItem );

            // Current status
            QTableWidgetItem *testStatusItem = new QTableWidgetItem;
            int status_index = cut_stats.at(j).toInt();
            QString cut_status = status_list.at(status_index);
            testStatusItem->setText( cut_status );
            ui->tblHistory->setItem( j, curCol++, testStatusItem );

            // Supplier numbers
            QTableWidgetItem *testSupplierItem = new QTableWidgetItem;
            tmpstr = cut_supps.at(j);
            tmpstr = tmpstr.replace("\"", "");
            testSupplierItem->setText( tmpstr );
            ui->tblHistory->setItem( j, curCol++, testSupplierItem );

            // Costs
            QTableWidgetItem *testCostsItem = new QTableWidgetItem;
            tmpStr = cut_costs.at(j);
            tmpstr = tmpstr.replace("\"", "");
            util->youGottaSetTheSeparator( tmpStr );
            testCostsItem->setText( currSymbol + tmpStr );
            ui->tblHistory->setItem( j, curCol++, testCostsItem );

            // Weights
            QTableWidgetItem *testWeightsItem = new QTableWidgetItem;
            tmpStr = cut_wghts.at(j);
            tmpstr = tmpstr.replace("\"", "");
            util->youGottaSetTheSeparator( tmpStr );
            testWeightsItem->setText( tmpStr + unit );
            ui->tblHistory->setItem( j, curCol++, testWeightsItem );

            // Lot numbers
            QTableWidgetItem *testLotsItem = new QTableWidgetItem;
            tmpstr = cut_lots.at(j);
            tmpstr = tmpstr.replace("\"", "");
            testLotsItem->setText( tmpstr );
            ui->tblHistory->setItem( j, curCol++, testLotsItem );

            // Packing dates
            QTableWidgetItem *testDatesItem = new QTableWidgetItem;
            testDatesItem->setText( cut_dates.at(j) );
            ui->tblHistory->setItem( j, curCol++, testDatesItem );
        }
    }
}

bool CuttingTestHistory::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CuttingTestHistory::onSessionMessage( const SessionMessage& msg )
{
    int device = msg[0].toInt();
    int event = msg[1].toInt();

    if ( ( CutTestDev == device ) && ( CuttingTestSaveToUSBFinished == event ) )
    {
        ui->lblUserInfo->setText( tr("File saved") );
    }
}

