/**
 * @file operatorlogindialog.h
 * Dialog for logging into operate mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef OPERATORLOGINDIALOG_H
#define OPERATORLOGINDIALOG_H

#include "textinputboxdialog.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class OperatorLoginDialog
 * @brief Dialog for logging into operate mode.
 */
class OperatorLoginDialog : public TextInputBoxDialog
{
    Q_OBJECT

    Q_ENUMS( LoginType )
    Q_PROPERTY( bool remoteLoginType READ isRemoteLoginType )
    Q_PROPERTY( LoginType loginType READ loginType )

    typedef OperatorLoginDialog _Myt;
    typedef TextInputBoxDialog _Mybase;

public:

    /**
     * Operator login types.
     */
    enum LoginType
    {
        OperatorLoginRequired,                  ///< Prompt for an operator id due to session requirements.
        OperatorLoginRequiredFieldAccess,       ///< Prompt for an operator id due to field access.
        RemoteOperatorIdRequired,               ///< Prompt for an operator id for remote access.
        RemoteUsernamePasswordRequired,         ///< Prompt for a username and password for remote access.
        OperatorLoginRequiredForCutTest,        ///< Prompt for an operator id due to performing a cut test
        OpLoginRequiredForCutTestHistory        ///< Prompt for an operator id due to access rights when viewing history
    };

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] loginType  type of login
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit OperatorLoginDialog( LoginType loginType, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /**
     * Destructor.
     */
    virtual ~OperatorLoginDialog();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Whether or not clicking the overlay rejects the dialog.
     *
     * @returns  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const;

    /**
     * Check if remote login type.
     *
     * @returns  @c true if remote login, @c false otherwise
     */
    virtual bool isRemoteLoginType() const;

    /**
     * Retrieve login type.
     *
     * @returns  type of login
     */
    virtual LoginType loginType() const {return loginType_;}

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve priority of this dialog.
     *
     * When showing a high priority dialog all dialogs of lower priority will be automatically
     * closed.
     */
    virtual int priority() const;

    ///////////////////////////////////////////////////////////////////////////
    // Events

    /**
     * Key press event.
     *
     * @param[in,out] e  key event
     */
    virtual void keyPressEvent( QKeyEvent *e );

    /**
     * Key release event.
     *
     * @param[in,out] e  key event
     */
    virtual void keyReleaseEvent( QKeyEvent *e );

    /**
     * Show event.
     *
     * @param[in,out] e  show event
     */
    virtual void showEvent( QShowEvent *e );

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Handle button click.
     *
     * @param[in] button  pointer to button clicked
     */
    virtual void onButtonClicked( QAbstractButton *button );

    /**
     * Callback for input editing finished.
     */
    virtual void onInputEditingFinished();

    /**
     * Callback for input return pressed.
     */
    virtual void onInputReturnPressed();

    /**
     * Callback for session messages.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

    /**
     * Callback for user name return key pressed.
     */
    virtual void onUserNameReturnKeyPressed();

private Q_SLOTS:

    /**
     * Slot to handle user name return key pressed.
     */
    void onUserNameReturnKeyPressedInternal();

private:

    QLabel *userNameLabel_;
    QLineEdit *userNameLineEdit_;

    QLabel *passwordLabel_;

    LoginType loginType_;
    bool accepted_;

    bool ignoreEvents_;

    bool processAcceptLogin_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Accept login.
     */
    void acceptLogin( const QString& id );

    /**
     * Reject login.
     */
    void rejectLogin();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // OPERATORLOGINDIALOG_H
