#ifndef SERVICEWEIGHERRESET_H
#define SERVICEWEIGHERRESET_H

#include "screenwidget.h"
#include <Devices.h>
#include <Events.h>
#include <Messages.h>

namespace Ui {
class serviceWeigherReset;
}

/**
 * @class serviceWeigherReset
 * @brief Screen for allowing the user to reset the Weigher to factory defaults.
 *
 * This class is used in Service to allow the operator to reset the Weigher to factory defaults.
 */


class serviceWeigherReset : public ScreenWidget
{
    Q_OBJECT

    typedef serviceWeigherReset _Myt;
    typedef ScreenWidget _Mybase;


public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceWeigherReset(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceWeigherReset();

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
    * when resetting the Weigher to factory defaults.
    *
    * @param[in] none
    * @returns  none
    */
    void confirmWeigherReset();

private:

    Ui::serviceWeigherReset *ui;

    void initialize();

    bool weigherResetSuccessful;
    QTimer *startWeigherResetTimer;
    bool weigherResetTimerDone;

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

#endif // SERVICEWEIGHERRESET_H
