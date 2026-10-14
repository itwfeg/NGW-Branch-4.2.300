/**
  * @file superPrinterSetup.h
  * @brief Allows Printers to be assigned as Primary, Secondary or Tertiary.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup superPrinterSetupCsu
  * @{
  */

#ifndef SUPERPRINTERSETUP_H
#define SUPERPRINTERSETUP_H

#include "screenwidget.h"

class DomPrWriteAllExternalPrinter;

namespace Ui {
class superPrinterSetup;
}

/**
 * @class superPrinterSetup
 * @brief Allows Printers to be assigned as Primary, Secondary or Tertiary.
 */

class superPrinterSetup : public ScreenWidget
{
    Q_OBJECT

    typedef superPrinterSetup _Myt;
    typedef ScreenWidget _Mybase;
    
public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit superPrinterSetup(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superPrinterSetup();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when a field on the screen has changed.
     * Sets property indicating that the setup needs to be saved when exiting.
     */
    void setupChanged(int index);

    /**
     * Slot for Printing test label.
     */
    void PrintTestLabel_0();

    /**
     * Slot for Saving Print Contrast when it changes.
     */
    void SavePrintContrast_0( int index );

    /**
     * Slot for when the Update button is clicked.
     */
    void UpdatePrinters();

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

    Ui::superPrinterSetup *ui;

    // Used to save off the printerAddress values from the Printer, PrPrinterType message
    QString printerAddress_0;
    QString printerAddress_1;
    QString printerAddress_2;
    QString currentPrinter_;

    bool bSetupChanged;

    /**
     * Initialize dropdowns.
     *
     */
    void initialize();

    /**
     * Initialize fields.
     *
     */
    void initFields();

    /**
     * Disables dropdowns and buttons.
     *
     */
    void disableFields();

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveDataOnExit();

    /**
     * Turn off Printer Type Tracking.
     *
     */
    void TurnOffPrinterTypeTracking();

    /**
     * Determine if either an EPP Printer (old style-running out of parts) is attached.
     *
     */
    bool isEppPrinterAttached(int printerLocation);

    /**
     * Determine if either an USB Printer is attached.
     *
     */
    bool isUsbPrinterAttached(int printerLocation);

    /**
     * Processes the list of printer names and updates the combo box and labels on the screen where needed.
     *
     */
    void updatePrinterSettings( const SessionMessage &msg );

};

#endif // SUPERPRINTERSETUP_H
