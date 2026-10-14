#ifndef SESSION_H
#define SESSION_H

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>

class QHostAddress;
class QTcpSocket;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Network session object.
class session : public QObject
{
    Q_OBJECT

    typedef session _Myt;
    typedef QObject _Mybase;

public:

    /// Map of tag to values.
    typedef QMap<QString, QString> tagMap;

    /// List of tag maps.
    typedef QList<tagMap> tagMapList;

    /// Session types.
    enum sessionType
    {
        PULL_AND_CLEAR_TOTALS,                      ///< Pull and clear totals.
        PULL_EXPIRATION_LABELS,                     ///< Pull expiration labels.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] t  session type
     * @param[in,out] parent  parent object
     */
    session( sessionType t, QObject *parent = nullptr );

    /// Destructor.
    virtual ~session();

    //
    // Properties
    //

    /// Retrieve results.
    /**
     * @return  tag records
     */
    virtual tagMapList results() const {return results_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Connect to host.
    /**
     * @param[in] address  host address
     * @param[in] port  host port
     * @return  @c true if connection established, @c false otherwise
     */
    virtual bool connectToHost( const QHostAddress& address, quint16 port );

signals:

    /// Signal for session complete.
    /**
     * This signal is emitted when the session completes. Data may or may not have been pulled
     * from the host machine.
     */
    void complete();

protected:

    sessionType t_;                                 ///< Session type.

    QTcpSocket *socket_;                            ///< Client socket.

    tagMapList results_;                            ///< Tag results.

private slots:

    /// Slot for socket connected.
    void onSocketConnected();

    /// Slot for socket disconnected.
    void onSocketDisconnected();

    /// Slot for socket ready read.
    void onSocketReadyRead();

    /// Slot for timeout.
    void onTimeout();

private:

    enum {SESSION_TIMEOUT = 60000};

    static const char RS;
    static const char US;

    QByteArray a_;

    /// Process buffer.
    void processBuffer();

    /// Process response.
    void processResponse( const QByteArray& response );

    /// Register session.
    void registerSession() const;

    /// Terminate session.
    void terminateSession() const;

    /// Pull and clear totals.
    void pullAndClearTotals() const;

    /// Pull expiration labels.
    void pullExpirationLabels() const;

    /// Split tags.
    static tagMap splitTags( const QByteArray& response );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SESSION_H
