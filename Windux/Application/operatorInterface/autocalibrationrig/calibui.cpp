/**
 * @file calibui.cpp
 *
 * @copyright Copyright (c) 2020, ITW Hobart Food Equipment Group, U.S.A.
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

static const int MAX_STEPS = 26;
static const int MAX_READINGS = 12;

static const bool TESTING_SENSORS = false;
static const bool TESTING_VERIFY_SCANNED = false;        // pops scanned proof of concept dialog
static const bool DISPLAY_GENERATE_FIT_RESULTS = false;
static const bool OUTPUT_SAVED_READ_FIT_PARAMS = false;
static const bool DISPLAY_SAVED_READ_FIT_PARAMS = false;
static const bool DISPLAY_X_Y_Z_COUNTS = true;
static const bool DISPLAY_WRITTEN_DATE = false;


//These constants pop up the "Incorrect Loadcell orientation" error message
static const int X_POSITIVE_TILTED_MAX_COUNTS = 35000;
static const int X_POSITIVE_TILTED_MIN_COUNTS = 15000;

static const int X_LEVEL_MAX_COUNTS = 15000;
static const int X_LEVEL_MIN_COUNTS = -5000;

static const int X_NEGATIVE_TILTED_MAX_COUNTS = -20000;
static const int X_NEGATIVE_TILTED_MIN_COUNTS = -30000;

static const int Z_POSITIVE_TILTED_MAX_COUNTS = 30000;
static const int Z_POSITIVE_TILTED_MIN_COUNTS = 5000;

static const int Z_LEVEL_MAX_COUNTS = 5000;
static const int Z_LEVEL_MIN_COUNTS = -15000;

static const int Z_NEGATIVE_TILTED_MAX_COUNTS = -30000;
static const int Z_NEGATIVE_TILTED_MIN_COUNTS = -45000;

///////////////////////////////////////////////////////////////////////////////////////////////////

calibui::calibui(kpc *device, mtrd24MotorDrive *motor, QWidget *parent) : QWidget(parent),
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
      assemblyFrames_( 0 ),
      settledWeight_(false),
      sensorPath_( "" ),
      sensorFile_( "" ),
      stack_dir( home )
{
    ui->setupUi(this);

    device_ = device;
    motor_ = motor;

    if( device_ )
    {
        device_->weigherWakeup();                                   // REQ: 0x401 - Pulled from main.cpp
        device_->setWeigherMode( SCALE_MANUAL_MODE );               // REQ: 0x451,3 - manual
        device_->weigherEnable();      
        flags_ = 0;
    }

    initializeText();
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
    int pStep = step_;
    setNextCalibrationStep(pStep);                                  // refresh current photo

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool calibui::eventFilter( QObject *obj, QEvent *evt )
{
    if (  showPhoto_ == obj )
    {
        if( QEvent::MouseButtonRelease == evt->type() )
        {
            onLabelClicked();
        }
    }

    return false;
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
    //outfile_( "debuglog.dat");

    if ( 0/*TESTING_SENSORS*/ )
        sensorPath_ = "/home/hobart/Source/Hobart/branches/gpio/";
    else
        sensorPath_ = "/usr/local/gpio/";

    sensorFile_ = "value";
    
#if 0
    qDebug() <<  "Testing sensors - Carlos - \r\n";    
    i = setSensorDUTPowerEnable(0);
    qDebug() << i;
    i = getSensorCarriageHome();
    qDebug() << i;
    i = getSensorCarriagePulledOut();
    qDebug() << i;
    i = getSensorTableHorizontal();
    qDebug() << i;
    i = getSensorTableTilted();
    qDebug() << i;
    i = getSensorPositionOne(); 
    qDebug() << i;
    i = getSensorPositionTwo();
    qDebug() << i;
    i = getSensorPositionThree();
    qDebug() << i;
    i = getSensorPositionFour();
    qDebug() << i;
#endif   

    grav_const_ = 980023;                  // defaults to Hillsboro
    isHillsboroGravity_ = true;

    if ( QFile::exists( "/etc/hobart/gravity_troy.dat" ) )
    {
        grav_const_ = 980104;               // Troy
        isHillsboroGravity_ = false;
    }

    showPhoto_    = ui->lblShowHowTo;
    explainPhoto_ = ui->lblTellHowTo;
    nextPhoto_    = ui->butNextHowTo;

    showPhoto_->installEventFilter( this );

    photoButtonMsg_.push_back( tr( "START TEST") );
    photoLabelMsg_.push_back( tr( "Use static protection when calibrating the load cell.\nPress the START button to begin.") );

    // clear out existing photos - shouldnt be any
    stepPhotos_.clear();
    QString stepPhotoName;
    QPixmap stepPhoto;

    for (int j = 0; j <= MAX_STEPS; j++ )
    {
        stepPhotoName = ":/steps/step" + QString::number( j ) + ".png";
        stepPhoto = stepPhotoName;
        stepPhotos_.append( stepPhoto );
    }

    settledWeightsWithDroppedStack_ = new QTimer( this );
    settledWeightsWithDroppedStack_->setSingleShot( false );
    settledWeightsWithDroppedStack_->setInterval( 2000 );                    // does this need to be lowered to 250? 333? 100?

    connect( nextPhoto_,                       SIGNAL( clicked() ),         SLOT( onClicked() ) );
    connect( settledWeightsWithDroppedStack_,  SIGNAL( timeout() ),         SLOT( takeCurrentCountsReading() ) );

    initDUTTimer_ = new QTimer( this );
    initDUTTimer_->setSingleShot( true );
    initDUTTimer_->setInterval( 1500 );//1.5 Sec
    connect( initDUTTimer_,  SIGNAL( timeout() ),         SLOT( initDUTTimerExpired() ) );


#ifndef QT_DEBUG
    connect( device_, SIGNAL( updateWeigherStateChanged(weigherState, float) ), SLOT( onUpdateWeigherStateChanged(weigherState, float) ) );
    connect( device_, SIGNAL( updateValueMaxWeigherStatus(uint16_t, uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int16_t, int16_t, int16_t, int16_t) ), SLOT( onUpdateValueMaxWeigherStatus(uint16_t, uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int16_t, int16_t, int16_t, int16_t) ) );
    connect( device_, SIGNAL( updateValueMaxFitParameters(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) ),           SLOT( onUpdateValueMaxFitParameters(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) ) );       // this might have to be 25 individual args, or just some of them
    connect( device_, SIGNAL( updateValueMaxDate(uint32_t) ),                   SLOT( onUpdateValueMaxDate(uint32_t) ) );
    connect( device_, SIGNAL( updateValueMaxAssemblyNumber(QString) ),          SLOT( onUpdateValueMaxAssemblyNumber(QString) ) );
    connect( device_, SIGNAL( updateValueMaxHWVersion(QString) ),               SLOT( onUpdateValueMaxHWVersion(QString) ) );
    connect( device_, SIGNAL( updateValueMaxSerialNumber(QString) ),            SLOT( onUpdateValueMaxSerialNumber(QString) ) );
#endif


    connect(&verifyStepIsVerified_, SIGNAL( timeout() ),                        SLOT( moveOnWithoutVerification() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//void calibui::updateLoadCellData(int32_t acc_x, int32_t acc_y, int32_t acc_z, uint32_t counts)
void calibui::updateLoadCellData(int32_t acc_x, int32_t acc_y, int32_t acc_z, uint32_t counts, double_t rig_tilt_x, double_t rig_tilt_y)
{
    ui->lbl_x->setText("X: " + QString::number(acc_x));
    ui->lbl_y->setText("Y: " + QString::number(acc_y));
    ui->lbl_z->setText("Z: " + QString::number(acc_z));
    ui->lbl_counts->setText("Load Cell: " + QString::number(counts));
    ui->lbl_rig_tilt->setText("Rig Tilt (X,Y):\t" + QString::number(rig_tilt_x) + ",\t" + QString::number(rig_tilt_y));

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::initializeText()
{
    units_ = "kg";
    unitsSize_.clear();
    unitsSize_.push_back( "0" );
    unitsSize_.push_back( "5" );
    unitsSize_.push_back( "10" );           // not used
    unitsSize_.push_back( "15" );

    photoButtonMsg_.clear();                                                                                        // clear
    photoButtonMsg_.push_back( tr( "START TEST") );                                                                 // 0
    photoButtonMsg_.push_back( tr( "All fields are scanned") );                                                     // 1
    photoButtonMsg_.push_back( tr( "Loadcell is attached") );                                       // 2
    photoButtonMsg_.push_back( tr( "Accelerometer cable is connected") );                                           // 3
    photoButtonMsg_.push_back( tr( "Loadcell cable is connected") );                                               // 4
    photoButtonMsg_.push_back( tr( "Top Plate is attached") );                                          // 5
    photoButtonMsg_.push_back( tr( "Position 1 is set") );                                                          // 6
    photoButtonMsg_.push_back( tr( "Top Wedge Plate is HORIZONTAL") );                                              // 7
    photoButtonMsg_.push_back( tr( "Sliding Carriage is returned HOME") );                                          // 8 NEW STEP
    photoButtonMsg_.push_back( tr( "POWER ON") );                                                                   // 9
    photoButtonMsg_.push_back( tr( "Start Level Readings") );                                                  // 10
    photoButtonMsg_.push_back( tr( "Lower Stack and take LEVEL %3 %4 Reading").arg(unitsSize_[3], units_) );        // 11
    photoButtonMsg_.push_back( tr( "The Bottom Plate has been tilted up") );                                        // 12 NEW STEP
    photoButtonMsg_.push_back( tr( "Start Position 1 Tilt Readings") );                                                // 13
    photoButtonMsg_.push_back( tr( "Lower %1 %2 stack and take Back Tilt %3 %4 Reading").arg(unitsSize_[1], units_, unitsSize_[1], units_) );   // 14
    photoButtonMsg_.push_back( tr( "Lower %1 %2 stack and take Back Tilt %3 %4 Reading").arg(unitsSize_[2], units_, unitsSize_[3], units_) );   // 15
    photoButtonMsg_.push_back( tr( "Start Position 2 Readings") );                                                                           // 16
    photoButtonMsg_.push_back( tr( "Lower %1 %2 stack and take Left Tilt %3 %4 Reading").arg(unitsSize_[3], units_, unitsSize_[3], units_) );   // 17
    photoButtonMsg_.push_back( tr( "Start Position 3 Readings") );                                                                           // 18
    photoButtonMsg_.push_back( tr( "Lower %1 %2 stack and take Front Tilt %3 %4 Reading").arg(unitsSize_[1], units_, unitsSize_[1], units_) );  // 19
    photoButtonMsg_.push_back( tr( "Lower %1 %2 stack and take Front Tilt %3 %4 Reading").arg(unitsSize_[2], units_, unitsSize_[3], units_) );  // 20
    photoButtonMsg_.push_back( tr( "Start Position 4 Readings") );                                                                     // 21
    photoButtonMsg_.push_back( tr( "Lower %1 %2 stack and take Right Tilt %3 %4 Reading").arg(unitsSize_[3], units_, unitsSize_[3], units_) );  // 22
    photoButtonMsg_.push_back( tr( "Calculate and Write Calibration") );                                                                        // 23
    photoButtonMsg_.push_back( tr( "Verify Calibration") );                                                                                     // 24
    photoButtonMsg_.push_back( tr( "Reset Everything") );                                                                                       // 25
    photoButtonMsg_.push_back( tr( "FINISH TEST") );                                                                                            // 26

    photoLabelMsg_.clear();                                                                                                                                                                 // clear
    photoLabelMsg_.push_back( tr( "Press the START TEST button to begin Loadcell Calibration") );                                                                                      // 0
    photoLabelMsg_.push_back( tr( "Scan all barcode fields into their proper location.\nClick All fields are scanned when entered.") );                                                     // 1
    photoLabelMsg_.push_back( tr( "Attach Loadcell to Base Plate. Do not tighten over 78 in lb.\nClick Loadcell is attached.") );                   // 2
    photoLabelMsg_.push_back( tr( "Connect Accelerometer cable. \nClick Accelerometer cable is connected.") );                             // 3
    photoLabelMsg_.push_back( tr( "Connect Load Cell. Route cable around Load Cell without binding the cable.\nClick Load Cell cable is connected.") );                                     // 4
    photoLabelMsg_.push_back( tr( "Attach Top Plate to Loadcell. Do not tighten over 78 in lb.\nClick Top plate is attached.") );                                       // 5
    
    photoLabelMsg_.push_back( tr( "Set Base Plate to Position 1 and Top Platter with flat side up and arrow pointing as shown.\nClick Position 1 is set.") );// 6
    photoLabelMsg_.push_back( tr( "Flip the Top Wedge Plate so the plate is HORIZONTAL and the wedge side is down.\nThe white HOME block is facing the Operator and the locator pin is set.\nClick Top Wedge Plate is HORIZONTAL.") );                               // 7
    photoLabelMsg_.push_back( tr( "Push the Sliding Carriage in to the HOME Position.\nClick Sliding Carriage is returned HOME.") );                                                        // 8 NEW STEP
    photoLabelMsg_.push_back( tr( "Apply Power to the Load Cell by pressing the POWER button.\nClick POWER ON.") );                                                                       // 9
    photoLabelMsg_.push_back( tr( "Take level readings.\nClick Start Level Readings.") );                                                           // 10
    photoLabelMsg_.push_back( tr( "With the Top Wedge Plate still wedge side down, lower %1 %2 onto the platter.\nClick Lower stack and take LEVEL %3 %4 Reading").arg(unitsSize_[3], units_, unitsSize_[3], units_) );  // 11
    //photoLabelMsg_.push_back( tr( "Push the handle on the right hand side up to tilt the bottom sliding plate up 5°.\n Click the Bottom Plate has been tilted up.") );                      // 12 NEW STEP
    photoLabelMsg_.push_back( tr( "Lift the handle on the right to tilt the Base Plate.\n ") );                      // 12 NEW STEP
    photoLabelMsg_.push_back( tr( "Flip Top Platter so wedge side is up and arrow pointing as shown.\nClick Start Position 1 Tilt Readings.") );                                      // 13
    photoLabelMsg_.push_back( tr( "Leave all plates in their current locations.\nClick Lower %1 %2 stack and take Back Tilt %3 %4 Reading.").arg(unitsSize_[1], units_, unitsSize_[1], units_) );  // 14
    photoLabelMsg_.push_back( tr( "Leave all plates in their current locations.\nClick Lower %1 %2 stack and take Back Tilt %3 %4 Reading.").arg(unitsSize_[2], units_, unitsSize_[3], units_) );  // 15
    photoLabelMsg_.push_back( tr( "Set Base Plate to Position 2 and rotate Top Platter with arrow pointing as shown.\nClick Start Position 2 Readings.") );  // 16
    photoLabelMsg_.push_back( tr( "Leave all plates in their current locations.\nClick Lower %1 %2 stack and take Left Tilt %3 %4 Reading.").arg(unitsSize_[3], units_, unitsSize_[3], units_) );   // 17
    photoLabelMsg_.push_back( tr( "Set Base Plate to Position 3 and rotate Top Platter so arrow is pointing as shown.\nClick Start Position 3 Readings.") ); // 18
    photoLabelMsg_.push_back( tr( "Leave all plates in their current locations.\nClick Lower %1 %2 stack and take Front Tilt %3 %4 Reading.").arg(unitsSize_[1], units_, unitsSize_[1], units_) );  // 19
    photoLabelMsg_.push_back( tr( "Leave all plates in their current locations.\nClick Lower %1 %2 stack and take Front Tilt %3 %4 Reading.").arg(unitsSize_[2], units_, unitsSize_[3], units_) );  // 20
    photoLabelMsg_.push_back( tr( "Set Base Plate to Position 4 and rotate Top Platter so arrow is pointing as shown.\nClick Start Position 4 Readings.") ); // 21
    photoLabelMsg_.push_back( tr( "Leave all plates in their current location.\nClick Lower %1 %2 stack and take Right Tilt %3 %4 Reading.").arg(unitsSize_[3], units_, unitsSize_[3], units_) );  // 22
    photoLabelMsg_.push_back( tr( "Press Calculate to write the calibration to Accelerometer.\nClick Calculate and Write Calibration.") );                                                         // 23
    photoLabelMsg_.push_back( tr( "Verify the Calibration procedure worked.\nClick Verify Calibration.") );                                                                                        // 24
    photoLabelMsg_.push_back( tr( "The test is complete.\nClick Reset Everything to start over.") );                                                                                                             // 25
    photoLabelMsg_.push_back( tr( "Follow all of the Instructions to finish the up test.\nClick FINISH TEST.") );                                                                                  // 26

    sensorNames_.clear();                                                                                                                                                                 // clear
    sensorNames_.push_back( tr( "Base Plate Position 1") );  // 0
    sensorNames_.push_back( tr( "Base Plate Position 2") );  // 1
    sensorNames_.push_back( tr( "Base Plate Position 3") );// 2
    sensorNames_.push_back( tr( "Base Plate Positoin 4") ); // 3
    sensorNames_.push_back( tr( "Base Plate Level Position") );    // 4
    sensorNames_.push_back( tr( "Base Plate Tilt Position") );          // 5
    sensorNames_.push_back( tr( "Carriage Home Position") ); // 6
    sensorNames_.push_back( tr( "Carriage Pulled Out Position") );   // 7
    
    
    //onClicked();        // continue on with the starting the test
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::initializeTestParameters()
{
    // Turn on now just to test it
    ui->lblShowHowTo->setStyleSheet("QLabel#lblShowHowTo {"
        "border-style:outset;"
        "border-radius:13px;"
        "border:8px solid gray;"
        "}");

    receivedFitParams_ = false;                                     // reset if we have passed verification of Gain written. Assume yes unless not received
    passVerification_ = false;                               // reset pass or fail condition
    timeout_error_ = false;
    freezeReads_ = false;
    settledWeight_ = false;

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

    counts_level_0kg_   = "0.0";         // Counts 0kg   LEVEL
    counts_level_5kg_  = "0.0";         // Counts 5kg   LEVEL (not used)
    counts_level_10kg_ = "0.0";         // Counts 10kg  LEVEL (not used)
    counts_level_15kg_ = "0.0";         // Counts 15kg  LEVEL

    counts_front_0kg_   = "0.0";         // Counts 0kg   FRONT TILT
    counts_front_5kg_  = "0.0";         // Counts 5kg   FRONT TILT
    counts_front_10kg_ = "0.0";         // Counts 10kg  FRONT TILT (not used)
    counts_front_15kg_ = "0.0";         // Counts 15kg  FRONT TILT

    counts_back_0kg_    = "0.0";         // Counts 0kg   BACK TILT
    counts_back_5kg_   = "0.0";         // Counts 5kg   BACK TILT
    counts_back_10kg_  = "0.0";         // Counts 10kg  BACK TILT (not used)
    counts_back_15kg_  = "0.0";         // Counts 15kg  BACK TILT

    counts_left_0kg_    = "0.0";         // Counts 0kg   LEFT TILT
    counts_left_5kg_   = "0.0";         // Counts 5kg   LEFT TILT (not used)
    counts_left_10kg_  = "0.0";         // Counts 10kg  LEFT TILT (not used)
    counts_left_15kg_  = "0.0";         // Counts 15kg  LEFT TILT

    counts_right_0kg_   = "0.0";         // Counts 0kg   RIGHT TILT
    counts_right_5kg_  = "0.0";         // Counts 5kg   RIGHT TILT (not used)
    counts_right_10kg_ = "0.0";         // Counts 10kg  RIGHT TILT (not used)
    counts_right_15kg_ = "0.0";         // Counts 15kg  RIGHT TILT

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

    qDebug() <<  "readCurrentCalibrationSettings(): current_step: " << current_step;
    int result = 0;
    SENSOR_TEST_RESULT sensor_result;
    //sensorsPassed_ = true;
    settledWeight_ = false;

    switch( current_step )
    {
		case SetupStep:
        {
            initializeTestParameters();                                             // reset everything

            freezeReads_ = false;
            countmsgs_ = 0;

            // Need to determine the date for future use
            QDateTime currentDateTime = QDateTime::currentDateTime();
            QString str = currentDateTime.toString();                               // EX: Fri Mar 2 13:04:05 2018
            QString now = currentDateTime.toString( "MM/dd/yyyy " );                // format the date like this
            curTimeSecs_  = currentDateTime.toTime_t();                             // EX 1519995845 - this is in GMT FROM 01-01-1970

            qDebug() << "generated time currentDateTime: " << str;
            qDebug()  << "formatted time: " + now;       // EX: 03/02/2018
            qDebug()  << "epoch seconds: " + QString::number(curTimeSecs_);

            formattedDate_ = now;                                                   // EX: 03/02/2018 - used to display in date field on screen
            settledWeight_ = true;                                                  // to move to next step

            if(motor_)
            {
                ui->lblError->setText(tr("Homing Weight Stack. Please wait..."));
                explainPhoto_->setText("");

                // check to see if this is 1st time through
                if(motor_->isMtrd24HomePositionDetermined() == true)
                {
                    result = motor_->Mtrd24ReturnHome();
                }
                else
                {
                    result = motor_->Mtrd24FindHome();
                }

                ui->lblError->setText(tr(""));

                if( result > 0 )
                {
                    QString msg( "Motor stack failed to home: " + QString::number( result) + "\nCycle all power and start the tool again." );
                    popErrorDialog(msg, true);
                }
            }
            else
            {
                QString msg( "Motor could not be found to be Powered On.\nCycle all power and start the tool again." );
                popErrorDialog( msg, true );  
            }            

            break;
        }

        case ScanStep:
        {
            qDebug() << "readCurrentCalibrationSettings(): ScanStep does nothing";
            settledWeight_ = true;                                                  // to move to next step

            break;
        }

        case AttachLoadCellStep:
        {
            qDebug() << "readCurrentCalibrationSettings(): Attach Load Cell Step does nothing";
            settledWeight_ = true;                                                  // to move to next step

            break;
        }

        case AttachTopPlateStep:
        {
            qDebug() << "readCurrentCalibrationSettings(): Attach Top Plate Step does nothing";
            settledWeight_ = true;                                                  // to move to next step

            break;
        }

        case ConnectWire1Step:
        {
            qDebug() << "readCurrentCalibrationSettings(): Connect Wire #2 Step does nothing";
            settledWeight_ = true;                                                  // to move to next step

            break;
        }

        case ConnectWire2Step:
        {
            qDebug() << "readCurrentCalibrationSettings(): Connect Wire #2 Step does nothing";
            settledWeight_ = true;                                                  // to move to next step

            break;
        }
#if 0
        case ResetBottomPlateStep:
        {
            qDebug() << "readCurrentCalibrationSettings(): Reset Bottom Plate all the way counterclockwise does nothing";
            settledWeight_ = true;                                                  // to move to next step

            break;
        }
#endif
        case ResetBottomPlateStep:
        case ResetTopWedgePlateStep:
        {
            bool is_rig_level = true;

            qDebug() << "readCurrentCalibrationSettings(): Reset Top Wedge Plate Step does nothing";
            sensorsPassed_ = testSensors( SENSOR_START_TEST );
            
             /* verify that the Cal rig is leveled */ 
            if(calRigAcclXTiltDegrees_ > 2 || calRigAcclYTiltDegrees_ > 2   ||
               calRigAcclXTiltDegrees_ < -2 || calRigAcclYTiltDegrees_ < -2 )
            {
                is_rig_level = false;
            }

            if( sensorsPassed_.result == true  && is_rig_level == true)
            {
                if(step_ == ResetBottomPlateStep)
                {
                    //skip next step. It's not necessary 
                    step_ ++;
                }
                settledWeight_ = true; // to move to next step
            } 
            else
            {
                QString msg;

                if(sensorsPassed_.result == false)
                {
                    msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                    
                    if(sensorsPassed_.value == 1)
                        msg += " is set\n";
                    else
                        msg += " is not set\n";
                    
                    msg += "\nCorrect issue and try again.";
                }
                else
                {
                    msg = "Calibration Rig is out of level\n\n X, Y = " + QString::number(calRigAcclXTiltDegrees_) + ",\t" + QString::number(calRigAcclYTiltDegrees_);

                }        

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
            }

            break;
        }

        case PushInSlidingCarriageStep:
        {
            qDebug() << "readCurrentCalibrationSettings(): NEW STEP - push the sliding carriage all of the way in";
            sensorsPassed_ = testSensors( SENSOR_PUSHEDIN_TEST );

            if(sensorsPassed_.result == true)
            {
                settledWeight_          = true;                                                  // to move to next step
                initDUTTimerExpired_    = false;//used in next step
            }
            else
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";
                
                popErrorDialog(msg, true);
                setEnabledButtonState(true);
            }
               
            break;
        }

        case PowerOnStep:
        {
            qDebug() << "readCurrentCalibrationSettings(): Begin Step - Power On Step";

            setEnabledButtonState(false);

            //is this the 1st time through? did we start init timer yet
            if(initDUTTimerExpired_ == false)
            {
                qDebug() << "readCurrentCalibrationSettings(): Start DUT Timer";
                
                ui->lblError->setText(tr("Detecting Loadcell..."));
                explainPhoto_->setText("");
                nextPhoto_->setText("");
                
                //clear flags and accl data
                flags_          = 0;
                acclXRawCounts_ = 0;
                acclYRawCounts_ = 0;
                acclZRawCounts_ = 0;

                // Apply power to DUT Accelerometer
                setSensorDUTPowerEnable( 1 ); 

                // Tell Weigher peripheral to Init DUT Accelerometer
                device_->setWeigherMode( SCALE_INITIALIZE_CAL_RIG_ACCL );          // REQ: 0x451,4

                // Start 1 sec timer to allow DUT initialization to complete before advancing to next step
                initDUTTimer_->start();
            }
            else
            {
                qDebug() << "readCurrentCalibrationSettings(): DUT Timer expired";
                
                // 1. Cammand motor to home Weight Stack,
                // 2. Ensure DUT is ON and start tracking mode
                if(startDeviceUnderTest() > 0)//error
                {
                    settledWeight_ = false;

                    /*allow user to try again */
                    setSensorDUTPowerEnable( 0 );
                    initDUTTimerExpired_    = false;
                    setEnabledButtonState(false);

                    ui->lblError->setText(tr(""));
                    explainPhoto_->setText("");
                    nextPhoto_->setText("");
                }
                else
                {
                    settledWeight_          = true;
                    initDUTTimerExpired_    = false;

                }    
            }            
            break;
        }

        case Flat0kgStep:
        {
            qDebug() << "Flat0kgStep(): Request date if turned on";

            // TEST the written date - which also requests all other current config values
            if( DISPLAY_WRITTEN_DATE )
            {
                qDebug() << "(): Date testing was turned on";
                if( device_ )
                    device_->requestDate();
            }

            sensorsPassed_ = testSensors( SENSOR_LEVEL_POSTITION1_TEST );

            if( sensorsPassed_.result == true )
            {
                allowWeightsToSettle();   // start timer to see if the weights have settled so we can save the current counts and move to next step
            } 
            else
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
            }            

            break;
        }

        case Flat15kgStep:
        {
            int result = 1;
            sensorsPassed_ = testSensors( SENSOR_LEVEL_POSTITION1_TEST );

            if(sensorsPassed_.result == false)
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
                break;
            }

            if( device_ && sensorsPassed_.result == true)
            {
                if( motor_ )
                {
                    ui->lblError->setText(tr("Lowering Weight Stack. Please wait..."));

                    result = motor_->Mtrd24Drop15KgAtLevel(); // going 0kg->15kg

                    if( result > 0 )
                    {
                        QString msg = "Lowering 15 kg with the motor stack failed: " + QString::number( result ) + "\nCycle all power and start the test again.";
                        popErrorDialog(msg);
                        setEnabledButtonState(true);
                        break;
                    }

                    stack_dir = lowered;
                }
                else
                {
                    QString msg( "Motor could not be found and was not initialized.""\nCycle all power and start the tool again." );
                    popErrorDialog( msg );
                    setEnabledButtonState(true);
                    break;
                }
            }

            if(sensorsPassed_.result == true && result == 0)
            {
                allowWeightsToSettle();   // start timer to see if the weights have settled so we can save the current counts and move to next step
            }
                

            if( TESTING_SENSORS )     // TODO - remove this later
            {
                settledWeight_ = true;    // used to move on to the next screen after a save
                takeCurrentCountsReading();
            }

            break;
        }

        case TiltBottomPlateStep:
        {
            qDebug() << "(): Tilt the bottom plate";

            sensorsPassed_ = testSensors( SENSOR_TABLE_TILTED );
            
            if( sensorsPassed_.result == true )
            {
                settledWeight_ = true; // to move to next step
            } 
            else
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
            }

            break;
        }

        case Back0kgStep:
        {
            sensorsPassed_ = testSensors( SENSOR_TILTED_POSTITION1_TEST );

            if( sensorsPassed_.result == true )
            {
                allowWeightsToSettle();   // start timer to see if the weights have settled so we can save the current counts and move to next step
            } 
            else
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
            }                  

            break;
        }

        case Back5kgStep:
        {
            int result = 1;
            sensorsPassed_ = testSensors( SENSOR_TILTED_POSTITION1_TEST );

            if(sensorsPassed_.result == false)
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
                break;
            }

            if( device_ && sensorsPassed_.result == true )
            {
                stack_dir = lowered;
                if( motor_ )
                {
                    ui->lblError->setText(tr("Lowering Weight Stack. Please wait..."));

                    result = motor_->Mtrd24DropFirst5KgAtTilt();                      // Drop 0->5 kg on to the platter

                    if( result > 0 )
                    {
                        QString msg( "Lowering 5 kg with the motor stack failed: " + QString::number( result) + "\nCycle all power and start the Auto Rig again." );
                        popErrorDialog(msg);
                        setEnabledButtonState(true);
                        break;
                    }
                }
                else
                {
                    QString msg( "Motor could not be found and was not initialized.""\nCycle all power and start the tool again." );
                    popErrorDialog( msg );
                    setEnabledButtonState(true);
                    break;
                }
            }

            if(sensorsPassed_.result == true && result == 0)
            {
                allowWeightsToSettle();                                                 // start timer to see if the weights have settled so we can save the current counts and move to next step
            }
            
            if( TESTING_SENSORS )                                                   // TODO - remove this later
            {
                settledWeight_ = true;
                takeCurrentCountsReading();
            }

            break;
        }

        case Back15kgStep:
        {
            int result = 1;
            sensorsPassed_ = testSensors( SENSOR_TILTED_POSTITION1_TEST );

            if(sensorsPassed_.result == false)
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
                break;
            }

            if( device_ && sensorsPassed_.result == true )
            {
                stack_dir = lowered;
                if( motor_ )
                {
                    ui->lblError->setText(tr("Lowering Weight Stack. Please wait...")); 

                    result = motor_->Mtrd24DropSecondAndThird5Kg();             // Drop 5->15 kg on to the platter

                    if( result > 0 )
                    {
                        QString msg( "Lowering 10 kg with the motor stack failed: " + QString::number( result) + "\nCycle all power and start the Auto Rig again." );
                        popErrorDialog(msg);
                        setEnabledButtonState(true);
                        break;
                    }
                }
                else
                {
                    QString msg( "Motor could not be found and was not initialized.""\nCycle all power and start the tool again." );
                    popErrorDialog( msg );
                    setEnabledButtonState(true);
                    break;
                }
            }

            if(sensorsPassed_.result == true && result == 0)
            {
                allowWeightsToSettle();                                                 // start timer to see if the weights have settled so we can save the current counts and move to next step
            }
            
            if( TESTING_SENSORS )                                                   // TODO - remove this later
            {
                settledWeight_ = true;
                takeCurrentCountsReading();
            }

            break;
        }

        case Left0kgStep:
        {
            sensorsPassed_ = testSensors( SENSOR_TILTED_POSTITION2_TEST );

            if( sensorsPassed_.result == true )
            {
                allowWeightsToSettle();   // start timer to see if the weights have settled so we can save the current counts and move to next step
            } 
            else
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
            }

            break;
        }

        case Left15kgStep:
        {
            int result = 1;
            sensorsPassed_ = testSensors( SENSOR_TILTED_POSTITION2_TEST );

            if(sensorsPassed_.result == false)
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
                break;
            }

            if( device_ && sensorsPassed_.result == true )
            {
                stack_dir = lowered;

                if( motor_ )
                {
                    ui->lblError->setText(tr("Lowering Weight Stack. Please wait..."));

                    result = motor_->Mtrd24Drop15KgAtTilt();                          // Drop 0->15 kg on to the platter

                    if( result > 0 )
                    {
                        QString msg( "Lowering 15 kg with the motor stack failed: " + QString::number( result) + "\nCycle all power and start the Auto Rig again." );
                        popErrorDialog(msg);
                        setEnabledButtonState(true);
                        break;
                    }
                }
                else
                {
                    QString msg( "Motor could not be found and was not initialized.""\nCycle all power and start the tool again." );
                    popErrorDialog( msg );
                    setEnabledButtonState(true);
                    break;
                }
            }

            if(sensorsPassed_.result == true && result == 0)
            {
                allowWeightsToSettle();                                                 // start timer to see if the weights have settled so we can save the current counts and move to next step
            }

            if( TESTING_SENSORS )                                                   // TODO - remove this later
            {
                settledWeight_ = true;
                takeCurrentCountsReading();
            }

            break;
        }

        case Front0kgStep:
        {
            sensorsPassed_ = testSensors( SENSOR_TILTED_POSTITION3_TEST );

            if( sensorsPassed_.result == true )
            {
                allowWeightsToSettle();   // start timer to see if the weights have settled so we can save the current counts and move to next step
            } 
            else
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
            }

            break;
        }

        case Front5kgStep:
        {
            int result = 1;
            sensorsPassed_ = testSensors( SENSOR_TILTED_POSTITION3_TEST );

            if(sensorsPassed_.result == false)
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
                break;
            }

            if( device_ && sensorsPassed_.result == true )
            {
                stack_dir = lowered;
                if( motor_ )
                {
                    ui->lblError->setText(tr("Lowering Weight Stack. Please wait..."));

                    result = motor_->Mtrd24DropFirst5KgAtTilt();                      // Drop 0->5 kg on to the platter

                    if( result > 0 )
                    {
                        QString msg( "Lowering 5 kg with the motor stack failed: " + QString::number( result) + "\nCycle all power and start the Auto Rig again." );
                        popErrorDialog(msg);
                        setEnabledButtonState(true);
                        break;
                    }
                }
                else
                {
                    QString msg( "Motor could not be found and was not initialized.""\nCycle all power and start the tool again." );
                    popErrorDialog( msg );
                    setEnabledButtonState(true);
                    break;
                }
            }

            if(sensorsPassed_.result == true && result == 0)
            {
                allowWeightsToSettle();                                                 // start timer to see if the weights have settled so we can save the current counts and move to next step
            }

            if( TESTING_SENSORS )                                                   // TODO - remove this later
            {
                settledWeight_ = true;
                takeCurrentCountsReading();
            }
        
            break;
        }

        case Front15kgStep:
        {
            int result = 1;
            sensorsPassed_ = testSensors( SENSOR_TILTED_POSTITION3_TEST );

            if(sensorsPassed_.result == false)
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
                break;
            }

            if( device_ && sensorsPassed_.result == true )
            {
                stack_dir = lowered;
                if( motor_ )
                {
                    ui->lblError->setText(tr("Lowering Weight Stack. Please wait..."));

                    result = motor_->Mtrd24DropSecondAndThird5Kg();             // Drop 5->15 kg on to the platter

                    if( result > 0 )
                    {
                        QString msg( "Lowering 10 kg with the motor stack failed: " + QString::number( result) + "\nCycle all power and start the Auto Rig again." );
                        popErrorDialog(msg);
                        setEnabledButtonState(true);
                        break;
                    }
                }
                else
                {
                    QString msg( "Motor could not be found and was not initialized.""\nCycle all power and start the tool again." );
                    popErrorDialog( msg );
                    setEnabledButtonState(true);
                    break;
                }
            }

            if(sensorsPassed_.result == true && result == 0)
            {
                allowWeightsToSettle();                                                 // start timer to see if the weights have settled so we can save the current counts and move to next step
            }

            if( TESTING_SENSORS )                                                   // TODO - remove this later
            {
                settledWeight_ = true;
                takeCurrentCountsReading();
            }

            break;
        }

        case Right0kgStep:
        {
            sensorsPassed_ = testSensors( SENSOR_TILTED_POSTITION4_TEST );

            if( sensorsPassed_.result == true )
            {
                allowWeightsToSettle();   // start timer to see if the weights have settled so we can save the current counts and move to next step
            }
            else
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
            }

            break;
        }

        case Right15kgStep:
        {
            int result = 1;
            sensorsPassed_ = testSensors( SENSOR_TILTED_POSTITION4_TEST );

            if(sensorsPassed_.result == false)
            {
                QString msg = "Wrong sensor reading:\t" + sensorNames_[(int)sensorsPassed_.sensor];
                
                if(sensorsPassed_.value == 1)
                    msg += " is set\n";
                else
                    msg += " is not set\n";
                
                msg += "\nCorrect issue and try again.";

                popErrorDialog(msg, true);
                setEnabledButtonState(true);
                break;
            }

            if( device_ && sensorsPassed_.result == true )
            {
                stack_dir = lowered;

                if( motor_ )
                {
                    ui->lblError->setText(tr("Lowering Weight Stack. Please wait..."));

                    result = motor_->Mtrd24Drop15KgAtTilt();                          // Drop 0->15 kg on to the platter

                    if( result > 0 )
                    {
                        QString msg( "Lowering 15 kg with the motor stack failed: " + QString::number( result) + "\nCycle all power and start the Auto Rig again." );
                        popErrorDialog(msg);
                        setEnabledButtonState(true);
                        break;
                    }
                }
                else
                {
                    QString msg( "Motor could not be found and was not initialized.""\nCycle all power and start the tool again." );
                    popErrorDialog( msg );
                    setEnabledButtonState(true);
                    break;
                }
            }

            if(sensorsPassed_.result == true && result == 0)
            {
                allowWeightsToSettle();                                                 // start timer to see if the weights have settled so we can save the current counts and move to next step
            }

            if( TESTING_SENSORS )                                                   // TODO - remove this later
            {
                settledWeight_ = true;
                takeCurrentCountsReading();
            }

            break;
        }

        case WriteStep:
        {
            grav_const_ = 980023;                  // defaults to Hillsboro
            isHillsboroGravity_ = true;

            if ( QFile::exists( "/etc/hobart/gravity_troy.dat" ) )
            {
                grav_const_ = 980104;               // Troy
                isHillsboroGravity_ = false;
            }

            settledWeight_ = true;                                                  // to move to next step
            writeCalibrationToFlash();                                              // 7 write the calibration to the chip

            break;
        }
        case VerifyStep:
        {
            verifyReadings_ = true;                                                 // set true when we want to read the values FROM the board
            settledWeight_ = true;                                                  // to move to next step

            if( device_ )
                device_->requestDate();

            // Now we are waiting for a reply to verify the write process step.
            // If we never receive a reply, we are locked up here.
            // I added a timeout to handle the case where we never receive back a reply to verify

            verifyStepIsVerified_.setSingleShot( true );
            if( TESTING_SENSORS)
                verifyStepIsVerified_.start( 10000 );
            else
                verifyStepIsVerified_.start( 2000 );

            break;
        }
        case PassFailStep:
        {   
#if 1                     
            settledWeight_ = true;   // to move to next step
         
            endDeviceUnderTest();
            
            //skip over to StartOver Step. no need to display dismanteling intrunctions
            step_ = CompleteStep;
#else//bypass
            writeCalibrationToFlash();
            settledWeight_ = false;     
            setEnabledButtonState(true);                                             // to move to next step
#endif            
            
            break;
        }
        case CompleteStep:
        {            
            settledWeight_ = true;                                                  // to move to next step
            break;
        }
        case StartOver:
        {            
            settledWeight_ = true;                                                  // to move to next step
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::setNextCalibrationStep(int current_step)
{
    // When calling this method you are telling the UI where to go, and this case makes you GO to that NEXT step.

    qDebug() << "calibui::setNextCalibrationStep() START: current_step:" << current_step;
    
    //clear status field
    ui->lblError->setText(tr(""));

    switch( current_step )
    {
        case SetupStep:
        {
            // Displays the Initial/Default/Resting State so go to it. Indicates START of Calibration when CLICKED
            if( !isHillsboroGravity_ )
                ui->lblError->setText( tr("Using Troy gravity constant") );
            else
                ui->lblError->setText( tr("Using Hillsboro gravity constant") );

            ui->wdgtStackedWidgetMain->setCurrentIndex( 0 );
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case ScanStep:
        {
            // Displays the Scan Barcodes State so go to it. Click lock in load cell when done
            ui->wdgtStackedWidgetMain->setCurrentIndex( 1 );
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

        case AttachLoadCellStep:
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

        case AttachTopPlateStep:
        {
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case ConnectWire1Step:
        {
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case ConnectWire2Step:
        {
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case ResetBottomPlateStep:
        {
            // Rotate the bottom plate all of the way counterclockwise to return it to the HOME position
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case ResetTopWedgePlateStep:
        {
            // Flip the top wedge plate so it's shiny flat side up and the white wedge side down
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case PushInSlidingCarriageStep:
        {
            // display to the user the pushed in sliding carriage
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case PowerOnStep:
        {
            // start everything up
            //startDeviceUnderTest();

            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            setEnabledButtonState(false);
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case Flat0kgStep:
        {
            writeScannedFields();                           // Now that we are powered up - send the scanned fields
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case Flat15kgStep:
        {
            setPhotoStep(current_step);
            nextPhoto_->setText("");
            explainPhoto_->setText("");
            break;
        }

        case TiltBottomPlateStep:
        {
            setPhotoStep(current_step);
            //nextPhoto_->setText( photoButtonMsg_[current_step] );
            nextPhoto_->setText( "" );
            explainPhoto_->setText( photoLabelMsg_[current_step] );            
            break;
        }

        case Back0kgStep:
        {
            highlightDataLabels(true,false,false,false);            //highlight x axis labels
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case Back5kgStep:
        case Back15kgStep:
        {
            highlightDataLabels(true,false,false,false);            //highlight x axis labels
            setPhotoStep(current_step);
            nextPhoto_->setText( "" );
            explainPhoto_->setText( "" );
            break;
        }

        case Left0kgStep:
        {
            highlightDataLabels(false,false,true,false);            //highlight Z axis labels
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case Left15kgStep:
        {
            highlightDataLabels(false,false,true,false);            //highlight Z axis labels
            setPhotoStep(current_step);
            nextPhoto_->setText("");
            explainPhoto_->setText("");
            break;
        }

        case Front0kgStep:
        {
            highlightDataLabels(true,false,false,false);            //hilight x axis labels
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case Front5kgStep:
        case Front15kgStep:
        {
            highlightDataLabels(true,false,false,false);            //hilight x axis labels
            setPhotoStep(current_step);
            nextPhoto_->setText("");
            explainPhoto_->setText("");
            break;
        }

        case Right0kgStep:
        {
            highlightDataLabels(false,false,true,false);            //hilight Z axis labels
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case Right15kgStep:
        {
            highlightDataLabels(false,false,true,false);            //hilight Z axis labels
            setPhotoStep(current_step);
            nextPhoto_->setText("");
            explainPhoto_->setText("");
            break;
        }

        case WriteStep:
        {
            //reset label backgrounds
            highlightDataLabels(false,false,false,false);

            // Display Write Calibration Step
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case VerifyStep:
        {
            // Display the PASS OR FAIL image and advance text to Finish Test
            qDebug() << "setNextStep VerifyStep ";
            updateLoadCellData(0, 0, 0, 0, calRigAcclXTiltDegrees_, calRigAcclYTiltDegrees_);
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case PassFailStep:
        {
            // Display Pass OR Fail here, then reset everything so we can begin another test
            qDebug() << "setNextStep PassFailStep ";
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );       // reset text everywhere
            explainPhoto_->setText( photoLabelMsg_[current_step] );

            if( device_ )
                device_->setWeigherMode( SCALE_MANUAL_MODE );               // REQ: 0x451,3 - manua
#if 1//bypass
            //turn power to DUT off
            setSensorDUTPowerEnable( 0 );
#endif            
            break;
        }

        case CompleteStep:
        {
            // Display instructions on how to shut everything off and down

            //endDeviceUnderTest();

            qDebug() << "setNextStep CompleteStep ";
            setPhotoStep(current_step);
            nextPhoto_->setText( photoButtonMsg_[current_step] );
            explainPhoto_->setText( photoLabelMsg_[current_step] );
            break;
        }

        case StartOver:
        {
            // Display the START TEST step again.  Start over. Reset all variables and steps
            step_ = SetupStep;                                       // reset steps
            setPhotoStep(step_);                                     // reset pic step
            nextPhoto_->setText( photoButtonMsg_[step_] );           // reset button text
            explainPhoto_->setText( photoLabelMsg_[step_] );         // reset instructions
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

    if( PassFailStep == current_step)
    {
        qDebug() << "\n===== calibui::setPhotoStep" << " =====";
        if( passVerification_ )
        {
            if( timeout_error_ )
                pass_image =  ":/steps/step" + QString::number(MAX_STEPS-1) + "_fail.png";
            else
                pass_image =  ":/steps/step" + QString::number(MAX_STEPS-1) + "_pass.png";
        }
        else
            pass_image =  ":/steps/step" + QString::number(MAX_STEPS-1) + "_fail.png";

        qDebug() << pass_image;

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
    if( !TESTING_SENSORS )
    {
        setEnabledButtonState(false);

        if( (step_ >= Flat0kgStep && step_ <= Right15kgStep) )
        {
#if 0            
            if( !countsValid(step_) )
            {
                ui->lblError->setText(tr("Incorrect Loadcell orientation"));
                return;
            }
#endif            
        }
    }

    qDebug() << "\n===== calibui::onClicked() =====: Perform next step, step_:" << step_;

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

        if( settledWeight_ )
        {
            step_++;
            pStep = step_;
            setNextCalibrationStep(pStep);          // dont go to next steps if you haven't passed the current sensor tests
            setEnabledButtonState(true);
        }
        else
        {
            qDebug() << "OnClicked - settledWeight_ == false, could not move on to next step.";
        }
    }

    if( !(step_ == PowerOnStep || ( step_ >= Flat0kgStep && step_ <= Right15kgStep) ) )
    {
        //setEnabledButtonState(true);        // these  waits for the events to happen before enabling the next step
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateValueMaxDate( uint32_t mfgDate )
{
    qDebug() << "\n===== calibui::onUpdateValueMaxDate(): START ";

    readDateOfMfg_ = mfgDate;           // Im just going to compare seconds - not any kinds of formatted date

    qDebug() << "onUpdateValueMaxDate(): readdinDateOfMfg: " << readDateOfMfg_;
    qDebug() << "onUpdateValueMaxDate(): writtenDateOfMfg: " << writtenDateOfMfg_;
    qDebug() << "onUpdateValueMaxDate(): NOW call requestAssemblyNumber()";

    // Date calls Assembly Number which calls Hardware Version which calls Serial Number which call FIT Struct
    if( device_ )
        device_->requestAssemblyNumber();               // 0x420,0
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateValueMaxAssemblyNumber( QString assemNum )
{
    qDebug() << "\n===== calibui::onUpdateValueMaxAssemblyNumber(): START ";

    // This method gets called twice because it has to handle 2 frames.  So I need a var to keep track of which call it is so I can append data

    assemblyFrames_++;

    if( 1 == assemblyFrames_ )
    {
        readAssembNum_ = assemNum;              // add the initial frame 1 string
        qDebug() << "onUpdateValueMaxAssemblyNumber(): Frame Count = 1 readAssemNum: "  << readAssembNum_;
    }
    else if( 2 == assemblyFrames_ )
    {
        readAssembNum_.append( assemNum );        // for frame 2 add to the initial frame 1 string
        qDebug() << "onUpdateValueMaxAssemblyNumber(): Frame Count = 2  readdinAssemNum: "  << readAssembNum_;
        qDebug() << "onUpdateValueMaxAssemblyNumber(): Frame Count = 2  writtenAssemNum: "  << writtenAssembNum_;
        qDebug() << "onUpdateValueMaxAssemblyNumber(): NOW call requestHardwareVersion";

        if( DISPLAY_WRITTEN_DATE )
            assemblyFrames_ = 0;

        // Assembly Number which calls Hardware Version which calls Serial Number
        if( device_ )
            device_->requestHardwareVersion();      // 0x420,1 only do this if we have both pieces of the assemblyNumber processes
    }
    else
    {
        qDebug() << "onUpdateValueMaxAssemblyNumber(): CRITICAL ERROR: FRAME COUNTER > 2";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateValueMaxHWVersion( QString hwVersion )
{
    qDebug() << "\n===== calibui::onUpdateValueMaxHWVersion(): START ";

    qDebug() << "onUpdateValueMaxHWVersion(): Perform next step, step_:" << step_;
    readHWVersion_ = hwVersion;

    qDebug() << "onUpdateValueMaxHWVersion(): readdinHwVersion: " << readHWVersion_;
    qDebug() << "onUpdateValueMaxHWVersion(): writtenHwVersion: " << writtenHWVersion_;
    qDebug() << "onUpdateValueMaxHWVersion(): NOW call requestSerialNumber()";

    // Hardware Version calls Serial Number
    if( device_ )
        device_->requestSerialNumber();                     // 0x420,2
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateValueMaxSerialNumber( QString serialNum )
{
    qDebug() << "\n===== calibui::onUpdateValueMaxSerialNumber(): START ";

    serialFrames_++;

    if( 1 == serialFrames_ )
    {
        readSerialNum_ = serialNum.trimmed();                 // initial frame 1 string
        qDebug() << "onUpdateValueMaxSerialNumber(): Frame Count = 1 readSerialNum_: "  << readSerialNum_;
    }
    else if( 2 == serialFrames_ )
    {
        readSerialNum_.append( serialNum.trimmed() );         // in frame 2 add to frame 1 string
        qDebug() << "onUpdateValueMaxSerialNumber(): Frame Count = 2 readdinSerialNum_: "  << readSerialNum_;
        qDebug() << "onUpdateValueMaxSerialNumber(): Frame Count = 2 writtenSerialNum_: "  << writtenSerialNum_;

        // Serial Number requests the FIT Parameter
        qDebug() << "onUpdateValueMaxSerialNumber(): TEST DISPLAY_WRITTEN_DATE so pop dialog "  << writtenSerialNum_;

        if( DISPLAY_WRITTEN_DATE )
        {
            qDebug() << "onUpdateValueMaxSerialNumber(): TEST DISPLAY_WRITTEN_DATE is TRUE so pop dialog "  << writtenSerialNum_;
            QString errMsg = "\n=============================== Scanned Values ===============================";
            if( writtenDateOfMfg_ != readDateOfMfg_ )
            {
                errMsg += "\n\nThe written Date of Manufacturing did not match the saved Date of Manufacturing.";
                errMsg += "\nFAILED   writtenDateOfMfg_ = " + QString::number(writtenDateOfMfg_) + "     readDateOfMfg_ = " + QString::number(readDateOfMfg_);
                qDebug() << "FAILED   writtenDateOfMfg_ = " << writtenDateOfMfg_ << "     readDateOfMfg_ = " << readDateOfMfg_;
            }
            else
            {
                errMsg += "\n\nThe written Date of Manufacturing matched the saved Date of Manufacturing.";
                errMsg += "\nPASSED   writtenDateOfMfg_ = " + QString::number(writtenDateOfMfg_) + "     readDateOfMfg_ = " + QString::number(readDateOfMfg_);
                qDebug() << "PASSED   writtenDateOfMfg_ = " << writtenDateOfMfg_ << "     readDateOfMfg_ = " << readDateOfMfg_;
            }

            if( readSerialNum_.compare(writtenSerialNum_, Qt::CaseInsensitive) != 0 )
            {
                errMsg += "\n\nThe written Serial Number did not match the saved Serial Number.";
                errMsg +="\nFAILED   writtenSerialNumber = " + writtenSerialNum_ + "     readSerialNumber_ = " + readSerialNum_;
                qDebug() << "FAILED   writtenSerialNumber = " << writtenSerialNum_ << "     readSerialNumber_ = " << readSerialNum_;
            }
            else
            {
                errMsg += "\n\nThe written Serial Number matched the saved Serial Number.";
                errMsg += "\nPASSED   writtenSerialNumber = " + writtenSerialNum_ + "     readSerialNumber_ = " + readSerialNum_;
                qDebug() << "PASSED   writtenSerialNumber = " << writtenSerialNum_ << "     readSerialNumber_ = " << readSerialNum_;
            }

            if( readHWVersion_.compare(writtenHWVersion_, Qt::CaseInsensitive) != 0 )
            {
                errMsg += "\n\nThe written Hardware Version did not match the saved Hardware Version.";
                errMsg += "\nFAILED   writtenHWVersion = " + writtenHWVersion_ + "     readHWVersion_ = " + readHWVersion_;
                qDebug() << "FAILED   writtenHWVersion = " << writtenHWVersion_ << "     readHWVersion_ = " << readHWVersion_;
            }
            else
            {
                errMsg += "\n\nThe written Hardware Version matched the saved Hardware Version.";
                errMsg += "\nPASSED   writtenHWVersion = " + writtenHWVersion_ + "     readHWVersion_ = " + readHWVersion_;
                qDebug() << "PASSED   writtenHWVersion = " << writtenHWVersion_ << "     readHWVersion_ = " << readHWVersion_;
            }

            if( readAssembNum_.compare(writtenAssembNum_, Qt::CaseInsensitive) != 0 )
            {
                errMsg += "\n\nThe written Assembly Number did not match the saved Assembly Number.";
                errMsg += "\nFAILED   writtenAssembNum = " + writtenAssembNum_ + "     readAssembNum_ = " + readAssembNum_;
                qDebug() << "FAILED   writtenAssembNum = " << writtenAssembNum_ << "     readAssembNum_ = " << readAssembNum_;
            }
            else
            {
                errMsg += "\n\nThe written Assembly Number matched the saved Assembly Number.";
                errMsg += "\nPASSED   writtenAssembNum = " + writtenAssembNum_ + "     readAssembNum_ = " + readAssembNum_;
                qDebug() << "PASSED   writtenAssembNum = " << writtenAssembNum_ << "     readAssembNum_ = " << readAssembNum_;
            }

            popErrorDialog(errMsg);
        }

        if( verifyReadings_ )
        {
            qDebug() << "onUpdateValueMaxSerialNumber() CALL requestFitCalibration() ";
            if( device_ )
                device_->requestFitCalibration();
        }
    }
    else
    {
        qDebug() << "onUpdateValueMaxSerialNumber(): CRITICAL ERROR: FRAME COUNTER > 2";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateWeigherStateChanged(weigherState state, float weight)
{
    qDebug() << "\n===== calibui::onWeigherStateChanged(): START =====";

    // This is where the weigher status will be handled
    if( stable == state )
    {
        // do something
        qDebug() << "onWeigherStateChanged(): "  << state;
        qDebug() << "onWeigherStateChanged(): "  << weight;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//void calibui::onUpdateValueMaxWeigherStatus(uint16_t flags, uint32_t rawAtoDCounts, int32_t nonZeroCalibCounts, int32_t zeroCalibCounts, int32_t acclXRawCounts, int32_t acclYRawCounts, int32_t acclZRawCounts, int16_t acclXTiltDegrees, int16_t acclYTiltDegrees)
void calibui::onUpdateValueMaxWeigherStatus(uint16_t flags, uint32_t rawAtoDCounts, int32_t nonZeroCalibCounts, int32_t zeroCalibCounts, int32_t acclXRawCounts, int32_t acclYRawCounts, int32_t acclZRawCounts, int16_t acclXTiltDegrees, int16_t acclYTiltDegrees, int16_t CalRigAcclXTiltDegrees, int16_t CalRigAcclYTiltDegrees)
{
    countmsgs_++;

    if( freezeReads_ )
    {
        qDebug() << "====== onUpdateValueMaxWeigherStatus() freezeReads_ is TRUE so EXIT";
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
    calRigAcclXTiltDegrees_ = (double_t)CalRigAcclXTiltDegrees / 100;
    calRigAcclYTiltDegrees_ = (double_t)CalRigAcclYTiltDegrees / 100;
    
    //qDebug() << "====== onUpdateValueMaxWeigherStatus() flags: " << std::hex << flags << std::dec << " Step " << step_;
    //qDebug() << "====== onUpdateValueMaxWeigherStatus() calRigAcclXTiltDegrees_: " << std::dec << calRigAcclXTiltDegrees_;
    //qDebug() << "====== onUpdateValueMaxWeigherStatus() calRigAcclYTiltDegrees_: " << std::dec << calRigAcclYTiltDegrees_;

    updateLoadCellData(acclXRawCounts_, acclYRawCounts_, acclZRawCounts_,rawAtoDCounts_, calRigAcclXTiltDegrees_, calRigAcclYTiltDegrees_);

    //In Power on step we wait for the Power Switch to be toggled before we continue
    //In all of these steps we are waiting for weight readings to stablize before we move on
    if(step_ == PowerOnStep)
    {
#if 0 //no physical power button       
        if( ( flags_ & WEIGHER_ACCL_COMM_ON ) == WEIGHER_ACCL_COMM_ON )
        {
            // WEIGHER_ACCL_COMM_ON = 0x1000        // means POWER IS ON
            ui->lblError->setText(tr(""));
            setEnabledButtonState(true);
        }
        else
        {
            setEnabledButtonState(false);
            ui->lblError->setText(tr("Turn on the Power Switch to enable the next step"));
        }
#endif        
    }
    else if ( step_ >= Flat0kgStep && step_ <= Right15kgStep )
    {
        if(!((flags_ & SMALL_MOTION) == SMALL_MOTION || (flags_ & LARGE_MOTION) == LARGE_MOTION))
        {
#if 0            
            if(countsValid(step_))
            {
                //ui->lblError->setText(tr(""));
            }
            else
            {
                ui->lblError->setText(tr("Incorrect Loadcell orientation"));
            }
#endif
            //setEnabledButtonState(true);
            settledWeight_ = true;
        }
        else
        {
            //setEnabledButtonState(false);
            //ui->lblError->setText(tr("Allow the weight to settle to enable the next step"));
        }

        if(step_ == TiltBottomPlateStep)
        {
            SENSOR_TEST_RESULT sensor_test = testSensors( SENSOR_TABLE_TILTED );
         
            if(sensor_test.result == true)
            {
                step_++;
                setNextCalibrationStep(step_);          // dont go to next steps if you haven't passed the current sensor tests
                setEnabledButtonState(true);
            }            
        }

        //verify connection with Accl
        if((flags_ & WEIGHER_ACCL_COMM_ON) != WEIGHER_ACCL_COMM_ON) 
        {
            QString msg = "Loadcell communications lost. \n\nMake sure cables are connected and try again.";
                
            settledWeight_ = true;   // to move to next step

            endDeviceUnderTest();

            //skip over to StartOver Step. no need to display dismanteling intrunctions
            step_ = StartOver;

            popErrorDialog(msg, true);

            setNextCalibrationStep(step_);          // dont go to next steps if you haven't passed the current sensor tests
            setEnabledButtonState(true);

        }
        
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int calibui::startDeviceUnderTest()
{
    int result;

    if( TESTING_SENSORS )
    {
        // Turn on now just to test it
        ui->lblShowHowTo->setStyleSheet("QLabel#lblShowHowTo {"
            "border-style:outset;"
            "border-radius:13px;"
            "border:8px solid green;"
            "}");
    }

    
    if( device_ )
    {
        stack_dir = home;

        if( motor_ )
        {
            // check to see if this is 1st time through
            if(motor_->isMtrd24HomePositionDetermined() == true)
            {
                result = motor_->Mtrd24ReturnHome();
            }
            else
            {
                result = motor_->Mtrd24FindHome();
            }
            
            
            if( result > 0 )
            {
                QString msg( "Motor stack failed to home: " + QString::number( result) + "\nCycle all power and start the tool again." );
                popErrorDialog(msg, true);
            }
            else
            {
                //see if DUT is powered and we've received accl counts
                if(((flags_ & WEIGHER_ACCL_COMM_ON) == WEIGHER_ACCL_COMM_ON)    /*&&
                    (acclXRawCounts_ != 0) && (acclYRawCounts_ != 0) && (acclZRawCounts_ != 0)*/)
                {
                    // maybe light up an onscreen indicator that means power is on - has no impact. why not?
                    ui->lblShowHowTo->setStyleSheet("QLabel#lblShowHowTo {"
                        "border-style:outset;"
                        "border-radius:13px;"
                        "border:8px solid green;"
                        "}");

                    // Tell Weigher to start tracking  mode
                    device_->setWeigherMode( SCALE_WEIGHT_TRACKING_MODE );          // REQ: 0x451,4

                }
                else
                {
                    QString msg( "Load Cell not detected: \n\n Make sure Acclerometer cable is connected and try again." );
                    popErrorDialog(msg,true);
                    result = 1;//error
                }
            }

            setEnabledButtonState(true);
        }
        else
        {
            QString msg( "Motor could not be found to be Powered On.\nCycle all power and start the tool again." );
            popErrorDialog( msg, true );
        }
    }
    else
    {
        QString msg( "The Device driver could not be found to initialize anything.\nCycle all power and start the tool again." );
        popErrorDialog( msg, true );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::endDeviceUnderTest()
{
    // maybe light up RED an onscreen indicator that means power is off- has no impact. why not?
    ui->lblShowHowTo->setStyleSheet("QLabel#lblShowHowTo {"
        "border-style:outset;"
        "border-radius:13px;"
        "border:8px solid red;"
        "}");

    //  turn DUT power off
    setSensorDUTPowerEnable( 0 );
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::allowWeightsToSettle()
{
    // after we lower the weight stack on the platter we have to wait for the counts to settle. So fire a timer to wait for them to settle
#if 0
    if( stack_dir == home)
        qDebug() << "Raising weight stack to home so dont start the settled weight timer.";
    else
#endif    
    {
        ui->lblError->setText(tr("Taking Loadcell reading..."));
        settledWeight_ = false;
        settleWeightTries_ = 0;
        settledWeightsWithDroppedStack_->start();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onLabelClicked()
{
    if( step_ == PowerOnStep )
    {
        int pStep = step_;
        readCurrentCalibrationSettings(pStep);

        if( settledWeight_ )
        {
            step_++;
            pStep = step_;
            setNextCalibrationStep(pStep);          // dont go to next steps if you haven't passed the current sensor tests
        }
        else
        {
            qDebug() << "Sensors failed, could not move past onscreen POWER ON button.";
        }
    }
}

void calibui::initDUTTimerExpired()
{
    qDebug() << "\n===== calibui::initDUTTimerExpired() =====: Perform next step, step_:" << step_;    

    initDUTTimerExpired_ = true;

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

        if( settledWeight_ )
        {
            step_++;
            pStep = step_;
            setNextCalibrationStep(pStep);          // dont go to next steps if you haven't passed the current sensor tests
        }
        else
        {
            qDebug() << "Sensors failed, could not move on to next test.";
        }
    }

    if( !(step_ == PowerOnStep || ( step_ >= Flat0kgStep && step_ <= Right15kgStep) ) )
    {
        setEnabledButtonState(true);        // these  waits for the events to happen before enabling the next step
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::takeCurrentCountsReading()
{
    // Once we get a settled weight drop, save the current counts
    // Verify what step we are on, and do we have the right weight dropped for that step
    // Do I need to test the weight and step at each step? I could save a lot of code otherwisr

    int current_step = step_;
    bool saved_counts = false;
    bool resetStack = false;

    qDebug() << "Settle timer expired.";

    settleWeightTries_ ++;

    //verify loadcell signal
    if(settleWeightTries_ == 4 && settledWeight_ == false)
    {
        qDebug() << "Settle timeout reached.";

        QString msg = "Bad Loadcell signal. \n\nMake sure cables are connected and try again.";
            
        settledWeight_ = true;   // to move to next step

        endDeviceUnderTest();

        //skip over to StartOver Step. no need to display dismanteling intrunctions
        step_ = CompleteStep;

        //step_ ++;

        setNextCalibrationStep(step_);  

        popErrorDialog(msg, true);
    } 

    if( settledWeight_ )
    {
        settledWeightsWithDroppedStack_->stop();

        switch( current_step )
        {
            case Flat0kgStep:
            {
                if(countsValid(step_) == false)
                {
                    ui->lblError->setText(tr("Incorrect Loadcell orientation"));
                }

                saved_counts = true;
                saveCurrentCounts( current_step );  // well then by all means, save the current step
                break;              // Counts Step #1
            }
            
            case Flat15kgStep:
            {
                saved_counts = true;
                saveCurrentCounts( current_step );  // well then by all means, save the current step

                 // done with reading so send weights HOME again
                if( device_ )
                    resetStack = true;

                break;              // Counts Step #2
            }

            case Back0kgStep:
            {
                if(countsValid(step_) == false)
                {
                    ui->lblError->setText(tr("Incorrect Loadcell orientation"));
                }

                saved_counts = true;
                saveCurrentCounts( current_step );  // well then by all means, save the current step
                break;              // Counts Step #3
            }

            case Back5kgStep:
            {
                saved_counts = true;
                saveCurrentCounts(current_step);
                break;              // Counts Step #4
            }

            case Back15kgStep:
            {
                saved_counts = true;
                saveCurrentCounts(current_step);

                // done with reading so send weights HOME again
                if( device_ )
                    resetStack = true;

                break;              // Counts Step #5
            }

            case Left0kgStep:
            {
                if(countsValid(step_) == false)
                {
                    ui->lblError->setText(tr("Incorrect Loadcell orientation"));
                }

                saved_counts = true;
                saveCurrentCounts( current_step );  // well then by all means, save the current step
                break;              // Counts Step #6
            }

            case Left15kgStep:
            {
                saved_counts = true;
                saveCurrentCounts(current_step);

                // done with reading so send weights HOME again
                if( device_ )
                    resetStack = true;

                break;              // Counts Step #7
            }

            case Front0kgStep:
            {
                if(countsValid(step_) == false)
                {
                    ui->lblError->setText(tr("Incorrect Loadcell orientation"));
                }

                saved_counts = true;
                saveCurrentCounts( current_step );  // well then by all means, save the current step
                break;              // Counts Step #8
            }
                
            case Front5kgStep:
            {
                saved_counts = true;
                saveCurrentCounts(current_step);
                break;              // Counts Step #9
            }

            case Front15kgStep:
            {
                saved_counts = true;
                saveCurrentCounts(current_step);

                // done with reading so send weights HOME again
                if( device_ )
                    resetStack = true;

                break;              // Counts Step #10
            }

            case Right0kgStep:
            {
                if(countsValid(step_) == false)
                {
                    ui->lblError->setText(tr("Incorrect Loadcell orientation"));
                }

                saved_counts = true;
                saveCurrentCounts( current_step );  // well then by all means, save the current step
                break;              // Counts Step #11
            }

            case Right15kgStep:
            {
                saved_counts = true;
                saveCurrentCounts(current_step);

                /* stop weight streaming mode */
                if( device_ )
                device_->setWeigherMode( SCALE_MANUAL_MODE );

                //go ahead and write cal values now
                /////////////////////////////////////////////////
                grav_const_ = 980023;                  // defaults to Hillsboro (not sure why we are doing this again after initialize)
                isHillsboroGravity_ = true;

                if ( QFile::exists( "/etc/hobart/gravity_troy.dat" ) )
                {
                    grav_const_ = 980104;               // Troy
                    isHillsboroGravity_ = false;
                }

                step_ = WriteStep;
                writeCalibrationToFlash();  

                if(passVerification_ == true)
                {
                    verifyReadings_ = true;                                                 // set true when we want to read the values FROM the board
                }
                    
                
                settledWeight_ = true;                                                  // to move to next step  
                //////////////////////

                // done with reading so send weights HOME again
                if( device_ )
                    resetStack = true;
                

                break;              // // Counts Step #12
            }

            default:
                break;
        }
    }

    if( resetStack )
    {
        stack_dir = raised;
        if( motor_ )
        {
            ui->lblError->setText(tr("Lifting Weight Stack. Please wait..."));
            int result = motor_->Mtrd24ReturnHome();                         // Drop 0->15 kg on to the platter

            if( result > 0 )
            {
                QString msg( "Raising the motor stack to HOME failed: " + QString::number( result) + "\nCycle all power and start the Auto Rig again." );
                popErrorDialog(msg);
            }
        }
        else
        {
            QString msg( "Motor could not be found and was not initialized.""\nCycle all power and start the tool again." );
            popErrorDialog( msg );
        }
    }

    if( saved_counts )
    {
        if(WriteStep == step_ )
        {
            if( device_ )
                device_->requestDate();

            // Now we are waiting for a reply to verify the write process step.
            // If we never receive a reply, we are locked up here.
            // I added a timeout to handle the case where we never receive back a reply to verify

            verifyStepIsVerified_.setSingleShot( true );
            if( TESTING_SENSORS)
                verifyStepIsVerified_.start( 10000 );
            else
                verifyStepIsVerified_.start( 2000 );

        }
        //else
        {
            step_++;
            int pStep = step_;
            setNextCalibrationStep(pStep);          // dont go to next steps if you haven't passed the current sensor tests
            
            if(autoStartNextStep() == true)
            {
                readCurrentCalibrationSettings(step_);
            }
            else
            {   //user click is needed
                setEnabledButtonState(true);    
            }    
        }
       
    }
    else
    {
        qDebug() << "Sensors failed, could not move on to next test.";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::onUpdateValueMaxFitParameters(int32_t g, int32_t m, int32_t zerovalue, int32_t spanvalue, int32_t Z0, int32_t D, int32_t x2, int32_t x1, int32_t x0, int32_t y2, int32_t y1, int32_t y0, int32_t xrot0, int32_t xrot1, int32_t xrot2, int32_t yrot0, int32_t yrot1, int32_t yrot2, int32_t zrot0, int32_t zrot1, int32_t zrot2, int32_t xgain, int32_t ygain, int32_t gcoeff, int32_t checksum)
{
    qDebug() << "\n===== calibui::onUpdateValueMaxFitParameters() START" << " =====";

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

        qDebug() << "===== passOrFailCalibration() RESULTS ===== ";
        if( DISPLAY_SAVED_READ_FIT_PARAMS )
            popErrorDialog(failMsg);
#if 1
        step_++;
        int pStep = step_;
        setNextCalibrationStep(pStep);
        setEnabledButtonState(true);
#endif        
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::moveOnWithoutVerification()
{
    qDebug() << "moveOnWithoutVerification() START ";

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
    qDebug() << "writeCalibrationToFlash() - start writing calibration";

    const std::vector<double> counts( weigherValues() );
    const std::vector<double> acc_values( accValues() );

    QString msg = "";
    msg = "Print ACC_VALUES AFTER applying resolution";
    msg += "\n\n===== Acc Values =====";
    for( std::size_t i = 0; i < acc_values.size(); i++)
        msg += "\nacc_values[" + QString::number(i) + "]: " + QString::number(acc_values[i]);
    qDebug() << msg;

    msg = "";
    msg = "Print WEIGHER VALUES After toDouble()";
    msg += "\n\n===== Weigher Values =====";
    for( std::size_t i = 0; i < counts.size(); i++)
        msg += "\nweigher_counts[" + QString::number(i) + "]: " + QString::number(counts[i]);

    if( DISPLAY_GENERATE_FIT_RESULTS )
        qDebug() << msg;

    const bool use_metric( (units_ == "kg") ? true : false );

    std::string result;

    fit_flash_section f;
    saved_f.checksum = 0;

    double gravity_const = (double) (grav_const_ / RESOLUTION_100K);

    qDebug() << "writeCalibrationToFlash() - now call generate_fit()";

    bool passed_generate_fit = generate_fit( counts, use_metric, gravity_const, acc_values, f, result );

    if( DISPLAY_GENERATE_FIT_RESULTS )
    {
        QString resultsStr = QString::fromStdString( result );
        popErrorDialog(resultsStr);
    }
    else
    {
        qDebug() <<  QString::fromStdString(result);
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
        qDebug() << "writeCalibrationToFlash - generate_fit routine has PASSED. Now write data to the board.";

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
    qDebug() << "\n=====Calibui::writeScannedFields =====";

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

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::sleepFor( int ms ) const
{
    // amout of time to sleep
    QTime sleep( QTime::currentTime().addMSecs( ms ) );

    do
    {
        QApplication::processEvents();

    } while ( QTime::currentTime() < sleep );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::saveCurrentCounts(int current_step)
{
    uint32_t Kg0_cnts;
    uint32_t Kg15_cnts;
    uint32_t result_cnts;

    // Counts are received all of the time and this method saves the current status of the current counts
    QString msg = "\n\n===== saveCurrentCounts() START  ===== ";
    msg += "\nstatus counts " + QString::number(countmsgs_) + " and current_step " + current_step;

    freezeReads_ = true;

    switch( current_step )
    {
        case Flat0kgStep:
            counts_level_0kg_ = QString::number(rawAtoDCounts_);
            accx_level_ = QString::number(acclXRawCounts_);
            accy_level_ = QString::number(acclYRawCounts_);
            accz_level_ = QString::number(acclZRawCounts_); 

            msg += "\nsaveCurrentCounts::LevelStep 0";
            msg += "\naccl_X_Counts_: " + accx_level_ + "\naccl_Y_Counts_: " + accy_level_ + "\naccl_Z_Counts_: " + accz_level_;
            msg += "\nRawA2D-Flat 0:" + counts_level_0kg_;
 
            break;

        case Flat15kgStep:
            counts_level_15kg_ = QString::number(rawAtoDCounts_);

            msg += "\nsaveCurrentCounts::LevelStep 15";
            msg += "\nRawA2D-Level 15: " + counts_level_15kg_;
            
#if 1            
            Kg0_cnts = counts_level_0kg_.toLong();
            Kg15_cnts = counts_level_15kg_.toLong();
            result_cnts = labs(Kg15_cnts - Kg0_cnts);

            //verify loadcell signal
            if(result_cnts < 1000000 )
            {
                QString msg = "Bad Loadcell signal. \n\nMake sure cables are connected and try again.";
                    
                settledWeight_ = true;   // to move to next step

                endDeviceUnderTest();

                //skip over to StartOver Step. no need to display dismanteling intrunctions
                step_ = CompleteStep;

                popErrorDialog(msg, true);
            }          
#endif            
            break;

        case Back0kgStep:
            counts_back_0kg_ = QString::number(rawAtoDCounts_);
            accx_back_ = QString::number(acclXRawCounts_);
            accy_back_ = QString::number(acclYRawCounts_);
            accz_back_ = QString::number(acclZRawCounts_);

            msg += "\nsaveCurrentCounts::Back0kgStep 0";
            msg += "\nacclXRawCounts_: " + accx_back_ + "\nacclYRawCounts_: " + accy_back_ + "\nacclZRawCounts_: " + accz_back_;
            msg += "\nRawA2D-Back 0: " + counts_back_0kg_;
            break;

        case Back5kgStep:
            counts_back_5kg_ = QString::number(rawAtoDCounts_);

            msg += "\nsaveCurrentCounts::Back5kgStep 5";
            msg += "\nRawA2D-Back 5: " + counts_back_5kg_;
            break;

        case Back15kgStep:
            counts_back_15kg_ = QString::number(rawAtoDCounts_);

            msg += "\nsaveCurrentCounts::Back15kgStep 15";
            msg += "\nRawA2D-Back15: " + counts_back_15kg_;
#if 1       
            //verify loadcell signal     
            Kg0_cnts = counts_back_0kg_.toLong();
            Kg15_cnts = counts_back_15kg_.toLong();
            result_cnts = labs(Kg15_cnts - Kg0_cnts);
            
            if(result_cnts < 1000000 )
            {
                QString msg = "Bad Loadcell signal. \n\nMake sure cables are connected and try again.";
                    
                settledWeight_ = true;   // to move to next step

                endDeviceUnderTest();

                //skip over to StartOver Step. no need to display dismanteling intrunctions
                step_ = CompleteStep;

                popErrorDialog(msg, true);
            }          
#endif            
            break;

        case Left0kgStep:
            counts_left_0kg_ = QString::number(rawAtoDCounts_);
            accx_left_ = QString::number(acclXRawCounts_);
            accy_left_ = QString::number(acclYRawCounts_);
            accz_left_ = QString::number(acclZRawCounts_);

            msg += "\nsaveCurrentCounts::Left0kgStep 0";
            msg += "\nacclXRawCounts_: " + accx_left_ + "\nacclYRawCounts_: " + accy_left_ + "\nacclZRawCounts_: " + accz_left_;
            msg += "\nRawA2D-Left 0: " + counts_left_0kg_;
            break;

        case Left15kgStep:
            counts_left_15kg_ = QString::number(rawAtoDCounts_);

            msg += "\nsaveCurrentCounts::Left15kgStep 15";
            msg += "\nRawA2D-Left 15: " + counts_left_15kg_;
#if 1       
            //verify loadcell signal     
            Kg0_cnts = counts_left_0kg_.toLong();
            Kg15_cnts = counts_left_15kg_.toLong();
            result_cnts = labs(Kg15_cnts - Kg0_cnts);
            
            if(result_cnts < 1000000 )
            {
                QString msg = "Bad Loadcell signal. \n\nMake sure cables are connected and try again.";
                    
                settledWeight_ = true;   // to move to next step

                endDeviceUnderTest();

                //skip over to StartOver Step. no need to display dismanteling intrunctions
                step_ = CompleteStep;

                popErrorDialog(msg, true);
            }          
#endif            
            break;

        case Front0kgStep:
            counts_front_0kg_ = QString::number(rawAtoDCounts_);
            accx_front_ = QString::number(acclXRawCounts_);
            accy_front_ = QString::number(acclYRawCounts_);
            accz_front_ = QString::number(acclZRawCounts_);

            msg += "\nsaveCurrentCounts::Front0kgStep 0";
            msg += "\nacclXRawCounts_: " + accx_front_ + "\nacclYRawCounts_: " + accy_front_ + "\nacclZRawCounts_:" + accz_front_;
            msg += "\nRawA2D Front 0: " + counts_front_0kg_;

            break;

        case Front5kgStep:
            counts_front_5kg_ = QString::number(rawAtoDCounts_);

            msg += "\nsaveCurrentCounts::FrontStep 5";
            msg += "\nRawA2D Front 5: " + counts_front_5kg_;
            break;

        case Front15kgStep:
            counts_front_15kg_ = QString::number(rawAtoDCounts_);

            msg += "\nsaveCurrentCounts::FrontStep 15";
            msg += "\nRawA2D-Front 15: " + counts_front_15kg_;
#if 1       
            //verify loadcell signal     
            Kg0_cnts = counts_front_0kg_.toLong();
            Kg15_cnts = counts_front_15kg_.toLong();
            result_cnts = labs(Kg15_cnts - Kg0_cnts);
            
            if(result_cnts < 1000000 )
            {
                QString msg = "Bad Loadcell signal. \n\nMake sure cables are connected and try again.";
                    
                settledWeight_ = true;   // to move to next step

                endDeviceUnderTest();

                //skip over to StartOver Step. no need to display dismanteling intrunctions
                step_ = CompleteStep;

                popErrorDialog(msg, true);
            }          
#endif             
            break;

        case Right0kgStep:
            counts_right_0kg_ = QString::number(rawAtoDCounts_);
            accx_right_ = QString::number(acclXRawCounts_);
            accy_right_ = QString::number(acclYRawCounts_);
            accz_right_ = QString::number(acclZRawCounts_);

            msg += "\nsaveCurrentCounts::Right0kgStep 0";
            msg += "\nacclXRawCounts_: " + accx_right_ + "\nacclYRawCounts_: " + accy_right_ + "\nacclZRawCounts_: " + accz_right_;
            msg += "\nRawA2D Right 0: " + counts_right_0kg_;
            break;

        case Right15kgStep:
            counts_right_15kg_ = QString::number(rawAtoDCounts_);

            msg += "\nsaveCurrentCounts::Right15kgStep 15";
            msg += "\nRawA2D-Right 15: " + counts_right_15kg_;
#if 1       
            //verify loadcell signal     
            Kg0_cnts = counts_right_0kg_.toLong();
            Kg15_cnts = counts_right_15kg_.toLong();
            result_cnts = labs(Kg15_cnts - Kg0_cnts);
            
            if(result_cnts < 1000000 )
            {
                QString msg = "Bad Loadcell signal. \n\nMake sure cables are connected and try again.";
                    
                settledWeight_ = true;   // to move to next step

                endDeviceUnderTest();

                //skip over to StartOver Step. no need to display dismanteling intrunctions
                step_ = CompleteStep;

                popErrorDialog(msg, true);
            }          
#endif               
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
        qDebug() << "FAILED   writtenDateOfMfg_ = " << writtenDateOfMfg_ << "     readDateOfMfg_ = " << readDateOfMfg_;
    }
    else
        qDebug() << "PASSED   writtenDateOfMfg_ = " << writtenDateOfMfg_ << "     readDateOfMfg_ = " << readDateOfMfg_;

    tempRead = readSerialNum_.trimmed();
    tempWritten = writtenSerialNum_.trimmed();
    if( tempRead.compare(tempWritten, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe written Serial Number did not match the saved Serial Number.";
        passVerification_ = false;
        qDebug() << "FAILED   writtenSerialNumber = " << writtenSerialNum_ << "     readSerialNumber_ = " << readSerialNum_;
        qDebug() << "FAILED   writtenSerialNumberLEN = " << tempWritten.length() << "     readSerialNumberLEN_ = " << tempRead.length();

        if( readSerialNum_ == writtenSerialNum_)
            qDebug() << "     BUT PASSED   writtenSerialNumber == readSerialNumber_";
        else
            qDebug() << "     AND FAILED   writtenSerialNumber == readSerialNumber_";
    }
    else
        qDebug() << "PASSED   writtenSerialNumber = " << writtenSerialNum_ << "     readSerialNumber_ = " << readSerialNum_;

    tempRead = readHWVersion_.trimmed();
    tempWritten = writtenHWVersion_.trimmed();
    if( tempRead.compare(tempWritten, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe written Hardware Version did not match the saved Hardware Version.";
        passVerification_ = false;
        qDebug() << "FAILED   writtenHWVersion = " << writtenHWVersion_ << "     readHWVersion_ = " << readHWVersion_;
    }
    else
        qDebug() << "PASSED   writtenHWVersion = " << writtenHWVersion_ << "     readHWVersion_ = " << readHWVersion_;

    tempRead = readAssembNum_.trimmed();
    tempWritten = writtenAssembNum_.trimmed();
    if( tempRead.compare(tempWritten, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe written Assembly Number did not match the saved Assembly Number.";
        passVerification_ = false;
        qDebug() << "FAILED   writtenAssembNum = " << writtenAssembNum_ << "     readAssembNum_ = " << readAssembNum_;
    }
    else
        qDebug() << "PASSED   writtenAssembNum = " << writtenAssembNum_ << "     readAssembNum_ = " << readAssembNum_;

    if( read_g_.compare(saved_fg_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fg_ did not match the read_g_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fg_ = " << saved_fg_ << "     read_g_ = " << read_g_;
    }
    else
        qDebug() << "PASSED   saved_fg_ = " << saved_fg_ << "     read_g_ = " << read_g_;

    if( read_m_.compare(saved_fm_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fm_ did not match the read_m_.";
        passVerification_ = false;
       qDebug() << "FAILED   saved_fm_ = " << saved_fm_ << "     read_m_ = " << read_m_;
    }
    else
        qDebug() << "PASSED   saved_fm_ = " << saved_fm_ << "     read_m_ = " << read_m_;

    if( read_zerovalue_.compare(saved_fzerovalue_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fzerovalue_ did not match the read_zerovalue_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fzerovalue_ = " << saved_fzerovalue_ << "     read_zerovalue_ = " << read_zerovalue_;
    }
    else
        qDebug() << "PASSED   saved_fzerovalue_ = " << saved_fzerovalue_ << "     read_zerovalue_ = " << read_zerovalue_;

    if( read_spanvalue_.compare(saved_fspanvalue_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fspanvalue_ did not match the read_spanvalue_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fspanvalue_ = " << saved_fspanvalue_ << "     read_spanvalue_ = " << read_spanvalue_;
    }
    else
        qDebug() << "PASSED   saved_fspanvalue_ = " << saved_fspanvalue_ << "     read_spanvalue_ = " << read_spanvalue_;

    if( read_Z0_.compare(saved_fZ0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fZ0_ did not match the read_Z0_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fZ0_ = " << saved_fZ0_ << "     read_Z0_ = " << read_Z0_;
    }
    else
        qDebug() << "PASSED   saved_fZ0_ = " << saved_fZ0_ << "     read_Z0_ = " << read_Z0_;

    if( read_D_.compare(saved_fD_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fD_ did not match the read_D_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fD_ = " << saved_fD_ << "     read_D_ = " << read_D_;
    }
    else
        qDebug() << "PASSED   saved_fD_ = " << saved_fD_ << "     read_D_ = " << read_D_;

    if( read_x2_.compare(saved_fx2_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fx2_ did not match the read_x2_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fx2_ = " << saved_fx2_ << "     read_x2_ = " << read_x2_;
    }
    else
        qDebug() << "PASSED   saved_fx2_ = " << saved_fx2_ << "     read_x2_ = " << read_x2_;

    if( read_x1_.compare(saved_fx1_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fx1_ did not match the read_x1_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fx1_ = " << saved_fx1_ << "     read_x1_ = " << read_x1_;
    }
    else
        qDebug() << "PASSED   saved_fx1_ = " << saved_fx1_ << "     read_x1_ = " << read_x1_;

    if( read_x0_.compare(saved_fx0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fx0_ did not match the read_x0_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fx0_ = " << saved_fx0_ << "     read_x0_ = " << read_x0_;
    }
    else
        qDebug() << "PASSED   saved_fx0_ = " << saved_fx0_ << "     read_x0_ = " << read_x0_;

    if( read_y2_.compare(saved_fy2_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fy2_ did not match the read_y2_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fy2_ = " << saved_fy2_ << "     read_y2_ = " << read_y2_;
    }
    else
        qDebug() << "PASSED   saved_fy2_ = " << saved_fy2_ << "     read_y2_ = " << read_y2_;

    if( read_y1_.compare(saved_fy1_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fy1_ did not match the read_y1_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fy1_ = " << saved_fy1_ << "     read_y1_ = " << read_y1_;
    }
    else
        qDebug() << "PASSED   saved_fy1_ = " << saved_fy1_ << "     read_y1_ = " << read_y1_;

    if( read_y0_.compare(saved_fy0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fy0_ did not match the read_y0_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fy0_ = " << saved_fy0_ << "     read_y0_ = " << read_y0_;
    }
    else
        qDebug() << "PASSED   saved_fy0_ = " << saved_fy0_ << "     read_y0_ = " << read_y0_;

    if( read_xrot0_.compare(saved_fxrot0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fxrot0_ did not match the read_xrot0_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fxrot0_ = " << saved_fxrot0_ << "     read_xrot0_ = " << read_xrot0_;
    }
    else
        qDebug() << "PASSED   saved_fxrot0_ = " << saved_fxrot0_ << "     read_xrot0_ = " << read_xrot0_;

    if( read_xrot1_.compare(saved_fxrot1_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fxrot1_ did not match the read_xrot1_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fxrot1_ = " << saved_fxrot1_ << "     read_xrot1_ = " << read_xrot1_;
    }
    else
        qDebug() << "PASSED   saved_fxrot1_ = " << saved_fxrot1_ << "     read_xrot1_ = " << read_xrot1_;

    if( read_xrot2_.compare(saved_fxrot2_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fxrot2_ did not match the read_xrot2_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fxrot2_ = " << saved_fxrot2_ << "     read_xrot2_ = " << read_xrot2_;
    }
    else
        qDebug() << "PASSED   saved_fxrot2_ = " << saved_fxrot2_ << "     read_xrot2_ = " << read_xrot2_;

    if( read_yrot0_.compare(saved_fyrot0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fyrot0_ did not match the read_yrot0_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fyrot0_ = " << saved_fyrot0_ << "     read_yrot0_ = " << read_yrot0_;
    }
    else
        qDebug() << "PASSED   saved_fyrot0_ = " << saved_fyrot0_ << "     read_yrot0_ = " << read_yrot0_;

    if( read_yrot1_.compare(saved_fyrot1_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fyrot1_ did not match the read_yrot1_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fyrot1_ = " << saved_fyrot1_ << "     read_yrot1_ = " << read_yrot1_;
    }
    else
        qDebug() << "PASSED   saved_fyrot1_ = " << saved_fyrot1_ << "     read_yrot1_ = " << read_yrot1_;

    if( read_yrot2_.compare(saved_fyrot2_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fyrot2_ did not match the read_yrot2_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fyrot2_ = " << saved_fyrot2_ << "     read_yrot2_ = " << read_yrot2_;
    }
    else
        qDebug() << "PASSED   saved_fyrot2_ = " << saved_fyrot2_ << "     read_yrot2_ = " << read_yrot2_;

    if( read_zrot0_.compare(saved_fzrot0_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fzrot0_ did not match the read_zrot0_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fzrot0_ = " << saved_fzrot0_ << "     read_zrot0_ = " << read_zrot0_;
    }
    else
        qDebug() << "PASSED   saved_fzrot0_ = " << saved_fzrot0_ << "     read_zrot0_ = " << read_zrot0_;

    if( read_zrot1_.compare(saved_fzrot1_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fzrot1_ did not match the read_zrot1_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fzrot1_ = " << saved_fzrot1_ << "     read_zrot1_ = " << read_zrot1_;
    }
    else
        qDebug() << "PASSED   saved_fzrot1_ = " << saved_fzrot1_ << "     read_zrot1_ = " << read_zrot1_;

    if( read_zrot2_.compare(saved_fzrot2_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fzrot2_ did not match the read_zrot2_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fzrot2_ = " << saved_fzrot2_ << "     read_zrot2_ = " << read_zrot2_;
    }
    else
        qDebug() << "PASSED   saved_fzrot2_ = " << saved_fzrot2_ << "     read_zrot2_ = " << read_zrot2_;

    if( read_xgain_.compare(saved_fxgain_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fxgain_ did not match the read_xgain_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fxgain_ = " << saved_fxgain_ << "     read_xgain_ = " << read_xgain_;
    }
    else
        qDebug() << "PASSED   saved_fxgain_ = " << saved_fxgain_ << "     read_xgain_ = " << read_xgain_;

    if( read_ygain_.compare(saved_fygain_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fygain_ did not match the read_ygain_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fygain_ = " << saved_fygain_ << "     read_ygain_ = " << read_ygain_;
    }
    else
        qDebug() << "PASSED   saved_fygain_ = " << saved_fygain_ << "     read_ygain_ = " << read_ygain_;

    if( read_gcoeff_.compare(saved_fgcoeff_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fgcoeff_ did not match the read_gcoeff_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fgcoeff_ = " << saved_fgcoeff_ << "     read_gcoeff_ = " << read_gcoeff_;
    }
    else
        qDebug() << "PASSED   saved_fgcoeff_ = " << saved_fgcoeff_ << "     read_gcoeff_ = " << read_gcoeff_;

    if( read_checksum_.compare(saved_fchecksum_, Qt::CaseInsensitive) != 0 )
    {
        errMsg += "\nThe saved_fchecksum_ did not match the read_checksum_.";
        passVerification_ = false;
        qDebug() << "FAILED   saved_fchecksum_ = " << saved_fchecksum_ << "     read_checksum_ = " << read_checksum_;
    }
    else
        qDebug() << "PASSED   saved_fchecksum_ = " << saved_fchecksum_ << "     read_checksum_ = " << read_checksum_;

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
    counts.push_back( counts_level_0kg_.toDouble()   );
    counts.push_back( counts_front_0kg_.toDouble()   );
    counts.push_back( counts_back_0kg_.toDouble()   );
    counts.push_back( counts_left_0kg_.toDouble()   );
    counts.push_back( counts_right_0kg_.toDouble()  );

    // 5kgs 10lbs
    counts.push_back( counts_level_5kg_.toDouble()  );         // 20pt
    counts.push_back( counts_front_5kg_.toDouble()  );
    counts.push_back( counts_back_5kg_.toDouble()  );
    counts.push_back( counts_left_5kg_.toDouble()  );          // 20pt
    counts.push_back( counts_right_5kg_.toDouble()  );         // 20pt

    // 10kgs 20lbs
    counts.push_back( counts_level_10kg_.toDouble() );         // 20pt
    counts.push_back( counts_front_10kg_.toDouble() );         // 20pt
    counts.push_back( counts_back_10kg_.toDouble() );          // 20pt
    counts.push_back( counts_left_10kg_.toDouble() );          // 20pt
    counts.push_back( counts_right_10kg_.toDouble() );        // 20pt

    // 15kgs 30lbs
    counts.push_back( counts_level_15kg_.toDouble() );
    counts.push_back( counts_front_15kg_.toDouble() );
    counts.push_back( counts_back_15kg_.toDouble() );
    counts.push_back( counts_left_15kg_.toDouble() );
    counts.push_back( counts_right_15kg_.toDouble() );

    return counts;
}
////////////////////////////////////////// END ////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::popErrorDialog(QString err_msg, bool popItNoMatterWhat)
{
    qDebug() << "popErrorDialog() err_msg:\n" << err_msg;

    // either one of these - or both - can be true depending on what I want to see


    if( popItNoMatterWhat || DISPLAY_SAVED_READ_FIT_PARAMS || DISPLAY_GENERATE_FIT_RESULTS || DISPLAY_WRITTEN_DATE)
    {
        warningError_ = new failure( this, err_msg );
        warningError_->move( 137, 77 );
        warningError_->show();
    }

    if( OUTPUT_SAVED_READ_FIT_PARAMS )
        qDebug() << err_msg;
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
bool calibui::countsValid(int step)
{
    bool result = true;
    //qDebug() << "Step: " << step;
    //qDebug() << "X: " << X_LEVEL_MAX_COUNTS << " <= " << acclXRawCounts_ << " <= " << X_LEVEL_MIN_COUNTS;
    //qDebug() << "Xn: " << X_NEGATIVE_TILTED_MAX_COUNTS << " <= " << acclXRawCounts_ << " <= " << X_NEGATIVE_TILTED_MIN_COUNTS;
    //qDebug() << "Xp: " << X_POSITIVE_TILTED_MAX_COUNTS << " <= " << acclXRawCounts_ << " <= " << X_POSITIVE_TILTED_MAX_COUNTS;

    //qDebug() << "Z: " << Z_LEVEL_MAX_COUNTS << " <= " << acclZRawCounts_ << " <= " << Z_LEVEL_MIN_COUNTS;
    //qDebug() << "Zn: " << Z_NEGATIVE_TILTED_MAX_COUNTS << " <= " << acclZRawCounts_ << " <= " << Z_NEGATIVE_TILTED_MIN_COUNTS;
    //qDebug() << "Zp: " << Z_POSITIVE_TILTED_MAX_COUNTS << " <= " << acclZRawCounts_ << " <= " << Z_POSITIVE_TILTED_MAX_COUNTS;

    switch (step)
    {
        case Flat0kgStep:
        case Flat15kgStep:
            result = (acclXRawCounts_ <= X_LEVEL_MAX_COUNTS && acclXRawCounts_ >= X_LEVEL_MIN_COUNTS &&
                      acclZRawCounts_ <= Z_LEVEL_MAX_COUNTS && acclZRawCounts_ >= Z_LEVEL_MIN_COUNTS);
            break;
        case Front0kgStep:
        case Front5kgStep:
        case Front15kgStep:
            result = (acclXRawCounts_ <= X_NEGATIVE_TILTED_MAX_COUNTS && acclXRawCounts_ >= X_NEGATIVE_TILTED_MIN_COUNTS &&
                      acclZRawCounts_ <= Z_LEVEL_MAX_COUNTS && acclZRawCounts_ >= Z_LEVEL_MIN_COUNTS);
            break;
        case Right0kgStep:
        case Right15kgStep:
            result = (acclXRawCounts_ <= X_LEVEL_MAX_COUNTS && acclXRawCounts_ >= X_LEVEL_MIN_COUNTS &&
                      acclZRawCounts_ <= Z_POSITIVE_TILTED_MAX_COUNTS && acclZRawCounts_ >= Z_POSITIVE_TILTED_MIN_COUNTS);
            break;
        case Back0kgStep:
        case Back5kgStep:
        case Back15kgStep:
            result = (acclXRawCounts_ <= X_POSITIVE_TILTED_MAX_COUNTS && acclXRawCounts_ >= X_POSITIVE_TILTED_MIN_COUNTS &&
                      acclZRawCounts_ <= Z_LEVEL_MAX_COUNTS && acclZRawCounts_ >= Z_LEVEL_MIN_COUNTS);
            break;
        case Left0kgStep:
        case Left15kgStep:
            result = (acclXRawCounts_ <= X_LEVEL_MAX_COUNTS && acclXRawCounts_ >= X_LEVEL_MIN_COUNTS &&
                      acclZRawCounts_ <= Z_NEGATIVE_TILTED_MAX_COUNTS && acclZRawCounts_ >= Z_NEGATIVE_TILTED_MIN_COUNTS);
            break;
        default:
            //result should still be true and we dont care about the case so do nothing
            break;
    }
    //result = true;
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::highlightDataLabels(bool acc_x, bool acc_y, bool acc_z, bool counts)
{
    if(acc_x)
    {
        ui->lbl_x->setStyleSheet("QLabel#lbl_x {"
        "background-color: orange;"
        "border-style:outset;"
        "border-radius:6px;"
        "border:3px solid gray;"
        "}");
    }
    else
    {
        ui->lbl_x->setStyleSheet("QLabel#lbl_x {"
        "background-image: url(:/images/none.png);"
        "border-style:outset;"
        "border-radius:6px;"
        "border:3px solid gray;"
        "}");
    }

    if(acc_y)
    {
        ui->lbl_y->setStyleSheet("QLabel#lbl_y {"
        "background-color: orange;"
        "border-style:outset;"
        "border-radius:6px;"
        "border:3px solid gray;"
        "}");

    }
    else
    {
        ui->lbl_y->setStyleSheet("QLabel#lbl_y {"
        "background-image: url(:/images/none.png);"
        "border-style:outset;"
        "border-radius:6px;"
        "border:3px solid gray;"
        "}");
    }

    if(acc_z)
    {
        ui->lbl_z->setStyleSheet("QLabel#lbl_z {"
        "background-color: orange;"
        "border-style:outset;"
        "border-radius:6px;"
        "border:3px solid gray;"
        "}");

    }
    else
    {
        ui->lbl_z->setStyleSheet("QLabel#lbl_z {"
        "background-image: url(:/images/none.png);"
        "border-style:outset;"
        "border-radius:6px;"
        "border:3px solid gray;"
        "}");
    }

    if(counts)
    {
        ui->lbl_counts->setStyleSheet("QLabel#lbl_counts {"
        "background-color: orange;"
        "border-style:outset;"
        "border-radius:6px;"
        "border:3px solid gray;"
        "}");

    }
    else
    {
        ui->lbl_counts->setStyleSheet("QLabel#lbl_counts {"
        "background-image: url(:/images/none.png);"
        "border-style:outset;"
        "border-radius:6px;"
        "border:3px solid gray;"
        "}");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///                                     NEW SEMI AUTO TILT RIG                                  ///
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
void calibui::setEnabledButtonState(bool state)
{
    if( TESTING_SENSORS )
    {
        nextPhoto_->setEnabled(true);       // always true if testing - lets us get all the way through the test in VM
        return;
    }

    nextPhoto_->setEnabled(state);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
calibui::SENSOR_TEST_RESULT calibui::testSensors(table_sensors sensors)
{
    bool isgood = false;
    unsigned int sensor_curent_reading[RIG_MAX_INPUT_SENSORS]   = {0};
    unsigned int sensor_to_test[RIG_MAX_INPUT_SENSORS]          = {0};
    SENSOR_TEST_RESULT test; 

    test.result = 1;//pass

    /* 
    *   sensor array mapping 
    *       0           1           2           3           4           5       6           7
    *   [position1],[position2],[position3],[position4],[Horizontal],[Tilt],[CarriageIn],[CarriageOut],
    */

    /* read all sensors */
    sensor_curent_reading[POSITION_ONE_SENSOR]         = getSensorPositionOne();
    sensor_curent_reading[POSITION_TWO_SENSOR]         = getSensorPositionTwo();
    sensor_curent_reading[POSITION_THREE_SENSOR]       = getSensorPositionThree();
    sensor_curent_reading[POSITION_FOUR_SENSOR]        = getSensorPositionFour();
    sensor_curent_reading[HORIZONTAL_SENSOR]           = getSensorTableHorizontal();
    sensor_curent_reading[TILT_SENSOR]                 = getSensorTableTilted();
    sensor_curent_reading[CARRIAGE_HOME_SENSOR]        = getSensorCarriageHome();
    sensor_curent_reading[CARRIAGE_PULLED_OUT_SENSOR]  = getSensorCarriagePulledOut();

    switch( sensors )
    {
        case SENSOR_START_TEST:
        {
            // flag the sensors we want to test. These all have to be true
            //sensor_to_test[CARRIAGE_PULLED_OUT_SENSOR]  = 1;
            sensor_to_test[HORIZONTAL_SENSOR]           = 1;
            sensor_to_test[POSITION_ONE_SENSOR]         = 1;
            
            //test for true
            for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
            {
                if(sensor_to_test[i] == 1)
                {
                    if(sensor_curent_reading[i] != 1)
                    {
                        test.result = 0;//fail
                        test.sensor = (RIG_INPUT_SENSORS)i;
                        test.value  = sensor_curent_reading[i];
                        break;
                    }
                }
            }

            if(test.result == 1)
            {
                memset(&sensor_to_test,0,sizeof(sensor_to_test));

                // flag the sensors we want to test. These all have to be false
                //sensor_to_test[CARRIAGE_HOME_SENSOR]  = 1;
                sensor_to_test[TILT_SENSOR]           = 1;
                sensor_to_test[POSITION_TWO_SENSOR]   = 1;
                sensor_to_test[POSITION_THREE_SENSOR] = 1;
                sensor_to_test[POSITION_FOUR_SENSOR]  = 1;

                //test for false
                for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
                {
                    if(sensor_to_test[i] == 1)
                    {
                        if(sensor_curent_reading[i] != 0)
                        {
                            test.result = 0;//fail
                            test.sensor = (RIG_INPUT_SENSORS)i;
                            test.value  = sensor_curent_reading[i];
                            break;
                        }
                    }
                }
            }
#if 0            
            if( getSensorCarriagePulledOut() && getSensorTableHorizontal() && getSensorPositionOne() )
                isgood = true;          // these all have to be true to start the test

            if( getSensorCarriageHome() || getSensorTableTilted() || getSensorPositionTwo() || getSensorPositionThree() || getSensorPositionFour() )
                isgood = false;         // if any of these are true, and they should not be, then we can't start test. Must be resolved
#endif
            break;
        }

        case SENSOR_PUSHEDIN_TEST:
        {
            if(sensor_curent_reading[CARRIAGE_HOME_SENSOR] == 0)            
            {
                test.result = 0;//fail
                test.sensor = CARRIAGE_HOME_SENSOR;
                test.value  = sensor_curent_reading[CARRIAGE_HOME_SENSOR];
            }

            if(test.result == 1)
            {
                if(sensor_curent_reading[CARRIAGE_PULLED_OUT_SENSOR] == 1)            
                {
                    test.result = 0;//fail
                    test.sensor = CARRIAGE_PULLED_OUT_SENSOR;
                    test.value  = sensor_curent_reading[CARRIAGE_PULLED_OUT_SENSOR];
                }   
            }

#if 0            
            if( getSensorCarriageHome() && !getSensorCarriagePulledOut() )
                isgood = true;
#endif
            break;
        }

        case SENSOR_LEVEL_POSTITION1_TEST:
        {
            // flag the sensors we want to test. These all have to be true
            sensor_to_test[CARRIAGE_HOME_SENSOR]        = 1;
            sensor_to_test[HORIZONTAL_SENSOR]           = 1;
            sensor_to_test[POSITION_ONE_SENSOR]         = 1;
            
            //test for true
            for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
            {
                if(sensor_to_test[i] == 1)
                {
                    if(sensor_curent_reading[i] != 1)
                    {
                        test.result = 0;//fail
                        test.sensor = (RIG_INPUT_SENSORS)i;
                        test.value  = sensor_curent_reading[i];
                        break;
                    }
                }
            }

            if(test.result == 1)
            {
                memset(&sensor_to_test,0,sizeof(sensor_to_test));

                // flag the sensors we want to test. These all have to be false
                sensor_to_test[TILT_SENSOR]           = 1;
                sensor_to_test[POSITION_TWO_SENSOR]   = 1;
                sensor_to_test[POSITION_THREE_SENSOR] = 1;
                sensor_to_test[POSITION_FOUR_SENSOR]  = 1;

                //test for false
                for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
                {
                    if(sensor_to_test[i] == 1)
                    {
                        if(sensor_curent_reading[i] != 0)
                        {
                            test.result = 0;//fail
                            test.sensor = (RIG_INPUT_SENSORS)i;
                            test.value  = sensor_curent_reading[i];
                            break;
                        }
                    }
                }
            }


#if 0            
            if( getSensorCarriageHome() && getSensorTableHorizontal() && getSensorPositionOne() )
                isgood = true;          // these all have to be true to take level position1 state

            if( getSensorTableTilted() || getSensorPositionTwo() || getSensorPositionThree() || getSensorPositionFour() )
                isgood = false;         // if any of these are true, and they should not be, then we can't start test. Must be resolved
#endif
            break;
        }

        case SENSOR_CARRIAGE_HOME:
        {
            if(sensor_curent_reading[CARRIAGE_HOME_SENSOR] == 0)            
            {
                test.result = 0;//fail
                test.sensor = CARRIAGE_HOME_SENSOR;
                test.value  = sensor_curent_reading[CARRIAGE_HOME_SENSOR];
            }
                
#if 0            
            if( getSensorCarriageHome() )
                isgood = true;
#endif
            break;
        }

        case SENSOR_TABLE_TILTED:
        {
            if(sensor_curent_reading[TILT_SENSOR] == 0)            
            {
                test.result = 0;//fail
                test.sensor = TILT_SENSOR;
                test.value  = sensor_curent_reading[TILT_SENSOR];
            }

            if(test.result == 1)//passing
            {
                if(sensor_curent_reading[HORIZONTAL_SENSOR] == 1)            
                {
                    test.result = 0;//fail
                    test.sensor = HORIZONTAL_SENSOR;
                    test.value  = sensor_curent_reading[HORIZONTAL_SENSOR];
                }   
            }
#if 0            
            if( getSensorTableTilted() && !getSensorTableHorizontal() )
                isgood = true;
#endif
            break;
        }

        case SENSOR_TILTED_POSTITION1_TEST:
        {
            // flag the sensors we want to test. These all have to be true
            sensor_to_test[CARRIAGE_HOME_SENSOR]        = 1;
            sensor_to_test[TILT_SENSOR]                 = 1;
            sensor_to_test[POSITION_ONE_SENSOR]         = 1;
            
            //test for true
            for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
            {
                if(sensor_to_test[i] == 1)
                {
                    if(sensor_curent_reading[i] != 1)
                    {
                        test.result = 0;//fail
                        test.sensor = (RIG_INPUT_SENSORS)i;
                        test.value  = sensor_curent_reading[i];
                        break;
                    }
                }
            }

            if(test.result == 1)
            {
                memset(&sensor_to_test,0,sizeof(sensor_to_test));

                // flag the sensors we want to test. These all have to be false
                sensor_to_test[HORIZONTAL_SENSOR]     = 1;
                sensor_to_test[POSITION_TWO_SENSOR]   = 1;
                sensor_to_test[POSITION_THREE_SENSOR] = 1;
                sensor_to_test[POSITION_FOUR_SENSOR]  = 1;

                //test for false
                for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
                {
                    if(sensor_to_test[i] == 1)
                    {
                        if(sensor_curent_reading[i] != 0)
                        {
                            test.result = 0;//fail
                            test.sensor = (RIG_INPUT_SENSORS)i;
                            test.value  = sensor_curent_reading[i];
                            break;
                        }
                    }
                }
            }

#if 0            
            if( getSensorCarriageHome() && getSensorTableTilted() && getSensorPositionOne() )
                isgood = true;          // these all have to be true to take level position1 state

            if( getSensorTableHorizontal() || getSensorPositionTwo() || getSensorPositionThree() || getSensorPositionFour() )
                isgood = false;         // if any of these are true, and they should not be, then we can't start test. Must be resolved
#endif
            break;
        }

        case SENSOR_TILTED_POSTITION2_TEST:
        {
            // flag the sensors we want to test. These all have to be true
            sensor_to_test[CARRIAGE_HOME_SENSOR]        = 1;
            sensor_to_test[TILT_SENSOR]                 = 1;
            sensor_to_test[POSITION_TWO_SENSOR]         = 1;
            
            //test for true
            for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
            {
                if(sensor_to_test[i] == 1)
                {
                    if(sensor_curent_reading[i] != 1)
                    {
                        test.result = 0;//fail
                        test.sensor = (RIG_INPUT_SENSORS)i;
                        test.value  = sensor_curent_reading[i];
                        break;
                    }
                }
            }

            if(test.result == 1)
            {
                memset(&sensor_to_test,0,sizeof(sensor_to_test));

                // flag the sensors we want to test. These all have to be false
                sensor_to_test[HORIZONTAL_SENSOR]     = 1;
                sensor_to_test[POSITION_ONE_SENSOR]   = 1;
                sensor_to_test[POSITION_THREE_SENSOR] = 1;
                sensor_to_test[POSITION_FOUR_SENSOR]  = 1;

                //test for false
                for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
                {
                    if(sensor_to_test[i] == 1)
                    {
                        if(sensor_curent_reading[i] != 0)
                        {
                            test.result = 0;//fail
                            test.sensor = (RIG_INPUT_SENSORS)i;
                            test.value  = sensor_curent_reading[i];
                            break;
                        }
                    }
                }
            }
#if 0
            if( getSensorCarriageHome() && getSensorTableTilted() && getSensorPositionTwo() )
                isgood = true;          // these all have to be true to take level position1 state

            if( getSensorTableHorizontal() || getSensorPositionOne() || getSensorPositionThree() || getSensorPositionFour() )
                isgood = false;         // if any of these are true, and they should not be, then we can't start test. Must be resolved
#endif
            break;
        }

        case SENSOR_TILTED_POSTITION3_TEST:
        {
            // flag the sensors we want to test. These all have to be true
            sensor_to_test[CARRIAGE_HOME_SENSOR]        = 1;
            sensor_to_test[TILT_SENSOR]                 = 1;
            sensor_to_test[POSITION_THREE_SENSOR]       = 1;
            
            //test for true
            for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
            {
                if(sensor_to_test[i] == 1)
                {
                    if(sensor_curent_reading[i] != 1)
                    {
                        test.result = 0;//fail
                        test.sensor = (RIG_INPUT_SENSORS)i;
                        test.value  = sensor_curent_reading[i];
                        break;
                    }
                }
            }

            if(test.result == 1)
            {
                memset(&sensor_to_test,0,sizeof(sensor_to_test));

                // flag the sensors we want to test. These all have to be false
                sensor_to_test[HORIZONTAL_SENSOR]     = 1;
                sensor_to_test[POSITION_ONE_SENSOR]   = 1;
                sensor_to_test[POSITION_TWO_SENSOR]   = 1;
                sensor_to_test[POSITION_FOUR_SENSOR]  = 1;

                //test for false
                for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
                {
                    if(sensor_to_test[i] == 1)
                    {
                        if(sensor_curent_reading[i] != 0)
                        {
                            test.result = 0;//fail
                            test.sensor = (RIG_INPUT_SENSORS)i;
                            test.value  = sensor_curent_reading[i];
                            break;
                        }
                    }
                }
            }
#if 0            
            if( getSensorCarriageHome() && getSensorTableTilted() && getSensorPositionThree() )
                isgood = true;          // these all have to be true to take level position1 state

            if( getSensorTableHorizontal() || getSensorPositionTwo() || getSensorPositionOne() || getSensorPositionFour() )
                isgood = false;         // if any of these are true, and they should not be, then we can't start test. Must be resolved
#endif
            break;
        }

        case SENSOR_TILTED_POSTITION4_TEST:
        {
            // flag the sensors we want to test. These all have to be true
            sensor_to_test[CARRIAGE_HOME_SENSOR]        = 1;
            sensor_to_test[TILT_SENSOR]                 = 1;
            sensor_to_test[POSITION_FOUR_SENSOR]        = 1;
            
            //test for true
            for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
            {
                if(sensor_to_test[i] == 1)
                {
                    if(sensor_curent_reading[i] != 1)
                    {
                        test.result = 0;//fail
                        test.sensor = (RIG_INPUT_SENSORS)i;
                        test.value  = sensor_curent_reading[i];
                        break;
                    }
                }
            }

            if(test.result == 1)
            {
                memset(&sensor_to_test,0,sizeof(sensor_to_test));

                // flag the sensors we want to test. These all have to be false
                sensor_to_test[HORIZONTAL_SENSOR]     = 1;
                sensor_to_test[POSITION_ONE_SENSOR]   = 1;
                sensor_to_test[POSITION_TWO_SENSOR]   = 1;
                sensor_to_test[POSITION_THREE_SENSOR]  = 1;

                //test for false
                for(int i=0; i<RIG_MAX_INPUT_SENSORS; i++)
                {
                    if(sensor_to_test[i] == 1)
                    {
                        if(sensor_curent_reading[i] != 0)
                        {
                            test.result = 0;//fail
                            test.sensor = (RIG_INPUT_SENSORS)i;
                            test.value  = sensor_curent_reading[i];
                            break;
                        }
                    }
                }
            }
#if 0
            if( getSensorCarriageHome() && getSensorTableTilted() && getSensorPositionFour() )
                isgood = true;          // these all have to be true to take level position1 state

            if( getSensorTableHorizontal() || getSensorPositionTwo() || getSensorPositionThree() || getSensorPositionOne() )
                isgood = false;         // if any of these are true, and they should not be, then we can't start test. Must be resolved
#endif
            break;
        }

        default:
            break;
    }

    if( TESTING_SENSORS )
        isgood = true;

    return test;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int calibui::getSensorCarriageHome()
{
    int isHomePosition = 0;
    int at_home_logic = 0;//define the logic state that is equal to home position
    QString io( sensorPath_ );
    QString gpio_dir( "tRPlateEngIn");
    io.append( gpio_dir );
    io.append( "/" + sensorFile_ );


    if( QFile::exists( io ) )
    {
        QFile f( io );
        if ( f.open( QFile::ReadOnly) )
        {
            QString filedata = f.readAll();            // better just be one digit
            QString sensor_setting = filedata.trimmed();
            if ( sensor_setting.size() == 1 )
                isHomePosition = sensor_setting.toInt();

            if ( isHomePosition == at_home_logic )
            {
                qDebug() << "The sliding carriage is in the HOME position";
                isHomePosition = 1;
            }
            else
            {
                qDebug() << "The sliding carriage is not in the HOME position";
                isHomePosition = 0;
            }
        }
        f.close();
    }
    else
    {
        // TODO: need to pop something up on the screen saying the sensor could not be read
        qDebug() << "The file carriagehome could not be read.";
        isHomePosition = 0;
    }

    return isHomePosition;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int calibui::getSensorCarriagePulledOut()
{
    int isPulledOutPosition = 0;
    int at_pulled_out_logic = 0;//define the logic state that is equal to pulled out position
    QString io( sensorPath_ );
    QString gpio_dir( "tRPlateEngOut");
    io.append( gpio_dir );
    io.append( "/" + sensorFile_ );

    if( QFile::exists( io ) )
    {
        QFile f( io );
        if ( f.open( QFile::ReadOnly) )
        {
            QString filedata = f.readAll();                 // if I need error checking go get from sensor one
            isPulledOutPosition = filedata.toInt();

            if ( isPulledOutPosition == at_pulled_out_logic )
            {
                qDebug() << "The sliding carriage is in the PULLED OUT position";
                isPulledOutPosition = 1;
            }
            else
            {
                qDebug() << "The sliding carriage is not in the PULLED OUT position";
                isPulledOutPosition = 0;
            }
                
        }
        f.close();
    }
    else
    {
        // TODO: need to pop something up on the screen saying the sensor could not be read
          qDebug() << "The file carriagepulled could not be read.";
          isPulledOutPosition = 0;
    }

    return isPulledOutPosition;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int calibui::getSensorTableHorizontal()
{
    int isHorizontalPosition = 0;
    int at_horizontal_logic = 0;//define the logic that is equal to horizontal position
    QString io( sensorPath_ );
    QString gpio_dir( "tRPlateLevel");
    io.append( gpio_dir );
    io.append( "/" + sensorFile_ );

    if( QFile::exists( io ) )
    {
        QFile f( io );
        if ( f.open( QFile::ReadOnly) )
        {
            QString filedata = f.readAll();                 // if I need error checking go get from sensor one
            isHorizontalPosition = filedata.toInt();

            if ( isHorizontalPosition == at_horizontal_logic )
            {
                qDebug() << "The sliding carriage is in the HORIZONTAL position";
                isHorizontalPosition = 1;
            }
            else
            {
                qDebug() << "The sliding carriage is not in the HORIZONTAL position";
                isHorizontalPosition = 0;
            }
                
        }
        f.close();
    }
    else
    {
        // TODO: need to pop something up on the screen saying the sensor could not be read
        qDebug() << "The file platehorizontal could not be read.";
        isHorizontalPosition = 0;
    }

    return isHorizontalPosition;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int calibui::getSensorTableTilted()
{
    int isTiltedPosition = 0;
    int at_position_tilted_logic = 0;//define the logic that is equal to position tilted
    QString io( sensorPath_ );
    QString gpio_dir( "tRPlateTilt");
    io.append( gpio_dir );
    io.append( "/" + sensorFile_ );

    if( QFile::exists( io ) )
    {
        QFile f( io );
        if ( f.open( QFile::ReadOnly) )
        {
            QString filedata = f.readAll();                 // if I need error checking go get from sensor one
            isTiltedPosition = filedata.toInt();

            if ( isTiltedPosition == at_position_tilted_logic )
            {
                isTiltedPosition = 1;
                qDebug() << "The sliding carriage is in the TILTED position";
            }                
            else
            {
                isTiltedPosition = 0;
                qDebug() << "The sliding carriage is not in the TILTED position";
            }
                
        }
        f.close();
    }
    else
    {
        isTiltedPosition = 0;
        // TODO: need to pop something up on the screen saying the sensor could not be read
        qDebug() << "The file platetilted could not be read.";
    }

    return isTiltedPosition;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int calibui::getSensorPositionOne()
{
    int isPositionOne = 0;
    int at_position_one_logic = 0;//define the logic that is equal to position 1
    QString io( sensorPath_ );
    QString gpio_dir( "tRPlate0");
    io.append( gpio_dir );
    io.append( "/" + sensorFile_ );

    if( QFile::exists( io ) )
    {
        QFile f( io );
        if ( f.open( QFile::ReadOnly) )
        {
            QString filedata = f.readAll();                 // if I need error checking go get from sensor one
            isPositionOne = filedata.toInt();

            if ( isPositionOne == at_position_one_logic )
            {
                isPositionOne = 1;
                qDebug() << "The sliding carriage is in the POSITION ONE position";
            }
            else
            {
                isPositionOne = 0;
                qDebug() << "The sliding carriage is not in the POSITION ONE position";
            }
                
        }
        f.close();
    }
    else
    {
        isPositionOne = 0;
        // TODO: need to pop something up on the screen saying the sensor could not be read
        qDebug() << "The file positionone could not be read.";
    }

    return isPositionOne;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int calibui::getSensorPositionTwo()
{
    int isPositionTwo = 0;
    int at_position_two_logic = 0;//define the logic that is equal to position 2
    QString io( sensorPath_ );
    QString gpio_dir( "tRPlate90");
    io.append( gpio_dir );
    io.append( "/" + sensorFile_ );

    if( QFile::exists( io ) )
    {
        QFile f( io );
        if ( f.open( QFile::ReadOnly) )
        {
            QString filedata = f.readAll();                 // if I need error checking go get from sensor one
            isPositionTwo = filedata.toInt();

            if ( isPositionTwo == at_position_two_logic )
            {
                isPositionTwo = 1;
                qDebug() << "The sliding carriage is in the POSITION TWO position";
            }                
            else
            {
                isPositionTwo = 0;
                qDebug() << "The sliding carriage is not in the POSITION TWO position";
            }
                
        }
        f.close();
    }
    else
    {
        isPositionTwo = 0;
        // TODO: need to pop something up on the screen saying the sensor could not be read
        qDebug() << "The file positiontwo could not be read.";
    }

    return isPositionTwo;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int calibui::getSensorPositionThree()
{
    int isPositionThree = 0;
    int at_position_three_logic = 0;//define the logic that is equal to position 3
    QString io( sensorPath_ );
    QString gpio_dir( "tRPlate180");
    io.append( gpio_dir );
    io.append( "/" + sensorFile_ );

    if( QFile::exists( io ) )
    {
        QFile f( io );
        if ( f.open( QFile::ReadOnly) )
        {
            QString filedata = f.readAll();                 // if I need error checking go get from sensor one
            isPositionThree = filedata.toInt();

            if ( isPositionThree == at_position_three_logic )
            {
                isPositionThree = 1;
                qDebug() << "The sliding carriage is in the POSITION THREE position";
            }               
            else
            {
                isPositionThree = 0;
                qDebug() << "The sliding carriage is not in the POSITION THREE position";
            }
                
        }
        f.close();
    }
    else
    {
        isPositionThree = 0;
        // TODO: need to pop something up on the screen saying the sensor could not be read
        qDebug() << "The file positionthree could not be read.";
    }

    return isPositionThree;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int calibui::getSensorPositionFour()
{
    int isPositionFour = 0;
    int at_position_four_logic = 0;//define the logic that is equal to position 4
    QString io( sensorPath_ );
    QString gpio_dir( "tRPlate270");
    io.append( gpio_dir );
    io.append( "/" + sensorFile_ );

    if( QFile::exists( io ) )
    {
        QFile f( io );
        if ( f.open( QFile::ReadOnly) )
        {
            QString filedata = f.readAll();                 // if I need error checking go get from sensor one
            isPositionFour = filedata.toInt();

            if ( isPositionFour == at_position_four_logic )
            {
                isPositionFour = 1;
                qDebug() << "The sliding carriage is in the POSITION FOUR position";
            }                
            else
            {
                isPositionFour = 0;                   
                qDebug() << "The sliding carriage is not in the POSITION FOUR position";
            }

        }
        f.close();
    }
    else
    {
        isPositionFour = 0;
        // TODO: need to pop something up on the screen saying the sensor could not be read
        qDebug() << "The file positionfour could not be read.";
    }

    return isPositionFour;
}

int calibui::setSensorDUTPowerEnable(int onOff)
{
    int isWritten = 0;
    QString io( sensorPath_ );
    QString gpio_dir( "tRDutPwrEn");
    io.append( gpio_dir );
    io.append( "/" + sensorFile_ );

    if( QFile::exists( io ) )
    {
        QFile f( io );
        if ( f.open( QFile::WriteOnly ) )
        {
            isWritten = 1;
            QByteArray buf;
            buf.append( QString::number( onOff ) );

            f.write( buf );
            f.close();

            if ( isLinux() )
                system( "sync" );         // flush the file to disc
        }
    }
    else
    {
        // TODO: need to pop something up on the screen saying the sensor could not be read
          qDebug() << "The file carriagepulled could not be read.";
    }

    return isWritten;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool calibui::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
bool calibui::autoStartNextStep(void)
{
    /*
    *   Determine if we can go ahead
    *   and automatically advance to next
    *   step without user intervention.
    */
   bool auto_start = false;

   switch(step_)
   {
       case Flat15kgStep:
       case Back5kgStep:
       case Back15kgStep:
       case Left15kgStep:
       case Front5kgStep:
       case Front15kgStep:
       case Right15kgStep:
       {
           auto_start = true;

           break;
       }

   }

   return auto_start;

}
