/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "calibui.h"

#include "readThread.h"
#include <QMutex>
#include <QObject>
#include <iostream>

#include <QApplication>
#include <QEventLoop>
#include <QTimer>

#include "hobart/mtrd24motor.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#if defined ( __linux__ )		// Windows 
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/unistd.h>
#endif

extern QMutex g_serial_mutex;
///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );
    mtrd24MotorDrive weightStackControl;
    int mytty_fd; 

    mytty_fd = weightStackControl.get_tty_fd();

    ReadThread *workerThread = new ReadThread(mytty_fd);

    QObject::connect( workerThread, SIGNAL(dataReady( QString, int )), &weightStackControl, SLOT(handleDataReady( QString, int  ) ));
    
    workerThread->start();

    std::cerr << "Calling - InitializeWeightStack =  " << std::endl << std::endl;

    ERROR_CODE result;
    result = weightStackControl.initializeWeightStack();
    std::cerr << "InitializeWeightStack =  " << (int)result << std::endl << std::endl;
#if 0   
    result = weightStackControl.Mtrd24FindHome();
    std::cerr << "Mtrd24Home() =  " << (int)result << std::endl << std::endl;
#endif

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

    calibui w(device, &weightStackControl);
    w.show();

    return a.exec();
#else

    calibui w;
    w.show();

    return a.exec();

#endif
}
