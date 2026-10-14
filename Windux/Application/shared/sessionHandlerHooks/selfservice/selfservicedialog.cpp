/**
 * @file selfservicedialog.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfservicedialog.h"

#include "selfserveStart.h"
#include "selfserveLogin.h"
#include "selfserveInstruct.h"
#include "selfservePlaylist.h"
#include "selfserveItemLookup.h"
#include "selfserveImageLookup.h"
#include "selfserveByCount.h"
#include "selfserveRandomWeight.h"
#include "selfserveFinish.h"
#include "selfserveWeightsMeasures.h"
#include "selfserveAuditLog.h"

#include "selfservicebuttonbar.h"
#include "selfserviceweightbar.h"

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

const QColor selfServiceDialog::offlineBackgroundColor_( hobart::colorPalette::red );
const QColor selfServiceDialog::onlineBackgroundColor_( QColor( 203, 203, 203 ) );

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceDialog::selfServiceDialog( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    hook_( h ),
    scaleConfig_(NULL),
    online_( true )
{
    // setup dialog
    setObjectName( "selfServiceDialog" );
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint );

    // initialize
    initialize();

    // connect signals/slots
    connect( hook_, SIGNAL( currentStateChanged() ), SLOT( onHookCurrentStateChanged() ) );
    connect( hook_, SIGNAL( currentLookOfStateChanged() ), SLOT( onHookCurrentLookOfStateChange() ) );
    connect( hook_, SIGNAL( reloadImages() ), SLOT( onHookReloadImages() ) );

    if( scaleConfig_->scaleConfig.selfServeFixedPlu() != "0" )
    {
        weightBar_->setCurrentMode( selfServiceWeightBar::weightOnly );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceDialog::~selfServiceDialog()
{
    if (scaleConfig_ )
        delete scaleConfig_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::loadSoftkeyGroup( const DomLoadSoftkeyGroup *m )
{
    itemEntryKeyed_->loadSoftkeyGroup( m );
    itemEntryImages_->loadSoftkeyGroup( m );

    if( hobart::softkeysWidget::PresetTareGrid == m->featureNum() )
    {
        // new
        // check to see if we have any flashkeys
        int totalKeys = m->totalNumOfSoftkeys();
        if( totalKeys > 0 )
            scaleConfig_->tareFlashkeysDoExist = true;
        else
            scaleConfig_->tareFlashkeysDoExist = false;
        // orig code
        randomWeightItem_->loadSoftkeyGroup( m );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::renderBarcodeOnlyLabel()
{
    LOG_CLASS_INFO( "selfServiceDialog", "renderBarcodeOnlyLabel" );

    if( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() )
    {
        selfServiceHook::state s( hook_->currentState() );
        LOG_DEBUG( "handling state %d...", s );

        if( s == selfServiceHook::processRandomWeightItem )
            randomWeightItem_->renderBarcodeOnlyLabel();
        else if( s == selfServiceHook::processByCountItem )
            byCountItem_->renderBarcodeOnlyLabel();
        else
            LOG_DEBUG( "handling renderBarcodeOnlyLabel() do nothing with state %d...", s );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::networkConnectivity( const MadpNetworkConnectivity *m )
{
    // set background palette for online/offline
    updateBackground( (online_ = m->isNetworkConnected()) );

    // forward message to children
    weightBar_->networkConnectivity( m );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::networkDeviceType( const MadpNetworkDeviceType *m )
{
    weightBar_->networkDeviceType( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::refreshItem()
{
    byCountItem_->refresh();
    randomWeightItem_->refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::resetByCountNumpad()
{
    byCountItem_->resetKeypad();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::resetByCountNumpadDisplay()
{
    weightBar_->clearByCount();               // clear the bycount field in the weight bar

    // SOF-4349 - ByCount issues - moved this to ByCount->ShowEvent()
    //byCountItem_->resetKeypadDisplay();         // clear it in the keypad for good measure
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceDialog::getByCountStackState()
{
    int stackstate = byCountItem_->getStackState();
    return stackstate;                                  // did this in 2 lines so I could debug it
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::refreshSettings()
{
    byCountItem_->refresh();
    itemEntryKeyed_->refresh();

    weightsMeasures_->refresh();
    auditLog_->refresh();

    buttonBar_->refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::refreshWeight()
{
    weightBar_->refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::translate()
{
    // translate widgets
    attract_->translate();
    instruct_->translate();
    playlist_->translate();
    login_->translate();

    if( scaleConfig_->internalConfig.ssactive_lock )
        itemEntryKeyed_->translate();                       // bc the screen was already loaded and flashkeys requested which would happen again if we did this again
    
    itemEntryImages_->translate();
    byCountItem_->translate();
    randomWeightItem_->translate();
    takeLabelAndStartOver_->translate();

    weightsMeasures_->translate();
    auditLog_->translate();

    buttonBar_->translate();
    weightBar_->translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::refreshPlaylist(const QString playlist)
{
    // update playlist
    playlist_->setPlaylist( playlist );
}

void selfServiceDialog::resetNumPadEntry()
{
    itemEntryKeyed_->resetEntry();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    updateBackground( online_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::showEvent( QShowEvent *e )
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
void selfServiceDialog::onButtonBarCancel()
{
    if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && SelfServicePrintOption::SelfServOption_PrintToLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
        return;

    // pjr383 - Amazon GO - what does this message do? Leave as is since we still support this config
    if ( scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() )
        hook_->session()->send( ProductDev, ProductAcceptSelfServiceBarcode, "0" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::onButtonBarEnter()
{
    LOG_CLASS_INFO( "selfServiceDialog", "onButtonBarEnter" );

    selfServiceHook::state s( hook_->currentState() );
    LOG_DEBUG( "handling state %d...", s );

    if ( selfServiceHook::itemEntryKeyed == s )
    {
        QString itemCode( itemEntryKeyed_->itemCode() );

        if ( !itemCode.isEmpty() )
        {
            LOG_INFO( "have keyed item '%s'", qPrintable( itemCode ) );
            hook_->emitChangeFieldValue( sessionHooks::itemCode, itemCode );
        }
    }

    // PJR413 (SOF-4772) - Enter the fixed PLU if that config is set
    if ( scaleConfig_->scaleConfig.selfServeFixedPlu() != "0" )
    {
        QString fixedPlu( scaleConfig_->scaleConfig.selfServeFixedPlu() );
        LOG_INFO( "have fixed item '%s'", qPrintable( fixedPlu ) );
        hook_->emitChangeFieldValue( sessionHooks::itemCode, fixedPlu );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::onShowBackButton()
{
    // TRIGGERED:   connect( byCountItem_, SIGNAL( showBackButton() ), SLOT( onShowBackButton() ) );
    // CALLED BY:   selfServiceHook::resetBackButton()
    // CALLS:       selfServiceButtonBar::setBackButtonOn() - the end goal to turn it on)

    buttonBar_->setBackButtonOn();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::onButtonBarOkay()
{
    // pjr383 - use original
    //if( SelfServicePrintOption::SelfServOption_PrintToLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
    if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && SelfServicePrintOption::SelfServOption_PrintToLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
        return;

    // pjr383 - use original
    //if ( SelfServicePrintOption::SelfServOption_PrintToScreenAndBarcode == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
    if ( scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() || scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
        hook_->session()->send( ProductDev, ProductAcceptSelfServiceBarcode, "1" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::onPrintPressed()
{
    // pjr383 - use original
    //if( scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() > SelfServicePrintOption::SelfServOption_PrintToLabel  )
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
void selfServiceDialog::onPrintError(int errorNum )
{
     hook_->session()->send( Error, ErrProductDevice, QString::number(errorNum) + ",5" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::onSendConfigPressed()
{
    scaleConfig_->internalConfig.selfServicePreventClicks = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::onHookCurrentLookOfStateChange()
{
    LOG_CLASS_INFO( "selfServiceDialog", "onHookCurrentLookOfStateChange" );

    selfServiceHook::state s( hook_->currentState() );
    LOG_DEBUG( "handling state %d...", s );

    // Pressed Print Label on Print or Scan Screen (Finish Screen)
    if( scaleConfig_->internalConfig.clickedPrintLabelFromFinishScreen )
    {
        // Standard Print a label to a label and pull it and start over. Also BigY Option 1 bc for option 1 PrintToScreen is disabled
        createLayout(
        takeLabelAndStartOver_,
        true,
        selfServiceWeightBar::capacityOnly );
    }
    else
    {
        if( "0" == hook_->fieldValue( sessionHooks::selfServiceKeyEntryStyle ) && ( scaleConfig_->scaleConfig.selfServeItemEntryScreenState() == SelfServicePrintOption::SelfServOption_LookupCategoriesShowKeypad) )
        {
                // right now we are (flashkeys) so display the numpad if on print label screen we are already good
                sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceKeyEntryStyle, "1");       // show lone  keypad - Displays Flashkey button
        }
        else
        {
            // right now we are 1 (numpad) so display the flashkeys (0)
            sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceKeyEntryStyle, "0");           // display lone flashkeys - Display Keypad button
        }

        buttonBar_->refresh();
        LOG_DEBUG( "BEGIN Look of State Changed", NULL );

        if( scaleConfig_->scaleConfig.selfServeSearchScreenState() == SelfServicePrintOption::SelfServOption_SearchNone )
        {
            // This set of buttons was created for Delhaize. They wanted to hide the Search button  and not get a keypad button.  But we need to allow other people to have it if turned on
            LOG_DEBUG( "START create Layout", NULL );
            if( scaleConfig_->scaleConfig.selfServeItemEntryScreenState() == SelfServicePrintOption::SelfServOption_LookupCategoriesShowKeypad )
            {
                createLayout(
                    itemEntryKeyed_,
                    true,
                    selfServiceWeightBar::weightOnly,
                    selfServiceButtonBar::showItemEntryKeyed | selfServiceButtonBar::showToggleLanguage | selfServiceButtonBar::showRezero | selfServiceButtonBar::showKeypad | selfServiceButtonBar::showEnter );
            }
            else
            {
                createLayout(
                    itemEntryKeyed_,
                    true,
                    selfServiceWeightBar::weightOnly,
                    selfServiceButtonBar::showItemEntryKeyed | selfServiceButtonBar::showToggleLanguage | selfServiceButtonBar::showRezero | selfServiceButtonBar::showEnter );
            }
            LOG_DEBUG( "DONE create Layout", NULL );
            // This gets itemLookupScreen and these buttons:  Start Over(showItemEntryKeyed), Language(showToggleLanguage), Rezero(showRezero), ShowEnter (but this does not add Enter button I dont know what this does)
        }
        else if( scaleConfig_->scaleConfig.selfServeItemEntryScreenState() == SelfServicePrintOption::SelfServOption_LookupCategoriesShowKeypad )
        {
            // This set of buttons was created for BigY. There is a new Keypad button that lets users flip back and forth from the Keypad to the full screen categories
            createLayout(
                itemEntryKeyed_,
                true,
                selfServiceWeightBar::weightOnly,
                selfServiceButtonBar::showItemEntryKeyed | selfServiceButtonBar::showItemEntryImages | selfServiceButtonBar::showToggleLanguage | selfServiceButtonBar::showKeypad | selfServiceButtonBar::showRezero | selfServiceButtonBar::showEnter );
                // custom itemLookupScreen buttons:  Start Over(showItemEntryKeyed), Search(showItemEntryImages), Language(showToggleLanguage), Keypad(showKeypad), Rezero(showRezero), ShowEnter?
        }
        else
        {
            // Default Option. Includes a Search button with no Keypad button
            createLayout(
                itemEntryKeyed_,
                true,
                selfServiceWeightBar::weightOnly,
                selfServiceButtonBar::showItemEntryKeyed | selfServiceButtonBar::showItemEntryImages | selfServiceButtonBar::showToggleLanguage | selfServiceButtonBar::showRezero | selfServiceButtonBar::showEnter );
                // Default itemLookupScreen and buttons:  Start Over(showItemEntryKeyed), Search(showItemEntryImages), Language(showToggleLanguage), Rezero(showRezero), ShowEnter?
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::onHookCurrentStateChanged()
{
    LOG_CLASS_INFO( "selfServiceDialog", "onHookCurrentStateChanged" );

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
    int displayBarcode( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() );

    // handle state change
    switch ( s )
    {
    case selfServiceHook::attract:
    {
        if( SelfServicePrintOption::SelfServOption_StartOperLogin == scaleConfig_->scaleConfig.selfServeStartScreenState() )
        {
            session_->send( BarcodeScanner, ScannerEnabled );
        }
        else
        {
            if ( session_ && scaleConfig_->internalConfig.ssactive_lock)
                session_->send( BarcodeScanner, ScannerDisabled );     // disable the scanner on the print a label screen
        }

        // pjr383 use original
        if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
            sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceKeyEntryStyle, "0");           // display the flashkeys

        scaleConfig_->internalConfig.selfServicePreventClicks = false;

        turnOnPrintToScreen();

        // Initial 3 image instruction page start screen with big green START button
        if ( scaleConfig_->scaleConfig.selfServeWeightRequiredToSelectItem() )
        {
            createLayout(
                instruct_,
                true,
                selfServiceWeightBar::weightOnly );
        }
        else if( scaleConfig_->scaleConfig.selfServeEnablePlaylists() )
        {    // Initial playlist page start screen
            createLayout(
                playlist_ );
        }
        else if( SelfServicePrintOption::SelfServOption_StartOperLogin == scaleConfig_->scaleConfig.selfServeStartScreenState() )
        {
            // Initial login page start screen
            createLayout(
                login_,
                true,
                selfServiceWeightBar::weightOnly );		// bigWeightOnly
        }
        else
        {
            // Green Go button - Timeout to START
            createLayout(
                attract_ );
        }

        // Operate Mode numpad focus solution
        show();
        activateWindow();
        raise();
    }

    break;

    case selfServiceHook::itemEntryKeyed:
    {
        // keypad screen
        // WEH-102 Scanner needs to turned off while in operate mode
        // SOF-5303 - Consecutive barcode scans bypassing login screen. Disabled certain state change when on login screen

        if ( session_ && scaleConfig_->internalConfig.ssactive_lock )
            session_->send( BarcodeScanner, ScannerEnabled );      // enable scanner on key entry screen

        turnOnPrintToScreen();

        // PJR413 (SOF-4772) If Fixed PLU exists, bypass attract screen and go straight to
        // processRandomWeightItem screen with PLU already entered
        if( scaleConfig_->scaleConfig.selfServeFixedPlu() != "0" )
        {
            // Set weightBar
            createLayout(
                randomWeightItem_,
                true,
                selfServiceWeightBar::weightOnly,
                selfServiceButtonBar::showToggleLanguage );//| selfServiceButtonBar::showStartOver );
            // Enter item PLU via the button bar enter key
            onButtonBarEnter();
            //Hide keypad - moved to bottom for everyone who needs it
            //hook_->emitShutdownKeyboards(); //This actually causes a problem where the focus switches back to the main screen on reboot, so we are no longer using it
            //show();
            //activateWindow();
            //raise();
        }
        // pjr383 - use original test
        else if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
        {
            // Pressed Print Label on Print or Scan Screen (Finish Screen)
            if( scaleConfig_->internalConfig.clickedPrintLabelFromFinishScreen )
            {
                // Clicked Print on TakeLabel Screen, need to reload the screen now - but already loaded it so exit out
                return;
            }
            else
            {
                LOG_DEBUG( "BEGIN #2 Current State Changed", NULL );

                if( scaleConfig_->scaleConfig.selfServeSearchScreenState() == SelfServicePrintOption::SelfServOption_SearchNone )
                {
                    // This set of buttons was created for Delhaize. They wanted to hide the Search button  and not get a keypad button.  But we need to allow other people to have it if turned on
                    LOG_DEBUG( "START Create Layout", NULL );
                    if( scaleConfig_->scaleConfig.selfServeItemEntryScreenState() == SelfServicePrintOption::SelfServOption_LookupCategoriesShowKeypad )
                    {
                        createLayout(
                            itemEntryKeyed_,
                            true,
                            selfServiceWeightBar::weightOnly,
                            selfServiceButtonBar::showItemEntryKeyed | selfServiceButtonBar::showToggleLanguage | selfServiceButtonBar::showRezero | selfServiceButtonBar::showKeypad | selfServiceButtonBar::showEnter );
                    }
                    else
                    {
                        createLayout(
                            itemEntryKeyed_,
                            true,
                            selfServiceWeightBar::weightOnly,
                            selfServiceButtonBar::showItemEntryKeyed | selfServiceButtonBar::showToggleLanguage | selfServiceButtonBar::showRezero | selfServiceButtonBar::showEnter );
                    }
                    LOG_DEBUG( "DONE Create Layout", NULL );
                }
                else if( scaleConfig_->scaleConfig.selfServeItemEntryScreenState() == SelfServicePrintOption::SelfServOption_LookupCategoriesShowKeypad )
                {
                    // This set of buttons was created for BigY. There is a new Keypad button that lets users flip back and forth from the Keypad to the full screen categories
                    createLayout(
                        itemEntryKeyed_,
                        true,
                        selfServiceWeightBar::weightOnly,
                        selfServiceButtonBar::showItemEntryKeyed | selfServiceButtonBar::showItemEntryImages | selfServiceButtonBar::showToggleLanguage | selfServiceButtonBar::showKeypad | selfServiceButtonBar::showRezero | selfServiceButtonBar::showEnter );
                       // custom itemLookupScreen and buttons:  Start Over(showItemEntryKeyed), Search(showItemEntryImages), Language(showToggleLanguage), Keypad(showKeypad), Rezero(showRezero), ShowEnter?
                }
                else
                {
                    // defafult
		            createLayout(
		                itemEntryKeyed_,
		                true,
		                selfServiceWeightBar::weightOnly,
                        selfServiceButtonBar::showItemEntryKeyed | selfServiceButtonBar::showItemEntryImages | selfServiceButtonBar::showToggleLanguage | selfServiceButtonBar::showRezero | selfServiceButtonBar::showEnter );
                       // Default itemLookupScreen and buttons:  Start Over(showItemEntryKeyed), Search(showItemEntryImages), Language(showToggleLanguage), Rezero(showRezero), ShowEnter?
                }
            }
        }

        if( scaleConfig_->scaleConfig.selfServeStartScreenState() == 1 )
        {
            // if no start screen then raise the itemEntry Screen so the keypad wont get focus in Operate
            show();
            activateWindow();
            raise();
        }
    }

    break;

    case selfServiceHook::itemEntryImages:
    {
        // SEARCH SCREEN
        // WEH-102 Scanner needs to turned off while in operate mode
        // SOF-5303 - Consecutive barcode scans bypassing login screen. Disabled certain state change when on login screen

        if ( session_ && scaleConfig_->internalConfig.ssactive_lock )
            session_->send( BarcodeScanner, ScannerEnabled );      // enable scanner on flashkey screen

        turnOnPrintToScreen();

        // pjr383 - use original
        if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
            sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceKeyEntryStyle, "0");           // display the flashkeys on itemEntry Screen, not Search Screen

        createLayout(
            itemEntryImages_,
            true,
            selfServiceWeightBar::weightOnly,
            selfServiceButtonBar::showItemEntryKeyed | selfServiceButtonBar::showToggleLanguage | selfServiceButtonBar::showRezero );
        break;

        if( scaleConfig_->scaleConfig.selfServeStartScreenState() == 1 )
        {
            // if no start screen then raise the itemEntry Screen so the keypad wont get focus in Operate
            show();
            activateWindow();
            raise();
        }
    }

    break;

    case selfServiceHook::processRandomWeightItem:
    {
        // SOF-5303
        if(hook_->isLoginScreen()){
            break;
        }
        // after entering a RW item, screen that says put a weight on platter and print label
        if ( session_ && scaleConfig_->internalConfig.ssactive_lock)
            session_->send( BarcodeScanner, ScannerDisabled );     // disable the scanner on the print a label screen

        turnOnPrintToScreen();

        //PJR413 (SOF-4772) If Fixed PLU exists, show weight only
        if( scaleConfig_->scaleConfig.selfServeFixedPlu() != "0" )
        {
            //Hide keypad
            //hook_->emitShutdownKeyboards();
            // Enter item PLU via the button bar enter key
            onButtonBarEnter();

            createLayout(
                randomWeightItem_,
                true,
                selfServiceWeightBar::weightOnly,
                selfServiceButtonBar::showToggleLanguage );//| selfServiceButtonBar::showStartOver );
        }
        else
        {
            createLayout(
                randomWeightItem_,
                true,
                selfServiceWeightBar::weightAndPrice,
                selfServiceButtonBar::showStartOver );
                // dropped print button - selfServiceButtonBar::showStartOver | selfServiceButtonBar::showPrintLabel );
        }
    }

    break;

    case selfServiceHook::processByCountItem:
    {
        // SOF-5303
        if(hook_->isLoginScreen()){
            break;
        }
        // after entering BC item, screen that says enter the quantity and press print
        if ( session_ && scaleConfig_->internalConfig.ssactive_lock)
            session_->send( BarcodeScanner, ScannerDisabled );     // disable the scanner on the print a label screen

        turnOnPrintToScreen();


        // we can combine these once I can turn the back button when I want
        if( displayBarcode )
        {
            createLayout(
                byCountItem_,
                true,
                selfServiceWeightBar::byCountAndPrice,
                selfServiceButtonBar::showStartOver | selfServiceButtonBar::showBack );
        }
        else
        {
            createLayout(
                byCountItem_,
                true,
                selfServiceWeightBar::byCountAndPrice,
                selfServiceButtonBar::showStartOver );
                // dropped print button - selfServiceButtonBar::showStartOver | selfServiceButtonBar::showPrintLabel );
        }
    }
    
    break;

    case selfServiceHook::takeLabel:
    {
        // screen where the video(image) indidates to take the label
        if ( session_ && scaleConfig_->internalConfig.ssactive_lock)
            session_->send( BarcodeScanner, ScannerDisabled );     // disable the scanner on the print a label screen

        LOG_DEBUG( "handling case takeLabel %d...", s );

        resetByCountNumpadDisplay();
        bool turnedOff = turnOffPrintToScreen();

        // Flipped this to NOT - have to see if it works with all other configs
        // This original logic is backwards, bc we just sent a msg to update config with a 0, we just hadn't received it back yet, Now this is correct in real time
        // I think this is ALWAYS TRUE from what I can tell
        if( turnedOff )
        {
            if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )         // I think this is always TRUE
            {
                if ( SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() )
                {   // Display Label On Screen and Print  ( display label and print from the same screen )
                    // Big Y Option 3 of 3 - Print the label to the Screen but provide a button to also print it
                    createLayout(
                    takeLabelAndStartOver_,
                    true,
                    selfServiceWeightBar::capacityOnly, selfServiceButtonBar::showOkay | selfServiceButtonBar::showPrintLabel );        // showOkay=Done, showPrintLabel=Print label
                }
                else if( scaleConfig_->scaleConfig.selfServePrintScreenState()  == SelfServicePrintOption::SelfServOption_PrintToScreen )
                {
                    // If we are Amazon GO (nobody is) then we scan label on screen and need a DONE button
                    createLayout(
                    takeLabelAndStartOver_,
                    true,
                    selfServiceWeightBar::capacityOnly,
                    selfServiceButtonBar::showOkay );
                }
                else
                {
                    // Big Y Option 2 of 3 - Print the label to the Screen so it can be scannedt
                    createLayout(
                    takeLabelAndStartOver_,
                    true,
                    selfServiceWeightBar::capacityOnly,
                    selfServiceButtonBar::showCancel | selfServiceButtonBar::showOkay );
                }
            }
            else
            {
                // Standard Print Label to Screen so you can scan it method (Existing Amazon PrintToScreen Option)
                if( scaleConfig_->scaleConfig.selfServePrintScreenState()  == SelfServicePrintOption::SelfServOption_PrintToScreen )
                {
                    // If we are Amazon GO (nobody is) then we scan label on screen and need a DONE button
                    createLayout(
                    takeLabelAndStartOver_,
                    true,
                    selfServiceWeightBar::capacityOnly,
                    selfServiceButtonBar::showOkay );
                }
                else
                {
                    createLayout(
                    takeLabelAndStartOver_,
                    true,
                    selfServiceWeightBar::capacityOnly,
                    selfServiceButtonBar::showCancel | selfServiceButtonBar::showOkay );
                }
            }
        }
        // pjr383 - use if image not wt req
        else if ( SelfServicePrintOption::SelfServOption_AfterPrintImage == scaleConfig_->scaleConfig.selfServeAfterPrintScreenState() )
        {
            // Standard Weight on a Platter required (Walmart MX) methods

            if( scaleConfig_->scaleConfig.selfServePrintScreenState()  == SelfServicePrintOption::SelfServOption_PrintToScreen )
            {
                // If we are Amazon GO (nobody is) then we scan label on screen and need a DONE button
                createLayout(
                takeLabelAndStartOver_,
                true,
                selfServiceWeightBar::capacityOnly,
                selfServiceButtonBar::showOkay );
            }
            else
            {
                // dont show metro info
                createLayout(
                takeLabelAndStartOver_,
                true,
                selfServiceWeightBar::capacityOnly );
            }
        }
        else
        {
            // Standard Print a label to a label and pull it and start over. Also BigY Option 1 bc for option 1 PrintToScreen is disabled

            if( scaleConfig_->scaleConfig.selfServePrintScreenState()  == SelfServicePrintOption::SelfServOption_PrintToScreen )
            {
                // If we are Amazon GO (nobody is) then we scan label on screen and need a DONE button
                createLayout(
                takeLabelAndStartOver_,
                true,
                selfServiceWeightBar::capacityOnly,
                selfServiceButtonBar::showOkay );
            }
            else
            {
                createLayout(
                takeLabelAndStartOver_,
                true,
                selfServiceWeightBar::capacityOnly );
            }
        }
    }

    break;

    case selfServiceHook::weightsMeasuresDialog:
        createLayout(
            weightsMeasures_ );
        break;

    case selfServiceHook::auditLogDialog:
        createLayout(
            auditLog_ );
        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::onHookReloadImages()
{
    background_.load( filesystem::absoluteFilePath( "background.png", folderType::SelfServeImages ) );

    updateBackground( online_ );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::onWeightBarPressed()
{
// for hobart's most esteemed and wise development staff we always want to offer up this handy dandy
// swell ability to simply go into Weights and Measures Mode and allow exit the rather forcefull 
// self service mode and return the poor, helpless developer to the previously scheduled scale application 
#ifdef QT_DEBUG
    // switch states to W&M dialog
    hook_->setCurrentState( selfServiceHook::weightsMeasuresDialog );
    return;
#endif

    // switch states to W&M dialog
    if ( !scaleConfig_->scaleConfig.selfServeWeightRequiredToSelectItem() )     // pjr383 - KEEP
        hook_->setCurrentState( selfServiceHook::weightsMeasuresDialog );
	
	// switch states to W&M dialog
    if ( !scaleConfig_->scaleConfig.selfServeEnablePlaylists() )                // pjr383 - KEEP
        hook_->setCurrentState( selfServiceHook::weightsMeasuresDialog );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::initialize()
{
    LOG_CLASS_INFO( "selfServiceDialog", "initialize()" );

    // Set up the hook to access ScaleConfiguration
    Session *s = hook_->session();
    scaleConfig_ = s->scaleConfig;

    // setup transparent background during child creation
    QPalette p( palette() );
    p.setColor( backgroundRole(), Qt::transparent );
    setPalette( p );

    // setup widgets
    attract_ = new hobart::selfserveStart( hook_, this );
    instruct_ = new hobart::selfserveInstruct( hook_, this );
	playlist_ = new hobart::selfservePlaylist( hook_, this );
    login_    = new hobart::selfserveLogin( hook_, this );
    itemEntryKeyed_ = new hobart::selfserveItemLookup( hook_, this );
    itemEntryImages_ = new hobart::selfserveImageLookup( hook_, this );
    byCountItem_ = new hobart::selfserveByCount( hook_, this );
    randomWeightItem_ = new hobart::selfserveRandomWeight( hook_, this );
    takeLabelAndStartOver_ = new hobart::selfserveFinish( hook_, this );
    weightsMeasures_ = new hobart::selfserveWeightsMeasures( hook_, this );
    auditLog_ = new hobart::selfserveAuditLog( hook_, this );

    connect( itemEntryKeyed_, SIGNAL( lookup() ), SLOT( onButtonBarEnter() ) );

    connect( byCountItem_, SIGNAL( print() ), SLOT( onPrintPressed() ) );        // added bc we added the ENTER and PRINT to the numpad and main screen area
    connect( byCountItem_, SIGNAL( sendConfig() ), SLOT( onSendConfigPressed() ) );
    connect( byCountItem_, SIGNAL( printError(int) ), SLOT( onPrintError(int) ) );
    connect( byCountItem_, SIGNAL( showBackButton() ), SLOT( onShowBackButton() ) );

    buttonBar_ = new selfServiceButtonBar( hook_, this );
    connect( buttonBar_, SIGNAL( cancel() ), SLOT( onButtonBarCancel() ) );
    connect( buttonBar_, SIGNAL( enter() ), SLOT( onButtonBarEnter() ) );
    connect( buttonBar_, SIGNAL( okay() ), SLOT( onButtonBarOkay() ) );

    weightBar_ = new selfServiceWeightBar( hook_, this );
    connect( weightBar_, SIGNAL( weightPressed() ), SLOT( onWeightBarPressed() ) );

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
void selfServiceDialog::createLayout( QWidget *w, bool showBar, int mode, int buttons )
{
    //LOG_CLASS_INFO( "selfServiceDialog", "createLayout" );

    if ( layout() )
        delete layout();

    hideAllChildren();

    QVBoxLayout *screen( new QVBoxLayout( this ) );
    screen->setContentsMargins( QMargins() );

    if ( showBar )
    {
        weightBar_->setCurrentMode( (selfServiceWeightBar::mode) mode );
        buttonBar_->showButtons( (selfServiceButtonBar::showButtonTypes) buttons );

        // PJR413 (SOF-4772) UNUSED If we are in Fixed PLU mode (Walmart CA),
        // place the weightBar above the UI, and center both the
        // weightBar and buttonBar
        // (Use Alternate option to put only the weightBar above the UI
        // but keep the buttonBar below it)
        //if( scaleConfig_->scaleConfig.selfServeFixedPlu() != "0" )
        //{
        //    QHBoxLayout *bar( new QHBoxLayout() );
        //    bar->setContentsMargins( QMargins() );
        //
        //    bar->addWidget( weightBar_, 0, Qt::AlignLeft );
        //    bar->addWidget( buttonBar_, 1 );
        //
        //    screen->addLayout( bar );
        //
        //    screen->addWidget( w, 1 );
        //    w->show();
        //}
        //Alternate
        //if ( scaleConfig_->scaleConfig.selfServeFixedPlu() != "0" )
        //{
        //    screen->addWidget( weightBar_, 1, Qt::AlignHCenter );
        //    screen->addWidget( w, 1 );
        //    w->show();
        //    screen->addWidget( buttonBar_, 1, Qt::AlignHCenter );
        //}
        //else
        //{
            screen->addWidget( w, 1 );
            w->show();

            QString dummy_plu = scaleConfig_->scaleConfig.selfServeFixedPlu();          // on init() these hooks were blank so used configs
            bool weigt_req = scaleConfig_->scaleConfig.selfServeWeightRequiredToSelectItem();
            if( ( dummy_plu != "0" ) && weigt_req )
            {
                activateWindow();
                raise();
            }

            QHBoxLayout *bar( new QHBoxLayout() );
            bar->setContentsMargins( QMargins() );

            bar->addWidget( weightBar_, 0, Qt::AlignLeft );
            bar->addWidget( buttonBar_, 1 );

            screen->addLayout( bar );
        //}

        if( checkScreenSize() > 10 )
        {
            //int screenWidth = checkScreenWidth();

            if( 1 == mode  )
            {
                if( scaleConfig_->scaleConfig.selfServeDisplayTareOnRandomWeight() )
                {
                    weightBar_->setGeometry( 0, 0, 1204, 120);
                    weightBar_->setFixedSize(1204,120);
                }
                else
                {
                    weightBar_->setGeometry( 0, 0, 904, 120);
                    weightBar_->setFixedSize(904,120);
                }

                weightBar_->show();
                //weightBar_->updateGeometry();
            }
            else
            {

                if( 0 == mode )
                {
                    weightBar_->setGeometry( 0, 0, 420, 120);
                    weightBar_->setFixedSize(420,120);
                }
                else if( 2 == mode )
                {
                    weightBar_->setFixedSize(904,120);
                    weightBar_->setGeometry( 0, 0, 904, 120);
                }
                else
                {
                    weightBar_->setFixedSize(0,0);
                    weightBar_->updateGeometry();
                }
                weightBar_->show();
            }
        }
        else
        {
            weightBar_->show();
            weightBar_->updateGeometry();
        }

        //PJR413 (SOF-4772) UNUSED Adjust size and padding of both bars
        //if( scaleConfig_->scaleConfig.selfServeFixedPlu() != "0" )
        //{
        //    weightBar_->setGeometry( 0, 0, 420, 120 );
        //    weightBar_->setFixedSize( 420, 120 );
        //    weightBar_->updateGeometry();
        //    buttonBar_->setGeometry( 0, 0, 904, 120 );
        //    buttonBar_->setFixedSize( 904, 120 );
        //}

        buttonBar_->show();
        buttonBar_->updateGeometry();
        //buttonBar_->setBackButton( false );     // on init, make it false (this was always commented out)
    }
    else
    {
        screen->addWidget( w, 1 );
        w->show();

        QString dummy_plu = scaleConfig_->scaleConfig.selfServeFixedPlu();          // on init() these hooks were blank so used configs
        bool weigt_req = scaleConfig_->scaleConfig.selfServeWeightRequiredToSelectItem();
        if( ( dummy_plu != "0" ) && weigt_req )
        {
            activateWindow();
            raise();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfServiceDialog::turnOffPrintToScreen()
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
bool selfServiceDialog::turnOnPrintToScreen()
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
void selfServiceDialog::hideAllChildren()
{
    attract_->hide();
    instruct_->hide();
    playlist_->hide();
    login_->hide();
    itemEntryKeyed_->hide();
    itemEntryImages_->hide();
    byCountItem_->hide();
    randomWeightItem_->hide();
    takeLabelAndStartOver_->hide();

    weightsMeasures_->hide();
    auditLog_->hide();

    buttonBar_->hide();
    weightBar_->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceDialog::updateBackground( bool online )
{
    // scale background to size
    QPixmap background( size() );
    background.fill( Qt::transparent );

    QPainter painter( &background );
    painter.setRenderHint( QPainter::Antialiasing, true );

    painter.drawPixmap( QPoint( 0, 0 ), background_.scaled( size() ) );

    // setup brush
    QBrush offlineBrush( offlineBackgroundColor_ );
    QBrush onlineBrush( background_.isNull() ? QBrush( onlineBackgroundColor_ ) : QBrush( background ) );

    // set background palette for online/offline
    QPalette p( palette() );
    p.setBrush( backgroundRole(), online ? onlineBrush : offlineBrush );
    setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceDialog::checkScreenSize()
{
    // laptop (1920x1080)
    int screen_size = 10;

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

        if( resolution_.width() > 1034 && resolution_.height() > 610 )  // gave it 10 more but do I need to?
            screen_size = 14;
    }

    return screen_size;
}
