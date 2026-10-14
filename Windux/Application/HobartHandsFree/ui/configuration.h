/**
 * @file configuration.h
 * Widget for viewing and editing configuration.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CONFIGURATION
#define CONFIGURATION

#include "configeditorwidget.h"

#include <QMap>
#include <QWidget>

class commClient;
class deviceCommandResult;
class deviceConfig;
class deviceError;
class deviceState;
class message;

class QLabel;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for viewing and editing configuration.
class configurationWidget : public QWidget
{
    Q_OBJECT

    typedef configurationWidget _Myt;
    typedef QWidget _Mybase;

signals:

    /// Signal for disconnected.
    void disconnected();

    /// Signal for when pending configuration changes.
    /**
     * @param[in] count  number of config pending
     */
    void pendingConfigUpdated( int count );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    configurationWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~configurationWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if save pending.
    /**
     * @return  @c true if save pending, @c false otherwise
     */
    virtual bool savePending() const {return !savePending_.isEmpty();}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Connect to device.
    /**
     * @param[in] address  tag address
     */
    virtual void connectToDevice( const QString& address );

    /// Disconnect from device.
    virtual void disconnectFromDevice();

    /// Save configuration.
    /**
     * @return  @c true if config save sent to device, @c false otherwise
     */
    virtual bool save();

    /// Translate text.
    virtual void translate();

private slots:

    /// Slot to clear all device messages.
    void onClearAllMessages();

    /// Slot for pending config updated.
    void onPendingConfigUpdated( int count );

    /// Slot for receive message.
    void onRecvMessage( const message *msg );

    /// Slot to send logo to device.
    void onSendLogo( const QByteArray& logo );

    /// Slot to send message to device.
    void onSendMessage( const QString& message );

    /// Slot for timeout.
    void onTimeout();

    /// Slot to update firmware.
    void onUpdateFirmware( unsigned int id, const QString& filename );

private:

    typedef configEditorWidget::configMap configMap;

    enum {CONNECT_TIMEOUT = 30000};

    enum state
    {
        STATE_CONNECTING,
        STATE_READ_ALL_CONFIG,
        STATE_WAIT_CONFIG,
        STATE_WAIT_LOGO,
        STATE_READY,
        STATE_DISCONNECTING,
        STATE_DISCONNECTED
    };

    commClient *client_;

    QString address_;
    state state_;

    QTimer *timeout_;

    configMap savePending_;
    bool error_;

    // ---- //

    QLabel *tagAddressLabel_;
    QLabel *tagAddress_;

    QLabel *firmwareLabel_;
    QLabel *firmware_;

    configEditorWidget *configuration_;

    QLabel *connectionState_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Update connection state.
    void updateConnectionState( const QColor& fg, const QColor& bg, const QString& text );

    /// Process message.
    void processMessage( const deviceCommandResult *msg );

    /// Process message.
    void processMessage( const deviceConfig *msg );

    /// Process message.
    void processMessage( const deviceError *msg );

    /// Process message.
    void processMessage( const deviceState *msg );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CONFIGURATION
