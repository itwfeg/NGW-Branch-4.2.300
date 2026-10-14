/**
 * @file selfservicehook.cpp
 *
 * @copyright Copyright (c) 2023 Hobart Corporation
 * @author Joe Crew
 * @copied from Randy Blankley
 */

#include <logmanager.h>

#include "selfservicehook.h"
#include "selfservicehookerror.h"

#include "selfservice/selfservicedialog.h"
#include "selfservice/selfservicecamerapredictions.h"

#include "selfserviceGen2/selfservicedialogG2.h"
#include "selfserviceGen2/selfservicecamerapredictionsG2.h"

#include <logmanager.h>
#include <pixmap.h>
#include <filesystem.h>
#include <softkeyswidget.h>

#include <Messages.h>
#include <Session.h>
#include <ScaleConfiguration.h>
#include <ScaleClasses.h>

#include "../../source/HFC/CfgBaseFieldOffsets.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QDesktopWidget>
#include <QEvent>
#include <QFile>
#include <QTimer>

const QSize selfServiceHook::storeLogoSize_( 180, 100 );
const QSize selfServiceHook::pullLabelSize_( 640, 360 );

const static bool DEVELOPER10 = false;
const static bool DEVELOPER14 = true;

// these timers are used to fix focus issues by calling raise(),
// primarily when rebooting while in SS mode
QTimer *raiseTimer1;
QTimer *raiseTimer5;
QTimer *raiseTimer15;
QTimer *raiseTimer30;

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceHook::selfServiceHook( Session *s, QObject *parent ) :
    _Mybase( QString( "selfServiceHook" ), parent ),
    isScaleOnline_( true ),
    active_( false ),
    currentState_( (state) -1 ),
    prevState_( (state) -1 ),
    d_( NULL ),
    d_G2_( NULL ),
    scaleConfig_( NULL ),
    error_( NULL ),
    cameraPredictions_( NULL )
{
    LOG_CLASS_INFO( "selfServiceHook", "selfServiceHook" );

    LOG_INFO( "SELF SERVICE HOOK INSTALLED", NULL );

    // set pointer to session
    setSession( s );

    scaleConfig_ = s->scaleConfig;

    // initialize
    initialize();

    // change state
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        changeCurrentState( attractGen2 );
    else
        changeCurrentState( attract );

    // initialize timers
    raiseTimer1 = new QTimer( this );
    raiseTimer5 = new QTimer( this );
    raiseTimer15 = new QTimer( this );
    raiseTimer30 = new QTimer( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceHook::~selfServiceHook()
{
    if ( d_ )
    {
        delete d_;
        d_ = NULL;
    }
    else if( d_G2_ )
    {
        delete d_G2_;
        d_G2_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfServiceHook::activeOnStartup() const
{
    // FT-1735 Unable to Access Self Service - start tracking with 100,10,13 msg
    // changed this from looking for a file on the file system to just setting an
    // internalConfig variable upon receiving a 100,10,13 message. 13 = self service

    return scaleConfig_->internalConfig.ssactive_lock;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString selfServiceHook::currentLanguage() const
{
    return language_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceHook::state selfServiceHook::currentState() const
{
    return currentState_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceHook::state selfServiceHook::previousState() const
{
    return prevState_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::emitChangeLanguage()
{
    // reset idle timer due to activity
    resetIdleTimer();

    _Mybase::emitChangeLanguage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::emitResetPrimaryLanguage()
{
    _Mybase::emitResetPrimaryLanguage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::emitDialogButtonPress( int role )
{
    // reset idle timer due to activity
    resetIdleTimer();

    _Mybase::emitDialogButtonPress( role );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::emitPrint()
{
    // reset idle timer due to activity
    resetIdleTimer();

    // SOF-3950 PJR-357 Big Y Self Service Modifications
    if( selfServiceHook::takeLabel == currentState() )
    {
        if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && (SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors()))
        {
            // if we Printing ON the Take Label screen, send print command if user clicks it
            // Also we do 2 prints for these actions. Print 1 to display the label, Print 2 when user clicks Print
            // We need to set this hook so when we receive a 2nd print reply in operatemode.cpp, we dont do anything.
            // Test hook there, if 1, then set to 0 and exit out
            sessionHooks::instance()->updateFieldValue( sessionHooks::selfServicePrintDisplayingLabel, "1");
            session()->send( SessionDev, SnPrintKeyPressed );

            // Pressed Print Label on Print or Scan Screen (Finish Screen)
            if( scaleConfig_->internalConfig.clickedPrintLabelFromFinishScreen )
            {
                setCurrentState( takeLabel );
                return;
            }
            else
            {
                // reset button state and return home
                sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceKeyEntryStyle, "0");           // display the flashkeys(0) over lone keypad(1)
                setCurrentState( attract );
                return;
            }
        }
    }
    else if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        // reset button state and return home
        session()->send( SessionDev, SnPrintKeyPressed );
        sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceKeyEntryStyle, "0");           // display the flashkeys(0) over lone keypad(1)

        // Dont do this now.  Instead wait on prompt and dismissing of it.  Will we always get the prompt?
        // setCurrentState( itemEntryKeyedGen2 );
        return;
    }

    _Mybase::emitPrint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::emitSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature )
{
    // reset idle timer due to activity
    resetIdleTimer();

    if ( scaleConfig_->internalConfig.selfServicePreventClicks )
        return;

    scaleConfig_->internalConfig.selfServicePreventClicks = true;

    _Mybase::emitSoftkeyClicked( w, b, action, param, feature );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::emitShutdownKeyboards()
{
    // reset idle timer due to activity
    resetIdleTimer();

    _Mybase::emitShutdownKeyboards();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::emitZeroScale()
{
    // reset idle timer due to activity
    resetIdleTimer();

    _Mybase::emitZeroScale();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::resetByCountScreen()
{
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
            d_G2_->resetByCountNumpad();
    }
    else
    {
        if( d_ )
            d_->resetByCountNumpad();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::resetBackButton()
{
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        // CALLED BY:   selfserveByCountG2::onShowBackButton() -> this is the only thing that should make it visible. Otherwise its off
        // CALLS:       selfServiceDialogG2::onShowBackButton() which calls buttonbar->set Back Button visible
        if( d_G2_ )
            d_G2_->onShowBackButton();
    }
    else
    {

        // CALLED BY:   selfserveByCount::onShowBackButton() -> this is the only thing that should make it visible. Otherwise its off
        // CALLS:       selfServiceDialog::onShowBackButton() which calls buttonbar->set Back Button visible
        if( d_ )
            d_->onShowBackButton();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceHook::getKeypadStackState()
{
    int stackIndex = 0;
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
            stackIndex = d_G2_->getByCountStackState();         // didnt return it directly so I could debug with a breakpoint
    }
    else
    {
        if( d_ )
            stackIndex = d_->getByCountStackState();            // didnt return it directly so I could debug with a breakpoint
    }

    return stackIndex;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString selfServiceHook::getErrorStateDialogCode()
{
    if( d_G2_ )
        return d_G2_->getDialogBoxCode();
    else
        return "";
}

/// what is the style code of current dialog - if any

///////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceHook::getErrorStateDialogStyle()
{
    if( d_G2_)
        return d_G2_->getDialogBoxStyle();
    else
        return 5;			// OK button
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::emitViewAuditLog()
{
    // reset idle timer due to activity
    resetIdleTimer();

    _Mybase::emitAuditLog();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::enter()
{
    LOG_CLASS_INFO( "selfServiceHook", "enter" );

    // nothing to do
    if ( active_ )
    {
        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            if( d_G2_ )
            {
                d_G2_->showFullScreen();
                active_ = true;
                d_G2_->raise();
            }
        }
        else
        {
            if( d_ )
            {
                d_->showFullScreen();
                active_ = true;
                d_->raise();
            }
        }
        return;
    }

    LOG_INFO( "===== ENTER SELF SERVICE MODE =====", NULL );

	// dont log out Opers here. Causes focus to shift to Operator Name field in Operate Mode and can flip back to Operate Mode completely

    // enter self service mode
    // FT-1735 Unable to Access Self Service - start tracking with 100,10,13 msg
    session()->send( SessionDev, SnWriteWeighModeTransactionType, QString::number( OpModeSelfService ) );
    session()->send( SessionDev, SnReadWeighModeTransactionType );

    scaleConfig_->internalConfig.ssactive_lock = true;
    scaleConfig_->internalConfig.ssactive_tenative_lock = true;     // just keep it true tho its not used?
    session()->handleSelfServModeMsg( true );
    sessionHooks::instance()->updateFieldValue(sessionHooks::selfServiceModeActive, "1" );

    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
       // set some default flashkey properties
       DomSoftkeyGroup softkeyGroup;
       // Send: 136,86,888050,6,3,1,1
       softkeyGroup.device() = QString::number( SoftkeyDev );
       softkeyGroup.event() = QString::number( SaveSoftkeyGroup );
       softkeyGroup.setGroupNum( hobart::softkeysWidget::FreshServeFixedButtons );
       softkeyGroup.setHorizGridSize( 5 );
       softkeyGroup.setVertGridSize( 3 );
       softkeyGroup.setHorizSpacing( 1 );
       softkeyGroup.setVertSpacing( 1 );
       session()->send( softkeyGroup );

       // Send: 136,86,999017,7,2,10,10
       softkeyGroup.device() = QString::number( SoftkeyDev );
       softkeyGroup.event() = QString::number( SaveSoftkeyGroup );
       softkeyGroup.setGroupNum( hobart::softkeysWidget::SelfservCategoryResultsButtons );
       softkeyGroup.setHorizGridSize( 7 );
       softkeyGroup.setVertGridSize( 1 );
       softkeyGroup.setHorizSpacing( 1 );
       softkeyGroup.setVertSpacing( 1 );
       session()->send( softkeyGroup );

       scaleConfig_->gen2UIActive = true;
       sessionHooks::instance()->updateFieldValue( sessionHooks::selfServiceRestrictedDialog, "0");
       QString test =  sessionHooks::instance()->getFieldValue( sessionHooks::selfServiceRestrictedDialog );
       LOG_DEBUG( "Close dialog reset selfServiceRestrictedDialog = 0 as (%s)", qPrintable( test ) );
    }

    // need to enable the mainWindow or self service will be locked out too
    Q_EMIT receiveLockDownScreenMessage( false );     // false means no longer isLocked

    // remove the dialog telling the user we are coming here
    session()->send( Error, ErrRemoveAnyModalDialogDevice, "100056, 3");

    // initial state is attract
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        changeCurrentState( attractGen2, true );
    else
        changeCurrentState( attract, true );

    // show!!

    //QDesktopWidget *desktop;
    //QSize size( 1024, 600 );

    QDesktopWidget *desktop;
    QSize size( 1366, 768 );

    if ( (desktop = QApplication::desktop()) )
    {
        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            if( d_G2_ )
                size = desktop->screenGeometry( d_G2_ ).size();
        }
        else
        {
            if( d_ )
                size = desktop->screenGeometry( d_ ).size();
        }
    }

#ifdef QT_DEBUG
    if( DEVELOPER10 )
    {
        size = QSize( 1024, 600 );
        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        d_->setFixedSize( size ); // FT-1385
        d_->move(200, 200);
    }
    else if( DEVELOPER14 )
    {
        // This is to display the self service on Windows in the correct size and location
        size = QSize( 1366, 768 );
        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            if( d_G2_ )
            {
                d_G2_->setFixedSize( size );    // FT-1385
                d_G2_->move(1, 1);              //d_->move(200, 200);
            }
        }
        else
        {
            if( d_ )
            {
                d_->setFixedSize( size );   // FT-1385
                d_->move(1, 1);             //d_->move(200, 200);
            }
        }
    }
    else
    {
        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            if( d_G2_ )
            {
                d_G2_->setFixedSize( size );   // FT-1385
                d_G2_->showFullScreen();
            }
        }
        else
        {
            if( d_ )
            {
                d_->setFixedSize( size );   // FT-1385
                d_->showFullScreen();
            }
        }
    }
#else
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
        {
            d_G2_->setFixedSize( size ); // FT-1385
            d_G2_->showFullScreen();
        }
    }
    else
    {
        if( d_ )
        {
            d_->setFixedSize( size ); // FT-1385
            d_->showFullScreen();
        }
    }

#endif

    active_ = true;

    // process!
    Q_EMIT activeChanged();


    // We need to determine if there was a weight on the platter when we came into self service mode.
    // Right now we only leave the attract screen if we receive a weigher status 1,4.  Well there
    // may have already been a 1,4 received, so call the same methods and test the 1,4
    // WEH-93 Entering self-serve mode with a weight on the platter doesn't proceed to the item selection page
    // Well then check to see if there is a weight already there.
    QString wt_data = scaleConfig_->weightData.netWeight();
    sessionHooks::instance()->updateFieldValue(sessionHooks::netWeight, wt_data);

    // reset the idle timer
    resetIdleTimer();

    // start timers
    raiseTimer1->setSingleShot( true );
    raiseTimer5->setSingleShot( true );
    raiseTimer15->setSingleShot( true );
    raiseTimer30->setSingleShot( true );

    connect( raiseTimer1, SIGNAL(timeout()), this, SLOT(raiseTimeout()) );
    connect( raiseTimer5, SIGNAL(timeout()), this, SLOT(raiseTimeout()) );
    connect( raiseTimer15, SIGNAL(timeout()), this, SLOT(raiseTimeout()) );
    connect( raiseTimer30, SIGNAL(timeout()), this, SLOT(raiseTimeout()) );

    raiseTimer1->start( 1000 );
    raiseTimer5->start( 5000 );
    raiseTimer15->start( 15000 );
    raiseTimer30->start( 30000 );

    LOG_INFO( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::leave()
{
    LOG_CLASS_INFO( "selfServiceHook", "leave" );

    // nothing to do
    if ( !active_ )
        return;

    LOG_INFO( "===== LEAVE SELF SERVICE MODE =====", NULL );

    idleTimer_->stop();

    // exit self service mode
    // FT-1735 Unable to Access Self Service - start tracking with 100,10,13 msg

    session()->send( SessionDev, SnWriteWeighModeTransactionType, QString::number( OpModeManualWeigh ) );
    session()->send( SessionDev, SnReadWeighModeTransactionType );

    scaleConfig_->gen2UIActive = false;

    // turn off the scanner
    session()->send( BarcodeScanner, ScannerDisabled );             // Disable the scanner on the way out

    // send message to log out any operators, if there are any
    if( scaleConfig_->getIsOperatorLoggedIn() )
        session()->send( OperatorDev, OpLogout );

    // ORIGINAL TEST: if any BigY option AND not Amazon GO.  WHAT? Why?  And if leaving Self Serv - Why do I want to turn printing to the screen ON???????
    // The test now states if printing any label to the screen, turn it on. I dont get it.
    //if ( scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() > SelfServicePrintOption::SelfServOption_PrintToLabel )
    if(  scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() && !scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() )
    {
        QString param( "," + QString::number( fieldOffsetCfgSelfServePrintToScreen ) );
        param += ",1,0";
        session()->send( ConfigurationDev, CfgSaveSingleField, param );
    }

    scaleConfig_->internalConfig.ssactive_lock = false;
    scaleConfig_->internalConfig.ssactive_tenative_lock = false;
    session()->handleSelfServModeMsg( false );
    sessionHooks::instance()->updateFieldValue(sessionHooks::selfServiceModeActive, "0" );

    resetPrimaryLanguage();
    // go dormant
    active_ = false;

    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
            d_G2_->hide();
    }
    else
    {
        if( d_ )
            d_->hide();
    }

    // disable the timers if they are still going
    raiseTimer1->stop();
    raiseTimer5->stop();
    raiseTimer15->stop();
    raiseTimer30->stop();

    // process!
    Q_EMIT activeChanged();

    LOG_INFO( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::sendSurrogateMsg(int device, int event)
{
    // Similar to Client.send in the previous mom. Forwards the message on to the socket to send to the Application
    session()->send( QString( QString::number(device) + "," + QString::number(event) ) );
}

void selfServiceHook::sendSurrogateMsg( int device, int event, const QString& msgBody )
{
    // Similar to Client.send in the previous mom. Forwards the message on to the socket to send to the Application
    session()->send( QString( QString::number(device) + "," + QString::number(event) + "," + msgBody ) );
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::resetIdleTimer()
{
    idleTimer_->setSingleShot( true );
    idleTimer_->start( timeoutT_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::setCurrentState( state value )
{
    LOG_CLASS_INFO( "selfServiceHook", "setCurrentState" );

    // translate state
    value = mappedState( value );

    if ( scaleConfig_->scaleConfig.enableUseCharacterSearching() )
    {
        // This is used to do character by character searches. This is used to do a keyword search on a word
        // that we know won't be a product description. This effectively clears the grid of all items.
        if( value == itemEntryImages )
            session()->send( SoftkeyDev, ExecuteSoftkey, "39,Joberoni," + QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );
    }

    // check for valid state change
    // check for dialog state to any state
    // check for any state to dialog states
    //state test1 = currentState();
    if (( allowedStateChanges_.contains( currentState(), value ) ) ||
        ( isDialogState( currentState() ) ) || ( isDialogState( value ) ))
    {
        changeCurrentState( value );
    }
    else if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
    {
        changeLookOfCurrentState( value );
    }
    else
    {
        LOG_WARN( "invalid state change %d to %d!", currentState(), value );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::setPreviousState( state value )
{
    changePreviousState( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfServiceHook::isDialogState( state value ) const
{
    return dialogStates_.contains( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfServiceHook::isErrorState() const
{
    QMutexLocker guard( &errorMutex_ );
    return ( error_ ? true : false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceHook::state selfServiceHook::mappedState( state value ) const
{
    // check meta states
    if ( previous == value )
        return previousState();
    else if ( ( attract == value )  && ( fieldValue( sessionHooks::selfServiceDisableAttract ).toInt() ))
        return startScreenState();
    else if( attractGen2 == value )
        return startScreenState();
    else if ( startScreen == value )
        return startScreenState();

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceHook::state selfServiceHook::startScreenState() const
{
    LOG_CLASS_INFO( "selfServiceHook", "startScreenState" );

    //bool test1 = scaleConfig_->scaleConfig.selfServeEnableFreshServeUI();
    //int test2 = fieldValue( sessionHooks::selfServiceDisableAttract ).toInt();

    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
            d_G2_->resetNumPadEntry();

        // enter attract screen when zero weight
        if ( "1" == fieldValue( sessionHooks::attractWhenZeroWeight ) )
            if ( SZ_Yes == isScaleZero() )
                return attractGen2;

        //return attractGen2;

        // not active, start screen is attract screen (if enabled)
        //if ( !active() )
        //    return attractGen2;
    }
    else
    {
        if( d_ )
            d_->resetNumPadEntry();

        // not active, start screen is attract screen (if enabled)
        if ( !active() )
            if ( !fieldValue( sessionHooks::selfServiceDisableAttract ).toInt() )
                return attract;

        // enter attract screen when zero weight
        if ( "1" == fieldValue( sessionHooks::attractWhenZeroWeight ) )
            if ( SZ_Yes == isScaleZero() )
                return attract;
    }



    // SOF-4624 Home Panel setting affect Self-Service mode and it should not
    // sessionHooks::selfServiceStartScreen is checking an OperateMode Center Panel, Left Panel, or Cut Test start location,
    // not SS Start Screen location. But if we always set the startScreen variable to itemEntryKeyed, in
    // method onCurrentStateChanged() in case onCurrentcase selfServiceHook::itemEntryKeyed
    // it will decide the proper place to return to START based on all of the new PJR383 configurations
    // QString startScreen( fieldValue( sessionHooks::selfServiceStartScreen ) );
    QString startScreen( "keyed" );

    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if ( "keyed" == startScreen )
            return itemEntryKeyedGen2;                  // itemEntryKeyed for Gen1
        else if ( "softkey" == startScreen )
            return itemEntryImagesGen2;                 // for gen2
    }
    else
    {
        if ( "keyed" == startScreen )
            return itemEntryKeyed;
        else if ( "softkey" == startScreen )
            return itemEntryImages;
    }

    LOG_WARN( "unknown start screen %s", qPrintable( startScreen ) );
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        return itemEntryKeyedGen2;
    else
        return itemEntryKeyed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onBeginPrinting( const QString &filename )
{
    // PJR-82
    setFieldValue( sessionHooks::selfServiceBarcodeFile, filename );

    // change state
    changeCurrentState( takeLabel );

    _Mybase::onBeginPrinting( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onBeginBarcodeOnlyPrinting( const QString &filename )
{
    // PJR-82
    setFieldValue( sessionHooks::selfServiceBarcodeOnlyFile, filename );

    // forward message onto dialog
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        d_G2_->renderBarcodeOnlyLabel();
    else
        d_->renderBarcodeOnlyLabel();

    _Mybase::onBeginBarcodeOnlyPrinting( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onDismissDialog()
{
    QMutexLocker guard( &errorMutex_ );

    // hide and delete widget
    if ( error_ )
    {
        error_->hide();
        error_->deleteLater();

        resetIdleTimer();
    }

    if ( cameraPredictions_ )
    {
        cameraPredictions_->hide();
        cameraPredictions_->deleteLater();

        resetIdleTimer();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onEndPrinting()
{
    // change state
    changeCurrentState( startScreen );
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
            d_G2_->resetNumPadEntry();
    }
    else
    {
        if( d_ )
            d_->resetNumPadEntry();
    }

    _Mybase::onEndPrinting();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onLoadSoftkeyGroup( const DomLoadSoftkeyGroup* m )
{
    // reset idle timer due to activity
    if ( hobart::softkeysWidget::SelfservCategoryResultsButtons == m->featureNum() )
        resetIdleTimer();

    // forward message onto dialog
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
            d_G2_->loadSoftkeyGroup( m );
    }
    else
    {
        if( d_ )
            d_->loadSoftkeyGroup( m );
    }

    _Mybase::onLoadSoftkeyGroup( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onLoadCurrentState()
{
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        // change state
        if ( selfServiceHook::auditLogDialog == currentState() )
            changeCurrentState( selfServiceHook::auditLogDialogGen2 );
        else
            changeCurrentState( selfServiceHook::weightsMeasuresDialogGen2 );
    }
    else
    {
        // change state
        if ( selfServiceHook::auditLogDialog == currentState() )
            changeCurrentState( selfServiceHook::auditLogDialog );
        else
            changeCurrentState( selfServiceHook::weightsMeasuresDialog );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onUpdateStartState()
{
    changeCurrentState( selfServiceHook::startScreen );

    // will this do the same as this?
    //hook_->setCurrentState( selfServiceHook::startScreen );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onNetworkConnectivity( const MadpNetworkConnectivity *m )
{
    // forward message onto dialog
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( "1" == scaleConfig_->normalScaleOperation.checkNetConnectivity() )
            isScaleOnline_ = m->isNetworkConnected() ? true: false;
        else
            isScaleOnline_ = true;                      // always online if not testing for it

        if( d_G2_ )
            d_G2_->networkConnectivity( m );
    }
    else
    {
        if( d_ )
            d_->networkConnectivity( m );
    }

    _Mybase::onNetworkConnectivity( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onNetworkDeviceType( const MadpNetworkDeviceType *m )
{
    // forward message onto dialog
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
            d_G2_->networkDeviceType( m );
    }
    else
    {
        if( d_ )
            d_->networkDeviceType( m );
    }

    _Mybase::onNetworkDeviceType( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onRemoveCameraPredictionsDialog()
{
    // do nothing when not active
    if ( !active() )
        return;

    // ----- //

    QMutexLocker guard( &errorMutex_ );

    // hide and delete widget
    if ( cameraPredictions_ )
    {
        cameraPredictions_->hide();
        cameraPredictions_->deleteLater();

        resetIdleTimer();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onShowDialog( const QString& title, const QString& message, const buttonMap& buttons )
{
    // do nothing when not active
    if ( !active() )
        return;

    // ----- //

    QMutexLocker guard( &errorMutex_ );

    if ( error_ )
    {
        error_->hide();
        error_->deleteLater();
    }

    if ( cameraPredictions_ )
    {
        cameraPredictions_->hide();
        cameraPredictions_->deleteLater();
    }

    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {

        selfServiceHookError *popup( new selfServiceHookError( title, message, buttons, d_G2_ ) );
        error_ = popup;

        connect( popup, SIGNAL( dialogButtonPress(int) ), SIGNAL( dialogButtonPress(int) ) );
        connect( popup, SIGNAL( destroyed(QObject*) ), SLOT( onDestroyed(QObject*) ) );

        guard.unlock();

        // ----- //
        if( d_G2_ )
        {
            popup->setFixedSize( d_G2_->size() );
            popup->raise();

            popup->show();

            d_G2_->dimBackground();
        }
    }
    else
    {
        selfServiceHookError *popup( new selfServiceHookError( title, message, buttons, d_ ) );
        error_ = popup;

        connect( popup, SIGNAL( dialogButtonPress(int) ), SIGNAL( dialogButtonPress(int) ) );
        connect( popup, SIGNAL( destroyed(QObject*) ), SLOT( onDestroyed(QObject*) ) );

        guard.unlock();

        // ----- //

        popup->setFixedSize( d_->size() );
        popup->raise();

        popup->show();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onShowCameraPredictionsDialog( const DomLoadSoftkeyGroup *m )
{
    // do nothing when not active
    if ( !active() )
        return;

    // ----- //

    QMutexLocker guard( &errorMutex_ );

    // when error on screen dont do anything.. errors trump camera predictions
    if ( error_ )
        return;

    if ( cameraPredictions_ )
    {
        cameraPredictions_->hide();
        cameraPredictions_->deleteLater();
    }

    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
        {
            selfServiceCameraPredictionsG2 *popup( new selfServiceCameraPredictionsG2( this, m, d_G2_ ) );
            cameraPredictions_ = popup;

            connect( popup, &selfServiceCameraPredictionsG2::productSearchRequested, this, &_Myt::onProductSearchRequested );
            connect( popup, &selfServiceCameraPredictionsG2::softkeyClicked, this, &_Myt::softkeyClicked );
            connect( popup, &selfServiceCameraPredictionsG2::destroyed, this, &_Myt::onDestroyed );

            guard.unlock();

            // ----- //

            //popup->setFixedSize( d_G2_->size().width() - 300, d_G2_->size().height() - 150 );
            popup->move( QPoint( 253, 52 ) );
            popup->raise();

            popup->show();
            // updates camera predictions but I think Im not going to do that popup
            popup->updateNetworkConnectivity( isScaleOnline_ );
            d_G2_->dimBackground();
        }
    }
    else
    {
        if( d_ )
        {
            selfServiceCameraPredictions *popup( new selfServiceCameraPredictions( this, m, d_ ) );
            cameraPredictions_ = popup;

            connect( popup, &selfServiceCameraPredictions::productSearchRequested, this, &_Myt::onProductSearchRequested );
            connect( popup, &selfServiceCameraPredictions::softkeyClicked, this, &_Myt::softkeyClicked );
            connect( popup, &selfServiceCameraPredictions::destroyed, this, &_Myt::onDestroyed );

            guard.unlock();

            // ----- //

            popup->setFixedSize( d_->size() );
            popup->raise();

            popup->show();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onUpdateCurrentItem( itemType type )
{
    LOG_CLASS_INFO( "selfServiceHook", "onUpdateCurrentItem" );

    state s( currentState() );

    if ( sessionHooks::randomWeightItem == type )
    {
        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            changeCurrentState( processRandomWeightItemGen2 );
        }
        else
        {
            if ( takeLabel != s )
                changeCurrentState( processRandomWeightItem );
        }
    }
    else if ( sessionHooks::forcedByCount == type )
    {
        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            changeCurrentState( processByCountItemGen2 );
        }
        else
        {
            if ( takeLabel != s )
                changeCurrentState( processByCountItem );
        }
    }
    else if ( sessionHooks::none == type )
    {
        // when in dialog state use previous state instead of current state
        if ( isDialogState( s ) )
            s = previousState();

        // when in item entry states there isn't an item in the system so don't switch
        // states or flicker may be introduced
        // do not switch out of take label state due to items
        if (( attract == s )                || ( attractGen2 == s ) ||
            ( itemEntryKeyed == s )         || ( itemEntryKeyedGen2 == s ) ||
            ( itemEntryImages == s )        || ( itemEntryImagesGen2 == s ) ||
            ( itemEntryImagesViewAllGen2 == s ) ||
            ( takeLabel == s ))
        {
            // do nothing
            LOG_WARN( "type %d so do nothing", type );
        }
        else
        {
            changeCurrentState( startScreen );
        }
    }
    else
    {
        LOG_WARN( "unknown item type %d", type );
        changeCurrentState( startScreen );
    }

    _Mybase::onUpdateCurrentItem( type );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onUpdateCurrentLanguage( const QString& language )
{
    // save language
    language_ = language;

    // translate ui
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
            d_G2_->translate();
    }
    else
    {
        if( d_ )
            d_->translate();
    }

    _Mybase::onUpdateCurrentLanguage( language );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onUpdateCurrentPlaylist( const QString& playlist )
{
    // save language
    playlist_ = playlist;

    // translate ui
    d_->refreshPlaylist( playlist );

    _Mybase::onUpdateCurrentPlaylist( playlist );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onUpdateCurrentErrDialog( const QString& code , int buttonstyle )
{
    // save error code
    code_ = code;
    style_ = buttonstyle;

    // translate ui
    if( d_G2_ )
        d_G2_->refreshDialog( code, buttonstyle );

    _Mybase::onUpdateCurrentErrDialog( code_, style_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onDismissCurrentErrDialog(const QString& code , int buttonstyle)
{
    // translate ui
    if( d_G2_ )
        d_G2_->dismissErrorDialog( code, buttonstyle );

    if( ( PrinterErrors::PrinterErrTakeLabel == code.toInt() ) || ( PrinterErrors::PrinterErrTakeLabelBeep == code.toInt()  ) )
        setCurrentState( itemEntryKeyedGen2 );

    _Mybase::onDismissCurrentErrDialog( code, buttonstyle );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onDismissCurrentErrDialogWithAnyCode()
{
    // translate ui
    if( d_G2_ )
        d_G2_->dismissErrorDialogNoCode();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onShowErrDialog()
{
    // translate ui
    if( d_G2_ )
        d_G2_->showErrorDialog();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onHideErrDialog()
{
    // translate ui
    if( d_G2_ )
        d_G2_->hideErrorDialog();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onUpdateFieldValue( fieldType type, const QString& value )
{
    //LOG_CLASS_INFO( "selfServiceHook", "onUpdateFieldValue" );

    _Mybase::onUpdateFieldValue( type, value );

    // handle update
    switch ( type )
    {
    // capacity info
    case sessionHooks::capacityIndicator:
    case sessionHooks::capacityIndicatorString:
    case sessionHooks::capacityString:
    case sessionHooks::dualCapacityIndicator:
    case sessionHooks::dualCapacityIndicatorString:
    case sessionHooks::netWeight:
    case sessionHooks::netWeightString:
    case sessionHooks::zeroIndicator:
    case sessionHooks::zeroIndicatorString:

    // weight/item info
    // case sessionHooks::itemByCount:              // MOVED TO BOTTOM
    case sessionHooks::itemExceptionPrice:
    case sessionHooks::itemHasExceptionPricing:
    case sessionHooks::itemTare:
    case sessionHooks::itemTotalPrice:
    case sessionHooks::itemUnitPrice:
    case sessionHooks::itemUnitPriceCountFor:
    case sessionHooks::itemUnitPriceFractional:
    case sessionHooks::itemUnitPricePoundsFor:

        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            if( d_G2_ )
                d_G2_->refreshWeight();
        }
        else
        {
            if( d_ )
                d_->refreshWeight();
        }
        break;

    // item info
    case sessionHooks::itemCode:
    case sessionHooks::itemDescription:
    //case sessionHooks::itemByCount:           // MOVED TO BOTTOM: The bycount was getting pushed into the netweight field. brought it down here from above.
    case sessionHooks::itemDiscountedSavings:   // I can actually see the entire block under bycount up above having to come down here too
    case sessionHooks::itemDiscountedTotalPrice:
    case sessionHooks::itemDiscountedUnitPrice:
    case sessionHooks::itemHasFreqShopperPricing:
    case sessionHooks::itemExpandedText:
    case sessionHooks::itemImage:
    case sessionHooks::itemNutrifactsPanel:

        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            if( d_G2_ )
                d_G2_->refreshItem();
        }
        else
        {
            if( d_ )
                d_->refreshItem();
        }
        break;

    // settings
    case sessionHooks::autoLookupProduct:
    case sessionHooks::autoLookupNumOfDigits:
    case sessionHooks::autoLookupOverrideDigit:
    case sessionHooks::primaryLanguage:
    case sessionHooks::numberPadStyle:
    case sessionHooks::secondaryLanguage:
    case sessionHooks::selfServiceKey:
    case sessionHooks::selfServiceShowBestSellers:
    case sessionHooks::serviceKey:
    case sessionHooks::supervisorKey:
    case sessionHooks::weightsAndMeasuresInfo:
	case sessionHooks::selfServiceTareFlashkeys:
    case sessionHooks::selfServiceKeyEntryStyle:

        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            if( d_G2_ )
                d_G2_->refreshSettings();
        }
        else
        {
            if( d_ )
                d_->refreshSettings();
        }
        break;

        // SOF-5710 5.2.100: Keypad entries for by count quantity other than 1 are blanked out in keypad
        // what fixed the bycount field for G2 and ghost bycount values totally wrecks regular bycounts
        // because we have the area that shares the bycount value with the instructions where for G2 the
        // instructions are above the field. Each bycount needs it's own update.  TEST TEST TEST
    case sessionHooks::itemByCount:
        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            if( d_G2_ )
                d_G2_->refreshItem();
        }
        else
        {
            if( d_ )
                d_->refreshWeight();
        }
        break;

    default:
        break;
    }

    // reset idle timer for weight changes
    if ( sessionHooks::netWeight == type )
        resetIdleTimer();

    if ( active() )
    {
        //LOG_INFO( "active is TRUE", NULL );

        if (( sessionHooks::zeroIndicator == type ) || ( sessionHooks::netWeight == type ))
        {
            //LOG_INFO( "sessionHooks::zeroIndicator == type || sessionHooks::netWeight == type", NULL );

            QString testNetWt = fieldValue( sessionHooks::netWeight );
            if( testNetWt.contains( " " ) )
            {
                // need to get rid of the units lb/kg - everything past a space
                testNetWt = testNetWt.mid(0, testNetWt.indexOf(" "));
            }

            float testerWt = 0;
            if( testNetWt.length() > 0)
            {
                testerWt = testNetWt.toFloat();
            }

            bool z = true;
            if ( "0" == fieldValue( sessionHooks::zeroIndicator )  && ( testerWt > 0.005 ) )
                z = false;

            // when scale zero remove any item predictions from screen
            if ( sessionHooks::zeroIndicator == type )
                if ( "1" == fieldValue( sessionHooks::zeroIndicator ) )
                    onRemoveCameraPredictionsDialog();

            if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
            {
                if( d_G2_ )
                {
                    if ( attractGen2 == currentState() )
                    {
                        if ( z == false )
                            setCurrentState( itemEntryKeyedGen2 );
                    }
                }
            }
            else if ( scaleConfig_->scaleConfig.selfServeWeightRequiredToSelectItem() || scaleConfig_->scaleConfig.selfServeEnablePlaylists() )     // pjr383 - KEEP
            {
                // so after much thought and test cases and trying different configurations and reviewing previous code and even more tests, I think
                // I am at the point where really all references to weight on a bycount only when you are on the start screen and when you add a weight.
                // Almost all other dependencies have been removed at this point.  Including now you can even click this screen.  So really, all I want these
                // to do is if you put a weight on the screen, at start, you can leave it.  That's it. In the past depending on certain conditions of having
                // no weight you would be forced back here.  Like if Weight was required, and you lifted it off while on the Lookup Screen, and you entered a
                // PLU, since weight was required and it was removed, you were forced back to 3-image screen.  All of that has now been gutted.

                // leave attract screen when stable non-zero weight

                if ( ( attractGen2 == currentState() ) || ( attract == currentState() )  )
                {
                    if ( z == false )
                        setCurrentState( startScreen );
                }
            }

 /*
            // SOF-4350 "Print Label" self-serve configuration has errors and won't print By Count items
            // Once we test this a little more and it passes the test, delete all of this block
            Im also going to leave this code here in case I find a case to print a piece or all of it back for some reason.
            if( "1" == fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ) )
            {
                if ( attract == currentState() )
                    if ( z == false )
                        setCurrentState( itemEntryKeyed );

            }
            else if ( scaleConfig_->scaleConfig.selfServeWeightRequiredToSelectItem() || scaleConfig_->scaleConfig.selfServeEnablePlaylists() )     // pjr383 - KEEP
            {
                // leave attract screen when stable non-zero weight
                if ( attract == currentState() )
                {
                    if ( z == false )
                        setCurrentState( startScreen );
                }
                else
                {
                    //LOG_INFO( "attract != currentState() ", NULL );

                    if ( z == true )
                    {
                        //LOG_INFO( "z == true so  session()->send( ProductDev, ProductRead, 0,3 )", NULL );
                        session()->send( ProductDev, ProductRead, "0,3" );
                        setCurrentState( attract );
                    }
                }
            }
*/
        }
    }
    // update
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
            d_G2_->update();
    }
    else
    {
        if( d_ )
            d_->update();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onDestroyed( QObject *obj )
{
    if (( error_ == obj ) || ( cameraPredictions_ == obj ))
    {
        QMutexLocker guard( &errorMutex_ );

        if ( error_ == obj )
            error_ = NULL;
        if ( cameraPredictions_ == obj )
        {
            cameraPredictions_ = NULL;
            if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
            {
                if( d_G2_ )
                    d_G2_->resetBackground();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onFileMonitorTimerTimeout()
{
    bool changed( false );

    // check for file different
    for ( fileHashMap::iterator i = fileHashTable_.begin(); i != fileHashTable_.end(); ++i )
    {
        const QByteArray hash( generateFileHash( i.key() ) );

        // different!
        if ( hash != i.value() )
        {
            i.value() = hash;
            changed = true;
        }
    }

    // nothing to do
    if ( !changed )
        return;

    // load store logo from filesystem
    loadStoreLogo();

    // load the pull label image from the filesystem
    loadPullLabel();

    // reload images
    Q_EMIT reloadImages();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onUpdateSaveMessage()
{
    Q_EMIT emitUpdateSaveMessage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onUpdateAuditLogList(const DomAuditLogInformation *m)
{
    Q_UNUSED(m);

    Q_EMIT emitUpdateAuditLogList();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onIdleTimerTimeout()
{
    LOG_CLASS_INFO("selfServiceHook", "onIdleTimerTimeout");

    // ignore idle timer in these states
    if (( attract == currentState() ) ||
        ( attractGen2 == currentState() ) ||
        ( takeLabel == currentState() ) ||
        ( isErrorState() ) ||
        ( scaleConfig_->scaleConfig.selfServeFixedPlu() != "0" ))        // PJR413 (SOF-4772) - For testing purposes, this is also ignored if we have a Fixed PLU
        return;

    // these states need to dump active item
    if (( processRandomWeightItem == currentState() ) || ( processByCountItem == currentState() ))
    {
        emitChangeFieldValue( sessionHooks::itemCode, "0" );
    }

    // these states need to dump active item
    if (( processRandomWeightItemGen2 == currentState() ) || ( processByCountItemGen2 == currentState() ))
    {
        emitChangeFieldValue( sessionHooks::itemCode, "0" );
    }

    // SOF-4365 Self-Serve character by character Search does not time out if there is a weight on the platter
    // Added the Search Screen as one of these states that time out when there is a weight on it no matter what.  Not too many states left.
    if( itemEntryImages == currentState() )
    {        
		changeCurrentState(attract);
        emitResetPrimaryLanguage();
    }
    else if ( isDialogState( currentState() ) )
    {
        // FT-1206: dialog states timeout to previous state
        changeCurrentState( previous );
    }
    else
    {
        // do not timeout when weight on platter and videos play on attract screen

        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            if ( SZ_No == isScaleZero() &&  ( itemEntryKeyedGen2 == currentState() )  )
            {
                resetIdleTimer();
                return;                 // stays on Flashkey page when there is a weight on the screen
            }
            else if( SZ_No == isScaleZero() )
            {
                resetIdleTimer();
                changeCurrentState( itemEntryKeyedGen2 );
                return;
            }

            // then must be on main flashkey screen with no weight so timeout to start
            resetIdleTimer();
            changeCurrentState( attractGen2 );
            emitResetPrimaryLanguage();
            return;
        }
        else if ( "1" == fieldValue( sessionHooks::attractWhenZeroWeight ) )
        {
            if ( SZ_Yes != isScaleZero() && (scaleConfig_->scaleConfig.selfServeWeightRequiredToSelectItem() == true
                 || scaleConfig_->scaleConfig.selfServeEnablePlaylists() == true) )
            {
                resetIdleTimer();
                return;                 // stays on Flashkey page when there is a weight on the screen
            }
            else if ( SZ_Yes != isScaleZero() && scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
            {
                resetIdleTimer();
                return;                 // stays on Flashkey page when there is a weight on the screen
            }
        }

        if( scaleConfig_->getIsOperatorLoggedIn() )
            session()->send( OperatorDev, OpLogout );

        changeCurrentState( attract );
        emitResetPrimaryLanguage();
    }
    scaleConfig_->internalConfig.selfServicePreventClicks = false;

    // SOF-5069: if number pad entry, clear on timeout
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        if( d_G2_ )
            d_G2_->resetNumPadEntry();
    }
    else
    {
        if( d_ )
            d_->resetNumPadEntry();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::onProductSearchRequested()
{
    // search not enabled
    if ( SelfServicePrintOption::SelfServOption_SearchNone == scaleConfig_->scaleConfig.selfServeSearchScreenState() )
        return;

    setCurrentState( selfServiceHook::itemEntryImages );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool selfServiceHook::eventFilter( QObject *obj, QEvent *e )
{
    if ( d_ == obj )
    {
        // every time user presses screen reset the idle timer
        if ( QEvent::MouseButtonRelease == e->type() )
            resetIdleTimer();

        // when screen resizes we must resize error also
        else if ( QEvent::Resize == e->type() )
        {
            if (( error_ ) || ( cameraPredictions_ ))
            {
                QMutexLocker guard( &errorMutex_ );

                if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
                {
                    if ( d_G2_ && error_ )
                        error_->setFixedSize( d_G2_->size() );
                    if ( d_G2_ && cameraPredictions_ )
                        cameraPredictions_->setFixedSize( d_G2_->size() );
                }
                else
                {
                    if ( d_ && error_ )
                        error_->setFixedSize( d_->size() );
                    if ( d_ && cameraPredictions_ )
                        cameraPredictions_->setFixedSize( d_->size() );
                }
            }
        }
    }

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::loadStoreLogo()
{
    LOG_CLASS_INFO( "selfServiceHook", "loadStoreLogo" );

    QString storeLogoName( filesystem::absoluteFilePath( "storelogo.png", folderType::SelfServeImages ) );

    if ( !QFile::exists( storeLogoName ) )
        LOG_WARN( "store logo '%s' does not exist!", qPrintable( storeLogoName ) );
    else
    {
        QPixmap pixmap( storeLogoName );

        if ( pixmap.isNull() )
            LOG_WARN( "error loading store logo '%s'!", qPrintable( storeLogoName ) );
        else
        {
            // save image for later use
            sizeToFitPixmap stretcher( storeLogoSize_ );
            storeLogo_ = stretcher( pixmap );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::loadPullLabel()
{
    LOG_CLASS_INFO( "selfServiceHook", "loadPullLabel" );

    QString pullLabelName( filesystem::absoluteFilePath( "pulllabel.png", folderType::SelfServeImages ) );

    if ( !QFile::exists( pullLabelName ) )
        LOG_WARN( "pull label image '%s' does not exist!", qPrintable( pullLabelName ) );
    else
    {
        QPixmap pixmap( pullLabelName );

        if ( pixmap.isNull() )
            LOG_WARN( "error loading pull label image '%s'!", qPrintable( pullLabelName ) );
        else
        {
            // save image for later use
            sizeToFitPixmap stretcher( pullLabelSize_ );
            pullLabel_ = stretcher( pixmap );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::initialize()
{
    // here are the files we will be monitoring
    QStringList files;

    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {
        files.append( "attract_G2.png" );

        QString lang = scaleConfig_->scaleConfig.languageCode().toUpper() + "." + scaleConfig_->scaleConfig.countryCode().toUpper();

        QString image_lang;
        if ( "ES.MX" == lang )
            image_lang = "_es-mx";
        else if ( "FR.CA" == lang )
            image_lang = "_fr-ca";
        else if ( "EN.US" == lang )
            image_lang = "_en-us";

        files.append( ( "attract1" + image_lang + "_G2.png" ) );
        files.append( ( "attract2" + image_lang + "_G2.png" ) );
        files.append( ( "attract3" + image_lang + "_G2.png" ) );

        files.append( "background_G2.png" );
    }
    else
    {
        files.append( "attract.png" );

        QString lang = scaleConfig_->scaleConfig.languageCode().toUpper() + "." + scaleConfig_->scaleConfig.countryCode().toUpper();

        QString image_lang;
        if ( "ES.MX" == lang )
            image_lang = "_es-mx";
        else if ( "FR.CA" == lang )
            image_lang = "_fr-ca";
        else if ( "EN.US" == lang )
            image_lang = "_en-us";

        files.append( ( "attract1" + image_lang + ".png" ) );
        files.append( ( "attract2" + image_lang + ".png" ) );
        files.append( ( "attract3" + image_lang + ".png" ) );

        files.append( "background.png" );
    }

    files.append( "itemLookup.png" );
    files.append( "nextArrow.png" );
    files.append( "prevArrow.png" );
    files.append( "storelogo.png" );
    files.append( "pulllabel.png" );


    foreach ( const QString& file, files )
    {
        const QString filename( filesystem::absoluteFilePath( file, folderType::SelfServeImages ) );

        if ( QFile::exists( filename ) )
            fileHashTable_[filename] = generateFileHash( filename );
    }

    // setup file monitor timer
    fileMonitorTimer_ = new QTimer( this );
    connect( fileMonitorTimer_, SIGNAL( timeout() ), SLOT( onFileMonitorTimerTimeout() ) );

    fileMonitorTimer_->start( 60000 );

    // setup idle timer
    idleTimer_ = new QTimer( this );
    connect( idleTimer_, SIGNAL( timeout() ), SLOT( onIdleTimerTimeout() ) );
    timeoutT_ = session()->scaleConfig->normalScaleOperation.selfServeTimeout();

    // if timeout value not found in config, set 30 second default timeout
    if(timeoutT_ == 0)
    {
        timeoutT_ = 30000;
    }

//#ifdef QT_DEBUG
//    timeoutT_ = 2000000000;
//#endif

    // these are the allowed state changes
    allowedStateChanges_.insert( attract, itemEntryKeyed );
    allowedStateChanges_.insert( attract, itemEntryImages );

    //allowedStateChanges_.insert( attract, itemEntryMainFlashkeysGen2 );


    allowedStateChanges_.insert( itemEntryKeyed, attract );
    allowedStateChanges_.insert( itemEntryKeyed, itemEntryImages );
    allowedStateChanges_.insert( itemEntryKeyed, processRandomWeightItem );
    allowedStateChanges_.insert( itemEntryKeyed, processByCountItem );

    allowedStateChanges_.insert( itemEntryImages, attract );
    allowedStateChanges_.insert( itemEntryImages, itemEntryKeyed );
    allowedStateChanges_.insert( itemEntryImages, processRandomWeightItem );
    allowedStateChanges_.insert( itemEntryImages, processByCountItem );

    allowedStateChanges_.insert( processRandomWeightItem, attract );
    allowedStateChanges_.insert( processRandomWeightItem, itemEntryKeyed );
    allowedStateChanges_.insert( processRandomWeightItem, itemEntryImages );
    allowedStateChanges_.insert( processRandomWeightItem, takeLabel );

    allowedStateChanges_.insert( processByCountItem, attract );
    allowedStateChanges_.insert( processByCountItem, itemEntryKeyed );
    allowedStateChanges_.insert( processByCountItem, itemEntryImages );
    allowedStateChanges_.insert( processByCountItem, takeLabel );

    allowedStateChanges_.insert( takeLabel, itemEntryKeyed );
    allowedStateChanges_.insert( takeLabel, itemEntryImages );

    // these are the dialog states
    dialogStates_.append( weightsMeasuresDialog );
    dialogStates_.append( auditLogDialog );

    // ================ GEN 2 ==============================
    // From the Attract Start Screen (the instruct_ screen) the only place to go is flashkeys
    allowedStateChanges_.insert( attractGen2, itemEntryKeyedGen2 );
    allowedStateChanges_.insert( attractGen2, itemEntryImagesGen2 );

    // From where we can go from the Flashkey Screen
    allowedStateChanges_.insert( itemEntryKeyedGen2, attractGen2 );                         // back to start
    allowedStateChanges_.insert( itemEntryKeyedGen2, itemEntryImagesGen2 );                 // we can search
    allowedStateChanges_.insert( itemEntryKeyedGen2, processRandomWeightItemGen2 );         // enter random weight PLU
    allowedStateChanges_.insert( itemEntryKeyedGen2, processByCountItemGen2 );              // enter bycount PLU

    allowedStateChanges_.insert( itemEntryImagesGen2, attractGen2 );
    allowedStateChanges_.insert( itemEntryImagesGen2, itemEntryKeyedGen2 );
    allowedStateChanges_.insert( itemEntryImagesGen2, processRandomWeightItemGen2 );
    allowedStateChanges_.insert( itemEntryImagesGen2, processByCountItemGen2 );
    allowedStateChanges_.insert( itemEntryImagesGen2, itemEntryImagesViewAllGen2 );

    allowedStateChanges_.insert( itemEntryImagesViewAllGen2, itemEntryKeyedGen2 );
    allowedStateChanges_.insert( itemEntryImagesViewAllGen2, itemEntryImagesGen2 );
    allowedStateChanges_.insert( itemEntryImagesViewAllGen2, attractGen2 );

    // where can you go from random weight Screen
    allowedStateChanges_.insert( processRandomWeightItemGen2, itemEntryKeyedGen2 );
    allowedStateChanges_.insert( processRandomWeightItemGen2, attractGen2 );
    allowedStateChanges_.insert( processRandomWeightItemGen2, itemEntryImagesGen2 );

    // where can you go from by count Screen
    allowedStateChanges_.insert( processByCountItemGen2, itemEntryKeyedGen2 );
    allowedStateChanges_.insert( processByCountItemGen2, itemEntryImagesGen2 );
    allowedStateChanges_.insert( processByCountItemGen2, attractGen2 );

    allowedStateChanges_.insert( takeLabel, itemEntryKeyedGen2 );

    // these are the dialog states
    dialogStates_.append( weightsMeasuresDialogGen2 );
    dialogStates_.append( auditLogDialogGen2 );

    // load store logo from filesystem
    loadStoreLogo();

    // load pull label image from filesystem
    loadPullLabel();

    // create user interface
    if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
    {

        d_G2_ = new selfServiceDialogG2( this );
        d_G2_->installEventFilter( this );

    }
    else
    {
        d_ = new selfServiceDialog( this );
        d_->installEventFilter( this );
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::changeLookOfCurrentState( state value, bool force )
{
    Q_UNUSED( force );

    // translate state
    value = mappedState( value );

    // process!
    Q_EMIT currentLookOfStateChanged();

    // reset the idle timer
    resetIdleTimer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::changeCurrentState( state value, bool force )
{
    // translate state
    value = mappedState( value );

    // nothing to do
    if ( !force )
    {
        if ( currentState_ == value )
            return;
    }

    // dont let previous state be a dialog
    if ( !isDialogState( currentState_ ) )
        prevState_ = currentState_;

    currentState_ = value;

    // process!
    Q_EMIT currentStateChanged();

    // reset the idle timer
    resetIdleTimer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::changePreviousState( state value )
{
    prevState_ = value;
}

bool selfServiceHook::isLoginScreen()
{
    const MadpScaleConfig *scaleConfig( &session()->scaleConfig->scaleConfig );
    int start_state = scaleConfig->selfServeStartScreenState();
    bool loggedIn  = session()->scaleConfig->getIsOperatorLoggedIn();

    if( SelfServicePrintOption::SelfServOption_StartOperLogin == start_state && !loggedIn ){
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceHook::ScaleZeroResult selfServiceHook::isScaleZero() const
{
    const double minWeightToPrint( fieldValue( sessionHooks::minWeightToPrint ).toDouble() );

    // check zero indicator
    bool scaleZero( "1" == fieldValue( sessionHooks::zeroIndicator ) );

    // when we have min weight to print, use that instead
    if ( 0.0 < minWeightToPrint )
    {
        // parse net weight
        const QString netWeightField( fieldValue( sessionHooks::netWeight ) );

        if ( netWeightField.contains( "----" ) )
            return SZ_Unknown;
        else if ( netWeightField.isEmpty() )
            return SZ_Unknown;
        else
        {
            double netWeight( 0.0 );

            if ( netWeightField.contains( ' ' ) )
                netWeight = netWeightField.left( netWeightField.indexOf( ' ' ) ).toDouble();
            else
                netWeight = netWeightField.toDouble();

            scaleZero = (netWeight < minWeightToPrint);
        }
    }

    return (scaleZero ? SZ_Yes : SZ_No);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray selfServiceHook::generateFileHash( const QString& filename )
{
    QFile f( filename );
    QByteArray hash;

    // open file and generate hash
    if ( f.open( QIODevice::ReadOnly ) )
    {
        // generate hash
        hash = QCryptographicHash::hash( f.readAll(), QCryptographicHash::Md5 );

        f.close();
    }

    return hash;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceHook::raiseTimeout()
{
    LOG_CLASS_INFO( "selfServiceHook", "raiseTimeout" );

    // dont do anything if we are not in self service mode
    if( activeOnStartup() )
    {
        LOG_INFO( "===== RAISE TIMER FIRED =====", NULL );
        if( scaleConfig_->scaleConfig.selfServeEnableFreshServeUI() )
        {
            if( d_G2_ )
            {
                d_G2_->show();
                d_G2_->activateWindow();
                d_G2_->raise();
            }
        }
        else
        {
            if( d_ )
            {
                d_->show();
                d_->activateWindow();
                d_->raise();
            }
        }
    }
}
