#include <qscriptengine.h>
#include <qdebug.h>
#include <qnetworkinterface.h>
#include <qhostaddress.h>
#include <qhostinfo.h>
#include <qregexp.h>
#include <qfile.h>

#include <proxyautoconfig.h>

proxyAutoConfig::proxyAutoConfig( QObject *parent ) :
    _Mybase( parent ),
    engine_( new QScriptEngine( this ) )
{
    install();
}

proxyAutoConfig::~proxyAutoConfig()
{
}

bool proxyAutoConfig::setConfig( const QString &value )
{
    // Read the PAC file in a String
    QFile scriptFile( config_ = value );

    if ( !scriptFile.open(QIODevice::ReadOnly|QIODevice::Text) )
        return false;

    QString str( scriptFile.readAll() );

    scriptFile.close();

    if ( !engine_->canEvaluate( str ) )
        return false;

    engine_->evaluate( str );
    return true;
}

void proxyAutoConfig::install()
{
    QScriptValue globalObject( engine_->globalObject() );
    QScriptValue fun;

    fun = engine_->newFunction( debug );
    globalObject.setProperty( QString("debug"), fun );

    fun = engine_->newFunction( myIpAddress );
    globalObject.setProperty( QString("myIpAddress"), fun );

    fun = engine_->newFunction( isInNet );
    globalObject.setProperty( "isInNet", fun );

    fun = engine_->newFunction( isPlainHostName );
    globalObject.setProperty( "isPlainHostName", fun );

    fun = engine_->newFunction( shExpMatch );
    globalObject.setProperty( "shExpMatch", fun );

    fun = engine_->newFunction( dnsDomainIs );
    globalObject.setProperty( "dnsDomainIs", fun );

    fun = engine_->newFunction( dnsResolve );
    globalObject.setProperty( "dnsResolve", fun );
}

/* void debug message */
QScriptValue proxyAutoConfig::debug( QScriptContext *context, QScriptEngine *engine )
{
    if ( 1 != context->argumentCount() )
        return context->throwError( "Debug takes one argument" );

    qDebug() << context->argument(0).toString();

    return engine->undefinedValue();
}

/* string myIpAddress */
QScriptValue proxyAutoConfig::myIpAddress( QScriptContext *context, QScriptEngine *engine )
{
    if ( 0 != context->argumentCount() )
        return context->throwError( "myIpAddress takes no arguments" );

    const QHostAddress localHost( QHostAddress::LocalHost );
    const QHostAddress localHostIPv6( QHostAddress::LocalHostIPv6 );

    foreach ( QHostAddress address, QNetworkInterface::allAddresses() )
        if (( localHost != address ) && ( localHostIPv6 != address ))
            return QScriptValue( engine, address.toString() );

    return engine->undefinedValue();
}

/* bool isInNet ipaddress, netaddress, netmask */
QScriptValue proxyAutoConfig::isInNet( QScriptContext *context, QScriptEngine *engine )
{
    if ( 3 != context->argumentCount() )
        return context->throwError( "isInNet takes three arguments" );

    QHostAddress addr( context->argument(0).toString() );
    QHostAddress netaddr( context->argument(1).toString() );
    QHostAddress netmask( context->argument(2).toString() );

    if ( (netaddr.toIPv4Address() & netmask.toIPv4Address()) == (addr.toIPv4Address() & netmask.toIPv4Address()) )
        return QScriptValue( engine, true );

    return QScriptValue( engine, false );
}

/* bool isPlainHostName hostname */
QScriptValue proxyAutoConfig::isPlainHostName( QScriptContext *context, QScriptEngine *engine )
{
    if ( 1 != context->argumentCount() )
        return context->throwError( "isPlainHostName takes one argument" );

    QString hostname( context->argument(0).toString() );
    qDebug() << "isPlainHostName - hostname: " << hostname;

    return QScriptValue( engine, !hostname.contains('.') );
}

/* bool shExpMatch url, glob */
QScriptValue proxyAutoConfig::shExpMatch( QScriptContext *context, QScriptEngine *engine )
{
    if ( 2 != context->argumentCount() )
        return context->throwError( "shExpMatch takes two arguments" );

    QRegExp re( context->argument(1).toString(), Qt::CaseSensitive, QRegExp::Wildcard );

    return QScriptValue( engine, re.exactMatch( context->argument(0).toString() ) );
}

/* bool dnsDomainIs host, domain */
QScriptValue proxyAutoConfig::dnsDomainIs( QScriptContext *context, QScriptEngine *engine )
{
    if ( 2 != context->argumentCount() )
        return context->throwError( "dnsDomainIs takes two arguments" );

    QString host( context->argument(0).toString() );
    QString domain( context->argument(1).toString() );

    return QScriptValue( engine, host.contains( domain, Qt::CaseInsensitive ) );
}

/* string dnsResolve hostname */
QScriptValue proxyAutoConfig::dnsResolve( QScriptContext *context, QScriptEngine *engine )
{
    if ( 1 != context->argumentCount() )
        return context->throwError( "dnsResolve takes one arguments" );

    QHostInfo info( QHostInfo::fromName( context->argument(0).toString() ) );
    QList<QHostAddress> addresses( info.addresses() );

    if ( addresses.isEmpty() )
        return engine->nullValue(); // TODO: Should this be undefined or an exception? check other implementations

    return QScriptValue( engine, addresses.first().toString() );
}

QString proxyAutoConfig::findProxyForUrl( const QString &url, const QString &host )
{
    QScriptValue global( engine_->globalObject() );
    QScriptValue fun( global.property( "FindProxyForURL" ) );

    if ( !fun.isFunction() )
        return QString( "DIRECT" );

    QScriptValueList args;
    args << engine_->toScriptValue( url ) << engine_->toScriptValue( host );

    QScriptValue val( fun.call( global, args ) );

    return val.toString();
}

