/**
 * @file session.h
 * @brief Abstract Session object.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef SESSION_H
#define SESSION_H

#include <messagecenterclient.h>

#include <QMultiMap>
#include <QMutex>

class FileResourceEvent;
class JsonDocumentEvent;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract Session object.
class Session : public MessageCenterClient
{
    Q_OBJECT

    using _Myt = Session;
    using _Mybase = MessageCenterClient;

public:

    /// Service and object path.
    using ServiceObjectPath = QPair<QString, QString>;

    /// Map of service name to service providers.
    using ServiceProviderMap = QMultiMap<QString, ServiceObjectPath>;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] n  session name
     * @param[in,out] parent  parent object
     */
    Session( const QString& n, QObject *parent = nullptr );

    /// Destructor.
    ~Session();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set service providers.
    /**
     * @param[in] value  service providers
     */
    void setServices( const ServiceProviderMap& value );

    /// Retrieve service providers.
    /**
     * @return  service providers
     */
    ServiceProviderMap services() const;

public slots:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve session id.
    /**
     * @return  id
     */
    QString id() const;

    /// Retrieve session name.
    /**
     * @return  name
     */
    QString name() const;

protected:

    QString id_;                                    ///< Session identifier.
    QString name_;                                  ///< Session name.

    QList<ServiceObjectPath> owners_;               ///< Session owner(s).

    ServiceProviderMap services_;                   /// Service providers.

    // ========================================================================
    // Events
    // ========================================================================

    /// Process event message signal.
    /**
     * @param[in] e  event
     * @param[in,out] destroy  set to @c true if event should be destroyed, @c false if ownership taken
     */
    virtual void onProcessEvent( const Event *e, bool& destroy ) override;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Send event message to service provider.
    /**
     * @param[in] service  dbus service to send to
     * @param[in] device  device type
     * @param[in] event  event type
     */
    void sendToService( const QString& service, DeviceType device, uint event ) const;

    /// Send event message to service provider.
    /**
     * @param[in] service  dbus service to send to
     * @param[in,out] e  event
     */
    void sendToService( const QString& service, const Event *e ) const;

private:

    static QMutex m_;
    static uint nextSession_;

    QList<DeviceType> deptNumRequired_;

    /// Process session event.
    void processSessionEvent( const Event *e );

    /// Process resource event.
    void processResourceEvent( const FileResourceEvent *e );

    /// Process scale database event.
    void processScaleDatabaseEvent( const JsonDocumentEvent *e );

    /// Retrieve next session identifier.
    static QString nextSessionId();

    // not implemented
    Session( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SESSION_H
