/**
 * @file mainwindow.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "configurewidget.h"
#include "mainwindow.h"
#include "predictionwidget.h"
#include "terminalwidget.h"
#include "webserviceslogviewerwidget.h"

#include <QByteArray>
#include <QColor>
#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QSettings>
#include <QSvgRenderer>
#include <QTcpSocket>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow( const QMap<QString, QString> &params, QWidget *parent ) :
    _Mybase( parent ),
    params_( params ),
    unlocked_( false )
{
    // init
    initialize();
    createLayout();
    translate();

    // default hook action names
    hookActionNames_["ResultAction"] = "Predict";
    hookActionNames_["LabelLastResultAction"] = "LabelLastResult";
    hookActionNames_["ServerSettingsAction"] = "ServerSettings";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::translate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::keyReleaseEvent( QKeyEvent *e )
{
    // keep track of keys
    keyboard_.append( e->text() );

    while ( 6 < keyboard_.length() )
        keyboard_ = keyboard_.mid( 1 );

    if ( "expert" == keyboard_ )
    {
        unlocked_ = true;

        // enable buttons
        if ( scanner_ )
            scanner_->setEnabled( unlocked_ );

        if ( terminal_ )
            terminal_->setEnabled( unlocked_ );
    }

    _Mybase::keyReleaseEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::enableButtons()
{
    enableDisableButtons( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::showMainScreen()
{
    QWidget *w( centralWidget() );

    if ( centralWidget_ == w )
        return;

    initialize();
    createLayout();

    setCentralWidget( centralWidget_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::onButtonClicked()
{
    if ( !sender() )
        return;

    if ( scanner_ == sender() )
    {
        qDebug() << "scanner";

        PredictionWidget *w( new PredictionWidget( this ) );
        connect( w, SIGNAL(exitScreen()), SLOT(showMainScreen()) );

        setCentralWidget( w );
    }
    else if ( viewLogs_ == sender() )
    {
        qDebug() << "view logs";

        WebServicesLogViewerWidget *w( new WebServicesLogViewerWidget( this ) );
        connect( w, SIGNAL(exitScreen()), SLOT(showMainScreen()) );

        setCentralWidget( w );
    }
    else if ( terminal_ == sender() )
    {
        qDebug() << "terminal";

        TerminalWidget *w( new TerminalWidget( this ) );
        connect( w, SIGNAL(exitScreen()), SLOT(showMainScreen()) );

        setCentralWidget( w );
    }
    else if ( config_ == sender() )
    {
        qDebug() << "config";

        ConfigureWidget *w( new ConfigureWidget( this ) );
        connect( w, SIGNAL(exitScreen()), SLOT(showMainScreen()) );

        setCentralWidget( w );
    }
    else if ( exit_ == sender() )
    {
        qDebug() << "close application";
        close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::initialize()
{
    // setup font
    QFont f( font() );
    f.setPointSize( params_["fontpointsize"].toInt() );
    f.setFamily( params_["fontfamily"] );
    setFont( f );

    // setup palette
    QPalette p( palette() );
    p.setColor( QPalette::Button, QColor( params_["background"] ) );
    p.setColor( QPalette::Window, QColor( params_["background"] ) );
    p.setColor( QPalette::ButtonText, QColor( params_["fontcolor"] ) );
    p.setColor( QPalette::Text, QColor( params_["fontcolor"] ) );
    p.setColor( QPalette::WindowText, QColor( params_["fontcolor"] ) );

    const QString bgimage( params_["bgimage"] );

    if ( QFile::exists( bgimage ) )
        p.setBrush( QPalette::Window, QPixmap( bgimage ) );

    setPalette( p );

    // ---- //

    centralWidget_ = new QWidget( this );
    centralWidget_->setObjectName( "centralWidget" );

    // logo
    const QString logosvg( params_["logosvg"] );

    logo_ = new QLabel( centralWidget_ );
    logo_->setObjectName( "logo" );

    // draw logo from svg
    if ( QFile::exists( logosvg ) )
    {
        static const QSize s( 800, 200 );

        QSvgRenderer renderer( logosvg );
#if QT_VERSION_CHECK(5, 15, 0) <= QT_VERSION
        renderer.setAspectRatioMode( Qt::KeepAspectRatio );
#endif
        QImage logo( s, QImage::Format_ARGB32  );
        logo.fill( QColor( params_["background"] ) );

        QPainter p( &logo );
        renderer.render( &p );

        logo_->setPixmap( QPixmap::fromImage( logo ) );
        logo_->setFixedSize( s );
    }

    // load logo
    else
    {
        const QPixmap logo( params_["logo"] );

        if ( logo.isNull() )
            logo_->hide();
        else
        {
            logo_->setPixmap( logo );
            logo_->setFixedSize( logo.size() );
        }
    }

    // scanner
    scanner_ = new QPushButton( centralWidget_ );
    scanner_->setFixedSize( QSize( 200, 200 ) );
    scanner_->setFocusPolicy( Qt::NoFocus );
    scanner_->setIcon( QPixmap( "res/scanner.png") );
    scanner_->setIconSize( QSize( 64, 64 ) );
    scanner_->setObjectName( "scanner" );
    scanner_->setText( tr( "Prediction" ) );
    connect( scanner_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    // view logs
    viewLogs_ = NULL;

    if ( QFile::exists( WebServicesLogViewerWidget::filename() ) )
    {
        viewLogs_ = new QPushButton( centralWidget_ );
        viewLogs_->setFixedSize( QSize( 200, 200 ) );
        viewLogs_->setFocusPolicy( Qt::NoFocus );
        viewLogs_->setIcon( QPixmap( "res/viewLogs.png") );
        viewLogs_->setIconSize( QSize( 64, 64 ) );
        viewLogs_->setObjectName( "viewLogs" );
        viewLogs_->setText( tr( "View Web\nRequests" ) );
        connect( viewLogs_, SIGNAL(clicked()), SLOT(onButtonClicked()) );
    }

    // terminal
    terminal_ = new QPushButton( centralWidget_ );
    terminal_->setFixedSize( QSize( 200, 200 ) );
    terminal_->setFocusPolicy( Qt::NoFocus );
    terminal_->setIcon( QPixmap( "res/terminal.png") );
    terminal_->setIconSize( QSize( 64, 64 ) );
    terminal_->setObjectName( "terminal" );
    terminal_->setText( tr( "Terminal" ) );
    connect( terminal_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    // config
    config_ = new QPushButton( centralWidget_ );
    config_->setFixedSize( QSize( 200, 200 ) );
    config_->setFocusPolicy( Qt::NoFocus );
    config_->setIcon( QPixmap( "res/settings.png") );
    config_->setIconSize( QSize( 64, 64 ) );
    config_->setObjectName( "config" );
    config_->setText( tr( "Settings" ) );
    connect( config_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    // exit
    exit_ = new QPushButton( centralWidget_ );
    exit_->setFixedSize( QSize( 800, 100 ) );
    exit_->setFocusPolicy( Qt::NoFocus );
    exit_->setIcon( QPixmap( "res/exitIcon.png") );
    exit_->setIconSize( QSize( 64, 64 ) );
    exit_->setObjectName( "exit" );
    exit_->setText( tr( "Close" ) );
    exit_->setVisible( "0" != params_["allowclose"] );
    connect( exit_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    // ---- //

    // disable buttons
    if ( scanner_ )
        scanner_->setEnabled( unlocked_ );

    if ( terminal_ )
        terminal_->setEnabled( unlocked_ );


    setCentralWidget( centralWidget_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::createLayout()
{
    QHBoxLayout *upper( new QHBoxLayout );
    upper->addStretch();
    upper->addWidget( logo_ );
    upper->addStretch();

    QHBoxLayout *lower( new QHBoxLayout );
    lower->addStretch();
    lower->addWidget( scanner_ );
    lower->addStretch();

    if ( viewLogs_ )
    {
        lower->addWidget( viewLogs_ );
        lower->addStretch();
    }

    lower->addWidget( terminal_ );
    lower->addStretch();
    lower->addWidget( config_ );
    lower->addStretch();

    QHBoxLayout *exit( new QHBoxLayout );
    exit->addStretch();
    exit->addWidget( exit_ );
    exit->addStretch();

    QVBoxLayout *layout( new QVBoxLayout( centralWidget_ ) );
    layout->addLayout( upper );
    layout->addStretch();
    layout->addLayout( lower );
    layout->addStretch();
    layout->addLayout( exit );
    layout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::enableDisableButtons( bool enabled )
{
    QList<QPushButton*> buttons;
    buttons.append( scanner_ );

    foreach ( QPushButton *b, buttons )
        if ( b )
            b->setEnabled( enabled );

    // in an effort to prevent them from spamming the "database load" buttons lets disable them
    // for a small period of time
    if ( !enabled )
        QTimer::singleShot( BUTTON_TIMEOUT_PERIOD, this, SLOT(enableButtons()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool MainWindow::performHookAction( const QString& action )
{
    const QString server( params_["server"] );
    const quint16 port( params_["port"].toUShort() );

    if (( server.isEmpty() ) || ( port <= 0 ))
    {
        qWarning() << "bad server setup";
        return false;
    }

    qDebug() << "send action" << action;

    // setup buffers
    QByteArray buffer;
    buffer.append( "RTDO\x1f" );
    buffer.append( "HA" + action.toLatin1() + "\x1e" );

    // connect to server
    QTcpSocket s;
    s.connectToHost( QHostAddress( server ), port );

    if ( !s.waitForConnected( 2000 ) )
    {
        qWarning() << "failed to connect";
        return false;
    }
    else if ( !s.write( buffer ) )
    {
        qWarning() << "failed to write command";
        return false;
    }

    s.waitForReadyRead( 5000 );
    s.close();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString MainWindow::lookupHookActionName( const QString& key ) const
{
#if defined( Q_OS_LINUX )
    static const QString configFile( "/usr/share/edgify-apphook/hobart_internal.config" );
#else
    static const QString configFile( "c:\\hobart_internal.config" );
#endif

    // default value
    QString result( hookActionNames_[key] );

    // configured value
    QSettings s( configFile, QSettings::IniFormat );

    const QString value( s.value( key ).toString() );

    if ( value.length() )
    {
        qDebug() << "configured value" << value;
        result = value;
    }

    // give result to caller
    qDebug() << "hook action" << key << result;
    return result;
}

