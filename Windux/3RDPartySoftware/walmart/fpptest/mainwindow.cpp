/**
 * @file mainwindow.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "clientalertswidget.h"
#include "clientcommswidget.h"
#include "clientproxycommswidget.h"
#include "healthcheckcommswidget.h"
#include "healthcheckwidget.h"
#include "mainwindow.h"
#include "proxyserverwidget.h"
#include "smtpserverwidget.h"
#include "webserverwidget.h"

#include <QTabWidget>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::mainWindow( QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::~mainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::initialize()
{
    setCentralWidget( central_ = new QTabWidget( this ) );

    fpp_ = new QWidget( central_ );
    fppServer_ = new webServerWidget( fpp_ );
    fppCommLog_ = new clientCommsWidget( fpp_ );

    connect( fppServer_, SIGNAL(legacySupportEnabled(bool)), fppCommLog_, SLOT(setLegacyUpdateAllowed(bool)) );
    connect( fppServer_, SIGNAL(processRequest(QTcpSocket*,QStringList,QByteArray)), fppCommLog_, SLOT(processRequest(QTcpSocket*,QStringList,QByteArray)) );
    connect( fppServer_, SIGNAL(stateChanged(webServerState::type)), fppCommLog_, SLOT(setServerState(webServerState::type)) );

    alerts_ = new QWidget( central_ );
    alertsServer_ = new smtpServerWidget( alerts_ );
    alertsLog_ = new clientAlertsWidget( alerts_ );

    connect( alertsServer_, SIGNAL(processDisconnect(QTcpSocket*)), alertsLog_, SLOT(processDisconnect(QTcpSocket*)) );
    connect( alertsServer_, SIGNAL(processNewConnection(QTcpSocket*)), alertsLog_, SLOT(processNewConnection(QTcpSocket*)) );
    connect( alertsServer_, SIGNAL(processRequest(QTcpSocket*,QString)), alertsLog_, SLOT(processRequest(QTcpSocket*,QString)) );

    proxy_ = new QWidget( central_ );
    proxyServer_ = new proxyServerWidget( proxy_ );
    proxyCommLog_ = new clientProxyCommsWidget( proxy_ );

    connect( proxyServer_, SIGNAL(processRequest(QTcpSocket*,QStringList,QByteArray)), proxyCommLog_, SLOT(processRequest(QTcpSocket*,QStringList,QByteArray)) );

    healthCheck_ = new QWidget( central_ );
    healthCheckServer_ = new healthCheckWidget( healthCheck_ );
    healthCheckCommLog_ = new healthCheckCommsWidget( healthCheck_ );

    connect( healthCheckServer_, SIGNAL(eventSettingsChanged(const QString&,bool,const QString&,bool,const QString&,const QString&,int)), healthCheckCommLog_, SLOT(setEvent(const QString&,bool,const QString&,bool,const QString&,const QString&,int)) );
    connect( healthCheckServer_, SIGNAL(processRequest(QTcpSocket*,QStringList,QByteArray)), healthCheckCommLog_, SLOT(processRequest(QTcpSocket*,QStringList,QByteArray)) );
    connect( healthCheckServer_, SIGNAL(stateChanged(webServerState::type)), healthCheckCommLog_, SLOT(setServerState(webServerState::type)) );

    central_->addTab( fpp_, tr( "Web/FPP" ) );
    central_->addTab( alerts_, tr( "Email" ) );
    central_->addTab( proxy_, tr( "Proxy" ) );
    central_->addTab( healthCheck_, tr( "Health Check" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::createLayout()
{
    QVBoxLayout *central( new QVBoxLayout( central_ ) );

    QVBoxLayout *fpp( new QVBoxLayout( fpp_ ) );
    fpp->setContentsMargins( QMargins() );
    fpp->setSpacing( 0 );
    fpp->addWidget( fppServer_ );
    fpp->addWidget( fppCommLog_, 1 );

    QVBoxLayout *alerts( new QVBoxLayout( alerts_ ) );
    alerts->setContentsMargins( QMargins() );
    alerts->setSpacing( 0 );
    alerts->addWidget( alertsServer_ );
    alerts->addWidget( alertsLog_, 1 );

    QVBoxLayout *proxy( new QVBoxLayout( proxy_ ) );
    proxy->setContentsMargins( QMargins() );
    proxy->setSpacing( 0 );
    proxy->addWidget( proxyServer_ );
    proxy->addWidget( proxyCommLog_, 1 );

    QVBoxLayout *healthCheck( new QVBoxLayout( healthCheck_ ) );
    healthCheck->setContentsMargins( QMargins() );
    healthCheck->setSpacing( 0 );
    healthCheck->addWidget( healthCheckServer_ );
    healthCheck->addWidget( healthCheckCommLog_, 1 );
}
