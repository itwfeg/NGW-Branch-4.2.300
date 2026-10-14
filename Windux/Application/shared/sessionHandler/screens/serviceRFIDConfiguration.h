/**
 * @file serviceRFIDConfiguration.h
 * @brief ...describe...
 *
 * @copyright Copyright (c) 2026, ITW/Hobart, Food Equipment Group, U.S.A.
 * @Author: Chris King
 * @{
*/

#ifndef SERVICERFIDCONFIGURATION_H
#define SERVICERFIDCONFIGURATION_H

#include "screenwidget.h"
#include "Messages.h"
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QScrollBar>
#include <QTextCursor>


namespace Ui {
class serviceRFIDConfiguration;
}

/**
* @class serviceRFIDConfiguration
* @brief Allows for the setup of the RFID configuration.
*/
class serviceRFIDConfiguration : public ScreenWidget
{
    Q_OBJECT
    
    typedef serviceRFIDConfiguration _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
      * Constructor.
      */
    explicit serviceRFIDConfiguration(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~serviceRFIDConfiguration();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve screen help text.
     *
     * When help is activated on a screen this property returns the screen title and help text.
     *
     * @param[out] title  screen title
     * @param[out] description  screen help description
     * @returns  @c true if help exists for the screen, @c false otherwise
     */
    virtual bool helpText( QString& title, QString& description ) const;

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
    * SLOT method named onUpdateRFIDFirmwareClicked 
    *
    * @param[in] none
    * @returns  none
    */
    void onUpdateRFIDFirmwareClicked();

    /**
    * SLOT method named onStartRFIDTestClicked 
    *
    * @param[in] none
    * @returns  none
    */
    void onStartRFIDTestClicked();

    /**
    * SLOT method named onCalibrateRFIDClicked 
    *
    * @param[in] none
    * @returns  none
    */
    void onCalibrateRFIDClicked();

    /**
    * SLOT method named readNewLogData 
    *
    * @param[in] none
    * @returns  none
    */
    void readNewLogData(); 

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

    Ui::serviceRFIDConfiguration *ui;

    QFile rfidLogFile;
    QTextStream rfidLogTextStream;
    qint64 lastRfidLogPosition;
    QTimer *rfidLogTimer;

    bool screenBusy = false;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();
};

#endif // SERVICERFIDCONFIGURATION_H