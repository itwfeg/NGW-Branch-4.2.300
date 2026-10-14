/**
 * @file superPrinterSetup.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "superPrinterSetup.h"
#include "ui_superPrinterSetup.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

static const int NoPrinter    = 0;          // Not sure if this is even possible, unless we have major issue and cant see it
static const int EPPPrinter   = 1;          // Standalone EPP Printer, but we are running out of parts for it, moving to EPP2.
static const int ApcPrinter   = 4;          // The APC Printer is the normal everyday printer for the HT scales as a primary printer.
static const int EPP2Printer  = 5;          // Newest Atmel EPP Printer which is becoming the newer standard.
static const int UsbPrinter   = 6;

static const QString SelfService    = "1";  // Newest Atmel EPP Printer which is becoming the newer standard.
static const QString AccessWrapper  = "2";  // Access Wrapper
static const QString CounterScale   = "3";  // Newest Atmel EPP Printer which is becoming the newer standard.

// Defaults for dropdowns
static const QString PrinterTypeDefault = "3";
static const QString PrintContrastDefault = "0";
static const QString TestLabelPatternDefault = "2";
static const QString NoResponseValue = "3";

superPrinterSetup::superPrinterSetup(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::superPrinterSetup)
{
    ui->setupUi(this);
    scaleScreen();

    printerAddress_0 = "0";
    printerAddress_1 = "0";
    printerAddress_2 = "0";
    currentPrinter_  = "0";

    initialize();

    disableFields();

    //------------------------------------------
    // Set connections for Printing a Test Label
    //------------------------------------------
    connect( ui->btnPrintTestLabel_0, SIGNAL( clicked() ), this, SLOT( PrintTestLabel_0() ) );

    //------------------------------------------------------------------
    // Set the connections for when a one of the setup fields is changed.
    // These all call the same method to set a property that indicates
    // something in the setup changed and the data needs to be saved.
    //------------------------------------------------------------------
    connect( ui->cbPrinterSelect_0, SIGNAL( currentIndexChanged(int) ), this, SLOT( setupChanged(int) ) );

    //---------------------------------------------------------
    // Set the connection for when the Update button is clicked
    //---------------------------------------------------------
    connect( ui->btnUpdate, SIGNAL( clicked() ), this, SLOT( UpdatePrinters() ) );

    //------------------------------
    // Turn on Printer Type Tracking
    //------------------------------
    session_->send( Printer, PrPrinterTypeTracking, "0,1" );

    //--------------------------------------
    // Request a Printer Inquiry - Printer 0
    //--------------------------------------
    session_->send( Printer, PrReqInquire, "0" );

    bSetupChanged = false;

    translate();
}

superPrinterSetup::~superPrinterSetup()
{
    delete ui;
}

void superPrinterSetup::translate()
{
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString scaleModelSetting = settings->scaleModel();

    ui->retranslateUi(this);
    ui->cbTestLabelPattern->setItemText(0, tr( "Checker Board" ));
    ui->cbTestLabelPattern->setItemText(1, tr( "Horizontal Bars" ));
    ui->cbTestLabelPattern->setItemText(2, tr( "Vertical Lines" ));

    ui->cbPrinterSelect_0->setItemText( 0, tr( "Primary" ));

    if( scaleModelSetting == AccessWrapper )
    {
        ui->cbPrinterSelect_0->setItemText( 1, tr( "Second" ) );
        ui->cbPrinterSelect_0->setItemText( 2, tr( "Third" ) );
    }
    else if(scaleModelSetting == CounterScale)
    {
        // If we have an HT scale, see if we need to add some EPP printers to the combo
        if ( isEppPrinterAttached(2) )
            ui->cbPrinterSelect_0->setItemText( 1, tr( "EPP" ));
        else if( isUsbPrinterAttached(2) )
            ui->cbPrinterSelect_0->setItemText( 1, tr( "USB" ));

        if ( isEppPrinterAttached(3) )
            ui->cbPrinterSelect_0->setItemText( 1, tr( "EPP" ));
        else if( isUsbPrinterAttached(3) )
            ui->cbPrinterSelect_0->setItemText( 1, tr( "USB" ));
    }

    update();
}

bool superPrinterSetup::onScreenChanging()
{
    TurnOffPrinterTypeTracking();

    SaveDataOnExit();

    return _Mybase::onScreenChanging();
}

void superPrinterSetup::onSessionMessage( const SessionMessage& msg )
{
    //--------------------------
    // Printer Locations message
    //--------------------------
    if ((Printer == msg[0].toInt() ) && ( PrPrinterType == msg[1].toInt() ))
    {
        DomPrinterLocations m( msg );

        QString printerType = m.printerType();
        int intNoResponseSelection;

        switch( printerType.toInt() )
        {
            // Enable controls for the correct printer
            case 0:

                printerAddress_0 = m.printerAddress();
                ui->cbPrinterSelect_0->setCurrentIndex( ui->cbPrinterSelect_0->findData( m.printerType() ) );
                ui->cbPrinterSelect_0->setEnabled( true );
                ui->cbPrinterContrastSelect_0->setEnabled( true );
                ui->btnPrintTestLabel_0->setEnabled( true );

                // Remove selection for "No Response" from the dropdown
                intNoResponseSelection = ui->cbPrinterSelect_0->findData( NoResponseValue );
                if(intNoResponseSelection > -1)
                    ui->cbPrinterSelect_0->removeItem( intNoResponseSelection );

                // FT-102 - Always disable the Printer Type dropdown
                ui->cbPrinterSelect_0->setDisabled( true );

                break;
        }
        if ((Printer == msg[0].toInt() ) && ( PrWriteAllExternalPrinter == msg[1].toInt() ))
        {
            updatePrinterSettings( msg );
        }

        bSetupChanged = false;
    }

    //-----------------------------------------------------
    // Printer Configuration (from Printer Inquiry request)
    //-----------------------------------------------------
    if ((Printer == msg[0].toInt() ) && ( PrInquire == msg[1].toInt() ))
    {
        DomPrinterConfigRead m( msg );

        int printerType = m.printerType();

        switch( printerType )
        {
            // Select the correct value in the Print Contrast dropdown
            case 0:

                ui->cbPrinterContrastSelect_0->setCurrentIndex( ui->cbPrinterContrastSelect_0->findData( m.printContrast() ) );

                //------------------------------------------------------------
                // Set connection for saving Print Contrast when it is changed
                //------------------------------------------------------------
                connect( ui->cbPrinterContrastSelect_0, SIGNAL( currentIndexChanged(int) ), this, SLOT(SavePrintContrast_0( int ) ) );

                break;
        }

        bSetupChanged = false;
    }
}

void superPrinterSetup::updatePrinterSettings( const SessionMessage& /*msg*/ )
{
    MadpScaleConfig *settings( &session_->scaleConfig->scaleConfig );
    QString scaleModel( settings->scaleModel() );

    ui->cbPrinterSelect_0->addItem( tr( "Primary" ),     0);

    if( scaleModel == AccessWrapper )
    {
        ui->cbPrinterSelect_0->addItem( tr( "Second" ), 1);
        ui->cbPrinterSelect_0->addItem( tr( "Third" ),  2);
    }
    else if( scaleModel == CounterScale)
    {
        // If we have an HT scale, see if we need to add some EPP printers to the combo
        if ( isEppPrinterAttached(2) )
            ui->cbPrinterSelect_0->addItem( "EPP", 1);                // We can only have one EPP printer attached if counter scale, so its the 2nd item
        else if( isUsbPrinterAttached(2) )
            ui->cbPrinterSelect_0->addItem( "USB", 1);

        if ( isEppPrinterAttached(3) )
            ui->cbPrinterSelect_0->addItem( "EPP", 2);
        else if( isUsbPrinterAttached(3) )
            ui->cbPrinterSelect_0->addItem( "USB", 2);
    }
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void superPrinterSetup::setupChanged(int /*index*/)
{
    TurnOffPrinterTypeTracking();       // turn off tracking for current printer
    bSetupChanged = true;

    currentPrinter_ = ui->cbPrinterSelect_0->itemData(ui->cbPrinterSelect_0->currentIndex()).toString();

    QString  param = currentPrinter_  + ",1";
    session_->send( Printer, PrPrinterTypeTracking, param );
}

//--------------------------------------
// Slot for printing a single test label
//--------------------------------------
void superPrinterSetup::PrintTestLabel_0()
{
    QString param;

    // PrinterNum + TestPattern + "0"
    param = "0," + (ui->cbTestLabelPattern->itemData( ui->cbTestLabelPattern->currentIndex() ) ).toString()  + ",0";
    session_->send( Printer, PrTestLabel, param );
}

//-----------------------------------------------
// Slot for Saving Print Contrast when it changes
//-----------------------------------------------
void superPrinterSetup::SavePrintContrast_0( int index )
{
    QString param;

    param = "0,";   // PrinterNum
    param += ( ui->cbPrinterContrastSelect_0->itemData( index ) ).toString();

    session_->send( Printer, PrConfig, param );
}

//-------------------------------------------
// Slot for when the Update button is clicked
//-------------------------------------------
void superPrinterSetup::UpdatePrinters()
{
    QString param;

    param = (ui->cbPrinterSelect_0->itemData( ui->cbPrinterSelect_0->currentIndex() ) ).toString();
    param += "," + printerAddress_0;
    param += "," + NoResponseValue;
    param += "," + printerAddress_1;
    param += "," + NoResponseValue;
    param += "," + printerAddress_2;

    session_->send( Printer, PrSetPrinters, param);

    initFields();
}

//---------------------
// Initialize dropdowns
//---------------------
void superPrinterSetup::initialize()
{
    // Print contrast dropdowns
    ui->cbPrinterContrastSelect_0->addItem( "0", 0);
    ui->cbPrinterContrastSelect_0->addItem( "1", 1);
    ui->cbPrinterContrastSelect_0->addItem( "2", 2);
    ui->cbPrinterContrastSelect_0->addItem( "3", 3);
    ui->cbPrinterContrastSelect_0->addItem( "4", 4);
    ui->cbPrinterContrastSelect_0->addItem( "5", 5);
    ui->cbPrinterContrastSelect_0->addItem( "6", 6);
    ui->cbPrinterContrastSelect_0->addItem( "7", 7);

    // Default Print Contrast dropdowns to "0"
    ui->cbPrinterContrastSelect_0->setCurrentIndex( ui->cbPrinterContrastSelect_0->findData( PrintContrastDefault ) );

    // Test Label Pattern
    ui->cbTestLabelPattern->addItem( tr( "Checker Board" ),   2 );
    ui->cbTestLabelPattern->addItem( tr( "Horizontal Bars" ), 3 );
    ui->cbTestLabelPattern->addItem( tr( "Vertical Lines" ),  4 );

    // Default Test Label Pattern dropdown to Checkerboard
    ui->cbTestLabelPattern->setCurrentIndex( ui->cbTestLabelPattern->findData( TestLabelPatternDefault ) );
}

//----------------------------------------------------------------
// Initialize fields after having sent the Printer Update messages
//----------------------------------------------------------------
void superPrinterSetup::initFields()
{
    //-----------
    // Printer #1
    //-----------

    // Check if the Printer Type dropdown contains a selection for "No Response"
    if(ui->cbPrinterSelect_0->count() == NoResponseValue.toInt() && (ui->cbPrinterSelect_0->findData( NoResponseValue ) == -1))
    {
        // Add "No Response" into the dropdown
        ui->cbPrinterSelect_0->addItem( tr( "No response" ), 3);
    }

    // Now check if there are more selections in the Printer Type dropdown
    // than the value for "No Response"
    if(ui->cbPrinterSelect_0->count() > NoResponseValue.toInt() )
    {
        // Set the Printer Type selection to "No Response"
        ui->cbPrinterSelect_0->setCurrentIndex( ui->cbPrinterSelect_0->findData( NoResponseValue ) );

        // Disable the Printer Type and Print Contrast dropdowns
        ui->cbPrinterSelect_0->setDisabled( true );
        ui->cbPrinterContrastSelect_0->setDisabled( true );
    }

    session_->send(Printer, PrReadAllExternalPrinters);             // Request Printer External Printer Settings
}

void superPrinterSetup::disableFields()
{
    ui->cbPrinterContrastSelect_0->setDisabled( true );

    ui->btnPrintTestLabel_0->setDisabled( true );
}

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void superPrinterSetup::SaveDataOnExit()
{
    if (bSetupChanged)
    {
        UpdatePrinters();

        bSetupChanged = false;
    }
}

//-------------------------------
// Turn off Printer Type Tracking
//-------------------------------
void superPrinterSetup::TurnOffPrinterTypeTracking()
{
    QString  param = currentPrinter_  + ",0";
    session_->send( Printer, PrPrinterTypeTracking, param );
}

bool superPrinterSetup::isEppPrinterAttached(int printerLocation)
{
    MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString scaleModelSetting = settings->scaleModel();

    bool isAttached = false;

    if(scaleModelSetting == CounterScale)
    {
        if ( 2 == printerLocation )
        {
            if (periphSettings->secondPrinter()  == EPPPrinter  || periphSettings->secondPrinter()  == EPP2Printer )
                isAttached = true;
        }
        else if ( 3 == printerLocation )
        {
            if ( periphSettings->thirdPrinter() == EPPPrinter || periphSettings->thirdPrinter()  == EPP2Printer)
                isAttached = true;
        }
    }

    return isAttached;
}

bool superPrinterSetup::isUsbPrinterAttached(int printerLocation)
{
    MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString scaleModelSetting = settings->scaleModel();

    bool isAttached = false;

    if(scaleModelSetting == CounterScale)
    {
        if ( 2 == printerLocation )
        {
            if (periphSettings->secondPrinter()  == UsbPrinter )
                isAttached = true;
        }
        else if ( 3 == printerLocation )
        {
            if ( periphSettings->thirdPrinter() == UsbPrinter )
                isAttached = true;
        }
    }

    return isAttached;
}
