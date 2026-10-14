/**
 * @file serviceWeigherConfig.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "serviceWeigherConfig.h"
#include "ui_serviceWeigherConfig.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"

#include <UiUtilities.h>
#include <QIntValidator>


static const bool FAKE_WEIGHER_TESTING =  false;
static const bool FAKE_PRESSED_SWITCH = false;
static const bool FAKE_USB_KEY = false;

namespace hobart {
    class numericLineEdit;
}

serviceWeigherConfig::serviceWeigherConfig(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::serviceWeigherConfig)
{
    ui->setupUi(this);
    scaleScreen();

    bWeigherSwitchPressed = false;
    bReloadScreen = false;
    doNotSaveWhileUpdate_ = false;
    valuemaxIsEnabled = false;
    hasSystemTestKey = false;

    showHideEnableValueMax();

    setMinWeightToPrintValidator();

    initialize();

    if( FAKE_WEIGHER_TESTING )
        qDebug() << "constructor() initial value weigherScaleFactor: " << QString::number(weigherScaleFactor);

    //----------------------------------------------------------------
    // Set the connections for when a field in the screen is changed.
    // Set a property that indicates something in the setup changed
    // and the data needs to be saved.
    //----------------------------------------------------------------
    connect( ui->cbWeigherModel,          SIGNAL( currentIndexChanged(int)),  this, SLOT( WeigherModelChanged() ) );
    connect( ui->cbWeigherModel,          SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );

    connect( ui->cbFilterSpeed,           SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );
    connect( ui->cbDivisionSize,          SIGNAL( currentIndexChanged(int) ), this, SLOT( DivisionSizeChanged() ) );
    connect( ui->cbMaximumWeight,         SIGNAL( currentIndexChanged(int) ), this, SLOT( SetupChanged() ) );
    connect( ui->cbMaximumWeight,         SIGNAL( currentIndexChanged(int) ), this, SLOT( MaximumWeightChanged() ) );
    connect( ui->txtMinimumWeightToPrint, SIGNAL( editingFinished() ),        this, SLOT( SetupChanged() ) );
    connect( ui->chkEnableValueMax,       SIGNAL( stateChanged (int)),        this, SLOT(checkboxChangedValueMax(int)));

    //---------------------------
    // Put Weigher in manual mode
    //---------------------------
    session_->send( Weigher, WgMode, "3" );

    //---------------------
    // Request Weigher Info
    //---------------------
    session_->send( Weigher, WgReqInfo );

    //-----------------------------
    // Send Weigher Inquiry Request
    //-----------------------------
    session_->send( Weigher, WgReqInquire );

    //-----------------------
    // Request Weigher Config
    //-----------------------
    session_->send( Weigher, WgConfigurationBegin );

    bSetupChanged = false;

    translate();

    if( FAKE_WEIGHER_TESTING )
    {
        //HLX1,       // 0.005 dual range
        //AWS,        // 0.005 dual range
        //EPSA2,      // 0.01 single range
        //AWS2        // 0.01 single range
        // turn FAKE_WEIGHER_TESTING to true to allow you to run and see the weigher configure screen
        // on your own laptop.  Set the values to anything you want them to be
        MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
        settings->isScaleMetricRaw() = "0";

        setSwitchToPressed();
        ui->lblPressWeigherSwitch->hide();
        ui->lblWeigherShutdownMessage->show();

        weigherModel             = SessionMainWindow::AWS2;
        weigherFilterSpeed       = "1";
        weigherScaleFactor       = 1;
        weigherMaximumWt         = "30";
        weigherMininumWt         = "0.20";
        netWeightDRLimitCheck    = 1;
        valuemaxIsEnabled        = true;

        doNotSaveWhileUpdate_    = true;
        initialize();
        doNotSaveWhileUpdate_    = false;

        bSetupChanged = false;
    }
}

serviceWeigherConfig::~serviceWeigherConfig()
{
    delete ui;
}

void serviceWeigherConfig::translate()
{
    ui->retranslateUi(this);

    update();
}

//---------------------------------------------------------------------
// Used with numericLineEdit widget to validate Minimum Weight to Print
//---------------------------------------------------------------------
int serviceWeigherConfig::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

//-----------------------------------------------
// Populate the dropdowns based on the scaleModel
//-----------------------------------------------
void serviceWeigherConfig::initialize()
{
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString scaleModelSetting = settings->scaleModel();

    ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
    MadpKeyActivation serviceModeKeyActivation( serviceMode->usbKeyActivation() );

    const MadpKeyActivation *keyz = &serviceModeKeyActivation;

    if( FAKE_WEIGHER_TESTING )
        scaleModelSetting = "2";

    // block signals on combos
    ui->cbWeigherModel->blockSignals(true);
    ui->cbFilterSpeed->blockSignals(true);
    ui->cbMaximumWeight->blockSignals(true);

    // valid if they already have it inserted when they come to this screen
    hasSystemTestKey = keyz->isQATestKey() || keyz->isManfTestKey();

    showHideEnableValueMax();

    //-----------------------
    // Weigher Model dropdown
    //-----------------------
    ui->cbWeigherModel->clear();

    ui->cbWeigherModel->addItem( tr( "None" ), 0 );

    if(scaleModelSetting == "3")
    {
        // Service Counter

        MadpScaleConfig *settings( &session_->scaleConfig->scaleConfig );
        const QString scaleModel( settings->scaleName() );

        if( ( scaleModel.size() >= 3 && scaleModel.toUpper().startsWith( "HTM" ) ) ||
            ( scaleModel.size() >= 2 && scaleModel.toUpper().startsWith( "FS" ) ) ||    // Free Standing Self-Serve
            ( scaleModel.size() >= 2 && scaleModel.toUpper().startsWith( "KS" ) ) )     // Kiosk
        {
            // SOF-3152 - Only allow "HTM" weigher on scale model of "HTm"
            ui->cbWeigherModel->addItem( "HTM", SessionMainWindow::HTM );
        }
        else
        {
            ui->cbWeigherModel->addItem( "HT Series", SessionMainWindow::HLX1 ); // FT-203: was "HLX-1";
            ui->cbWeigherModel->addItem( "SASCALE-1", SessionMainWindow::SASCALE1 ); // HT-374
            ui->cbWeigherModel->addItem( "SASCALE-2", SessionMainWindow::SASCALE2 ); // HT-374
        }
    }

    if(scaleModelSetting == "2")
    {
        // Pre-Pack

//      ui->cbWeigherModel->addItem( "AWS" , SessionMainWindow::EzUSB );         // EzUSB weigher - not used on 5i and 6x (VxWorks and Windux)
        ui->cbWeigherModel->addItem( "AWS REV 2" , SessionMainWindow::AWS2 );    // single range ATMEL weigher - 2 digit precision
        ui->cbWeigherModel->addItem( "AWS REV 2 DR" , SessionMainWindow::AWS );  // dual range ATMEL weigher - 3 digit precision
//      ui->cbWeigherModel->addItem( "EPSA" , SessionMainWindow::EPSA );         // EzUSB weigher - not used on 5i or 6x (VxWorks and Windux)
        ui->cbWeigherModel->addItem( "EPSA REV 2" , SessionMainWindow::EPSA2 );
    }

    //----------------------
    // Filter Speed dropdown
    //----------------------
    ui->cbFilterSpeed->clear();

    ui->cbFilterSpeed->addItem( tr( "Normal" ), 0 );
    ui->cbFilterSpeed->addItem( tr( "Slow" ), 1 );
    ui->cbFilterSpeed->addItem( tr( "Fast" ), 2 );

    //-----------------------
    // Division Size dropdown
    //-----------------------
    PopulateDivisionSize();

    //------------------------
    // Maximum Weight dropdown
    //------------------------
    PopulateMaximumWeight();

    //---------------------------------
    // Mininum Weight - set Units label
    //---------------------------------
    SetUnits();

    //--------------------------------------------------------
    // Select correct data in dropdowns, set text field values
    //--------------------------------------------------------

    // Select correct data in dropdown - default to None
    int index = ui->cbWeigherModel->findData( weigherModel);
    if(index == -1)
        // If not found then select the first item in the list - i.e. "None"
        ui->cbWeigherModel->setCurrentIndex( 0 );
    else
        ui->cbWeigherModel->setCurrentIndex( index );

    //HA-98
    setMinWeightToPrintValidator();

    ui->cbFilterSpeed->setCurrentIndex( ui->cbFilterSpeed->findData( weigherFilterSpeed.toInt() ) );
    ui->cbMaximumWeight->setCurrentIndex( ui->cbMaximumWeight->findText( weigherMaximumWt, Qt::MatchContains ) );

    // allow signals again
    ui->cbWeigherModel->blockSignals(false);
    ui->cbFilterSpeed->blockSignals(false);
    ui->cbMaximumWeight->blockSignals(false);

    // This is set in PopulateDivisionSize, no need to do here again. Or maybe it should be moved from there
    //ui->cbDivisionSize->setCurrentIndex( ui->cbDivisionSize->findData( weigherScaleFactor) );

    QString minWtToPrint = weigherMininumWt;
    session_->uiUtilities->youGottaSetTheSeparator( minWtToPrint );
    ui->txtMinimumWeightToPrint->setText( minWtToPrint );

    ui->chkEnableValueMax->setChecked( valuemaxIsEnabled );

    // Is the Weigher Model "None" or something else?
    int intWeigherModel = ui->cbWeigherModel->itemData( ( ui->cbWeigherModel->currentIndex() ) ).toInt();

    if( FAKE_WEIGHER_TESTING )
        intWeigherModel = SessionMainWindow::AWS2;

    if(intWeigherModel == SessionMainWindow::None)
    {
        HideAllFields();
    }
    else
    {
        ShowAllFields();
    }

    // Was Weigher switch previously pressed?
    if(wasWeigherSwitchPressed())
    {
        EnableAllFields();

        ui->lblPressWeigherSwitch->hide();
        ui->lblWeigherShutdownMessage->show();
    }
    else
    {
        DisableAllFields();

        ui->lblWeigherShutdownMessage->hide();
    }
}

//-----------------------------------------------------------------
// Populates the Division Size dropdopwn based on the Weigher Model
// and whether or not the scale is metric
//-----------------------------------------------------------------
void serviceWeigherConfig::PopulateDivisionSize()
{

    ui->cbDivisionSize->blockSignals(true);

    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

    ui->cbDivisionSize->clear();

    if( weigherModel == SessionMainWindow::EPSA2 ||
        weigherModel == SessionMainWindow::AWS2  ||
        weigherModel == SessionMainWindow::SASCALE1 )
    {
        if(settings->isScaleMetric())
        {
            ui->cbDivisionSize->addItem( "0.005 kg", 5);
        }
        else
        {
            ui->cbDivisionSize->addItem( "0.01 lb", 1);
        }
    }
    else if( weigherModel == SessionMainWindow::HLX1 ||
             weigherModel == SessionMainWindow::SASCALE2 ||
             weigherModel == SessionMainWindow::HTM )
    {
        if(settings->isScaleMetric())
        {
            ui->cbDivisionSize->addItem( "0.002 kg", 2);
            ui->cbDivisionSize->addItem( "0.005 kg", 5);
        }
        else
        {
            ui->cbDivisionSize->addItem( "0.01 lb", 1);           // adds 0.01 division size and precision weighing
            ui->cbDivisionSize->addItem( "0.005 lb", 5);
        }
    }
    else if( weigherModel == SessionMainWindow::AWS)
    {
        // HT-209 AWS REV2 DR should only allow 0.002 kg / 0.005 lb
        if(settings->isScaleMetric())
        {
            ui->cbDivisionSize->addItem( "0.002 kg", 2);
        }
        else
        {
            ui->cbDivisionSize->addItem( "0.005 lb", 5);
        }
    }

    // Select correct data in dropdown
    int index = ui->cbDivisionSize->findData( weigherScaleFactor);

    if( FAKE_WEIGHER_TESTING )
    {
        qDebug() << "PopulateDivisionSize() weigherScaleFactor: " << QString::number(weigherScaleFactor);
        qDebug() << "PopulateDivisionSize() index: " << QString::number(index);
    }

    if(index == -1)
        // If not found then select the first item in the list
        ui->cbDivisionSize->setCurrentIndex( 0 );
    else
        ui->cbDivisionSize->setCurrentIndex( index );

    ui->cbDivisionSize->blockSignals(false);

    // Set up validator for Minimum Weight to Print using the Numeric Line Edit
    SetupMinWtValidator();
}

//---------------------------------------------------------------------------------
// Sets the validator for min weight to print based on dual-range vs non dual-range
//---------------------------------------------------------------------------------
void serviceWeigherConfig::setMinWeightToPrintValidator()
{
    MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

    if( scaleConfig->isScaleMetric() || !scaleConfig->isSingleRangeWeigher() )
    {
        // 0.000 precision
        // Set up validator for Minimum Weight to Print using the Numeric Line Edit
        ui->txtMinimumWeightToPrint->setValidator( minimumWeightToPrintValidator_ = new QIntValidator(0, INT_MAX, this));
        ui->txtMinimumWeightToPrint->setDecimal( "." );
        ui->txtMinimumWeightToPrint->setDecimalPlaces( 3 );
        ui->txtMinimumWeightToPrint->setInputWrapping(true);
        minimumWeightToPrintValidator_->setTop(makeValidatorTop( 4 ));
    }
    else
    {
        // 0.00 precision
        // Set up validator for Minimum Weight to Print using the Numeric Line Edit
        ui->txtMinimumWeightToPrint->setValidator( minimumWeightToPrintValidator_ = new QIntValidator(0, INT_MAX, this));
        ui->txtMinimumWeightToPrint->setDecimal( "." );
        ui->txtMinimumWeightToPrint->setDecimalPlaces( 2 );
        ui->txtMinimumWeightToPrint->setInputWrapping(true);
        minimumWeightToPrintValidator_->setTop(makeValidatorTop( 3 ));
    }

    //Blank out the value as requested
    ui->txtMinimumWeightToPrint->setText( "0" );
}

//------------------------------------------------------------------
// Set up the minumum weight to print depending if division size is 2 or 3 digits
//------------------------------------------------------------------
void serviceWeigherConfig::SetupMinWtValidator()
{
    weigherScaleFactor =  ui->cbDivisionSize->itemData( (ui->cbDivisionSize->currentIndex()) ).toInt();

    if( FAKE_WEIGHER_TESTING )
        qDebug() << "SetupMinWtValidator() weigherScaleFactor: " << QString::number(weigherScaleFactor);

    ui->txtMinimumWeightToPrint->setValidator( minimumWeightToPrintValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtMinimumWeightToPrint->setDecimal( "." );
    ui->txtMinimumWeightToPrint->setInputWrapping(true);

    if ( weigherScaleFactor == 1 )
    {
        // indicates 0.01, 2 digits
        ui->txtMinimumWeightToPrint->setDecimalPlaces( 2 );
        minimumWeightToPrintValidator_->setTop(makeValidatorTop( 3 ));
    }
    else
    {
        // indicates 3 digit precision
        ui->txtMinimumWeightToPrint->setDecimalPlaces( 3 );
        minimumWeightToPrintValidator_->setTop(makeValidatorTop( 4 ));
    }
}

//------------------------------------------------------------------
// Populates the Maximum Weight dropdopwn based on the Weigher Model
//------------------------------------------------------------------
void serviceWeigherConfig::PopulateMaximumWeight()
{
    ui->cbMaximumWeight->clear();

    if( weigherModel == SessionMainWindow::HLX1 ||
        weigherModel == SessionMainWindow::AWS  ||
        weigherModel == SessionMainWindow::AWS2 ||
        weigherModel == SessionMainWindow::SASCALE2 ||
        weigherModel == SessionMainWindow::HTM )
    {
        ui->cbMaximumWeight->addItem( "30 lb", cap30lb );
        ui->cbMaximumWeight->addItem( "15 kg", cap15kg );
    }
    else if( weigherModel == SessionMainWindow::EPSA2 )

    {
        ui->cbMaximumWeight->addItem( "30 lb", cap30lb );
        ui->cbMaximumWeight->addItem( "50 lb", cap50lb );
        ui->cbMaximumWeight->addItem( "15 kg", cap15kg );
        ui->cbMaximumWeight->addItem( "25 kg", cap25kg );
    }
    else if( weigherModel == SessionMainWindow::SASCALE1 )
    {
        ui->cbMaximumWeight->addItem( "50 lb", cap50lb );
        ui->cbMaximumWeight->addItem( "25 kg", cap25kg );
    }
}

//------------------------------------------------------
// Set the units on the Minimum Weight to Print text box
//------------------------------------------------------
void serviceWeigherConfig::SetUnits()
{
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

    if(settings->isScaleMetric())
        ui->lblMinimumWeightUnits->setText( "kg" );
    else
        ui->lblMinimumWeightUnits->setText( "lb" );
}

//--------------------------------------------------
// Reloads the screen when Weigher Model set to None
//--------------------------------------------------
void serviceWeigherConfig::ReloadScreen()
{
    bReloadScreen = true;

    // Is the Weigher Model "None" or something else?
    int intWeigherModel = ui->cbWeigherModel->itemData( ( ui->cbWeigherModel->currentIndex() ) ).toInt();
    if(intWeigherModel == SessionMainWindow::None)
    {
        HideAllFields();
    }
    else
    {
        ShowAllFields();

        PopulateDivisionSize();

        PopulateMaximumWeight();

        SetUnits();
    }

    // Was Weigher switch previously pressed?
    if(wasWeigherSwitchPressed())
    {
        EnableAllFields();

        ui->lblPressWeigherSwitch->hide();
        ui->lblWeigherShutdownMessage->show();
    }
    else
    {
        DisableAllFields();

        ui->lblWeigherShutdownMessage->hide();
    }

    //---------------------------
    // Put Weigher in manual mode
    //---------------------------
    session_->send( Weigher, WgMode, "3" );

    bReloadScreen = false;
}

bool serviceWeigherConfig::onScreenChanging()
{
    doNotSaveWhileUpdate_  = false;
    bSetupChanged = true;

    SaveWeigherConfig();

    return _Mybase::onScreenChanging();
}

void serviceWeigherConfig::onSessionMessage( const SessionMessage& msg )
{
    // Scale Inquiry Info
    if ((Weigher == msg[0].toInt() ) && ( WgInquire == msg[1].toInt() ))
    {
        DomWeigherConfigRead m( msg );

        weigherModel            = m.weigherModel();
        weigherFilterSpeed      = m.weigherFilterSpeed();
        weigherScaleFactor      = m.weigherScaleFactor();
        weigherMaximumWt        = m.weigherMaximumWt();
        weigherMininumWt        = m.weigherMininumWt();
        netWeightDRLimitCheck   = m.netWeightDRLimitCheck();

        weigherDualRangeLimit   = m.dualRangeLimitRaw();
        weigherGcdLanguage      = m.gcdLanguage();

        valuemaxIsEnabled       = m.isScaleValuemax();

        doNotSaveWhileUpdate_   = true;
        initialize();
        doNotSaveWhileUpdate_   = false;

        bSetupChanged = false;
    }

    // Weigher Switch Pressed - enable fields for editing
    if ((Weigher == msg[0].toInt() ) && ( WgConfigurationEnd == msg[1].toInt() ))
    {
        setSwitchToPressed();

        ui->lblPressWeigherSwitch->hide();
        ui->lblWeigherShutdownMessage->show();
    }

    // Service Key inserted / Removed
    if (( SessionDev == msg[0].toInt() ) && ( SnKeyActivationStates == msg[1].toInt() ))
    {
        MadpKeyActivation m( msg );

        // valid if they inserted after they come to this screen
        //hasSystemTestKey = mainWindow_->checkForSystemTestKey();
        hasSystemTestKey = m.isManfTestKey();  // SOF-4090

        showHideEnableValueMax();
    }
}

//----------------------------------------------------------------------------------------------
// Called after the Weigher Switch has been pressed and corresponding message has been processed
//----------------------------------------------------------------------------------------------
void serviceWeigherConfig::EnableAllFields()
{
    ui->cbWeigherModel->setEnabled(true);
    ui->cbFilterSpeed->setEnabled(true);
    ui->cbDivisionSize->setEnabled(true);
    ui->cbMaximumWeight->setEnabled(true);
    ui->txtMinimumWeightToPrint->setEnabled(true);
    ui->chkEnableValueMax->setEnabled(true);
}

//--------------------------
// Disable all of the fields
//--------------------------
void serviceWeigherConfig::DisableAllFields()
{
    ui->cbWeigherModel->setDisabled(true);
    ui->cbFilterSpeed->setDisabled(true);
    ui->cbDivisionSize->setDisabled(true);
    ui->cbMaximumWeight->setDisabled(true);
    ui->txtMinimumWeightToPrint->setDisabled(true);
    ui->chkEnableValueMax->setDisabled(true);
}

//-------------------------------------------------------
// Hides all fields.  Used when there is no Weigher Model
//-------------------------------------------------------
void serviceWeigherConfig::HideAllFields()
{
    ui->cbFilterSpeed->hide();
    ui->cbDivisionSize->hide();
    ui->cbMaximumWeight->hide();
    ui->txtMinimumWeightToPrint->hide();
    ui->lblMinimumWeightUnits->hide();
    ui->chkEnableValueMax->hide();
}

//------------------------------------------------------------------
// Displays all fields.  Used when there is a Weigher Model selected
//------------------------------------------------------------------
void serviceWeigherConfig::ShowAllFields()
{
    ui->cbFilterSpeed->show();
    ui->cbDivisionSize->show();
    ui->cbMaximumWeight->show();
    ui->txtMinimumWeightToPrint->show();
    ui->lblMinimumWeightUnits->show();

    showHideEnableValueMax();
}

//--------------------------------------------------------------------------
// Called when message is received indicating that Weigher switch is pressed
//--------------------------------------------------------------------------
void serviceWeigherConfig::setSwitchToPressed()
{
    bWeigherSwitchPressed = true;

    // Update global variable in the DOM.
    DomWeigherSwitch *settings = &session_->scaleConfig->weigherSwitch;
    settings->switchPressed() = "1";

    EnableAllFields();
}

//---------------------------------------------------------
// Determines if Weigher switch has previously been pressed
//---------------------------------------------------------
bool serviceWeigherConfig::wasWeigherSwitchPressed()
{
    DomWeigherSwitch *settings = &session_->scaleConfig->weigherSwitch;

    if(settings->switchPressed() == "1")
    {
        bWeigherSwitchPressed = true;
    }

    if ( FAKE_PRESSED_SWITCH || FAKE_WEIGHER_TESTING )
        bWeigherSwitchPressed = true;

    return bWeigherSwitchPressed;
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void serviceWeigherConfig::SetupChanged()
{
    bSetupChanged = true;
}

//------------------------------------------------------
// Update property indicating that the setup has changed
// and needs to be saved
//------------------------------------------------------
void serviceWeigherConfig::DivisionSizeChanged()
{
    bSetupChanged = true;

    SetupMinWtValidator();
}

//------------------------------------------------------------------------
// Slot for when a Weigher Model dropdown is changed.
// Sets property indicating that the setup needs to be saved when exiting.
// Also hides/shows fields on the screen as needed.
//------------------------------------------------------------------------
void serviceWeigherConfig::WeigherModelChanged()
{
    bSetupChanged = true;

    SaveWeigherConfig();

    if( SessionMainWindow::None != (ui->cbWeigherModel->itemData( ( ui->cbWeigherModel->currentIndex() ) ).toInt() ) )
    {
        // Change global variable so that dropdowns, etc. will populate properly
        weigherModel = (ui->cbWeigherModel->itemData( ( ui->cbWeigherModel->currentIndex() ) ).toInt() );

        setMinWeightToPrintValidator();

        ShowAllFields();

        SaveWeigherNormalScaleConfig();

        // HT-13 CLONE - * Weigher displaying 0.0 lbs when changing inbetween certain weighers
        // quietly doing a rezero updates and fixes it
        // It updates and forces the correct number of digits in Operate Mode
        // Now save new normal scale operation config values

        session_->scaleConfig->quietDialogs = true;
        session_->send( Weigher, WgRezero);
    }

    if(!bReloadScreen)
    {
        ReloadScreen();
    }
}

//----------------------------------------------------
// Maximum Weight changed - Need to update based on if
// scale changed to/from lbs/kgs
//----------------------------------------------------
void serviceWeigherConfig::MaximumWeightChanged()
{
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

    bool bNewIsMetric = false;
    switch( ui->cbMaximumWeight->itemData( ( ui->cbMaximumWeight->currentIndex() ) ).toInt() )
    {
        case cap30lb:
        case cap50lb:
        case cap300lb:
            bNewIsMetric = false;
            break;

        case cap15kg:
        case cap25kg:
        case cap150kg:
            bNewIsMetric = true;
            break;
    }

    if(bNewIsMetric != settings->isScaleMetric() )
    {
        QString minWtToPrint;

        if(bNewIsMetric)
        {
            // Set Units for Minimum Weight
            ui->lblMinimumWeightUnits->setText( "kg" );

            minWtToPrint = QString::number( (weigherMininumWt.toFloat() / 10 ), 'f' , 3);
            session_->uiUtilities->youGottaSetTheSeparator( minWtToPrint );
            ui->txtMinimumWeightToPrint->setText( minWtToPrint );

            // Update Division Size
            if(weigherModel == SessionMainWindow::QuantHC)
                weigherScaleFactor = div0_05_kg;
            else
                weigherScaleFactor = div0_005_kg;

            // Update the GCD Manager
            session_->send( GCDSessionDev, GcdSessionMetric );
        }
        else
        {
            // Set Units for Minimum Weight
            ui->lblMinimumWeightUnits->setText( "lb" );

            minWtToPrint = QString::number( (weigherMininumWt.toFloat() * 10 ), 'f' , 3);
            session_->uiUtilities->youGottaSetTheSeparator( minWtToPrint );
            ui->txtMinimumWeightToPrint->setText( minWtToPrint );

            // Update Division Size
            if(weigherModel == SessionMainWindow::QuantHC)
                weigherScaleFactor = div0_1_lb;
            else
                weigherScaleFactor = div0_01_lb;

            // Update the GCD Manager
            session_->send( GCDSessionDev, GcdSessionAvoir );
        }

         if( FAKE_WEIGHER_TESTING )
             qDebug() << "MaximumWeightChanged() after change lb/kg mode weigherScaleFactor: " << QString::number(weigherScaleFactor);

        // Update global variable "Metric Scale" setting in the DOM.
        settings->isScaleMetricRaw() = (bNewIsMetric ? "1" : "0");
    }

    // Now to to re-populate Divison Size dropdown - lbs/kgs change
    PopulateDivisionSize();
}

//----------------------------------------------------
// Value Max enabled changed - Need to update based on if
// scale needs Value Max On or Off
//----------------------------------------------------
void serviceWeigherConfig::checkboxChangedValueMax(int state)
{
    bSetupChanged = true;

    if( state > 0 )
        valuemaxIsEnabled = true;
    else
        valuemaxIsEnabled = false;
}

//--------------------------------------------------------------------------
// SOF-4090: Determines whether to show or hide the "Enable Value Max" field
//--------------------------------------------------------------------------
void serviceWeigherConfig::showHideEnableValueMax()
{
    // Only display IF we have a System Test Key - by default off
    if( FAKE_USB_KEY )
    {
        ui->lblValueMax->setVisible( true );
        ui->chkEnableValueMax->setVisible( true );
    }
    else
    {
        ui->lblValueMax->setVisible( hasSystemTestKey );
        ui->chkEnableValueMax->setVisible( hasSystemTestKey );
    }

    // SOF-3154: Only display Value Max fields for "HTM" weigher model
    int intWeigherModel = ui->cbWeigherModel->itemData( ( ui->cbWeigherModel->currentIndex() ) ).toInt();
    if( intWeigherModel != SessionMainWindow::HTM )
    {
        ui->lblValueMax->setVisible( false );
        ui->chkEnableValueMax->setVisible( false );
    }
}

//-------------------------------------------------------------------
// Build up the string containing the setting and update the database
//-------------------------------------------------------------------
void serviceWeigherConfig::SaveWeigherConfig()
{
    QString param;

    if( FAKE_WEIGHER_TESTING )
        bSetupChanged = true;

    if ( doNotSaveWhileUpdate_ )
        return;

    if (bSetupChanged)
    {
        MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

        param = ( ui->cbFilterSpeed->itemData( ( ui->cbFilterSpeed->currentIndex() ) ) ).toString();

        param += "," + QString(settings->isScaleMetric() ? "1" : "0");

        param += "," + ( ui->cbDivisionSize->itemData( ( ui->cbDivisionSize->currentIndex( ) ) ) ).toString();

        QString MaxWtValue;
        switch( ui->cbMaximumWeight->itemData( ( ui->cbMaximumWeight->currentIndex() ) ).toInt() )
        {
            case cap30lb:
                MaxWtValue = "30";
                break;

            case cap50lb:
                MaxWtValue = "50";
                break;

            case cap300lb:
                MaxWtValue = "300";
                break;

            case cap15kg:
                MaxWtValue = "15";
                break;

            case cap25kg:
                MaxWtValue = "25";
                break;

            case cap150kg:
                MaxWtValue = "150";
                break;
        }

        param += ",\"" + MaxWtValue + "\"";

        param += ",\"" + ui->txtMinimumWeightToPrint->text() + "\"";

        param += ",,,,,"; // placeholders for lastDateCalibrated, timesCalibrated, lastDateConfigured, timesConfigured

        param += ui->cbWeigherModel->itemData( ( ui->cbWeigherModel->currentIndex() ) ).toString();

        param += "," + netWeightDRLimitCheck;

        param += "," + weigherDualRangeLimit;

        param += "," + QString( valuemaxIsEnabled ? "1" : "0" );

        param += "," + weigherGcdLanguage;

        session_->send( Weigher, WgConfig, param);
    }

    bSetupChanged = false;
}

void serviceWeigherConfig::SaveWeigherNormalScaleConfig()
{
    normalScaleOperation = &session_->scaleConfig->normalScaleOperation;
    QString maxTare = normalScaleOperation->maxTare();
    QString msg = ",,,,,,,,,,,,,,,,,,,";

    if( SessionMainWindow::HLX1 == weigherModel || SessionMainWindow::AWS == weigherModel || SessionMainWindow::SASCALE2 == weigherModel )
    {
        // These should not be hardcoded at all but have the real value of the fields.
        // This had to be a test of some sort. Otherwise it will always be 9.995
        // dual range weighers

        if ( maxTare.size() == 0 )
            msg += "\"9.995\",";                // set a default
        else
            msg += "\"" + maxTare + "\"";

        msg +=
        msg += ",,,,";
        msg += "3";
    }
    else if( SessionMainWindow::AWS2 == weigherModel || SessionMainWindow::EPSA2 == weigherModel || SessionMainWindow::SASCALE1 == weigherModel )
    {
        // These should not be hardcoded at all but have the real value of the fields.
        // This had to be a test of some sort. Otherwise it will always be 9.995
        // non-dual range ATMEL weighers

        if ( maxTare.size() == 0 )
            msg += "\"9.99\",";                 // set a default
        else
            msg += "\"" + maxTare + "\"";

        msg += ",,,,";
        msg += "2";
    }

    msg += ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";

    session_->send(ConfigurationDev, CfgScaleOperationSave, msg);     //	112,72,msg
    session_->send(ConfigurationDev, CfgScaleRead);                  //	    112,20 - to make sure we update
}
