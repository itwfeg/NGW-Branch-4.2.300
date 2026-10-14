
#include "HobartCustomerHook.h"

#include <QFile>
#include <QThread>

static const int NUM_LOOPS = 1024;

///////////////////////////////////////////////////////////////////////////////////////////////////
QString findScaleDatabase()
{
    static const QString letters( "abcdefghijklmnopqrstuvwxyz" );
    static const QString dbnametemplate( "dbft%1.db" );

#if defined(Q_OS_LINUX)
    static const QString path( "/usr/local/hobart/db/scale/dbfiles/" );
#elif defined(Q_OS_WIN32)
    static const QString path;
#endif

    foreach ( const QChar& c, letters )
    {
        const QString f( path + dbnametemplate.arg( c ) );

        if ( QFile::exists( f ) )
            return f;
    }

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    Q_UNUSED( argc )
    Q_UNUSED( argv )

    const QString dbname( findScaleDatabase() );

    // find a database
    if ( dbname.length() )
    {
        hobart_cust_hook_init();

        hobart_cust_hook_before_open_database( dbname.toLatin1() );
        hobart_cust_hook_open_database();

        for ( int i = 0; i < NUM_LOOPS; ++i )
        {
            hobart_cust_hook_custom_action( "Predict", nullptr, nullptr, 0 );
            QThread::msleep( 15 * 1000 );
        }

        hobart_cust_hook_finalize();
    }

    return 0;
}
