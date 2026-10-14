/**
 * @file messagecenterclient.h
 * @brief Base class for clients of message center.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef MESSAGECENTERCLIENT_H
#define MESSAGECENTERCLIENT_H

#include "devicetype.h"
#include "events_global.h"

#include <QList>
#include <QObject>
#include <QPair>

class Event;

class ComHobartcorpMessageCenterInterface;

namespace com {
    namespace hobartcorp {
        using MessageCenter = ::ComHobartcorpMessageCenterInterface;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Message Center Client.
class EVENTS_EXPORT MessageCenterClient : public QObject
{
    Q_OBJECT

    using _Myt = MessageCenterClient;
    using _Mybase = QObject;

public:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve service.
    /**
     * @return  service
     */
    QString service() const;

    /// Retrieve object path.
    /**
     * @return  path
     */
    QString objectPath() const;

public slots:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Process event message slot.
    /**
     * @param[in] a  event data
     */
    void processEvent( const QByteArray& a );

protected:

    QString service_;                               ///< DBUS service
    QString objectPath_;                            ///< DBUS object path

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] service  service
     * @param[in] objectPath  object path
     * @param[in,out] parent  parent object
     */
    MessageCenterClient( const QString& service, const QString& objectPath, QObject *parent = nullptr );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve if self processing is enabled.
    /**
     * This is disabled by default.
     * @return  @c true when self processing is enabled, @c false otherwise
     */
    bool enableSelfProcessing() const;

    /// Set if self processing is enabled.
    /**
     * @param[in] value  @c true when self processing is enabled, @c false otherwise
     */
    void setEnableSelfProcessing( bool value );

    /// Register for all events.
    /**
     * @param[in] value  @c true to receive all events, @c false otherwise
     */
    void setRegisterAllEvents( bool value );

    // ========================================================================
    // Events
    // ========================================================================

    /// Process event message signal.
    /**
     * @note When setting @p destroy to @c false you are taking ownership of the object and you should
     * call @fn destroyEvent() after you are done with the @c Event object.
     * @param[in] e  event
     * @param[in,out] destroy  set to @c true if event should be destroyed, @c false if ownership taken
     */
    virtual void onProcessEvent( const Event *e, bool& destroy );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Destroy event.
    /**
     * @param[in,out] doomed  event to delete
     */
    void destroyEvent( const Event *doomed ) const;

    /// Post event message.
    /**
     * @param[in] device  device type
     * @param[in] event  event type
     */
    void post( DeviceType device, uint event ) const;

    /// Post event message.
    /**
     * @param[in,out] e  event
     */
    void post( const Event *e ) const;

    /// Register for event processing.
    /**
     * @param[in] device  device type
     * @param[in] event  event type; use zero (0) for all events
     */
    void registerEvent( DeviceType device, uint event = 0 );

    /// Send event message.
    /**
     * @param[in] service  dbus service to send to
     * @param[in] objectPath  dbus object path to send to
     * @param[in] device  device type
     * @param[in] event  event type
     */
    void send( const QString& service, const QString& objectPath, DeviceType device, uint event ) const;

    /// Send event message.
    /**
     * @param[in] service  dbus service to send to
     * @param[in] objectPath  dbus object path to send to
     * @param[in,out] e  event
     */
    void send( const QString& service, const QString& objectPath, const Event *e ) const;

    /// Unregister event from processing.
    /**
     * @param[in] device  device type
     * @param[in] event  event type; use zero (0) for all events
     */
    void unregisterEvent( DeviceType device, uint event = 0 );

private:

    using EventType = QPair<DeviceType, uint> ;
    using EventTypeList = QList<EventType>;

    com::hobartcorp::MessageCenter *mc_;

    EventTypeList events_;

    bool all_;
    bool self_;

    // not implemented
    MessageCenterClient( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MESSAGECENTERCLIENT_H
