/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "bluenicauploader.h"
#include "dbuploader.h"
#include "hcidev.h"
#include "hcidevthread.h"

#include <signal.h>
#include <unistd.h>

#include <sys/time.h>

#include <cstdlib>
#include <iostream>
#include <string>

static volatile bool needToExit( false );

///////////////////////////////////////////////////////////////////////////////////////////////////
void sig_handler( int /*unused*/ )
{
    std::cout << "exiting..." << std::endl;
    needToExit = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    // set current working dir to application working dir
    std::string pwd( argv[0] );

    size_t len( 0 );
    size_t idx_bs( pwd.find_last_of( '\\' ) );
    size_t idx_fs( pwd.find_last_of( '/' ) );

    if ( std::string::npos != idx_bs )
        if ( len < idx_bs )
            len = idx_bs;

    if ( std::string::npos != idx_fs )
        if ( len < idx_fs )
            len = idx_fs;

    if ( len )
    {
        std::string new_pwd( pwd.substr( 0, len ) );
        chdir( new_pwd.c_str() );
    }

    // ---- //

    hcidev::scanType type( hcidev::SCAN_TYPE_PASSIVE );

    bool enablebluenica( true );
    bool enabledatabase( true );

    bool offline( false );
    bool print( false );

    // register ctrl-c interrupt handler
    signal( SIGINT, sig_handler );

    // register term signal handler
    signal( SIGTERM, sig_handler );

    // parse arguments
    for ( int i( 1 ); i < argc; ++i )
    {
        if ( 0 == strcmp( argv[i], "-active" ) )
            type = hcidev::SCAN_TYPE_ACTIVE;
        else if ( 0 == strcmp( argv[i], "-passive" ) )
            type = hcidev::SCAN_TYPE_PASSIVE;

        if ( 0 == strcmp( argv[i], "-nobluenica" ) )
            enablebluenica = false;

        if ( 0 == strcmp( argv[i], "-nodatabase" ) )
            enabledatabase = false;

        if ( 0 == strcmp( argv[i], "-offline" ) )
            offline = true;

        if ( 0 == strcmp( argv[i], "-print" ) )
            print = true;
    }

    // ---- //

    // make sure bluetooth device is up
    std::system( "hciconfig hci0 up" );

    // ---- //

    hcidev dev( hcidev::openDefaultDevice() );

    if ( !dev.isValid() )
        std::cout << "could not open device!" << std::endl;
    else if ( !dev.startScanning( type ) )
        std::cout << "failed to enter scan mode!" << std::endl;
    else
    {
        hcidevThread t( &dev );

        bluenicaUploader uploader;
        dbUploader db;

        // set adapter address
        bdaddr_t bdaddr;

        if ( dev.adapterAddress( &bdaddr ) )
            uploader.setAdapterAddress( &bdaddr );

        // set application version
        uploader.setApplicationVersion( 1, 13, 0 );

        // disable uploading
        if ( offline )
            uploader.setEnableUploads( false );

        // start up device read thread
        if (( t.start() ) && ( uploader.start() ))
        {
            static const unsigned int housekeepingTimer( 5 * 60 ); // every 5m

            time_t housekeeping( 0 );

            do
            {
                // process event
                while ( 0 < t.eventCount() )
                {
                    leAdvertInfo info( t.event() );

                    // print event
                    if ( print )
                        info.print();

                    // queue event to uploader
                    if ( enablebluenica )
                        uploader.postEvent( info );

                    // queue event to db
                    if ( enabledatabase )
                        db.postEvent( info );
                }

                // get timestamp
                timeval now;
                gettimeofday( &now, NULL );

                // check housekeeping time
                if ( housekeeping <= now.tv_sec )
                {
                    db.housekeeping();

                    housekeeping = now.tv_sec + housekeepingTimer;
                }

                // chill out a while...
                usleep( 250000 );

            } while ( !needToExit );
        }

        uploader.stop();
        t.stop();

        dev.stopScanning();
    }

    return 0;
}
