/**
 * @file networkacess.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "networkaccess.h"

#include <QAuthenticator>
#include <QDebug>
#include <QNetworkReply>
#include <QSslCertificate>
#include <QSslConfiguration>

///////////////////////////////////////////////////////////////////////////////////////////////////
networkAccess::networkAccess( QObject *parent ) :
    _Mybase( parent ),
    ignoreAllSslErrors_( false )
{
    // connect slots
    connect( this, &_Myt::authenticationRequired, this, &_Myt::onAuthenticationRequired );
    connect( this, &_Myt::finished, this, &_Myt::onFinished );
    connect( this, &_Myt::networkAccessibleChanged, this, &_Myt::onNetworkAccessibleChanged );
    connect( this, &_Myt::preSharedKeyAuthenticationRequired, this, &_Myt::onPreSharedKeyAuthenticationRequired );
    connect( this, &_Myt::proxyAuthenticationRequired, this, &_Myt::onProxyAuthenticationRequired );
    connect( this, &_Myt::sslErrors, this, &_Myt::onSslErrors );

    // check access
    if ( QNetworkAccessManager::Accessible == networkAccessible() )
        LOG_DEBUG << "READY";
    else
    {
        LOG_WARN << "attempting to force online status...";
        setNetworkAccessible( QNetworkAccessManager::Accessible );

        if ( QNetworkAccessManager::Accessible == networkAccessible() )
            LOG_DEBUG << "READY";
        else
            LOG_WARN << "network not accessible: " << networkAccessible();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
networkAccess::~networkAccess()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkAccess::addCredentials( const QString &host, const QString &user, const QString &pwd )
{
    credentials_[host] = userPasswordPair( user, pwd );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QNetworkReply *networkAccess::createRequest( Operation op, const QNetworkRequest& request, QIODevice *outgoingData )
{
    QNetworkReply *reply( _Mybase::createRequest( op, request, outgoingData ) );

    // hook into reply
    if ( reply )
    {
        LOG_DEBUG << "reply created";
        connect( reply, &QNetworkReply::finished, this, &_Myt::onReplyFinished );
    }

    return reply;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkAccess::onAuthenticationRequired( QNetworkReply *reply, QAuthenticator *authenticator )
{
    const QUrl url( reply->url() );

    const QString host( url.host() );
    LOG_WARN << "authentication required " << qPrintable( host );

    // check if we have credentials for this host
    if (( credentials_.contains( url.host() ) ) && ( authenticator ))
    {
        LOG_DEBUG << "using saved credentials";

        authenticator->setUser( credentials_[url.host()].first );
        authenticator->setPassword( credentials_[url.host()].second );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkAccess::onFinished( QNetworkReply *reply )
{
    Q_UNUSED( reply )
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkAccess::onNetworkAccessibleChanged( QNetworkAccessManager::NetworkAccessibility accessible )
{
    LOG_DEBUG << "network accessible changed: " << accessible;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkAccess::onPreSharedKeyAuthenticationRequired( QNetworkReply *reply, QSslPreSharedKeyAuthenticator *authenticator )
{
    Q_UNUSED( reply )
    Q_UNUSED( authenticator )

    LOG_DEBUG << "pre-shared key authentication required";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkAccess::onProxyAuthenticationRequired( const QNetworkProxy &proxy, QAuthenticator *authenticator )
{
    Q_UNUSED( proxy )
    Q_UNUSED( authenticator )

    LOG_DEBUG << "proxy authentication required";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkAccess::onReplyFinished()
{
    QNetworkReply *reply( dynamic_cast<QNetworkReply*>( sender() ) );

    if ( !reply )
        LOG_WARN << "bad reply!";
    else if ( QNetworkReply::NoError == reply->error() )
        LOG_TRACE << "reply finished";
    else
    {
        LOG_DEBUG << "reply error " << reply->error() << " " << qPrintable( reply->errorString() );

        if ( QNetworkReply::UnknownNetworkError == reply->error() )
        {
            LOG_WARN << "attempting to force online status...";
            setNetworkAccessible( QNetworkAccessManager::Accessible );

            // check access
            if ( QNetworkAccessManager::Accessible == networkAccessible() )
                LOG_DEBUG << "READY";
            else
                LOG_WARN << "network not accessible: " << networkAccessible();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkAccess::onSslErrors( QNetworkReply *reply, const QList<QSslError> &errors )
{
    Q_UNUSED( reply )

    // show list of errors
    foreach( const QSslError &error, errors )
    {
        const QString errorString( error.errorString() );

        LOG_DEBUG << "ssl error: " << error.error() << " " << qPrintable( errorString );
    }

    // check ignore the error
    if ( ignoreAllSslErrors_ )
        reply->ignoreSslErrors();
    else if ( ignoreSslErrors_.size() )
        reply->ignoreSslErrors( ignoreSslErrors_ );
    else
    {
        LOG_TRACE << "not ignoring errors";

        // show list of configured ssl certificates
        const QSslConfiguration conf( reply->request().sslConfiguration() );

        foreach ( const QSslCertificate& cert, conf.caCertificates() )
        {
            const QStringList orgs( cert.subjectInfo( QSslCertificate::Organization ) );

            LOG_TRACE << "cert serial number: " << cert.serialNumber().constData();

            foreach ( const QString& org, orgs )
                LOG_TRACE << "    org " << qPrintable( org );
        }
    }
}
