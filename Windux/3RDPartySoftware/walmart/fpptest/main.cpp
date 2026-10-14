
#include "mainwindow.h"

#include <QApplication>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );
    a.setApplicationName( QApplication::tr( "Web/FPP Server App" ) );

    mainWindow w;
    w.show();

    return a.exec();
}
