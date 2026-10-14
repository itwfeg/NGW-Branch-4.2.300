/**
 * @file wrappersettings.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "plumainscreen.h"
#include "wrappersettings.h"

#include "../sessionmainwindow.h"

#include "../modes/operatemode.h"

#include "../widgets/labelednumericlineedit.h"
#include "../widgets/labeledpluedit.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <numericlineedit.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>

#include <UiUtilities.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperSettingsScreen::WrapperSettingsScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    ignoreSignals_( false ),
    registered_( false ),
    prevPeripheralList_( new MadpPeripheralList() )
{

    LOG_CLASS_INFO( "WrapperSettingsScreen", "WrapperSettingsScreen" );

    LOG_DEBUG("TOP of WrapperSettingsScreen()", NULL);

    setObjectName( "WrapperSettingsScreen" );

    peripheralList_ = &session_->scaleConfig->peripheralList;

    internalConfigData_ = &session_->scaleConfig->internalConfig;

    // connect slots
    connect( operateMode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );

    connect( mainWindow_, SIGNAL( numberPadMoved(Qt::DockWidgetArea) ), SLOT( onNumberPadMoved(Qt::DockWidgetArea) ) );

    // initialize
    initialize();
    createLayout( Qt::RightDockWidgetArea );
    translate();

    // disable user interface to begin
    disableInterface();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperSettingsScreen::~WrapperSettingsScreen()
{
    if ( prevPeripheralList_ )
    {
        delete prevPeripheralList_;
        prevPeripheralList_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::showEvent( QShowEvent *e )
{
    LOG_CLASS_INFO( "WrapperSettingsScreen", "showEvent" );

    LOG_DEBUG("TOP of showEvent()", NULL);

    _Mybase::showEvent( e );

    LOG_DEBUG("showEvent(): type()=%d", e->type() );

    if( session_->uiUtilities->isWrapOnly() )
    {
        // SOF-835: If Wrap-Only, need to restart timer and start getting the settings again ...
        //          When pressing Home from Service on Wrap-only, the user is returned directly
        //          to Wrapper Settings, and this screen is not re-initialized. Need to call
        //          requestSettings() again to populate the screen.

        MadpPeripheralList *myPeripheralList_;
        myPeripheralList_ = &session_->scaleConfig->peripheralList;

        LOG_DEBUG("showEvent(): myPeripheralList_->wrapper()=%d", myPeripheralList_->wrapper() );

        // Make sure the peripheral list has been received and is a valid wrapper ...
        if( myPeripheralList_->wrapper() )
            requestSettings();
    }
}

void WrapperSettingsScreen::hideEvent( QHideEvent *e)
{
    // if timer is still running, stop that thing.  It was probably supposed to lose focus and stop, but it's still running
    if( requestSettings_->isActive() )
        requestSettings_->stop();

    _Mybase::hideEvent( e );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::disableInterface()
{
    // disable widget
    foreach ( const labeledWidget& p, allWidgets_ )
    {
        QWidget *w( p.second );

        // nothing to do for labels
        if ( !dynamic_cast<QLabel*>( w ) )
            w->setEnabled( false );
    }

    // update frame backgrounds
    film_->setBackground( largeFrames_[ hobart::ReadOnly ] );
    film_->setDropShadowVisible( false );

    belt_->setBackground( largeFrames_[ hobart::ReadOnly ] );
    belt_->setDropShadowVisible( false );

    intake_->setBackground( smallFrames_[ hobart::ReadOnly ] );
    intake_->setDropShadowVisible( false );

    tension_->setBackground( largeFrames_[ hobart::ReadOnly ] );
    tension_->setDropShadowVisible( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::translate()
{
    static const bool convertUpperCase( true );

    QString packLengthText( tr( "Pack Length" ) );
    QString packWidthText( tr( "Pack Width" ) );
    QString packHeightText( tr( "Pack Height" ) );
    QString lipHeightText( tr( "Lip Height" ) );

    QString filmSelectionText( tr( "Film Selection" ) );
    QString upperFilmWidthText( tr( "Upper Film Width" ) );
    QString lowerFilmWidthText( tr( "Lower Film Width" ) );

    QString roomTempText( tr( "Room Temperature" ) );

    QString sealerBeltPowerText( tr( "Sealer Belt Power" ) );
    QString beltTempSettingText( tr( "Belt Temp. Setting" ) );
    QString currentBeltTempText( tr( "Current Belt Temp" ) );

    QString trayNameText( tr( "Tray Name" ) );
    QString filmUsedText( tr( "Film Used" ) );
    QString knifeTimeText( tr( "Knife Time" ) );

    QString intakeModeText( tr( "Intake Mode" ) );

    QString filmTensionText( tr( "Film Tension" ) );
    QString tensionFrontToBackText( tr( "Tension Front to Back" ) );
    QString tensionLeftToRightText( tr( "Tension Left to Right" ) );

    if ( convertUpperCase )
    {
        QList<QString*> strings;

        strings.append( &packLengthText );
        strings.append( &packWidthText );
        strings.append( &packHeightText );
        strings.append( &lipHeightText );

        strings.append( &filmSelectionText );
        strings.append( &upperFilmWidthText );
        strings.append( &lowerFilmWidthText );

        strings.append( &roomTempText );

        strings.append( &sealerBeltPowerText );
        strings.append( &beltTempSettingText );
        strings.append( &currentBeltTempText );

        strings.append( &trayNameText );
        strings.append( &filmUsedText );
        strings.append( &knifeTimeText );

        strings.append( &intakeModeText );

        strings.append( &filmTensionText );
        strings.append( &tensionFrontToBackText );
        strings.append( &tensionLeftToRightText );

        foreach ( QString *s, strings )
            (*s) = s->toUpper();
    }

    packLength_->setLabelText( packLengthText );
    packWidth_->setLabelText( packWidthText );
    packHeight_->setLabelText( packHeightText );
    lipHeight_->setLabelText( lipHeightText );

    filmSelectionLabel_->setText( filmSelectionText );
    filmSelection_->setItemText( 0, tr( "Upper" ) );
    filmSelection_->setItemText( 1, tr( "Lower" ) );
    filmSelection_->setItemText( 2, tr( "Automatic" ) );

    upperFilmWidthLabel_->setText( upperFilmWidthText );

    lowerFilmWidthLabel_->setText( lowerFilmWidthText );

    cutFilm_->setText( tr( "Cut" ) );
    loadUpperRoll_->setText( tr( "Load" ) );
    loadLowerRoll_->setText( tr( "Load" ) );

    roomTempLabel_->setText( roomTempText );

    clean_->setText( tr( "Clean Wrapper" ) );

    sealerBeltPowerLabel_->setText( sealerBeltPowerText );
    sealerBeltPower_->setItemText( 0, tr( "Off" ) );
    sealerBeltPower_->setItemText( 1, tr( "On" ) );

    beltTempSettingLabel_->setText( beltTempSettingText );

    currentBeltTempLabel_->setText( currentBeltTempText );

    trayNameLabel_->setText( trayNameText );

    filmUsedLabel_->setText( filmUsedText );

    knifeTimeLabel_->setText( knifeTimeText );

    intakeModeLabel_->setText( intakeModeText );
    intakeMode_->setItemText( 0, tr( "Gentle" ) );
    intakeMode_->setItemText( 1, tr( "Normal" ) );

    filmTensionLabel_->setText( filmTensionText );
    filmTension_->setItemText( 0, tr( "Low" ) );
    filmTension_->setItemText( 1, tr( "Medium" ) );

    tensionFrontToBackLabel_->setText( tensionFrontToBackText );
    tensionFrontToBack_->setItemText( 5, tr( "None" ) );

    tensionLeftToRightLabel_->setText( tensionLeftToRightText );
    tensionLeftToRight_->setItemText( 5, tr( "None" ) );

    update();

    _Mybase::translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperSettingsScreen::isWrapperSupported() const
{
    LOG_CLASS_INFO( "WrapperSettingsScreen", "isWrapperSupported" );

    if ( !peripheralList_->wrapper() )
        return false;
    else if ( MadpPeripheralList::wtEvolWrapper != peripheralList_->wrapper() )
    {
        LOG_WARN( "unsupported wrapper type %d", peripheralList_->wrapper() );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *WrapperSettingsScreen::swipeScreenRight() const
{
    // check what screen is the right screen...
    // 0 = PluInfoScreen
    // 1 = WrapperSettingsScreen
    // 2 = LabelPlacementScreen
    int rightScreen( session_->scaleConfig->scaleConfig.rightScreenType() );

    if ( ( ( session_->isPrepack() ) && ( 1 != rightScreen ) ) ||
         ( session_->uiUtilities->isWrapOnly() ) )
        return NULL;

    ScreenWidget *screen = mode_->createScreen<PluMainScreen>();

    return screen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString WrapperSettingsScreen::temperatureScale() const
{
    static const bool isCelsius( false );

    return (isCelsius ? QString::fromUtf8( "°C" ) : QString::fromUtf8( "°F" ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "WrapperSettingsScreen", "onSessionMessage" );

    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    // register session
    if (( SessionDev == device ) && ( SnRegister == event ))
    {
        if ( !registered_ )
        {
            LOG_INFO( "session has been registered!", NULL );
            registered_ = true;

            // request settings
            requestSettings();
        }
    }

    // peripheral list
    else if (( System == device ) && ( SysPeripheralList == event ))
    {
        // this hides all fields until we get some proper settings
        if ( !peripheralList_->wrapper() )
        {
            updateConfig( NULL );

            // wrapper dropped offline... disable the interface
            if ( prevPeripheralList_->wrapper() )
                disableInterface();
        }

        // Note:
        // For some reason or another backend gets into a loop where it keeps sending us peripheral
        // list data... so lets compare the new one versus the previous one and only take action if
        // different.
        else if ( prevPeripheralList_->wrapper() != peripheralList_->wrapper() )
        {
            // request settings
            requestSettings();
        }

        (*prevPeripheralList_) = (*peripheralList_);
    }

    // wrapper settings
    else if (( Wrapper == device ) && ( WrEvolSettings == event ))
    {
        DomEvolWrapperSettings m( msg );

        updateSettings( &m );
        settings_ |= 0x01;
    }

    // wrapper status
    else if (( Wrapper == device ) && ( WrEvolStatus == event ))
    {
        DomEvolWrapperStatus m( msg );

        updateStatus( &m );
        settings_ |= 0x02;
    }

    // wrapper system info
    else if (( Wrapper == device ) && ( WrEvolSystemInfo == event ))
    {
        DomEvolWrapperSystemInformation m( msg );

        updateConfig( &m );
        settings_ |= 0x04;
    }

    // wrapper knife time
    else if (( Wrapper == device ) && ( WrEvolEventCapture == event ))
    {
        DomEvolWrapperEventCapture m( msg );

        if ( DomEvolWrapperEventCapture::KnifeUpEvent == m.captureType() )
            updateKnifeTime( &m );
    }

    // wrapper package description
    else if (( Wrapper == device ) && ( WrEvolPackageDesc == event ))
    {
        DomEvolWrapperPkgDesc m( msg );

        updatePackageDesc( &m );
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::onNumberPadMoved( Qt::DockWidgetArea where )
{
    createLayout( where );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::onWidgetUpdated( QWidget *widget )
{
    LOG_CLASS_INFO( "WrapperSettingsScreen", "onWidgetUpdated" );

    // nothing to do
    if ( ignoreSignals_ )
        return;
    else if ( !isWrapperSupported() )
        return;

    QComboBox *comboBox;
    hobart::numericLineEdit *numericLineEdit;
    QPushButton *pushButton;

    //
    // combo boxes
    //

    if ( (comboBox = dynamic_cast<QComboBox*>( widget )) )
    {
        if ( widgetCommands_.contains( widget ) )
        {
            int command( widgetCommands_[widget] );
            int itemData( comboBox->itemData( comboBox->currentIndex() ).toInt() );

            LOG_DEBUG( "sending control message %d value %d", command, itemData );
            session_->send( Wrapper, WrEvolControl, QString::number( command ) + "," + QString::number( itemData ) );

            // reset film selection background color
            // SOF-240: disable yellow highlight
            //if ( filmSelection_ == widget )
            //    updateNetStretch( false );
        }
    }

    //
    // numeric line edits
    //

    else if ( (numericLineEdit = dynamic_cast<hobart::numericLineEdit*>( widget )) )
    {
        if ( widgetCommands_.contains( widget ) )
        {
            int command( widgetCommands_[widget] );
            int itemData( numericLineEdit->toInt() );

            LOG_DEBUG( "sending control message %d value %d", command, itemData );
            session_->send( Wrapper, WrEvolControl, QString::number( command ) + "," + QString::number( itemData ) );
        }
    }

    //
    // push buttons
    //

    else if ( (pushButton = dynamic_cast<QPushButton*>( widget )) )
    {
        if ( cutFilm_ == widget )
            session_->send( Wrapper, WrEvolTest, "5" );
        else if ( widgetCommands_.contains( widget ) )
        {
            int command( widgetCommands_[widget] );

            LOG_DEBUG( "sending load message %d", command );
            session_->send( Wrapper, WrEvolLoad, QString::number( command ) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    // HA-139: they want the weight bar to be shown now
/*
    // make sure weight bar is shown
    if ( operateMode_ )
        operateMode_->showWeightBar( WeightBarWidget::NetWeightOnlyMode );
*/

    // SOF-2798: Make sure the correct weight bar is visible
    if ( operateMode_ )
        operateMode_->showWeightBar();

    plu_->setTotalTypeVisible( session_->scaleConfig->scaleConfig.showTotalTypeOnRunScreen() ); // HA-176

    // request status
    if ( isWrapperSupported() )
        requestSettings();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::onTimerTimeout()
{
    LOG_CLASS_INFO( "WrapperSettingsScreen", "onTimerTimeout" );

    LOG_DEBUG("TOP of onTimerTimeout()", NULL);

    if ( settingsMask_ == settings_ )
    {
        requestSettings_->stop();

        LOG_DEBUG("requestSettings_->STOP() - #1, settingsMask_=%d, settings_=%d", settingsMask_, settings_ );
    }
    else if ( !isWrapperSupported() )
    {
        requestSettings_->stop();

        LOG_DEBUG("NOT SUPPORTED: requestSettings_->STOP() - #2", NULL);
    }
    else
    {
        requestSettings();

        LOG_DEBUG("requestSettings() called", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::onStartCleanMode()
{
    // send message to start clean mode and lift the elevator and slats
    // standard style - session_->send( Wrapper, WrReqEvolSystemInfo );

    session_->send( Wrapper, WrEvolTest, QString::number( WrWrapperCleanTest ) );
    clean_->setAutoFillBackground( true );

    cycleCleanTimer_->start(500);
    cycleCleanTimer_->setSingleShot(true);
    clean_->setStyleSheet( QLatin1String("background-color: rgb(0,0,0);\n" "color: rgb(255,255,255);\n" "border-width: 2px;\n"  "border-style: solid;\n" "border-radius: 5px;\n" "border-width: 2px;") );


    //QPalette p( clean_->palette() );
    //p.setBrush( clean_->foregroundRole(), hobart::colorPalette::black );                           // arrow
    //p.setBrush( QPalette::ButtonText, hobart::colorPalette::white );                                // scroll bar arrows
    //p.setBrush( QPalette::Text, hobart::colorPalette::black );                                      // item text
    //p.setBrush( QPalette::Disabled, clean_->foregroundRole(), hobart::colorPalette::lightGray );   // arrow
    //p.setBrush( QPalette::Disabled, QPalette::ButtonText, hobart::colorPalette::lightGray );        // scroll bar arrows
    //p.setBrush( QPalette::Disabled, QPalette::Text, hobart::colorPalette::lightGray );              // item text
    //clean_->setPalette( p );

    const int widgetFixedHeight( scaleHeightForScreen( 40 ) );
    modifyFontPointSize setFontPointSize14( scaleFontPointSizeForScreen( 14 ) );

    clean_->setFixedHeight( widgetFixedHeight );
    setFontPointSize14( clean_ );

    clean_->repaint();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::requestSettings()
{
    LOG_CLASS_INFO( "WrapperSettingsScreen", "requestSettings" );

    LOG_DEBUG("TOP of requestSettings()", NULL);

    DomEvolWrapperSystemInformation *evolWrapperSystemInformation( &session_->scaleConfig->evolWrapperSystemInformation );

    settingsMask_ = 0x03;
    settings_ = 0x00;

    //-----------------------------------------------------------------------------
    // SOF-4929: Do not request the Wrapper messages while the wrapper node board(s)
    //           are being initialized.  This applies to AWS, EWS and CLA.
    //           These messages interfere with the board upgrade process.
    //-----------------------------------------------------------------------------
    if( !( internalConfigData_->isWrapperModelInitialization ) )
    {
        // unknown wrapper model
        if ( DomEvolWrapperSystemInformation::None == evolWrapperSystemInformation->awsModelNumber() )
        {
            session_->send( Wrapper, WrReqEvolSystemInfo );
            settingsMask_ |= 0x04;
        }
        // request wrapper info
        session_->send(WrapperSystem, WrSysReqStatus);              // 10,13

        session_->send( Wrapper, WrReqSettings );
        session_->send( Wrapper, WrReqStatus );
    }
    requestSettings_->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::initialize()
{
    static const QMargins margins( 10, 10, 10, 6 );

    const int widgetFixedHeight( scaleHeightForScreen( 40 ) );

    modifyFontPointSize setFontPointSize14( scaleFontPointSizeForScreen( 14 ) );
    modifyFontPointSize setFontPointSize32( scaleFontPointSizeForScreen( 32 ) );

    modifyPaletteForeground setButtonForegroundPalette(
        modifyPaletteForeground::createPalette( hobart::colorPalette::black, hobart::colorPalette::lightGray ) );

    widgetUpdatedSignalMapper_ = new QSignalMapper( this );

    connect( widgetUpdatedSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onWidgetUpdated(QWidget*) ) );

    requestSettings_ = new QTimer( this );
    requestSettings_->setSingleShot( false );
    requestSettings_->setInterval( 5000 );

    connect( requestSettings_, SIGNAL( timeout() ), SLOT( onTimerTimeout() ) );

    // renderers
    int pluWidth( plu_->width() );

    if ( session_->uiUtilities->isWrapOnly() )
        pluWidth = 0;

    const QSize packageDimensionsSize( (mainWindow_->width() - pluWidth) / 4, plu_->height() );
    const QSize largeFrameSize( (mainWindow_->width() - 45) / 2, scaleHeightForScreen( 155 ) );
    const QSize smallFrameSize( (mainWindow_->width() - 45) / 2, scaleHeightForScreen( 380 ) -  (largeFrameSize.height() * 2) );

    const hobart::backgroundWidgetRenderer renderPackageDimensions(
        new hobart::rectangleBackgroundWidget,
        new hobart::darkOperatorBackgroundWidgetRendererAppearance ); ;

    const hobart::backgroundWidgetRenderer renderLargeFrame(
        new hobart::rectangleBackgroundWidget,
        new hobart::darkOperatorBackgroundWidgetRendererAppearance );

    const hobart::backgroundWidgetRenderer renderSmallFrame(
        new hobart::rectangleBackgroundWidget,
        new hobart::darkOperatorBackgroundWidgetRendererAppearance );

    const QPixmap packageDimensionsNormal( renderPackageDimensions.drawControl( packageDimensionsSize, hobart::Normal ) );
    const QPixmap packageDimensionsReadOnly( renderPackageDimensions.drawControl( packageDimensionsSize, hobart::ReadOnly ) );
    const QPixmap packageDimensionsDropShadow( renderPackageDimensions.drawDropShadow( packageDimensionsSize ) );

    largeFrames_.insert( hobart::Normal, renderLargeFrame.drawControl( largeFrameSize, hobart::Normal ) );
    largeFrames_.insert( hobart::ReadOnly, renderLargeFrame.drawControl( largeFrameSize, hobart::ReadOnly ) );

    smallFrames_.insert( hobart::Normal, renderSmallFrame.drawControl( smallFrameSize, hobart::Normal ) );
    smallFrames_.insert( hobart::ReadOnly, renderSmallFrame.drawControl( smallFrameSize, hobart::ReadOnly ) );

    //
    // package dimensions
    //

    packLength_ = new hobart::labeledNumericLineEdit( this, packageDimensionsNormal );
    packLength_->setBackgroundReadOnly( packageDimensionsReadOnly );
    packLength_->setDropShadow( packageDimensionsDropShadow );
    packLength_->setContentsMargins( margins );
    packLength_->setLabelObjectName( "packLength_label" );
    packLength_->setObjectName( "packLength" );
    packLength_->setReadOnly( true );

    setFontPointSize32( packLength_ );

    packWidth_ = new hobart::labeledNumericLineEdit( this, packageDimensionsNormal );
    packWidth_->setBackgroundReadOnly( packageDimensionsReadOnly );
    packWidth_->setDropShadow( packageDimensionsDropShadow );
    packWidth_->setContentsMargins( margins );
    packWidth_->setLabelObjectName( "packWidth_label" );
    packWidth_->setObjectName( "packWidth" );
    packWidth_->setReadOnly( true );

    setFontPointSize32( packWidth_ );

    packHeight_ = new hobart::labeledNumericLineEdit( this, packageDimensionsNormal );
    packHeight_->setAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    packHeight_->setBackgroundReadOnly( packageDimensionsReadOnly );
    packHeight_->setDropShadow( packageDimensionsDropShadow );
    packHeight_->setContentsMargins( margins );
    packHeight_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    packHeight_->setLabelObjectName( "packHeight_label" );
    packHeight_->setObjectName( "packHeight" );
    packHeight_->setReadOnly( true );

    setFontPointSize32( packHeight_ );

    lipHeight_ = new hobart::labeledNumericLineEdit( this, packageDimensionsNormal );
    lipHeight_->setAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    lipHeight_->setBackgroundReadOnly( packageDimensionsReadOnly );
    lipHeight_->setDropShadow( packageDimensionsDropShadow );
    lipHeight_->setContentsMargins( margins );
    lipHeight_->setLabelAlignment( Qt::AlignRight | Qt::AlignVCenter  );
    lipHeight_->setLabelObjectName( "lipHeight_label" );
    lipHeight_->setObjectName( "lipHeight" );
    lipHeight_->setReadOnly( true );

    setFontPointSize32( lipHeight_ );

    QString comboBoxStyle("font: 20pt;");

    //
    // film
    //

    film_ = new frameWidget( largeFrames_[hobart::ReadOnly], this );
    film_->setDropShadow( renderLargeFrame.drawDropShadow( largeFrameSize ) );
    film_->setContentsMargins( margins );
    film_->setObjectName( "film" );

    filmSelectionLabel_ = new QLabel( film_ );
    filmSelectionLabel_->setObjectName( "filmSelectionLabel" );

    filmSelection_ = new QComboBox( film_ );
    filmSelection_->setFocusPolicy( Qt::NoFocus );
    filmSelection_->setObjectName( "filmSelection" );
    filmSelection_->setStyleSheet(comboBoxStyle);

    filmSelection_->addItem( "", 0 );
    filmSelection_->addItem( "", 1 );
    filmSelection_->addItem( "", 2 );

    connect( filmSelection_, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( filmSelection_, filmSelection_ );

    upperFilmWidthLabel_ = new QLabel( film_ );
    upperFilmWidthLabel_->setObjectName( "upperFilmWidthLabel" );

    upperFilmWidth_ = new QComboBox( film_ );
    upperFilmWidth_->setFocusPolicy( Qt::NoFocus );
    upperFilmWidth_->setObjectName( "upperFilmWidth" );
    upperFilmWidth_->setStyleSheet(comboBoxStyle);

    upperFilmWidth_->addItem( "13\"", 130 );
    upperFilmWidth_->addItem( "14\"", 140 );
    upperFilmWidth_->addItem( "15\"", 150 );
    upperFilmWidth_->addItem( "16\"", 160 );
    upperFilmWidth_->addItem( "17\"", 170 );
    upperFilmWidth_->addItem( "18\"", 180 );
    upperFilmWidth_->addItem( "19\"", 190 );
    upperFilmWidth_->addItem( "20\"", 200 );
    upperFilmWidth_->setCurrentIndex( upperFilmWidth_->findData( 200 ) );

    connect( upperFilmWidth_, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( upperFilmWidth_, upperFilmWidth_ );

    lowerFilmWidthLabel_ = new QLabel( film_ );
    lowerFilmWidthLabel_->setObjectName( "lowerFilmWidthLabel" );

    lowerFilmWidth_ = new QComboBox( film_ );
    lowerFilmWidth_->setFocusPolicy( Qt::NoFocus );
    lowerFilmWidth_->setObjectName( "lowerFilmWidth" );
    lowerFilmWidth_->setStyleSheet(comboBoxStyle);

    lowerFilmWidth_->addItem( "13\"", 130 );
    lowerFilmWidth_->addItem( "14\"", 140 );
    lowerFilmWidth_->addItem( "15\"", 150 );
    lowerFilmWidth_->addItem( "16\"", 160 );
    lowerFilmWidth_->addItem( "17\"", 170 );
    lowerFilmWidth_->addItem( "18\"", 180 );
    lowerFilmWidth_->addItem( "19\"", 190 );
    lowerFilmWidth_->addItem( "20\"", 200 );
    lowerFilmWidth_->setCurrentIndex( lowerFilmWidth_->findData( 200 ) );

    connect( lowerFilmWidth_, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( lowerFilmWidth_, lowerFilmWidth_ );

    cutFilm_ = new QPushButton( film_ );
    cutFilm_->setFixedHeight( widgetFixedHeight );
    cutFilm_->setFocusPolicy( Qt::NoFocus );
    cutFilm_->setObjectName( "cutFilm" );

    setButtonForegroundPalette( cutFilm_ );
    setFontPointSize14( cutFilm_ );

    connect( cutFilm_, SIGNAL( pressed() ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( cutFilm_, cutFilm_ );

    loadUpperRoll_ = new QPushButton( film_ );
    loadUpperRoll_->setFixedHeight( widgetFixedHeight );
    loadUpperRoll_->setFocusPolicy( Qt::NoFocus );
    loadUpperRoll_->setObjectName( "loadUpperRoll" );

    setButtonForegroundPalette( loadUpperRoll_ );
    setFontPointSize14( loadUpperRoll_ );

    connect( loadUpperRoll_, SIGNAL( pressed() ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( loadUpperRoll_, loadUpperRoll_ );

    loadLowerRoll_ = new QPushButton( film_ );
    loadLowerRoll_->setFixedHeight( widgetFixedHeight );
    loadLowerRoll_->setFocusPolicy( Qt::NoFocus );
    loadLowerRoll_->setObjectName( "loadLowerRoll" );

    setButtonForegroundPalette( loadLowerRoll_ );
    setFontPointSize14( loadLowerRoll_ );

    connect( loadLowerRoll_, SIGNAL( pressed() ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( loadLowerRoll_, loadLowerRoll_ );

    spacer_ = new QLabel( film_ );
    spacer_->setFixedHeight( widgetFixedHeight );
    spacer_->setObjectName( "spacer" );

    //
    // room
    //

    room_ = new frameWidget( smallFrames_[hobart::ReadOnly], this );
    room_->setDropShadow( renderSmallFrame.drawDropShadow( smallFrameSize ) );
    room_->setContentsMargins( margins );
    room_->setObjectName( "room" );

    roomTempLabel_ = new QLabel( room_ );
    roomTempLabel_->setObjectName( "roomTempLabel" );

    roomTemp_ = new QLabel( room_ );
    roomTemp_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    roomTemp_->setObjectName( "roomTemp" );

    clean_ = new QPushButton( this );
    clean_->setObjectName( "clean" );

    connect( clean_, SIGNAL( clicked() ), SLOT( onStartCleanMode() ) );

    cycleCleanTimer_ = new QTimer(this);
    cycleCleanTimer_->setInterval(100);         // half sec timeout
    connect( cycleCleanTimer_, SIGNAL( timeout() ), this, SLOT( onCleanTimeout() ) );

    //
    // belt
    //

    belt_ = new frameWidget( largeFrames_[hobart::ReadOnly], this );
    belt_->setDropShadow( renderLargeFrame.drawDropShadow( largeFrameSize ) );
    belt_->setContentsMargins( margins );
    belt_->setObjectName( "belt" );

    sealerBeltPowerLabel_ = new QLabel( belt_ );
    sealerBeltPowerLabel_->setObjectName( "sealerBeltPowerLabel" );

    sealerBeltPower_ = new QComboBox( belt_ );
    sealerBeltPower_->setFocusPolicy( Qt::NoFocus );
    sealerBeltPower_->setObjectName( "sealerBeltPower" );
    sealerBeltPower_->setStyleSheet(comboBoxStyle);

    sealerBeltPower_->addItem( "", 0 );
    sealerBeltPower_->addItem( "", 1 );

    connect( sealerBeltPower_, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( sealerBeltPower_, sealerBeltPower_ );

    beltTempSettingLabel_ = new QLabel( belt_ );
    beltTempSettingLabel_->setObjectName( "beltTempSettingLabel" );

    beltTempSetting_ = new hobart::numericLineEdit( belt_ );
    beltTempSetting_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    beltTempSetting_->setInputTruncation( true );
    beltTempSetting_->setObjectName( "beltTempSetting" );
    beltTempSetting_->setSuffix( " " + temperatureScale() );
    beltTempSetting_->setValidator( new QIntValidator( 0, 999, belt_ ) );

    connect( beltTempSetting_, SIGNAL( editingFinished() ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( beltTempSetting_, beltTempSetting_ );

    currentBeltTempLabel_ = new QLabel( belt_ );
    currentBeltTempLabel_->setObjectName( "currentBeltTempLabel" );

    currentBeltTemp_ = new QLabel( belt_ );
    currentBeltTemp_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    currentBeltTemp_->setObjectName( "currentBeltTemp" );

    //
    // advanced wrapper settings
    //

    advancedSettings_ = new frameWidget( largeFrames_[hobart::ReadOnly], this );
    advancedSettings_->setDropShadow( renderLargeFrame.drawDropShadow( largeFrameSize ) );
    advancedSettings_->setContentsMargins( margins );
    advancedSettings_->setObjectName( "advancedSettings" );

    trayNameLabel_ = new QLabel( advancedSettings_ );
    trayNameLabel_->setObjectName( "intakeModeLabel" );

    trayName_ = new QLabel( advancedSettings_ );
    trayName_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    trayName_->setObjectName( "trayName" );

    filmUsedLabel_ = new QLabel( advancedSettings_ );
    filmUsedLabel_->setObjectName( "filmUsedLabel" );

    filmUsed_ = new QLabel( advancedSettings_ );
    filmUsed_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    filmUsed_->setObjectName( "filmUsed" );

    knifeTimeLabel_ = new QLabel( advancedSettings_ );
    knifeTimeLabel_->setObjectName( "knifeTimeLabel" );

    knifeTime_ = new QLabel( advancedSettings_ );
    knifeTime_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    knifeTime_->setObjectName( "knifeTime" );

    //
    // intake
    //

    intake_ = new frameWidget( smallFrames_[hobart::ReadOnly], this );
    intake_->setDropShadow( renderSmallFrame.drawDropShadow( smallFrameSize ) );
    intake_->setContentsMargins( margins );
    intake_->setObjectName( "intake" );

    intakeModeLabel_ = new QLabel( intake_ );
    intakeModeLabel_->setObjectName( "intakeModeLabel" );

    intakeMode_ = new QComboBox( intake_ );
    intakeMode_->setFocusPolicy( Qt::NoFocus );
    intakeMode_->setObjectName( "intakeMode" );
    intakeMode_->setStyleSheet(comboBoxStyle);

    intakeMode_->addItem( "", 1 );
    intakeMode_->addItem( "", 2 );

    connect( intakeMode_, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( intakeMode_, intakeMode_ );

    //
    // tension
    //

    tension_ = new frameWidget( largeFrames_[hobart::ReadOnly], this );
    tension_->setDropShadow( renderLargeFrame.drawDropShadow( largeFrameSize ) );
    tension_->setContentsMargins( margins );
    tension_->setObjectName( "tension" );

    filmTensionLabel_ = new QLabel( tension_ );
    filmTensionLabel_->setObjectName( "filmTensionLabel" );

    filmTension_ = new QComboBox( tension_ );
    filmTension_->setFocusPolicy( Qt::NoFocus );
    filmTension_->setObjectName( "filmTension" );
    filmTension_->setStyleSheet(comboBoxStyle);

    filmTension_->addItem( "", 0 );
    filmTension_->addItem( "", 1 );

    connect( filmTension_, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( filmTension_, filmTension_ );

    tensionFrontToBackLabel_ = new QLabel( tension_ );
    tensionFrontToBackLabel_->setObjectName( "tensionFrontToBackLabel" );

    tensionFrontToBack_ = new QComboBox( tension_ );
    tensionFrontToBack_->setFocusPolicy( Qt::NoFocus );
    tensionFrontToBack_->setObjectName( "tensionFrontToBack" );
    tensionFrontToBack_->setStyleSheet(comboBoxStyle);

    tensionFrontToBack_->addItem( "-5", -5 );
    tensionFrontToBack_->addItem( "-4", -4 );
    tensionFrontToBack_->addItem( "-3", -3 );
    tensionFrontToBack_->addItem( "-2", -2 );
    tensionFrontToBack_->addItem( "-1", -1 );
    tensionFrontToBack_->addItem( "", 0 );
    tensionFrontToBack_->addItem( "+1", 1 );
    tensionFrontToBack_->addItem( "+2", 2 );
    tensionFrontToBack_->addItem( "+3", 3 );
    tensionFrontToBack_->addItem( "+4", 4 );
    tensionFrontToBack_->addItem( "+5", 5 );
    tensionFrontToBack_->setCurrentIndex( tensionFrontToBack_->findData( 5 ) );

    connect( tensionFrontToBack_, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( tensionFrontToBack_, tensionFrontToBack_ );

    tensionLeftToRightLabel_ = new QLabel( tension_ );
    tensionLeftToRightLabel_->setObjectName( "tensionLeftToRightLabel" );

    tensionLeftToRight_ = new QComboBox( tension_ );
    tensionLeftToRight_->setFocusPolicy( Qt::NoFocus );
    tensionLeftToRight_->setObjectName( "tensionLeftToRight" );
    tensionLeftToRight_->setStyleSheet(comboBoxStyle);

    tensionLeftToRight_->addItem( "-5", -5 );
    tensionLeftToRight_->addItem( "-4", -4 );
    tensionLeftToRight_->addItem( "-3", -3 );
    tensionLeftToRight_->addItem( "-2", -2 );
    tensionLeftToRight_->addItem( "-1", -1 );
    tensionLeftToRight_->addItem( "", 0 );
    tensionLeftToRight_->addItem( "+1", 1 );
    tensionLeftToRight_->addItem( "+2", 2 );
    tensionLeftToRight_->addItem( "+3", 3 );
    tensionLeftToRight_->addItem( "+4", 4 );
    tensionLeftToRight_->addItem( "+5", 5 );
    tensionLeftToRight_->setCurrentIndex( tensionLeftToRight_->findData( 5 ) );

    connect( tensionLeftToRight_, SIGNAL( currentIndexChanged(int) ), widgetUpdatedSignalMapper_, SLOT( map() ) );
    widgetUpdatedSignalMapper_->setMapping( tensionLeftToRight_, tensionLeftToRight_ );

    // ---- //

    allWidgets_.append( labeledWidget( filmSelectionLabel_, filmSelection_ ) );
    allWidgets_.append( labeledWidget( upperFilmWidthLabel_, upperFilmWidth_ ) );

    if( !( session_->uiUtilities->isWrapOnly() ) )
        allWidgets_.append( labeledWidget( lowerFilmWidthLabel_, lowerFilmWidth_ ) );

    allWidgets_.append( labeledWidget( roomTempLabel_, roomTemp_ ) );

    allWidgets_.append( labeledWidget( sealerBeltPowerLabel_, sealerBeltPower_ ) );
    allWidgets_.append( labeledWidget( beltTempSettingLabel_, beltTempSetting_ ) );
    allWidgets_.append( labeledWidget( currentBeltTempLabel_, currentBeltTemp_ ) );

    allWidgets_.append( labeledWidget( trayNameLabel_, trayName_ ) );
    allWidgets_.append( labeledWidget( filmUsedLabel_, filmUsed_ ) );
    allWidgets_.append( labeledWidget( knifeTimeLabel_, knifeTime_ ) );

    allWidgets_.append( labeledWidget( intakeModeLabel_, intakeMode_ ) );

    allWidgets_.append( labeledWidget( filmTensionLabel_, filmTension_ ) );
    allWidgets_.append( labeledWidget( tensionFrontToBackLabel_, tensionFrontToBack_ ) );
    allWidgets_.append( labeledWidget( tensionLeftToRightLabel_, tensionLeftToRight_ ) );

    // ---- //

    // evolution control messages
    widgetCommands_[filmSelection_] = 5;
    widgetCommands_[upperFilmWidth_] = 6;
    widgetCommands_[lowerFilmWidth_] = 7;
    widgetCommands_[sealerBeltPower_] = 8;
    widgetCommands_[beltTempSetting_] = 9;
    widgetCommands_[intakeMode_] = 1;
    widgetCommands_[filmTension_] = 2;
    widgetCommands_[tensionFrontToBack_] = 3;
    widgetCommands_[tensionLeftToRight_] = 4;

    // evolution load messages
    widgetCommands_[loadUpperRoll_] = 0;
    widgetCommands_[loadLowerRoll_] = 1;

    // ---- //

    // lets groom our widgets
    foreach ( const labeledWidget& w, allWidgets_ )
    {
        QWidget *groomed( w.second );

        // setup fixed height
        w.first->setFixedHeight( widgetFixedHeight );
        w.second->setFixedHeight( widgetFixedHeight );

        // setup font
        setFontPointSize14( groomed );

        // setup palette
        if (( dynamic_cast<QComboBox*>( groomed ) ) ||
            ( dynamic_cast<QLineEdit*>( groomed ) ))
        {
            QPalette p( groomed->palette() );
            p.setBrush( groomed->foregroundRole(), hobart::colorPalette::black );                           // arrow
            p.setBrush( QPalette::ButtonText, hobart::colorPalette::black );                                // scroll bar arrows
            p.setBrush( QPalette::Text, hobart::colorPalette::black );                                      // item text
            p.setBrush( QPalette::Disabled, groomed->foregroundRole(), hobart::colorPalette::lightGray );   // arrow
            p.setBrush( QPalette::Disabled, QPalette::ButtonText, hobart::colorPalette::lightGray );        // scroll bar arrows
            p.setBrush( QPalette::Disabled, QPalette::Text, hobart::colorPalette::lightGray );              // item text
            groomed->setPalette( p );
        }
    }

    clean_->setFixedHeight( widgetFixedHeight );
    setFontPointSize14( clean_ );

    QPalette p( clean_->palette() );
    p.setBrush( clean_->foregroundRole(), hobart::colorPalette::black );                           // arrow
    p.setBrush( QPalette::ButtonText, hobart::colorPalette::black );                                // scroll bar arrows
    p.setBrush( QPalette::Text, hobart::colorPalette::black );                                      // item text
    p.setBrush( QPalette::Disabled, clean_->foregroundRole(), hobart::colorPalette::lightGray );   // arrow
    p.setBrush( QPalette::Disabled, QPalette::ButtonText, hobart::colorPalette::lightGray );        // scroll bar arrows
    p.setBrush( QPalette::Disabled, QPalette::Text, hobart::colorPalette::lightGray );              // item text
    clean_->setPalette( p );

    clean_->setStyleSheet(     QLatin1String("background-color: rgb(192,192,192);\n" "border-width: 2px;\n"  "border-style: solid;\n" "border-radius: 5px;\n" "border-width: 2px;") );


    clean_->setAutoFillBackground( true );
    clean_->repaint();
    update();

    plu_->setTotalTypeVisible( session_->scaleConfig->scaleConfig.showTotalTypeOnRunScreen() ); // HA-176
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::createLayout( Qt::DockWidgetArea numPadLocation )
{
    // delete previous layout
    if ( film_->layout() )
        delete film_->layout();

    QVBoxLayout *filmLeftLabels( new QVBoxLayout() );
    filmLeftLabels->setContentsMargins( QMargins() );

    filmLeftLabels->addWidget( filmSelectionLabel_ );
    filmLeftLabels->addWidget( upperFilmWidthLabel_ );
    filmLeftLabels->addWidget( lowerFilmWidthLabel_ );

    QVBoxLayout *filmLeftButtons( new QVBoxLayout() );
    filmLeftButtons->setContentsMargins( QMargins() );

    filmLeftButtons->addWidget( cutFilm_ );
    filmLeftButtons->addWidget( loadUpperRoll_ );

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        filmLeftButtons->addWidget( loadLowerRoll_ );
        filmLeftButtons->addWidget( spacer_ );
    }

    QHBoxLayout *filmLeft( new QHBoxLayout() );
    filmLeft->setContentsMargins( QMargins() );

    filmLeft->addLayout( filmLeftLabels );
    filmLeft->addLayout( filmLeftButtons );

    QVBoxLayout *filmRight( new QVBoxLayout() );
    filmRight->setContentsMargins( QMargins() );

    filmRight->addWidget( filmSelection_ );
    filmRight->addWidget( upperFilmWidth_ );
    filmRight->addWidget( lowerFilmWidth_ );

    QHBoxLayout *filmLayout( new QHBoxLayout( film_ ) );
    filmLayout->setContentsMargins( QMargins() );

    filmLayout->addLayout( filmLeft );
    filmLayout->addLayout( filmRight );

    // ---- //

    // delete previous layout
    if ( room_->layout() )
        delete room_->layout();

    QHBoxLayout *roomLeft( new QHBoxLayout() );
    roomLeft->setContentsMargins( QMargins() );

    roomLeft->addWidget( roomTempLabel_ );
    roomLeft->addWidget( roomTemp_ );           // move readout to the left side

    QVBoxLayout *roomRight( new QVBoxLayout() );
    roomRight->setContentsMargins( QMargins() );
    roomRight->addWidget( clean_ );             // add clean wrapper in its place

    QHBoxLayout *roomLayout( new QHBoxLayout( room_ ) );
    roomLayout->setContentsMargins( QMargins() );

    roomLayout->addLayout( roomLeft );
    roomLayout->addLayout( roomRight );

    // ---- //

    // delete previous layout
    if ( belt_->layout() )
        delete belt_->layout();

    QVBoxLayout *beltLeft( new QVBoxLayout() );
    beltLeft->setContentsMargins( QMargins() );

    beltLeft->addWidget( sealerBeltPowerLabel_ );
    beltLeft->addWidget( beltTempSettingLabel_ );
    beltLeft->addWidget( currentBeltTempLabel_ );

    QVBoxLayout *beltRight( new QVBoxLayout() );
    beltRight->setContentsMargins( QMargins() );

    beltRight->addWidget( sealerBeltPower_ );
    beltRight->addWidget( beltTempSetting_ );
    beltRight->addWidget( currentBeltTemp_ );

    QHBoxLayout *beltLayout( new QHBoxLayout( belt_ ) );
    beltLayout->setContentsMargins( QMargins() );
    beltLayout->addLayout( beltLeft, 1 );
    beltLayout->addLayout( beltRight, 1 );

    // ---- //

    // delete previous layout
    if ( advancedSettings_->layout() )
        delete advancedSettings_->layout();

    QVBoxLayout *advancedSettingsLeft( new QVBoxLayout() );
    advancedSettingsLeft->setContentsMargins( QMargins() );

    advancedSettingsLeft->addWidget( trayNameLabel_ );
    advancedSettingsLeft->addWidget( filmUsedLabel_ );
    advancedSettingsLeft->addWidget( knifeTimeLabel_ );

    QVBoxLayout *advancedSettingsRight( new QVBoxLayout() );
    advancedSettingsRight->setContentsMargins( QMargins() );

    advancedSettingsRight->addWidget( trayName_ );
    advancedSettingsRight->addWidget( filmUsed_ );
    advancedSettingsRight->addWidget( knifeTime_ );

    QHBoxLayout *advancedSettingsLayout( new QHBoxLayout( advancedSettings_ ) );
    advancedSettingsLayout->setContentsMargins( QMargins() );
    advancedSettingsLayout->addLayout( advancedSettingsLeft );
    advancedSettingsLayout->addLayout( advancedSettingsRight );

    // ---- //

    // delete previous layout
    if ( intake_->layout() )
        delete intake_->layout();

    QVBoxLayout *intakeLeft( new QVBoxLayout() );
    intakeLeft->setContentsMargins( QMargins() );

    intakeLeft->addWidget( intakeModeLabel_ );

    QVBoxLayout *intakeRight( new QVBoxLayout() );
    intakeRight->setContentsMargins( QMargins() );

    intakeRight->addWidget( intakeMode_ );

    QHBoxLayout *intakeLayout( new QHBoxLayout( intake_ ) );
    intakeLayout->setContentsMargins( QMargins() );

    intakeLayout->addLayout( intakeLeft );
    intakeLayout->addLayout( intakeRight );

    // ---- //

    // delete previous layout
    if ( tension_->layout() )
        delete tension_->layout();

    QVBoxLayout *tensionLeft( new QVBoxLayout() );
    tensionLeft->setContentsMargins( QMargins() );

    tensionLeft->addWidget( filmTensionLabel_ );
    tensionLeft->addWidget( tensionFrontToBackLabel_ );
    tensionLeft->addWidget( tensionLeftToRightLabel_ );

    QVBoxLayout *tensionRight( new QVBoxLayout() );
    tensionRight->setContentsMargins( QMargins() );

    tensionRight->addWidget( filmTension_ );
    tensionRight->addWidget( tensionFrontToBack_ );
    tensionRight->addWidget( tensionLeftToRight_ );

    QHBoxLayout *tensionLayout( new QHBoxLayout( tension_ ) );
    tensionLayout->setContentsMargins( QMargins() );

    tensionLayout->addLayout( tensionLeft );
    tensionLayout->addLayout( tensionRight );

    // ---- //

    // delete previous layout
    if ( layout() )
        delete layout();

    QVBoxLayout *bottomLeft( new QVBoxLayout() );
    bottomLeft->setContentsMargins( QMargins() );

    bottomLeft->addWidget( film_ );
    bottomLeft->addWidget( room_ );
    bottomLeft->addWidget( belt_ );

    QVBoxLayout *bottomRight( new QVBoxLayout() );
    bottomRight->setContentsMargins( QMargins() );

    bottomRight->addWidget( advancedSettings_ );
    bottomRight->addWidget( intake_ );
    bottomRight->addWidget( tension_ );

    QHBoxLayout *top( new QHBoxLayout() );
    top->setContentsMargins( QMargins() );
    top->setSpacing( 0 );

    top->addWidget( plu_ );
    top->addWidget( packLength_ );
    top->addWidget( packWidth_ );
    top->addWidget( packHeight_ );
    top->addWidget( lipHeight_ );

    QHBoxLayout *bottom( new QHBoxLayout() );
    bottom->setContentsMargins( QMargins() );
    bottom->setSpacing( 0 );

    bottom->addLayout( bottomLeft );
    bottom->addLayout( bottomRight );

    QHBoxLayout *bottomLayout( new QHBoxLayout() );
    bottomLayout->setContentsMargins( QMargins() );
    bottomLayout->setSpacing( 0 );

    if ( Qt::LeftDockWidgetArea == numPadLocation )
    {
        bottomLayout->addStretch();
        bottomLayout->addLayout( bottom, 1 );
        bottomLayout->addSpacing( 45 );
    }
    else if ( Qt::RightDockWidgetArea == numPadLocation )
    {
        bottomLayout->addSpacing( 45 );
        bottomLayout->addLayout( bottom, 1 );
        bottomLayout->addStretch();
    }

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins() );
    screenLayout->setSpacing( 0 );

    screenLayout->addLayout( top );
    screenLayout->addLayout( bottomLayout, 1 );

    if( session_->uiUtilities->isWrapOnly() )
    {
        plu_->hide();

        lowerFilmWidthLabel_->hide();
        lowerFilmWidth_->hide();
        loadLowerRoll_->hide();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::updateConfig( DomEvolWrapperSystemInformation *m )
{
    LOG_CLASS_INFO( "WrapperSettingsScreen", "updateConfig" );

    bool enabled( peripheralList_->isWrapperAttached() );
    bool allFeatures( true );

    // NULL system info means disable interface
    if ( !m )
        enabled = false;

    // check for "AWS Lite", Single-Roll
    else if (( DomEvolWrapperSystemInformation::AWS1BWRL == m->awsModelNumber() ) ||
             ( DomEvolWrapperSystemInformation::AWS1BWLR == m->awsModelNumber() ) ||
             ( DomEvolWrapperSystemInformation::AWS3BRL == m->awsModelNumber() ) ||
             ( DomEvolWrapperSystemInformation::AWS3BLR == m->awsModelNumber() ) ||
             ( DomEvolWrapperSystemInformation::AWS4BRL == m->awsModelNumber() ) ||
             ( DomEvolWrapperSystemInformation::AWS4BLR == m->awsModelNumber() ) ||
             ( DomEvolWrapperSystemInformation::NGW1_C_RL == m->awsModelNumber() ) || // SOF-2871
             ( DomEvolWrapperSystemInformation::NGW1_C_LR == m->awsModelNumber() ) )  // SOF-2871
    {
        LOG_INFO( "have defeatured wrapper model %d", m->awsModelNumber() );
        allFeatures = false;
    }

    // film
    filmSelectionLabel_->setVisible( enabled && allFeatures );
    filmSelection_->setVisible( enabled && allFeatures );

    upperFilmWidthLabel_->setVisible( enabled );
    upperFilmWidth_->setVisible( enabled );

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        lowerFilmWidthLabel_->setVisible( enabled && allFeatures );
        lowerFilmWidth_->setVisible( enabled && allFeatures );
    }

    cutFilm_->setVisible( mainWindow_->isPrimarySessionType() && enabled );
    loadUpperRoll_->setVisible( mainWindow_->isPrimarySessionType() && enabled );

    if( !( session_->uiUtilities->isWrapOnly() ) )
        loadLowerRoll_->setVisible( mainWindow_->isPrimarySessionType() && enabled && allFeatures );

    spacer_->setVisible( mainWindow_->isPrimarySessionType() && enabled && !allFeatures );

    // room
    roomTempLabel_->setVisible( enabled );
    roomTemp_->setVisible( enabled );

    // belt
    sealerBeltPowerLabel_->setVisible( enabled );
    sealerBeltPower_->setVisible( enabled );

    beltTempSettingLabel_->setVisible( enabled );
    beltTempSetting_->setVisible( enabled );

    currentBeltTempLabel_->setVisible( enabled );
    currentBeltTemp_->setVisible( enabled );

    // advanced wrapper settings
    trayNameLabel_->setVisible( enabled );
    trayName_->setVisible( enabled );

    filmUsedLabel_->setVisible( enabled );
    filmUsed_->setVisible( enabled );

    knifeTimeLabel_->setVisible( enabled );
    knifeTime_->setVisible( enabled );

    // intake
    intakeModeLabel_->setVisible( enabled );
    intakeMode_->setVisible( enabled );

    // tension
    filmTensionLabel_->setVisible( enabled );
    filmTension_->setVisible( enabled );

    tensionFrontToBackLabel_->setVisible( enabled );
    tensionFrontToBack_->setVisible( enabled );

    tensionLeftToRightLabel_->setVisible( enabled );
    tensionLeftToRight_->setVisible( enabled );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::updateKnifeTime( DomEvolWrapperEventCapture *m )
{
    knifeTime_->setText( QString::number( m->value() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::updatePackageDesc( DomEvolWrapperPkgDesc *m )
{
    packLength_->fromSessionField( m->length() );
    packWidth_->fromSessionField( m->width() );
    packHeight_->fromSessionField( m->height() );
    lipHeight_->fromSessionField( m->lip() );

    filmUsed_->setText( m->filmWidthUsedToWrap() );
    trayName_->setText( m->trayName() );

    // SOF-240: disable yellow highlight
    //updateNetStretch( m->modifiedNetStretch() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::updateSettings( DomEvolWrapperSettings *m )
{
    ignoreSignals_ = true;

    setComboBoxIndex( intakeMode_, m->intakeMode() );

    // Have to be able to guard against the illegal HIGH value which has been removed.
    setFilmTensionComboBoxIndex(filmTension_, m->filmTension() );       // setComboBoxIndex( filmTension_, m->filmTension() );
    setComboBoxIndex( tensionFrontToBack_, m->ftFrontToBack() );
    setComboBoxIndex( tensionLeftToRight_, m->ftLeftToRight() );
    setComboBoxIndex( filmSelection_, m->filmSelection() );
    setComboBoxIndex( upperFilmWidth_, m->upperRollWidth() );
    setComboBoxIndex( lowerFilmWidth_, m->lowerRollWidth() );
    setComboBoxIndex( sealerBeltPower_, m->sealerBeltPower() );

    beltTempSetting_->fromSessionField( m->sealerBeltSetting() );
    beltTempSetting_->setEnabled( m->sealerBeltSetting().isEmpty() ? false : true );

    ignoreSignals_ = false;

    // ---- //

    // update frame backgrounds
    bool filmEnabled( isVisibleAndEnabled( filmSelection_ ) || isVisibleAndEnabled( upperFilmWidth_ ) || isVisibleAndEnabled( lowerFilmWidth_ ) );
    bool beltEnabled( isVisibleAndEnabled( sealerBeltPower_ ) || isVisibleAndEnabled( beltTempSetting_ ) );
    bool intakeEnabled( isVisibleAndEnabled( intakeMode_ ) );
    bool tensionEnabled( isVisibleAndEnabled( filmTension_ ) || isVisibleAndEnabled( tensionFrontToBack_ ) || isVisibleAndEnabled( tensionLeftToRight_ ) );

    film_->setBackground( largeFrames_[ filmEnabled ? hobart::Normal : hobart::ReadOnly ] );
    film_->setDropShadowVisible( filmEnabled );

    belt_->setBackground( largeFrames_[ beltEnabled ? hobart::Normal : hobart::ReadOnly ] );
    belt_->setDropShadowVisible( beltEnabled );

    intake_->setBackground( smallFrames_[ intakeEnabled ? hobart::Normal : hobart::ReadOnly ] );
    intake_->setDropShadowVisible( intakeEnabled );

    tension_->setBackground( largeFrames_[ tensionEnabled ? hobart::Normal : hobart::ReadOnly ] );
    tension_->setDropShadowVisible( tensionEnabled );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::setFilmTensionComboBoxIndex(QComboBox *widget, QString comboData)
{
    int len = widget->count();      // for Film Tension, it better be 2.
    QString data = "";

    int combo_int = comboData.toInt();

    if( combo_int > 1 )
        combo_int = 1;              // so if above 1, make it 1
    else if( combo_int <= 0 )
        combo_int = 0;              // so if below 0, make it 0

    for(int j = 0; j < len; j++)
    {
        data = widget->itemData(j).toString();
        if(data.toInt() == combo_int)
        {
            widget->setEnabled( true );
            widget->setCurrentIndex(j);
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::updateStatus( DomEvolWrapperStatus *m )
{
    if ( m->sealerBeltTemp().isEmpty() )
        currentBeltTemp_->clear();
    else
        currentBeltTemp_->setText( m->sealerBeltTemp() + " " + temperatureScale() );

    if ( m->ambientTemp().isEmpty() )
        roomTemp_->clear();
    else
        roomTemp_->setText( m->ambientTemp() + " " + temperatureScale() );

    ignoreSignals_ = true;

    setComboBoxIndex( sealerBeltPower_, m->sealerBeltPower() );

    ignoreSignals_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::updateNetStretch( bool wasModified )
{
    QPalette p( filmSelection_->palette() );

    // when net stretch modified we need to turn background yellow
    QColor c( wasModified ? hobart::colorPalette::yellow : hobart::colorPalette::black );

    p.setBrush( filmSelection_->foregroundRole(), c );                  // arrow
    p.setBrush( QPalette::ButtonText, c );                              // scroll bar arrows
    p.setBrush( QPalette::Text, c );                                    // item text

    filmSelection_->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::setComboBoxIndex( QComboBox *comboBox, const QString& data )
{
    int index;

    if (( data.isEmpty() ) || ( -1 == (index = comboBox->findData( data.toInt() )) ))
        comboBox->setEnabled( false );
    else
    {
        comboBox->setEnabled( true );
        comboBox->setCurrentIndex( index );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WrapperSettingsScreen::isVisibleAndEnabled( QWidget *w )
{
    return (( w->isVisible() ) && ( w->isEnabled() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	HA-201
bool WrapperSettingsScreen::beltTempSettingHasFocus() const
{
    return beltTempSetting_->hasFocus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperSettingsScreen::onCleanTimeout()
{
    if( clean_ )
        clean_->setStyleSheet( QLatin1String("background-color: rgb(192,192,192);\n" "color: rgb(0,0,0);\n" "border-width: 2px;\n"  "border-style: solid;\n" "border-radius: 5px;\n" "border-width: 2px;") );
}
