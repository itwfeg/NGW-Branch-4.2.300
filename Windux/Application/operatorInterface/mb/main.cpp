
#include <iostream>

#include "opermsg.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFileInfo>

///////////////////////////////////////////////////////////////////////////////////////////////////
void displayUsage()
{
    std::cout << "MB" << std::endl;
    std::cout << "Copyright (c) 2013 Hobart Corporation" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: mb [[-full] | [[-width w] [-height h]]]" << std::endl;
    std::cout << "              [-fontFamily ff] [-fontPointSize fps]" << std::endl;
    std::cout << "              [-msg m] [-timer t] [-nologo]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "    -full               Show message fullscreen" << std::endl;
    std::cout << "    -width w            Make message dialog of width w" << std::endl;
    std::cout << "    -height h           Make message dialog of height h" << std::endl;
    std::cout << "    -nologo             Suppress default logo" << std::endl;
    std::cout << std::endl;
    std::cout << "    -fontFamily ff      Message font family name" << std::endl;
    std::cout << "    -fontPointSize fps  Message font point size" << std::endl;
    std::cout << "    -msg m              The message to display" << std::endl;
    std::cout << "    -timer t            Close dialog automatically in t milliseconds" << std::endl;
    std::cout << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );

    // Set Application name for Phonon DBUS Interface
    a.setApplicationName( "mb" );

    ///////////////////////////////////////////////////////////////////////////
    // parse arguments

    QString message;

    QSize size( operMsg::defaultSize() );
    bool showFullScreen = false;

    bool showLogo = true;

    QString fontFamily;
    int fontPointSize = 0;

    int timer = 0;

    QStringList arguments = a.arguments();

    // show usage
    if ( arguments.contains( "-help" ) )
    {
        displayUsage();
        return 0;
    }

    // evaluate argument list
    for ( int i = 0; i < arguments.size(); ++i )
    {
        if ( "-fontFamily" == arguments[i] )
            if ( (i+1) < arguments.size() )
                fontFamily = arguments.at( i+1 );

        if ( "-fontPointSize" == arguments[i] )
            if ( (i+1) < arguments.size() )
                fontPointSize = arguments.at( i+1 ).toInt();

        if ( "-full" == arguments[i] )
            showFullScreen = true;

        if ( "-nologo" == arguments[i] )
            showLogo = false;

        if ( "-height" == arguments[i] )
            if ( (i+1) < arguments.size() )
                size.rheight() = arguments.at( i+1 ).toInt();

        if ( "-msg" == arguments[i] )
            if ( (i+1) < arguments.size() )
                message = arguments.at( i+1 );

        if ( "-timer" == arguments[i] )
            if ( (i+1) < arguments.size() )
                timer = arguments.at( i+1 ).toInt();

        if ( "-width" == arguments[i] )
            if ( (i+1) < arguments.size() )
                size.rwidth() = arguments.at( i+1 ).toInt();
    }

    // Override any specified font size to 10 for portion scale slicer
    if (operMsg::isSlicerDevice())
        fontPointSize = 10;

    if ( showFullScreen )
    {
        // check desktop dimensions
        QDesktopWidget *desktop( a.desktop() );

        if ( desktop )
        {
            const QRect desktopRect( desktop->screenGeometry( 0 ) );

            size.rheight() = desktopRect.height();
            size.rwidth() = desktopRect.width();
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // setup dialog

    operMsg w;

    if ( !fontFamily.isEmpty() )
        w.setMessageFontFamily( fontFamily );

    if (operMsg::isSlicerDevice())
        fontPointSize = 10;

    if ( fontPointSize )
        w.setMessageFontPointSize( fontPointSize );

    w.setMessage( message );

    if ( timer )
        w.setTimer( timer );

    w.setWindowTitle( "Message Box" );
    w.setFixedSize( size );
    w.setWindowFlags( Qt::FramelessWindowHint );

    if ( !showLogo )
        w.setLogoVisible( false );

    if ( showFullScreen )
        w.showFullScreen();
    else
        w.show();

    return w.exec();
}
