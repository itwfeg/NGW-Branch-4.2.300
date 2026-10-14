/**
 * @file qtappprivate.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "errordialogmgr.h"
#include "hookhte.h"
#include "qtappprivate.h"

#include <QCoreApplication>
#include <QThread>

#if defined(Q_OS_LINUX)
#include <cstdlib>
#endif

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

        LOG_DEBUG << "waiting for event loop to start...";

        do
        {
            QThread::msleep( 50 );

        } while (( !appThread_->isRunning() ) && ( !app_ ));

        LOG_DEBUG << "waiting for event loop to start... done";

        // allow event loop exec() to start
        QThread::msleep( 2500 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qtAppPrivate::~qtAppPrivate()
{
    if ( app_ )
    {
        LOG_INFO << "exit event loop...";

        // exit app
        app_->exit();

        LOG_DEBUG << "waiting for event loop to exit...";

        // wait for termination
        do
        {
            QThread::msleep( 50 );

        } while ( app_ );

        LOG_DEBUG << "waiting for event loop to exit... done";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool qtAppPrivate::isLocalInstance() const
{
    if ( app_ )
        return true;

    return false;
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
void qtAppPrivate::destroy()
{
    if ( instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( instance_ )
        {
            delete instance_;
            instance_ = nullptr;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void qtAppPrivate::eventLoopThread()
{
    static char app[] = "private.app";

    static int argc = 1;
    static char *argv[] = {app, NULL};

#if defined(Q_OS_LINUX)
    // make sure we have a display
    setenv( "DISPLAY", ":0", 0 );
#endif

    LOG_INFO << "creating application";

    // create application
    QCoreApplication *a( new QCoreApplication( argc, argv ) );

    LOG_TRACE << "create dependants";

    // create dependants
    hteHook::instance();
    errorDialogManager::instance();

    // enter loop
    LOG_DEBUG << "entering event loop...";
    app_ = a;

    a->exec();

    LOG_TRACE << "destroy dependants";

    // destroy dependants
    hteHook::destroy();

    LOG_DEBUG << "exiting event loop";
    app_ = nullptr;

    delete a;
}
