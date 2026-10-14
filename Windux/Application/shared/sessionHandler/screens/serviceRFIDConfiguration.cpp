/**
 * @file serviceRFIDConfiguration.h
 * @brief ...describe...
 *
 * @copyright Copyright (c) 2026, ITW/Hobart, Food Equipment Group, U.S.A.
 * @Author: Chris King
 * @issues:
 * @{
*/

#include <logmanager.h>

#include <QtGui>
#include <QtCore>

#include "../sessionmainwindow.h"
#include "../modes/supervisormode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "TokenParser.h"

#include "serviceRFIDConfiguration.h"
#include "ui_serviceRFIDConfiguration.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceRFIDConfiguration::serviceRFIDConfiguration(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceRFIDConfiguration)
{
    LOG_CLASS_INFO( "serviceRFIDConfiguration", "serviceRFIDConfiguration" );

    ui->setupUi(this);
    scaleScreen();
    initialize();
    translate();

    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceRFIDConfiguration::~serviceRFIDConfiguration()
{
    LOG_CLASS_INFO( "serviceRFIDConfiguration", "~serviceRFIDConfiguration" );

    delete ui;
    
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceRFIDConfiguration::initialize()
{
    LOG_CLASS_INFO( "serviceRFIDConfiguration", "initialize" );

    ui->lblTitleBar->setText( tr( "RFID Configuration" ) );
    ui->lblInstructionsLine1->setText( tr( "If the firmware needs to be updated, press Update Firmware first" ) );
    ui->lblInstructionsLine2->setText( tr( "Press the Start Test button to initiate RFID read/write test" ) );
    ui->lblFirmwareUpdateMsg->setText( tr( "RFID Firmware Update:" ) );
    ui->lblTestResultsMsg->setText( tr( "RFID Read/Write Test Result:" ) );
    ui->lblCalResultsMsg->setText( tr( "RFID Calibration Result:" ) );
    ui->lblLogTitle->setText( tr( "Log" ) );
    ui->lblModuleConnectedMsg->setText( tr( "Module Connection Status" ) );
    ui->lblModuleConnectedResult->setText( tr( "Disconnected" ) );

    ui->lblFirmwareUpdateResult->setVisible( false );
    ui->lblTestResultsResult->setVisible( false );
    ui->lblCalResultsResult->setVisible( false );

    ui->lblFirmwareUpdateResult->clear();
    ui->lblTestResultsResult->clear();
    ui->lblCalResultsResult->clear();

    ui->btnStartRFIDTest->setEnabled( false );
    ui->btnCalibrateRFID->setEnabled( false );
    ui->btnUpdateRFIDFirmware->setEnabled( false );

    //ui->pteRFIDLog->setLineWrapMode(QPlainTextEdit::NoWrap);
    ui->pteRFIDLog->setMaximumBlockCount(5000); 

    rfidLogFile.setFileName("/usr/share/rfid-hook/rfid_log.txt");

    if (rfidLogFile.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        rfidLogTextStream.setDevice(&rfidLogFile);
        ui->pteRFIDLog->setPlainText(rfidLogTextStream.readAll());
        lastRfidLogPosition = rfidLogFile.pos();
    }

    ui->pteRFIDLog->moveCursor(QTextCursor::End);
    ui->pteRFIDLog->ensureCursorVisible();

    rfidLogTimer = new QTimer(this);
    connect(rfidLogTimer, &QTimer::timeout, this, &serviceRFIDConfiguration::readNewLogData);
    connect(ui->btnUpdateRFIDFirmware, SIGNAL( clicked() ), this, SLOT( onUpdateRFIDFirmwareClicked() ) );
    connect(ui->btnStartRFIDTest,      SIGNAL( clicked() ), this, SLOT( onStartRFIDTestClicked() ) );
    connect(ui->btnCalibrateRFID,      SIGNAL( clicked() ), this, SLOT( onCalibrateRFIDClicked() ) );

    rfidLogTimer->start(500);

    screenBusy = false;

    session_->send( Printer, PrRfidQryRequest );

    update();

    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceRFIDConfiguration::translate()
{
    LOG_CLASS_INFO( "serviceRFIDConfiguration", "translate" );

    update();
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceRFIDConfiguration::helpText( QString& title, QString& description ) const
{
    LOG_CLASS_INFO( "serviceRFIDConfiguration", "helpText" );
    Q_UNUSED( title );
    Q_UNUSED( description );

    LOG_TRACE( "Done", NULL );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceRFIDConfiguration::onScreenChanging()
{
    LOG_CLASS_INFO( "serviceRFIDConfiguration", "onScreenChanging" );

    if(screenBusy == true)
    {
        QMessageBox msgBox;
        msgBox.setText( tr("RFID Configuration screen is busy.") );
        msgBox.setWindowTitle("Hidden Title"); 
        msgBox.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        msgBox.exec();

        return false;
    }
    else
    {
        //turn off qry timer
        //session_->send( Printer, PrRfidQryRequest );

        return _Mybase::onScreenChanging();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
//                                    PRIVATE METHODS                                               //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceRFIDConfiguration::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "serviceRFIDConfiguration", "onSessionMessage" );

    if ( Printer == msg[0].toInt() )
    {
        if ( PrRfidTestPassed == msg[1].toInt() )
        {
            ui->lblTestResultsResult->setText( tr( "PASS" ) );
            ui->lblTestResultsResult->setVisible( true );

            ui->btnStartRFIDTest->setEnabled( true );
            ui->btnCalibrateRFID->setEnabled( true );
            ui->btnUpdateRFIDFirmware->setEnabled( true );

            screenBusy = false;
        }
        else if ( PrRfidTestFailed == msg[1].toInt() )
        {
            ui->lblTestResultsResult->setText( tr( "FAIL" ) );
            ui->lblTestResultsResult->setVisible( true );

            ui->btnStartRFIDTest->setEnabled( true );
            ui->btnCalibrateRFID->setEnabled( true );
            ui->btnUpdateRFIDFirmware->setEnabled( true );

            screenBusy = false;
        }
        else if ( PrRfidCalPassed == msg[1].toInt() )
        {
            ui->lblCalResultsResult->setText( tr( "PASS" ) );
            ui->lblCalResultsResult->setVisible( true );

            ui->btnStartRFIDTest->setEnabled( true );
            ui->btnCalibrateRFID->setEnabled( true );
            ui->btnUpdateRFIDFirmware->setEnabled( true );

            screenBusy = false;
        }
        else if ( PrRfidCalFailed == msg[1].toInt() )
        {
            ui->lblCalResultsResult->setText( tr( "FAIL" ) );
            ui->lblCalResultsResult->setVisible( true );

            ui->btnStartRFIDTest->setEnabled( true );
            ui->btnCalibrateRFID->setEnabled( true );
            ui->btnUpdateRFIDFirmware->setEnabled( true );

            screenBusy = false;
        }
        else if ( PrRfidUpgradePassed == msg[1].toInt() )
        {
            ui->lblFirmwareUpdateResult->setText( tr( "PASS" ) );
            ui->lblFirmwareUpdateResult->setVisible( true );

            ui->btnStartRFIDTest->setEnabled( true );
            ui->btnCalibrateRFID->setEnabled( true );
            ui->btnUpdateRFIDFirmware->setEnabled( true );

            screenBusy = false;
        }
        else if ( PrRfidUpgradeFailed == msg[1].toInt() )
        {
            ui->lblFirmwareUpdateResult->setText( tr( "FAIL" ) );
            ui->lblFirmwareUpdateResult->setVisible( true );

            ui->btnStartRFIDTest->setEnabled( true );
            ui->btnCalibrateRFID->setEnabled( true );
            ui->btnUpdateRFIDFirmware->setEnabled( true );

            screenBusy = false;
        }
        else if ( PrRfidQryPassed == msg[1].toInt() )
        {
            LOG_INFO( "PrRfidQryPassed", NULL );

            ui->lblModuleConnectedResult->setText( tr( "Connected" ) );

            ui->btnStartRFIDTest->setEnabled( true );
            ui->btnCalibrateRFID->setEnabled( true );
            ui->btnUpdateRFIDFirmware->setEnabled( true );
        }
        else if ( PrRfidQryFailed == msg[1].toInt() )
        {
            LOG_INFO( "PrRfidQryFailed", NULL );

            ui->lblModuleConnectedResult->setText( tr( "Error" ) );

            ui->btnStartRFIDTest->setEnabled( false );
            ui->btnCalibrateRFID->setEnabled( false );
            ui->btnUpdateRFIDFirmware->setEnabled( false );

            screenBusy = false;
        }
        else if ( PrRfidQryBusy == msg[1].toInt() )
        {
            LOG_INFO( "PrRfidQryBusy", NULL );

            ui->lblModuleConnectedResult->setText( tr( "Busy" ) );

            ui->btnStartRFIDTest->setEnabled( false );
            ui->btnCalibrateRFID->setEnabled( false );
            ui->btnUpdateRFIDFirmware->setEnabled( false );
        }
    }

    LOG_TRACE( "Done", NULL );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
//                                   SLOT METHODS                                                   //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceRFIDConfiguration::onUpdateRFIDFirmwareClicked()
{
    LOG_CLASS_INFO( "serviceRFIDConfiguration", "onUpdateRFIDFirmwareClicked" );

    //LOG_INFO("firstPrinter() = %d", periphList->firstPrinter());
    LOG_INFO("updating RFID firmware...", NULL);

    ui->lblFirmwareUpdateResult->setText( tr( "Upgrading..." ) );
    ui->lblFirmwareUpdateResult->setVisible( true );

    ui->btnStartRFIDTest->setEnabled( false );
    ui->btnCalibrateRFID->setEnabled( false );
    ui->btnUpdateRFIDFirmware->setEnabled( false );

    screenBusy = true;

    session_->send( Printer, PrRfidUpgradeRequest );

    LOG_TRACE( "Done", NULL );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceRFIDConfiguration::onStartRFIDTestClicked()
{
    LOG_CLASS_INFO( "serviceRFIDConfiguration", "onStartRFIDTestClicked" );
    LOG_INFO("starting RFID test...", NULL);

    ui->lblTestResultsResult->setText( tr( "Testing..." ) );
    ui->lblTestResultsResult->setVisible( true );

    ui->btnStartRFIDTest->setEnabled( false );
    ui->btnCalibrateRFID->setEnabled( false );
    ui->btnUpdateRFIDFirmware->setEnabled( false );

    screenBusy = true;

    session_->send( Printer, PrRfidTestRequest );

    LOG_TRACE( "Done", NULL );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceRFIDConfiguration::onCalibrateRFIDClicked()
{
    LOG_CLASS_INFO( "serviceRFIDConfiguration", "onCalibrateRFIDClicked" );
    LOG_INFO("calibrating RFID...", NULL);

    ui->lblCalResultsResult->setText( tr( "Calibrating..." ) );
    ui->lblCalResultsResult->setVisible( true );

    ui->btnStartRFIDTest->setEnabled( false );
    ui->btnCalibrateRFID->setEnabled( false );
    ui->btnUpdateRFIDFirmware->setEnabled( false );

    screenBusy = true;

    session_->send( Printer, PrRfidCalRequest );

    LOG_TRACE( "Done", NULL );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceRFIDConfiguration::readNewLogData() 
{
    if (!rfidLogFile.isOpen()) return;

    rfidLogFile.flush(); 

    qint64 currentSize = rfidLogFile.size();

    if (currentSize < lastRfidLogPosition) 
    {
        lastRfidLogPosition = 0; 
    }

    rfidLogFile.seek(lastRfidLogPosition);

    if (!rfidLogFile.atEnd()) 
    {
        QString newData = rfidLogTextStream.readAll();
        
        ui->pteRFIDLog->appendPlainText(newData.trimmed());

        //ui->pteRFIDLog->moveCursor(QTextCursor::End);

        //ui->pteRFIDLog->insertPlainText(newData.trimmed());

        lastRfidLogPosition = rfidLogFile.pos(); 
        
        QScrollBar *vScrollBar = ui->pteRFIDLog->verticalScrollBar();
        vScrollBar->setValue(vScrollBar->maximum());
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
