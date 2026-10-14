/* @file superOperatorChanges.cpp
 * Operator Changes tracks all changes by an operator to a PLU in operator mode.
 * The changes to the PLU can tracked by a single PLU or all PLUs together. Changes can
 * but the changes record is not committed until the operator presses print. The changes
 * tracked include the bycount, weight, net weight, and total price. The information
 * offered includes the operator who perfored the change, what product, what field,
 * and % gained and loss.
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 */

#include "superOperatorChanges.h"
#include "ui_superOperatorChanges.h"

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"

#include <numericlineedit.h>
#include "../dialogs/filedialog.h"
#include "python.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

#include <QtGui>
#include <QtCore>
#include <QIntValidator>

static const int DEFAULT_ROWS = 6;
static const int DEFAULT_COLS = 5;
static const int DEFAULT_DETAILS_ROWS = 7;
static const int DEFAULT_DETAILS_COLS = 7;
static const int NOT_CHECKED = 0;
static const int CHECKED = 2;
static const int NUMBER_OF_ROWS = 1000;                               // crank this number up to see if we can have as many as we want

namespace hobart{
    class numericLineEdit;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

SuperOperatorChanges::SuperOperatorChanges(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperOperatorChanges),
    transactionDates_(NULL)
{
    // saveOnExit_(false),
    ui->setupUi(this);
    scaleScreen();

    // retrieve config object
    if ( mainWindow_->isSupervisorMode() )
        transactionDates_ = &dynamic_cast<SupervisorMode*>( mode_ )->transactionDates_;

    // setup validators
    ui->dialogPluNumberInput->setValidator(dialogPluNumberValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->dialogPluNumberInput->setDecimalPlaces(0);
    ui->dialogPluNumberInput->setInputWrapping(true);
    dialogPluNumberValidator_->setTop(999999);

    initialize();
    translate();
}

SuperOperatorChanges::~SuperOperatorChanges()
{
    // May have to hide a keypad - find out how. Haven't triggered it yet
    //if(( saveOnExit_ ) && (transactionDates_))
    //    mainWindow_->hideKeyboard();

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::initialize()
{
    // initialize vars
    requestingData = false;
    requestingDetailsData = false;
    receivedTransTypesData = false;
    receivedDates = false;
    hasAlreadyBeenLoaded = false;                                   // indicates if DETAILS table has been loaded at least once

    numOfOpers = 0;
    whatPage = 1;                                                   // Operator Changes = 1, Operator Details = 2
    currentSelectedRow = -1;                                        // no row selected
    prevButtCtr = 0;                                                // what Details button we pressed
    requestPLUNumber = "";                                          // current PLU
    requestOperatorID = "";                                         // current Operator ID
    requestStartDate = "";                                          // current Start Date
    requestEndDate = "";                                            // current End Date

    ui->tblDetails->setVisible(false);
    ui->btnBack->setVisible(false);
    ui->btnPrevious->setVisible(false);                             // See note in SLOT sections
    ui->btnNext->setVisible(false);

    // create all of the checkbox signals and slots
    connect(ui->tblOperatorChanges,      SIGNAL(cellClicked(int,int)),     this, SLOT(selectTheRow(int,int)));             // click cell
    connect(ui->btnDetails,              SIGNAL(clicked()),                this, SLOT(transactionsDetails()));             // click details button
    connect(ui->retrieveDataButton,      SIGNAL(clicked()),                this, SLOT(filterDialogCallback()));            // click retrieve data on dialog
    connect(ui->dialogPluNumberInput,    SIGNAL(textChanged(QString)),     this, SLOT(evaluateRetrieveButton(QString)));   // The PLU field was changed
    connect(ui->dialogStartDateSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEndDateSelector(int)) );       // The date changed
    connect(ui->checkboxAllProducts,     SIGNAL(stateChanged(int)),        this, SLOT(checkCheckbox(int)) );               // clicked the checkbox for all product

    connect(ui->btnBack,                 SIGNAL(clicked()),                this, SLOT(goBackToSummary()));                 // clicked the Back button
    connect(ui->btnPrint,                SIGNAL(clicked()),                this, SLOT(printOperatorChanges()));
    connect(ui->btnExport,               SIGNAL(clicked()),                this, SLOT(exportOperatorChanges()));

    // See if we can get rid of these. Originally because we only showed one page of data at a time. Now we can scroll.
    // Need to test with big lists though, like over 100, to see we can be fast enough to update teh table.
    //connect(ui->btnNext,                 SIGNAL(clicked()),                this, SLOT(onClickedNext()));                   // clicked Next button
    //connect(ui->btnPrevious,             SIGNAL(clicked()),                this, SLOT(onClickedPrevious()));               // clicked Previous button

    scaleConfig_ = &session_->scaleConfig->scaleConfig;

    // initialize the dialog
    initDialog();

    ui->dialogPluNumberInput->setValidator(new QIntValidator(1, 999999, this));

    ui->lblUserInfo->setText("");
    clearOperatorChangesTable();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::clearOperatorChangesTable()
{
    ui->tblOperatorChanges->clear();
    ui->tblOperatorChangesTotals->clear();

    // Set a default number of Rows and Cols, otherwise won't be able to set the headers
    ui->tblOperatorChanges->setRowCount(DEFAULT_ROWS);
    ui->tblOperatorChanges->setColumnCount(DEFAULT_COLS);

    // Headers for changes table
    for (int i = 0; i < DEFAULT_COLS; i++)
    {
        ui->tblOperatorChanges->setColumnWidth( i, scaleWidthForScreen( 172 ) );
        if( i == 0 )
            ui->tblOperatorChangesTotals->setColumnWidth( i, scaleWidthForScreen( 187 ) );
        else
            ui->tblOperatorChangesTotals->setColumnWidth( i, scaleWidthForScreen( 172 ) );

        QTableWidgetItem *headerItem = new QTableWidgetItem;
        ui->tblOperatorChanges->setHorizontalHeaderItem(0, headerItem);
    }

    // Set the labels for the main table
    QString headersStr = tr("Operator;") + tr("Number of\n transactions;") + tr("Total before\n change;") + tr("Total\n Gain(Loss);") + tr("Percentage\n Gain(Loss)");
    ui->tblOperatorChanges->setHorizontalHeaderLabels(headersStr.split(";"));

    // Update the label for the Totals Header ROW at the bottom
    QTableWidgetItem *totalItem = new QTableWidgetItem;
    totalItem->setText( tr("Totals") );
    ui->tblOperatorChangesTotals->setItem(0, 0, totalItem);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperOperatorChanges::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::translate()
{
	ui->retranslateUi( this );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::updateOperatorChanges(const SessionMessage& msg)
{
    // For future reference - use if needed, didn't need the delegate here, so far. SEE HEADER
    // ui->tblOperatorChanges->setItemDelegate(new ItemDelegate());
    // QColor titleBackground(Qt::purple);
    // QFont titleFont = ui->tblOperatorChanges->font();
    // titleFont.setBold(true);
    // ui->tblOperatorChanges->item(0, 0)->setBackgroundColor(titleBackground);
    // ui->tblOperatorChanges->item(0, 0)->setFont(titleFont);

    DomOperatorChangesSummary m( msg );

    // Split up the lists
    operIDs   = m.opIdsList().split(",");
    QStringList names = m.opNamesList().split(",");
    QStringList trans = m.transactionList().split(",");
    QStringList oldtotals = m.oldTotalsList().split(",");
    QStringList deltas    = m.deltaList().split(",");

    numOfOpers = m.numOfOpIds().toInt();
    float grandTrans = 0;
    float grandStartValueTotals = 0;
    float grandGainLoss = 0;
    float grandPercentGainLoss = 0;
    float floatGain = 0;
    float floatStartValue = 0;
    float floatPercent = 0;

    ui->tblOperatorChangesTotals->setRowCount(1);                   // only one row for the totals at the bottom
    ui->tblOperatorChangesTotals->setColumnCount(DEFAULT_COLS);

    int rows =  m.numOfOpIds().toInt();

    ui->tblOperatorChanges->setRowCount(rows);
    ui->tblOperatorChanges->setColumnCount(DEFAULT_COLS);

    // update the table if operator changes exist
    int curCol;
    for(int j = 0; j < rows; j++)
    {                                                               // Example of first time though for updating a row
        curCol = 0;                                                 // always reset to 0 each time around
        QTableWidgetItem *nameItem = new QTableWidgetItem;
        nameItem->setText(names.at(j));
        ui->tblOperatorChanges->setItem(j, curCol++, nameItem);     // add to row 0, col 0, then advance curCol

        QTableWidgetItem *transItem = new QTableWidgetItem;
        transItem->setText(trans.at(j));
        ui->tblOperatorChanges->setItem(j, curCol++, transItem);    // add to row 0, col 1, then advance curCol

        QTableWidgetItem *oldItem = new QTableWidgetItem;
        oldItem->setText(oldtotals.at(j));
        ui->tblOperatorChanges->setItem(j, curCol++, oldItem);      // add to row 0, col 2, then advance curCol

        QTableWidgetItem *deltaItem = new QTableWidgetItem;
        deltaItem->setText(deltas.at(j));
        ui->tblOperatorChanges->setItem(j, curCol++, deltaItem);    // add to row 0, col 3, then advance curCol

        QTableWidgetItem *percentItem = new QTableWidgetItem;
        floatGain = deltas.at(j).toFloat();
        floatStartValue = oldtotals.at(j).toFloat();
        if(floatStartValue != 0)                                    // add to row 0, col 4, then advance curCol
        {
            floatPercent = ((floatGain /  floatStartValue) * 100);
            percentItem->setText(QString::number( floatPercent, 'f', 2 ) + "%");
            ui->tblOperatorChanges->setItem(j, curCol, percentItem);
        }

        grandTrans += trans.at(j).toFloat();                     // update overall total values
        grandStartValueTotals += floatStartValue;
        grandGainLoss += floatGain;
    }

    // Update the grand Totals Row
    if( numOfOpers == 0 )
    {
        // clear the tables if no operator changes
        clearOperatorChangesTable();
    }
    else
    {
        // we have operator changes so everything is okay, supposebly
        QTableWidgetItem *grandItem = new QTableWidgetItem;
        grandItem->setText(QString::number(grandTrans));
        ui->tblOperatorChangesTotals->setItem(0, 1, grandItem);

        QTableWidgetItem *totalPriceItem = new QTableWidgetItem;
        totalPriceItem->setText(QString::number(grandStartValueTotals, 'f', 2));
        ui->tblOperatorChangesTotals->setItem(0, 2, totalPriceItem);

        QTableWidgetItem *gainLossItem = new QTableWidgetItem;
        gainLossItem->setText(QString::number(grandGainLoss, 'f', 2));
        ui->tblOperatorChangesTotals->setItem(0, 3, gainLossItem);

        if(grandStartValueTotals != 0)
        {
            QTableWidgetItem *percentGainLossItem = new QTableWidgetItem;
            grandPercentGainLoss = ((grandGainLoss / grandStartValueTotals) * 100);
            percentGainLossItem->setText(QString::number(grandPercentGainLoss, 'f', 2) + "%");
            ui->tblOperatorChangesTotals->setItem(0, 4, percentGainLossItem);
        }

        //PTR #771 -> show no operator changes exists records message!!
        if (numOfOpers == 0 || trans.count() == 0 )     //either no operators or no transactions for the operators
            session_->send(Error, OperatorChangesDev, ",131002,5");
    }

    // If we don't have enough rows, should we fill the rest of the grid with blank ones? Easy enough.
    if (rows < 6)
       ui->tblOperatorChanges->setRowCount(6);

    requestingData = false;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::enableControl(QWidget &widget)
{
    widget.setDisabled(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::disableControl(QWidget& widget)
{
    widget.setDisabled(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                              DIALOG BOX METHODS
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::initDialog()
{
    // don't really know how to do this yet
    //domDatesForm = parent.dom_frame.document.forms["transactionDates"];

    // NEW CODE
    ui->dialogStartDateSelector->addItem(tr("Today"));
    ui->dialogStartDateSelector->addItem(tr("Yesterday"));
    ui->dialogEndDateSelector->addItem(tr("Today"));

    disableAllControls();                                           // when requesting date ranges
    session_->send(TransactionDev, TransactionReadDates);           // 151,30 -> get date ranges
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::updateTransactionDates(const SessionMessage& msg)
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::disableAllControls()
{
    if(whatPage == 1)                                               // Dialog
    {
        ui->dialogPluNumberInput->setEnabled(false);
        ui->checkboxAllProducts->setEnabled(false);
        ui->dialogStartDateSelector->setEnabled(false);
        ui->dialogEndDateSelector->setEnabled(false);
        ui->retrieveDataButton->setEnabled(false);
    }
    else if( whatPage == 2)
    {
        requestingDetailsData = true;                               // Disables selecting rows
        ui->btnBack->setEnabled(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::enableAllControls()
{

    if ( receivedDates )
    {
        ui->dialogPluNumberInput->setEnabled(true);
        ui->checkboxAllProducts->setEnabled(true);
        ui->dialogStartDateSelector->setEnabled(true);
        ui->dialogEndDateSelector->setEnabled(true);
        ui->retrieveDataButton->setEnabled(true);
        evaluateRetrieveButton( ui->dialogPluNumberInput->text() );

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                              OPERATOR CHANGES DETAILS METHODS
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void SuperOperatorChanges::initializeDetails()
{
    whatPage = 2;


    // Set a default number of Rows and Cols, otherwise won't be able to set the headers
    ui->tblDetails->setRowCount(DEFAULT_DETAILS_ROWS);
    ui->tblDetails->setColumnCount(DEFAULT_DETAILS_COLS);

     // Headers for Details table
    QVector<int> rowsizes;
    rowsizes << 212 << 122 << 72 << 82 << 202 << 82 << 82;
    for (int i = 0; i < DEFAULT_COLS; i++)
    {
        ui->tblDetails->setColumnWidth(i,rowsizes.at(i));
        QTableWidgetItem *headerItem = new QTableWidgetItem;
        ui->tblDetails->setHorizontalHeaderItem(0, headerItem);
    }

    // Set the labels for the main table
    QString headersStr = tr("Date\nTime;") + tr("Transaction\ntype;") + tr("PLU;") + tr("Total\nPrice;")+ tr("Changed\nField;") + tr("Original\nvalue;") + tr("New\nvalue;");
    ui->tblDetails->setHorizontalHeaderLabels(headersStr.split(";"));

    if(hasAlreadyBeenLoaded)
    {
        requestTransactionGroup("0");                               // Navigate First Group
    }
    else
    {
        hasAlreadyBeenLoaded = true;
        session_->send(TransactionTypeDev,TransactionReadNonVoidGroup);
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::updateOperatorChangesDetails(const SessionMessage &msg)
{
    // updates the grid if there are transactions to update the grid of Operator Details
    // Called by 131,31 as a reply to 131,30

    DomOperatorChangesDetails m( msg );   // access with: msg[0].toInt() OR     m.myMethodVariable().toInt();

    int numOfTrans = m.numOfTransIDs().toInt();
    ui->tblDetails->setRowCount(numOfTrans);                        // update num of rows based on num of transactions

    if(numOfTrans > DEFAULT_DETAILS_ROWS)
    {
        // make room for scroll bar
        ui->tblDetails->setColumnWidth( 4, scaleWidthForScreen( 172 ) );
        ui->tblDetails->setColumnWidth( 6, scaleWidthForScreen( 82 ) );
    }

    QStringList transIdList    = m.transIdList().split(",");

    QVector<QString> dateTimesList   = TokenParser::splitStringTokens( m.dateTimesList() );
    QStringList transTypesList  = m.transTypesList().split(",");
    QStringList pluList         = m.pluList().split(",");
    QVector<QString> totalPriceList  = TokenParser::splitStringTokens( m.totalPriceList() );
    QStringList changeFieldList = m.changeFieldList().split(",");

    QVector<QString> oldValsList    = TokenParser::splitStringTokens( m.oldValsList() );
    QVector<QString> newValsList    = TokenParser::splitStringTokens( m.newValsList() );
    QVector<QString> changeFieldArray;
    changeFieldArray  << tr("Unknown") << tr("Unit Price") << tr("Tare") << tr("By Count") << tr("Shelf Life Days")
                      << tr("Product Life Days") << tr("Primary Label Type") << tr("Secondary Label Type") << tr("Net Weight")
                      << tr("UPC Digit") << tr("Total Price") << tr("Label Graphic 1") << tr("Label Graphic 2") << tr("Label Graphic 3")
                      << tr("Label Graphic 4") << tr("Label Type 3") << tr("Shelf Life Hours") << tr("Product Life Hours")  << tr("2nd Tare");

    ui->lblTitle->setText(tr("Operator Changes for") + " " + m.operatorName());

    // THESE ARE FROM THE ASP
    //var currentRows = SupervisorScreen.tableBody.children;
    //var numberCurrentRows = currentRows.length;
    //var dataRow = null;
    QString theFieldName = "";
    int prevTransID = 0;
    int theFieldNum = 0;
    int curCol;


    UiUtilities *util = session_->uiUtilities;

    // update the table if operator changes exist
    for(int j = 0; j < numOfTrans; j++)
    {                                                               // Example of first time though for updating a row
        curCol = 0;                                                 // always reset to 0 each time around for each column
        if(prevTransID != transIdList.at(j).toInt() )
        {
            QTableWidgetItem *dateItem = new QTableWidgetItem;
            dateItem->setText( dateTimesList.at(j) );
            ui->tblDetails->setItem(j, curCol++, dateItem);         // add to row 0, col 0, then advance curCol

            QTableWidgetItem *transtypeItem = new QTableWidgetItem;
            transtypeItem->setText( retrieveTransactionType( transTypesList.at(j).toInt() )  );
            ui->tblDetails->setItem(j, curCol++, transtypeItem);    // add to row 0, col 1, then advance curCol

            QTableWidgetItem *pluItem = new QTableWidgetItem;
            pluItem->setText( pluList.at(j) );
            ui->tblDetails->setItem(j, curCol++, pluItem);          // add to row 0, col 2, then advance curCol

            QTableWidgetItem *totalPriceItem = new QTableWidgetItem;
            QString tmpStr = totalPriceList.at(j);
            totalPriceItem->setText( util->youGottaSetTheSeparator(tmpStr) );
            ui->tblDetails->setItem(j, curCol++, totalPriceItem);   // add to row 0, col 3, then advance curCol
        }
        else
        {
            // may not need this else. Carry over from legacy. Test with lost of changes
            // and see if I can get rid of this. I'm thinking it does not provide anything
            // so likely test it and then delete it.
            QTableWidgetItem *blankItem1 = new QTableWidgetItem;
            blankItem1->setText(" ");
            ui->tblDetails->setItem(j, curCol++, blankItem1);
            QTableWidgetItem *blankItem2 = new QTableWidgetItem;
            blankItem2->setText(" ");
            ui->tblDetails->setItem(j, curCol++, blankItem2);
            QTableWidgetItem *blankItem3 = new QTableWidgetItem;
            blankItem3->setText(" ");
            ui->tblDetails->setItem(j, curCol++, blankItem3);
            QTableWidgetItem *blankItem4 = new QTableWidgetItem;
            blankItem4->setText(" ");
            ui->tblDetails->setItem(j, curCol++, blankItem4);
        }

        prevTransID = transIdList.at(j).toInt();
        theFieldNum = changeFieldList.at(j).toInt();
        theFieldName = changeFieldArray.at( theFieldNum );

        QTableWidgetItem *fieldnameItem = new QTableWidgetItem;
        fieldnameItem->setText( theFieldName );
        ui->tblDetails->setItem(j, curCol++, fieldnameItem);     // add to row 0, col 4, then advance curCol

        QTableWidgetItem *oldItem = new QTableWidgetItem;
        QString tmpStr = "";
        if(oldValsList.at(j) == "")
           tmpStr = " " ;
        else
            tmpStr =  oldValsList.at(j);
         oldItem->setText( util->youGottaSetTheSeparator(tmpStr) );
         ui->tblDetails->setItem(j, curCol++, oldItem);          // add to row 0, col 5, then advance curCol

        QTableWidgetItem *newItem = new QTableWidgetItem;
        tmpStr = newValsList.at(j);
        newItem->setText( util->youGottaSetTheSeparator(tmpStr) );
        ui->tblDetails->setItem(j, curCol++, newItem);              // add to row 0, col 6, then advance curCol
    }

    // I am thinking of getting rid of the previous and next buttons all together.
    // See note in SLOT sections in constructor
    //if(prevButtCtr > 0 || m.previousAvailable().toInt() == 1)
    //    ui->btnPrevious->setEnabled(true);
    //else
    //    ui->btnPrevious->setEnabled(false);

    // See note in SLOT sections in constructor
    //if(m.nextAvailable().toInt() == 1)
    //    ui->btnNext->setEnabled(true);
    //else
    //    ui->btnNext->setEnabled(false);

    requestingDetailsData = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperOperatorChanges::makeValidatorTop( int digits )
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
void SuperOperatorChanges::requestTransactionGroup(QString whichGroup)
{
    // parameter which: -1 = previous, 0 = first, 1 = next
    // PLU#,(or nothing for 'All Products'),operator ID (id) from the row selected in the Operator Summary,
    // transaction type: 0 = for All Production transaction,Start Date/Time (or nothing for 'no limit')
    // End Date/Time (or nothing for 'no limit'), group: Prev/First/Next (i.e., -1/0/1), qty: Number of rows

    QString plu = requestPLUNumber;                                 // plu number field or all PLUs from checkbox
    QString operatorID = requestOperatorID;                         // operator ID (id) from the row selected in the Operator Summary
    QString startDate = requestStartDate;                           // Start Date/Time (or nothing for 'no limit')
    QString endDate = requestEndDate;                               // End Date/Time (or nothing for 'no limit')

    // FORMAT: 131,30,plu#,operator id,transaction type, start date, end date, group, qty to display
    session_->send(OperatorChangesDev, ReadOperatorChangesGroup, plu + "," + operatorID + ",0,\"" + startDate + "\",\"" + endDate + "\"," + whichGroup + "," + QString::number( NUMBER_OF_ROWS ) + ",0"  );	// OPERATOR CHANGES

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperOperatorChanges::retrieveTransactionType(int typeNumber)
{
    QString typeName = "";

    ScaleConfiguration *myScaleConfig = session_->scaleConfig;
    int curLang = myScaleConfig->currentLanguage;

    if ( receivedTransTypesData )
        typeName = (curLang ? transTypesPri[typeNumber] : transTypesSec[typeNumber]);

    if ( typeName.length() == 0 )
        typeName = tr("Supervisor");

    return typeName;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::updateTransactionTypeNames(const SessionMessage &msg)
{
    // We receied a 152,21 from the Scale Application - Transaction Type Message

    DomTotalTypesList m( msg );   // access with: msg[0].toInt() OR     m.myMethodVariable().toInt();

    receivedTransTypesData = true;
    int transTypeNum = 0;

    int transTypesTotal = m.numOfRecordIds();                       // How many transactions are there?
    QStringList transTypeNumberList = m.allRecordIds().split(",");  // The list of Operator IDs
    QStringList transTypeName1List = m.namesList().split(",");      // list of primary language names
    QStringList transTypeName2List = m.secNamesList().split(",");   // secondary trans list names

    for(int j = 0; j < transTypesTotal; ++j)
    {
        transTypeNum = transTypeNumberList.at(j).toInt();

        transTypesPri[transTypeNum] = transTypeName1List.at(j);
        transTypesSec[transTypeNum] = transTypeName2List.at(j);
    }

    requestTransactionGroup("0");                                    // Navigate First Group
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::selectTheRow(int row, int col)
{
    Q_UNUSED( col );

    // use currentSelectedRow to map to the operatorID when then selecting DETAILS
    if( !requestingData )
    {
        // FT-876: fix crash
        if (( 0 <= row ) && ( row < numOfOpers ))
        {
            // means we have a row already selected
            if ( row == currentSelectedRow )
            {
                // then we clicked the same row - WHY? I don't know
                ui->tblOperatorChanges->clearSelection();
            }
            else
            {
                // Let QTTable do its normal behavior - switch rows
                ui->tblOperatorChanges->selectRow(row);
                currentSelectedRow = row;
            }

            requestOperatorID = operIDs.at( currentSelectedRow );     // save off the current selected Operator ID

            ui->btnDetails->setDisabled( false );
        }
        else
        {
            ui->btnDetails->setDisabled( true );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::checkCheckbox(int state)
{
    if(state == CHECKED)
        ui->dialogPluNumberInput->setEnabled(false);
    else if(state == NOT_CHECKED)
        ui->dialogPluNumberInput->setEnabled(true);

    evaluateRetrieveButton( ui->dialogPluNumberInput->text() );
}

//! transactionsDetails()
/*!
 * transactionsDetails - Method to gather and request the data required when the user presses
 * the Details button for a specific row
 *
 * \param[in] none
 * \returns  none
 */
void SuperOperatorChanges::transactionsDetails()
{
    // LOAD DETAILS PAGE

    whatPage = 2;                                                   // who are we displaying?

    // hide everything about Operator Changes
    ui->tblOperatorChanges->setVisible(false);
    ui->tblOperatorChangesTotals->setVisible(false);
    ui->btnDetails->setVisible(false);
    ui->btnPrint->setVisible(false);
    ui->btnExport->setVisible(false);

    // Display everything about Operator Changes with Details
    ui->tblDetails->clear();
    ui->tblDetails->setVisible(true);
    ui->btnBack->setVisible(true);

    update();

    initializeDetails();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::filterDialogCallback()
{
    // Request Operator Changes Summary
    // Message paremeters: PLU#,(or nothing for 'All Products'),
    // transaction type (0 for Production[all] or trans type num otherwise)
    // Start Date/Time (or nothing for 'no limit'), End Date/Time (or nothing for 'no limit')

    int tmpHour = 0;
    int tmpMin = 0;

    // Get rid of the dialogbox
    ui->Dialog->hide();

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

    //PTR #771 -> do this before requesting data, so the message actually shows up...
    session_->send(Error, OperatorChangesDev, ",131001,5");			// 103,131 - pop a dialog telling the user we are getting records

    // 131,20
    session_->send(OperatorChangesDev,ReadOperatorChangeSummary, pluNumber + ",0,\"" + startDate + "\",\"" + endDate + "\"" + ",0" );
    requestingData = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::evaluateRetrieveButton(QString newText)
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
void SuperOperatorChanges::updateEndDateSelector(int newIndex)
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
void SuperOperatorChanges::onClickedPrevious()
{
    prevButtCtr = prevButtCtr - 1;
    requestTransactionGroup("-1");
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::onClickedNext()
{
    prevButtCtr = prevButtCtr + 1;
    requestTransactionGroup("1");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::goBackToSummary()
{
    // This, in legacy, just resets the active frame.
    // Here we need to HIDE ALL Details information and DISPLAY ALL Operator Changes information

    whatPage = 1;                                                   // go back to Operator Changes
    ui->lblUserInfo->setText("");
    // Display everything about Operator Changes with Details
    ui->tblDetails->clear();
    ui->tblDetails->setVisible(false);
    ui->btnBack->setVisible(false);

    // hide everything about Operator Change
    ui->lblTitle->setText(tr("Operator Changes - Summary"));
    ui->tblOperatorChanges->setVisible(true);
    ui->tblOperatorChangesTotals->setVisible(true);
    ui->btnDetails->setVisible(true);
    ui->btnPrint->setVisible(true);
    ui->btnExport->setVisible(true);
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::printOperatorChanges()
{
    ui->lblUserInfo->setText("");
    session_->send(ReportManagerDev, StartReport, "14,0,");			// Report,starit,reporttype=14,FirstPrinter
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::exportOperatorChanges()
{
    QString startDirStr = "";
    QString formatStr( "CSV (*.csv);;*.* (*)" );

    ui->lblUserInfo->setText( tr("Setup Exporting...") );

    FileDialog filepicker( mainWindow_ );
    filepicker.setFilter( formatStr );
    filepicker.setOptions( QFileDialog::DontConfirmOverwrite );

    QStringList folders;

#if defined( Q_OS_LINUX )
        folders.push_back( "/media/thumb" );
        folders.push_back( "/media/" );
        folders.push_back( "/home/" );
#elif defined( Q_OS_WIN )
        folders.push_back( "D:\\" );
        folders.push_back( "E:\\" );
        folders.push_back( "C:\\" );
#endif

    // find existing folder
    foreach ( const QString& f, folders )
        if ( QDir( startDirStr = f ).exists() )
            break;

    filepicker.setAcceptMode( QFileDialog::AcceptOpen );
    filepicker.setDirectory( startDirStr );
    filepicker.setWindowTitle( tr( "Save to USB" ) );

    if ( QDialog::Accepted == filepicker.exec() )
    {
        qApp->processEvents();

        QStringList selected( filepicker.selectedFiles() );
        QString fileName;

        if ( 1 == selected.count() )
            fileName = selected[0];


        bool hasGoodFileName = checkFileName( fileName, 1 );  // the 2nd arg, 1 = file, 2 = folder, here we are checking a file

        if ( hasGoodFileName )
        {
            ui->lblUserInfo->setText( tr("Exporting, please wait ...") );

#if defined( Q_OS_WIN )
            fileName.replace( "/", "\\\\" );
#endif
            int tareDigits( scaleConfig_->tarePrecision() );

            python pythonProcess;
            QStringList args;
            args.append("3");                       // indicates write operate changes
            args.append(QString::number(tareDigits));                // what is the precision
            args.append(fileName);

            if( pythonProcess.execCommand("reportUtils", args))
            {
                // check exit code of python command
               int textMyCode = pythonProcess.getExitCode();

               if (textMyCode <= 1)
               {
                   ui->lblUserInfo->setText( tr("File saved") );
               }
               else
               {
                   // TODO - what do we do here?
                   ui->lblUserInfo->setText("   Export failed. Error Code: " + QString::number(textMyCode) );
               }

               if( isLinux() )
                   system("sync");         // flush the file to disc
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperOperatorChanges::checkFileName(QString &myfile, int fileOrFolder)
{
    // fileOrFolder = 1 if checking a file, fileOrFolder = 2 if checking a folder

    bool validFile = false;
    QString errStr = "";

    if(myfile == "")
        errStr = tr( "A file was not selected. Please select a valid file to continue." );                                       // This is the string part of the error message,
    else
    {
        int filelen = myfile.length();

        if(filelen == 0)
            errStr = tr( "A file was not selected. Please select a valid file to continue." );
        else if(myfile.at(filelen - 1) == ('/'))
            errStr = tr( "The filename ends in a slash [/].  Please enter a valid file name." );
        else
        {
            if(fileOrFolder == 1)
                myfile = getFileName(myfile);				//checking a file
            else if(fileOrFolder == 2)
                myfile = myfile;							// checking a folder

            if(filelen <= 0)
                errStr = tr( "A valid file name has not been given.  The file name has no length." );
            else
            {
                if(fileOrFolder == 1 && myfile.indexOf("\\") > 0)
                    errStr = tr( "The \\ [slash] character is not allowed in the filename." );
                else if(myfile.indexOf(";") > 0)
                    errStr = tr( "The ; [semi-colon] character is not allowed in the filename." );
                else if(myfile.indexOf("*") > 0)
                    errStr = tr( "The * [asterisk] character is not allowed in the filename." );
                else if(myfile.indexOf("?") > 0)
                    errStr = tr( "The ? [question mark] character is not allowed in the filename." );
                else if((myfile.indexOf("\"") > 0) || (myfile.indexOf("\'") > 0))
                    errStr = tr( "The \" or \' [single or double quotes] character is not allowed in the filename." );
                else if((myfile.indexOf("<") > 0) || (myfile.indexOf(">") > 0))
                    errStr = tr( "The < or > [qreater or less than] character is not allowed in the filename." );
                else if((myfile.indexOf("+") > 0) || (myfile.indexOf("-") > 0))
                    errStr = tr( "The + or - [plus or minus] character is not allowed in the filename." );
                else if(myfile.indexOf("|") > 0)
                    errStr = tr( "The | [separator bar] character is not allowed in the filename." );
                //else if( myfile.indexOf("C:") > 0 )     // cannot contain C:
                //    errStr = tr("The file cannot be saved to the root file system");
                //else if( isLinux() && myfile.indexOf("media") < 0 )  // has to contain media (a path to a thumb)
                //    errStr = tr("The file cannot be saved to the root file system");
                else
                    validFile = true;
            }
        }
    }

    if ( validFile )
        ui->lblUserInfo->setText( tr("Valid file name") );
    else
        ui->lblUserInfo->setText( errStr );

    return validFile;
}

// Need to figure out, what this is trying to figure out.
QString SuperOperatorChanges::getFileName(QString myfile)
{
    //var fileObj;
    QString strippedFileName = "";
    QString retFile = "";

    bool doesFileAlreadyExist = QFile::exists(myfile);

    if( doesFileAlreadyExist )
    {
        retFile = myfile;                                           // then file name already exists and we can return it as is
    }
    else                                                            // we have to create a new one then
    {
        if( myfile.indexOf(".csv") > 0 )
        {
            // we dont have a file, so we need to create one. Do not include the entire path,
            // but do include the entension if the extension is .csv
            strippedFileName = myfile;                              // then we are good to go
        }
        else if(myfile.indexOf(".") > 0)
        {
            // we dont have a file, so we need to create one. Do not include the entire path, just the file.
            // An extension was added to the file, but we don't recogonize it, but include it.
            int getDot = myfile.indexOf(".") + 1;
            if (myfile.length() == getDot + 3)
                strippedFileName = myfile;
        }
        else
        {
            // we don't have a file, so create one. Do not include the entire path of the file.
            // No extension was given for the file, so default to the file to an csv file ext.
            strippedFileName = myfile;
        }

        retFile = strippedFileName;
    }
    return retFile;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperOperatorChanges::onSessionMessage( const SessionMessage& msg )
{
    if (( OperatorChangesDev == msg[0].toInt() ) && ( WriteOperatorChangeSummary == msg[1].toInt() ))
    {
        // 131,21 - Operator Changes
        session_->send(Error, ErrRemoveAnyModalDialogDevice, ",131001,5"); // kill 131,21 dialog
        updateOperatorChanges(msg);
    }
    else if (( TransactionDev == msg[0].toInt() ) && ( WriteOpertorChangesGroup == msg[1].toInt() ))
    {
        // 151,31 - Operator Changes
        updateTransactionDates(msg);
    }
    else if(( OperatorChangesDev == msg[0].toInt() ) && ( WriteOpertorChangesGroup == msg[1].toInt() ))
    {
        // 131,31 - Operator Details
        updateOperatorChangesDetails(msg);
    }
    else if(( TransactionTypeDev == msg[0].toInt() ) && ( TransactionTypeWriteAllNames == msg[1].toInt() ))
    {
        // 152,21 - Operator Details
        updateTransactionTypeNames(msg);
    }
}

bool SuperOperatorChanges::isLinux()
{
#if defined( Q_OS_LINUX )
    return true;
#elif defined( Q_OS_WIN )
    return false;
#endif
}



