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
#include <QNetworkReply>

class QLabel;
class QToolButton;
class QWidget;

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

    // returns address_ member variable
    QString address(){ return address_; }
    
    static QSqlError saveConfig(const QString &key, const QString &value);
    static QString loadConfig(const QString &key);
    static void writeTag(const QString &tag, const QString &value);

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

    /// Slot for button clicked.
    void onButtonClicked();

    /// Slot for date/time changed.
    void onDateTimeChanged(const QDateTime &dt);

public Q_SLOTS:
    /// Populate zone selection box with available zones (callback)
    void populateZoneBox(QNetworkReply* reply);


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

    int store_;
    int dept_;
    QString address_;

    QStackedWidget *stack_;
    QWidget *centralWidget_;
    QWidget *secondPageWidget_;
    hobart::webView *webView_;

    QWidget *buttonBar_;
    QLabel *logo_;
    QLabel *title_;
    QLabel *txt;
    QLabel *zoneLabel;

    QToolButton *toggleKeyboard_;
    QToolButton *refresh_;
    QToolButton *minimize_;
    QToolButton *exit_;
    QToolButton *toggleConfig_;

    QPushButton *export_;
    QPushButton *backup_;
    QPushButton *restore_;
    QLineEdit *serverIp_;
    QComboBox *scaleZone_;
    QPushButton *statistics_;
    QPushButton *submit_;
    QPushButton *submitZone_;
    QPushButton *upload_;
    QPushButton *download_;
    QPushButton *connectionLogs_;

    QComboBox *reportSelector_;

    QDateTimeEdit *startDTE_;
    QDateTimeEdit *endDTE_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Check if floating buttons.
    bool isFloatingButtons() const {return (StyleFloatingLeft <= style_);}

    /// Request store settings.
    bool requestStoreSettings();

    /// Check if end date is before start date; if so, return false
    bool isDateOrderCorrect();

    /// make request for list of zones
    void requestZones(void);

    /// calculate number of offline minutes from OO connectivity log
    int countOfflineMinutes(void);

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // MAINWINDOW
