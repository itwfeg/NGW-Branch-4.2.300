
#include "HobartCustomerHook.h"

#include <QThread>

static const int NUM_LOOPS = 4;

int main(int argc, char *argv[])
{
    hobart_cust_hook_init();

    hobart_cust_hook_before_open_database( "dbftp.db" );
    hobart_cust_hook_open_database();

    for ( int i = 0; i < NUM_LOOPS; ++i )
    {
        hobart_cust_hook_before_print( "123", "0" );
        hobart_cust_hook_after_print( true, 1, 1, false );

        QThread::msleep( 60 * 1000 );
    }

    hobart_cust_hook_finalize();

    return 0;
}
