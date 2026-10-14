/**
 * @file oodbadapter.h
 * @brief Blackbox online ordering database adapter service.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef OODBADAPTER_H
#define OODBADAPTER_H

#include <messagecenterclient.h>

#include <QList>
#include <QMap>
#include <QMutex>

class FileResourceEvent;
class JsonDocumentEvent;
class OnlineOrderingAPI;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Blackbox online ordering database adapter service.
class OnlineOrderingDatabaseAdapter : public MessageCenterClient
{
    Q_OBJECT

    using _Myt = OnlineOrderingDatabaseAdapter;
    using _Mybase = MessageCenterClient;

public:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set server address.
    /**
     * @param[in] value  server address (i.e. https://some.server.com:port)
     */
    virtual void setServerAddress( const QString& value );

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

private slots:

    /// Slot for category received.
    void onCategoryReceived( const QJsonObject& obj );

    /// Slot for category image received.
    void onCategoryImageReceived( const QString& name, const QByteArray& a );

    /// Slot for department received.
    void onDepartmentReceived( const QJsonObject& obj );

    /// Slot for operator received.
    void onOperatorReceived( const QJsonObject& obj );

    /// Slot for product received.
    void onProductReceived( const QJsonObject& obj );

    /// Slot for category image received.
    void onProductImageReceived( const QString& name, const QByteArray& a );

private:

    using StringMapping = QMap<QString, QString>;

    static _Myt *instance_;
    static QMutex instanceMutex_;

    mutable QMutex m_;

    QList<const Event*> events_;
    QMutex eventsProcessing_;

    QString respService_;
    QString respObjectPath_;

    OnlineOrderingAPI *api_;

    StringMapping departmentRecToDatabase_;
    StringMapping departmentRecFromDatabase_;

    StringMapping productRecToDatabase_;
    StringMapping productRecFromDatabase_;

    StringMapping operatorRecToDatabase_;
    StringMapping operatorRecFromDatabase_;

    StringMapping categoryRecToDatabase_;
    StringMapping categoryRecFromDatabase_;

    /// Constructor.
    OnlineOrderingDatabaseAdapter();

    /// Destructor.
    ~OnlineOrderingDatabaseAdapter();

    /// Process resource event.
    void processResourceEvent( const FileResourceEvent *e );

    /// Process scale database event.
    void processScaleDatabaseEvent( const JsonDocumentEvent *e );

    /// Create database mappings.
    void createDatabaseMappings();

    /// Create database anti-mapping from mapping.
    static void createAntiMapping( const StringMapping& map, StringMapping& antiMap );

    /// Transform a JSON object using a map.
    static QJsonObject transform( const StringMapping& map, const QJsonObject& obj );

    // not implemented
    OnlineOrderingDatabaseAdapter( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // OODBADAPTER_H
