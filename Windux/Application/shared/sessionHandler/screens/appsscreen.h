/**
 * @file apps.h
 * Third party applications.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef APPS_H
#define APPS_H

#include "screenwidget.h"

#include <QDateTime>
#include <QMap>
#include <QString>

class QAbstractButton;
class QProcess;
class QSignalMapper;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Third party applications.
class AppsScreen : public ScreenWidget
{
    Q_OBJECT

    typedef AppsScreen _Myt;
    typedef ScreenWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit AppsScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~AppsScreen();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve installed applications.
    /**
     * @return  list of installed apps
     */
    virtual QStringList apps() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Launch application.
    /**
     * @param[in] app  app name
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool launchApp( const QString &name );

    /// Translate the user interface to current language.
    virtual void translate();

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Check if external applications are installed.
    /**
     * @return  @c true if apps installed, @c false otherwise
     */
    static bool isExternalAppInstalled();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Screen that gets displayed when swiping (gesturing) the screen up.
    /**
     * @return  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenUp() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Load application icons.
    virtual void loadAppIcons();

    /// Handle screen changing.
    /**
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @return  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /// Refresh (redraw) application icons.
    virtual void refreshAppIcons();

private Q_SLOTS:

    /// Slot for button clicked.
    void onButtonClicked( QWidget *button );

    /// Slot for process finished.
    void onProcFinished( int exitCode );

    /// Slot for screen changed.
    void onScreenChanged();

private:

    enum {LOGIN_PROMPT_DELAY = 5000}; // 5s
    enum {LOGIN_PROMPT_TIME = 30000}; // 30s

    enum {LOGOUT_DELAY = 5000}; // 5s

    typedef QMap<QString, QString> settings;

    typedef QMap<QAbstractButton*, QString> applicationMap;
    typedef QMap<QString, settings> applicationSettingsMap;

    applicationMap apps_;
    applicationSettingsMap appSettings_;

    QSignalMapper *clickedSignalMapper_;

    QDateTime promptLoginDateTime_;
    bool promptLoginActive_;
    settings promptLoginSettings_;

    QDateTime operLogoutDateTime_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Factory method to create application icon.
    QAbstractButton *createAppIcon( const QString &conf, settings &s );

    /// Launch application.
    bool launchAppFromSettings( const settings &s );

    /// Find and maximize window.
    bool maximizeWindow( const QString &title );

    /// Validate operator access level.
    bool validateAccess( const settings &s, int requiredAccessLevel );

    /// Parse a conf file.
    static settings parseConf( const QString &conf );

    /// Check for valid settings.
    static bool isSettingsValid( const settings &s );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // APPS_H
