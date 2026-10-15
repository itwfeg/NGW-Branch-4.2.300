//#define USE_QDEBUG
/**
 * @file operatemode.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <selfservicehook.h>
#include <softkeybutton.h>

#include "operatemode.h"

#include "../numberpadwidget.h"
#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../dialogs/aboutdialog.h"
#include "../dialogs/filedialog.h"
#include "../dialogs/nowservingdialog.h"
#include "../dialogs/operatorlogindialog.h"
#include "../dialogs/portionzerodialog.h"
#include "../dialogs/productionentryprintingdialog.h"
#include "../dialogs/printersettingsdialog.h"
#include "../dialogs/productnotedialog.h"
#include "../dialogs/wrappermodedialog.h"
#include "../dialogs/calendardialog.h"
#include "../dialogs/sessionerrordialog.h"
#include "../dialogs/wrapperforcetraysizedialog.h"

#include "../panels/operatemodepanel.h"

#include "../screens/appsscreen.h"
#include "../screens/labelplacement.h"
#include "../screens/buildamealscreen.h"
#include "../screens/meatspecsheetscreen.h"
#include "../screens/pluinfoscreen.h"
#include "../screens/plumainscreen.h"
#include "../screens/webbrowserscreen.h"
#include "../screens/wrappersettings.h"
#include "../../sessionHandlerWidgets/softkeyswidget.h"

#include "../widgets/dynamiccontentwidget.h"
#include "../../source/HFC/CfgBaseFieldOffsets.h"

#include <backgroundwidgettypes.h>
#include <margins.h>
#include <palette.h>
#include <softkeyswidget.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <ScaleClasses.h>

#include <QDir>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>

// This flag controls moving the capacity and weight bar. When true, they are forced together
// always instead of being independant.
const bool OperateMode::keepBarsTogether_ = true;

// When cool is required we request cool info and wait for a response before processing any forced
// fields. It is possible the backend will never reply to us (which is a bug!)... we need to handle
// this situation. The timer here serves this need, we will process forced fields regardless of any
// missing messages when this timer expires.
const int OperateMode::missingCoolTimeout_ = 1000;

// FT-228
// When an item is loaded from a flashkey it is possible to click another flashkey with another
// item before the first item is loaded. In some of these cases the backend will crash (maybe due
// to renderer or bad item pointers, who knows...). This timer controls how long the operator must
// wait between flashkey item loads.
const int OperateMode::missingItemMessageTimeout_ = 2500;

///////////////////////////////////////////////////////////////////////////////////////////////////
OperateMode::OperateMode( ScreenWidget *screen, SessionMainWindow *parent ) :
    _Mybase( screen, Operate, parent ),
    waitingOnCool_( false ),
    waitingOnItemMessage_( false ),
    refreshBestSellers_( false ),
    printingProductionEntry_( false ),
    needToShowProductNote_( false ),
    needToShowSuggestiveSell_( false ),
    needToShowCookingInstructions_( false ),
    suggestiveSellCounter_( 0 ),
    capacityBar_( NULL ),
    weightBar_( NULL ),
    oldCapacityBar_( NULL ),
    oldWeightBar_( NULL ),
    weightBarVisible_( false ),
    isStableNonZeroWeight_( false ),
    minWeightToPrint_( 0.0 ),
    widgetDragLock_( true ),
    widgetSettingsLock_( true ),
    operatorLoggedInWidget_( NULL ),
    selfServiceOperatorRequest_( false ),
    selfServiceForceWriteSingleField_( false ),

    // initialize plate positions to home position
    primaryPlatePosition_ ( 0 ),
    secondaryPlatePosition_ ( 0 ),
    requiredPrimaryPlatePosition_ ( 0 ),
    requiredSecondaryPlatePosition_ ( 0 ),
    packageId_ ( 0 )
{
    // initialize
    initialize();

    // install event filter
    mainWindow_->installEventFilter( this );

    // connect signals
    connect( this, SIGNAL( operatorLoggedIn() ), SLOT( onOperatorLoggedIn() ) );

    connect( sessionHooks::instance(), SIGNAL( changeLanguage() ), SLOT( onHookChangeLanguage() ) );
    connect( sessionHooks::instance(), SIGNAL( resetPrimaryLanguage() ), SLOT( onHookResetPrimaryLanguage() ) );
    connect( sessionHooks::instance(), SIGNAL( changeFieldValue(int,QString) ), SLOT( onHookChangeFieldValue(int,QString) ) );
    connect( sessionHooks::instance(), SIGNAL( softkeyClicked(QWidget*,QWidget*,int,QString,int) ), SLOT( handleFlashkey(QWidget*,QWidget*,int,QString,int) ) );
    connect( sessionHooks::instance(), SIGNAL( shutdownKeyboards() ), SLOT( shutdownKeyboards() ) );
    connect( sessionHooks::instance(), SIGNAL( zeroScale() ), SLOT( onHookZeroWeigher() ) );

    // request store information
    session_->send( ConfigurationDev, CfgStoreInfoRead );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OperateMode::OperateMode( SessionMainWindow *parent ) :
    _Mybase( Operate, parent ),
    waitingOnCool_( false ),
    waitingOnItemMessage_( false ),
    refreshBestSellers_( false ),
    printingProductionEntry_( false ),
    needToShowProductNote_( false ),
    needToShowSuggestiveSell_( false ),
    needToShowCookingInstructions_( false ),
    suggestiveSellCounter_( 0 ),
    capacityBar_( NULL ),
    weightBar_( NULL ),
    oldCapacityBar_( NULL ),
    oldWeightBar_( NULL ),
    weightBarVisible_( false ),
    isStableNonZeroWeight_( false ),
    minWeightToPrint_( 0.0 ),
    widgetDragLock_( true ),
    widgetSettingsLock_( true ),
    operatorLoggedInWidget_( NULL ),
    selfServiceOperatorRequest_( false ),
    selfServiceForceWriteSingleField_( false )
{
    // initialize
    initialize();

    // install event filter
    mainWindow_->installEventFilter( this );

    // connect signals
    connect( this, SIGNAL( operatorLoggedIn() ), SLOT( onOperatorLoggedIn() ) );

    connect( sessionHooks::instance(), SIGNAL( changeLanguage() ), SLOT( onHookChangeLanguage() ) );
    connect( sessionHooks::instance(), SIGNAL( resetPrimaryLanguage() ), SLOT( onHookResetPrimaryLanguage() ) );
    connect( sessionHooks::instance(), SIGNAL( changeFieldValue(int,QString) ), SLOT( onHookChangeFieldValue(int,QString) ) );
    connect( sessionHooks::instance(), SIGNAL( softkeyClicked(QWidget*,QWidget*,int,QString,int) ), SLOT( handleFlashkey(QWidget*,QWidget*,int,QString,int) ) );
    connect( sessionHooks::instance(), SIGNAL( shutdownKeyboards() ), SLOT( shutdownKeyboards() ) );
    connect( sessionHooks::instance(), SIGNAL( zeroScale() ), SLOT( onHookZeroWeigher() ) );

    // request store information
    session_->send( ConfigurationDev, CfgStoreInfoRead );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OperateMode::~OperateMode()
{
    // delete the screen
    if ( screen_ )
    {
        delete screen_;
        screen_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DomCurrentPLU *OperateMode::currentProduct() const
{
    return &const_cast<_Myt*>( this )->currentPLU_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DomPluSupport *OperateMode::currentProductSupport( ProductSupportType type ) const
{
    return &const_cast<_Myt*>( this )->currentPLUSupportMap_[type];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MadpUpdateCurrentPLUTotals *OperateMode::currentProductTotals() const
{
    return &const_cast<_Myt*>( this )->currentPLUTotals_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OperateMode::isForcedFields() const
{
    LOG_CLASS_INFO( "OperateMode", "isForcedFields" );

    // cannot force fields until we get cool
    if ( waitingOnCool_ )
    {
        LOG_DEBUG( "waiting on cool", NULL );
        return false;
    }

    if ( needToShowProductNote_ )
    {
        LOG_DEBUG( "product note to be displayed", NULL );
        return true;
    }

    if ( needToShowSuggestiveSell_ )
    {
        LOG_DEBUG( "suggestive sell to be displayed", NULL );
        return true;
    }

    if ( needToShowCookingInstructions_ )
    {
        LOG_DEBUG( "cooking instructions to be displayed", NULL );
        return true;
    }

    PluMainScreen *mainScreen( isCached<PluMainScreen>() );

    if ( mainScreen )
        if ( mainScreen->isForcedFields() )
        {
            LOG_TRACE( "forced fields on plu main screen", NULL );
            return true;
        }

    if ( weightBar_ )
        if ( weightBar_->isForcedFields() )
        {
            LOG_TRACE( "forced fields on weight bar", NULL );
            return true;
        }

    return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OperateMode::isForcedTrayEntry() const
{
    PluMainScreen *pluMainScreen( isCached<PluMainScreen>() );

    return (( pluMainScreen ) && ( pluMainScreen->isForcedTrayEntry() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::clearPriceAndWeightFields()
{
    weightBar_->clearProductTotals();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::clearPLU()
{
    session_->send( ProductDev, ProductRead, + "0,3" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OperateMode::isPopupNumberPad() const
{
    // check screen configuration
    if ( screen() )
    {
        PluMainScreen *w;

        if ( (w = dynamic_cast<PluMainScreen*>( screen() )) )
            return w->isPopupNumberPad();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OperateMode::isPrintingAllowed( bool printKeyPressed ) const
{
    LOG_CLASS_INFO( "OperateMode", "isPrintingAllowed" );

    const DomWeighModeTotalType *weighModeTotalType( &session_->scaleConfig->weighModeTotalType );
    bool switchToMainScreenBeforePrinting = true;

    QString itemNumber( const_cast<DomCurrentPLU*>( &currentPLU_ )->itemNumber() );

    // FT-805
    // do not switch screen when in pressing print button and in pre-pack mode... this will cause
    // a "print key not allowed in pre-pack" error while still on the info screen
    if (( printKeyPressed ) && ( OpModeAutoManualApply == weighModeTotalType->weighMode() || OpModeMealBuilder == weighModeTotalType->weighMode() ))
        switchToMainScreenBeforePrinting = false;

    // not in operate mode
    if ( !mainWindow_->isOperateMode() )
    {
        LOG_INFO( "print not allowed (not in operate mode)", NULL );
        return false;
    }

    // if we are 2P and we are locked out from printing until we get the next productComplete message
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ScaleConfiguration *scaleConfig( session_->scaleConfig );

    if( scaleConfig->isWrapperTwoPrinter() && !( session_->uiUtilities->isWrapOnly() ) )
    {
        LOG_INFO( "isWrapperTwoPrinter so test if we hold up print (does it hold up intake too?)", NULL );

        if( !scaleConfig->internalConfig.isOkToLookUpPlu )
        {
            LOG_INFO( "!scaleConfig->internalConfig.isOkToLookUpPlu is TRUE so test if we have a droppedPluNum and its > 0", NULL );

            if( ( scaleConfig->internalConfig.isOkDroppedPLU > 0 ) && ( scaleConfig->internalConfig.isOkDroppedPLU != scaleConfig->currentPLU.itemNumber().toInt() ) )
            {
                LOG_INFO( "2P print not allowed because isOkToLookupPLU is FALSE and we have a dropped PLU waiting", NULL );
                return false;

                // attempted effort at shortcutting system - but it did not work
                /*
                if( ( scaleConfig->previousPLU.primaryLabelTypeWidth()  != scaleConfig->currentPLU.primaryLabelTypeWidth() ) ||
                    ( scaleConfig->previousPLU.primaryLabelTypeLength() != scaleConfig->currentPLU.primaryLabelTypeLength() ) ||
                    ( scaleConfig->previousPLU.primaryLabelPlacement()  != scaleConfig->currentPLU.primaryLabelPlacement() ) ||
                    ( scaleConfig->previousPLU.labelStockRotation1()    != scaleConfig->currentPLU.labelStockRotation1() ) ||
                    ( scaleConfig->previousPLU.primaryLabelRotation()   != scaleConfig->currentPLU.primaryLabelRotation() ) ||
                    ( scaleConfig->previousPLU.secondLabelTypeWidth()   != scaleConfig->currentPLU.secondLabelTypeWidth() ) ||
                    ( scaleConfig->previousPLU.secondLabelTypeLength()  != scaleConfig->currentPLU.secondLabelTypeLength() ) ||
                    ( scaleConfig->previousPLU.secondLabelPlacement()   != scaleConfig->currentPLU.secondLabelPlacement() ) ||
                    ( scaleConfig->previousPLU.labelStockRotation2()    != scaleConfig->currentPLU.labelStockRotation2() ) ||
                    ( scaleConfig->previousPLU.secondLabelRotation()    != scaleConfig->currentPLU.secondLabelRotation() ) )
                {
                    scaleConfig->internalConfig.poppedRezero = false;
                    scaleConfig->internalConfig.isOkDroppedPLU = 0;
                    scaleConfig->internalConfig.isOkToLookUpPlu = true;

                    LOG_INFO( "2P can print because the 2 PLUs have the same orientation", NULL );
                    return true;
                }
                else
                {
                    // this state indicates that we put a tray on the intake supper fast and entered a PLU
                    // but the first PLUs productComplete has not been received, so the weight on the platter
                    // is accepted as the product of the first plu, not the looked up 2nd PLU, because it hasnt
                    // actually been looked up yet, (it will be looked up upon receiving productComplete)
                    // so lets hold the item on the platter in a pseudo window closed, and dont approve this print
                    // yet until productComplete received
                    LOG_INFO( "2P print not allowed because isOkToLookupPLU is FALSE and we have a dropped PLU waiting", NULL );
                    return false;
                }
                */
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // FT-530: allow when accumulated items
    // no current item
    // HA-65: Allow the print key to be sent without a PLU for Prepack only.
    if ( ( !session_->isPrepack() ) &&
         ( ( OpModeAccumulateItems != weighModeTotalType->weighMode() ) &&
           ( OpModeProductionEntry != weighModeTotalType->weighMode() ) ) && // SOF-2827: Need to allow Production Entry print
         ( itemNumber.isEmpty() ) )
    {
        LOG_INFO( "print not allowed (no item)", NULL );
        return false;
    }

    // cannot print when dialog on screen
    if ( mainWindow_->isDialogOpen() )
    {
        // SOF-5628 - "Prepack mode not queuing labels ..." - from 4.2.0-5.2.0 April 6
        LOG_INFO( "isPrintingAllowed - isDialogOpen() true ", NULL );
        QString errStr =  QString::number(mainWindow_->getErrorCode());
        LOG_INFO( QString::number(mainWindow_->getErrorCode()).toUtf8().constData(), NULL );

        LOG_INFO( ( mainWindow_->isDialogOpen<SessionErrorDialog>() ) ? "mainWindow_->isDialogOpen<SessionErrorDialog>() true" : "mainWindow_->isDialogOpen<SessionErrorDialog>() false", NULL );

        bool isCounterScale( false );
        MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
        if (settings && settings->scaleModel() == "3")  // from superPrinterSetup.cpp
            isCounterScale = true;

        LOG_INFO( ( isCounterScale ) ? "isCounterScale true" : "isCounterScale false", NULL );

        bool isInPrepackMode( OpModeAutoManualApply == weighModeTotalType->weighMode() );
        LOG_INFO( ( isInPrepackMode ) ? "isInPrepackMode true" : "isInPrepackMode false", NULL );


        // HA-12: allow printing when mode dialog up
        if (( session_->isPrepack() ) && ( mainWindow_->isDialogOpen<WrapperModeDialog>() ))
        {
            LOG_INFO( "allowing print for wrappermodedialog", NULL );
        }
        else if (( session_->isPrepack() ) && ( mainWindow_->isDialogOpen<SessionErrorDialog>() && mainWindow_->getErrorCode() == 5010 ))
        {
            LOG_INFO( "allowing print for pkg overrun", NULL );
        }
        else if (( session_->isPrepack() ) && ( mainWindow_->isDialogOpen<SessionErrorDialog>() && (mainWindow_->getErrorCode() == 101067 ) ))
        {
            LOG_INFO( "allowing print for flashkey placement/rotation change", NULL );
        }
        // SOF-5628 "Prepack mode not queuing labels ..."
        else if ( isCounterScale && isInPrepackMode && mainWindow_->isDialogOpen<SessionErrorDialog>() && mainWindow_->getErrorCode() == 7005 )
        {
            LOG_INFO( "allowing print for prepack mode with take label dialog", NULL );
        }
        else
        {
            LOG_INFO( "print not allowed (dialog open)", NULL );
            return false;
        }

    }

    // FT-125: it appears the backend handles this and will pop error dialog if more forced fields
    // cannot print when forced fields
/*    if ( isForcedFields() )
    {
            LOG_INFO( "print not allowed (forced fields)", NULL );
            return false;
    }
*/
    // check forced tray entry
    if ( isForcedTrayEntry() )
    {
        LOG_INFO( "print not allowed (need to entry tray size)", NULL );
        return false;
    }

    // Note:
    //
    // This behavior as per Gary Cornish and Jim Highley. When print button pressed on info
    // screen (page 3) we are to switch to run screen (page 2) and then perform the print. This
    // is to get around W&M rule that weight must be visible to operator before printing a label.
    //
    // If for some reason this is not acceptable to W&M, change switchToMainScreenBeforePrinting
    // to false for original behavior.

    PluInfoScreen *pluInfoScreen( dynamic_cast<PluInfoScreen*>( screen() ) );

    if (( pluInfoScreen ) && ( switchToMainScreenBeforePrinting ))
        mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();

    PluMainScreen *pluMainScreen( dynamic_cast<PluMainScreen*>( screen() ) );
    MealBuilderScreen *mealBuilderScreen( dynamic_cast<MealBuilderScreen*>( screen() ) );

    // cannot print unless on main screen or build-a-meal screen (only when accumulating)
    if (( pluMainScreen ) ||
        ( mealBuilderScreen && ( OpModeAccumulateItems == weighModeTotalType->weighMode() || OpModeMealBuilder == weighModeTotalType->weighMode()) ))
        ; // valid
    else
    {
        bool printingNotAllowed( true );

        // for prepack, printing allowed on these screens
        if ( session_->isPrepack() )
            if (( dynamic_cast<LabelPlacementScreen*>( screen() ) ) ||
                ( dynamic_cast<WrapperSettingsScreen*>( screen() ) ))
            {
                printingNotAllowed = false;
            }

        if ( printingNotAllowed )
        {
            LOG_INFO( "print not allowed (not on run screen)", NULL );
            return false;
        }
    }

    // FT-1092
    // cannot print while printing production
    if ( printingProductionEntry_ )
    {
        LOG_INFO( "print not allowed (printing production)", NULL );
        return false;
    }

    LOG_INFO( "print allowed", NULL );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OperateMode::isPrintingProductionEntry() const
{
    if ( printingProductionEntry_ )
    {
        const DomWeighModeTotalType *weighModeTotalType( &session_->scaleConfig->weighModeTotalType );

        return (OpModeProductionEntry == weighModeTotalType->weighMode());
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OperateMode::isSelfService() const
{
    selfServiceHook *hook( dynamic_cast<selfServiceHook*>( sessionHooks::instance()->findHook( "selfServiceHook" ) ) );

    if ( !hook )
        return false;
    else if ( hook->active() )
        return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CapacityBarArea OperateMode::capacityBarArea() const
{
    return capacityBar_->area();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::setCapacityBarArea( CapacityBarArea value )
{
    if ( capacityBarArea() == value )
        return;

    WeightBarArea weightArea = weightBarArea();

    if ( keepBarsTogether_ )
        weightArea = (TopCapacityBarArea == value ? TopWeightBarArea : BottomWeightBarArea);

    // setup the bars
    setupBars( value, weightArea );

    // emit signal
    Q_EMIT marginsChanged();

    // update widget
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::setFocusForPopupNumberPad()
{
    LOG_CLASS_INFO( "OperateMode", "setFocusForPopupNumberPad" );

    LOG_TRACE( "setting focus widget...", NULL );

    MadpInternalConfig *internalConfig( &session_->scaleConfig->internalConfig );

    // nothing to do, waiting on operator login message
    if ( operatorLoggedInWidget_ )
    {
        LOG_DEBUG( "no focus change, waiting for operator login", NULL );
        return;
    }

    // hide number pad when popup
    if( isSelfService() || internalConfig->ssactive_lock )
    {
        LOG_DEBUG( "NUMPAD Focus: IF Do nothing in self service we dont EVER want the numpad have focus and pop up on our screen", NULL );
        mainWindow_->hideInputWidgets();
        return;
    }
    else if ( isPopupNumberPad() )
    {
        LOG_DEBUG( "NUMPAD Focus: ELSE IF Unknown Condition but hideInputWidgets()", NULL );
        mainWindow_->hideInputWidgets();
    }
    // restore when not popup number pad
    // only restore when not widget layout locked
    else if (( screen_ ) && ( widgetSettingsLock() ))
    {
        // set focus to active screen in order to pop numberpad
        LOG_DEBUG( "NUMPAD Focus: ELSE screen_->setFocus()", NULL );
        screen_->setFocus();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QMargins OperateMode::capacityBarMargins() const
{
    return capacityBar_->margins();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WeightBarArea OperateMode::weightBarArea() const
{
    return weightBar_->area();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::setWeightBarArea( WeightBarArea value )
{
    if ( weightBarArea() == value )
        return;
    else if (( TopCapacityBarArea == capacityBarArea() ) && ( TopWeightBarArea != value ))
        return;

    CapacityBarArea capacityArea = capacityBarArea();

    if ( keepBarsTogether_ )
        capacityArea = (TopWeightBarArea == value ? TopCapacityBarArea : BottomCapacityBarArea);

    // setup the bars
    setupBars( capacityArea, value );

    // emit signal
    Q_EMIT marginsChanged();

    // update widget
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::setWidgetDragLock( bool value )
{
    LOG_CLASS_INFO( "OperateMode", "setWidgetDragLock" );

    // nothing to do
    if ( widgetDragLock() == value )
        return;

    widgetDragLock_ = value;

    // we are locked, need to save settings!
    if ( widgetDragLock_ )
    {
        MadpOperatorDisplayFields *operatorDisplayFields = &session_->scaleConfig->operatorDisplayFields;

        int capacityBarLocation = (TopCapacityBarArea == capacityBarArea() ? 0 : 1);
        int keypadLocation = (Qt::RightDockWidgetArea == mainWindow_->numberPadArea() ? 0 : 1);

        // save if different
        if (( capacityBarLocation != operatorDisplayFields->capacityBarLocation() ) ||
            ( keypadLocation != operatorDisplayFields->keypadLocation() ))
        {
            operatorDisplayFields->setCapacityBarLocation( capacityBarLocation );
            operatorDisplayFields->setKeypadLocation( keypadLocation );

            LOG_DEBUG( "sending screen configuration...", NULL );
            session_->send( *operatorDisplayFields );
            session_->send( ConfigurationDev, CfgPrepackSave );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::setWidgetSettingsLock( bool value )
{
    // nothing to do
    if ( widgetSettingsLock() == value )
        return;

    // emit signal
    Q_EMIT widgetSettingsLockChanged( widgetSettingsLock_ = value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QMargins OperateMode::weightBarMargins() const
{
    return weightBar_->margins();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::createCachedScreens()
{
    scaleFlags flags( session_->uiUtilities->scaleNameEx( NULL ) );

    if( session_->uiUtilities->isWrapOnly() )
    {
        cache( createScreen<WrapperSettingsScreen>() );
    }
    else
    {
        // cache frequently used screens (create in order of usage, most used first)
        bool allowWebBrowserScreen( true );

        // HTs
        // this feature is disabled
        if (( session_->isServiceCounter() ) && ( sfReducedFeatureSet & flags ))
            allowWebBrowserScreen = false;
        else if ( session_->scaleConfig->scaleConfig.disableStartScreen() )
        {
            // HT-181
            allowWebBrowserScreen = false;
        }

        cache( createScreen<PluMainScreen>() );
        cache( createScreen<PluInfoScreen>() );

        if ( allowWebBrowserScreen )
            cache( createScreen<WebBrowserScreen>() );

        if ( session_->isPrepack() )
        {
            cache( createScreen<WrapperSettingsScreen>() );
            cache( createScreen<LabelPlacementScreen>() );
        }

    }

    // create capacity and weight bars
    setupBars( TopCapacityBarArea, TopWeightBarArea );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::activate()
{
    MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;
    MadpInternalConfig *internalConfig( &session_->scaleConfig->internalConfig );

    // have to figure out what do we want to set Add Extra Sell By Day to
    if( internalConfig->scaleBootComplete )
    {
        // by default on boot its off if we have Toggle Flashkey.  Since we came here as already booted we must be going back to operate from somewhere.
        // Maybe Super where we changed this value. Thing is, their users are not allowed in Super so this was for our benefit.
        // So I am always enabling or disabling the flag based upon the last value set in Operate since that is all who can change it.
        bool enableXtraDay      = scaleConfig->enableVariableShelfLifeTime();
        bool hasXtraDayFlashkey = session_->scaleConfig->extraShelfLifeDayAvail;

        if( session_->scaleConfig->toggleExtraDayON || session_->scaleConfig->toggleExtraDayNumPadON || session_->scaleConfig->toggleExtraDayLeftON )
        {
            // if any of the toggles are set true, but in super, we changed this to false, set it back to true
            if( !enableXtraDay )
            {
                QString param;
                param += "," + QString::number( fieldOffsetCfgEnableVariableShelfLifeTime );
                param += ",1,0";        // Enables extra shelf life day
                session_->send( ConfigurationDev, CfgSaveSingleField, param );
            }
        }
        else
        {
           // so all of the toggle fields are false, but if in super we set it to true, then set it back to false.
            if( enableXtraDay && hasXtraDayFlashkey )
            {
                QString param;
                param += "," + QString::number( fieldOffsetCfgEnableVariableShelfLifeTime );
                param += ",0,1";        // Disables extra shelf life day
                session_->send( ConfigurationDev, CfgSaveSingleField, param );
            }
        }
    }

    // restore flashkey data
    if ( internalConfig->updateOperateFlashkeyData )
    {
        updateSoftkeyGroups();
        internalConfig->updateOperateFlashkeyData = false;
    }

    // restore label data
    if ( internalConfig->updateOperateLabelData )
    {
        PluInfoScreen *pluInfoScreen = NULL;

        // plu main screen
        if ( (pluInfoScreen = isCached<PluInfoScreen>() ) )
            pluInfoScreen->resetLabelData();

        internalConfig->updateOperateLabelData = false;
    }

    // Want to be able to clear this if its true
    // FT-1092
    if ( printingProductionEntry_ )
    {
        PluMainScreen *pluMainScreen( isCached<PluMainScreen>() ); // FT-1114

        if ( pluMainScreen )
            pluMainScreen->hideOperatorMessage();

        printingProductionEntry_ = false;

        pluMainScreen->setPluDisabled( printingProductionEntry_ );
    }

    // call base handler
    _Mybase::activate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PanelWidget *OperateMode::createMenuPanel( int panelEx, SessionDialog *parent )
{
    return new OperateModePanel( panelEx, mainWindow_, parent );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::processForcedFields()
{
    LOG_CLASS_INFO( "OperateMode", "processForcedFields" );

    // when no item nothing to do
    if (( currentProduct() ) && ( currentProduct()->itemNumber().isEmpty() ))
        return;

    // when dialog open nothing to do... we will be called again when dialog closes
    if ( mainWindow_->isDialogOpen() )
        return;

    // check for forced fields
    if ( isForcedFields() )
    {
        // schedule timer to handle forced fields
        timerProcessForcedFields_->setSingleShot( true );
        timerProcessForcedFields_->start( 0 );
    }

    // reset waiting on item
    waitingOnItemMessage_ = false;

    // invoke hook
    sessionHooks::itemType t( sessionHooks::randomWeightItem );

    if (( currentProduct() ) && ( 3 == currentProduct()->productType() ) )
        t = sessionHooks::forcedByCount;

    ScaleConfiguration *scaleConfig( session_->scaleConfig );

    if( !scaleConfig->internalConfig.ssactive_lock )
    {
        LOG_WARN( "processForcedFields() - NOT self serv so exit with type %d", t );
        return;
    }

    sessionHooks::instance()->updateCurrentItem( t );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::processInvalidAccessLevel( QWidget *widget )
{
    LOG_CLASS_INFO( "OperateMode", "processInvalidAccessLevel" );

    // access denied
    if ( loggedIn() )
        session_->send( Error, ErrOperatorDevice, QString::number( 102016 ) + ",5" );

    // prompt operator login
    else
    {
        OperatorLoginDialog d( OperatorLoginDialog::OperatorLoginRequiredFieldAccess, mainWindow_ );

        LOG_TRACE( "widget forcing login is 0x%08x [%s]", widget, qPrintable( widget->objectName() ) );

        // Note:
        // This widget should be reset (set to NULL) after the operator logs in or if the login
        // fails. I have tried to capture all the cases I know of but there may be more. This is
        // especially true for when we are processing the login due to some system event (i.e. key
        // press, mouse clicks)... focus logic depends on this variable being properly (we attempt
        // to restore focus to the widget that triggered the login).
        operatorLoggedInWidget_ = widget;

        LOG_TRACE( "processing forced login...", NULL );

        // show operator id login dialog
        if ( QDialog::Accepted != d.exec() )
        {
            // login failed or cancelled
            operatorLoggedInWidget_ = NULL;

            // restore focus
            setFocusForPopupNumberPad();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::toggleCurrentLanguage()
{
    LOG_CLASS_INFO( "OperateMode", "toggleCurrentLanguage" );

    ScaleConfiguration *scaleConfig( session_->scaleConfig );

    const QString oldLanguage( scaleConfig->getFPPLanguage() );

    // toggle language
    scaleConfig->currentLanguage = (1 == scaleConfig->currentLanguage ? 2 : 1);


    // FT-1321
    // language is same or second language is None... nothing to do
    if ( oldLanguage == scaleConfig->getFPPLanguage() )
    {
        scaleConfig->currentLanguage = (1 == scaleConfig->currentLanguage ? 2 : 1);
        return;
    }

    // update backend with language change
    session_->send( SoftkeyDev, ExecuteSoftkey, "4," + QString::number( scaleConfig->currentLanguage ) );
    // set language
    LOG_TRACE( "set language %s...", qPrintable( scaleConfig->getFPPLanguage() ) );
    mainWindow_->setLanguage( scaleConfig->getFPPLanguage() );

    // language was not set!
    if ( oldLanguage == mainWindow_->language() )
    {
        // toggle language
        scaleConfig->currentLanguage = (1 == scaleConfig->currentLanguage ? 2 : 1);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::togglePrimaryLanguage()
{
    LOG_CLASS_INFO( "OperateMode", "togglePrimaryLanguage" );

    ScaleConfiguration *scaleConfig( session_->scaleConfig );

    if ( 2 == scaleConfig->currentLanguage )
    {
        // update backend with language change
        session_->send( SoftkeyDev, ExecuteSoftkey, "4,1" );

        // set language
        scaleConfig->currentLanguage = 1;                      // if secondary is set to None, this this always true

        LOG_TRACE( "set language %s...", qPrintable( scaleConfig->getFPPLanguage() ) );
        mainWindow_->setLanguage( scaleConfig->getFPPLanguage() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::translate()
{
    if( !(session_->uiUtilities->isWrapOnly() ) )
    {
        capacityBar_->translate();
        weightBar_->translate();
    }
    _Mybase::translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::writeSingleField( ProductWriteSingleFieldOffset off, const QString& newValue, const QString& oldValue )
{
    LOG_CLASS_INFO( "OperateMode", "writeSingleField" );

    const QString itemNumber( currentPLU_.itemNumber() );

    // sanity check
    if ( itemNumber.isEmpty() )
        return;
    else if ( !off )
        return;
    else if (( newValue == oldValue ) && ( !selfServiceForceWriteSingleField_ ))
        return;

    // save off old value for session update, it may change on us in updateField() if caller
    // passed in reference to the raw product record value
    QString oldValueSend( oldValue );

    LOG_DEBUG( "WRITE product offset %d from '%s' to '%s'", off, qPrintable( oldValue ), qPrintable( newValue ) );
    updateField( off, newValue );

    // write!
    session_->send( ProductDev, ProductWriteSingleField,
        itemNumber + "," + QString::number( off ) + ",\"" + newValue + "\",\"" + oldValueSend + "\",0" );
}

int OperateMode::getPrimaryPlatePosition() const
{
    return primaryPlatePosition_;
}

int OperateMode::getSecondaryPlatePosition() const
{
    return secondaryPlatePosition_;
}

int OperateMode::getRequiredPrimaryPlatePosition() const
{
    return requiredPrimaryPlatePosition_;
}

int OperateMode::getRequiredSecondaryPlatePosition() const
{
    return requiredSecondaryPlatePosition_;
}

int OperateMode::getPackageId() const
{
    return packageId_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::hideWeightBar()
{
    if ( !weightBarVisible_ )
        return;

    // hide!
    weightBarVisible_ = false;
    weightBar_->hide();

    // reset layout margins
    if ( layout() )
       layout()->setContentsMargins( overlayMargins() );

    // emit signal
    Q_EMIT marginsChanged();

    // update widget
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::showWeightBar( WeightBarWidget::DisplayMode mode )
{
    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        if ( weightBarVisible_ )
        {
            weightBar_->setDisplayMode( mode );
            return;
        }

        // show!
        weightBarVisible_ = true;
        weightBar_->setDisplayMode( mode );
        weightBar_->show();

        // reset layout margins
        if ( layout() )
            layout()->setContentsMargins( overlayMargins() );

        // emit signal
        Q_EMIT marginsChanged();

        // update widget
        update();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::handleFlashkey( QWidget *w, QWidget *f, int action, const QString& param, int feature )
{
#ifdef USE_QDEBUG
    qDebug() << "OperateMode::handleFlashkey(..., " << action << ", " << param << ", " << feature << ")";
#endif
    LOG_CLASS_INFO( "OperateMode", "handleFlashkey" );

 #ifndef USE_QDEBUG
   Q_UNUSED( f );
#endif

    ScaleConfiguration *scaleConfig = session_->scaleConfig;
    hobart::softkeysWidget *softkeys = dynamic_cast<hobart::softkeysWidget*>( w );
    hobart::softkeyButton *button = dynamic_cast<hobart::softkeyButton*>( f );
    scaleConfig->toggledState = false;
    int leftPanelOption( scaleConfig->scaleConfig.dcaStartScreenType() );

    // handle action
    switch ( action )
    {
    // no action
    case Action_NoAction:
        break;

    // print label
    case Action_PrintLabel:
        // FT-997
        setFocusForPopupNumberPad();

        mainWindow_->printKeyPressed();

        break;

    // change screen
    case Action_ChangeScreen:

        switch ( param.toInt() )
        {
        case 0: // operateRunScreen
        case 1: // operateRunScreenRandom
        case 2: // operateRunScreenFixed
        case 3: // operateRunScreenByCount
        case 5: // operateChangeOperator
        case 7: // operateShelfLife
        case 13: // operateCOOL

            // Note:
            // In existing legacy code these flashkeys would switch to the appropriate frame for
            // field input. In fastrack code, we simply click in the field we wish to edit. Clicking
            // in the field is exactly like clicking a flashkey... there is no reason to support
            // these cases.
            //
            // operateShelfLife = click on sell by
            // operateCOOL = click on cool

            mainWindow_->navigateHome();
            break;

        case 16: // operatePresetTares

            // operatePresetTares = click on tare (assumes you have preset tare softkey feature setup properly)

            // SOF-2806: Need to also show the Preset Tare grid by setting focus on the Tare field
            mainWindow_->navigateHome();
            weightBar_->setTareFocus();

            break;

        case 4: // operateSearch
            mainWindow_->searchKeyPressed();
            break;

        case 6: // operateLabelData
            mainWindow_->switchToModeScreen<OperateMode, PluInfoScreen>();
            break;

        case 8: // operateWrapperSettings
        case 11: // operateWrapperSettingsAdvanced
            if ( session_->isPrepack() )
                mainWindow_->switchToModeScreen<OperateMode, WrapperSettingsScreen>();
            break;

        case 9: // operatePrinterSettings
            showPrinterSettings();
            break;

        case 10: // operateLabelPlacement
            if ( session_->isPrepack() )
                mainWindow_->switchToModeScreen<OperateMode, LabelPlacementScreen>();
            break;

        case 12: // operateWeighModeTotalType
            mainWindow_->showMenuPanel( SessionMainWindow::OperateMenuPanelMode, OperateModePanel::TotalTypesExtendedPanel );
            break;

        case 14: // operateExpirationLabels
            mainWindow_->showMenuPanel( SessionMainWindow::OperateMenuPanelMode, OperateModePanel::ExpirationHistoryExtendedPanel );
            break;

        case 15: // operateVoidTransactions
            mainWindow_->showMenuPanel( SessionMainWindow::OperateMenuPanelMode, OperateModePanel::VoidTransactionExtendedPanel );
            break;

        case 18: // about scale dialog
        {
            AboutDialog *dialog = new AboutDialog( mainWindow_ );

            // show dialog!
            dialog->open();

            break;
        }

        case 19: // wrapper tray override
        {
            WrapperForcedTraySizeDialog d( mainWindow_ );

            if ( d.traySizesExist() > 0 )
                d.exec();
            else
            {
                MessageBoxDialog dialog( mainWindow_ );
                dialog.setText( tr( "No trays exist within the flashkey group. Please add trays to the group and try again." ) );
                dialog.setStandardButtons( QDialogButtonBox::Ok );

                dialog.exec();
            }

            break;
        }

        // show product support text
        case (40+ExpandedText): // ExpandedText
        case (40+SpecialMessage): // SpecialMessage
        case (40+ProductNote): // ProductNote
        case (40+Marquee): // Marquee
        case (40+CookingInstructions): // CookingInstructions
        case (40+Recipe): // Recipe
        case (40+ExpandedText2): // ExpandedText2
        case (40+SuggestiveSell): // SuggestiveSell
        case (40+Allergens): // Allergens
        case (40+Nutrifacts): // Nutrifacts

            // see the ProductSupportType enumeration... this is a linear 1:1 mapping starting at 40
            showProductNote( (ProductSupportType) (param.toInt() - 40) );
            break;

        default:
            LOG_WARN( "unhandled change screen '%s'", qPrintable( param ) );
            mainWindow_->navigateHome();
            break;
        }

        break;

    // browse web
    case Action_BrowseWeb:
    case Action_FPP_URL:
    case Action_Application_URL:

        {
            QString page( param.trimmed() );

            int cgiIndex = param.indexOf( "?" );

            // FT-1110: cgi params do not apply to "browse web"
            if ( 3 == action )
                cgiIndex = -1;

            // find out if we have arguments to send... this would be true for store applications right now
            if ( 0 < cgiIndex )
            {
                QString sep = "?";

                // build up page with all the variables
                page = param.left( cgiIndex );

                if ( 0 < param.indexOf( "store" ) )
                {
                    DomStoreInformation *storeInfo( &session_->scaleConfig->storeInfo );

                    page += (sep + "store=" + storeInfo->storeID());
                    sep = "&";
                }

                if ( 0 < param.indexOf( "dept" ) )
                {
                    DomStoreInformation *storeInfo( &session_->scaleConfig->storeInfo );

                    page += (sep + "dept=" + storeInfo->deptNum());
                    sep = "&";
                }

                if ( 0 < param.indexOf( "plu" ) )
                {
                    page += (sep + "plu=" + currentPLU_.itemNumber());
                    sep = "&";
                }

                if ( 0 < param.indexOf( "qty" ) )
                {
                    if ( 1 == currentPLU_.productType() )
                        page += (sep + "qty=1");
                    else
                        page += (sep + "qty=" + QString::number( currentPLU_.byCount() ) );

                    sep = "&";
                }

                if ( 0 < param.indexOf( "lang" ) )
                {
                    page += (sep + "lang=" + scaleConfig->getFPPLanguage());
                    sep = "&";
                }

                if ( 0 < param.indexOf( "country" ) )
                {
                    page += (sep + "country=" + scaleConfig->scaleConfig.countryCode());
                    sep = "&";
                }
            }

            // the page text is blank
            if ( !page.length() )
                session_->send( Error, ErrProductDevice, "136016,5" );
            else
            {
                // As per Jim Highley, flashkeys linked to a web page will show in a dialog.
                mainWindow_->showWebpage( page );
            }
        }

        break;

    // change language
    case Action_ChangeLanguage:
        // check for languages
        if (( scaleConfig->scaleConfig.languageCode().length() ) && ( scaleConfig->scaleConfig.secLanguageCode().length() ))
        {
            LOG_INFO( "toggle language...", NULL );
            toggleCurrentLanguage();
        }
        else
        {
            session_->send( Error, ErrProductDevice, "136015,5" );
        }

        break;

    // login operator
    case Action_LoginOperator:
        session_->send( OperatorDev, OpLogin, param + ",,,,,,1" );
        break;

    // reset price
    // set platter tare
    case Action_ResetPrice:
    case Action_SetPlatterTare:
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) );
        break;

    // select product
    case Action_SelectProduct:

/* FT-1469: zero product number will clear active plu now instead of error
        if ( !param.toInt() )
            session_->send( Error, ErrProductDevice, "101047,5" );
*/
/* FT-634: this does not seem to be needed anymore due to backend fixes
        // make sure we are not waiting on another item load already
        else if ( waitingOnItemMessage_ )
            LOG_WARN( "cannot process product flashkey, still waiting on previous product", NULL );
*/
        // load run screen
        if ( mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>() )
        {
            PluMainScreen *pluMainScreen = dynamic_cast<PluMainScreen*>( screen() );

            if ( pluMainScreen )
            {
                // heading back back to the CENTER just make sure this is true.
                session_->scaleConfig->internalConfig.isLeftPanelActivated = false;

                // we are now waiting on an item
                timerResetWaitingOnItemMessage_->setSingleShot( true );
                timerResetWaitingOnItemMessage_->start( missingItemMessageTimeout_ );

                // prevent another item from being loaded via flashkey
                waitingOnItemMessage_ = true;

                // request plu number
                 pluMainScreen->setCurrentItemNumber( param.isEmpty() ? "0" : param );
            }
        }

        break;

    // load softkey group
    case Action_LoadSoftkeyGroup:
        if ( !feature )
            feature = softkeys->feature();

        // DROP TO NEXT CASE (same message)

    // load first category
    // load next category
    // load previous category
    case Action_LoadCategory_First:
    case Action_LoadCategory_Next:
    case Action_LoadCategory_Previous:

        if( isSelfService() || ( hobart::dynamicContentWidget::FixedFlashkeys  == leftPanelOption ) )
            setToggledCoordinates( w, f, feature, action ); // FS-UI - on click of CAT - or apples in CAT

        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );

        if( !isSelfService() && ( hobart::dynamicContentWidget::FixedFlashkeys != leftPanelOption ) )
            setToggledCoordinates( w, f, feature, action );

        break;

    // discount - cents off
    // discount - percent off
    case Action_ApplyDiscount_CentsOff:
    case Action_ApplyDiscount_PercentOff:
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param );
        break;

    // void transaction
    case Action_VoidTransaction:
        session_->send( ProductDev, ProductPackageComplete );
        break;

    // load first group of specials
    case Action_LoadSpecials_First:

        if( isSelfService()  || ( hobart::dynamicContentWidget::FixedFlashkeys == leftPanelOption ) )
            setToggledCoordinates( w, f, feature, action );

        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + ",," + QString::number( feature ) );

        if( !isSelfService()  && ( hobart::dynamicContentWidget::FixedFlashkeys != leftPanelOption ) )
            setToggledCoordinates( w, f, feature, action );     // send request for toggled flashkey

        break;

    // load next group of specials
    // load previous group of specials
    // load first group of searched items
    // load next group of searched items
    // load previous group of searched items
    case Action_LoadSpecials_Next:
    case Action_LoadSpecials_Previous:

        if( isSelfService() || ( hobart::dynamicContentWidget::FixedFlashkeys == leftPanelOption ) )
            setToggledCoordinates( w, f, feature, action );

        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );

        if( !isSelfService() && ( hobart::dynamicContentWidget::FixedFlashkeys != leftPanelOption ) )
            setToggledCoordinates( w, f, feature, action );

        break;

    case Action_LoadSearch_First:
    case Action_LoadSearch_Next:
    case Action_LoadSearch_Previous:
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );
        break;

    // print existing and new shelf life expiration label
    case Action_PrintExistingExpirationLabel:
    case Action_PrintNewExpirationLabel:
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) );
        break;

    // start cutting test
    // stop cutting test
    case Action_BeginCutTest:
        {
            MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

            // Start the Cut Test but first kill any currently active PLU displayed on the screen
            if ( CutTestOption::CutTestOption_Default == scaleConfig->cuttingTestType() )       // DEFAULT
            {
                if( 0 < currentPLU_.itemNumber().toInt() )
                {
                    // reset the screen of the current PLU
                    session_->send( ProductDev, ProductRead, "0" );
                }
            }

            // START CUT TEST FLASHKEY
            session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );
            ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
            scaleConfiguration->askOperatorToLoginToStartCutTest = true;
        }
        break;

    case Action_EndCutTest:
        {
            // END CUT TEST FLASHKEY

            MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

            // SOF-3921 Cut Test Stop Button
            if( !mainWindow_->isCuttingTestActive() )
            {
                QString errMsg = QString::number(ProductDevError::ProductErrCannotStopCutTestNotStarted) + ",5";
                session_->send( Error, ErrProductDevice, errMsg );      // No Cut Test to Stop
                return;
            }

            if ( CutTestOption::CutTestOption_Default == scaleConfig->cuttingTestType() )   // DEFAULT
            {
                if( 0 < currentPLU_.itemNumber().toInt() )
                {
                    // reset the screen of the current PLU
                    session_->send( ProductDev, ProductRead, "0" );
                }

                session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );
                if( feature != 999015 )
                    mainWindow_->loadLastCuttingTestReport();
                else
                    mainWindow_->toggleCuttingTest( false );

                // reset the cut test plu and spec sheet data
                ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
                MadpCutTestPlu *cutTestPlu( &scaleConfiguration->cutTestPlu );

                cutTestPlu->pluNumber = 0;
                cutTestPlu->meatSpecSheet = 0;
                cutTestPlu->cutTestNumber = 0;

                PluMainScreen *pluMainScreen( dynamic_cast<PluMainScreen*>( screen() ) );

                // Reset the Main Screen Flashkeys to 999003
                if ( pluMainScreen )
                    pluMainScreen->setSpecialFlasheyWidget( false );  // hide the cut test run screen

            }
            else if ( CutTestOption::CutTestOption_Option1 == scaleConfig->cuttingTestType() )     // OPTION1
            {
                // Tell the UI who made us go to the Details screen so we can return here later on when done
                session_->scaleConfig->returnHomeLocation = CutTestState::CutTestState_End;
                mainWindow_->launchCuttingTest( CutTestState::CutTestState_End );
            }
        }
        break;

        case Action_CutTestSuspend:
        {
            MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

            if ( CutTestOption::CutTestOption_Default == scaleConfig->cuttingTestType() )       // DEFAULT
                return;

            // SOF-3921 Cut Test Stop Button
            if( !mainWindow_->isCuttingTestActive() )
            {
                QString errMsg = QString::number(ProductDevError::ProductErrCannotSuspendCutTestNotStarted) + ",5";
                session_->send( Error, ErrProductDevice, errMsg );      // No Cut Test to Stop
                return;
            }

             // SUSPEND CUT TEST FLASHKEY - Now save the current cut test data
            session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( Action_CutTestSuspend ) + "," + "" + "," + "999035" );

            mainWindow_->resetCuttingTest( false );

            // reset the cut test plu and spec sheet data
            ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
            MadpCutTestPlu *cutTestPlu( &scaleConfiguration->cutTestPlu );

            cutTestPlu->pluNumber = 0;
            cutTestPlu->meatSpecSheet = 0;
            cutTestPlu->cutTestNumber = 0;

            PluMainScreen *pluMainScreen( dynamic_cast<PluMainScreen*>( screen() ) );

            // Reset the Main Screen Flashkeys to 999003
            if ( pluMainScreen )
                pluMainScreen->setSpecialFlasheyWidget( false );  // hide the cut test run screen

            // reset the screen of the current PLU
            session_->send( ProductDev, ProductRead, "0" );
        }
        break;

    case Action_CutTestHistory:
        {
            ScaleConfiguration *scaleConfiguration( session_->scaleConfig );

            if( CutTestOption::CutTestOption_Default == scaleConfiguration->scaleConfig.cuttingTestType() )     // DEFAULT
            {
                // If Option1 - we cant delete or resume tests, so no reason to restrict the tests shown.  So show all
                session_->scaleConfig->opLoggedInForCutTestHistory = true;

                // dont just straight launch it. Ask politely for data first
                session_->send( CutTestDev, CuttingTestHistoryRead );
            }
            else
            {
                if( session_->scaleConfig->getIsOperatorLoggedIn() )
                {
                    // CUT TEST HISTORY FLASHKEY Login the operator IF they are already logged in
                    session_->scaleConfig->opLoggedInForCutTestHistory = true;

                    // dont just straight launch it. Ask politely for data first
                    session_->send( CutTestDev, CuttingTestHistoryRead );
                }
                else
                {
                    // CUT TEST HISTORY FLASHKEY Login the operator IF they are not already logged in
                    OperatorLoginDialog::LoginType method = OperatorLoginDialog::OpLoginRequiredForCutTestHistory;

                    OperatorLoginDialog *dialog = new OperatorLoginDialog( method, mainWindow_ );   // create dialog
                    dialog->open();                // prompt operator login
                }
            }
        }
        break;

    // request a product
    case Action_SelectAnyProduct:
    {
        // END RUN FLASHKEY - Need to be able to go to Option One flashkey feature grid
        //                 right here. Based upon if a meat spec sheet exists go the plu grid screen

        if ( mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>() )
        {
            PluMainScreen *pluMainScreen( dynamic_cast<PluMainScreen*>( screen() ) );

            if ( pluMainScreen )
                pluMainScreen->setFocusPluEntry();

            // If the primal has a meat spec sheet, then I need to transition to it
            if( mainWindow_->isCuttingTestActive() )
            {
                ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
                MadpCutTestPlu *cutTestPlu( &scaleConfiguration->cutTestPlu );

                if( cutTestPlu->meatSpecSheet > 0 )
                {
                    //------------------------------------------------------------------------
                    // HT-201: If there is a Meat Spec Sheet there is no need to clear the PLU
                    //------------------------------------------------------------------------

                    // after we print if we are in the middle of a cutting test,
                    // return to the spec IF we have one for the cut test plu
                    // we are here bc we have a meat sheet and clicked end run
                    scaleConfiguration->cuttingTestMeatSpecActive = true;
                    mainWindow_->switchToModeScreen<OperateMode, MeatSheetScreen>(); // MeatSheetScreen
                }
                else
                {
                    // Then the active product does not have a meat spec sheet so I need to
                    // clear the screen of any plu information currently on the screen
                    Q_EMIT clearPluForCutTest();
                }
            }
        }
        break;
    }

    // print run totals
    case Action_PrintRunTotals:
        // SOF-3399 [PJR-329] Custom Run Totals Label
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );
        break;

    // load tare value
    case Action_LoadTareSoftkey:

        // remove focus from input field
        setFocusForPopupNumberPad();

        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );
        break;

    // label placement change
    case Action_ChangeLabelPlacementAndPosition:
        {
            
            // no 2nd label - so we have to subtract a million hex or 16777216 decimal
            //int intparam = param.toInt();
            //intparam = intparam - 16777216;
            //QString newparam( QString::number( intparam ) );

            session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );
        }    

        break;

    // serve next customer
    case Action_ServeNextCustomer:
        if ( session_->uiUtilities->isFeatureLicensed( SCALE_NOW_SERVING, false ) )
        {
            if ( mainWindow_->isPrimarySessionType() )
            {
                int command( NowServingQueueManagement::ServingNextCustomer );

                // using pre-printed paper roll, not electronic system
                if ( 2 == scaleConfig->scaleConfig.nowServingSysType() )
                    command = NowServingQueueManagement::IncrementCustomerNumber;

                session_->send( SessionDev, SnNowServingQueueManagement, QString::number( command ) );
            }
        }
        break;

    // now serving queue management
    case Action_NowServingQueueManagement:
        if ( session_->uiUtilities->isFeatureLicensed( SCALE_NOW_SERVING, false ) )
        {
            NowServingDialog *dialog = new NowServingDialog( mainWindow_ );
            dialog->open();
        }

        break;

    case Action_ChangeLabelType:
    {
#ifdef USE_QDEBUG
    qDebug() << "OperateMode::handleFlashkey():  Action_ChangeLabelType,   param = [" << param << "]";
#endif
        const QString itemNumber( currentPLU_.itemNumber() );
        if ( !itemNumber.isEmpty() )
        {
            if( session_->scaleConfig->toggleChangeLabel == NULL )
                session_->scaleConfig->toggleChangeLabel = new hobart::softkeyToggleManager();

            if( session_->scaleConfig->toggleChangeLabel )
                setToggledCoordinates( w, f, feature, action );

            // create a new flashkey to handle creating toggling change label types

            session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + ",\"" + param + "\"," + QString::number( feature ) );
        }

        break;
    }

    case Action_LoadAlphabeticSearch_First:
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + ",\"" + param + "\"," + QString::number( feature ) );
        break;

    // load best sellers
    case Action_LoadMostCommon_First:
        if( isSelfService() || ( hobart::dynamicContentWidget::FixedFlashkeys == leftPanelOption ) )
            setToggledCoordinates( w, f, feature, action );

        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );

        if( !isSelfService() && ( hobart::dynamicContentWidget::FixedFlashkeys != leftPanelOption ) )
            setToggledCoordinates( w, f, feature, action );

        break;

    // logout operator
    case Action_LogoutOperator:
        session_->send( OperatorDev, OpLogout );
        session_->scaleConfig->setIsOperatorLoggedIn( false );
        session_->scaleConfig->setLoggedOperatorId("");
        session_->scaleConfig->setLoggedOperatorName("");
        session_->scaleConfig->setLoggedOperatorAccess(0);

        break;

    // play video
    case Action_PlayVideo:
        if ( param.isEmpty() )
            LOG_WARN( "empty video filename", NULL );
        else
        {
            LOG_WARN( "showing video '%s'", qPrintable( param ) );
            mainWindow_->showVideo( param );
        }

        break;

    // portioning (portion zero)
    case Action_Portioning:
        {
            // switch to main screen
            if ( mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>() )
            {
                PluMainScreen *pluMainScreen( isCached<PluMainScreen>() );

                if ( pluMainScreen )
                {
                    // remove active plu
                    const QString itemNumber( const_cast<DomCurrentPLU*>( &currentPLU_ )->itemNumber() );

                    if (( itemNumber.length() ) && ( "0" != itemNumber ))
                        pluMainScreen->setCurrentItemNumber( "0" );
                }
            }

            // pop dialog
            PortionZeroDialog *dialog( new PortionZeroDialog( mainWindow_ ) );
            dialog->open();
        }

        break;

    case Action_LaunchExternalApp:
        {
            AppsScreen *appsScreen( isCached<AppsScreen>() );

            if ( appsScreen )
            {
                if ( appsScreen->launchApp( param ) )
                    LOG_INFO( "launched external app '%s'", qPrintable( param ) );
                else
                    LOG_WARN( "failed to launch external app '%s'", qPrintable( param ) );
            }
        }

        break;

    case Action_ShowAccumulatedItems:
        {
            // retrieve run screen
            PluMainScreen *pluMainScreen( isCached<PluMainScreen>() );

            if ( pluMainScreen )
                pluMainScreen->showAccumulateItems();
        }

        break;

    case Action_OpenDocumentFolder:
        {
            // only works on primary scale
            if ( !mainWindow_->isPrimarySessionType() )
            {
                LOG_WARN( "open document only works on primary scale", NULL );
                break;
            }

            QString nameFilter;
            nameFilter += "Portable Document Format (*.pdf)";
            nameFilter += ";;";
            nameFilter += "Word Document (*.doc *.dot *.docx *.docm *.dotx *.dotm)";
            nameFilter += ";;";
            nameFilter += "Excel Document (*.xls *.xlt *.xlm *.xlsx *.xlsm *.xltx *.xltm *.xlsb *.xla *.xlam *.xll *.xlw)";
            nameFilter += ";;";
            nameFilter += "PowerPoint Document (*.ppt *.pps *.pptx *.pptm *.potx *.potm *.ppam *.ppsx *.ppsm *.sldx *.sldm)";

            QStringList folders;
            QString folder;

#if defined( Q_OS_LINUX )
            folders.push_back( "/usr/local/hobart/docs" );
            folders.push_back( "/media/thumb" );
            folders.push_back( "/media" );
#elif defined( Q_OS_WIN )
            folders.push_back( "C:\\Program Files\\Hobart\\Docs" );
            folders.push_back( "D:\\" );
            folders.push_back( "E:\\" );
            folders.push_back( "C:\\" );
#endif

            // find existing folder
            foreach ( const QString& f, folders )
                if ( QDir( folder = f ).exists() )
                    break;

            FileDialog dialog( mainWindow_ );
            dialog.setAcceptMode( QFileDialog::AcceptOpen );
            dialog.setDirectory( folder );
            dialog.setFilter( nameFilter );
            dialog.setOptions( QFileDialog::DontConfirmOverwrite );
            dialog.setWindowTitle( tr( "Open Document" ) );

            LOG_INFO( "prompting for open document...", NULL );

            if ( QDialog::Accepted != dialog.exec() )
                LOG_INFO( "user cancelled open document", NULL );
            else
            {
                QStringList selected( dialog.selectedFiles() );

                if ( 1 == selected.count() )
                {
                    const QString doc( selected[0] );

                    if ( doc.isEmpty() )
                        LOG_INFO( "user cancelled open document", NULL );
                    else
                    {
                        LOG_INFO( "opening document", qPrintable( doc ) );
                        mainWindow_->showWebpage( "file://" + doc );
//                        mainWindow_->showDocument( doc );
                    }
                }
            }
        }

        break;

    case Action_TogglePick5Active:
        {
            // Enable Pick 5 per Flashkey
            session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );

            ScaleConfiguration *scaleConfiguration( session_->scaleConfig );

            bool isPick5Enabled = scaleConfiguration->getIsPick5Enabled();

            if( isPick5Enabled )
            {
                isPick5Enabled = false;
                scaleConfiguration->setIsPick5Active( false );
            }
            else
                isPick5Enabled = true;

            scaleConfiguration->setIsPick5Enabled( isPick5Enabled );

            // we need to check if we have a PLU on the screen,
            // and clear background if we do set if not
            const QString itemNumber( currentPLU_.itemNumber() );
            if ( !itemNumber.isEmpty() )
            {
                if( isPick5Enabled )
                {
                    // do we have non blank fields > 0 in weight or price.
                    // If so, turn orange. Dialog pops when printing happens.
                    const QString minWtStr( currentPLU_.minWeight() );
                    const QString maxWtStr( currentPLU_.maxWeight() );
                    const QString minPrStr( currentPLU_.minPrice() );
                    const QString maxPrStr( currentPLU_.maxPrice() );

                    float minWt = minWtStr.toFloat();
                    float maxWt = maxWtStr.toFloat();
                    float minPr = minPrStr.toFloat();
                    float maxPr = maxPrStr.toFloat();

                    if( (minWt >= 0 && maxWt > 0) || (minPr >= 0 || maxPr > 0) )
                        scaleConfiguration->setIsPick5Active( true );
                    else
                        scaleConfiguration->setIsPick5Active( false );
                }

                mainWindow_->togglePickFive( scaleConfiguration->getIsPick5Active() );
            }
        }

        break;


    case Action_Swipe:
    {
      int swipe = QString(param).toInt();

      switch(swipe)
      {
        case ScreenWidget::Down:
        case ScreenWidget::Left:
        case ScreenWidget::Right:
        case ScreenWidget::Up:
          mainWindow_->swipeTheScreen(swipe);
          break;

        default:
          LOG_WARN( "invalid swipe direction: '%s'", qPrintable( param ));
          break;
      }
    }
    break;

   case Action_ChangeDepartment:
    {
        session_->send( DeptDev, DeptDevDtSetCurrentDept, param);
        session_->send( ProductDev, ProductRead, "0" );
        mainWindow_->navigateHome();
        break;
    }

    case Action_ToggleExtraShelfLifeDay:
    {
        // Enable Extra Shelf Life Day Flashkey

        if ( isExtraDayAlreadyEnabled( feature ) )
        {
            session_->send( Error, ProductDev, QString::number( ProductDevError::ProductErrExtraDayAlreadyEnabled ) + ",5" );
            return;
        }

        MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

        if( mainWindow_->isOperateMode() )
        {
            mainWindow_->setEnabled( false );

            session_->scaleConfig->toggleExtraDayNow = true;                                                        // toggles here from first click until drawn

            if ( hobart::softkeysWidget::RunScreenButtons == feature )
                session_->scaleConfig->toggleExtraDayON = !session_->scaleConfig->toggleExtraDayON;                 // toggled or not toggled
            else if( hobart::softkeysWidget::RunScreenNumericKeypadButtons == feature )
                session_->scaleConfig->toggleExtraDayNumPadON = !session_->scaleConfig->toggleExtraDayNumPadON;     // toggled or not toggled
            else if( hobart::softkeysWidget::ProductGrid == feature )
            {
                session_->scaleConfig->toggleExtraLeftPanelDayNow = true;
                session_->scaleConfig->toggleExtraDayLeftON = !session_->scaleConfig->toggleExtraDayLeftON;         // toggled or not toggled
            }
            else if( hobart::softkeysWidget::FixedRowColumnGrid == feature )
            {
                // we are not acting on the Fixed 999020 feature but the RunScreenButtons feature embedded in it
                session_->scaleConfig->toggleExtraDayON = !session_->scaleConfig->toggleExtraDayON;                 // toggled or not toggled
                feature = hobart::softkeysWidget::RunScreenButtons;                                                 // because its a feature INSIDE the fixed flashkey feature in the center panel
            }

            bool enableTime = scaleConfig->enableVariableShelfLifeTime();

            if( session_->scaleConfig->toggleExtraDayON || session_->scaleConfig->toggleExtraDayNumPadON || session_->scaleConfig->toggleExtraDayLeftON || session_->scaleConfig->toggleExtraDayON)
            {
                if( enableTime == 0)
                {
                    // if we are toggled and we are false, set to true
                    QString param;
                    param += "," + QString::number( fieldOffsetCfgEnableVariableShelfLifeTime );
                    param += ",1,0";        // Enables extra shelf life day
                    session_->send( ConfigurationDev, CfgSaveSingleField, param );
                }
            }
            else
            {
                if( enableTime == 1)
                {
                    // if we are not toggled and we are true, set to false
                    QString param;
                    param += "," + QString::number( fieldOffsetCfgEnableVariableShelfLifeTime );
                    param += ",0,1";        // Disables extra shelf life day
                    session_->send( ConfigurationDev, CfgSaveSingleField, param );
                }
            }

            // Toggle Add Extra Sell by Day flashkey and send softkey action
            session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );

            // This will set up the toggled flashkey but send no messages like usual for fixed rows
            setToggledCoordinates( w, f, feature, action );

            // request the default group, 136,40, no guarantee the feature and group are the same
            session_->send( SoftkeyDev, ReadSoftkeyFeature, QString::number( feature ) );

            timerToggleFlashkeyEnable_->setSingleShot( true );
            timerToggleFlashkeyEnable_->start( 5000 );
        }
    }
    break;

    // Forced Camera Override
    case Action_OverrideWrapperCameraDimensions:
        {
            const bool useCamera(( button->actionParam().isEmpty() ) || ( "0_0_0" == button->actionParam() ));

            const QString trayName( useCamera ? QString() : button->text().left( 10 ) );

            // retrieve run screen
            PluMainScreen *pluMainScreen( isCached<PluMainScreen>() );

            if ( pluMainScreen )
            {
                pluMainScreen->setForcedTrayEntry( false );
                pluMainScreen->setTrayName( trayName );
            }

            // retrieve operate screen
            PluOperateScreen *pluOperateScreen( isCached<PluOperateScreen>() );

            if ( pluOperateScreen )
            {
                pluOperateScreen->setForcedTrayEntry( false );
                pluOperateScreen->setTrayName( trayName );
            }

            session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );
        }

        break;
		
    case Action_Toggle2ndLabelEnable:
    {
        //SOF-5092 Disable secondary label Flashkey
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );

    }
    break;


    default:
#ifdef USE_QDEBUG
        qDebug() << "OperateMode::handleFlashkey():  unhandled flashkey action: " << action;
#endif
        LOG_WARN( "unhandled flashkey action %d, '%s' (%d)", action, qPrintable( param ), feature );
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::setToggledCoordinates(QWidget *w, QWidget *f, int feature, int action)
{
    LOG_CLASS_INFO( "OperateMode", "setToggledCoordinates" );

    ScaleConfiguration *scaleConfig( session_->scaleConfig );

    if( action ==  Action_ToggleExtraShelfLifeDay )
    {
        if( ( hobart::softkeysWidget::RunScreenButtons == feature ) ||
            ( hobart::softkeysWidget::ProductGrid == feature ) ||
            ( hobart::softkeysWidget::RunScreenNumericKeypadButtons == feature ) ||
            ( hobart::softkeysWidget::FixedRowColumnGrid == feature ) )
        {

            hobart::softkeysWidget *softkeys = dynamic_cast<hobart::softkeysWidget*>( w );

            if ( softkeys != w )
                return;

            hobart::softkeyButton *flashkey = dynamic_cast<hobart::softkeyButton*>( f );

            if ( !flashkey )
            {
                LOG_WARN( "bad flashkey click", NULL );
                return;
            }

            LOG_DEBUG( "flashkey '%s' selected 0x%08x", qPrintable( flashkey->text() ), flashkey );

            QPoint pos( flashkey->gridPos() );
            int posX = pos.x();
            int posY = pos.y();

            if( ( hobart::softkeysWidget::RunScreenButtons == feature ) || ( hobart::softkeysWidget::FixedRowColumnGrid == feature ) )
            {
                scaleConfig->toggleExtraDayRow = posX;                  // I added
                scaleConfig->toggleExtraDayCol = posY;                  // center panel
            }
            else if( hobart::softkeysWidget::RunScreenNumericKeypadButtons == feature )
            {
                scaleConfig->toggleExtraDayRowNumPad = posX;            // I added
                scaleConfig->toggleExtraDayColNumPad = posY;            // numpad panel
            }
            else if( hobart::softkeysWidget::ProductGrid == feature )
            {
                scaleConfig->toggleExtraDayRowLeft = posX;              // I added
                scaleConfig->toggleExtraDayColLeft = posY;              // left panel
            }
        }
    }
    else if( ( hobart::softkeysWidget::FixedRowColumnGrid == feature ) || ( hobart::softkeysWidget::FreshServeFixedButtons == feature ) )
    {
        hobart::softkeysWidget *softkeys = dynamic_cast<hobart::softkeysWidget*>( w );

        if ( softkeys != w )
            return;

        hobart::softkeyButton *flashkey = dynamic_cast<hobart::softkeyButton*>( f );

        if ( !flashkey )
        {
            LOG_WARN( "bad flashkey click", NULL );
            return;
        }

        LOG_DEBUG( "flashkey '%s' selected 0x%08x", qPrintable( flashkey->text() ), flashkey );

        QPoint pos( flashkey->gridPos() );
        int posX = pos.x();
        int posY = pos.y();

        int clickedCol = scaleConfig->toggledCol;                   // y coordinate cell
        int clickedRow = scaleConfig->toggledRow;                   // x coordinate cell
        int clickedFixedRowCol = scaleConfig->toggledFixed;         // the fixed row or column (1=top,2=left,3=right,4=bottom)

        int hSize = scaleConfig->toggledLastRow;                    // number of rows
        int vSize = scaleConfig->toggledLastCol;                    // number of columns

        if( 1 == clickedFixedRowCol )          // top row
        {
            if( posY != 0 )
            {
                posX = clickedRow;
                posY = 0;
            }
        }
        else if( 2 == clickedFixedRowCol )     // left col
        {
            if( posX != 0 )
            {
                posX = 0;
                posY = clickedCol;
            }
        }
        else if( 3 == clickedFixedRowCol )     // bottom row
        {
            if( posY != (vSize - 1) )
            {
                posX = clickedRow;              // only come in here if we clicked an item in the actual grid
                posY = clickedCol;
            }
        }
        else if( 4 == clickedFixedRowCol)      // right col
        {
            if( posX != (hSize - 1) )
            {
                posX = clickedRow;
                posY = clickedCol;
            }
        }

        scaleConfig->toggledRow = posX;
        scaleConfig->toggledCol = posY;
        scaleConfig->toggledState = true;

        // scaleConfig->softkeyFeature.defaultGroupNum() - use to send this, but if we clicked on Add Sell By Day
        // and then we clicked on the Fixed First Column, the click would send the default group of the Add Sell By Day Feature

        // will need to check it using add sell by date in self service.  Can that even be a thing?
        if( scaleConfig->internalConfig.ssactive_lock && ( action !=  Action_ToggleExtraShelfLifeDay) )
            scaleConfig->internalConfig.fixedFlashkeyDefaultGroup = scaleConfig->softkeyFeature.defaultGroupNum();

        // (136,20)
        session_->send(
           SoftkeyDev,
           ReadSoftkey,
           QString::number( softkeys->feature() ) + "," +
           QString::number( scaleConfig->internalConfig.fixedFlashkeyDefaultGroup ) + "," +
           QString::number( posX ) + "," +
           QString::number( posY ) );
    }
    else if( action ==  Action_ChangeLabelType )
    {
        if( ( hobart::softkeysWidget::RunScreenButtons == feature ) || ( hobart::softkeysWidget::ProductGrid == feature ) )
        {

            hobart::softkeysWidget *softkeys = dynamic_cast<hobart::softkeysWidget*>( w );

            if ( softkeys != w )
                return;

            hobart::softkeyButton *flashkey = dynamic_cast<hobart::softkeyButton*>( f );

            if ( !flashkey )
            {
                LOG_WARN( "bad flashkey click", NULL );
                return;
            }

            LOG_DEBUG( "flashkey '%s' selected 0x%08x", qPrintable( flashkey->text() ), flashkey );

            QPoint pos( flashkey->gridPos() );
            int posX = pos.x();
            int posY = pos.y();
            int defGroup = softkeys->group();

            // (136,20)
            session_->send(
               SoftkeyDev, ReadSoftkey,
               QString::number( softkeys->feature() ) + "," + QString::number( defGroup ) + "," +
               QString::number( posX ) + "," + QString::number( posY ) );

            if( session_->scaleConfig->toggleChangeLabel->toggleFlashkeyON() )
            {
                session_->scaleConfig->toggleChangeLabel->setToggleFlashkeyON( false );
                session_->scaleConfig->toggleChangeLabel->setToggleFlashkeyRow( -1 );
                session_->scaleConfig->toggleChangeLabel->setToggleFlashkeyCol( -1 );
                session_->scaleConfig->toggleChangeLabel->setToggleFlashkeyGroup( -1 );
                session_->scaleConfig->toggleChangeLabel->setToggleFlashkeyBorder( 0 );
            }
            else
            {
                // select and toggle on the change label flashkey
                session_->scaleConfig->toggleChangeLabel->setToggleFlashkeyON( true );
                session_->scaleConfig->toggleChangeLabel->setToggleFlashkeyRow( pos.x() );
                session_->scaleConfig->toggleChangeLabel->setToggleFlashkeyCol( pos.y() );
                session_->scaleConfig->toggleChangeLabel->setToggleFlashkeyGroup( defGroup );
                session_->scaleConfig->toggleChangeLabel->setToggleFlashkeyBorder( 20 );
            }
            session_->scaleConfig->toggleChangeLabel->setToggledFlashkeyImage("");                        // always
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OperateMode::isExtraDayAlreadyEnabled(int feature)
{
    // Ok this tests if the feature is already turned by a different feature.
    // So for example, we are in the run screen center panel and click the
    // toggle Add Sell By, see if the NumPad or Left Panel already turned it on

    bool retVal( false );

    if ( ( hobart::softkeysWidget::RunScreenButtons == feature ) || ( hobart::softkeysWidget::FixedRowColumnGrid == feature ) )
    {
        if( session_->scaleConfig->toggleExtraDayNumPadON || session_->scaleConfig->toggleExtraDayLeftON )
            retVal = true;
    }
    else if( hobart::softkeysWidget::RunScreenNumericKeypadButtons == feature )
    {
        if( session_->scaleConfig->toggleExtraDayON || session_->scaleConfig->toggleExtraDayLeftON )
            retVal = true;
    }
    else if( hobart::softkeysWidget::ProductGrid == feature )
    {
        if( session_->scaleConfig->toggleExtraDayON || session_->scaleConfig->toggleExtraDayNumPadON )
            retVal = true;
    }

    return retVal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QMargins OperateMode::overlayMargins() const
{
    QMargins result( _Mybase::overlayMargins() );

    if ( capacityBar_ )
        result += capacityBar_->margins();

    if (( weightBar_ ) && ( weightBarVisible_ ))
        result += weightBar_->margins();

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool OperateMode::eventFilter( QObject *obj, QEvent *e )
{
    bool result = _Mybase::eventFilter( obj, e );

    if ( mainWindow_ == obj )
        if ( QEvent::Resize == e->type() )
        {
            // adjust capacity bar location
            if ( capacityBar_ )
            {
                if ( BottomCapacityBarArea == capacityBar_->area() )
                    capacityBar_->move( 0, mainWindow_->height() - capacityBar_->height() );
            }

            // adjust weight bar location
            if ( weightBar_ )
            {
                QMargins margins = capacityBarMargins();

                if ( BottomWeightBarArea == weightBar_->area() )
                    weightBar_->move( 0, mainWindow_->height() - weightBar_->height() - margins.bottom() );
                else if ( TopWeightBarArea == weightBar_->area() )
                    weightBar_->move( 0, margins.top() );
            }
        }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::onDeleteWidgets()
{
    if ( oldCapacityBar_ )
    {
        delete oldCapacityBar_;
        oldCapacityBar_ = NULL;
    }

    if ( oldWeightBar_ )
    {
        delete oldWeightBar_;
        oldWeightBar_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::onHookChangeFieldValue( int type, const QString& value )
{

    switch ( type )
    {
    case sessionHooks::itemCode:

        // switch to main screen
        if ( mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>() )
        {
            PluMainScreen *pluMainScreen( isCached<PluMainScreen>() );

            if ( pluMainScreen )
                pluMainScreen->setCurrentItemNumber( value );
        }

        break;

    case sessionHooks::itemByCount:

        // self service by count items are weird... they are forced fields but ui has no idea
        // they are forced because the field is not set
        selfServiceForceWriteSingleField_ = true;
        weightBar_->setByCount( value );
        selfServiceForceWriteSingleField_ = false;
        break;

    case sessionHooks::selfServicePassword:

        // self service mode password is simply an operator id... if the operator id exists we
        // should unlock the screen
        if ( value.length() )
        {
            selfServiceOperatorRequest_ = true;
            session_->send( OperatorDev, OpDoesIDExist, value );
        }

        break;

    case sessionHooks::selfServiceOperatorLogin:

        // self service operator login for pickers, or any operator that exists, unlocks the screen
        if ( value.length() )
        {
            selfServiceOperatorRequest_ = true;
            session_->send( OperatorDev, OpLogin, value + ",,,,,,1" );
        }

        break;

    default:
        break;
    }

    //PJR413 (SOF-4772) - Close keyboard that gets opened during this event
    shutdownKeyboards();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::onHookChangeLanguage()
{
    LOG_CLASS_INFO( "OperateMode", "onHookChangeLanguage" );

    LOG_INFO( "toggle language...", NULL );
    toggleCurrentLanguage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::onHookResetPrimaryLanguage()
{
    LOG_CLASS_INFO( "OperateMode", "onHookResetPrimaryLanguage" );

    LOG_INFO( "toggle language...", NULL );

    togglePrimaryLanguage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::onHookZeroWeigher()
{
    session_->uiUtilities->rezeroWeigher();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "OperateMode", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    bool isOperateMode = mainWindow_->isOperateMode();

    // plu write
    if (( ProductDev == device ) && ( ProductWrite == event ) && ( isOperateMode ))
    {
        currentPLU_ = msg;

        if ( !currentPLU_.itemNumber().isEmpty() )
        {
            const DomWeighModeTotalType *weighModeTotalType( &session_->scaleConfig->weighModeTotalType );

            if( session_->scaleConfig->internalConfig.ssactive_lock )
            {
                session_->send( ProductDev, ProductReadSupportData, currentPLU_.itemNumber() + "," + QString::number( ExpandedText ) );
                session_->send( ProductDev, ProductReadSupportData, currentPLU_.itemNumber() + "," + QString::number( Nutrifacts ) );

                LOG_DEBUG( "onSessionMessage case 101,7 - update self serv so call sessionHooks::updateCurrentItem() and Q_EMIT currentProductChanged", NULL );
                sessionHooks::instance()->updateCurrentItem( sessionHooks::none );
                Q_EMIT currentProductChanged( false );

                processForcedFields();              // we use to send a cool req message but we dont need that in self serv
            }
            else
            {
                // reset focus
                setFocusForPopupNumberPad();

                // clear out previous data
                currentPLUSupportMap_.clear();

                session_->send( ProductDev, ProductReadSupportData, currentPLU_.itemNumber() + "," + QString::number( ProductNote ) );
                session_->send( ProductDev, ProductReadSupportData, currentPLU_.itemNumber() + "," + QString::number( Recipe ) );
                session_->send( ProductDev, ProductReadSupportData, currentPLU_.itemNumber() + "," + QString::number( SuggestiveSell ) );
                session_->send( ProductDev, ProductReadSupportData, currentPLU_.itemNumber() + "," + QString::number( Allergens ) );
                session_->send( ProductDev, ProductReadSupportData, currentPLU_.itemNumber() + "," + QString::number( CookingInstructions ) );
                session_->send( ProductDev, ProductReadSupportData, currentPLU_.itemNumber() + "," + QString::number( ExpandedText2 ) );
                session_->send( ProductDev, ProductReadSupportData, currentPLU_.itemNumber() + "," + QString::number( ExpandedText ) );
                session_->send( ProductDev, ProductReadSupportData, currentPLU_.itemNumber() + "," + QString::number( Nutrifacts ) );

                // HT-489
                // This was fixed in the trunk June 7,2017 but the remote print branch has this issue too and while
                // fixing it in remote print, I decided to fix it a little different than the trunk.  I'm guessing
                // when this was originally put in, and we are in accumulate items mode, we dont care to have to
                // fill out forced cool and even care about cool.  So I am ifing out the cool part but bringing in
                // or bringing back the very important Q_EMIT currentProductChanged that updates the PLU screen
                if ( OpModeAccumulateItems != weighModeTotalType->weighMode() )
                {
                    // request cool
                    waitingOnCool_ = (currentPLU_.forcedCool() || currentPLU_.coolRequired());

                    LOG_TRACE( "requesting cool (%d)...", waitingOnCool_ );
                    session_->send( COOLDev, COOLDevShortlistReadAllCoolText, currentPLU_.itemNumber() + ",1" );

                    // schedule timer to handle forced fields
                    if ( waitingOnCool_ )
                    {
                        timerResetWaitingOnCool_->setSingleShot( true );
                        timerResetWaitingOnCool_->start( missingCoolTimeout_ );
                    }
                }

                LOG_DEBUG( "onSessionMessage case 101,7 - update Operate Mode and EMIT currentProductChanged()", NULL );
                Q_EMIT currentProductChanged( false );
            }
        }
    }

    // plu support write
    else if (( ProductDev == device ) && ( ProductWriteSupportData == event ) && ( isOperateMode ))
    {
        DomPluSupport m( msg );

        ProductSupportType type = (ProductSupportType) m.textType();

        currentPLUSupportMap_[ type ] = m;

        MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

        // FT-1185
        // check for forced showing of fields
        if (( !isPrintingProductionEntry() ) && ( !isSelfService() ))
        {
            // check for product note
            if (( ProductNote == type ) && ( scaleConfig->autoDisplayProductNotes() ))
            {
                QString text( session_->uiUtilities->stripFontChanges( m.supportText() ) );

                needToShowProductNote_ = !text.isEmpty();
            }

            // check for suggestive sell
            if (( SuggestiveSell == type ) && ( scaleConfig->autoDisplaySuggestiveSelling() ))
            {
                const int frequency( qMax( 1, scaleConfig->suggestiveSellingFreq() ) );

                QString text( session_->uiUtilities->stripFontChanges( m.supportText() ) );

                if ( !text.isEmpty() )
                    if ( 0 == ((++suggestiveSellCounter_) % frequency) )
                        needToShowSuggestiveSell_ = true;
            }

            // check for cooking instructions
            if (( CookingInstructions == type ) && ( scaleConfig->autoDisplayCookingNotes() ))
            {
                QString text( session_->uiUtilities->stripFontChanges( m.supportText() ) );

                needToShowCookingInstructions_ = !text.isEmpty();
            }
        }

        Q_EMIT currentProductSupportChanged( type );

        // display text to operator
        if (( needToShowProductNote_ ) ||
            ( needToShowSuggestiveSell_ ) ||
            ( needToShowCookingInstructions_ ))
        {
            processForcedFields();
        }
    }

    // plu not on file
    else if (( ProductDev == device ) && ( ProductClearUI == event ))
    {
        DomProductClearUI m( msg );
        int i;

        bool isDialogActive       = session_->scaleConfig->cuttingTestDialogActive;
        bool isHistoryActive      = session_->scaleConfig->cuttingTestHistoryActive;
        bool isMeatSheetActive    = session_->scaleConfig->cuttingTestMeatSpecActive;

        if( isDialogActive || isHistoryActive || isMeatSheetActive )
        {
            // if we are in a cut test initial information, we don't want to clear anything
            Q_EMIT currentProductChanged( true );
            return;
        }

        // reset focus
        if ( isOperateMode )
            setFocusForPopupNumberPad();

        // clear out previous data
        currentPLUSupportMap_.clear();

        for ( i = 0; i < currentPLU_.count(); ++i )
            currentPLU_[i] = "";

        for ( i = 0; i < currentPLUTotals_.count(); ++i )
            currentPLUTotals_[i] = "";

        waitingOnCool_ = false;
        waitingOnItemMessage_ = false;

        needToShowProductNote_ = false;
        needToShowSuggestiveSell_ = false;
        needToShowCookingInstructions_ = false;

        Q_EMIT currentProductChanged( true );

        // return to home screen (unless we are moving widgets around)
        // FT-326: do not navigate home when logging into supervisor mode (they may move stuff around)
        if (( OpModeOperate == operationMode() ) && ( m.returnToStartScreen() ))
        {
            if (( widgetDragLock() ) && ( widgetSettingsLock() ))
            {
                if( mainWindow_->getIsHeadingToHome() )                // dont do it if we have already done it
                    mainWindow_->setIsHeadingToHome( false );          // Reset it for next time
                else
                    mainWindow_->navigateHome();
            }
        }

        // if we clear the PLU, then we reset the background if PICK 5 ON and DONE BY PLU
        ScaleConfiguration *scaleConfiguration( session_->scaleConfig );

        bool isActive = scaleConfiguration->getIsPick5Active();
        if( isActive )
        {
            mainWindow_->togglePickFive( false );
            scaleConfiguration->setIsPick5Active( false );
        }

        // invoke hook
        // it seems like this locked up the UI leaving OperateMode (and calling ClearPlu in the background)
        // and then going to Supervisor Mode and we are trying to update self serv. No need to if not self serv
        if( scaleConfiguration->internalConfig.ssactive_lock )
        {
            // pjr383: use original
            // SOF-3995 #1 Big Y Self-Serve: Self-Serve "Home page" is inconsistent - always reset to default - flashkeys
            sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceKeyEntryStyle, "0");               // display flashkeys(0) over lone keypad(1)

            //LOG_WARN( "onSessionMessage - case 101,47 jhc LOCK UP TEST self serv so call updateCurrentItem()", NULL );
            sessionHooks::instance()->updateCurrentItem( sessionHooks::none );
        }
        //else
        //    LOG_WARN( "onSessionMessage - case 101,47 jhc LOCK UP TEST NOT self serv so SKIP call updateCurrentItem()", NULL );
    }

    // plu grand totals

    else if (( ProductDev == device ) && ( ProductWriteCurrentTotalValues == event ))
    {
        // HT-70 Total price not removed after switching modes
        // we get this coming out of Supervisor too to clear the PLU.  If we receive it, do it.
        currentPLUTotals_ = msg;

        Q_EMIT currentProductTotalsChanged();
    }

    // plu field
    else if (( ProductDev == device ) && ( ProductWriteSingleField == event ) && ( isOperateMode ))
    {
        DomUpdateRunModeField m( msg );

        bool process = true;

        // FT-777
        // Special case for tare entered with preset flashkey... if invalid the backend will
        // send a single field write rejection even though we did not send it a single field write
        // message. So ignore this message.
        if (( OffsetTare == m.offsetIndex() || OffsetTareB == m.offsetIndex() ) && // HT-309
            ( !m.isUpdateLegal() ) &&
            ( m.newValue().isEmpty() ) &&
            ( m.prevDomElem().isEmpty() ))
        {
            process = false;
        }

        // process message
        if ( process )
        {
            LOG_DEBUG( "UPDATE product offset %d to '%s' (previous '%s')",
                m.offsetIndex(),
                qPrintable( m.newValue() ),
                qPrintable( m.prevDomElem() ) );

            updateField( (ProductWriteSingleFieldOffset) m.offsetIndex(), m.newValue() );

            Q_EMIT currentProductUpdateField( m );
        }
    }

    // start production entry label run
    else if (( ProductDev == device ) && ( ProductStartPrintAllLabels == event ) && ( isOperateMode ))
    {
        // FT-1092
        if ( !printingProductionEntry_ && !currentPLU_.itemNumber().isEmpty()  )
        {
            PluMainScreen *pluMainScreen( isCached<PluMainScreen>() ); // FT-1114

            if ( pluMainScreen )
            {
                //pluMainScreen->showOperatorMessage(
                //    hobart::dynamicContentWidget::OperatorMessage,
                //    tr( "Printing production items...\n\nPress Cancel to stop." ),
                //    true );
                //PortionZeroDialog *dialog( new PortionZeroDialog( mainWindow_ ) );
                //dialog->open();
                ProductionEntryPrintingDialog *dialog( new ProductionEntryPrintingDialog( mainWindow_ ) );
                dialog->open();
            }

            printingProductionEntry_ = true;

            pluMainScreen->setPluDisabled( printingProductionEntry_ );           // disable the PLU input field
        }
    }

    // stop production entry label run
    else if (( ProductDev == device ) && (( ProductPausePrintAllLabels == event ) || ( ProductStopPrintAllLabels == event ) || ( ProductFinishedPrintingAllLabels == event )) && ( isOperateMode ))
    {
        // FT-1092
        if ( printingProductionEntry_ )
        {
            PluMainScreen *pluMainScreen( isCached<PluMainScreen>() ); // FT-1114

            if ( pluMainScreen )
                pluMainScreen->hideOperatorMessage();

            printingProductionEntry_ = false;

            pluMainScreen->setPluDisabled( printingProductionEntry_ );
        }
    }

    // printing labels (self service)
    else if (( ProductDev == device ) && ( ProductPrint == event ) && ( isOperateMode ))
    {
        //LOG_DEBUG( "operatemode() RECEIVED 101,4 message", NULL);

        MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );
        ProductPrintRequestUpdateMode m( msg );

        // Pressed Print Label on Print or Scan Screen (Finish Screen)
        if ( session_->scaleConfig->internalConfig.clickedPrintLabelFromFinishScreen && ( 1 != m.isAnimateMyPrint() ) )
        {
            // Clicked Print on TakeLabel Screen, need to reload the screen now - flag never turned on if not BigY Combined feature not enabled so no need to retest it
            session_->scaleConfig->internalConfig.clickedPrintLabelFromFinishScreen = false;
        }

        // JIRA SOF-3950- PJR-357 Big Y Self Service Modifications
        if( session_->scaleConfig->internalConfig.ssactive_lock )
        {
            // pjr383 - use original
            if( scaleConfig->selfServeCombineSearchKeypadFlashkeys() && (SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig->selfServeIgnorePrinterErrors()))
            {
                if (  "1" == sessionHooks::instance()->getFieldValue( sessionHooks::selfServicePrintDisplayingLabel ) && (1 != m.isAnimateMyPrint() ) )
                {
                    // means we are on the 2nd loop of the print and we want to reset everything
                    sessionHooks::instance()->updateFieldValue( sessionHooks::selfServicePrintDisplayingLabel, "0");

                    // TURN ON PrintToScreen in case we press the PRINT button next time we go to the print a label screen
                    //LOG_DEBUG( "operatemode() 101,4 TURN ON", NULL);
                    sessionHooks::instance()->endPrinting();                                // now set us back to reset screen
                    //LOG_DEBUG( "operatemode() 101,4 endPrinting() DONE", NULL);
                    return;
                }
            }
        }

        // PJR-82
        QString filename( m.bitmapToDisplay() );

        // if NOT amazonGo AND NOT Combine (printToScreen) but this also includes print, which print is print
        if ( !scaleConfig->selfServeDisplayLabelOnScreenGO() && !scaleConfig->selfServeCombineSearchKeypadFlashkeys() )
            filename.clear();

        // show printer page
        if ( 1 == m.isAnimateMyPrint() )
            sessionHooks::instance()->beginPrinting( filename );                // For QT DEV - I need to add a endPrinting() call like a second later to reset the state
        else if( 2 == m.isAnimateMyPrint() )
        {
            //LOG_DEBUG( "operatemode() RX: 101,4 Call barcode printing with TotalPrice: %s", qPrintable(currentPLUTotals_.totalPrice()) );
            sessionHooks::instance()->beginBarcodeOnlyPrinting( filename );
        }
        else
            sessionHooks::instance()->endPrinting();

        // JIRA SOF-3950- PJR-357 Big Y Self Service Modifications
        // we get this message back when we want to PRINT a label on a screen - not on normal option1 to just print
        // So we sent a print 100,25 to Applicaiton, and get this message back with the label image. In case we want
        // to print, we need to TURN OFF print the label to the screen again
        // if we have the combination print/scan option turned on, we can now
        if( session_->scaleConfig->internalConfig.ssactive_lock )
        {
            // pjr383 - use original
            if( scaleConfig->selfServeCombineSearchKeypadFlashkeys() && (SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig->selfServeIgnorePrinterErrors()))
            {
                // TURN OFF PrintToScreen in case we press the PRINT button
                LOG_DEBUG( "operatemode() 101,4 TURN OFF", NULL);
            }
        }

        //LOG_DEBUG( "operatemode() RECEIVED 101,4 message DONE", NULL);
    }

    // cool
    else if (( COOLDev == device ) && ( COOLDevShortlistWriteAllCoolText == event ) && ( isOperateMode ))
    {
        waitingOnCool_ = false;

        // Note:
        // When a new item is entered into the system we request cool and other fields. We do
        // not process forced fields while waiting on cool data (because cool is the very
        // first field we force). So now that we have cool data lets try and process those
        // forced fields again...
        processForcedFields();
    }

    // print key pressed
    else if (( SessionDev == device ) && ( SnPrintKeyPressed == event ))
    {
        QString itemNumber( currentPLU_.itemNumber() );

        // reject print
        if ( !isPrintingAllowed() )
        {
            LOG_INFO( "SnPrintKeyPressed: rejecting print bc isPrintAllowed = false", NULL );
            session_->send( ProductDev, ProductWriteSingleField, itemNumber + ",0,0,0,0" );
        }

        // look for field updates
        else
        {
            bool allFieldsCurrent( true );

            if( (session_->isPrepack() && mainWindow_->isDialogOpen<SessionErrorDialog>() && mainWindow_->getErrorCode() == 5010 ) )
            {
                LOG_INFO( "SnPrintKeyPressed: changing print bc allFieldsCurrent is false in overrun error", NULL );
                allFieldsCurrent = true;
            }
            else
            {
                PluMainScreen *pluMainScreen( dynamic_cast<PluMainScreen*>( screen() ) );

                // for main screen, verify no field being edited at the moment
                if ( pluMainScreen )
                {
                    LOG_TRACE( "We are on the PluMainScreen", NULL );
                    if ( session_->isPrepack() && mainWindow_->isDialogOpen<WrapperModeDialog>() )
                    {
                        LOG_INFO( "The WrapperModeDialog is up. Carry on and print.  allFieldsCurrent = %d", allFieldsCurrent );
                    }
                    else
                    {
                        allFieldsCurrent = pluMainScreen->pluHasFocus();

                        // for popup numberpad, will be visible when editing a field
                        if ( pluMainScreen->isPopupNumberPad() )
                            if ( !mainWindow_->numberPad()->isVisible() )
                                allFieldsCurrent = true;
                    }
                }
                else if ( session_->isPrepack() )		//	HA-201
                {
                    WrapperSettingsScreen *wrapperSettingsScreen( dynamic_cast<WrapperSettingsScreen*>( screen() ) );
                    LabelPlacementScreen *labelPlacementScreen( dynamic_cast<LabelPlacementScreen*>( screen() ) );

                    LOG_INFO( "wrapperSettingsScreen = %08X,  labelPlacementScreen = %08X", wrapperSettingsScreen, labelPlacementScreen );

                    if ( wrapperSettingsScreen || labelPlacementScreen )
                    {
                        QString screenName((wrapperSettingsScreen ? "WrapperSettingsScreen" : "LabelPlacementScreen"));
                        LOG_INFO( "We are on the %s", qPrintable(screenName) );

                        if ( mainWindow_->numberPad()->isVisible() )
                        {
                            if (labelPlacementScreen || (wrapperSettingsScreen && (wrapperSettingsScreen->beltTempSettingHasFocus() == false)))
                                allFieldsCurrent = false;
                        }
                    }
                }
            }

            // indicate we have no more fields to send
            if ( allFieldsCurrent )
            {
                LOG_INFO( "SnPrintKeyPressed: allFieldsCurrent, sending productWriteSingleField", NULL );
                session_->send( ProductDev, ProductWriteSingleField, itemNumber + ",0,0,0,1" );
            }
            else
            {
                LOG_INFO( "updating field for print", NULL );

                // this will send single field write for currently focused field
                setFocusForPopupNumberPad();
            }

            // request best sellers
            if ( refreshBestSellers_ )
            {
                LOG_INFO( "SnPrintKeyPressed: requesting best sellers", NULL );
                session_->send( SoftkeyDev, ExecuteSoftkey, "42,," + QString::number( hobart::softkeysWidget::MostCommonlyUsedItems ) );
                refreshBestSellers_ = false;
            }
        }
    }

    // operator id required
    else if (( OperatorDev == device ) &&
        (( OpPromptForOperatorIDOrRFID == event ) || ( OpPromptForOperatorIDOrRFIDFieldAccess == event )))
    {

        ScaleConfiguration *scaleConfig = session_->scaleConfig;
        OperatorLoginDialog::LoginType loginType = OperatorLoginDialog::OperatorLoginRequired;

        if ( scaleConfig->askOperatorToLoginToStartCutTest )
        {
            bool areDialogsTurnedOff = scaleConfig->getAreDialogsTurnedOff();
            if( areDialogsTurnedOff )
                return;

            loginType = OperatorLoginDialog::OperatorLoginRequiredForCutTest;
        }
        else if ( OpPromptForOperatorIDOrRFIDFieldAccess == event )
            loginType = OperatorLoginDialog::OperatorLoginRequiredFieldAccess;

        OperatorLoginDialog *d = new OperatorLoginDialog( loginType, mainWindow_ );

        operatorLoggedInWidget_ = NULL;

        // show operator id login dialog
        d->open();
    }

    // weigher status (i.e. weight)
    else if (( Weigher == device ) && ( WgStatus == event ))
    {
        const MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

        MadpWeightData m( msg );

        // reset flag
        if ( m.zeroFlag() )
            isStableNonZeroWeight_ = false;

        // FT-1062
        // when switching from zero to non-zero stable weight on left screen we need to switch
        // to center panel (run screen)
        else if (( isOperateMode ) && ( scaleConfig->switchToRunScreenOnStableWeight() ) && ( !isStableNonZeroWeight_ ) && ( 0.0 < minWeightToPrint_ ))
        {
            WebBrowserScreen *webBrowserScreen( dynamic_cast<WebBrowserScreen*>( screen() ) );

            if (( webBrowserScreen ) && ( minWeightToPrint_ <= m.netWeight().toDouble() ))
                if ( mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>() )
                    isStableNonZeroWeight_ = true;
        }
    }

    // scale setup
    else if (( Weigher ==device ) && ( WgInquire == event ))
    {
        DomWeigherConfigRead m( msg );

        // FT-1062
        // keep track of minimum weight to print
        minWeightToPrint_ = m.weigherMininumWt().toDouble();
    }

    // configuration
    else if (( ConfigurationDev == device ) && ( CfgPrepackWrite == event ))
    {
        MadpOperatorDisplayFields *operatorDisplayFields = &session_->scaleConfig->operatorDisplayFields;

        CapacityBarArea capacityBarLocation = TopCapacityBarArea;
        Qt::DockWidgetArea keypadLocation = Qt::RightDockWidgetArea;

        if ( 1 == operatorDisplayFields->capacityBarLocation() )
            capacityBarLocation = BottomCapacityBarArea;

        if ( 1 == operatorDisplayFields->keypadLocation() )
            keypadLocation = Qt::LeftDockWidgetArea;

        // move widget if different
        if ( capacityBarLocation != capacityBarArea() )
            setCapacityBarArea( capacityBarLocation );

        // move widget if different
        if ( keypadLocation != mainWindow_->numberPadArea() )
            mainWindow_->setNumberPadArea( keypadLocation );
    }

    // operator id existance
    else if (( OperatorDev == device ) && ( OpIDExists == event ) )
    {
        //LOG_DEBUG( "(OperatorDev,OpIDExists) UPDATE" , NULL );

        if( selfServiceOperatorRequest_ )
        {
            selfServiceOperatorRequest_ = false;

            // pulse the self service key to unlock access
            sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceKey, true );
            sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceKey, false );
        }
    }

    // operator id existance
    else if (( OperatorDev == device ) && ( OpIDDoesntExist == event ) )
    {
        //LOG_DEBUG( "(OperatorDev,OpIDExists) UPDATE where selfServiceOperatorRequest_ = %d" , selfServiceOperatorRequest_ );

        if( selfServiceOperatorRequest_ )
            selfServiceOperatorRequest_ = false;
    }

    else if( ( OperatorDev == device ) && ( OpLogin == event ) )
    {
        //LOG_DEBUG( "(OperatorDev,OpLogin) UPDATE where selfServiceOperatorRequest_ = %d" , selfServiceOperatorRequest_ );

        if( selfServiceOperatorRequest_ )
        {
            // selfServiceOperatorRequest_ is true when loggin in an operator in Self Service mode
            selfServiceOperatorRequest_ = false;

            // since the operator exists, then unlock the screen and proceed
            sessionHooks::instance()->updateStartState();
        }
        else
        {
            //LOG_DEBUG( "(BarcodeScanner, ScannerReadProduct) we have a possible hands free login", NULL );

            // didnt use the scanner so selfserviceoperatorrequest
            // is false so need to check if it have a hands free login
            bool selfserv_state  = session_->scaleConfig->internalConfig.ssactive_lock;

            LOG_DEBUG( "(BarcodeScanner, ScannerReadProduct) are we in self service? %d", selfserv_state );

            if( selfserv_state )
            {
                // in self serv, now see if we are a handsfree login
                if ( session_->uiUtilities->isFeatureLicensed( SCALE_HANDSFREEDEVICE ) )
                {
                    //LOG_DEBUG( "(BarcodeScanner, ScannerReadProduct) we are SCALE_HANDSFREEDEVICE licensed", NULL);

                    DomOperatorLoginData m( msg );
                    QString value = m.operatorRFID();

                    //LOG_DEBUG( "(BarcodeScanner, ScannerReadProduct) we might have an operator so log them in: %s" , qPrintable( value ) );

                    if( 0 < value.length() )
                    {
                        // since the operator of some RFID value exists, then unlock the screen and proceed
                        sessionHooks::instance()->updateStartState();
                    }
                }
            }
        }
    }
    else if ( ( BarcodeScanner == device ) && ( ScannerReadProduct == event ) )
    {
        LOG_DEBUG( "(BarcodeScanner, ScannerReadProduct) onSessionMessage() update", NULL );

        DomNewScannedCodeReadPlu m( msg );
        QString value = m.itemNumber();

        const MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );
        int start_state = scaleConfig->selfServeStartScreenState();
        bool log_state  = session_->scaleConfig->getIsOperatorLoggedIn();

        LOG_DEBUG( "(BarcodeScanner, ScannerReadProduct) where start_state=%d, log_state=%d, value=%s" , start_state, log_state, qPrintable( value ) );

        // self service operator SCANNED login for pickers, or any operator that exists, unlocks the screen
        // if login operators in Self Serv is enabled but no operator is currently logged in, then allow it
        if( SelfServicePrintOption::SelfServOption_StartOperLogin == start_state && !log_state )
        {
            if ( value.length() )
            {
                selfServiceOperatorRequest_ = true;
                session_->send( OperatorDev, OpLogin, value + ",,,,,,1" );
            }
        }
    }
    // operator error
    else if (( Error == device ) && ( ErrOperatorDevice == event ))
    {
        returnedErrorsOperatorDev m( msg );

        // unknown operator id
        if ( 102001 == m.errorCode() )
            operatorLoggedInWidget_ = NULL;
    }

    // load all softkey groups
    else if (( SoftkeyDev == device ) && ( LoadAllSoftkeyGroups == event ))
    {
        ScaleConfiguration *scaleConfig = session_->scaleConfig;
        updateSoftkeyGroups();
        scaleConfig->internalConfig.selfServicePreventClicks = false;
    }
    else if ( ( SoftkeyDev == device) && ( LoadSoftkeyGroup == event ) )
    {
        ScaleConfiguration *scaleConfig = session_->scaleConfig;
        PluMainScreen *pluMainScreen( dynamic_cast<PluMainScreen*>( screen() ) );

        // Reset the Main Screen Flashkeys to 999003
        if ( pluMainScreen && scaleConfig->meatSheetScreenTimedOut )
        {
            pluMainScreen->setSpecialFlasheyWidget( false );  // hide the cut test run screen
            scaleConfig->meatSheetScreenTimedOut = false;
        }
        scaleConfig->internalConfig.selfServicePreventClicks = false;
    }
    else if(LabelApplier == msg[0].toInt() && LaSysInfo == msg[1].toInt())
    {
        DomLabelApplierSystemInformation m(msg);

        if( m.awsModelNumber() == 3 || m.awsModelNumber() == 2 )
        {
            session_->scaleConfig->isCLA_ = true;
        }
        else
        {
            session_->scaleConfig->isCLA_ = false;
        }
    }
/* No longer use this
    // 9,94, ngwState => enum { PAUSE,RESUME,REJECT}, primaryPlatePosition, requiredPrimaryPlatePosition, secondaryPlatePosition, requiredSecondaryPlatePosition
    else if( Wrapper == device && WrPause == event )
    {
        LOG_INFO("WrPause received", NULL);

        // getting information in inches, but position screen uses mm so need to convert these to mm
        primaryPlatePosition_ = msg[4].toInt();
        requiredPrimaryPlatePosition_ = msg[5].toInt();
        secondaryPlatePosition_ = msg[6].toInt();
        requiredSecondaryPlatePosition_ = msg[7].toInt();

        packageId_ = msg[3].toInt();

        // pause was sent
        // We no longer pop the printer plate positioning screen


        LOG_INFO( "WrPause message handle end - No longer POP printer plate POS dialog", NULL );
    }
*/
    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::shutdownKeyboards()
{
    // go for it all LOL
    mainWindow_->hideKeyboard();
    mainWindow_->hideNumberPad();

    QApplication::processEvents();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::onOperatorLoggedIn()
{
    LOG_CLASS_INFO( "OperateMode", "onOperatorLoggedIn" );

    QWidget *widget( operatorLoggedInWidget_ );

    if ( widget )
    {
        LOG_TRACE( "setting focus to log in widget 0x%08x [%s]", widget, qPrintable( widget->objectName() ) );
        widget->setFocus();
    }

    operatorLoggedInWidget_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::onProcessForcedFields()
{
    LOG_CLASS_INFO( "OperateMode", "onProcessForcedFields" );

    // check for product note
    if ( needToShowProductNote_ )
    {
        showProductNote( ProductNote );
        needToShowProductNote_ = false;
        return;
    }

    // check for suggestive sell
    if ( needToShowSuggestiveSell_ )
    {
        showProductNote( SuggestiveSell  );
        needToShowSuggestiveSell_ = false;
        return;
    }

    // check for cooking instructions
    if ( needToShowCookingInstructions_ )
    {
        showProductNote( CookingInstructions  );
        needToShowCookingInstructions_ = false;
        return;
    }

    PluMainScreen *mainScreen( isCached<PluMainScreen>() );

    if ( mainScreen )
        if ( mainScreen->isForcedFields() )
        {
            // make sure we are on proper screen!
            if ( mainScreen != screen() )
                mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();

            LOG_TRACE( "processing forced fields on plu main screen", NULL );
            mainScreen->processForcedFields();
            return;
        }

    if ( weightBar_ )
        if ( weightBar_->isForcedFields() )
        {
            // make sure we are on proper screen!
            if ( mainScreen != screen() )
                mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();

            LOG_TRACE( "processing forced fields on weight bar", NULL );
            weightBar_->processForcedFields();
            return;
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::onResetWaitingOnItemMessage()
{
    LOG_CLASS_INFO( "OperateMode", "onResetWaitingOnItemMessage" );

    // reset waiting item message
    if ( waitingOnItemMessage_ )
    {
        LOG_WARN( "did not receive item message in a timely manner!", NULL );
        waitingOnItemMessage_ = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::onTimerToggleFlashkeyEnable()
{
    mainWindow_->setEnabled( true );            // just in case. I dont want to lock out the user
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::onResetWaitingOnCool()
{
    LOG_CLASS_INFO( "OperateMode", "onResetWaitingOnCool" );

    // reset waiting cool
    if ( waitingOnCool_ )
    {
        LOG_WARN( "did not receive cool in a timely manner!", NULL );
        waitingOnCool_ = false;

        processForcedFields();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::initialize()
{
    // set object name
    if ( objectName().isEmpty() )
        setObjectName( "OperateMode" );

    // set font
    QFont f = font();
    f.setPointSize( 9 );
    setFont( f );

    // set palette
    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white, hobart::colorPalette::white ) );
    setForeground( this );

    // Randy - hook for NEW DELI VISION license
    // SCALE_ENHANCED_CENTERSCREEN

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        // cache frequently used screens (create in order of usage, most used first)
        bool allowWebBrowserScreen( true );

        // HTs
        // this feature is disabled
        if (( !session_->uiUtilities->isFeatureLicensed( SCALE_WEB_BROWSER ) ) &&
            ( !session_->uiUtilities->isFeatureLicensed( SCALE_FULLSCREEN_FLASHKEYS ) ))
        {
            allowWebBrowserScreen = false;
        }
        // FT-1323: option to remove left screen
        else if ( session_->scaleConfig->scaleConfig.disableStartScreen() )
        {
            allowWebBrowserScreen = false;
        }

        cache( createScreen<PluMainScreen>() );
        cache( createScreen<PluInfoScreen>() );

        if ( allowWebBrowserScreen )
            cache( createScreen<WebBrowserScreen>() );

        cache( createScreen<AppsScreen>() );

        // FT-1695: build-a-meal
        if ( session_->uiUtilities->isFeatureLicensed( SCALE_MEAL_BUILDER ) )
            cache( createScreen<MealBuilderScreen>() );

        // Meat Specification Sheet Screen
        cache( createScreen<MeatSheetScreen>() );
    }

    // I need to know this information so I can display fixed columns and rows
    // ( 136,93,999020)
    session_->send( SoftkeyDev, LoadSoftkeyGroup, QString::number( hobart::softkeysWidget::FixedRowColumnGrid ) );      // On boot do initialize()

    // create capacity and weight bars
    setupBars( TopCapacityBarArea, TopWeightBarArea );

    // create timers
    timerDeleteWidgets_ = new QTimer( this );
    connect( timerDeleteWidgets_, SIGNAL( timeout() ), SLOT( onDeleteWidgets() ) );

    timerProcessForcedFields_ = new QTimer( this );
    connect( timerProcessForcedFields_, SIGNAL( timeout() ), SLOT( onProcessForcedFields() ) );

    timerResetWaitingOnCool_ = new QTimer( this );
    connect( timerResetWaitingOnCool_, SIGNAL( timeout() ), SLOT( onResetWaitingOnCool() ) );

    timerResetWaitingOnItemMessage_ = new QTimer( this );
    connect( timerResetWaitingOnItemMessage_, SIGNAL( timeout() ), SLOT( onResetWaitingOnItemMessage() ) );

    timerToggleFlashkeyEnable_ = new QTimer();
    connect( timerToggleFlashkeyEnable_, SIGNAL( timeout() ), SLOT( onTimerToggleFlashkeyEnable() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::updateField( ProductWriteSingleFieldOffset off, const QString& value )
{
    switch ( off )
    {
    // weight bar
    case OffsetBarcodeProductNum:
        currentPLU_.productNumber() = value; break;
    case OffsetBarcodeVendorNum:
        currentPLU_.vendorNumber() = value; break;
    case OffsetByCount:
        currentPLU_.byCountRaw() = value; break;
    case OffsetExceptionPrice:
        currentPLU_.exceptionPrice() = value; break;
    case OffsetUnitPrice:
        currentPLU_.unitPrice() = value; break;
    case OffsetTare:
        currentPLU_.tare() = value; break;
    case OffsetTareB:   // HT-309
        currentPLU_.tareB() = value; break;
    case OffsetFixedWeight:
        currentPLU_.fixedWeight() = value; break;
    case OffsetFractionalPriceUnit:
        currentPLU_.fractionalPriceUnitRaw() = value; break;
    case OffsetPoundsForAmount:
        currentPLU_.poundsForAmountRaw() = value; break;

    // main plu screen
    case OffsetShelfLifeDays:
        currentPLU_.shelfLifeDaysRaw() = value; break;
    case OffsetProductLifeDays:
        currentPLU_.productLifeDaysRaw() = value; break;
    case OffsetBackDateDays:
        currentPLU_.backDateDaysRaw() = value; break;
    case OffsetDoNotPrintPackedOn:
        currentPLU_.dontPrintPackedOnRaw() = value; break;
    case OffsetDoNotPrintSellBy:
        currentPLU_.dontPrintSellByRaw() = value; break;
    case OffsetDoNotPrintBestBefore:
        currentPLU_.dontPrintBestBeforeRaw() = value; break;
    case OffsetSellByDate:
        currentPLU_.sellByDateRaw() = value; break;
    case OffsetBestBeforeDate:
        currentPLU_.bestBeforeDateRaw() = value; break;
    case OffsetCoolTextNum:
        currentPLU_.coolTextNumRaw() = value; break;
    case OffsetForcedCool:
        currentPLU_.forcedCoolRaw() = value; break;
    case OffsetCoolTrackingText:
        currentPLU_.coolTrackingTextRaw() = value; break;
    case OffsetCoolRequired:
        currentPLU_.coolRequiredRaw() = value; break;
    case OffsetKeyboardNetWeight:
        currentPLU_.keyboardNetWeight() = value; break;
    case OffsetAcceptKbdNetWeight:
        currentPLU_.acceptKbdNetWeightRaw() = value; break;
    case OffsetNumOfLabelsForced:
        currentPLU_.numOfLabelsForcedRaw() = value; break;
    case OffsetNumOfLabels:
        currentPLU_.numOfLabelsRaw() = value; break;
    case OffsetExpirationDays:
        currentPLU_.expirationDaysRaw() = value; break;
    case OffsetPackedOnDate:
        currentPLU_.backdateDateRaw() = value; break;

    // plu info screen
    case OffsetShelfLifeHours:
        currentPLU_.shelfLifeHoursRaw() = value; break;
    case OffsetProductLifeHours:
        currentPLU_.productLifeHoursRaw() = value; break;
    case OffsetPrimaryLabelType:                              // Not using these anymore at all
        currentPLU_.primaryLabelTypeRaw() = value; break;
    case OffsetSecondLabelType:
        currentPLU_.secondLabelTypeRaw() = value; break;
    case OffsetThirdLabelType:
        currentPLU_.thirdLabelTypeRaw() = value; break;
    //case OffsetDefaultLabelTypeNum1:
    //    currentPLU_.primaryDefaultLabelTypeRaw() = value; break;
    //case OffsetDefaultLabelTypeNum2:
    //    currentPLU_.secondDefaultLabelTypeRaw() = value; break;
    //case OffsetDefaultLabelTypeNum3:
    //    currentPLU_.thirdDefaultLabelTypeRaw() = value; break;
    //case OffsetManualLabelTypeNum1:
    //    currentPLU_.primaryManualLabelTypeRaw() = value; break;
    //case OffsetManualLabelTypeNum2:
    //    currentPLU_.secondManualLabelTypeRaw() = value; break;
    //case OffsetManualLabelTypeNum3:
    //    currentPLU_.thirdManualLabelTypeRaw() = value; break;
    //case OffsetPrepackLabelTypeNum1:
    //    currentPLU_.primaryPrepackLabelTypeRaw() = value; break;
    //case OffsetPrepackLabelTypeNum2:
    //    currentPLU_.secondPrepackLabelTypeRaw() = value; break;
    //case OffsetPrepackLabelTypeNum3:
    //    currentPLU_.thirdPrepackLabelTypeRaw() = value; break;
    //case OffsetProdEntryLabelTypeNum1:
    //    currentPLU_.primaryProdEntryLabelTypeRaw() = value; break;
    //case OffsetProdEntryLabelTypeNum2:
    //    currentPLU_.secondProdEntryLabelTypeRaw() = value; break;
    //case OffsetProdEntryLabelTypeNum3:
    //    currentPLU_.thirdProdEntryLabelTypeRaw() = value; break;
    //case OffsetSelfServLabelTypeNum1:
    //    currentPLU_.primarySelfServLabelTypeRaw() = value; break;
    //case OffsetSelfServLabelTypeNum2:
    //    currentPLU_.secondSelfServLabelTypeRaw() = value; break;
    //case OffsetSelfServLabelTypeNum3:
    //    currentPLU_.thirdSelfServLabelTypeRaw() = value; break;
    //case OffsetPick5LabelTypeNum1:
    //    currentPLU_.primaryPick5LabelTypeRaw() = value; break;
    //case OffsetPick5LabelTypeNum2:
    //    currentPLU_.secondPick5LabelTypeRaw() = value; break;
    //case OffsetPick5LabelTypeNum3:
    //    currentPLU_.thirdPick5LabelTypeRaw() = value; break;

    case OffsetGraphic1Name:
        currentPLU_.graphic1Name() = value; break;
    case OffsetGraphic2Name:
        currentPLU_.graphic2Name() = value; break;
    case OffsetGraphic3Name:
        currentPLU_.graphic3Name() = value; break;
    case OffsetGraphic4Name:
        currentPLU_.graphic4Name() = value; break;
    case OffsetPrimaryLabelTypeWidth:
        currentPLU_.primaryLabelTypeWidth() = value; break;
    case OffsetPrimaryLabelTypeLength:
        currentPLU_.primaryLabelTypeLength() = value; break;
    case OffsetSecondLabelTypeWidth:
        currentPLU_.secondLabelTypeWidth() = value; break;
    case OffsetSecondLabelTypeLength:
        currentPLU_.secondLabelTypeLength() = value; break;
    case OffsetThirdLabelTypeWidth:
        currentPLU_.thirdLabelTypeWidth() = value; break;
    case OffsetThirdLabelTypeLength:
        currentPLU_.thirdLabelTypeLength() = value; break;

    // label placement screen
    case OffsetPrimaryLabelPlacement:
        currentPLU_.primaryLabelPlacementRaw() = value; break;
    case OffsetPrimaryLabelRotation:
        currentPLU_.primaryLabelRotationRaw() = value; break;
    case OffsetSecondLabelPlacement:
        currentPLU_.secondLabelPlacementRaw() = value; break;
    case OffsetSecondLabelRotation:
        currentPLU_.secondLabelRotationRaw() = value; break;
    case OffsetThirdLabelPlacement:
        currentPLU_.thirdLabelPlacementRaw() = value; break;
    case OffsetThirdLabelRotation:
        currentPLU_.thirdLabelRotationRaw() = value; break;

    case OffsetShelfLifeMinutes:
        currentPLU_.shelfLifeMinutesRaw() = value; break;
    case OffsetProductLifeMinutes:
        currentPLU_.productLifeMinutesRaw() = value; break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::updateSoftkeyGroups()
{
    PluMainScreen *pluMainScreen = NULL;
    WebBrowserScreen *webBrowserScreen = NULL;
    ScaleConfiguration *config( session_->scaleConfig );

    // plu main screen
    if ( (pluMainScreen = isCached<PluMainScreen>()) )
    {
        config->internalConfig.updateSelfServiceFlashkeys = true;
        pluMainScreen->resetFlashkeys( true );
    }

    // web browser screen
    if ( (webBrowserScreen = isCached<WebBrowserScreen>()) )
        webBrowserScreen->resetFlashkeys( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::showProductNote( ProductSupportType supportType )
{
    LOG_CLASS_INFO( "OperateMode", "showProductNote" );

    DomPluSupport note( currentPLUSupportMap_[ supportType ] );

    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    QString title;
    QString text( session_->uiUtilities->stripFontChanges( note.supportText() ) );
    ProductNoteStyle style( NoButtons );

    // validate text
    if ( !text.length() )
    {
        LOG_DEBUG( "no support text for type %d", supportType );
        return;
    }

    // dialog title
    switch ( supportType )
    {
    case Allergens:
        title = tr( "Allergens" );
        break;
    case CookingInstructions:
        title = tr( "Cooking Instructions" );
        break;
    case ExpandedText:
        title = tr( "Expanded Text" );
        break;
    case ExpandedText2:
        title = tr( "Expanded Text 2" );
        break;
    case Marquee:
        title = tr( "Marquee" );
        break;
    case Nutrifacts:
        title = tr( "Nutrifacts" );
        break;
    case ProductNote:
        title = tr( "Product Note" );
        break;
    case Recipe:
        title = tr( "Recipe" );
        break;
    case SpecialMessage:
        title = tr( "Special Message" );
        break;
    case SuggestiveSell:

        if ( 1 == scaleConfig->suggestiveSellingStyle() )
            style = OkayButton;
        else if ( 2 == scaleConfig->suggestiveSellingStyle() )
            style = YesNoButtons;

        title = tr( "Suggestive Sell" );
        break;
    }

    // validate title
    if ( !title.length() )
    {
        LOG_DEBUG( "no title text for type %d", supportType );
        return;
    }

    // create dialog
    ProductNoteDialog *dialog = new ProductNoteDialog( mainWindow_ );
    dialog->setTitle( title );
    dialog->setText( text );

    if ( OkayButton == style )
        dialog->addButton( tr( "Okay" ), QDialogButtonBox::AcceptRole );
    else if ( YesNoButtons == style )
    {
        dialog->addButton( tr( "Yes" ), QDialogButtonBox::AcceptRole );
        dialog->addButton( tr( "No" ), QDialogButtonBox::RejectRole );

        dialog->setAllowOverlayClickDismiss( false );
    }

    // show dialog!
    dialog->open();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::showPrinterSettings()
{
    PrinterSettingsDialog *dialog = new PrinterSettingsDialog( mainWindow_ );

    // show dialog!
    dialog->open();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateMode::setupBars( CapacityBarArea capacityArea, WeightBarArea weightArea )
{
    oldCapacityBar_ = capacityBar_;
    oldWeightBar_ = weightBar_;

    // force weight bar to top when capacity bar at top
    if ( TopCapacityBarArea == capacityArea )
        weightArea = TopWeightBarArea;

    // create new bars
    capacityBar_ = createCapacityBar( capacityArea );

    if( !( session_->uiUtilities->isWrapOnly() ) )
        weightBar_ = createWeightBar( weightArea );

    // reset layout margins
    if ( layout() )
        layout()->setContentsMargins( overlayMargins() );

    // connect slots
    connect( capacityBar_, SIGNAL( moved(CapacityBarArea) ), this, SLOT( setCapacityBarArea(CapacityBarArea) ) );

    if( !( session_->uiUtilities->isWrapOnly() ) )
        connect( weightBar_, SIGNAL( moved(WeightBarArea) ), this, SLOT( setWeightBarArea(WeightBarArea) ) );

    if( !( session_->uiUtilities->isWrapOnly() ) )
        // set z-order
        weightBar_->stackUnder( capacityBar_ );

    if ( screen_ )
        screen_->lower();

    // delete the old bars
    if ( oldCapacityBar_ )
    {
        oldCapacityBar_->hide();

        timerDeleteWidgets_->setSingleShot( true );
        timerDeleteWidgets_->start( 0 );
    }

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        if ( oldWeightBar_ )
        {
            oldWeightBar_->hide();

            timerDeleteWidgets_->setSingleShot( true );
            timerDeleteWidgets_->start( 0 );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CapacityBarWidget *OperateMode::createCapacityBar( CapacityBarArea area )
{
    CapacityBarWidget *bar = new CapacityBarWidget( mainWindow_, this, area );

    // move bar to bottom
    if ( BottomCapacityBarArea == area )
        bar->move( 0, mainWindow_->height() - bar->height() );

    // show capacity bar
    bar->show();

    return bar;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WeightBarWidget *OperateMode::createWeightBar( WeightBarArea area )
{
    WeightBarWidget *bar = new WeightBarWidget( mainWindow_, this, area );

    // move bar to proper location
    QMargins margins = capacityBarMargins();

    if ( TopWeightBarArea == area )
        bar->move( 0, margins.top() );
    else
        bar->move( 0, height() - bar->height() - margins.bottom() );

    // set weight bar visibility
    if ( weightBarVisible_ )
        bar->show();
    else
        bar->hide();

    return bar;
}
