/**
 * @file superHourlyTotals.h
 * @brief Hourly Totals tracks all transactions made hour by hour. A printed label
 * represents a single transaction. The fields tracked for Hourly Transactions include
 * the hour the transaction occurred, total weight of all transctions, total price
 * of all transcactions a specific hour, the number of items an hour, and the number of
 * transactions per hour. The summary information of all of these fields are displayed 
 * at the bottom for a total of all hours searched in a range.
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperHourlyTotalsCsu
 * @{
*/

#ifndef SUPERHOURLYTOTALS
#define SUPERHOURLYTOTALS

#include "screenwidget.h"

class DomTransactionDates;

class QTableWidgetItem;
class QTableWidget;
class QIntValidator;

namespace Ui {
class SuperHourlyTotals;
}

/**
* @class SuperHourlyTotals
* @brief Tracks all Hourly Transactions performed (labels printed) in operate mode 
*/

class SuperHourlyTotals  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperHourlyTotals _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
      */
    explicit SuperHourlyTotals(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperHourlyTotals();

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

protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::SuperHourlyTotals *ui;

    DomTransactionDates *transactionDates_;

    QIntValidator *dialogPluNumberValidator_;   // enforce limits on the plu number

    bool requestingData;                        // are transactions currently being requested
    bool receivedDates;                         // if date ranges were received to display in date combos
    bool receivedTransTypesData;                // was transaction type data received
    bool hasAlreadyBeenLoaded;                  // indicates if the DETAILS table has been loaded previously
	bool doneInitializingCombo;                 // indicates if the transaction name combo box has been updated

    int numOfOpers;                             // how many operators have Operator Changes
    int prevButtCtr;                            // current state of previous and next buttons, each click is -1 or +1
    int whatPage;                               // 1 = Operator Changes, 2=Operator Change Details
    int transTypesMax;                          // The maximumm transaction type number

    QString requestPLUNumber;                   // current PLU
    QString requestOperatorID;                  // current Operator ID
    QString requestStartDate;                   // current Start Date
    QString requestEndDate;                     // current End Date
    QStringList operIDs;                        // the list of operator IDs
    QStringList transTypeNumberList;            // The transaction record ID

    QVector<QString> transTypes1;
    QVector<QString> transTypes2;

    /**
     * Method to validate fields
     *
     * @param[in] digits  digits is current digits in the text input line.
     * @returns digits  where digits is the new number that has been rolled over due to being over length
     */
    int makeValidatorTop(int digits);           // for line edit masking

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
    void updateHourlyTransactions(const SessionMessage &msg);

    /**
     * Method to update the starting and ending date combo boxes on the dialog
     *
     * @param[in] msg  system message containing all of the date information
     * @returns none
     */
    void updateDialogsDueToLanguageChange(const SessionMessage &msg);

    /**
     * Method to set up the dates in the range on the dialog so the user can select a range
     * of dates in order to track changes in that range of dates.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateTransactionDates(const SessionMessage &msg);

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
    void updateHourlyTotalsDetails(const SessionMessage &msg);

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

#endif // SUPERHOURLYTOTALS_H
