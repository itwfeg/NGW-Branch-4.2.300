/**
 * @file main.cpp
 * Application entry point.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <QtGui/QApplication>
#include "marqueetest.h"

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
    QApplication a(argc, argv);
    MarqueeTest w;
    w.show();
    
    return a.exec();
}
