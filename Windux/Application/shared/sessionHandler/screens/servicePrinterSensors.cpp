
/**
 * @file servicePrinterSensors.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "servicePrinterSensors.h"
#include "ui_servicePrinterSensors.h"
#include <UiUtilities.h>

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"
#include <QTimer>

#include <bitset>

#include "logmanager.h"

#include "../sessionmainwindow.h"


static const int NoPrinter    = 0;          // Not sure if this is even possible, unless we have major issue and cant see it
static const int EPPPrinter   = 1;          // Standalone EPP Printer, but we are running out of parts for it, moving to EPP2.
static const int ApcPrinter   = 4;          // The APC Printer is the normal everyday printer for the HT scales as a primary printer.
static const int EPP2Printer  = 5;          // Newest Atmel EPP Printer which is becoming the newer standard.
static const int SSKPrinter   = 12;         // New free standing self service scale printer
static const QString SelfService    = "1";  // Newest Atmel EPP Printer which is becoming the newer standard.
static const QString AccessWrapper  = "2";  // Access Wrapper
static const QString CounterScale   = "3";  // Newest Atmel EPP Printer which is becoming the newer standard.

servicePrinterSensors::servicePrinterSensors(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::servicePrinterSensors)
{
    ui->setupUi(this);
    scaleScreen();

    initialize();

    bPrintHeadVoltageOn = false;
    cutterAttached_ = false;
    cutter_fired_ = false;

    cutter_state_ = CutterHome;        // default position

    //---------------------------------------------------------
    // Set the connection for when the Printer dropdown changes
    //---------------------------------------------------------
    connect(ui->cbPrinterSelection, SIGNAL( currentIndexChanged(int) ), this, SLOT( PrinterChanged() ) );

    //---------------------------------------------------------------------
    // Set the connection for when the Printhead Voltage setting is toggled
    //---------------------------------------------------------------------
    connect(ui->btnTogglePrintheadVoltage, SIGNAL( clicked() ), this, SLOT( TogglePrintheadVoltage() ) );

    //---------------------------------------------
    // Set the connection for the Test Label button
    //---------------------------------------------
    connect(ui->btnTestLabel, SIGNAL( clicked() ), this, SLOT( PrintTestLabel() ) );

    //---------------------------------------------
    // Set the connection for the Test Cutter button
    //---------------------------------------------
    connect(ui->butTestCutter, SIGNAL( clicked() ), this, SLOT( TestCutter() ) );
    ui->butTestCutter->setVisible( false );
    ui->lblCutterLock->setVisible( false );
    ui->lblCutterLockData->setVisible( false );

    cutterTimer_ = new QTimer( this );
    connect( cutterTimer_, SIGNAL( timeout() ), SLOT( onCutterTimeout() ) );

    connect( ui->butTestEdge, SIGNAL( clicked() ), this, SLOT( TestEdge() ) );
    connect( ui->butTestGap, SIGNAL( clicked() ), this, SLOT( TestGap() ) );

    // Get the info for the Primary printer
    initPrinterValues("0");

    ui->lblLowStockSensor->setEnabled( false );
    ui->lblLowStockSensor->setVisible( false );
    ui->butTestGap->setEnabled( false );
    ui->butTestGap->setVisible( false );
    ui->butTestEdge->setEnabled( false );
    ui->butTestEdge->setVisible( false );

    if( isSSKPrinterAttached() )
    {
        //ui->lblMediaSensor->setText( "Shoot Through" );

        ui->lblLowStockSensor->setEnabled( true );
        ui->lblLowStockSensor->setVisible( true );

        ui->butTestEdge->setEnabled( true );
        ui->butTestEdge->setVisible( true );

        ui->butTestGap->setEnabled( true );
        ui->butTestGap->setVisible( true );
    }

    translate();
}

servicePrinterSensors::~servicePrinterSensors()
{
    delete ui;
}

void servicePrinterSensors::translate()
{
    ui->cbPrinterSelection->setItemText(0, tr( "Primary" ));

    if(scaleModel == "2")
    {
        ui->cbPrinterSelection->setItemText(1, tr( "Second" ));
        ui->cbPrinterSelection->setItemText(2, tr( "Third" ));
    }

    displayLabelStockLengths();

    ui->retranslateUi(this);

    update();
}

//------------------------------------------------------
// Populate the Printer dropdown based on the scaleModel
//------------------------------------------------------
void servicePrinterSensors::initialize()
{
    LOG_CLASS_INFO("servicePrinterSensors", "initialize");
    ui->cbPrinterSelection->addItem( tr( "Primary" ), 0);

    if( session_->isPrepack() )
    {
        ui->cbPrinterSelection->addItem( tr( "Second" ), 1);            // NOTE: I don't think this will work for Access
        ui->cbPrinterSelection->addItem( tr( "Third" ),  2);            // because we may not have both of these attached - jhc
    }
    else
    {
        // If we have an HT scale, see if we need to add some EPP printers to the combo
        if ( isEppPrinterAttached() )
            ui->cbPrinterSelection->addItem( "EPP", 1);                // We can only have one EPP printer attached if counter scale, so its the 2nd item
    }

    ui->cbPrinterSelection->setCurrentIndex(0);

    stockLength = "";
    stockLengthInLines = "";

    if( isSSKPrinterAttached() )
    {
        LOG_INFO("Shoot Through", NULL);
        ui->lblMediaSensor->setText( "Shoot Through" );
    }
    else
    {
        LOG_INFO("Media Sensor", NULL);
        ui->lblMediaSensor->setText( "Media sensor" );
    }
}

//----------------------------------------------------------------
// Clears existing data and sends messages to request setting info
//----------------------------------------------------------------
void servicePrinterSensors::initPrinterValues(QString printerType)
{
    // Clear exiting data
    ui->lblLabelSizeData->clear();
    ui->lblLabelStockBarToBarData->clear();
    ui->lblPrintheadIsData->clear();
    ui->lblPrintheadTempData->clear();
    ui->lblLabelWidthData->clear();
    ui->lblLabelTakenSensorData->clear();
    ui->lblMediaSensorData->clear();
    ui->lblPrintheadVoltageData->clear();

    PrintheadVoltageOff();

    currentPrinter = printerType;

    //--------------------------------------
    // Printer Sensor ON for current printer
    //--------------------------------------
    session_->send( Printer, PrSensorsTracking, currentPrinter + ",1" );

    //----------------------
    // Printhead Voltage OFF
    //----------------------
    session_->send( Printer, PrReqPrintheadPower, currentPrinter + ",0,1" );

    //------------------------------
    // Request Printer Stock Metrics
    //------------------------------
    session_->send( Printer, PrReqStockMetrics, currentPrinter );

    //--------------------------------------
    // Printer status request to see if the cutter is attached
    //--------------------------------------
    session_->send(Printer, PrReqCutterStatus);       // primary printer = 0

    //--------------------------------------
    // Printer status request to check low stock label
    //--------------------------------------
    session_->send(Printer, PrReqStatus);       // primary printer = 0
}

//-------------------------------------------------------------------------
// Send messages to turn off sensor tracking and turn off printhead voltage
//-------------------------------------------------------------------------
void servicePrinterSensors::ClearSettings()
{
    QString param;

    // Turn off sensor tracking for current printer
    param = currentPrinter;
    param += ",0";

    session_->send( Printer, PrSensorsTracking, param);

    if(bPrintHeadVoltageOn)
    {
        // Turn off Printhead voltage for the current printer if it is on
        param = currentPrinter;
        param += ",0,1";

        session_->send( Printer, PrReqPrintheadPower, param);

        bPrintHeadVoltageOn = false;
    }
}

//-------------------------------------------------------------------------
//  Set up the Label Stock length fields. stockLength & stockLength
//  should be set by the Printer, PrStockMetrics message.
//-------------------------------------------------------------------------
void servicePrinterSensors::displayLabelStockLengths()
{
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

    if(stockLength.toLower() == "continuous")
    {
        ui->lblLabelSizeData->setText( tr( "Continuous" ) );
        ui->lblLabelStockBarToBarData->setText( tr( "Continuous" ) );
    }
    else
    {
        if(stockLength.toFloat() > 0.000)
        {
            session_->uiUtilities->youGottaSetTheSeparator( stockLength );
            QString tmpStr = (settings->isScaleMetric() ? " mm." : "\"");
            ui->lblLabelSizeData->setText( stockLength + tmpStr);
        }
        else
        {
            ui->lblLabelSizeData->setText(tr( "Unknown" ) );
        }

        ui->lblLabelStockBarToBarData->setText(session_->uiUtilities->youGottaSetTheSeparator( stockLengthInLines ));
    }
}

bool servicePrinterSensors::onScreenChanging()
{
    ClearSettings();

    return _Mybase::onScreenChanging();
}

void servicePrinterSensors::onSessionMessage( const SessionMessage& msg )
{
    //-------------------------------------------------------------------
    // Printer Stock Metrics
    //-------------------------------------------------------------------
    // Processes message for Label size and Label stock bar-to-bar fields
    //-------------------------------------------------------------------
    if ((Printer == msg[0].toInt() ) && ( PrStockMetrics == msg[1].toInt() ))
    {
        DomPrinterStockMetrics m( msg );

        if(m.printerTypeRaw() == currentPrinter)
        {
            stockLength = m.stockLength();
            stockLengthInLines = m.stockLengthInLinesRaw();

            displayLabelStockLengths();
        }
    }

    //------------------------------------------------------------------------
    // Printer Monitor Sensors (this message are received continuously)
    //------------------------------------------------------------------------
    // Processes message for Printhead is, Printhead temperature, Label width,
    // Label taken sensor, Media sensor and Printhead voltage fields
    //------------------------------------------------------------------------
    if ((Printer == msg[0].toInt() ) && ( PrSensors == msg[1].toInt() ))
    {
        DomPrinterMonitorSensors m( msg );

        // self service free standing
        if( isSSKPrinterAttached() )
        {
            if( m.isPrintheadDisengaged() == "1" )
            {
                ui->butTestCutter->setEnabled( false );
                ui->butTestEdge->setEnabled( false );
                ui->butTestGap->setEnabled( false );
            }
            else
            {
                ui->butTestCutter->setEnabled( true );
                ui->butTestEdge->setEnabled( true );
                ui->butTestGap->setEnabled( true );
            }

            // test bit 0 to see if engine is busy
            std::bitset<sizeof(int)> regTemp ( m.regulatorTemp() );
            bool isEngineBusy = regTemp.test(0);
            if(isEngineBusy)
            {
                ui->butTestEdge->setEnabled( false );
                ui->butTestGap->setEnabled( false );
            }

            ui->lblLowStockData->setText( QString::number( regTemp[1] ) );
        }

        if(m.printerType() == currentPrinter)
        {
            //---------------------------------------
            //-- Printhead is (Engaged/Disengaged) --
            //---------------------------------------

            QString isPrintHeadEngaged = m.isPrintheadDisengaged();
            QString isPrintHeadEngagedText;

            switch(isPrintHeadEngaged.toInt())
            {
                case 0:
                    isPrintHeadEngagedText = tr( "Engaged" );
                    break;

                case 1:
                    isPrintHeadEngagedText = tr( "Disengaged" );
                    break;

                default:
                    isPrintHeadEngagedText = tr( "Unknown" );
                    break;
            }

            ui->lblPrintheadIsData->setText( isPrintHeadEngagedText );

            //---------------------------
            //-- Printhead temperature --
            //---------------------------

#ifdef ENCODING_CHECK		//	This worked for Windows but not Linux. Test this new change under Windows soon.
            // added using ALT+0176 NOT WRAPPING in a string literal - see Service Printer Config AND Help About
            ui->lblPrintheadTempData->setText( m.printheadTemp() + "°C" );
#else
            QChar deg(0x00b0);
            QString tempText(m.printheadTemp());
            tempText.append(" ").append(deg).append("C");
            ui->lblPrintheadTempData->setText( tempText );
#endif

            //-----------------
            //-- Label width --
            //-----------------

            QString labelWidth = m.labelWidth();
            QString labelWidthText;

            switch(labelWidth.toInt())
            {
                case 0:
                    labelWidthText = "2.25";
                    break;

                case 1:
                    labelWidthText = "1.50";
                    break;

                case 2:
                    labelWidthText = "3.0";
                    break;

                default:
                    labelWidthText = tr( "Unknown" );
                    break;
            }

            ui->lblLabelWidthData->setText( session_->uiUtilities->youGottaSetTheSeparator( labelWidthText) + "\"" );

            //------------------------
            //-- Label taken sensor --
            //------------------------

            QString labelSensorReading = m.labelTakenSensor();
            int isBlocking;

            if(labelSensorReading.toInt() >= 250)
                isBlocking = 0;
            else
                isBlocking = 1;

            ui->lblLabelTakenSensorData->setText((isBlocking == 0 ? tr( "Label taken" ) : tr( "Label blocking sensor ..." ) ) );

            //------------------
            //-- Media sensor --
            //------------------

            if( isSSKPrinterAttached() )
            {
                ui->lblMediaSensor->setText( "Shoot Through" );
            }

            ui->lblMediaSensorData->setText( m.mediaSensor() );

            //-----------------------
            //-- Printhead voltage --
            //-----------------------

            QString printHeadVoltage = m.printheadVoltage();

            if(bPrintHeadVoltageOn)
            {
                if(printHeadVoltage.toFloat() == -1.0)
                    ui->lblPrintheadVoltageData->setText( "N/A" );
                else
                    ui->lblPrintheadVoltageData->setText( printHeadVoltage );
            }
            else
            {
                ui->lblPrintheadVoltageData->setText( tr( "Off" ) );
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    if ((Printer == msg[0].toInt() ) && ( PrCutterStatus == msg[1].toInt() ))
    {
        DomPrinterCutterStatus m( msg );

        cutterAttached_ = m.installed();        // based on a wire - physical presence, its there or not

        if( cutterAttached_ )
        {
            ui->butTestCutter->setVisible( true );          // just in case it comes back
            ui->lblCutterLock->setVisible( true );
            ui->lblCutterLockData->setVisible( true );

            if ( cutter_fired_ )
            {
                if( (cutter_state_ == CutterHome) && ( !m.home() ) )
                {
                    // we were at home but cutter left home to cut
                    // onSessionMessage PrCutterStatus CutterLeftHome && has not returned yet
                    cutter_state_ = CutterLeftHome;
                }
                else if( (cutter_state_ == CutterHome) && ( m.home() ) )
                {
                    // means we have sent cut command but it has not started cutting yet
                    // onSessionMessage PrCutterStatus CutterHome and still home - cant really happen tho
                }
                else if( (cutter_state_ == CutterLeftHome) && ( m.home() ) )
                {
                    // means we have returned home after cutting and the test passes
                    cutter_fired_ = false;
                    cutter_state_ = CutterHome;
                }
                else if(  m.jammed() || (!m.interlock()) )
                {
                    // means we are jammed. Could of been jammed to start or while testing. Report it
                    cutter_fired_ = false;
                    if( m.home() )
                        cutter_state_ = CutterHome;
                    else
                        cutter_state_ = CutterLeftHome;

                    if( m.jammed() )
                        session_->send(Error,ImportDev, "7031,5");      // tell user we are jammed

                    if( false == m.interlock() )
                        session_->send(Error,ImportDev, "7032,5");      // then we must have an open interlock
                }
            }

            // set cutter interlock state
            if( false == m.interlock() )
                ui->lblCutterLockData->setText( tr("Cutter Interlock Open") );
            else
                ui->lblCutterLockData->setText( tr("Cutter Interlock Closed") );
        }
        else
        {
            ui->butTestCutter->setVisible( false );
            ui->lblCutterLock->setVisible( false );
            ui->lblCutterLockData->setVisible( false );
        }
    }
}

//-----------------------------------------------
// Slot for when the Printer selection is changed
//-----------------------------------------------
void servicePrinterSensors::PrinterChanged()
{
    ClearSettings();

    initPrinterValues((ui->cbPrinterSelection->itemData(ui->cbPrinterSelection->currentIndex())).toString());
}

//----------------------------------------------------------------
// Slot for when "Turn Printhead voltage ON/OFF" button is clicked
//----------------------------------------------------------------
void servicePrinterSensors::TogglePrintheadVoltage()
{
    QString param;

    param = currentPrinter;

    if(bPrintHeadVoltageOn)
    {
        // Turn Printhead voltage OFF
        param += ",0,1";

        session_->send( Printer, PrReqPrintheadPower, param);

        PrintheadVoltageOff();
    }
    else
    {
        // Turn Printhead voltage ON
        param += ",1,1";

        session_->send( Printer, PrReqPrintheadPower, param);

        PrintheadVoltageOn();
    }
}

//---------------------------------------------------
// Toggles Printhead voltage settings to OFF position
//---------------------------------------------------
void servicePrinterSensors::PrintheadVoltageOff()
{
    bPrintHeadVoltageOn = false;

    ui->lblPrintheadVoltageData->setText( tr( "Off" ) );

    ui->btnTogglePrintheadVoltage->setText( tr( "Turn Printhead voltage ON" ) );
}

//--------------------------------------------------
// Toggles Printhead voltage settings to ON position
//--------------------------------------------------
void servicePrinterSensors::PrintheadVoltageOn()
{
    bPrintHeadVoltageOn = true;

    ui->btnTogglePrintheadVoltage->setText( tr( "Turn Printhead voltage OFF" ) );
}

//-------------------------------------------------
// Slot for when the "Test Label" button is clicked
//-------------------------------------------------
void servicePrinterSensors::PrintTestLabel()
{
    QString param;

    param = currentPrinter;
    param += ",2,0"; // Label test pattern: 2=Checkerboard, 3=Horizontal Bars, 4=Vertical Lines, 5=High Power

    session_->send( Printer, PrTestLabel, param );
}

//-------------------------------------------------
// Slot for when the "Test Cutter" button is clicked
//-------------------------------------------------
void servicePrinterSensors::TestCutter()
{
    cutter_state_ = CutterHome;                        // start position
    cutter_fired_ = true;
    session_->send( Printer, PrCutterCut );             // send a cut command

    cutterTimer_->setSingleShot( true );
    cutterTimer_->start( 3000 );
}

void servicePrinterSensors::onCutterTimeout()
{
    // A command has been sent to cut.  Need to find out if it did and the cutter moved from home
    if( ( cutter_fired_) && ( CutterHome == cutter_state_)  )
    {
        session_->send(Error,ImportDev, "7033,5");      // tell user we timed out trying to spin/engage the cutter
        cutter_fired_ = false;
    }
}

void servicePrinterSensors::TestEdge()
{
    session_->send( Printer, PrTest, "2" );

    ui->butTestEdge->setEnabled( false );
}

void servicePrinterSensors::TestGap()
{
    session_->send( Printer, PrTest, "1" );

    ui->butTestGap->setEnabled( false );
}

bool servicePrinterSensors::isEppPrinterAttached()
{
    MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString scaleModelSetting = settings->scaleModel();

    bool isAttached = false;

    if(scaleModelSetting == CounterScale)
    {
        if (periphSettings->secondPrinter()  == EPPPrinter  || periphSettings->thirdPrinter()  == EPPPrinter)
            isAttached = true;

        if (periphSettings->secondPrinter()  == EPP2Printer  || periphSettings->thirdPrinter()  == EPP2Printer)
            isAttached = true;
    }

    return isAttached;
}

bool servicePrinterSensors::isSSKPrinterAttached()
{
    LOG_CLASS_INFO("servicePrinterSensors", "isSSKPrinterAttached");

    MadpPeripheralList *periphList = &session_->scaleConfig->peripheralList;

    bool isAttached = false;

    LOG_INFO("firstPrinter() = %d", periphList->firstPrinter());
    if (periphList->firstPrinter() == SSKPrinter)
    {
        isAttached = true;
    }

    return isAttached;
}
