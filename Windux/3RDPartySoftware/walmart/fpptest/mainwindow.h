/**
 * @file mainwindow.h
 * Application main window.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class clientAlertsWidget;
class clientCommsWidget;
class clientProxyCommsWidget;
class healthCheckCommsWidget;
class healthCheckWidget;
class proxyServerWidget;
class smtpServerWidget;
class webServerWidget;

class QTabWidget;
class QWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Main window.
class mainWindow : public QMainWindow
{
    Q_OBJECT

    typedef mainWindow _Myt;
    typedef QMainWindow _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  perent widget
     */
    mainWindow( QWidget *parent = 0 );

    /// Destructor.
    virtual ~mainWindow();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size
     */
    virtual QSize sizeHint() const {return QSize( 1280, 1024 );}

private:

    QTabWidget *central_;

    QWidget *fpp_;
    webServerWidget *fppServer_;
    clientCommsWidget *fppCommLog_;

    QWidget *alerts_;
    smtpServerWidget *alertsServer_;
    clientAlertsWidget *alertsLog_;

    QWidget *proxy_;
    proxyServerWidget *proxyServer_;
    clientProxyCommsWidget *proxyCommLog_;

    QWidget *healthCheck_;
    healthCheckWidget *healthCheckServer_;
    healthCheckCommsWidget *healthCheckCommLog_;

    /// Initialize
    void initialize();

    /// Create layout;
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW_H
