/**
 * @file selfservicedialogG2.cpp
 *
 * @copyright Copyright (c) 2023 Hobart Corporation
 * @author Joe Crew
 * @borrowed from Randy Blankley
 */

#include "selfservicedialogG2.h"

#include "selfserveStartG2.h"
#include "selfserveInstructG2.h"
#include "selfserveItemLookupG2.h"
#include "selfserveImageLookupG2.h"
#include "selfserveImageLookupViewAllG2.h"
#include "selfserveWeightsMeasuresG2.h"
#include "selfserveAuditLogG2.h"
#include "selfserveRandomWeightG2.h"
#include "selfserveByCountG2.h"

#include "selfserveDialogG2.h"

#include <backgroundwidgettypes.h>
#include <filesystem.h>
#include <logmanager.h>
#include <selfservicehook.h>

#include <Session.h>
#include <Messages.h>
#include <softkeyswidget.h>
#include <ScaleConfiguration.h>
#include <ScaleClasses.h>

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>

#include "../../source/HFC/CfgBaseFieldOffsets.h"

const QColor selfServiceDialogG2::offlineBackgroundColor_( hobart::colorPalette::red );
const QColor selfServiceDialogG2::onlineBackgroundColor_( QColor( 244, 247, 250 ) );

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceDialogG2::selfServiceDialogG2( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    hook_( h ),
    scaleConfig_(NULL),
    online_( true ),
    networkFlipped_( false )
{
    // setup dialog
    setObjectName( "selfServiceDialogG2" );
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint );

    // initialize
    initialize();

    // connect signals/slots
    connect( hook_, SIGNAL( currentStateChanged() ), SLOT( onHookCurrentStateChanged() ) );
    connect( hook_, SIGNAL( currentLookOfStateChanged() ), SLOT( onHookCurrentLookOfStateChange() ) );
    connect( hook_, SIGNAL( reloadImages() ), SLOT( onHookReloadImages() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceDialogG2::~selfServiceDialogG2()
{
    if (scaleConfig_ )
        delete scaleConfig_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::loadSoftkeyGroup( const DomLoadSoftkeyGroup *m )
{
    itemEntryKeyed_->loadSoftkeyGroup( m );         // Best Sellers on main ss screen
    itemEntryImages_->loadSoftkeyGroup( m );        // Char by Char

    itemEntryImagesViewAll_->loadSoftkeyGroup( m ); // A-Z
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::renderBarcodeOnlyLabel()
{
    if( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() )
    {
        selfServiceHook::state s( hook_->currentState() );


        if( s == selfServiceHook::processRandomWeightItemGen2 )
        {
            randomWeightItem_->renderBarcodeOnlyLabel();

            QString netWeightField( hook_->fieldValue( sessionHooks::netWeight ) );
            netWeightField.replace(" kg", "");      // get rid of it if its present
            netWeightField.replace(" lb", "");

            if( netWeightField.contains( "----" ) || netWeightField.isEmpty() )
                return;

            double wgt = netWeightField.toDouble();
            

            if( ( 0 == netWeightField.length() ) ||  ( wgt <= 0 ) ) 
                return;
            else if( wgt > 0 )
            {
                // if the weight is now > 0, go ahead and kill off the dialog for them
                // we only want to close off the dialogs that deal with the weights < 0 and rezero dialogs
                if( "0" == sessionHooks::instance()->getFieldValue( sessionHooks::selfServiceRestrictedDialog ) )
                {
                    if( autoCloseDialogOnWeight( errorDialog_->getErrorCode().toInt() ) )
                    {
                        // only close when we are not required to stay open.
                        errorDialog_->closeDialog("1");
                        scaleConfig_->internalConfig.selfServicePreventClicks = false;
                        onDialogDone();
                    }
                }
            }
        }
        else if( s == selfServiceHook::processByCountItemGen2 )
            byCountItem_->renderBarcodeOnlyLabel();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::networkConnectivity( const MadpNetworkConnectivity *m )
{
    // set background palette for online/offline
    updateBackground( (online_ = m->isNetworkConnected()) );

    // FS UI TESTING ONLY - remove before checkin.
    // USE LINE ABOVE - so uncomment it
    // updateBackground( false );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::networkDeviceType( const MadpNetworkDeviceType *m )
{
    Q_UNUSED( m );

    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::refreshItem()
{
    randomWeightItem_->refresh();
    byCountItem_->refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::resetByCountNumpad()
{
    byCountItem_->resetKeypad();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::resetByCountNumpadDisplay()
{
    //weightBar_->clearByCount();               // clear the bycount field in the weight bar

    // SOF-4349 - ByCount issues - moved this to ByCount->ShowEvent()
    //byCountItem_->resetKeypadDisplay();         // clear it in the keypad for good measure
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceDialogG2::getByCountStackState()
{
/*
    int stackstate = byCountItem_->getStackState();
    return stackstate;                                  // did this in 2 lines so I could debug it
*/
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfServiceDialogG2::autoCloseDialogOnWeight( int code)
{
    // we wantt o automatically close the dialog if we are below 0, and we receive a weight that takes us back above 0 so
    // automatically close the dialog since the dialog has no buttons on it.
    if( ( WeigherDevError::WeigherErrNegativeGrossWeight == code ) || ( WeigherDevError::ssWeigherErrNegativeGrossWeight == code ) ||
        ( WeigherDevError::WeigherErrCantRezeroNegativeGrossWeight == code ) || ( WeigherDevError::ssWeigherErrCantRezeroNegativeGrossWeight == code ) ||
        ( WeigherDevError::WeigherErrUnderMinimumWeight == code ) || ( WeigherDevError::ssWeigherErrUnderMinimumWeight == code ) )
        return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::refreshSettings()
{
    weightsMeasures_->refresh();
    auditLog_->refresh();

    byCountItem_->refresh();
    itemEntryKeyed_->refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::refreshWeight()
{
    randomWeightItem_->refreshWeight();
    itemEntryKeyed_->refreshWeight();
    instruct_->refreshWeight();
    byCountItem_->refreshPriceAndCount();

    itemEntryImages_->refreshWeight();
    itemEntryImagesViewAll_->refreshWeight();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::translate()
{
    // translate widgets
    attract_->translate();
    instruct_->translate();

    if( scaleConfig_->internalConfig.ssactive_lock )
    {
        itemEntryKeyed_->translate();                       // bc the screen was already loaded and flashkeys requested which would happen again if we did this again
        itemEntryImages_->translate();
        itemEntryImagesViewAll_->translate();
    }

    byCountItem_->translate();
    randomWeightItem_->translate();
    weightsMeasures_->translate();
    auditLog_->translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::refreshPlaylist(const QString playlist)
{
    return;
}

void selfServiceDialogG2::resetNumPadEntry()
{
    itemEntryKeyed_->resetEntry();
    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    //updateBackground( online_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::dimBackground()
{
    LOG_CLASS_INFO( "selfServiceDialogG2", "renderBarcodeOnlyLabel" );

    selfServiceHook::state s( hook_->currentState() );
    LOG_INFO( "dimBackground state passed in %d", s );

    switch( s )
    {
        case selfServiceHook::attractGen2:
            instruct_->dimScreenOpacity();
            break;

        case selfServiceHook::itemEntryKeyedGen2:
            itemEntryKeyed_->dimScreenOpacity();
            break;

        case selfServiceHook::processRandomWeightItemGen2:
            randomWeightItem_->dimScreenOpacity();
            break;

        case selfServiceHook::processByCountItemGen2:
            byCountItem_->dimScreenOpacity();
            break;

        case selfServiceHook::itemEntryImagesGen2:
            itemEntryImages_->dimScreenOpacity();
            break;

        case selfServiceHook::itemEntryImagesViewAllGen2:
            itemEntryImagesViewAll_->dimScreenOpacity();
            break;

        case selfServiceHook::takeLabel:
            // when printing, youre thrown in takeLabel state with the 1,4 message
            randomWeightItem_->dimScreenOpacity();
            byCountItem_->dimScreenOpacity();
            break;

        default:
            // must be on a screen we dont pop dialogs
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::resetBackground()
{
    // I was seeing instances where the "currentMode" would change in the split second this was supposed to happen
    // and the dinmess was getting reset to the incorrect screen.  So I am resetting them all just like updating weights
    instruct_->resetScreenOpacity();
    itemEntryKeyed_->resetScreenOpacity();
    randomWeightItem_->resetScreenOpacity();
    byCountItem_->resetScreenOpacity();
    itemEntryImages_->resetScreenOpacity();
    itemEntryImagesViewAll_->resetScreenOpacity();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString selfServiceDialogG2::getDialogBoxCode()
{
    return errorDialog_->getErrorCode();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceDialogG2::getDialogBoxStyle()
{
    return errorDialog_->getStyleCode();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::refreshDialog( const QString code, int buttonstyle )
{
    if( errorDialog_ == NULL )
        errorDialog_  = new hobart::selfserveDialogG2( hook_, hook_->session(), this );

    if( errorDialog_ )
    {
        // open dialog
        if ( errorDialog_->isValid() )
        {
            if ( errorDialog_->invalidErrorCode( code.toInt() ) )
                return;

            code_ = code.toInt();
            errorDialog_->updateCodes( code, buttonstyle );     // need these before the show()
            errorDialog_->move( QPoint( 253, 52 ) );
            errorDialog_->show();
            
            bgflag_ = true;
            onHookReloadImages();

            dimBackground();
        }
        else
        {
            delete errorDialog_;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::showErrorDialog()
{
    if( errorDialog_ )
    {
        if ( errorDialog_->invalidErrorCode( errorDialog_->getErrorCode().toInt() ) )
            return;

        errorDialog_->show();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::hideErrorDialog()
{
    errorDialog_->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::dismissErrorDialog(const QString code, int buttonstyle )
{
    Q_UNUSED( buttonstyle );

    // I hope we always get a dismiss 255 message otherwise we are going forever have a dimmed screen.
    // Auto pop and dismiss no button dialogs/Rezero work?

    if( errorDialog_ )
    {
        // absolute error dialog dismissal so dismiss them all without even checking

        if ( bgflag_ )
        {
            errorDialog_->closeDialog("1");
            scaleConfig_->internalConfig.selfServicePreventClicks = false;
            onDialogDone();
        }

        resetBackground();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::dismissErrorDialogNoCode()
{
    LOG_CLASS_INFO( "selfServiceDialog", "dismissErrorDialogNoCode" );

    LOG_DEBUG( "made it to selfservicedialog.dismissErrorDialogNoCode()", NULL );

    if( errorDialog_ )
        dismissErrorDialog( errorDialog_->getErrorCode(), errorDialog_->getStyleCode() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::showEvent( QShowEvent *e )
{
    // invoke base method
    _Mybase::showEvent( e );

    if(  scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() && ( SelfServicePrintOption::SelfServOption_PrintToLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() ) )
    {
        // if we are just print a physical label, not to the screen - turn off label to screen so we can
        // Turn on the print to screen flag, if it is off
        QString param( "," + QString::number( fieldOffsetCfgSelfServePrintToScreen ) );
        param += ",0,1";
        scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGORaw() = "0";
        hook_->session()->send( ConfigurationDev, CfgWriteSingleField, param );
    }
    else if(  scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() && ( SelfServicePrintOption::SelfServOption_PrintToScreen == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() ) )
    {
        // if we are just print to the screen, not to a label - turn on label to screen so we can
        // Turn on the print to screen flag, if it is off
        QString param( "," + QString::number( fieldOffsetCfgSelfServePrintToScreen ) );
        param += ",1,0";
        scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGORaw() = "1";
        hook_->session()->send( ConfigurationDev, CfgWriteSingleField, param );
    }

    // raise dialog to top most z-order
    raise();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onButtonBarCancel()
{
    if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && SelfServicePrintOption::SelfServOption_PrintToLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
        return;

    // pjr383 - Amazon GO - what does this message do? Leave as is since we still support this config
    if ( scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() )
        hook_->session()->send( ProductDev, ProductAcceptSelfServiceBarcode, "0" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onButtonBarEnter()
{

    LOG_CLASS_INFO( "selfServiceDialogG2", "onButtonBarEnter" );

    selfServiceHook::state s( hook_->currentState() );
    LOG_DEBUG( "handling state %d...", s );

    if ( selfServiceHook::itemEntryKeyedGen2 == s )
    {
        QString itemCode( itemEntryKeyed_->itemCode() );

        if ( !itemCode.isEmpty() )
        {
            LOG_INFO( "have keyed item '%s'", qPrintable( itemCode ) );
            hook_->emitChangeFieldValue( sessionHooks::itemCode, itemCode );
        }
    }

    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onShowBackButton()
{
/*
    // TRIGGERED:   connect( byCountItem_, SIGNAL( showBackButton() ), SLOT( onShowBackButton() ) );
    // CALLED BY:   selfServiceHook::resetBackButton()

    buttonBar_->setBackButtonOn();
*/
    return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onButtonBarOkay()
{
    if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && SelfServicePrintOption::SelfServOption_PrintToLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
        return;

    if ( scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() || scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
        hook_->session()->send( ProductDev, ProductAcceptSelfServiceBarcode, "1" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onPrintPressed()
{
    if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
    {
        // have to handle clicks and double clicks even if we are ready or not. But I dont want to handle them again
        // if I havent drawn the print screen in the first place. so only allow again once Ive displayed the print screen
        if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
        {
            if ( scaleConfig_->internalConfig.selfServicePreventClicks )
                return;

            scaleConfig_->internalConfig.selfServicePreventClicks = true;
        }

        // Clicking PRINT on ByCount keypad calls this. The emitPrint() below was causing some weird activity
        hook_->session()->send( SessionDev, SnPrintKeyPressed );
        return;
    }

    // does anyone call this?
    hook_->emitPrint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onPrintError(int errorNum )
{
     hook_->session()->send( Error, ErrProductDevice, QString::number(errorNum) + ",5" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onSendConfigPressed()
{
    scaleConfig_->internalConfig.selfServicePreventClicks = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onHookCurrentLookOfStateChange()
{
    selfServiceHook::state s( hook_->currentState() );

    Session *session_ = hook_->session();

    // 2nd Generation configurations handle state change
    switch ( s )
    {
        case selfServiceHook::processRandomWeightItemGen2:
        {
            // so far the only reason I have seen this called is when we click startover on RW screen

            if ( session_ && scaleConfig_->internalConfig.ssactive_lock)
                session_->send( BarcodeScanner, ScannerDisabled );     // disable the scanner on the print a label screen

            turnOnPrintToScreen();

            // Green Go button - Timeout to START
            createLayout( attract_ );
        }
        break;

        default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onHookCurrentStateChanged()
{
    LOG_CLASS_INFO( "selfServiceDialogG2", "onHookCurrentStateChanged" );

    // Main Screen that moves the user from screen to screen depending on
    // what is looked up or selected at any time

    if( !scaleConfig_->internalConfig.ssactive_lock )
    {
        LOG_DEBUG( "Tried to change screens while in OPERATE MODE", NULL );
        return;
    }

    selfServiceHook::state s( hook_->currentState() );
    LOG_DEBUG( "handling state %d...", s );

    Session *session_ = hook_->session();

    scaleConfig_->internalConfig.selfServicePreventClicks = false;

    // 2nd Generation configurations handle state change
    switch ( s )
    {
        case selfServiceHook::attractGen2:
        {
            if ( session_ && scaleConfig_->internalConfig.ssactive_lock)
                session_->send( BarcodeScanner, ScannerDisabled );     // disable the scanner on the print a label screen

            if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
                sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceKeyEntryStyle, "0");

            turnOnPrintToScreen();          // might be able to get rid of this since we are not having sepearate screens for print and scan.

            createLayout( instruct_ );

            instruct_->show();
            instruct_->activateWindow();
            instruct_->raise();
        }
        break;

        case selfServiceHook::itemEntryKeyedGen2:
        {
            // Main Flashkey and Numpad PLU Lookup Screen
            if ( session_ && scaleConfig_->internalConfig.ssactive_lock )
                session_->send( BarcodeScanner, ScannerEnabled );      // enable scanner on key entry screen

            turnOnPrintToScreen();

            createLayout( itemEntryKeyed_ );

            //itemEntryKeyed_->resetScreen();       // does this when we createLayout()
        }
        break;

        case selfServiceHook::processRandomWeightItemGen2:
        {

            // after entering a RW item, screen that says put a weight on platter and print label
            if ( session_ && scaleConfig_->internalConfig.ssactive_lock)
                session_->send( BarcodeScanner, ScannerDisabled );     // disable the scanner on the print a label screen

            turnOnPrintToScreen();

            createLayout( randomWeightItem_ );

            randomWeightItem_->resetScreen();
        }
        break;

        case selfServiceHook::processByCountItemGen2:
        {
            // after entering BC item, screen that says enter the quantity and press print
            if ( session_ && scaleConfig_->internalConfig.ssactive_lock)
                session_->send( BarcodeScanner, ScannerDisabled );     // disable the scanner on the print a label screen

            turnOnPrintToScreen();

            createLayout( byCountItem_ );

            byCountItem_->resetScreen();
        }

        break;

        case selfServiceHook::itemEntryImagesGen2:
        {
            // SEARCH SCREEN
            // WEH-102 Scanner needs to turned off while in operate mode
            if ( session_ && scaleConfig_->internalConfig.ssactive_lock )
                session_->send( BarcodeScanner, ScannerEnabled );      // enable scanner on flashkey screen

            turnOnPrintToScreen();

            createLayout( itemEntryImages_ );
        }

        break;

        case selfServiceHook::itemEntryImagesViewAllGen2:
        {
            // SEARCH SCREEN
            // WEH-102 Scanner needs to turned off while in operate mode
            if ( session_ && scaleConfig_->internalConfig.ssactive_lock )
                session_->send( BarcodeScanner, ScannerEnabled );      // enable scanner on flashkey screen

            turnOnPrintToScreen();

            createLayout( itemEntryImagesViewAll_ );
        }

        break;

        case selfServiceHook::weightsMeasuresDialogGen2:
            createLayout( weightsMeasures_ );
            break;

        case selfServiceHook::auditLogDialogGen2:
            createLayout( auditLog_ );
            break;

        default:
            break;
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onHookReloadImages()
{
    background_.load( filesystem::absoluteFilePath( "background_G2.png", folderType::SelfServeImages ) );

    updateBackground( online_ );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onWeightBarPressed()
{
// for hobart's most esteemed and wise development staff we always want to offer up this handy dandy
// swell ability to simply go into Weights and Measures Mode and allow exit the rather forcefull 
// self service mode and return the poor, helpless developer to the previously scheduled scale application 
#ifdef QT_DEBUG
    // switch states to W&M dialog
    hook_->setCurrentState( selfServiceHook::weightsMeasuresDialog );
    return;
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onDialogDone()
{
    bgflag_ = false;
    code_ = 0;
    onHookReloadImages();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::onPopQuantity()
{
    byCountItem_->resetEntry();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::initialize()
{
    LOG_CLASS_INFO( "selfServiceDialogG2", "initialize()" );

    // Set up the hook to access ScaleConfiguration
    Session *s = hook_->session();
    scaleConfig_ = s->scaleConfig;

    // setup transparent background during child creation
    QPalette p( palette() );
    p.setColor( backgroundRole(), Qt::transparent );
    setPalette( p );

    // setup widgets
    
    attract_                = new hobart::selfserveStartG2( hook_, this );
    instruct_               = new hobart::selfserveInstructG2( hook_, this );
    itemEntryKeyed_         = new hobart::selfserveItemLookupG2( hook_, this );     // Main FLashkey and Numpad Lookup
    itemEntryImages_        = new hobart::selfserveImageLookupG2( hook_, this );
    itemEntryImagesViewAll_ = new hobart::selfserveImageLookupViewAllG2(hook_, this);
    byCountItem_            = new hobart::selfserveByCountG2( hook_, this );
    randomWeightItem_       = new hobart::selfserveRandomWeightG2( hook_, this );

    weightsMeasures_        = new hobart::selfserveWeightsMeasuresG2( hook_, this );
    auditLog_               = new hobart::selfserveAuditLogG2( hook_, this );

    errorDialog_            = new hobart::selfserveDialogG2( hook_, hook_->session(), this );
    errorDialog_->hide();

    connect( itemEntryKeyed_, SIGNAL( lookup() ), SLOT( onButtonBarEnter() ) );

    // Add signal to pop open weight only dialogs if needed
    connect(errorDialog_,     SIGNAL( dialogDone() ), SLOT( onDialogDone()) );

    // Add signal to pop open weight only dialogs if needed
    connect(errorDialog_,     SIGNAL( popQuantity() ), SLOT( onPopQuantity()) );

    // load images
    onHookReloadImages();

#ifdef QT_DEBUG

    LOG_DEBUG( "Showing developer X button", NULL );

    // for hobart's most esteemed and wise development staff we offer up this handy dandy "X"
    // button which shall exit the rather forcefull self service mode and return the poor, helpless
    // developer to the previously scheduled scale application
    QPushButton *holyCrapWhereDidMyDesktopGo( new QPushButton( "X", this ) );
    holyCrapWhereDidMyDesktopGo->setFixedSize( 32, 32 );
    connect( holyCrapWhereDidMyDesktopGo, SIGNAL( clicked() ), hook_, SLOT( leave() ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::createLayout( QWidget *w, bool showBar, int mode, int buttons )
{
    Q_UNUSED( buttons );
    Q_UNUSED( mode );
    Q_UNUSED( showBar );

    if ( layout() )
        delete layout();

    hideAllChildren();

    QVBoxLayout *screen( new QVBoxLayout( this ) );
    screen->setContentsMargins( QMargins() );

    screen->addWidget( w, 1 );
    w->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfServiceDialogG2::turnOffPrintToScreen()
{
    bool turnedOff = false;

    QString itemCode( hook_->fieldValue( sessionHooks::itemCode ) );
    if(  itemCode.isEmpty() || itemCode.toInt() <= 0)
        return false;

    Session *session_ = hook_->session();

    if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() )
    {
        if(  SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
        {
            // If BigY turned on (which requires Print to screen ON), and PrintOption=PrintLabel (print a regular label), and PrintToScreen is On,
            // then turn PrintScreen Off so we can print a regular label

            // Turn off
            turnedOff = true;
            QString param( "," + QString::number( fieldOffsetCfgSelfServePrintToScreen ) );
            param += ",0,1";
            scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGORaw() = "0";
            session_->send( ConfigurationDev, CfgWriteSingleField, param );
        }
    }
    return turnedOff;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfServiceDialogG2::turnOnPrintToScreen()
{
    bool turnedOn = false;

    QString itemCode( hook_->fieldValue( sessionHooks::itemCode ) );
    if(  itemCode.isEmpty() || itemCode.toInt() <= 0)
        return false;

    Session *session_ = hook_->session();

    if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && !scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() )
    {
        if( SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
        {
            // Turn on
            turnedOn = true;
            QString param( "," + QString::number( fieldOffsetCfgSelfServePrintToScreen ) );
            param += ",1,0";
            scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGORaw() = "1";
            session_->send( ConfigurationDev, CfgWriteSingleField, param );
        }
    }
    return turnedOn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::hideAllChildren()
{
    attract_->hide();
    instruct_->hide();
    itemEntryKeyed_->hide();
    itemEntryImages_->hide();
    itemEntryImagesViewAll_->hide();
    byCountItem_->hide();
    randomWeightItem_->hide();

    weightsMeasures_->hide();
    auditLog_->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialogG2::updateBackground( bool online )
{
    if( networkFlipped_ == online )
       return;

	networkFlipped_ = online;

	// UPDATE all of the backgrounds
    //attract_->updateNetworkConnectivity( online );
    instruct_->updateNetworkConnectivity( online );
    itemEntryKeyed_->updateNetworkConnectivity( online );
    itemEntryImages_->updateNetworkConnectivity( online );
    itemEntryImagesViewAll_->updateNetworkConnectivity( online );
    weightsMeasures_->updateNetworkConnectivity( online );
    auditLog_->updateNetworkConnectivity( online );
    byCountItem_->updateNetworkConnectivity( online );
    randomWeightItem_->updateNetworkConnectivity( online );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceDialogG2::checkScreenSize()
{
    return 14;          // 1366x768
}
