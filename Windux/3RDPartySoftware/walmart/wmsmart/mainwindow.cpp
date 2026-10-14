/**
 * @file mainwindow.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "mainwindow.h"
#include "settingsviewerwidget.h"
#include "webserviceslogviewerwidget.h"
#include "wmconfigviewerwidget.h"

#include <QApplication>
#include <QByteArray>
#include <QColor>
#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QTcpSocket>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

static const QString healthCheckScript( "/usr/local/hobart/python/walmartHealthCheck.py" );

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::mainWindow( const QMap<QString, QString> &params, QWidget *parent ) :
    _Mybase( parent ),
    params_( params )
{
    qDebug() << "hook version" << hookVersion();

    // init
    initialize();
    createLayout();
    translate();

    // default hook action names
    hookActionNames_["FullPluLoadWithDeleteCommand"] = "FullDatabaseLoad";
    hookActionNames_["FullPluLoadCommand"] = "MergeDatabaseLoad";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::~mainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::translate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::enableButtons()
{
    enableDisableButtons( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::showMainScreen()
{
    QWidget *w( centralWidget() );

    if ( centralWidget_ == w )
        return;

    initialize();
    createLayout();

    setCentralWidget( centralWidget_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onButtonClicked()
{
    if ( !sender() )
        return;

    if ( fullDatabaseLoad_ == sender() )
    {
        const QString a( lookupHookActionName( "FullPluLoadWithDeleteCommand" ) );

        qDebug() << "full db load";
        performHookAction( a );

        enableDisableButtons( false );
    }
    else if ( refreshDatabaseLoad_ == sender() )
    {
        const QString a( lookupHookActionName( "FullPluLoadCommand" ) );

        qDebug() << "refresh db load";
        performHookAction( a );

        enableDisableButtons( false );
    }
    else if ( manualHealthCheck_ == sender() )
    {
        qDebug() << "manual health check";
        performHealthCheck();

        enableDisableButtons( false );
    }
    else if ( resetScaleConfig_ == sender() )
    {
        qDebug() << "reset scale config";

        const int rc = QMessageBox::critical(
            this,
            tr( "RESET SCALE" ),
            tr( "You are about to erase this scales configuration and reboot." ) + "\n" +
            "\n" +
            tr( "This action cannot be undone!" ) + "\n" +
            "\n" +
            tr( "ARE YOU SURE YOU WANT TO DO THIS?" ),
            QMessageBox::Yes,
            QMessageBox::No | QMessageBox::Default );

        if ( QMessageBox::Yes != rc )
            qDebug() << "user did not reset scale";
        else
        {
            const QString filename = walmartConfigViewerWidget::walmartConfFilename();
            const QString filenameBackup = filename + ".backup";

            if ( QFile::exists( filenameBackup ) )
                QFile::remove( filenameBackup );

            if ( QFile::exists( filename ) )
                QFile::rename( filename, filenameBackup );

            system( "reboot" );

            enableDisableButtons( false );
        }
    }
    else if ( viewLogs_ == sender() )
    {
        qDebug() << "view logs";

        webServicesLogViewerWidget *w( new webServicesLogViewerWidget( this ) );
        connect( w, SIGNAL(exitScreen()), SLOT(showMainScreen()) );

        setCentralWidget( w );
    }
    else if ( viewSettings_ == sender() )
    {
        qDebug() << "view settings";

        if ( 1 == hookVersion() )
        {
            settingsViewerWidget *w( new settingsViewerWidget( this ) );
            connect( w, SIGNAL(exitScreen()), SLOT(showMainScreen()) );

            setCentralWidget( w );
        }
        else if ( 2 == hookVersion() )
        {
            walmartConfigViewerWidget *w( new walmartConfigViewerWidget( this ) );
            connect( w, SIGNAL(exitScreen()), SLOT(showMainScreen()) );

            setCentralWidget( w );
        }
    }
    else if ( exit_ == sender() )
    {
        qDebug() << "close application";
        close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::initialize()
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
    setPalette( p );

    // ---- //

    const QPixmap logo( params_["logo"] );

    centralWidget_ = new QWidget( this );
    centralWidget_->setObjectName( "centralWidget" );

    // logo
    logo_ = new QLabel( centralWidget_ );
    logo_->setObjectName( "logo" );

    if ( logo.isNull() )
        logo_->hide();
    else
    {
        logo_->setPixmap( logo );
        logo_->setFixedSize( logo.size() );
    }

    fullDatabaseLoad_ =
        refreshDatabaseLoad_ =
        manualHealthCheck_ =
        resetScaleConfig_ =
        viewLogs_ =
        viewSettings_ = NULL;

    if ( 1 == hookVersion() )
    {
        // full plu load
        fullDatabaseLoad_ = new QPushButton( centralWidget_ );
        fullDatabaseLoad_->setFixedSize( QSize( 200, 200 ) );
        fullDatabaseLoad_->setFocusPolicy( Qt::NoFocus );
        fullDatabaseLoad_->setIcon( QPixmap( "res/downloadDatabaseIcon.png") );
        fullDatabaseLoad_->setIconSize( QSize( 64, 64 ) );
        fullDatabaseLoad_->setObjectName( "fullDatabaseLoad" );
        fullDatabaseLoad_->setText( tr( "Full PLU\nLoad" ) );
        connect( fullDatabaseLoad_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

        // refresh plu load
        refreshDatabaseLoad_ = new QPushButton( centralWidget_ );
        refreshDatabaseLoad_->setFixedSize( QSize( 200, 200 ) );
        refreshDatabaseLoad_->setFocusPolicy( Qt::NoFocus );
        refreshDatabaseLoad_->setIcon( QPixmap( "res/refreshDatabaseIcon.png") );
        refreshDatabaseLoad_->setIconSize( QSize( 64, 64 ) );
        refreshDatabaseLoad_->setObjectName( "fullDatabaseLoad" );
        refreshDatabaseLoad_->setText( tr( "Delta PLU\nLoad" ) );
        connect( refreshDatabaseLoad_, SIGNAL(clicked()), SLOT(onButtonClicked()) );
    }

    else if ( 2 == hookVersion() )
    {
        // manual health check
        if ( QFile::exists( healthCheckScript ) )
        {
            manualHealthCheck_ = new QPushButton( centralWidget_ );
            manualHealthCheck_->setFixedSize( QSize( 200, 200 ) );
            manualHealthCheck_->setFocusPolicy( Qt::NoFocus );
            manualHealthCheck_->setIcon( QPixmap( "res/heartbeatIcon.png") );
            manualHealthCheck_->setIconSize( QSize( 64, 64 ) );
            manualHealthCheck_->setObjectName( "manualHealthCheck" );
            manualHealthCheck_->setText( tr( "Health\nCheck" ) );
            connect( manualHealthCheck_, SIGNAL(clicked()), SLOT(onButtonClicked()) );
        }

        // reset scale config
        if ( QFile::exists( walmartConfigViewerWidget::walmartConfFilename() ) )
        {
            resetScaleConfig_ = new QPushButton( centralWidget_ );
            resetScaleConfig_->setFixedSize( QSize( 200, 200 ) );
            resetScaleConfig_->setFocusPolicy( Qt::NoFocus );
            resetScaleConfig_->setIcon( QPixmap( "res/resetIcon.png") );
            resetScaleConfig_->setIconSize( QSize( 64, 64 ) );
            resetScaleConfig_->setObjectName( "resetScaleConfig" );
            resetScaleConfig_->setText( tr( "Reset\nConfig" ) );
            connect( resetScaleConfig_, SIGNAL(clicked()), SLOT(onButtonClicked()) );
        }
    }

    // view settings
    if (( QFile::exists( settingsViewerWidget::filename() ) ) || QFile::exists( walmartConfigViewerWidget::hobartConfFilename() ))
    {
        viewSettings_ = new QPushButton( centralWidget_ );
        viewSettings_->setFixedSize( QSize( 200, 200 ) );
        viewSettings_->setFocusPolicy( Qt::NoFocus );
        viewSettings_->setIcon( QPixmap( "res/settingsIcon.png") );
        viewSettings_->setIconSize( QSize( 64, 64 ) );
        viewSettings_->setObjectName( "viewSettings" );
        viewSettings_->setText( tr( "View\nSettings" ) );
        connect( viewSettings_, SIGNAL(clicked()), SLOT(onButtonClicked()) );
    }

    // view logs
    if ( QFile::exists( webServicesLogViewerWidget::filename() ) )
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

    setCentralWidget( centralWidget_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::createLayout()
{
    QHBoxLayout *upper( new QHBoxLayout );
    upper->addStretch();
    upper->addWidget( logo_ );
    upper->addStretch();

    QHBoxLayout *lower( new QHBoxLayout );
    lower->addStretch();

    if ( fullDatabaseLoad_ )
    {
        lower->addWidget( fullDatabaseLoad_ );
        lower->addStretch();
    }

    if ( refreshDatabaseLoad_ )
    {
        lower->addWidget( refreshDatabaseLoad_ );
        lower->addStretch();
    }

    if ( manualHealthCheck_ )
    {
        lower->addWidget( manualHealthCheck_ );
        lower->addStretch();
    }

    if ( resetScaleConfig_ )
    {
        lower->addWidget( resetScaleConfig_ );
        lower->addStretch();
    }

    if ( viewLogs_ )
    {
        lower->addWidget( viewLogs_ );
        lower->addStretch();
    }

    if ( viewSettings_ )
    {
        lower->addWidget( viewSettings_ );
        lower->addStretch();
    }

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
void mainWindow::enableDisableButtons( bool enabled )
{
    QList<QPushButton*> buttons;
    buttons.append( fullDatabaseLoad_ );
    buttons.append( refreshDatabaseLoad_ );
    buttons.append( manualHealthCheck_ );
    buttons.append( resetScaleConfig_ );

    foreach ( QPushButton *b, buttons )
        if ( b )
            b->setEnabled( enabled );

    // in an effort to prevent them from spamming the "database load" buttons lets disable them
    // for a small period of time
    if ( !enabled )
        QTimer::singleShot( BUTTON_TIMEOUT_PERIOD, this, SLOT(enableButtons()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mainWindow::performHookAction( const QString& action )
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
QString mainWindow::lookupHookActionName( const QString& key ) const
{
#if defined( Q_OS_LINUX )
    static const QString configFile( "/usr/share/walmart-apphook/hobart_internal.config" );
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

///////////////////////////////////////////////////////////////////////////////////////////////////
int mainWindow::hookVersion()
{
#if defined( Q_OS_LINUX )
    if ( QFile::exists( "/usr/lib/libHobartCustomerHookWalmart.so.2" ) )
        return 2;

    else if ( QFile::exists( "/usr/lib/libHobartCustomerHookWalmart.so" ) )
        return 1;
#endif

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::performHealthCheck()
{
#if defined(Q_OS_LINUX)
    static const QString program( "python" );

    QStringList args;
    args.append( healthCheckScript );

    QProcess p;
    p.start( program, args );

    if ( !p.waitForStarted() )
        qWarning() << "python process did not start!";
    else if ( !p.waitForFinished() )
        qWarning() << "python process did not complete!";

#endif
}
