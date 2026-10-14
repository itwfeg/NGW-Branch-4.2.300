/**
 * @file healthcheckcommswidget.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "clientcommdialog.h"
#include "healthcheckcommswidget.h"

#include <QtDebug>

#include <QCryptographicHash>
#include <QByteArray>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QHostAddress>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QTcpSocket>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVariant>
#include <QVBoxLayout>

// uncomment to use old meta events method for file downloads
//#define USE_META_DOWNLOADS

///////////////////////////////////////////////////////////////////////////////////////////////////
healthCheckCommsWidget::healthCheckCommsWidget( QWidget *parent ) :
    _Mybase( parent ),
    state_( webServerState::Okay )
{
    // init
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
healthCheckCommsWidget::~healthCheckCommsWidget()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::setEvent( const QString& event, bool enabled, const QString& url, bool forced, const QString& urlCallback, const QString& file, int parts )
{
    events_[event] = enabled;
    eventUrl_[event] = url;
    eventForced_[event] = forced;
    eventUrlCallback_[event] = urlCallback;
    eventFile_[event] = file;
    eventParts_[event] = parts;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::processRequest( QTcpSocket *client, const QStringList &header, const QByteArray &body )
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
    // parse data
    //

    QJsonObject request;
    QString requestUuid( "Unknown" );

    QJsonParseError e;

    if ( "GET" != requestType )
    {
        const QJsonDocument requestDoc( QJsonDocument::fromJson( body, &e ) );

        if ( QJsonParseError::NoError != e.error )
        {
            qWarning() << "error parsing document" << e.error << e.offset << e.errorString();
            return;
        }

        request = requestDoc.object();

        if ( request.contains( "requestUuid" ) )
            requestUuid = request["requestUuid"].toString();
    }

    //
    // keep track of data
    //

    requestComms_[tree_->topLevelItemCount()] = clientCommData( header, body );

    //
    // create tree widget item
    //

    // row data
    QStringList rows;
    rows.append( QString::number( tree_->topLevelItemCount() ) );
    rows.append( now.toString( "MM/dd/yyyy hh:mm:ss.zzz A" ) );
    rows.append( client->peerAddress().toString() );
    rows.append( requestType );
    rows.append( requestAddress );
    rows.append( requestUuid );
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
    else if ( webServerState::ThrowException == state_ )
    {
        static const QString fname( "exception.json" );

        if ( QFile::exists( fname ) )
        {
            QFile f( fname );

            if ( f.open( QFile::ReadOnly ) )
            {
                sendResponseJSON( client, "400 Bad Request", f.readAll() );
                f.close();
            }
        }
    }

    // good responses
    else
    {
        static const QJsonValue nullVal( QJsonValue::Null );

        // if they want 404 lets purposefully switch to something that is not handled
        if ( webServerState::NotFound == state_ )
            requestAddress = "XXX";

        // --------------------------
        //  Metadata file processing
        // --------------------------
        if ( requestAddress.contains( "?fileName=" ) )
        {
            const int i( requestAddress.indexOf( "fileName=" ) );

            // send file to caller
            if ( 0 < i )
            {
                const QString filename( requestAddress.mid( i+9 ) );

#if defined( USE_META_DOWNLOADS )
                // upgrade firmware
                if ( requestAddress.startsWith( "/UpgradeFirmware" ) )
                {
                    sendResponseFILE( client, "200 OK", filename );
                    return;
                }

                // install feature
                else if ( requestAddress.startsWith( "/InstallFeature" ) )
                {
                    sendResponseFILE( client, "200 OK", filename );
                    return;
                }

                // load labels
                else if ( requestAddress.startsWith( "/LoadLabels" ) )
                {
                    sendResponseFILE( client, "200 OK", eventFile_["LOAD_LABELS"] + QDir::separator() + filename );
                    return;
                }
#else
                sendResponseFILE( client, "200 OK", filename );
                return;
#endif
            }
        }

        // ------------------
        //  State processing
        // ------------------
        else if ( "POST" == requestType )
        {
            // initialize event
            // create new configuration
            // save to disk for future use
            if ( requestAddress.startsWith( "/UpdateConfiguration?initializeScaleFlg=true" ) )
            {
                const QUuid uuid( QUuid::createUuid() );

                // make sure current settings is null
                if ( !request.contains( "currentConfiguration" ) )
                    return;
                else
                {
                    const QJsonValue current( request["currentConfiguration"] );

                    if ( current != nullVal )
                        return;
                }

                const QJsonObject response( newConfig( uuid, request ) );

                // save to file so user can modify
                QFile f( uuid.toString( QUuid::WithoutBraces ) );

                if ( f.open( QFile::WriteOnly ) )
                {
                    const QJsonObject newConfig( response["newConfiguration"].toObject() );
                    const QJsonDocument newConfigDoc( newConfig );

                    f.write( newConfigDoc.toJson( QJsonDocument::Indented ) );
                    f.close();
                }

                // send response
                const QJsonDocument responseDoc( response );

                QByteArray a( responseDoc.toJson() );

                if ( webServerState::MalformedBody == state_ )
                    a = a.left( a.length() / 2 );

                sendResponseJSON( client, "200 OK", a );
                return;
            }

            // callback event
            // send back what we recieved
            else if ( requestAddress.startsWith( "/Callback" ) )
            {
                const QDateTime now( QDateTime::currentDateTime() );

                QJsonObject response( request );
                response["responseUuid"] = response["requestUuid"];
                response["responseTimestamp"] = now.toString( Qt::ISODate );

                response.remove( "requestUuid" );
                response.remove( "requestTimestamp" );
                response.remove( "callback" );

                response["acknowledgement"] = true;

                // send response
                const QJsonDocument responseDoc( response );

                QByteArray a( responseDoc.toJson() );

                if ( webServerState::MalformedBody == state_ )
                    a = a.left( a.length() / 2 );

                sendResponseJSON( client, "200 OK", a );
                return;
            }

            // update configuration
            // read configuration file from disk and send that
            else if ( requestAddress.startsWith( "/UpdateConfiguration?initializeScaleFlg=false" ) )
            {
                const QDateTime now( QDateTime::currentDateTime() );

                QJsonObject response( request );
                response["responseUuid"] = response["requestUuid"];
                response["responseTimestamp"] = now.toString( Qt::ISODate );

                response.remove( "requestUuid" );
                response.remove( "requestTimestamp" );

                // get config uuid
                const QJsonObject currentConfig( response["currentConfiguration"].toObject() );
                const QJsonObject config( currentConfig["config"].toObject() );

                const QString uuid( config["uuid"].toString() );

                response.remove( "configTimestamp" );
                response.remove( "currentConfiguration" );

                // read from file
                QFile f( uuid );

                if ( !f.open( QFile::ReadOnly ) )
                    qWarning() << "config does not exist locally" << uuid;
                else
                {
                    const QJsonDocument newConfigDoc( QJsonDocument::fromJson( f.readAll(), &e ) );

                    if ( QJsonParseError::NoError != e.error )
                        qWarning() << "error parsing document" << e.error << e.offset << e.errorString();
                    else
                    {
                        response["newConfiguration"] = newConfigDoc.object();

                        // send response
                        const QJsonDocument responseDoc( response );

                        QByteArray a( responseDoc.toJson() );

                        if ( webServerState::MalformedBody == state_ )
                            a = a.left( a.length() / 2 );

                        sendResponseJSON( client, "200 OK", a );
                        return;
                    }

                    f.close();
                }
            }

            // apply settings
            // send back what we recieve (for now)
            else if ( requestAddress.startsWith( "/ApplySettings" ) )
            {
                const QDateTime now( QDateTime::currentDateTime() );

                QJsonObject response( request );
                response["responseUuid"] = response["requestUuid"];
                response["responseTimestamp"] = now.toString( Qt::ISODate );

                response.remove( "requestUuid" );
                response.remove( "requestTimestamp" );

                // get current settings
                const QJsonObject currentSettings( response["currentSettings"].toObject() );

                response.remove( "currentSettings" );

                // new settings
                response["rebootScale"] = false;
                response["newSettings"] = currentSettings;

                if ( requestAddress.contains( "rebootFlg=true" ) )
                    response["rebootScale"] = true;

                // send response
                const QJsonDocument responseDoc( response );

                QByteArray a( responseDoc.toJson() );

                if ( webServerState::MalformedBody == state_ )
                    a = a.left( a.length() / 2 );

                sendResponseJSON( client, "200 OK", a );
                return;
            }

            // fetch plu
            // read product data from disk and use that as response
            else if ( requestAddress.startsWith( "/FetchPlu" ) )
            {
                const QDateTime now( QDateTime::currentDateTime() );

                QJsonObject response( request );
                response["responseUuid"] = response["requestUuid"];
                response["responseTimestamp"] = now.toString( Qt::ISODate );
                response["newFetchPluOffsetTimestamp"] = now.toUTC().toString( Qt::ISODateWithMs );

                response.remove( "requestUuid" );
                response.remove( "requestTimestamp" );
                response.remove( "plu" );

                // read from file
                QFile f( "fetchplu.json" );

                if ( !f.open( QFile::ReadOnly ) )
                    qWarning() << "fetchplu does not exist locally";
                else
                {
                    const QJsonDocument fetchPluDoc( QJsonDocument::fromJson( f.readAll(), &e ) );

                    if ( QJsonParseError::NoError != e.error )
                        qWarning() << "error parsing document" << e.error << e.offset << e.errorString();
                    else
                    {
                        const QJsonObject fetchPlu( fetchPluDoc.object() );

                        QStringList objects;
                        objects.append( "removeProductsOnNotInList" );
                        objects.append( "deletePluList" );
                        objects.append( "products" );
                        objects.append( "cool" );

                        foreach ( const QString& object, objects )
                            if ( fetchPlu.contains( object ) )
                                response[object] = fetchPlu[object];
                    }

                    f.close();
                }

                // send response
                const QJsonDocument responseDoc( response );

                QByteArray a( responseDoc.toJson() );

                if ( webServerState::MalformedBody == state_ )
                    a = a.left( a.length() / 2 );

                sendResponseJSON( client, "200 OK", a );
                return;
            }

            // health check event
            // trigger events
            else if ( requestAddress.startsWith( "/EventController" ) )
            {
                const QDateTime now( QDateTime::currentDateTime() );

                const QJsonObject healthCheck( request["healthCheck"].toObject() );

                // make sure this scale has registered with us
                const QString uuid( healthCheck["configUuid"].toString() );

                if (( uuid.isEmpty() ) || ( !QFile::exists( uuid ) ))
                {
                    qWarning() << "bad or missing config uuid" << uuid;
                    return;
                }

                QJsonObject response;
                response["responseUuid"] = request["requestUuid"];
                response["responseTimestamp"] = now.toString( Qt::ISODate );
                response["scale"] =  request["scale"];

                // events
                QStringList supportedEvents;
                supportedEvents.append( "UPDATE_CONFIGURATION" );
                supportedEvents.append( "APPLY_SETTINGS" );
                supportedEvents.append( "FETCH_PLU" );
                supportedEvents.append( "UPGRADE_FIRMWARE" );
                supportedEvents.append( "INSTALL_FEATURE" );
                supportedEvents.append( "LOAD_LABELS" );
                supportedEvents.append( "CAPTURE_METRICS" );

                QJsonArray events;

                foreach ( const QString& e, supportedEvents )
                    if (( events_.contains( e ) ) && ( events_[e] ))
                    {
                        const QUuid uuidEvent( QUuid::createUuid() );

#if defined( USE_META_DOWNLOADS )
                        QJsonObject metadata;

                        if (( "UPGRADE_FIRMWARE" == e ) || ( "INSTALL_FEATURE" == e ))
                        {
                            // break firmware into multiple parts
                            createMetadataForFirmware( metadata, eventFile_[e], eventParts_[e] );
                        }
                        else if ( "LOAD_LABELS" == e )
                        {
                            // give all tag files to caller
                            createMetadataForLabels( metadata, eventFile_[e] );
                        }
#endif
                        QJsonObject event;
                        event["event"] = e;
                        event["uuid"] = uuidEvent.toString( QUuid::WithoutBraces );
                        event["url"] = eventUrl_[e];
                        event["alwaysEnabled"] = eventForced_[e];
                        event["callbackUrl"] = eventUrlCallback_[e].isEmpty() ? nullVal : eventUrlCallback_[e];
#if defined( USE_META_DOWNLOADS )
                        event["metadata"] = metadata;
#else
                        event["metadata"] = nullVal;
#endif

                        events.append( event );
                    }

                response["events"] = events.isEmpty() ? nullVal : events;

                // ---- //

                const QJsonDocument responseDoc( response );

                QByteArray a( responseDoc.toJson() );

                if ( webServerState::MalformedBody == state_ )
                    a = a.left( a.length() / 2 );

                sendResponseJSON( client, "200 OK", a );
                return;
            }
#if !defined( USE_META_DOWNLOADS )
            // upgrade firmware
            else if ( requestAddress.startsWith( "/UpgradeFirmware" ) )
            {
                const QDateTime now( QDateTime::currentDateTime() );

                QJsonObject response( request );
                response["responseUuid"] = response["requestUuid"];
                response["responseTimestamp"] = now.toString( Qt::ISODate );

                response.remove( "requestUuid" );
                response.remove( "requestTimestamp" );

                // create a file download response
                createFileDownload( response, "UPGRADE_FIRMWARE" );

                // send response
                const QJsonDocument responseDoc( response );

                QByteArray a( responseDoc.toJson() );

                if ( webServerState::MalformedBody == state_ )
                    a = a.left( a.length() / 2 );

                sendResponseJSON( client, "200 OK", a );
                return;
            }

            // install feature
            else if ( requestAddress.startsWith( "/InstallFeature" ) )
            {
                const QDateTime now( QDateTime::currentDateTime() );

                QJsonObject response( request );
                response["responseUuid"] = response["requestUuid"];
                response["responseTimestamp"] = now.toString( Qt::ISODate );

                response.remove( "requestUuid" );
                response.remove( "requestTimestamp" );

                // create a file download response
                createFileDownload( response, "INSTALL_FEATURE" );

                // send response
                const QJsonDocument responseDoc( response );

                QByteArray a( responseDoc.toJson() );

                if ( webServerState::MalformedBody == state_ )
                    a = a.left( a.length() / 2 );

                sendResponseJSON( client, "200 OK", a );
                return;
            }

            // load labels
            else if ( requestAddress.startsWith( "/LoadLabels" ) )
            {
                const QDateTime now( QDateTime::currentDateTime() );

                QJsonObject response( request );
                response["responseUuid"] = response["requestUuid"];
                response["responseTimestamp"] = now.toString( Qt::ISODate );

                response.remove( "requestUuid" );
                response.remove( "requestTimestamp" );

                // create a file download response
                createFileDownload( response, "LOAD_LABELS" );

                // send response
                const QJsonDocument responseDoc( response );

                QByteArray a( responseDoc.toJson() );

                if ( webServerState::MalformedBody == state_ )
                    a = a.left( a.length() / 2 );

                sendResponseJSON( client, "200 OK", a );
                return;
            }
#endif
        }

        // unsupported file
        QByteArray body(
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "  <title>Health Check</title>\n"
            "</head>\n"
            "<body>\n"
            "  <font size=\"7\">Walmart Health Check Server</font><br>\n"
            "  <font size=\"5\">404 Page not found!</font>\n"
            "</body>\n"
            "</html>\n" );

        sendResponseHTML( client, "404 Not Found", body );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::resizeEvent( QResizeEvent *e )
{
    setTreeHeaders();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::sendResponse( QTcpSocket *client, const QString &responseCode )
{
    sendResponse( client, responseCode, QString(), QByteArray() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::sendResponseJSON( QTcpSocket *client, const QString &responseCode, const QByteArray &body )
{
    sendResponse( client, responseCode, "application/json", body );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::sendResponseHTML( QTcpSocket *client, const QString &responseCode, const QByteArray &body )
{
    sendResponse( client, responseCode, "text/html; charset=utf-8", body );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::sendResponseFILE( QTcpSocket *client, const QString &responseCode, const QString &filename )
{
    if (( filename.length() ) && ( QFile::exists( filename  ) ))
    {
        QFile f( filename );

        if ( f.open( QFile::ReadOnly ) )
        {
            sendResponse( client, responseCode, "application/octet-stream", f.readAll(), filename );
            f.close();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::onPushButtonClicked()
{
    if ( clear_ == sender() )
    {
        requestComms_.clear();
        responseComms_.clear();
        tree_->clear();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::onTreeItemDoubleClicked( QTreeWidgetItem *item, int /*column*/ )
{
    const QString valueStr( item->data( 0, Qt::DisplayRole ).toString() );
    const int value( valueStr.toInt() );

    const QString requestType( item->data( 3, Qt::DisplayRole ).toString() );

    if ( "GET" == requestType )
        return;

    if ( requestComms_.contains( value ) )
    {
        const clientCommData requestData( requestComms_[value] );

        if ( responseComms_.contains( value ) )
        {
            const clientCommData responseData( responseComms_[value] );

            clientCommDialog dialog( requestData.first, requestData.second, responseData.first, responseData.second, this );
            dialog.exec();
        }
        else
        {
            clientCommDialog dialog( requestData.first, requestData.second, this );
            dialog.exec();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::initialize()
{
    tree_ = new QTreeWidget( this );
    tree_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    tree_->setRootIsDecorated( false );
    tree_->setSelectionMode( QAbstractItemView::SingleSelection );
    tree_->setUniformRowHeights( true );
    connect( tree_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(onTreeItemDoubleClicked(QTreeWidgetItem*,int)) );

    setup_ = new QLabel( tr( "Scale Init: /UpdateConfiguration?initializeScaleFlg=true" ), this );

    clear_ = new QPushButton( tr( "Clear List" ), this );
    connect( clear_, SIGNAL(clicked()), SLOT(onPushButtonClicked()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::createLayout()
{
    QHBoxLayout *buttons( new QHBoxLayout );
    buttons->addWidget( setup_, 1 );
    buttons->addWidget( clear_ );

    QVBoxLayout *form( new QVBoxLayout( this ) );
    form->addWidget( tree_ );
    form->addLayout( buttons );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::setTreeHeaders()
{
    const int treeWidth( tree_->width() - 32 );

    // tree headers
    QStringList treeHeaders;
    treeHeaders.push_back( tr( "" ) );
    treeHeaders.push_back( tr( "Time" ) );
    treeHeaders.push_back( tr( "Client" ) );
    treeHeaders.push_back( tr( "Type" ) );
    treeHeaders.push_back( tr( "Address" ) );
    treeHeaders.push_back( tr( "UUID" ) );
    treeHeaders.push_back( tr( "Content Type" ) );
    treeHeaders.push_back( tr( "Content Length" ) );
    treeHeaders.push_back( tr( "Response" ) );

    // table
    tree_->setColumnCount( treeHeaders.size() );
    tree_->setHeaderLabels( treeHeaders );

    tree_->setColumnHidden( 0, true );
    tree_->setColumnWidth( 1, (14 * treeWidth) / 100 );     // time
    tree_->setColumnWidth( 2, (8 * treeWidth) / 100 );      // client
    tree_->setColumnWidth( 3, (4 * treeWidth) / 100 );      // type
    tree_->setColumnWidth( 4, (24 * treeWidth) / 100 );     // address
    tree_->setColumnWidth( 5, (20 * treeWidth) / 100 );     // uuid
    tree_->setColumnWidth( 6, (10 * treeWidth) / 100 );     // content type
    tree_->setColumnWidth( 7, (8 * treeWidth) / 100 );      // content length
    tree_->setColumnWidth( 8, (12 * treeWidth) / 100 );     // response
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::sendResponse( QTcpSocket *client, const QString &responseCode, const QString &contentType, const QByteArray &body, const QString &attachmentName )
{
    const QDateTime now( QDateTime::currentDateTimeUtc() );

    QStringList headers;
    headers.append( QString( "HTTP/1.0 %0" ).arg( responseCode ) );
    headers.append( QString( "Date: %0" ).arg( now.toString( "ddd, dd MMM yyyy hh:mm:ss GMT" ) ) );

    if ( body.length() )
    {
        headers.append( QString( "Content-Length: %0" ).arg( body.length() ) );
        headers.append( QString( "Content-Type: %0" ).arg( contentType ) );

        if ( attachmentName.length() )
        {
            headers.append( QString( "Content-Disposition: attachment; filename=%0" ).arg( attachmentName ) );
            headers.append( "Content-Transfer-Encoding: binary" );
            headers.append( "Accept-Ranges: bytes" );
        }
    }

    headers.append( QString( "Connection: %0" ).arg( "close" ) );

    if ( attachmentName.length() )
        responseComms_[tree_->topLevelItemCount() - 1] = clientCommData( headers, "<binary blob>" );
    else
        responseComms_[tree_->topLevelItemCount() - 1] = clientCommData( headers, body );

    // ---- //

    QByteArray response;

    foreach ( const QString& header, headers )
        response.append( header + "\r\n" );

    response.append( "\r\n" );
    response.append( body );

    client->write( response );
    client->close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject healthCheckCommsWidget::newConfig( const QUuid& uuid, const QJsonObject& request ) const
{
    const QDateTime now( QDateTime::currentDateTime() );

    const QJsonObject scale( request["scale"].toObject() );

    QString response =
        "{"
            "\"responseUuid\": \"" + request["requestUuid"].toString() + "\","
            "\"responseTimestamp\": \"" + now.toString( Qt::ISODate ) + "\","
            "\"scale\": {"
                "\"serialNbr\": \"" + scale["serialNbr"].toString() + "\","
                "\"scaleVendor\": \"" + scale["scaleVendor"].toString() + "\","
                "\"scaleModel\": \"" + scale["scaleModel"].toString() + "\","
                "\"scaleModelText\": \"" + scale["scaleModelText"].toString() + "\","
                "\"storeNbr\": " + QString::number( scale["storeNbr"].toInt() ) + ","
                "\"countryCode\": \"" + scale["countryCode"].toString() + "\""
            "},"
            "\"newConfiguration\": ${NEWCONFIG}"
        "}";

    // load default config from filesystem
    QFile f( "walmart.config" );

    if ( !f.open( QFile::ReadOnly ) )
        qWarning() << "default config does not exist locally";
    else
    {
        QString newConfig( f.readAll() );
        newConfig.replace( "${UUID}", uuid.toString( QUuid::WithoutBraces ) );
        newConfig.replace( "${MODEL}", scale["scaleModelText"].toString() );
        newConfig.replace( "${SERIALNUM}", scale["serialNbr"].toString() );

        response.replace( "${NEWCONFIG}", newConfig );

        f.close();
    }

    QJsonParseError e;

    const QJsonDocument doc( QJsonDocument::fromJson( response.toLatin1(), &e ) );

    if ( QJsonParseError::NoError != e.error )
    {
        qWarning() << "error parsing document" << e.error << e.offset << e.errorString();
        qWarning() << response.mid( e.offset, 16 );
    }

    return doc.object();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::createMetadataForFirmware( QJsonObject& metadata, const QString& filename, int parts )
{
    if ( !QFile::exists( filename ) )
    {
        qWarning() << "firmware file does not exist" << filename;
        return;
    }

    QJsonArray files;

    // ---- //

    QFile f( filename );

    if ( f.open( QFile::ReadOnly ) )
    {
        static const qint64 bufferLength( 4 * 1024 * 1024 );

        const qint64 totalSize = f.size();
        const qint64 partSizeMin = totalSize / qMax( parts, 1 );

        QCryptographicHash totalChecksum( QCryptographicHash::Sha256 );

        QCryptographicHash partChecksum( QCryptographicHash::Sha256 );
        QByteArray partBuffer;
        int partSeqNo = 1;


        for ( qint64 progress( 0 ); progress < totalSize; progress += bufferLength )
        {
            const QByteArray buffer = f.read( bufferLength );

            // update checksum
            totalChecksum.addData( buffer );

            if ( 1 < parts )
            {
                partChecksum.addData( buffer );
                partBuffer.append( buffer );

                if ( partSizeMin <= partBuffer.size() )
                {
                    QFile part( filename + "_part" + QString::number( partSeqNo ) );

                    if ( part.open( QFile::WriteOnly ) )
                    {
                        part.write( partBuffer );
                        part.close();
                    }

                    const QByteArray partChecksumResult( partChecksum.result() );

                    QJsonObject obj;
                    obj["fileName"] = part.fileName();
                    obj["fileSize"] = partBuffer.size();
                    obj["fileCheckSum"] = QString( partChecksumResult.toHex() );
                    obj["fileSequenceNbr"] = partSeqNo;

                    files.append( obj );

                    partChecksum.reset();
                    partBuffer.clear();
                    ++partSeqNo;
                }
            }
        }

        const QByteArray totalChecksumResult( totalChecksum.result() );

        if (( 1 < parts ) && ( partBuffer.size() ))
        {
            QFile part( filename + "_part" + QString::number( partSeqNo ) );

            if ( part.open( QFile::WriteOnly ) )
            {
                part.write( partBuffer );
                part.close();
            }

            const QByteArray partChecksumResult( partChecksum.result() );

            QJsonObject obj;
            obj["fileName"] = part.fileName();
            obj["fileSize"] = partBuffer.size();
            obj["fileCheckSum"] = QString( partChecksumResult.toHex() );
            obj["fileSequenceNbr"] = partSeqNo;

            files.append( obj );
        }
        else if ( 1 == parts )
        {
            QJsonObject obj;
            obj["fileName"] = filename;
            obj["fileSize"] = totalSize;
            obj["fileCheckSum"] = QString( totalChecksumResult.toHex() );
            obj["fileSequenceNbr"] = 1;

            files.append( obj );
        }

        f.close();

        // ---- //

        metadata["firmwareName"] = filename;
        metadata["packageFileName"] = filename;
        metadata["firmwareSize"] = totalSize;
        metadata["checkSum"] = QString( totalChecksumResult.toHex() );
        metadata["files"] = files;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::createMetadataForLabels( QJsonObject& metadata, const QString& folder )
{
    const QDir d( folder );

    if ( !d.exists() )
    {
        qWarning() << "label folder file does not exist" << folder;
        return;
    }

    QJsonArray files;

    // ---- //

    QStringList filters;
    filters.append( "*.ht" );

    QDirIterator it( folder, filters, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks );

    while ( it.hasNext() )
    {
        QFile f( it.next() );

        if ( f.open( QFile::ReadOnly ) )
        {
            static const qint64 bufferLength( 4 * 1024 * 1024 );

            const qint64 totalSize = f.size();

            QCryptographicHash totalChecksum( QCryptographicHash::Sha256 );

            for ( qint64 progress( 0 ); progress < totalSize; progress += bufferLength )
            {
                const QByteArray buffer = f.read( bufferLength );

                // update checksum
                totalChecksum.addData( buffer );
            }

            f.close();

            // ---- //

            const QByteArray totalChecksumResult( totalChecksum.result() );

            const QFileInfo fi( f.fileName() );

            QJsonObject obj;
            obj["fileName"] = fi.fileName();
            obj["fileSize"] = totalSize;
            obj["fileCheckSum"] = QString( totalChecksumResult.toHex() );

            files.append( obj );
        }
    }

    metadata["labelTemplateSetName"] = folder;
    metadata["files"] = files;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void healthCheckCommsWidget::createFileDownload( QJsonObject& response, const QString& e ) const
{
    const QString filename( eventFile_[e] );

    QFile f( filename );

    if ( f.open( QFile::ReadOnly ) )
    {
        static const qint64 bufferLength( 4 * 1024 * 1024 );

        const qint64 sizeNbr( f.size() );
        QCryptographicHash checksum( QCryptographicHash::Sha256 );

        // update checksum
        for ( qint64 progress( 0 ); progress < sizeNbr; progress += bufferLength )
            checksum.addData( f.read( bufferLength ) );

        const QByteArray checksumResult( checksum.result() );

        // download url
        QString downloadUrl( eventUrl_[e] );

        const int loc( downloadUrl.lastIndexOf( "/" ) );

        if ( 6 < loc )
        {
            downloadUrl.truncate( loc+1 );
            downloadUrl.append( "download?fileName=" + filename );
        }

        // ---- //

        response["downloadUrl"] = downloadUrl;

        if ( "UPGRADE_FIRMWARE" == e )
        {
            response["firmwareSizeNbr"] = sizeNbr;
            response["firmwareChecksum"] = QString( checksumResult.toHex() );
            response["packageFileName"] = "core.tgz";
        }
        else if ( "INSTALL_FEATURE" == e )
        {
            response["featureSizeNbr"] = sizeNbr;
            response["featureChecksum"] = QString( checksumResult.toHex() );
            response["packageFileName"] = "feature.tgz";
        }
        else if ( "LOAD_LABELS" == e )
        {
            response["labelsSizeNbr"] = sizeNbr;
            response["labelsChecksum"] = QString( checksumResult.toHex() );
            response["packageFileName"] = "labels.ht";
        }

        // ---- //

        f.close();
    }
}
