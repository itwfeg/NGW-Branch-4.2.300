// HobartCustomerHook.cpp : Defines the exported functions for the DLL application.
//

#include "HobartCustomerHook.h"

#include "common.h"
#include "hookhte.h"
#include "mutextrylocker.h"
#include "qtappprivate.h"

#include <QMutex>

#include <cstdlib>

static QMutex hookMutex_;
static const int hookMutexTimeout_ = 16000;

static bool hookInit_ = false;
static bool hookDatabaseOpen_ = false;
static bool hookComplete_ = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_init()
{
    //LOG_DEBUG << "hobart_cust_hook_init()";

    mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

    if ( !guard.isLocked() )
        return false;
    else if ( hookComplete_ )
        return false;
    else if ( hookInit_ )
        return true;

#if defined(Q_OS_LINUX)
    CLIO_INIT( CLIO_LOGGING_CONFIG );
#endif

    LOG_DEBUG << "hobart_cust_hook_init() - REALLY INITIALIZING!";

    qtAppPrivate *p( qtAppPrivate::instance() );

    // finalize hook at exit
    std::atexit( hobart_cust_hook_finalize );

    return (hookInit_ = hteHook::instance()->initialize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_finalize()
{
    LOG_DEBUG << "hobart_cust_hook_finalize()";

    mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

    if ( !guard.isLocked() )
    {
        LOG_WARN << "could not lock hook!";
        return;
    }
    else if ( !hookInit_ )
        return;

    // finalize
    hteHook::instance()->finalize();

    // check for local instance and destroy
    qtAppPrivate *p( qtAppPrivate::instance() );

    if ( p->isLocalInstance() )
        qtAppPrivate::destroy();
    else
    {
        // destroy hook directly
        hteHook::destroy();
    }

    hookComplete_ = true;
    hookDatabaseOpen_ = false;
    hookInit_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_before_open_database( const char *dbname )
{
    LOG_DEBUG << "hobart_cust_hook_before_open_database()  dbname:" << qPrintable(QString(dbname));

    if ( hobart_cust_hook_init() )
    {
        //LOG_DEBUG << "hobart_cust_hook_before_open_database()  hobart_cust_hook_init'ed";

        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return;
        }

        hteHook::instance()->beforeOpenDatabase( dbname );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_open_database()
{
    LOG_DEBUG << "hobart_cust_hook_open_database()";

    if ( hobart_cust_hook_init() )
    {
        //LOG_DEBUG << "hobart_cust_hook_open_database()  hobart_cust_hook_init'ed";

        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return;
        }

        hteHook::instance()->openDatabase();

        hookDatabaseOpen_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_custom_action( const char *action, int *rc, char *rcex, unsigned int rcexLen )
{
    LOG_DEBUG << "hobart_cust_hook_custom_action()  debug  action:" << qPrintable(QString(action));

    if ( hobart_cust_hook_init() )
    {
        //LOG_DEBUG << "hobart_cust_hook_custom_action()  hobart_cust_hook_init'ed";

        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return;
        }
        else if ( !hookDatabaseOpen_ )
        {
            LOG_WARN << "no database!";
            return;
        }

        int result;
        QString resultExtended;

        hteHook::instance()->customAction( action, result, resultExtended );

        LOG_DEBUG << "hobart_cust_hook_custom_action()  after call to customAction(): result=" << result << "  desc=" << qPrintable(QString(resultExtended));

        if ( rc )
            *rc = result;

        if (( rcex ) && ( 0 < rcexLen ))
        {
            const QByteArray a( resultExtended.toLatin1() );
            strncpy( rcex, a.constData(), rcexLen );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_update_peripheral_version( int dev, int devex, int kind, const char *swVer, const char *hwVer )
{
    if ( hobart_cust_hook_init() )
    {
        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return;
        }
        else if ( !hookDatabaseOpen_ )
        {
            LOG_WARN << "no database!";
            return;
        }

        hteHook::instance()->updatePeripheralVersion( dev, devex, kind, swVer, hwVer );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_update_scale_state( int type, const char *value )
{
    if ( hobart_cust_hook_init() )
    {
        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return;
        }
        else if ( !hookDatabaseOpen_ )
        {
            LOG_WARN << "no database!";
            return;
        }

        hteHook::instance()->updateScaleState( type, value );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_config_saved()
{
    LOG_DEBUG << "hobart_cust_hook_config_saved()";

    if ( hobart_cust_hook_init() )
    {
        LOG_DEBUG << "hobart_cust_hook_config_saved()  hobart_cust_hook_init'ed";

        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return;
        }
        else if ( !hookDatabaseOpen_ )
        {
            LOG_WARN << "no database!";
            return;
        }

        hteHook::instance()->configSaved();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_plu_read( const char *plu, const char *deptNum )
{
    if ( hobart_cust_hook_init() )
    {
        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return false;
        }
        else if ( !hookDatabaseOpen_ )
        {
            LOG_WARN << "no database!";
            return false;
        }

        return hteHook::instance()->beforePluRead( plu, deptNum );
    }
    
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_plu_text_read( const char *number, const char *deptNum, int type )
{
    if ( hobart_cust_hook_init() )
    {
        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return false;
        }
        else if ( !hookDatabaseOpen_ )
        {
            LOG_WARN << "no database!";
            return false;
        }

        return hteHook::instance()->beforePluTextRead( number, deptNum, type );
    }
    
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_print( const char *plu, const char *deptNum )
{
    if ( hobart_cust_hook_init() )
    {
        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return false;
        }
        else if ( !hookDatabaseOpen_ )
        {
            LOG_WARN << "no database!";
            return false;
        }

        return hteHook::instance()->beforePrint( plu, deptNum );
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_before_remote_print( const char *plu, const char *deptNum )
{
    if ( hobart_cust_hook_init() )
    {
        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return false;
        }
        else if ( !hookDatabaseOpen_ )
        {
            LOG_WARN << "no database!";
            return false;
        }

        return hteHook::instance()->beforePrint( plu, deptNum, true );
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_after_print( bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel )
{
    //LOG_DEBUG << "hobart_cust_hook_after_print()  success:" << QVariant(success).toString() << "  numLabelsToPrint:" << QVariant(numLabelsToPrint).toString() << "  templateLabel:" << QVariant(templateLabel).toString();

    if ( hobart_cust_hook_init() )
    {
        LOG_DEBUG << "hobart_cust_hook_after_print()  hobart_cust_hook_init'ed";

        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return;
        }
        else if ( !hookDatabaseOpen_ )
        {
            LOG_WARN << "no database!";
            return;
        }

        hteHook::instance()->afterPrint( success, numLabelsToPrint, numLabelsPrinted, templateLabel );
    }
}
