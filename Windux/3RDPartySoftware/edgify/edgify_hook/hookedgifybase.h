/**
 * @file hookedgifybase.h
 * Edgify Base.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOOKEDGIFYBASE_H
#define HOOKEDGIFYBASE_H

#include <QList>
#include <QMutex>
#include <QObject>
#include <QTemporaryDir>

class edgifyapi;
class edgifyuart;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Edgify Base.
class edgifyHookBase : public QObject
{
    Q_OBJECT

    typedef edgifyHookBase _Myt;
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

    edgifyapi *api_;                                ///< Edgify API.

    QString hostAddress_;                           ///< Edgify Host Address.
    int hostPort_;                                  ///< Edgify Host Port.

    QString predictionUrl_;                         ///< Url for predictions.
    QString setGroundTruthUrl_;                     ///< Url for setting ground truth.

    int requestTimeout_;                            ///< Request timeout time (seconds).
    int requestNumRetries_;                         ///< Request number of retries.

    edgifyuart *uart_;                              ///< Edgify UART Wrapper.
    int uartErrors_;                                ///< Edgify UART Error Counter.

    QString uartFile_;                              ///< Serial Port file.
    int uartBaudRate_;                              ///< Serial Port Baud Rate.

    bool haveResult_;                               ///< Have result (for update truth command).

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    edgifyHookBase( QObject *parent = nullptr );

    /// Destructor.
    virtual ~edgifyHookBase();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve Edgify host address from database.
    /**
     * @return  host address
     */
    virtual QString hostAddress() const {return QString();}

    /// Retrieve Edgify host port from database.
    /**
     * @return  host port
     */
    virtual int hostPort() const {return 0;}

    /// Check if hook initialized and ready.
    /**
     * @return  @c true if ready, @c false otherwise
     */
    virtual bool isReady() const = 0;

    /// Reset result.
    virtual void resetResult() {haveResult_ = false;}

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Enable serial comm wrapper to Edgify.
    virtual void enableSerialComms();

    /// Trigger Edgify prediction.
    /**
     * Sends a request to Edgify device to retrieve prediction result.
     */
    virtual void predict();

    /// Send prediction results to Hobart service layer.
    /**
     * @param[in] result  prediction result
     * @param[in] image  captured image (if applicable)
     * @param[in] items  prediction items
     * @param[in] probs  prediction probabilities
     */
    virtual void sendPredictResults( PredictResult result, const QString& image = QString(), const QStringList& items = QStringList(), const QStringList& probs = QStringList() ) = 0;

    /// Update last online time.
    virtual void updateLastOnlineTime() = 0;

    /// Update Edgify truth.
    /**
     * @param[in] label  label
     * @param[in] source  truth source
     */
    virtual void updateTruth( const QString& label, const QString& source );

private slots:

    /// Slot for document processing.
    void onProcessDocument( const QJsonDocument& request, int status, const QJsonDocument& response );

    /// Slot for file processing.
    void onProcessFile( const QJsonDocument& request, int status, const QString& tempFileLocation, const QString& filename );

private:

    static const int NUM_SAVED_PREDS = 8;

    QTemporaryDir tempDir_;

    QList<QJsonObject> preds_;

    /// Load web headers.
    void loadWebHeaders();

    /// Format endpoint url.
    QUrl formatEndpointUrl( const QString& url ) const;

    /// Save prediction.
    QString savePrediction( const QJsonObject& obj );

    // not implemented
    edgifyHookBase( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOOKEDGIFYBASE_H
