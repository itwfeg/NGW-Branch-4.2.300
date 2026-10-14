/**
 * @file superOperatorChanges.h
 * @brief Operator Changes tracks all changes by an operator to a PLU in operator mode.
 * The changes to the PLU can tracked by a single PLU or all PLUs together. Changes can
 * but the changes record is not committed until the operator presses print. The changes
 * tracked include the bycount, weight, net weight, and total price. The information
 * offered includes the operator who perfored the change, what product, what field,
 * and % gained and loss.
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperOperatorChangesCsu
 * @{
*/

#ifndef SUPEROPERATORCHANGES_H
#define SUPEROPERATORCHANGES_H

#include "screenwidget.h"

#include <QItemDelegate>
#include "python.h"

class DomTransactionDates;
class MadpScaleConfig;
class QIntValidator;

class QTableWidgetItem;
class QTableWidget;

namespace Ui {
class SuperOperatorChanges;
}

/**
* @class SuperOperatorChanges
* @brief Tracks Operator Changes to pricing fields in Operate Mode whe the Operator prints. Tracks field, amount, date, and plu.
*/

class SuperOperatorChanges  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperOperatorChanges _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
      */
    explicit SuperOperatorChanges(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperOperatorChanges();

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
     * SLOT method to display the previous set of Operator Details after clicking the Previous button
     *
     * @param[in] none
     * @returns none
     */
    void onClickedPrevious();

    /**
     * SLOT method to display the next set of Operator Details after clicking the Next button
     *
     * @param[in] none
     * @returns none
     */
    void onClickedNext();

    /**
     * SLOT method to redisplay the Operator Changes after viewing the Operator Details and clicking BACK
     *
     * @param[in] none
     * @returns none
     */
    void goBackToSummary();

    /**
     * SLOT method to print a report with the Operator Changes on it
     *
     * @param[in] none
     * @returns none
     */
     void printOperatorChanges();

     /**
      * SLOT method to Export Operator Changes
      *
      * @param[in] none
      * @returns none
      */
      void exportOperatorChanges();

protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::SuperOperatorChanges *ui;

    DomTransactionDates *transactionDates_;
    MadpScaleConfig *scaleConfig_;

    QIntValidator *dialogPluNumberValidator_;	// enforce limits on the plu number

    bool requestingData;                        // are operator changes currently being requested
    bool requestingDetailsData;                 // indicates we are asking for Operator Details
    bool receivedDates;                         // if date ranges were received to display in date combos
    bool receivedTransTypesData;                // was transaction type data received
    bool hasAlreadyBeenLoaded;                  // indicates if the DETAILS table has been loaded previously

    int currentSelectedRow;                     // indicates the current Operator Changes Row selected
    int numOfOpers;                             // how many operators have Operator Changes
    int prevButtCtr;                            // current state of previous and next buttons, each click is -1 or +1
    int whatPage;                               // 1 = Operator Changes, 2=Operator Change Details

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
     * Method to set up the operator changes table
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateOperatorChanges(const SessionMessage &msg);

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
    void updateOperatorChangesDetails(const SessionMessage &msg);

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
     * Clear the Operator Changes Table.
     */
    void clearOperatorChangesTable();

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


/**
* @class ItemDelegate
* @brief Currently not being used. I implemented this class in order to change some properties of cell rows,
* cells selected, unselected, change highlighted color, background color, etc. In the end I did not use or
* need the ItemDelegate class but its a powerful class and I see us using it in the future so I am leaving it
* in here for future reference. All of the concepts in this class work.
*/
class ItemDelegate: public QItemDelegate
{

public:

    ItemDelegate()
    {}

    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        QPalette pal = option.palette;
        QStyleOptionViewItem  viewOption(option);

        //viewOption.palette.setColor(QPalette::Highlight, Qt::blue);



        //painter->fillRect(option.rect, Qt::lightGray);

        //if(index.row() == 0)
        //{
            // To Change the CLICKED ROW highlighed text
            //viewOption.palette.setColor(QPalette::HighlightedText, Qt::white);
            //painter->fillRect(option.rect, Qt::lightGray);

        //}
        // etc if I wanted to change other rows - left as a future reference
        // else if(index.row() == 1)
        //{
        //    viewOption.palette.setColor(QPalette::HighlightedText, Qt::yellow);
        //    painter->fillRect(option.rect, Qt::tomato);
        //}

        QItemDelegate::paint(painter, viewOption, index);
    }
};

#endif // SUPEROPERATORCHANGES_H
