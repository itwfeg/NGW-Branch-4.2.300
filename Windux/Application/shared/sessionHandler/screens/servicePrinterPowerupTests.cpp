/**
 * @file servicePrinterPowerupTests.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "servicePrinterPowerupTests.h"
#include "ui_servicePrinterPowerupTests.h"

#include <ScaleConfiguration.h>
#include <ScaleClasses.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

servicePrinterPowerupTests::servicePrinterPowerupTests(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::servicePrinterPowerupTests)
{
    ui->setupUi(this);
    scaleScreen();

    initialize();

    //---------------------------------------------------------
    // Set the connection for when the Printer dropdown changes
    //---------------------------------------------------------
    connect(ui->cbPrinterSelection, SIGNAL( currentIndexChanged(int) ), this, SLOT( PrinterChanged() ));

    translate();

    ui->lblSoftwareVersionData->setText( "0.0.0" );
    ui->lblHardwareVersionData->setText( "0.0" );

    ui->lblPrintHeadTypeData->setText( tr( "Unknown" ));
    ui->lblPrintHeadDotsData->setText( "0" );
}

servicePrinterPowerupTests::~servicePrinterPowerupTests()
{
    delete ui;
}

void servicePrinterPowerupTests::translate()
{
    //---------------------------------------------------
    // Update text in dropdowns when the language changes
    //---------------------------------------------------
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    int scaleModelSetting = settings->scaleModel().toInt();

    ui->cbPrinterSelection->setItemText( 0, tr( "Primary" ) );

    if(scaleModelSetting == ScaleModel::ScaleModel_EPP )
    {
        ui->cbPrinterSelection->setItemText( 1, tr( "Second" ) );
        ui->cbPrinterSelection->setItemText( 2, tr( "Third" ) );
    }
    else if(scaleModelSetting == ScaleModel::ScaleModel_ESC )
    {
        // If we have an HT scale, see if we need to add some EPP printers to the combo
        if ( isEppPrinterAttached() )
            ui->cbPrinterSelection->setItemText( 1, tr("EPP") );                // We can only have one EPP printer attached if counter scale, so its the 2nd item
    }

    //------------------------------------------------
    // Request the Printer info in translate() method,
    // text is determined by the messages
    //------------------------------------------------
    session_->send( PrinterSystem, PrSysReqStatus, (ui->cbPrinterSelection->itemData(ui->cbPrinterSelection->currentIndex())).toString() );
    session_->send( MfgTestDev, RequestPrinterLabelCount, (ui->cbPrinterSelection->itemData(ui->cbPrinterSelection->currentIndex())).toString() );

    ui->retranslateUi(this);

    update();
}

void servicePrinterPowerupTests::onSessionMessage( const SessionMessage& msg )
{
    //--------------------
    // Printer Information
    //--------------------
    if ((PrinterSystem == msg[0].toInt() ) && ( PrSysStatus == msg[1].toInt() ))
    {
        DomPrinterPowerupTests m( msg );

        //---------------------------------------
        // SOF-671: Display "N/A" for KPC printer
        //---------------------------------------
        int printerType = m.printerType().toInt();
        int printerKind = PrinterKind::fieldOffsetNoPrinter;

        MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;

        if( printerType == PrinterType::FirstPrinter )
            printerKind = periphSettings->firstPrinter();
        else if( printerType == PrinterType::SecondPrinter )
            printerKind = periphSettings->secondPrinter();
        else if( printerType == PrinterType::ThirdPrinter )
            printerKind = periphSettings->thirdPrinter();

        bool isKPCPrinter    = false;
        bool isPreKPCPrinter = false;

        if( ( printerKind == PrinterKind::fieldOffsetKPCPrinter) ||
            ( printerKind == PrinterKind::fieldOffsetSSKPrinter) )
        {
            session_->send( Printer, PrReqHeadType );
            isKPCPrinter    = true;
        }
        else
        {
            isPreKPCPrinter = true;

            ui->lblCalculatedChecksumData->setText(m.calculatedChecksum());
            ui->lblStoredChecksumData->setText(m.storedChecksum());
            ui->lblEEPChecksumTestData->setText( (m.EEPChecksum() == "0" ? tr( "Passed") : tr( "Failed" ) ) );
            ui->lblWordTestData->setText( (m.wordTest() == "0" ? tr( "Passed") : tr( "Failed" ) ) );
            ui->lblAddressTestData->setText( (m.addressTest() == "0" ? tr( "Passed") : tr( "Failed" ) ) );
        }

        // KPC
        ui->lblPrintHeadDots->setVisible( isKPCPrinter );
        ui->lblPrintHeadType->setVisible( isKPCPrinter );
        ui->lblPrintHeadDotsData->setVisible( isKPCPrinter );
        ui->lblPrintHeadTypeData->setVisible( isKPCPrinter );

        // Pre-KPC
        ui->lblCalculatedChecksum->setVisible( isPreKPCPrinter );
        ui->lblStoredChecksum->setVisible( isPreKPCPrinter );
        ui->lblEEPChecksumTest->setVisible( isPreKPCPrinter );
        ui->lblWordTest->setVisible( isPreKPCPrinter );
        ui->lblAddressTest->setVisible( isPreKPCPrinter );
        ui->lblCalculatedChecksumData->setVisible( isPreKPCPrinter );
        ui->lblStoredChecksumData->setVisible( isPreKPCPrinter );
        ui->lblEEPChecksumTestData->setVisible( isPreKPCPrinter );
        ui->lblWordTestData->setVisible( isPreKPCPrinter );
        ui->lblAddressTestData->setVisible( isPreKPCPrinter );

        ui->lblSoftwareVersionData->setText(m.softwareVersion());               // always visible
        ui->lblHardwareVersionData->setText(m.hardwareVersion());               // always visible
    }

    else if ((MfgTestDev == msg[0].toInt() ) && ( PrinterLabelCount == msg[1].toInt() ))
    {
        DomPrinterLabelCount m( msg );

        ui->lblLabelCountData->setText( QString::number( m.count() ) );         // always visible
    }
    else if ( (Printer == msg[0].toInt() ) && ( PrHeadType == msg[1].toInt() ))
    {
        DomPrinterHeadType m( msg );

        ui->lblPrintHeadTypeData->setText( printHeads_[m.printerHeadType()] );  // visible on KPC ONLY
        ui->lblPrintHeadDotsData->setText( m.printerDotSize() + tr( " dots" )); // visible on KPC ONLY
    }

}

void servicePrinterPowerupTests::initialize()
{
    // Pre-KPC
    ui->lblCalculatedChecksum->setVisible( false );
    ui->lblStoredChecksum->setVisible( false );
    ui->lblEEPChecksumTest->setVisible( false );
    ui->lblWordTest->setVisible( false );
    ui->lblAddressTest->setVisible( false );
    ui->lblCalculatedChecksumData->setVisible( false );
    ui->lblStoredChecksumData->setVisible( false );
    ui->lblEEPChecksumTestData->setVisible( false );
    ui->lblWordTestData->setVisible( false );
    ui->lblAddressTestData->setVisible( false );
    // KPC
    ui->lblPrintHeadDots->setVisible( false );
    ui->lblPrintHeadType->setVisible( false );
    ui->lblPrintHeadDotsData->setVisible( false );
    ui->lblPrintHeadTypeData->setVisible( false );

    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    int scaleModelSetting = settings->scaleModel().toInt();

    // Populate the Printer dropdown based on the scaleModel
    ui->cbPrinterSelection->addItem( tr( "Primary" ), 0);

    if(scaleModelSetting == ScaleModel::ScaleModel_EPP )
    {
        ui->cbPrinterSelection->addItem( tr( "Second" ), 1);
        ui->cbPrinterSelection->addItem( tr( "Third" ),  2);
    }
    else if(scaleModelSetting == ScaleModel::ScaleModel_ESC )
    {
        // If we have an HT scale, see if we need to add some EPP printers to the combo
        if ( isEppPrinterAttached() )
            ui->cbPrinterSelection->addItem( "EPP", 1);                // We can only have one EPP printer attached if counter scale, so its the 2nd item
    }

    printHeads_[UNKNOWN_HEAD] = "Unknown Head";
    printHeads_[TDK753_OHM] = "TDK 753 OHM";
    printHeads_[TDK800_OHM] = "TDK 800 OHM";
    printHeads_[TDK849_OHM] = "TDK 849 OHM";
    printHeads_[KYOCERA753_OHM] = "KYOCERA 753 OHM";
    printHeads_[KYOCERA800_OHM] = "KYOCERA 800 OHM";
    printHeads_[KYOCERA849_OHM] = "KYOCERA 849 OHM";
}

void servicePrinterPowerupTests::PrinterChanged()
{
    // Clear the test results data for the current printer
    ui->lblCalculatedChecksumData->clear();
    ui->lblStoredChecksumData->clear();
    ui->lblEEPChecksumTestData->clear();
    ui->lblWordTestData->clear();
    ui->lblAddressTestData->clear();
    ui->lblSoftwareVersionData->clear();
    ui->lblHardwareVersionData->clear();
    ui->lblLabelCountData->clear();

    //--------------------------------------------------
    // Request the printer info for the printer selected
    //--------------------------------------------------
    session_->send( PrinterSystem, PrSysReqStatus, (ui->cbPrinterSelection->itemData(ui->cbPrinterSelection->currentIndex())).toString() );
    session_->send( MfgTestDev, RequestPrinterLabelCount, (ui->cbPrinterSelection->itemData(ui->cbPrinterSelection->currentIndex())).toString() );
}

bool servicePrinterPowerupTests::isEppPrinterAttached()
{
    MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    int scaleModelSetting = settings->scaleModel().toInt();

    bool isAttached = false;

    if(scaleModelSetting == ScaleModel::ScaleModel_ESC )
    {
        if (periphSettings->secondPrinter() == PrinterKind::fieldOffsetEppPrinter  || periphSettings->thirdPrinter()  == PrinterKind::fieldOffsetEppPrinter)
            isAttached = true;

        if (periphSettings->secondPrinter()  == PrinterKind::fieldOffsetEpp2Printer  || periphSettings->thirdPrinter()  == PrinterKind::fieldOffsetEpp2Printer)
            isAttached = true;
    }

    return isAttached;
}
