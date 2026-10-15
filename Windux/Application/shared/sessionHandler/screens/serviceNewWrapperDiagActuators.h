/**
 * @file serviceNewWrapperDiagActuators.h
 * Service screen for testing wrapper knife.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SERVICENEWWRAPPERDIAGACTUATORS_H
#define SERVICENEWWRAPPERDIAGACTUATORS_H

#include "screenwidget.h"
#include "../platform/python.h"

#include <QMap>

class QLabel;
class QPushButton;
class QSignalMapper;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Service screen for actuator diagnostics
class serviceNewWrapperActuatorDiag : public ScreenWidget
{
    Q_OBJECT

    typedef serviceNewWrapperActuatorDiag _Myt;
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
    explicit serviceNewWrapperActuatorDiag( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /// Destructor.
    virtual ~serviceNewWrapperActuatorDiag();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate the user interface to current language.
    virtual void translate();

protected:

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
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot for push button clicked.
    void onPushButtonClicked();

    /// Slot for timer timeout.
    void onTimerTimeout();

private:

    static const int TITLE_FONT_POINT_SIZE = 18;        // was 26
    static const int BUTTON_FONT_POINT_SIZE = 18;       // was 22
    static const int FONT_POINT_SIZE = 16;              // was 18

    QLabel *title_;
    QLabel *lastSavedLabel_;
    QLabel *lastSaved_;

    QPushButton *diagStart_;
    QPushButton *diagStop_;

    QPushButton *saveHistory_;
    QPushButton *clearHistory_;
    QPushButton *saveTimings_;

    //
    // sensors
    //

    QLabel *sensorTitle1_;
    QLabel *sensorOpenLabel_;
    QLabel *sensorCloseLabel_;

    QLabel *openCloseTitle1_;
    QLabel *openSavedLabel_;
    QLabel *openCurrentLabel_;
    QLabel *closeSavedLabel_;
    QLabel *closeCurrentLabel_;

    QLabel *lastTimeCalibrated_;
    QLabel *lastTime_;
    QLabel *howManyTimesCalibrated_;
    QLabel *howManyTimes_;

    QLabel *gripperNarrowLabel_;
    QLabel *gripperNarrowOpenSaved_;
    QLabel *gripperNarrowOpen_;
    QLabel *gripperNarrowCloseSaved_;
    QLabel *gripperNarrowClose_;

    QLabel *gripperWideLeftLabel_;
    QLabel *gripperWideLeftOpenSaved_;
    QLabel *gripperWideLeftOpen_;
    QLabel *gripperWideLeftCloseSaved_;
    QLabel *gripperWideLeftClose_;

    QLabel *gripperWideRightLabel_;
    QLabel *gripperWideRightOpenSaved_;
    QLabel *gripperWideRightOpen_;
    QLabel *gripperWideRightCloseSaved_;
    QLabel *gripperWideRightClose_;

    QLabel *sideClampLeftLabel_;
    QLabel *sideClampLeftOpenSaved_;
    QLabel *sideClampLeftOpen_;
    QLabel *sideClampLeftCloseSaved_;
    QLabel *sideClampLeftClose_;

    QLabel *sideClampRightLabel_;
    QLabel *sideClampRightOpenSaved_;
    QLabel *sideClampRightOpen_;
    QLabel *sideClampRightCloseSaved_;
    QLabel *sideClampRightClose_;

    QLabel *sensorTitle2_;
    QLabel *sensorUpLabel_;
    QLabel *sensorDownLabel_;

    QLabel *upDownTitle1_;
    QLabel *upSavedLabel_;
    QLabel *upCurrentLabel_;
    QLabel *downSavedLabel_;
    QLabel *downCurrentLabel_;

    QLabel *knifeLabel_;
    QLabel *knifeUpSaved_;
    QLabel *knifeUp_;
    QLabel *knifeDownSaved_;
    QLabel *knifeDown_;

    QLabel *wandLabel_;
    QLabel *wandUpSaved_;
    QLabel *wandUp_;
    QLabel *wandDownSaved_;
    QLabel *wandDown_;

    QLabel *wand2Label_;
    QLabel *wand2UpSaved_;
    QLabel *wand2Up_;
    QLabel *wand2DownSaved_;
    QLabel *wand2Down_;

    QLabel *filmSelectorLabel_;
    QLabel *filmSelectorUpSaved_;
    QLabel *filmSelectorUp_;
    QLabel *filmSelectorDownSaved_;
    QLabel *filmSelectorDown_;

    QLabel *sensorTitle3_;
    QLabel *sensorHomeLabel_;

    QLabel *homeTitle1_;
    QLabel *homeSavedLabel_;
    QLabel *homeCurrentLabel_;

    QLabel *applierHandLabel_;
    QLabel *applierHandSaved_;
    QLabel *applierHand_;

    QLabel *applier2HandLabel_;
    QLabel *applier2HandSaved_;
    QLabel *applier2Hand_;

    //
    // values
    //

    QLabel *sensorTitle4_;
    QLabel *sensorValueLabel1_;
    QLabel *sensorValueLabel2_;

    QLabel *inputVoltageLabel_;
    QLabel *inputVoltageSaved_;
    QLabel *inputVoltage_;

    QLabel *vacuumLabel_;
    QLabel *vacuumSaved_;
    QLabel *vacuum_;

    QLabel *vacuum2Label_;
    QLabel *vacuum2Saved_;
    QLabel *vacuum2_;

    QLabel *mainPressureLabel_;
    QLabel *mainPressureSaved_;
    QLabel *mainPressure_;

    QLabel *linePressureLabel_;
    QLabel *linePressureSaved_;
    QLabel *linePressure_;

    QMap<int, QLabel*> valueLabels_;                    // all QLabel fields on the screen
    QMap<int, QString> currentTimings_;                 // current incoming values
    QMap<int, QString> savedTimings_;                   // read from saved file
    QMap<int, QMap<int, int> > validateRange_;          // upper and lower range for a field

    QPalette pushButtonPalette_;

    bool diagActive_;
    bool testActive_;

    QTimer *statusRequestTimer_;

    QMap<int, QList<SessionMessage>* > history_;            // make it a pointer to the list

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Create Fake Data for Display Testing
    void createFakeData();

    /// Create new header label.
    QLabel *createHeaderLabel( const QString &name, bool isTitle = false );

    /// Create new footnote label.
    QLabel *createFootnoteLabel( const QString &name );

    /// Create new title label.
    QLabel *createTitleLabel( const QString &name, Qt::Alignment alignment = Qt::AlignCenter );

    /// Create new label.
    QLabel *createLabel( const QString &name, Qt::Alignment alignment = Qt::AlignCenter );

    /// Crate a new no frills label.
    QLabel *createSimpleLabel( const QString &name, Qt::Alignment alignment = Qt::AlignLeft );

    /// Create new label.
    void outOfRangeLabel(int index, int curValue, QLabel *checkLabel );

    /// Test if a pair of values are within 10 points of one another.
    void inRangeByTenLabel(int index, bool curVal = true);

    /// Save Timings to a file for historic use to compare against current timings.
    void saveTimingsToFile();

    /// Read Timings from a file to display on the screen to compare against current timings
    void readTimingsFromFile();
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SERVICENEWWRAPPERDIAGACTUATORS_H
