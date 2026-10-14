/* @file superHourlyTotals.cpp
 * Hourly Totals tracks all transactions made hour by hour. A printed label
 * represents a single transaction. The fields tracked for Hourly Transactions include
 * the hour the transaction occurred, total weight of all transctions, total price
 * of all transcactions a specific hour, the number of items an hour, and the number of
 * transactions per hour. The summary information of all of these fields are displayed 
 * at the bottom for a total of all hours searched in a range.
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 */

#include "superHourlyTotals.h"
#include "ui_superHourlyTotals.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>

static const int DEFAULT_ROWS = 7;
static const int DEFAULT_COLS = 5;
static const int NOT_CHECKED = 0;
static const int CHECKED = 2;
static const int NUMBER_OF_ROWS = 1000;                               // crank this number up to see if we can have as many as we want


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart{
    class numericLineEdit;
}

SuperHourlyTotals::SuperHourlyTotals(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperHourlyTotals),
    transactionDates_(NULL)
{
    // saveOnExit_(false),
    ui->setupUi(this);
    scaleScreen();

    // retrieve config object
    if ( mainWindow_->isSupervisorMode() )
        transactionDates_ = &dynamic_cast<SupervisorMode*>( mode_ )->transactionDates_;

    initialize();
    translate();
}

SuperHourlyTotals::~SuperHourlyTotals()
{
    // May have to hide a keypad - find out how. Haven't triggered it yet
    //if(( saveOnExit_ ) && (transactionDates_))
    //    mainWindow_->hideKeyboard();

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::initialize()
{
    // initialize vars
    requestingData = false;											// when retrieve data button is selected
    receivedTransTypesData = false;									// indicates we have received the transaction names
    receivedDates = false;											// indicates we have received the list of dates
    doneInitializingCombo = false;									// are the transaction types updated yet? Ignores SIGNALS until then
    numOfOpers = 0;
    transTypesMax = 0;
    requestPLUNumber = "";                                          // current PLU
    requestOperatorID = "";                                         // current Operator ID
    requestStartDate = "";                                          // current Start Date
    requestEndDate = "";                                            // current End Date
    transTypeNumberList.clear();
    ui->tblHourlyTotals->clear();
    ui->tblHourlyTotalsTotals->clear();

    // create all of the checkbox signals and slots
    connect(ui->retrieveDataButton,         SIGNAL(clicked()),                this, SLOT(filterDialogCallback()));            // click retrieve data on dialog
    connect(ui->cboTransactionTypeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(filterDialogCallback()));            // select a different value in the combo
    connect(ui->dialogPluNumberInput,       SIGNAL(textChanged(QString)),     this, SLOT(evaluateRetrieveButton(QString)));   // The PLU field was changed
    connect(ui->dialogStartDateSelector,    SIGNAL(currentIndexChanged(int)), this, SLOT(updateEndDateSelector(int)) );       // The date changed
    connect(ui->checkboxAllProducts,        SIGNAL(stateChanged(int)),        this, SLOT(checkCheckbox(int)) );               // clicked the checkbox for all product

    initDialog();                                                   // initialize the dialog

    // set validator for PLU field
    ui->dialogPluNumberInput->setValidator(dialogPluNumberValidator_ = new QIntValidator(0,INT_MAX, this) );
    ui->dialogPluNumberInput->setDecimalPlaces(0);
    ui->dialogPluNumberInput->setInputWrapping(true);
    dialogPluNumberValidator_->setTop(999999);

    // Set a default number of Rows and Cols, otherwise won't be able to set the headers
    ui->tblHourlyTotals->setRowCount(DEFAULT_ROWS);
    ui->tblHourlyTotals->setColumnCount(DEFAULT_COLS);

    // Headers for changes table
    for (int i = 0; i < DEFAULT_COLS; i++)
    {
        ui->tblHourlyTotals->setColumnWidth( i, scaleWidthForScreen( 172 ) );
        if(i == 0)
             ui->tblHourlyTotalsTotals->setColumnWidth( 0, scaleWidthForScreen( 192 ) );
        else
            ui->tblHourlyTotalsTotals->setColumnWidth( i, scaleWidthForScreen( 172 ) );
    }

    // Update the label for the Totals Header ROW at the bottom
    QTableWidgetItem *totalItem = new QTableWidgetItem;
    totalItem->setText( tr("Totals") );
    ui->tblHourlyTotalsTotals->setItem(0, 0, totalItem);

    session_->send(TransactionTypeDev,TransactionReadNonVoidGroup);	// 152,20 - Request Transaction Type names

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperHourlyTotals::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::translate()
{
	ui->retranslateUi( this );

    // update the start and end date combos on the dialog
    if(receivedDates)
        session_->send(TransactionDev, TransactionReadDates);           // 151,30 -> get date ranges fresh in the correct langguage

    // update the table titles
    QStringList headersStr;
    headersStr << tr("Hour") << tr("Weight") << tr("Price") << tr("Items") << tr("Transactions");
    ui->tblHourlyTotals->setHorizontalHeaderLabels(headersStr);

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::updateHourlyTransactions(const SessionMessage& msg)
{
    // updates the grid if there are transactions to update the grid of Transactions
    // Called from domSuper, by 153,21 as a reply to 153,20, which is called by
    // when the user clicks the retrieve data button

    session_->send(Error,ErrRemoveAnyModalDialogDevice,"153001,5");   // remove message saying updating

    DomHourlyTotalsData m( msg );

    ui->tblHourlyTotals->clear();
    ui->tblHourlyTotalsTotals->clear();

    // Split up the lists
    int numOfHours           = m.numOfHours().toInt();
    QStringList hours        = m.hoursList().split(",");
    QVector<QString> weights = TokenParser::splitStringTokens( m.weightsList() );
    QVector<QString> prices  = TokenParser::splitStringTokens( m.pricesList() );
    QStringList items        = m.itemsList().split(",");
    QStringList trans        = m.transactionList().split(",");

    float grandWeights = 0;
    float grandPrices = 0;
    int grandItems = 0;
    int grandTrans = 0;

    ui->tblHourlyTotalsTotals->setRowCount(1);                   // only one row for the totals at the bottom
    ui->tblHourlyTotalsTotals->setColumnCount(DEFAULT_COLS);

    int rows =  numOfHours;

    // FT-488
    rows = qMin( rows, hours.size() );
    rows = qMin( rows, weights.size() );
    rows = qMin( rows, prices.size() );
    rows = qMin( rows, items.size() );
    rows = qMin( rows, trans.size() );

    if (rows < DEFAULT_ROWS)
       ui->tblHourlyTotals->setRowCount(DEFAULT_ROWS);
    else
    	ui->tblHourlyTotals->setRowCount(rows);

    ui->tblHourlyTotals->setColumnCount(DEFAULT_COLS);

    // Set the labels for the main table - appears anytime I resize the table I lose my headers, so redo each time
    QStringList headersStr;
    headersStr << tr("Hour") << tr("Weight") << tr("Price") << tr("Items") << tr("Transactions");
    ui->tblHourlyTotals->setHorizontalHeaderLabels(headersStr);

    // update the table if operator changes exist
    if (numOfHours > 0 )
    {
        int curCol;
        for(int j = 0; j < rows; j++)
        {                                                               // Example of first time though for updating a row
            curCol = 0;                                                 // always reset to 0 each time around
            QTableWidgetItem *hourItem = new QTableWidgetItem;
            hourItem->setText(hours.at(j));
            ui->tblHourlyTotals->setItem(j, curCol++, hourItem);     // add to row 0, col 0, then advance curCol

            QTableWidgetItem *weightItem = new QTableWidgetItem;
            weightItem->setText(weights.at(j));
            ui->tblHourlyTotals->setItem(j, curCol++, weightItem);    // add to row 0, col 1, then advance curCol

            QTableWidgetItem *priceItem = new QTableWidgetItem;
            priceItem->setText(prices.at(j));
            ui->tblHourlyTotals->setItem(j, curCol++, priceItem);      // add to row 0, col 2, then advance curCol

            QTableWidgetItem *itemItem = new QTableWidgetItem;
            itemItem->setText(items.at(j));
            ui->tblHourlyTotals->setItem(j, curCol++, itemItem);    // add to row 0, col 3, then advance curCol

            QTableWidgetItem *transItem = new QTableWidgetItem;
            transItem->setText(trans.at(j));
            ui->tblHourlyTotals->setItem(j, curCol++, transItem);  // add to row 0, col 3, then advance curCol

            grandWeights += weights.at(j).toFloat();               // update overall total values
            grandPrices  += prices.at(j).toFloat();
            grandItems   += items.at(j).toInt();
            grandTrans   += trans.at(j).toInt();
        }
     }
    else
    {
        session_->send(Error, HourlyTotalDev, ",153002,5");         // 103,153
    }

    // we have hourly transactions so update the GrandTotals
    // Update the label for the Totals Header ROW at the bottom
    QTableWidgetItem *totalItem = new QTableWidgetItem;
    totalItem->setText( tr("Totals") );
    ui->tblHourlyTotalsTotals->setItem(0, 0, totalItem);

    QTableWidgetItem *totalWeightItem = new QTableWidgetItem;
    totalWeightItem->setText(QString::number(grandWeights, 'f', 2));
    ui->tblHourlyTotalsTotals->setItem(0, 1, totalWeightItem);

    QTableWidgetItem *totalPriceItem = new QTableWidgetItem;
    totalPriceItem->setText(QString::number(grandPrices, 'f', 2));
    ui->tblHourlyTotalsTotals->setItem(0, 2, totalPriceItem);

    QTableWidgetItem *grandItemItem = new QTableWidgetItem;
    grandItemItem->setText(QString::number(grandItems));
    ui->tblHourlyTotalsTotals->setItem(0, 3, grandItemItem);

    QTableWidgetItem *grandTransItem = new QTableWidgetItem;
    grandTransItem->setText(QString::number(grandTrans));
    ui->tblHourlyTotalsTotals->setItem(0, 4, grandTransItem);


    // If we don't have enough rows, should we fill the rest of the grid with blank ones? Easy enough.

    requestingData = false;
    ui->cboTransactionTypeSelector->setEnabled(true);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::enableControl(QWidget &widget)
{
    widget.setDisabled(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::disableControl(QWidget& widget)
{
    widget.setDisabled(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                              DIALOG BOX METHODS
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::initDialog()
{
    ui->Dialog->setVisible(true);
    ui->dialogStartDateSelector->addItem(tr("Today"));
    ui->dialogStartDateSelector->addItem(tr("Yesterday"));
    ui->dialogEndDateSelector->addItem(tr("Today"));

    disableAllControls();                                           // when requesting date ranges
    session_->send(TransactionDev, TransactionReadDates);           // 151,30 -> get date ranges
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::updateTransactionDates(const SessionMessage& msg)
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

    updateEndDateSelector(0);

    // Now enable the corresponding controls
    receivedDates = true;
    enableAllControls();

    // get the transactionDates class to reuse it later
    if ( transactionDates_ )
        *transactionDates_ = m;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::updateDialogsDueToLanguageChange(const SessionMessage& msg)
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperHourlyTotals::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::disableAllControls()
{
    ui->dialogPluNumberInput->setEnabled(false);
    ui->checkboxAllProducts->setEnabled(false);
    ui->dialogStartDateSelector->setEnabled(false);
    ui->dialogEndDateSelector->setEnabled(false);
    ui->dialogEndDateSelector->setEnabled(true);
    ui->dialogStartTime->setEnabled(true);
    ui->retrieveDataButton->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::enableAllControls()
{

    if ( receivedDates )
    {
        ui->dialogPluNumberInput->setEnabled(true);
        ui->checkboxAllProducts->setEnabled(true);
        ui->dialogStartDateSelector->setEnabled(true);
        ui->dialogEndDateSelector->setEnabled(true);
        ui->dialogStartTime->setEnabled(true);
        ui->dialogEndTime->setEnabled(true);
        ui->retrieveDataButton->setEnabled(true);
        evaluateRetrieveButton( ui->dialogPluNumberInput->text() );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperHourlyTotals::retrieveTransactionType(int typeNumber)
{
    QString typeName = "";

    ScaleConfiguration *myScaleConfig = session_->scaleConfig;
    int curLang = myScaleConfig->currentLanguage;

    if( receivedTransTypesData )
    {
        if( (0 < typeNumber) && (typeNumber <= transTypesMax) )
        {
            typeName = curLang ? transTypes1.at(typeNumber) : transTypes2.at(typeNumber);
        }
    }

    if ( typeName.length() == 0 )
        typeName = tr("Unknown");

    return typeName;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::updateTransactionTypeNames(const SessionMessage &msg)
{
    // We receied a 152,21 from the Scale Application - Transaction Type Message

    DomTotalTypesList m( msg );   // access with: msg[0].toInt() OR     m.myMethodVariable().toInt();

    receivedTransTypesData = true;
    int transTypeNum = 0;

    int transTypesTotal = m.numOfRecordIds();                       // How many transactions are there?
    transTypeNumberList = m.allRecordIds().split(",");              // The list of Transaction Record IDs
    QStringList transTypeName1List = m.namesList().split(",");      // list of primary language names
    QStringList transTypeName2List = m.secNamesList().split(",");   // secondary trans list names

    for(int j = 0; j < transTypesTotal; ++j)
    {
        transTypeNum = transTypeNumberList.at(j).toInt();

        if(transTypes1.size() == j)
        {
            transTypes1.append (transTypeName1List.at(j) );
            transTypes2.append (transTypeName2List.at(j) );
        }
        else
        {
            transTypes1.insert(transTypeNum, transTypeName1List.at(j));  // wanted to just append but not
            transTypes2.insert(transTypeNum, transTypeName2List.at(j));  // positive if transTypeNums are linear
        }

        if(transTypesMax < transTypeNum)
            transTypesMax = transTypeNum;
    }

    // Now that we are done updating the list of transaction, insert All Transactions at id=0 into the first position
    transTypeNumberList.insert(0,"0");

    // Now that we have the list sorted all nice and neat, add it to the combo list

    ui->cboTransactionTypeSelector->addItem( tr("All Production") );

    if(transTypesTotal > 0)
    {
        ScaleConfiguration *myScaleConfig = session_->scaleConfig;
        int curLang = myScaleConfig->currentLanguage;
        if(curLang == 1)
        {
            for(int j = 0; j < transTypesTotal; ++j)
                ui->cboTransactionTypeSelector->addItem( transTypes1[j] );
        }
        else
        {
            for(int j = 0; j < transTypesTotal; ++j)
                ui->cboTransactionTypeSelector->addItem( transTypes2[j] );
        }
    }

	doneInitializingCombo = true;

    // not sure why this is here. Copied over - but not needed so far. In fact causes issues.
    //filterDialogCallback();                                    // Navigate First Group
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::checkCheckbox(int state)
{
    if(state == CHECKED)
        ui->dialogPluNumberInput->setEnabled(false);
    else if(state == NOT_CHECKED)
        ui->dialogPluNumberInput->setEnabled(true);

    evaluateRetrieveButton( ui->dialogPluNumberInput->text() );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::filterDialogCallback()
{
    // THIS IS WHERE THE DIALOG OPTIONS ARE PUT TOGETHER TO MAKE A REQUEST

    // Request Operator Changes Summary
    // Message paremeters: PLU#,(or nothing for 'All Products'),
    // transaction type (0 for Production[all] or trans type num otherwise)
    // Start Date/Time (or nothing for 'no limit'), End Date/Time (or nothing for 'no limit')

	if(doneInitializingCombo)
	{
	    int tmpHour = 0;
	    int tmpMin = 0;
        QString transNameIndex = "";

	    QString pluNumber = ui->checkboxAllProducts->isChecked() ? "" : ui->dialogPluNumberInput->text();
	    QStringList dates = transactionDates_->datesList().split(",");

	    tmpHour = ui->dialogStartTime->time().hour();
	    tmpMin = ui->dialogStartTime->time().minute();
        QString startDate = dates[ui->dialogStartDateSelector->currentIndex()] + " " + QString::number(tmpHour) + ":" + QString::number(tmpMin) + ":00";
	    tmpHour = ui->dialogEndTime->time().hour();
	    tmpMin = ui->dialogEndTime->time().minute();
        QString endDate = dates[ui->dialogEndDateSelector->currentIndex()] + " " + QString::number(tmpHour) + ":" + QString::number(tmpMin) + ":59";

	    requestPLUNumber = pluNumber;                                   // save off the current PLU
	    requestStartDate = startDate;                                   // save off the start date
	    requestEndDate = endDate;                                       // save off the end date

        int recordNum = ui->cboTransactionTypeSelector->currentIndex();
        transNameIndex = transTypeNumberList[recordNum];

    	ui->Dialog->setVisible(false);									// Get rid of the dialogbox

	    if(receivedTransTypesData)
	    {
	        // Pop Dialog indicating requesting information
	        session_->send(Error, HourlyTotalDev, ",153001,5");			// 103,153 - pop a dialog telling the user we are getting records

	        // 153,20
            session_->send(HourlyTotalDev,ReadFilteredHourlyTotals, pluNumber + "," + transNameIndex + ",\"" + startDate + "\",\"" + endDate + "\"");
	    }

	    requestingData = true;
	}
}

// GOOD /////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::evaluateRetrieveButton(QString newText)
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
void SuperHourlyTotals::updateEndDateSelector(int newIndex)
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


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperHourlyTotals::onSessionMessage( const SessionMessage& msg )
{

    if (( TransactionDev == msg[0].toInt() ) && ( WriteOpertorChangesGroup == msg[1].toInt() ))
    {
        // 151,31 - Update dates in date range combos
        if( receivedDates )
            updateDialogsDueToLanguageChange(msg);
        else
            updateTransactionDates(msg);
    }
    else if(( TransactionTypeDev == msg[0].toInt() ) && ( TransactionTypeWriteAllNames == msg[1].toInt() ))
    {
        // 152,21 - Operator Details
        updateTransactionTypeNames(msg);
    }
    else if (( HourlyTotalDev == msg[0].toInt() ) && ( WriteFilteredHourlyTotals == msg[1].toInt() ))
    {
        // 153,21 - Operator Changes
        updateHourlyTransactions(msg);
    }
}
