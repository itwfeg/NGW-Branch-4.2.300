// HobartCustomerHook.cpp : Defines the exported functions for the DLL application.
//

#include "HobartCustomerHook.h"

#include "customerhook.h"
#include "mutextrylocker.h"
#include "qtappprivate.h"

#include <QMutex>

#include <cstdlib>

#include <clio.h>

static QMutex hookMutex_;
static const int hookMutexTimeout_ = 16000;

static bool hookInit_ = false;
static bool hookDatabaseOpen_ = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool CUSTHOOKDLL hobart_cust_hook_init()
{
    mutexTryLocker guard( &hookMutex_, hookMutexTimeout_ );

    if ( !guard.isLocked() )
        return false;
    else if ( hookInit_ )
        return true;

    CLIO_INIT( "/usr/share/walmart-apphook/apphook.config" );

    qtAppPrivate *p( qtAppPrivate::instance() );

    // finalize hook at exit
    std::atexit( hobart_cust_hook_finalize );

    return (hookInit_ = customerHook::instance()->init());
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

    customerHook::destroy();

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

        customerHook::instance()->configSaved();
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

        customerHook::instance()->beforeOpenDatabase( dbname );
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

        customerHook::instance()->openDatabase();

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

        return customerHook::instance()->beforePluRead( plu, deptNum );
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

        return customerHook::instance()->beforePluTextRead( number, deptNum, type );
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

        return customerHook::instance()->beforePrint( plu, deptNum );
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

        return customerHook::instance()->beforePrint( plu, deptNum, true );
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_after_print( bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel )
{
    Q_UNUSED( templateLabel )

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

        customerHook::instance()->afterPrint( success, numLabelsToPrint, numLabelsPrinted );
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

        customerHook::instance()->customAction( action, result, resultExtended );

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
    Q_UNUSED( dev )
    Q_UNUSED( devex )
    Q_UNUSED( kind )
    Q_UNUSED( swVer )
    Q_UNUSED( hwVer )
}

///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void CUSTHOOKDLL hobart_cust_hook_update_scale_state( int type, const char *value )
{
    Q_UNUSED( type )
    Q_UNUSED( value )
}
