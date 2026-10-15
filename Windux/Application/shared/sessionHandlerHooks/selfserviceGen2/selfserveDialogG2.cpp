
/**
 * @file selfserveDialogG2.cpp
 *
 * @copyright Copyright (c) 20022, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 */


#include "selfserveDialogG2.h"
#include "ui_selfserveDialogG2.h"

//#include "../sessioninfo.h"

//#include "../modes/supervisormode.h"

#include <Session.h>
#include <sessionhooks.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <ScaleClasses.h>
#include <logmanager.h>


#include <QtGui>
#include <QtCore>
#include <QDebug>
#include <QTimer>
#include <QPixmap>

// Hobart namespace
namespace hobart {

selfserveDialogG2::selfserveDialogG2( selfServiceHook *h, Session *s, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveDialogG2),
    hook_( h ),
    //mainWindow_( NULL ),
    session_( s ),
    config_( NULL ),
    scaleConfig_( NULL ),
    errorTitle_( NULL ),
    errorInstructions_( NULL ),
    code_( "0" ),
    style_( 0 ),
    valid_( true )
{
    ui->setupUi(this);

    // QWidget#selfserveDialogG2 {\n	"background-image: url(:/images/wo_dialog.png)";\n	/* border: 3px solid rgb(0,176,240); */\n}
    //setStyleSheet("selfserveDialogG2 {background-image: url(:/images/wo_dialog.png);}");

    // This works when I add it to the UI FORM, and I can test display it, but when I RUN it, the QWidget goes completely transparent
    //#selfserveDialogG2{\nbackground-image: url(:/images/wo_dialog.png);\n}

    //mainWindow_     = mainWindow;

    config_         = session_->scaleConfig;
    scaleConfig_    = &config_->scaleConfig;

    initialize();
}

selfserveDialogG2::~selfserveDialogG2()
{
    delete ui;

    if( config_ )
        delete config_;

    if( scaleConfig_ )
        delete scaleConfig_;
}

//-----------------------------------------------
// Only show certain fields on the initial screen
//-----------------------------------------------
void selfserveDialogG2::initialize()
{
    connect(ui->butOK,          SIGNAL(clicked()),      this,           SLOT(onOKExit()     )  );
    connect(ui->butClose,       SIGNAL(clicked()),      this,           SLOT(onCancelExit() )  );
    connect(ui->butCancel,      SIGNAL(clicked()),      this,           SLOT(onCancelExit() )  );
    connect(ui->butRezero,      SIGNAL(clicked()),      this,           SLOT(onRezeroExit() )  );
    
    errorTitle_         = ui->lblErrorTitle;
    errorInstructions_  = ui->lblErrorText;

    ok_     = ui->butOK;
    cancel_ = ui->butCancel;
    rezero_ = ui->butRezero;

    ok_->setVisible( false );
    cancel_->setVisible( false );
    rezero_->setVisible( false );
    ui->butClose->setVisible( false );

    //updateText();
    //translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::translate()
{
    // updating it in just the initialize didn't even draw it.
    updateText();

    ui->retranslateUi( this );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString selfserveDialogG2::getErrorCode() const
{
    return code_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveDialogG2::getStyleCode() const
{
    return style_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::updateText()
{
    // QTimer got these to work
    QString errorTitle( getErrorTitle() );
    QString errorText( getErrorText() );

    errorTitle_->setText( errorTitle );
    errorInstructions_->setText( errorText );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::updateCodes( const QString& value, int style )
{
    code_ = value;
    style_ = style;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::refreshText()
{
    if( 0 == code_ )
        return;

    if( ( WeigherDevError::WeigherErrNegativeGrossWeight == code_.toInt() ) || ( WeigherDevError::ssWeigherErrNegativeGrossWeight == code_.toInt() ) )
    {
        ui->butOK->setVisible( false );                             // Yes this is the only message that has a rezero button
        ui->butRezero->setVisible( true );
    }
    else
    {
        ui->butOK->setVisible( false );
        ui->butRezero->setVisible( false );
        ui->butCancel->setVisible( false );
        addButtons( style_ );
    }

    updateText();

    errorTitle_->setVisible( false );
    errorInstructions_->setVisible( false );
    errorTitle_->setVisible( true );
    errorInstructions_->setVisible( true );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::closeDialog(QString okCancel )
{
    LOG_CLASS_INFO( "selfserveDialogG2", "closeDialog" );

    QString params = code_ + "," + QString::number( style_ ) + "," + okCancel;
    session_->send(Error, ErrRemoveAnyModalDialogDevice, params);

    config_->internalConfig.selfServicePreventClicks = false;

    cancel_->setText( tr("Cancel") );        // we could of changed it
    ok_->setText( tr("OK") );

    close();

    if( code_.toInt() == ProductDevError::ProductErrPrintBadByCount )
        emit popQuantity();

    if( ( 0 == style_ ) || ( 3 == style_ ) || ( 9 == style_ ) )
    {
        QString test =  sessionHooks::instance()->getFieldValue( sessionHooks::selfServiceRestrictedDialog );
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceRestrictedDialog, "0");
        LOG_DEBUG( "Close dialog reset selfServiceRestrictedDialog = 0 as (%s)", qPrintable( test ) );
    }

    code_ = "0";
    style_ = 0;

    config_->internalConfig.selfServicePreventClicks = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString selfserveDialogG2::getErrorTitle()
{ 
    LOG_CLASS_INFO( "selfserveDialogG2", "getErrorTitle" );

    if( code_ == "" )
        return "";
        
    switch ( code_.toInt() )
    {
        // Weigher Errors - 1000
        case WeigherDevError::WeigherErrUnknownError:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrCommLoss:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrNegativeGrossWeight:                                                    // 1002
            // only rezero button message
            return tr( "Weigher Error" );            // REPLACES: Please clear the platter of all products and debris. Check for obstructions contacting the platter. Contact store personnel to REZERO the scale.
        case WeigherDevError::WeigherErrOverGrossWeightLimit:
            return tr( "Weigher Error" );          //
        case WeigherDevError::WeigherErrScaleWarmup:
            return tr( "Weigher Warmup" );
        case WeigherDevError::WeigherErrRezeroing:
            return tr( "Rezero" );
        case WeigherDevError::WeigherErrZeroOutOfMaintZone:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrZeroOutOfMaintZoneStatus:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrRemoveWeight:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrSelectWeightsType:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrPressCalibrateSwitch:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrDeltaChangeTooBig:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrCalibrationSuccessful:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrNotAttached:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrScaleNotZero:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrInvalidAddress:
            return tr( "Weigher Error" );
        case WeigherDevError::WeigherErrManagerDeleted:
            // Not Used:
            return tr( "Weigher Offline" );
        case WeigherDevError::WeigherErrTareDisabled:
            return tr( "Tare Disabled" );
        case WeigherDevError::WeigherErrConfigOutOfRange:
            return tr( "Contact Service" );
        case WeigherDevError::WeigherErrCantRezeroNegativeGrossWeight:
            return tr( "Contact Store Personnel" );
        case WeigherDevError::WeigherErrRemoteConfigurationStarted:
            return tr( "Remote Update" );
        case WeigherDevError::WeigherErrOutOfCalibrateTiltRange:
            return tr( "Level Error" );
        case WeigherDevError::WeigherErrValMaxEepCommFailure:
            return tr( "Max EPP Failure" );
        case WeigherDevError::WeigherErrValMaxEepChecksumFailure:
            return tr( "Contact Service" );
        case WeigherDevError::WeigherErrValMaxEepAcclCommFailure:
            return tr( "Reboot Scale" );
        case WeigherDevError::WeigherErrValMaxTiltReached:
            return tr( "Scale Error" );
        case WeigherDevError::WeigherErrUnderMinimumWeight:
            return tr( "Weigher Error" );                       // > 0 but < min wt to print

        // Weigher Self Service versions of the same messages. Customer friendly text.
        case WeigherDevError:: ssWeigherErrCommLoss:
            return tr( "Out of Service" );
        case WeigherDevError::ssWeigherErrNegativeGrossWeight:
            return tr( "Out of Service" );           // REPLACES: Please clear the platter of all products and debris. Check for obstructions contacting the platter. Contact store personnel to REZERO the scale.
        case WeigherDevError::ssWeigherErrOverGrossWeightLimit:
            return tr( "Too Much Weight" );
        case WeigherDevError::ssWeigherErrZeroOutOfMaintZoneStatus:
            return tr( "Out of Service" );
        case WeigherDevError::ssWeigherErrDeltaChangeTooBig:
            // I think this number is wrong, so I repeated it correctly at 1513: 1522 is actually a Service > Calibration message
            return tr( "Out of Service" );
        case WeigherDevError::ssWeigherErrNotAttached:
            // I think this is the right enum of what was being displayed as the error for 1511
            return tr( "Out of Service" );
        case WeigherDevError::ssWeigherErrCantRezeroNegativeGrossWeight:
            return tr( "Out of Service" );
        case WeigherDevError::ssWeigherErrUnderMinimumWeight:
            // > 0 but < min wt to print
            return tr( "Add Weight" );

        // No 2000s or 3000s

        // Operate Mode Errors - 4000s
        case 4101:
            return tr( "Select Item" );
        case 4102:
            return tr( "Select Item" );
        case 4103:
            return tr( "Print" );
        case 4104:
            return tr( "Add Item" );
        case 4105:
            return tr( "Select Item" );

        // No 4000s, 5000s, or 6000s

        // Printer Errors
        case PrinterErrors::PrinterErrCommLoss:
        case PrinterErrors::PrinterErrHardwareError:
            return tr( "Printer Error" );
        case PrinterErrors::PrinterErrManagerDeleted:
            return tr( "Printer Off" );
        case PrinterErrors::PrinterErrHeadUp:
            return tr( "Insert Cassette" );
        case PrinterErrors::PrinterErrLabelSizing:
            return tr( "Printer Sizing" );
        case PrinterErrors::PrinterErrTakeLabel:
        case PrinterErrors::PrinterErrTakeLabelBeep:
            return tr( "Take Label" );
        case PrinterErrors::PrinterErrLabelJammed:
            return tr( "Check Labels" );
        case PrinterErrors::PrinterErrLabelMissing:
        case PrinterErrors::PrinterErrOutOfMedia:
            return tr( "Label Error" );  
        case PrinterErrors::PrinterErrCassetteMissing:
        case PrinterErrors::PrinterErrThermalOverload:
        case PrinterErrors::PrinterErrThermalShutdown:
            return tr( "System Error" );

        case PrinterErrors::PrinterErrInvalidTypeSetup:
            return tr( "Resetting." );
        case PrinterErrors::PrinterErrMSDisengage:
        case PrinterErrors::PrinterErrMSSituateLabelStock:
        case PrinterErrors::PrinterErrMSEngage:
        case PrinterErrors::PrinterErrMSLabelStockTaut:
        case PrinterErrors::PrinterErrMSCalibrating:
            return tr( "Calibrate Media Sensor" );
        case PrinterErrors::PrinterErrMSComplete:
            return tr( "Calibration Complete" );
        case PrinterErrors::PrinterErrLTClearSensor:
        case PrinterErrors::PrinterErrLTCalibrating:
        case PrinterErrors::PrinterErrLTComplete:
            return tr( "Calibrate Label Taken Sensor" );
        case PrinterErrors::PrinterErrSLAIsOff:
        case PrinterErrors::PrinterErrSLAOutOfLabels:
            return tr( "Label applier error" );
        case PrinterErrors::PrinterErrHeadUpForEpp:
            return tr( "Engage the Printhead." );
        case PrinterErrors::PrinterErrNoSpaceAllowedInName:
        case PrinterErrors::PrinterErrInvalidCharInName:
            return tr( "Invalid Name" );
        case PrinterErrors::PrinterErrNoSizingDueToContinuous:
            return tr( "Label Information" );
        case PrinterErrors::PrinterErrSizedContinuousOK:
            return tr( "Continuous label Information" );
        case PrinterErrors::PrinterErrLabelCutterJammed:
        case PrinterErrors::PrinterErrLabelCutterInterlockOpen:
        case PrinterErrors::PritnerErrLabelCutterTimedOut:
            return tr( "Label Cutter Error" );
        case PrinterErrors::PrinterErrProductionEntryInterlockOpen:
            return tr( "Interlock Open" );
        case PrinterErrors::PrinterErrLowStock:
            return tr( "Label Information" );
        case PrinterErrors::PrinterErrOutOfStock:
            return tr( "Label Error" );
        case PrinterErrors::PrinterErrFSSLabelCutterInterlockOpen:
        case PrinterErrors::PrinterErrFSSLabelDoorInterlockOpen:
            return tr( "Interlock Open" );

        // Printer Self Service Errors - 7500s
        case PrinterErrors::ssPrinterErrCommLoss:
            return tr( "Printer Error" );
        case PrinterErrors::ssPrinterErrManagerDeleted:
            return tr( "Printer Error" );
        case PrinterErrors::ssPrinterErrHeadUp:
            return tr( "Scale Error" );
        case PrinterErrors::ssPrinterErrLabelSizing:
            return tr( "Printer Sizing" );
        case PrinterErrors::ssPrinterErrTakeLabel:
            return tr( "Take Label" );
        case PrinterErrors::ssPrinterErrTakeLabelBeep:
            return tr( "Take Label" );
        case PrinterErrors::ssPrinterErrLabelJammed:
            return tr( "Label Error" );
        case PrinterErrors::ssPrinterErrLabelMissing:
            return tr( "Label Error" );
        case PrinterErrors::ssPrinterErrOutOfMedia:
            return tr( "Label Error" );
        case PrinterErrors::ssPrinterErrCassetteMissing:
            return tr( "System Error" );
        case PrinterErrors::ssPrinterErrHardwareError:
            return tr( "Printer Error" );
        case PrinterErrors::ssPrinterErrThermalOverload:
            return tr( "System Error" );
        case PrinterErrors::ssPrinterErrThermalShutdown:
            return tr( "System Error" );

        case 7557:
            return tr( "Insert Cassette" );
        case 7558:
        case 7559:
        case 7560:
        case 7561:
        case 7562:
        case 7563:
            return tr( "Printer Error" );

        // a bunch of devices with no errors to report to self service - Wrapper, Albel Appliers

        // Barcode Scanner - 16000s
        case 16000:
           return tr( "Scanner Error" );
        case 16001:  // from app, received a read 1 from ui, and cant find it in the db. cant really happen tho bc we sent the item to the ui initially anyway
           return tr( "System Error" );
        case 16002:  // from ui, cant add&save current scan code bc plu field is < 0 (how can this happen???)
           return tr( "Product Error" );
        case 16003:  // from ui, cant add&save current scan code bc plu field is blank
           return tr( "Product Error" );
        case 16004:  // from ui, cant delete or add&save an item because there is no scan code bc no row selected or item scanned
           return tr( "The GS1 code is currently blank. Please scan a new item" );

        // Session Errors - 100000
        case 100000:
            return tr( "Session Error" );
        case 100010:
            return tr( "Processing" );
        case 100022:
            return tr( "Shutdown" );
        case 100023:
           return tr( "Shutdown" );
        case 100026:
            return tr( "Database Error" );
        case 100027:
            return tr( "Database Error" );
        case 100033:
            return tr( "Database Error" );
        case 100043:
            return tr( "Label Error" );
        case 100044:
            return tr( "Label Error" );
        case 100050:
            return tr( "Database Processing");
        case 100051:
            return tr( "Database Error");
        case 100055:
           return tr("Loading...");
        case 100056:
           return tr("Loading..." );
        case 100057:
           return tr("Search Error" );

        // Product Errors
        case ProductDevError::ProductErr:
        case ProductDevError::ssProductErr:
            return tr( "Product Error" );
        case ProductDevError::ProductErrCannotChangeProduct:
        case ProductDevError::ssProductErrCannotChangeProduct:
            return tr( "Product Error");
        case ProductDevError::ProductErrCannotPrintLabel:
            return tr( "Weigher Error" );
        case ProductDevError::ProductErrReportingClass:
        case ProductDevError::ssProductErrReportingClass:
            return tr( "Product Error" );
        case ProductDevError::ProductErrLabelType:
            return tr( "Label Product Error" );
        case ProductDevError::ProductErrGraphic:
        case ProductDevError::ssProductErrGraphic:
            return tr( "Label Error" );


        // Self Serve Product Errors - 101500s
        case ProductDevError::ssProductErrLabelType:
            return tr( "Label Error" );
        case ProductDevError::ssProductErrCannotPrintLabel:
            return tr( "Weigher Error" );

        case 101013:
            return tr( "Information" );
        case 101014:
            return tr( "Weigher Error" );
        case 101015:
            return tr( "System Error" );
        case 101016:
            return tr( "Label Error" );
        case 101019:
            return tr( "Label Error" );
        case 101022:
            return tr( "Label Information" );
        case ProductDevError::ProductErrPrintBadByCount:
            return tr( "Product Information" );
        case 101027:
            return tr( "Weigher Error" );
        case 101033:
            return tr( "Printer Information" );
        case 101036:
            return tr( "Print" );
        case 101037:
            return tr( "Product Error" );
        case 101042:
            return tr( "Label Error" );
        case 101046:
            return tr( "Product Information" );
        case 101051:
            return tr( "Label Error" );
        case 101066:
            return tr( "Product Error" );
        case 101079:
            return tr( "Product Error" );
        case 101080:
            return tr( "Product Error" );
        case 101086:
            return tr( "Product Error" );
        case 101096:
            return tr( "Print Error" );
        case 101118:
            return tr( "System Error" );
        case 101119:
            return tr( "Sizing" );
        case 101120:
            return tr( "Label Error" );
        case 101121:
            return tr( "Labels Error" );
        case 101122:
            return tr( "Label Error" );
        case 101123:
            return tr( "System Error" );
        case 101124:
            return tr( "System Error" );
        case 101125:
            return tr( "System Error" );
        case 101126:
            return tr( "Printer Error" );
        case 101127:
            return tr( "Printer Error" );
        case 101128:
            return tr( "Label Error" );
        case 101129:
            return tr( "Label Error" );
        case 101132:
            return tr( "Label Information" );
        case 101133:
            return tr( "Label Information" );
        case 101134:
            return tr( "Label Error" );
        case 101135:
            return tr( "Label Error" );
        case 101136:
            return tr( "Label Error" );
        case 101140:
            return tr( "Label Error" );
        case 101141:
            return tr( "System Error" );
        case 101142:
            return tr( "Product Information" );
        case 101170:
            return tr( "Product Information" );
        case 101173:
            return tr( "Product Information" );

        //case 109022:
        //    return tr( "Reboot" );

        // Flashkey Errors - 136000s

        // Network Connectivity
        case 145000:
            return tr( "Unknown Network Connectivity error." );
        case 145001:
            return tr( "Lost network connectivity." );

        default:
            LOG_DEBUG( "Title: Unknown Code", NULL );
            return "";
    }

    return QString();

}

QString selfserveDialogG2::getErrorText()
{
    LOG_CLASS_INFO( "selfserveDialogG2", "getErrorText" );
    if(  code_ == "0" )
        return "";

    switch ( code_.toInt() )
    {
        // Weigher Errors - 1000
        case WeigherDevError::WeigherErrUnknownError:
            return tr( "An unknown weigher error has occurred. Please reboot the scale. If the problem persists please contact Hobart Service." );
        case WeigherDevError::WeigherErrCommLoss:
            return tr( "Check to be sure the weigher cable is plugged in." );
        case WeigherDevError::WeigherErrNegativeGrossWeight:                                                    // 1002
            // only rezero button message
            return tr( "Apologies for the inconvenience. Please ask an associate for assistance." );            // REPLACES: Please clear the platter of all products and debris. Check for obstructions contacting the platter. Contact store personnel to REZERO the scale.
        case WeigherDevError::WeigherErrOverGrossWeightLimit:
            return tr( "You'll need to remove some items to continue." );          //
        case WeigherDevError::WeigherErrScaleWarmup:
            return tr( "The weigher has been detected and is now going through its warmup procedure, please wait." );
        case WeigherDevError::WeigherErrRezeroing:
            return tr( "Rezero in progress.\n\nPlease wait...." );
        case WeigherDevError::WeigherErrZeroOutOfMaintZone:
            return tr( "Outside zero maintenance zone. Please contact store personnel. If problem persists please contact Hobart Service." );
        case WeigherDevError::WeigherErrZeroOutOfMaintZoneStatus:
            return tr( "Outside zero maintenance zone. Please contact store personnel. If problem persists please contact Hobart Service." );
        case WeigherDevError::WeigherErrRemoveWeight:
            return tr( "Remove the weight from platter. Check the platter for any obstructions." );
        case WeigherDevError::WeigherErrSelectWeightsType:
            return tr( "Select lb or kg weights you will be using to calibrate the weigher." );
        case WeigherDevError::WeigherErrPressCalibrateSwitch:
            return tr( "Press the calibrate switch on the side of the scale." );
        case WeigherDevError::WeigherErrDeltaChangeTooBig:
            return tr( "Delta change is too big." );
        case WeigherDevError::WeigherErrCalibrationSuccessful:
            return tr( "Weigher calibration succeeded." );
        case WeigherDevError::WeigherErrNotAttached:
            return tr( "Weigher not attached." );
        case WeigherDevError::WeigherErrScaleNotZero:
            return tr( "Remove the weight from the platter or press the 'REZERO' button." );
        case WeigherDevError::WeigherErrInvalidAddress:
            return tr( "Invalid station address.\nResetting weigher." );
        case WeigherDevError::WeigherErrManagerDeleted:
            // Not Used:
            return tr( "Weigher dropped offline. Check if the weigher cable is plugged in." );
        case WeigherDevError::WeigherErrTareDisabled:
            return tr( "Remove all weight from the platter and enter a value for the tare." );
        case WeigherDevError::WeigherErrConfigOutOfRange:
            return tr( "Contact Hobart Service to troubleshoot the scale." );
        case WeigherDevError::WeigherErrCantRezeroNegativeGrossWeight:
            return tr( "Please contact store personnel immediately for assistance." );
        case WeigherDevError::WeigherErrRemoteConfigurationStarted:
            return tr( "Weigher is being updated remotely.  Please wait ..." );
        case WeigherDevError::WeigherErrOutOfCalibrateTiltRange:
            return tr( "To level the scale please go to the Menu > Service > Weigher Information Screen and adjust the X and Y tilt under 1/2 degree." );
        case WeigherDevError::WeigherErrValMaxEepCommFailure:
            return tr( "Please remove all weight from the platter and press the 'REBOOT' button or Click 'CANCEL'." );
        case WeigherDevError::WeigherErrValMaxEepChecksumFailure:
            return tr( "Please contact Hobart Service for assistance." );
        case WeigherDevError::WeigherErrValMaxEepAcclCommFailure:
            return tr( "Please remove all weight from the platter and press the 'REBOOT' button or Click 'CANCEL'." );
        case WeigherDevError::WeigherErrValMaxTiltReached:
            return tr( "Please level your scale or contact Hobart Service." );
        case WeigherDevError::WeigherErrUnderMinimumWeight:
            return tr( "Please add more items to generate\na QR code." );                       // > 0 but < min wt to print

        // Weigher Self Service versions of the same messages. Customer friendly text.
        case WeigherDevError:: ssWeigherErrCommLoss:
            return tr( "The scale is out of service.  Please contact store personnel." );
        case WeigherDevError::ssWeigherErrNegativeGrossWeight:
            return tr( "Apologies for the inconvenience. Please ask an associate for assistance." );           // REPLACES: Please clear the platter of all products and debris. Check for obstructions contacting the platter. Contact store personnel to REZERO the scale.
        case WeigherDevError::ssWeigherErrOverGrossWeightLimit:
            return tr( "You'll need to remove some items to continue" );
        case WeigherDevError::ssWeigherErrZeroOutOfMaintZoneStatus:
            return tr( "Outside zero maintenance zone. Please contact store personnel. If problem persists please contact Hobart Service." );
        case WeigherDevError::ssWeigherErrDeltaChangeTooBig:
            // I think this number is wrong, so I repeated it correctly at 1513: 1522 is actually a Service > Calibration message
            return tr( "The scale is out of service.  Please contact store personnel." );
        case WeigherDevError::ssWeigherErrNotAttached:
            // I think this is the right enum of what was being displayed as the error for 1511
            return tr( "The scale is out of service.  Please contact store personnel." );
        case WeigherDevError::ssWeigherErrCantRezeroNegativeGrossWeight:
            return tr( "The scale is out of service.  Please contact store personnel." );
        case WeigherDevError::ssWeigherErrUnderMinimumWeight:
            // > 0 but < min wt to print
            return tr( "Please add more items to generate\na QR code." );

        // No 2000s or 3000s

        // Operate Mode Errors - 4000s
        case 4101:
            return tr( "Select a product or category" );
        case 4102:
            return tr( "Select a product or category" );
        case 4103:
            return tr( "Please press print key" );
        case 4104:
            return tr( "Please place your item on the scale" );
        case 4105:
            return tr( "Select a product or category" );

        // No 4000s, 5000s, or 6000s

        // Printer Errors
        case PrinterErrors::PrinterErrCommLoss:
            return tr( "Printer communication lost. Check printer cable." );
        case PrinterErrors::PrinterErrManagerDeleted:
            return tr( "Printer turned off." );
        case PrinterErrors::PrinterErrHeadUp:
            return tr( "Insert label cassette to continue printing.\n\nPlease always use the original printer cassette that was purchased with the scale.\n\nPlease remember to clean the printhead." );
        case PrinterErrors::PrinterErrLabelSizing:
            return tr( "Printer label sizing." );
        case PrinterErrors::PrinterErrTakeLabel:
            return tr( "Take the label and place it on the package." );
        case PrinterErrors::PrinterErrTakeLabelBeep:
            return tr( "Take the label and place it on the package." );
        case PrinterErrors::PrinterErrLabelJammed:
            return tr( "Check labels and cycle printhead lever." );
        case PrinterErrors::PrinterErrLabelMissing:
            return tr( "The label is missing.\nPlease attempt to print the item again." );
        case PrinterErrors::PrinterErrOutOfMedia:
            return tr( "Out of labels." );
        case PrinterErrors::PrinterErrCassetteMissing:
            return tr( "Label cassette missing." );
        case PrinterErrors::PrinterErrHardwareError:
            return tr( "Printer hardware error." );
        case PrinterErrors::PrinterErrThermalOverload:
            return tr( "Thermal overload print error." );
        case PrinterErrors::PrinterErrThermalShutdown:
            return tr( "Printer shutdown due to thermal overload." );
        case PrinterErrors::PrinterErrInvalidTypeSetup:
            return tr( "Invalid setup. Resetting." );
        case PrinterErrors::PrinterErrMSDisengage:
            return tr( "Calibrate media sensor\n\nDisengage the printhead." );
        case PrinterErrors::PrinterErrMSSituateLabelStock:
            return tr( "Calibrate media sensor\n\nPosition the label stock so there is only backing paper without a label on it and no black bar covering the media sensor." );
        case PrinterErrors::PrinterErrMSEngage:
            return tr( "Calibrate media sensor\n\nEngage the printhead." );
        case PrinterErrors::PrinterErrMSLabelStockTaut:
            return tr( "Calibrate media sensor\n\nTurn the supply roll clockwise until the backing paper under the media sensor is taut. Hold in this position until calibration is complete." );
        case PrinterErrors::PrinterErrMSCalibrating:
            return tr( "Calibrate media sensor\n\nCalibrating." );
        case PrinterErrors::PrinterErrMSComplete:
            return tr( "Calibration complete.\n\nPrinter will size the label stock." );
        case PrinterErrors::PrinterErrLTClearSensor:
            return tr( "Calibrate label taken sensor\n\nMake sure that the label taken sensor is clear from all items." );
        case PrinterErrors::PrinterErrLTCalibrating:
            return tr( "Calibrate label taken sensor\n\nCalibrating." );
        case PrinterErrors::PrinterErrLTComplete:
            return tr( "Calibrate label taken sensor\n\nCalibration is complete." );
        case PrinterErrors::PrinterErrSLAIsOff:
            return tr( "\nThe Security label applier is off." );
        case PrinterErrors::PrinterErrSLAOutOfLabels:
            return tr( "\nThe Security label applier is out of labels." );
        case PrinterErrors::PrinterErrHeadUpForEpp:
            return tr( "Printer lever forward.\n\nMove printer lever back\nto engage the printhead." );
        case PrinterErrors::PrinterErrNoSpaceAllowedInName:
            return tr( "Invalid character in printer name.\nThe printer name may not contain a space in the name." );
        case PrinterErrors::PrinterErrInvalidCharInName:
            return tr( "Invalid character in printer name. The following characters are invalid: space comma \\n @ ` ~ ! # $ ^ & * ( ) = + [ ] { } \\ | ; : \\ , < > / ?" );
        case PrinterErrors::PrinterErrNoSizingDueToContinuous:
            return tr( "\nContinuous label stock\ndoes not require sizing." );
        case PrinterErrors::PrinterErrSizedContinuousOK:
            return tr( "\nContinuous label stock detected.\nIf this is correct, press OK.\n\nIf not correct, press CANCEL, then remove and reinsert the label cassette." );
        case PrinterErrors::PrinterErrLabelCutterJammed:
            return tr( "\nLabel Cutter Jammed.\n\nCheck the cutter for jammed or bound label stock." );
        case PrinterErrors::PrinterErrLabelCutterInterlockOpen:
            return tr( "\nLabel Cutter Interlock Open.\n\nClose panel on the front of the cutter.\n\nSnap the Cutter back in place when finished." );
        case PrinterErrors::PritnerErrLabelCutterTimedOut:
            return tr( "\nLabel Cutter Timeout.\n\nThe command to engage the label cutter timed out." );
        case PrinterErrors::PrinterErrProductionEntryInterlockOpen:
            return tr( "Print job has been canceled - Interlock is open.\n\nNumber of labels to print: %s\nNumber of labels printed: %s\nNumber of labels remaining: %s" );
        case PrinterErrors::PrinterErrLowStock:
            return tr( "Warning! Label stock is running out." );
        case PrinterErrors::PrinterErrOutOfStock:
            return tr( "Out of Label stock.\n\nThis scale is out of service.\n\nPlease contact store personnel." );
        case PrinterErrors::PrinterErrFSSLabelCutterInterlockOpen:
            return tr( "\nLabel Cutter Interlock Open.\n\nClose side door when finished.\n\nCutter cannot operate while door is open." ); //SOF-5065
        case PrinterErrors::PrinterErrFSSLabelDoorInterlockOpen:
            return tr( "\nPrinter Door Interlock Open.\n\nClose side door when finished." );

        // Printer Self Service Errors - 7500s
        case PrinterErrors::ssPrinterErrCommLoss:
            return tr( "Printer communication lost. Check printer cable.\nThis scale is out of service.  Please contact store personnel." );
        case PrinterErrors::ssPrinterErrManagerDeleted:
            return tr( "Printer turned off.\nThis scale is out of service.  Please contact store personnel." );
        case PrinterErrors::ssPrinterErrHeadUp:
            return tr( "Printhead up - This scale is out of service.\nThe printhead must be engaged to continue printing.\nPlease contact store personnel." );
        case PrinterErrors::ssPrinterErrLabelSizing:
            return tr( "Printer label sizing." );
        case PrinterErrors::ssPrinterErrTakeLabel:
            return tr( "Take the label and place it on the package." );
        case PrinterErrors::ssPrinterErrTakeLabelBeep:
            return tr( "Take the label and place it on the package." );
        case PrinterErrors::ssPrinterErrLabelJammed:
            return tr( "Label jammed.\nThis scale is out of service.  Please contact store personnel." );
        case PrinterErrors::ssPrinterErrLabelMissing:
            return tr( "The label is missing.\nPlease attempt to print the item again. \nContact store personnel if the problem continues." );
        case PrinterErrors::ssPrinterErrOutOfMedia:
            return tr( "Out of labels.\nThis scale is out of service.  Please contact store personnel." );
        case PrinterErrors::ssPrinterErrCassetteMissing:
            return tr( "Label cassette missing.\nThis scale is out of service.  Please contact store personnel." );
        case PrinterErrors::ssPrinterErrHardwareError:
            return tr( "Printer hardware error.\nThis scale is out of service.  Please contact store personnel." );
        case PrinterErrors::ssPrinterErrThermalOverload:
            return tr( "Thermal overload print error.\nThis scale is out of service.  Please contact store personnel." );
        case PrinterErrors::ssPrinterErrThermalShutdown:
            return tr( "Printer shutdown due to thermal overload.\nThis scale is out of service.  Please contact store personnel." );
        case 7557:
            return tr( "Label \ncassette open.\n\nInsert \n\nlabel cassette to \ncontinue \n\nprinting.\nThis scale \n\nis \nout of \nservice.  Please contact store personnel.");
        case 7558:
            return tr( "Two external EPP printers cannot be selected at the same time." );
        case 7559:
            return tr( "A network printer is not attached to print the document." );
        case 7560:
            return tr( "The command to print the web page failed." );
        case 7561:
            return tr( "No file loaded to print." );
        case 7562:
            return tr( "The selected file was sent to the printer." );
        case 7563:
            return tr( "A invalid network printer was selected to print the document." );

        // a bunch of devices with no errors to report to self service - Wrapper, Albel Appliers

        // Barcode Scanner
        case 16000:
           return tr( "Unknown Barcode Scanner error." );
        case 16001:  // from app, received a read 1 from ui, and cant find it in the db. cant really happen tho bc we sent the item to the ui initially anyway
           return tr( "Record not found in database." );
        case 16002:  // from ui, cant add&save current scan code bc plu field is < 0 (how can this happen???)
           return tr( "Invalid product number." );
        case 16003:  // from ui, cant add&save current scan code bc plu field is blank
           return tr( "Please enter a Product Number." );
        case 16004:  // from ui, cant delete or add&save an item because there is no scan code bc no row selected or item scanned
           return tr( "The GS1 code is currently blank. Please scan a new item" );
        case 16007:  // from ui, trying to add a scan code and plu that already exist in the table
           return tr( "Cannot add the following item.\nThe selected scan code and product already exists" );
        case 16008:  // from application after looking up a scan code and there isnt one in the database. The PLU still like;y exists by itself though
           return tr( "The scanned item does not exist in the database.\nPlease enter the product number manually." );
        case 16009:  // from application after looking up a scan code and there isnt one in the database. The PLU still like;y exists by itself though
           return tr( "The scanned item already exists in the database." );
        case 16010:  // from application after clicking on a row but the plu in the row doesnt exist
           return tr( "The selected barcode has a product number currently not in the database." );


        // Session Errors - 100000
        case 100000:
            return tr( "Unknown Session error." );
        case 100010:
            return tr( "Working...\nPlease wait while the scale changes language." );
        case 100022:
            return tr( "The scale is shutting down.\n\nPlease wait ..." );
        case 100023:
           return tr( "The scale application is ready to shutdown.\n\nPress the Shut Down button to power down the scale or Reboot to restart." );
        case 100026:
            return tr( "Please wait.\nDatabase closing temporarily ..." );
        case 100027:
            return tr( "The database was corrupt and has been replaced with an empty database.  The corrupted directory has been renamed.\n\nPlease load your database again." );
        case 100033:
            return tr( "The database was corrupt and has been replaced with a previous backup.  Verify current pricing before labeling product." );
        case 100043:
            return tr( "\nInstalled Label Stock is Not Defined." );
        case 100044:
            return tr( "\n No Label Type available for the installed Label Stock." );
        case 100050:
            return tr( "Please wait.\nBacking up the database ...");
        case 100051:
            return tr( "The database is corrupt!  The scale will now restart to handle this issue.");
        case 100055:
           return tr("Begin Self Service");
        case 100056:
           return tr("Self Service initialization and loading sequence.\n\nPlease wait ...." );
        case 100057:
           return tr("The Search feature has been intentionally disabled." );

        // Product Errors
        case ProductDevError::ProductErr:
        case ProductDevError::ssProductErr:
            return tr( "Item Code not found." );
        case ProductDevError::ProductErrCannotChangeProduct:
        case ProductDevError::ssProductErrCannotChangeProduct:
            return tr( "Cannot change the product record at this time.");
        case ProductDevError::ProductErrCannotPrintLabel:
            return tr( "Cannot print the label because of a weighing error.\n\nThe only available option is to scan a label." );
        case ProductDevError::ProductErrReportingClass:
        case ProductDevError::ssProductErrReportingClass:
            return tr( "Invalid reporting class for this product." );
        case ProductDevError::ProductErrLabelType:
            return tr( "Invalid label type for this product.\nPlease contact store personnel." );
        case ProductDevError::ProductErrGraphic:
        case ProductDevError::ssProductErrGraphic:
            return tr( "Invalid or missing graphic for this product." );


        // Self Serve Product Errors - 101500s
        case ProductDevError::ssProductErrLabelType:
            return tr( "Invalid label type for this product.\nPlease contact store personnel." );
        case ProductDevError::ssProductErrCannotPrintLabel:
            return tr( "Cannot print the label because of a weighing error.\n\nThe only available option is to scan a label." );


        case ProductDevError::ProductErrPrintNoWeight:
            return tr( "Please place your item on the scale and then press the print key." );
        case ProductDevError::ProductErrPrintBelowMinWeight:
            return tr( "The item on the scale does not weigh enough to print." );
        case ProductDevError::ProductErrScaleNotPresent:
            return tr( "This product cannot be processed when a scale is not present in the system." );
        case ProductDevError::ProductErrPriLabelNotFound:
            return tr( "Primary label type does not exist." );
        case ProductDevError::ProductErrPriLabelWontFit:
            return tr( "Primary label type will not fit on the label stock.\n\nThe only available option is to scan a label." );
        case ProductDevError::ProductErrPriLabelTypeChanged:
            return tr( "Primary label type was changed so it will fit on the label stock." );
        case ProductDevError::ProductErrPrintBadByCount:
            return tr( "You must enter a valid quantity to continue." );
        case ProductDevError::ProductErrInvalidWeightToPrint:
            return tr( "A label cannot be printed.\nThe weigher has not correctly weighed the item." );
        case ProductDevError::ProductErrPriLabelNotSized:
            return tr( "The Primary printer needs to size the label.\n\nPlease size the Primary Printer." );
        case ProductDevError::ProductErrPrintingLabel:
            return tr( "Printing label." );
        case ProductDevError::ProductErrInvalidInSelfServeMode:
            return tr( "You may not access a fixed weight or fluid ounce product on this scale." );
        case ProductDevError::ProductErrNoLabelTypesSelected:
            return tr( "No label types have been selected." );
        case ProductDevError::ProductErrForcedFieldNotEntered:
            return tr( "You must first enter the quantity on the keypad to the right and then press Print Label" );
        case ProductDevError::ProductErrLabelOnlyForRandomWeight:
            return tr( "The selected label type may only be used with a Random weight product.\n\nThe only available option is to scan a label." );
        case ProductDevError::ProductErrTotalPriceOutOfRange:
            return tr( "The total price is invalid." );
        case ProductDevError::ProductErrForcedFieldEntryInvalidInSelfService:
            return tr( "This product cannot be accessed on a self-service scale.  Please contact store personnel for assistance." );
        case ProductDevError::ProductErrTareGreaterThanMaxTare:
            return tr( "The tare on the platter exceeds the maximum tare." );
        case ProductDevError::ProductErrProductNotValidInThisMode:
            return tr( "This item is not available in self-service mode." );
        case ProductDevError::ProductErrCannotPrintInSelfServiceMode:
            return tr( "Cannot print remotely to a scale in Self Service Mode." );
        case ProductDevError::ProductErrPrintHeadDisengaged:
            return tr( "Printhead up - This scale is out of service.\nThe printhead must be engaged to continue printing.\nPlease contact store personnel." );
        case ProductDevError::ProductErrPrintLabelSizing:
            return tr( "Printer label sizing." );
        case ProductDevError::ProductErrPrintMediaJammed:
            return tr( "Label jammed.\n\nThe only available option is to scan a label." );
        case ProductDevError::ProductErrPrintOutOfMedia:
            return tr( "Out of labels." );
        case ProductDevError::ProductErrPrintCassetteMissing:
            return tr( "Label cassette missing." );
        case ProductDevError::ProductErrPrintThermalOverload:
            return tr( "Printer shutdown due to thermal overload." );
        case ProductDevError::ProductErrPrintThermalShutdown:
            return tr( "Printer shutdown due to thermal shutdown." );
        case ProductDevError::ProductErrPrintHardwareError:
            return tr( "Printer shutdown due to printer hardware error." );
        case ProductDevError::ProductErrPrintMediaShutdown:
            return tr( "Printer error due to media shutdown." );
        case ProductDevError::ProductErrPrintManagerDeleted:
            return tr( "Cannot print label due to Printer Manager being deleted." );
        case ProductDevError::ProductErrPrintLabelTakenError:
            return tr( "Label Taken Error." );
        case ProductDevError::ProductErrPrintTakeLabelError:
            return tr( "Take Label Error." );
        case ProductDevError::ProductErrPrintScaleNotLicensed:
            return tr( "Printing on the default label because the overridden label cannot be used." );
        case ProductDevError::ProductErrPrintPrintOnCorrectedLabel:
            return tr( "Printing on corrected default label because the overridden label cannot be used." );
        case ProductDevError::ProductErrPrintInvalidDefaultAndOverride:
            return tr( "Invalid default and overridden label type." );
        case ProductDevError::ProductErrPrintLabelDoesNotFit:
            return tr( "Cannot print on the default or overridden label type because the label cannot fit." );
        case ProductDevError::ProductErrPrintUnknownLabelError:
            return tr( "Unknown label error. Cannot not print on the default or overridden label type" );
        case ProductDevError::ProductErrPrintLabelMissing:
            return tr( "Label missing. Please print the item again and check if the scale is out of paper." );
        case ProductDevError::ProductErrPrintPeripheralComm:
            return tr( "Cannot print. Peripheral Communication Error." );
        case ProductDevError::ProductErrPrintRandomWeightPluReturned:
            return tr( "A Random Weight PLU was sent to the scale. Please add a weight and press the Print button on the scale to receive a label." );
        case ProductDevError::ProductErrInvalidBarcodePrefix:
            return tr( "The UPC number must be a value greater than 0 if making UPC barcodes." );
        case ProductDevError::ProductErrDoNotPrintEmptyNutrifactsLabel:
            return tr( "The assigned nutrifacts label will not be printed because there is no data to fill it." );

        //case 109022:
        //    return tr( "Self Service Configuration has been modified.\n\nPLEASE REBOOT" );

        // Flashkey Errors - 136000s

            // Network Connectivity
        case 145000:
            return tr( "Unknown Network Connectivity error." );
        case 145001:
            return tr( "Please check to see if ethernet cable is plugged in properly.\nIf using wireless make sure you can connect to access point." );

        default:
            LOG_DEBUG( "Title: Unknown Error Code: %s", qPrintable( code_ ) );
            return "";
    }

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::paintEvent(QPaintEvent *pe)
{
    setStyleSheet("QWidget{ border: 1px solid black; }");        // made background transparent

    _Mybase::paintEvent( pe );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::resizeEvent( QResizeEvent *e )
{
     update();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::showEvent( QShowEvent *e )
{
    refreshText();

    //translate();

    update();

    _Mybase::showEvent( e );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::onOKExit()
{
    config_->internalConfig.selfServicePreventClicks = false;

    // this would close dialogs instantly instead of waiting for reply
    // bring back to see if it makes them close faster.  See if it causes any issues

    emit dialogDone();
    closeDialog("1");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::onCancelExit()
{
    config_->internalConfig.selfServicePreventClicks = false;

    // this would close dialogs instantly instead of waiting for reply
    // bring back to see if it makes them close faster.  See if it causes any issues

    emit dialogDone();
    closeDialog("0");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::onRezeroExit()
{
    //setVisible(false);
    ui->butOK->setVisible( false );
    ui->butRezero->setVisible( false );
    ui->butCancel->setVisible( false );
    hide();

    config_->internalConfig.selfServicePreventClicks = false;

    // this would close dialogs instantly instead of waiting for reply
    // bring back to see if it makes them close faster.  See if it causes any issues
    emit dialogDone();
    closeDialog("1");
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveDialogG2::addButtons( int buttonstyle )
{
    LOG_CLASS_INFO( "selfserveDialogG2", "addButtons" );

    // almost all dialogs triggered on the UI side are style 5.  The few that are not do not apply to self serv

    int testcode = code_.toInt();
    if(  ( testcode == PrinterErrors::PrinterErrOutOfStock ) || ( testcode == PrinterErrors::ssPrinterErrOutOfMedia )  )
        buttonstyle = 0;

    switch( buttonstyle )
    {
        case 0:
        case 3:
        case 9:
        {
            // hide all buttons
            ok_->setVisible( false );
            cancel_->setVisible( false );
            rezero_->setVisible( false );
            ui->butClose->setVisible( false );
            QString test =  sessionHooks::instance()->getFieldValue( sessionHooks::selfServiceRestrictedDialog );
            sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceRestrictedDialog, "1");
            LOG_DEBUG( "Open dialog set selfServiceRestrictedDialog = 1 as (%s)", qPrintable( test ) );
            break;
        }

        case 1:
        case 2:
            // display OK and Cancel
            ok_->move( 50, 440 );
            cancel_->move( 494, 440 );
            ok_->setVisible( true );
            cancel_->setVisible( true );
            ui->butClose->setVisible( false );
            break;

        case 5:
        case 7:
            // display OK button
            ok_->move( 272, 440 );
            ok_->setVisible( true );
            cancel_->setVisible( false );
            rezero_->setVisible( false );
            ui->butClose->setVisible( false );
            break;

        case 6:
        case 8:
            // display Cancel button
            cancel_->move( 272, 440 );
            cancel_->setVisible( true );
            break;

        case 11:
            // Display Rezero and Ignore buttons (see if this is really needed - when does it pop?)
            rezero_->move( 50,440 );
            cancel_->move( 494, 440 );
            cancel_->setText( "Ignore");
            rezero_->setVisible( true );
            cancel_->setVisible( true );

            ok_->setVisible( false );
            ui->butClose->setVisible( false );

            break;

        case 17:    // Rezero
            rezero_->move( 272,440 );
            rezero_->setVisible( true );

            ok_->setVisible( false );
            cancel_->setVisible( false );
            ui->butClose->setVisible( false );

            break;

        case 4:     // handled elsewhere - should never receive
        case 12:    // Ignore and Shutdown - do not need in SelfServ
        case 13:    // Recalibrate -
        case 14:    // Update and Print button and a Cancel button (when is this used?)
        case 15:    // Update Only button and a Cancel button (when is this used?)
        case 16:    // case 14 and 15 together (ditto?)
        case 18:    // Reboot and Cancel button - do not need in self serve
            break;

        default:
            break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool selfserveDialogG2::invalidErrorCode( int code )
{
    bool isInvalid = false;
    // if the code is any one of these, then it is thrown out and not popped up to the use
    switch( code )
    {
        case 109022:
            isInvalid = true;
    }

    return isInvalid;
}

} // hobart namespace
