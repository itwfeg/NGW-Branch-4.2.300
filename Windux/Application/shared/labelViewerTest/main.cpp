/**
 * @file main.cpp
 * Application entry point.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include <QApplication>
#include "labelviewertest.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Application entry point.
 *
 * Initialize the application and show dialog.
 *
 * @param[in] argc  argument count
 * @param[in] argv  arguments passed to application
 * @returns  exit code
 */
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );

    ///////////////////////////////////////////////////////
    // initialize logging

    logManager::initialize( "labelviewertest.txt", create_root_file_appender );
    logManager::instance()->rootAppender().setLevel( logLevel::Debug );

    ///////////////////////////////////////////////////////
    // show dialog

    labelViewerTest w;
    w.show();
    
    return a.exec();
}
