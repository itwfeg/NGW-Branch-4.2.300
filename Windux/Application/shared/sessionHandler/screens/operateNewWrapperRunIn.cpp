/* @file operateNewWrapperRunIn.cpp
 * operateNewWrapperRunIn
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for putting the wrapper in run-in mode
 */

#include "operateNewWrapperRunIn.h"

#include "ui_operateNewWrapperRunIn.h"

#include "../sessionmainwindow.h"

#include "../modes/operatemode.h"
#include "../platform/python.h"

#include <QDebug>
#include <QDir>
#include <QTimer>
#include <backgroundwidgetstyles.h>
#include <palette.h>
#include <logmanager.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <ScaleClasses.h>

#include "serviceNewWrapperSetTimings.h"
#include "serviceNewWrapperEditBox.h"

#include "../dialogs/messageboxdialog.h"

const static bool TESTING = false;
static const bool DEMO_ALL_GOOD_MODE = false;

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
operateNewWrapperRunIn::operateNewWrapperRunIn(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::operateNewWrapperRunIn),
    screenLoaded_( false )
{
    ui->setupUi(this);
    scaleScreen();

    session_->scaleConfig->internalConfig.isRunMode = 2;

    initialize();

    translate();
    screenLoaded_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
operateNewWrapperRunIn::~operateNewWrapperRunIn()
{
    saveCycleCountsToFile();

    // stop actuation
    session_->send( Wrapper, WrEvolTest, "28,0" ); // CycleActuatorsTest, Stop (0)

    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::translate()
{
    ui->retranslateUi(this);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::initialize()
{
    // SOF-4216
    //----------------------------------
    // Request the Maximum Run-In Cycles
    //----------------------------------
	strMaxRunInCycles = "12000";

    // SOF-4217
    maxRunInCycles_ = 0;
    // request serial number
    session_->send( MfgTestDev, RequestMfgData );   // 164,3
    // request version info
    session_->send( SessionDev, SnVersionInfo );    // 100,2
    // request wrapper info
    session_->send(WrapperSystem, WrSysReqStatus);  // 10,13



    // the date the last time timings were saved

    //lastSaved_                  = ui->lastSaved;
    //lastSavedLabel_             = ui->lastSavedLabel;

    lblCycleCountsTitle_        = ui->lblCycleCountsTitle;
    cycleCounts_                = ui->cycleCounts;

    knifeUp_                    = ui->knifeUp;
    knifeDown_                  = ui->knifeDown;
    gripperNarrowClose_         = ui->gripperNarrowClose;
    gripperNarrowOpen_          = ui->gripperNarrowOpen;
    gripperWideLeftClose_       = ui->gripperWideLeftClose;
    gripperWideLeftOpen_        = ui->gripperWideLeftOpen;
    gripperWideRightClose_      = ui->gripperWideRightClose;
    gripperWideRightOpen_       = ui->gripperWideRightOpen;
    sideClampLeftClose_         = ui->sideClampLeftClose;
    sideClampLeftOpen_          = ui->sideClampLeftOpen;
    sideClampRightClose_        = ui->sideClampRightClose;
    sideClampRightOpen_         = ui->sideClampRightOpen;
    wandUp_                     = ui->wandUp;
    wandDown_                   = ui->wandDown;
    filmSelectorUp_             = ui->filmSelectorUp;
    filmSelectorDown_           = ui->filmSelectorDown;
    mainPressure_               = ui->mainPressure;
    linePressure_               = ui->linePressure;
    vacuum_                     = ui->vacuum;
    applierHand_                = ui->applierHand;


    // save off labels for future use
    valueLabels_[DomEvolWrapperEventCapture::KnifeUpEvent] = knifeUp_;
    valueLabels_[DomEvolWrapperEventCapture::KnifeDownEvent] = knifeDown_;
    valueLabels_[DomEvolWrapperEventCapture::GripperNarrowCloseEvent] = gripperNarrowClose_;
    valueLabels_[DomEvolWrapperEventCapture::GripperNarrowOpenEvent] = gripperNarrowOpen_;
    valueLabels_[DomEvolWrapperEventCapture::GripperWideLeftCloseEvent] = gripperWideLeftClose_;
    valueLabels_[DomEvolWrapperEventCapture::GripperWideLeftOpenEvent] = gripperWideLeftOpen_;
    valueLabels_[DomEvolWrapperEventCapture::GripperWideRightCloseEvent] = gripperWideRightClose_;
    valueLabels_[DomEvolWrapperEventCapture::GripperWideRightOpenEvent] = gripperWideRightOpen_;
    valueLabels_[DomEvolWrapperEventCapture::SideClampLeftCloseEvent] = sideClampLeftClose_;
    valueLabels_[DomEvolWrapperEventCapture::SideClampLeftOpenEvent] = sideClampLeftOpen_;
    valueLabels_[DomEvolWrapperEventCapture::SideClampRightCloseEvent] = sideClampRightClose_;
    valueLabels_[DomEvolWrapperEventCapture::SideClampRightOpenEvent] = sideClampRightOpen_;
    valueLabels_[DomEvolWrapperEventCapture::WandUpEvent] = wandUp_;
    valueLabels_[DomEvolWrapperEventCapture::WandDownEvent] = wandDown_;
    valueLabels_[DomEvolWrapperEventCapture::FilmSelectorUpEvent] = filmSelectorUp_;
    valueLabels_[DomEvolWrapperEventCapture::FilmSelectorDownEvent] = filmSelectorDown_;
    valueLabels_[DomEvolWrapperEventCapture::TankPressureReadingEvent] = mainPressure_;
    valueLabels_[DomEvolWrapperEventCapture::RegulatedPressureReadingEvent] = linePressure_;
    valueLabels_[DomEvolWrapperEventCapture::ApplierVacuumReadingEvent] = vacuum_;
    valueLabels_[DomEvolWrapperEventCapture::ApplierHandHomeEvent] = applierHand_;

    /// Create vector of vectors to check for range values
    /// [0] = Lower Limit , [1] = Upper Limit
    validateRange_[DomEvolWrapperEventCapture::KnifeUpEvent][0]                       =  80; // Knife UP LOWER LIMIT
    validateRange_[DomEvolWrapperEventCapture::KnifeUpEvent][1]                       = 110; // Knife UP UPPER LIMIT
    validateRange_[DomEvolWrapperEventCapture::KnifeDownEvent][0]                     = 280; // Knife DOWN
    validateRange_[DomEvolWrapperEventCapture::KnifeDownEvent][1]                     = 400; //
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowCloseEvent][0]            =  20; // Gripper Narrow CLOSE
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowCloseEvent][1]            =  40;
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowOpenEvent][0]             =  65; // Gripper Narrow OPEN
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowOpenEvent][1]             =  95;
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftCloseEvent][0]          =  20; // Gripper Wide Left CLOSE
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftCloseEvent][1]          =  40;
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftOpenEvent][0]           =  75; // Gripper Wide Left OPEN
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftOpenEvent][1]           = 110;
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightCloseEvent][0]         =  20; // Gripper Wide Right CLOSE
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightCloseEvent][1]         =  40;
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightOpenEvent][0]          =  75; // Gripper Wide Right OPEN
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightOpenEvent][1]          = 110;
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftCloseEvent][0]            =  40; // Side Clamp Left CLOSE
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftCloseEvent][1]            =  70;
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftOpenEvent][0]             = 120; // Side Clamp Left OPEN
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftOpenEvent][1]             = 160;
    validateRange_[DomEvolWrapperEventCapture::SideClampRightCloseEvent][0]           =  40; // Side Clamp Right CLOSE
    validateRange_[DomEvolWrapperEventCapture::SideClampRightCloseEvent][1]           =  70;
    validateRange_[DomEvolWrapperEventCapture::SideClampRightOpenEvent][0]            = 120; // Side Clamp Right OPEN
    validateRange_[DomEvolWrapperEventCapture::SideClampRightOpenEvent][1]            = 160;
    validateRange_[DomEvolWrapperEventCapture::WandUpEvent][0]                        = 200; // Wand UP - 400->200 updated 04-13-2026
    validateRange_[DomEvolWrapperEventCapture::WandUpEvent][1]                        = 325; // Wand UP - 525->325 updated 04-13-2026
    validateRange_[DomEvolWrapperEventCapture::WandDownEvent][0]                      = 625; // Wand DOWN 675->625 04-14-2026
    validateRange_[DomEvolWrapperEventCapture::WandDownEvent][1]                      = 775;
    validateRange_[DomEvolWrapperEventCapture::FilmSelectorUpEvent][0]                = 300; // Film Selector UP
    validateRange_[DomEvolWrapperEventCapture::FilmSelectorUpEvent][1]                = 400;
    validateRange_[DomEvolWrapperEventCapture::FilmSelectorDownEvent][0]              = 120; // Film Selector DOWN
    validateRange_[DomEvolWrapperEventCapture::FilmSelectorDownEvent][1]              = 160;
    validateRange_[DomEvolWrapperEventCapture::TankPressureReadingEvent][0]           =  88; // Tank Pressure
    validateRange_[DomEvolWrapperEventCapture::TankPressureReadingEvent][1]           = 125;
    validateRange_[DomEvolWrapperEventCapture::RegulatedPressureReadingEvent][0]      =  45; // Regulated Pressure
    validateRange_[DomEvolWrapperEventCapture::RegulatedPressureReadingEvent][1]      =  55;
    validateRange_[DomEvolWrapperEventCapture::ApplierVacuumReadingEvent][0]          =   4; // Vacuum
    validateRange_[DomEvolWrapperEventCapture::ApplierVacuumReadingEvent][1]          =  30;
    validateRange_[DomEvolWrapperEventCapture::ApplierHandHomeEvent][0]               =  60; // Applier Hand 120->60 04-14-2026
    validateRange_[DomEvolWrapperEventCapture::ApplierHandHomeEvent][1]               = 220; // Applier Hand 200->220 04-14-2026
    // not displayed on this screen
    validateRange_[DomEvolWrapperEventCapture::MainACVoltageReadingEvent][0]          = 198; // Main AC Input Voltage
    validateRange_[DomEvolWrapperEventCapture::MainACVoltageReadingEvent][1]          = 242;

    currentTimings_[DomEvolWrapperEventCapture::KnifeUpEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::KnifeDownEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::GripperNarrowCloseEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::GripperNarrowOpenEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideLeftCloseEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideLeftOpenEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideRightCloseEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideRightOpenEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::SideClampLeftCloseEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::SideClampLeftOpenEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::SideClampRightCloseEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::SideClampRightOpenEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::WandUpEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::WandDownEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::FilmSelectorUpEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::FilmSelectorDownEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::TankPressureReadingEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::RegulatedPressureReadingEvent] = "0";      // 18
    currentTimings_[DomEvolWrapperEventCapture::ApplierVacuumReadingEvent] = "0";          // 19
    currentTimings_[DomEvolWrapperEventCapture::ApplierHandHomeEvent] = "0";

    connect( lblCycleCountsTitle_, SIGNAL( clicked() ), SLOT( onCycleCountsClicked() ) );
    connect( cycleCounts_,         SIGNAL( clicked() ), SLOT( onCycleCountsClicked() ) );

    readCycleCountsFromFile();

    // start actuation
    session_->send( Wrapper, WrEvolTest, "28,1" ); // CycleActuatorsTest, Start (1)

    saveCycleCountsTimer_ = new QTimer( this );
    saveCycleCountsTimer_->setInterval( 60000 );
    saveCycleCountsTimer_->setSingleShot( false );

    connect( saveCycleCountsTimer_, SIGNAL(timeout()),        SLOT( onTimerTimeout()) );

    connect( ui->butPrintReport,    SIGNAL(clicked()),  this, SLOT( generateRunInReport() ) );
    saveCycleCountsTimer_->start();

    connect( ui->butBackToTimings, SIGNAL(clicked()),   this, SLOT(onBackToTimings()));
    ui->butBackToTimings->setVisible( false );

    if( TESTING )
    {
        fakeCycleTimer_ = new QTimer( this );
        fakeCycleTimer_->setInterval( 1500 );
        fakeCycleTimer_->setSingleShot( false );

        connect( fakeCycleTimer_, SIGNAL(timeout()), SLOT( onTimerTimeout() ) );

        fakeCycleTimer_->start();
    }

    // by default, we are going to say we passed the Run In Tests - up until we dont
    session_->scaleConfig->internalConfig.runInTestState = TimingsTestState::Test_Passed;
    session_->scaleConfig->internalConfig.runInFailures = "";
    session_->scaleConfig->internalConfig.runInSensorFailures = "";

    // SOF-4216
    if( session_->scaleConfig->internalConfig.isTimingsActive )
        ui->butBackToTimings->setVisible( true );
    else
        checkForLimitCycles();              // dont need to check this if we came from Timings - but otherwise check it
}

// SOF-4216
void operateNewWrapperRunIn::checkForLimitCycles()
{
    LOG_CLASS_INFO( "WrapperServiceModeDiagnosticsPanel", "checkForLimitCycles" );

    //-----------------------------------
    // SOF-4216: Stop after 12,000 cyles?
    //-----------------------------------

    int maxCycles = strMaxRunInCycles.toInt();

    QString msgStr = tr( "Do you want to stop Manufacturing Run-In after " ) + QString::number( maxCycles ) + tr( " cycles?" );

    MessageBoxDialog dialog( mainWindow_ );
    dialog.setTitle( tr( "MANUFACTURING RUN IN" ) );
    dialog.setText( msgStr );
    dialog.setAllowOverlayClickDismiss( false );
    dialog.setStandardButtons( QDialogButtonBox::Yes | QDialogButtonBox::No );

    int result = dialog.exec();

    if( QDialog::Accepted == result )
    {
        LOG_DEBUG( "Set Manufacturing Run-in to %d cycles", maxCycles );

        setRunInMaxCycles( maxCycles );
    }
}

void operateNewWrapperRunIn::setRunInMaxCycles( int maxCycles )
{
    LOG_CLASS_INFO( "operateNewWrapperRunIn", "setRunInMaxCycles" );

    maxRunInCycles_ = maxCycles;

    LOG_DEBUG( "Set Max Run-In Cycles: %d", maxRunInCycles_ );
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::onTimerTimeout()
{
    // once a minute save off the cycle counts
    if ( saveCycleCountsTimer_ == sender() )
    {
        saveCycleCountsToFile();
    }

    // fake cycle count timeout
    else if ( fakeCycleTimer_ == sender() )
    {
        createFakeCycleCounter();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::highlight(QLabel* label, int status)
{
    switch( status )
    {
    case 0:
        highlightGreen(label);
        break;
    case 1:
        if( DEMO_ALL_GOOD_MODE )
            highlightGreen(label);
        else
            highlightYellow(label);
        break;
    case 2:
        if( DEMO_ALL_GOOD_MODE )
            highlightGreen(label);
        else
            highlightRed(label);
        break;
    default:
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::highlightGreen(QLabel* label)
{
    label->setStyleSheet("background-color: green; border-radius: 10px;");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::highlightYellow(QLabel* label)
{
    if( DEMO_ALL_GOOD_MODE )
        highlightGreen(label);
    else
        label->setStyleSheet("background-color: yellow; border-radius: 10px;");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::highlightRed(QLabel* label)
{
    if( DEMO_ALL_GOOD_MODE )
        highlightGreen(label);
    else
    {
        LOG_CLASS_INFO( "operateNewWrapperRunIn", "highlightRed" );
        LOG_DEBUG("highlightRed() Flip RunInTest red - is this being called?", NULL );

        // Kraig - what if we have network connectivity on and field turns red then we wont see it.  Flip it magneto/fuchsia instead
        label->setStyleSheet("background-color: red ; border-radius: 10px;");       // background-color:rgb(255,0,0) ff0000

        // all tests were good, by default, until we get a FAILED status.  Not a warning, but FAILED, and we flip this flag
        session_->scaleConfig->internalConfig.runInTestState = TimingsTestState::Test_Failed;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::updateValues(MadpEvolWrapperMotorProfile msg)
{
    int status = msg.mechStatus().toInt();

    switch( msg.evolMechanism().toInt() )
    {
    case Primary:
        ui->lblPrimary->setText(msg.motorCapacityUsage());
        highlight(ui->lblPrimary, status);
        break;
    case Secondary:
        ui->lblSecondary->setText(msg.motorCapacityUsage());
        highlight(ui->lblSecondary, status);
        break;
    case Elevator:
        ui->lblElevator->setText(msg.motorCapacityUsage());
        highlight(ui->lblElevator, status);
        break;
    case FrontFolder:
        ui->lblFront->setText(msg.motorCapacityUsage());
        highlight(ui->lblFront, status);
        break;
    case RearFolder:
        ui->lblRear->setText(msg.motorCapacityUsage());
        highlight(ui->lblRear, status);
        break;
    case Pusher:
        ui->lblPusher->setText(msg.motorCapacityUsage());
        highlight(ui->lblPusher, status);
        break;
    case SideFolder:
        ui->lblSide->setText(msg.motorCapacityUsage());
        highlight(ui->lblSide, status);
        break;
    case Gripper:
        ui->lblGripper->setText(msg.motorCapacityUsage());
        highlight(ui->lblGripper, status);
        break;
    case SideClamps:
        ui->lblSideClamps->setText(msg.motorCapacityUsage());
        highlight(ui->lblSideClamps, status);
        break;
    case SealerBelt:
        ui->lblBelt->setText(msg.motorCapacityUsage());
        highlight(ui->lblBelt, status);
        break;
    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool operateNewWrapperRunIn::onScreenChanging()
{
    // cannot leave this screen until test complete
   if ( session_->scaleConfig->internalConfig.isTimingsActive )
        return false;

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "operateNewWrapperRunIn", "onSessionMessage" );

    int device = msg[0].toInt();
    int event  = msg[1].toInt();

    // motor profile voltages
    if( Wrapper == device && WrEvolMotorProfile == event )
    {
        MadpEvolWrapperMotorProfile m(msg);
        updateValues(m);
    }

    // event capture
    else if ( ( Wrapper == device )  && ( WrEvolEventCapture == event ) )
    {
        DomEvolWrapperEventCapture m( msg );

        if ( valueLabels_.contains( m.captureType() ) )
        {
            QLabel *l( valueLabels_[m.captureType()] );

            if ( l )
            {
                l->setText( QString::number( m.value() ) );

                // Save all current timings to a list, because if we choose to SAVE TIMINGS, these will
                // be written to a file, and read later on against new current timings.
                currentTimings_[m.captureType()] = QString::number( m.value() );
                outOfRangeLabel(m.captureType(), m.value(), l);                 // checks all current values - not saved values
                inRangeByTenLabel( m.captureType(), true );                           // is it within 10 of comparable field?
            }
        }
    }
    // handle cycle counts
    else if ( ( Wrapper == device )  &&  ( WrEvolPackageDesc == event ) )
    {
        // just receiving the msg increments it

        msgCounter_++;
        //SOF-2377
        //if( msgCounter_ % 2 == 0 )
        //{
            runinCycleCounter_++;
            QString countsStr = QString::number(runinCycleCounter_);

            updateCycleCounterText( countsStr );

            LOG_DEBUG( "Run-In Cycle Counter; runinCycleCounter_= %s", countsStr.toLatin1().constData() );

            // SOF-4216: Stop at 12,000 cycles?
            if( maxRunInCycles_ )
            {
                if( runinCycleCounter_ >= maxRunInCycles_ )
                {
                    LOG_DEBUG( "Reached the max cycles for Wrapper Run-In: %d", maxRunInCycles_ );

                    session_->send( Wrapper, WrEvolReqSystemReset ); // 9,190 ... this will stop Run-in

                    LOG_DEBUG( "Just sent \"Wrapper, WrEvolReqSystemReset\" (9,190) message to backend to STOP Run-In", NULL );

                    QString msgStr = tr( "Reached maximum cycles of " ) + QString::number( maxRunInCycles_ ) + tr( ".\n\n\nStopping Manufacturing Run-In." );
                    MessageBoxDialog dialog( real_time_priority, mainWindow_ );
                    dialog.setTitle( tr( "MANUFACTURING RUN IN" ) );
                    dialog.setText( msgStr );
                    dialog.setAllowOverlayClickDismiss( false );
                    dialog.setStandardButtons( QDialogButtonBox::Ok );

                    dialog.exec();
                }
            }
        //}
    }
    else if ( ( MfgTestDev == device ) && ( ReadMfgData == event ) )            // 164,4
    {
        // SOF-4217
        DomMfgReadData m( msg );

        serialNumber_ =  m.serialNumber();
    }
    else if ( ( SessionDev == device ) && ( SnVersionInfo == event ) )          // 100,2
    {
        // SOF-4217
        DomCurrentScaleSetup m( msg );

        applicationVer_ = m.version();
    }
    else if ( ( WrapperSystem == device ) && ( SysEvolWrSysStatus == event ) )  // 10,21
    {
        // SOF-4217

        DomEvolWrapperSystemStatus m( msg );

        QString tmpVerList = m.appVersionList();
        QStringList tmpAppVersionList_ = tmpVerList.split(",");
        appVersionList_ = tmpAppVersionList_;

        QString tmpBootList = m.bootVersionList();
        QStringList tmpBootVersionList_ = tmpBootList.split(",");
        bootVersionList_ = tmpBootVersionList_;

        const int numOfItems( m.numOfItems().toInt() );
        const QStringList nodeList( m.nodeList().split(",") );


        if( (0 < numOfItems) && (numOfItems <= 6 ) &&
            ( numOfItems == tmpAppVersionList_.size() ) && ( numOfItems == nodeList.size() ) )
        {
            QMap<int,int> nodeMapping; // node id, array index

            // display nodes in order of node id
            int n( 0 );

            // map node id's to array index
            for ( int i( numOfItems ); i--; )
                nodeMapping.insert( nodeList[i].toInt(), i );

            foreach ( int i, nodeMapping.values() )
            {
                // Sort the node app & boot versions based on the order of the nodeList

                appVersionList_[n] = tmpAppVersionList_[i];

                bootVersionList_[n] = tmpBootVersionList_[i];

                ++n;
            }
        }
    }
    else if ( ( Error == device )  && ( Wrapper == event )   )
    {
        QString code( msg[2] );

        if( session_->scaleConfig->internalConfig.runInFailures.length() > 0 )
            session_->scaleConfig->internalConfig.runInFailures.append(",");                    // format: 9522,9572,####,...

        if( !session_->scaleConfig->internalConfig.runInFailures.contains(code) )
            session_->scaleConfig->internalConfig.runInFailures.append(code);                   // only add new code if not already listed

        //LOG_DEBUG("onSessionMessage() added to failures: %s", qPrintable( session_->scaleConfig->internalConfig.runInFailures ) );
    }
}

void operateNewWrapperRunIn::readCycleCountsFromFile()
{
#ifdef Q_OS_WIN
    QString filename("C:/hobart/runinCycleCounts.log");          // change to C:/Program Files/hobart/runinCycleCounts.log
#else
    QString filename("/usr/local/hobart/runinCycleCounts.log");
#endif // Q_OS_WIN

    runinCycleCounter_ = 0;                             // init to 0 in case file doesnt exist or there was a file read error
    msgCounter_        = 0;

    QFile f( filename );

    if ( f.open( QIODevice::ReadOnly ) )
    {
        QTextStream fs( &f );

        QString line( fs.readLine().trimmed() );        // only have one value in file

        int cycleCounts = line.toInt();                 // returns 0 if toInt() fails or if value just 0.

        runinCycleCounter_ = cycleCounts;

        QString countsStr = QString::number(runinCycleCounter_);
        updateCycleCounterText( countsStr );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::saveCycleCountsToFile()
{

#ifdef Q_OS_WIN
    QString filename("C:/hobart/runinCycleCounts.log");          // change to C:/Program Files/hobart/runinCycleCounts.log
#else
    QString filename("/usr/local/hobart/runinCycleCounts.log");
#endif // Q_OS_WIN

    QFile f( filename );

    f.open( QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text );

    QTextStream fs( &f );

    QString line( fs.readLine().trimmed() );            // read just the one line
    line = "";                                          // delete the data in it

    // save cycle counts to a file for future reference
    QString tmpVal = QString::number( runinCycleCounter_ );

    fs << tmpVal << endl;
    f.flush();
    f.close();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::outOfRangeLabel(int index, int curValue, QLabel *checkLabel )
{
    LOG_CLASS_INFO( "operateNewWrapperRunIn", "outOfRangeLabel" );
    LOG_DEBUG("operaterRunIn::outOfRangeLabel() START", NULL );

    // I was exiting based on this condition, but if it was previously red (which we wont know)
    // then we need to turn it back green again so if else it.

    bool good_reading = false;

    if( curValue >= validateRange_[index][0] && curValue <= validateRange_[index][1] )
        good_reading = true;        //    return;

    // If it's not in the current range, which lets you exit this method - it must INVALID

    modifyPaletteBackground redBackground( QColor( 255, 0, 0 ) ); // cant use red bc network connectivity
    //modifyPaletteBackground whiteBackground( hobart::colorPalette::white );
    modifyPaletteBackground whiteBackground( hobart::colorPalette::green );

    modifyPaletteForeground whiteForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );
    modifyPaletteForeground blackForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::black ) );


    checkLabel->setAutoFillBackground( true );

    if( good_reading )
    {
        LOG_DEBUG("operaterRunIn::outOfRangeLabel() good_reading", NULL );
        whiteBackground( checkLabel );
        blackForeground( checkLabel );
    }
    else
    {
        if( !screenLoaded_ )
            return;

        LOG_DEBUG("operaterRunIn::outOfRangeLabel() bad_reading", NULL );
        session_->scaleConfig->internalConfig.runInTestState = TimingsTestState::Test_Failed;

        QString indexStr = QString::number(index);
        if( !session_->scaleConfig->internalConfig.runInSensorFailures.contains(indexStr) )
        {
            if( session_->scaleConfig->internalConfig.runInSensorFailures.length() > 0 )
                session_->scaleConfig->internalConfig.runInSensorFailures.append(",");              // format: 9522,9572,####,...

            session_->scaleConfig->internalConfig.runInSensorFailures.append(indexStr);                 // only add new code if not already listed
        }

        //LOG_DEBUG("onSessionMessage() added to failures: %s", qPrintable( session_->scaleConfig->internalConfig.runInSensorFailures ) );
        if( DEMO_ALL_GOOD_MODE )
        {
            whiteBackground( checkLabel );
            blackForeground( checkLabel );
        }
        else
        {
            redBackground( checkLabel );
            whiteForeground( checkLabel );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::inRangeByTenLabel(int index, bool curVal)
{
    int leftIndex = 0;
    int rightIndex = 0;

    switch (index )
    {
        case DomEvolWrapperEventCapture::GripperWideLeftCloseEvent:
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftCloseEvent;
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightCloseEvent;
        break;

        case DomEvolWrapperEventCapture::GripperWideLeftOpenEvent:
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftOpenEvent;
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightOpenEvent;
        break;

        case DomEvolWrapperEventCapture::GripperWideRightCloseEvent:
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightCloseEvent;
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftCloseEvent;
        break;

        case DomEvolWrapperEventCapture::GripperWideRightOpenEvent:
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightOpenEvent;
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftOpenEvent;
        break;

        case DomEvolWrapperEventCapture::SideClampLeftCloseEvent:
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftCloseEvent;
            rightIndex = DomEvolWrapperEventCapture::SideClampRightCloseEvent;
        break;

        case DomEvolWrapperEventCapture::SideClampLeftOpenEvent:
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftOpenEvent;
            rightIndex = DomEvolWrapperEventCapture::SideClampRightOpenEvent;
        break;

        case DomEvolWrapperEventCapture::SideClampRightCloseEvent:
            rightIndex = DomEvolWrapperEventCapture::SideClampRightCloseEvent;
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftCloseEvent;
        break;

        case DomEvolWrapperEventCapture::SideClampRightOpenEvent:
            rightIndex = DomEvolWrapperEventCapture::SideClampRightOpenEvent;
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftOpenEvent;
        break;

        case DomEvolWrapperEventCapture::GripperWideLeftCloseSavedEvent:
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftCloseSavedEvent;
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightCloseSavedEvent;
        break;

        case DomEvolWrapperEventCapture::GripperWideLeftOpenSavedEvent:
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftOpenSavedEvent;
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightOpenSavedEvent;
        break;

        case DomEvolWrapperEventCapture::GripperWideRightCloseSavedEvent:
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightCloseSavedEvent;
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftCloseSavedEvent;
        break;

        case DomEvolWrapperEventCapture::GripperWideRightOpenSavedEvent:
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightOpenSavedEvent;
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftOpenSavedEvent;
        break;

        case DomEvolWrapperEventCapture::SideClampLeftCloseSavedEvent:
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftCloseSavedEvent;
            rightIndex = DomEvolWrapperEventCapture::SideClampRightCloseSavedEvent;
        break;

        case DomEvolWrapperEventCapture::SideClampLeftOpenSavedEvent:
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftOpenSavedEvent;
            rightIndex = DomEvolWrapperEventCapture::SideClampRightOpenSavedEvent;
        break;

        case DomEvolWrapperEventCapture::SideClampRightCloseSavedEvent:
            rightIndex = DomEvolWrapperEventCapture::SideClampRightCloseSavedEvent;
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftCloseSavedEvent;
        break;

        case DomEvolWrapperEventCapture::SideClampRightOpenSavedEvent:
            rightIndex = DomEvolWrapperEventCapture::SideClampRightOpenSavedEvent;
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftOpenSavedEvent;
        break;
    }

    int left  = 0;
    int right = 0;

    if ( curVal )
    {
        left  = currentTimings_[leftIndex].toInt();         // get the value of this enum
        right = currentTimings_[rightIndex].toInt();        // get the value of this enum
    }
    else
    {
        left = savedTimings_[leftIndex].toInt();
        right = savedTimings_[rightIndex].toInt();
    }

    if( 0 == right || 0 == left)
        return;             // unfair comparrison, both values havent been updated yet

    int l_minus_r = left - right;

    if( l_minus_r < 10 && l_minus_r > -10)
        return;

    if( DEMO_ALL_GOOD_MODE )
        return;

    QLabel *leftLabel( valueLabels_[leftIndex] );
    QLabel *rightLabel( valueLabels_[rightIndex] );

    // If the 2 values are not within 10, which lets you exit this method - it must INVALID

    modifyPaletteBackground redBackground( QColor( 255, 0, 0 ) );     // cant use red due to network connectivity
    modifyPaletteForeground whiteForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    leftLabel->setAutoFillBackground( true );
    rightLabel->setAutoFillBackground( true );

    redBackground( leftLabel );
    redBackground( rightLabel );
    whiteForeground( leftLabel );
    whiteForeground( rightLabel );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::onCycleCountsClicked()
{
    // Read it from the file. Set variable, update it, reset it, leave page write it
    // here we just update the var to 0

    msgCounter_        = 0;
    runinCycleCounter_ = 0;

    updateCycleCounterText("0");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::onBackToTimings()
{
    LOG_CLASS_INFO( "operateNewWrapperRunIn", "onBackToTimings" );
    LOG_DEBUG("onBackToTimings() START", NULL );

    // stop run in
    session_->send( Wrapper, WrEvolReqSystemReset );    // 9,190 ... this will stop Run-in with System Reset

    // stop actuators
    session_->send( Wrapper, WrEvolTest, "28,0" );      // CycleActuatorsTest, Stop (0)

    // lets you leave the screen
    session_->scaleConfig->internalConfig.isTimingsActive = false;      // by setting this false, it allows us to leave and return to Timings

    // goto Wrapper timings
    mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();

    //LOG_DEBUG("onBackToTimings() what are my failures: %s", qPrintable( session_->scaleConfig->internalConfig.runInFailures ) );
    //LOG_DEBUG("onBackToTimings() what are my sensor failures: %s", qPrintable( session_->scaleConfig->internalConfig.runInSensorFailures ) );

    // TODO - Need to report back pass or fail
    LOG_DEBUG("onBackToTimings() END", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::updateCycleCounterText(QString ctr)
{
    cycleCounts_->setText( ctr );
    cycleCounts_->repaint();
    qApp->processEvents();
}

// SOF-4217
void operateNewWrapperRunIn::generateRunInReport()
{
    LOG_CLASS_INFO( "operateNewWrapperRunIn", "generateRunInReport" );

    QString msg;
    QString title;

    QString HobartPoint4 = QString::fromUtf8("\x1b""\x1b""fn=Hobart;ps=4""\x1b");
    QString HobartPoint6 = QString::fromUtf8("\x1b""\x1b""fn=Hobart;ps=6""\x1b");
    QString HobartPoint7 = QString::fromUtf8("\x1b""\x1b""fn=Hobart;ps=7""\x1b");
    QString HobartBoldPoint12 = QString::fromUtf8("\x1b""\x1b""fn=HobartBold;ps=12""\x1b");

    QString lineIndent = "  ";
    title = HobartPoint7;

    title += lineIndent + "Wrapper Run-In";

    msg = HobartPoint7;

    msg += lineIndent + "Date: ";
    msg += QDate::currentDate().toString("MM/dd/yyyy  ");
    msg += QTime::currentTime().toString("hh:mm:ss AP");
    msg += "\n";
    msg += "\n";


    msg += lineIndent + "Serial number: ";
    msg += serialNumber_;
    msg += "\n";

    msg += lineIndent + "Application:  ";
    msg += applicationVer_;
    msg += "\n";
    msg += "\n";

    msg += lineIndent + "Camera Node/Boot:   ";
    if( appVersionList_.size() >= 1 )
        msg += appVersionList_[0] + " / " + bootVersionList_[0];
    else
        msg += "0.0.0 / 0.0.0";
    msg += "\n";

    msg += lineIndent + "Wrapper Node/Boot 1:  ";
    if( appVersionList_.size() >= 2 )
        msg += appVersionList_[1] + " / " + bootVersionList_[1];
    else
        msg += "0.0.0 / 0.0.0";
    msg += "\n";

    msg += lineIndent + "Wrapper Node/Boot 2:  ";
    if( appVersionList_.size() >= 3 )
        msg += appVersionList_[2] + " / " + bootVersionList_[2];
    else
        msg += "0.0.0 / 0.0.0";
    msg += "\n";

    msg += lineIndent + "Wrapper Node/Boot 3:  ";
    if( appVersionList_.size() >= 4 )
        msg += appVersionList_[3] + " / " + bootVersionList_[3];
    else
        msg += "0.0.0 / 0.0.0";;
    msg += "\n";

    msg += lineIndent + "Wrapper Node/Boot 4:  ";
    if( appVersionList_.size() >= 5 )
        msg += appVersionList_[4] + " / " + bootVersionList_[4];
    else
        msg += "0.0.0 / 0.0.0";
    msg += "\n";

    msg += lineIndent + "Wrapper Node/Boot 5:  ";
    if( appVersionList_.size() >= 6 )
        msg += appVersionList_[5] + " / " + bootVersionList_[5];
    else
        msg += "0.0.0 / 0.0.0";
    msg += "\n";
    msg += "\n";


    msg += lineIndent + "Primary: ";
    msg += ui->lblPrimary->text();
    msg += "\n";

    msg += lineIndent + "Secondary: ";
    msg += ui->lblSecondary->text();
    msg += "\n";

    msg += lineIndent + "Elevator: ";
    msg += ui->lblElevator->text();
    msg += "\n";

    msg += lineIndent + "Gripper: ";
    msg += ui->lblGripper->text();
    msg += "\n";

    msg += lineIndent + "Side Clamps: ";
    msg += ui->lblSideClamps->text();
    msg += "\n";

    msg += "\n";

    msg += lineIndent + "Front: ";
    msg += ui->lblFront->text();
    msg += "\n";

    msg += lineIndent + "Pusher: ";
    msg += ui->lblPusher->text();
    msg += "\n";

    msg += lineIndent + "Rear: ";
    msg += ui->lblRear->text();
    msg += "\n";

    msg += lineIndent + "Side: ";
    msg += ui->lblSide->text();
    msg += "\n";

    msg += lineIndent + "Belt: ";
    msg += ui->lblBelt->text();
    msg += "\n";

    msg += lineIndent + "Cycle Counts: ";
    msg += ui->cycleCounts->text();
    msg += "\n";

    msg += "\n";

    msg += lineIndent + "Gripper Narrow Open: ";
    msg += ui->gripperNarrowOpen->text();
    msg += "\n";

    msg += lineIndent + "Gripper Narrow Close: ";
    msg += ui->gripperNarrowClose->text();
    msg += "\n";

    msg += "\n";

    msg += lineIndent + "Grip Wide Left Open: ";
    msg += ui->gripperWideLeftOpen->text();
    msg += "\n";

    msg += lineIndent + "Grip Wide Left Close: ";
    msg += ui->gripperWideLeftClose->text();
    msg += "\n";

    msg += "\n";

    msg += lineIndent + "Grip Wide Right Open: ";
    msg += ui->gripperWideRightOpen->text();
    msg += "\n";

    msg += lineIndent + "Grip Wide Right Close: ";
    msg += ui->gripperWideRightClose->text();
    msg += "\n";

    msg += "\n";

    msg += lineIndent + "Side Clamp Left Open: ";
    msg += ui->sideClampLeftOpen->text();
    msg += "\n";

    msg += lineIndent + "Side Clamp Left Close: ";
    msg += ui->sideClampLeftClose->text();
    msg += "\n";

    msg += "\n";

    msg += lineIndent + "Side Clamp Right Open: ";
    msg += ui->sideClampRightOpen->text();
    msg += "\n";

    msg += lineIndent + "Side Clamp Right Close: ";
    msg += ui->sideClampRightClose->text();
    msg += "\n";

    msg += "\n";

    msg += lineIndent + "Knife Up: ";
    msg += ui->knifeUp->text();
    msg += "\n";

    msg += lineIndent + "Knife Down: ";
    msg += ui->knifeDown->text();
    msg += "\n";

    msg += "\n";

    msg += lineIndent + "Wand Up: ";
    msg += ui->wandUp->text();
    msg += "\n";

    msg += lineIndent + "Wand Down: ";
    msg += ui->wandDown->text();
    msg += "\n";

    msg += "\n";
    msg += lineIndent + "Film Selector Up: ";
    msg += ui->filmSelectorUp->text();
    msg += "\n";

    msg += lineIndent + "Film Selector Down: ";
    msg += ui->filmSelectorDown->text();
    msg += "\n";

    msg += "\n";

    msg += lineIndent + "Applier Hand: ";
    msg += ui->applierHand->text();
    msg += "\n";

    msg += lineIndent + "Vacuum: ";
    msg += ui->vacuum->text();
    msg += "\n";

    msg += lineIndent + "Tank Pressure: ";
    msg += ui->mainPressure->text();
    msg += "\n";

    msg += lineIndent + "Regulated Pressure: ";
    msg += ui->linePressure->text();
    msg += "\n";

    session_->send( ReportManagerDev, StartReport, "2,0,0,0,0," + title + "," + msg ); // 116,0

    LOG_DEBUG("Wrapper Run-In Report - Generating: Exiting", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::createFakeData()
{
    // These are simulated as incoming messages to update the fields as the current readings

    knifeUp_->setText("92");
    QLabel *label12( valueLabels_[DomEvolWrapperEventCapture::KnifeUpEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::KnifeUpEvent, 92, label12);

    knifeDown_->setText("254");
    QLabel *label28( valueLabels_[DomEvolWrapperEventCapture::KnifeDownEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::KnifeDownEvent, 254, label28);

    gripperNarrowClose_->setText("32");
    QLabel *label18( valueLabels_[DomEvolWrapperEventCapture::GripperNarrowCloseEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::GripperNarrowCloseEvent, 32, label18);

    gripperNarrowOpen_->setText("69");
    QLabel *label2( valueLabels_[DomEvolWrapperEventCapture::GripperNarrowOpenEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::GripperNarrowOpenEvent, 69, label2);

    gripperWideLeftClose_->setText("18");
    QLabel *label20( valueLabels_[DomEvolWrapperEventCapture::GripperWideLeftCloseEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::GripperWideLeftCloseEvent, 18, label20);
    inRangeByTenLabel( DomEvolWrapperEventCapture::GripperWideLeftCloseEvent );                           // is it within 10 of comparable field?

    gripperWideLeftOpen_->setText("88");
    QLabel *label4( valueLabels_[DomEvolWrapperEventCapture::GripperWideLeftOpenEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::GripperWideLeftOpenEvent, 88, label4);
    inRangeByTenLabel( DomEvolWrapperEventCapture::GripperWideLeftOpenEvent );

    gripperWideRightClose_->setText("43");
    QLabel *label22( valueLabels_[DomEvolWrapperEventCapture::GripperWideRightCloseEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::GripperWideRightCloseEvent, 43, label22);
    inRangeByTenLabel( DomEvolWrapperEventCapture::GripperWideRightCloseEvent );

    gripperWideRightOpen_->setText("90");
    QLabel *label6( valueLabels_[DomEvolWrapperEventCapture::GripperWideRightOpenEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::GripperWideRightOpenEvent, 90, label6);
    inRangeByTenLabel( DomEvolWrapperEventCapture::GripperWideRightOpenEvent );

    sideClampLeftClose_->setText("56");
    QLabel *label24( valueLabels_[DomEvolWrapperEventCapture::SideClampLeftCloseEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::SideClampLeftCloseEvent, 56, label24);
    inRangeByTenLabel( DomEvolWrapperEventCapture::SideClampLeftCloseEvent );

    sideClampLeftOpen_->setText("127");
    QLabel *label8( valueLabels_[DomEvolWrapperEventCapture::SideClampLeftOpenEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::SideClampLeftOpenEvent, 127, label8);
    inRangeByTenLabel( DomEvolWrapperEventCapture::SideClampLeftOpenEvent );

    sideClampRightClose_->setText("67");
    QLabel *label26( valueLabels_[DomEvolWrapperEventCapture::SideClampRightCloseEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::SideClampRightCloseEvent, 67, label26);
    inRangeByTenLabel( DomEvolWrapperEventCapture::SideClampRightCloseEvent );

    sideClampRightOpen_->setText("152");
    QLabel *label10( valueLabels_[DomEvolWrapperEventCapture::SideClampRightOpenEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::SideClampRightOpenEvent, 152, label10);
    inRangeByTenLabel( DomEvolWrapperEventCapture::SideClampRightOpenEvent );

    wandUp_->setText("273");
    QLabel *label14( valueLabels_[DomEvolWrapperEventCapture::WandUpEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::WandUpEvent, 273, label14);

    wandDown_->setText("704");
    QLabel *label30( valueLabels_[DomEvolWrapperEventCapture::WandDownEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::WandDownEvent, 704, label30);

    filmSelectorUp_->setText("381");
    QLabel *label16( valueLabels_[DomEvolWrapperEventCapture::FilmSelectorUpEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::FilmSelectorUpEvent, 381, label16);

    filmSelectorDown_->setText("154");
    QLabel *label32( valueLabels_[DomEvolWrapperEventCapture::FilmSelectorDownEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::FilmSelectorDownEvent, 154, label32);

    mainPressure_->setText("107");
    QLabel *label40( valueLabels_[DomEvolWrapperEventCapture::TankPressureReadingEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::TankPressureReadingEvent, 107, label40);

    linePressure_->setText("43");
    QLabel *label42( valueLabels_[DomEvolWrapperEventCapture::RegulatedPressureReadingEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::RegulatedPressureReadingEvent, 43, label42);

    vacuum_->setText("24");
    QLabel *label38( valueLabels_[DomEvolWrapperEventCapture::ApplierVacuumReadingEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::ApplierVacuumReadingEvent, 24, label38);

    applierHand_->setText("146");
    QLabel *label34( valueLabels_[DomEvolWrapperEventCapture::ApplierHandHomeEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::ApplierHandHomeEvent, 146, label34);

    currentTimings_[DomEvolWrapperEventCapture::KnifeUpEvent] = "92";
    currentTimings_[DomEvolWrapperEventCapture::KnifeDownEvent] = "254";
    currentTimings_[DomEvolWrapperEventCapture::GripperNarrowCloseEvent] = "32";
    currentTimings_[DomEvolWrapperEventCapture::GripperNarrowOpenEvent] = "69";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideLeftCloseEvent] = "18";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideLeftOpenEvent] = "88";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideRightCloseEvent] = "43";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideRightOpenEvent] = "90";
    currentTimings_[DomEvolWrapperEventCapture::SideClampLeftCloseEvent] = "56";
    currentTimings_[DomEvolWrapperEventCapture::SideClampLeftOpenEvent] = "127";
    currentTimings_[DomEvolWrapperEventCapture::SideClampRightCloseEvent] = "67";
    currentTimings_[DomEvolWrapperEventCapture::SideClampRightOpenEvent] = "152";
    currentTimings_[DomEvolWrapperEventCapture::WandUpEvent] = "273";
    currentTimings_[DomEvolWrapperEventCapture::WandDownEvent] = "704";
    currentTimings_[DomEvolWrapperEventCapture::FilmSelectorUpEvent] = "381";
    currentTimings_[DomEvolWrapperEventCapture::FilmSelectorDownEvent] = "154";
    currentTimings_[DomEvolWrapperEventCapture::TankPressureReadingEvent] = "107";
    currentTimings_[DomEvolWrapperEventCapture::RegulatedPressureReadingEvent] = "43";      // 18
    currentTimings_[DomEvolWrapperEventCapture::ApplierVacuumReadingEvent] = "24";          // 19
    currentTimings_[DomEvolWrapperEventCapture::ApplierHandHomeEvent] = "146";               // 20
    currentTimings_[DomEvolWrapperEventCapture::MainACVoltageReadingEvent] = "230";          // 23
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operateNewWrapperRunIn::createFakeCycleCounter()
{
    runinCycleCounter_++;
    QString countsStr = QString::number(runinCycleCounter_);
    updateCycleCounterText( countsStr );
}
