/**
  * @file servicePrinterConfig.h
  * @brief Allows printer(s) to be configured.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup servicePrinterConfigCsu
  * @{
  */

#ifndef SERVICEPRINTERCONFIG_H
#define SERVICEPRINTERCONFIG_H

#include "screenwidget.h"
//#include "messageboxdialog.h"

class QIntValidator;
class MessageBoxDialog;

namespace Ui {
class servicePrinterConfig;
}

/**
 * @class servicePrinterConfig
 * @brief Allows printer(s) to be configured.
 */

class servicePrinterConfig : public ScreenWidget
{
    Q_OBJECT

    typedef servicePrinterConfig _Myt;
    typedef ScreenWidget _Mybase;


public:
    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit servicePrinterConfig(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~servicePrinterConfig();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when a field on the screen has changed.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChanged();

    /**
     * Slot for when the Printer selection is changed.
     */
    void PrinterChanged(int index);

    /**
     * Slot for when the Test Label Pattern selection is changed.
     */
    void UpdateTestPatternLabel(int index);

    /**
     * Slot for Printing single test label.
     */
    void PrintSingleTestLabel();

    /**
     * Slot for continuously printing single test labels.
     */
    void PrintMultipleTestLabels();

    /**
     * Slot for stopping the printing of continous test labels.
     */
    void StopMultipleTestLabels();

    /**
     * Slot for calibrating the Media Sensor.
     */
    void CalibrateMediaSensor();

    /**
     * Slot for calibrating the Label Taken Sensor.
     */
    void CalibrateLabelTakenSensor();

    /**
     * Slot to advance the label stock.
     */
    void AdvanceLabelStock();

    /**
     * Slot to calibrate shoot through (gap) sensor.
     */
    void CalibrateShootThrough();

    /**
     * Slot for when the Printer Settings button is clicked.
     */
    // FT-636
    void PrinterSettings();

    /**
     * Slot to move on to next shoot through calibration step when dialog clears
     */
    void sendCalibrationNext();

protected:

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    enum GapCalStates
    {
        _CalInit,
        _BackingPaper,
        _LabelPlusBacking,
        _CalcDeflections,
        _FailureSettingI,
        _FailureMemory,
        _Done
    };

    Ui::servicePrinterConfig *ui;

    QString currentPrinter;

    QString currentTestLabel;

    bool bSetupChanged;
    bool shootThroughCalStarted;
    bool printheadDisengaged_;

    MessageBoxDialog *calDialog;
    MessageBoxDialog *calInitDialog;
    MessageBoxDialog *calBackingDialog;

    QIntValidator *continuousLabelsExpelPositionValidator_;
    QIntValidator *verticalPrintPositionValidator_;
    QIntValidator *horizontalPrintPositionValidator_;

    /**
     * Used with numericLineEdit widget to validate Continuous Labels Expel Position,
     * Vertical Print Position and Horizontal Print Position.
     *
     */
    int makeValidatorTop(int digits);


    /**
     * Initialize dropdowns.
     *
     */
    void initialize();

    /**
     * Get the initial values for the current printer.
     *
     * @param[in] Printer to request
     */
    void initPrinterValues(QString printerType);

    /**
     * Hide all fields on the screen.
     *
     */
    void HideAllFields();

    /**
     * Show all fields on the screen.
     *
     */
    void ShowAllFields();

    /**
     * Determines if Label Width field should enabled.
     *
     * @param[in] Printer to request
     */
    void LabelWidthEnabledEnabled(QString printerType);

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveDataOnExit();

    /**
     * Send messages to turn off sensor tracking and turn off continuous test labels.
     *
     */
    void ClearMessages();

    /**
     * Determine if either an EPP Printer (old style-running out of parts) or EPP 2 Printer (New Style - Atmel) is attached.
     *
     */
    bool isEppPrinterAttached(int printerLocation);

    /**
     * Determine if the first printer is an EPP printer and if it is attached
     *
     */
    bool isFirstPrinterEpp();

    /**
     * Determine if either an USB Printer is attached.
     *
     */
    bool isUsbPrinterAttached(int printerNum);

    /**
     * Determine if either Network Printer is attached.
     *
     */
    bool isNetworkPrinterAttached(int printerNum);

    bool isSSKPrinterAttached();

    // SOF-3853
    bool cutterAttached_;
};

#endif // SERVICEPRINTERCONFIG_H
