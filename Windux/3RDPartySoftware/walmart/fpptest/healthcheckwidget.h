/**
 * @file healthcheckwidget.h
 * Health Check server widget.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HEALTHCHECKWIDGET_H
#define HEALTHCHECKWIDGET_H

#include "webserverstate.h"

#include <QByteArray>
#include <QMap>
#include <QTcpServer>
#include <QWidget>

class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QTcpSocket;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Health Check server widget.
class healthCheckWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY( webServerState::type state READ state WRITE setState NOTIFY stateChanged )

    typedef healthCheckWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  perent widget
     */
    healthCheckWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~healthCheckWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve web server state.
    /**
     * @return  state
     */
    virtual webServerState::type state() const {return state_;}

public Q_SLOTS:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve web server state.
    /**
     * @return  state
     */
    virtual void setState( webServerState::type value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Close web server.
    virtual void closeWebServer();

    /// Open web server.
    virtual void openWebServer();

Q_SIGNALS:

    /// Emitted when event settings changed.
    /**
     * @param[in] event  event name
     * @param[in] enabled  @c true to enable, @c false otherwise
     * @param[in] url  event url
     * @param[in] forced  @c true to force callback, @c false otherwise
     * @param[in] urlCallback  callback url
     * @param[in] file  filename
     * @param[in] parts  number of parts
     */
    void eventSettingsChanged( const QString& event, bool enabled, const QString& url, bool forced, const QString& urlCallback, const QString& file = QString(), int parts = 0 );

    /// Process client request.
    /**
     * @param[in,out] client  client socket
     * @param[in] header  message header
     * @param[in] body  message body
     */
    void processRequest( QTcpSocket *client, const QStringList &header, const QByteArray &body );

    /// Emitted when web server state changes.
    /**
     * @param[in] newState  new state
     */
    void stateChanged( webServerState::type newState );

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Show event.
    /**
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e ) override;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Process client buffer.
    /**
     * @param[in,out] client  client socket
     * @param[in,out] buffer  buffer
     */
    virtual void processClientBuffer( QTcpSocket *client, QByteArray &buffer );

private Q_SLOTS:

    /// Slot for client destroyed.
    void onClientDestroyed();

    /// Slot for client ready read.
    void onClientReadyRead();

    /// Slot for editing finished.
    void onLineEditEditingFinished();

    /// Slot for button clicked.
    void onPushButtonClicked();

    /// Slot for radio button clicked.
    void onRadioButtonClicked();

    /// Slot for server accept error.
    void onServerAcceptError( QAbstractSocket::SocketError error );

    /// Slot for server new connection.
    void onServerNewConnection();

private:

    typedef QMap<QTcpSocket*, QByteArray> bufferMap;

    QTcpServer server_;
    bufferMap clientBuffers_;

    webServerState::type state_;

    QLabel *portLabel_;
    QLineEdit *port_;
    QPushButton *open_;
    QPushButton *close_;

    QGroupBox *options_;
    QRadioButton *okay_;
    QRadioButton *okayNoContent_;
    QRadioButton *incompleteHeader_;
    QRadioButton *malformedHeader_;
    QRadioButton *malformedBody_;
    QRadioButton *noResponse_;
    QRadioButton *closeSocket_;
    QRadioButton *moved_;
    QRadioButton *unauthorized_;
    QRadioButton *notFound_;
    QRadioButton *exception_;

    QWidget *callback_;
    QLabel *callbackTitle_;
    QLineEdit *callbackUrl_;

    QWidget *updateConfig_;
    QLabel *updateConfigTitle_;
    QCheckBox *updateConfigEnabled_;
    QCheckBox *updateConfigCallbackEnabled_;
    QCheckBox *updateConfigCallbackForced_;
    QLineEdit *updateConfigUrl_;

    QWidget *applySettings_;
    QLabel *applySettingsTitle_;
    QCheckBox *applySettingsEnabled_;
    QCheckBox *applySettingsCallbackEnabled_;
    QCheckBox *applySettingsCallbackForced_;
    QLineEdit *applySettingsUrl_;

    QWidget *fetchProducts_;
    QLabel *fetchProductsTitle_;
    QCheckBox *fetchProductsEnabled_;
    QCheckBox *fetchProductsCallbackEnabled_;
    QCheckBox *fetchProductsCallbackForced_;
    QLineEdit *fetchProductsUrl_;

    QWidget *upgradeFirmware_;
    QLabel *upgradeFirmwareTitle_;
    QCheckBox *upgradeFirmwareEnabled_;
    QCheckBox *upgradeFirmwareCallbackEnabled_;
    QCheckBox *upgradeFirmwareCallbackForced_;
    QLineEdit *upgradeFirmwareUrl_;

    QLabel *upgradeFirmwareFileTitle_;
    QLineEdit *upgradeFirmwareFile_;

    QLabel *upgradeFirmwareNumSplitsTitle_;
    QLineEdit *upgradeFirmwareNumSplits_;

    QWidget *installFeature_;
    QLabel *installFeatureTitle_;
    QCheckBox *installFeatureEnabled_;
    QCheckBox *installFeatureCallbackEnabled_;
    QCheckBox *installFeatureCallbackForced_;
    QLineEdit *installFeatureUrl_;

    QLabel *installFeatureFileTitle_;
    QLineEdit *installFeatureFile_;

    QLabel *installFeatureNumSplitsTitle_;
    QLineEdit *installFeatureNumSplits_;

    QWidget *loadLabels_;
    QLabel *loadLabelsTitle_;
    QCheckBox *loadLabelsEnabled_;
    QCheckBox *loadLabelsCallbackEnabled_;
    QCheckBox *loadLabelsCallbackForced_;
    QLineEdit *loadLabelsUrl_;

    QLabel *loadLabelsFolderTitle_;
    QLineEdit *loadLabelsFolder_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Find a file matching pattern.
    static QString findFileWithPattern( const QString& pattern );

    /// Fetch network address.
    static QString netAddress();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HEALTHCHECKWIDGET_H
