/**
 * @file upgradeKPCSoftware.h
 * @brief Upgrades KPC Software and displays progress.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author: Job E Crew
 */

#ifndef UPGRADEKPCSOFTWARE_H
#define UPGRADEKPCSOFTWARE_H

#include "screenwidget.h"

#include <QVector>

class QLabel;
class QProgressBar;
class QPushButton;
class QTimer;

class MadpScaleConfig;
class MadpInternalConfig;

///////////////////////////////////////////////////////////////////////////////////////////////////

namespace Ui {
class upgradeKPCSoftware;
}

/// Upgrades next generation wrapper and displays progress.
class upgradeKPCSoftware  : public ScreenWidget
{
    Q_OBJECT

    typedef upgradeKPCSoftware _Myt;
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
    explicit upgradeKPCSoftware(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /// Destructor.
    virtual ~upgradeKPCSoftware();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

    /// Screen states.
    enum state
    {
        Initial,
        UpgradeRequested,
        UpgradeInProgress,
        UpgradeComplete,
        CancelRequested
    };

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

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

    /// Update state.
    /**
     * @param[in] s  new state
     */
    virtual void updateState( state s );

    /// Update node states.
    virtual void updateNodeStates();

    /// Update progress.
    virtual void updateProgressState();

private Q_SLOTS:

    /// Slot for push button clicked.
    void onPushButtonClicked();

    /// Slot for screen changed.
    void onScreenChanged();

    /// Slot for updating node states.
    void onUpdateNodeStates();

    /// Slot for updating progress.
    void onUpdateProgressState();

    /// Slot for testing.
    void onTest();

    /// Slot for kicking off the upgrade
    void onStartUpgrade();

    /// Slot for kicking ending the upgrade
    void onFinishedUpgrade();

    /// Slot for when cancel timer runs out
    void onCancelTimeout();

    /// Slot for keeping track of the cancel timer
    void updateCancelTime();

private:


    enum nodeState
    {
        Pending = 0,
        Upgrading,
        Complete,

        ErrorNone,                          // NOFAULT,                 // ErrorNone = 100,
        ErrorTimeout,                       // TIMEOUT                  // ErrorNoResponseFromNode,
        ErrorWriteFailure,                  // WRITE_FAILURE            // ErrorNodeTimeout,
        ErrorChecksumFailure,               // CHECKSUM                 // ErrorNodeWriteFailure,
        ErrorUnknownRecordType,             // UNKNOWN_RECORD_TYPE      // ErrorNodeChecksumFailure,
        ErrorEraseAppFailure,               // ERASE_FAILURE_APP        // ErrorNodeEraseBank1Failure,
        ErrorBootFailure,                   // ERASE_FAILURE_BOOT,      // ErrorNodeEraseBank2Failure,
        ErrorComponentNotSet,               // COMPONENT_NOT_SET        // ErrorNodeIdFailure,
        ErrorImageMismatch,                 // IMAGE_MISMATCH           // ErrorNodeWriteAddressOutOfRange
        ErrorImageChecksumMismatch,         // IMAGE_CHECKSUM_MISMATCH,
        ErrorModelTypeError,                // MODEL_TYPE_ERROR,
        ErrorAddressOutOfRange,             // WRITE_ADDRESS_OUT_OF_RANGE,
        ErrorFrameOutOfRange                // FRAME_OUT_OF_RANGE
    };

    Ui::upgradeKPCSoftware *ui;

    MadpInternalConfig *internalConfigData;

    state state_;
    nodeState nodeinfo_state_;
    int nodeinfo_percentComplete_;

    QLabel *nodeinfo_description_;
    QLabel *nodeinfo_version_;

    QLabel *title_;
    QLabel *message_;

    QLabel *kpc_type_label_;
    QLabel *new_version_label_;
    QLabel *old_version_label_;
    QLabel *status_label_;

    QLabel *kpc_type_;
    QLabel *new_version_;
    QLabel *old_version_;
    QLabel *status_;

    QPushButton *start_;
    QPushButton *done_;
    QPushButton *cancel_;
    QProgressBar *completedness_;

    QTimer *updateNodeStatesTimer_;
    QTimer *updateProgressTimer_;
    QTimer *startTimer_;
    QTimer *doneTimer_;
    QTimer *testTimer_;
    QTimer *cancelTimer_;
    QTimer *cancelCountdown_; // for a 10 second display on the cancel button

    bool alldone_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // UPGRADEKPCSOFTWARE_H
