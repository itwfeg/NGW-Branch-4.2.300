/**
 * @file selfservicehook.h
 * Self service hook.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICEHOOK_H
#define SELFSERVICEHOOK_H

#include "sessionhook.h"

#include <QList>
#include <QMultiMap>
#include <QMutex>
#include <QPixmap>

class selfServiceDialog;
class selfServiceDialogG2;
class ScaleConfiguration;

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service hook.
class selfServiceHook : public sessionHook
{
    Q_OBJECT

    Q_ENUMS( state )
    Q_PROPERTY( bool active READ active NOTIFY activeChanged )
    Q_PROPERTY( state currentState READ currentState WRITE setCurrentState NOTIFY currentStateChanged )
    Q_PROPERTY( state previousState READ previousState )
    Q_PROPERTY( QPixmap storeLogo READ storeLogo )
    Q_PROPERTY( QPixmap pullLabel READ pullLabel )

    typedef selfServiceHook _Myt;
    typedef sessionHook _Mybase;

Q_SIGNALS:

    /// Emitted when active is changed.
    void activeChanged();

    /// Emitted when user enters selfserv to enable manual screen again
    void receiveLockDownScreenMessage( bool isLocked );

    /// Emitted when the look of the current state is changed
    void currentLookOfStateChanged();

    /// Emitted when current state is changed.
    void currentStateChanged();

    /// Emitted when images change.
    void reloadImages();

    /// Emitted when the screen needs updated after saving an audit log file
    void emitUpdateSaveMessage();
    
    /// Emitted when the user clicks to see the Audit Log Screen
    void emitUpdateAuditLogList();

public:
    /// Hook states.
    enum state
    {
        // dialogs
        weightsMeasuresDialog,                      ///< 0 Weights and measures dialog.
        auditLogDialog,                             ///<   Audit Log dialog

        // screens
        attract,                                    ///<   Attract screen.
        itemEntryKeyed,                             ///<   Key in item screen.
        itemEntryImages,                            ///<   Item flashkey screen.
        processRandomWeightItem,                    ///< 5 Random weight item.
        processByCountItem,                         ///<   By count item.
        takeLabel,                                  ///<   Take label.

        // Gen 2 dialogs
        weightsMeasuresDialogGen2,
        auditLogDialogGen2,

        // Gen 2 screens
        attractGen2,                                ///< 10
        itemEntryKeyedGen2,                         ///< Main Flashkey and PLU Entry Lookup Screen
        itemEntryImagesGen2,                        ///< Search Char by Char
        itemEntryImagesViewAllGen2,                 ///< Search A-Z Cats
        processRandomWeightItemGen2,                ///< 15
        processByCountItemGen2,                     ///< 16

        // meta states
        // these are not "real" states, they map to something above
        previous,                                   ///< 17 Previous state.
        startScreen                                 ///< 18 Start screen (item entry).
    };

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /// CTOR.
    /**
     * @param[in] s  pointer to session
     * @param[in] parent  parent object
     */
    selfServiceHook( Session *s, QObject *parent = NULL );

    /// DTOR
    virtual ~selfServiceHook();

    // ************************************************************************
    // Properties
    // ************************************************************************

    /// Check if hook active/visible.
    /**
     * @return  @c true if hook is active and visible, @c false otherwise
     */
    virtual bool active() const {return active_;}

    /// Check if hook should be active on startup.
    /**
     * @return  @c true if should be active, @c false otherwise
     */
    virtual bool activeOnStartup() const;

    /// Retrieve current language.
    /**
     * @return  current language
     */
    virtual QString currentLanguage() const;

    /// Retrieve current state.
    /**
     * @return  current state
     */
    virtual state currentState() const;

    /// Retrieve previous state.
    /**
     * @return  previous state
     */
    virtual state previousState() const;

    /// Retrieve store logo.
    /**
     * @return  pixmap
     */
    virtual QPixmap storeLogo() const {return storeLogo_;}

    /// Retrieve pull label image instruction.
    /**
     * @return  pixmap
     */
    virtual QPixmap pullLabel() const {return pullLabel_;}

    /**
     * @brief Display the hidden dialog.
     * @param[in] nil
     */
    virtual void onShowErrDialog();

    /**
     * @brief Hide current dialog to bring back later.
     * @param[in] nil
     */
    virtual void onHideErrDialog();

    bool isLoginScreen();


public Q_SLOTS:

    /// Emit change language signal.
    virtual void emitChangeLanguage();

    /// Emit primary language signal.
    virtual void emitResetPrimaryLanguage();

    /// Emit dialog button press signal.
    /**
     * @param[in] role  button role
     */
    virtual void emitDialogButtonPress( int role );

    /// Emit print key pressed signal.
    virtual void emitPrint();

    /// Emit softkey clicked signal.
    /**
     * @param[in] w  pointer to widget emitting action
     * @param[in] b  pointer to button clicked
     * @param[in] action  action
     * @param[in] param  action parameter
     * @param[in] feature  action feature
     */
    virtual void emitSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature );

    /// Emit kill keyboard signal.
    virtual void emitShutdownKeyboards();

    /// Emit zero scale signal.
    virtual void emitZeroScale();

    /// Emit redisplaying bycount screen to Start Over
    virtual void resetByCountScreen();

    /// Enit showing the back button in menu bar
    virtual void resetBackButton();

    /// what is the stack state of the keypad (keypad=0, scan/print = 1)
    virtual int getKeypadStackState();

    /// what is the error code of current dialog - if any
    virtual QString getErrorStateDialogCode();

    /// what is the style code of current dialog - if any
    virtual int getErrorStateDialogStyle();

    /// launch the Audit Log Dialog
    virtual void emitViewAuditLog();

    /// Enter into self service mode.
    virtual void enter();

    /// Leave self service mode.
    virtual void leave();

    /// Reset the idle timer.
    virtual void resetIdleTimer();

    /// send a message for me for those who cant
    virtual void sendSurrogateMsg(int device, int event);

    /// send a message for me for those who cant
    virtual void sendSurrogateMsg( int device, int event, const QString& msgBody );

    /// Set current state.
    /**
     * @param[in] value  new state
     */
    virtual void setCurrentState( state value );

    /// Set the previous state. Currently used by Audit Log to get things back on track
    /**
     * @return  Nothing
     */
    virtual void setPreviousState( state value );



protected:

    // ************************************************************************
    // Properties
    // ************************************************************************

    /// Check if state is dialog.
    /**
     * @param[in] value  state to check
     * @return  @c true if dialog, @c false otherwise
     */
    virtual bool isDialogState( state value ) const;

    /// Check if error displayed.
    /**
     * @return  @c true when error on screen, @c false otherwise
     */
    virtual bool isErrorState() const;

    /// Translate state to "real" state.
    /**
     * @param[in] value  state to translate
     * @return  state
     */
    virtual state mappedState( state value ) const;

    /// Retrieve the start screen state.
    /**
     * @return  start screen state
     */
    virtual state startScreenState() const;

    // ************************************************************************
    // Events
    // ************************************************************************

    /// Printing of current item has begun.
    /**
     * @param[in] filename  image filename
     */
    virtual void onBeginPrinting( const QString &filename );

    /// Printing of current item has begun.
    /**
     * @param[in] filename  image filename
     */
    virtual void onBeginBarcodeOnlyPrinting( const QString &filename );

    /// Dismiss active dialog.
    virtual void onDismissDialog();

    /// Printing of current item has ended.
    virtual void onEndPrinting();

    /// Load softkey group.
    /**
     * @param[in] m  pointer to message
     */
    virtual void onLoadSoftkeyGroup( const DomLoadSoftkeyGroup* m );

    /// Load current state window.
    /**
     * @param[in] none
     */
    virtual void onLoadCurrentState();

    /**
     * @brief  Load current start state.
     * @param[in] none
     */
    virtual void onUpdateStartState();
    
    /**
     * @param[in] none
     */
    virtual void onUpdateSaveMessage();

    /// updates current weights and measures audit log
    /**
     * @param[in] m pointer to message
     */
    virtual void onUpdateAuditLogList( const DomAuditLogInformation *m );

    /// Network connectivity message.
    /**
     * @param[in] m  pointer to message
     */
    virtual void onNetworkConnectivity( const MadpNetworkConnectivity *m );

    /// Network device type message.
    /**
     * @param[in] m  pointer to message
     */
    virtual void onNetworkDeviceType( const MadpNetworkDeviceType *m );

    /// Remove camera predictions dialog.
    virtual void onRemoveCameraPredictionsDialog();

    /// Show dialog on screen.
    /**
     * @param[in] title  dialog title
     * @param[in] message  dialog message
     * @param[in] buttons  dialog buttons
     */
    virtual void onShowDialog( const QString& title, const QString& message, const buttonMap& buttons );

    /// Show camera predictions dialog on screen.
    /**
     * @param[in] m  predictions
     */
    virtual void onShowCameraPredictionsDialog( const DomLoadSoftkeyGroup *m );

    /// Update current item.
    /**
     * @param[in] type  item type
     */
    virtual void onUpdateCurrentItem( itemType type );

    /// Update current language.
    /**
     * @param[in] language  new language
     */
    virtual void onUpdateCurrentLanguage( const QString& language );

    /// Update current playlist.
    /**
     * @param[in] playlist  new playlist
     */
    virtual void onUpdateCurrentPlaylist( const QString& playlist );

    /**
     * @brief Update current dialog text.
     * @param[in] code  error code
     */
    virtual void onUpdateCurrentErrDialog(const QString& code , int buttonstyle);

    /**
     * @brief Dismiss current dialog text.
     * @param[in] code  error code
     */
    virtual void onDismissCurrentErrDialog( const QString& code, int buttonstyle );

    /**
     * @brief Dismiss current dialog text without knowing the code first.
     * @param[in] nil
     */
    virtual void onDismissCurrentErrDialogWithAnyCode();

    /// Update field value.
    /**
     * @param[in] type  field type
     * @param[in] value  field value
     */
    virtual void onUpdateFieldValue( fieldType type, const QString& value );

    // ************************************************************************
    // Methods
    // ************************************************************************

    /// Event filter.
    /**
     * @param[in] obj  object pointer
     * @param[in] e  event pointer
     * @return  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /// Load store logo from filesystem.
    virtual void loadStoreLogo();

    /// Load pull label image from filesystem.
    virtual void loadPullLabel();
    
private Q_SLOTS:

    /// Slot to handle object destruction.
    void onDestroyed( QObject *obj );

    /// Slot for file monitor timer timeout.
    void onFileMonitorTimerTimeout();

    /// Slot for idle timer timeout.
    void onIdleTimerTimeout();

    /// Slot for product search requested.
    void onProductSearchRequested();

    void raiseTimeout();


private:

    enum ScaleZeroResult
    {
        SZ_Unknown,
        SZ_Yes,
        SZ_No
    };

    bool isScaleOnline_;
    bool active_;
    int timeoutT_;
    QString language_;
    QString playlist_;
    QString code_;
    int style_;

    state currentState_;
    state prevState_;

    QPixmap storeLogo_;
    QPixmap pullLabel_;

    selfServiceDialog   *d_;
    selfServiceDialogG2 *d_G2_;
    ScaleConfiguration  *scaleConfig_;

    QWidget *error_;
    QWidget *cameraPredictions_;
    mutable QMutex errorMutex_;

    typedef QMultiMap<state, state> stateChangesMap;
    stateChangesMap allowedStateChanges_;

    typedef QList<state> stateList;
    stateList dialogStates_;

    QTimer *fileMonitorTimer_;
    QTimer *idleTimer_;

    typedef QMap<QString, QByteArray> fileHashMap;
    fileHashMap fileHashTable_;

    static const QSize storeLogoSize_;

    static const QSize pullLabelSize_;

    // not implemented
    selfServiceHook( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    /// Initialize object.
    void initialize();

    /// Change look of the current state
    void changeLookOfCurrentState( state value, bool force = false );

    /// Change the current state.
    void changeCurrentState( state value, bool force = false );

    /// Set the current state
    void changePreviousState( state value );

    /// Check for scale zero.
    ScaleZeroResult isScaleZero() const;

    /// Generate hash of file.
    static QByteArray generateFileHash( const QString& filename );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SELFSERVICEHOOK_H
