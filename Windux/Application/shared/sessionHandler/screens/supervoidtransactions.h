/**
 * @file SuperVoidTransactions.h
 * @brief Super Void Transactions is where an operator can go in and void specific transactions from 
 * the list of current transactions in the scale. The list of transactions can be sorted by
 * picked from a specific date range, and be selected for just a specific operator in a 
 * particular date range. Transactions can then be selected for deletion.
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperVoidTransactionCsu
 * @{
*/

#ifndef SUPERVOIDTRANSACTIONS_H
#define SUPERVOIDTRANSACTIONS_H

#include "screenwidget.h"

class DomTotalTypeData;
class DomTransactionDates;
class QIntValidator;

class QTableWidgetItem;
class QTableWidget;

namespace Ui {
class SuperVoidTransactions;
}

/**
* @class SuperVoidTransactions
* @brief Displays all transactions across a specific date range which can then be narrowed 
* even further by operator. Transactions can then be examined or selected and deleted. 
*/

class SuperVoidTransactions  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperVoidTransactions _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
      */
    explicit SuperVoidTransactions(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperVoidTransactions();

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
     * SLOT method to set up and request the operator changes table. Called by the Retrieve Data button on dialog panel
     *
     * @param[in] none
     * @returns  none
     */
    void filterDialogCallback();

    /**
     * SLOT method to evaluate when the user presses changes the PLU field in the dialog
     *
     * @param[in] none
     * @returns  none
     */
    void evaluateRetrieveButton(QString newText);

    /**
    * SLOT method called when the first start day combo is selected
    *
    * @param[in] newStr newStr is the new item in the Combo Box selected
    * @returns  none
    */
    void updateEndDateSelector(int newIndex);

    /**
    * SLOT method called when the All Products checkbox is selected
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkCheckbox(int state);

    /**
    * SLOT method called when the List of Operators Combo Box is selected on the main form
    *
    * @param[in] index, where index is the selected index of the combo box selection
    * @returns  none
    */
    void updateOperatorSelection(int index);

    /**
     * SLOT method to highlight a row when it is selected
     *
     * @param[in] row  row is the row of the cell selected.
     * @param[in] col  col is the column of the cell selected. Col is currently not being used but reqired by the SIGNAL.
     * @returns  none
     */
    void selectTheRow(int row, int col);

    /**
     * SLOT method to delete the transaction rows selected in the transaction table. Each row, as selected, is added to
     * an overall list of selectedItems in selectRow. When the voidSelectedTransactions is clicked, this list is passed
     * to the scale applicaiton for deletion.
     * @param[in] none.
     * @param[in] none.
     * @returns  none
     */
    void voidSelectedTransactions();


protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::SuperVoidTransactions *ui;

    DomTotalTypeData *totalTypeData_;
    DomTransactionDates *transactionDates_;

    QIntValidator *dialogPluNumberValidator_;	// enforce limits on the plu number

    bool canOperatorCombosReceiveEvents;        // can they? Not when updating and adding items
    bool canDateCombosReceiveEvents;            // can they? Not when being updated. Wonder if there is a property that can do this. Hmmmmmm
    bool requestingData;                        // are transactions currently being requested
    bool receivedDates;                         // if date ranges were received to display in date combos
    bool receivedTransTypesData;                // was transaction type data received
    bool receivedOperators;
    bool doneInitializingCombo;                 // indicates if the transaction name combo box has been updated

    int numOfTrans;                             // how many transactions need to be displayed
    int currentSelectedRow;                     // indicates the current Operator Changes Row selected
    int selectedDiaogOperator;                  // operator id selected on the initial dialog

    QString requestPLUNumber;                   // current PLU
    QString requestOperatorID;                  // current Operator ID
    QString requestStartDate;                   // current Start Date
    QString requestEndDate;                     // current End Date

    QStringList operatorIDs;
    QStringList operatorNames;
    QStringList ids;                            // These will map to the rows we delete

    QVector<QString> selectedRows;
    QMap<int, QString> transTypes1;
    QMap<int, QString> transTypes2;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();


    /**
     * Method to set up the dates in the range on the dialog so the user can select a range
     * of dates in order to track changes in that range of dates.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateTransactionDates(const SessionMessage &msg);

    /**
     * Method to update the starting and ending date combo boxes on the dialog
     *
     * @param[in] digits  digits is current digits in the text input line.
     * @returns digits  where digits is the new number that has been rolled over due to being over length
     */
    void updateDialogsDueToLanguageChange(const SessionMessage &msg);

    /**
     * Method used to update the list of voidable and viewable transactions in transactions table.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateVoidableTransactions(const SessionMessage &msg);

    /**
     * Method used to update the list of operators in the operator selector combo box.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateOperators(const SessionMessage &msg);

    /**
     * Method to update the types of transactions each operator may have used while
     * performing changes.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateTransactionTypeNames(const SessionMessage &msg);

    /**
    * Method to validate fields
    *
    * @param[in] digits  digits is current digits in the text input line.
    * @returns digits  where digits is the new number that has been rolled over due to being over length
    */
    int makeValidatorTop(int digits);

    /**
     * Method to enable a specific widget
     *
     * @param[in] widget the Qwidget to be enabled.
     * @returns  none
     */
    void enableControl(QWidget& widget);

    /**
     * Method to disable a specific widget
     *
     * @param[in] widget the Qwidget to be disabled.
     * @returns  none
     */
    void disableControl(QWidget &widget);

    /**
      * Method to request data for dialog box such as the date range
      *
      * @param[in] none
      * @returns none
      */
    void initDialog();

    /**
      * Method to disable all of the controls on the dialog
      *
      * @param[in] none
      * @returns none
      */
    void disableAllControls();

    /**
      * Method to enable all of the controls on the dialog
      *
      * @param[in] none
      * @returns none
      */
    void enableAllControls();

    /**
     * Method to request and receive the transaction type for a given transaction number
     *
     * @param[in] typeNumber which indicates the number of the transaction type being examined
     * @returns typeName which is the transaction type currently indexed by the typeNumber
     */
    QString retrieveTransactionType(int typeNumber);

};

#endif // SuperVoidTransactions
