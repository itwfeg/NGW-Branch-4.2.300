/**
 * @file proxysettings.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "webviewnetworkproxyfactory.h"

#include <QApplication>
#include <QCryptographicHash>
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
webViewNetworkProxyFactory::webViewNetworkProxyFactory( const QString& config, QObject *parent ) :
    _Mybase( parent ),
    _Myimpl(),
    config_( System ),
    redirects_( 0 )
{
    // generate hash of this application and its directory; this will be used to cache the pac file
    QCryptographicHash id( QCryptographicHash::Sha1 );
    id.addData( QApplication::applicationDirPath().toLatin1() );
    id.addData( QApplication::applicationName().toLatin1() );

    // local pac file cache
    QDir temp( QDir::tempPath() );
    pacFile_ = temp.absoluteFilePath( QString( "proxy%0.pac" ).arg( QString( id.result().toHex() ) ) );

    // manual proxy by default
    QNetworkProxyFactory::setUseSystemConfiguration( true );

    // check for proxy settings
    if ( !QFile::exists( config ) )
        qDebug( "using system config; file '%s' not found", qPrintable( config ) );
    else if ( !parseProxySettings( config ) )
        qWarning( "using system config; error parsing '%s'", qPrintable( config ) );
    else
    {
        // system proxy
        if (( params_.contains( "system" ) ) && ( "1" == params_["system"] ))
            qDebug( "using system config" );

        // auto proxy
        else if (( params_.contains( "auto" ) ) && ( "1" == params_["auto"] ))
        {
            if ( QFile::exists( pacFile_ ) )
            {
                qDebug( "using cached auto proxy settings" );

                autoProxy_.setConfig( pacFile_ );
                config_ = Auto;
            }
            else if ( !configureAutoProxy() )
                qWarning( "error setting up auto proxy!" );
            else
            {
                qDebug( "using auto proxy settings" );
            }
        }

        // manual proxy
        else if (( params_.contains( "manual" ) ) && ( "1" == params_["manual"] ))
        {
            if ( !configureManualProxy() )
                qWarning( "error setting up manual proxy!" );
            else
            {
                QNetworkProxyFactory::setUseSystemConfiguration( false );

                qDebug( "using manual proxy settings" );
                config_ = Manual;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewNetworkProxyFactory::~webViewNetworkProxyFactory()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<QNetworkProxy> webViewNetworkProxyFactory::queryProxy( const QNetworkProxyQuery& query )
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
void webViewNetworkProxyFactory::onNetworkReply( QNetworkReply *reply )
{
    // check reply
    if ( QNetworkReply::NoError != reply->error() )
        qWarning( "error pulling file (%d): %s", reply->error(), qPrintable( reply->errorString() ) );
    else
    {
        int statusCode( reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toUInt() );

        // check status code
        switch ( statusCode )
        {
        case RESPONSE_OK:

            if ( !reply->isReadable() )
                qWarning( "reply is not readable!" );
            else
            {
                QByteArray buffer( reply->readAll() );

                if ( !buffer.length() )
                    qWarning( "invalid response!" );
                else
                {
                    QFile f( pacFile_ );

                    if ( !f.open( QIODevice::WriteOnly ) )
                        qWarning( "error opening file!" );
                    else
                    {
                        qint64 written( f.write( buffer ) );

                        f.close();

                        if ( written != buffer.length() )
                            qWarning( "error writing file!" );
                        else
                        {
                            qDebug( "have pac file for auto proxy '%s'", qPrintable( pacFile_ ) );

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
                qWarning( "too many redirects; aborting!" );
            else
            {
                QUrl redirectUrl( reply->attribute( QNetworkRequest::RedirectionTargetAttribute ).toUrl() );

                QNetworkAccessManager *net( new QNetworkAccessManager( this ) );

                connect( net, SIGNAL( finished(QNetworkReply*) ), SLOT( onNetworkReply(QNetworkReply*) ) );

                qDebug( "redirection to url: '%s'", qPrintable( redirectUrl.toString() ) );
                net->get( QNetworkRequest( redirectUrl ) );
            }
            break;

        default:
            qWarning( "bad status code: %d", statusCode );
            break;
        }
    }

    if ( reply->isFinished() )
        reply->deleteLater();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webViewNetworkProxyFactory::parseProxySettings( const QString& config )
{
    QFile f( config );

    if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        qWarning( "error opening file '%s': %s", qPrintable( config ), qPrintable( f.errorString() ) );
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
        qWarning( "invalid proxy file '%s'", qPrintable( config ) );
        return false;
    }

    // log out settings
    for ( paramMap::iterator p( params_.begin() ); p != params_.end(); ++p )
        qDebug( "proxy setting %s: '%s'", qPrintable( p.key() ), qPrintable( p.value() ) );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webViewNetworkProxyFactory::configureAutoProxy()
{
    if ( !params_.contains( "url" ) )
    {
        qWarning( "invalid proxy settings!" );
        return false;
    }

    QNetworkAccessManager *net( new QNetworkAccessManager( this ) );
    QString pacFile( params_["url"] );

    connect( net, SIGNAL( finished(QNetworkReply*) ), SLOT( onNetworkReply(QNetworkReply*) ) );

    qDebug( "pulling pac file '%s'...", qPrintable( pacFile ) );
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
bool webViewNetworkProxyFactory::configureManualProxy()
{
    if (( !params_.contains( "type" ) ) || ( !params_.contains( "hostName" ) ))
    {
        qWarning( "invalid proxy settings!" );
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
QList<QNetworkProxy> webViewNetworkProxyFactory::parseRule( const QString& rule )
{
    QList<QNetworkProxy> result;

    qDebug( "parsing pac rule: '%s'", qPrintable( rule ) );
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
            qWarning( "unsupported PAC rule '%s'", qPrintable( rt ) );
        }
    }

    if ( result.isEmpty() )
        result.append( QNetworkProxy( QNetworkProxy::NoProxy ) );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QNetworkProxy::ProxyType webViewNetworkProxyFactory::manualProxyType( int t )
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
void webViewNetworkProxyFactory::writeProxySettings( const QNetworkProxy& proxy )
{
    qDebug( "NEW PROXY" );
    qDebug( "type: %d", proxy.type() );
    qDebug( "host name: %s", qPrintable( proxy.hostName() ) );
    qDebug( "port: %d", proxy.port() );
    qDebug( "caps: %d", (int)( proxy.capabilities() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
