
#include "errordialogmgr.h"

#include <QCoreApplication>
#include <QStringList>
#include <QTime>
#include <QTimer>

#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <sstream>

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#endif

#include <clio.h>

QMutex errorDialogManager::instanceMutex_( QMutex::Recursive );
errorDialogManager *errorDialogManager::instance_( nullptr );

#define WINDOW_TITLE_DIALOG "HookErrorDialog"
#define WINDOW_TITLE_SCALE_UI "Hobart Scale UI"

#define COMMON_ARGS \
    args.append( "-capture" ); \
    args.append( "-w" ); \
    args.append( "700" ); \
    args.append( "-h" ); \
    args.append( "350" ); \
    args.append( "-fps" ); \
    args.append( "18" ); \
    args.append( "-fg" ); \
    args.append( "#FFFFFF" ); \
    args.append( "-bg" ); \
    args.append( "#E82020" ); \
    args.append( "-msg" ); \
    args.append( msg )


template <typename T>
inline std::string int_to_hex(T val, size_t width=sizeof(T)*2)
{
    static std::locale l( "" );

    std::stringstream ss;
    ss.imbue( l ); // remove commas from formatting

    ss << std::setfill('0') << std::setw(width) << std::hex << (val|0);

    return ss.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
errorDialogManager::errorDialogManager( QObject *parent ) :
    _Mybase( parent ),
    d_( nullptr )
{
    LOG_TRACE << "ctor";
/*
    NOT NEEDED
    I added code into offlineErrorDialog to enforce zorder

    // setup timer to check zorder
    dialogChecking_ = new QTimer( this );
    dialogChecking_->setInterval( CHECKING_INTERVAL );
    dialogChecking_->setSingleShot( false );

    connect( dialogChecking_, SIGNAL(timeout()), SLOT(raiseDialog()) );

    dialogChecking_->start();
*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////
errorDialogManager::~errorDialogManager()
{
    LOG_TRACE << "dtor";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool errorDialogManager::showCommError( const QDateTime& offlineTime, int hoursRemaining )
{
    const QString when( offlineTime.toString("hh:mm ap on dd MMM") );

    const QString msg(
        "Communication error detected at &lt;" + when + "&gt;<br>"
        "<br>"
        "The data on this scale will only be available for &lt;<b>" + QString::number( hoursRemaining ) + "</b>&gt; hours.<br>"
        "<br>"
        "If you wish to print, please make sure the network cable is<br>"
        "connected to the scale properly before making a selection below." );

    const QString reject( "Cancel print request" );
    const QString accept( "Continue with print request" );

    // ---- //

    const QString program( "offlineErrorDialog" );

    QStringList args;
    COMMON_ARGS;
    args.append( "-a" );
    args.append( accept );
    args.append( "-r" );
    args.append( reject );

    // ---- //

    LOG_INFO << "create comm error dialog...";
    const bool result( createProcess( program, args, true ) );

    LOG_TRACE << "done " << result;

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorDialogManager::showContinueToUse( const QDateTime& offlineTime, int hoursRemaining, void **d )
{
    const QString when( offlineTime.toString("hh:mm ap on dd MMM") );

    const QString msg(
        "Communication error detected at &lt;" + when + "&gt;<br>"
        "<br>"
        "The data on this scale will only be available for &lt;<b>" + QString::number( hoursRemaining ) + "</b>&gt; hours.<br>"
        "<br>"
        "Continue to use scale.");

    const QString accept( "Continue to use scale" );

    // ---- //

    const QString program( "offlineErrorDialog" );

    QStringList args;
    COMMON_ARGS;
    args.append( "-a" );
    args.append( accept );

    // ---- //

    LOG_INFO << "create continue to use dialog...";
    createProcess( program, args, false );

    LOG_TRACE << "done";

    if ( d )
    {
        QMutexLocker guard( &instanceMutex_ );
        *d = reinterpret_cast<void*>( d_ );

        LOG_DEBUG << "dialog given to caller 0x" << int_to_hex( (size_t) *d );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorDialogManager::showErrorStillDetected( int waitTime )
{
    const QString msg(
        "Communication error still detected, but the label is printing.<br>"
        "<br>"
        "Please contact Field Services to raise an Incident." );

    // ---- //

    const QString program( "offlineErrorDialog" );

    QStringList args;
    COMMON_ARGS;
    args.append( "-timer" );
    args.append( QString::number( waitTime ) );

    // ---- //

    LOG_INFO << "create error still detected dialog...";
    createProcess( program, args, false );

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorDialogManager::showNotAvailable( const QDateTime& offlineTime, void **d )
{
    const QString when( offlineTime.toString("hh:mm ap on dd MMM") );

    const QString msg(
        "Communication error detected at &lt;" + when + "&gt;<br>"
        "<br>"
        "This scale is not available for use.<br>"
        "<br>"
        "Please escalate the Service Incident for this device." );

    // ---- //

    const QString program( "offlineErrorDialog" );

    QStringList args;
    COMMON_ARGS;

    // ---- //

    LOG_INFO << "create not available dialog...";
    createProcess( program, args, false );

    LOG_TRACE << "done";

    if ( d )
    {
        QMutexLocker guard( &instanceMutex_ );
        *d = reinterpret_cast<void*>( d_ );

        LOG_DEBUG << "dialog given to caller 0x" << int_to_hex( (size_t) *d );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorDialogManager::showLoadingDatabase( int waitTime )
{
    const QString msg(
        "Please wait while PLU data is refreshed.<br>"
        "<br>"
        "This will take a few minutes to complete..." );

    // ---- //

    const QString program( "offlineErrorDialog" );

    QStringList args;
    COMMON_ARGS;
    args.append( "-timer" );
    args.append( QString::number( waitTime ) );

    // ---- //

    LOG_INFO << "create loading database dialog...";
    createProcess( program, args, false );

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorDialogManager::showLoadingDatabaseInProgressError( int waitTime )
{
    const QString msg(
        "A PLU data refresh is already in process!<br>"
        "<br>"
        "Please wait for it to complete before starting another." );

    // ---- //

    const QString program( "offlineErrorDialog" );

    QStringList args;
    COMMON_ARGS;
    args.append( "-timer" );
    args.append( QString::number( waitTime ) );

    // ---- //

    LOG_INFO << "create loading database in progress error dialog...";
    createProcess( program, args, false );

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorDialogManager::terminate( void *d )
{
    QMutexLocker guard( &instanceMutex_ );

    if ( d_ )
    {
        LOG_TRACE << "checking for 0x" << int_to_hex( (size_t) d );

        if (( !d ) || ( d == reinterpret_cast<void*>( d_ ) ))
        {
            LOG_DEBUG << "terminate dialog process 0x" << int_to_hex( (size_t) reinterpret_cast<void*>( d_ ) );
            d_->terminate();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
errorDialogManager *errorDialogManager::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
            instance_ = new _Myt();
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorDialogManager::onProcessFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    QProcess *p( qobject_cast<QProcess*>( sender() ) );

    if ( !p )
        return;

    LOG_INFO << "dialog process 0x" << int_to_hex( (size_t) reinterpret_cast<void*>( p ) ) << " completed " << exitCode << " " << exitStatus;

    // emit result
    emit dialogComplete( p, (( QProcess::NormalExit == exitStatus ) && ( 0 == exitCode )) );

    LOG_TRACE << "flag process for delete";
    p->deleteLater();

    {
        QMutexLocker guard( &instanceMutex_ );

        if ( d_ == p )
            d_ = nullptr;
    }

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void errorDialogManager::raiseDialog()
{
    QMutexLocker guard( &instanceMutex_ );

    LOG_TRACE << "raise dialog";

    if ( d_ )
        bringToFront( WINDOW_TITLE_DIALOG );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool errorDialogManager::createProcess( const QString& program, const QStringList& args, bool blocking )
{
    bool result( true );

    QProcess *d;
    QProcess *dterm( nullptr );

    LOG_TRACE << "entry";

    {
        QMutexLocker guard( &instanceMutex_ );

        if ( d_ )
            dterm = d_;

        // create new process
        d_ = d = new QProcess();

        connect( d, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(onProcessFinished(int,QProcess::ExitStatus)) );
    }

    // terminate old process
    if ( dterm )
    {
        LOG_INFO << "terminate previous dialog process 0x" << int_to_hex( (size_t) reinterpret_cast<void*>( dterm ) );
        dterm->terminate();
    }

    LOG_INFO << "dialog process 0x" << int_to_hex( (size_t) reinterpret_cast<void*>( d ) ) << " starting";
    d->setProgram( program );
    d->setArguments( args );
    d->start();

    LOG_TRACE << "wait for started";
    d->waitForStarted();

    LOG_TRACE << "raise";
    raiseDialog();

    if ( !blocking )
    {
        LOG_DEBUG << "moving to background";
        d->moveToThread( thread() );
        d->setParent( this );
    }
    else
    {
        const QDateTime startTime( QDateTime::currentDateTime() );

        LOG_DEBUG << "wait for finished";
        d->waitForFinished( std::numeric_limits<int>::max() );

        LOG_INFO << "dialog process 0x" << int_to_hex( (size_t) reinterpret_cast<void*>( d ) ) << " completed " << d->exitCode() << " " << d->exitStatus();
        result = (( QProcess::NormalExit == d->exitStatus() ) && ( 0 == d->exitCode() ));

        LOG_INFO << "dialog process 0x" << int_to_hex( (size_t) reinterpret_cast<void*>( d ) ) << " took " << startTime.msecsTo( QDateTime::currentDateTime() ) << " ms";

        {
            QMutexLocker guard( &instanceMutex_ );

            if ( d_ == d )
                d_ = nullptr;
        }

        LOG_TRACE << "delete dialog";
        delete d;
    }

    LOG_TRACE << "done";
    return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool errorDialogManager::bringToFront( const QString& name )
{
    const QByteArray windowTitle( name.toLatin1() );

    bool result( false );

#ifdef Q_OS_LINUX
    Display *disp( XOpenDisplay( NULL ) );

    // this code will find the window we just displayed and resize it to fullscreen and
    // also modify zorder to bring to top

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
                LOG_WARN << "failed to get window property 0x" << int_to_hex( rootWindow ) << " " << status;
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
                        LOG_TRACE << "found: 0x" << int_to_hex( w ) << " " << name;

                        if ( 0 == strcmp( windowTitle.constData(), name ) )
                        {
                            const int screen( DefaultScreen( disp ) );
                            const int sw( DisplayWidth( disp, screen ) );
                            const int sh( DisplayHeight( disp, screen ) );

                            LOG_DEBUG << "updating window and zorder...";

                            XMoveResizeWindow( disp, w, 0, 0, sw, sh );
                            XSetInputFocus( disp, w, RevertToPointerRoot, CurrentTime );
                            XMapRaised( disp, w );
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
