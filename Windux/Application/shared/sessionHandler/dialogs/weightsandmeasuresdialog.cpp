/**
 * @file weightsandmeasuresdialog.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */


#include "weightsandmeasuresdialog.h"
#include "auditlogdialog.h"
#include "ui_weightsandmeasuresdialog.h"

#include "../sessionmainwindow.h"


#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "logmanager.h"

static const int MAX_MESSAGES = 3;

weightsandmeasuresdialog::weightsandmeasuresdialog( QWidget *parent, bool blocking ) :
    SessionDialog( TopDialogLocation, parent ),
    ui( new Ui::weightsandmeasuresdialog ),
    blockSessionDialogs_( blocking )
{
    setObjectName( "weightsandmeasuresdialog" );

    ui->setupUi(this);

    const QObjectList children( this->children() );

    if(mainWindow_->width() > mainWindow_->scalingDividerX_){
        foreach ( QObject *child, children )
            if ( child->isWidgetType() )
                SessionDialog::scaleWidget( qobject_cast<QWidget*>( child ) );
    }

    // set up SLOTS
    connect(ui->butOK,          SIGNAL( clicked() ),      this,   SLOT( onOkButtonClicked() ) );
    connect(ui->butAuditLog,    SIGNAL( clicked() ),      this,   SLOT( onAuditTrailLogButtonClicked() ) );

    // Check if the right weigher model is connected
    DomWeigherConfigRead *weigherFields = &session_->scaleConfig->weigherConfigRead;
    int wayer_model = weigherFields->weigherModel();

    if( ( SessionMainWindow::HLX1 == wayer_model ||
          SessionMainWindow::SASCALE1 == wayer_model ||     // HT-374
          SessionMainWindow::SASCALE2 == wayer_model ||
          SessionMainWindow::HTM == wayer_model ) &&   // SOF-3140
        ( session_->uiUtilities->isFeatureLicensed( SCALE_CAT3_AUDITTRAIL ) ) )
    {
        ui->butAuditLog->setVisible( true );

        ui->lblConfigParameters->setVisible( false );
        ui->lblConfigParameters_Data->setVisible( false );
        ui->lblLastChanged_Config->setVisible( false );
        ui->lblLastChanged_Config_Data->setVisible( false );
        ui->lblCalibration->setVisible( false );
        ui->lblCalibration_Data->setVisible( false );
        ui->lblLastChanged_Calibration->setVisible( false );
        ui->lblLastChanged_Calibration_Data->setVisible( false );
    }
    else
    {
        ui->butAuditLog->setVisible( false );

        ui->lblConfigParameters->setVisible( true );
        ui->lblConfigParameters_Data->setVisible( true );
        ui->lblLastChanged_Config->setVisible( true );
        ui->lblLastChanged_Config_Data->setVisible( true );
        ui->lblCalibration->setVisible( true );
        ui->lblCalibration_Data->setVisible( true );
        ui->lblLastChanged_Calibration->setVisible( true );
        ui->lblLastChanged_Calibration_Data->setVisible( true );
    }

    // NGW-125
    const QString scaleModel( session_->uiUtilities->scaleName() );

    if ( scaleModel.contains( "NGW" ) )
    {
        ui->lblIndicatorApproval->hide();
        ui->lblIndicatorApproval_Data->hide();
    }

    // SOF-2737: Do not display Hardware version for wrapper
    if( session_->isPrepack() )
    {
        ui->lblHardwareVersion->hide();
        ui->lblHardwareVersion_Data->hide();
    }

    // Weigher Software Version
    ui->lblSoftwareVersion_Data->setText( "0.0.0" );
    ui->lblHardwareVersion_Data->setText( "0.0" );

    updateVersionInfo( WgInquire );                 // use cached data immediately
    updateVersionInfo( SysUsbWgStatus );            // if it changes, then we will use it

    //---------------------------------
    // Send request for Weigher Inquiry 1,36
    //---------------------------------
    session_->send( Weigher, WgReqInquire );

    //---------------------------------------
    // Send request for Weigher System Status 2,13
    //---------------------------------------
    session_->send( WeigherSystem, SysReqStatus );

    //------------------------------
    // Send request for Version info 100,2 - not used - comment out. Find out what this screen does use.
    //------------------------------
    //session_->send( SessionDev, SnVersionInfo );      // not used - excused from its duties

    // Set the text for Class - it should always be "III"
    ui->lblClass_Data->setText( "III" );

    // The Temperature Range is hard-coded, so set the text
    SetTemperatureRange();

    translate();

    // FT-415
    // while this widget is open do not show system dialogs
    if ( blockSessionDialogs_ )
        session_->send( SessionDev, SnStopDialogs );

}

weightsandmeasuresdialog::~weightsandmeasuresdialog()
{
    // FT-415
    // resume dialogs
    if ( blockSessionDialogs_ )
        session_->send( SessionDev, SnRestartDialogs );

    ClearSettings();

    delete ui;
}

QStringList weightsandmeasuresdialog::allDataFields() const
{
    QTime doneWaitingTime( QTime::currentTime().addSecs( 10 ) );  // HW-52: Increased the timeout
    QStringList result;

    // wait up to 3 seconds for message to come in
    while (( 0 < MAX_MESSAGES ) && ( QTime::currentTime() < doneWaitingTime ))
    {
        // process pending events
        QApplication::sendPostedEvents();
        QApplication::flush();

        QApplication::processEvents();
    }

    // format result
    result.append( ui->lblIndicatorSoftwareVersion_Data->text() );
    result.append( ui->lblIndicatorApproval_Data->text() );
    result.append( ui->lblIndicatorN_Data->text() );

    result.append( ui->lblWeigherModel_Data->text() );
    result.append( ui->lblSoftwareVersion_Data->text() );
    result.append( ui->lblHardwareVersion_Data->text() );
    result.append( ui->lblCapacity_Data->text() );
    result.append( ui->lblE_Data->text() );
    result.append( ui->lblClass_Data->text() );
    result.append( ui->lblN_Data->text() );
    result.append( ui->lblApproval_US_Data->text() );
    result.append( ui->lblApproval_CAN_Data->text() );

    if( !( session_->uiUtilities->isFeatureLicensed( SCALE_CAT3_AUDITTRAIL ) ) ||
         ( session_->isPrepack() ) )
    {
        result.append( ui->lblConfigParameters_Data->text() );
        result.append( ui->lblLastChanged_Config_Data->text() );
        result.append( ui->lblCalibration_Data->text() );
        result.append( ui->lblLastChanged_Calibration_Data->text() );
    }

    result.append( ui->lblTempRange_Data->text() );

    return result;
}

void weightsandmeasuresdialog::translate()
{
    ui->retranslateUi(this);

    update();
}

void weightsandmeasuresdialog::onOkButtonClicked()
{
    accept();
}

void weightsandmeasuresdialog::onAuditTrailLogButtonClicked()
{
    // Show the Audit Trail Log for the weigher
    bool isBlocking = true;
    QString init_text = "Audit trail information...";
    auditlogdialog *dialog = new auditlogdialog( mainWindow_, isBlocking, init_text);
    dialog->open();

    accept();
}

void weightsandmeasuresdialog::onSessionMessageReceive( const SessionMessage& msg )
{
    // Software Version Info
   // deleted it, message data not even used and its the slowest

    // Weigher Inquiry Info
    if ((Weigher == msg[0].toInt() ) && ( WgInquire == msg[1].toInt() ))
    {
        updateVersionInfo( WgInquire );
    }

    // Weigher Version Info - USB Weigher
    else if ((WeigherSystem == msg[0].toInt() ) && ( SysUsbWgStatus == msg[1].toInt() ))
    {
        updateVersionInfo( SysUsbWgStatus );
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void weightsandmeasuresdialog::updateVersionInfo( int updateInfo )
{

    if( WgInquire == updateInfo )
    {
        DomWeigherConfigRead *weigherConfigRead = &session_->scaleConfig->weigherConfigRead;
        MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;

        QString MaxWeight = weigherConfigRead->weigherMaximumWt();
        int ScaleFactor = weigherConfigRead->weigherScaleFactor();
        int WeigherModel = weigherConfigRead->weigherModel();
        const QString scaleModel( settings->scaleName() );
        QString tmpStr("");

        //-------------------
        // Indicator Approval
        //-------------------
        if( session_->isServiceCounter() )
        {
            // FT-199 - Can't set the approval yet. Indicate Pending, but once we get approval,
            // uncomment the next line again and remove the Pending text. Of course set the
            // Indicator Approval to another value besides 07-047A.
            // Update 10-09-2013: Updated approval the new NTEP CC#. No tr() needed
            // SOF-3526: New NTEP CC# for HTm scale
            if( WeigherModel == SessionMainWindow::HTM )
            {
                if(scaleModel.startsWith("FS"))
                {
                    if( ui->lblIndicatorApproval_Data->text() != "22-078" )
                        ui->lblIndicatorApproval_Data->setText("22-078");

                    if( ui->lblApproval_CAN_Data->text() != "AM-6216C"  )
                        ui->lblApproval_CAN_Data->setText( "AM-6216C" );
                }
                else
                {
                    if( ui->lblIndicatorApproval_Data->text() != "20-026" )
                        ui->lblIndicatorApproval_Data->setText("20-026");
                    if( ui->lblApproval_CAN_Data->text() !=  "AM-6148CC" )
                        ui->lblApproval_CAN_Data->setText( "AM-6148CC" );
                }
            }
            else
            {
                if( ui->lblIndicatorApproval_Data->text() != "13-101" )
                    ui->lblIndicatorApproval_Data->setText("13-101");
                if( ui->lblApproval_CAN_Data->text() != "AM-5927C" )
                    ui->lblApproval_CAN_Data->setText( "AM-5927C" );
            }
        }
        else
        {
            if( ui->lblIndicatorApproval_Data->text() != "11-005" )
                ui->lblIndicatorApproval_Data->setText( "11-005" );		//	HA-203

        }

        //----------------
        // Indicator n max
        //----------------
        QString Indicator_n_Max;

        if( session_->isServiceCounter() )
            //-----------------------------------------------------
            // HT-374
            // If/when Canada approves the 50 lb SASCALE-1 Weigher,
            // then this should be "5000" for Service Counter also.
            //-----------------------------------------------------

            // SOF-893
            if( WeigherModel == SessionMainWindow::SASCALE1 )
                Indicator_n_Max = "5000";
            else
                Indicator_n_Max = "3000";
        else
            Indicator_n_Max = "3000";

        if( ui->lblIndicatorN_Data->text() != Indicator_n_Max  )
            ui->lblIndicatorN_Data->setText( Indicator_n_Max );

        // Weigher Model
        if( ui->lblWeigherModel_Data->text() !=  SetWeigherModelName( WeigherModel ) )
            ui->lblWeigherModel_Data->setText( SetWeigherModelName( WeigherModel ) );

        // Capacity
        tmpStr = MaxWeight + (settings->isScaleMetric() ? " kg" : " lb");
        if( ui->lblCapacity_Data->text() != tmpStr )
            ui->lblCapacity_Data->setText( tmpStr );

        // eMin
        QString eMinValue;
        if(ScaleFactor == 2)
            eMinValue = "0.002";
        else if(ScaleFactor == 5)
            eMinValue = "0.005";
        else
            eMinValue = "0.01";

        session_->uiUtilities->youGottaSetTheSeparator( eMinValue );
        tmpStr = eMinValue + (settings->isScaleMetric() ? " kg" : " lb") ;
        if( ui->lblE_Data->text() != tmpStr )
            ui->lblE_Data->setText( tmpStr );

        // n max
        QString n_Max;

        if(MaxWeight == "15" || MaxWeight == "30")
            n_Max = "3000";
        else
            n_Max = "5000";

        if( ui->lblN_Data->text() !=  n_Max )
            ui->lblN_Data->setText( n_Max );

        // Approval - U.S.
        // FT-199 - Can't set the approval yet. Indicate Pending, but once we get approval,
        // uncomment the next line again and remove the Pending text
        // ui->lblApproval_US_Data->setText( Set_US_Approval( WeigherModel ) );
        // Update 10-09-2013: Updated approval the new NTEP CC#. No tr() needed
        tmpStr = Set_US_Approval( WeigherModel );
        if( ui->lblApproval_US_Data->text() != tmpStr )
            ui->lblApproval_US_Data->setText(tmpStr );

        // Approval - Canada
        // FT-199 - Can't set the approval yet. Indicate Pending, but once we get approval,
        // uncomment the next line again and remove the Pending text
        tmpStr = Set_CAN_Approval( WeigherModel );
        if( ui->lblApproval_CAN_Data->text() != tmpStr )
            ui->lblApproval_CAN_Data->setText(tmpStr );

        // SOF-894 Hide Canada Approval for SASCALE-1
        if( WeigherModel == SessionMainWindow::SASCALE1 )
        {
            if( settings->countryCode() == "CA" )
            {
                ui->lblApproval_CAN->hide();
                ui->lblApproval_CAN_Data->hide();
            }
        }

        if( !( session_->uiUtilities->isFeatureLicensed( SCALE_CAT3_AUDITTRAIL ) ) ||
             ( session_->isPrepack() ) )
        {
            // Configuration Parameters
            if( ui->lblConfigParameters_Data->text() !=  weigherConfigRead->timesConfiguredRaw() )
                ui->lblConfigParameters_Data->setText( weigherConfigRead->timesConfiguredRaw() );

            // Configuration - Last Date Changed
            if( ui->lblLastChanged_Config_Data->text() != weigherConfigRead->lastDateConfiguredRaw() )
                ui->lblLastChanged_Config_Data->setText( weigherConfigRead->lastDateConfiguredRaw() );

            // Calibration
            if( ui->lblCalibration_Data->text() != weigherConfigRead->timesCalibratedRaw() )
                ui->lblCalibration_Data->setText( weigherConfigRead->timesCalibratedRaw() );

            // Calibration - Last Date Changed
            if( ui->lblLastChanged_Calibration_Data->text() !=  weigherConfigRead->lastDateCalibratedRaw() )
                ui->lblLastChanged_Calibration_Data->setText( weigherConfigRead->lastDateCalibratedRaw() );
        }
    }
    else if( SysUsbWgStatus == updateInfo )
    {
        DomUsbWeigherPowerupTests *weigherPowerupTests = &session_->scaleConfig->weigherPowerupTests;

        // Weigher Software Version
        if( ui->lblSoftwareVersion_Data->text() != weigherPowerupTests->weigherSoftwareVersion() )
            ui->lblSoftwareVersion_Data->setText( weigherPowerupTests->weigherSoftwareVersion() );

        // Weigher Hardware Version
        if( ui->lblHardwareVersion_Data->text() != weigherPowerupTests->hardwareVersion() )
            ui->lblHardwareVersion_Data->setText( weigherPowerupTests->hardwareVersion() );
    }
}

//------------------------------------------------------
// Send message to turn Continuous Date and Time back on
//------------------------------------------------------
void weightsandmeasuresdialog::ClearSettings()
{
    //-----------------------------------------------
    // Send request to stop the date and time message
    //-----------------------------------------------
    session_->send( SessionDev, SnContinuousDateTime );
}


//////////////////////////////////////////////////////////
QSize weightsandmeasuresdialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 950) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 575) / SessionMainWindow::scalingDividerY_ );
}


//-------------------------------------------------------
// Sets the Weigher Model Name based on the Weigher Model
//-------------------------------------------------------
QString weightsandmeasuresdialog::SetWeigherModelName( int WeigherModel )
{
    QString weigherModelName = tr( "None" );
    const QString scaleModel( session_->scaleConfig->scaleConfig.scaleName() );

    switch ( WeigherModel )
    {
        case SessionMainWindow::EPSA:
	        weigherModelName = "EPSA";
	        break;

        case SessionMainWindow::HLX1:
	        weigherModelName = "HT Series"; // FT-203: was "HLX-1";
	        break;

        case SessionMainWindow::AWS:
        case SessionMainWindow::AWS2:
        {
            // NGW-125
            if ( scaleModel.contains( "NGW" ) )
                weigherModelName = "NGW";
            else
                weigherModelName = "AWS";

	        break;
        }

        case SessionMainWindow::EPSA2:
	        weigherModelName = "EPSA-2";
	        break;

        case SessionMainWindow::SASCALE1:
            weigherModelName = "SASCALE-1";
	        break;

        case SessionMainWindow::SASCALE2:
            weigherModelName = "SASCALE-2";
            break;
        // is a KPC can weigher
        case SessionMainWindow::HTM:{

            if(scaleModel.startsWith("FS"))
            {
               weigherModelName = "FS";
            }
            else
            {
                // SOF-3526: Changed to "HTm" to match NTEP certificate
                weigherModelName ="HTm";
            }


            break;
         }

	    default:
	        break;
    }

    return weigherModelName;
}

//--------------------------------------------------
// Sets the U.S. Approval based on the Weigher Model
//--------------------------------------------------
QString weightsandmeasuresdialog::Set_US_Approval( int WeigherModel )
{
    QString US_Approval = "";
    const QString scaleModel( session_->scaleConfig->scaleConfig.scaleName() );


    switch( WeigherModel )
    {
        case SessionMainWindow::HLX1:
            US_Approval = "13-101";
            break;

        case SessionMainWindow::EPSA:
        case SessionMainWindow::EPSA2:
        case SessionMainWindow::AWS:
        case SessionMainWindow::AWS2:
        {
            // NGW-125
            if ( scaleModel.contains( "NGW" ) )
                US_Approval = "17-028";
            else
                US_Approval = "11-005";

            break;
        }

        case SessionMainWindow::SASCALE1:
        case SessionMainWindow::SASCALE2:
            US_Approval = "16-076";
            break;

        case SessionMainWindow::HTM:{

            if(scaleModel.startsWith("FS"))
            {
                US_Approval = "22-078";
            }
            else
            {
                // SOF-3526: New NTEP CC# for HTm scale
                US_Approval = "20-026";
            }

            break;
    }
    }

    return US_Approval;
}

//------------------------------------------------------
// Sets the Canadian Approval based on the Weigher Model
//------------------------------------------------------
QString weightsandmeasuresdialog::Set_CAN_Approval( int WeigherModel )
{
    QString CAN_Approval = "";

    switch( WeigherModel )
    {
        case SessionMainWindow::HLX1:
            CAN_Approval = "AM-5927C";
            break;

        case SessionMainWindow::EPSA:
        case SessionMainWindow::EPSA2:
            CAN_Approval = "AM-5664C";
            break;

        case SessionMainWindow::AWS:
        case SessionMainWindow::AWS2:
        {
            const QString scaleModel( session_->uiUtilities->scaleName() );

            // NGW-125
            if ( scaleModel.contains( "NGW" ) )
                CAN_Approval = "AM-6056C";
            else
                CAN_Approval = "AM-5635C";

            break;
        }

        case SessionMainWindow::SASCALE1:
        case SessionMainWindow::SASCALE2:
            CAN_Approval = "AM-16-0052";
            break;

        case SessionMainWindow::HTM:
            // SOF-3923: New Canadian Approval for HTm
            CAN_Approval = "AM-6148C";
            break;
    }

    return CAN_Approval;
}

//----------------------------------------
// Sets the text for the Temperature Range
//----------------------------------------
void weightsandmeasuresdialog::SetTemperatureRange()
{
    QString TempRangeText = "-10 C " + tr( "to" ) + " 40 C";

    DomWeigherConfigRead *weigherFields = &session_->scaleConfig->weigherConfigRead;
    int weigher_model = weigherFields->weigherModel();

    if ( weigher_model == SessionMainWindow::SASCALE1 || weigher_model == SessionMainWindow::SASCALE2
            || weigher_model == SessionMainWindow::EPSA || weigher_model == SessionMainWindow::EPSA2 )
        TempRangeText = "0 C " + tr( "to " ) + "40 C";

    if( ui->lblTempRange_Data->text() != TempRangeText )
        ui->lblTempRange_Data->setText( TempRangeText );
}
