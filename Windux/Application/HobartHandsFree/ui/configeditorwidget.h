/**
 * @file configeditorwidget.h
 * Widget for viewing and editing configuration.
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CONFIGEDITORWIDGET
#define CONFIGEDITORWIDGET

#include <QByteArray>
#include <QImage>
#include <QMap>
#include <QTabWidget>

class QLabel;
class QLineEdit;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for viewing and editing configuration.
class configEditorWidget : public QTabWidget
{
    Q_OBJECT

    typedef configEditorWidget _Myt;
    typedef QTabWidget _Mybase;

signals:

    /// Signal to clear all device messages.
    void clearAllMessages();

    /// Signal for when pending configuration changes.
    /**
     * @param[in] count  number of config pending
     */
    void pendingConfigUpdated( int count );

    /// Signal to send logo to device.
    /**
     * @param[in] logo  logo image
     */
    void sendLogo( const QByteArray& logo );

    /// Signal so send message to device.
    /**
     * @param[in] message  message
     */
    void sendMessage( const QString& message );

    /// Signal to update device firmware.
    /**
     * @param[in] ota id
     * @param[in] filename
     */
    void updateFirmware( unsigned int id, const QString& filename );

public:

    /// Configuration value map.
    typedef QMap<int, QByteArray> configMap;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    configEditorWidget( QWidget *parent = 0 );

    /// Destructor.
    virtual ~configEditorWidget();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve firmware version.
    /**
     * @return  firmware version
     */
    virtual QString firmwareVersion() const;

    /// Retrieve pending configuration updates.
    /**
     * @return  pending config
     */
    virtual configMap pending() const;

    /// Set configuration value.
    /**
     * @param[in] id  id
     * @param[in] value  value
     */
    virtual void setValue( int id, const QByteArray& value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Clear widget and all configuration values.
    virtual void clear();

    /// Clear pending configuration values.
    virtual void clearPending();

    /// Process configuration values.
    /**
     * @return  @c true if fields processed, @c false otherwise
     */
    virtual bool process();

    /// Translate text.
    virtual void translate();

private slots:

    /// Slot for clicked.
    void onClicked();

    /// Slot for editing finished.
    void onEditingFinished();

private:

    typedef QPair<QLabel*, QWidget*> fieldPair;
    typedef QMap<int, fieldPair> fieldPairMap;

    typedef QMap<int, QWidget*> fieldParentMap;

    int version_;

    configMap config_;
    configMap configPending_;

    fieldPairMap fields_;

    fieldParentMap configExpected_v1_;

    // ---- //

    QWidget *operConfig_;
    int operConfigTabIndex_;

    QWidget *shiftTextConfig_;
    int shiftTextConfigTabIndex_;

    QWidget *advertConfig_;
    int advertConfigTabIndex_;

    QWidget *messaging_;
    int messagingTabIndex_;

    QLabel *messageLabel_;
    QLineEdit *message_;

    QPushButton *sendMessage_;
    QPushButton *clearAllMessages_;

    QWidget *logo_;
    int logoTabIndex_;

    QLabel *logoFilenameLabel_;
    QLineEdit *logoFilename_;

    QPushButton *findLogo_;
    QPushButton *sendLogo_;

    QLabel *logoPreview_;

    QImage logoImage_;

    QWidget *firmware_;
    int firmwareTabIndex_;

    QLabel *firmwareFilenameLabel_;
    QLineEdit *firmwareFilename_;

    QPushButton *findFirmware_;

    QLabel *updateFirmwareLabel_;
    QPushButton *updateFirmware_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Enable interface.
    void enableUserInput();

    /// Update combo box.
    void updateComboBox( int config, int configLen, const QString &text, int val, const QStringList& vals );

    /// Update label.
    void updateLabel( int config, int configLen, const QString &text, const QString& val );

    /// Update line edit.
    void updateLineEdit( int config, int configLen, const QString &text, const QString& val );

    /// Update spin box.
    void updateSpinBox( int config, int configLen, const QString &text, int val, int minVal, int maxVal );

    /// Create config field widget.
    template <class T>
    bool createField( int config, QWidget *parent, QLabel **fieldLabel, T **field, const QString& fieldLabelText );

    /// Connect config field signals.
    template <class T>
    void connectFieldSignals( T *field );

    /// Update config field background.
    template <class T>
    void updateFieldBackground( T *field, bool changed ) const;

    /// Check if config field has changed.
    template <class T>
    bool hasFieldChanged( int config, T *field, QByteArray& value ) const;

    /// Retrieve widget background role.
    template <class T>
    static QPalette::ColorRole widgetBackgroundRole( const T *widget );

    /// Retrieve widget default background color.
    template <class T>
    static QColor widgetDefaultBackgroundColor();

    /// Retrieve widget config value.
    template <class T>
    static QByteArray widgetConfigValue( const T *widget, int len );

    /// Convert byte array to hexadecimal string.
    static QString toHexString( const QByteArray &a );

    /// Convert byte array to integer.
    static int toInt8( const QByteArray &a );

    /// Convert byte array to integer.
    static unsigned int toUInt8( const QByteArray &a );

    /// Convert byte array to integer.
    static int toInt16( const QByteArray &a );

    /// Convert byte array to integer.
    static unsigned int toUInt16( const QByteArray &a );

    /// Convert byte array to integer.
    static int toInt32( const QByteArray &a );

    /// Convert byte array to integer.
    static unsigned int toUInt32( const QByteArray &a );

    /// Validate firmware image.
    static bool validateFirmwareImage( const QString &filename, int &version );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CONFIGEDITORWIDGET
