/**
 * @file clientcommswidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "clientcommdialog.h"
#include "clientcommswidget.h"

#include <QtDebug>

#include <QByteArray>
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
clientCommsWidget::clientCommsWidget( QWidget *parent ) :
    _Mybase( parent ),
    allowLegacy_( false ),
    state_( webServerState::Okay )
{
    // init
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
clientCommsWidget::~clientCommsWidget()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::processRequest( QTcpSocket *client, const QStringList &header, const QByteArray &body )
{
    const QDateTime now( QDateTime::currentDateTime() );

    //
    // parse header
    //

    QString requestType;
    QString requestAddress;
    QString contentType;
    QString contentLength;

    // request address
    if ( header.length() )
    {
        const QString s( header.first() );

        QStringList req( s.split( ' ' ) );

        // type
        // path
        // version
        if ( req.length() < 3 )
        {
            qDebug() << "invalid request" << s;
            return;
        }

        requestType = req[0];
        requestAddress = req[1];
    }

    foreach ( const QString &s, header )
    {
        if ( s.contains( "Content-Type: " ) )
            contentType = s.mid( 14 );

        if ( s.contains( "Content-Length: " ) )
            contentLength = s.mid( 16 );
    }

    //
    // keep track of data
    //

    comms_[tree_->topLevelItemCount()] = clientCommData( header, body );

    //
    // create tree widget item
    //

    // row data
    QStringList rows;
    rows.append( QString::number( tree_->topLevelItemCount() ) );
    rows.append( now.toString( "MM/dd/yyyy hh:mm:ss A" ) );
    rows.append( client->peerAddress().toString() );
    rows.append( requestType );
    rows.append( requestAddress );
    rows.append( contentType );
    rows.append( contentLength );
    rows.append( webServerState::description( state_ ) );

    // create widget item
    QTreeWidgetItem *item( new QTreeWidgetItem( rows ) );

    tree_->addTopLevelItem( item );
    tree_->scrollToBottom();

    //
    // respond to client
    //

    // error responses
    if ( webServerState::OkayNoContent == state_ )
    {
        sendResponse( client, "204 No Content" );
    }
    else if ( webServerState::IncompleteHeader == state_ )
    {
        QByteArray response;
        response.append( "HTTP/1.0 200 OK\r\n" );
        response.append( "Date: " );

        client->write( response );
    }
    else if ( webServerState::MalformedHeader == state_ )
    {
        QByteArray response;
        response.append( "HTTP/1.0 200 OK\r\n" );
        response.append( "Date: " );

        client->write( response );
        client->close();
    }
    else if ( webServerState::NoResponse == state_ )
    {
        // do nothing
    }
    else if ( webServerState::CloseSocket == state_ )
    {
        client->close();
    }
    else if ( webServerState::Moved == state_ )
    {
        QByteArray response;
        response.append( "HTTP/1.0 301 Moved Permanently\r\n" );
        response.append( "Location: http://127.0.0.1\r\n" );
        response.append( "\r\n" );

        client->write( response );
        client->close();
    }
    else if ( webServerState::Unauthorized == state_ )
    {
        sendResponse( client, "401 Unauthorized" );
    }

    // good responses
    else
    {
        // if they want 404 lets purposefully switch to something that is not handled
        if ( webServerState::NotFound == state_ )
            requestAddress = "XXX";

        // page shown in operate mode
        if (( "GET" == requestType ) && ( requestAddress.contains( "/lookupitem.fpp" ) ))
        {
            QByteArray body;

            body += QString(
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<head>\n"
                "  <title>FPP</title>\n"
                "</head>\n"
                "<body>\n"
                "  <font size=\"7\">FPP Server</font><br>\n"
                "  <font size=\"5\">lookupitem</font><br>\n"
                "  <br>\n" );

            body += now.toString() + "<br>\n";
            body += requestAddress + "<br>\n";

            body += QString(
                "</body>\n"
                "</html>\n" );

            sendResponseHTML( client, "200 OK", body );
        }
        // post of sold item
        else if (( !legacyUpdateAllowed() ) && ( "POST" == requestType ) && ( requestAddress.contains( "/update.fpp" ) ))
        {
            sendResponse( client, "200 OK" );
        }
        // post of alert
        else if (( "POST" == requestType ) && ( requestAddress.contains( "/sendalert" ) ))
        {
            sendResponse( client, "200 OK" );
        }
        // test page
        else if (( "GET" == requestType ) && ( requestAddress.contains( "/test.fpp" ) ))
        {
            QByteArray body;

            body += QString(
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<head>\n"
                "  <title>FPP</title>\n"
                "  <link rel=\"stylesheet\" href=\"page.css\" type=\"text/css\" />\n"
                "</head>\n"
                "<body>\n"
                "  <font size=\"7\">FPP Server</font><br>\n"
                "  <font size=\"5\">test</font><br>\n"
                "  <br>\n" );

            body += now.toString() + "<br>\n";
            body += requestAddress + "<br>\n";

            body += QString(
                "</body>\n"
                "</html>\n" );

            sendResponseHTML( client, "200 OK", body );
        }
        // example html to request plu update from Walmart ISD
        else if (( "GET" == requestType ) && ( requestAddress.contains( "/smsformplu.cgi" ) ))
        {
            QByteArray body;

            body += QString(
                "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
                "  <head>\n"
                "    <title>Scale Service</title>\n"
                "    <meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />\n"
                "    <meta name=\"generator\" content=\"CoffeeCup HTML Editor (www.coffeecup.com)\" />\n"
                "    <meta name=\"created\" content=\"Sat, 21 Apr 2012 00:32:50 GMT\" />\n"
                "    <meta name=\"description\" content=\"Scale Management\" />\n"
                "    <meta name=\"keywords\" content=\"Scale Management\" />\n"
                "  </head>\n"
                "<body>\n"
                "<p style=\"TEXT-ALIGN: center; MARGIN-LEFT: auto; MARGIN-RIGHT: auto\"><br > </p>\n"
                "<div  align=\"center\">\n"
                "<h1>PLU Check</h1>\n"
                "<p>Scale Managment System PLU Check smsformplu.cgi </p>\n"
                "<FORM ACTION=\"scripts/smsformplu.cgi\" METHOD=\"POST\" ENCTYPE=\"No Method Used\" >\n"
                "<input type=\"PLU\" name=\"plu\" value=\"100\" SIZE=\"5\" MAXLENGTH=\"6\">\n"
                "<p> <INPUT TYPE=\"SUBMIT\" VALUE=\"SUBMIT\" NAME=\"SUBMIT\"> </p>\n"
                "</form>\n"
                "</div>\n"
                "</body>\n"
                "</html>\n" );

            sendResponseHTML( client, "200 OK", body );
        }
        // example html to request plu update from Walmart ISD
        else if (( "POST" == requestType ) && ( requestAddress.contains( "/scripts/smsformplu.cgi" ) ))
        {
            QByteArray body;

            body += QString(
                "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
                "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
                "  <head>\n"
                "    <title>Scale Management Service Control Center</title>\n"
                "    <meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />\n"
                "    <meta name=\"generator\" content=\"CoffeeCup HTML Editor (www.coffeecup.com)\" />\n"
                "    <meta name=\"created\" content=\"Sat, 21 Apr 2012 00:32:50 GMT\" />\n"
                "    <meta name=\"description\" content=\"Scale Management Service\" />\n"
                "    <meta name=\"keywords\" content=\"Scale Management Service\" />\n"
                "  </head>\n"
                "<body>\n"
                "<P>\n"
                "Your request is being processed.\n"
                "</P>\n"
                "<P>Checking for PLU=100, ITEM=9802865, UCODE=78742981000, DEPT=98\n"
                "</P>\n"
                "<P>PLUOK=1, 100 is Good\n"
                "</P>\n"
                "</body>\n"
                "</html>\n" );

            sendResponseHTML( client, "200 OK", body );
        }

        // ---- //

        // old style post of sold item
        else if (( legacyUpdateAllowed() ) && ( "GET" == requestType ) && ( requestAddress.contains( "/update.fpp" ) ))
        {
            sendResponse( client, "200 OK" );
        }

        // ---- ///

        // style sheet
        else if (( "GET" == requestType ) && ( requestAddress.contains( "/page.css" ) ))
        {
            QByteArray body;

            body += QString(
                "body { font-family: arial; background-color: #a9bdd8; }\n" );

            sendResponseCSS( client, "200 OK", body );
        }

        // proxy rules pac file
        else if (( "GET" == requestType ) && ( requestAddress.contains( "/proxyconfig.pac" ) ))
        {
            QByteArray body;

            body += QString( "function FindProxyForURL(url, host) {\r\n" );
            body += QString( "\r\n" );
            body += QString( "// If the requested website is hosted within the internal network, send direct.\r\n" );
            body += QString( "    if (isPlainHostName(host) ||\r\n" );
            body += QString( "        shExpMatch(host, \"*.local\") ||\r\n" );
            body += QString( "        isInNet(dnsResolve(host), \"10.0.0.0\", \"255.0.0.0\") ||\r\n" );
            body += QString( "        isInNet(dnsResolve(host), \"172.16.0.0\",  \"255.240.0.0\") ||\r\n" );
            body += QString( "        isInNet(dnsResolve(host), \"192.168.0.0\",  \"255.255.0.0\") ||\r\n" );
            body += QString( "        isInNet(dnsResolve(host), \"127.0.0.0\", \"255.255.255.0\"))\r\n" );
            body += QString( "        return \"DIRECT\";\r\n" );
            body += QString( "\r\n" );
            body += QString( "// DEFAULT RULE: All other traffic, use below proxies, in fail-over order.\r\n" );
            body += QString( "    return \"PROXY %0:8080\";\r\n" ).arg( client->localAddress().toString() );
            body += QString( "\r\n" );
            body += QString( "}\r\n" );

            sendResponsePAC( client, "200 OK", body );
        }

        // unsupported file
        else
        {
            QByteArray body(
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<head>\n"
                "  <title>FPP</title>\n"
                "</head>\n"
                "<body>\n"
                "  <font size=\"7\">FPP Server</font><br>\n"
                "  <font size=\"5\">404 Page not found!</font>\n"
                "</body>\n"
                "</html>\n" );

            sendResponseHTML( client, "404 Not Found", body );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::resizeEvent( QResizeEvent *e )
{
    setTreeHeaders();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::sendResponse( QTcpSocket *client, const QString &responseCode )
{
    sendResponse( client, responseCode, QString(), QByteArray() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::sendResponseCSS( QTcpSocket *client, const QString &responseCode, const QByteArray &body )
{
    sendResponse( client, responseCode, "text/css", body );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::sendResponseHTML( QTcpSocket *client, const QString &responseCode, const QByteArray &body )
{
    sendResponse( client, responseCode, "text/html; charset=utf-8", body );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::sendResponsePAC( QTcpSocket *client, const QString &responseCode, const QByteArray &body )
{
    sendResponse( client, responseCode, "application/x-ns-proxy-autoconfig", body );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::onPushButtonClicked()
{
    if ( clear_ == sender() )
    {
        comms_.clear();
        tree_->clear();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::onTreeItemDoubleClicked( QTreeWidgetItem *item, int /*column*/ )
{
    const QString valueStr( item->data( 0, Qt::DisplayRole ).toString() );
    const int value( valueStr.toInt() );

    if ( comms_.contains( value ) )
    {
        const clientCommData data( comms_[value] );

        clientCommDialog dialog( data.first, data.second, this );
        dialog.exec();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::initialize()
{
    tree_ = new QTreeWidget( this );
    tree_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    tree_->setRootIsDecorated( false );
    tree_->setSelectionMode( QAbstractItemView::SingleSelection );
    tree_->setUniformRowHeights( true );
    connect( tree_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(onTreeItemDoubleClicked(QTreeWidgetItem*,int)) );

    setup_ = new QLabel( tr( "Test Page: /test.fpp  |  Product Lookup: /lookupitem.fpp  |  Updates: /update.fpp  |  Alerts: /sendalert  |  PAC: /proxyconfig.pac" ), this );

    clear_ = new QPushButton( tr( "Clear List" ), this );
    connect( clear_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::createLayout()
{
    QHBoxLayout *buttons( new QHBoxLayout );
    buttons->addWidget( setup_, 1 );
    buttons->addWidget( clear_ );

    QVBoxLayout *form( new QVBoxLayout( this ) );
    form->addWidget( tree_ );
    form->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::setTreeHeaders()
{
    const int treeWidth( tree_->width() );

    // tree headers
    QStringList treeHeaders;
    treeHeaders.push_back( tr( "" ) );
    treeHeaders.push_back( tr( "Time" ) );
    treeHeaders.push_back( tr( "Client" ) );
    treeHeaders.push_back( tr( "Type" ) );
    treeHeaders.push_back( tr( "Address" ) );
    treeHeaders.push_back( tr( "Content Type" ) );
    treeHeaders.push_back( tr( "Content Length" ) );
    treeHeaders.push_back( tr( "Response" ) );

    // table
    tree_->setColumnCount( treeHeaders.size() );
    tree_->setHeaderLabels( treeHeaders );

    tree_->setColumnHidden( 0, true );
    tree_->setColumnWidth( 1, (10 * treeWidth) / 100 );
    tree_->setColumnWidth( 2, (8 * treeWidth) / 100 );
    tree_->setColumnWidth( 3, (4 * treeWidth) / 100 );
    tree_->setColumnWidth( 4, (46 * treeWidth) / 100 );
    tree_->setColumnWidth( 5, (10 * treeWidth) / 100 );
    tree_->setColumnWidth( 6, (8 * treeWidth) / 100 );
    tree_->setColumnWidth( 7, (12 * treeWidth) / 100 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void clientCommsWidget::sendResponse( QTcpSocket *client, const QString &responseCode, const QString &contentType, const QByteArray &body )
{
    const QDateTime now( QDateTime::currentDateTimeUtc() );

    QByteArray response;
    response.append( QString( "HTTP/1.0 %0\r\n" ).arg( responseCode ) );
    response.append( QString( "Date: %0\r\n" ).arg( now.toString( "ddd, dd MMM yyyy hh:mm:ss GMT" ) ) );

    if ( body.length() )
    {
        response.append( QString( "Content-Length: %0\r\n" ).arg( body.length() ) );
        response.append( QString( "Content-Type: %0\r\n" ).arg( contentType ) );
    }

    response.append( QString( "Connection: %0\r\n" ).arg( "close" ) );
    response.append( "\r\n" );
    response.append( body );

    client->write( response );
    client->close();
}
