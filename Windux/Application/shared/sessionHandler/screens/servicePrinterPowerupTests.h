/**
  * @file servicePrinterPowerupTests.h
  * @brief Screen for displaying powerup test result information about the printer(s) attached to the system.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup servicePrinterPowerupTestsCsu
  * @{
  */

#ifndef SERVICEPRINTERPOWERUPTESTS_H
#define SERVICEPRINTERPOWERUPTESTS_H

#include "screenwidget.h"
#include <QMap>

namespace Ui {
class servicePrinterPowerupTests;
}

/**
 * @class servicePrinterPowerupTests
 * @brief Screen for displaying powerup test result information about the printer(s) attached to the system.
 */

class servicePrinterPowerupTests : public ScreenWidget
{
    Q_OBJECT

    typedef servicePrinterPowerupTests _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit servicePrinterPowerupTests(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~servicePrinterPowerupTests();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when the Printer selection is changed.
     */
    void PrinterChanged();

protected:

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );


private:
    Ui::servicePrinterPowerupTests *ui;

    enum
    {
      UNKNOWN_HEAD,     // 0
      TDK753_OHM,
      TDK800_OHM,       // 2
      TDK849_OHM,
      KYOCERA753_OHM,   // 4
      KYOCERA800_OHM,
      KYOCERA849_OHM    // 6
    };

    QMap<int, QString> printHeads_;                 // current incoming values

    /**
     * Populate the Printer dropdown based on the scaleModel found in MadpScaleConfig.
     *
     */
    void initialize();

    /**
     * Determine if either an EPP Printer (old style-running out of parts) or EPP 2 Printer (New Style - Atmel) is attached.
     *
     */
    bool isEppPrinterAttached();

};

#endif // SERVICEPRINTERPOWERUPTESTS_H
