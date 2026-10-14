/**
  * @file servicePrinterSensors.h
  * @brief Allows printer sensors to be monitored.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup servicePrinterSensorsCsu
  * @{
  */


#ifndef SERVICEPRINTERSENSORS_H
#define SERVICEPRINTERSENSORS_H

#include "screenwidget.h"

class QTimer;

namespace Ui {
class servicePrinterSensors;
}

/**
 * @class servicePrinterSensors
 * @brief Allows printer sensors to be monitored.
 */

class servicePrinterSensors : public ScreenWidget
{
    Q_OBJECT

    typedef servicePrinterSensors _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit servicePrinterSensors(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~servicePrinterSensors();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when the Printer selection is changed.
     */
    void PrinterChanged();

    /**
     * Slot for when "Turn Printhead voltage ON/OFF" button is clicked.
     */
    void TogglePrintheadVoltage();

    /**
     * Slot for when the "Test Label" button is clicked.
     */
    void PrintTestLabel();

    /**
     * Slot for when the "Test Cutter" button is clicked.
     */
    void TestCutter();

    /**
     * Slot for when the "Test Cutter" button is clicked and see if it happens or not.
     */
    void onCutterTimeout();

    /**
     * Slot for when the "Test Edge" button is clicked.
     */
    void TestEdge();

    /**
     * Slot for when the "Test Gap" button is clicked.
     */
    void TestGap();

protected:

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

private:
    Ui::servicePrinterSensors *ui;

    enum CutterState
    {
        CutterNone = 0,
        CutterHome = 1,
        CutterLeftHome = 2,
        CutterReturnedHome = 3,
        CutterTestFinished = 4
    };

    QTimer *cutterTimer_;

    QString currentPrinter;
    QString scaleModel;
    QString stockLength;
    QString stockLengthInLines;

    // Holds current Printhead voltage state
    bool bPrintHeadVoltageOn;
    bool cutterAttached_;
    bool cutter_fired_;

    CutterState cutter_state_;

    /**
     * Populate the Printer dropdown based on the scaleModel found in MadpScaleConfig.
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
     * Toggles Printhead voltage settings to ON position.
     *
     */
    void PrintheadVoltageOn();

    /**
     * Toggles Printhead voltage settings to OFF position.
     *
     */
    void PrintheadVoltageOff();

    /**
     * Send messages to turn off sensor tracking and turn off printhead voltage.
     *
     */
    void ClearSettings();

    /**
     * Determine if either an EPP Printer (old style-running out of parts) or EPP 2 Printer (New Style - Atmel) is attached.
     *
     */
    bool isEppPrinterAttached();

    /**
     * Set up the Label Stock length fields.
     *
     */
    void displayLabelStockLengths();

    /**
     * Determine if either an SSK Printer is attached
     *
     */
    bool isSSKPrinterAttached();

};

#endif // SERVICEPRINTERSENSORS_H
