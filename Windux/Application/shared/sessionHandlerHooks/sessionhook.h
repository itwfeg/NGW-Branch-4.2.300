/**
 * @file sessionhook.h
 * Session handler hook object.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SESSIONHOOK_H
#define SESSIONHOOK_H

#include "sessionhooks.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class  sessionHook
 * @brief  Session handler hook object.
 */
class sessionHook : public QObject
{
    Q_OBJECT

    friend class sessionHooks;

    typedef sessionHook _Myt;
    typedef QObject _Mybase;

public:

    /// Button map type.
    typedef sessionHooks::buttonMap buttonMap;

    /// Field type.
    typedef sessionHooks::fieldType fieldType;

    /// Field type list.
    typedef sessionHooks::fieldTypeList fieldTypeList;

    /// Field type map.
    typedef sessionHooks::fieldTypeMap fieldTypeMap;

    /// Item type.
    typedef sessionHooks::itemType itemType;

Q_SIGNALS:

    /// Emitted to change a field value.
    /**
     * @param[in] type  field type
     * @param[in] value  field value
     */
    void changeFieldValue( int type, const QString &value );

    /// Emitted when language changed.
    void changeLanguage();

    /// Emitted when the language is reset to the primary language
    void resetPrimaryLanguage();

    /// Emitted when dialog button pressed.
    /**
     * @param[in] role  button role
     */
    void dialogButtonPress( int role );

    /// Emitted when print key pressed.
    void print();

    /// Emitted when softkey clicked.
    /**
     * @param[in] w  pointer to widget emitting action
     * @param[in] b  pointer to button clicked
     * @param[in] action  action
     * @param[in] param  action parameter
     * @param[in] feature  action feature
     */
    void softkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature );

    /// Emitted when we want to dismiss numpads and keypads.
    void shutdownKeyboards();

    /// Emitted when zero scale key pressed.
    void zeroScale();

    /// Emitted when Audit Log key pressed.
    void auditLog();

public:

    // ************************************************************************
    // DTOR
    // ************************************************************************

    /// DTOR
    virtual ~sessionHook();

    // ************************************************************************
    // Properties
    // ************************************************************************

    /// Retrieve field value.
    /**
     * @param[in] type  field type
     * @return  field value
     */
    virtual QString fieldValue( fieldType type ) const;

    /// Retrieve session pointer.
    /**
     * @return  pointer to session
     */
    virtual Session *session() const;

    /// Set field value.
    /**
     * @param[in] type  field type
     * @param[in] value  field value
     */
    virtual void setFieldValue( fieldType type, const QString& value );

public Q_SLOTS:

    /// Emit change field value signal.
    /**
     * @param[in] type  field type
     * @param[in] value  field value
     */
    virtual void emitChangeFieldValue( int type, const QString &value );

    /// Emit change language signal.
    virtual void emitChangeLanguage();

    /// Emit change to primary language signal.
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

    /// Emit signal to shutdown all keypads, numpads, and keyboards
    virtual void emitShutdownKeyboards();

    /// Emit zero scale signal.
    virtual void emitZeroScale();

    /// Emit launch audit log signal.
    virtual void emitAuditLog();

protected:

    // ************************************************************************
    // CTOR
    // ************************************************************************

    /// CTOR.
    /**
     * @param[in] name  hook name
     * @param[in] parent  parent object
     */
    sessionHook( const QString& name, QObject *parent = NULL );

    /// CTOR.
    /**
     * @param[in] parent  parent object
     */
    sessionHook( QObject *parent = NULL );

    // ************************************************************************
    // Properties
    // ************************************************************************

    /// Set session pointer.
    /**
     * @param[in] value  pointer to session
     */
    virtual void setSession( Session *value );

    // ************************************************************************
    // Events
    // ************************************************************************

    /// Printing of current item has begun.
    /**
     * @param[in] filename  image filename
     */
    virtual void onBeginPrinting( const QString& ) {}

    /// Printing of current item has begun.
    /**
     * @param[in] filename  image filename
     */
    virtual void onBeginBarcodeOnlyPrinting( const QString& ) {}

    /// Dismiss active dialog.
    virtual void onDismissDialog() {}

    /// Printing of current item has ended.
    virtual void onEndPrinting() {}

    /// Load softkey group.
    /**
     * @param[in] m  pointer to message
     */
    virtual void onLoadSoftkeyGroup( const DomLoadSoftkeyGroup* /*m*/ ) {}

    /// Load current state.
    /**
     * @param[in] none
     */
    virtual void onLoadCurrentState() {}

    /**
     * @brief  Load start state after operator logs in.
     * @param[in] none
     */
    virtual void onUpdateStartState() {}
    
    /// Updates current weights and measures audit log
    /**
     * @param[in] m pointer to message
     */
    virtual void onUpdateAuditLogList( const DomAuditLogInformation* /*m*/ ) {}

    /// updates the screen when we are finished saving the augit log file
    /**
     * @param[in]
     */
    virtual void onUpdateSaveMessage() {}

    /// Network connectivity message.
    /**
     * @param[in] m  pointer to message
     */
    virtual void onNetworkConnectivity( const MadpNetworkConnectivity* /*m*/ ) {}

    /// Network device type message.
    /**
     * @param[in] m  pointer to message
     */
    virtual void onNetworkDeviceType( const MadpNetworkDeviceType* /*m*/ ) {}

    /// Remove camera predictions dialog.
    virtual void onRemoveCameraPredictionsDialog() {}

    /// Show dialog on screen.
    /**
     * @param[in] title  dialog title
     * @param[in] message  dialog message
     * @param[in] buttons  dialog buttons
     */
    virtual void onShowDialog( const QString& /*title*/, const QString& /*message*/, const buttonMap& /*buttons*/ ) {}

    /// Show camera predictions dialog on screen.
    /**
     * @param[in] m  predictions
     */
    virtual void onShowCameraPredictionsDialog( const DomLoadSoftkeyGroup* /*m*/ ) {}

    /// Update current item.
    /**
     * @param[in] type  item type
     */
    virtual void onUpdateCurrentItem( itemType /*type*/ ) {}

    /// Update current language.
    /**
     * @param[in] language  new language
     */
    virtual void onUpdateCurrentLanguage( const QString& /*language*/ ) {}

    /// Update current playlist.
    /**
     * @param[in] language  new playlist
     */
    virtual void onUpdateCurrentPlaylist( const QString& /*playlist*/ ) {}

    /**
     * @brief Update current self service dialog text.
     * @param[in] code  error code
     */
    virtual void onUpdateCurrentErrDialog( const QString& /*error code*/, int /* button style */ ) {}

    /**
     * @brief Dismiss current self service dialog text.
     * @param[in] code  error code
     */
    virtual void onDismissCurrentErrDialog( const QString& /*error code*/, int /* button style */  ) {}
    /// Update field value.
    /**
     * @param[in] type  field type
     * @param[in] value  field value
     */
    virtual void onUpdateFieldValue( fieldType /*type*/, const QString& /*value*/ ) {}

    /// Update the session pointer.
    /**
     * @param[in] value  pointer to session
     */
    virtual void onUpdateSession( Session* /*value*/ ) {}

    // ************************************************************************
    // Methods
    // ************************************************************************

    /// Clear all item field values.
    virtual void clearItemFieldValues();

private:

    // not implemented
    sessionHook( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

    Session *session_;
    fieldTypeMap fields_;

    fieldTypeList itemFields_;

    /// Initialize.
    void initialize();

    /// Set current item.
    void setCurrentItem( itemType type );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SESSIONHOOK_H
