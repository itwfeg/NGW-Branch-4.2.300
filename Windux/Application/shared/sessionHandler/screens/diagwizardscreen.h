/**
 * @file diagwizardscreen.h
 * Diagnostics wizard screen.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef DIAGWIZARDSCREEN_H
#define DIAGWIZARDSCREEN_H

#include "screenwidget.h"

class DiagShowWizardMsg;

class QLabel;
class QPushButton;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics wizard screen.
class DiagnosticsWizardScreen : public ScreenWidget
{
    Q_OBJECT

    typedef DiagnosticsWizardScreen _Myt;
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
    explicit DiagnosticsWizardScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~DiagnosticsWizardScreen();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Show wizard.
    /**
     * @param[in] msg  show wizard message
     */
    virtual void showWizard( const DiagShowWizardMsg &msg );

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    QString errorInfo_event_;
    QString errorInfo_diagEvent;
    int errorInfo_diagErrorCode;

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

    /// Set error info.
    /**
     * @param[in] msg  show wizard message
     */
    virtual void setErrorInfo( const DiagShowWizardMsg &msg );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle screen changing.
    /**
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for calibrate wrapper.
    void onCalibrateWrapper();

    /// Slot for button clicked.
    void onClicked();

    /// Slot for screen changed.
    void onScreenChanged();

    /// Slot for timer timeout.
    void onTimeout();

private:

    enum state
    {
        Unknown,
        Init,
        CancelledInit,
        Ready,
        Testing,
        CancelledTesting,
        Complete
    };

    state state_;
    int failureCode_;

    QLabel *title_;
    QLabel *hardwareModel_;
    QLabel *hardwareSerial_;
    QLabel *hardwareCycles_;

    QLabel *errorCode_;
    QLabel *errorCodeNum_;
    QLabel *description_;
    QLabel *stamp_;

    QLabel *photo_;

    QLabel *message_;
    QPushButton *next_;
    QPushButton *home_;
    QPushButton *calibrate_;
    QPushButton *healthAgent_;

    QTimer *wizardInitTimer_;
    QTimer *wizardTestingTimer_;

    QTimer *animationTimer_;

    QString serviceContactInfo_;

    QVector<QPixmap> diagPhotos_;
    int diagPhotoIndex_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Refresh error info.
    void refreshErrorInfo();

    /// Refresh photo.
    void refreshPhoto();

    /// Process error specific events.
    void processErrorSpecificEvents();

    /// Retrieve testing passed text.
    QString testingPassedText();

    /// Retrieve testing failed text.
    QString testingFailedText( int failureCode );

    /// Show and enable health agent button.
    void showHealthAgentButton();

    bool isTwoPrinterPhoto( int code );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // DIAGWIZARDSCREEN_H
