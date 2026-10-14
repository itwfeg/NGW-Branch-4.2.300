// -*- c++ -*-

#ifndef PROXYAUTOCONFIG_H
#define PROXYAUTOCONFIG_H

#include <qobject.h>
#include <qscriptvalue.h>

class QScriptContext;
class QScriptEngine;

/**
 * Class implementing the proxy auto-configuration (PAC) JavaScript api.
 */
class proxyAutoConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString config READ config WRITE setConfig )

    typedef proxyAutoConfig _Myt;
    typedef QObject _Mybase;

public:

    proxyAutoConfig( QObject *parent = 0 );
    ~proxyAutoConfig();

    /**
     * Retrieve config file name.
     */
    QString config() const {return config_;}

    /**
     * Call this to set the script to be executed. Note that the argument should be
     * the content of the .pac file to be used, not the URL where it is located.
     */
    bool setConfig( const QString &value );

    /**
     * Returns the result 
     */
    Q_SCRIPTABLE QString findProxyForUrl( const QString &url, const QString &host );

private:

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

private:

    QString config_;

    QScriptEngine *engine_;

};

#endif // PROXYAUTOCONFIG_H
