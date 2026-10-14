
#include "customerhook.h"
#include "errordialogmgr.h"
#include "qtappprivate.h"

#include <QCoreApplication>
#include <QThread>

#include <cstdlib>

#include <clio.h>

QMutex qtAppPrivate::instanceMutex_;
qtAppPrivate *qtAppPrivate::instance_( nullptr );

///////////////////////////////////////////////////////////////////////////////////////////////////
qtAppPrivate::qtAppPrivate() :
    _Mybase( nullptr )
{
    LOG_TRACE << "ctor";

    if ( !(app_ = QCoreApplication::instance()) )
    {
        LOG_TRACE << "create thread";

        // start event loop thread
        appThread_ = new QThread();
        connect( appThread_, SIGNAL(started()), SLOT(eventLoopThread()), Qt::DirectConnection );

        LOG_INFO << "starting event loop...";
        appThread_->start();

        do
        {
            QThread::currentThread()->msleep( 50 );

        } while (( !appThread_->isRunning() ) && ( !app_ ));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qtAppPrivate::~qtAppPrivate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qtAppPrivate *qtAppPrivate::instance()
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
void qtAppPrivate::eventLoopThread()
{
    static char appname[] = "private.app";

    static int argc = 1;
    static char * argv[] = {appname, NULL};

    // make sure we have a display
    setenv( "DISPLAY", ":0", 0 );

    LOG_INFO << "creating application";

    // create application
    QCoreApplication *a( new QCoreApplication( argc, argv ) );

    // create dependants
    customerHook::instance();
    errorDialogManager::instance();

    // enter loop
    LOG_DEBUG << "entering event loop...";
    app_ = a;

    a->exec();

    LOG_DEBUG << "exiting event loop";
    app_ = nullptr;

    delete a;
}
