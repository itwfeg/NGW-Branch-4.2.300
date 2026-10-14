/**
 * @file modewidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "modewidget.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../screens/screenwidget.h"

#include <margins.h>
#include <openauth.h>
#include <palette.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QTimer>
#include <QVBoxLayout>

ModeWidget::sessionModeMap ModeWidget::modeMap_;

///////////////////////////////////////////////////////////////////////////////////////////////////
ModeWidget::ModeWidget( ScreenWidget *screen, Mode scaleMode, SessionMainWindow *parent ) :
    _Mybase( parent ),
    mainWindow_( parent ),
    screen_( NULL ),
    scaleMode_( scaleMode ),
    currentWeighMode_( 0 ),
    loggedIn_( false ),
    loggingOutOfMode_( false ),
    usbKeyPresent_( false ),
    supervisorTokenPresent_( false ),
    serviceTokenPresent_( false )
{
    // retrieve session
    session_ = mainWindow_->session();

    initialize( screen );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ModeWidget::ModeWidget( Mode scaleMode, SessionMainWindow *parent ) :
    _Mybase( parent ),
    mainWindow_( parent ),
    screen_( NULL ),
    scaleMode_( scaleMode ),
    currentWeighMode_( 0 ),
    loggedIn_( false ),
    loggingOutOfMode_( false ),
    usbKeyPresent_( false ),
    supervisorTokenPresent_( false ),
    serviceTokenPresent_( false )
{
    // retrieve session
    session_ = mainWindow_->session();

    initialize( NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ModeWidget::~ModeWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *ModeWidget::isCached( ScreenWidget *screen ) const
{
    // look for screen already in cache
    foreach ( const ScreenWidget *cached, cache_ )
        if ( screen == cached )
            return screen;

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ModeWidget::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    if (( screen() ) && ( screen()->isErrorMessageHandler( errorMsg ) ))
        return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ModeWidget::loggedIn() const
{
    return (( loggedIn_ ) || ( isKeyPresent() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int ModeWidget::operatorAccessLevel() const
{
    if ( isKeyPresent() )
        return 9;
    else if ( loggedIn_ )
    {
        DomOperatorLoginData *data = const_cast<DomOperatorLoginData*>( &operatorLoginData_ );
        return data->operatorAccessLevel();
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString ModeWidget::operatorID() const
{
    DomOperatorLoginData *data = const_cast<DomOperatorLoginData*>( &operatorLoginData_ );
    return data->operatorID();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString ModeWidget::operatorName() const
{
    DomOperatorLoginData *data = const_cast<DomOperatorLoginData*>( &operatorLoginData_ );
    return data->operatorName();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::setFontFamily( const QString& value )
{
    QList<QWidget*> children( findChildren<QWidget*>() );
    children.append( this );

    foreach ( QWidget *w, children )
    {
        QFont f( w->font() );
        f.setFamily( value );

        w->setFont( f );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::setScreen( ScreenWidget *screen )
{
    LOG_CLASS_INFO( "ModeWidget", "setScreen" );

    LOG_DEBUG( "setting screen '%s'", screen ? screen->objectName().toLatin1().constData() : "(null)" );

    if ( screen_ == screen )
        return;

    // delete the old screen
    if ( screen_ )
    {
        LOG_DEBUG("Deleting old screen", NULL);
        screen_->hide();

        if ( !isCached( screen_ ) ){
            LOG_DEBUG("Deleting LATER", NULL);
            screen_->deleteLater();
        }
    }

    // delete old layout
    if ( layout() )
        delete layout();

    // hide input widgets
    LOG_DEBUG("hide input midgets", NULL);
    mainWindow_->hideInputWidgets();
    LOG_DEBUG("hiding input midgets successful", NULL);

    // set the screen and show it
    if ( (screen_ = screen) )
        screen_->show();

    // create layout for widgets
    LOG_DEBUG("Creating box layout for midgets", NULL);
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setContentsMargins( overlayMargins() );
    LOG_DEBUG("Box layout set", NULL);

    // add screen to layout
    if ( screen_ )
    {
        LOG_DEBUG("Adding screen to layout", NULL);
        screen_->setParent( this );
        screen_->lower();

        layout->addWidget( screen_ );
        LOG_DEBUG("Finished adding screen", NULL);
    }

    // emit changed signal
    Q_EMIT screenChanged();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
OpModeEvent ModeWidget::operationMode()
{
    return modeMap_[ session_ ];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::setOperationMode( OpModeEvent value )
{
    // we track these operation modes
    if (( OpModeOperate == value ) ||
        ( OpModeSupervisor == value ) ||
        ( OpModeServiceLogin == value ) ||
        ( OpModeService == value ))
    {
        OpModeEvent oldValue = operationMode();

        // nothing to do
        if ( oldValue == value )
            return;

        // when leaving supervisor mode we need to log out of it first
        if ( OpModeSupervisor == oldValue )
            mainWindow_->retrieveMode<SupervisorMode>()->logout();

        // when leaving service mode we need to log out of it first
        // when leaving service login mode into a mode other than supervisor we need to logout
        else if (( OpModeService == oldValue ) ||
            (( OpModeServiceLogin == oldValue ) && ( OpModeService != value )))
            mainWindow_->retrieveMode<ServiceMode>()->logout();

        if (( OpModeOperate == value ) ||
            ( OpModeSupervisor == value ))
        {
            // Set the weigher switch to false because we left service mode
            DomWeigherSwitch *mySwitch = &session_->scaleConfig->weigherSwitch;
            mySwitch->switchPressed() = "0";
        }

        // if exiting service mode, and key is present on wrapper, remind user to remove it
        // SOF-3688
        if ((OpModeService == oldValue ) && session_->isPrepack() && usbKeyPresent_)
        {
            session_->send(Error, ErrSessionDevice, "100061,5");
        }

        modeMap_[ session_ ] = value;
    }

    // set scale mode
    session_->send( OperationMode, value );

    // request scale mode
    session_->send( SessionDev, SnReadWeighModeTransactionType );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ModeWidget::operatorsExist() const
{
    if ( session_ )
    {
        const MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

        return scaleConfig->operatorsExist();
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QMargins ModeWidget::margins() const
{
    QMargins result( overlayMargins() );

    if ( screen_ )
        result += screen_->margins();

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::activate()
{
    LOG_CLASS_INFO("ModeWidget", "activate");

    LOG_DEBUG("inside activate function", NULL);
    // switch to proper operation mode
    if ( ModeWidget::Operate == scaleMode_ )
    {
        LOG_DEBUG("set to operator mode", NULL);
        setOperationMode( OpModeOperate );
        LOG_DEBUG("set to operator mode success", NULL);

        // going to operate mode, unchache service key
        // SOF-3574 HTm: Service mode can be entered with a key
        cachedUsbKeyActivation_ = session_->scaleConfig->keyActivation;
        cachedUsbKeyActivation_.setSupervisorKey(false);
        cachedUsbKeyActivation_.setSupervisorF10(false);
        cachedUsbKeyActivation_.setServiceKey(false);
        cachedUsbKeyActivation_.setSelfServeKey(false);
        cachedUsbKeyActivation_.setQATestKey(false);
        cachedUsbKeyActivation_.setManfTestKey(false);

        LOG_DEBUG( "activate updated service key: %d %d %d %d %d %d",
            cachedUsbKeyActivation_.isSupervisorKey(),
            cachedUsbKeyActivation_.isSupervisorF10(),
            cachedUsbKeyActivation_.isServiceKey(),
            cachedUsbKeyActivation_.isSelfServeKey(),
            cachedUsbKeyActivation_.isQATestKey(),
            cachedUsbKeyActivation_.isManfTestKey() );
    }
    else if ( ModeWidget::Supervisor == scaleMode_ )
    {
        LOG_DEBUG("set to supervisor mode", NULL);
        session_->scaleConfig->internalConfig.isOkToLookUpPlu = true;
        setOperationMode( OpModeSupervisor );
        LOG_DEBUG("set to supervisor mode success", NULL);
    }
    else if ( ModeWidget::Service == scaleMode_ )
    {
        LOG_DEBUG("set to service mode", NULL);
        session_->scaleConfig->internalConfig.isOkToLookUpPlu = true;
        setOperationMode( OpModeService );
        LOG_DEBUG("set to service mode success", NULL);
    }

    // set browser menu mode
    if ( session_->scaleConfig->internalConfig.scaleMode != scaleMode_ )
    {
        LOG_DEBUG("set browser menu mode ", NULL);
        session_->scaleConfig->internalConfig.scaleMode = scaleMode_;
        LOG_DEBUG("scaleMode_ set to session_->scaleConfig->internalConfig.scaleMode", NULL);
        session_->send( SessionDev, SnBrowserMenuChanging, QString::number( scaleMode_ ) );
        LOG_DEBUG("set browser menu mode success", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::createCachedScreens()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PanelWidget *ModeWidget::createMenuPanel( int panelEx, SessionDialog *parent )
{
    Q_UNUSED( panelEx );
    Q_UNUSED( parent );

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::login( const QString& id )
{
    LOG_CLASS_INFO( "ModeWidget", "login" );

    // already logged into mode
    if ( loggedIn_ )
    {
        LOG_WARN( "already logged into mode", NULL );
        return;
    }

    // send login id
    if ( id.length() )
    {
        LOG_INFO( "logging into mode %d with id '%s'...", scaleMode_, id.toLatin1().constData() );

        // switch to proper operation mode
        if ( ModeWidget::Operate == scaleMode_ )
            setOperationMode( OpModeOperate );
        else if ( ModeWidget::Supervisor == scaleMode_ )
            setOperationMode( OpModeSupervisor );
        else if ( ModeWidget::Service == scaleMode_ )
        {
            QString validKey = getPassKey();

            // allow open auth key for service mode
            OpenAuth gen( validKey, 14400 );

            if ( gen.validateCode( id ) )
            {
                serviceTokenPresent_ = true;

                // setup timer to expire token
                expireTokenTimer_->start( 5 * 60 * 1000 ); // 5m

                // show unlocked mode panel
                Q_EMIT usbKeyInserted();
                return;
            }

            // only allow login when secondary session
            if ( mainWindow_->isPrimarySessionType() )
                return;

            setOperationMode( OpModeServiceLogin );
        }

        session_->send( OperatorDev, OpLogin, id + ",,,,,," + QString::number( scaleMode_ ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString ModeWidget::getPassKey()
{
    QString rvalQString = "";

    //--------------------------------------------------------
    // Array of passwords for 8 years, beginning January 2017.
    // Each password is for one quarter of the year
    //  i.e. Jan/Feb/Mar = quarter 1; Apr/May/Jun = quarter 2
    //--------------------------------------------------------
    QList <QString> passCodes;

    passCodes << "AJXV7S74URM5QMA7" << "WLPEV4W53PVJQ3XQ" << "DVJZAKD64EG6ZYXA" << "PH62F5353VVM7DUN"
              << "ENT27MBWHMHRY6WF" << "Y66GQUURU45AP3MF" << "665HVGMA55YZTPY6" << "EB56V25UTW65VP4E"
              << "MTHV622Y2A3V42SS" << "ZU23VN5C56WMQNUR" << "7XQAXFX465W7E5TR" << "2QSLUMEEN7YPCR6M"
              << "DXMU6L6SCN476GAK" << "64H7HRWV5S5F72EV" << "R6FRLSZECW6ELBV5" << "M2GWXM75BKCKQU66"
              << "3KPG54PPMAG6VNJB" << "7E4TZSA6HVDND5K2" << "L23AEKMEJZXGDWBU" << "GHG5BC3ZCBL3D5ST"
              << "TJYHFZHZ2FBA3KLV" << "MSR5NK6KBXHN4STR" << "F6FZXBD5GLWU47ZN" << "T6BQLLMPMVU6NY6P"
              << "VNBHE62V7DZQ53X2" << "GFPM5A643HLCUVPC" << "MA65XUEUQMKVAJGX" << "RR6E6556YFCYLFFH"
              << "J5NMSZ2DAXNCMN65" << "WGZ7JZT54MPWKUBH" << "DZ2THM7QKN67ZR3R" << "JW7QU35E5FKGQFC5";

    int currMonth = QDate::currentDate().month();
    int currYear = QDate::currentDate().year();

    int yearCalc = ( currYear % 8 ) * 4 ;

    int quarterCalc = 0;
    if( currMonth >= 1 && currMonth <= 3 )
        quarterCalc = 0;
    else if( currMonth >= 4 && currMonth <= 6 )
        quarterCalc = 1;
    else if( currMonth >= 7 && currMonth <= 9 )
        quarterCalc = 2;
    else if( currMonth >= 10 && currMonth <= 12 )
        quarterCalc = 3;

    int passOffset = yearCalc + quarterCalc;

    rvalQString = passCodes.at( passOffset );

    return rvalQString;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::logout()
{
    LOG_CLASS_INFO( "ModeWidget", "logout" );

    // send logout message
    if ( loggedIn_ )
    {
        LOG_INFO( "logging out of mode %d...", scaleMode_ );

        // no longer logged in
        loggingOutOfMode_ = true;
        loggedIn_ = false;

        // send!!
        session_->send( OperatorDev, OpLogout );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::translate()
{
    // translate current screen
    if ( screen_ )
        screen_->translate();

    // translate cached screens
    foreach ( ScreenWidget *s, cache_ )
        s->translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QMargins ModeWidget::overlayMargins() const
{
    return QMargins();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::cache( ScreenWidget *screen )
{
    // add to cache
    if ( !isCached( screen ) )
    {
        cache_.push_back( screen );

        // set screen visibility
        screen->setVisible( false );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ModeWidget::onScreenChanging()
{
    LOG_CLASS_INFO( "ModeWidget", "onScreenChanging" );

    ScreenWidget *currentScreen = screen();

    // unload current screen
    if ( currentScreen )
    {
        LOG_DEBUG( "unloading screen...", NULL );

        if ( !currentScreen->onScreenChanging() )
        {
            LOG_WARN( "cannot unload current screen '%s'!", currentScreen->objectName().toLatin1().constData() );
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "ModeWidget", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // login message
    if (( OperatorDev == device ) && ( OpLogin == event ))
    {
        DomOperatorLoginData m( msg );

        int loginMode = m.operatorMode();

        // check that this is a login to our mode
        if ( scaleMode_ == loginMode )
        {
            operatorLoginData_ = msg;

            // when hands free enabled switch to preferred language
            if ( session_->uiUtilities->isFeatureLicensed( SCALE_HANDSFREEDEVICE ) )
            {
                ScaleConfiguration *scaleConfig( session_->scaleConfig );
                scaleConfig->currentLanguage = ("2" == m.preferredLanguage() ? 2 : 1);

                // set language
                LOG_TRACE( "set language %s...", qPrintable( scaleConfig->getFPPLanguage() ) );
                mainWindow_->setLanguage( scaleConfig->getFPPLanguage() );
            }

            // sanity check... make sure operator id is valid...
            if ( (loggedIn_ = (0 < operatorID().length())) )
            {
                LOG_INFO( "operator logged in", NULL );
                Q_EMIT operatorLoggedIn();
            }

            // CUT TEST JHC: After logging in, we will get a cut test write
            //               which will then pass the cut test number to the init info
            //               so let that Cut Test Write handle launching CutTestInitialInformation 

            if( session_->scaleConfig->opLoggedInForCutTestHistory )
            {
                // Operator just logged in to start seeing HISTORY.
                // Need access rights to view, suspend, start, delete, restart, etc
                // Moved to sessionmainwindow. Now we just do a read here
                // mainWindow_->launchCuttingTestHistory();
                session_->send( CutTestDev, CuttingTestHistoryRead );
            }
        }

        // check that this is a logout
        else if (( loggedIn_ ) && ( 0 == loginMode ))
        {
            operatorLoginData_ = msg;
            loggedIn_ = false;

            // when hands free enabled revert to primary language
            if ( session_->uiUtilities->isFeatureLicensed( SCALE_HANDSFREEDEVICE ) )
            {
                ScaleConfiguration *scaleConfig( session_->scaleConfig );
                scaleConfig->currentLanguage = 1;

                // set language
                LOG_TRACE( "set language %s...", qPrintable( scaleConfig->getFPPLanguage() ) );
                mainWindow_->setLanguage( scaleConfig->getFPPLanguage() );
            }

            LOG_INFO( "operator logged out", NULL );
            Q_EMIT operatorLoggedOut();
        }
    }

    // service key inserted
    else if (( SessionDev == device ) && ( SnKeyActivationStates == event ))
    {
        // update cached key credentials
        if ( modeUsesCachedKeyActivation( currentWeighMode_ ) )
            cachedUsbKeyActivation_ |= session_->scaleConfig->keyActivation;
        else
            cachedUsbKeyActivation_ = session_->scaleConfig->keyActivation;

        LOG_DEBUG( "new key states: %d %d %d %d %d %d",
            cachedUsbKeyActivation_.isSupervisorKey(),
            cachedUsbKeyActivation_.isSupervisorF10(),
            cachedUsbKeyActivation_.isServiceKey(),
            cachedUsbKeyActivation_.isSelfServeKey(),
            cachedUsbKeyActivation_.isQATestKey(),
            cachedUsbKeyActivation_.isManfTestKey() );

        // key inserted
        if (( !usbKeyPresent_ ) && ( isKeyPresent() ))
        {
            usbKeyPresent_ = true;

            LOG_INFO( "usb key inserted", NULL );
            Q_EMIT usbKeyInserted();
        }

        // key removed
        else if (( usbKeyPresent_ ) && ( !isKeyPresent() ))
        {
            usbKeyPresent_ = false;

            LOG_INFO( "usb key removed", NULL );
            Q_EMIT usbKeyRemoved();
        }
    }

    // weigh mode
    else if (( SessionDev == device ) && ( SnWriteWeighModeTransactionType == event ))
    {
        DomWeighModeTotalType m( msg );

        int newWeighMode = m.weighMode();

        // when switching out of supervisor or service modes we need to reset cached key activation
        if (( currentWeighMode_ != newWeighMode ) && ( !modeUsesCachedKeyActivation( newWeighMode ) ))
        {
            cachedUsbKeyActivation_ = session_->scaleConfig->keyActivation;

            LOG_DEBUG( "key state reset: %d %d %d %d %d %d",
                cachedUsbKeyActivation_.isSupervisorKey(),
                cachedUsbKeyActivation_.isSupervisorF10(),
                cachedUsbKeyActivation_.isServiceKey(),
                cachedUsbKeyActivation_.isSelfServeKey(),
                cachedUsbKeyActivation_.isQATestKey(),
                cachedUsbKeyActivation_.isManfTestKey() );
        }

        // new mode is now current
        currentWeighMode_ = newWeighMode;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::onScreenDestroyed( QObject *obj )
{
    LOG_CLASS_INFO( "ModeWidget", "onScreenDestroyed" );

    if ( obj )
    {
        LOG_WARN( "destroy screen '%s'", qPrintable( obj->objectName() ) );

        ScreenWidget *w( reinterpret_cast<ScreenWidget*>( obj ) );

        if (( w ) && ( cache_.contains( w ) ))
        {
            LOG_WARN( "screen was cached", NULL );
            cache_.removeAll( w );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::onSessionMessageReceive( const SessionMessage& msg )
{
    // call derived class method
    onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::onTokenExpired()
{
    supervisorTokenPresent_ = false;
    serviceTokenPresent_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ModeWidget::initialize( ScreenWidget *screen )
{
    // set palette
    modifyPaletteBackground setBackground( Qt::transparent );
    setBackground( this );

    setAutoFillBackground( true );

    // set the screen
    setScreen( screen );

    // connect into session
    connect( session_, SIGNAL( receive(SessionMessage) ), this, SLOT( onSessionMessageReceive(SessionMessage) ) );

    // setup token epiration timer
    expireTokenTimer_ = new QTimer( this );
    expireTokenTimer_->setSingleShot( true );

    connect( expireTokenTimer_, SIGNAL(timeout()), SLOT(onTokenExpired()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ModeWidget::modeUsesCachedKeyActivation( int mode ) const
{
    return (( OpModeSupervisor == mode ) ||
        ( OpModeService == mode ) ||
        ( OpModeServiceLogin == mode ));
}
