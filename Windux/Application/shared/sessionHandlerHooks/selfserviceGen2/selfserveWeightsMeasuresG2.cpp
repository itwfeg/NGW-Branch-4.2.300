/**
 * @file selfserveWeightsMeasures.cpp
 *
 * @copyright Copyright (c) 2023 Hobart Corporation
 * @author Joe Crew
 * @ appropriated from Randy Blankley
 */

#include "selfserveWeightsMeasuresG2.h"

#include "ui_selfserveWeightsMeasuresG2.h"
#include <backgroundwidgettypes.h>
#include <pushbutton.h>
#include <selfservicehook.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QPainter>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QDesktopWidget>

#include <Session.h>
#include <UiUtilities.h>

#include "../../sessionHandler/sessionmainwindow.h"
#include <ScaleConfiguration.h>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveWeightsMeasuresG2::selfserveWeightsMeasuresG2( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveWeightsMeasuresG2),
    hook_( h ),
    unlocked_( false ),
    cat3_( false ),
    errorcode_( "0" ),
    errorstyle_( -1 ),
    activeKey_( NULL ),
    cycleNumpadTimer_( NULL )
{
    ui->setupUi(this);
    setObjectName( "selfserveWeightsMeasuresG2" );

    // CAT3
    // Check if the right weigher model is connected
    Session *s( hook_->session() );

    wayer_model = SessionMainWindow::None;

    if (( s ) && ( s->uiUtilities->isFeatureLicensed( SCALE_CAT3_AUDITTRAIL ) ))
    {
        DomWeigherConfigRead *weigherFields( &s->scaleConfig->weigherConfigRead );
        wayer_model = weigherFields->weigherModel();

        // SOF-894
        MadpScaleConfig *settings = &s->scaleConfig->scaleConfig;
        countryCode = settings->countryCode();

        if (( SessionMainWindow::HLX1 == wayer_model ) ||
            ( SessionMainWindow::SASCALE1 == wayer_model ) ||       // HT-374
            ( SessionMainWindow::SASCALE2 == wayer_model ) ||
            (SessionMainWindow::HTM == wayer_model ) )              // SOF-3140
        {
            cat3_ = true;
        }
    }

    // initialize
    initialize();
    createLayout();
    translate();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveWeightsMeasuresG2::~selfserveWeightsMeasuresG2()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasuresG2::refresh()
{
    // update fields
    QStringList fields( hook_->fieldValue( sessionHooks::weightsAndMeasuresInfo ).split( QChar( 0x1e ) ) );

    if ( fields.size() >= 12 )
    {
        //indicator_SoftwareVersion_->setText( fields.at( 0 ) );
        //indicator_SoftwareVersion_->repaint();
        indicator_Approval_->setText( fields.at( 1 ) );
        indicator_Approval_->repaint();
        indicator_nMax_->setText( fields.at( 2 ) );
        indicator_nMax_->repaint();

        weigherModel_->setText( fields.at( 3 ) );
        weigherModel_->repaint();
        softwareVersion_->setText( fields.at( 4 ) );
        softwareVersion_->repaint();
        hardwareVersion_->setText( fields.at( 5 ) );
        hardwareVersion_->repaint();
        capacity_->setText( fields.at( 6 ) );
        capacity_->repaint();
        eMin_->setText( fields.at( 7 ) );
        eMin_->repaint();
        class_->setText( fields.at( 8 ) );
        class_->repaint();
        nMax_->setText( fields.at( 9 ) );
        nMax_->repaint();
        approvalUS_->setText( fields.at( 10 ) );
        approvalUS_->repaint();
        approvalCan_->setText( fields.at( 11 ) );
        approvalCan_->repaint();
    }

    if ( cat3_ )
    {
        // Different offset for Temperature Range data when CAT-III license is installed.
        // Config Parameters and Calibration are only displayed when CAT-I
        if( fields.size() >= 13 )
        {
            tempRange_->setText( fields.at( 12 ) );
            tempRange_->repaint();
        }
    }
    else
    {
        QString lastChanged( "\t\t" + tr( "Last Changed" ) + " " );

        if( fields.size() >= 14 )
        {
            if ( fields.at( 13 ).trimmed().isEmpty() )
                configParams_->setText( fields.at( 12 ) );
            else
                configParams_->setText( fields.at( 12 ) + lastChanged + fields.at( 13 ) );

            configParams_->repaint();
        }

        if( fields.size() >= 16 )
        {
            if ( fields.at( 15 ).trimmed().isEmpty() )
                calibration_->setText( fields.at( 14 ) );
            else
                calibration_->setText( fields.at( 14 ) + lastChanged + fields.at( 15 ) );

            calibration_->repaint();
        }

        if( fields.size() >= 17 )
        {
            tempRange_->setText( fields.at( 16 ) );
            tempRange_->repaint();
        }
    }

    // check for unlocked screen
    if ( !unlocked_ )
    {
        if (( "1" == hook_->fieldValue( sessionHooks::selfServiceKey ) ) ||
            ( "1" == hook_->fieldValue( sessionHooks::serviceKey ) ) ||
            ( "1" == hook_->fieldValue( sessionHooks::supervisorKey ) ))
        {
            unlocked_ = true;
        }
    }

    quit_->setVisible( unlocked_ );
    title_->setVisible( !unlocked_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasuresG2::translate()
{
    rezero_->setText( tr( "Rezero" ) );
    auditlog_->setText( tr("Audit\nLog") );
    quit_->setText( tr( "Exit Self-\nService Mode" ) );
    exit_->setText( "X" );

    title_->setText( tr( "Enter ID to exit self-service mode,\nthen press Enter." ) );

    exitLabel_->setText( tr( "Close" ) );

    //indicator_SoftwareVersionLabel_->setText( tr( " " ) );          // no longer used
    indicator_ApprovalLabel_->setText( tr( "Indicator Approval" ) );
    indicator_nMaxLabel_->setText( tr( "Indicator n<sub>max</sub>" ) );

    weigherModelLabel_->setText( tr( "Weigher Model" ) );
    softwareVersionLabel_->setText( tr( "Metrology Software Version" ) );
    hardwareVersionLabel_->setText( tr( "Hardware Version" ) );
    capacityLabel_->setText( tr( "Capacity" ) );
    eMinLabel_->setText( tr( "e<sub>min</sub>" ) );
    classLabel_->setText( tr( "Class" ) );
    nMaxLabel_->setText( tr( "n<sub>max</sub>" ) );
    approvalUSLabel_->setText( tr( "Approval (US)" ) );
    approvalCanLabel_->setText( tr( "Approval (CAN)" ) );

    // hidden if cat 3
    configParamsLabel_->setText( tr( "Config Parameters" ) );
    calibrationLabel_->setText( tr( "Calibration" ) );

    tempRangeLabel_->setText( tr( "Temperature Range" ) );

    enter_->setText( tr( "Enter" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasuresG2::updateNetworkConnectivity(bool online)
{
    if( online )
        screen_->setStyleSheet( QLatin1String("background-color: rgb(244,247,250);" ) );
    else
        screen_->setStyleSheet( QLatin1String("background-color: rgb(255,0,0);" ) );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasuresG2::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasuresG2::showEvent( QShowEvent *e )
{
    opID_->blockSignals( true );
    opID_->clear();
    opID_->blockSignals( false );

    // force refresh
    unlocked_ = false;
    refresh();

    if( "1" == sessionHooks::instance()->getFieldValue( sessionHooks::selfServiceRestrictedDialog ) )
    {
        errorcode_ = hook_->getErrorStateDialogCode();
        errorstyle_ = hook_->getErrorStateDialogStyle();
        //hook_->session()->send( Error, ErrRemoveAnyModalDialogDevice,  errorcode_ + "," + QString::number(errorstyle_) );
        hook_->onHideErrDialog();
    }

    _Mybase::showEvent( e );
}

void selfserveWeightsMeasuresG2::hideEvent( QHideEvent *e)
{
    if( ( errorcode_.toInt() > 0 ) && ( errorstyle_ > 0 ) )
    {
        int errorevent = errorcode_.toInt() / 1000;
        hook_->session()->send( Error, errorevent,  errorcode_ + "," + QString::number(errorstyle_) );
        errorcode_ = "0";
        errorstyle_ = -1;
        hook_->onShowErrDialog();
    }

    _Mybase::hideEvent( e );
}



///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfserveWeightsMeasuresG2::eventFilter( QObject *obj, QEvent *e )
{
    // exit self service mode when exit pressed
    if ( exitLabel_ == obj )
        if ( QEvent::MouseButtonRelease == e->type() )
            hook_->setCurrentState( selfServiceHook::previous );

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasuresG2::onButtonClicked( QWidget *w )
{
    if ( rezero_ == w )
        hook_->emitZeroScale();
    else if ( auditlog_ == w )
        hook_->setCurrentState( selfServiceHook::auditLogDialogGen2 );
    else if ( quit_ == w )
        hook_->leave();                                           // exit self serve
    else if ( exit_ == w )
        hook_->setCurrentState( selfServiceHook::attractGen2 );      // close window
    else if( backspace_ == w )
    {
        // Do I need to do a 50% opacity on this button?  They didn't mention this fact and didn't provide one
        QString value( opID_->text() );

        if( value.length() > 1 )
            value.chop(1);
        else if( value.length() == 1 )
            value = "";

        opID_->setText( value );
    }
    else if ( enter_ == w )
    {
        // Do I need to do a 50% opacity on this button?  They didn't mention this fact and didn't provide one

        hook_->emitChangeFieldValue( sessionHooks::selfServicePassword, opID_->text() );
        opID_->clear();

        opID_->repaint();
    }
    else
    {
        QPushButton *button( dynamic_cast<QPushButton*>( w ) );

        // append button text to line edit
        if ( button )
        {
            // in case there is a key already pressed and it hasn't timed out yet
            if( activeKey_ )
            {
                activeKey_->setStyleSheet(QLatin1String("background-color: rgb(239,242,246);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );

                if( cycleNumpadTimer_->isActive() )
                    cycleNumpadTimer_->stop();
            }

            // trigger the pressed button
            button->setStyleSheet(QLatin1String("background-color: rgb(247,248,250);\n" "color: rgb(128,128,128);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );
            activeKey_ = button;

            cycleNumpadTimer_->start(500);
            cycleNumpadTimer_->setSingleShot(true);

            QString id( opID_->text() );
            QString value( "" );
            if( ( "0" == id ) || ( "" == id ) )
            {
                value = button->text();
            }
            else
            {
                value = id.append( button->text() );
                if( value.length() > 6 )
                    value.remove(0, 1);
            }

            value.remove( QRegExp("^[0]*") );

            opID_->setText( value );

        }       // if a button
    }           // else
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasuresG2::onKeypadTimeout()
{
    if( activeKey_ )
    {
        activeKey_->setStyleSheet(QLatin1String("background-color: rgb(239,242,246);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );
        activeKey_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasuresG2::onReloadImages()
{
    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPixmap selfserveWeightsMeasuresG2::exitButtonPixmap()
{
    QPixmap pixmap( 32, 32 );
    pixmap.fill( Qt::transparent );

    QPainter p( &pixmap );
    p.setRenderHint( QPainter::Antialiasing, true );

    p.setBrush( hobart::colorPalette::red );
    p.setPen( hobart::colorPalette::red );

    p.drawEllipse( QPointF( 15.5, 15.5 ), 15, 15 );

    return pixmap;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveWeightsMeasuresG2::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasuresG2::initialize()
{
    QWidgetList buttons;

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    rezero_     = ui->butRezero;
    auditlog_   = ui->butAuditLog;
    quit_       = ui->butQuit;
    exit_       = ui->butExit;
    enter_      = ui->butEnter;
    backspace_  = ui->butBackspace;

    opID_     = ui->txtOpID;

    auditlog_->setVisible( cat3_ );

#ifdef QT_DEBUG
auditlog_->setVisible( true );
#endif

    weightsMeasures_ = ui->wgtWeightsMeasures;

    connect( rezero_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( rezero_, rezero_ );

    connect( auditlog_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( auditlog_, auditlog_ );

    connect( quit_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( quit_, quit_ );

    connect( exit_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( exit_, exit_ );

    connect( enter_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( enter_, enter_ );

    connect( enter_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( enter_, enter_ );

    connect( backspace_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( backspace_, backspace_ );

    connect( ui->butZero, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butZero, ui->butZero );
    connect( ui->butOne, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butOne, ui->butOne );
    connect( ui->butTwo, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butTwo, ui->butTwo );
    connect( ui->butThree, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butThree, ui->butThree );
    connect( ui->butFour, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butFour, ui->butFour );
    connect( ui->butFive, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butFive, ui->butFive );
    connect( ui->butSix, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butSix, ui->butSix );
    connect( ui->butSeven, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butSeven, ui->butSeven );
    connect( ui->butEight, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butEight, ui->butEight );
    connect( ui->butNine, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( ui->butNine, ui->butNine );

    screen_             = ui->screenContainer;

    title_ =    ui->lblTitle;

    exitLabel_ = ui->lblExitLabel;
    exitLabel_->installEventFilter( this );

    // weights and measures info
    weightsMeasures_ = new QWidget( this );

    //indicator_SoftwareVersionLabel_ = ui->lblIndicatorSoftwareVersionLabel;
    //indicator_SoftwareVersion_      = ui->lblIndicatorSoftwareVersion;

    indicator_ApprovalLabel_        = ui->lblIndicatorApprovalLabel;
    indicator_Approval_             = ui->lblIndicatorApproval;

    indicator_nMaxLabel_            = ui->lblIndicator_nMaxLabel;
    indicator_nMax_                 = ui->lblIndicator_nMax;

    //spacer1_Label_                  = ui->lblSpacer1Label;
    //spacer1_                        = ui->lblSpacer1;

    weigherModelLabel_              = ui->lblWeigherModelLabel;
    weigherModel_                   = ui->lblWeigherModel;

    softwareVersionLabel_           = ui->lblSoftwareVersionLabel;
    softwareVersion_                = ui->lblSoftwareVersion;

    hardwareVersionLabel_           = ui->lblHardwareVersionLabel;
    hardwareVersion_                = ui->lblHardwareVersion;

    capacityLabel_                  = ui->lblCapacityLabel;
    capacity_                       = ui->lblCapacity;

    eMinLabel_                      = ui->lbleMinLabel;
    eMin_                           = ui->lbleMin;

    classLabel_                     = ui->lblClassLabel;
    class_                          = ui->lblClass;

    nMaxLabel_                      = ui->lblnMaxLabel;
    nMax_                           = ui->lblnMax;

    approvalUSLabel_                = ui->lblApprovalUSLabel;
    approvalUS_                     = ui->lblApprovalUS;

    approvalCanLabel_               = ui->lblApprovalCanLabel;
    approvalCan_                    = ui->lblApprovalCan;

    configParamsLabel_              = ui->lblConfigParamsLabel;
    configParams_                   = ui->lblConfigParams;

    calibrationLabel_               = ui->lblCalibrationLabel;
    calibration_                    = ui->lblCalibration;

    tempRangeLabel_                 = ui->lblTempRangeLabel;
    tempRange_                      = ui->lblTempRange;

    opID_->setValidator( new QIntValidator( 0, 99999999, this ) );
    connect( opID_, SIGNAL( textChanged(QString) ), hook_, SLOT( resetIdleTimer() ) );

    if ( cat3_ )
    {
        configParamsLabel_->setVisible( false );
        configParams_->setVisible( false );

        calibrationLabel_->setVisible( false );
        calibration_->setVisible( false );
    }

    cycleNumpadTimer_ = new QTimer(this);
    cycleNumpadTimer_->setInterval(500);         // half sec timeout

    connect( cycleNumpadTimer_, SIGNAL( timeout() ), this, SLOT( onKeypadTimeout() ) );

    opID_ = ui->txtOpID;
    opID_->setValidator(numValidator_ = new QIntValidator(0, INT_MAX, this));
    opID_->setDecimalPlaces(0);
    opID_->setInputWrapping(true);
    numValidator_->setTop(makeValidatorTop(6));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasuresG2::createLayout()
{
    // SOF-894 Hide Canada Approval for SASCALE-1
    if( wayer_model == SessionMainWindow::SASCALE1 )
    {
        if( countryCode == "CA" )
        {
            approvalCanLabel_->hide();
            approvalCan_->hide();
        }
    }
    // Do not have Canadian approval code yet, hide for now.
    if(wayer_model == SessionMainWindow::HTM)
    {
        if ( hook_->session()->scaleConfig->scaleConfig.scaleName().startsWith("FS"))
        {
            approvalCanLabel_->hide();
            approvalCan_->hide();
        }
    }


    if ( cat3_ )
    {
        configParamsLabel_->setVisible( false );
        configParams_->setVisible( false );
        calibrationLabel_->setVisible( false );
        calibration_->setVisible( false );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
