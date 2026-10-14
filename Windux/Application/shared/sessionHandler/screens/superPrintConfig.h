/**
 * @file superPrintConfig.h
 * @brief Supervisor Print Config
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SUPERPRINTCONFIG_H
#define SUPERPRINTCONFIG_H

#include "screenwidget.h"

namespace Ui {
class superPrintConfig;
}

/**
* @class superPrintConfig
* @brief  Allows for printing the scale configuration to the scale's printer. When this page is laucnhed it will start printing.
*  
*/

class superPrintConfig  : public ScreenWidget
{
    Q_OBJECT

    typedef superPrintConfig _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit superPrintConfig(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superPrintConfig();

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
// Sample Slot Methods for various widgets

    /**
    * SLOT method called when the Cancel button is clicked
    *
    * @param[in] none
    * @returns  none
    */
    void butCancel_clicked();

    /**
    * SLOT method called to allow printer time to begine printing.
    *
    * @param[in] none
    * @returns  none
    */
    void timerSlot();


protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::superPrintConfig *ui;

    /**
     * Method to begin printing.
     *
     */
    void startPrinting();
    /**
     * Method to continue printing.
     *
     */
    void keepPrinting();
    /**
     * Method to notify printing. has finished.
     *
     */

    void printConfigReportFinished();

    /**
     * Method to delay a print job.
     *
    */
    void delay(int);

    QString reportStatus;
    QTimer * timer;
    int state;
};

#endif // superPrintConfig
