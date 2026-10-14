/* @file SuperCoolCreatePretext.cpp
 * Allows users to create Cool Pretext strings that can later be added to Cool Short Lists 
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Job E. Crew
 */

#include "superCoolCreatePretext.h"
#include "ui_superCoolCreatePretext.h"

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
static const int DEFAULT_COLS = 2;
static const int MAX_ROWS = 1000;                                   // crank this number up to see if we can have as many as we want

namespace hobart{
    class numericLineEdit;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

SuperCoolCreatePretext::SuperCoolCreatePretext(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperCoolCreatePretext),
    coolPretext_(NULL)
{
    ui->setupUi(this);
    scaleScreen();

    // retrieve config object
    if ( mainWindow_->isSupervisorMode() )
        coolPretext_ = &dynamic_cast<SupervisorMode*>( mode_ )->coolPretext_;

    // setup validators
    ui->txtRecordNum->setValidator(txtRecordNumValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtRecordNum->setDecimalPlaces(0);
    ui->txtRecordNum->setInputWrapping(true);\
    ui->txtRecordNum->setInputTruncation(true);
    txtRecordNumValidator_->setTop(9999);

    initialize();
    translate();
}

SuperCoolCreatePretext::~SuperCoolCreatePretext()
{
    saveDataOnExit();
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::initialize()
{
    hasChanged = false;
    requestingData = false;
    activeRow = -1;
    numOfItems = 0;
    numOfRows = 0;

    connect(ui->tblCoolList,        SIGNAL(cellClicked(int,int)),   this,       SLOT(clickRow(int,int)));       // click cell
    connect(ui->butDeleteButton,    SIGNAL(clicked()),              this,       SLOT(deleteRecord()));          // click delete record
    connect(ui->txtRecordNum,       SIGNAL(editingFinished()),      this,       SLOT(readRecordFromElement())); // change cool number field
    connect(ui->txtRecordName,      SIGNAL(editingFinished()),      this,       SLOT(updateHasChanged()));      // change cool pretext name field

    update();

    QRegExp validRecordNameRegEx("^[À-Ýà-ý,\\w \\&\\-]*$");
    
    QValidator* recordNameValidator;
    recordNameValidator = new QRegExpValidator(validRecordNameRegEx, this);
    ui->txtRecordName->setValidator(recordNameValidator);

    requestingData = true;
    session_->send(COOLDev, COOLDevReadAllPretexts);              // 135,61 -> read all Pretext
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperCoolCreatePretext::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::translate()
{
    ui->retranslateUi( this );

    update();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperCoolCreatePretext::makeValidatorTop( int digits )
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
void SuperCoolCreatePretext::clearTheFields()
{
    ui->txtRecordNum->clear();                                      // clean everything up
    ui->txtRecordName->clear();
    activeRow = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::updateCoolList(const SessionMessage& msg)
{

    // Received 135,63 - Update COOL Pretext, sent 135,61 to get this back

    ui->tblCoolList->clear();                                       // clear it before we do anything
    activeRow = -1;

    DomCoolPretextList domList( msg );

    int myItems = domList.numOfItems();
    numOfItems = myItems;

    if (myItems < DEFAULT_ROWS)
    {
        ui->tblCoolList->setRowCount(DEFAULT_ROWS);                  // set up how many rows and colums we have.
        numOfRows = DEFAULT_ROWS;
    }
    else
    {
        ui->tblCoolList->setRowCount(myItems);
        numOfRows = myItems;
    }

    ui->tblCoolList->setColumnCount(DEFAULT_COLS);

    ui->tblCoolList->setColumnWidth( 0, scaleWidthForScreen( 141 ) );                         // set the width of the first column
    ui->tblCoolList->setColumnWidth( 1, scaleWidthForScreen( 731 ) );                         // the second one too

    if (myItems > 0)
    {
        QStringList numberArray = domList.recordNums().split(",");
        QVector<QString> nameArray = TokenParser::splitStringTokens( domList.names() );

        for(int j = 0; j < myItems; j++ )
        {
            QTableWidgetItem *numberItem = new QTableWidgetItem;    // create a column for cool numbers
            numberItem->setText( numberArray.at(j) );
            ui->tblCoolList->setItem(j, 0, numberItem);             // add it

            QTableWidgetItem *nameItem = new QTableWidgetItem;      // create a column for cool names
            nameItem->setText( nameArray.at(j) );
            ui->tblCoolList->setItem(j, 1, nameItem);               // add it
        }
    }
	requestingData = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::displayCoolPretext(const SessionMessage &msg)
{
    DomCoolPretext m( msg );

    QString myName  = m.name1();
    ui->txtRecordNum->setText( m.recordNum() );
    ui->txtRecordName->setText( myName );

    if (myName == "")
    {
        insertNewCoolListRow(msg);                                  //	This is a new record. Add it to the top of the list.
    }

    requestingData = false;

    if ( coolPretext_ )
        *coolPretext_ = m;                                          // get the coolPretext class to reuse it later

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::viewCOOLRecord()
{
    QString recordNum = "";
    QString recordName = "";

    QTableWidgetItem *numItem = NULL;                               // get the record number from clicked row
    if ( (numItem = ui->tblCoolList->item(activeRow, 0)) )
        recordNum = numItem->text();

    QTableWidgetItem *nameItem = NULL;                              // get the Pretext name from the clicked row
    if ( (nameItem = ui->tblCoolList->item(activeRow, 1)) )
        recordName = nameItem->text();

    ui->txtRecordNum->setText(recordNum);                           // update the number text field on the form
    ui->txtRecordName->setText(recordName);                         // now the name field

    coolPretext_->recordNum() = recordNum;                         // now update the Dom values
    coolPretext_->name1() = recordName;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::readRecordFromRow(int row)
{
    Q_UNUSED( row );

    QString recordNum = "";

    QTableWidgetItem *numItem = NULL;                               // get the record number from clicked row
    if ( (numItem = ui->tblCoolList->item(row, 0)) )                // insert it at the top of the list, even though it will likely be out of order
        recordNum = numItem->text();                                // next request for the list will order it

    readRecord(recordNum);                                          // now read the record
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::insertNewCoolListRow(const SessionMessage &msg)
{
    DomCoolPretext m( msg );

    activeRow = numOfItems;

    if( numOfItems == numOfRows )
        ui->tblCoolList->insertRow(numOfRows);


    QTableWidgetItem *numberItem = new QTableWidgetItem;            // create a column for cool numbers
    numberItem->setText(  m.recordNum() );
    ui->tblCoolList->setItem(activeRow, 0, numberItem);             // add it

    QTableWidgetItem *nameItem = new QTableWidgetItem;              // create a column for cool names
    nameItem->setText( m.name1() );
    ui->tblCoolList->setItem(activeRow, 1, nameItem);               // add it

    numOfItems = numOfItems + 1;
    numOfRows = ui->tblCoolList->rowCount();
    //activeRow = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::readRecord(const QString& record)
{
    requestingData = true;
    session_->send(COOLDev,COOLDevReadPretext,record);             //	135,60,record
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::saveCurrentRecord()
{
    if(hasChanged)
    {
        QString msg = coolPretext_->recordNum();
        msg += ",\"" + coolPretext_->name1() + "\"";
        msg += ",\"" + coolPretext_->name2() + "\"";                // now do absolutely nothing with this. Display it...nothing
        session_->send(COOLDev, COOLDevSavePretext, msg);           // 135,66

        hasChanged = 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::saveDataOnExit()
{
    saveCurrentRecord();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::updateActiveRowRecordName(const QString& name)
{
    if (name.length() > 0)                                      // shouldn't I even update this field even if they clear it?
    {
        QTableWidgetItem *nameItem = new QTableWidgetItem;
        nameItem->setText( name );
        ui->tblCoolList->setItem(activeRow, 1, nameItem);
        activeRow = -1;
        update();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::createNewCOOLPretext(const SessionMessage &msg)
{
    returnedErrorsCOOLDev m( msg );

    int okPressed( 0 < m.errorData2() );

    if ( okPressed )
    {
        QString numItem = ui->txtRecordNum->text();
        session_->send(COOLDev,COOLDevCreateNewPretext, numItem);	//	135, 72, pretext number
    }
    else
        clearTheFields();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::clickRow(int row, int col)
{
    Q_UNUSED( col );

    if( (row + 1) <= numOfItems)
    {
        QString recordName = "";

        saveCurrentRecord();

        ui->tblCoolList->selectRow(row);
        activeRow = row;                                                // save off for future use

        QTableWidgetItem *nameItem = NULL;                              // get the Pretext name from the clicked row
        if ( (nameItem = ui->tblCoolList->item(activeRow, 1)) )
            recordName = nameItem->text();

        if(recordName.length() == 0)                                    //	Not sure why we were looking for a length of 1. Maybe because of a space char? [CCD]
            viewCOOLRecord();
        else
            readRecordFromRow(row);
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::deleteRecord()
{
    if (activeRow != -1)
    {
        QString recNum = ui->txtRecordNum->text();
        session_->send(COOLDev,COOLDevDeletePretext, recNum);		//	135, 64, record number

        ui->tblCoolList->removeRow(activeRow);
        clearTheFields();
        numOfItems--;
        numOfRows = ui->tblCoolList->rowCount();
        activeRow = -1;

        hasChanged = 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::readRecordFromElement()
{
    saveCurrentRecord();
    QString coolNum = ui->txtRecordNum->text();
    readRecord(coolNum);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::updateHasChanged()
{
    QString myName  = ui->txtRecordName->text();
    coolPretext_->name1() = myName;
    updateActiveRowRecordName(myName);
    hasChanged = 1;
    saveCurrentRecord();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperCoolCreatePretext::onScreenChanging()
{
    saveDataOnExit();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreatePretext::onSessionMessage( const SessionMessage& msg )
{
    if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteAllPretexts == msg[1].toInt() ))
    {
            // Received 135,63 - Update COOL Pretext, sent 135,61 to get this back
            updateCoolList(msg);
    }
    else if (( COOLDev == msg[0].toInt() ) && ( COOLDevWritePretext == msg[1].toInt() ))
    {
            // Received 135,62 - Update COOL Pretext Record, sent 135,60
            displayCoolPretext(msg);
    }
    else if (( Error == msg[0].toInt() ) && ( ErrCOOLDevice == msg[1].toInt() ))
    {
            // Received 103,135 - send read pretext, it doesn't exist so create a new one
            if( 135004 ==  msg[2].toInt() )
                createNewCOOLPretext(msg);
    }
	else
		requestingData = false;
}
