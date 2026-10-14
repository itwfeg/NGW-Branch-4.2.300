/**
 * @file configurewidget.cpp
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#include "configurewidget.h"
#include "devicediscovery.h"

#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>

static const QString DOWNLOAD_FOLDER( "/usr/local/hobart/download" );

///////////////////////////////////////////////////////////////////////////////////////////////////
ConfigureWidget::ConfigureWidget( QWidget *parent ) :
    _Mybase( parent ),
    downloadFolder_( DOWNLOAD_FOLDER )
{
    if ( downloadFolder_.exists() )
        qDebug() << "download folder exists";
    else
        qWarning() << "download folder DOES NOT EXIST";

    // initialize
    initialize();
    createLayout();

    // parse config
    parseConfig();

    // populate devices
    onDevicesChanged();

    connect( DeviceDiscovery::instance(), &DeviceDiscovery::devicesChanged, this, &_Myt::onDevicesChanged );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigureWidget::onButtonClicked()
{
    if ( accept_ == sender() )
    {
        saveConfig();
    }
    else if ( exit_ == sender() )
    {
        emit exitScreen();
    }
    else
    {
        QPushButton *b( qobject_cast<QPushButton*>( sender() ) );

        // copy tag file to download folder
        if (( b ) && ( tags_.contains( b ) ))
        {
            qDebug() << "handle button press" << b->objectName();

            const QString f( tags_[b] );

            if ( f.isEmpty() )
                qWarning() << "button" << b->objectName() << "has no associated tag";
            else
            {
                const QStringList fparts( f.split( QDir::separator() ) );
                const QString fnew( downloadFolder_.absolutePath() + QDir::separator() + fparts.last() );

                if ( !fnew.isEmpty() )
                {
                    qDebug() << "copy" << f << "to" << fnew << "...";
                    QFile::copy( f, fnew );
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigureWidget::onDevicesChanged()
{
    QStringList devices( DeviceDiscovery::instance()->devices() );

    // clear existing devices
    servers_->clear();

    foreach ( const QString& device, devices )
    {
        const QString hostname( DeviceDiscovery::instance()->hostname( device ) );

        if ( hostname.isEmpty() )
            servers_->addItem( device );
        else
            servers_->addItem( device + " (" + hostname + ")" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigureWidget::onItemClicked( QListWidgetItem *item )
{
    if ( !item )
        return;

    // strip off hostname
    QString addr( item->text() );

    const int pos( addr.indexOf( ' ' ) );

    if ( 0 < pos )
        addr.truncate( pos );

    server_->setText( addr );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigureWidget::initialize()
{
    // server label
    serverLabel_ = new QLabel( tr( "Server Address:" ), this );
    serverLabel_->setObjectName( "serverLabel" );

    // server
    server_ = new QLineEdit( this );
    server_->setObjectName( "server" );

    // servers
    servers_ = new QListWidget( this );
    servers_->setFocusPolicy( Qt::NoFocus );
    servers_->setObjectName( "servers" );

    connect( servers_, &QListWidget::itemClicked, this, &_Myt::onItemClicked );

    // accept
    accept_ = new QPushButton( this );
    accept_->setMinimumHeight( 64 );
    accept_->setFocusPolicy( Qt::NoFocus );
    accept_->setObjectName( "accept" );
    accept_->setText( tr( "Accept" ) );
    connect( accept_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // edgify off
    edgifyOff_ = new QPushButton( this );
    edgifyOff_->setMinimumHeight( 64 );
    edgifyOff_->setFocusPolicy( Qt::NoFocus );
    edgifyOff_->setObjectName( "edgifyOff" );
    edgifyOff_->setText( tr( "Disable Camera" ) );
    connect( edgifyOff_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // edgify trust 1
    edgifyTrust1_ = new QPushButton( this );
    edgifyTrust1_->setMinimumHeight( 64 );
    edgifyTrust1_->setFocusPolicy( Qt::NoFocus );
    edgifyTrust1_->setObjectName( "edgifyTrust1" );
    edgifyTrust1_->setText( tr( "Trust L1" ) );
    connect( edgifyTrust1_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // edgify trust 2
    edgifyTrust2_ = new QPushButton( this );
    edgifyTrust2_->setMinimumHeight( 64 );
    edgifyTrust2_->setFocusPolicy( Qt::NoFocus );
    edgifyTrust2_->setObjectName( "edgifyTrust2" );
    edgifyTrust2_->setText( tr( "Trust L2 (Certain)" ) );
    connect( edgifyTrust2_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // edgify learning mode
    edgifyLearningOnly_ = new QPushButton( this );
    edgifyLearningOnly_->setMinimumHeight( 64 );
    edgifyLearningOnly_->setFocusPolicy( Qt::NoFocus );
    edgifyLearningOnly_->setObjectName( "edgifyLearningOnly" );
    edgifyLearningOnly_->setText( tr( "Learn Mode" ) );
    connect( edgifyLearningOnly_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // exit
    exit_ = new QPushButton( this );
    exit_->setMinimumHeight( 64 );
    exit_->setFocusPolicy( Qt::NoFocus );
    exit_->setObjectName( "exit" );
    exit_->setText( tr( "Back" ) );
    connect( exit_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // ---- //

    // save off buttons
    tags_[edgifyOff_] = "/usr/share/edgify-apphook/tags/camera_off.ht";
    tags_[edgifyTrust1_] = "/usr/share/edgify-apphook/tags/camera_trust1.ht";
    tags_[edgifyTrust2_] = "/usr/share/edgify-apphook/tags/camera_trust2.ht";
    tags_[edgifyLearningOnly_] = "/usr/share/edgify-apphook/tags/camera_trust3_learning_only.ht";

    // enable/disable buttons
    for ( ButtonTagMap::const_iterator tag( tags_.constBegin() ); tag != tags_.constEnd(); ++tag )
    {
        const bool exist( QFile::exists( tag.value() ) );

        if ( !exist )
            qWarning() << "file" << tag.value() << "does NOT EXIST";

        tag.key()->setEnabled( downloadFolder_.exists() && exist );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigureWidget::createLayout()
{
    QHBoxLayout *server( new QHBoxLayout );
    server->addWidget( serverLabel_ );
    server->addWidget( server_, 1 );
    server->addStretch();

    QVBoxLayout *upper( new QVBoxLayout );
    upper->addLayout( server );
    upper->addWidget( servers_, 1 );

    QHBoxLayout *buttons( new QHBoxLayout );
    buttons->addWidget( accept_ );
    buttons->addStretch();
    buttons->addWidget( edgifyOff_ );
    buttons->addWidget( edgifyTrust1_ );
    buttons->addWidget( edgifyTrust2_ );
    buttons->addWidget( edgifyLearningOnly_ );
    buttons->addStretch();
    buttons->addWidget( exit_ );

    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addLayout( upper, 1 );
    layout->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigureWidget::parseConfig()
{
    QString filename( "/usr/share/edgify-apphook/hobart_internal.config" );

    if ( !QFile::exists( filename ) )
        filename = "hobart_internal.config";

    if ( !QFile::exists( filename ) )
        return;

    QFile f( filename );

    if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return;

    QTextStream is( &f );

    while ( !is.atEnd() )
    {
        const QString line( is.readLine() );

        if ( line.startsWith( "HostAddress" ) )
        {
            const QStringList parts( line.split( '=' ) );

            if ( 2 != parts.size() )
                continue;

            const QString addr( parts[1].trimmed() );

            if ( 3 != addr.count( "." ) )
                continue;

            // found valid host address
            server_->setText( addr );
            break;
        }
    }

    f.close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigureWidget::saveConfig()
{
    const QString addr( server_->text().trimmed() );

    if ( 3 != addr.count( "." ) )
        return;

    QString filename( "/usr/share/edgify-apphook/hobart_internal.config" );

    if ( !QFile::exists( filename ) )
        filename = "hobart_internal.config";

    if ( !QFile::exists( filename ) )
        return;

    QFile f( filename );

    if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return;

    // read in old config
    QStringList lines;

    {
        QTextStream is( &f );

        while ( !is.atEnd() )
            lines.append( is.readLine() );
    }

    f.close();

    // backup old config
    const QString filenameBackup( filename + ".backup" );

    if ( QFile::exists( filenameBackup ) )
        QFile::remove( filenameBackup );

    QFile::rename( filename, filenameBackup );

    // write new config
    if ( !f.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        QFile::rename( filenameBackup, filename );
        return;
    }

    {
        QTextStream os( &f );

        foreach ( const QString& line, lines )
        {
            if ( line.startsWith( "HostAddress" ) )
                os << "HostAddress = " + addr;
            else
                os << line;

            os << "\n";
        }
    }

    f.close();

    emit exitScreen();
}
