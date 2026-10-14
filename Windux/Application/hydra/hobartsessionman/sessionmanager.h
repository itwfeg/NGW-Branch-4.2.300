/**
 * @file sessionmanager.h
 * @brief Session manager for Hobart services.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <messagecenterclient.h>

#include <QMultiMap>
#include <QMutex>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Session Manager.
class SessionManager : public MessageCenterClient
{
    Q_OBJECT

    using _Myt = SessionManager;
    using _Mybase = MessageCenterClient;

public slots:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Retrieve number of child sessions.
    /**
     * @return  number of child sessions
     */
    uint count() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create (or resume) named session.
    /**
     * @param[in] type  session type
     * @param[in] name  session name
     * @return  session name upon success, empty string upon failure
     */
    QString createNamedSession( uint type, const QString& name );

    /// Create session.
    /**
     * @param[in] type  session type
     * @return  session name upon success, empty string upon failure
     */
    QString createSession( uint type );

    /// Destroy session.
    /**
     * @param[in] name
     */
    void destroySession( const QString& name );

    /// Resume named session.
    /**
     * @param[in] name  session name
     * @return  session name upon success, empty string upon failure
     */
    QString resumeNamedSession( const QString& name );

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve global instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Process event message signal.
    /**
     * @param[in] e  event
     * @param[in,out] destroy  set to @c true if event should be destroyed, @c false if ownership taken
     */
    virtual void onProcessEvent( const Event *e, bool& destroy ) override;

private:

    /// Service and object path.
    using ServiceObjectPath = QPair<QString, QString>;

    /// Map of service name to service providers.
    using ServiceProviderMap = QMultiMap<QString, ServiceObjectPath>;

    static _Myt *instance_;
    static QMutex instanceMutex_;

    QList<QString> wellKnownServices_;

    ServiceProviderMap providerAliases_;

    /// Constructor.
    SessionManager();

    /// Process session manager event.
    void processSessionManagerEvent( const Event *e );

    // not implemented
    SessionManager( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SESSIONMANAGER_H
