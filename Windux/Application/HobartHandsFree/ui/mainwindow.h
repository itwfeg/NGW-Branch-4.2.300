/**
 * @file mainwindow.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef MAINWINDOW
#define MAINWINDOW

#include <QDateTime>
#include <QMainWindow>

class bleAgent;
class commClient;
class configurationWidget;
class firmwareUpgradeDialog;
class message;
class sensorSelectionWidget;

class QAbstractButton;
class QDialogButtonBox;
class QPushButton;
class QStackedWidget;
class QTabWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Main window.
class mainWindow : public QMainWindow
{
    Q_OBJECT

    typedef mainWindow _Myt;
    typedef QMainWindow _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    mainWindow( QWidget *parent = 0 );

    /// Destructor.
    virtual ~mainWindow();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate text.
    virtual void translate();

protected:

    /// States.
    enum state
    {
        STATE_UNKNOWN,                              ///< Unknown state.
        STATE_DASHBOARD,                            ///< Dashboard.
        STATE_CONFIGURATION,                        ///< Configuration.
    };

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve currently selected sensor id.
    /**
     * @return  sensor id
     */
    virtual QString selectedSensor() const;

    /// Set current state.
    /**
     * @param[in] value  state
     */
    virtual void setCurrentState( state value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Remove all buttons.
    virtual void removeAllButtons();

private Q_SLOTS:

    /// Slot for button clicked.
    void onButtonClicked( QAbstractButton *button );

    /// Slot for disconnected.
    void onDisconnected();

    /// Slot for item double clicked.
    void onItemDoubleClicked( const QString &id );

    /// Slot for item selection changed.
    void onItemSelectionChanged();

    /// Slot for pending config updated.
    void onPendingConfigUpdated( int count );

    /// Slot for receive message.
    void onRecvMessage( const message *msg );

    /// Slot for refresh pending.
    void onUpdateRefreshPending( bool pending );

private:

    commClient *client_;

    firmwareUpgradeDialog *firmwareUpgrade_;

    QStringList ignoredFirmwareRequests_;
    QDateTime pendingFirmware_;

    state state_;

    QWidget *centralWidget_;

    QStackedWidget *stackedWidget_;
    configurationWidget *configuration_;
    sensorSelectionWidget *sensorSelection_;

    QDialogButtonBox *buttons_;
    QPushButton *back_;
    QPushButton *edit_;
    QPushButton *exit_;
    QPushButton *save_;
    QPushButton *refresh_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW
