
#include "HobartCustomerHook.h"

#include <QThread>

static const int NUM_LOOPS = 4;

///////////////////////////////////////////////////////////////////////////////////////////////////
void doHealthChecks()
{
    QThread::msleep( 2 * 60 * 1000 );

    hobart_cust_hook_custom_action( "HealthCheck|MANUAL", nullptr, nullptr, 0 );

    for ( int i = 0; i < NUM_LOOPS; ++i )
    {
        QThread::msleep( 4 * 60 * 1000 );
        hobart_cust_hook_custom_action( "HealthCheck|INTERVAL", nullptr, nullptr, 0 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void lookupItem( const QString& plu )
{
    const QString cmd( "MergeProductLoad|" + plu + "|0" );

    hobart_cust_hook_custom_action( qPrintable( cmd ), nullptr, nullptr, 0 );
}

void doSingleProductLoads()
{
    lookupItem( "63115" );
/*
    lookupItem( "204" );

    lookupItem( "1" );
    lookupItem( "4011" );
    lookupItem( "60252" );
    lookupItem( "98751" );

    // threaded
    QThread *i1 = QThread::create( lookupItem, "1" );
    i1->start();

    QThread *i98751 = QThread::create( lookupItem, "98751" );
    i98751->start();

    i98751->wait();
    delete i98751;

    i1->wait();
    delete i1;
*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    hobart_cust_hook_init();

    hobart_cust_hook_before_open_database( "dbftp.db" );
    hobart_cust_hook_open_database();

    //doHealthChecks();
    QThread::msleep( 60 * 1000 );
    doSingleProductLoads();
    QThread::msleep( 60 * 1000 );

    hobart_cust_hook_finalize();

    return 0;
}
