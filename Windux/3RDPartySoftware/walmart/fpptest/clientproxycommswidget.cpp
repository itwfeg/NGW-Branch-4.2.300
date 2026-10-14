/**
 * @file clientproxycommswidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "clientproxycommswidget.h"

#include <QtDebug>

#include <QDateTime>
#include <QHostAddress>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QTcpSocket>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVariant>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
clientProxyCommsWidget::clientProxyCommsWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
clientProxyCommsWidget::~clientProxyCommsWidget()
{
    // cleanup blocks
    foreach ( controlBlock *block, blocks_ )
        delete block;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::processRequest( QTcpSocket *client, const QStringList &header, const QByteArray &body )
{
    QString server;
    quint16 port( 0 );

    // parse out server and port info
    if ( !header.size() )
        return;

    QStringList request( header[0].split( " " ) );

    if ( request.size() < 3 )
        return;

    QString type( request[0].toUpper() );
    QString remoteServer( request[1] );

    // well know ports
    if ( remoteServer.contains( "http://" ) )
    {
        remoteServer.replace( "http://", "" );
        port = 80;
    }
    else if ( remoteServer.contains( "https://" ) )
    {
        remoteServer.replace( "https://", "" );
        port = 443;
    }
    else if ( remoteServer.contains( "ftp://" ) )
    {
        remoteServer.replace( "ftp://", "" );
        port = 21;
    }

    // strip out suffix
    if ( remoteServer.contains( "/" ) )
         remoteServer = remoteServer.left( remoteServer.indexOf( "/" ) );
    else if ( remoteServer.contains( "\\" ) )
        remoteServer = remoteServer.left( remoteServer.indexOf( "\\" ) );

    // check for explicit port
    if ( !remoteServer.contains( ":" ) )
        server = remoteServer;
    else
    {
        QStringList remoteServerInfo( remoteServer.split( ":" ) );

        if ( 2 == remoteServerInfo.size() )
        {
            server = remoteServerInfo[0];
            port = remoteServerInfo[1].toUShort();
        }
    }

    // validate
    if (( server.isEmpty() ) || ( port <= 0 ))
        return;

    qDebug() << "new proxy connection:" << client->peerAddress().toString() << "-->" << server;

    // setup control block
    controlBlock *block( new controlBlock );
    block->index_ = tree_->topLevelItemCount();
    block->type_ = type;
    block->state_ = Connecting;
    block->client_ = client;
    block->server_ = new QTcpSocket;
    block->send_ = 0;
    block->recv_ = 0;

    // for non-tunnel, queue up tx
    if ( "CONNECT" != type )
    {
        // forward header, excepting proxy info
        foreach ( const QString &h, header )
            if ( "proxy" != h.left( 5 ).toLower() )
            {
                block->pending_.append( h );
                block->pending_.append( "\r\n" );
            }

        // forward body
        block->pending_.append( "\r\n" );
        block->pending_.append( body );
    }

    // update mapping tables
    comms_[block->client_] = block;
    comms_[block->server_] = block;
    blocks_.append( block );

    //
    // create tree widget item
    //

    const QDateTime now( QDateTime::currentDateTime() );

    // row data
    QStringList rows;
    rows.append( now.toString( "MM/dd/yyyy hh:mm:ss A" ) );
    rows.append( tr( "Connecting" ) );
    rows.append( type );
    rows.append( client->peerAddress().toString() );
    rows.append( request[1] );
    rows.append( "0" );
    rows.append( "0" );

    // create widget item
    QTreeWidgetItem *item( new QTreeWidgetItem( rows ) );

    tree_->addTopLevelItem( item );
    tree_->scrollToBottom();

    // update mapping
    items_[item] = block;

    //
    // connect sockets
    //

    connect( block->client_, SIGNAL(destroyed()), SLOT(onClientDestroyed()) );
    connect( block->client_, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(onClientError(QAbstractSocket::SocketError)) );
    connect( block->client_, SIGNAL(readyRead()), SLOT(onClientReadyRead()) );

    connect( block->server_, SIGNAL(connected()), SLOT(onClientConnected()) );
    connect( block->server_, SIGNAL(destroyed()), SLOT(onClientDestroyed()) );
    connect( block->server_, SIGNAL(disconnected()), client, SLOT(deleteLater()) );
    connect( block->server_, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(onClientError(QAbstractSocket::SocketError)) );
    connect( block->server_, SIGNAL(readyRead()), SLOT(onClientReadyRead()) );

    // go!
    block->server_->connectToHost( server, port );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::resizeEvent( QResizeEvent *e )
{
    setTreeHeaders();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::onClientConnected()
{
    QTcpSocket *sock( dynamic_cast<QTcpSocket*>( sender() ) );

    if (( sock ) && ( comms_.contains( sock ) ))
    {
        controlBlock *block( comms_[sock] );

        qDebug() << "have connection:" << sock->peerName();

        // tunnel
        // send result code to client
        if ( "CONNECT" == block->type_ )
        {
            // send validation to client
            QByteArray response;
            response.append( "HTTP/1.0 200 OK\r\n" );
            response.append( "\r\n" );

            if ( block->client_->write( response ) )
                block->state_ = Active;
            else
            {
                qDebug() << "failed to update client status";
                block->state_ = Error;

                block->client_->close();
                block->server_->close();
            }
        }

        // non-tunnel
        // forward pending rx
        else
        {
            if ( block->server_->write( block->pending_ ) )
            {
                block->state_ = Active;
                block->send_ += block->pending_.size();
            }
            else
            {
                qDebug() << "failed to forward client transmission";
                block->state_ = Error;

                block->client_->close();
                block->server_->close();
            }
        }

        // update tree
        updateTreeItem( block->index_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::onClientDestroyed()
{
    QTcpSocket *sock( dynamic_cast<QTcpSocket*>( sender() ) );

    if (( sock ) && ( comms_.contains( sock ) ))
    {
        controlBlock *block( comms_[sock] );

        // update control block
        if ( sock == block->client_ )
            block->client_ = NULL;
        else if ( sock == block->server_ )
            block->server_ = NULL;

        comms_.remove( sock );

        // disconnect peer
        if ( block->client_ )
            block->client_->close();
        else if ( block->server_ )
            block->server_->close();

        // update state
        if ( Active == block->state_ )
        {
            block->state_ = Complete;

            updateTreeItem( block->index_ );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::onClientError( QAbstractSocket::SocketError error )
{
    QTcpSocket *sock( dynamic_cast<QTcpSocket*>( sender() ) );

    if (( sock ) && ( comms_.contains( sock ) ))
    {
        controlBlock *block( comms_[sock] );

        // disconnect
        if ( block->client_ )
        {
            qDebug() << "client error" << block->client_->peerAddress().toString() << error;
            block->client_->close();
        }

        if ( block->server_ )
        {
            qDebug() << "server error" << block->server_->peerName() << error;
            block->server_->close();
        }

        // update state
        if (( Connecting == block->state_ ) || ( Active == block->state_ ))
        {
            if ( QAbstractSocket::RemoteHostClosedError == error )
                block->state_ = Complete;
            else
                block->state_ = Error;

            updateTreeItem( block->index_ );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::onClientReadyRead()
{
    QTcpSocket *sock( dynamic_cast<QTcpSocket*>( sender() ) );

    if (( sock ) && ( comms_.contains( sock ) ))
    {
        controlBlock *block( comms_[sock] );

        // read data
        QByteArray buffer( sock->readAll() );

        // forward traffic
        if ( sock == block->client_ )
        {
            if ( block->server_->write( buffer ) )
                block->send_ += buffer.size();
            else
            {
                qDebug() << "error forwarding client traffic" << block->client_->peerAddress().toString();
                block->state_ = Error;

                block->client_->close();
                block->server_->close();
            }
        }
        else if ( sock == block->server_ )
        {
            if ( block->client_->write( buffer ) )
                block->recv_ += buffer.size();
            else
            {
                qDebug() << "error forwarding server traffic" << block->server_->peerName();
                block->state_ = Error;

                block->client_->close();
                block->server_->close();
            }
        }

        // update
        updateTreeItem( block->index_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::onPushButtonClicked()
{
    if ( clear_ == sender() )
    {
        // cleanup blocks
        foreach ( controlBlock *block, blocks_ )
            delete block;

        blocks_.clear();
        comms_.clear();
        items_.clear();

        tree_->clear();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::onTreeItemDoubleClicked( QTreeWidgetItem* /*item*/, int /*column*/ )
{
/*
    const QString valueStr( item->data( 0, Qt::DisplayRole ).toString() );
    const int value( valueStr.toInt() );

    if ( comms_.contains( value ) )
    {
        const clientCommData data( comms_[value] );

        QByteArray message;

        foreach ( const QString &s, data.second.second )
            message.append( s + "\r\n" );

        clientCommDialog dialog( QStringList(), message, this );
        dialog.exec();
    }
*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::initialize()
{
    tree_ = new QTreeWidget( this );
    tree_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    tree_->setRootIsDecorated( false );
    tree_->setSelectionMode( QAbstractItemView::SingleSelection );
    tree_->setUniformRowHeights( true );
    connect( tree_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(onTreeItemDoubleClicked(QTreeWidgetItem*,int)) );

    setup_ = new QLabel( tr( "Proxy Login: randilus/maximus" ), this );

    clear_ = new QPushButton( tr( "Clear List" ), this );
    connect( clear_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::createLayout()
{
    QHBoxLayout *buttons( new QHBoxLayout );
    buttons->addWidget( setup_, 1 );
    buttons->addWidget( clear_ );

    QVBoxLayout *form( new QVBoxLayout( this ) );
    form->addWidget( tree_ );
    form->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::setTreeHeaders()
{
    const int treeWidth( tree_->width() );

    // tree headers
    QStringList treeHeaders;
    treeHeaders.push_back( tr( "Time" ) );
    treeHeaders.push_back( tr( "State" ) );
    treeHeaders.push_back( tr( "Type" ) );
    treeHeaders.push_back( tr( "Client" ) );
    treeHeaders.push_back( tr( "Address" ) );
    treeHeaders.push_back( tr( "Bytes Sent" ) );
    treeHeaders.push_back( tr( "Bytes Recv" ) );

    // table
    tree_->setColumnCount( treeHeaders.size() );
    tree_->setHeaderLabels( treeHeaders );

    tree_->setColumnWidth( 0, (10 * treeWidth) / 100 );
    tree_->setColumnWidth( 1, (8 * treeWidth) / 100 );
    tree_->setColumnWidth( 2, (8 * treeWidth) / 100 );
    tree_->setColumnWidth( 3, (8 * treeWidth) / 100 );
    tree_->setColumnWidth( 4, (52 * treeWidth) / 100 );
    tree_->setColumnWidth( 5, (6 * treeWidth) / 100 );
    tree_->setColumnWidth( 6, (6 * treeWidth) / 100 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientProxyCommsWidget::updateTreeItem( int index )
{
    QTreeWidgetItem *item( tree_->topLevelItem( index ) );

    if (( item ) && ( items_.contains( item ) ))
    {
        controlBlock *block( items_[item] );

        // update state
        switch ( block->state_ )
        {
        case Connecting:
            item->setText( 1, tr( "Connecting" ) );
            break;
        case Active:
            item->setText( 1, tr( "Active" ) );
            break;
        case Complete:
            item->setText( 1, tr( "Complete" ) );
            break;
        case Error:
            item->setText( 1, tr( "Error" ) );
            break;
        default:
            item->setText( 1, tr( "Unknown" ) );
            break;
        }

        // update bytes sent/recv
        item->setText( 5, QString::number( block->send_ ) );
        item->setText( 6, QString::number( block->recv_ ) );
    }
}

