/**
 * @file servicePrinterConfig.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "servicePrinterConfig.h"
#include "setupUtility.h"
#include "ui_servicePrinterConfig.h"

#include <UiUtilities.h>

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../dialogs/printersettingsdialog.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include <logmanager.h>

#include <QIntValidator>

static const int NoPrinter    = 0;          // Not sure if this is even possible, unless we have major issue and cant see it
static const int EPPPrinter   = 1;          // Standalone EPP Printer, but we are running out of parts for it, moving to EPP2.
static const int ApcPrinter   = 4;          // The APC Printer is the normal everyday printer for the HT scales as a primary printer.
static const int EPP2Printer  = 5;          // Newest Atmel EPP Printer which is becoming the newer standard.
static const int UsbPrinter   = 6;          // The APC Printer is the normal everyday printer for the HT scales as a primary printer.
static const int NetPrinter   = 7;          // Newest Atmel EPP Printer which is becoming the newer standard.
static const int SSKPrinter   = 12;         // New free standing self service scale printer

static const QString SelfService    = "1";  // Newest Atmel EPP Printer which is becoming the newer standard.
static const QString AccessWrapper  = "2";  // Access Wrapper
static const QString CounterScale   = "3";  // Newest Atmel EPP Printer which is becoming the newer standard.


servicePrinterConfig::servicePrinterConfig(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::servicePrinterConfig)
{
    ui->setupUi(this);
    scaleScreen();

    // Set up validator for Continuous Labels Expel Position, Vertical Print Position
    // and Horizontal Print Position fields using the Numeric Line Edit
    ui->txtContinuousExpelPosition->setValidator(continuousLabelsExpelPositionValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtContinuousExpelPosition->setDecimalPlaces(0);
    ui->txtContinuousExpelPosition->setInputWrapping(true);
    continuousLabelsExpelPositionValidator_->setTop(makeValidatorTop( 3 ));

    ui->txtVerticalPrintPosition->setValidator(verticalPrintPositionValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtVerticalPrintPosition->setDecimalPlaces(0);
    ui->txtVerticalPrintPosition->setInputWrapping(true);
    verticalPrintPositionValidator_->setTop(makeValidatorTop( 2 ));

    ui->txtHorizontalPrintPosition->setValidator(horizontalPrintPositionValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtHorizontalPrintPosition->setDecimalPlaces(0);
    ui->txtHorizontalPrintPosition->setInputWrapping(true);
    horizontalPrintPositionValidator_->setTop(makeValidatorTop( 2 ));

    initialize();

    //---------------------------------------------------------
    // Set the connection for when the Printer dropdown changes
    //---------------------------------------------------------
    connect(ui->cbPrinterSelection, SIGNAL( currentIndexChanged(int) ), this, SLOT( printerChanged(int) ) );

    //------------------------------------------------------------------
    // Set the connections for when a one of the setup fields is changed.
    // These all call the same method to set a property that indicates
    // something in the setup changed and the data needs to be saved.
    //------------------------------------------------------------------
    connect( ui->cbPrintContrast,            SIGNAL( currentIndexChanged(int) ), this,   SLOT( setupChanged() ) );
    connect( ui->txtVerticalPrintPosition,   SIGNAL( editingFinished() ),        this,   SLOT( setupChanged() ) );
    connect( ui->cbLabelWidth,               SIGNAL( currentIndexChanged(int) ), this,   SLOT( setupChanged() ) );
    connect( ui->txtHorizontalPrintPosition, SIGNAL( editingFinished() ),        this,   SLOT( setupChanged() ) );
    connect( ui->txtContinuousExpelPosition, SIGNAL( editingFinished() ),        this,   SLOT( setupChanged() ) );
    connect( ui->cboMediaSensorType,         SIGNAL( currentIndexChanged(int) ), this,   SLOT( setupChanged() ) );

    //---------------------------------------------------
    // Set connection for when Test Label Pattern changes
    //---------------------------------------------------
    connect( ui->cbTestLabelPattern, SIGNAL( currentIndexChanged(int) ), this, SLOT( updateTestPatternLabel(int) ) );

    //---------------------------------------
    // Set connections for the action buttons
    //---------------------------------------
    connect( ui->btnPrintOne, SIGNAL( clicked() ), this, SLOT( printSingleTestLabel() ) );
    connect( ui->btnPrintMultiple, SIGNAL( clicked() ), this, SLOT( printMultipleTestLabels() ) );
    connect( ui->btnStopMultiple, SIGNAL( clicked() ), this, SLOT( stopMultipleTestLabels() ) );
    connect( ui->btnCalibrateMediaSensor, SIGNAL( clicked() ), this, SLOT( calibrateMediaSensor() ) );
    connect( ui->btnCalibrateLabelTakenSensor, SIGNAL( clicked() ), this, SLOT( calibrateLabelTakenSensor() ) );
    connect( ui->btnAdvanceStock, SIGNAL( clicked() ), this, SLOT( advanceLabelStock() ) );
    connect( ui->btnCalibShootThrough, SIGNAL( clicked() ), this, SLOT( calibrateShootThrough() ) );

    //---------------------------------------------------
    // Set the connection for the Printer Settings button
    //---------------------------------------------------
    // FT-636
    connect( ui->btnOperatorPrinterSettings, SIGNAL( clicked() ), this, SLOT( printerSettings() ) );

    // SOF-3853: Find out if a Cutter is attached
    cutterAttached_ = false;
    session_->send( Printer, PrReqCutterStatus );

    printheadDisengaged_ = false;
    isShootThruSensor_ = 0;

    // Get the info for the Primary printer
    initPrinterValues("0");

    bSetupChanged = false;

    translate();
}

servicePrinterConfig::~servicePrinterConfig()
{
    delete ui;
}

void servicePrinterConfig::translate()
{
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString scaleModelSetting = settings->scaleModel();

    // Test Label Pattern
    ui->cbTestLabelPattern->setItemText(0, tr("Checker Board"));
    ui->cbTestLabelPattern->setItemText(1, tr("Horizontal Bars"));
    ui->cbTestLabelPattern->setItemText(2, tr("Vertical Lines"));
    ui->cbTestLabelPattern->setItemText(3, tr("High Power"));

    // Printer Dialog
    ui->cbPrinterSelection->setItemText( 0, tr( "Primary" ) );

    if(scaleModelSetting == AccessWrapper)
    {
        ui->cbPrinterSelection->setItemText( 1, tr( "Second" ) );
        ui->cbPrinterSelection->setItemText( 2, tr( "Third" )  );
    }
    else if(scaleModelSetting == CounterScale)
    {
        // If we have an HT scale, see if we need to add some EPP printers to the combo
        if ( isEppPrinterAttached(2) )
            ui->cbPrinterSelection->setItemText( 1, "EPP" );                // We can only have one EPP printer attached if counter scale, so its the 2nd item
        else if( isUsbPrinterAttached(2) )
            ui->cbPrinterSelection->setItemText( 1, "USB" );

        if ( isEppPrinterAttached(3) )
            ui->cbPrinterSelection->setItemText( 2, "EPP" );
        else if( isUsbPrinterAttached(3) )
            ui->cbPrinterSelection->setItemText( 2, "USB" );
    }

    ui->retranslateUi(this);

    update();
}

//-------------------------------------------------------------------------------
// Used with numericLineEdit widget to validate Continuous Labels Expel Position,
// Vertical Print Position and Horizontal Print Position
//-------------------------------------------------------------------------------
int servicePrinterConfig::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

//---------------------
// Initialize dropdowns
//---------------------
void servicePrinterConfig::initialize()
{
    LOG_CLASS_INFO("servicePrinterConfig", "initialize");

    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

    // Printer selection
    ui->cbPrinterSelection->addItem( tr( "Primary" ), 0);

    if(session_->isPrepack())
    {
        ui->cbPrinterSelection->addItem( tr( "Second" ), 1);
        ui->cbPrinterSelection->addItem( tr( "Third" ),  2);
    }
    else
    {
        // If we have an HT scale, see if we need to add some EPP printers to the combo
        if ( isEppPrinterAttached(2) )
            ui->cbPrinterSelection->addItem( "EPP", 1);                // We can only have one EPP printer attached if counter scale, so its the 2nd item
        else if( isUsbPrinterAttached(2) )
            ui->cbPrinterSelection->addItem( "USB", 1);

        if ( isEppPrinterAttached(3) )
            ui->cbPrinterSelection->addItem( "EPP", 2);
        else if( isUsbPrinterAttached(3) )
            ui->cbPrinterSelection->addItem( "USB", 2);
    }

    // Print Contrast
    ui->cbPrintContrast->addItem( "0", 0 );
    ui->cbPrintContrast->addItem( "1", 1 );
    ui->cbPrintContrast->addItem( "2", 2 );
    ui->cbPrintContrast->addItem( "3", 3 );
    ui->cbPrintContrast->addItem( "4", 4 );
    ui->cbPrintContrast->addItem( "5", 5 );
    ui->cbPrintContrast->addItem( "6", 6 );
    ui->cbPrintContrast->addItem( "7", 7 );

    // Label Width
    QString labelWidthText;

    labelWidthText = (settings->isScaleMetric()) ? "38.100" : "1.50";
    session_->uiUtilities->youGottaSetTheSeparator( labelWidthText );
    ui->cbLabelWidth->addItem( labelWidthText, 1 );
    labelWidthText = (settings->isScaleMetric()) ? "57.150" : "2.25";
    session_->uiUtilities->youGottaSetTheSeparator( labelWidthText );
    ui->cbLabelWidth->addItem( labelWidthText, 0 );
    labelWidthText = (settings->isScaleMetric()) ? "76.200" : "3.00";
    session_->uiUtilities->youGottaSetTheSeparator( labelWidthText );
    ui->cbLabelWidth->addItem( labelWidthText,  2 );

    // Test Label Pattern
    ui->cbTestLabelPattern->addItem( tr( "Checker Board" ),   2 );
    ui->cbTestLabelPattern->addItem( tr( "Horizontal Bars" ), 3 );
    ui->cbTestLabelPattern->addItem( tr( "Vertical Lines" ),  4 );
    ui->cbTestLabelPattern->addItem( tr( "High Power" ),      5 );

    // Media Sensor Type
    ui->cboMediaSensorType->addItem( tr( "Reflective" ), 0 );
    ui->cboMediaSensorType->addItem( tr( "Shoot thru" ), 1 );

    //HA-138: This variable wasn't initialized
    currentTestLabel = (ui->cbTestLabelPattern->itemData( ui->cbTestLabelPattern->currentIndex() )).toString();

    calDialog = NULL;
    calInitDialog = NULL;
    calBackingDialog = NULL;

    shootThroughCalStarted = false;
    ui->btnCalibShootThrough->setEnabled( true );
    ui->btnCalibShootThrough->setText("Calibrate Shoot Through");

    if( isSSKPrinterAttached() )
    {
        LOG_INFO("SSK printer is attached", NULL);

        ui->lblMediaSensor->setText( "Shoot Through" );
        ui->btnCalibShootThrough->setHidden( false );
    }
    else
    {
        LOG_INFO("SSK printer is NOT attached", NULL);
        ui->lblMediaSensor->setText( "Media Sensor" );
        ui->btnCalibShootThrough->setVisible( false );
        ui->btnCalibShootThrough->setEnabled( false );
        ui->lblStatus->setVisible( false );
        ui->lblStatus->setEnabled( false );
        ui->lblStatusValue->setVisible( false );
        ui->lblStatusValue->setEnabled( false );
        ui->lblBackingVoltage->setVisible( false );
        ui->lblBackingVoltage->setEnabled( false );
        ui->lblBackingVoltageValue->setVisible( false );
        ui->lblBackingVoltageValue->setEnabled( false );
        ui->lblLabelVoltage->setVisible( false );
        ui->lblLabelVoltage->setEnabled( false );
        ui->lblLabelVoltageValue->setVisible( false );
        ui->lblLabelVoltageValue->setEnabled( false );
        ui->lblDriveCurrent->setVisible( false );
        ui->lblDriveCurrent->setEnabled( false );
        ui->lblDriveCurrentValue->setVisible( false );
        ui->lblDriveCurrentValue->setEnabled( false );
        ui->lblDeflectionVoltage->setVisible( false );
        ui->lblDeflectionVoltage->setEnabled( false );
        ui->lblDeflectionVoltageValue->setVisible( false );
        ui->lblDeflectionVoltageValue->setEnabled( false );
        ui->btnCalibrateMediaSensor->setVisible( false );
        ui->btnCalibrateMediaSensor->setEnabled( false );
    }
}

void servicePrinterConfig::hideAllFields()
{
    ui->lblPrintContrast->hide();
    ui->cbPrintContrast->hide();

    ui->lblVerticalPrintPosition->hide();
    ui->txtVerticalPrintPosition->hide();

    ui->lblLabelWidth->hide();
    ui->cbLabelWidth->hide();

    ui->lblMediaSensorType->hide();
    ui->cboMediaSensorType->hide();

    ui->lblHorizontalPrintPosition->hide();
    ui->txtHorizontalPrintPosition->hide();

    ui->lblContinuousLabelExpel->hide();
    ui->txtContinuousExpelPosition->hide();

    ui->lblTestLabelPattern->hide();
    ui->cbTestLabelPattern->hide();

    ui->btnPrintOne->hide();
    ui->btnPrintMultiple->hide();
    ui->btnStopMultiple->hide();

    ui->lblMediaSensor->hide();
    ui->lblMediaSensorData->hide();

    ui->lblLabelTakenSensor->hide();
    ui->lblLabelTakenSensorData->hide();

    ui->lblPrintheadTemp->hide();
    ui->lblPrintheadTempData->hide();

    ui->lblLabelSize->hide();
    ui->lblLabelSizeData->hide();

    ui->lblPrintLines->hide();
    ui->lblPrintLinesData->hide();

    ui->btnCalibrateMediaSensor->hide();
    ui->btnCalibrateLabelTakenSensor->hide();
    ui->btnAdvanceStock->hide();
    ui->btnOperatorPrinterSettings->hide();
}

void servicePrinterConfig::showAllFields()
{
    ui->lblPrintContrast->show();
    ui->cbPrintContrast->show();

    ui->lblVerticalPrintPosition->show();
    ui->txtVerticalPrintPosition->show();

    ui->lblLabelWidth->show();
    ui->cbLabelWidth->show();

    if( isSTEppPrinter() )
    {
        ui->lblMediaSensorType->show();
        ui->cboMediaSensorType->show();
    }
    else
    {
        ui->lblMediaSensorType->hide();
        ui->cboMediaSensorType->hide();     // should of already been hidden
    }

    ui->lblHorizontalPrintPosition->show();
    ui->txtHorizontalPrintPosition->show();

    // Continuous Labels Expel Position - only displayed if scaleModel not equal to 2
    if(!session_->isPrepack())
    {
        ui->lblContinuousLabelExpel->show();
        ui->txtContinuousExpelPosition->show();
    }

    ui->lblTestLabelPattern->show();
    ui->cbTestLabelPattern->show();

    ui->btnPrintOne->show();
    ui->btnPrintMultiple->show();
    ui->btnStopMultiple->show();

    //ui->lblMediaSensor->show();
    ui->lblMediaSensor->show();
    ui->lblMediaSensorData->show();

    ui->lblLabelTakenSensor->show();
    ui->lblLabelTakenSensorData->show();

    ui->lblPrintheadTemp->show();
    ui->lblPrintheadTempData->show();

    ui->lblLabelSize->show();
    ui->lblLabelSizeData->show();

    ui->lblPrintLines->show();
    ui->lblPrintLinesData->show();

    if( !cutterAttached_ && !isSSKPrinterAttached() ) // SOF-3853
        ui->btnCalibrateMediaSensor->show();

    // ui->btnCalibrateLabelTakenSensor->show(); // FT-577
    ui->btnAdvanceStock->show();
    ui->btnOperatorPrinterSettings->show();
}

//----------------------------------------------------------------
// Clears existing data and sends messages to request setting info
//----------------------------------------------------------------
void servicePrinterConfig::initPrinterValues(QString printerType)
{
    hideAllFields();

    labelWidthEnabledEnabled(printerType);

    currentPrinter = printerType;
    slaValue = "0";

    //--------------------------
    // Request a Printer Inquiry
    //--------------------------
    session_->send( Printer, PrReqInquire, currentPrinter );

    //------------------------------
    // Request Printer Stock Metrics
    //------------------------------
    session_->send( Printer, PrReqStockMetrics, currentPrinter );

    //--------------------------------------
    // Printer Sensor ON for current printer
    //--------------------------------------
    session_->send( Printer, PrSensorsTracking, currentPrinter + ",1" );

    // Get the attached printers
    session_->send( System, SysReqPeripheralList );                     //11, 101 - Request current peripheral list
}

//------------------------------------------------------------------
// Determines if the Label Width dropdown should be enabled/disabled
//------------------------------------------------------------------
void servicePrinterConfig::labelWidthEnabledEnabled( QString /*printerType*/ )
{
    if( isFirstPrinterEpp() || isEppPrinterAttached(2) || isEppPrinterAttached(3) )
    {
        ui->cbLabelWidth->setEnabled(true);		//	This should be enabled for Prepack
    }
    else
    {
        ui->cbLabelWidth->setDisabled(true);	//	Disabled, apparently for ServiceCounter
    }
}

bool servicePrinterConfig::onScreenChanging()
{
    clearMessages();

    saveDataOnExit();

    return _Mybase::onScreenChanging();
}

void servicePrinterConfig::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "servicePrinterConfig", "onSessionMessage" );

    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

    //--------------------------------------------------------
    // Printer Stock Metrics
    //--------------------------------------------------------
    // Processes message for Label Size and Print Lines fields
    //--------------------------------------------------------
    if ((Printer == msg[0].toInt() ) && ( PrStockMetrics == msg[1].toInt() ))
    {
        DomPrinterStockMetrics m( msg );

        if(m.printerTypeRaw() == currentPrinter)
        {
            QString stockLength = m.stockLength();

            if(stockLength.toLower() == "continuous")
            {
                ui->lblLabelSizeData->setText( tr( "Continuous" ) );
                ui->lblPrintLinesData->setText( tr( "Continuous" ) );
            }
            else
            {
                if(stockLength.toFloat() > 0.000)
                {
                    session_->uiUtilities->youGottaSetTheSeparator( stockLength );
                    ui->lblLabelSizeData->setText( stockLength + (settings->isScaleMetric() ? " mm" : "\""));
                }
                else
                {
                    ui->lblLabelSizeData->setText(tr( "Unknown" ) );
                }

                QString labelStockLength = m.stockLengthInLinesRaw();
                ui->lblPrintLinesData->setText(session_->uiUtilities->youGottaSetTheSeparator( labelStockLength ));
            }
        }
    }

    //-----------------------------------------------------------------
    // Printer Monitor Sensors (this message are received continuously)
    //-----------------------------------------------------------------
    // Processes message for Media sensor, Label taken sensor and
    // Printhead temperature
    //-----------------------------------------------------------------
    if ((Printer == msg[0].toInt() ) && ( PrSensors == msg[1].toInt() ))
    {
        DomPrinterMonitorSensors m( msg );

        if(m.printerType() == currentPrinter)
        {
            //------------------
            //-- Media sensor --
            //------------------

            if( !isSSKPrinterAttached() )
            {
                ui->lblMediaSensor->setText( "Media Sensor" );
                ui->btnCalibrateMediaSensor->setEnabled( true );
            }
            ui->lblMediaSensorData->setText( m.mediaSensor() );

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

            //---------------------------
            //-- Printhead temperature --
            //---------------------------

#ifdef ENCODING_CHECK		//	This worked for Windows but not Linux. Test this new change under Windows soon.
            // added using ALT+0176 WRAPPED INSIDE A STRING LITERAL - see Service Printer Sensors AND Help About
            ui->lblPrintheadTempData->setText( m.printheadTemp() + "°C");
#else
            QChar deg(0x00b0);
            QString tempText(m.printheadTemp());
            tempText.append(" ").append(deg).append("C");
            ui->lblPrintheadTempData->setText( tempText );
#endif
        }
    }

    //-------------------------------------------------------
    // Printer Configuration (from Printer Inquiry request)
    //-------------------------------------------------------
    // Processes message for Print Contrast, Label Width,
    // Vertical Print Position, Horizontal Print Position and
    // Labels Expel Position
    //-------------------------------------------------------
    if ((Printer == msg[0].toInt() ) && ( PrInquire == msg[1].toInt() ))
    {
        DomPrinterConfigRead m( msg );

        if(m.printerTypeRaw() == currentPrinter)
        {
            // Print Contrast
            ui->cbPrintContrast->setCurrentIndex( ui->cbPrintContrast->findData( m.printContrast() ) );

            // Label Width
            ui->cbLabelWidth->setCurrentIndex( ui->cbLabelWidth->findData( m.labelWidth() ) );

            // media sensor type
            ui->cboMediaSensorType->setCurrentIndex( m.mediaSensorType() );
            isShootThruSensor_ = m.mediaSensorType();

            // Vertical Print Position
            ui->txtVerticalPrintPosition->setText( m.vertPrintPositionRaw() );

            // Horizontal Print Position
            ui->txtHorizontalPrintPosition->setText( m.horizPrintPositionRaw() );

            // Continuous Labels Expel Position - only displayed if scaleModel not equal to 2
            if(!session_->isPrepack())
            {
                ui->txtContinuousExpelPosition->setText( m.expelPositionRaw() );
            }

            slaValue = m.slaEjectLineRaw();             // save for later in case we need it.

            // Test Label
//            updateTestPatternLabel(m.testLabel().toInt());	//	This does not exist in DomPrinterConfigRead. Defaults to Checkerboard.
//            currentTestLabel = m.testLabel();

            showAllFields();

            bSetupChanged = false;
        }
    }

    //--------------------------------------------
    // SOF-3853: Determine if a Cutter is attached
    //--------------------------------------------
    if ((Printer == msg[0].toInt() ) && ( PrCutterStatus == msg[1].toInt() ))
    {
        LOG_INFO("CutterStatus recieved...", NULL);
        DomPrinterCutterStatus m( msg );

        cutterAttached_ = m.installed();        // based on a wire - physical presence, its there or not

        LOG_INFO("%d , %d , %d", shootThroughCalStarted, m.interlock(), printheadDisengaged_);
        if( shootThroughCalStarted && m.interlock() && !printheadDisengaged_ )
        {
            if( calInitDialog )
            {
                calInitDialog->accept();
                calInitDialog = NULL;

                calDialog = new MessageBoxDialog( mainWindow_ );
                calDialog->setStandardButtons( QDialogButtonBox::Cancel );
                calDialog->setTitle( "Position Paper" );
                calDialog->setText( tr( "Calibrating..." ) );
                calDialog->open();
            }

            if( calBackingDialog )
            {
                calBackingDialog->accept();
                calBackingDialog = NULL;

                calDialog = new MessageBoxDialog( mainWindow_ );
                calDialog->setStandardButtons( QDialogButtonBox::Cancel );
                calDialog->setTitle( "Position Paper" );
                calDialog->setText( tr( "Calibrating..." ) );
                calDialog->open();
            }

            shootThroughCalStarted = false;
            sendCalibrationNext();
        }

        ui->btnCalibrateMediaSensor->setVisible( false );

        LOG_DEBUG( "onSessionMessage PrCutterStatus msg CUTTER ATTACHED=%d", cutterAttached_ );
    }

    if ((Printer == msg[0].toInt() ) && ( PrSensors == msg[1].toInt() ))
    {
        DomPrinterMonitorSensors m( msg );
        if( m.isPrintheadDisengaged() == "0" )
        {
            printheadDisengaged_ = false;
        }
        else
        {
            printheadDisengaged_ = true;
        }
    }

    if( ( Printer == msg[0].toInt() ) && ( PrGapCalStatus == msg[1].toInt() ) )
    {
        if( isSSKPrinterAttached() )
        {
            ui->lblMediaSensor->setText( "Shoot Through" );
        }

        float deflectionVoltage = msg[3].toFloat();
        float driveCurrent = msg[4].toFloat();
        float backingVoltage = msg[5].toFloat();
        float labelVoltage = msg[6].toFloat();

        ui->lblDeflectionVoltageValue->setText( QString::number( deflectionVoltage, 'f', 2 ) );
        ui->lblDriveCurrentValue->setText( QString::number( driveCurrent, 'f', 2 ) );
        ui->lblBackingVoltageValue->setText( QString::number( backingVoltage, 'f', 2 ) );
        ui->lblLabelVoltageValue->setText( QString::number( labelVoltage, 'f', 2 ) );

        //connect( calDialog.button( QDialogButtonBox::Ok ), SIGNAL( clicked() ), this, SLOT( sendCalibrationNext() ) );

        int state = msg[2].toInt();
        switch( state )
        {
            case _CalInit:
            {
                ui->btnCalibShootThrough->setEnabled( false );

                //session_->send( Printer, PrGapCalNext );
                shootThroughCalStarted = true;
                ui->lblStatusValue->setText( "Cal Init" );

                calInitDialog = new MessageBoxDialog( mainWindow_ );
                calInitDialog->setStandardButtons( QDialogButtonBox::Cancel );
                calInitDialog->setTitle( "Position Paper" );
                calInitDialog->setText( tr( "Open interlock door, disengage the printhead, and position backing paper ONLY under the shoot through (gap) sensor.\nThen engage the printhead, and close door." ) );

                calInitDialog->open();

                break;
            }
            case _BackingPaper:
            {
                session_->send( Printer, PrSensorsTracking, currentPrinter + ",0" );

                ui->btnCalibShootThrough->setEnabled( false );

                ui->lblStatusValue->setText( "Backing Paper" );
                shootThroughCalStarted = true;

                if( calDialog )
                {
                    calDialog->accept();
                    calDialog = NULL;

                    calBackingDialog = new MessageBoxDialog( mainWindow_ );
                    calBackingDialog->setStandardButtons( QDialogButtonBox::Cancel );
                    calBackingDialog->setTitle( "Position Paper" );
                    calBackingDialog->setText( tr( "Open interlock door, disengage the printhead, and position backing paper AND label under the shoot through (gap) sensor.\nThen engage the printhead, and close interlock door." ) );
                    calBackingDialog->open();
                }

                break;
            }
            case _LabelPlusBacking:
            {
                session_->send( Printer, PrSensorsTracking, currentPrinter + ",0" );

                ui->btnCalibShootThrough->setEnabled( false );

                ui->lblStatusValue->setText( "Label + Backing Paper" );
                shootThroughCalStarted = true;

                if( calDialog )
                {
                    calDialog->accept();
                    calDialog = NULL;
                }

                MessageBoxDialog calDoneDialog( mainWindow_ );
                calDoneDialog.setTitle( "Position Paper" );
                calDoneDialog.setText( tr( "Shoot Through Calibration Done, printer will now size labels.\nHit \"Ok\"" ) );
                calDoneDialog.setStandardButtons( QDialogButtonBox::Ok );
                calDoneDialog.exec();

                sendCalibrationNext();
                break;
            }
            case _CalcDeflections:
            {
                session_->send( Printer, PrSensorsTracking, currentPrinter + ",1" );

                ui->lblStatusValue->setText( "Calc Deflections" );

                break;
            }
            case _FailureSettingI:
            {
                ui->lblStatusValue->setText( "Failure Setting I" );
                session_->send( Printer, PrGapCalNext );

                break;
            }
            case _FailureMemory:
            {
                ui->lblStatusValue->setText( "Failure Memory" );
                session_->send( Printer, PrGapCalNext );

                break;
            }
            case _Done:
            {
                shootThroughCalStarted = false;

                ui->lblStatusValue->setText( "Done" );

                ui->btnCalibShootThrough->setEnabled( true );
                ui->btnCalibShootThrough->setText( "Calibrate Shoot Through" );

                break;
            }
        }
    }

    if (( System == msg[0].toInt() ) && ( SysPeripheralList == msg[1].toInt() ))
    {    
        if( isSTEppPrinter() )
        {
            ui->lblMediaSensorType->show();
            ui->cboMediaSensorType->show();
        }
        else
        {
            ui->lblMediaSensorType->hide();
            ui->cboMediaSensorType->hide();     // should of already been hidden
        }
    }
}


//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void servicePrinterConfig::setupChanged()
{
    bSetupChanged = true;
}

//-----------------------------------------------
// Slot for when the Printer selection is changed
//-----------------------------------------------
void servicePrinterConfig::printerChanged(int index)
{
    clearMessages();

//    initPrinterValues((ui->cbPrinterSelection->itemData(ui->cbPrinterSelection->currentIndex())).toString());

    QString cboText = ui->cbPrinterSelection->itemText(index);
    if ( cboText == tr("Network") || cboText == "USB" )
    {
        mainWindow_->switchToModeScreen<ServiceMode, setupUtility>();
    }
    else
    {
        initPrinterValues((ui->cbPrinterSelection->itemData(index)).toString());    // EPP or Primary
    }
}

//---------------------------------------------------------
// Slot for when the Test Label Pattern dropdown is changed
//---------------------------------------------------------
void servicePrinterConfig::updateTestPatternLabel(int index)
{
    currentTestLabel = (ui->cbTestLabelPattern->itemData( index )).toString();
}

//--------------------------------------
// Slot for printing a single test label
//--------------------------------------
void servicePrinterConfig::printSingleTestLabel()
{
    QString param;

    saveDataOnExit();

    param = currentPrinter + "," + currentTestLabel + ",0";
    session_->send( Printer, PrTestLabel, param );
}

//---------------------------------------
// Slot for printing multiple test labels
//---------------------------------------
void servicePrinterConfig::printMultipleTestLabels()
{
    QString param;

    saveDataOnExit();

    param = currentPrinter + "," + currentTestLabel + ",1";
    session_->send( Printer, PrTestLabel, param );
}

//-------------------------------------------------------
// Slot for stopping the printing of multiple test labels
//-------------------------------------------------------
void servicePrinterConfig::stopMultipleTestLabels()
{
    QString param;

    param = currentPrinter + "," + currentTestLabel + ",2";
    session_->send( Printer, PrTestLabel, param );
}

//--------------------------------------
// Slot for Calibrating the Media sensor
//--------------------------------------
void servicePrinterConfig::calibrateMediaSensor()
{
    QString param;

    param = currentPrinter + ",1";
    session_->send( Printer, PrReqCalibrate, param );
}

//--------------------------------------------
// Slot for Calibrating the Label Taken sensor
//--------------------------------------------
void servicePrinterConfig::calibrateLabelTakenSensor()
{
    QString param;

    param = currentPrinter + ",2";
    session_->send( Printer, PrReqCalibrate, param );
}

//-----------------------------------
// Slot for Advancing the Label Stock
//-----------------------------------
void servicePrinterConfig::advanceLabelStock()
{
    session_->send( Printer, PrReqAdvLabelStock, currentPrinter );
}

//------------------------------------------------
// Slot for Calibrating Shoot Through (Gap) Sensor
//------------------------------------------------

void servicePrinterConfig::calibrateShootThrough()
{
    LOG_CLASS_INFO("servicePrinterConfig", "CalibrateShootThrough");

    /*
    if( !shootThroughCalStarted )
    {
        LOG_INFO("Starting Shoot Through Calibration....", NULL);
        session_->send( Printer, PrStartGapCalibration );
    }
    else
    {
        ui->lblStatusValue->setText( "Calibrating..." );
        session_->send( Printer, PrGapCalNext );
    }
    */

    LOG_INFO("Starting Shoot Through Calibration....", NULL);
    session_->send( Printer, PrStartGapCalibration );

    ui->btnCalibShootThrough->setEnabled( false );
}

//-----------------------------------------------------
// Slot for when the Printer Settings button is clicked
//-----------------------------------------------------
// FT-636
void servicePrinterConfig::printerSettings()
{
    PrinterSettingsDialog *dialog = new PrinterSettingsDialog( mainWindow_ );

    // show dialog!
    dialog->open();
}

//--------------------------------------------------------------------------
// Slot to move on to next shoot through calibration step when dialog clears
//--------------------------------------------------------------------------
// FSSS SOF-4989
void servicePrinterConfig::sendCalibrationNext()
{
    ui->lblStatusValue->setText( "Calibrating..." );
    session_->send( Printer, PrGapCalNext );
}

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void servicePrinterConfig::saveDataOnExit()
{
    if (bSetupChanged)
    {
        QString param;

        param = currentPrinter;
        param += "," + ( ui->cbPrintContrast->itemData( ( ui->cbPrintContrast->currentIndex() ) ) ).toString();
        param += "," + ui->txtVerticalPrintPosition->text();
        param += "," + ui->txtHorizontalPrintPosition->text();
        param += ",\"" + ( ui->cbLabelWidth->itemData( ( ui->cbLabelWidth->currentIndex() ) ) ).toString() + "\"";

        if(session_->isPrepack())
            param += ",";                   // no idea why we dont send this if a wrapper.  Maybe EPPs cant change their expel position????? Or do it diff???
        else
            param += "," + ui->txtContinuousExpelPosition->text();

        param += "," +  QString::number(ui->cboMediaSensorType->currentIndex() );

        //param += "," + slaValue;            // Never used on this screen even tho in msg. I dont know. Future use?

        session_->send( Printer, PrConfig, param);

        bSetupChanged = false;
    }
}

//------------------------------------------------------------------------------
// Send messages to turn off sensor tracking and turn off continuous test labels
//------------------------------------------------------------------------------
void servicePrinterConfig::clearMessages()
{
    QString param;

    // Turn off sensor tracking for current printer
    param = currentPrinter + ",0";
    session_->send( Printer, PrSensorsTracking, param );

    stopMultipleTestLabels();
}

bool servicePrinterConfig::isSTEppPrinter()
{
    MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;

    // want to use this better, stronger, test, but not sure yet if it follows thru
    if( ( periphSettings->firstPrinter() == PrinterKind::fieldOffsetSTEPPPrinter ) || ( periphSettings->secondPrinter() == PrinterKind::fieldOffsetSTEPPPrinter ) )
        return true;
    else
        return false;
}

bool servicePrinterConfig::isFirstPrinterEpp()
{
    MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;

    if (periphSettings->firstPrinter()  == EPPPrinter  || periphSettings->firstPrinter()  == EPP2Printer)
        return true;
    else
        return false;
}

bool servicePrinterConfig::isEppPrinterAttached(int printerLocation)
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

bool servicePrinterConfig::isUsbPrinterAttached(int printerLocation)
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

bool servicePrinterConfig::isSSKPrinterAttached()
{
    //LOG_CLASS_INFO("servicePrinterSensors", "isSSKPrinterAttached");

    MadpPeripheralList *periphList = &session_->scaleConfig->peripheralList;

    bool isAttached = false;

    //LOG_INFO("firstPrinter() = %d", periphList->firstPrinter());
    if (periphList->firstPrinter() == SSKPrinter)
    {
        isAttached = true;
    }

    return isAttached;
}

