
#include "HobartCustomerHook.h"

#include <QThread>

static const int NUM_LOOPS = 4;

int main(int argc, char *argv[])
{
    hobart_cust_hook_init();

    hobart_cust_hook_before_open_database( "dbftp.db" );
    hobart_cust_hook_open_database();

    QThread::msleep( 2 * 60 * 1000 );

    hobart_cust_hook_custom_action( "HealthCheck|MANUAL", nullptr, nullptr, 0 );

    for ( int i = 0; i < NUM_LOOPS; ++i )
    {
        QThread::msleep( 4 * 60 * 1000 );
        hobart_cust_hook_custom_action( "HealthCheck|INTERVAL", nullptr, nullptr, 0 );
    }

    hobart_cust_hook_finalize();

    return 0;
}
