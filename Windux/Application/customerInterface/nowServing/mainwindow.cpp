/**
 * @file mainwindow.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "mainwindow.h"
#include "printticketwidget.h"
#include "queuesignagewidget.h"
#include "servercomm.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::mainWindow( modeType mode, const QMap<QString, QString>& params, QWidget *parent ) :
    _Mybase( parent ),
    mode_( mode ),
    params_( params )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::~mainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::translate()
{
    printTicketWidget *printTicket;
    queueSignageWidget *queueSignage;

    // translate central widget
    if ( (printTicket = dynamic_cast<printTicketWidget*>( centralWidget() )) )
        printTicket->translate();
    else if ( (queueSignage = dynamic_cast<queueSignageWidget*>( centralWidget() )) )
        queueSignage->translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::initialize()
{
    QString server( params_["server"] );
    int port( params_["port"].toInt() );
    QString queue( params_["queue"] );
    int updatePort( params_["updateport"].toInt() );
    serverComm::modeType updateMode( serverComm::tcpServerMode );

    if ( "tcp" == params_["updatetype"] )
        updateMode = serverComm::tcpServerMode;
    else if ( "udp" == params_["updatetype"] )
        updateMode = serverComm::udpServerMode;

    // allocate comm
    serverComm::alloc( server, port, queue, updatePort, updateMode, this );

    // set screen cursor to nothing
    setCursor( QCursor( Qt::BlankCursor ) );

    // setup ui
    if ( printTicket == mode() )
        setCentralWidget( new printTicketWidget( params_, this ) );
    else if ( signage == mode() || onlineOrdering == mode() )
        setCentralWidget( new queueSignageWidget( params_, mode(), this ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::createLayout()
{
}
