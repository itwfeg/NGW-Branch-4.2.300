/* @file superProductTotals.cpp
 * Product Totals tracks all transactions per PLU. A printed label
 * represents a single transaction. The fields tracked for Product Transactions include
 * the transaction type, total weight of all transctions, total price
 * of all transcactions, the number of items, and the number of
 * transactions. The summary information of all of these fields are displayed
 * at the bottom for a total of all products searched in a range.
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 */

#include "superProductTotals.h"
#include "ui_superProductTotals.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>

static const int DEFAULT_ROWS = 9;
static const int DEFAULT_COLS = 5;
static const int NUMBER_OF_ROWS = 1000;                               // crank this number up to see if we can have as many as we want

namespace hobart{
    class numericLineEdit;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

SuperProductTotals::SuperProductTotals(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperProductTotals),
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

SuperProductTotals::~SuperProductTotals()
{
    // May have to hide a keypad - find out how. Haven't triggered it yet
    //if(( saveOnExit_ ) && (transactionDates_))
    //    mainWindow_->hideKeyboard();

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::initialize()
{
    // initialize vars
    requestingData = false;											// when retrieve data button is selected
    receivedTransTypesData = false;									// indicates we have received the transaction names
    receivedDates = false;											// indicates we have received the list of dates
    doneInitializingCombo = false;									// are the transaction types updated yet? Ignores SIGNALS until then
    numOfOpers = 0;
    requestPLUNumber = "";                                          // current PLU
    requestOperatorID = "";                                         // current Operator ID
    requestStartDate = "";                                          // current Start Date
    requestEndDate = "";                                            // current End Date

    ui->tblProductTotals->clear();
    ui->tblProductTotalsTotals->clear();

    // create all of the checkbox signals and slots
    connect(ui->retrieveDataButton,         SIGNAL(clicked()),                this, SLOT(filterDialogCallback()));            // click retrieve data on dialog
    connect(ui->dialogPluNumberInput,       SIGNAL(textChanged(QString)),     this, SLOT(evaluateRetrieveButton(QString)));   // The PLU field was changed
    connect(ui->dialogStartDateSelector,    SIGNAL(currentIndexChanged(int)), this, SLOT(updateEndDateSelector(int)) );       // The date changed
    connect(ui->retrieveDataAgain,          SIGNAL(clicked()),                this, SLOT(updatePluFieldData()));              // click retrieve data on dialog

    initDialog();                                                   // initialize the dialog

    ui->pluNumberEntry->installEventFilter( this );
    ui->dialogPluNumberInput->setValidator(new QIntValidator(1, 999999, this));
    ui->pluNumberEntry->setValidator(new QIntValidator(1, 999999, this));

    // Set a default number of Rows and Cols, otherwise won't be able to set the headers
    ui->tblProductTotals->setRowCount(DEFAULT_ROWS);
    ui->tblProductTotals->setColumnCount(DEFAULT_COLS);

    // Headers for changes table
    for (int i = 0; i < DEFAULT_COLS; i++)
    {
        ui->tblProductTotals->setColumnWidth( i, scaleWidthForScreen( 172 ) );
        if(i == 0)
             ui->tblProductTotalsTotals->setColumnWidth( 0, scaleWidthForScreen( 192 ) );
        else
            ui->tblProductTotalsTotals->setColumnWidth( i, scaleWidthForScreen( 172 ) );
    }

    // Update the label for the Totals Header ROW at the bottom
    QTableWidgetItem *totalItem = new QTableWidgetItem;
    totalItem->setText( tr("Totals") );
    ui->tblProductTotalsTotals->setItem(0, 0, totalItem);

    // setup validators
    ui->dialogPluNumberInput->setValidator(dialogPluNumberValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->dialogPluNumberInput->setDecimalPlaces(0);
    ui->dialogPluNumberInput->setInputWrapping(true);
    dialogPluNumberValidator_->setTop(999999);

    ui->pluNumberEntry->setValidator(pluNumberValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->pluNumberEntry->setDecimalPlaces(0);
    ui->pluNumberEntry->setInputWrapping(true);
    pluNumberValidator_->setTop(999999);

    // send off request for data
    session_->send(TransactionTypeDev,TransactionReadNonVoidGroup);	// 152,20 - Request Transaction Type names

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperProductTotals::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::translate()
{
	ui->retranslateUi( this );

    // update the start and end date combos on the dialog
    if(receivedDates)
        session_->send(TransactionDev, TransactionReadDates);           // 151,30 -> get date ranges fresh in the correct langguage

    // update the table titles
    QStringList headersStr;
    headersStr << tr("Trans. Type") << tr("Weight") << tr("Price") << tr("Items") << tr("Transactions");
    ui->tblProductTotals->setHorizontalHeaderLabels(headersStr);

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::updateProductTransactions(const SessionMessage& msg)
{
    // updates the grid if there are transactions to update the grid of Transactions
    // Called from domSuper, by 153,21 as a reply to 153,20, which is called by
    // when the user clicks the retrieve data button

    DomProductTotalsData m( msg );
    UiUtilities *util = session_->uiUtilities;

    ui->tblProductTotals->clear();
    ui->tblProductTotalsTotals->clear();

    // Split up the lists
    int numOfTrans           = m.numOfTrans().toInt();
    QStringList transList    = m.transList().split(",");
    QVector<QString> weights = TokenParser::splitStringTokens( m.weightsList() );
    QVector<QString> prices  = TokenParser::splitStringTokens( m.pricesList() );
    QStringList items        = m.itemsList().split(",");
    QStringList trans        = m.transTypeList().split(",");

    float grandWeights = 0;
    float grandPrices = 0;
    int grandItems = 0;
    int grandTrans = 0;

    ui->tblProductTotalsTotals->setRowCount(1);                   // only one row for the totals at the bottom
    ui->tblProductTotalsTotals->setColumnCount(DEFAULT_COLS);

    int rows =  numOfTrans;

    // If we don't have enough rows, should we fill the rest of the grid with blank ones? Easy enough.
    if (rows < DEFAULT_ROWS)
       ui->tblProductTotals->setRowCount(DEFAULT_ROWS);
    else
    	ui->tblProductTotals->setRowCount(rows);
    ui->tblProductTotals->setColumnCount(DEFAULT_COLS);

    // Set the labels for the main table
    QStringList headersStr;
    headersStr << tr("Trans. Type") << tr("Weight") << tr("Price") << tr("Items") << tr("Transactions");
    ui->tblProductTotals->setHorizontalHeaderLabels(headersStr);

    // update the table if operator changes exist
    if (numOfTrans > 0 )
    {
        int curCol;
        for(int j = 0; j < rows; j++)
        {
            curCol = 0;                                                 // always reset to 0 each time around
            QTableWidgetItem *transItem = new QTableWidgetItem;
            transItem->setText( retrieveTransactionType(transList.at(j).toInt() ) );
            ui->tblProductTotals->setItem(j, curCol++, transItem);     // add to row 0, col 0, then advance curCol

            QTableWidgetItem *weightItem = new QTableWidgetItem;
            QString tmpStr = weights.at(j);
            util->youGottaSetTheSeparator(tmpStr);
            weightItem->setText( tmpStr );
            ui->tblProductTotals->setItem(j, curCol++, weightItem);    // add to row 0, col 1, then advance curCol

            QTableWidgetItem *priceItem = new QTableWidgetItem;
            tmpStr = prices.at(j);
            util->youGottaSetTheSeparator(tmpStr);
            priceItem->setText(tmpStr);
            ui->tblProductTotals->setItem(j, curCol++, priceItem);      // add to row 0, col 2, then advance curCol

            QTableWidgetItem *itemItem = new QTableWidgetItem;
            itemItem->setText(items.at(j));
            ui->tblProductTotals->setItem(j, curCol++, itemItem);    // add to row 0, col 3, then advance curCol

            QTableWidgetItem *transTypesItem = new QTableWidgetItem;
            transTypesItem->setText(trans.at(j));
            ui->tblProductTotals->setItem(j, curCol++, transTypesItem);  // add to row 0, col 3, then advance curCol

            grandWeights += weights.at(j).toFloat();               // update overall total values
            grandPrices  += prices.at(j).toFloat();
            grandItems   += items.at(j).toInt();
            grandTrans   += trans.at(j).toInt();
        	
            session_->send(Error,ErrRemoveAnyModalDialogDevice,"156001,5");   // remove message saying updating
        }
     }
    else
    {
        session_->send(Error,ErrRemoveAnyModalDialogDevice,"156001,5");   // remove message saying updating
        session_->send(Error, ProductTotalDev, "156002,5");        // 103,156 - Tell user there are no records
    }

    // we have product transactions so update the GrandTotals
    // Update the label for the Totals Header ROW at the bottom
    QTableWidgetItem *totalItem = new QTableWidgetItem;
    totalItem->setText( tr("Totals") );
    ui->tblProductTotalsTotals->setItem(0, 0, totalItem);

    QTableWidgetItem *totalWeightItem = new QTableWidgetItem;
    QString tmpStr = QString::number(grandWeights);
    util->youGottaSetTheSeparator(tmpStr);
    totalWeightItem->setText(QString::number(tmpStr.toFloat(), 'f', 2));
    ui->tblProductTotalsTotals->setItem(0, 1, totalWeightItem);

    QTableWidgetItem *totalPriceItem = new QTableWidgetItem;
    tmpStr = QString::number(grandPrices);
    util->youGottaSetTheSeparator(tmpStr);
    totalPriceItem->setText(QString::number(tmpStr.toFloat(), 'f', 2));
    ui->tblProductTotalsTotals->setItem(0, 2, totalPriceItem);

    QTableWidgetItem *grandItemItem = new QTableWidgetItem;
    grandItemItem->setText(QString::number(grandItems));
    ui->tblProductTotalsTotals->setItem(0, 3, grandItemItem);

    QTableWidgetItem *grandTransItem = new QTableWidgetItem;
    grandTransItem->setText(QString::number(grandTrans));
    ui->tblProductTotalsTotals->setItem(0, 4, grandTransItem);

    ui->pluNumberEntry->setEnabled(true);
    requestingData = false;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::enableControl(QWidget &widget)
{
    widget.setDisabled(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::disableControl(QWidget& widget)
{
    widget.setDisabled(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                              DIALOG BOX METHODS
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::initDialog()
{
    ui->Dialog->setVisible(true);
    ui->dialogStartDateSelector->addItem(tr("Today"));
    ui->dialogStartDateSelector->addItem(tr("Yesterday"));
    ui->dialogEndDateSelector->addItem(tr("Today"));

    disableAllControls();                                           // when requesting date ranges
    session_->send(TransactionDev, TransactionReadDates);           // 151,30 -> get date ranges
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::updateTransactionDates(const SessionMessage& msg)
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
void SuperProductTotals::updateDialogsDueToLanguageChange(const SessionMessage& msg)
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
int SuperProductTotals::makeValidatorTop( int digits )
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
void SuperProductTotals::disableAllControls()
{
    ui->dialogPluNumberInput->setEnabled(false);
    ui->dialogStartDateSelector->setEnabled(false);
    ui->dialogEndDateSelector->setEnabled(false);
    ui->dialogEndDateSelector->setEnabled(true);
    ui->dialogStartTime->setEnabled(true);
    ui->retrieveDataButton->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::enableAllControls()
{

    if ( receivedDates )
    {
        ui->dialogPluNumberInput->setEnabled(true);
        ui->dialogStartDateSelector->setEnabled(true);
        ui->dialogEndDateSelector->setEnabled(true);
        ui->dialogStartTime->setEnabled(true);
        ui->dialogEndTime->setEnabled(true);
        ui->retrieveDataButton->setEnabled(true);
        evaluateRetrieveButton( ui->dialogPluNumberInput->text() );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperProductTotals::retrieveTransactionType(int typeNumber)
{
    QString typeName = "";

    ScaleConfiguration *myScaleConfig = session_->scaleConfig;
    int curLang = myScaleConfig->currentLanguage;

    if( receivedTransTypesData )
    {
        if( (0 < typeNumber) && (typeNumber + 1 <= transTypes1.size()) )
        {
            // FT-487
            typeName = curLang ? transTypes1.at(typeNumber) : transTypes2.at(typeNumber);
        }
    }

    if ( typeName.length() == 0 )
        typeName = tr("Unknown");

    return typeName;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::updateTransactionTypeNames(const SessionMessage &msg)
{
    // We receied a 152,21 from the Scale Application - Transaction Type Message

    DomTotalTypesList m( msg );   // access with: msg[0].toInt() OR     m.myMethodVariable().toInt();

    receivedTransTypesData = true;
    int transTypeNum = 0;

    int transTypesTotal = m.numOfRecordIds();                       // How many transactions are there?
    QStringList transTypeNumberList = m.allRecordIds().split(",");  // The list of Operator IDs
    QStringList transTypeName1List = m.namesList().split(",");      // list of primary language names
    QStringList transTypeName2List = m.secNamesList().split(",");   // secondary trans list names

    // There usually aren't many Total Types (Transaction Types) in a database,
    // so reserve space for enough, and I'll resize if I have to.
    transTypes1.resize(16);
    transTypes2.resize(16);

    for(int j = 0; j < transTypesTotal; ++j)
    {
        transTypeNum = transTypeNumberList.at(j).toInt();

        if ( transTypes1.size() <= transTypeNum )
        {
            transTypes1.resize(transTypeNum + 1);
            transTypes2.resize(transTypeNum + 1);
        }

        transTypes1[transTypeNum] = transTypeName1List.at(j);  // wanted to just append but not
        transTypes2[transTypeNum] = transTypeName2List.at(j);  // positive if transTypeNums are linear
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::filterDialogCallback()
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

        QString pluNumber = "";

	    QStringList dates = transactionDates_->datesList().split(",");

	    tmpHour = ui->dialogStartTime->time().hour();
	    tmpMin = ui->dialogStartTime->time().minute();

        // FT-487
        QString startDate = dates[ui->dialogStartDateSelector->currentIndex()] + " " + QString::number(tmpHour) + ":" + QString::number(tmpMin) + ":00";
	    tmpHour = ui->dialogEndTime->time().hour();
	    tmpMin = ui->dialogEndTime->time().minute();

        // FT-487
        QString endDate = dates[ui->dialogEndDateSelector->currentIndex()] + " " + QString::number(tmpHour) + ":" + QString::number(tmpMin) + ":59";


        requestStartDate = startDate;                               // save off the start date
        requestEndDate = endDate;                                   // save off the end date

        if (ui->Dialog->isVisible() )
        {
            // received the PLU number from the dialog
            ui->pluNumberEntry->setText( ui->dialogPluNumberInput->text() );
            requestPLUNumber = ui->dialogPluNumberInput->text();
            ui->Dialog->setVisible(false);					        // Get rid of the dialogbox
        }
        else
        {
            // received the plu number from the plu field on the screen - not dialog, so date range remains the same
            requestPLUNumber = ui->pluNumberEntry->text();
        }

        TEST_PLU = requestPLUNumber.toInt();                        // to use later when testing

	    if(receivedTransTypesData)
	    {
	        // Pop Dialog indicating requesting information
            session_->send(Error, ProductTotalDev, "156001,5");    // 103,156 - pop a dialog telling the user we are getting records

	        // 156,20
            // FT-487
            session_->send(ProductTotalDev,ReadFilteredProductTotals, requestPLUNumber + "," +	"0" + ",\"" + startDate + "\",\"" + endDate + "\"");
	    }

	    requestingData = true;
	}
}

void SuperProductTotals::updatePluFieldData()
{
    QString newPLU = ui->pluNumberEntry->text();

    if(newPLU != requestPLUNumber)
    {
        requestPLUNumber = newPLU;
        TEST_PLU = requestPLUNumber.toInt();                        // to use later when testing

        // Pop Dialog indicating requesting information
        session_->send(Error, ProductTotalDev, "156001,5");        // 103,156 - pop a dialog telling the user we are getting records

        // 156,20
        session_->send(ProductTotalDev,ReadFilteredProductTotals, requestPLUNumber + "," +	"0" + ",\"" + requestStartDate + "\",\"" + requestEndDate + "\"");

        requestingData = true;
    }
}


// GOOD /////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::evaluateRetrieveButton(QString newText)
{
    if(newText.length() > 6)
        return;             // this occurs when the new character gets put on, but before the first digit gets rolled off

    if ( receivedDates )
    {

      int plu = newText.toInt();

      if ( 0 < plu && plu <= 999999 )
          ui->retrieveDataButton->setEnabled(true);
      else
          ui->retrieveDataButton->setEnabled(false);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperProductTotals::updateEndDateSelector(int newIndex)
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
void SuperProductTotals::onSessionMessage( const SessionMessage& msg )
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
        // 152,21 - Transaction Names
        updateTransactionTypeNames(msg);
    }
    else if (( ProductTotalDev == msg[0].toInt() ) && ( WriteFilteredProductTotals == msg[1].toInt() ))
    {
        // 156,21 - Product Transactions
        updateProductTransactions(msg);
    }
}
