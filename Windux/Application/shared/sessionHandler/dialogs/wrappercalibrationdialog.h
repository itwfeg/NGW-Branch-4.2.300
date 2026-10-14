/**
 * @file wrappercalibrationdialog.h
 * Wrapper Calibration dialog.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef WRAPPERCALIBRATIONDIALOG_H
#define WRAPPERCALIBRATIONDIALOG_H

#include "../sessiondialog.h"

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QMap>
#include <QPalette>

class QHBoxLayout;
class QLabel;
class QPushButton;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Wrapper Calibration dialog.
class WrapperCalibrationDialog : public SessionDialog
{
    Q_OBJECT

    typedef WrapperCalibrationDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit WrapperCalibrationDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Constructor.
    /**
     * @param[in] retainMode  retain current wrapper mode
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit WrapperCalibrationDialog( bool retainMode, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~WrapperCalibrationDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Whether or not clicking the overlay rejects the dialog.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const {return !active_;}

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Add specific mechanism to calibrate.
    /**
     * @param[in] number  mechanism
     * @param[in] name  mechanism name
     */
    virtual void addMechanism( int number, const QString &name );

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Callback for session messages.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for button clicked.
    void onButtonClicked();

    /// Slot for timer timeout.
    void onTimerTimeout();

private:

    bool retainMode_;
    bool active_;
    bool testFailed_;

    DomEvolWrapperStatus evolWrapperStatus_;

    QPalette defaultPalette_;

    QLabel *titleLabel_;

    QLabel *instructionsLabel_;
    QLabel *warningLabel_;

    QLabel *statusLabel_;

    QMap<QPushButton*, int> mechanisms_;
    QPushButton *calibrate_;
    QPushButton *exit_;

    QHBoxLayout *buttonsLayout_;

    QTimer *timeout_;
    QTimer *blink_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Process wrapper status.
    void processWrapperStatus();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // WRAPPERCALIBRATIONDIALOG_H
