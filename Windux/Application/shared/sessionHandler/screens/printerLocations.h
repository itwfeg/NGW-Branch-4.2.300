/**
 * @file printerLocations.h
 * @brief Widget to set the location of each printer.
  *
  * @copyright Copyright (c) 2014 Hobart Corporation
  * @author Alex Zavala
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/
#ifndef PRINTERLOCATIONS_H
#define PRINTERLOCATIONS_H
#include "screenwidget.h"
#include "deviceTests.h"
#include <QtGui>

class DomCurrentPLU;

namespace Ui {
class printerLocations;
}

/**
* @class printerLocations
* @brief  Class to set the location of each printer
*
*/

class printerLocations  : public ScreenWidget
{
    Q_OBJECT

    typedef printerLocations _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
      */
    explicit printerLocations(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~printerLocations();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:
// Sample Slot Methods for various widgets

    /**
    * SLOT method called to print a test label for the first printer
    *
    * @param[in] none
    * @returns  none
    * @brief testLabel1
    */
    void testLabel1();

    /**
     * SLOT method called to print a test label for the second printer
     *
     * @param[in] none
     * @returns none
     * @brief testLabel2
     */
    void testLabel2();

    /**
     * SLOT method called to print a test label for the third printer
     *
     * @param[in] none
     * @returns none
     * @brief testLabel3
     */
    void testLabel3();

    /**
     * SLOT method called to save the values changed on the page
     *
     * @param[in] none
     * @returns  none
     * @brief updateLabel
     */
    void updateLabel();

    /**
     * SLOT method called to change the print contrast of the first printer
     *
     * @brief printContrast1
     * @param index the selection in cboxPrintContrast1
     * @returns none
     */
    void printContrast1(int index);

    /**
     * SLOT method called to change the print contrast of the second printer
     *
     * @brief printContrast2
     * @param index the selection in cboxPrintContrast2
     * @returns none
     */
    void printContrast2(int index);

    /**
     * SLOT method called to change the print contrast of the third printer
     *
     * @brief printContrast3
     * @param index the selection in cboxPrintContrast3
     * @returns none
     */
    void printContrast3(int index);

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

    /**
     * Check for specific error message is handled by screen.
     *
     * Session will call this method upon receiving an error message. The default error handling
     * (i.e. popup an error dialog) shall be invoked unless @c true is returned from
     * this property.
     *
     * @returns  @c true to use custom error handler, @c false otherwise
     */
    virtual bool isErrorMessageHandler( const SessionMessage& errorMsg ) const;

    virtual bool eventFilter( QObject *obj, QEvent *event );

private:
    Ui::printerLocations *ui;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to reload widget's fields with new values.
     *
     * @param[in] none
     * @returns  none
     */
    void reloadFields();

    /**
     * Method that enables the printer type and contrast combo boxes
     *
     * @param printerNum 0/Printer1, 1/Printer2, 2/Printer3
     * @brief enablePrinter
     */
    void enablePrinter(int printerNum);

    /**
     * Method that disables the printer type and contrast combo boxes
     *
     * @param printerNum 0/Printer1, 1/Printer2, 2/Printer3
     * @returns  none
     * @brief disablePrinter
     */
    void disablePrinter(int printerNum);

    //Enum for printerType
    typedef enum printerType
    {
        PrimaryPrinter     = 0,
        SecondaryPrinter   = 1,
        TertiaryPrinter    = 2
    } printerType_et;

    /**
     * Method for setting the printer type
     *
     * @brief setPrinterType
     * @param printerType PrimaryPrinter, SecondaryPrinter, TertiaryPrinter
     * @param printerLocation the printer's address
     */
    void setPrinterType(int printerType, QString printerLocation);

    /**
     * Method for checking the validity of the contrast level.
     *
     * @brief validContrast
     * @param contrast the printer's contrast level
     * @return true if contrast is within valid range, false otherwise
     */
    bool validContrast(int contrast);

    /**
     * Method for checking the validity of the printerLocation
     *
     * @brief validPrinterLocation
     * @param printerLocation the printer's location
     * @return true if printerLocation is within valid range, false otherwise
     */
    bool validPrinterLocation(int printerLocation);

    /**
     * Method that sets the printer's contrast
     *
     * @brief setPrinterContrast
     * @param printerType PrimaryPrinter, SecondaryPrinter, TertiaryPrinter
     * @param contrast printer's contrast level
     */
    void setPrinterContrast(int printerType, int contrast);

    /**
     * Method that disables all of the printer's types, contrasts, and testLabel fields
     *
     * @brief disableAll
     */
    void disableAll();

    /**
     * Method that enables all of the printer's types, contrasts, and testLabel fields
     *
     * @brief enableAll
     */
    void enableAll();

    /**
     * Method that disables the printer and set its type to "No Response"
     *
     * @brief setNoResponse
     * @param printerNum PrimaryPrinter,SecondaryPrinter,TertiaryPrinter
     */
    void setNoResponse(int printerNum);

    QString primaryPrinterMemAddr;
    QString secondaryPrinterMemAddr;
    QString tertiaryPrinterMemAddr;
};
#endif // PRINTERLOCATIONS_H
