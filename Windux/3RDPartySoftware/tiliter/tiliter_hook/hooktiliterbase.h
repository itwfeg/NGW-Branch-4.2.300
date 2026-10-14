/**
 * @file hooktiliterbase.h
 * Tiliter Base.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOOKTILITERBASE_H
#define HOOKTILITERBASE_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPair>

class tiliterapi;

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Tiliter Base.
class tiliterHookBase : public QObject
{
    Q_OBJECT

    typedef tiliterHookBase _Myt;
    typedef QObject _Mybase;

signals:

    /// Send request.
    /**
     * @param[in] url  url to download
     * @param[in] request  request
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    void apiSend( const QUrl& url, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts );

public:


protected:

    /// Prediction results.
    enum PredictResult
    {
        Items = 0,                                  ///< Have items.
        LowScore,                                   ///< No items detected.
        Background,                                 ///< Background (i.e. platter) detected.
        Hand                                        ///< Hand detected.
    };

    mutable QMutex m_;                              ///< Mutex for access control.

    tiliterapi *api_;                               ///< Tiliter API.

    QString hostAddress_;                           ///< Tiliter Host Address.
    int hostPort_;                                  ///< Tiliter Host Port.

    QString resultUrl_;                             ///< Url for results.
    QString getWeightUrl_;                          ///< Url for weight information.
    QString labelLastResultUrl_;                    ///< Url for last label result.

    int requestTimeout_;                            ///< Request timeout time (seconds).
    int requestNumRetries_;                         ///< Request number of retries.

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    tiliterHookBase( QObject *parent = nullptr );

    /// Destructor.
    virtual ~tiliterHookBase();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve Tiliter host address from database.
    /**
     * @return  host address
     */
    virtual QString hostAddress() const {return QString();}

    /// Retrieve Tiliter host port from database.
    /**
     * @return  host port
     */
    virtual int hostPort() const {return 0;}

    /// Check if hook initialized and ready.
    /**
     * @return  @c true if ready, @c false otherwise
     */
    virtual bool isReady() const = 0;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Trigger Tiliter prediction.
    /**
     * Sends a request to Tiliter device to retrieve prediction result.
     */
    virtual void predict();

    /// Send prediction results to Hobart service layer.
    /**
     * @param[in] result  prediction result
     * @param[in] items  predictions items
     */
    virtual void sendPredictResults( PredictResult result, const QStringList& items = QStringList() ) = 0;

    /// Update last online time.
    virtual void updateLastOnlineTime() = 0;

private slots:

    /// Slot for document processing.
    void onProcessDocument( const QJsonDocument& request, int status, const QJsonDocument& response );

    /// Slot for file processing.
    void onProcessFile( const QJsonDocument& request, int status, const QString& tempFileLocation, const QString& filename );

private:

    /// Load web headers.
    void loadWebHeaders();

    /// Format endpoint url.
    QUrl formatEndpointUrl( const QString& url ) const;

    // not implemented
    tiliterHookBase( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOOKTILITERBASE_H
