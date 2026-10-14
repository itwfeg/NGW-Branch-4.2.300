/**
 * @file modewidget.h
 * Base class for all mode widget objects.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef MODEWIDGET_H
#define MODEWIDGET_H

#include <Messages.h>
#include <Session.h>

#include <QList>
#include <QMargins>
#include <QWidget>

class SessionMainWindow;
class SessionDialog;

class ScreenWidget;
class PanelWidget;

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class ModeWidget
 * @brief Base class for all mode widget objects.
 */
class ModeWidget : public QWidget
{
    Q_OBJECT

    friend class SessionMainWindow;

    Q_ENUMS( Mode )
    Q_PROPERTY( bool loggedIn READ loggedIn )
    Q_PROPERTY( int operatorAccessLevel READ operatorAccessLevel )
    Q_PROPERTY( QString operatorID READ operatorID )
    Q_PROPERTY( QString operatorName READ operatorName )
    Q_PROPERTY( Mode mode READ mode )
    Q_PROPERTY( QMargins margins READ margins )

    typedef ModeWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /**
     * Emitted when the screen changes.
     */
    void screenChanged();

    /**
     * Emitted when the margins change.
     */
    void marginsChanged();

    /**
     * Emitted when an operator logs in.
     */
    void operatorLoggedIn();

    /**
     * Emitted when an operator is logged out.
     */
    void operatorLoggedOut();

    /**
     * Emitted when usb key inserted.
     */
    void usbKeyInserted();

    /**
     * Emitted when usb key removed.
     */
    void usbKeyRemoved();

public:

    /**
     * Scale modes.
     */
    enum Mode
    {
        Operate         = 1,                    ///< Operator mode.
        Supervisor      = 2,                    ///< Supervisor mode.
        Service         = 3                     ///< Service mode.
    };

    ///////////////////////////////////////////////////////////////////////////
    // DTOR

    /**
     * Destructor.
     */
    virtual ~ModeWidget();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve usb key activation states.
     *
     * @returns  key states
     */
    virtual MadpKeyActivation usbKeyActivation() const {return cachedUsbKeyActivation_;}

    /**
     * Check for cached screen.
     *
     * @returns  pointer to screen if cached, @c NULL otherwise
     *
     * @tparam Screen  screen type
     */
    template <class Screen>
    Screen *isCached() const;

    /**
     * Check for cached screen.
     *
     * @param[in] screen  screen to check
     * @returns  pointer to @p screen if cached, @c NULL otherwise
     */
    virtual ScreenWidget *isCached( ScreenWidget *screen ) const;

    /**
     * Check if operator has logged into the mode.
     *
     * @returns  @c true if logged in, @c false otherwise
     */
    virtual bool loggedIn() const;

    /**
     * Retrieve margins.
     *
     * Some screens add widgets to the top and/or bottom. This property returns how much space
     * the widgets are taking up. This property is useful because we may want to offset the
     * numeric keypad or other widgets.
     *
     * @returns  margins
     */
    virtual QMargins margins() const;

    /**
      * Retrieve scale mode.
      *
      * @returns  mode
      */
    virtual Mode mode() const {return scaleMode_;}

    /**
     * Retrieve operator access level.
     *
     * @returns  access level
     */
    virtual int operatorAccessLevel() const;

    /**
     * Retrieve currently logged in operator id.
     *
     * @returns  id of operator
     */
    virtual QString operatorID() const;

    /**
     * Retrieve currently logged in operator name.
     *
     * @returns  name of operator
     */
    virtual QString operatorName() const;

    /**
     * Retrieve the current screen.
     *
     * @returns  screen widget
     */
    virtual ScreenWidget *screen() const {return screen_;}

    /**
     * Set the font family for this mode.
     *
     * @param[in] value  font name
     */
    virtual void setFontFamily( const QString& value );

    /**
     * Set the mode screen.
     *
     * @param[in] screen  screen to set
     */
    virtual void setScreen( ScreenWidget *screen );

    /**
     * Retrieve current session operation mode.
     *
     * This method will return what mode session is in. This mode may be different than what mode
     * the session main window it in.
     *
     * @returns  mode
     */
    virtual OpModeEvent operationMode();

    /**
     * Set session operation mode.
     *
     * This method will set session operation mode.
     *
     * @param[in] value  mode
     */
    virtual void setOperationMode( OpModeEvent value );

    /**
     * Check if any operators exist in the database.
     *
     * @returns  @c true if operators exist, @c false otherwise
     */
    virtual bool operatorsExist() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Activate the mode.
     *
     * This method is called whenever the mode is set active. Most modes will send a
     * @c SnBrowserMenuChanging message when this method is called.
     */
    virtual void activate();

    /**
     * Create cached screens.
     */
    virtual void createCachedScreens();

    /**
     * Create a menu panel object.
     *
     * @param[in] panelEx  extended panel type
     * @param[in] parent  panel parent dialog
     * @returns  pointer to panel
     */
    virtual PanelWidget *createMenuPanel( int panelEx, SessionDialog *parent );

    /**
     * Create a screen object.
     *
     * @returns  pointer to screen
     *
     * @tparam Screen  screen type
     */
    template <class Screen>
    ScreenWidget *createScreen();

    /**
     * Login operator to mode.
     *
     * @param[in] id  operator id
     */
    virtual void login( const QString& id );

    /**
     * Logout active operator.
     */
    virtual void logout();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    SessionMainWindow *mainWindow_;             ///< Pointer to the main window.
    ScreenWidget *screen_;                      ///< Pointer to the screen.

    Session *session_;                          ///< Pointer to the session.

    ///////////////////////////////////////////////////////////////////////////
    // CTOR

    /**
     * Constructor.
     *
     * @param[in] screen  screen to set
     * @param[in] scaleMode  scale mode (i.e. 1 = operator, 2 = supervisor, 3 = service)
     * @param[in] parent  parent @c SessionMainWindow widget
     */
    explicit ModeWidget( ScreenWidget *screen, Mode scaleMode, SessionMainWindow *parent = 0 );

    /**
     * Constructor.
     *
     * @param[in] scaleMode  scale mode (i.e. 1 = operator, 2 = supervisor, 3 = service)
     * @param[in] parent  parent @c SessionMainWindow widget
     */
    explicit ModeWidget( Mode scaleMode, SessionMainWindow *parent = 0 );

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check for specific error message is handled by mode.
     *
     * Session will call this method upon receiving an error message. The default error handling
     * (i.e. popup an error dialog) shall be invoked unless @c true is returned from
     * this property.
     *
     * @returns  @c true to use custom error handler, @c false otherwise
     */
    virtual bool isErrorMessageHandler( const SessionMessage& errorMsg ) const;

    /**
     * Check if logged in via the usb key.
     *
     * @returns  @c true if usb key present, @c false otherwise
     */
    virtual bool isKeyPresent() const {return false;}

    /**
     * Check if supervisor token present.
     *
     * @returns  @c true if token present, @c false otherwise
     */
    virtual bool isSupervisorTokenPresent() const {return supervisorTokenPresent_;}

    /**
     * Check if service token present.
     *
     * @returns  @c true if token present, @c false otherwise
     */
    virtual bool isServiceTokenPresent() const {return serviceTokenPresent_;}

    /**
     * Retrieve overlay margins.
     *
     * Some modes contain overlays that take up the top or bottom of the screen. This property
     * returns how much room those overlays take up.
     *
     * @returns  overlay margins
     */
    virtual QMargins overlayMargins() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Cache a pre-loaded screen widget.
     *
     * @param[in] screen  pointer to screen
     */
    virtual void cache( ScreenWidget *screen );

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /**
     * Slot to handle screen destroyed.
     */
    void onScreenDestroyed( QObject *obj );

    /**
     * Slot to handle session message.
     */
    void onSessionMessageReceive( const SessionMessage& msg );

    /**
     * Slot to handle token expiration.
     */
    void onTokenExpired();

private:

    typedef QMap<Session*, OpModeEvent> sessionModeMap;

    Mode scaleMode_;
    int currentWeighMode_;

    bool loggedIn_;
    DomOperatorLoginData operatorLoginData_;

    bool loggingOutOfMode_;

    bool usbKeyPresent_;
    MadpKeyActivation cachedUsbKeyActivation_;

    bool supervisorTokenPresent_;
    bool serviceTokenPresent_;
    QTimer *expireTokenTimer_;

    QList<ScreenWidget*> cache_;

    static sessionModeMap modeMap_;

    /**
     * Initialize.
     */
    void initialize( ScreenWidget *screen );

    /**
     * Check if mode uses cached key activations.
     */
    bool modeUsesCachedKeyActivation( int mode ) const;

    /**
     * Return pass code based on the quarter of the year
     * that the current date falls in.
     */
    QString getPassKey();
};

template <class Screen>
Screen *ModeWidget::isCached() const
{
    // iterate cache
    foreach ( const ScreenWidget *cached, cache_ )
    {
        const Screen *screen = NULL;

        // check cached screen
        if ( (screen = dynamic_cast<const Screen*>( cached )) )
            return const_cast<Screen*>( screen );
    }

    return NULL;
}

template <class Screen>
ScreenWidget *ModeWidget::createScreen()
{
    // look for screen in cache
    ScreenWidget *screen( isCached<Screen>() );

    // create screen if not found
    if ( !screen )
    {
        screen = new Screen( mainWindow_, this );
        connect( screen, &Screen::destroyed, this, &ModeWidget::onScreenDestroyed );
    }

    return screen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MODEWIDGET_H
