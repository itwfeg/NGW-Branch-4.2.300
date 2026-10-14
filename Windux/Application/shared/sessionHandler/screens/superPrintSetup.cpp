/**
 * @file superPrintSetup.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "superPrintSetup.h"
#include "ui_superPrintSetup.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

#include <QIntValidator>

namespace hobart {
class numericLineEdit;
}

superPrintSetup::superPrintSetup(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::superPrintSetup)
{
    ui->setupUi(this);
    scaleScreen();

    // Set up validator for Top and Bottom of Label Margins using the Numeric Line Edit
    ui->txtTopOfLabelMargin->setValidator(topOfLabelMarginValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtTopOfLabelMargin->setDecimalPlaces(0);
    ui->txtTopOfLabelMargin->setInputWrapping(true);
    topOfLabelMarginValidator_->setTop(makeValidatorTop( 4 ));

    ui->txtBottomOfLabelMargin->setValidator(bottomOfLabelMarginValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtBottomOfLabelMargin->setDecimalPlaces(0);
    ui->txtBottomOfLabelMargin->setInputWrapping(true);
    bottomOfLabelMarginValidator_->setTop(makeValidatorTop( 4 ));

    //----------------------------------------------------------------
    // Set the connections for when one of the fields on the screen is
    // changed.  These all call the same method to set a property that
    // indicates something in the setup changed and the data needs to
    // be saved.
    //----------------------------------------------------------------
    connect( ui->txtTopOfLabelMargin, SIGNAL( editingFinished() ) , this, SLOT (SetupChanged() ) );
    connect( ui->txtBottomOfLabelMargin, SIGNAL( editingFinished() ), this, SLOT (SetupChanged() ) );

    //--------------------------------------------------------------
    // Set the connections for Starting and Stopping the test report
    //--------------------------------------------------------------
    connect( ui->btnPrintTestReport, SIGNAL(clicked( )), this, SLOT( StartPrintingTestReport() ) );
    connect( ui->btnStopPrinting, SIGNAL(clicked( )), this, SLOT( StopPrintingTestReport() ) );

    //---------------------------------
    // Request the Print Setup settings
    //---------------------------------
    session_->send( Printer, PrReqStockMetrics, "0" );  // Primary Printer

    session_->send( ConfigurationDev, CfgReportsSetupRead );

    bSetupChanged = false;

    ui->btnPrintTestReport->setEnabled(true);

    ui->btnStopPrinting->setEnabled(false);

    translate();
}

superPrintSetup::~superPrintSetup()
{
    delete ui;
}

void superPrintSetup::translate()
{
    ui->retranslateUi(this);

    update();
}

//----------------------------------------------------------------------------
// Used with numericLineEdit widget to validate Top and Bottom of Label Margin
//----------------------------------------------------------------------------
int superPrintSetup::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

bool superPrintSetup::onScreenChanging()
{
    SaveDataOnExit();

    return _Mybase::onScreenChanging();
}

void superPrintSetup::onSessionMessage( const SessionMessage& msg )
{
    // Printer Stock Metrics Settings
    if ((Printer == msg[0].toInt() ) && ( PrStockMetrics == msg[1].toInt() ))
    {
        DomPrinterStockMetrics m( msg );

        //--------------------------------------------
        // NOT SURE WHAT TO DO WITH THIS MESSAGE:
        //  . . . ASP PAGE DIDN'T USE IT EITHER
        //--------------------------------------------

        bSetupChanged = false;
    }

    // Margin Offset Settings
    if ((ConfigurationDev == msg[0].toInt() ) && ( CfgReportsSetupWrite == msg[1].toInt() ))
    {
        DomReportsSetup m( msg );

        ui->txtTopOfLabelMargin->setText(m.offsetTop());
        ui->txtBottomOfLabelMargin->setText(m.offsetBottom());

        bSetupChanged = false;
    }

    // Test Report finished printing - reset buttons
    if ((ReportManagerDev == msg[0].toInt() ) && ( TerminateReport == msg[1].toInt() ))
    {
        DomReportCompleted m( msg );

        // There are no fields in the DOM to check
        //  ... Just reset the buttons if this message is received

        ui->btnPrintTestReport->setEnabled(true);

        ui->btnStopPrinting->setEnabled(false);
    }
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void superPrintSetup::SetupChanged()
{
    bSetupChanged = true;
}

//--------------------------------
// Starts printing the test report
//--------------------------------
void superPrintSetup::StartPrintingTestReport()
{
    // Save any changes before printing the test report
    SaveDataOnExit();

    session_->send( ReportManagerDev, StartReport, "1,0" );

    ui->btnPrintTestReport->setEnabled(false);

    ui->btnStopPrinting->setEnabled(true);
}

//-------------------------------
// Stops printing the test report
//-------------------------------
void superPrintSetup::StopPrintingTestReport()
{
    session_->send( ReportManagerDev, CancelReport, "1,0" );

    ui->btnPrintTestReport->setEnabled(true);

    ui->btnStopPrinting->setEnabled(false);
}

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void superPrintSetup::SaveDataOnExit()
{
    if (bSetupChanged)
    {
        QString param;

        param = ui->txtTopOfLabelMargin->text();
        param += "," + ui->txtBottomOfLabelMargin->text();

        session_->send( ConfigurationDev, CfgReportsSetupWrite, param );

        bSetupChanged = false;
    }
}
