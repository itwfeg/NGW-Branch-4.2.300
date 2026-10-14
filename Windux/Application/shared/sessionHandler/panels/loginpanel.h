/**
 * @file loginpanel.h
 * Panel for logging into modes.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LOGINPANEL_H
#define LOGINPANEL_H

#include "modepanel.h"

class QGroupBox;
class QLabel;
class QLineEdit;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for logging into modes.
class LoginPanel : public ModePanel
{
    Q_OBJECT

    typedef LoginPanel _Myt;
    typedef ModePanel _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /**
     * Constructor.
     *
     * @param[in] mode  pointer to the mode object
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit LoginPanel( ModeWidget *mode, SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~LoginPanel();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Allow service key only.
    virtual void allowServiceKeyOnly();

    /// Allow login or service key.
    virtual void allowLoginOrServiceKey();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Show event.
    /**
     * @param[in] event  show event
     */
    virtual void showEvent( QShowEvent *event );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve renderer traits.
    /**
     * @return  traits object
     */
    virtual hobart::backgroundWidgetBase *rendererTraits() const;

private Q_SLOTS:

    /// Slot for login button clicked.
    void onLoginButtonClicked();

    /// Slot for operator logged in.
    void onOperatorLoggedIn();

    /// Slot to handle session message.
    void onSessionMessageReceive( const SessionMessage& msg );

    /// Slot to handle timer timeout.
    void onTimeout();

private:

    ModeWidget *loginMode_;

    QLabel *title_;
    QGroupBox *loginGroupBox_;
    QLineEdit *loginId_;
    QLabel *scaleVersion_;

    QTimer *timer_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // LOGINPANEL_H
