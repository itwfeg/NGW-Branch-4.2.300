/**
 * @file mainwindow.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "mainwindow.h"
#include "app_hook_v2/htedb.h"
#include "app_hook_v2/common.h"

#include <QApplication>
#include <QByteArray>
#include <QColor>
#include <QPalette>
#include <QDir>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QLabel>
#include <QTcpSocket>
#include <QToolButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QWindow>
#include <QWidget>
#include <QMessageBox>
#include <QGridLayout>
#include <QFileDialog>
#include <QPen>
#include <QProcess>
#include <QPainter>
#include <QPainterPath>
#include <QFont>

#include <regex>

/*
#if defined(Q_OS_LINUX)
#include <clio.h>
#elif defined(Q_OS_WIN32)
#include <QDebug>
#define LOG_ERROR qCritical()
#define LOG_WARN qWarning()
#define LOG_INFO qInfo()
#define LOG_DEBUG qDebug()
#define LOG_TRACE qDebug()
#endif
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::mainWindow( const QMap<QString, QString> &params, QWidget *parent ) :
    _Mybase( parent ),
    params_( params ),
    style_( StyleNormal )
{
    htedb_ = new hteDatabase( nullptr );
    if ( !htedb_ || !htedb_->isValid() )
        qCritical() << "error opening HTe-scale database ";

    // init
    initialize();
    createLayout();
    translate();

    // refresh timer
    refreshTimer_ = new QTimer( this );
    refreshTimer_->setInterval( 15000 );
    refreshTimer_->setSingleShot( false );
    refreshTimer_->start();

    connect( refreshTimer_, SIGNAL(timeout()), SLOT(onProcessRefresh()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::~mainWindow()
{
    if (htedb_)
        delete htedb_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::translate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::resizeEvent( QResizeEvent *e )
{
    if (( isFloatingButtons() ) && ( headerButtonBar_ ))
    {
        const QColor c( params_["background"] );

        // update button bar size and placement
        headerButtonBar_->setFixedSize( QSize( width() / 5, 40 ) );
        headerButtonBar_->raise();
    }

    _Mybase::resizeEvent( e );
}

/*
///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onTextChanged(const QString &newText)
{
    refreshTimer_->stop();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onTextEdited(const QString &newText)
{
    refreshTimer_->stop();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onCursorPositionChanged(int oldpos, int newpos)
{
    refreshTimer_->stop();

}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onButtonClicked()
{
    if ( toggleKeyboard_ == sender() )
        Q_EMIT showInputDevice( secondPageWidget_, true );
    else if ( exit_ == sender() )
    {
        close();
    }
    else if ( performHeartbeat_ == sender() )
    {
        hook_request( "HAHealthCheck|MANUAL", true, 100 );
        QThread::msleep( 1000 );
        onProcessRefresh();

        //QMessageBox msgBox;
        //msgBox.setText( "Heartbeat request issued ..." );
        //msgBox.setIcon(QMessageBox::Information);
        //msgBox.exec();
    }
    else if ( clearUuid_ == sender() )
    {
        if ( !htedb_ || !htedb_->isValid() )
            qCritical() << "error opening HTe-scale database ";
        else
        {
            htedb_->saveConfig(hteDatabase::CFG_UUID, "");
            onProcessRefresh();
            //QString dbData( htedb_->readConfig(hteDatabase::CFG_UUID) );
            //myUuid_->setText( (dbData != "") ? ("UUID:  " + dbData) : "UUID:  not set" );
        }
    }
    else if ( submit_ == sender() )
    {   
        if ( !htedb_ || !htedb_->isValid() )
            qCritical() << "error opening HTe-scale database ";
        else
        {
            QString address_;
            bool validIp( ("0" == params_["validateIpAddress"]) ? true : false);

            std::string ip = serverIp_->text().toStdString();
            std::smatch match;
            const std::regex reg("^[[:digit:]]{1,3}[.][[:digit:]]{1,3}[.][[:digit:]]{1,3}[.][[:digit:]]{1,3}[:][[:digit:]]{1,4}$");

            if ( !validIp )
            {
                if ( std::regex_search( ip, match, reg) )
                {
                    address_ = QString::fromStdString(ip);
                    qDebug() << "Server IP: " << address_;

                    validIp = true;

                    //QMessageBox msgBox;
                    //msgBox.setText( "Server address received." );
                    //msgBox.setIcon(QMessageBox::Information);
                    //msgBox.exec();
                }
                else
                {
                    qDebug() << "Invalid server address format";
                    QMessageBox msgBox;
                    msgBox.setText( "Invalid server address format." );
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();
                }
            }

            if ( validIp )
                htedb_->saveConfig(hteDatabase::CFG_HEARTBEATURL, serverIp_->text());


            validIp = ("0" == params_["validateIpAddress"]) ? true : false;

            if ( !validIp )
            {
                ip = backupServerIp_->text().toStdString();
                if ( std::regex_search( ip, match, reg) )
                {
                    address_ = QString::fromStdString(ip);
                    qDebug() << "Server IP: " << address_;

                    validIp = true;

                    //QMessageBox msgBox;
                    //msgBox.setText( "Server address received." );
                    //msgBox.setIcon(QMessageBox::Information);
                    //msgBox.exec();
                }
                else
                {
                    qDebug() << "Invalid backup server address format";
                    QMessageBox msgBox;
                    msgBox.setText( "Invalid backup server address format." );
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();
                }
            }

            if ( validIp )
                htedb_->saveConfig(hteDatabase::CFG_HEARTBEATBACKUPURL, backupServerIp_->text());

            htedb_->saveConfig(hteDatabase::CFG_HEARTBEATENDPOINT, heartbeatEndpoint_->text());

            htedb_->saveConfig(hteDatabase::CFG_HEARTBEATDOWNLOADURL, downloadServerUrl_->text());
        }

        refreshTimer_->start();

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onProcessRefresh()
{
    QString dbData("");
    bool haveDB( htedb_ && htedb_->isValid() );

    if ( haveDB )
    {
        dbData = htedb_->readConfig(hteDatabase::CFG_UUID);
        myUuid_->setText( ( dbData != "" ) ? ("UUID:  " + dbData) : "UUID:  not set" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::refreshFields()
{
    QString dbData("");
    bool haveDB( htedb_ && htedb_->isValid() );

    if ( haveDB )
    {
        onProcessRefresh();

        dbData = htedb_->readConfig(hteDatabase::CFG_HEARTBEATURL);
        if ( dbData != serverIp_->text() )
            serverIp_->setText( dbData );

        dbData = htedb_->readConfig(hteDatabase::CFG_HEARTBEATBACKUPURL);
        if ( dbData != backupServerIp_->text() )
            backupServerIp_->setText( dbData );

        dbData = htedb_->readConfig(hteDatabase::CFG_HEARTBEATENDPOINT);
        if ( dbData != heartbeatEndpoint_->text() )
            heartbeatEndpoint_->setText( dbData );

        dbData = htedb_->readConfig(hteDatabase::CFG_HEARTBEATDOWNLOADURL);
        if ( dbData != downloadServerUrl_->text() )
            downloadServerUrl_->setText( dbData );
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
    p.setColor( backgroundRole(), QColor( params_["background"] ) );
    p.setColor( QPalette::ButtonText, QColor( params_["fontcolor"] ) );
    p.setColor( QPalette::Text, QColor( params_["fontcolor"] ) );
    p.setColor( QPalette::WindowText, QColor( params_["fontcolor"] ) );
    setPalette( p );

    // ---- //

    const QPixmap logo( params_["logo"] );

    centralWidget_ = new QWidget( this );
    centralWidget_->setObjectName( "centralWidget" );

    stack_ = new QStackedWidget( centralWidget_ );

    secondPageWidget_ = new QWidget();
    secondPageWidget_->setObjectName( "secondPageWidget" );
    secondPageWidget_->setStyleSheet( "background-color:rgb(240,240,240);" );

    myUuid_ = new QLabel( secondPageWidget_ );
    myUuid_->setAlignment( Qt::AlignLeft );
    myUuid_->setFont(f);
    myUuid_->setObjectName( "uuidLabel" );

    serverLabel_ = new QLabel( secondPageWidget_ );
    serverLabel_->setAlignment( Qt::AlignLeft );
    serverLabel_->setFont(f);
    serverLabel_->setObjectName( "serverLabel" );
    serverLabel_->setText( "Primary Server URL" );

    serverIp_ = new QLineEdit(secondPageWidget_);
    serverIp_->setStyleSheet( "background-color:white;" );
    serverIp_->setMaxLength(100);
    //serverIp_->setMaximumWidth(550);
    //serverIp_->setFixedWidth(550);
    serverIp_->setFont(f);
    serverIp_->setPlaceholderText("Enter the primary server address");
    //connect( serverIp_, SIGNAL(textChanged(text)), SLOT(onTextChanged(text)) );
    //connect( serverIp_, SIGNAL(textEdited(text)), SLOT(onTextEdited(text)) );
    //connect( serverIp_, SIGNAL(cursorPositionChanged(oldpos, newpos)), SLOT(onCursorPositionChanged(oldpos, newpos)) );

    backupServerLabel_ = new QLabel( secondPageWidget_ );
    backupServerLabel_->setAlignment( Qt::AlignLeft );
    backupServerLabel_->setFont(f);
    backupServerLabel_->setObjectName( "backupServerLabel" );
    backupServerLabel_->setText( "Backup Server URL" );

    backupServerIp_ = new QLineEdit(secondPageWidget_);
    backupServerIp_->setStyleSheet( "background-color:white;" );
    backupServerIp_->setMaxLength(100);
    //backupServerIp_->setMaximumWidth(550);
    //backupServerIp_->setFixedWidth(550);
    backupServerIp_->setFont(f);
    backupServerIp_->setPlaceholderText("Enter the backup server address");

    heartbeatEndpointLabel_ = new QLabel( secondPageWidget_ );
    heartbeatEndpointLabel_->setAlignment( Qt::AlignLeft );
    heartbeatEndpointLabel_->setFont(f);
    heartbeatEndpointLabel_->setObjectName( "heartbeatEndpointLabel" );
    heartbeatEndpointLabel_->setText( "Heartbeat Endpoint" );

    heartbeatEndpoint_ = new QLineEdit(secondPageWidget_);
    heartbeatEndpoint_->setStyleSheet( "background-color:white;" );
    heartbeatEndpoint_->setMaxLength(200);
    //heartbeatEndpoint_->setMaximumWidth(250);
    //heartbeatEndpoint_->setFixedWidth(250);
    heartbeatEndpoint_->setFont(f);
    heartbeatEndpoint_->setPlaceholderText("Enter the server's heartbeat endpoint");

    // Temporary!!
    downloadServerLabel_ = new QLabel( secondPageWidget_ );
    downloadServerLabel_->setAlignment( Qt::AlignLeft );
    downloadServerLabel_->setFont(f);
    downloadServerLabel_->setObjectName( "downloadServerLabel" );
    downloadServerLabel_->setText( "Download Server URL   **Temporary**" );

    downloadServerUrl_ = new QLineEdit(secondPageWidget_);
    downloadServerUrl_->setStyleSheet( "background-color:white;" );
    downloadServerUrl_->setMaxLength(200);
    downloadServerUrl_->setFont(f);
    downloadServerUrl_->setPlaceholderText("Enter the download server address");


    //QHBoxLayout *secondPageLayout = new QHBoxLayout(secondPageWidget_);
    QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPageWidget_);
    //secondPageLayout->
    secondPageLayout->addSpacing(20);
    secondPageLayout->addWidget(myUuid_);
    secondPageLayout->addSpacing(30);
    secondPageLayout->addWidget(serverLabel_);
    secondPageLayout->addSpacing(3);
    secondPageLayout->addWidget(serverIp_);
    secondPageLayout->addSpacing(20);
    secondPageLayout->addWidget(backupServerLabel_);
    secondPageLayout->addSpacing(3);
    secondPageLayout->addWidget(backupServerIp_);
    secondPageLayout->addSpacing(20);
    secondPageLayout->addWidget(heartbeatEndpointLabel_);
    secondPageLayout->addSpacing(3);
    secondPageLayout->addWidget(heartbeatEndpoint_);
    secondPageLayout->addSpacing(20);
    secondPageLayout->addWidget(downloadServerLabel_);
    secondPageLayout->addSpacing(3);
    secondPageLayout->addWidget(downloadServerUrl_);
    secondPageLayout->addSpacing(15);

    headerButtonBar_ = new QWidget( centralWidget_ );
    headerButtonBar_->setAutoFillBackground( true );
    headerButtonBar_->setObjectName( "buttonBar" );

    logo_ = new QLabel( headerButtonBar_ );
    logo_->setObjectName( "logo" );

    if ( logo.isNull() )
        logo_->hide();
    else
    {
        logo_->setPixmap( logo );
        logo_->setFixedSize( logo.size() );
    }

    title_ = new QLabel( headerButtonBar_ );
    title_->setAlignment( Qt::AlignCenter );
    title_->setObjectName( "title" );
    title_->setText( params_["title"] );

    toggleKeyboard_ = new QToolButton( headerButtonBar_ );
    toggleKeyboard_->setFocusPolicy( Qt::NoFocus );
    toggleKeyboard_->setIcon( QPixmap( "res/showIcon.png") );
    toggleKeyboard_->setIconSize( QSize( 40, 40 ) );
    toggleKeyboard_->setObjectName( "exit" );
    toggleKeyboard_->setStyleSheet( "background: transparent; border: none;" );
    toggleKeyboard_->setVisible( "0" != params_["allowkeyboard"] );
    connect( toggleKeyboard_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    exit_ = new QToolButton( headerButtonBar_ );
    exit_->setFocusPolicy( Qt::NoFocus );
    exit_->setIcon( QPixmap( "res/exitIcon2.png") );
    exit_->setIconSize( QSize( 64, 64 ) );
    exit_->setObjectName( "exit" );
    exit_->setFont(f);
    exit_->setStyleSheet( "background: transparent; border: none;" );
    exit_->setVisible( true); //( "0" != params_["allowclose"] );
    connect( exit_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    actionButtonBar_ = new QWidget( centralWidget_ );
    actionButtonBar_->setAutoFillBackground( true );
    actionButtonBar_->setObjectName( "actionbuttonBar" );

    submit_ = new QPushButton("Save", actionButtonBar_);
    submit_->setStyleSheet( "background-color:green;" );
    connect( submit_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    performHeartbeat_ = new QPushButton("Heartbeat", actionButtonBar_);
    performHeartbeat_->setStyleSheet( "background-color:blue;" );
    connect( performHeartbeat_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    clearUuid_ = new QPushButton("Clear UUID", actionButtonBar_);
    clearUuid_->setStyleSheet( "background-color:yellow;" );
    connect( clearUuid_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    stack_->addWidget( secondPageWidget_ );

    // ---- //
    setCentralWidget( centralWidget_ );

    refreshFields();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::createLayout()
{
    QHBoxLayout *buttons( new QHBoxLayout( headerButtonBar_ ) );

    buttons->setContentsMargins( QMargins( 5, 5, 5, 5 ) );
    buttons->setSpacing( 8 );

    buttons->addWidget( logo_ );
    buttons->addWidget( title_, 1 );
    buttons->addWidget( toggleKeyboard_ );
    buttons->addWidget( exit_ );

    QHBoxLayout *actbuttons( new QHBoxLayout( actionButtonBar_ ) );

    actbuttons->setContentsMargins( QMargins( 5, 5, 5, 5 ) );
    actbuttons->setSpacing( 15 );

    actbuttons->addWidget( submit_ );
    actbuttons->addWidget( performHeartbeat_ );
    actbuttons->addWidget( clearUuid_ );

    QVBoxLayout *layout( new QVBoxLayout( centralWidget_ ) );
    layout->setContentsMargins( QMargins() );
    layout->setSpacing( 0 );

    layout->addWidget( headerButtonBar_ );
    layout->addWidget( stack_, 1 );
    layout->addWidget( actionButtonBar_ );
}

