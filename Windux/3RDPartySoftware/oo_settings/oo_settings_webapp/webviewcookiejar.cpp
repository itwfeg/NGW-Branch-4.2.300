/**
 * @file webviewcookiejar.cpp
 *
 * @copyright Copyright (c) 2017Hobart Corporation
 * @author Randy Blankley
 */

#include "webviewcookiejar.h"

#include <QCoreApplication>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMetaEnum>
#include <QNetworkCookie>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>

static const unsigned int AUTOSAVE_TIMER = 3000;

static const unsigned int JAR_VERSION = 23;

QT_BEGIN_NAMESPACE

/// Output stream operator.
/**
 * param[in,out] stream  stream
 * param[out] list  list to write
 * return  reference to @p stream
 */
QDataStream& operator << ( QDataStream &stream, const QList<QNetworkCookie> &list )
{
    stream << JAR_VERSION;
    stream << quint32( list.size() );

    foreach ( const QNetworkCookie& c, list )
        stream << c.toRawForm();

    return stream;
}

/// Input stream operator.
/**
 * param[in,out] stream  stream
 * param[in,out] list  list to read
 * return  reference to @p stream
 */
QDataStream& operator >> ( QDataStream &stream, QList<QNetworkCookie> &list )
{
    list.clear();

    quint32 version;
    stream >> version;

    if ( JAR_VERSION != version )
        return stream;

    quint32 count;
    stream >> count;

    for ( quint32 i( 0 ); i < count; ++i )
    {
        QByteArray value;
        stream >> value;

        const QList<QNetworkCookie> newCookies( QNetworkCookie::parseCookies( value ) );

        if (( 0 == newCookies.count() ) && ( 0 != value.length() ))
            qWarning() << "CookieJar: Unable to parse saved cookie:" << value;

        foreach ( const QNetworkCookie& c, newCookies )
            list.append( c );

        if ( stream.atEnd() )
            break;
    }

    return stream;
}

QT_END_NAMESPACE

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewCookieJar::webViewCookieJar( QObject *parent ) :
    _Mybase( parent ),
    loaded_( false ),
    changeOccurred_( false ),
    acceptCookies_( AcceptOnlyFromSitesNavigatedTo ),
    keepCookies_( KeepUntilExpire )
{
    saveTimer_ = new QTimer( this );
    saveTimer_->setSingleShot( false );

    connect( saveTimer_, SIGNAL(timeout()), SLOT(saveIfNeccessary()) );

    saveTimer_->start( AUTOSAVE_TIMER );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewCookieJar::~webViewCookieJar()
{
    if ( KeepUntilExit == keepCookies_ )
        clear();

    saveIfNeccessary();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewCookieJar::clear()
{
    setAllCookies( QList<QNetworkCookie>() );

    changeOccurred_ = true;
    Q_EMIT cookiesChanged();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QLatin1String webViewCookieJar::cookiePolicyFile() const
{
/*
    QString directory( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) );

    if ( directory.isEmpty() )
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();

    if ( !QFile::exists( directory ) )
    {
        QDir dir;
        dir.mkpath(directory);
    }

    return directory + QLatin1String( "/cookiePolicy.ini" );
*/
    return QLatin1String( "./cookiePolicy" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QLatin1String webViewCookieJar::cookiesFile() const
{
/*
    QString directory( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) );

    if ( directory.isEmpty() )
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();

    if ( !QFile::exists( directory ) )
    {
        QDir dir;
        dir.mkpath(directory);
    }

    return directory + QLatin1String( "/cookies.ini" );
*/
    return QLatin1String( "./cookies" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewCookieJar::load()
{
    if ( loaded_ )
        return;

    // load cookies and exceptions
    qRegisterMetaTypeStreamOperators<QList<QNetworkCookie> >( "QList<QNetworkCookie>" );

    QSettings cookieSettings( cookiesFile(), QSettings::IniFormat );
    setAllCookies( qvariant_cast<QList<QNetworkCookie> >( cookieSettings.value( QLatin1String( "cookies" ) ) ) );

    cookieSettings.beginGroup( QLatin1String( "Exceptions" ) );
    block_ = cookieSettings.value( QLatin1String( "block" ) ).toStringList();
    allow_ = cookieSettings.value( QLatin1String( "allow" ) ).toStringList();
    allowForSession_ = cookieSettings.value( QLatin1String( "allowForSession" ) ).toStringList();

    qSort( block_.begin(), block_.end() );
    qSort( allow_.begin(), allow_.end() );
    qSort( allowForSession_.begin(), allowForSession_.end() );

    loadSettings();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewCookieJar::loadSettings()
{
    QByteArray value;

    QSettings settings( cookiePolicyFile(), QSettings::IniFormat );
    settings.beginGroup( QLatin1String( "cookies") );

    value = settings.value( QLatin1String( "acceptCookies" ), QLatin1String( "AcceptOnlyFromSitesNavigatedTo" ) ).toByteArray();
    QMetaEnum acceptPolicyEnum( staticMetaObject.enumerator( staticMetaObject.indexOfEnumerator( "AcceptPolicy" ) ) );
    acceptCookies_ = acceptPolicyEnum.keyToValue( value ) == -1 ?
                        AcceptOnlyFromSitesNavigatedTo :
                        static_cast<AcceptPolicy>( acceptPolicyEnum.keyToValue(value) );

    value = settings.value( QLatin1String( "keepCookiesUntil" ), QLatin1String( "KeepUntilExpire" ) ).toByteArray();
    QMetaEnum keepPolicyEnum( staticMetaObject.enumerator( staticMetaObject.indexOfEnumerator( "KeepPolicy" ) ) );
    keepCookies_ = keepPolicyEnum.keyToValue( value ) == -1 ?
                        KeepUntilExpire :
                        static_cast<KeepPolicy>( keepPolicyEnum.keyToValue( value ) );

    if ( KeepUntilExit == keepCookies_ )
        setAllCookies( QList<QNetworkCookie>() );

    loaded_ = true;
    Q_EMIT cookiesChanged();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewCookieJar::save()
{
    if ( !loaded_ )
        return;

    purgeOldCookies();

    QList<QNetworkCookie> cookies( allCookies() );
    for ( int i( cookies.count() ); i--; )
        if ( cookies.at( i ).isSessionCookie() )
            cookies.removeAt( i );

    QSettings cookieSettings( cookiesFile(), QSettings::IniFormat );
    cookieSettings.setValue( QLatin1String("cookies"), QVariant::fromValue<QList<QNetworkCookie> >( cookies ) );
    cookieSettings.beginGroup( QLatin1String("Exceptions") );
    cookieSettings.setValue( QLatin1String("block"), block_ );
    cookieSettings.setValue( QLatin1String("allow"), allow_ );
    cookieSettings.setValue( QLatin1String("allowForSession"), allowForSession_ );

    // save cookie settings
    QSettings settings( cookiePolicyFile(), QSettings::IniFormat );
    settings.beginGroup( QLatin1String( "cookies" ) );

    const QMetaEnum acceptPolicyEnum( staticMetaObject.enumerator( staticMetaObject.indexOfEnumerator( "AcceptPolicy" ) ) );
    settings.setValue( QLatin1String( "acceptCookies" ), QLatin1String( acceptPolicyEnum.valueToKey( acceptCookies_ ) ) );

    const QMetaEnum keepPolicyEnum( staticMetaObject.enumerator( staticMetaObject.indexOfEnumerator( "KeepPolicy" ) ) );
    settings.setValue( QLatin1String( "keepCookiesUntil" ), QLatin1String( keepPolicyEnum.valueToKey( keepCookies_ ) ) );

    changeOccurred_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewCookieJar::saveIfNeccessary()
{
    if (( loaded_ ) && ( changeOccurred_ ))
        save();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewCookieJar::purgeOldCookies()
{
    QList<QNetworkCookie> cookies( allCookies() );
    if ( cookies.isEmpty() )
        return;

    const QDateTime now( QDateTime::currentDateTime() );
    const int oldCount( cookies.count() );

    for ( int i( cookies.count() ); i--; )
        if (( !cookies.at( i ).isSessionCookie() ) && ( cookies.at( i ).expirationDate() < now ))
            cookies.removeAt( i );

    if ( oldCount == cookies.count() )
        return;

    setAllCookies( cookies );

    changeOccurred_ = true;
    Q_EMIT cookiesChanged();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<QNetworkCookie> webViewCookieJar::cookiesForUrl( const QUrl &url ) const
{
    if ( !loaded_ )
    {
        _Myt *that( const_cast<_Myt*>( this ) );
        that->load();
    }
/*
    QWebEngineSettings *globalSettings = QWebEngineSettings::globalSettings();
    if (globalSettings->testAttribute(QWebEngineSettings::PrivateBrowsingEnabled)) {
        QList<QNetworkCookie> noCookies;
        return noCookies;
    }
*/
    return _Mybase::cookiesForUrl (url );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webViewCookieJar::setCookiesFromUrl( const QList<QNetworkCookie> &cookieList, const QUrl &url )
{
    if ( !loaded_ )
        load();
/*
    QWebEngineSettings *globalSettings = QWebEngineSettings::globalSettings();
    if (globalSettings->testAttribute(QWebEngineSettings::PrivateBrowsingEnabled))
        return false;
*/
    const QString host( url.host() );
    const bool eBlock( block_.end() != qBinaryFind( block_.begin(), block_.end(), host ) );
    const bool eAllow( allow_.end() != qBinaryFind( allow_.begin(), allow_.end(), host ) );
    const bool eAllowSession( allowForSession_.end() != qBinaryFind( allowForSession_.begin(), allowForSession_.end(), host ) );

    bool addedCookies = false;
    // pass exceptions
    bool acceptInitially = (acceptCookies_ != AcceptNever);
    if ((acceptInitially && !eBlock)
        || (!acceptInitially && (eAllow || eAllowSession))) {
        // pass url domain == cookie domain
        QDateTime soon = QDateTime::currentDateTime();
        soon = soon.addDays(90);
        foreach (QNetworkCookie cookie, cookieList) {
            QList<QNetworkCookie> lst;
            if (keepCookies_ == KeepUntilTimeLimit
                && !cookie.isSessionCookie()
                && cookie.expirationDate() > soon) {
                    cookie.setExpirationDate(soon);
            }
            lst += cookie;
            if (QNetworkCookieJar::setCookiesFromUrl(lst, url)) {
                addedCookies = true;
            } else {
                // finally force it in if wanted
                if (acceptCookies_ == AcceptAlways) {
                    QList<QNetworkCookie> cookies = allCookies();
                    cookies += cookie;
                    setAllCookies(cookies);
                    addedCookies = true;
                }
#if 0
                else
                    qWarning() << "setCookiesFromUrl failed" << url << cookieList.value(0).toRawForm();
#endif
            }
        }
    }

    if ( addedCookies )
    {
        changeOccurred_ = true;
        Q_EMIT cookiesChanged();
    }

    return addedCookies;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewCookieJar::AcceptPolicy webViewCookieJar::acceptPolicy() const
{
    if ( !loaded_ )
    {
        _Myt *that( const_cast<_Myt*>( this ) );
        that->load();
    }

    return acceptCookies_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewCookieJar::setAcceptPolicy( AcceptPolicy policy )
{
    if ( !loaded_ )
        load();

    if ( policy == acceptCookies_ )
        return;

    acceptCookies_ = policy;
    changeOccurred_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewCookieJar::KeepPolicy webViewCookieJar::keepPolicy() const
{
    if ( !loaded_ )
    {
        _Myt *that( const_cast<_Myt*>( this ) );
        that->load();
    }

    return keepCookies_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewCookieJar::setKeepPolicy(KeepPolicy policy)
{
    if ( !loaded_ )
        load();

    if ( policy == keepCookies_ )
        return;

    keepCookies_ = policy;
    changeOccurred_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList webViewCookieJar::blockedCookies() const
{
    if ( !loaded_ )
    {
        _Myt *that( const_cast<_Myt*>( this ) );
        that->load();
    }

    return block_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList webViewCookieJar::allowedCookies() const
{
    if ( !loaded_ )
    {
        _Myt *that( const_cast<_Myt*>( this ) );
        that->load();
    }

    return allow_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList webViewCookieJar::allowForSessionCookies() const
{
    if ( !loaded_ )
    {
        _Myt *that( const_cast<_Myt*>( this ) );
        that->load();
    }

    return allowForSession_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewCookieJar::setBlockedCookies(const QStringList &list)
{
    if ( !loaded_ )
        load();

    block_ = list;
    qSort( block_.begin(), block_.end() );

    changeOccurred_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewCookieJar::setAllowedCookies(const QStringList &list)
{
    if (!loaded_)
        load();

    allow_ = list;
    qSort( allow_.begin(), allow_.end() );

    changeOccurred_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewCookieJar::setAllowForSessionCookies(const QStringList &list)
{
    if (!loaded_)
        load();

    allowForSession_ = list;
    qSort( allowForSession_.begin(), allowForSession_.end() );

    changeOccurred_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
