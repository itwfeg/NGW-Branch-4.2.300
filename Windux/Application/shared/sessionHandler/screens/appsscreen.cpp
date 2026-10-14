/**
 * @file apps.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "appsscreen.h"
#include "plumainscreen.h"
#include "meatspecsheetscreen.h"

#include <sessionmainwindow.h>
#include <sessionmainwindowimpl.h>

#include <modes/modewidget.h>
#include <modes/operatemode.h>
#include <modes/supervisormode.h>

#include <Session.h>
#include <Messages.h>
#include <UiUtilities.h>
#include <ScaleConfiguration.h>

#include <logmanager.h>
#include <filesystem.h>
#include <toolbutton.h>

#include <QDir>
#include <QDirIterator>
#include <QProcess>
#include <QSignalMapper>
#include <QStringList>
#include <QToolButton>

static const int BUILD_A_MEAL_SCREEN = 1;
static const int CUT_TEST_ONE_SCREEN = 2;

#ifdef Q_OS_WIN
#else
#include <X11/Xlib.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
AppsScreen::AppsScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    promptLoginActive_( false )
{
    setObjectName( "AppsScreen" );

    // init
    initialize();
    createLayout();
    translate();

    // load icons
    loadAppIcons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
AppsScreen::~AppsScreen()
{
    LOG_CLASS_INFO( "AppsScreen", "loadAppIcons" );
    LOG_DEBUG( "DTOR", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList AppsScreen::apps() const
{
    QStringList a;

    // update apps
    const_cast<_Myt*>( this )->loadAppIcons();

    // populate list with app names
    applicationMap::const_iterator app( apps_.begin() );

    while ( app != apps_.end() )
    {
        settings s( appSettings_[app.value()] );

        // validate
        if ( isSettingsValid( s ) )
            a.append( s["NAME"] );

        ++app;
    }

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool AppsScreen::launchApp( const QString &name )
{
    // find app name
    for ( applicationSettingsMap::const_iterator as( appSettings_.begin() ); as != appSettings_.end(); ++as )
    {
        const settings s( as.value() );

        // validate
        if (( isSettingsValid( s ) ) && ( name == s["NAME"] ))
            return launchAppFromSettings( s );
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AppsScreen::translate()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool AppsScreen::isExternalAppInstalled()
{
    static bool foundApps( false );

    // if we have already found conf files then just return true
    if ( foundApps )
        return true;

    // list of extensions we are interested in
    QStringList exts;
    exts.append( "*.conf" );

    // iterate conf files
    QDirIterator files( filesystem::folder( folderType::Apps ), exts, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks );

    while ( files.hasNext() )
    {
        foundApps = true;
        break;
    }

    return foundApps;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AppsScreen::resizeEvent( QResizeEvent *e )
{
    refreshAppIcons();

    promptLoginActive_ = false;

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *AppsScreen::swipeScreenUp() const
{
    const int bottomScreen( mainWindow_->session()->scaleConfig->scaleConfig.bottomScreenType() );

    if ( BUILD_A_MEAL_SCREEN == bottomScreen )
        return mode_->createScreen<PluMainScreen>();
    else if ( CUT_TEST_ONE_SCREEN == bottomScreen )
        return mode_->createScreen<MeatSheetScreen>();

    return mode_->createScreen<PluMainScreen>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AppsScreen::loadAppIcons()
{
    LOG_CLASS_INFO( "AppsScreen", "loadAppIcons" );

    QList<QAbstractButton*> valid;
    QList<QAbstractButton*> invalid;

    LOG_TRACE( "loading app icons...", NULL );

    // list of extensions we are interested in
    QStringList exts;
    exts.append( "*.conf" );

    // iterate conf files
    QDirIterator files( filesystem::folder( folderType::Apps ), exts, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks );

    while ( files.hasNext() )
    {
        QString file( files.next() );
        bool found( false );

        LOG_DEBUG( "processing file '%s'...", qPrintable( file ) );

        // check current apps
        for ( applicationMap::const_iterator a( apps_.begin() ); a != apps_.end(); ++a )
            if ( (found = (a.value() == file)) )
            {
                LOG_DEBUG( "found existing app!", NULL );

                valid.push_back( a.key() );
                break;
            }

        // new app, add to list
        if ( !found )
        {
            QAbstractButton *app;
            settings s;

            LOG_DEBUG( "detected new app!", NULL );

            // generate icon
            if ( !(app = createAppIcon( file, s )) )
                LOG_WARN( "failed to create new app '%s'", qPrintable( file ) );
            else
            {
                apps_[app] = file;
                appSettings_[file] = s;

                valid.push_back( app );
            }
        }
    }

    // remove invalid apps
    for ( applicationMap::const_iterator a( apps_.begin() ); a != apps_.end(); ++a )
        if ( !valid.contains( a.key() ) )
            invalid.push_back( a.key() );

    foreach ( QAbstractButton *doomed, invalid )
    {
        apps_.remove( doomed );
        delete doomed;
    }

    // refresh icons
    refreshAppIcons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AppsScreen::refreshAppIcons()
{
    LOG_CLASS_INFO( "AppsScreen", "refreshAppIcons" );

    static const int numButtonsWidth( 5 );
    static const int numButtonsHeight( 4 );

    LOG_TRACE( "refreshing app icons...", NULL );

    // sanity check
    if (( width() <= 128 ) || ( height() <= 128 ))
        return;

    const QRect m( 64, 8, width() - 128, height() - 16 );

    // calc button size
    const int buttonWidth( (m.right() - m.left()) / numButtonsWidth );
    const int buttonHeight( (m.bottom() - m.top()) / numButtonsHeight );

    const QSize buttonSize( buttonWidth, buttonHeight );

    // calc icon size
    const int iconWidth( (buttonWidth * 90) / 100 );
    const int iconHeight( (buttonHeight * 75) / 100 );

    const QSize iconSize( qMin( iconWidth, iconHeight ), qMin( iconWidth, iconHeight ) );

    // re-order buttons
    applicationMap::iterator button( apps_.begin() );

    for ( int x( 0 ); x < numButtonsWidth; ++x )
        for ( int y( 0 ); y < numButtonsHeight; ++y )
        {
            if ( button == apps_.end() )
                return;

            // modify button properties
            hobart::toolButton *b( dynamic_cast<hobart::toolButton*>( button.key() ) );
            b->setFixedSize( buttonSize );
            b->setIconSize( iconSize );
            b->setVisible( true );

            b->move( m.left() + x*buttonWidth, m.top() + y*buttonHeight );

            ++button;
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool AppsScreen::maximizeWindow( const QString &title )
{
    LOG_CLASS_INFO( "AppsScreen", "maximizeWindow" );
    Q_UNUSED( title );

    Q_UNUSED( title );

    bool result( false );

#ifdef Q_OS_WIN

    // TODO

#else
    Display *disp( XOpenDisplay( NULL ) );

    if ( disp )
    {
        Window rootWindow( XDefaultRootWindow( disp ) );

        if ( rootWindow )
        {
            Status status;

            Atom a( XInternAtom( disp, "_NET_CLIENT_LIST", True ) );
            Atom actualType;
            int actualFormat;

            unsigned long num;
            unsigned long bytes;
            unsigned char *data( NULL );

            status = XGetWindowProperty(
                disp,
                rootWindow,
                a,
                0L,
                (~0L),
                false,
                AnyPropertyType,
                &actualType,
                &actualFormat,
                &num,
                &bytes,
                &data );

            if ( Success != status )
                LOG_WARN( "failed to get window propery 0x%08x %d", rootWindow, status );
            else if (( data ) && ( 32 == actualFormat ))
            {
                // cast to proper format, and iterate through values
                unsigned long *array( (unsigned long*) data );

                while (( num-- ) && ( !result ))
                {
                    char *name( NULL );

                    // get window id
                    Window w( array[num] );

                    if (( 0 != XFetchName( disp, w, &name ) ) && ( name ))
                    {
                        LOG_INFO( "Found: 0x%08x '%s'", w, name );

                        if ( title == name )
                        {
                            const int screen( DefaultScreen( disp ) );
                            const int sw( DisplayWidth( disp, screen ) );
                            const int sh( DisplayHeight( disp, screen ) );

                            XMoveResizeWindow( disp, w, 0, 0, sw, sh );
                            XMapWindow( disp, w );
                            XSetInputFocus( disp, w, RevertToPointerRoot, CurrentTime );
                            XFlush( disp );

                            result = true;
                        }
                    }

                    if ( name )
                        XFree( name );
                }
            }

            if ( data )
                XFree( data );
        }

        XCloseDisplay( disp );
    }

#endif

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool AppsScreen::validateAccess( const settings& s, int requiredAccessLevel )
{
    LOG_CLASS_INFO( "AppsScreen", "validateAccess" );

    LOG_INFO( "required access level is %d", requiredAccessLevel );

    // no access required
    if ( requiredAccessLevel <= 0 )
    {
        LOG_DEBUG( "no restrictions", NULL );
        return true;
    }

    // ===========================
    // Check current access levels
    // ===========================

    // retrieve access level
    int accessLevel( 0 );

    if ( !session_->scaleConfig->operatorLoggedIn )
        LOG_DEBUG( "no operator logged in", NULL );
    else
    {
        accessLevel = session_->scaleConfig->loggedOperatorAccess;

        LOG_DEBUG( "operator logged in level %d '%s' '%s'",
            accessLevel,
            qPrintable( session_->scaleConfig->loggedOperatorId ),
            qPrintable( session_->scaleConfig->loggedOperatorName ) );
    }

    LOG_INFO( "current access level is %d", accessLevel );

    if ( requiredAccessLevel <= accessLevel )
    {
        LOG_DEBUG( "access granted, operator already logged in", NULL );
        return true;
    }

    // check for supervisor or service key
    const MadpKeyActivation keys( mainWindow_->mode()->usbKeyActivation() );

    if ( keys.isServiceKey() )
    {
        LOG_DEBUG( "access granted, service key", NULL );
        return true;
    }
    else if (( keys.isSupervisorKey() ) || ( keys.isSupervisorF10() ))
    {
        if ( 9 < requiredAccessLevel )
            LOG_DEBUG( "found supervisor key but service key required", NULL );
        else
        {
            LOG_DEBUG( "access granted, supervisor key", NULL );
            return true;
        }
    }
    else
    {
        LOG_DEBUG( "no key detected", NULL );
    }

    // =========================
    // Prompt for operator login
    // =========================

    // show access error
    if ( session_->scaleConfig->operatorLoggedIn )
    {
        LOG_DEBUG( "operator logged in, skipping prompt", NULL );
        session_->send( Error, ErrOperatorDevice, "102006,1" );
    }

    // when nobody logged in prompt to login
    else
    {
        promptLoginDateTime_ = QDateTime::currentDateTime();
        promptLoginSettings_ = s;

        LOG_DEBUG( "prompting for operator id...", NULL );
        session_->send( OperatorDev, OpPromptForOperatorIDOrRFID );
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool AppsScreen::onScreenChanging()
{
    LOG_CLASS_INFO( "AppsScreen", "onScreenChanging" );

    // when logging out of scale we will return to home screen... this may cause a numberpad or
    // keyboard to popup over the app we just launched. when logging out an operator lets force
    // this page to remain on screen.
    if (( operLogoutDateTime_.isValid() ) && ( isVisible() ))
    {
        const qint64 delta( operLogoutDateTime_.msecsTo( QDateTime::currentDateTime() ) );

        LOG_DEBUG( "operator logout took %d ms", delta );

        if ( delta < LOGOUT_DELAY )
            return false;
    }

    return _Mybase::onScreenChanging();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AppsScreen::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "AppsScreen", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // operator id required
    if (( OperatorDev == device ) && ( OpPromptForOperatorIDOrRFID == event ) && ( isVisible() ))
    {
        if ( promptLoginDateTime_.isValid() )
        {
            const qint64 delta( promptLoginDateTime_.msecsTo( QDateTime::currentDateTime() ) );

            LOG_DEBUG( "prompt for operator id took %d ms", delta );

            if ( delta < LOGIN_PROMPT_DELAY )
                promptLoginActive_ = true;
        }
    }

    // operator login
    else if (( OperatorDev == device ) && ( OpLogin == event ) && ( isVisible() ))
    {
        if (( promptLoginDateTime_.isValid() ) && ( promptLoginActive_ ))
        {
            const qint64 delta( promptLoginDateTime_.msecsTo( QDateTime::currentDateTime() ) );

            LOG_DEBUG( "operator id login took %d ms", delta );

            if (( delta < LOGIN_PROMPT_TIME ) && ( session_->scaleConfig->operatorLoggedIn ))
            {
                // start app they were trying to launch earlier
                launchAppFromSettings( promptLoginSettings_ );

                // log them out
                session_->send( OperatorDev, OpLogout );
                promptLoginActive_ = false;

                operLogoutDateTime_ = QDateTime::currentDateTime();
            }
        }
    }

    return _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AppsScreen::onButtonClicked( QWidget *button )
{
    // get app
    applicationMap::iterator a( apps_.find( (QAbstractButton*) button ) );

    if ( apps_.end() == a )
        return;

    // get settings
    applicationSettingsMap::iterator as( appSettings_.find( a.value() ) );

    if ( appSettings_.end() == as )
        return;

    launchAppFromSettings( as.value() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool AppsScreen::launchAppFromSettings( const settings &s )
{
    LOG_CLASS_INFO( "AppsScreen", "launchAppFromSettings" );

    // get process
    QString cmd( s["EXECUTABLE"] );
    QString title;

    if ( cmd.isEmpty() )
        return false;

    if ( s.contains( "TITLE" ) )
        title = s["TITLE"];

    bool isAbsolutePath( false );

#ifdef Q_OS_WIN
    if ( 2 < cmd.length() )
        if ( ':' == cmd[1] )
            isAbsolutePath = true;
#else
    if ( 1 < cmd.length() )
        if ( '/' == cmd[0] )
            isAbsolutePath = true;
#endif

    if ( !isAbsolutePath )
        cmd = filesystem::folder( folderType::Apps ) + QDir::separator() + cmd;

    // SOF-3876
    // read in access level before executing.
    QString requiredAccessLevel( "0" );

    if ( s.contains( "ACCESS" ) )
        requiredAccessLevel = s["ACCESS"];

    if ( !validateAccess( s, requiredAccessLevel.toInt() ) )
        return false;

    // hide input widgets
    mainWindow_->hideInputWidgets();

    // when app running already simply maximize it
    if (( title.length() ) && ( maximizeWindow( title ) ))
    {
        LOG_INFO( "maximized process '%s'", qPrintable( cmd ) );
        return true;
    }

    // launch!!
    LOG_INFO( "launching process '%s'", qPrintable( cmd ) );

    QProcess *p( new QProcess( this ) );

    connect( p, SIGNAL(finished(int)), SLOT(onProcFinished(int)) );

    p->start( cmd );

    if ( !p->waitForStarted( 5000 ) )
        LOG_WARN( "process did not start in a timely manner!", NULL );
    else
    {
        LOG_INFO( "started external process %d!", p );
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AppsScreen::onProcFinished( int exitCode )
{
    LOG_CLASS_INFO( "AppsScreen", "onProcFinished" );

    LOG_INFO( "process %d completed code %d", sender(), exitCode );

    sender()->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AppsScreen::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
    {
        promptLoginActive_ = false;
        return;
    }

    OperateMode *operateMode( dynamic_cast<OperateMode*>( mode_ ) );

    // hide weight bar
    if ( operateMode )
        operateMode->hideWeightBar();

    // load icons
    loadAppIcons();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AppsScreen::initialize()
{
    clickedSignalMapper_ = new QSignalMapper( this );

    connect( clickedSignalMapper_, SIGNAL(mapped(QWidget*)), SLOT(onButtonClicked(QWidget*)) );

    // connect slots
    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AppsScreen::createLayout()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
QAbstractButton *AppsScreen::createAppIcon( const QString &conf, settings &s )
{
    LOG_CLASS_INFO( "AppsScreen", "onButtonClicked" );

    // load settings
    s = parseConf( conf );

    LOG_DEBUG( "conf '%s' contains %d keys", qPrintable( conf ), s.size() );

    // validate
    if ( !isSettingsValid( s ) )
    {
        LOG_WARN( "conf '%s' missing required keys!", qPrintable( conf ) );
        return NULL;
    }

    QDir path( filesystem::folder( folderType::Apps ) );

    QPixmap background( 512, 512 );
    background.fill( Qt::transparent );

    // generate icon
    QToolButton *icon( new hobart::toolButton( this, background, background ) );
    icon->setIcon( QPixmap( path.absoluteFilePath( s["ICON"] ) ) );
    icon->setText( s["NAME"] );
    icon->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

    // connect icon signals
    clickedSignalMapper_->setMapping( icon, icon );
    connect( icon, SIGNAL(clicked()), clickedSignalMapper_, SLOT(map()) );

    return icon;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
AppsScreen::settings AppsScreen::parseConf( const QString &conf )
{
    LOG_CLASS_INFO( "AppsScreen", "parseConf" );

    QMap<QString, QString> s;
    QFile f( conf );

    // load settings
    if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
        LOG_WARN( "error loading config '%s'", qPrintable( conf ) );
    else
    {
        QTextStream is( &f );

        while ( !is.atEnd() )
        {
            QString line( is.readLine() );

            if (( 2 <= line.length() ) && ( ' ' != line[0] ) && ( '#' != line[0] ))
            {
                QStringList fields( line.split( "=" ) );

                if ( 2 == fields.size() )
                {
                    QString value( fields[1].trimmed() );

                    if ( 2 < value.length() )
                    {
                        // strip double quotes
                        if ( '\"' == value[0] )
                            if ( '\"' == value[value.length()-1] )
                                value = value.mid( 1, value.length() - 2 );

                        s[fields[0].trimmed()] = value;
                    }
                }
            }
        }

        f.close();
    }

    return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool AppsScreen::isSettingsValid( const settings &s )
{
    if (( !s.contains( "NAME" ) ) ||
        ( !s.contains( "ICON" ) ) ||
        ( !s.contains( "EXECUTABLE" ) ))
    {
        return false;
    }

    return true;
}
