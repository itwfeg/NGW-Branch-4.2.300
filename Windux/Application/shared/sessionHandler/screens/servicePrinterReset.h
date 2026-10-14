#ifndef SERVICEPRINTERRESET_H
#define SERVICEPRINTERRESET_H

#include "screenwidget.h"
#include <Devices.h>
#include <Events.h>
#include <Messages.h>

namespace Ui {
class servicePrinterReset;
}

/**
 * @class servicePrinterReset
 * @brief Screen for allowing the user to reset the Printer to factory defaults.
 *
 * This class is used in Service to allow the operator to reset the Printer to factory defaults.
 */


class servicePrinterReset : public ScreenWidget
{
    Q_OBJECT

    typedef servicePrinterReset _Myt;
    typedef ScreenWidget _Mybase;


public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit servicePrinterReset(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~servicePrinterReset();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private slots:

    void btnYesClicked();
    void btnNoClicked();

    /**
    * SLOT method called when the timer used to wait for confirmation
    * when resetting the Printer to factory defaults.
    *
    * @param[in] none
    * @returns  none
    */
    void confirmPrinterReset();

private:

    Ui::servicePrinterReset *ui;

    void initialize();

    bool printerResetSuccessful;
    QTimer *startPrinterResetTimer;
    bool printerResetTimerDone;

    QString periphControllerVer;

    void populateBoardType();
    QString boardManf;

    // SOF-3047 - support 3 digit version numbers
    bool compareSoftwareVersions(QString v1, QString v2);

    bool isLinux();

    QTimer *waitForFpcVersionTimer;
    bool haveFpcVersion;

    /**
     * Sleep function.
     */
    void sleepFor( int milliseconds ) const;
};

#endif // SERVICEPRINTERRESET_H
