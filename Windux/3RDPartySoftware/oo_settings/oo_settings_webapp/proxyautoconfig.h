/**
 * @file proxyautoconfig.h
 * Class implementing the proxy auto-configuration (PAC) JavaScript API.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef PROXYAUTOCONFIG_H
#define PROXYAUTOCONFIG_H

#include <QObject>
#include <QScriptValue>

class QScriptContext;
class QScriptEngine;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class implementing the proxy auto-configuration (PAC) JavaScript API.
class proxyAutoConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString config READ config WRITE setConfig )

    typedef proxyAutoConfig _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    proxyAutoConfig( QObject *parent = 0 );

    /// Destructor.
    virtual ~proxyAutoConfig();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve PAC configuration.
    /**
     * @return  PAC configuration file
     */
    virtual QString config() const {return config_;}

    /// Set configuration.
    /**
     * @param[in] value  PAC configuration file
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool setConfig( const QString &value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Find proxy for url.
    /**
     * @param[in] url  url
     * @param[in] host  host
     * @return  result
     */
    Q_SCRIPTABLE QString findProxyForUrl( const QString &url, const QString &host );

private:

    QString config_;

    QScriptEngine *engine_;

    /// Install configuration.
    void install();

    /* void debug message */
    static QScriptValue debug( QScriptContext *context, QScriptEngine *engine );

    /* string myIpAddress */
    static QScriptValue myIpAddress( QScriptContext *context, QScriptEngine *engine );

    /* bool isInNet ipaddress, netaddress, netmask */
    static QScriptValue isInNet( QScriptContext *context, QScriptEngine *engine );

    /* bool isPlainHostName hostname */
    static QScriptValue isPlainHostName( QScriptContext *context, QScriptEngine *engine );

    /* bool shExpMatch url, glob */
    static QScriptValue shExpMatch( QScriptContext *context, QScriptEngine *engine );

    /* bool dnsDomainIs host, domain */
    static QScriptValue dnsDomainIs( QScriptContext *context, QScriptEngine *engine );

    /* string dnsResolve hostname */
    static QScriptValue dnsResolve( QScriptContext *context, QScriptEngine *engine );

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

#endif // PROXYAUTOCONFIG_H
