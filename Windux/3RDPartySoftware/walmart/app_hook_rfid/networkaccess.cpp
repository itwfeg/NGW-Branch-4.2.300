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
    connect( this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), SLOT(onAuthenticationRequired(QNetworkReply*,QAuthenticator*)) );
    connect( this, SIGNAL(finished(QNetworkReply*)), SLOT(onFinished(QNetworkReply*)) );
    connect( this, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)), SLOT(onNetworkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)) );
    connect( this, SIGNAL(preSharedKeyAuthenticationRequired(QNetworkReply*,QSslPreSharedKeyAuthenticator*)), SLOT(onPreSharedKeyAuthenticationRequired(QNetworkReply*,QSslPreSharedKeyAuthenticator*)) );
    connect( this, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), SLOT(onProxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)) );
    connect( this, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(onSslErrors(QNetworkReply*,QList<QSslError>)) );

    // check access
    if ( QNetworkAccessManager::Accessible == networkAccessible() )
        LOG_DEBUG << "READY";
    else
        LOG_WARN << "network not accessible: " << networkAccessible();
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
