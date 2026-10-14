// HobartCustomerHook.cpp : Defines the exported functions for the DLL application.
//

#include "HobartCustomerHook.h"

#include "common.h"
#include "hooktiliter.h"
#include "mutextrylocker.h"
#include "qtappprivate.h"

#include <QMutex>

#include <cstdlib>

#if defined(Q_OS_LINUX)
#define CLIO_LOGGING_CONFIG "/usr/share/tiliter-apphook/logging.config"
#elif defined(Q_OS_WIN32)
#define CLIO_LOGGING_CONFIG "logging.config"
#endif

static QMutex hookMutex_;
static const int hookMutexTimeout_ = 16000;

static bool hookInit_ = false;
static bool hookDatabaseOpen_ = false;
static bool hookComplete_ = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_init()
{
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

    qtAppPrivate *p( qtAppPrivate::instance() );

    // finalize hook at exit
    std::atexit( hobart_cust_hook_finalize );

    return (hookInit_ = tiliterHook::instance()->initialize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_finalize()
{
    mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

    if ( !guard.isLocked() )
    {
        LOG_WARN << "could not lock hook!";
        return;
    }
    else if ( !hookInit_ )
        return;

    // finalize
    tiliterHook::instance()->finalize();

    // check for local instance and destroy
    qtAppPrivate *p( qtAppPrivate::instance() );

    if ( p->isLocalInstance() )
        qtAppPrivate::destroy();
    else
    {
        // destroy hook directly
        tiliterHook::destroy();
    }

    hookComplete_ = true;
    hookDatabaseOpen_ = false;
    hookInit_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_config_saved()
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

        tiliterHook::instance()->configSaved();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_before_open_database( const char *dbname )
{
    if ( hobart_cust_hook_init() )
    {
        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return;
        }

        tiliterHook::instance()->beforeOpenDatabase( dbname );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_open_database()
{
    if ( hobart_cust_hook_init() )
    {
        mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

        if ( !guard.isLocked() )
        {
            LOG_WARN << "could not lock hook!";
            return;
        }

        tiliterHook::instance()->openDatabase();

        hookDatabaseOpen_ = true;
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

        return tiliterHook::instance()->beforePluRead( plu, deptNum );
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

        return tiliterHook::instance()->beforePluTextRead( number, deptNum, type );
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

        return tiliterHook::instance()->beforePrint( plu, deptNum );
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

        return tiliterHook::instance()->beforePrint( plu, deptNum, true );
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_after_print( bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel )
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

        tiliterHook::instance()->afterPrint( success, numLabelsToPrint, numLabelsPrinted, templateLabel );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_custom_action( const char *action, int *rc, char *rcex, unsigned int rcexLen )
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

        int result;
        QString resultExtended;

        tiliterHook::instance()->customAction( action, result, resultExtended );

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

        tiliterHook::instance()->updatePeripheralVersion( dev, devex, kind, swVer, hwVer );
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

        tiliterHook::instance()->updateScaleState( type, value );
    }
}
