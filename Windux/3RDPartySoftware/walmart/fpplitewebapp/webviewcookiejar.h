/**
 * @file webviewcookiejar.h
 * Cookie jar for web viewer.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef WEBVIEWCOOKIEJAR_H
#define WEBVIEWCOOKIEJAR_H

#include <QNetworkCookieJar>
#include <QStringList>

class QTimer;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Cookie jar object.
class webViewCookieJar : public QNetworkCookieJar
{
    typedef webViewCookieJar _Myt;
    typedef QNetworkCookieJar _Mybase;

    Q_OBJECT
    Q_PROPERTY(AcceptPolicy acceptPolicy READ acceptPolicy WRITE setAcceptPolicy)
    Q_PROPERTY(KeepPolicy keepPolicy READ keepPolicy WRITE setKeepPolicy)
    Q_PROPERTY(QStringList blockedCookies READ blockedCookies WRITE setBlockedCookies)
    Q_PROPERTY(QStringList allowedCookies READ allowedCookies WRITE setAllowedCookies)
    Q_PROPERTY(QStringList allowForSessionCookies READ allowForSessionCookies WRITE setAllowForSessionCookies)
    Q_ENUMS(KeepPolicy)
    Q_ENUMS(AcceptPolicy)

Q_SIGNALS:

    /// Signal emitted when cookies changed.
    void cookiesChanged();

public:

    /// Cookie jar accept policy.
    enum AcceptPolicy
    {
        AcceptAlways,                               ///< Accept all cookies.
        AcceptNever,                                ///< Never accept cookies.
        AcceptOnlyFromSitesNavigatedTo              ///< Accept cookies only from sites navigated to.
    };

    /// Cookie jar keep policy.
    enum KeepPolicy
    {
        KeepUntilExpire,                            ///< Keep cookies until expiration date.
        KeepUntilExit,                              ///< Keep cookies until we exit.
        KeepUntilTimeLimit                          ///< Keep cookies for amount of time.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    webViewCookieJar( QObject *parent = 0 );

    /// Destructor.
    virtual ~webViewCookieJar();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve cookies for url.
    /**
     * @param[in] url  url to retrieve
     * @return  list of cookies
     */
    virtual QList<QNetworkCookie> cookiesForUrl( const QUrl &url ) const;

    /// Set cookies for url.
    /**
     * @param[in] cookieList  cookies
     * @param[in] url  url to set
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool setCookiesFromUrl( const QList<QNetworkCookie> &cookieList, const QUrl &url );

    /// Retrieve accept policy.
    /**
     * @return  accept policy
     */
    virtual AcceptPolicy acceptPolicy() const;

    /// Set accept policy.
    /**
     * @param[in] policy  accept policy
     */
    virtual void setAcceptPolicy( AcceptPolicy policy );

    /// Retrieve keep policy.
    /**
     * @return  keep policy
     */
    virtual KeepPolicy keepPolicy() const;

    /// Set keep policy.
    /**
     * @param[in] policy  keep policy
     */
    virtual void setKeepPolicy( KeepPolicy policy );

    /// Retrieve blocked cookies.
    /**
     * @return  list of blocked cookies
     */
    virtual QStringList blockedCookies() const;

    /// Retrieve allowed cookies.
    /**
     * @return  list of allowed cookies
     */
    virtual QStringList allowedCookies() const;

    /// Retrieve allowed for session cookies.
    /**
     * @return  list of allowed for session cookies
     */
    virtual QStringList allowForSessionCookies() const;

    /// Set blocked cookies.
    /**
     * @param[in] list  list of blocked cookies
     */
    virtual void setBlockedCookies( const QStringList &list );

    /// Set allowed cookies.
    /**
     * @param[in] list  list of allowed cookies
     */
    virtual void setAllowedCookies( const QStringList &list );

    /// Set allowed for session cookies.
    /**
     * @param[in] list  list of allowed for session cookies
     */
    virtual void setAllowForSessionCookies( const QStringList &list );

    // ========================================================================
    // Methods
    // ========================================================================

public Q_SLOTS:

    /// Clear all cookies.
    void clear();

    /// Load settings.
    void loadSettings();

    /// Save cookies.
    void save();

private slots:

    /// Save cookies if neccessary.
    void saveIfNeccessary();

private:

    QTimer *saveTimer_;

    bool loaded_;
    bool changeOccurred_;

    AcceptPolicy acceptCookies_;
    KeepPolicy keepCookies_;

    QStringList block_;
    QStringList allow_;
    QStringList allowForSession_;

    /// Retrieve cookie policy file.
    QLatin1String cookiePolicyFile() const;

    /// Retrieve cookies file.
    QLatin1String cookiesFile() const;

    /// Purge old cookies.
    void purgeOldCookies();

    /// Load cookies.
    void load();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

#endif // WEBVIEWCOOKIEJAR_H
