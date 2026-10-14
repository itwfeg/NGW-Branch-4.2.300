/**
 * @file SuperCoolCreateProdSteps.h
 * @brief Super Cool Main allows operators to assign COOL countries, COOL Production Steps, and
 * COOL ProdSteps to Defined COOL ProdSteps.  The countries, steps and ProdSteps can also be
 * added to the ANY category. The asp page that represents this class is the vaguely named
 * and misunderstood web page named superCOOLLinkProdSteps.asp

 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperCoolCreateProdStepsCsu
 * @{
 */

#ifndef SUPERCOOLCREATEPRODSTEPS_H
#define SUPERCOOLCREATEPRODSTEPS_H

#include "screenwidget.h"

class DomCoolProductionStep;

namespace Ui {
    class SuperCoolCreateProdSteps;
}

/**
* @class SuperCoolCreateProdSteps
* @brief Allow users to create new Production Steps or delete them
*
*/

class SuperCoolCreateProdSteps  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperCoolCreateProdSteps _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
      */
    explicit SuperCoolCreateProdSteps(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperCoolCreateProdSteps();

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
    * SLOT method called when a row in the table is clicked
    *
    * @param[in] row  row is the row of the cell selected.
    * @param[in] col  col is the column of the cell selected. Col is currently not being used but reqired by the SIGNAL.
    * @returns  none
    */
    void clickRow(int row, int col);

    /**
    * SLOT method called when a record has been selected, and then the Delete button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void deleteRecord();

    /**
    * SLOT method called when a record has been selected, and then the Cancel button has been clicked
    *
    * @param[in] none
    * @returns  none
    */
    void cancelRecord();

    /**
    * SLOT method called when the user presses the Add a Production Step button
    *
    * @param[in] none
    * @returns  none
    */
    void addProductionStep();

    /**
    * SLOT method called when the <ENTER> key is pressed after entering a value in the name field
    *
    * @param[in] none
    * @returns  none
    */
    void editProductionStepName();

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
    Ui::SuperCoolCreateProdSteps *ui;
    DomCoolProductionStep *coolProductionStep_;                     // pointer to the DOM

    QString origRecordName;
    bool hasChanged;                                                // indicates if something has changed
    bool requestingData;                                            // so we dont overload and fat finger click
    int activeRow;                                                  // the current row selected
    int numOfItems;                                                 // how many are there?
    int numOfRows;                                                  // how many total rows do we have?

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to save anything at the end that may need to be saved.
     *
     * @param[in] none
     * @returns  none
     */
    void saveDataOnExit();

    /**
     * Method to clear all input fields and the Cool Table.
     *
     * @param[in] none
     * @returns  none
     */
    void clearTheFields();

    /**
     * Method to save the current record either by creating a new one or editing an existing record.
     *
     * @param[in] none
     * @returns  none
     */
    void saveCurrentRecord();

    /**
     * Method to a currently selected record from the table in the description and number fields.
     *
     * @param[in] none
     * @returns  none
     */
    void viewCOOLRecord();

    /**
     * Method to send a message to the scale application when the user selects a row in the table.
     *
     * @param[in] none
     * @returns  none
     */
    void readRecord(const QString &record);

    /**
     * Method to a read from the Scale Application a selected record from the table.
     *
     * @param[in] row  row is the current operator selected row
     * @returns  none
     */
    void readRecordFromRow(int row);

    /**
     * Method to display either the clicked upon Production Step or creating a new one by entering a number that doesn't exist.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void displayCoolProductionStep(const SessionMessage& msg);

    /**
    * Method used to build and update all of the Production Step information tables
    *
    * @param[in] msg  msg is the data received from the Scale Application.
    * @returns  none
    */
   void updateCoolList(const SessionMessage &msg);

   /**
   * Method used to insert a new record as a row into the Cool table
   *
   * @param[in] none
   * @returns  none
   */
   void insertNewCoolListRow();

   /**
   * Method used to build and update all of the Production Step information tables
   *
   * @param[in] name  name is the text string to display in the name field.
   * @returns  none
   */
   void updateActiveRowRecordName(const QString &name);

   /**
   * Method used to create a new record after it was determined the numberd record
   * does not currently exist. A user enters a record number and the user gets back
   * a dialog box indicating the record does not currently exist and is given the
   * option to create a new one. Clicking OK on the dialog will allow creation.
   *
   * @param[in] name  name is the text string to display in the name field.
   * @returns  none
   */
   void createNewCOOLProductionStep(const SessionMessage &msg);

};

#endif // SUPERCOOLCREATEPRODSTEPS_H
