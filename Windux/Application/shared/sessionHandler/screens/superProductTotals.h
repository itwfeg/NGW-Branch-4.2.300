/**
 * @file superProductTotals.h
 * @brief Product Totals tracks all transactions per PLU. A printed label
 * represents a single transaction. The fields tracked for Product Transactions include
 * the transaction type, total weight of all transctions, total price
 * of all transcactions, the number of items, and the number of
 * transactions. The summary information of all of these fields are displayed
 * at the bottom for a total of all products searched in a range.
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @issues: no masking of fields at this point and no auto lookup on ENTER.
 * @addtogroup SuperProductTotalsCsu
 * @{
*/

#ifndef SUPERPRODUCTTOTALS_H
#define SUPERPRODUCTTOTALS_H

#include "screenwidget.h"

class DomTransactionDates;
class QIntValidator;

class QTableWidgetItem;
class QTableWidget;

namespace Ui {
class SuperProductTotals;
}

/**
* @class SuperProductTotals
* @brief Tracks all Product Transactions performed (labels printed) in operate mode
*/

class SuperProductTotals  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperProductTotals _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
      */
    explicit SuperProductTotals(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperProductTotals();

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
    * SLOT method called when the plu field on the form changes, not the
    * plu field on the initial dialog overlay
    *
    * @param[in] newPlu newPlu is the new plu number in the text field
    * @returns  none
    */
    void updatePluFieldData();

protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::SuperProductTotals *ui;

    DomTransactionDates *transactionDates_;

    QIntValidator *dialogPluNumberValidator_;   // enforce limits on the plu number
    QIntValidator *pluNumberValidator_;         // enforce limits on the plu number

    bool requestingData;                        // are transactions currently being requested
    bool receivedDates;                         // if date ranges were received to display in date combos
    bool receivedTransTypesData;                // was transaction type data received
    bool hasAlreadyBeenLoaded;                  // indicates if the DETAILS table has been loaded previously
	bool doneInitializingCombo;                 // indicates if the transaction name combo box has been updated

    int numOfOpers;                             // how many operators have Operator Changes
    int prevButtCtr;                            // current state of previous and next buttons, each click is -1 or +1
    int whatPage;                               // 1 = Operator Changes, 2=Operator Change Details
    int TEST_PLU;                               // Enter a plu to search

    QString requestPLUNumber;                   // current PLU
    QString requestOperatorID;                  // current Operator ID
    QString requestStartDate;                   // current Start Date
    QString requestEndDate;                     // current End Date
    QStringList operIDs;                        // the list of operator IDs

    //QVector<int> transTypeNums;
    QVector<QString> transTypes1;
    QVector<QString> transTypes2;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to set up the operator changes table
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateProductTransactions(const SessionMessage &msg);

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

    ///////////////////////////////////////////////////////////////////////////
    // Operator Changes Details Methods

   /**
     * Method to initialize everything to do with with Operator Changes Details
     *
     * @param[in] none
     * @returns none
     */
    void initializeDetails();

    /**
     * Method to update the types of transactions each operator may have used while
     * performing changes.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateTransactionTypeNames(const SessionMessage &msg);

    /**
     * Method to update the Operator Changes Details
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateProductTotalsDetails(const SessionMessage &msg);

    /**
     * Method to request the appropriate transaction group dependant upon which details
     * button was pressed between previous, first, or next
     *
     * @param[in] whichGroup where -1 = previous, 0 = first, 1 = next
     * @returns none
     */
    void requestTransactionGroup(QString whichGroup);

    /**
     * Method to request and receive the transaction type for a given transaction number
     *
     * @param[in] typeNumber which indicates the number of the transaction type being examined
     * @returns typeName which is the transaction type currently indexed by the typeNumber
     */
    QString retrieveTransactionType(int typeNumber);


};

#endif // SUPERPRODUCTTOTALS_H
