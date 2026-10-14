/**
  * @file weigherCalibrate.h
  * @brief Widget to allow Weigher to be calibrated.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup weigherCalibrateCsu
  * @{
  */

#ifndef WEIGHERCALIBRATE_H
#define WEIGHERCALIBRATE_H

#include <QWidget>

#include "../screens/screenwidget.h"

class QIntValidator;
class SessionMainWindow;
class MadpInternalConfig;

namespace Ui {
class weigherCalibrate;
}

/// Hobart namespace.
namespace hobart {

/**
 * @class weigherCalibrate
 * @brief Widget to allow Weigher to be calibrated.
 */

class weigherCalibrate : public QWidget
{
    Q_OBJECT
    
    typedef weigherCalibrate _Myt;
    typedef QWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent @c ModeWidget widget
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] calledBy  1 Indicates Service Mode Panel, 0 Indicates Manufacturing Test Screen
     */
    explicit weigherCalibrate(QWidget *parent, SessionMainWindow *mainWindow, int calledBy );

    /**
     * Destructor.
     */
    virtual ~weigherCalibrate();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot to allow the widget to handle processing of incoming session messages.
     *
     * @param[in] msg  session message
     */
    void onSessionMessageReceive( const SessionMessage& msg );

    /**
     * Slot for when Start button is clicked.
     */
    void StartClicked();

    /**
     * Slots for when a Next Step button is clicked.
     */
    void NextStep_1_Clicked();
    void NextStep_2_Clicked();
    void NextStep_3_Clicked();
    void NextStep_4_Clicked();
    void NextStep_5_Clicked();
    void NextStep_6_Clicked();
    void NextStep_7_Clicked();

    /**
     * Slot for when "Complete Calibration" button is clicked.
     */
    void CompleteCalibrationClicked();

    /**
     * Slot for when Cancel button is clicked.
     */
    void CancelClicked();

    /**
     * Slot for when Apply gain factor button is clicked on the Calibration Complete screen.
     */
    void CC_ApplyGainFactorClicked();

    /**
     * Slot for when Done button is clicked on the Calibration Complete screen.
     */
    void CC_DoneClicked();

private:
    Ui::weigherCalibrate *ui;

    Session *session_;
    SessionMainWindow *mainWindow_;

    MadpInternalConfig *internalConfig_;

    int calledFromServicePanel;

    QIntValidator *CC_GainFactorValidator_;

    int waitOnTiltMsg_;

    int cancelClicked;
    int weigherCts;             // dont use every count

    double tiltX;
    double tiltY;

    bool valuemaxIsEnabled;     // is this a valuemax enabled scale

    // FT-695
    QString unitsType;

    /**
     * Used with numericLineEdit widget to validate Calibration Complete Gain Factor.
     *
     */
    int makeValidatorTop(int digits);

    /**
     * Only show certain fields on the initial screen.
     *
     */
    void initialize();

    /**
     * Display the weight increments for the steps, in Lbs or Kgs.
     *
     */
    void UpdateStepWeightDisplay(QString weightType);

    /**
     * Updates field in the Zeroed column.
     *
     */
    void UpdateZeroedData(int stepNo, QString value);

    /**
     * Updates field in the Delta column.
     *
     */
    void UpdateDeltaData(int stepNo, QString value);

    /**
     * Updates field in the Non-Zeroed column.
     *
     */
    void UpdateNonZeroedData(int stepNo, QString value);

    /**
     * Shows or Hide a Next Step button.
     *
     */
    void ShowOrHideStepButton(int stepNo, bool visibityFlag);

    /**
     * Shows the Calibration Complete screen.
     *
     */
    void ShowCalibrationCompleteScreen(QString zeroedValue, QString nonZeroedValue, QString deltaValue);

    /**
     * Sends message for next calibration step number.
     *
     */
    void SendCalibrationStepNumber(QString stepNumber);

    // check if the scale is a half degree out of level and send error if we are
    bool sendOutOfHalfDegreeError();

    /**
     * Sends message to end calibration.
     *
     */
    void SendCalibrationEnd();

    /**
     * Clears fields when Cancel button is clicked.
     *
     */
    void ResetOnCancel();

    /**
     * Displays the Service Mode Panel.
     *
     */
    void DisplayServiceModePanel();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // WEIGHERCALIBRATE_H
