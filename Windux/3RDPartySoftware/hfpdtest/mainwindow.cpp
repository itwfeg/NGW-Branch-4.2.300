
#include "devicediscovery.h"
#include "mainwindow.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::mainWindow( QWidget *parent ) :
    _Mybase( parent )
{
    deviceDiscovery *w( new deviceDiscovery( this ) );

    connect( w, SIGNAL(exit()), SLOT(close()) );

    // setup form
    setCentralWidget( w );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::~mainWindow()
{
}
