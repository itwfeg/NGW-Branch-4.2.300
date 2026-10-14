/* @file SuperVoidTransactions.cpp
 * Super Void Totals is where an operator can go in and void specific transactions from 
 * the list of current transactions in the scale. The list of transactions can be sorted by
 * a date range, picked from a specific date range, and be selected for just a specific operator
 * in a particular date range. Transactions can then be selected for deletion.
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 */

#include "supervoidtransactions.h"
#include "ui_supervoidtransactions.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>

static const int DEFAULT_ROWS = 6;
static const int DEFAULT_COLS = 7;
static const int NOT_CHECKED = 0;
static const int CHECKED = 2;

namespace hobart{
    class numericLineEdit;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

SuperVoidTransactions::SuperVoidTransactions(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperVoidTransactions),
    totalTypeData_(NULL),
    transactionDates_(NULL)
{
    // saveOnExit_(false),
    ui->setupUi(this);
    scaleScreen();

    // retrieve config object
    if ( mainWindow_->isSupervisorMode() )
        totalTypeData_ = &dynamic_cast<SupervisorMode*>( mode_ )->totalTypeData_;

    if ( mainWindow_->isSupervisorMode() )
        transactionDates_ = &dynamic_cast<SupervisorMode*>( mode_ )->transactionDates_;

    initialize();
    translate();
}

SuperVoidTransactions::~SuperVoidTransactions()
{
    // Will have to hide a keypad - find out how. Haven't triggered it yet
    //if(( saveOnExit_ ) && (transactionDates_))
    //    mainWindow_->hideKeyboard();

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::initialize()
{
    // initialize vars
    canOperatorCombosReceiveEvents = true;                          // can they? Not when updating and adding items
    canDateCombosReceiveEvents = true;                              // can they? Not when updating and adding items

    requestingData = false;                                         // when retrieve data button is selected
    receivedTransTypesData = false;                                 // indicates we have received the transaction names
    receivedDates = false;                                          // indicates we have received the dates
    receivedOperators = false;                                      // have we received the operator list message
    doneInitializingCombo = false;                                  // are the transaction types updated yet? Ignores SIGNALS until then
    numOfTrans = 0;
    selectedDiaogOperator = -1;                                      // operator id selected on the initial dialog

    requestPLUNumber = "";                                          // current PLU
    requestOperatorID = "";                                         // current Operator ID
    requestStartDate = "";                                          // current Start Date
    requestEndDate = "";                                            // current End Date

    ui->tblTransactionTableData->clear();

    // create all of the checkbox signals and slots

    // buttons
    connect(ui->retrieveDataButton,         SIGNAL(clicked()),                this, SLOT(filterDialogCallback()));            // click retrieve data on dialog
    connect(ui->butVoidTransactions,        SIGNAL(clicked()),                this, SLOT(voidSelectedTransactions()));        // clicked void transactions button

    // checkbox
    connect(ui->checkboxAllProducts,        SIGNAL(stateChanged(int)),        this, SLOT(checkCheckbox(int)) );               // clicked the checkbox for all product

    // combos
    connect(ui->cboOperatorSelector,        SIGNAL(currentIndexChanged(int)), this, SLOT(updateOperatorSelection(int)));      // select transaction type
    connect(ui->dialogStartDateSelector,    SIGNAL(currentIndexChanged(int)), this, SLOT(updateEndDateSelector(int)) );       // The date changed

    // table
    connect(ui->tblTransactionTableData,    SIGNAL(cellClicked(int,int)),     this, SLOT(selectTheRow(int,int)));             // click cell
    connect(ui->dialogPluNumberInput,       SIGNAL(textChanged(QString)),     this, SLOT(evaluateRetrieveButton(QString)));   // The PLU field was changed

    initDialog();                                                   // initialize the dialog

    // Set a default number of Rows and Cols, otherwise won't be able to set the headers
    ui->tblTransactionTableData->setRowCount(DEFAULT_ROWS);
    ui->tblTransactionTableData->setColumnCount(DEFAULT_COLS);

    // Headers for changes table
    ui->tblTransactionTableData->setColumnWidth( 0, scaleWidthForScreen( 186 ) );             // date time
    ui->tblTransactionTableData->setColumnWidth( 1, scaleWidthForScreen( 162 ) );             // Operator
    ui->tblTransactionTableData->setColumnWidth( 2, scaleWidthForScreen( 162 ) );             // trans type
    ui->tblTransactionTableData->setColumnWidth( 3, scaleWidthForScreen( 75 ) );              // plu
    ui->tblTransactionTableData->setColumnWidth( 4, scaleWidthForScreen( 110 ) );             // weight
    ui->tblTransactionTableData->setColumnWidth( 5, scaleWidthForScreen( 110 ) );             // price
    ui->tblTransactionTableData->setColumnWidth( 6, scaleWidthForScreen( 110 ) );              // items
    // WIDTH                                      941/1015

    ui->tblTransactionTableData->setSelectionMode(QAbstractItemView::MultiSelection);

    // setup validators
    ui->dialogPluNumberInput->setValidator(dialogPluNumberValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->dialogPluNumberInput->setDecimalPlaces(0);
    ui->dialogPluNumberInput->setInputWrapping(true);
    dialogPluNumberValidator_->setTop(999999);

    session_->send(TransactionTypeDev,TransactionReadNonVoidGroup);	// 152,20 - Request Transaction Type names

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperVoidTransactions::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::translate()
{
	ui->retranslateUi( this );

    // update the start and end date combos on the dialog
    if(receivedDates)
        session_->send(TransactionDev, TransactionReadDates);           // 151,30 -> get date ranges fresh in the correct langguage

    // update the "All Operators" combo on the screen and the dialog
    ui->cboDialogOperatorSelector->setItemText(0, tr("All operators"));
    ui->cboOperatorSelector->setItemText(0, tr("All operators"));

    // update the table titles
    QStringList headersStr;
    headersStr << tr("Date/Time") << tr("Operator") << tr("Trans. Type") << tr("PLU") << tr("Weight") << tr("Price") << tr("Items");
    ui->tblTransactionTableData->setHorizontalHeaderLabels(headersStr);

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::enableControl(QWidget &widget)
{
    widget.setDisabled(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::disableControl(QWidget& widget)
{
    widget.setDisabled(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                              DIALOG BOX METHODS
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::initDialog()
{
    // NEW CODE
    canDateCombosReceiveEvents = false;
    ui->Dialog->setVisible(true);
    ui->dialogStartDateSelector->addItem(tr("Today"));
    ui->dialogStartDateSelector->addItem(tr("Yesterday"));
    ui->dialogEndDateSelector->addItem(tr("Today"));

    disableAllControls();                                           // when requesting date ranges

    operatorIDs.clear();                                            // should already be clear regardless;
    operatorNames.clear();

    session_->send(TransactionDev, TransactionReadDates);           // 151,30 -> get date ranges
    session_->send(OperatorDev, OpDtReadAll);                       // 102,1  -> Request list of operators
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::updateTransactionDates(const SessionMessage& msg)
{
    // Received because we received a 151,31, which is the reply to a 151,30 to request the transaction dates
    // Requested through requestDialogData(), which is called by initDialog()
    //C.Update: 151,31,5,"2,1,0,6,5",2,"""Nov 29, 12 "",""Nov 28, 12 """,9,"2012/12/6,2012/12/5,2012/12/4,2012/12/3,2012/12/2,2012/12/1,2012/11/30,2012/11/29,2012/11/28"

    DomTransactionDates m( msg );   // access with: msg[0].toInt() OR     m.numOfOpIds().toInt();

    int quantityWeekDays = m.numOfWeekDays().toInt();           // 5
    int quantityDatesDisplay = m.numOfDatesDisplay().toInt();   // 2
    QStringList weekDays = m.weekDaysList().split(",");         // 2,1,0,6,5

    QVector<QString> datesDisplay = TokenParser::splitStringTokens( m.datesDisplayList() ); //"""Nov 29, 12 "",""Nov 28, 12 """

    QStringList weekDayNames;
    weekDayNames << tr("Sunday") << tr("Monday") << tr("Tuesday") << tr("Wednesday") << tr("Thursday") << tr("Friday") << tr("Saturday");

    canDateCombosReceiveEvents = false;                             // should of already been false by default, but wanted to enforce it.
    QString theDay = "";
    for ( int i = 0; i < quantityWeekDays; i++ )
    {
        theDay = weekDayNames.at( weekDays.at(i).toInt() );
        ui->dialogStartDateSelector->addItem( theDay );
    }

    for ( int i = 0; i < quantityDatesDisplay; i++ )
    {
        ui->dialogStartDateSelector->addItem( datesDisplay[i] ) ;
    }
    canDateCombosReceiveEvents = true;

    updateEndDateSelector(0);

    // Now enable the corresponding controls
    receivedDates = true;
    enableAllControls();

    // get the transactionDates class to reuse it later
    if ( transactionDates_ )
        *transactionDates_ = m;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::updateDialogsDueToLanguageChange(const SessionMessage& msg)
{
    DomTransactionDates m( msg );

    int quantityWeekDays = m.numOfWeekDays().toInt();
    QStringList weekDays = m.weekDaysList().split(",");
    QVector<QString> datesDisplay = TokenParser::splitStringTokens( m.datesDisplayList() ); //"""Nov 29, 12 "",""Nov 28, 12 """

    QStringList weekDayNames;
    weekDayNames << tr("Sunday") << tr("Monday") << tr("Tuesday") << tr("Wednesday") << tr("Thursday") << tr("Friday") << tr("Saturday");

    // we can update the first 3 items ourselves.  Remember we are not creating these like initially.
    // We are just updating the text and not the dates
    ui->dialogStartDateSelector->setItemText(0, tr("Today"));
    ui->dialogStartDateSelector->setItemText(1, tr("Yesterday"));
    ui->dialogStartDateSelector->setItemText(2, tr("Today"));

    QString theDay = "";
    for ( int i = 0; i < quantityWeekDays; i++ )
    {
        theDay = weekDayNames.at( weekDays.at(i).toInt() );
        ui->dialogStartDateSelector->setItemText(i+2, theDay);
    }

    int quantityDatesDisplay = m.numOfDatesDisplay().toInt();
    for(int j = 0; j < quantityDatesDisplay; j++)
    {
        ui->dialogStartDateSelector->setItemText(j+7, datesDisplay[j] );
    }

    // update the end date combo by starting over.
    ui->dialogEndDateSelector->clear();
    int selectedIndex = ui->dialogStartDateSelector->currentIndex();

    for(int i = 0; i <= selectedIndex; ++i)
    {
        ui->dialogEndDateSelector->addItem(ui->dialogStartDateSelector->itemText(i));
    }

    enableAllControls();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::disableAllControls()
{
    ui->dialogPluNumberInput->setEnabled(false);
    ui->cboDialogOperatorSelector->setEnabled(false);
    ui->cboOperatorSelector->setEnabled(false);
    ui->checkboxAllProducts->setEnabled(false);
    ui->dialogStartDateSelector->setEnabled(false);
    ui->dialogEndDateSelector->setEnabled(false);
    ui->dialogStartTime->setEnabled(false);
    ui->dialogEndTime->setEnabled(false);
    ui->butVoidTransactions->setEnabled(false);
    ui->retrieveDataButton->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::enableAllControls()
{
    // only enable the controls again once I receive all of my initial requests for data.
    if ( receivedOperators && receivedDates && receivedTransTypesData )
    {
        ui->dialogPluNumberInput->setEnabled(true);
        ui->cboDialogOperatorSelector->setEnabled(true);
        ui->cboOperatorSelector->setEnabled(true);
        ui->checkboxAllProducts->setEnabled(true);
        ui->dialogStartDateSelector->setEnabled(true);
        ui->dialogEndDateSelector->setEnabled(true);
        ui->dialogStartTime->setEnabled(true);
        ui->dialogEndTime->setEnabled(true);

        //ui->retrieveDataButton->setEnabled(true);                 // dont do this until a PLU selected or check all
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperVoidTransactions::retrieveTransactionType(int typeNumber)
{
    QString typeName = "";

    ScaleConfiguration *myScaleConfig = session_->scaleConfig;
    int curLang = myScaleConfig->currentLanguage;

    if (( 2 == curLang ) && ( transTypes2.contains( typeNumber ) ))
        typeName = transTypes2[typeNumber];
    else if ( transTypes1.contains( typeNumber ) )
        typeName = transTypes1[typeNumber];

    if ( typeName.length() == 0 )
        typeName = tr("Unknown");

    return typeName;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::updateVoidableTransactions(const SessionMessage &msg)
{
    DomVoidableTransactionGroup m( msg );
    UiUtilities *util = session_->uiUtilities;

    selectedRows.clear();
    ui->tblTransactionTableData->clear();                           // clean up any previous data

    if(selectedDiaogOperator > -1)                                  // if we selected an operator on the dialog, set the operator on the screen to the same thing
    {
        // only true the first time thru when dismissing the dialog
        if( selectedDiaogOperator == 0 )
        {
            ui->cboDialogOperatorSelector->setCurrentIndex(0);
        }
        else
        {
            for(int j = 0; j < operatorIDs.size(); ++j)
            {
                if(selectedDiaogOperator == operatorIDs.at(j).toInt() )
                {
                    ui->cboDialogOperatorSelector->setCurrentIndex(j);
                    break;
                }
            }
        }
        selectedDiaogOperator = -1;                                 // reset it back to -1 forever
    }

    numOfTrans = m.numberIds();                                     // [var previously known as quantityReceived ]numOfIDs matches the number of transactions made.

    int rows =  numOfTrans;
    if (rows < DEFAULT_ROWS)
       ui->tblTransactionTableData->setRowCount(DEFAULT_ROWS);      // If we don't have enough rows fill the rest of the grid with blank ones? Easy enough.
    else
        ui->tblTransactionTableData->setRowCount(rows);

    QStringList headersStr;
    headersStr << tr("Date/Time") << tr("Operator") << tr("Trans. Type") << tr("PLU") << tr("Weight") << tr("Price") << tr("Items");
    ui->tblTransactionTableData->setHorizontalHeaderLabels(headersStr);
    ids.clear();

    if(rows > 0)
    {
        ids                        = m.listIds().split(",");
        QVector<QString> dateTimes = TokenParser::splitStringTokens( m.listDateTimes() );
        QVector<QString> weights   = TokenParser::splitStringTokens( m.listWeights() );
        QVector<QString> prices    = TokenParser::splitStringTokens(  m.listPrices() );
        QStringList operators      = m.listOperators().split(",");
        QStringList types          = m.listTransTypes().split(",");
        QStringList plus           = m.listPLUs().split(",");
        QStringList items          = m.listItems().split(",");

        // fix crash
        rows = qMin( rows, ids.count() );
        rows = qMin( rows, dateTimes.count() );
        rows = qMin( rows, weights.count() );
        rows = qMin( rows, prices.count() );
        rows = qMin( rows, operators.count() );
        rows = qMin( rows, types.count() );
        rows = qMin( rows, plus.count() );
        rows = qMin( rows, items.count() );

        int curCol;
        for(int j = 0; j < rows; ++j)
        {
            curCol = 0;                                                     // always reset to 0 each time around
            QTableWidgetItem *dateItem = new QTableWidgetItem;
            dateItem->setText(dateTimes.at(j));
            ui->tblTransactionTableData->setItem(j, curCol++, dateItem);    // add to row 0, col 0, then advance curCol

            QTableWidgetItem *nameItem = new QTableWidgetItem;
            QString tmpStr = operators.at(j);
            tmpStr = tmpStr.size() ? tmpStr : tr("Unknown");
            nameItem->setText(tmpStr);
            ui->tblTransactionTableData->setItem(j, curCol++, nameItem);     // add to row 0, col 1, then advance curCol

            QTableWidgetItem *transItem = new QTableWidgetItem;
            transItem->setText( retrieveTransactionType(types.at(j).toInt()) );
            ui->tblTransactionTableData->setItem(j, curCol++, transItem);    // add to row 0, col 2, then advance curCol

            QTableWidgetItem *pluItem = new QTableWidgetItem;
            pluItem->setText(plus.at(j));
            ui->tblTransactionTableData->setItem(j, curCol++, pluItem);      // add to row 0, col 3, then advance curCol

            QTableWidgetItem *weightItem = new QTableWidgetItem;
            tmpStr = weights.at(j);
            util->youGottaSetTheSeparator(tmpStr);
            weightItem->setText(tmpStr);
            ui->tblTransactionTableData->setItem(j, curCol++, weightItem);    // add to row 0, col 4, then advance curCol

            QTableWidgetItem *priceItem = new QTableWidgetItem;
            tmpStr = prices.at(j);
            util->youGottaSetTheSeparator(tmpStr);
            priceItem->setText( tmpStr );
            ui->tblTransactionTableData->setItem(j, curCol++, priceItem);      // add to row 0, col 5, then advance curCol

            QTableWidgetItem *itemItem = new QTableWidgetItem;
            itemItem->setText(items.at(j));
            ui->tblTransactionTableData->setItem(j, curCol++, itemItem);       // add to row 0, col 6, then advance curCol
        }
    }
    else
    {
        session_->send("103,154,151002,5");                                    // pop a dialog telling the user we have no records
    }

    requestingData = false;
    ui->cboOperatorSelector->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperVoidTransactions::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::voidSelectedTransactions()
{
    int numOf = selectedRows.size();

    if ( numOf > 0 && ids.size() > 0 )                                  // if we have items to delete
    {
        QString list = "";
        QString idVal = "";

        for ( int j = 0; j < numOf; ++j )
        {
            idVal = ids[ selectedRows.at(j).toInt() ];
            list += idVal;

            if( (j+1) < numOf)
                list += ",";
        }

        ui->cboOperatorSelector->setEnabled(false);


        session_->send(TransactionDev,TransactionEventVoidTransactions, "\"" + QString::number(numOf) + ",\"\"" + list + "\"\"\"");
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::updateOperators(const SessionMessage &msg)
{
    // Called when we make a request for the list of operators, 102,1 in initDialogData().
    // Fills out the operator combo box in the Filter Dialog

    DomOperatorInfomation m( msg );

    int numOfOperators = m.numOfIDs();
    operatorIDs    = m.allOperatorIDs().split(",");
    operatorNames  = m.allOperatorNames().split(",");

    canOperatorCombosReceiveEvents = false;                         // while updating

    ui->cboDialogOperatorSelector->addItem( tr("All operators") );
    ui->cboOperatorSelector->addItem( tr("All operators") );

    for ( int j = 0; j < numOfOperators; ++j )
    {
        if (operatorNames.isEmpty() || operatorNames.at(j).length() == 0 )
        {
            ui->cboDialogOperatorSelector->addItem( operatorIDs.at(j) );
            ui->cboOperatorSelector->addItem( operatorIDs.at(j) );
        }
        else
        {
            ui->cboDialogOperatorSelector->addItem( operatorNames.at(j) );
            ui->cboOperatorSelector->addItem( operatorNames.at(j) );
        }
    }

    canOperatorCombosReceiveEvents = true;                         // good to go

    // Now enable the corresponding controls
    receivedOperators = true;
    enableAllControls();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::updateTransactionTypeNames(const SessionMessage &msg)
{
    // We receied a 152,21 from the Scale Application - Transaction Type Message

    DomTotalTypesList m( msg );   // access with: msg[0].toInt() OR     m.myMethodVariable().toInt();

    int transTypeNum = 0;

    int transTypesTotal = m.numOfRecordIds();                       // How many transactions are there?
    QStringList transTypeNumberList = m.allRecordIds().split(",");  // The list of Operator IDs
    QStringList transTypeName1List = m.namesList().split(",");      // list of primary language names
    QStringList transTypeName2List = m.secNamesList().split(",");   // secondary trans list names

    transTypesTotal = qMin( transTypesTotal, transTypeNumberList.size() );
    transTypesTotal = qMin( transTypesTotal, transTypeName1List.size() );
    transTypesTotal = qMin( transTypesTotal, transTypeName2List.size() );

    for(int j = 0; j < transTypesTotal; ++j)
    {
        transTypeNum = transTypeNumberList.at(j).toInt();

        transTypes1[transTypeNum] = transTypeName1List.at(j);
        transTypes2[transTypeNum] = transTypeName2List.at(j);
    }

    doneInitializingCombo = true;

    receivedTransTypesData = true;
    enableAllControls();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::checkCheckbox(int state)
{
    if(state == CHECKED)
        ui->dialogPluNumberInput->setEnabled(false);
    else if(state == NOT_CHECKED)
        ui->dialogPluNumberInput->setEnabled(true);

    evaluateRetrieveButton( ui->dialogPluNumberInput->text() );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::filterDialogCallback()
{
    // THIS IS WHERE THE DIALOG OPTIONS ARE PUT TOGETHER TO MAKE A REQUEST

    // Request Operator Changes Summary
    // Message paremeters: PLU#,(or nothing for 'All Products'),
    // transaction type (0 for Production[all] or trans type num otherwise)
    // Start Date/Time (or nothing for 'no limit'), End Date/Time (or nothing for 'no limit')

    if(doneInitializingCombo)
    {

        // Message parameters: PLU number (or nothing for 'All Products'), Operator ID (or nothing
        // for 'All Operators'), Start Date/Time (or nothing for 'no limit'), End Date/Time (or
        // nothing for 'no limit'), Prev/First/Next (i.e., -1/0/1), Number of rows
        // NOTE jhc, 01-09-2013: The HLX requested 11 transactions at time because we could not do scroll bars
        // very well with tables and the page was slow to display the results. So we had to go thru page after
        // page of transactions selecting a stupid NEXT button time after time. Uggghhhhh. I am going to
        // request the entire list as FIRST, and make the limit 10,000 items.  I will find out if there is a
        // default parameter i can pass in that means the entire list, like 0, blank, or -1.

        int tmpHour = 0;
        int tmpMin = 0;

        QString opId = "";
        if( ui->cboDialogOperatorSelector->isVisible() )            // only true the first dismissing the dialog box
        {
            if( ui->cboDialogOperatorSelector->currentIndex() == 0)
            {
                opId = "";
                selectedDiaogOperator = 0;
            }
            else
            {
                opId = operatorIDs.at( ui->cboDialogOperatorSelector->currentIndex()-1 );
                selectedDiaogOperator = opId.toInt();
            }
        }
        else
        {
            if( ui->cboOperatorSelector->currentIndex() == 0)
                opId = "";
            else
                opId = operatorIDs.at( ui->cboOperatorSelector->currentIndex()-1 );
        }

        QString pluNumber = ui->checkboxAllProducts->isChecked() ? "" : ui->dialogPluNumberInput->text();
        QStringList dates = transactionDates_->datesList().split(",");

        tmpHour = ui->dialogStartTime->time().hour();
        tmpMin = ui->dialogStartTime->time().minute();
        QString startDate = dates[ui->dialogStartDateSelector->currentIndex()] + " " + QString::number(tmpHour) + ":" + QString::number(tmpMin) + ":00";
        tmpHour = ui->dialogEndTime->time().hour();
        tmpMin = ui->dialogEndTime->time().minute();
        QString endDate = dates[ui->dialogEndDateSelector->currentIndex()] + " " + QString::number(tmpHour) + ":" + QString::number(tmpMin) + ":59";

        requestOperatorID = opId;                                   // save off the operator id
        requestPLUNumber = pluNumber;                               // save off the current PLU
        requestStartDate = startDate;                               // save off the start date
        requestEndDate = endDate;                                   // save off the end date

        ui->Dialog->setVisible(false);                              // Get rid of the dialogbox

        if(receivedTransTypesData)
        {
            // 151,20
            session_->send(TransactionDev,TransactionReadNonVoidGroup, pluNumber + "," +	requestOperatorID + ",\"" + requestStartDate + "\",\"" + requestEndDate + "\"" + ",0,0");
        }

        requestingData = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::evaluateRetrieveButton(QString newText)
{
    if(newText.length() > 6)
        return;             // this occurs when the new character gets put on, but before the first digit gets rolled off

    if ( receivedDates )
    {
        if ( ui->checkboxAllProducts->isChecked() )
            ui->retrieveDataButton->setEnabled(true);
        else
        {
            int plu = newText.toInt();

            if ( 0 < plu && plu <= 999999 )
                ui->retrieveDataButton->setEnabled(true);
            else
                ui->retrieveDataButton->setEnabled(false);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::updateEndDateSelector(int newIndex)
{
   int selectedIndex = newIndex;
   int endIndex = ui->dialogEndDateSelector->count();

   if(selectedIndex >= endIndex )
   {
       for(int i = endIndex; i <= selectedIndex; ++i)
       {
           ui->dialogEndDateSelector->addItem(ui->dialogStartDateSelector->itemText(i));
       }

   }

   // If there are too many options in the dialogEndDateSelector, remove them from the end
   int lastIndex = endIndex - 1;
   while(lastIndex > selectedIndex)
   {
        ui->dialogEndDateSelector->removeItem(lastIndex);
        lastIndex = ui->dialogEndDateSelector->count() - 1;
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::selectTheRow(int row, int col)
{
    Q_UNUSED( col );

    if( !requestingData )
    {
        if(row >= 0 && row <= numOfTrans)
        {
            // See if the row is already in our list of rows array
            bool foundIt = false;
            for(int j = 0; j < selectedRows.size(); ++j)
            {
                if(selectedRows.at(j).toInt() == row)
                {
                    //selectedRows.remove(j);                     // now search the vector for this row item and then remove it
                    selectedRows.clear();                           // have to clear all items b\c of behavior of clearSelection.
                    foundIt = true;
                    ui->tblTransactionTableData->clearSelection();  // clears all selected items, no way to clear one.

                    // The only way to clear just one item would be clunky but it could be done if QA insists.
                    // 1. do clearSelection() which clears all.
                    // 2. remove the single item commented out above
                    // 3. iterate thru the remaining items in the selectedRows vector,
                    //    and reselect each one, one by one with .selectRow(row)
                    break;
                }
            }

            if(!foundIt)
            {
                // Then select the row and add it to the list of rows selected
                ui->tblTransactionTableData->selectRow(row);
                 selectedRows.append( QString::number(row) );
            }

            if(selectedRows.size() > 0)
                ui->butVoidTransactions->setEnabled(true);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::updateOperatorSelection(int index)
{
    Q_UNUSED( index );

    if( canOperatorCombosReceiveEvents )
        filterDialogCallback();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperVoidTransactions::onSessionMessage( const SessionMessage& msg )
{
    if (( OperatorDev == msg[0].toInt() ) && ( OpDtWriteAll == msg[1].toInt() ))
    {
        // 102,3 - update list of operators in the combo
        updateOperators(msg);
    }
    else if (( TransactionDev == msg[0].toInt() ) && ( TransactionWriteNonVoidGroup == msg[1].toInt() ))
    {
        // 151,21 - update the void transactions table
        updateVoidableTransactions(msg);
    }
    else if(( TransactionTypeDev == msg[0].toInt() ) && ( TransactionTypeWriteAllNames == msg[1].toInt() ))
    {
        // 152,21 - Operator Details
        updateTransactionTypeNames(msg);
    }
    else if (( TransactionDev == msg[0].toInt() ) && ( WriteOpertorChangesGroup == msg[1].toInt() ))
    {
        // 151,31 - Update dates in date range combos
        if( receivedDates )
            updateDialogsDueToLanguageChange(msg);
        else
            updateTransactionDates(msg);
    }
}
