/**
 * @file superSupervisorChanges.h
 * @brief Supervisor Changes tracks all changes by an operator to a PLU in supervisor mode.
 * The changes to the PLU can tracked by a single PLU or all PLUs together. The changes
 * tracked include the bycount, net weight, and total price, and tare. The information
 * offered includes the operator who perfored the change, what product, what field,
 * and % gained and loss.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperSupervisorChangesCsu
 * @{
*/

#ifndef SUPERSUPERVISORCHANGES_H
#define SUPERSUPERVISORCHANGES_H

#include "screenwidget.h"

#include <QItemDelegate>
#include "python.h"

class DomTransactionDates;
class QIntValidator;
class MadpScaleConfig;

class QTableWidgetItem;
class QTableWidget;

namespace Ui {
class SuperSupervisorChanges;
}

/**
* @class SuperSupervisorChanges
* @brief Tracks Supervisor Changes to pricing fields in Supervisor Mode. Tracks field, amount, date, and plu.
*/

class SuperSupervisorChanges  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperSupervisorChanges _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
      */
    explicit SuperSupervisorChanges(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperSupervisorChanges();

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
     * SLOT method to highlight a row when it is selected
     *
     * @param[in] row  row is the row of the cell selected.
     * @param[in] col  col is the column of the cell selected. Col is currently not being used but reqired by the SIGNAL.
     * @returns  none
     */
    void selectTheRow(int row, int col);

    /**
     * Method to gather and request the data required when the user presses the Details button for a specific row
     *
     * @param[in] none
     * @returns  none
     */
    void transactionsDetails();

    /**
     * SLOT method to set up and request the changes table. Called by the Retrieve Data button on dialog panel
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
     * SLOT method to display the previous set of Supervisor Details after clicking the Previous button
     *
     * @param[in] none
     * @returns none
     */
    void onClickedPrevious();

    /**
     * SLOT method to display the next set of Supervisor Details after clicking the Next button
     *
     * @param[in] none
     * @returns none
     */
    void onClickedNext();

    /**
     * SLOT method to redisplay the Supervisor Changes after viewing the Supervisor Details and clicking BACK
     *
     * @param[in] none
     * @returns none
     */
    void goBackToSummary();

    /**
     * SLOT method to print a report with the Supervisor Changes on it
     *
     * @param[in] none
     * @returns none
     */
     void printSupervisorChanges();

     /**
      * SLOT method to Export Supervisor Changes
      *
      * @param[in] none
      * @returns none
      */
      void exportSupervisorChanges();

protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::SuperSupervisorChanges *ui;

    DomTransactionDates *transactionDates_;
    MadpScaleConfig *scaleConfig_;

    QIntValidator *dialogPluNumberValidator_;	// enforce limits on the plu number

    bool requestingData;                        // are supervisor changes currently being requested
    bool requestingDetailsData;                 // indicates we are asking for Supervisor Details
    bool receivedDates;                         // if date ranges were received to display in date combos
    bool receivedTransTypesData;                // was transaction type data received
    bool hasAlreadyBeenLoaded;                  // indicates if the DETAILS table has been loaded previously

    int currentSelectedRow;                     // indicates the current Supervisor Changes Row selected
    int numOfOpers;                             // how many operators have Supervisor Changes
    int prevButtCtr;                            // current state of previous and next buttons, each click is -1 or +1
    int whatPage;                               // 1 = Supervisor Changes, 2=Supervisor Change Details

    QString requestPLUNumber;                   // current PLU
    QString requestOperatorID;                  // current Operator ID
    QString requestStartDate;                   // current Start Date
    QString requestEndDate;                     // current End Date
    QStringList operIDs;                        // the list of operator IDs

    QMap<int,QString> transTypesPri;
    QMap<int,QString> transTypesSec;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to set up the changes table
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateSupervisorChanges(const SessionMessage &msg);

    /**
     * Method to set up the dates in the range on the dialog so the user can select a range
     * of dates in order to track changes in that range of dates.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateTransactionDates(const SessionMessage &msg);

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
    // Supervisor Changes Details Methods

   /**
     * Method to initialize everything to do with with Supervisor Changes Details
     *
     * @param[in] none
     * @returns none
     */
    void initializeDetails();

    /**
     * Method to update the types of transactions each Supervisor operator may have used while
     * performing changes.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateTransactionTypeNames(const SessionMessage &msg);

    /**
     * Method to update the Supervisor Changes Details
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateSupervisorChangesDetails(const SessionMessage &msg);

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

    /**
     * Clear the Supervisor Changes Table.
     */
    void clearSupervisorChangesTable();

    /** checkFileName()
     *
     *  checkFileName checks to determine if a valid file was entered. If the filename
     *  is an empty string, or one of a number of illegal characters, a warning will
     *  be displayed to the user indicating the problem.
     *  @param[in] myfile       - the file name the user entered
     *        [in] fileOrFolder - indicates if a filename or foldername was entered
     *  @return if file was valid
     */
    bool checkFileName( QString &myfile, int fileOrFolder );

    /** getFileName()
     *
     *  getFileName determines if a filename was entered with an extension. The extension
     *  is displayed in the file type combo box, and is automatically append the extension
     *  selected by default. However, without question some user will add the extension
     *  while entering the file name itself. This method fixes any issues that may be
     *  encountered if the user adds the entension themselves.
     *  @param: myfile       - the file name the user entered
     *  @return: String - the valid file name with the extension properly appended.
     */
    QString getFileName(QString myfile);

    /**
     * The method determines if we are running on LINUX or WINDOWS to set initial directories
     *
     * @param[in] none
     * @returns  @c true if system OS is a LINUX based system, @c else false which indicates Windows
     */
    static bool isLinux();



};

#endif // SUPERSUPERVISORCHANGES_H
