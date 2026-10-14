/**
 * @file proxysettings.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <filesystem.h>

#include "webviewernetworkproxyfactory.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTextStream>
#include <QTime>

#define RESPONSE_OK 200
#define RESPONSE_REDIRECT 301

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewerNetworkProxyFactory::webViewerNetworkProxyFactory( const QString& config, QObject *parent ) :
    _Mybase( parent ),
    _Myimpl(),
    config_( System ),
    redirects_( 0 )
{
    LOG_CLASS_INFO( "webViewerNetworkProxyFactory", "webViewerNetworkProxyFactory" );

    // local pac file cache
    QDir temp( QDir::tempPath() );
    pacFile_ = temp.absoluteFilePath( "proxy.pac" );

    // manual proxy by default
    QNetworkProxyFactory::setUseSystemConfiguration( true );

    // check for proxy settings
    if ( !QFile::exists( config ) )
        LOG_INFO( "using system config; file '%s' not found", qPrintable( config ) );
    else if ( !parseProxySettings( config ) )
        LOG_WARN( "using system config; error parsing '%s'", qPrintable( config ) );
    else
    {
        // system proxy
        if (( params_.contains( "system" ) ) && ( "1" == params_["system"] ))
            LOG_INFO( "using system config", NULL );

        // auto proxy
        else if (( params_.contains( "auto" ) ) && ( "1" == params_["auto"] ))
        {
            if ( QFile::exists( pacFile_ ) )
            {
                LOG_INFO( "using cached auto proxy settings", NULL );

                autoProxy_.setConfig( pacFile_ );
                config_ = Auto;
            }
            else if ( !configureAutoProxy() )
                LOG_WARN( "error setting up auto proxy!", NULL );
            else
            {
                LOG_INFO( "using auto proxy settings", NULL );
            }
        }

        // manual proxy
        else if (( params_.contains( "manual" ) ) && ( "1" == params_["manual"] ))
        {
            if ( !configureManualProxy() )
                LOG_WARN( "error setting up manual proxy!", NULL );
            else
            {
                QNetworkProxyFactory::setUseSystemConfiguration( false );

                LOG_INFO( "using manual proxy settings", NULL );
                config_ = Manual;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewerNetworkProxyFactory::~webViewerNetworkProxyFactory()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<QNetworkProxy> webViewerNetworkProxyFactory::queryProxy( const QNetworkProxyQuery& query )
{
    QList<QNetworkProxy> proxyList;

    configType c( config_ );

    // get list of network proxies depending upon configuration
    switch ( c )
    {
    case Auto:
        if ( QNetworkProxyQuery::UrlRequest != query.queryType() )
            proxyList = QNetworkProxyFactory::systemProxyForQuery( query );
        else
        {
            QUrl url( query.url() );
            QString p( autoProxy_.findProxyForUrl( url.toString(), url.host() ) );

            proxyList = parseRule( p );
        }

        break;

    case Manual:
        proxyList.append( manualProxy_ );
        break;

    default:
        proxyList = QNetworkProxyFactory::systemProxyForQuery( query );
        break;
    }

#ifdef QT_DEBUG
    foreach ( const QNetworkProxy& p, proxyList )
        writeProxySettings( p );
#endif

    return proxyList;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewerNetworkProxyFactory::onNetworkReply( QNetworkReply *reply )
{
    LOG_CLASS_INFO( "webViewerNetworkProxyFactory", "onNetworkReply" );

    // check reply
    if ( QNetworkReply::NoError != reply->error() )
        LOG_WARN( "error pulling file (%d): %s", reply->error(), qPrintable( reply->errorString() ) );
    else
    {
        int statusCode( reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toUInt() );

        // check status code
        switch ( statusCode )
        {
        case RESPONSE_OK:

            if ( !reply->isReadable() )
                LOG_WARN( "reply is not readable!", NULL );
            else
            {
                QByteArray buffer( reply->readAll() );

                if ( !buffer.length() )
                    LOG_WARN( "invalid response!", NULL );
                else
                {
                    QFile f( pacFile_ );

                    if ( !f.open( QIODevice::WriteOnly ) )
                        LOG_WARN( "error opening file!", NULL );
                    else
                    {
                        qint64 written( f.write( buffer ) );

                        f.close();

                        if ( written != buffer.length() )
                            LOG_WARN( "error writing file!", NULL );
                        else
                        {
                            LOG_INFO( "have pac file for auto proxy '%s'", qPrintable( pacFile_ ) );

                            // good result!
                            autoProxy_.setConfig( pacFile_ );
                            config_ = Auto;
                        }

                    }
                }
            }
            break;

        case RESPONSE_REDIRECT:

            if ( MAX_REDIRECTS <= ++redirects_ )
                LOG_WARN( "too many redirects; aborting!", NULL );
            else
            {
                QUrl redirectUrl( reply->attribute( QNetworkRequest::RedirectionTargetAttribute ).toUrl() );

                QNetworkAccessManager *net( new QNetworkAccessManager( this ) );

                connect( net, SIGNAL( finished(QNetworkReply*) ), SLOT( onNetworkReply(QNetworkReply*) ) );

                LOG_DEBUG( "redirection to url: '%s'", qPrintable( redirectUrl.toString() ) );
                net->get( QNetworkRequest( redirectUrl ) );
            }
            break;

        default:
            LOG_WARN( "bad status code: %s", statusCode );
            break;
        }
    }

    if ( reply->isFinished() )
        reply->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webViewerNetworkProxyFactory::parseProxySettings( const QString& config )
{
    LOG_CLASS_INFO( "webViewerNetworkProxyFactory", "parseProxySettings" );

    QFile f( config );

    if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        LOG_WARN( "error opening file '%s': %s", qPrintable( config ), qPrintable( f.errorString() ) );
        return false;
    }

    QTextStream is( &f );

    while ( !is.atEnd() )
    {
        QString line( is.readLine() );

        if (( 2 <= line.length() ) && ( ' ' != line[0] ) && ( '#' != line[0] ))
        {
            QStringList fields( line.split( "=" ) );

            if ( 2 == fields.size() )
                params_[fields[0].trimmed()] = fields[1].trimmed();
        }
    }

    f.close();

    if ( 0 == params_.size() )
    {
        LOG_WARN( "invalid proxy file '%s'", qPrintable( config ) );
        return false;
    }

    // log out settings
    for ( paramMap::iterator p( params_.begin() ); p != params_.end(); ++p )
        LOG_DEBUG( "proxy setting %s: '%s'", qPrintable( p.key() ), qPrintable( p.value() ) );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webViewerNetworkProxyFactory::configureAutoProxy()
{
    LOG_CLASS_INFO( "webViewerNetworkProxyFactory", "configureAutoProxy" );

    if ( !params_.contains( "url" ) )
    {
        LOG_WARN( "invalid proxy settings!", NULL );
        return false;
    }

    QNetworkAccessManager *net( new QNetworkAccessManager( this ) );
    QString pacFile( params_["url"] );

    connect( net, SIGNAL( finished(QNetworkReply*) ), SLOT( onNetworkReply(QNetworkReply*) ) );

    LOG_DEBUG( "pulling pac file '%s'...", qPrintable( pacFile ) );
    net->get( QNetworkRequest( QUrl( pacFile ) ) );

    // wait for a result
    QTime doneWaitingTime( QTime::currentTime().addSecs( 3 ) );

    // wait up to 3 seconds for message to come in
    while (( Auto != config_ ) && ( QTime::currentTime() < doneWaitingTime ))
    {
        // process pending events
        QApplication::sendPostedEvents();
        QApplication::flush();

        QApplication::processEvents();
    }

    return (Auto == config_);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webViewerNetworkProxyFactory::configureManualProxy()
{
    LOG_CLASS_INFO( "webViewerNetworkProxyFactory", "configureManualProxy" );

    if (( !params_.contains( "type" ) ) || ( !params_.contains( "hostName" ) ))
    {
        LOG_WARN( "invalid proxy settings!", NULL );
        return false;
    }

    QNetworkProxy proxy;

    // iterate settings
    for ( paramMap::const_iterator p( params_.begin() ); p != params_.end(); ++p )
    {
        if ( "type" == p.key() )
            proxy.setType( manualProxyType( p.value().toInt() ) );
        else if ( "hostName" == p.key() )
            proxy.setHostName( p.value() );
        else if ( "port" == p.key() )
            proxy.setPort( p.value().toInt() );
        else if ( "user" == p.key() )
            proxy.setUser( p.value() );
        else if ( "password" == p.key() )
            proxy.setPassword( p.value() );
    }

    manualProxy_ = proxy;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<QNetworkProxy> webViewerNetworkProxyFactory::parseRule( const QString& rule )
{
    LOG_CLASS_INFO( "webViewerNetworkProxyFactory", "parseRule" );

    QList<QNetworkProxy> result;

    LOG_DEBUG( "parsing pac rule: '%s'", qPrintable( rule ) );
    QStringList rules( rule.split( ";" ) );

    foreach ( const QString& r, rules )
    {
        QString rt( r.trimmed() );

        if ( "DIRECT" == rt.left( 6 ) )
            result.append( QNetworkProxy( QNetworkProxy::NoProxy ) );
        else if (( "PROXY " == rt.left( 6 ) ) && ( 10 <= rt.length() ))
        {
            QNetworkProxy proxy;
            proxy.setType( QNetworkProxy::HttpProxy );

            QString p( rt.mid( 6 ).trimmed() );

            // username:password@host:port
            if ( p.contains( "@" ) )
            {
                QStringList temp( p.split( "@" ) );

                QStringList login( temp[0].split( ":" ) );
                QStringList host( temp[1].split( ":" ) );

                if ( 1 == login.size() )
                    proxy.setUser( login[0] );
                else if ( 2 == login.size() )
                {
                    proxy.setUser( login[0] );
                    proxy.setPassword( login[1] );
                }

                if ( 1 == host.size() )
                    proxy.setHostName( host[0] );
                else if ( 2 == host.size() )
                {
                    proxy.setHostName( host[0] );
                    proxy.setPort( host[1].toInt() );
                }
            }

            // host:port
            else
            {
                QStringList host( p.split( ":" ) );

                if ( 1 == host.size() )
                    proxy.setHostName( host[0] );
                else if ( 2 == host.size() )
                {
                    proxy.setHostName( host[0] );
                    proxy.setPort( host[1].toInt() );
                }
            }

            result.append( proxy );
        }
        else
        {
            LOG_WARN( "unsupported PAC rule '%s'", qPrintable( rt ) );
        }
    }

    if ( result.isEmpty() )
        result.append( QNetworkProxy( QNetworkProxy::NoProxy ) );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QNetworkProxy::ProxyType webViewerNetworkProxyFactory::manualProxyType( int t )
{
    switch ( t )
    {
    case 0: return QNetworkProxy::NoProxy;
    case 1: return QNetworkProxy::Socks5Proxy;
    case 2: return QNetworkProxy::HttpProxy;
    case 3: return QNetworkProxy::HttpCachingProxy;
    case 4: return QNetworkProxy::FtpCachingProxy;
    default:
        break;
    }

    return QNetworkProxy::DefaultProxy;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewerNetworkProxyFactory::writeProxySettings( const QNetworkProxy& proxy )
{
    LOG_CLASS_INFO( "proxySettings", "writeProxySettings" );

    LOG_DEBUG( "NEW PROXY", NULL );
    LOG_DEBUG( "type: %d", proxy.type() );
    LOG_DEBUG( "host name: %s", qPrintable( proxy.hostName() ) );
    LOG_DEBUG( "port: %d", proxy.port() );
    LOG_DEBUG( "caps: %d", (int)( proxy.capabilities() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
