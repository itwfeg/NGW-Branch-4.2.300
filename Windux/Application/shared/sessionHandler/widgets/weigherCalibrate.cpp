
/**
 * @file weigherCalibrate.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "weigherCalibrate.h"
#include "ui_weigherCalibrate.h"

#include "../sessioninfo.h"

#include <ScaleConfiguration.h>
#include <ScaleClasses.h>
#include <QIntValidator>

static const bool FAKE_KPC_TESTING =  false;

namespace hobart {
    class numericLineEdit;
}

// Hobart namespace
namespace hobart {

weigherCalibrate::weigherCalibrate(QWidget *parent, SessionMainWindow *mainWindow, int calledBy ) :
    _Mybase( parent ),
    ui(new Ui::weigherCalibrate)
{
    ui->setupUi(this);

    // FT-695 Default to Lbs
    unitsType = "1";


    initialize();

    // Hide the Calibration Complete overlay screen
    ui->gbCalibrationComplete->hide();

    // Connect into session
    session_ = mainWindow->session();

    // Save off the mainWindow pointer for use later
    mainWindow_ = mainWindow;

    // Save where the Weigher Calibrate was called from:  Service Mode Panel or Manufacturing Test screen
    // 0 if called from manuf test, 1 if called from menu
    calledFromServicePanel = calledBy;

    // let the session know we are doing this
    internalConfig_ = &session_->scaleConfig->internalConfig;

    //-----------------------------------------
    // Signals and Slots for the initial screen
    //-----------------------------------------
    connect( ui->btnStart, SIGNAL( clicked() ), this, SLOT( StartClicked() ) );

    connect (ui->btnNextStep_1, SIGNAL( clicked() ), this, SLOT( NextStep_1_Clicked() ) );
    connect (ui->btnNextStep_2, SIGNAL( clicked() ), this, SLOT( NextStep_2_Clicked() ) );
    connect (ui->btnNextStep_3, SIGNAL( clicked() ), this, SLOT( NextStep_3_Clicked() ) );
    connect (ui->btnNextStep_4, SIGNAL( clicked() ), this, SLOT( NextStep_4_Clicked() ) );
    connect (ui->btnNextStep_5, SIGNAL( clicked() ), this, SLOT( NextStep_5_Clicked() ) );
    connect (ui->btnNextStep_6, SIGNAL( clicked() ), this, SLOT( NextStep_6_Clicked() ) );
    connect (ui->btnNextStep_7, SIGNAL( clicked() ), this, SLOT( NextStep_7_Clicked() ) );

    connect( ui->btnCompleteCalibration, SIGNAL( clicked() ), this, SLOT( CompleteCalibrationClicked() ) );
    connect( ui->btnCancel, SIGNAL( clicked() ), this, SLOT( CancelClicked() ) );

    //------------------------------------------------------
    // Signals and Slots for the Calibration Complete screen
    //------------------------------------------------------
    connect( ui->btnCC_ApplyGainFactor, SIGNAL( clicked() ), this, SLOT( CC_ApplyGainFactorClicked() ) );
    connect( ui->btnCC_Done, SIGNAL( clicked() ), this, SLOT( CC_DoneClicked() ) );

    //---------------------------------------------------------------------
    // Signal and slot so the widget can process incoming session messgages
    //---------------------------------------------------------------------
    connect( session_, SIGNAL( receive( SessionMessage ) ), this, SLOT( onSessionMessageReceive( SessionMessage ) ) );

    tiltX = 0.0;
    tiltY = 0.0;
    cancelClicked = 0;
    valuemaxIsEnabled = false;                      // value max not enabled by default

    waitOnTiltMsg_ = 0;

    //-----------------------------
    // Send Weigher Inquiry Request
    //-----------------------------
    session_->send( Weigher, WgReqInquire );        // request an inquire msg to determine if we are a valuemax enabled scale

    translate();
}

weigherCalibrate::~weigherCalibrate()
{
    if( internalConfig_->isScaleCalibrating )
    {
        SendCalibrationEnd();
        weigherCts = 0;
        internalConfig_->isScaleCalibrating = false;
    }

    delete ui;
}

void weigherCalibrate::translate()
{
    ui->retranslateUi(this);

    // FT-695
    ui->lblUnitsHeading->setText( unitsType == "1" ? tr( "lbs" ) : tr( "kgs" ) );

    update();
}

//------------------------------------------------------------------------------
// Used with numericLineEdit widget to validate Calibration Complete Gain Factor
//------------------------------------------------------------------------------
int weigherCalibrate::makeValidatorTop( int digits )
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
// Only show certain fields on the initial screen
//-----------------------------------------------
void weigherCalibrate::initialize()
{
    ui->btnNextStep_1->hide();
    ui->btnNextStep_2->hide();
    ui->btnNextStep_3->hide();
    ui->btnNextStep_4->hide();
    ui->btnNextStep_5->hide();
    ui->btnNextStep_6->hide();
    ui->btnNextStep_7->hide();
    ui->btnCompleteCalibration->hide();
}

//-----------------------------------------------------
// Initialize Weigher Units columns based on Lbs or Kgs
//-----------------------------------------------------
void weigherCalibrate::UpdateStepWeightDisplay(QString weightType)
{
    // FT-695
    unitsType = weightType;

    if(weightType == "1")
    {
        ui->lblUnitsHeading->setText( tr( "lbs" ) );
        ui->lblUnitsData_1->setText( "0" );
        ui->lblUnitsData_2->setText( "10" );
        ui->lblUnitsData_3->setText( "20" );
        ui->lblUnitsData_4->setText( "30" );
        ui->lblUnitsData_5->setText( "20" );
        ui->lblUnitsData_6->setText( "10" );
        ui->lblUnitsData_7->setText( "0" );
    }
    else
    {
        ui->lblUnitsHeading->setText( tr( "kgs" ) );
        ui->lblUnitsData_1->setText( "0" );
        ui->lblUnitsData_2->setText( "5" );
        ui->lblUnitsData_3->setText( "10" );
        ui->lblUnitsData_4->setText( "15" );
        ui->lblUnitsData_5->setText( "10" );
        ui->lblUnitsData_6->setText( "5" );
        ui->lblUnitsData_7->setText( "0" );
    }
}

//----------------------------------------------------------
// Update the data for the correct step in the Zeroed column
//----------------------------------------------------------
void weigherCalibrate::UpdateZeroedData(int stepNo, QString value)
{
    switch(stepNo)
    {
        case 1:
            ui->lblZeroedData_1->setText(value);
            break;

        case 2:
            ui->lblZeroedData_2->setText(value);
            break;

        case 3:
            ui->lblZeroedData_3->setText(value);
            break;

        case 4:
            ui->lblZeroedData_4->setText(value);
            break;

        case 5:
            ui->lblZeroedData_5->setText(value);
            break;

        case 6:
            ui->lblZeroedData_6->setText(value);
            break;

        case 7:
            ui->lblZeroedData_7->setText(value);
            break;
    }
}

//---------------------------------------------------------
// Update the data for the correct step in the Delta column
//---------------------------------------------------------
void weigherCalibrate::UpdateDeltaData(int stepNo, QString value)
{
    switch(stepNo)
    {
        case 1:
            ui->lblDeltaData_1->setText(value);
            break;

        case 2:
            ui->lblDeltaData_2->setText(value);
            break;

        case 3:
            ui->lblDeltaData_3->setText(value);
            break;

        case 4:
            ui->lblDeltaData_4->setText(value);
            break;

        case 5:
            ui->lblDeltaData_5->setText(value);
            break;

        case 6:
            ui->lblDeltaData_6->setText(value);
            break;

        case 7:
            ui->lblDeltaData_7->setText(value);
            break;
    }
}

//--------------------------------------------------------------
// Update the data for the correct step in the Non-Zeroed column
//--------------------------------------------------------------
void weigherCalibrate::UpdateNonZeroedData(int stepNo, QString value)
{
    switch(stepNo)
    {
        case 1:
            ui->lblNonZeroedData_1->setText(value);
            break;

        case 2:
            ui->lblNonZeroedData_2->setText(value);
            break;

        case 3:
            ui->lblNonZeroedData_3->setText(value);
            break;

        case 4:
            ui->lblNonZeroedData_4->setText(value);
            break;

        case 5:
            ui->lblNonZeroedData_5->setText(value);
            break;

        case 6:
            ui->lblNonZeroedData_6->setText(value);
            break;

        case 7:
            ui->lblNonZeroedData_7->setText(value);
            break;
    }
}

//-------------------------------------------------------
// Show or Hide the Next Step button for the correct step
//-------------------------------------------------------
void weigherCalibrate::ShowOrHideStepButton(int stepNo, bool visibityFlag)
{
    switch(stepNo)
    {
        case 1:
            ui->btnNextStep_1->setVisible(visibityFlag);
            break;

        case 2:
            ui->btnNextStep_2->setVisible(visibityFlag);
            break;

        case 3:
            ui->btnNextStep_3->setVisible(visibityFlag);
            break;

        case 4:
            ui->btnNextStep_4->setVisible(visibityFlag);
            break;

        case 5:
            ui->btnNextStep_5->setVisible(visibityFlag);
            break;

        case 6:
            ui->btnNextStep_6->setVisible(visibityFlag);
            break;

        case 7:
            ui->btnNextStep_7->setVisible(visibityFlag);
            break;
    }
}

//---------------------------------------------
// Show the Calibration Complete screen overlay
//---------------------------------------------
void weigherCalibrate::ShowCalibrationCompleteScreen(QString zeroedValue, QString nonZeroedValue, QString deltaValue)
{
    ui->lblCC_ZeroedData->setText( zeroedValue );
    ui->lblCC_NonZeroedData->setText( nonZeroedValue );
    ui->txtCC_GainFactor->setText( deltaValue );

    // Set up validator for Calibration Complete Gain Factor using the Numeric Line Edit
    ui->txtCC_GainFactor->setValidator(CC_GainFactorValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtCC_GainFactor->setDecimalPlaces(0);
    ui->txtCC_GainFactor->setInputWrapping(true);
    CC_GainFactorValidator_->setTop(makeValidatorTop( 7 ));		//	HA-39

    // Move the overlay in position so it covers the initial Weigher Calibrate screen
    ui->gbCalibrationComplete->move( (mainWindow_->width() * 5) / SessionMainWindow::scalingDividerX_, (mainWindow_->height() * 0) / SessionMainWindow::scalingDividerY_ );        // left,top
    ui->gbCalibrationComplete->show();
}

//---------------------------------
// Sends message to end calibration
//---------------------------------
void weigherCalibrate::SendCalibrationEnd()
{
    session_->send( Weigher, WgCalibrationEnd, "2" );

    // kill the counts too
    session_->send( Weigher, WgMode, "3" );
    weigherCts = 0;
}

//--------------------------------------------
// Clears fields when Cancel button is clicked
//--------------------------------------------
void weigherCalibrate::ResetOnCancel()
{
    initialize();

    ui->btnStart->show();

    ui->lblUnitsHeading->clear();
    ui->lblUnitsData_1->clear();
    ui->lblUnitsData_2->clear();
    ui->lblUnitsData_3->clear();
    ui->lblUnitsData_4->clear();
    ui->lblUnitsData_5->clear();
    ui->lblUnitsData_6->clear();
    ui->lblUnitsData_7->clear();

    ui->lblZeroedData_1->clear();
    ui->lblZeroedData_2->clear();
    ui->lblZeroedData_3->clear();
    ui->lblZeroedData_4->clear();
    ui->lblZeroedData_5->clear();
    ui->lblZeroedData_6->clear();
    ui->lblZeroedData_7->clear();

    ui->lblDeltaData_1->clear();
    ui->lblDeltaData_2->clear();
    ui->lblDeltaData_3->clear();
    ui->lblDeltaData_4->clear();
    ui->lblDeltaData_5->clear();
    ui->lblDeltaData_6->clear();
    ui->lblDeltaData_7->clear();

    ui->lblNonZeroedData_1->clear();
    ui->lblNonZeroedData_2->clear();
    ui->lblNonZeroedData_3->clear();
    ui->lblNonZeroedData_4->clear();
    ui->lblNonZeroedData_5->clear();
    ui->lblNonZeroedData_6->clear();
    ui->lblNonZeroedData_7->clear();

    // kill the counts too
    session_->send( Weigher, WgMode, "3" );
    weigherCts = 0;
}

//--------------------------------
// Displays the Service Mode Panel
//--------------------------------
void weigherCalibrate::DisplayServiceModePanel()
{
    mainWindow_->showMenuPanel( SessionMainWindow::ServiceMenuPanelMode );
}

//---------------------------------------------------------------------------
// Slot to allow the widget to handle processing of incoming session messages
//---------------------------------------------------------------------------
void weigherCalibrate::onSessionMessageReceive( const SessionMessage& msg )
{
    // Weigher Calibrate Step
    if ((Weigher == msg[0].toInt() ) && ( WgCalibrationStep == msg[1].toInt() ))
    {
        DomCalibrateScale m( msg );

        int currentStep = m.currentStep().toInt();

        //--------------------------------------------------------------
        // This message with currentStep of 99 is received when the user
        // clicks the selection on the "Select Type of Weights" dialog
        //--------------------------------------------------------------
        if(currentStep == 99)
        {
            //-------------------------------------------------------
            // The "zeroes" field indicates whether to use Lbs or Kgs
            //      A value of "1" indicates to use Lbs
            //-------------------------------------------------------
            QString weightType = m.zeroes();
            UpdateStepWeightDisplay(weightType);

            return;
        }

        //////////////////////////////////////////////////////////

        //--------------------------------------------------------
        // Othewise this message is received for each of the steps
        // in the calibration process
        //--------------------------------------------------------
        int nextStep = currentStep + 1;

        if(nextStep < 8)
        {
            const QString zeroesForDisplay( m.zeroesForDisplay() );

            UpdateZeroedData(nextStep, (zeroesForDisplay.length() ? zeroesForDisplay : m.zeroes()) );
            UpdateNonZeroedData(nextStep, m.nonzeroes() );

            ShowOrHideStepButton( currentStep, false );
            ShowOrHideStepButton( nextStep, true );

            if(currentStep > 0)
                UpdateDeltaData(currentStep, m.delta() );
        }
        else if(nextStep == 8)
        {
            UpdateDeltaData(currentStep, m.delta() );

            ShowOrHideStepButton( currentStep, false );

            ui->btnCompleteCalibration->show();
        }
        // This is received after the "Complete Calibration" message is sent
        else if(nextStep == 9)
        {
            const QString zeroesForDisplay( m.zeroesForDisplay() );

            ShowCalibrationCompleteScreen( (zeroesForDisplay.length() ? zeroesForDisplay : m.zeroes()), m.nonzeroes(), m.delta() );
        }
    }

    // 1,2 - Scale Inquiry Info - determine if valuemax scale
    else if ((Weigher == msg[0].toInt() ) && ( WgInquire == msg[1].toInt() ))
    {
        DomWeigherConfigRead m( msg );
        valuemaxIsEnabled  = m.isScaleValuemax();

        //------------------------------------------------------------
        // Send message to start the repeating Weigher Counts messages
        //------------------------------------------------------------
        weigherCts = 0;
        session_->send( Weigher, WgMode, "1" );
    }

    // 1,104 Weigher Inquire message - tells us if we are out of tilt
    else if ( (Weigher == msg[0].toInt() ) && ( WgStatusCounts == msg[1].toInt() ) )
    {
        if( valuemaxIsEnabled || FAKE_KPC_TESTING )
        {
            // dont care about this if not value max enabled
            DomWeigherInfoCountsRead m( msg );

            QString tiltXStr;
            QString tiltYStr;
            tiltXStr.sprintf( "%.2f", m.tiltX().toDouble() / 100.0 );
            tiltYStr.sprintf( "%.2f", m.tiltY().toDouble() / 100.0 );

            tiltX =  m.tiltX().toDouble() / 100.0;
            tiltY =  m.tiltY().toDouble() / 100.0;

            bool goodToGo = sendOutOfHalfDegreeError();

            if( goodToGo )
                weigherCts++;
        }
    }

    // Check for the message dialog dislayed when clicking the Cancel button
    if ((Error == msg[0].toInt() ) && ( ErrWeigherDevice == msg[1].toInt() )) // 103,1
    {
        returnedErrorsWeigherDev m( msg );

        if( 1011 == m.errorCode() ) // SOF-3559: Do not clear screen for "Delta change is too big ..."
            return;                 // 103,1,1011,5

        // Clear all of the fields and buttons

        // Dialog when Cancel clicked - Weigher warm-up procedure message
        if ( 1004 == m.errorCode() )// 103,1,1004
        {
            if( cancelClicked )
            {
                // Clear all of the fields and buttons
                ResetOnCancel();

                if( calledFromServicePanel == 1)
                    // If initially called by Service Mode Panel then display it now
                    DisplayServiceModePanel();

                cancelClicked = 0;
            }
        }
    }
}

//---------------------------------------------------------------------
// Slot for when the Start button clicked, so send the correct message,
// hide the Start button and show the Next Step button for first step
//---------------------------------------------------------------------
void weigherCalibrate::StartClicked()
{
    bool goodToGo = sendOutOfHalfDegreeError();

    if( goodToGo )
    {
        internalConfig_->isScaleCalibrating = true;

        SendCalibrationStepNumber( "0" );

        ui->btnStart->hide();

        ShowOrHideStepButton( 1, true );

        //------------------------------------------------------------
        // Send message to start the repeating Weigher Counts messages
        //------------------------------------------------------------
        weigherCts = 0;
        //session_->send( Weigher, WgMode, "1" );  // has already been done after receiving 1,2
    }
}

//---------------------------------------------------
// Slots for each of the Next Step buttons
//      send the message with the correct step number
//---------------------------------------------------
void weigherCalibrate::NextStep_1_Clicked()
{
    SendCalibrationStepNumber( "1" );
}

void weigherCalibrate::NextStep_2_Clicked()
{
    SendCalibrationStepNumber( "2" );
}

void weigherCalibrate::NextStep_3_Clicked()
{
    SendCalibrationStepNumber( "3" );
}

void weigherCalibrate::NextStep_4_Clicked()
{
    SendCalibrationStepNumber( "4" );
}

void weigherCalibrate::NextStep_5_Clicked()
{
    SendCalibrationStepNumber( "5" );
}

void weigherCalibrate::NextStep_6_Clicked()
{
    SendCalibrationStepNumber( "6" );
}

void weigherCalibrate::NextStep_7_Clicked()
{
    SendCalibrationStepNumber( "7" );
}

//--------------------------------------------------------------------------
// Used to send the Weigher Calibration message with the correct step number
//--------------------------------------------------------------------------
void weigherCalibrate::SendCalibrationStepNumber(QString stepNumber)
{
    bool goodToGo = sendOutOfHalfDegreeError();

    if( goodToGo )
        session_->send( Weigher, WgCalibrationStep, stepNumber + ",0,0,0" );
}

//------------------------------------------------------------
// Used to send an error if half degree out of tilt
//------------------------------------------------------------
bool weigherCalibrate::sendOutOfHalfDegreeError()
{
    bool val = true;

    if( ( tiltX > 0.501 ) || ( tiltX < -0.501 ) || ( tiltY > 0.501 ) || ( tiltY < -0.501 ) )
    {
        if( 40 == waitOnTiltMsg_ )
            waitOnTiltMsg_ = 0;

        if( 0 == waitOnTiltMsg_ )
        {
            QString tmpStr = QString::number(WeigherDevError::WeigherErrOutOfCalibrateTiltRange) + ",5";
            session_->send( Error, ErrWeigherDevice, tmpStr );
        }

        waitOnTiltMsg_++;

        val = false;
    }

    return val;
}

//------------------------------------------------------------
// Slot for when the Complete Calibration button is clicked on
// the initial screen
//------------------------------------------------------------
void weigherCalibrate::CompleteCalibrationClicked()
{
    session_->send( Weigher, WgCalibrationEnd, "0" );
    internalConfig_->isScaleCalibrating = false;
}

//-----------------------------------------------------------------
// Slot for when the Cancel button is clicked on the initial screen
//-----------------------------------------------------------------
void weigherCalibrate::CancelClicked()
{
    cancelClicked = 1;
    internalConfig_->isScaleCalibrating = false;

    weigherCts = 0;

    SendCalibrationEnd();
}

//------------------------------------------------------
// Slot for when the Apply Gain Factor button is clicked
// on the Calibration Complete screen
//------------------------------------------------------
void weigherCalibrate::CC_ApplyGainFactorClicked()
{
    QString currentGain = ui->txtCC_GainFactor->text();
    if(currentGain == "")
        currentGain = "0";

    session_->send( Weigher, WgCalibrationStep, "8,0," + currentGain + ",0" );
}

//-----------------------------------------
// Slot for when the Done button is clicked
// on the Calibration Complete screen
//-----------------------------------------
void weigherCalibrate::CC_DoneClicked()
{
    //-----------------------------------------------
    // Turn off the repeating Weigher Counts messages
    //-----------------------------------------------
    session_->send( Weigher, WgMode, "3" );
    weigherCts = 0;

    internalConfig_->isScaleCalibrating = false;

    // End the calibration
    session_->send( Weigher, WgCalibrationEnd, "1" );

    ui->btnCC_ApplyGainFactor->hide();
    ui->btnCC_Done->hide();
}

//////////////////////////////

} // hobart namespace
