/**
 * @file serviceNewWrapperDiagActuators.cpp
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "serviceNewWrapperDiagActuators.h"

#include "../sessionmainwindow.h"

#include "../dialogs/messageboxdialog.h"

#include "../modes/servicemode.h"
#include "../platform/python.h"

#include <QDebug>
#include <QTime>
#include <backgroundwidgetstyles.h>
#include <logmanager.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>

const static int    MAX_TIMINGS = 27;
const static bool   TESTING = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperActuatorDiag::serviceNewWrapperActuatorDiag( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    diagActive_( false ),
    testActive_( false )
{
    setObjectName( "serviceNewWrapperActuatorDiag" );

    // initialize
    initialize();
    createLayout();

    translate();

    //if ( TESTING )
    //    createFakeData();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperActuatorDiag::~serviceNewWrapperActuatorDiag()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperActuatorDiag::translate()
{
    lastSavedLabel_->setText( tr("Last Saved: ") );

    // add 2nd applier need the room - title_->setText( tr( "Actuators" ) );

    diagStart_->setText( tr( "Start Actuation" ) );
    diagStop_->setText( tr( "Stop Actuation" ) );

    saveTimings_->setText( tr( "Save Timings" ) );

    saveHistory_->setText( tr( "Save History to USB" ) );
    clearHistory_->setText( tr( "Clear History" ) );

    //
    // last time calibrated
    //

    //
    // sensors
    //

    sensorTitle1_->setText( "  " );                              // BLANK PLACEHOLDER
    sensorOpenLabel_->setText( tr( "OPEN" ) );
    sensorCloseLabel_->setText( tr( "CLOSE" ) );

    openCloseTitle1_->setText( " " );
    openSavedLabel_->setText( tr( "Saved" ) );
    openCurrentLabel_->setText( tr( "Current" ) );
    closeSavedLabel_->setText( tr( "Saved" ) );
    closeCurrentLabel_->setText( tr( "Current" ) );

    gripperNarrowLabel_->setText( tr( "Gripper Narrow" ) );
    gripperWideLeftLabel_->setText( tr( "Gripper Wide Left*" ) );
    gripperWideRightLabel_->setText( tr( "Gripper Wide Right*" ) );
    sideClampLeftLabel_->setText( tr( "Side Clamp Left*" ) );
    sideClampRightLabel_->setText( tr( "Side Clamp Right*" ) );

    sensorTitle2_->setText( "* If fields are red and in range, then the Left and Right Grippers or" );                              // BLANK PLACEHOLDER
    sensorUpLabel_->setText( tr( "UP" ) );
    sensorDownLabel_->setText( tr( "DOWN" ) );

    upDownTitle1_->setText( "the Left and Right Side Clamps may be more than 10 digits from each other." );
    upSavedLabel_->setText( tr( "Saved" ) );
    upCurrentLabel_->setText( tr( "Current" ) );
    downSavedLabel_->setText( tr( "Saved" ) );
    downCurrentLabel_->setText( tr( "Current" ) );

    knifeLabel_->setText( tr( "Knife" ) );
    wandLabel_->setText( tr( "Label Applier Wand" ) );
    wand2Label_->setText( tr("Second Applier Wand") );
    filmSelectorLabel_->setText( tr( "Film Selector" ) );

    sensorTitle3_->setText( " " );                               // BLANK PLACEHOLDER
    sensorHomeLabel_->setText( tr( "HOME" ) );

    homeTitle1_->setText( " " );
    homeSavedLabel_->setText( tr( "Saved" ) );
    homeCurrentLabel_->setText( tr( "Current" ) );

    applierHandLabel_->setText( tr( "Applier Hand" ) );
    applier2HandLabel_->setText( tr( "Second Applier Hand" ) );
    vacuumLabel_->setText( tr( "Applier Vacuum" ) );
    vacuum2Label_->setText( tr( "Second Applier Vacuum" ) );
    inputVoltageLabel_->setText( tr( "Main AC Input Voltage" ) );
    mainPressureLabel_->setText( tr( "Tank Pressure" ) );
    linePressureLabel_->setText( tr( "Regulated Pressure" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperActuatorDiag::resizeEvent( QResizeEvent *e )
{
    const int titleWidth( (width() * 30) / 100 );
    const int labelWidth( (width() * 25) / 100 );

    sensorTitle1_->setFixedWidth( titleWidth );             // blank space
    sensorOpenLabel_->setFixedWidth( labelWidth );          // OPEN
    sensorCloseLabel_->setFixedWidth( labelWidth );         // CLOSE

    sensorTitle2_->setFixedWidth( titleWidth );
    sensorUpLabel_->setFixedWidth( labelWidth );            // UP
    sensorDownLabel_->setFixedWidth( labelWidth );          // DOWN

    sensorTitle3_->setFixedWidth( titleWidth );
    sensorHomeLabel_->setFixedWidth( labelWidth );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperActuatorDiag::onScreenChanging()
{
    // cannot leave this screen until test complete
    if (( diagActive_ ) || ( testActive_ ))
        return false;

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperActuatorDiag::onSessionMessage( const SessionMessage& msg )
{
    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // event capture

    if (( Wrapper == device ) && ( WrEvolEventCapture == event ))
    {
        const DomEvolWrapperEventCapture m( msg );

 

        const int captureType( m.captureType() );
        const int captureValue( m.value() );
        const QString captureValueStr( QString::number( captureValue ) );

 

        // find and update the screen label
        QMap<int, QLabel*>::const_iterator vit( valueLabels_.constFind( captureType ) );

 

        if ( valueLabels_.constEnd() != vit )
        {
            QLabel *l( vit.value() );

 

            if ( l )
            {
                l->setText( captureValueStr );

 

                // Save all current timings to a list, because if we choose to SAVE TIMINGS, these will
                // be written to a file, and read later on against new current timings.
                currentTimings_[captureType] = captureValueStr;

 

                outOfRangeLabel( captureType, captureValue, l );                    // checks all current values - not saved values
                inRangeByTenLabel( captureType, true );                             // is it within 10 of comparable field?
            }
        }

 

        // find and update history
        QMap<int, QList<SessionMessage>* >::const_iterator hit( history_.constFind( captureType ) );

 

        if ( history_.constEnd() != hit )
            hit.value()->append( msg );
        else
        {
            QList<SessionMessage>* h( new QList<SessionMessage> );
            h->append( msg );

 

            history_[captureType] = h;
        }
    }
    // wrapper test complete
    else if (( Wrapper == device ) && ( WrEvolTestComplete == event ) && ( testActive_ ))
    {
        DomEvolWrapperTestComplete m( msg );

        // cycle actuators test
        if ( "28" == m.testNum() )
        {
            // success!
            if ( "1" == m.testPassed() )
                diagActive_ = ("1" == m.subTestNum());

            diagStart_->setEnabled( !diagActive_ );
            diagStart_->setPalette( pushButtonPalette_ );

            diagStop_->setEnabled( diagActive_ );
            saveTimings_->setEnabled( true );
            diagStop_->setPalette( pushButtonPalette_ );

            testActive_ = false;
        }
    }
     
    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperActuatorDiag::onPushButtonClicked()
{
    QPushButton *pushButtonWrapperTest( NULL );

    if (( diagStart_ == sender() ) && ( !testActive_ ))
    {
        // start actuation
        session_->send( Wrapper, WrEvolTest, "28,1" ); // CycleActuatorsTest, Start (1)     // 9,108
        pushButtonWrapperTest = diagStart_;
        saveTimings_->setEnabled( false );      // just in case its not

        // UNCOMMENT FOR CAN TESTING
        //statusRequestTimer_->start();

        //if ( TESTING )
        //{
        //    saveTimings_->setEnabled( true );
        //    diagStop_->setEnabled( true );
        //}
    }
    else if (( diagStop_ == sender()  ) && ( !testActive_ ))
    {
        // stop actuation
        session_->send( Wrapper, WrEvolTest, "28,0" ); // CycleActuatorsTest, Stop (0)
        pushButtonWrapperTest = diagStop_;
        saveTimings_->setEnabled( true );
        // UNCOMMENT FOR CAN TESTING
        //statusRequestTimer_->stop();
    }
    else if ( saveTimings_ == sender() )
    {
        saveTimingsToFile();
    }
    else if ( saveHistory_ == sender() )
    {
        QStringList folders;
        QString folder;

#if defined( Q_OS_LINUX )
        folders.push_back( "/media/thumb" );
        folders.push_back( "/media/thumb1" );
        folders.push_back( "/media/thumb2" );
        folders.push_back( "/tmp/" );
#elif defined( Q_OS_WIN )
        folders.push_back( "D:\\" );
        folders.push_back( "E:\\" );
        folders.push_back( "C:\\" );
#endif

        // find existing folder
        foreach ( const QString& f, folders )
            if ( QDir( f ).exists() )
            {
                folder = f;
                break;
            }

        if ( folder.isEmpty() )
            return;

        // iterate each device
        for ( QMap<int, QList<SessionMessage>* >::const_iterator i( history_.constBegin() ); i != history_.constEnd(); i++ )
        {
            QString filename;

            switch ( i.key() )
            {
            case DomEvolWrapperEventCapture::KnifeUpEvent: filename = "knifeUpEvent.csv"; break;
            case DomEvolWrapperEventCapture::KnifeDownEvent: filename = "knifeDownEvent.csv"; break;
            case DomEvolWrapperEventCapture::GripperNarrowCloseEvent: filename = "gripperNarrowCloseEvent.csv"; break;
            case DomEvolWrapperEventCapture::GripperNarrowOpenEvent: filename = "gripperNarrowOpenEvent.csv"; break;
            case DomEvolWrapperEventCapture::GripperWideLeftCloseEvent: filename = "gripperWideLeftCloseEvent.csv"; break;
            case DomEvolWrapperEventCapture::GripperWideLeftOpenEvent: filename = "gripperWideLeftOpenEvent.csv"; break;
            case DomEvolWrapperEventCapture::GripperWideRightCloseEvent: filename = "gripperWideRightCloseEvent.csv"; break;
            case DomEvolWrapperEventCapture::GripperWideRightOpenEvent: filename = "gripperWideRightOpenEvent.csv"; break;
            case DomEvolWrapperEventCapture::SideClampLeftCloseEvent: filename = "sideClampLeftCloseEvent.csv"; break;
            case DomEvolWrapperEventCapture::SideClampLeftOpenEvent: filename = "sideClampLeftOpenEvent.csv"; break;
            case DomEvolWrapperEventCapture::SideClampRightCloseEvent: filename = "sideClampRightCloseEvent.csv"; break;
            case DomEvolWrapperEventCapture::SideClampRightOpenEvent: filename = "sideClampRightOpenEvent.csv"; break;
            case DomEvolWrapperEventCapture::WandUpEvent: filename = "wandUpEvent.csv"; break;
            case DomEvolWrapperEventCapture::WandDownEvent: filename = "wandDownEvent.csv"; break;
            case DomEvolWrapperEventCapture::SecondWandUpEvent: filename = "SecondWandUpEvent.csv"; break;
            case DomEvolWrapperEventCapture::SecondWandDownEvent: filename = "SecondWandDownEvent.csv"; break;
            case DomEvolWrapperEventCapture::FilmSelectorUpEvent: filename = "filmSelectorUpEvent.csv"; break;
            case DomEvolWrapperEventCapture::FilmSelectorDownEvent: filename = "filmSelectorDownEvent.csv"; break;
            case DomEvolWrapperEventCapture::TankPressureReadingEvent: filename = "tankPressureReadingEvent.csv"; break;
            case DomEvolWrapperEventCapture::RegulatedPressureReadingEvent: filename = "regulatedPressureReadingEvent.csv"; break;
            case DomEvolWrapperEventCapture::ApplierVacuumReadingEvent: filename = "applierVacuumReadingEvent.csv"; break;
            case DomEvolWrapperEventCapture::SecondApplierVacuumReadingEvent: filename = "SecondApplierVacuumReadingEvent.csv"; break;
            case DomEvolWrapperEventCapture::ApplierHandHomeEvent: filename = "applierHandHomeEvent.csv"; break;
            case DomEvolWrapperEventCapture::SecondApplierHandHomeEvent: filename = "SecondApplierHandHomeEvent.csv"; break;
            case DomEvolWrapperEventCapture::SystemPressureDropReadingEvent: filename = "systemPressureDropReadingEvent.csv"; break;
            case DomEvolWrapperEventCapture::UpperFilmRollEncoderReadingEvent: filename = "upperFilmRollEncoderReadingEvent.csv"; break;
            case DomEvolWrapperEventCapture::LowerFilmRollEncoderReadingEvent: filename = "lowerFilmRollEncoderReadingEvent.csv"; break;
            case DomEvolWrapperEventCapture::MainACVoltageReadingEvent: filename = "mainACVoltageReadingEvent.csv"; break;
            }

            if ( filename.isEmpty() )
                return;

            // open file
            QFile file( folder + "/" + filename );

            if ( file.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QTextStream os( &file );

                foreach ( const SessionMessage &msg, *( i.value() ) )
                {
                    foreach ( const QString &v, msg )
                    {
                        os << v;
                        os << ",";
                    }

                    os << "\r\n";
                }

                file.close();
            }
        }

        // NGW-23
        // show save complete message
        MessageBoxDialog *dialog( new MessageBoxDialog( mainWindow_ ) );
        dialog->setTitle( tr( "Save Complete" ) );
        dialog->setText( tr( "History has been saved to thumb.\n\nPlease use the eject button before removing." ) );
        dialog->setStandardButtons( QDialogButtonBox::Ok );

        dialog->open();
    }
    else if ( clearHistory_ == sender() )
    {
        for ( QMap<int, QList<SessionMessage>* >::iterator i( history_.begin() ); i != history_.end(); i++ )
            i.value()->clear();

        // NGW-215
        // also clear timings on screen
        for ( QMap<int, QLabel*>::iterator i( valueLabels_.begin() ); i != valueLabels_.end(); i++ )
        {
            QLabel *l( i.value() );

            if ( l )
                l->clear();
        }
    }

    // disable test button button temporarily...
    if ( pushButtonWrapperTest )
    {
        QPalette p( pushButtonPalette_ );
        // p.setBrush( QPalette::Button, hobart::colorPalette::red );       // cant use red, network connectivity
        p.setBrush( QPalette::Button, QColor( 255, 0, 255 ) );

        pushButtonWrapperTest->setEnabled( false );
        pushButtonWrapperTest->setPalette( p );

        testActive_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperActuatorDiag::onTimerTimeout()
{
    //if ( statusRequestTimer_ == sender() )
    //   session_->send( Wrapper, WrReqStatus );
    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperActuatorDiag::initialize()
{
    QFont f( font() );
    f.setPointSize( BUTTON_FONT_POINT_SIZE );

    //statusRequestTimer_ = new QTimer( this );
    //statusRequestTimer_->setInterval( 100 );
    //statusRequestTimer_->setSingleShot( false );
    //connect( statusRequestTimer_, SIGNAL(timeout()), SLOT(onTimerTimeout()) );

    // ---- //

    // screen title
    // add 2nd applier need the room - title_ = createHeaderLabel( "title", true );

    // the date the last time timings were saved
    lastSaved_      = createSimpleLabel( "lastSaved_");
    lastSavedLabel_ = createSimpleLabel( "lastSavedLabel_" );

    // actuation start
    diagStart_ = new QPushButton( this );
    diagStart_->setEnabled( true );
    diagStart_->setFont( f );
    diagStart_->setObjectName( "diagStart" );

    connect( diagStart_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // actuation stop
    diagStop_ = new QPushButton( this );
    diagStop_->setEnabled( false );
    diagStop_->setFont( f );
    diagStop_->setObjectName( "diagStop" );

    connect( diagStop_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // save timimgs for historic purposes
    saveTimings_ = new QPushButton( this );
    saveTimings_->setEnabled( false );
    saveTimings_->setFont( f );
    saveTimings_->setObjectName( "saveTimings" );
    connect( saveTimings_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // save history
    saveHistory_ = new QPushButton( this );
    saveHistory_->setEnabled( true );
    saveHistory_->setFont( f );
    saveHistory_->setObjectName( "saveHistory" );

    connect( saveHistory_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // clear history
    clearHistory_ = new QPushButton( this );
    clearHistory_->setEnabled( true );
    clearHistory_->setFont( f );
    clearHistory_->setObjectName( "clearHistory" );

    connect( clearHistory_, SIGNAL( clicked() ), SLOT( onPushButtonClicked() ) );

    // save off palette for future use
    pushButtonPalette_ = diagStart_->palette();

    // ---- //

    // TOP - LEFT and RIGHT COLUMN
    sensorTitle1_ = createHeaderLabel( "sensorTitle1", false );                 // BLANK SPACE
    sensorOpenLabel_ = createHeaderLabel( "sensorOpenLabel" );                  // OPEN
    sensorCloseLabel_ = createHeaderLabel( "sensorCloseLabel" );                // CLOSE

    openCloseTitle1_ = createHeaderLabel( "openCloseTitle1", false );           // BLANK SPACE
    openSavedLabel_ = createHeaderLabel( "openSavedLabel" );                    // Saved
    openCurrentLabel_ = createHeaderLabel( "openCurrentLabel" );                // Current
    closeSavedLabel_ = createHeaderLabel( "closeSavedLabel" );                  // Saved
    closeCurrentLabel_ = createHeaderLabel( "closeCurrentLabel" );              // Current

    gripperNarrowLabel_ = createTitleLabel( "gripperNarrowLabel", Qt::AlignLeft );
    gripperNarrowOpenSaved_ = createLabel( "gripperNarrowOpenSaved" );
    gripperNarrowOpen_ = createLabel( "gripperNarrowOpen" );
    gripperNarrowCloseSaved_ = createLabel( "gripperNarrowCloseSaved" );
    gripperNarrowClose_ = createLabel( "gripperNarrowClose" );

    gripperWideLeftLabel_ = createTitleLabel( "gripperWideLeftLabel", Qt::AlignLeft );
    gripperWideLeftOpenSaved_ = createLabel( "gripperWideLeftOpenSaved" );
    gripperWideLeftOpen_ = createLabel( "gripperWideLeftOpen" );
    gripperWideLeftCloseSaved_ = createLabel( "gripperWideLeftCloseSaved" );
    gripperWideLeftClose_ = createLabel( "gripperWideLeftClose" );

    gripperWideRightLabel_ = createTitleLabel( "gripperWideRightLabel", Qt::AlignLeft );
    gripperWideRightOpenSaved_ = createLabel( "gripperWideRightOpenSaved" );
    gripperWideRightOpen_ = createLabel( "gripperWideRightOpen" );
    gripperWideRightCloseSaved_ = createLabel( "gripperWideRightCloseSaved" );
    gripperWideRightClose_ = createLabel( "gripperWideRightClose" );

    sideClampLeftLabel_ = createTitleLabel( "sideClampLeftLabel", Qt::AlignLeft );
    sideClampLeftOpenSaved_ = createLabel( "sideClampLeftOpenSaved" );
    sideClampLeftOpen_ = createLabel( "sideClampLeftOpen" );
    sideClampLeftCloseSaved_ = createLabel( "sideClampLeftCloseSaved" );
    sideClampLeftClose_ = createLabel( "sideClampLeftClose" );

    sideClampRightLabel_ = createTitleLabel( "sideClampRightLabel", Qt::AlignLeft );
    sideClampRightOpenSaved_ = createLabel( "sideClampRightOpenSaved" );
    sideClampRightOpen_ = createLabel( "sideClampRightOpen" );
    sideClampRightCloseSaved_ = createLabel( "sideClampRightCloseSaved" );
    sideClampRightClose_ = createLabel( "sideClampRightClose" );

    // 2nd Group, Right side
    sensorTitle2_ = createFootnoteLabel( "sensorTitle2" );                      // footnote
    sensorUpLabel_ = createHeaderLabel( "sensorUpLabel" );                      // UP
    sensorDownLabel_ = createHeaderLabel( "sensorDownLabel" );                  // DOWN

    upDownTitle1_ = createFootnoteLabel( "upDownTitle1" );                 // BLANK SPACE
    upSavedLabel_ = createHeaderLabel( "upSavedLabel" );                    // Saved
    upCurrentLabel_ = createHeaderLabel( "upCurrentLabel" );                    // Current
    downSavedLabel_ = createHeaderLabel( "downSavedLabel" );                // Saved
    downCurrentLabel_ = createHeaderLabel( "downCurrentLabel" );              // Current

    knifeLabel_ = createTitleLabel( "knifeLabel", Qt::AlignLeft );
    knifeUpSaved_ = createLabel( "knifeUpSaved" );
    knifeUp_ = createLabel( "knifeUp" );
    knifeDownSaved_ = createLabel( "knifeDownSaved" );
    knifeDown_ = createLabel( "knifeDown" );

    wandLabel_ = createTitleLabel( "wandLabel", Qt::AlignLeft );
    wandUpSaved_ = createLabel( "wandUpSaved" );
    wandUp_ = createLabel( "wandUp" );
    wandDownSaved_ = createLabel( "wandDownSaved" );
    wandDown_ = createLabel( "wandDown" );

    wand2Label_ = createTitleLabel( "wand2Label", Qt::AlignLeft );
    wand2UpSaved_ = createLabel( "wand2UpSaved" );
    wand2Up_ = createLabel( "wand2Up" );
    wand2DownSaved_ = createLabel( "wan2dDownSaved" );
    wand2Down_ = createLabel( "wand2Down" );

    filmSelectorLabel_ = createTitleLabel( "filmSelectorLabel", Qt::AlignLeft );
    filmSelectorUpSaved_ = createLabel( "filmSelectorUpSaved" );
    filmSelectorUp_ = createLabel( "filmSelectorUp" );
    filmSelectorDownSaved_ = createLabel( "filmSelectorDownSaved" );
    filmSelectorDown_ = createLabel( "filmSelectorDown" );

    // ---- //

    // Bottom Center Column
    sensorTitle3_ = createHeaderLabel( "sensorTitle3", false );                      // BLANK SPACE
    sensorHomeLabel_ = createHeaderLabel( "sensorHomeLabel" );                       // HOME

    homeTitle1_ = createHeaderLabel( "homeTitle1", false );                          // BLANK SPACE
    homeSavedLabel_ = createHeaderLabel( "homeSavedLabel" );                         // Saved
    homeCurrentLabel_ = createHeaderLabel( "homeCurrentLabel" );                     // Current

    applierHandLabel_ = createTitleLabel( "applierHandLabel", Qt::AlignLeft );
    applierHandSaved_ = createLabel( "applierHandSaved" );
    applierHand_ = createLabel( "applierHand" );

    applier2HandLabel_ = createTitleLabel( "applier2HandLabel", Qt::AlignLeft );
    applier2HandSaved_ = createLabel( "applier2HandSaved" );
    applier2Hand_ = createLabel( "applier2Hand" );

    vacuumLabel_ = createTitleLabel( "vacuumLabel", Qt::AlignLeft );
    vacuumSaved_ = createLabel( "vacuumSaved" );
    vacuum_ = createLabel( "vacuum" );

    vacuum2Label_ = createTitleLabel( "vacuum2Label", Qt::AlignLeft );
    vacuum2Saved_ = createLabel( "vacuum2Saved" );
    vacuum2_ = createLabel( "vacuum2" );

    inputVoltageLabel_ = createTitleLabel( "inputVoltageLabel", Qt::AlignLeft );
    inputVoltageSaved_ = createLabel( "inputVoltageSaved" );
    inputVoltage_ = createLabel( "inputVoltage" );

    mainPressureLabel_ = createTitleLabel( "mainPressureLabel", Qt::AlignLeft );
    mainPressureSaved_ = createLabel( "mainPressureSaved" );
    mainPressure_ = createLabel( "mainPressure" );

    linePressureLabel_ = createTitleLabel( "linePressureLabel", Qt::AlignLeft );
    linePressureSaved_ = createLabel( "linePressureSaved" );
    linePressure_ = createLabel( "linePressure" );

    // ---- //

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
    valueLabels_[DomEvolWrapperEventCapture::SecondWandUpEvent] = wand2Up_;
    valueLabels_[DomEvolWrapperEventCapture::WandDownEvent] = wandDown_;
    valueLabels_[DomEvolWrapperEventCapture::SecondWandDownEvent] = wand2Down_;
    valueLabels_[DomEvolWrapperEventCapture::FilmSelectorUpEvent] = filmSelectorUp_;
    valueLabels_[DomEvolWrapperEventCapture::FilmSelectorDownEvent] = filmSelectorDown_;
    valueLabels_[DomEvolWrapperEventCapture::TankPressureReadingEvent] = mainPressure_;
    valueLabels_[DomEvolWrapperEventCapture::RegulatedPressureReadingEvent] = linePressure_;
    valueLabels_[DomEvolWrapperEventCapture::ApplierVacuumReadingEvent] = vacuum_;
    valueLabels_[DomEvolWrapperEventCapture::SecondApplierVacuumReadingEvent] = vacuum2_;
    valueLabels_[DomEvolWrapperEventCapture::ApplierHandHomeEvent] = applierHand_;
    valueLabels_[DomEvolWrapperEventCapture::SecondApplierHandHomeEvent] = applier2Hand_;
    valueLabels_[DomEvolWrapperEventCapture::MainACVoltageReadingEvent] = inputVoltage_;

    // Saved Items
    valueLabels_[DomEvolWrapperEventCapture::KnifeUpSavedEvent] = knifeUpSaved_;
    valueLabels_[DomEvolWrapperEventCapture::KnifeDownSavedEvent] = knifeDownSaved_;
    valueLabels_[DomEvolWrapperEventCapture::GripperNarrowCloseSavedEvent] = gripperNarrowCloseSaved_;
    valueLabels_[DomEvolWrapperEventCapture::GripperNarrowOpenSavedEvent] = gripperNarrowOpenSaved_;
    valueLabels_[DomEvolWrapperEventCapture::GripperWideLeftCloseSavedEvent] = gripperWideLeftCloseSaved_;
    valueLabels_[DomEvolWrapperEventCapture::GripperWideLeftOpenSavedEvent] = gripperWideLeftOpenSaved_;
    valueLabels_[DomEvolWrapperEventCapture::GripperWideRightCloseSavedEvent] = gripperWideRightCloseSaved_;
    valueLabels_[DomEvolWrapperEventCapture::GripperWideRightOpenSavedEvent] = gripperWideRightOpenSaved_;
    valueLabels_[DomEvolWrapperEventCapture::SideClampLeftCloseSavedEvent] = sideClampLeftCloseSaved_;
    valueLabels_[DomEvolWrapperEventCapture::SideClampLeftOpenSavedEvent] = sideClampLeftOpenSaved_;
    valueLabels_[DomEvolWrapperEventCapture::SideClampRightCloseSavedEvent] = sideClampRightCloseSaved_;
    valueLabels_[DomEvolWrapperEventCapture::SideClampRightOpenSavedEvent] = sideClampRightOpenSaved_;
    valueLabels_[DomEvolWrapperEventCapture::WandUpSavedEvent] = wandUpSaved_;
    valueLabels_[DomEvolWrapperEventCapture::SecondWandUpSavedEvent] = wand2UpSaved_;
    valueLabels_[DomEvolWrapperEventCapture::WandDownSavedEvent] = wandDownSaved_;
    valueLabels_[DomEvolWrapperEventCapture::SecondWandDownSavedEvent] = wand2DownSaved_;
    valueLabels_[DomEvolWrapperEventCapture::FilmSelectorUpSavedEvent] = filmSelectorUpSaved_;
    valueLabels_[DomEvolWrapperEventCapture::FilmSelectorDownSavedEvent] = filmSelectorDownSaved_;
    valueLabels_[DomEvolWrapperEventCapture::TankPressureReadingSavedEvent] = mainPressureSaved_;
    valueLabels_[DomEvolWrapperEventCapture::RegulatedPressureReadingSavedEvent] = linePressureSaved_;
    valueLabels_[DomEvolWrapperEventCapture::ApplierVacuumReadingSavedEvent] = vacuumSaved_;
    valueLabels_[DomEvolWrapperEventCapture::SecondApplierVacuumReadingSavedEvent] = vacuum2Saved_;
    valueLabels_[DomEvolWrapperEventCapture::ApplierHandHomeSavedEvent] = applierHandSaved_;
    valueLabels_[DomEvolWrapperEventCapture::SecondApplierHandHomeSavedEvent] = applier2HandSaved_;
    valueLabels_[DomEvolWrapperEventCapture::MainACVoltageReadingSavedEvent] = inputVoltageSaved_;

    /// Create vector of vectors to check for range values
    /// [0] = Lower Limit , [1] = Upper Limit
    validateRange_[DomEvolWrapperEventCapture::KnifeUpEvent][0]                       =  80; // Knife UP LOWER LIMIT
    validateRange_[DomEvolWrapperEventCapture::KnifeUpEvent][1]                       = 110; // Knife UP UPPER LIMIT
    validateRange_[DomEvolWrapperEventCapture::KnifeDownEvent][0]                     = 200; // Knife DOWN
    validateRange_[DomEvolWrapperEventCapture::KnifeDownEvent][1]                     = 400; //
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowCloseEvent][0]            =  20; // Gripper Narrow CLOSE
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowCloseEvent][1]            =  40;
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowOpenEvent][0]             =  65; // Gripper Narrow OPEN
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowOpenEvent][1]             =  95; // was 85, changed due to plant request 04-22-2019
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftCloseEvent][0]          =  20; // Gripper Wide Left CLOSE
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftCloseEvent][1]          =  40;
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftOpenEvent][0]           =  75; // Gripper Wide Left OPEN
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftOpenEvent][1]           = 110; // was 105, changed due to plant request 04-22-2019
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightCloseEvent][0]         =  20; // Gripper Wide Right CLOSE
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightCloseEvent][1]         =  40;
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightOpenEvent][0]          =  75; // Gripper Wide Right OPEN
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightOpenEvent][1]          = 110; // was 105, changed due to plant request 04-22-2019
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftCloseEvent][0]            =  40; // Side Clamp Left CLOSE
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftCloseEvent][1]            =  70;
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftOpenEvent][0]             = 120; // Side Clamp Left OPEN
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftOpenEvent][1]             = 160;
    validateRange_[DomEvolWrapperEventCapture::SideClampRightCloseEvent][0]           =  40; // Side Clamp Right CLOSE
    validateRange_[DomEvolWrapperEventCapture::SideClampRightCloseEvent][1]           =  70;
    validateRange_[DomEvolWrapperEventCapture::SideClampRightOpenEvent][0]            = 120; // Side Clamp Right OPEN
    validateRange_[DomEvolWrapperEventCapture::SideClampRightOpenEvent][1]            = 160;
    validateRange_[DomEvolWrapperEventCapture::WandUpEvent][0]                        = 400; // Wand UP
    validateRange_[DomEvolWrapperEventCapture::WandUpEvent][1]                        = 525;
    validateRange_[DomEvolWrapperEventCapture::WandDownEvent][0]                      = 675; // Wand DOWN
    validateRange_[DomEvolWrapperEventCapture::WandDownEvent][1]                      = 775;
    validateRange_[DomEvolWrapperEventCapture::SecondWandUpEvent][0]                  = 400; // Second Applier Wand UP
    validateRange_[DomEvolWrapperEventCapture::SecondWandUpEvent][1]                  = 525;
    validateRange_[DomEvolWrapperEventCapture::SecondWandDownEvent][0]                = 675; // Second Applier Wand DOWN
    validateRange_[DomEvolWrapperEventCapture::SecondWandDownEvent][1]                = 775;
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
    validateRange_[DomEvolWrapperEventCapture::SecondApplierVacuumReadingEvent][0]    =   4; // Second Vacuum
    validateRange_[DomEvolWrapperEventCapture::SecondApplierVacuumReadingEvent][1]    =  30;
    validateRange_[DomEvolWrapperEventCapture::ApplierHandHomeEvent][0]               = 120; // Applier Hand
    validateRange_[DomEvolWrapperEventCapture::ApplierHandHomeEvent][1]               = 200;
    validateRange_[DomEvolWrapperEventCapture::SecondApplierHandHomeEvent][0]         = 120; // Second Applier Hand
    validateRange_[DomEvolWrapperEventCapture::SecondApplierHandHomeEvent][1]         = 200;
    validateRange_[DomEvolWrapperEventCapture::MainACVoltageReadingEvent][0]          = 198; // Main AC Input Voltage
    validateRange_[DomEvolWrapperEventCapture::MainACVoltageReadingEvent][1]          = 242;
    // Saved Items              // Saved Items                  // Saved Items               // Saved Items
    validateRange_[DomEvolWrapperEventCapture::KnifeUpSavedEvent][0]                  =  80;
    validateRange_[DomEvolWrapperEventCapture::KnifeUpSavedEvent][1]                  = 110;
    validateRange_[DomEvolWrapperEventCapture::KnifeDownSavedEvent][0]                = 200;
    validateRange_[DomEvolWrapperEventCapture::KnifeDownSavedEvent][1]                = 400;
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowCloseSavedEvent][0]       =  20;
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowCloseSavedEvent][1]       =  40;
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowOpenSavedEvent][0]        =  65;
    validateRange_[DomEvolWrapperEventCapture::GripperNarrowOpenSavedEvent][1]        =  85;
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftCloseSavedEvent][0]     =  20;
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftCloseSavedEvent][1]     =  40;
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftOpenSavedEvent][0]      =  75;
    validateRange_[DomEvolWrapperEventCapture::GripperWideLeftOpenSavedEvent][1]      = 105;
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightCloseSavedEvent][0]    =  20;
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightCloseSavedEvent][1]    =  40;
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightOpenSavedEvent][0]     =  75;
    validateRange_[DomEvolWrapperEventCapture::GripperWideRightOpenSavedEvent][1]     = 105;
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftCloseSavedEvent][0]       =  40;
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftCloseSavedEvent][1]       =  70;
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftOpenSavedEvent][0]        = 120;
    validateRange_[DomEvolWrapperEventCapture::SideClampLeftOpenSavedEvent][1]        = 160;
    validateRange_[DomEvolWrapperEventCapture::SideClampRightCloseSavedEvent][0]      =  40;
    validateRange_[DomEvolWrapperEventCapture::SideClampRightCloseSavedEvent][1]      =  70;
    validateRange_[DomEvolWrapperEventCapture::SideClampRightOpenSavedEvent][0]       = 120;
    validateRange_[DomEvolWrapperEventCapture::SideClampRightOpenSavedEvent][1]       = 160;
    validateRange_[DomEvolWrapperEventCapture::WandUpSavedEvent][0]                   = 400;
    validateRange_[DomEvolWrapperEventCapture::WandUpSavedEvent][1]                   = 525;
    validateRange_[DomEvolWrapperEventCapture::WandDownSavedEvent][0]                 = 675;
    validateRange_[DomEvolWrapperEventCapture::WandDownSavedEvent][1]                 = 775;
    validateRange_[DomEvolWrapperEventCapture::SecondWandUpSavedEvent][0]             = 400;
    validateRange_[DomEvolWrapperEventCapture::SecondWandUpSavedEvent][1]             = 525;
    validateRange_[DomEvolWrapperEventCapture::SecondWandDownSavedEvent][0]           = 675;
    validateRange_[DomEvolWrapperEventCapture::SecondWandDownSavedEvent][1]           = 775;
    validateRange_[DomEvolWrapperEventCapture::FilmSelectorUpSavedEvent][0]           = 300;
    validateRange_[DomEvolWrapperEventCapture::FilmSelectorUpSavedEvent][1]           = 400;
    validateRange_[DomEvolWrapperEventCapture::FilmSelectorDownSavedEvent][0]         = 120;
    validateRange_[DomEvolWrapperEventCapture::FilmSelectorDownSavedEvent][1]         = 160;
    validateRange_[DomEvolWrapperEventCapture::TankPressureReadingSavedEvent][0]      =  88;
    validateRange_[DomEvolWrapperEventCapture::TankPressureReadingSavedEvent][1]      = 125;
    validateRange_[DomEvolWrapperEventCapture::RegulatedPressureReadingSavedEvent][0] =  45;
    validateRange_[DomEvolWrapperEventCapture::RegulatedPressureReadingSavedEvent][1] =  55;
    validateRange_[DomEvolWrapperEventCapture::ApplierVacuumReadingSavedEvent][0]     =   4;
    validateRange_[DomEvolWrapperEventCapture::ApplierVacuumReadingSavedEvent][1]     =  30;
    validateRange_[DomEvolWrapperEventCapture::SecondApplierVacuumReadingSavedEvent][0]    =   4;
    validateRange_[DomEvolWrapperEventCapture::SecondApplierVacuumReadingSavedEvent][1]    =  30;
    validateRange_[DomEvolWrapperEventCapture::ApplierHandHomeSavedEvent][0]          = 120;
    validateRange_[DomEvolWrapperEventCapture::ApplierHandHomeSavedEvent][1]          = 200;
    validateRange_[DomEvolWrapperEventCapture::SecondApplierHandHomeSavedEvent][0]         = 120;
    validateRange_[DomEvolWrapperEventCapture::SecondApplierHandHomeSavedEvent][1]         = 200;
    validateRange_[DomEvolWrapperEventCapture::MainACVoltageReadingSavedEvent][0]     = 198;
    validateRange_[DomEvolWrapperEventCapture::MainACVoltageReadingSavedEvent][1]     = 242;

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
    currentTimings_[DomEvolWrapperEventCapture::SecondWandUpEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::SecondWandDownEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::FilmSelectorUpEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::FilmSelectorDownEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::TankPressureReadingEvent] = "0";
    currentTimings_[DomEvolWrapperEventCapture::RegulatedPressureReadingEvent] = "0";      // 18
    currentTimings_[DomEvolWrapperEventCapture::ApplierVacuumReadingEvent] = "0";          // 19
    currentTimings_[DomEvolWrapperEventCapture::SecondApplierVacuumReadingEvent] = "0";    // 19
    currentTimings_[DomEvolWrapperEventCapture::ApplierHandHomeEvent] = "0";               // 20
    currentTimings_[DomEvolWrapperEventCapture::SecondApplierHandHomeEvent] = "0";          // 20
    currentTimings_[DomEvolWrapperEventCapture::MainACVoltageReadingEvent] = "0";          // 23

    savedTimings_[DomEvolWrapperEventCapture::KnifeUpSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::KnifeDownSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::GripperNarrowCloseSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::GripperNarrowOpenSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::GripperWideLeftCloseSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::GripperWideLeftOpenSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::GripperWideRightCloseSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::GripperWideRightOpenSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::SideClampLeftCloseSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::SideClampLeftOpenSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::SideClampRightCloseSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::SideClampRightOpenSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::WandUpSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::WandDownSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::SecondWandUpSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::SecondWandDownSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::FilmSelectorUpSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::FilmSelectorDownSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::TankPressureReadingSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::RegulatedPressureReadingSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::ApplierVacuumReadingSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::SecondApplierVacuumReadingSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::ApplierHandHomeSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::SecondApplierHandHomeSavedEvent] = "0";
    savedTimings_[DomEvolWrapperEventCapture::MainACVoltageReadingSavedEvent] = "0";

    readTimingsFromFile();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperActuatorDiag::createLayout()
{

    QHBoxLayout *buttonLayout( new QHBoxLayout() );

    buttonLayout->addStretch();
    buttonLayout->addWidget( diagStart_ );
    buttonLayout->addStretch();
    buttonLayout->addWidget( diagStop_ );
    buttonLayout->addStretch();
    buttonLayout->addWidget( saveTimings_ );
    buttonLayout->addStretch();
    buttonLayout->addWidget( saveHistory_ );
    buttonLayout->addStretch();
    buttonLayout->addWidget( clearHistory_ );
    buttonLayout->addStretch();

    // ---- //

    QVBoxLayout *upperLabelsLayout( new QVBoxLayout() );
    upperLabelsLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    // TOP - far left column labels for TOP 2 Groups
    upperLabelsLayout->addWidget( sensorTitle1_ );                              // Blank Placeholder For OPEN/CLOSE
    upperLabelsLayout->addWidget( openCloseTitle1_ );                           // Blank Placeholder for SAVED/CURRENT
    upperLabelsLayout->addWidget( gripperNarrowLabel_ );                        // Label Gripper Narrow
    upperLabelsLayout->addWidget( gripperWideLeftLabel_ );                      // Label Gripper Wide Left
    upperLabelsLayout->addWidget( gripperWideRightLabel_ );                     // Label Gripper Wide Right
    upperLabelsLayout->addWidget( sideClampLeftLabel_ );                        // Label Side Clamp Left
    upperLabelsLayout->addWidget( sideClampRightLabel_ );                       // Label Side Clamp Right
    upperLabelsLayout->addWidget( sensorTitle2_ );                              // Blank placeholder For UP/DOWN
    upperLabelsLayout->addWidget( upDownTitle1_ );                              // Blank placeholder For SAVED/CURRENT
    upperLabelsLayout->addWidget( knifeLabel_ );                                // Label Knife
    upperLabelsLayout->addWidget( wandLabel_ );                                 // Label Wand
    upperLabelsLayout->addWidget( wand2Label_ );                                // Second Label Applier Wand
    upperLabelsLayout->addWidget( filmSelectorLabel_ );                         // Label Film Selector

    // GRIDLAYOUT UNDER OPEN
    QGridLayout *leftValuesLayout( new QGridLayout() );                         // ORIG QVBoxLayout
    leftValuesLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    leftValuesLayout->setSpacing( 1 );
    // LEFT VERTICLE GROUP UNDER OPEN
    leftValuesLayout->addWidget( sensorOpenLabel_,0,0,1,2,Qt::AlignCenter );    // OPEN
    leftValuesLayout->addWidget( openSavedLabel_,1,0 );                         // Saved
    leftValuesLayout->addWidget( openCurrentLabel_,1,1 );                       // Current
    leftValuesLayout->addWidget( gripperNarrowOpenSaved_,2,0 );
    leftValuesLayout->addWidget( gripperNarrowOpen_,2,1 );
    leftValuesLayout->addWidget( gripperWideLeftOpenSaved_,3,0);
    leftValuesLayout->addWidget( gripperWideLeftOpen_,3,1 );
    leftValuesLayout->addWidget( gripperWideRightOpenSaved_,4,0 );
    leftValuesLayout->addWidget( gripperWideRightOpen_,4,1 );
    leftValuesLayout->addWidget( sideClampLeftOpenSaved_,5,0 );
    leftValuesLayout->addWidget( sideClampLeftOpen_,5,1 );
    leftValuesLayout->addWidget( sideClampRightOpenSaved_,6,0 );
    leftValuesLayout->addWidget( sideClampRightOpen_,6,1 );
    // LEFT VERTICLE GROUP UNDER UP
    leftValuesLayout->addWidget( sensorUpLabel_,7,0,1,2,Qt::AlignCenter );      // UP
    leftValuesLayout->addWidget( upSavedLabel_,8,0 );
    leftValuesLayout->addWidget( upCurrentLabel_,8,1 );
    leftValuesLayout->addWidget( knifeUpSaved_,9,0 );
    leftValuesLayout->addWidget( knifeUp_,9,1 );
    leftValuesLayout->addWidget( wandUpSaved_,10,0 );
    leftValuesLayout->addWidget( wandUp_,10,1 );
    leftValuesLayout->addWidget( wand2UpSaved_,11,0 );
    leftValuesLayout->addWidget( wand2Up_,11,1 );
    leftValuesLayout->addWidget( filmSelectorUpSaved_,12,0 );
    leftValuesLayout->addWidget( filmSelectorUp_,12,1 );

    QGridLayout *rightValuesLayout( new QGridLayout );                          // ORIG QVBoxLayout
    rightValuesLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    rightValuesLayout->setSpacing( 1 );
    // RIGHT VERTICLE GROUP UNDER OPEN
    rightValuesLayout->addWidget( sensorCloseLabel_ ,0,0,1,2,Qt::AlignCenter);  // CLOSE
    rightValuesLayout->addWidget( closeSavedLabel_,1,0 );
    rightValuesLayout->addWidget( closeCurrentLabel_,1,1 );
    rightValuesLayout->addWidget( gripperNarrowCloseSaved_,2,0 );
    rightValuesLayout->addWidget( gripperNarrowClose_,2,1 );
    rightValuesLayout->addWidget( gripperWideLeftCloseSaved_,3,0 );
    rightValuesLayout->addWidget( gripperWideLeftClose_,3,1 );
    rightValuesLayout->addWidget( gripperWideRightCloseSaved_,4,0 );
    rightValuesLayout->addWidget( gripperWideRightClose_,4,1 );
    rightValuesLayout->addWidget( sideClampLeftCloseSaved_,5,0 );
    rightValuesLayout->addWidget( sideClampLeftClose_,5,1 );
    rightValuesLayout->addWidget( sideClampRightCloseSaved_,6,0 );
    rightValuesLayout->addWidget( sideClampRightClose_,6,1 );
    // RIGHT VERTICLE GROUP UNDER DOWN
    rightValuesLayout->addWidget( sensorDownLabel_,7,0,1,2,Qt::AlignCenter );
    rightValuesLayout->addWidget( downSavedLabel_,8,0 );
    rightValuesLayout->addWidget( downCurrentLabel_,8,1 );
    rightValuesLayout->addWidget( knifeDownSaved_,9,0 );
    rightValuesLayout->addWidget( knifeDown_,9,1 );
    rightValuesLayout->addWidget( wandDownSaved_,10,0 );
    rightValuesLayout->addWidget( wandDown_,10,1 );
    rightValuesLayout->addWidget( wand2DownSaved_,11,0 );
    rightValuesLayout->addWidget( wand2Down_,11,1 );
    rightValuesLayout->addWidget( filmSelectorDownSaved_,12,0 );
    rightValuesLayout->addWidget( filmSelectorDown_,12,1 );

    QHBoxLayout *upperChartLayout( new QHBoxLayout() );
    upperChartLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    upperChartLayout->setSpacing( 1 );

    upperChartLayout->addStretch();
    upperChartLayout->addLayout( upperLabelsLayout );
    upperChartLayout->addLayout( leftValuesLayout );
    upperChartLayout->addLayout( rightValuesLayout );
    upperChartLayout->addStretch();

    // ---- //

    // Bottom Center Layout
    QVBoxLayout *lowerLabelsLayout( new QVBoxLayout() );                        // ORIG QVBoxLayout
    lowerLabelsLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    lowerLabelsLayout->setSpacing( 1 );

    lowerLabelsLayout->addWidget( sensorTitle3_ );                              // Blank placeholder
    lowerLabelsLayout->addWidget( homeTitle1_ );                                // Blank Placeholder for Saved / Current
    lowerLabelsLayout->addWidget( applierHandLabel_ );                          // Label Applier Hand
    lowerLabelsLayout->addWidget( applier2HandLabel_ );                         // Second Label Applier Hand
    lowerLabelsLayout->addWidget( vacuumLabel_ );                               // Label Vacuum
    lowerLabelsLayout->addWidget( vacuum2Label_ );                              // Second Label Vacuum
    lowerLabelsLayout->addWidget( inputVoltageLabel_ );                         // Label Input Voltage
    lowerLabelsLayout->addWidget( mainPressureLabel_ );                         // Label Tank Pressure
    lowerLabelsLayout->addWidget( linePressureLabel_ );                         // Label Regulated Pressure

    QGridLayout *lowerValuesLayout( new QGridLayout() );                        // ORIG QVBoxLayout
    lowerValuesLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    lowerValuesLayout->setSpacing( 1 );

    lowerValuesLayout->addWidget( sensorHomeLabel_,0,0,1,2,Qt::AlignCenter);             // HOME COLUMN TITLE
    lowerValuesLayout->addWidget( homeSavedLabel_,1,0 );
    lowerValuesLayout->addWidget( homeCurrentLabel_,1,1 );
    lowerValuesLayout->addWidget( applierHandSaved_,2,0 );
    lowerValuesLayout->addWidget( applierHand_,2,1 );
    lowerValuesLayout->addWidget( applier2HandSaved_,3,0 );
    lowerValuesLayout->addWidget( applier2Hand_,3,1 );
    lowerValuesLayout->addWidget( vacuumSaved_,4,0 );
    lowerValuesLayout->addWidget( vacuum_,4,1 );
    lowerValuesLayout->addWidget( vacuum2Saved_,5,0 );
    lowerValuesLayout->addWidget( vacuum2_,5,1 );
    lowerValuesLayout->addWidget( inputVoltageSaved_,6,0 );
    lowerValuesLayout->addWidget( inputVoltage_,6,1 );
    lowerValuesLayout->addWidget( mainPressureSaved_,7,0 );
    lowerValuesLayout->addWidget( mainPressure_,7,1 );
    lowerValuesLayout->addWidget( linePressureSaved_,8,0 );
    lowerValuesLayout->addWidget( linePressure_,8,1 );

    QHBoxLayout *lowerChartLayout( new QHBoxLayout() );
    lowerChartLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    lowerChartLayout->setSpacing( 1 );

    lowerChartLayout->addStretch();
    lowerChartLayout->addLayout( lowerLabelsLayout );
    lowerChartLayout->addLayout( lowerValuesLayout );
    lowerChartLayout->addStretch();

    QHBoxLayout *lastSavedLayout( new QHBoxLayout() );
    lastSavedLayout->addStretch();
    lastSavedLayout->addWidget(lastSavedLabel_);
    lastSavedLayout->addWidget(lastSaved_);

    // ---- //

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins( 25, 8, 25, 8 ) );           // 25 was 48

    // add 2nd applier need the room - screenLayout->addWidget( title_ );
    //screenLayout->addStretch();
    screenLayout->addLayout( buttonLayout );
    screenLayout->addStretch();
    screenLayout->addLayout( upperChartLayout );
    screenLayout->addLayout( lowerChartLayout );
    screenLayout->addLayout( lastSavedLayout );

    // PJR-145: Wrap-only
    if( session_->uiUtilities->isWrapOnly() )
    {
        wandLabel_->hide();
        wandUp_->hide();
        wandDown_->hide();

        wand2Label_->hide();
        wand2Up_->hide();
        wand2Down_->hide();

        filmSelectorLabel_->hide();
        filmSelectorUp_->hide();
        filmSelectorDown_->hide();

        applierHandLabel_->hide();
        applierHand_->hide();

        applier2HandLabel_->hide();
        applier2Hand_->hide();

        vacuumLabel_->hide();
        vacuum_->hide();

        vacuum2Label_->hide();
        vacuum2_->hide();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
QLabel *serviceNewWrapperActuatorDiag::createHeaderLabel( const QString &name, bool isTitle )
{
    QFont f( font() );
    f.setPointSize( isTitle ? TITLE_FONT_POINT_SIZE : FONT_POINT_SIZE );

    static const modifyPaletteBackground blackBackground( hobart::colorPalette::black );
    static const modifyPaletteForeground whiteForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    // label
    QLabel *l( new QLabel( this ) );
    l->setAlignment( Qt::AlignCenter );
    l->setAutoFillBackground( true );
    l->setFont( f );
    l->setObjectName( name );

    blackBackground( l );
    whiteForeground( l );

    return l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QLabel *serviceNewWrapperActuatorDiag::createFootnoteLabel( const QString &name )
{
    QFont f( font() );
    f.setPointSize( 8 );

    static const modifyPaletteBackground blackBackground( hobart::colorPalette::black );
    static const modifyPaletteForeground whiteForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    // label
    QLabel *l( new QLabel( this ) );
    l->setAlignment( Qt::AlignCenter );
    l->setAutoFillBackground( true );
    l->setFont( f );
    l->setObjectName( name );

    blackBackground( l );
    whiteForeground( l );

    return l;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
QLabel *serviceNewWrapperActuatorDiag::createTitleLabel( const QString &name, Qt::Alignment alignment )
{
    QFont f( font() );
    f.setPointSize( FONT_POINT_SIZE );

    static const modifyPaletteBackground setBackgroundPallette( QColor( 192, 192, 192 ) );
    static const modifyPaletteForeground blackForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::black ) );

    // label
    QLabel *l( new QLabel( this ) );
    l->setAlignment( alignment );
    l->setAutoFillBackground( true );           // NEW - REMOVE LATER MAYBE
    l->setFont( f );
    l->setObjectName( name );

    blackForeground( l );
    setBackgroundPallette( l );

    return l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QLabel *serviceNewWrapperActuatorDiag::createLabel( const QString &name, Qt::Alignment alignment )
{
    QFont f( font() );
    f.setPointSize( FONT_POINT_SIZE );

    static const modifyPaletteForeground blackForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::black ) );
    static const modifyPaletteBackground whiteBackground( hobart::colorPalette::white );

    // label
    QLabel *l( new QLabel( this ) );
    l->setAlignment( alignment );
    l->setAutoFillBackground( true );           // NEW - REMOVE LATER MAYBE
    l->setFont( f );
    l->setObjectName( name );

    blackForeground( l );
    whiteBackground( l );

    return l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QLabel *serviceNewWrapperActuatorDiag::createSimpleLabel( const QString &name, Qt::Alignment alignment )
{
    QFont f( font() );
    f.setPointSize( FONT_POINT_SIZE );

    // label
    QLabel *l( new QLabel( this ) );
    l->setAlignment( alignment );
    l->setAutoFillBackground( true );
    l->setFont( f );
    l->setObjectName( name );

    return l;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperActuatorDiag::outOfRangeLabel(int index, int curValue, QLabel *checkLabel )
{
    // I was exiting based on this condition, but if it was previously red (which we wont know)
    // then we need to turn it back white again so if else it.

    bool good_reading = false;

    if( curValue >= validateRange_[index][0] && curValue <= validateRange_[index][1] )
        good_reading = true;        //    return;

    // If it's not in the current range, which lets you exit this method - it must INVALID

    static const modifyPaletteBackground redBackground( QColor( 255, 0, 255 ) );     // cant use red, network connectivity
    static const modifyPaletteBackground whiteBackground( hobart::colorPalette::white );

    static const modifyPaletteForeground whiteForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );
    static const modifyPaletteForeground blackForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::black ) );


    checkLabel->setAutoFillBackground( true );

    if( good_reading )
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

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperActuatorDiag::inRangeByTenLabel(int index, bool curVal)
{
    int leftIndex = 0;
    int rightIndex = 0;
    int doIt = false;       // not all values are tested so bail if we dont have right value

    switch ( index )
    {
        case DomEvolWrapperEventCapture::GripperWideLeftCloseEvent:
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftCloseEvent;
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightCloseEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::GripperWideLeftOpenEvent:
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftOpenEvent;
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightOpenEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::GripperWideRightCloseEvent:
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightCloseEvent;
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftCloseEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::GripperWideRightOpenEvent:
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightOpenEvent;
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftOpenEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::SideClampLeftCloseEvent:
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftCloseEvent;
            rightIndex = DomEvolWrapperEventCapture::SideClampRightCloseEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::SideClampLeftOpenEvent:
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftOpenEvent;
            rightIndex = DomEvolWrapperEventCapture::SideClampRightOpenEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::SideClampRightCloseEvent:
            rightIndex = DomEvolWrapperEventCapture::SideClampRightCloseEvent;
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftCloseEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::SideClampRightOpenEvent:
            rightIndex = DomEvolWrapperEventCapture::SideClampRightOpenEvent;
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftOpenEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::GripperWideLeftCloseSavedEvent:
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftCloseSavedEvent;
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightCloseSavedEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::GripperWideLeftOpenSavedEvent:
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftOpenSavedEvent;
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightOpenSavedEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::GripperWideRightCloseSavedEvent:
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightCloseSavedEvent;
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftCloseSavedEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::GripperWideRightOpenSavedEvent:
            rightIndex = DomEvolWrapperEventCapture::GripperWideRightOpenSavedEvent;
            leftIndex  = DomEvolWrapperEventCapture::GripperWideLeftOpenSavedEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::SideClampLeftCloseSavedEvent:
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftCloseSavedEvent;
            rightIndex = DomEvolWrapperEventCapture::SideClampRightCloseSavedEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::SideClampLeftOpenSavedEvent:
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftOpenSavedEvent;
            rightIndex = DomEvolWrapperEventCapture::SideClampRightOpenSavedEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::SideClampRightCloseSavedEvent:
            rightIndex = DomEvolWrapperEventCapture::SideClampRightCloseSavedEvent;
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftCloseSavedEvent;
            doIt = true;
        break;

        case DomEvolWrapperEventCapture::SideClampRightOpenSavedEvent:
            rightIndex = DomEvolWrapperEventCapture::SideClampRightOpenSavedEvent;
            leftIndex  = DomEvolWrapperEventCapture::SideClampLeftOpenSavedEvent;
            doIt = true;
        break;
    }

    if( doIt )
    {
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

        QLabel *leftLabel( valueLabels_[leftIndex] );
        QLabel *rightLabel( valueLabels_[rightIndex] );

        // If the 2 values are not within 10, which lets you exit this method - it must INVALID

        static const modifyPaletteBackground redBackground( QColor( 255, 0, 255 ) );     // cant use red, network connectivity
        static const modifyPaletteForeground whiteForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

        leftLabel->setAutoFillBackground( true );
        rightLabel->setAutoFillBackground( true );

        redBackground( leftLabel );
        redBackground( rightLabel );
        whiteForeground( leftLabel );
        whiteForeground( rightLabel );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperActuatorDiag::saveTimingsToFile()
{
    LOG_CLASS_INFO( "serviceNewWrapperActuatorDiag", "saveTimingsToFile" );

    LOG_DEBUG("saveTimingsToFile() START", NULL);

    // save current timings CSV style to a file for future reference
    QString timingsInfo = "";
    QStringList argsList;
    argsList.clear();

    QDateTime now( session_->scaleConfig->currentDate(), session_->scaleConfig->currentTime() );
    QDate today = now.date();
    QTime time  = now.time();
    QString myDate = today.toString("MMM dd, yyyy");
    QString myTime = time.toString("HH:mm");
    QString myDateTime = myDate + " " + myTime;
    lastSaved_->setText(myDateTime);                    // update the date on the screen

    timingsInfo.append(myDateTime);                     // the first item in the csv is the date time
    QString tmpVal = "";

    // Save the current timings to a file.  These BECOME the saved timings now

    foreach(int j, currentTimings_.keys() )
    {
        timingsInfo.append(",");
        tmpVal = currentTimings_.value(j);
        timingsInfo.append(tmpVal);
        //qDebug() << j << ": " << currentTimings_.value(j) << '\n';
    }

    argsList.append(timingsInfo);

    python pythonProcess;

    if (pythonProcess.execCommand("outputActuatorTimings", argsList))
    {
        if (pythonProcess.getExitCode() == 0)
        {
            LOG_DEBUG("outputActuatorTimings called, exitCode = 0 ", NULL);

            session_->send(Error, ErrWrapperDevice, "9726,1");   // Tell user we saved the file successfully

            // Now update any saved timings that were being displayed on the screen.

            readTimingsFromFile();      // hopefully they are there.
        }
        else
        {
            LOG_DEBUG("Save Error, exit code = %d", pythonProcess.getExitCode());
            session_->send(Error, ErrWrapperDevice, "9727,1");   // Tell user some items missing and cant process it

        }
    }
    else
    {
        LOG_DEBUG("Save Python Error, exit code = %d", pythonProcess.getExitCode());
        session_->send(Error, ErrWrapperDevice, "9728,1");   // Tell user some items missing and cant process it
    }

    LOG_DEBUG("saveTimingsToFile() END", NULL);
}

void serviceNewWrapperActuatorDiag::readTimingsFromFile()
{
    QStringList argsList;
    argsList.clear();

#ifdef Q_OS_WIN
    QString filename("C:/hobart/actuatorTimings.log");          // change to C:/Program Files/hobart/actuatorTimings.log
#else
    QString filename("/usr/local/hobart/actuatorTimings.log");
#endif // Q_OS_WIN

    QFile f( filename );

    if ( f.open( QIODevice::ReadOnly ) )
    {
        QTextStream fs( &f );                           //

        QString line( fs.readLine().trimmed() );        // only have one line

        QStringList timingsList( line.split( ',' ) );   // validate line

        if( 0 == timingsList.count() || timingsList.isEmpty() )
        {
            session_->send(Error, ErrWrapperDevice, "9725,1");   //  Tell user the file was empty even thou it exists
            return;
        }

        if( timingsList.count() < MAX_TIMINGS )
        {
            session_->send(Error, ErrWrapperDevice, "9724,1");   // Tell user some items missing and cant process it
            return;                                     // would not know how to map values
        }

        // process and display date
        QString dateStr = timingsList.at(0) + ", " + timingsList.at(1);
        lastSaved_->setText( dateStr );

        // now remove first 2 as they are dates and I dont need them to populate the enum list of values
        timingsList.removeFirst();
        timingsList.removeFirst();

        //for(int h = 0; h < (MAX_TIMINGS-2); h++)
        //    qDebug() << "timingsList[" << h << "]: " << timingsList[h];

        QMap<int, QString>::iterator itty;
        QString output;
        for (itty = savedTimings_.begin(); itty != savedTimings_.end(); ++itty)
        {
            // Format output here.
            output += QString("%1:%2").arg(itty.key()).arg(itty.value());
            output += ", ";
        }
        //qDebug() << "output: " << output;

        int j = 0;
        int new_key = 0;
        QString new_value;

        for ( QMap<int, QString>::const_iterator it = savedTimings_.cbegin(), end = savedTimings_.cend(); it != end; ++it )
        {
            new_key = it.key();                         // just want to check the values
            new_value = timingsList[j];                 // and see them before I move on

            //qDebug() << "The key["   << j << "]: " << new_key;
            //qDebug() << "The value[" << j << "]: " << new_value;   // it.value = (*it)

            if ( valueLabels_.contains( new_key ) )
            {
                QLabel *l( valueLabels_[new_key] );

                if ( l )
                {
                    l->setText( new_value );

                    savedTimings_[new_key] = new_value;

                    outOfRangeLabel(new_key, new_value.toInt(), l);  // checks all current values - not saved values
                    inRangeByTenLabel( new_key, false );             // is it within 10 of comparable field?

                    //qDebug() << "loop "   << j << " done." << new_key;
                }
            }
            j++;
        }
    }
    else
        session_->send(Error, ErrWrapperDevice, "9723,1");       //  Tell user file does not exist to read
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperActuatorDiag::createFakeData()
{
    // These are simulated as incoming messages to update the fields as the current readings
/*
    QString sensorVal = "";

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

    //sensorVal = "18"; // orig
    sensorVal = "31";
    gripperWideLeftClose_->setText(sensorVal);
    QLabel *label20( valueLabels_[DomEvolWrapperEventCapture::GripperWideLeftCloseEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::GripperWideLeftCloseEvent, sensorVal.toInt(), label20);
    inRangeByTenLabel( DomEvolWrapperEventCapture::GripperWideLeftCloseEvent );                           // is it within 10 of comparable field?

    //sensorVal = "43"; // orig
    sensorVal = "36";
    gripperWideRightClose_->setText(sensorVal);
    QLabel *label22( valueLabels_[DomEvolWrapperEventCapture::GripperWideRightCloseEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::GripperWideRightCloseEvent, sensorVal.toInt(), label22);
    inRangeByTenLabel( DomEvolWrapperEventCapture::GripperWideRightCloseEvent );

    //sensorVal = "88"; // orig
    sensorVal = "83";
    gripperWideLeftOpen_->setText(sensorVal);
    QLabel *label4( valueLabels_[DomEvolWrapperEventCapture::GripperWideLeftOpenEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::GripperWideLeftOpenEvent, sensorVal.toInt(), label4);
    inRangeByTenLabel( DomEvolWrapperEventCapture::GripperWideLeftOpenEvent );

    //sensorVal = "90"; // orig
    sensorVal = "83"; // orig
    gripperWideRightOpen_->setText(sensorVal);
    QLabel *label6( valueLabels_[DomEvolWrapperEventCapture::GripperWideRightOpenEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::GripperWideRightOpenEvent, sensorVal.toInt(), label6);
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

    wandUp_->setText("465");
    QLabel *label14( valueLabels_[DomEvolWrapperEventCapture::WandUpEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::WandUpEvent, 465, label14);

    wand2Up_->setText("466");
    QLabel *label14b( valueLabels_[DomEvolWrapperEventCapture::SecondWandUpEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::SecondWandUpEvent, 465, label14b);

    wandDown_->setText("704");
    QLabel *label30( valueLabels_[DomEvolWrapperEventCapture::WandDownEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::WandDownEvent, 704, label30);

    wand2Down_->setText("705");
    QLabel *label30b( valueLabels_[DomEvolWrapperEventCapture::SecondWandDownEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::WandDownEvent, 704, label30b);

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

    vacuum2_->setText("25");
    QLabel *label38b( valueLabels_[DomEvolWrapperEventCapture::ApplierVacuumReadingEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::SecondApplierVacuumReadingEvent, 25, label38b);

    applierHand_->setText("146");
    QLabel *label34( valueLabels_[DomEvolWrapperEventCapture::ApplierHandHomeEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::ApplierHandHomeEvent, 146, label34);

    applier2Hand_->setText("147");
    QLabel *label34b( valueLabels_[DomEvolWrapperEventCapture::SecondApplierHandHomeEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::ApplierHandHomeEvent, 147, label34b);

    inputVoltage_->setText("230");
    QLabel *label36( valueLabels_[DomEvolWrapperEventCapture::MainACVoltageReadingEvent] );
    outOfRangeLabel(DomEvolWrapperEventCapture::MainACVoltageReadingEvent, 230, label36);

    currentTimings_[DomEvolWrapperEventCapture::KnifeUpEvent] = "92";
    currentTimings_[DomEvolWrapperEventCapture::KnifeDownEvent] = "254";
    currentTimings_[DomEvolWrapperEventCapture::GripperNarrowCloseEvent] = "32";
    currentTimings_[DomEvolWrapperEventCapture::GripperNarrowOpenEvent] = "69";

    //currentTimings_[DomEvolWrapperEventCapture::GripperWideLeftCloseEvent] = "18";
    //currentTimings_[DomEvolWrapperEventCapture::GripperWideRightCloseEvent] = "43";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideLeftCloseEvent] = "31";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideRightCloseEvent] = "36";

    //currentTimings_[DomEvolWrapperEventCapture::GripperWideLeftOpenEvent] = "88";
    //currentTimings_[DomEvolWrapperEventCapture::GripperWideRightOpenEvent] = "90";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideLeftOpenEvent] = "83";
    currentTimings_[DomEvolWrapperEventCapture::GripperWideRightOpenEvent] = "83";

    currentTimings_[DomEvolWrapperEventCapture::SideClampLeftCloseEvent] = "56";
    currentTimings_[DomEvolWrapperEventCapture::SideClampLeftOpenEvent] = "127";
    currentTimings_[DomEvolWrapperEventCapture::SideClampRightCloseEvent] = "67";
    currentTimings_[DomEvolWrapperEventCapture::SideClampRightOpenEvent] = "152";
    currentTimings_[DomEvolWrapperEventCapture::WandUpEvent] = "465";
    currentTimings_[DomEvolWrapperEventCapture::WandDownEvent] = "704";
    currentTimings_[DomEvolWrapperEventCapture::SecondWandUpEvent] = "466";                      // pos 24 in file
    currentTimings_[DomEvolWrapperEventCapture::SecondWandDownEvent] = "705";                    // 25
    currentTimings_[DomEvolWrapperEventCapture::FilmSelectorUpEvent] = "381";
    currentTimings_[DomEvolWrapperEventCapture::FilmSelectorDownEvent] = "154";
    currentTimings_[DomEvolWrapperEventCapture::TankPressureReadingEvent] = "107";
    currentTimings_[DomEvolWrapperEventCapture::RegulatedPressureReadingEvent] = "43";      // 18
    currentTimings_[DomEvolWrapperEventCapture::ApplierVacuumReadingEvent] = "24";          // 19
    currentTimings_[DomEvolWrapperEventCapture::SecondApplierVacuumReadingEvent] = "25";    // 26
    currentTimings_[DomEvolWrapperEventCapture::ApplierHandHomeEvent] = "146";               // 20
    currentTimings_[DomEvolWrapperEventCapture::SecondApplierHandHomeEvent] = "147";        // 27
    currentTimings_[DomEvolWrapperEventCapture::MainACVoltageReadingEvent] = "230";          // 23
*/
}
