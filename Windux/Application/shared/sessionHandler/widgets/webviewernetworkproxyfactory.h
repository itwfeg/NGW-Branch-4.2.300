/**
 * @file webviewernetworkproxyfactory.h
 * Proxy settings object for managing network proxies.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WEBVIEWERNETWORKPROXYFACTORY_H
#define WEBVIEWERNETWORKPROXYFACTORY_H

#include "proxyautoconfig.h"

#include <QMap>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QNetworkProxyQuery>
#include <QObject>
#include <QString>

class QNetworkReply;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Proxy settings object for managing network proxies.
class webViewerNetworkProxyFactory : public QObject, public QNetworkProxyFactory
{
    Q_OBJECT

    typedef webViewerNetworkProxyFactory _Myt;
    typedef QObject _Mybase;

    typedef QNetworkProxyFactory _Myimpl;

public:

    /// Configuration type.
    enum configType
    {
        System,                                     ///< Using system settings.
        Auto,                                       ///< Automatic proxy.
        Manual                                      ///< Manual proxy configuration.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] config  settings file
     * @param[in] parent  parent object
     */
    webViewerNetworkProxyFactory( const QString& config, QObject *parent = 0 );

    /// Destructor.
    virtual ~webViewerNetworkProxyFactory();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve configuration type.
    /**
     * @return  configuration type
     */
    virtual configType type() const {return config_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Retrieve a list of proxies for a query.
    /**
     * This function takes the query request, @p query, examines the details of the type of socket
     * or request and returns a list of @c QNetworkProxy objects that indicate the proxy servers
     * to be used, in order of preference.
     *
     * @param[in] query  proxy query
     * @return  list of network proxies
     */
    virtual QList<QNetworkProxy> queryProxy( const QNetworkProxyQuery &query = QNetworkProxyQuery() );

private Q_SLOTS:

    /// Slot to handle network replies.
    void onNetworkReply( QNetworkReply *reply );

private:

    enum {MAX_REDIRECTS = 8};

    configType config_;

    unsigned int redirects_;

    typedef QMap<QString, QString> paramMap;
    paramMap params_;

    proxyAutoConfig autoProxy_;
    QNetworkProxy manualProxy_;

    QString pacFile_;

    /// Parse proxy settings from file.
    bool parseProxySettings( const QString& config );

    /// Setup auto proxy.
    bool configureAutoProxy();

    /// Setup manual proxy.
    bool configureManualProxy();

    /// Parse PAC rule.
    static QList<QNetworkProxy> parseRule( const QString& rule );

    /// Retrieve proxy type.
    static QNetworkProxy::ProxyType manualProxyType( int t );

    /// Log proxy settings.
    static void writeProxySettings( const QNetworkProxy& proxy );

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // WEBVIEWERNETWORKPROXYFACTORY_H
