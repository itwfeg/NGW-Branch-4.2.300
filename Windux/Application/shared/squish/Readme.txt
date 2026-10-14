
The qtbuiltinhook.h file in this directory is from Squish.  It is included in main() programs
to allow Squish to attach to them when they are already running.


Code changes needed in main():

1.  Include qtbuiltinhook.h header file

#include "qtbuiltinhook.h"


2.  Add call to allowAttaching() function

int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );


    // Argument is a port number that the application should listen on for squishserver to connect
    Squish::allowAttaching( int port_no );

    .
    .
    .
