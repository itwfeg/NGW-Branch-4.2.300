/**
 * @file mainwindow.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QStackedWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QtSql>

class QLabel;
class QToolButton;
class QWidget;

class hteDatabase;

namespace hobart {
class webView;
}

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

    static QSqlError saveConfig(const QString &key, const QString &value);
    static QString loadConfig(const QString &key);

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

    /// Slot for text changed.
    //void onTextChanged(const QString &newText);
    //void onTextEdited(const QString &newText);
    //void onCursorPositionChanged(int oldpos, int newpos);

    /// Slot for button clicked.
    void onButtonClicked();

    /// Slot for Refresh screen data.
    void onProcessRefresh();

private:

    enum style
    {
        StyleNormal = 0,
        StyleCompact,
        StyleFloatingLeft,
        StyleFloatingCenter,
        StyleFloatingRight,
        StyleFloatingOffsetLeft,
        StyleFloatingOffsetRight,
    };


    QMap<QString, QString> params_;

    style style_;

    hteDatabase *htedb_;   ///< HTe Database, tracks list of downloaded files

    QTimer *refreshTimer_;

    QStackedWidget *stack_;
    QWidget *centralWidget_;
    QWidget *secondPageWidget_;
    hobart::webView *webView_;

    QWidget *headerButtonBar_;
    QLabel *logo_;
    QLabel *title_;
    QLabel *txt;

    QToolButton *toggleKeyboard_;
    QToolButton *exit_;

    QWidget *actionButtonBar_;
    QPushButton *submit_;
    QPushButton *performHeartbeat_;
    QPushButton *clearUuid_;

    QLabel *myUuid_;
    QLabel *serverLabel_;
    QLineEdit *serverIp_;
    QLabel *backupServerLabel_;
    QLineEdit *backupServerIp_;
    QLabel *heartbeatEndpointLabel_;
    QLineEdit *heartbeatEndpoint_;

    // Temporary!!
    QLabel *downloadServerLabel_;
    QLineEdit *downloadServerUrl_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Check if floating buttons.
    bool isFloatingButtons() const {return (StyleFloatingLeft <= style_);}

    /// Request store settings.
    bool requestStoreSettings();

    void refreshFields();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW
