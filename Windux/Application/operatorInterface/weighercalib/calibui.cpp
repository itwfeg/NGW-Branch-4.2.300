/**
 * @file calibui.cpp
 *
 * @copyright Copyright (c) 2018, ITW Hobart Food Equipment Group, U.S.A.
 * @author Job E. Crew
 */

#include "calibui.h"
#include "ui_calibui.h"

#include <QFile>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include "common.h"

static const int MAX_STEPS = 24;
static const int MAX_READINGS = 12;

static const bool TESTING_VERIFY_SCANNED = false;        // pops scanned proof of concept dialog
static const bool DISPLAY_GENERATE_FIT_RESULTS = false;
static const bool OUTPUT_SAVED_READ_FIT_PARAMS = false;
static const bool DISPLAY_SAVED_READ_FIT_PARAMS = false;
static const bool DISPLAY_X_Y_Z_COUNTS = false;
static const bool DISPLAY_WRITTEN_DATE = false;

//These constants pop up the "Incorrect Loadcell orientation" error message
static const int X_POSITIVE_TILTED_MAX_COUNTS = 35000;
static const int X_POSITIVE_TILTED_MIN_COUNTS = 20000;

static const int X_LEVEL_MAX_COUNTS = 15000;
static const int X_LEVEL_MIN_COUNTS = -5000;

static const int X_NEGATIVE_TILTED_MAX_COUNTS = -10000;
static const int X_NEGATIVE_TILTED_MIN_COUNTS = -20000;

static const int Z_POSITIVE_TILTED_MAX_COUNTS = 20000;
static const int Z_POSITIVE_TILTED_MIN_COUNTS = 10000;

static const int Z_LEVEL_MAX_COUNTS = 5000;
static const int Z_LEVEL_MIN_COUNTS = -15000;

static const int Z_NEGATIVE_TILTED_MAX_COUNTS = -25000;
static const int Z_NEGATIVE_TILTED_MIN_COUNTS = -40000;

///////////////////////////////////////////////////////////////////////////////////////////////////

calibui::calibui(kpc *device, QWidget *parent) : QWidget(parent),
      ui(new Ui::calibui),
      step_( SetupStep ),
      passVerification_( false ),
      timeout_error_( false ),
      verifyReadings_(false),
      receivedFitParams_( false ),
      units_( " "),
      isHillsboroGravity_( true ),
      grav_const_( 0 ),
      serialFrames_( 0 ),
      assemblyFrames_( 0 )
{
    ui->setupUi(this);

    device_ = device;

    if( device_ )
    {
        device_->weigherWakeup();                                   // REQ: 0x401 - Pulled from main.cpp
        device_->setWeigherMode( SCALE_MANUAL_MODE );               // REQ: 0x451,3 - manual
        device_->weigherEnable();                                   // REQ: 0x454
        //device_->accelerometerStatusReq();                          // REQ: 0x402 lets get some data back - some counts ( DO WE NEED THIS?)
        //device_->weigherStatusReq();                                // REQ: 0x402 original counts ( DO WE NEED THIS?) this has been replaced with 0x452 request for acceleromter
    }

	initialize();
    createLayout();                             // delete later if end up not using.
    
    translate();                                // not really important since this is at The Hills

    int pStep = step_;
    setNextCalibrationStep(pStep);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
calibui::~calibui()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::translate()
{
    ui->retranslateUi(this);
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::resizeEvent( QResizeEvent *e )
{
    int ui_width = this->width();
    int but_width = ui_width * .96;
    nextPhoto_->setFixedWidth( but_width );

    int pStep = step_;
    setNextCalibrationStep(pStep);                                  // refresh current photo

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::refresh()
{
    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::createLayout()
{
    QDesktopWidget *desktop;
    QSize size( 1366, 768 );

    if ( (desktop = QApplication::desktop()) )
        size = desktop->screenGeometry( this ).size();

    if( device_ )
    {
        this->setFixedSize( size );         // dont want the screen fullsize when testing
        this->showFullScreen();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::initialize()
{
    grav_const_ = 980023;                  // defaults to Hillsboro
    isHillsboroGravity_ = true;

    if ( QFile::exists( "/etc/hobart/gravity_troy.dat" ) )
    {
        grav_const_ = 980104;               // Troy
        isHillsboroGravity_ = false;
    }

    //outfile_( "debuglog.dat");

    showPhoto_    = ui->lblShowHowTo;
    explainPhoto_ = ui->lblTellHowTo;
    nextPhoto_    = ui->butNextHowTo;

    photoButtonMsg_.push_back( tr( "START TEST") );
    photoLabelMsg_.push_back( tr( "Use static protection when calibrating the load cell.\nSelect the proper weights and begin Accelerometer calibration.") );

    ui->loadCellData->setVisible(false);
    // clear out existing photos - shouldnt be any
    stepPhotos_.clear();
    QString stepPhotoName;
    QPixmap stepPhoto;

    for (int j = 0; j <= MAX_STEPS; j++ )
    {
        stepPhotoName = ":/steps/step" + QString::number( j ) + ".png";
        stepPhoto = stepPhotoName;              // step10.png is added, but never used. Swapped for pass_fail
        stepPhotos_.append( stepPhoto );
    }

    connect( ui->butUsePounds,      SIGNAL(clicked()), SLOT(onClickedLbs()) );
    connect( ui->butUseKilograms,   SIGNAL(clicked()), SLOT(onClickedKgs()) );
    connect( nextPhoto_,            SIGNAL(clicked()), SLOT(onClicked()) );
#ifndef QT_DEBUG
    connect( device_, SIGNAL( updateWeigherStateChanged(weigherState, float) ),       SLOT( onUpdateWeigherStateChanged(weigherState, float) ) );
    connect( device_, SIGNAL( updateValueMaxWeigherStatus(uint16_t, uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int16_t, int16_t) ), SLOT( onUpdateValueMaxWeigherStatus(uint16_t, uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int16_t, int16_t) ) );
    connect( device_, SIGNAL( updateValueMaxFitParameters(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) ),           SLOT( onUpdateValueMaxFitParameters(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) ) );       // this might have to be 25 individual args, or just some of them
    connect( device_, SIGNAL( updateValueMaxDate(uint32_t) ),                   SLOT( onUpdateValueMaxDate(uint32_t) ) );
    connect( device_, SIGNAL( updateValueMaxAssemblyNumber(QString) ),          SLOT( onUpdateValueMaxAssemblyNumber(QString) ) );
    connect( device_, SIGNAL( updateValueMaxHWVersion(QString) ),               SLOT( onUpdateValueMaxHWVersion(QString) ) );
    connect( device_, SIGNAL( updateValueMaxSerialNumber(QString) ),            SLOT( onUpdateValueMaxSerialNumber(QString) ) );
#endif

    connect(&verifyStepIsVerified_, SIGNAL( timeout() ),                        SLOT( moveOnWithoutVerification() ) );
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::updateLoadCellData(int32_t acc_x, int32_t acc_y, int32_t acc_z, uint32_t counts){
    ui->lbl_x->setText("X: " + QString::number(acc_x));
    ui->lbl_y->setText("Y: " + QString::number(acc_y));
    ui->lbl_z->setText("Z: " + QString::number(acc_z));
    ui->lbl_counts->setText("Load Cell: " + QString::number(counts));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::initializeText()
{
    photoButtonMsg_.clear();                                                                                // clear
    photoButtonMsg_.push_back( tr( "START TEST") );                                                         // 1
    photoButtonMsg_.push_back( tr( "All fields are scanned.") );                                            // 2
    photoButtonMsg_.push_back( tr( "The Load Cell is clamped in place.") );                                 // 3 NEW STEP
    photoButtonMsg_.push_back( tr( "Spider is attached") );                                               // 4 +1
    photoButtonMsg_.push_back( tr( "Spider is attached") );                                               // 5 NEW STEP

    photoButtonMsg_.push_back( tr( "Load Cell is connected") );
    photoButtonMsg_.push_back( tr( "Acceleromter is connected") );
    photoButtonMsg_.push_back( tr( "Platter is installed") );
    photoButtonMsg_.push_back( tr( "Black Box is powered on. BEGIN TEST") );

    //photoButtonMsg_.push_back( tr( "Load Cell is locked in place") );
    photoButtonMsg_.push_back( tr( "Take Horizontal Reading") );
    photoButtonMsg_.push_back( tr( "Take Flat %1 %2 Reading").arg(unitsSize_[3], units_) );                 // +2
    photoButtonMsg_.push_back( tr( "Take Front Tilt Reading") );
    photoButtonMsg_.push_back( tr( "Take Front Tilt %1 %2 Reading.").arg(unitsSize_[1], units_) );          // +3
    photoButtonMsg_.push_back( tr( "Take Front Tilt %1 %2 Reading.").arg(unitsSize_[3], units_) );          // +4
    photoButtonMsg_.push_back( tr( "Take Right Tilt Reading") );
    photoButtonMsg_.push_back( tr( "Take Right Tilt %1 %2 Reading").arg(unitsSize_[3], units_) );           // +5
    photoButtonMsg_.push_back( tr( "Take Back Tilt Reading") );
    photoButtonMsg_.push_back( tr( "Take Back Tilt %1 %2 Reading").arg(unitsSize_[1], units_) );            // +6
    photoButtonMsg_.push_back( tr( "Take Back Tilt %1 %2 Reading").arg(unitsSize_[3], units_) );            // +7
    photoButtonMsg_.push_back( tr( "Take Left Tilt Reading") );
    photoButtonMsg_.push_back( tr( "Take Left Tilt %1 %2 Reading").arg(unitsSize_[3], units_) );            // +8
    photoButtonMsg_.push_back( tr( "Calculate and Write Calibration") );
    photoButtonMsg_.push_back( tr( "Verify Calibration") );
    photoButtonMsg_.push_back( tr( "FINISH TEST") );

    photoLabelMsg_.clear();                                                                                                                             // clear
    photoLabelMsg_.push_back( tr( "Press the START TEST button to begin Accelerometer Calibration") );                                                  // 1
    photoLabelMsg_.push_back( tr( "Scan all barcode fields into their proper location.\nClick All fields are scanned when entered.") );                 // 2
    photoLabelMsg_.push_back( tr( "Clamp the load cell into the Fixture Plate until handle clicks.\nClick Load Cell is clamped in place.") );           // 3 NEW STEP
    photoLabelMsg_.push_back( tr( "Attach the spider to the load cell. Do not tighten over 78 in lb.\nClick Spider is attached when connected.") );     // 4
    photoLabelMsg_.push_back( tr( "Another view of attaching the spider. Do not tighten over 78 in lb.\nClick Spider is attached when connected.") );   // 5 NEW STEP                                                                                              // 5 NEW STEP

    photoLabelMsg_.push_back( tr( "Connect the Load Cell cable. Make sure the cable is not touching Spider Assembly\nClick Load Cell is connected.") );
    photoLabelMsg_.push_back( tr( "Connect Accelerometer. Make sure the cable is routed behind Load Cell not touching Spider.\nClick Accelerometer is connected.") );
    photoLabelMsg_.push_back( tr( "Install the weight platter on the Spider Assembly.\nClick platter is installed.") );
    photoLabelMsg_.push_back( tr( "Power on the Black Box and wait for power on indicator. (what is that?)\nClick Black Box is powered on. BEGIN TEST.") );

    photoLabelMsg_.push_back( tr( "Place the Load Cell Fixture Plate flat on the table.\nClick Take Flat Horizontal Reading.") );
    photoLabelMsg_.push_back( tr( "Add %1 %2s to the to the Flat Fixture Plate.\nClick Take Flat %3 %4 Reading").arg(unitsSize_[3], units_, unitsSize_[3], units_) );
    photoLabelMsg_.push_back( tr( "Set the Fixture Plate into the Incline BASE with the handle at 12 o'clock sloping forward.\nClick Take Front Tilt Reading.") );
    photoLabelMsg_.push_back( tr( "Place %1 %2s on the scale platter.\nClick Take Front Tilt %3 %4 Reading.").arg(unitsSize_[1], units_, unitsSize_[1], units_) );
    photoLabelMsg_.push_back( tr( "Place %1 %2s on the scale platter.\nClick Take Front Tilt %3 %4 Reading.").arg(unitsSize_[3], units_, unitsSize_[3], units_) );
    photoLabelMsg_.push_back( tr( "Rotate the Incline Base 90 degrees COUNTER clockwise sloping to the right.\nKeep the handle at 12 o'clock. Click Take Right Tilt Reading.") );
    photoLabelMsg_.push_back( tr( "Place %1 %2s on the scale platter.\nClick Take Right Tilt %3 %4 Reading.").arg(unitsSize_[3], units_, unitsSize_[3], units_) );
    photoLabelMsg_.push_back( tr( "Rotate the Incline Base 90 degrees COUNTER clockwise sloping to the rear.\nKeep the handle at 12 o'clock. Click Take Back Tilt Reading.") );
    photoLabelMsg_.push_back( tr( "Place %1 %2s on the scale platter.\nClick Take Back Tilt %3 %4 Reading.").arg(unitsSize_[1], units_, unitsSize_[1], units_) );
    photoLabelMsg_.push_back( tr( "Place %1 %2s on the scale platter.\nClick Take Back Tilt %3 %4 Reading.").arg(unitsSize_[3], units_, unitsSize_[3], units_) );
    photoLabelMsg_.push_back( tr( "Rotate the Incline Base 90 degrees COUNTER clockwise sloping left.\nKeep the handle at 12 o'clock. Click Take Left Tilt Reading.") );
    photoLabelMsg_.push_back( tr( "Place %1 %2s on the scale platter.\nClick Take Left Tilt %3 %4 Reading.").arg(unitsSize_[3], units_, unitsSize_[3], units_) );
    photoLabelMsg_.push_back( tr( "Press Calculate to write the calibration to Accelerometer.\nPlease wait for verification.") );
    photoLabelMsg_.push_back( tr( "Verify the Calibration procedure worked.\nClick Verify.") );
    photoLabelMsg_.push_back( tr( "Remove the Load Cell from the Module Plate.\nClick FINISH TEST.") );

    onClicked();        // continue on with the starting the test
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::initializeTestParameters()
{
    receivedFitParams_ = false;                                     // reset if we have passed verification of Gain written. Assume yes unless not received
    passVerification_ = false;                               // reset pass or fail condition
    timeout_error_ = false;
    freezeReads_ = false;

    countmsgs_ = 0;

    ui->lblDateToday->clear();
    ui->txtSerNum->clear();
    ui->txtHwvNum->clear();
    ui->txtAssNum->clear();

    verifyReadings_ = false;

    curTimeSecs_ = 0;
    formattedDate_ = "";

    readDateOfMfg_ = 0;
    readSerialNum_ = "";
    readHWVersion_ = "";
    readAssembNum_ = "";

    writtenDateOfMfg_ = 0;
    //writtenDateOfMfg_ = "";
    writtenSerialNum_ = "";
    writtenHWVersion_ = "";
    writtenAssembNum_ = "";

    serialFrames_   = 0;
    assemblyFrames_ = 0;

    rawAtoDCounts_        	= 0;            // USE THIS
    nonZeroCalibCounts_   	= 0;            // dont use
    zeroCalibCounts_      	= 0;            // dont use
    acclXRawCounts_       	= 0;            // USE THIS
    acclYRawCounts_       	= 0;            // USE THIS
    acclZRawCounts_       	= 0;            // USE THIS
    acclXTiltDegrees_     	= 0;            // dont use
    acclYTiltDegrees_     	= 0;            // dont use

    // These are from Randys Application example
    accx_level_ =  "0";             // Accel X LEVEL
    accy_level_ =  "0";             // Accel Y LEVEL
    accz_level_ =  "0";             // Accel Z LEVEL

    accx_front_ =  "0";             // Accel X FRONT TILT
    accy_front_ =  "0";             // Accel Y FRONT TILT
    accz_front_ =  "0";             // Accel Z FRONT TILT

    accx_left_  =  "0";             // Accel X LEFT TILT
    accy_left_  =  "0";             // Accel Y LEFT TILT
    accz_left_  =  "0";             // Accel Z LEFT TILT

    accx_back_  =  "0";             // Accel X BACK TILT
    accy_back_  =  "0";             // Accel Y BACK TILT
    accz_back_  =  "0";             // Accel Z BACK TILT

    accx_right_ =  "0";             // Accel X RIGHT TILT
    accy_right_ =  "0";             // Accel Y RIGHT TILT
    accz_right_ =  "0";             // Accel Z RIGHT TILT

    counts_level_0kg_0lb_   = "0.0";         // Counts 0kg   LEVEL
    counts_level_5kg_10lb_  = "0.0";         // Counts 5kg   LEVEL (not used)
    counts_level_10kg_20lb_ = "0.0";         // Counts 10kg  LEVEL (not used)
    counts_level_15kg_30lb_ = "0.0";         // Counts 15kg  LEVEL

    counts_front_0kg_0lb_   = "0.0";         // Counts 0kg   FRONT TILT
    counts_front_5kg_10lb_  = "0.0";         // Counts 5kg   FRONT TILT
    counts_front_10kg_20lb_ = "0.0";         // Counts 10kg  FRONT TILT (not used)
    counts_front_15kg_30lb_ = "0.0";         // Counts 15kg  FRONT TILT

    counts_back_0kg_0lb_    = "0.0";         // Counts 0kg   BACK TILT
    counts_back_5kg_10lb_   = "0.0";         // Counts 5kg   BACK TILT
    counts_back_10kg_20lb_  = "0.0";         // Counts 10kg  BACK TILT (not used)
    counts_back_15kg_30lb_  = "0.0";         // Counts 15kg  BACK TILT

    counts_left_0kg_0lb_    = "0.0";         // Counts 0kg   LEFT TILT
    counts_left_5kg_10lb_   = "0.0";         // Counts 5kg   LEFT TILT (not used)
    counts_left_10kg_20lb_  = "0.0";         // Counts 10kg  LEFT TILT (not used)
    counts_left_15kg_30lb_  = "0.0";         // Counts 15kg  LEFT TILT

    counts_right_0kg_0lb_   = "0.0";         // Counts 0kg   RIGHT TILT
    counts_right_5kg_10lb_  = "0.0";         // Counts 5kg   RIGHT TILT (not used)
    counts_right_10kg_20lb_ = "0.0";         // Counts 10kg  RIGHT TILT (not used)
    counts_right_15kg_30lb_ = "0.0";         // Counts 15kg  RIGHT TILT

    // Test test values
    saved_fg_ = "";
    saved_fm_ = "";
    saved_fzerovalue_ = "";
    saved_fspanvalue_ = "";
    saved_fZ0_ = "";
    saved_fD_ = "";
    saved_fx2_ = "";
    saved_fx1_ = "";
    saved_fx0_ = "";
    saved_fy2_ = "";
    saved_fy1_ = "";
    saved_fy0_ = "";
    saved_fxrot0_ = "";
    saved_fxrot1_ = "";
    saved_fxrot2_ = "";
    saved_fyrot0_ = "";
    saved_fyrot1_ = "";
    saved_fyrot2_ = "";
    saved_fzrot0_ = "";
    saved_fzrot1_ = "";
    saved_fzrot2_ = "";
    saved_fxgain_ = "";
    saved_fygain_ = "";
    saved_fgcoeff_ = "";
    saved_fchecksum_ = "";

    read_g_ = "";
    read_m_ = "";
    read_zerovalue_ = "";
    read_spanvalue_ = "";
    read_Z0_ = "";
    read_D_ = "";
    read_x2_ = "";
    read_x1_ = "";
    read_x0_ = "";
    read_y2_ = "";
    read_y1_ = "";
    read_y0_ = "";
    read_xrot0_ = "";
    read_xrot1_ = "";
    read_xrot2_ = "";
    read_yrot0_ = "";
    read_yrot1_ = "";
    read_yrot2_ = "";
    read_zrot0_ = "";
    read_zrot1_ = "";
    read_zrot2_ = "";
    read_xgain_ = "";
    read_ygain_ = "";
    read_gcoeff_ = "";
    read_checksum_ = "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::readCurrentCalibrationSettings(int current_step)
{
    // When calling this method you are reading the calibration for the current picture on the screen.

    std::cout << "readCurrentCalibrationSettings(): current_step:" << current_step << std::endl;
    
    switch( current_step )
    {
		case SetupStep:
        {
            initializeTestParameters();                                 // reset everything

            freezeReads_ = false;
            countmsgs_ = 0;

            // Need to determine the date for future use
            QDateTime currentDateTime = QDateTime::currentDateTime();
            QString str = currentDateTime.toString();                               // EX: Fri Mar 2 13:04:05 2018
            QString now = currentDateTime.toString( "MM/dd/yyyy " );                // format the date like this
            curTimeSecs_  = currentDateTime.toTime_t();                             // EX 1519995845 - this is in GMT FROM 01-01-1970

            std::cout << "generated time currentDateTime: " + str.toStdString() << std::endl;
            std::cout << "formatted time: " + now.toStdString() << std::endl;       // EX: 03/02/2018
            std::cout << "epoch seconds: " + QString::number(curTimeSecs_).toStdString() << std::endl;

            formattedDate_ = now;                                                   // EX: 03/02/2018 - used to display in date field on screen

            break;
        }

        case ScanStep:
        {
            std::cout << "readCurrentCalibrationSettings(): ScanStep does nothing" << std::endl;

            break;
        }

        case ClampStep:
        {
            std::cout << "readCurrentCalibrationSettings(): NEW Clamp Step does nothing" << std::endl;

            break;
        }

        case SpiderStep:
        {
            std::cout << "readCurrentCalibrationSettings(): Spider Step does nothing" << std::endl;

            break;
        }

        case SpiderStep_2:
        {
            std::cout << "readCurrentCalibrationSettings(): NEW Spider-Step does nothing" << std::endl;

            break;
        }

        case NEW_ConnectLoadCellStep:
        {
            std::cout << "readCurrentCalibrationSettings(): NEW Connect Load Cell does nothing" << std::endl;

            break;
        }

        case NEW_ConnectAccelerometerStep:
        {
            std::cout << "readCurrentCalibrationSettings(): NEW Connect Accelerometer does nothing" << std::endl;

            break;
        }

        case NEW_InstallPlatterStep:
        {
            std::cout << "readCurrentCalibrationSettings(): NEW Install Platter does nothing" << std::endl;

            break;
        }

        case NEW_PowerOnStep:
        {
            std::cout << "readCurrentCalibrationSettings(): Begin Step - TURN ON Weigh Tracking Mode" << std::endl;

            if( device_ )
                device_->setWeigherMode( SCALE_WEIGHT_TRACKING_MODE );          // REQ: 0x451,1

            break;
        }

        case FlatStep:
        {
            // need to take an acceleromter reading here one tine to get the gravity constant
            // TODO - cant do this afterall.  The board doesnt know g until I write it. We have 2 g vals right now for
            // TODO - Troy and Hillsboro, and we write these in the end to the board. But what do we do in the field?

            // TEST the written date
            std::cout << "FLATSTEP(): Request date if turned on" << std::endl;
            if( DISPLAY_WRITTEN_DATE )
            {
                std::cout << "readCurrentCalibrationSettings(): Date testing was turned on" << std::endl;
                if( device_ )
                    device_->requestDate();
            }

            saveCurrentCounts(current_step);
            break;
        }

        case FlatStep30:
        {
            saveCurrentCounts(current_step);
            break;
        }

        case FrontStep:
        {
            saveCurrentCounts(current_step);
            break;
        }

        case FrontStep10:
        case FrontStep30:
        {
            saveCurrentCounts(current_step);
            break;
        }

        case RightStep:
        {
            saveCurrentCounts(current_step);
            break;
        }

        case RightStep30:
        {
            saveCurrentCounts(current_step);
            break;
        }

        case BackStep:
        {
            saveCurrentCounts(current_step);
            break;
        }

        case BackStep10:
        case BackStep30:
        {
            saveCurrentCounts(current_step);
            break;
        }

        case LeftStep:
        {
            saveCurrentCounts(current_step);
            break;
        }

        case LeftStep30:
        {
            saveCurrentCounts(current_step);
            break;
        }

        case WriteStep:
        {
            grav_const_ = 980023;                  // defaults to Hillsboro
            isHillsboroGravity_ = true;

            if ( QFile::exists( "/etc/hobart/gravity-troy" ) )
            {
                grav_const_ = 980104;               // Troy
                isHillsboroGravity_ = false;
            }

            writeCalibrationToFlash();                 // 7 write the calibration to the chip

            break;
        }
        case VerifyStep:
        {
            verifyReadings_ = true;                    // set true when we want to read the values FROM the board

            if( device_ )
                device_->requestDate();

            // Now we are waiting for a reply to verify the write process step.
            // If we never receive a reply, we are locked up here.
            // I added a timeout to handle the case where we never receive back a reply to verify

            verifyStepIsVerified_.setSingleShot( true );
            verifyStepIsVerified_.start( 10000 );

            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::setNextCalibrationStep(int current_step)
{
    // When calling this method you are telling the UI where to go, and this case makes you GO to that step.

    std::cout << "calibui::setNextCalibrationStep() START: current_step:" << current_step << std::endl;

    switch( current_step )
    {
        case SetupStep:
        {
            // Displays the Initial/Default/Resting State so go to it. Indicates START of Calibration when CLICKED
            if( isHillsboroGravity_ )
                ui->lblError->setText(tr("Using Hillsboro location gravity Constant"));
            else
                ui->lblError->setText(tr("Using Troy location gravity Constant"));

            ui->wdgtStackedWidgetMain->setCurrentIndex( 0 );
            ui->wdgtStackedWidgetButs->setCurrentIndex( 1 );
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case ScanStep:
        {
            // Displays the Scan Barcodes State so go to it. Click lock in load cell when done
            ui->wdgtStackedWidgetMain->setCurrentIndex( 1 );
            ui->wdgtStackedWidgetButs->setCurrentIndex( 0 );
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            
            ui->lblDateToday->setText( formattedDate_ );
            ui->txtAssNum->setFocus();

            // Im trying to resize that scan screen step
            int w_wgtSteps = ui->wgtSteps->width();
            int h_wgtSteps = ui->wgtSteps->height();

            int w_wgtScans = ui->wgtScans->width();
            int h_wgtScans = ui->wgtScans->height();

            showPhoto_->setGeometry(0,0,w_wgtSteps,h_wgtSteps);
            ui->wgtScans->setGeometry(0,0,w_wgtScans,h_wgtScans);
            break;
        }

        case ClampStep:
        {
            if( !isHillsboroGravity_ )
                ui->lblGravityConst->setText( tr( "Using Troy gravity constant" ) );
            else
                ui->lblGravityConst->setText( tr("Using Hillsboro gravity constant") );

            ui->wdgtStackedWidgetMain->setCurrentIndex( 0 );
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case SpiderStep:         // was originally named Spider-Step
        {
            //ui->wdgtStackedWidgetMain->setCurrentIndex( 0 );
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case SpiderStep_2:
        {
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case NEW_ConnectLoadCellStep:
        {
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case NEW_ConnectAccelerometerStep:
        {
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case NEW_InstallPlatterStep:
        {
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case NEW_PowerOnStep:
        {
            // Displays the Power on Black Box instructions.

            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            nextPhoto_->setEnabled(false);
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case FlatStep:
        case FlatStep30:
        {
            writeScannedFields();
            ui->loadCellData->setVisible(true);
            // Displays the plate flat on the table step. Indicates Horizontal Reading will be taken when CLICKED
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case FrontStep:
        case FrontStep10:
        case FrontStep30:
        {
            //hilight x axis labels
            highlightDataLabels(true,false,false,false);
            // Displays 12oclock STEP. Indicates 12oclock reading will be taken when CLICKED
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case RightStep:
        case RightStep30:
        {
            //hilight Z axis labels
            highlightDataLabels(false,false,true,false);
            // Display 3oclock STEP. Indicates 3oclock reading will be taken when clicked
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case BackStep:
        case BackStep10:
        case BackStep30:
        {
            //hilight x axis labels
            highlightDataLabels(true,false,false,false);
            // Display 6 STEP. Indicates 6oclock reading will be taken when clicked.
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case LeftStep:
        case LeftStep30:
        {
            //hilight Z axis labels
            highlightDataLabels(false,false,true,false);
            // Display 9oclock STEP. Indicates 9oclock reading will be taken when clicked.
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case WriteStep:
        {
            //reset label backgrounds
            highlightDataLabels(false,false,false,false);
            ui->loadCellData->setVisible(false);
            // Display Write Calibration Step
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case VerifyStep:
        {
            // Display the PASS OR FAIL image and advance text to Finish Test
            std::cout << "setNextStep VerifyStep " << std::endl;
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case CompleteStep:
        {
            // Display Pass OR Fail here, then reset everything so we can begin another test
            std::cout << "setNextStep CompleteStep " << std::endl;
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );       // reset text everywhere
            explainPhoto_->setText( photoLabelMsg_[current_step] );

            if( device_ )
                device_->setWeigherMode( SCALE_MANUAL_MODE );               // REQ: 0x451,3 - manua
            break;
        }

        case StartOver:
        {
            // Display the START TEST step again.  Start over. Reset all variables and steps
            step_ = SetupStep;                                       // reset steps
            ui->wdgtStackedWidgetButs->setCurrentIndex( 1 );         // displayed buttons
            setPhotoStep(step_);                                     // reset pic step
            nextPhoto_->setText( photoButtonMsg_[step_] );           // reset button text
            explainPhoto_->setText( photoLabelMsg_[step_] );         // reset instructions

            if( isHillsboroGravity_ )
                ui->lblError->setText(tr("Using Hillsboro location gravity Constant"));
            else
                ui->lblError->setText(tr("Using Troy location gravity Constant"));

            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::setPhotoStep(int current_step)
{
    if( current_step > MAX_STEPS )
    {
        step_ = 0;                     // something went wrong.  Reset everything.
        int pStep = step_;
        setNextCalibrationStep(pStep);
    }

    int w_wgtSteps = ui->wgtSteps->width();
    int h_wgtSteps = ui->wgtSteps->height();
    showPhoto_->setGeometry(0,0,w_wgtSteps,h_wgtSteps);

    // generate image
    QPixmap pixmap( showPhoto_->size() );   // showPhoto = the label
    pixmap.fill( Qt::transparent );

    QString pass_image;
    QPixmap pass_pixmap;
    QPixmap scaledPixmap;

    if( CompleteStep == current_step)
    {
        if( passVerification_ )
        {
            if( timeout_error_ )
                pass_image =  ":/steps/step23_fail.png";
            else
                pass_image =  ":/steps/step23_pass.png";
        }
        else
            pass_image =  ":/steps/step23_fail.png";

        pass_pixmap = pass_image;
        QPixmap scaledImage( pass_pixmap.scaled( pixmap.size(), Qt::KeepAspectRatio ) );
        scaledPixmap = scaledImage;
    }
    else
    {
        QPixmap scaledImage( stepPhotos_[current_step].scaled( pixmap.size(), Qt::KeepAspectRatio ) );
        scaledPixmap = scaledImage;
    }

    QPoint p( (pixmap.width() - scaledPixmap.width()) / 2, (pixmap.height() - scaledPixmap.height()) / 2 );

    // draw the image
    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.drawPixmap( p, scaledPixmap );

    // set the image
    QPalette palette( showPhoto_->palette() );
    palette.setBrush( showPhoto_->backgroundRole(), pixmap );
    showPhoto_->setPalette( palette );

    // show the image!
    showPhoto_->setAutoFillBackground( true );
    showPhoto_->show();

    update();                               // refresh ui
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onClicked()
{
    nextPhoto_->setEnabled(false);
    if((step_ >= FlatStep && step_ <= LeftStep30)){
        if(!countsValid(step_)){
            ui->lblError->setText(tr("Incorrect Loadcell orientation"));
            return;
        }
    }
    std::cout << "\n===== calibui::onClicked() =====: Perform next step, step_:" << step_ << std::endl;

    int pStep = step_;

    if( step_ == VerifyStep )
    {
        int pStep = step_;
        readCurrentCalibrationSettings(pStep);
    }
    else if( step_ == StartOver )
    {
        step_++;
        pStep = step_;
        setNextCalibrationStep(pStep);
    }
    else
    {
        readCurrentCalibrationSettings(pStep);      // if you are on SPECIFIC steps, you want to take a reading before going on to next step.
        step_++;
        pStep = step_;
        setNextCalibrationStep(pStep);
    }
    if(!(step_ == NEW_PowerOnStep || (step_ >= FlatStep && step_ <= LeftStep30))){
        nextPhoto_->setEnabled(true);// these steps waits for the events to happen before enabling the next step
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onClickedLbs()
{
    units_ = "lb";
    unitsSize_.clear();
    unitsSize_.push_back( "0" );
    unitsSize_.push_back( "10" );
    unitsSize_.push_back( "20" );
    unitsSize_.push_back( "30" );

    initializeText();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onClickedKgs()
{
    units_ = "kg";
    unitsSize_.clear();
    unitsSize_.push_back( "0" );
    unitsSize_.push_back( "5" );
    unitsSize_.push_back( "10" );
    unitsSize_.push_back( "15" );

    initializeText();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateValueMaxDate( uint32_t mfgDate )
{
    std::cout << "\n===== calibui::onUpdateValueMaxDate(): START "  << std::endl;

    readDateOfMfg_ = mfgDate;           // Im just going to compare seconds - not any kinds of formatted date

    std::cout << "onUpdateValueMaxDate(): readdinDateOfMfg: " << readDateOfMfg_ << std::endl;
    std::cout << "onUpdateValueMaxDate(): writtenDateOfMfg: " << writtenDateOfMfg_ << std::endl;
    std::cout << "onUpdateValueMaxDate(): NOW call requestAssemblyNumber()" << std::endl;

    // Date calls Assembly Number which calls Hardware Version which calls Serial Number which call FIT Struct
    if( device_ )
        device_->requestAssemblyNumber();               // 0x420,0
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateValueMaxAssemblyNumber( QString assemNum )
{
    std::cout << "\n===== calibui::onUpdateValueMaxAssemblyNumber(): START "  << std::endl;

    // This method gets called twice because it has to handle 2 frames.  So I need a var to keep track of which call it is so I can append data

    assemblyFrames_++;

    if( 1 == assemblyFrames_ )
    {
        readAssembNum_ = assemNum;              // add the initial frame 1 string
        std::cout << "onUpdateValueMaxAssemblyNumber(): Frame Count = 1 readAssemNum: "  << readAssembNum_.toStdString() << std::endl;
    }
    else if( 2 == assemblyFrames_ )
    {
        readAssembNum_.append( assemNum );        // for frame 2 add to the initial frame 1 string
        std::cout << "onUpdateValueMaxAssemblyNumber(): Frame Count = 2  readdinAssemNum: "  << readAssembNum_.toStdString() << std::endl;
        std::cout << "onUpdateValueMaxAssemblyNumber(): Frame Count = 2  writtenAssemNum: "  << writtenAssembNum_.toStdString() << std::endl;
        std::cout << "onUpdateValueMaxAssemblyNumber(): NOW call requestHardwareVersion" << std::endl;

        if( DISPLAY_WRITTEN_DATE )
            assemblyFrames_ = 0;

        // Assembly Number which calls Hardware Version which calls Serial Number
        if( device_ )
            device_->requestHardwareVersion();      // 0x420,1 only do this if we have both pieces of the assemblyNumber processes
    }
    else
    {
        std::cout << "onUpdateValueMaxAssemblyNumber(): CRITICAL ERROR: FRAME COUNTER > 2"  << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateValueMaxHWVersion( QString hwVersion )
{
    std::cout << "\n===== calibui::onUpdateValueMaxHWVersion(): START "  << std::endl;

    std::cout << "onUpdateValueMaxHWVersion(): Perform next step, step_:" << step_ << std::endl;
    readHWVersion_ = hwVersion;

    std::cout << "onUpdateValueMaxHWVersion(): readdinHwVersion: " << readHWVersion_.toStdString() << std::endl;
    std::cout << "onUpdateValueMaxHWVersion(): writtenHwVersion: " << writtenHWVersion_.toStdString() << std::endl;
    std::cout << "onUpdateValueMaxHWVersion(): NOW call requestSerialNumber()" << std::endl;

    // Hardware Version calls Serial Number
    if( device_ )
        device_->requestSerialNumber();                     // 0x420,2
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateValueMaxSerialNumber( QString serialNum )
{
    std::cout << "\n===== calibui::onUpdateValueMaxSerialNumber(): START "  << std::endl;

    serialFrames_++;

    if( 1 == serialFrames_ )
    {
        readSerialNum_ = serialNum.trimmed();                 // initial frame 1 string
        std::cout << "onUpdateValueMaxSerialNumber(): Frame Count = 1 readSerialNum_: "  << readSerialNum_.toStdString() << std::endl;
    }
    else if( 2 == serialFrames_ )
    {
        readSerialNum_.append( serialNum.trimmed() );         // in frame 2 add to frame 1 string
        std::cout << "onUpdateValueMaxSerialNumber(): Frame Count = 2 readdinSerialNum_: "  << readSerialNum_.toStdString() << std::endl;
        std::cout << "onUpdateValueMaxSerialNumber(): Frame Count = 2 writtenSerialNum_: "  << writtenSerialNum_.toStdString() << std::endl;

        // Serial Number requests the FIT Parameter
        std::cout << std::endl;
        std::cout << "onUpdateValueMaxSerialNumber(): TEST DISPLAY_WRITTEN_DATE so pop dialog "  << writtenSerialNum_.toStdString() << std::endl;

        if( DISPLAY_WRITTEN_DATE )
        {
            std::cout << "onUpdateValueMaxSerialNumber(): TEST DISPLAY_WRITTEN_DATE is TRUE so pop dialog "  << writtenSerialNum_.toStdString() << std::endl;
            QString errMsg = "\n=============================== Scanned Values ===============================";
            if( writtenDateOfMfg_ != readDateOfMfg_ )
            {
                errMsg += "\n\nThe written Date of Manufacturing did not match the saved Date of Manufacturing.";
                errMsg += "\nFAILED   writtenDateOfMfg_ = " + QString::number(writtenDateOfMfg_) + "     readDateOfMfg_ = " + QString::number(readDateOfMfg_);
                std::cout << "FAILED   writtenDateOfMfg_ = " << writtenDateOfMfg_ << "     readDateOfMfg_ = " << readDateOfMfg_  << std::endl;
            }
            else
            {
                errMsg += "\n\nThe written Date of Manufacturing matched the saved Date of Manufacturing.";
                errMsg += "\nPASSED   writtenDateOfMfg_ = " + QString::number(writtenDateOfMfg_) + "     readDateOfMfg_ = " + QString::number(readDateOfMfg_);
                std::cout << "PASSED   writtenDateOfMfg_ = " << writtenDateOfMfg_ << "     readDateOfMfg_ = " << readDateOfMfg_  << std::endl;
            }

            if( readSerialNum_.compare(writtenSerialNum_, Qt::CaseInsensitive) != 0 )
            {
                errMsg += "\n\nThe written Serial Number did not match the saved Serial Number.";
                errMsg +="\nFAILED   writtenSerialNumber = " + writtenSerialNum_ + "     readSerialNumber_ = " + readSerialNum_;
                std::cout << "FAILED   writtenSerialNumber = " << writtenSerialNum_.toStdString() << "     readSerialNumber_ = " << readSerialNum_.toStdString()  << std::endl;
            }
            else
            {
                errMsg += "\n\nThe written Serial Number matched the saved Serial Number.";
                errMsg += "\nPASSED   writtenSerialNumber = " + writtenSerialNum_ + "     readSerialNumber_ = " + readSerialNum_;
                std::cout << "PASSED   writtenSerialNumber = " << writtenSerialNum_.toStdString() << "     readSerialNumber_ = " << readSerialNum_.toStdString()  << std::endl;
            }

            if( readHWVersion_.compare(writtenHWVersion_, Qt::CaseInsensitive) != 0 )
            {
                errMsg += "\n\nThe written Hardware Version did not match the saved Hardware Version.";
                errMsg += "\nFAILED   writtenHWVersion = " + writtenHWVersion_ + "     readHWVersion_ = " + readHWVersion_;
                std::cout << "FAILED   writtenHWVersion = " << writtenHWVersion_.toStdString() << "     readHWVersion_ = " << readHWVersion_.toStdString()  << std::endl;
            }
            else
            {
                errMsg += "\n\nThe written Hardware Version matched the saved Hardware Version.";
                errMsg += "\nPASSED   writtenHWVersion = " + writtenHWVersion_ + "     readHWVersion_ = " + readHWVersion_;
                std::cout << "PASSED   writtenHWVersion = " << writtenHWVersion_.toStdString() << "     readHWVersion_ = " << readHWVersion_.toStdString()  << std::endl;
            }

            if( readAssembNum_.compare(writtenAssembNum_, Qt::CaseInsensitive) != 0 )
            {
                errMsg += "\n\nThe written Assembly Number did not match the saved Assembly Number.";
                errMsg += "\nFAILED   writtenAssembNum = " + writtenAssembNum_ + "     readAssembNum_ = " + readAssembNum_;
                std::cout << "FAILED   writtenAssembNum = " << writtenAssembNum_.toStdString() << "     readAssembNum_ = " << readAssembNum_.toStdString()  << std::endl;
            }
            else
            {
                errMsg += "\n\nThe written Assembly Number matched the saved Assembly Number.";
                errMsg += "\nPASSED   writtenAssembNum = " + writtenAssembNum_ + "     readAssembNum_ = " + readAssembNum_;
                std::cout << "PASSED   writtenAssembNum = " << writtenAssembNum_.toStdString() << "     readAssembNum_ = " << readAssembNum_.toStdString()  << std::endl;
            }

            popErrorDialog(errMsg);
        }

        if( verifyReadings_ )
        {
            std::cout << "onUpdateValueMaxSerialNumber() CALL requestFitCalibration() " << std::endl;
            if( device_ )
                device_->requestFitCalibration();
        }
    }
    else
    {
        std::cout << "onUpdateValueMaxSerialNumber(): CRITICAL ERROR: FRAME COUNTER > 2"  << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateWeigherStateChanged(weigherState state, float weight)
{
    std::cout << "\n===== calibui::onWeigherStateChanged(): START ====="  << std::endl;

    // This is where the weigher status will be handled
    if( stable == state )
    {
        // do something
        std::cout << "onWeigherStateChanged(): "  << state << std::endl;
        std::cout << "onWeigherStateChanged(): "  << weight << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateValueMaxWeigherStatus(uint16_t flags, uint32_t rawAtoDCounts, int32_t nonZeroCalibCounts, int32_t zeroCalibCounts, int32_t acclXRawCounts, int32_t acclYRawCounts, int32_t acclZRawCounts, int16_t acclXTiltDegrees, int16_t acclYTiltDegrees)
{
    countmsgs_++;

    if( freezeReads_ )
    {
        std::cout << "====== onUpdateValueMaxWeigherStatus() freezeReads_ is TRUE so EXIT" << std::endl;
        return;
    }
    flags_                  = flags;
    rawAtoDCounts_        	= rawAtoDCounts;                        // use this
    nonZeroCalibCounts_   	= nonZeroCalibCounts;
    zeroCalibCounts_      	= zeroCalibCounts;
    acclXRawCounts_       	= acclXRawCounts;                       // use this
    acclYRawCounts_       	= acclYRawCounts;                       // use this
    acclZRawCounts_       	= acclZRawCounts;                       // use this
    acclXTiltDegrees_     	= acclXTiltDegrees;
    acclYTiltDegrees_     	= acclYTiltDegrees;
    std::cout << "====== onUpdateValueMaxWeigherStatus() flags: " << std::hex << flags << std::dec << " Step " << step_ << std::endl;

    updateLoadCellData(acclXRawCounts_, acclYRawCounts_, acclZRawCounts_,rawAtoDCounts_);
    //In Power on step we wait for the Power Switch to be toggled before we continue
    if(step_ == NEW_PowerOnStep){
        if((flags_ & WEIGHER_ACCL_COMM_ON) == WEIGHER_ACCL_COMM_ON){
            if( isHillsboroGravity_ )
                ui->lblError->setText(tr("Using Hillsboro location gravity Constant"));
            else
                ui->lblError->setText(tr("Using Troy location gravity Constant"));
            nextPhoto_->setEnabled(true);
            //In all of these steps we are waiting for weight readings to stablize before we move on
        }else{
            nextPhoto_->setEnabled(false);
            ui->lblError->setText(tr("Turn on the Power Switch to enable the next step"));
        }
    }else if (step_ >= FlatStep && step_ <= LeftStep30){

        if(!((flags_ & SMALL_MOTION) == SMALL_MOTION || (flags_ & LARGE_MOTION) == LARGE_MOTION)){
            if(countsValid(step_)){
                if( isHillsboroGravity_ )
                    ui->lblError->setText(tr("Using Hillsboro location gravity Constant"));
                else
                    ui->lblError->setText(tr("Using Troy location gravity Constant"));
            }else{
                ui->lblError->setText(tr("Incorrect Loadcell orientation"));
            }
            nextPhoto_->setEnabled(true);
        }else{
            nextPhoto_->setEnabled(false);
            ui->lblError->setText(tr("Allow the weight to settle to enable the next step"));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateValueMaxFitParameters(int32_t g, int32_t m, int32_t zerovalue, int32_t spanvalue, int32_t Z0, int32_t D, int32_t x2, int32_t x1, int32_t x0, int32_t y2, int32_t y1, int32_t y0, int32_t xrot0, int32_t xrot1, int32_t xrot2, int32_t yrot0, int32_t yrot1, int32_t yrot2, int32_t zrot0, int32_t zrot1, int32_t zrot2, int32_t xgain, int32_t ygain, int32_t gcoeff, int32_t checksum)
{
    std::cout << "\n===== calibui::onUpdateValueMaxFitParameters() START" << " =====" << std::endl;

    receivedFitParams_ = true;              // call off the timeout

    read_g_ = QString::number(g);
    read_m_ = QString::number(m);
    read_zerovalue_ = QString::number(zerovalue);
    read_spanvalue_ = QString::number(spanvalue);
    read_Z0_ = QString::number(Z0);
    read_D_ = QString::number(D);
    read_x2_ = QString::number(x2);
    read_x1_ = QString::number(x1);
    read_x0_ = QString::number(x0);
    read_y2_ = QString::number(y2);
    read_y1_ = QString::number(y1);
    read_y0_ = QString::number(y0);
    read_xrot0_ = QString::number(xrot0);
    read_xrot1_ = QString::number(xrot1);
    read_xrot2_ = QString::number(xrot2);
    read_yrot0_ = QString::number(yrot0);
    read_yrot1_ = QString::number(yrot1);
    read_yrot2_ = QString::number(yrot2);
    read_zrot0_ = QString::number(zrot0);
    read_zrot1_ = QString::number(zrot1);
    read_zrot2_ = QString::number(zrot2);
    read_xgain_ = QString::number(xgain);
    read_ygain_ = QString::number(ygain);
    read_gcoeff_ = QString::number(gcoeff);
    read_checksum_ = QString::number(checksum);

    if( DISPLAY_SAVED_READ_FIT_PARAMS || OUTPUT_SAVED_READ_FIT_PARAMS )
    {
        QString msg = "";
        msg += "\nsaved_fg_         = " + saved_fg_           + "        readfg_            "	+ read_g_;
        msg += "\nsaved_fm_         = " + saved_fm_           + "        readfm_            "	+ read_m_;
        msg += "\nsaved_fzerovalue_ = " + saved_fzerovalue_	  + "        readfzerovalue_    " 	+ read_zerovalue_;
        msg += "\nsaved_fspanvalue_ = " + saved_fspanvalue_   + "        readfspanvalue_    "	+ read_spanvalue_;
        msg += "\nsaved_fZ0_        = " + saved_fZ0_          + "        readfZ0_           "	+ read_Z0_;
        msg += "\nsaved_fD_         = " + saved_fD_           + "        readfD_            " 	+ read_D_;
        msg += "\nsaved_fx2_        = " + saved_fx2_          + "        readfx2_           "	+ read_x2_;
        msg += "\nsaved_fx1_        = " + saved_fx1_          + "        readfx1_           "	+ read_x1_;
        msg += "\nsaved_fx0_        = " + saved_fx0_          + "        readfx0_           " 	+ read_x0_;
        msg += "\nsaved_fy2_        = " + saved_fy2_          + "        readfy2_           "	+ read_y2_;
        msg += "\nsaved_fy1_        = " + saved_fy1_          + "        readfy1_           "	+ read_y1_;
        msg += "\nsaved_fy0_        = " + saved_fy0_          + "        readfy0_           " 	+ read_y0_;
        msg += "\nsaved_fxrot0_     = " + saved_fxrot0_       + "        readfxrot0_        "	+ read_xrot0_;
        msg += "\nsaved_fxrot1_     = " + saved_fxrot1_       + "        readfxrot1_        "	+ read_xrot1_;
        msg += "\nsaved_fxrot2_     = " + saved_fxrot2_       + "        readfxrot2_        " 	+ read_xrot2_;
        msg += "\nsaved_fyrot0_     = " + saved_fyrot0_       + "        readfyrot0_        "	+ read_yrot0_;
        msg += "\nsaved_fyrot1_     = " + saved_fyrot1_       + "        readfyrot1_        "	+ read_yrot1_;
        msg += "\nsaved_fyrot2_     = " + saved_fyrot2_       + "        readfyrot2_        " 	+ read_yrot2_;
        msg += "\nsaved_fzrot0_     = " + saved_fzrot0_       + "        readfzrot0_        "	+ read_zrot0_;
        msg += "\nsaved_fzrot1_     = " + saved_fzrot1_       + "        readfzrot1_        "	+ read_zrot1_;
        msg += "\nsaved_fzrot2_     = " + saved_fzrot2_       + "        readfzrot2_        " 	+ read_zrot2_;
        msg += "\nsaved_fxgain_     = " + saved_fxgain_       + "        readfxgain_        "	+ read_xgain_;
        msg += "\nsaved_fygain_     = " + saved_fygain_       + "        readfygain_        "	+ read_ygain_;
        msg += "\nsaved_fgcoeff_    = " + saved_fgcoeff_      + "        readfg_coeff_      " 	+ read_gcoeff_;
        msg += "\nsaved_fchecksum_  = " + saved_fchecksum_    + "        readfchecksum_     "	+ read_checksum_;

        popErrorDialog(msg);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    if( VerifyStep == step_ )
    {
        verifyReadings_ = false;

        // Need to verify if values written to board equal the averages we determined.
        // verify date, serial number, assembly number, hardware version and FIT Calibration
        QString failMsg = passOrFailCalibration();

        std::cout << "===== passOrFailCalibration() RESULTS ===== " << std::endl;
        if( DISPLAY_SAVED_READ_FIT_PARAMS )
            popErrorDialog(failMsg);

        step_++;
        int pStep = step_;
        setNextCalibrationStep(pStep);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::moveOnWithoutVerification()
{
    std::cout << "moveOnWithoutVerification() START " << std::endl;

    if( receivedFitParams_ )
        return;

    // If nobody has set this flag false, then we have timed out and need to exit out.
    if( VerifyStep == step_ )
    {
        // We timed out.  Fail the test and move on to the next step.
        // Really can only come in here if we havent been here yet bc step_ will be advanced and != VerifyStep when verifyTimeout is true
        timeout_error_ = true;
        QString failMsg = "moveOnWithoutVerification() - Verifying the Fit Parameters written to the board has timed out.";
        popErrorDialog(failMsg, timeout_error_);

        step_++;
        int pStep = step_;
        setNextCalibrationStep(pStep);

        return;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::writeCalibrationToFlash()
{
    std::cout << "writeCalibrationToFlash() - start writing calibration" << std::endl;

    const std::vector<double> counts( weigherValues() );
    const std::vector<double> acc_values( accValues() );

    QString msg = "";
    msg = "Print ACC_VALUES AFTER applying resolution";
    msg += "\n\n===== Acc Values =====";
    for( std::size_t i = 0; i < acc_values.size(); i++)
        msg += "\nacc_values[" + QString::number(i) + "]: " + QString::number(acc_values[i]);
    std::cout << msg.toStdString() << std::endl;

    msg = "";
    msg = "Print WEIGHER VALUES After toDouble()";
    msg += "\n\n===== Weigher Values =====";
    for( std::size_t i = 0; i < counts.size(); i++)
        msg += "\nweigher_counts[" + QString::number(i) + "]: " + QString::number(counts[i]);

    if( DISPLAY_GENERATE_FIT_RESULTS )
        std::cout << msg.toStdString() << std::endl;

    const bool use_metric( (units_ == "kg") ? true : false );

    std::string result;

    fit_flash_section f;
    saved_f.checksum = 0;

    double gravity_const = (double) (grav_const_ / RESOLUTION_100K);

    std::cout << "writeCalibrationToFlash() - now call generate_fit()" << std::endl;

    bool passed_generate_fit = generate_fit( counts, use_metric, gravity_const, acc_values, f, result );

    if( DISPLAY_GENERATE_FIT_RESULTS )
    {
        QString resultsStr = QString::fromStdString( result );
        popErrorDialog(resultsStr);
    }
    else
    {
        std::cout << result << std::endl;
    }

    saved_fg_         = QString::number(f.g);
    saved_fm_         = QString::number(f.m);
    saved_fzerovalue_ = QString::number(f.zerovalue);
    saved_fspanvalue_ = QString::number(f.spanvalue);
    saved_fZ0_        = QString::number(f.Z0);
    saved_fD_         = QString::number(f.D);
    saved_fx2_        = QString::number(f.x2);
    saved_fx1_        = QString::number(f.x1);
    saved_fx0_        = QString::number(f.x0);
    saved_fy2_        = QString::number(f.y2);
    saved_fy1_        = QString::number(f.y1);
    saved_fy0_        = QString::number(f.y0);
    saved_fxrot0_     = QString::number(f.xrot[0]);
    saved_fxrot1_     = QString::number(f.xrot[1]);
    saved_fxrot2_     = QString::number(f.xrot[2]);
    saved_fyrot0_     = QString::number(f.yrot[0]);
    saved_fyrot1_     = QString::number(f.yrot[1]);
    saved_fyrot2_     = QString::number(f.yrot[2]);
    saved_fzrot0_     = QString::number(f.zrot[0]);
    saved_fzrot1_     = QString::number(f.zrot[1]);
    saved_fzrot2_     = QString::number(f.zrot[2]);
    saved_fxgain_     = QString::number(f.xgain);
    saved_fygain_     = QString::number(f.ygain);
    saved_fgcoeff_    = QString::number(f.g_coeff);
    saved_fchecksum_  = QString::number(f.checksum);

    if( passed_generate_fit )
    {
        std::cout << "writeCalibrationToFlash - generate_fit routine has PASSED. Now write data to the board." << std::endl;

        saved_f = f;
        passVerification_ = true;

        // write the data (what data, an entire fit routine?  What part of the structure do I write the data back in? All of it?)
        if( device_ )
            device_->setValueMaxFitParameters(f.g, f.m, f.zerovalue, f.spanvalue, f.Z0, f.D, f.x2, f.x1, f.x0, f.y2, f.y1, f.y0, f.xrot[0], f.xrot[1], f.xrot[2], f.yrot[0], f.yrot[1], f.yrot[2], f.zrot[0], f.zrot[1], f.zrot[2], f.xgain, f.ygain, f.g_coeff, f.checksum);            // 0x461 - 100 Bytes
    }
    else
    {
        if( DISPLAY_SAVED_READ_FIT_PARAMS || OUTPUT_SAVED_READ_FIT_PARAMS )
        {
            QString msg = "";
            msg += "The accelerometer FIT Calibration routine has failed.";

            msg += "\n\n===== Passed in values to generate_fit( ) =====";
            msg += "\npassed_generate_fit: " + QString::number(passed_generate_fit);
            msg += "\ngravity: " + QString::number(gravity_const);
            msg += "\nuse_metric: " + QString::number(use_metric);

            msg += "\n\n===== Weigher Counts =====";
            for( std::size_t i = 0; i < counts.size(); i++)
                msg += "\ncounts[" + QString::number(i) + "]: " + QString::number(counts[i]);

            msg += "\n\n===== ACC Values =====";
            for( std::size_t i = 0; i < acc_values.size(); i++)
                msg += "\nacc_values[" + QString::number(i) + "]: " + QString::number(acc_values[i]);

            msg += "\n\n===== The generate_fit &f() values =====";
            msg += "\nsaved_fg_                  = " + saved_fg_;
            msg += "\nsaved_fm_                 = " + saved_fm_;
            msg += "\nsaved_fzerovalue_ = " + saved_fzerovalue_;
            msg += "\nsaved_fspanvalue_ = " + saved_fspanvalue_;
            msg += "\nsaved_fZ0_                = " + saved_fZ0_;
            msg += "\nsaved_fD_                 = " + saved_fD_;
            msg += "\nsaved_fx2_               = " + saved_fx2_;
            msg += "\nsaved_fx1_               = " + saved_fx1_;
            msg += "\nsaved_fx0_               = " + saved_fx0_;
            msg += "\nsaved_fy2_               = " + saved_fy2_;
            msg += "\nsaved_fy1_               = " + saved_fy1_;
            msg += "\nsaved_fy0_               = " + saved_fy0_;
            msg += "\nsaved_fxrot0_         = " + saved_fxrot0_;
            msg += "\nsaved_fxrot1_         = " + saved_fxrot1_;
            msg += "\nsaved_fxrot2_         = " + saved_fxrot2_;
            msg += "\nsaved_fyrot0_         = " + saved_fyrot0_;
            msg += "\nsaved_fyrot1_         = " + saved_fyrot1_;
            msg += "\nsaved_fyrot2_         = " + saved_fyrot2_;
            msg += "\nsaved_fzrot0_         = " + saved_fzrot0_;
            msg += "\nsaved_fzrot1_         = " + saved_fzrot1_;
            msg += "\nsaved_fzrot2_         = " + saved_fzrot2_;
            msg += "\nsaved_fxgain_         = " + saved_fxgain_;
            msg += "\nsaved_fygain_         = " + saved_fygain_;
            msg += "\nsaved_fgcoeff_       = " + saved_fgcoeff_;
            msg += "\nsaved_fchecksum_     = " + saved_fchecksum_;

            popErrorDialog(msg);
        }

        // FIT calibration failed so no need to even compare reads and writes
        passVerification_ = false;
        step_++;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::writeScannedFields()
{
    // get the data from the scanned fields - can I just combine this with the next step
    // save these off for later verification when they are read back out. Test if the same
    std::cout << "\n=====Calibui::writeScannedFields ====="  << std::endl;

    QString serialStr = ui->txtSerNum->text();
    QString hdwareStr = ui->txtHwvNum->text();
    QString assembStr = ui->txtAssNum->text();
    writtenDateOfMfg_ = curTimeSecs_;               //writtenDateOfMfg_ = QString::number(curTimeSecs_);

    serialStr = formatScannedField(serialStr, 16);
    assembStr = formatScannedField(assembStr, 10);
    hdwareStr = formatScannedField(hdwareStr, 2);

    writtenSerialNum_ = serialStr;      // 16
    writtenHWVersion_ = hdwareStr;      // 2
    writtenAssembNum_ = assembStr;      // 10

    if( device_ )
    {
        device_->setValueMaxSerial( serialStr );            // 0x429 - 8 Bytes
        sleepFor(500);
        device_->setValueMaxAssembly( assembStr );          // 0x423 - 8 Bytes
        sleepFor(500);
        device_->setValueMaxDateOfMfg( writtenDateOfMfg_ ); // 0x42C - 4 Bytes
        sleepFor(500);
        device_->setValueMaxHwVersion( hdwareStr );         // 0x426 - 2 Bytes
    }
}

void calibui::sleepFor( int ms ) const
{
    // amout of time to sleep
    QTime sleep( QTime::currentTime().addMSecs( ms ) );

    do
    {
        QApplication::processEvents();

    } while ( QTime::currentTime() < sleep );
}


void calibui::saveCurrentCounts(int current_step)
{
    // Counts are received all of the time and this method saves the current status of the current counts
    QString msg = "\n\n===== saveCurrentCounts() START  ===== ";
    msg += "\nstatus counts " + QString::number(countmsgs_) + " and current_step " + current_step;

    freezeReads_ = true;

    switch( current_step )
    {
        case FlatStep:
            counts_level_0kg_0lb_ = QString::number(rawAtoDCounts_);
            accx_level_ = QString::number(acclXRawCounts_);
            accy_level_ = QString::number(acclYRawCounts_);
            accz_level_ = QString::number(acclZRawCounts_); 
            msg += "\nsaveCurrentCounts::LevelStep 0";
            msg += "\naccl_X_Counts_: " + accx_level_ + "\naccl_Y_Counts_: " + accy_level_ + "\naccl_Z_Counts_: " + accz_level_;
            msg += "\nRawA2D-Flat 0:" + counts_level_0kg_0lb_;

            break;

        case FlatStep30:
            counts_level_15kg_30lb_ = QString::number(rawAtoDCounts_);
            msg += "\nsaveCurrentCounts::LevelStep 15";
            msg += "\nRawA2D-Level 15: " + counts_level_15kg_30lb_;

            break;

        case FrontStep:
            counts_front_0kg_0lb_ = QString::number(rawAtoDCounts_);
            accx_front_ = QString::number(acclXRawCounts_);
            accy_front_ = QString::number(acclYRawCounts_);
            accz_front_ = QString::number(acclZRawCounts_);
            msg += "\nsaveCurrentCounts::FrontStep 0";
            msg += "\nacclXRawCounts_: " + accx_front_ + "\nacclYRawCounts_: " + accy_front_ + "\nacclZRawCounts_:" + accz_front_;
            msg += "\nRawA2D Front 0: " + counts_front_0kg_0lb_;

            break;

        case FrontStep10:
            counts_front_5kg_10lb_ = QString::number(rawAtoDCounts_);
            msg += "\nsaveCurrentCounts::FrontStep 5";
            msg += "\nRawA2D Front 5: " + counts_front_5kg_10lb_;
            break;

        case FrontStep30:
            counts_front_15kg_30lb_ = QString::number(rawAtoDCounts_);
            msg += "\nsaveCurrentCounts::FrontStep 15";
            msg += "\nRawA2D-Front 15: " + counts_front_15kg_30lb_;
            break;

        case RightStep:
            counts_right_0kg_0lb_ = QString::number(rawAtoDCounts_);
            accx_right_ = QString::number(acclXRawCounts_);
            accy_right_ = QString::number(acclYRawCounts_);
            accz_right_ = QString::number(acclZRawCounts_);
            msg += "\nsaveCurrentCounts::RightStep 0";
            msg += "\nacclXRawCounts_: " + accx_right_ + "\nacclYRawCounts_: " + accy_right_ + "\nacclZRawCounts_: " + accz_right_;
            msg += "\nRawA2D Right 0: " + counts_right_0kg_0lb_;
            break;

        case RightStep30:
            counts_right_15kg_30lb_ = QString::number(rawAtoDCounts_);
            msg += "\nsaveCurrentCounts::RightStep 15";
            msg += "\nRawA2D-Right 15: " + counts_right_15kg_30lb_;
            break;

        case BackStep:
            counts_back_0kg_0lb_ = QString::number(rawAtoDCounts_);
            accx_back_ = QString::number(acclXRawCounts_);
            accy_back_ = QString::number(acclYRawCounts_);
            accz_back_ = QString::number(acclZRawCounts_);
            msg += "\nsaveCurrentCounts::BackStep 0";
            msg += "\nacclXRawCounts_: " + accx_back_ + "\nacclYRawCounts_: " + accy_back_ + "\nacclZRawCounts_: " + accz_back_;
            msg += "\nRawA2D-Back 0: " + counts_back_0kg_0lb_;
            break;

        case BackStep10:
            counts_back_5kg_10lb_ = QString::number(rawAtoDCounts_);
            msg += "\nsaveCurrentCounts::BackStep 5";
            msg += "\nRawA2D-Back 5: " + counts_back_5kg_10lb_;
            break;

        case BackStep30:
            counts_back_15kg_30lb_ = QString::number(rawAtoDCounts_);
            msg += "\nsaveCurrentCounts::BackStep 15";
            msg += "\nRawA2D-Back15: " + counts_back_15kg_30lb_;
            break;

        case LeftStep:
            counts_left_0kg_0lb_ = QString::number(rawAtoDCounts_);
            accx_left_ = QString::number(acclXRawCounts_);
            accy_left_ = QString::number(acclYRawCounts_);
            accz_left_ = QString::number(acclZRawCounts_);
            msg += "\nsaveCurrentCounts::LeftStep 0";
            msg += "\nacclXRawCounts_: " + accx_left_ + "\nacclYRawCounts_: " + accy_left_ + "\nacclZRawCounts_: " + accz_left_;
            msg += "\nRawA2D-Left 0: " + counts_left_0kg_0lb_;
            break;

        case LeftStep30:
            counts_left_15kg_30lb_ = QString::number(rawAtoDCounts_);
            msg += "\nsaveCurrentCounts::LeftStep 15";
            msg += "\nRawA2D-Left 15: " + counts_left_15kg_30lb_;
            break;

        default:
            msg += "\nsaveCurrentCounts::default case saved NOTHING";
            break;
    }

    freezeReads_ = false;

    if( DISPLAY_X_Y_Z_COUNTS )
        popErrorDialog(msg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString calibui::passOrFailCalibration()
{
    // This compares read back in values from the board to see if they saved properly.

    passVerification_ = true;

    QString errMsg = "Board Write Error!\n";

    // TODO - Need to find out why the dates dont match

    QString tempRead    = "";
    QString tempWritten = "";

    if( writtenDateOfMfg_ != readDateOfMfg_ )
    {
        errMsg += "\nThe written Date of Manufacturing did not match the saved Date of Manufacturing.";
        passVerification_ = false;            // SKIP date for now bc we know it fails
        std::cout << "FAILED   writtenDateOfMfg_ = " << writtenDateOfMfg_ << "     readDateOfMfg_ = " << readDateOfMfg_  << std::endl;
    }
    else
        std::cout << "PASSED   writtenDateOfMfg_ = " << writtenDateOfMfg_ << "     readDateOfMfg_ = " << readDateOfMfg_  << std::endl;

    tempRead = readSerialNum_.trimmed();
    tempWritten = writtenSerialNum_.trimmed();
    if( tempRead.compare(tempWritten, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe written Serial Number did not match the saved Serial Number.";
        passVerification_ = false;
        std::cout << "FAILED   writtenSerialNumber = " << writtenSerialNum_.toStdString() << "     readSerialNumber_ = " << readSerialNum_.toStdString()  << std::endl;
        std::cout << "FAILED   writtenSerialNumberLEN = " << tempWritten.length() << "     readSerialNumberLEN_ = " << tempRead.length()  << std::endl;

        if( readSerialNum_ == writtenSerialNum_)
            std::cout << "     BUT PASSED   writtenSerialNumber == readSerialNumber_" << std::endl;
        else
            std::cout << "     AND FAILED   writtenSerialNumber == readSerialNumber_" <<  std::endl;
    }
    else
        std::cout << "PASSED   writtenSerialNumber = " << writtenSerialNum_.toStdString() << "     readSerialNumber_ = " << readSerialNum_.toStdString()  << std::endl;

    tempRead = readHWVersion_.trimmed();
    tempWritten = writtenHWVersion_.trimmed();
    if( tempRead.compare(tempWritten, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe written Hardware Version did not match the saved Hardware Version.";
        passVerification_ = false;
        std::cout << "FAILED   writtenHWVersion = " << writtenHWVersion_.toStdString() << "     readHWVersion_ = " << readHWVersion_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   writtenHWVersion = " << writtenHWVersion_.toStdString() << "     readHWVersion_ = " << readHWVersion_.toStdString()  << std::endl;

    tempRead = readAssembNum_.trimmed();
    tempWritten = writtenAssembNum_.trimmed();
    if( tempRead.compare(tempWritten, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe written Assembly Number did not match the saved Assembly Number.";
        passVerification_ = false;
        std::cout << "FAILED   writtenAssembNum = " << writtenAssembNum_.toStdString() << "     readAssembNum_ = " << readAssembNum_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   writtenAssembNum = " << writtenAssembNum_.toStdString() << "     readAssembNum_ = " << readAssembNum_.toStdString()  << std::endl;

    if( read_g_.compare(saved_fg_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fg_ did not match the read_g_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fg_ = " << saved_fg_.toStdString() << "     read_g_ = " << read_g_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fg_ = " << saved_fg_.toStdString() << "     read_g_ = " << read_g_.toStdString()  << std::endl;

    if( read_m_.compare(saved_fm_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fm_ did not match the read_m_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fm_ = " << saved_fm_.toStdString() << "     read_m_ = " << read_m_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fm_ = " << saved_fm_.toStdString() << "     read_m_ = " << read_m_.toStdString()  << std::endl;

    if( read_zerovalue_.compare(saved_fzerovalue_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fzerovalue_ did not match the read_zerovalue_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fzerovalue_ = " << saved_fzerovalue_.toStdString() << "     read_zerovalue_ = " << read_zerovalue_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fzerovalue_ = " << saved_fzerovalue_.toStdString() << "     read_zerovalue_ = " << read_zerovalue_.toStdString()  << std::endl;

    if( read_spanvalue_.compare(saved_fspanvalue_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fspanvalue_ did not match the read_spanvalue_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fspanvalue_ = " << saved_fspanvalue_.toStdString() << "     read_spanvalue_ = " << read_spanvalue_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fspanvalue_ = " << saved_fspanvalue_.toStdString() << "     read_spanvalue_ = " << read_spanvalue_.toStdString()  << std::endl;

    if( read_Z0_.compare(saved_fZ0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fZ0_ did not match the read_Z0_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fZ0_ = " << saved_fZ0_.toStdString() << "     read_Z0_ = " << read_Z0_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fZ0_ = " << saved_fZ0_.toStdString() << "     read_Z0_ = " << read_Z0_.toStdString()  << std::endl;

    if( read_D_.compare(saved_fD_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fD_ did not match the read_D_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fD_ = " << saved_fD_.toStdString() << "     read_D_ = " << read_D_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fD_ = " << saved_fD_.toStdString() << "     read_D_ = " << read_D_.toStdString()  << std::endl;

    if( read_x2_.compare(saved_fx2_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fx2_ did not match the read_x2_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fx2_ = " << saved_fx2_.toStdString() << "     read_x2_ = " << read_x2_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fx2_ = " << saved_fx2_.toStdString() << "     read_x2_ = " << read_x2_.toStdString()  << std::endl;

    if( read_x1_.compare(saved_fx1_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fx1_ did not match the read_x1_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fx1_ = " << saved_fx1_.toStdString() << "     read_x1_ = " << read_x1_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fx1_ = " << saved_fx1_.toStdString() << "     read_x1_ = " << read_x1_.toStdString()  << std::endl;

    if( read_x0_.compare(saved_fx0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fx0_ did not match the read_x0_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fx0_ = " << saved_fx0_.toStdString() << "     read_x0_ = " << read_x0_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fx0_ = " << saved_fx0_.toStdString() << "     read_x0_ = " << read_x0_.toStdString()  << std::endl;

    if( read_y2_.compare(saved_fy2_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fy2_ did not match the read_y2_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fy2_ = " << saved_fy2_.toStdString() << "     read_y2_ = " << read_y2_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fy2_ = " << saved_fy2_.toStdString() << "     read_y2_ = " << read_y2_.toStdString()  << std::endl;

    if( read_y1_.compare(saved_fy1_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fy1_ did not match the read_y1_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fy1_ = " << saved_fy1_.toStdString() << "     read_y1_ = " << read_y1_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fy1_ = " << saved_fy1_.toStdString() << "     read_y1_ = " << read_y1_.toStdString()  << std::endl;

    if( read_y0_.compare(saved_fy0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fy0_ did not match the read_y0_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fy0_ = " << saved_fy0_.toStdString() << "     read_y0_ = " << read_y0_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fy0_ = " << saved_fy0_.toStdString() << "     read_y0_ = " << read_y0_.toStdString()  << std::endl;

    if( read_xrot0_.compare(saved_fxrot0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fxrot0_ did not match the read_xrot0_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fxrot0_ = " << saved_fxrot0_.toStdString() << "     read_xrot0_ = " << read_xrot0_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fxrot0_ = " << saved_fxrot0_.toStdString() << "     read_xrot0_ = " << read_xrot0_.toStdString()  << std::endl;

    if( read_xrot1_.compare(saved_fxrot1_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fxrot1_ did not match the read_xrot1_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fxrot1_ = " << saved_fxrot1_.toStdString() << "     read_xrot1_ = " << read_xrot1_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fxrot1_ = " << saved_fxrot1_.toStdString() << "     read_xrot1_ = " << read_xrot1_.toStdString()  << std::endl;

    if( read_xrot2_.compare(saved_fxrot2_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fxrot2_ did not match the read_xrot2_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fxrot2_ = " << saved_fxrot2_.toStdString() << "     read_xrot2_ = " << read_xrot2_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fxrot2_ = " << saved_fxrot2_.toStdString() << "     read_xrot2_ = " << read_xrot2_.toStdString()  << std::endl;

    if( read_yrot0_.compare(saved_fyrot0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fyrot0_ did not match the read_yrot0_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fyrot0_ = " << saved_fyrot0_.toStdString() << "     read_yrot0_ = " << read_yrot0_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fyrot0_ = " << saved_fyrot0_.toStdString() << "     read_yrot0_ = " << read_yrot0_.toStdString()  << std::endl;

    if( read_yrot1_.compare(saved_fyrot1_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fyrot1_ did not match the read_yrot1_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fyrot1_ = " << saved_fyrot1_.toStdString() << "     read_yrot1_ = " << read_yrot1_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fyrot1_ = " << saved_fyrot1_.toStdString() << "     read_yrot1_ = " << read_yrot1_.toStdString()  << std::endl;

    if( read_yrot2_.compare(saved_fyrot2_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fyrot2_ did not match the read_yrot2_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fyrot2_ = " << saved_fyrot2_.toStdString() << "     read_yrot2_ = " << read_yrot2_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fyrot2_ = " << saved_fyrot2_.toStdString() << "     read_yrot2_ = " << read_yrot2_.toStdString()  << std::endl;

    if( read_zrot0_.compare(saved_fzrot0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fzrot0_ did not match the read_zrot0_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fzrot0_ = " << saved_fzrot0_.toStdString() << "     read_zrot0_ = " << read_zrot0_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fzrot0_ = " << saved_fzrot0_.toStdString() << "     read_zrot0_ = " << read_zrot0_.toStdString()  << std::endl;

    if( read_zrot1_.compare(saved_fzrot1_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fzrot1_ did not match the read_zrot1_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fzrot1_ = " << saved_fzrot1_.toStdString() << "     read_zrot1_ = " << read_zrot1_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fzrot1_ = " << saved_fzrot1_.toStdString() << "     read_zrot1_ = " << read_zrot1_.toStdString()  << std::endl;

    if( read_zrot2_.compare(saved_fzrot2_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fzrot2_ did not match the read_zrot2_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fzrot2_ = " << saved_fzrot2_.toStdString() << "     read_zrot2_ = " << read_zrot2_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fzrot2_ = " << saved_fzrot2_.toStdString() << "     read_zrot2_ = " << read_zrot2_.toStdString()  << std::endl;

    if( read_xgain_.compare(saved_fxgain_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fxgain_ did not match the read_xgain_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fxgain_ = " << saved_fxgain_.toStdString() << "     read_xgain_ = " << read_xgain_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fxgain_ = " << saved_fxgain_.toStdString() << "     read_xgain_ = " << read_xgain_.toStdString()  << std::endl;

    if( read_ygain_.compare(saved_fygain_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fygain_ did not match the read_ygain_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fygain_ = " << saved_fygain_.toStdString() << "     read_ygain_ = " << read_ygain_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fygain_ = " << saved_fygain_.toStdString() << "     read_ygain_ = " << read_ygain_.toStdString()  << std::endl;

    if( read_gcoeff_.compare(saved_fgcoeff_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fgcoeff_ did not match the read_gcoeff_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fgcoeff_ = " << saved_fgcoeff_.toStdString() << "     read_gcoeff_ = " << read_gcoeff_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fgcoeff_ = " << saved_fgcoeff_.toStdString() << "     read_gcoeff_ = " << read_gcoeff_.toStdString()  << std::endl;

    if( read_checksum_.compare(saved_fchecksum_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fchecksum_ did not match the read_checksum_.";
        passVerification_ = false;
        std::cout << "FAILED   saved_fchecksum_ = " << saved_fchecksum_.toStdString() << "     read_checksum_ = " << read_checksum_.toStdString()  << std::endl;
    }
    else
        std::cout << "PASSED   saved_fchecksum_ = " << saved_fchecksum_.toStdString() << "     read_checksum_ = " << read_checksum_.toStdString()  << std::endl;

    return errMsg;
}

/////////////////////////////////////////// NEW CODE ////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<double> calibui::accValues() const
{
    // adxl312: 2^10 bits / +/- 1.5 g = 341.3 LSB/g
    // adxl355: 2^20 bits / +/- 2.0 g = 262144 LSB/g

    QString accel_res_ = "20";      // resolution - hardcoded for now, dont know if these can be user entered
    QString accel_sens_ = "2.0";    // sensativity - hardcoded for now, dont know if these can be user entered

    //const double resolution( pow( 2.0, (accel_res_.toDouble() ) / (2.0 * accel_sens_.toDouble())) );
    const double resolution( pow( 2.0, 20.0 ) / (2.0 * 2.0) );

    std::vector<double> acc_values;
    acc_values.push_back( accx_level_.toDouble() / resolution );
    acc_values.push_back( accx_front_.toDouble() / resolution );
    acc_values.push_back( accx_back_.toDouble() / resolution );
    acc_values.push_back( accx_left_.toDouble() / resolution );
    acc_values.push_back( accx_right_.toDouble() / resolution );

    acc_values.push_back( accy_level_.toDouble() / resolution );
    acc_values.push_back( accy_front_.toDouble() / resolution );
    acc_values.push_back( accy_back_.toDouble() / resolution );
    acc_values.push_back( accy_left_.toDouble() / resolution );
    acc_values.push_back( accy_right_.toDouble() / resolution );

    acc_values.push_back( accz_level_.toDouble() / resolution );
    acc_values.push_back( accz_front_.toDouble() / resolution );
    acc_values.push_back( accz_back_.toDouble() / resolution );
    acc_values.push_back( accz_left_.toDouble() / resolution );
    acc_values.push_back( accz_right_.toDouble() / resolution );

    return acc_values;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<double> calibui::weigherValues() const
{

    std::vector<double> counts;
    counts.push_back( counts_level_0kg_0lb_.toDouble()   );
    counts.push_back( counts_front_0kg_0lb_.toDouble()   );
    counts.push_back( counts_back_0kg_0lb_.toDouble()   );
    counts.push_back( counts_left_0kg_0lb_.toDouble()   );
    counts.push_back( counts_right_0kg_0lb_.toDouble()  );

    // 5kgs 10lbs
    counts.push_back( counts_level_5kg_10lb_.toDouble()  );         // 20pt
    counts.push_back( counts_front_5kg_10lb_.toDouble()  );
    counts.push_back( counts_back_5kg_10lb_.toDouble()  );
    counts.push_back( counts_left_5kg_10lb_.toDouble()  );          // 20pt
    counts.push_back( counts_right_5kg_10lb_.toDouble()  );         // 20pt

    // 10kgs 20lbs
    counts.push_back( counts_level_10kg_20lb_.toDouble() );         // 20pt
    counts.push_back( counts_front_10kg_20lb_.toDouble() );         // 20pt
    counts.push_back( counts_back_10kg_20lb_.toDouble() );          // 20pt
    counts.push_back( counts_left_10kg_20lb_.toDouble() );          // 20pt
    counts.push_back( counts_right_10kg_20lb_.toDouble() );        // 20pt

    // 15kgs 30lbs
    counts.push_back( counts_level_15kg_30lb_.toDouble() );
    counts.push_back( counts_front_15kg_30lb_.toDouble() );
    counts.push_back( counts_back_15kg_30lb_.toDouble() );
    counts.push_back( counts_left_15kg_30lb_.toDouble() );
    counts.push_back( counts_right_15kg_30lb_.toDouble() );

    return counts;
}
////////////////////////////////////////// END ////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::popErrorDialog(QString err_msg, bool popItNoMatterWhat)
{
    std::cout << "popErrorDialog() err_msg:\n" << err_msg.toStdString() << std::endl;

    // either one of these - or both - can be true depending on what I want to see


    if( popItNoMatterWhat || DISPLAY_SAVED_READ_FIT_PARAMS || DISPLAY_GENERATE_FIT_RESULTS || DISPLAY_WRITTEN_DATE)
    {
        warningError_ = new failure( this, err_msg );
        warningError_->move( 137, 77 );
        warningError_->show();
    }

    if( OUTPUT_SAVED_READ_FIT_PARAMS )
        std::cout << err_msg.toStdString() << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString calibui::formatScannedField(QString scanned_field, int format_len)
{

    // try this if leftJustified doesnt work
    //serialStr.append(  QString(" ").fill(16);

    if( scanned_field.size() == format_len )
        return scanned_field;

    QString formatted_string = "";

    if( scanned_field.size() > format_len )
    {
        formatted_string = scanned_field.mid(0, format_len);
    }
    else if( scanned_field.size() < format_len )
    {
        formatted_string = scanned_field.leftJustified(format_len, ' ');
    }

    return formatted_string;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool calibui::countsValid(int step){
    bool result = true;
    qDebug() << "Step: " << step;
    qDebug() << "X: " << X_LEVEL_MAX_COUNTS << " <= " << acclXRawCounts_ << " <= " << X_LEVEL_MIN_COUNTS;
    qDebug() << "Xn: " << X_NEGATIVE_TILTED_MAX_COUNTS << " <= " << acclXRawCounts_ << " <= " << X_NEGATIVE_TILTED_MIN_COUNTS;
    qDebug() << "Xp: " << X_POSITIVE_TILTED_MAX_COUNTS << " <= " << acclXRawCounts_ << " <= " << X_POSITIVE_TILTED_MAX_COUNTS;

    qDebug() << "Z: " << Z_LEVEL_MAX_COUNTS << " <= " << acclZRawCounts_ << " <= " << Z_LEVEL_MIN_COUNTS;
    qDebug() << "Zn: " << Z_NEGATIVE_TILTED_MAX_COUNTS << " <= " << acclZRawCounts_ << " <= " << Z_NEGATIVE_TILTED_MIN_COUNTS;
    qDebug() << "Zp: " << Z_POSITIVE_TILTED_MAX_COUNTS << " <= " << acclZRawCounts_ << " <= " << Z_POSITIVE_TILTED_MAX_COUNTS;
    switch (step){
        case FlatStep:
        case FlatStep30:
            result = (acclXRawCounts_ <= X_LEVEL_MAX_COUNTS && acclXRawCounts_ >= X_LEVEL_MIN_COUNTS &&
                      acclZRawCounts_ <= Z_LEVEL_MAX_COUNTS && acclZRawCounts_ >= Z_LEVEL_MIN_COUNTS);
            break;
        case FrontStep:
        case FrontStep10:
        case FrontStep30:
            result = (acclXRawCounts_ <= X_NEGATIVE_TILTED_MAX_COUNTS && acclXRawCounts_ >= X_NEGATIVE_TILTED_MIN_COUNTS &&
                      acclZRawCounts_ <= Z_LEVEL_MAX_COUNTS && acclZRawCounts_ >= Z_LEVEL_MIN_COUNTS);
            break;
        case RightStep:
        case RightStep30:
            result = (acclXRawCounts_ <= X_LEVEL_MAX_COUNTS && acclXRawCounts_ >= X_LEVEL_MIN_COUNTS &&
                      acclZRawCounts_ <= Z_POSITIVE_TILTED_MAX_COUNTS && acclZRawCounts_ >= Z_POSITIVE_TILTED_MIN_COUNTS);
            break;
        case BackStep:
        case BackStep10:
        case BackStep30:
            result = (acclXRawCounts_ <= X_POSITIVE_TILTED_MAX_COUNTS && acclXRawCounts_ >= X_POSITIVE_TILTED_MIN_COUNTS &&
                      acclZRawCounts_ <= Z_LEVEL_MAX_COUNTS && acclZRawCounts_ >= Z_LEVEL_MIN_COUNTS);
            break;
        case LeftStep:
        case LeftStep30:
            result = (acclXRawCounts_ <= X_LEVEL_MAX_COUNTS && acclXRawCounts_ >= X_LEVEL_MIN_COUNTS &&
                      acclZRawCounts_ <= Z_NEGATIVE_TILTED_MAX_COUNTS && acclZRawCounts_ >= Z_NEGATIVE_TILTED_MIN_COUNTS);
            break;
        default:
            //result should still be true and we dont care about the case so do nothing
            break;
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::highlightDataLabels(bool acc_x, bool acc_y, bool acc_z, bool counts){
    if(acc_x){
        ui->lbl_x->setStyleSheet("QLabel#lbl_x {"
        "background-color: orange;"
        "border-style:outset;"
        "border-radius:8px;"
        "border:3px solid gray;"
        "}");
    }else{
        ui->lbl_x->setStyleSheet("QLabel#lbl_x {"
        "background-image: url(:/images/none.png);"
        "border-style:outset;"
        "border-radius:8px;"
        "border:3px solid gray;"
        "}");
    }

    if(acc_y){
        ui->lbl_y->setStyleSheet("QLabel#lbl_y {"
        "background-color: orange;"
        "border-style:outset;"
        "border-radius:8px;"
        "border:3px solid gray;"
        "}");

    }else{
        ui->lbl_y->setStyleSheet("QLabel#lbl_y {"
        "background-image: url(:/images/none.png);"
        "border-style:outset;"
        "border-radius:8px;"
        "border:3px solid gray;"
        "}");
    }

    if(acc_z){
        ui->lbl_z->setStyleSheet("QLabel#lbl_z {"
        "background-color: orange;"
        "border-style:outset;"
        "border-radius:8px;"
        "border:3px solid gray;"
        "}");

    }else{
        ui->lbl_z->setStyleSheet("QLabel#lbl_z {"
        "background-image: url(:/images/none.png);"
        "border-style:outset;"
        "border-radius:8px;"
        "border:3px solid gray;"
        "}");
    }

    if(counts){
        ui->lbl_counts->setStyleSheet("QLabel#lbl_counts {"
        "background-color: orange;"
        "border-style:outset;"
        "border-radius:8px;"
        "border:3px solid gray;"
        "}");

    }else{
        ui->lbl_counts->setStyleSheet("QLabel#lbl_counts {"
        "background-image: url(:/images/none.png);"
        "border-style:outset;"
        "border-radius:8px;"
        "border:3px solid gray;"
        "}");
    }
}
