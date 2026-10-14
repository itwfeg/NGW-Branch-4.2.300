/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "calibui.h"

#include <iostream>

#include <QApplication>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );


#ifndef QT_DEBUG
    // create kpc
    QString bus( "can0" );

    // check for passed in bus
    for ( int i( 1 ); i < argc; ++i )
    {
        const QString arg( argv[i] );

        if ( arg.contains( "can" ) )
            bus = arg;
    }

    // create manager
    QStringList interfaces;
    interfaces.append( bus );

    canDevManager *canMan( canDevManager::instance() );
    canMan->setInterfaces( interfaces );

    // create device
    kpc *device( canMan->createDevice<kpc>( bus ) );

    if ( !device )
    {
        std::cerr << "error creating kpc device!" << std::endl;
        return -1;
    }

    calibui w(device);
    w.show();

    return a.exec();
#else

    calibui w;
    w.show();

    return a.exec();

#endif
}
