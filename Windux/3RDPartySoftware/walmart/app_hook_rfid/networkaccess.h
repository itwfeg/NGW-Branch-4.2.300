/**
 * @file networkacess.h
 * Network Access.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef NETWORKACCESS_H
#define NETWORKACCESS_H

#include <QList>
#include <QNetworkAccessManager>
#include <QSslError>

class QAuthenticator;
class QNetworkProxy;
class QNetworkReply;
class QSslPreSharedKeyAuthenticator;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Network Access.
class networkAccess : public QNetworkAccessManager
{
    Q_OBJECT

    typedef networkAccess _Myt;
    typedef QNetworkAccessManager _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    networkAccess( QObject *parent = nullptr );

    /// Destructor.
    virtual ~networkAccess();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve ignore all ssl errors.
    /**
     * @return  @c true when ignore all ssl errors, @c false otherwise
     */
    virtual bool ignoreAllSslErrors() const {return ignoreAllSslErrors_;}

    /// Set ignore all ssl errors.
    /**
     * @param[in] value  @c true to ignore all ssl errors, @c false otherwise
     */
    virtual void setIgnoreAllSslErrors( bool value ) {ignoreAllSslErrors_ = value;}

    /// Retrieve list of ssl errors ignored.
    /**
     * @return  list of ignored ssl errors
     */
    virtual QList<QSslError> ignoreSslErrors() const {return ignoreSslErrors_;}

    /// Set list of ssl errors to ignore.
    /**
     * @param[in] value  list of ssl errors to ignore
     */
    virtual void setIgnoreSslErrors( const QList<QSslError>& value ) {ignoreSslErrors_ = value;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Add credentials for authentication.
    /**
     * @param[in] host  host name
     * @param[in] user  user name
     * @param[in] pwd  password
     */
    virtual void addCredentials( const QString &host, const QString &user, const QString &pwd );

private slots:

    /// Slot for authentication required.
    void onAuthenticationRequired( QNetworkReply *reply, QAuthenticator *authenticator );

    /// Slot for finished.
    void onFinished( QNetworkReply *reply );

    /// Slot for network accessible changed.
    void onNetworkAccessibleChanged( QNetworkAccessManager::NetworkAccessibility accessible );

    /// Slot for pre-shared key authentication required.
    void onPreSharedKeyAuthenticationRequired( QNetworkReply *reply, QSslPreSharedKeyAuthenticator *authenticator );

    /// Slot for proxy authentication required.
    void onProxyAuthenticationRequired( const QNetworkProxy &proxy, QAuthenticator *authenticator );

    /// Slot for ssl errors.
    void onSslErrors( QNetworkReply *reply, const QList<QSslError> &errors );

private:

    bool ignoreAllSslErrors_;

    QList<QSslError> ignoreSslErrors_;

    typedef QPair<QString, QString> userPasswordPair;

    QMap<QString, userPasswordPair> credentials_;

    // not implemented
    networkAccess( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // NETWORKACCESS_H
