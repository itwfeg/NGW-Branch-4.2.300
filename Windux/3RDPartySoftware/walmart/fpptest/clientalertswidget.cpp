/**
 * @file clientalertswidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "clientalertswidget.h"
#include "clientcommdialog.h"

#include <QtDebug>

#include <QDateTime>
#include <QHostAddress>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStringList>
#include <QTcpSocket>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVariant>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
clientAlertsWidget::clientAlertsWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
clientAlertsWidget::~clientAlertsWidget()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientAlertsWidget::processDisconnect( QTcpSocket *client )
{
    int ci( -1 );
    clientCommData c;

    // find client
    for ( int i( 0 ); i < comms_.size(); ++i )
        if ( client == comms_[i].first )
        {
            ci = i;
            c = comms_[i];
        }

    if ( ci < 0 )
    {
        qDebug() << "error locating client";
        return;
    }

    // check for error state
    if ( Complete != c.second.first )
    {
        c.second.first = Error;

        // update
        comms_[ci] = c;

        updateTreeItem( ci );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientAlertsWidget::processNewConnection( QTcpSocket *client )
{
    const QDateTime now( QDateTime::currentDateTime() );

    //
    // keep track of data
    //

    comms_[tree_->topLevelItemCount()] = clientCommData( client, clientComm( Connected, QStringList() ) );

    //
    // create tree widget item
    //

    // row data
    QStringList rows;
    rows.append( QString::number( tree_->topLevelItemCount() ) );
    rows.append( now.toString( "MM/dd/yyyy hh:mm:ss A" ) );
    rows.append( client->peerAddress().toString() );
    rows.append( tr( "Connected" ) );
    rows.append( QString() );

    // create widget item
    QTreeWidgetItem *item( new QTreeWidgetItem( rows ) );

    tree_->addTopLevelItem( item );
    tree_->scrollToBottom();

    //
    // respond to client
    //

    client->write( "220 smtp.hobartcorp.com SMTP\r\n" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientAlertsWidget::processRequest( QTcpSocket *client, const QString &msg )
{
    int ci( -1 );
    clientCommData c;

    // find client
    for ( int i( 0 ); i < comms_.size(); ++i )
        if ( client == comms_[i].first )
        {
            ci = i;
            c = comms_[i];
        }

    if ( ci < 0 )
    {
        qDebug() << "error locating client";
        return;
    }

    //
    // process message
    //

    // append message to buffer
    c.second.second.append( msg );

    // process
    if ( Data == c.second.first )
    {
        // check for data terminator
        if ( "." == msg )
        {
            client->write( "250 Ok\r\n" );
            c.second.first = Header;
        }
    }
    else
    {
        const QString cmd( msg.left( 4 ).toUpper() );

        if ( "HELO" == cmd )
        {
            client->write( "250 Hello, I am a test server and I am glad to meet you\r\n" );
            c.second.first = Hello;
        }
        else if ( "DATA" == cmd )
        {
            client->write( "354 End data with <CR><LF>.<CR><LF>\r\n" );
            c.second.first = Data;
        }
        else if ( "QUIT" == cmd )
        {
            client->write( "221 Bye\r\n" );
            client->close();

            c.second.first = Complete;
            c.first = NULL;
        }
        else
        {
            client->write( "250 Ok\r\n" );
        }
    }

    //
    // update tree
    //

    comms_[ci] = c;

    updateTreeItem( ci );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void clientAlertsWidget::resizeEvent( QResizeEvent *e )
{
    setTreeHeaders();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientAlertsWidget::onPushButtonClicked()
{
    if ( clear_ == sender() )
    {
        comms_.clear();
        tree_->clear();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientAlertsWidget::onTreeItemDoubleClicked( QTreeWidgetItem *item, int /*column*/ )
{
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
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientAlertsWidget::initialize()
{
    tree_ = new QTreeWidget( this );
    tree_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    tree_->setRootIsDecorated( false );
    tree_->setSelectionMode( QAbstractItemView::SingleSelection );
    tree_->setUniformRowHeights( true );
    connect( tree_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(onTreeItemDoubleClicked(QTreeWidgetItem*,int)) );

    clear_ = new QPushButton( tr( "Clear List" ), this );
    connect( clear_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientAlertsWidget::createLayout()
{
    QHBoxLayout *buttons( new QHBoxLayout );
    buttons->addStretch();
    buttons->addWidget( clear_ );

    QVBoxLayout *form( new QVBoxLayout( this ) );
    form->addWidget( tree_ );
    form->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientAlertsWidget::setTreeHeaders()
{
    const int treeWidth( tree_->width() );

    // tree headers
    QStringList treeHeaders;
    treeHeaders.push_back( tr( "" ) );
    treeHeaders.push_back( tr( "Time" ) );
    treeHeaders.push_back( tr( "Client" ) );
    treeHeaders.push_back( tr( "State" ) );
    treeHeaders.push_back( tr( "Subject" ) );


    // table
    tree_->setColumnCount( treeHeaders.size() );
    tree_->setHeaderLabels( treeHeaders );

    tree_->setColumnHidden( 0, true );
    tree_->setColumnWidth( 1, (10 * treeWidth) / 100 );
    tree_->setColumnWidth( 2, (8 * treeWidth) / 100 );
    tree_->setColumnWidth( 3, (8 * treeWidth) / 100 );
    tree_->setColumnWidth( 4, (72 * treeWidth) / 100 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientAlertsWidget::updateTreeItem( int index )
{
    if ( !comms_.contains( index ) )
        return;

    QTreeWidgetItem *item( tree_->topLevelItem( index ) );

    if ( !item )
        return;

    const clientCommData data( comms_[index] );

    // update state
    switch ( data.second.first )
    {
    case Connected:
        item->setText( 3, tr( "Connected" ) );
        break;
    case Hello:
        item->setText( 3, tr( "Hello" ) );
        break;
    case Header:
        item->setText( 3, tr( "Header" ) );
        break;
    case Data:
        item->setText( 3, tr( "Data" ) );
        break;
    case Complete:
        item->setText( 3, tr( "Complete" ) );
        break;
    case Error:
        item->setText( 3, tr( "Error" ) );
        break;
    default:
        item->setText( 3, tr( "Unknown" ) );
        break;
    }

    // update message
    QString message;

    foreach ( const QString &s, data.second.second )
        if ( "SUBJECT: " == s.left( 9 ).toUpper() )
        {
            message = s.mid( 9 );
            break;
        }

    item->setText( 4, message );
}
