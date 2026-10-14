/**
 * @file sessionerrordialog.h
 * Dialog message box for handling session error messages.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SESSIONERRORDIALOG_H
#define SESSIONERRORDIALOG_H

#include "messageboxdialog.h"

class MadpInternalConfig;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog message box for handling session error messages.
class SessionErrorDialog : public MessageBoxDialog
{
    Q_OBJECT

    typedef SessionErrorDialog _Myt;
    typedef MessageBoxDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] msg  session error message
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit SessionErrorDialog( const SessionMessage& msg, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~SessionErrorDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Whether or not clicking the overlay rejects the dialog.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const {return allowDismiss_;}

    /// Check if dialog is valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const {return valid_;}

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Retrieve error message string.
    /**
     * @param[in] code  error code
     * @return  message
     */
    static QString messageString( int code, int isScale );

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Hide event.
    /**
     * @param[in,out] event  hide event
     */
    virtual void hideEvent( QHideEvent *event );

    /// Show event.
    /**
     * @param[in,out] event  show event
     */
    virtual void showEvent( QShowEvent *event );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve priority of this dialog.
    /**
     * When showing a high priority dialog all dialogs of lower priority will be automatically
     * closed.
     *
     * @return  priority level
     */
    virtual int priority() const {return High;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle button click.
    /**
     * @param[in] button  pointer to button clicked
     */
    virtual void onButtonClicked( QAbstractButton *button );

    /// Callback for session error messages.
    /**
     * Callback for session messages.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

    /**
     * Callback for session error messages.
     *
     * @param[in] msg  session error message
     */
    virtual void onSessionMessageReceiveError( const SessionMessage& msg );

    /**
     * Callback for session error lockdown messages.
     *
     * @param[in] isLocked  indicates if we lock the screen
     */
    virtual void onSessionMessageReceiveLockDown( bool isLocked );

private Q_SLOTS:

    /// Handle diagnose button click.
    void onDiagnoseButtonClicked();

    /// Slot to handle hook dialog button press.
    void onHookDialogButtonPress( int role );

    /// Slot to handle session error messages.
    void onSessionMessageReceiveErrorInternal( const SessionMessage& msg );

    /// Slot to handle enabling or disabling the entire mainWindow.
    void onSessionMessageReceiveLockDownInternal( bool isLocked );

    /// Handle watch video button click.
    void onWatchVideoButtonClicked();

//private:
protected:

    SessionMessage msg_;

    bool allowDismiss_;
    bool valid_;

    bool needToSendClosedMessage_;
    bool needToForwardMessage_;
    bool needToKeepDisablingEverything_;

    bool ignoreButtonPressed_;

    QString diagEvent_;
    bool diagEventForced_;

    QString videoHelp_;

    QDialogButtonBox::ButtonRole defaultRole_;
    MadpInternalConfig *internalConfigData_;

    /// Send dialog closed messages.
    void sendClosedMessage( QDialogButtonBox::ButtonRole role );

    /// Retrieve printer type.
    static QString printerType( int code );
    static int& prKind();
    static int getPrKindValue(){return prKind();}

    bool checkForLabelErrors( int code );

    /// Retrieve error message string by event type.
    template <int event>
    static QString messageStringT( int code, int isScale );

private:

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SESSIONERRORDIALOG_H
