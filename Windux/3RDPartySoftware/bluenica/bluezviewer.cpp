/**
 * @file bluezviewer.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "inputwidgethook.h"
#include "mainwindow.h"
#include "touchscreenstyle.h"

#ifdef Q_OS_WIN
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <string>

#include <QApplication>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );
    a.setStyle( new touchScreenStyle() );

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

    // virtual keyboard
    inputWidgetHook virtualInputDevice;

    // launch!
    mainWindow w;
    w.showFullScreen();

    return a.exec();
}
