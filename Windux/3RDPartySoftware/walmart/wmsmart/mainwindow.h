/**
 * @file mainwindow.h
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>
#include <QMap>
#include <QString>

class QLabel;
class QPushButton;
class QWidget;

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
     * @param[in] params  parameters
     * @param[in] parent  parent object
     */
    mainWindow( const QMap<QString, QString> &params, QWidget *parent = 0 );

    /// Destructor.
    virtual ~mainWindow();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate text.
    virtual void translate();

Q_SIGNALS:

    /// Emitted when input device needs hidden.
    void hideInputDevice();

    /// Emitted when input device needs shown.
    /**
     * @param[in] widget  focus widget
     * @param[in] altLocation  use alternate location, if possible (i.e. top/bottom of screen)
     */
    void showInputDevice( QWidget *widget, bool altLocation = false );

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Enable buttons.
    void enableButtons();

    /// Show central widget.
    void showMainScreen();

    /// Slot for button clicked.
    void onButtonClicked();

private:

    enum {BUTTON_TIMEOUT_PERIOD = 30000}; // 30s

    QMap<QString, QString> params_;

    QWidget *centralWidget_;

    QLabel *logo_;

    QPushButton *fullDatabaseLoad_;
    QPushButton *refreshDatabaseLoad_;

    QPushButton *manualHealthCheck_;
    QPushButton *resetScaleConfig_;

    QPushButton *viewLogs_;
    QPushButton *viewSettings_;
    QPushButton *exit_;

    QMap<QString, QString> hookActionNames_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Enable/disable buttons.
    void enableDisableButtons( bool enabled );

    /// Perform hook action.
    bool performHookAction( const QString& action );

    /// Lookup hook action name from config file.
    QString lookupHookActionName( const QString& key ) const;

    /// Retrieve hook version.
    static int hookVersion();

    /// Perform health check.
    static void performHealthCheck();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW
