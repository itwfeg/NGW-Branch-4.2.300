/**
 * @file upgradeNextGenWrapper.h
 * @brief Upgrades next generation wrapper and displays progress.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author: Randy Blankley
 */

#ifndef UPGRADENEXTGENWRAPPER_H
#define UPGRADENEXTGENWRAPPER_H

#include "screenwidget.h"

#include <QVector>

class QLabel;
class QProgressBar;
class QPushButton;
class QTimer;
class QVBoxLayout;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Upgrades next generation wrapper and displays progress.
class upgradeNextGenWrapper  : public ScreenWidget
{
    Q_OBJECT

    typedef upgradeNextGenWrapper _Myt;
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
    explicit upgradeNextGenWrapper(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /// Destructor.
    virtual ~upgradeNextGenWrapper();

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
        UpgradeComplete
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

private Q_SLOTS:

    /// Slot for push button clicked.
    void onPushButtonClicked();

    /// Slot for screen changed.
    void onScreenChanged();

    /// Slot for testing.
    void onTest();

    /// Slot for updating node states.
    void onUpdateNodeStates();

private:

    static const int TITLE_FONT_POINT_SIZE = 26;
    static const int BUTTON_FONT_POINT_SIZE = 22;
    static const int FONT_POINT_SIZE = 18;

    enum nodeState
    {
        Pending = 0,
        Upgrading,
        Complete,

        ErrorNone = 100,
        ErrorNoResponseFromNode,
        ErrorNodeTimeout,
        ErrorNodeWriteFailure,
        ErrorNodeChecksumFailure,
        ErrorNodeEraseBank1Failure,
        ErrorNodeEraseBank2Failure,
        ErrorNodeIdFailure,
        ErrorNodeWriteAddressOutOfRange
    };

    struct nodeInfo
    {
        nodeState state;
        int percentComplete;

        QProgressBar *progress;
        QLabel *description;
        QLabel *version;
        QLabel *status;
    };

    QLabel *title_;

    QLabel *nodeProgressLabel_;
    QLabel *nodeVersionLabel_;
    QLabel *nodeStatusLabel_;

    QVBoxLayout *nodeProgress_;
    QVBoxLayout *nodeVersion_;
    QVBoxLayout *nodeStatus_;

    QVector<nodeInfo> nodes_;

    QPushButton *begin_;
    QPushButton *cancel_;

    QLabel *message_;

    QTimer *updateNodeStatesTimer_;
    QTimer *testTimer_;

    bool allowInfo_;
    QList<QLabel*> infoLabels_;

    state state_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Create new header label.
    QLabel *createHeaderLabel( const QString &name, bool isTitle = false );

    /// Create new label.
    QLabel *createLabel( const QString &name, Qt::Alignment alignment = Qt::AlignCenter );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // UPGRADENEXTGENWRAPPER_H
