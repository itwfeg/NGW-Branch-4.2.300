/**
 * @file operatemodedialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <sessionhooks.h>

#include "operatemodedialog.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>
#include <toolbutton.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <Messages.h>

#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

const bool OperateModeDialog::convertUppercase_ = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
OperateModeDialog::OperateModeDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    sendMessages_( false )
{
    setObjectName( "OperateModeDialog" );

    // initialize interface
    initialize();

    // set mode
    setMode();
    setItemEntry();

    sendMessages_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OperateModeDialog::~OperateModeDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OperateModeDialog::isEnabled( const ScaleConfiguration *scaleConfiguration )
{
    LOG_CLASS_INFO( "OperateModeDialog", "isEnabled" );

    const MadpAccumItemsConfig *accumItemsConfig( &scaleConfiguration->accumulateItemsConfig );
    const MadpOperatorDisplayFields *fields( &scaleConfiguration->operatorDisplayFields );
    const MadpScaleConfig *scaleConfig( &scaleConfiguration->scaleConfig );

    bool weigherAttached = (0 < const_cast<MadpScaleConfig*>( scaleConfig )->weigherAttached().toInt());

    bool enableAccumItems = accumItemsConfig->enableAccumItems();
    bool enableProdEntry = fields->enableProdEntry();
    bool enableWrapTypes = fields->enableWrapTypes();

    LOG_DEBUG( "config is w %d wt %d pe %d ai %d",
        weigherAttached,
        enableProdEntry,
        enableAccumItems,
        enableWrapTypes );

    // FT-439: when no weigher cannot perform pre-pack
    enableWrapTypes &= weigherAttached;

    return (( enableWrapTypes ) || ( enableProdEntry ) || ( enableAccumItems ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize OperateModeDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 600) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 400) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *OperateModeDialog::rendererAppearance() const
{
    return new hobart::darkOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int OperateModeDialog::rendererStyle() const
{
    return hobart::ReadOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModeDialog::onAccumulateItemEntryTypeToggled( bool value )
{
    LOG_CLASS_INFO( "OperateModeDialog", "onAccumulateItemEntryTypeToggled" );

    QColor color( value ? hobart::colorPalette::black : hobart::colorPalette::white );

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( color, color ) );
    setForegroundPalette( accumulateItemEntryType_ );

    if (( sendMessages_ ) && ( value ))
    {
        LOG_INFO( "setting mode to accumulate items", NULL );
        session_->send( SessionDev, SnWriteWeighModeTransactionType, QString::number( OpModeAccumulateItems ) );
        session_->send( SessionDev, SnReadWeighModeTransactionType );
        accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModeDialog::onManualModeTypeToggled( bool value )
{
    LOG_CLASS_INFO( "OperateModeDialog", "onManualModeTypeToggled" );

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::white, hobart::colorPalette::white ) );
    setForegroundPalette( manualModeType_ );

    if (( sendMessages_ ) && ( value ))
    {
        LOG_INFO( "setting mode to manual", NULL );
        session_->send( SessionDev, SnWriteWeighModeTransactionType, QString::number( OpModeManualWeigh ) );
        session_->send( SessionDev, SnReadWeighModeTransactionType );
        accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModeDialog::onNormalItemEntryTypeToggled( bool value )
{
    LOG_CLASS_INFO( "OperateModeDialog", "onNormalItemEntryTypeToggled" );

    QColor color( value ? hobart::colorPalette::black : hobart::colorPalette::white );

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( color, color ) );
    setForegroundPalette( normalItemEntryType_ );

    if (( sendMessages_ ) && ( value ))
    {
        LOG_INFO( "setting mode to manual", NULL );
        session_->send( SessionDev, SnWriteWeighModeTransactionType, QString::number( OpModeManualWeigh ) );
        session_->send( SessionDev, SnReadWeighModeTransactionType );
        accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModeDialog::onPrepackModeTypeToggled( bool value )
{
    LOG_CLASS_INFO( "OperateModeDialog", "onPrepackModeTypeToggled" );

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::white, hobart::colorPalette::white ) );
    setForegroundPalette( prepackModeType_ );

    if (( sendMessages_ ) && ( value ))
    {
        LOG_INFO( "setting mode to prepack", NULL );
        session_->send( SessionDev, SnWriteWeighModeTransactionType, QString::number( OpModeAutoManualApply ) );
        session_->send( SessionDev, SnReadWeighModeTransactionType );
        accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModeDialog::onProductionItemEntryTypeToggled( bool value )
{
    LOG_CLASS_INFO( "OperateModeDialog", "onProductionItemEntryTypeToggled" );

    QColor color( value ? hobart::colorPalette::black : hobart::colorPalette::white );

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( color, color ) );
    setForegroundPalette( productionItemEntryType_ );

    if (( sendMessages_ ) && ( value ))
    {
        LOG_INFO( "setting mode to production entry", NULL );
        session_->send( SessionDev, SnWriteWeighModeTransactionType, QString::number( OpModeProductionEntry ) );
        session_->send( SessionDev, SnReadWeighModeTransactionType );
        accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModeDialog::onSelfServiceModeTypeToggled( bool value )
{
    LOG_CLASS_INFO( "OperateModeDialog", "onSelfServiceModeTypeToggled" );

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::white, hobart::colorPalette::white ) );
    setForegroundPalette( selfServiceModeType_ );

    if (( sendMessages_ ) && ( value ))
    {
        LOG_INFO( "entering into self service mode!", NULL );

        // activate!
        if ( mainWindow_->activateSelfServiceMode() )
            accept();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModeDialog::initialize()
{
    LOG_CLASS_INFO( "OperateModeDialog", "initialize" );

    const MadpAccumItemsConfig *accumItemsConfig( &session_->scaleConfig->accumulateItemsConfig );
    const MadpOperatorDisplayFields *fields( &session_->scaleConfig->operatorDisplayFields );
    const MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    bool weigherAttached = (0 < const_cast<MadpScaleConfig*>( scaleConfig )->weigherAttached().toInt());

    bool enableAccumItems = accumItemsConfig->enableAccumItems();
    bool enableProdEntry = fields->enableProdEntry();
    bool enableSelfService = fields->enableSelfService();
    bool enableWrapTypes = (( fields->enableWrapTypes() ) && ( weigherAttached )); // FT-439

    // TODO - right now accumItems is not being requested at boot up time with the initial messages
    // and right now it can't because it doesn't exist in the database. Then I need to determine if
    // we want to use just accumItems.enableAccumItems() from 112,241, or is this value saved into
    // the real scaleConfig message, 112,21. Talk this over with Larry and see where he wants to
    // put it into the DB.
    // UPDATED 4-16-13: Decided to use the field from ScaleConfiguration - Test if it works as expected. Remove this after testing
    // UPDATED 4-18-13: When your field is added to scale config please uncomment this line (and remove accumItemsConfig code?)
    //                  Do we need to do the new way?  Using accumItemsConfig we can work with old scale (i.e. HLX).
    //enableAccumItems = scaleConfigData->enableAccumulateItems();

    QHBoxLayout *hlayout;
    QVBoxLayout *vlayout;

    modifyPaletteBackground setTransparentBackground( Qt::transparent );

    // HT-345
    modifyPaletteForeground setForegroundWhite( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );
    setForegroundWhite( this );

    modifyPaletteColorRole<QPalette::Text> setCheckBoxForegroundColor( hobart::colorPalette::black );
    modifyPaletteColorRole<QPalette::Window> setCheckBoxMouseOverColor( Qt::transparent );

    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );

    // setup layout
    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->setContentsMargins( QMargins( 16, 16, 16, 16 ) );

    ///////////////////////////////////////////////////////////////////////////
    // operation mode

    QString modeType( tr( "Mode" ) );
    QString manual( tr( "Manual Operation" ) );
    QString prepack( tr( "Prepack Operation" ) );
    QString selfService( tr( "Self Service Operation" ) );

    if ( convertUppercase_ )
    {
        modeType = modeType.toUpper();
        manual = manual.toUpper();
        prepack = prepack.toUpper();
        selfService = selfService.toUpper();
    }

    QGroupBox *modeTypeGroupBox = new QGroupBox( modeType, this );
    modeTypeGroupBox->setObjectName( "modeTypeGroupBox" );
    vlayout = new QVBoxLayout( modeTypeGroupBox );

    // manual
    manualModeType_ = new QCheckBox( manual, modeTypeGroupBox );
    manualModeType_->setObjectName( "manualModeType" );
    setCheckBoxForegroundColor( manualModeType_ );
    setCheckBoxMouseOverColor( manualModeType_ );
    setTransparentBackground( manualModeType_ );

    connect( manualModeType_, SIGNAL( toggled(bool) ), SLOT( onManualModeTypeToggled(bool) ) );

    vlayout->addWidget( manualModeType_ );

    // prepack
    prepackModeType_ = new QCheckBox( prepack, modeTypeGroupBox );
    prepackModeType_->setObjectName( "prepackModeType" );
    setCheckBoxForegroundColor( prepackModeType_ );
    setCheckBoxMouseOverColor( prepackModeType_ );
    setTransparentBackground( prepackModeType_ );

    connect( prepackModeType_, SIGNAL( toggled(bool) ), SLOT( onPrepackModeTypeToggled(bool) ) );

    vlayout->addWidget( prepackModeType_ );

    // self service
    selfServiceModeType_ = NULL;

    if ( enableSelfService )
    {
        selfServiceModeType_ = new QCheckBox( selfService, modeTypeGroupBox );
        selfServiceModeType_->setObjectName( "selfServiceModeType" );
        setCheckBoxForegroundColor( selfServiceModeType_ );
        setCheckBoxMouseOverColor( selfServiceModeType_ );
        setTransparentBackground( selfServiceModeType_ );

        connect( selfServiceModeType_, SIGNAL( toggled(bool) ), SLOT( onSelfServiceModeTypeToggled(bool) ) );

        vlayout->addWidget( selfServiceModeType_ );
    }

    ///////////////////////////////////////////////////////////////////////////
    // item entry type

    const QSize buttonSize(
        (mainWindow_->height() * 120) / SessionMainWindow::scalingDividerY_,
        (mainWindow_->height() * 120) / SessionMainWindow::scalingDividerY_ );

    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderPressedButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::lightButtonBackgroundWidgetRendererAppearance() );

    const QPixmap buttonPixmap( renderButton.drawControl( buttonSize, hobart::ReadOnly ) );
    const QPixmap buttonPixmapPressed( renderPressedButton.drawControl( buttonSize, hobart::Focused ) );

    QString itemEntryType( tr( "Item Entry Type" ) );
    QString normal( tr( "Normal" ) );
    QString accumulate( tr( "Accumulate\nItems" ) );
    QString production( tr( "Production\nEntry" ) );

    if ( convertUppercase_ )
    {
        itemEntryType = itemEntryType.toUpper();
        normal = normal.toUpper();
        accumulate = accumulate.toUpper();
        production = production.toUpper();
    }

    QGroupBox *itemEntryTypeGroupBox = new QGroupBox( itemEntryType, this );
    itemEntryTypeGroupBox->setObjectName( "itemEntryTypeGroupBox" );
    hlayout = new QHBoxLayout( itemEntryTypeGroupBox );

    // normal
    normalItemEntryType_ = new hobart::toolButton( normal, itemEntryTypeGroupBox, buttonPixmap, buttonPixmapPressed );
    normalItemEntryType_->setCheckable( true );
    normalItemEntryType_->setObjectName( "normalItemEntryType" );
    setFontPointSize12( normalItemEntryType_ );

    connect( normalItemEntryType_, SIGNAL( toggled(bool) ), SLOT( onNormalItemEntryTypeToggled(bool) ) );

    hlayout->addWidget( normalItemEntryType_ );

    // accumulate items
    accumulateItemEntryType_ = new hobart::toolButton( accumulate, itemEntryTypeGroupBox, buttonPixmap, buttonPixmapPressed );
    accumulateItemEntryType_->setCheckable( true );
    accumulateItemEntryType_->setObjectName( "accumulateItemEntryType" );
    setFontPointSize12( accumulateItemEntryType_ );

    connect( accumulateItemEntryType_, SIGNAL( toggled(bool) ), SLOT( onAccumulateItemEntryTypeToggled(bool) ) );

    hlayout->addWidget( accumulateItemEntryType_ );

    // production entry
    productionItemEntryType_ = new hobart::toolButton( production, itemEntryTypeGroupBox, buttonPixmap, buttonPixmapPressed );
    productionItemEntryType_->setCheckable( true );
    productionItemEntryType_->setObjectName( "productionItemEntryType" );
    setFontPointSize12( productionItemEntryType_ );

    connect( productionItemEntryType_, SIGNAL( toggled(bool) ), SLOT( onProductionItemEntryTypeToggled(bool) ) );

    hlayout->addWidget( productionItemEntryType_ );

    // button group
    QButtonGroup *buttonGroup = new QButtonGroup( itemEntryTypeGroupBox );
    buttonGroup->setObjectName( "buttonGroup" );
    buttonGroup->addButton( normalItemEntryType_ );
    buttonGroup->addButton( accumulateItemEntryType_ );
    buttonGroup->addButton( productionItemEntryType_ );

    ///////////////////////////////////////////////////////////////////////////

    screenLayout->addWidget( modeTypeGroupBox );
    screenLayout->addWidget( itemEntryTypeGroupBox );

    // check "turn on weigh mode selection"
    if ( !enableWrapTypes )
    {
        LOG_DEBUG( "operate modes disabled, not displaying", NULL );
        modeTypeGroupBox->setVisible( false );
    }

    // item entry modes
    // no modes available so hide box
    if (( !enableProdEntry ) && ( !enableAccumItems ))
    {
        LOG_DEBUG( "item entry types disabled, not displaying", NULL );
        itemEntryTypeGroupBox->setVisible( false );
    }

    else
    {
        // production entry
        if ( !enableProdEntry )
        {
            LOG_DEBUG( "production entry disabled, not displaying", NULL );
            productionItemEntryType_->setVisible( false );
        }

        // accumulate items
        if ( !enableAccumItems )
        {
            LOG_DEBUG( "accumulate items disabled, not displaying", NULL );
            accumulateItemEntryType_->setVisible( false );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModeDialog::setMode()
{
    LOG_CLASS_INFO( "OperateModeDialog", "setMode" );

    DomWeighModeTotalType *msg = &session_->scaleConfig->weighModeTotalType;
    QCheckBox *modeButton;

    LOG_DEBUG( "setMode: weighMode()=%d", msg->weighMode() );

    prepackModeType_->setChecked( false );
    prepackModeType_->setEnabled( true );

    manualModeType_->setChecked( false );
    manualModeType_->setEnabled( true );

    if ( OpModeAutoManualApply == msg->weighMode() )
        modeButton = prepackModeType_;
    else
        modeButton = manualModeType_;

    modeButton->setChecked( true );
    modeButton->setEnabled( false );

    // in addition to check also set font blue
    QPalette p( modeButton->palette() );
    p.setColor( QPalette::Active, QPalette::WindowText, hobart::colorPalette::blue );
    p.setColor( QPalette::Disabled, QPalette::WindowText, hobart::colorPalette::blue );
    p.setColor( QPalette::Active, QPalette::Text, hobart::colorPalette::black );
    p.setColor( QPalette::Disabled, QPalette::Text, hobart::colorPalette::black );
    modeButton->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModeDialog::setItemEntry()
{
    LOG_CLASS_INFO( "OperateModeDialog", "setItemEntry" );

    DomWeighModeTotalType *msg = &session_->scaleConfig->weighModeTotalType;
    QToolButton *itemEntryButton;

    LOG_DEBUG( "setItemEntry: weighMode()=%d", msg->weighMode() );

    productionItemEntryType_->setChecked( false );
    productionItemEntryType_->setEnabled( true );

    accumulateItemEntryType_->setChecked( false );
    accumulateItemEntryType_->setEnabled( true );

    normalItemEntryType_->setChecked( false );
    normalItemEntryType_->setEnabled( true );

    if ( OpModeProductionEntry == msg->weighMode() )
        itemEntryButton = productionItemEntryType_;
    else if ( OpModeAccumulateItems == msg->weighMode() )
        itemEntryButton = accumulateItemEntryType_;
    else
        itemEntryButton = normalItemEntryType_;

    itemEntryButton->setChecked( true );
    itemEntryButton->setEnabled( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// HT-227
void OperateModeDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "OperateModeDialog", "onSessionMessageReceive" );

    LOG_DEBUG( "In onSessionMessageReceive()", NULL );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // Weigh mode
    if ( ( SessionDev == device ) && ( SnWriteWeighModeTransactionType == event ) )
    {
        DomWeighModeTotalType m( msg );
        LOG_DEBUG( "onSessionMessageReceive: weighMode()=%d", m.weighMode() );

        sendMessages_ = false;

        setMode();
        setItemEntry();

        sendMessages_ = true;
    }

    // call base handler
    _Mybase::onSessionMessageReceive( msg );

}
