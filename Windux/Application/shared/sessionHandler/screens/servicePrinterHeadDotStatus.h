/**
  * @file servicePrinterHeadDotStatus.h
  * @brief Allows printer(s) to be configured.
  *
  * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
  * @author J Crew
  * @addtogroup servicePrinterHeadDotStatusCsu
  * @{
  */

#ifndef SERVICEPRINTERHEADDOTSTATUS_H
#define SERVICEPRINTERHEADDOTSTATUS_H

#include "screenwidget.h"

class QIntValidator;

namespace Ui {
class servicePrinterHeadDotStatus;
}

/**
 * @class servicePrinterHeadDotStatus
 * @brief Allows printer(s) to be configured.
 */

class servicePrinterHeadDotStatus : public ScreenWidget
{
    Q_OBJECT

    typedef servicePrinterHeadDotStatus _Myt;
    typedef ScreenWidget _Mybase;

    
public:
    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit servicePrinterHeadDotStatus(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~servicePrinterHeadDotStatus();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
     * Slot for when the Printer selection is changed.
     */
    void onPrinterChanged(int index);

    /**
     * Slot for when Code Checker is enabled or disabled.
     */
    void onEnableCodeCheckerClicked();

    /**
     * Slot for when Code Checker is in operate mode
     */
    void onScreenChanged();


    /**
     * Slot for when Dot status isn't returned
     */
    void onDotWearTimerTimeout();

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
    Ui::servicePrinterHeadDotStatus *ui;

    bool bSetupChanged;
    bool codeCheckerEnabled_;
    bool codeCheckerEnabledOriginal_;

    int badDots_;
    int marginalDots_;

    QString currentPrinter;

    QTimer * dotWearTimer;

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
     * Determine if either an EPP Printer (old style-running out of parts) or EPP 2 Printer (New Style - Atmel) is attached.
     *
     */
    bool isEppPrinterAttached(int printerLocation);

    /**
     * Determine if either an USB Printer is attached.
     *
     */
    bool isUsbPrinterAttached(int printerNum);

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveDataOnExit();

    /**
     * Sends a request for dot wear, disables the ui and starts a timeout timer.
     *
     */
    void startDotWearTest();

    /**
     * Re-Enables the ui and stops the timeout timer if active
     *
     */
    void dotWearTestFinished();
};

#endif // SERVICEPRINTERHEADDOTSTATUS_H
