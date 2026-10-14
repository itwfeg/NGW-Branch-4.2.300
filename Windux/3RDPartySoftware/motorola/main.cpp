
#include <QCoreApplication>
#include <QThread>

#include "motorolassiscanner.h"

#include <iostream>

int main( int argc, char *argv[] )
{
    Q_UNUSED( argc );
    Q_UNUSED( argv );

#ifdef __linux__
    motorolaScanner scanner( "/dev/ttySymbolScanner" );
#else
    motorolaScanner scanner( "COM4" );
#endif

    if ( scanner.isValid() )
    {
        static const size_t sleepTime( 250 );
        static const size_t idleTime( 5000 );               // original = 2000

        const size_t maxWaitTime( 10000 );                  // original = 30000

        // allow 10s for a scan
        size_t timer( 0 );

        // enable scanner
        scanner.setDisableAfterScan( true );
        scanner.setLedEnabled( true );

        scanner.setEnabled( true );

        do
        {
            motorolaScanner::barcodeType barcodeType;
            std::string barcode;

            // read!
            if ( scanner.read( barcodeType, barcode ) )
            {
                std::cout << "1 scan [" << barcodeType << "] " << barcode << std::endl;

                scanner.setEnabled( false );
                QThread::msleep( idleTime );

                scanner.setEnabled( true );
                timer = 0;
            }

            QThread::msleep( sleepTime );
            timer += sleepTime;

        } while ( timer < maxWaitTime );

        scanner.setEnabled( false );
    }

    return 0;
}
