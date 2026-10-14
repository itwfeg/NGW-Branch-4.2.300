/**
 * @file superScannedCodes.h
 * @brief Scanned Codes tracks all barcodes that have been scanned by the usb scanner.  Currently the
 * Scanner is intended to be attached to an elevated self service device. The customer will have
 * the ability to walk up to a elevated self service scale with their produce item and hold it under
 * the scanner and the barcode sticker attached to the item will be used to look up a PLU which the
 * customer can then print a label for purchase.
 *
 * @copyright Copyright (c) 2015, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperScannedCodesCsu
 * @{
*/

#ifndef SUPERSCANNEDCODES_H
#define SUPERSCANNEDCODES_H

#include <Messages.h>
#include "screenwidget.h"

class QTableWidgetItem;
class QTableWidget;
class QIntValidator;

namespace Ui {
class SuperScannedCodes;
}

class SuperScannedCodes  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperScannedCodes _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
      */
    explicit SuperScannedCodes(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperScannedCodes();

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
     * Method called by deleteScannedCode() to physically remove an item from the table.
     *
     * @param[in] none
     * @returns  true if row was removed from the table, otherwise false
     */
    void deleteScannedCode();

    /**
     * SLOT method to highlight a row when it is selected
     *
     * @param[in] row  row is the row of the cell selected.
     * @param[in] col  col is the column of the cell selected. Col is currently not being used but reqired by the SIGNAL.
     * @returns  none
     */
    void selectTheRow(int row, int col);

    /**
     * SLOT method to add and save the newly scanned barcode item when the user clicks ADD (SAVE)
     *
     * @param[in] none
     * @returns  none
     */
    void addAndSaveScannedItemCode();

    /**
     * SLOT method force a save when a plu is entered into the PLU field to save the PLU
     *
     * @param[in] none
     * @returns  none
     */
    void onAssignPluReturnPressed();

protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::SuperScannedCodes *ui;

    DomSingleScannedCode *singleScannedCode_;
    DomScannerCodeList   *scannerCodeList_;
    DomNewScannedCode    *newScannedCode_;
    DomCurrentPLU        *currentPLUData;

    QIntValidator *pluNumberValidator_;                             // enforce limits on the plu number

    int currentSelectedRow_;                                        // the id of the row clicked, may or may not match the id of the row

    bool ignoreSignals_;                                            // when certain events occur, don't trigger SIGNALS

    /**
    * Method to validate fields
    *
    * @param[in] digits  digits is current digits in the text input line.
    * @returns digits  where digits is the new number that has been rolled over due to being over length
    */
    int makeValidatorTop(int digits);

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to physically remove a selected row from the table
     *
     * @param[in] QString - the selected code returned from the application
     * @returns      bool - the removal was successful
     */
    bool removeScannedRow(QString scanId, QString codeId);

    /**
     * Method called by to physically clear the fields at the top of sceen where scanned or clicked items appear.
     *
     * @param[in] none
     * @returns  none
     */
    void clearScannedCodeFields();

    /**
     * Method called by to physically enable the fields at the top of sceen when a create new occurs
     *
     * @param[in] none
     * @returns  none
     */
    void enableTheFields(bool isEnabled);

    /**
     * Method to add the newly created Scanned Code to the top of the list
     *
     * @param[in] none
     * @returns  none
     */
    void insertScannedRow();

    /**
     * Method to update a single scanned code when clicking on a row in the table
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateScannedCode(const SessionMessage &msg);

    /**
     * Method to update the entire list of scanned codes of PLUs after a read all
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateScannedCodesList(const SessionMessage &msg);

    /**
     * Method to update the table when a new produce item has been scanned.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateNewScannedCode(const SessionMessage &msg);

    /**
     * Method to make a boop sound when the user scan a barcode.
     *
     * @param[in] none
     * @returns   none
     */
    void boopcode();

    /**
     * Method to verify we are trying to add a new and unique row.
     *
     * @param[in] none
     * @returns   bool - if the id already exists
     */
    bool checkIfItemAlreadyExists(QString codeNum, QString pluNum);

    /**
     * Resets ignoreSignals_ flag so we can continue on in the case we receive an error dialog
     */
    void resetIgnoredSignals();

    /**
     * Updates the top of the screen information from the data in the row clicked on
     */
    void updateInformationFromTable();

};

#endif // SUPERSCANNEDCODES_H

