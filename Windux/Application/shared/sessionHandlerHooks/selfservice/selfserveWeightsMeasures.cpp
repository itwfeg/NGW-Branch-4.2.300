/**
 * @file selfserveWeightsMeasures.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfserveWeightsMeasures.h"
#include "selfservicewmnumberpad.h"

#include <backgroundwidgettypes.h>
#include <pushbutton.h>
#include <selfservicehook.h>

#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPainter>
#include <QSignalMapper>
#include <QVBoxLayout>

#include <Session.h>
#include <UiUtilities.h>

#include "../../sessionHandler/sessionmainwindow.h"
#include <ScaleConfiguration.h>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveWeightsMeasures::selfserveWeightsMeasures( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    hook_( h ),
    unlocked_( false ),
    cat3_( false )
{
    setObjectName( "selfserveWeightsMeasures" );

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
selfserveWeightsMeasures::~selfserveWeightsMeasures()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasures::refresh()
{
    // update fields
    QStringList fields( hook_->fieldValue( sessionHooks::weightsAndMeasuresInfo ).split( QChar( 0x1e ) ) );

    if ( fields.size() >= 12 )
    {
        indicator_SoftwareVersion_->setText( fields.at( 0 ) );
        indicator_SoftwareVersion_->repaint();
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

    // update numberpad style
    numberPad_->setStyle( hook_->fieldValue( sessionHooks::numberPadStyle ) );

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
void selfserveWeightsMeasures::translate()
{
    rezero_->setText( tr( "Rezero" ) );
    auditlog_->setText( tr("Audit\nLog") );
    quit_->setText( tr( "Exit Self-\nService Mode" ) );

    title_->setText( tr( "Enter ID to exit self-service mode,\nthen press Enter." ) );

    exitLabel_->setText( tr( "Close" ) );
    exit_->setText( "X" );

    //indicator_SoftwareVersionLabel_->setText( tr( "Indicator Software Version" ) );
    indicator_SoftwareVersionLabel_->setText( tr( " " ) );
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

    if ( !cat3_ )
    {
        configParamsLabel_->setText( tr( "Config Parameters" ) );
        calibrationLabel_->setText( tr( "Calibration" ) );
    }

    tempRangeLabel_->setText( tr( "Temperature Range" ) );

    numberPad_->translate();
    enter_->setText( tr( "Enter" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasures::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // generate pixmap for weights and measures area
    QPixmap pixmap( weightsMeasures_->size() );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setBrush( QColor( 255, 255, 255, 127 ) );
    painter.setPen( QPen( Qt::black, 2 ) );

    // create button
    painter.drawRoundedRect( weightsMeasures_->rect().adjusted( 1, 1, -1, -1 ), 30, 30 );

    // set palette
    QPalette p( weightsMeasures_->palette() );
    p.setBrush( backgroundRole(), pixmap );
    weightsMeasures_->setPalette( p );

    weightsMeasures_->setAutoFillBackground( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasures::showEvent( QShowEvent *e )
{
    numberPad_->blockSignals( true );
    numberPad_->clear();
    numberPad_->blockSignals( false );

    // force refresh
    unlocked_ = false;
    refresh();

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfserveWeightsMeasures::eventFilter( QObject *obj, QEvent *e )
{
    // exit self service mode when exit pressed
    if ( exitLabel_ == obj )
        if ( QEvent::MouseButtonRelease == e->type() )
            hook_->setCurrentState( selfServiceHook::previous );

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasures::onButtonClicked( QWidget *w )
{
    if ( rezero_ == w )
        hook_->emitZeroScale();
    else if ( auditlog_ == w )
        hook_->setCurrentState( selfServiceHook::auditLogDialog );
    else if ( quit_ == w )
        hook_->leave();                                           // exit self serve
    else if ( exit_ == w )
        hook_->setCurrentState( selfServiceHook::previous );      // close window
    else if ( enter_ == w )
    {
        hook_->emitChangeFieldValue( sessionHooks::selfServicePassword, numberPad_->text() );
        numberPad_->clear();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasures::onReloadImages()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasures::initialize()
{
    QWidgetList buttons;

    const hobart::backgroundWidgetRenderer renderDarkButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderGreenButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::greenButtonBackgroundWidgetRendererAppearance() );

    const QPixmap darkButtonPixmap( renderDarkButton.drawControl( QSize( 150, 64 ), hobart::ReadOnly ) );
    const QPixmap greenButtonPixmap( renderGreenButton.drawControl( QSize( 150, 64 ), hobart::ReadOnly ) );

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    // rezero
    rezero_ = new hobart::pushButton( this, darkButtonPixmap );
    connect( rezero_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( rezero_, rezero_ );

    buttons.append( rezero_ );

    // audit log
    auditlog_ = new hobart::pushButton( this, darkButtonPixmap );
    auditlog_->setVisible( cat3_ );
    connect( auditlog_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( auditlog_, auditlog_ );
    buttons.append( auditlog_ );

    // exit self-service mode
    quit_ = new hobart::pushButton( this, darkButtonPixmap );
    connect( quit_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( quit_, quit_ );

    buttons.append( quit_ );

    // title
    title_ = new QLabel( this );
    title_->setAlignment( Qt::AlignCenter );

    QFont titleFont( title_->font() );
    titleFont.setPointSize( 18 );
    title_->setFont( titleFont );

    // close
    exitLabel_ = new QLabel( this );
    exitLabel_->setAlignment( Qt::AlignCenter );

    QPalette exitLabelPalette( exitLabel_->palette() );
    exitLabelPalette.setColor( exitLabel_->foregroundRole(), hobart::colorPalette::white );
    exitLabel_->setPalette( exitLabelPalette );

    QFont exitLabelFont( exitLabel_->font() );
    exitLabelFont.setPointSize( 12 );
    exitLabel_->setFont( exitLabelFont );

    exitLabel_->installEventFilter( this );

    // close button
    exit_ = new hobart::pushButton( this, exitButtonPixmap() );
    connect( exit_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( exit_, exit_ );

    buttons.append( exit_ );

    // weights and measures info
    weightsMeasures_ = new QWidget( this );

    // number pad
    numberPad_ = new selfServiceWmNumberPad( this );
    numberPad_->setValidator( new QIntValidator( 0, 99999999, this ) );

    connect( numberPad_, SIGNAL( textChanged(QString) ), hook_, SLOT( resetIdleTimer() ) );

    // enter button
    enter_ = new hobart::pushButton( this, greenButtonPixmap );
    connect( enter_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( enter_, enter_ );

    buttons.append( enter_ );

    // ----- //

    QFont weightsMeasuresFont( weightsMeasures_->font() );
    weightsMeasuresFont.setPointSize( 12 );
    weightsMeasures_->setFont( weightsMeasuresFont );

    indicator_SoftwareVersionLabel_ = new QLabel( weightsMeasures_ );
    indicator_SoftwareVersion_ = new QLabel( weightsMeasures_ );

    indicator_ApprovalLabel_ = new QLabel( weightsMeasures_ );
    indicator_Approval_ = new QLabel( weightsMeasures_ );

    indicator_nMaxLabel_ = new QLabel( weightsMeasures_ );
    indicator_nMax_ = new QLabel( weightsMeasures_ );

    spacer1_Label_ = new QLabel( weightsMeasures_ );
    spacer1_ = new QLabel( weightsMeasures_ );

    weigherModelLabel_ = new QLabel( weightsMeasures_ );
    weigherModel_ = new QLabel( weightsMeasures_ );

    softwareVersionLabel_ = new QLabel( weightsMeasures_ );
    softwareVersion_ = new QLabel( weightsMeasures_ );

    hardwareVersionLabel_ = new QLabel( weightsMeasures_ );
    hardwareVersion_ = new QLabel( weightsMeasures_ );

    capacityLabel_ = new QLabel( weightsMeasures_ );
    capacity_ = new QLabel( weightsMeasures_ );

    eMinLabel_ = new QLabel( weightsMeasures_ );
    eMin_ = new QLabel( weightsMeasures_ );

    classLabel_ = new QLabel( weightsMeasures_ );
    class_ = new QLabel( weightsMeasures_ );

    nMaxLabel_ = new QLabel( weightsMeasures_ );
    nMax_ = new QLabel( weightsMeasures_ );

    approvalUSLabel_ = new QLabel( weightsMeasures_ );
    approvalUS_ = new QLabel( weightsMeasures_ );

    approvalCanLabel_ = new QLabel( weightsMeasures_ );
    approvalCan_ = new QLabel( weightsMeasures_ );

    spacer2_Label_ = new QLabel( weightsMeasures_ );
    spacer2_ = new QLabel( weightsMeasures_ );

    if ( !cat3_ )
    {
        configParamsLabel_ = new QLabel( weightsMeasures_ );
        configParams_ = new QLabel( weightsMeasures_ );

        calibrationLabel_ = new QLabel( weightsMeasures_ );
        calibration_ = new QLabel( weightsMeasures_ );
    }

    tempRangeLabel_ = new QLabel( weightsMeasures_ );
    tempRange_ = new QLabel( weightsMeasures_ );

    // ----- //

    foreach ( QWidget *w, buttons )
    {
        QPalette palette( w->palette() );
        palette.setColor( QPalette::ButtonText, hobart::colorPalette::white );
        w->setPalette( palette );

        QFont font( w->font() );
        font.setPointSize( 16 );
        w->setFont( font );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveWeightsMeasures::createLayout()
{
    QHBoxLayout *closeButtonLayout( new QHBoxLayout() );
    closeButtonLayout->setContentsMargins( QMargins() );

    closeButtonLayout->addWidget( exitLabel_, 1 );
    closeButtonLayout->addWidget( exit_ );

    QVBoxLayout *closeButtonAreaLayout( new QVBoxLayout() );
    closeButtonAreaLayout->setContentsMargins( QMargins() );

    closeButtonAreaLayout->addLayout( closeButtonLayout );
    closeButtonAreaLayout->addStretch();

    QHBoxLayout *titleLayout( new QHBoxLayout() );
    titleLayout->setContentsMargins( QMargins() );

    titleLayout->addWidget( rezero_ );
    titleLayout->addWidget( auditlog_ );
    titleLayout->addWidget( quit_ );
    titleLayout->addStretch( 1 );
    titleLayout->addWidget( title_ );
    titleLayout->addStretch( 1 );
    titleLayout->addLayout( closeButtonAreaLayout );

    // ----- //

    QHBoxLayout *enterButtonLayout( new QHBoxLayout() );
    enterButtonLayout->setContentsMargins( QMargins() );

    enterButtonLayout->addStretch();
    enterButtonLayout->addWidget( enter_ );
    enterButtonLayout->addStretch();

    QVBoxLayout *numberPadLayout( new QVBoxLayout() );
    numberPadLayout->setContentsMargins( QMargins() );

    numberPadLayout->addStretch();
    numberPadLayout->addWidget( numberPad_ );
    numberPadLayout->addLayout( enterButtonLayout );
    numberPadLayout->addStretch();

    // ----- //

    QHBoxLayout *weightsMeasuresLayout( new QHBoxLayout() );
    weightsMeasuresLayout->setContentsMargins( QMargins( 6, 0, 0, 6 ) );
    weightsMeasuresLayout->setSpacing( 6 );

    weightsMeasuresLayout->addWidget( weightsMeasures_, 1 );
    weightsMeasuresLayout->addLayout( numberPadLayout );

    // ----- //

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins() );

    screenLayout->addLayout( titleLayout );
    screenLayout->addLayout( weightsMeasuresLayout, 1 );

    // ----- //

    QFormLayout *weightsMeasuresFormLayout( new QFormLayout() );

    weightsMeasuresFormLayout->addRow( indicator_SoftwareVersionLabel_, indicator_SoftwareVersion_ );
    weightsMeasuresFormLayout->addRow( indicator_ApprovalLabel_, indicator_Approval_ );
    weightsMeasuresFormLayout->addRow( indicator_nMaxLabel_, indicator_nMax_ );
    weightsMeasuresFormLayout->addRow( spacer1_Label_, spacer1_ );

    weightsMeasuresFormLayout->addRow( weigherModelLabel_, weigherModel_ );
    weightsMeasuresFormLayout->addRow( softwareVersionLabel_, softwareVersion_ );
    weightsMeasuresFormLayout->addRow( hardwareVersionLabel_, hardwareVersion_ );
    weightsMeasuresFormLayout->addRow( capacityLabel_, capacity_ );
    weightsMeasuresFormLayout->addRow( eMinLabel_, eMin_ );
    weightsMeasuresFormLayout->addRow( classLabel_, class_ );
    weightsMeasuresFormLayout->addRow( nMaxLabel_, nMax_ );
    weightsMeasuresFormLayout->addRow( approvalUSLabel_, approvalUS_ );
    weightsMeasuresFormLayout->addRow( approvalCanLabel_, approvalCan_ );
    weightsMeasuresFormLayout->addRow( spacer2_Label_, spacer2_ );

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


    if ( !cat3_ )
    {
        weightsMeasuresFormLayout->addRow( configParamsLabel_, configParams_ );
        weightsMeasuresFormLayout->addRow( calibrationLabel_, calibration_ );
    }

    weightsMeasuresFormLayout->addRow( tempRangeLabel_, tempRange_ );

    QVBoxLayout *weightsMeasuresFormLayout_V( new QVBoxLayout() );
    weightsMeasuresFormLayout_V->addStretch();
    weightsMeasuresFormLayout_V->addLayout( weightsMeasuresFormLayout );
    weightsMeasuresFormLayout_V->addStretch();

    QHBoxLayout *weightsMeasuresFormLayout_H( new QHBoxLayout( weightsMeasures_ ) );
    weightsMeasuresFormLayout_H->addStretch();
    weightsMeasuresFormLayout_H->addLayout( weightsMeasuresFormLayout_V );
    weightsMeasuresFormLayout_H->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPixmap selfserveWeightsMeasures::exitButtonPixmap()
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
} // hobart namespace
