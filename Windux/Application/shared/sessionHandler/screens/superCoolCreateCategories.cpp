/* @file SuperCoolCreateCategories.cpp
 * Allows operators to create Cool Category Strings which can be used in Cool Short Lists later on.
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Job E. Crew
 */

#include "superCoolCreateCategories.h"
#include "ui_superCoolCreateCategories.h"

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
static const int NUMBER_OF_ROWS = 1000;                               // crank this number up to see if we can have as many as we want

namespace hobart{
    class numericLineEdit;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

SuperCoolCreateCategories::SuperCoolCreateCategories(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperCoolCreateCategories),
    coolCategory_(NULL)
{
    ui->setupUi(this);
    scaleScreen();

    // retrieve config object
    if ( mainWindow_->isSupervisorMode() )
        coolCategory_ = &dynamic_cast<SupervisorMode*>( mode_ )->coolCategory_;

    initialize();
    translate();
}

SuperCoolCreateCategories::~SuperCoolCreateCategories()
{
    saveDataOnExit();
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::initialize()
{
    hasChanged = false;
    requestingData = false;
    activeRow = -1;
    numOfItems = 0;
    numOfRows = 0;

    ui->txtRecordName->setMaxLength( 50 );

    connect(ui->tblCoolList,        SIGNAL(cellClicked(int,int)),   this,       SLOT(clickRow(int,int)));       // click cell
    connect(ui->butDeleteButton,    SIGNAL(clicked()),              this,       SLOT(deleteRecord()));          // click delete record
    connect(ui->txtRecordNum,       SIGNAL(editingFinished()),      this,       SLOT(readRecordFromElement())); // change cool number field
    connect(ui->txtRecordName,      SIGNAL(editingFinished()),      this,       SLOT(updateHasChanged()));      // change cool category name field


    update();

    // setup validators
    ui->txtRecordNum->setValidator(txtRecordNumValidator_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtRecordNum->setDecimalPlaces(0);
    ui->txtRecordNum->setInputWrapping(true);
    ui->txtRecordNum->setInputTruncation(true);
    txtRecordNumValidator_->setTop(9999);

    QRegExp validRecordNameRegEx("^[À-Ýà-ý\\w \\&\\-]*$");
    
    QValidator* recordNameValidator;
    recordNameValidator = new QRegExpValidator(validRecordNameRegEx, this);
    ui->txtRecordName->setValidator(recordNameValidator);

    requestingData = true;
    session_->send(COOLDev, COOLDevReadAllCategories);              // 135,81 -> read all Categories
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperCoolCreateCategories::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::translate()
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
int SuperCoolCreateCategories::makeValidatorTop( int digits )
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
void SuperCoolCreateCategories::clearTheFields()
{
    ui->txtRecordNum->clear();                                      // clean everything up
    ui->txtRecordName->clear();
    activeRow = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::updateCoolList(const SessionMessage& msg)
{
    // Received 135,83 - Update COOL Categories, sent 135,81 to get this back

    ui->tblCoolList->clear();                                       // clear it before we do anything
    activeRow = -1;

    DomCoolCategoryList domList( msg );

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
void SuperCoolCreateCategories::displayCoolCategory(const SessionMessage &msg)
{
    DomCoolCategory m( msg );

    QString myName  = m.name1();
    ui->txtRecordNum->setText( m.recordNum() );
    ui->txtRecordName->setText( myName );

    if (myName == "")
    {
        insertNewCoolListRow(msg);                                  //	This is a new record. Add it to the top of the list.
    }

    requestingData = false;

    if ( coolCategory_ )
        *coolCategory_ = m;                                         // get the coolCategory class to reuse it later
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::viewCOOLRecord()
{
    QString recordNum = "";
    QString recordName = "";

    QTableWidgetItem *numItem = new QTableWidgetItem;               // get the record number from clicked row
    numItem = ui->tblCoolList->item(activeRow, 0);
    recordNum = numItem->text();

    QTableWidgetItem *nameItem = new QTableWidgetItem;              // get the Category name from the clicked row
    nameItem = ui->tblCoolList->item(activeRow, 1);
    recordName = nameItem->text();

    ui->txtRecordNum->setText(recordNum);                           // update the number text field on the form
    ui->txtRecordName->setText(recordName);                         // now the name field

    coolCategory_->recordNum() = recordNum;                         // now update the Dom values
    coolCategory_->name1() = recordName;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::readRecordFromRow(int row)
{
    Q_UNUSED( row );

    QString recordNum = "";

    QTableWidgetItem *numItem = new QTableWidgetItem;               // get the record number from clicked row
    numItem = ui->tblCoolList->item(activeRow, 0);
    recordNum = numItem->text();

    readRecord(recordNum);                                          // now read the record
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::insertNewCoolListRow(const SessionMessage &msg)
{
    DomCoolCategory m( msg );

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
void SuperCoolCreateCategories::readRecord(const QString& record)
{
    requestingData = true;
    session_->send(COOLDev,COOLDevReadCategory,record);             //	135,80,record
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::saveCurrentRecord()
{
    if(hasChanged)
    {
        QString msg = coolCategory_->recordNum();
        msg += ",\"" + coolCategory_->name1() + "\"";
        msg += ",\"" + coolCategory_->name2() + "\"";                   // now do absolutely nothing with this. Display it...nothing
        session_->send(COOLDev, COOLDevSaveCategory, msg);              // 135,86

        hasChanged = 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::saveDataOnExit()
{
    saveCurrentRecord();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::updateActiveRowRecordName(const QString& name)
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
void SuperCoolCreateCategories::createNewCOOLCategory(const SessionMessage &msg)
{
    returnedErrorsCOOLDev m( msg );

    int okPressed( 0 < m.errorData2() );

    if ( okPressed )
    {
        QString numItem = ui->txtRecordNum->text();
        session_->send(COOLDev,COOLDevCreateNewCategory, numItem);	//	135, 92, category number
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
void SuperCoolCreateCategories::clickRow(int row, int col)
{
    Q_UNUSED( col );

    if( (row + 1) <= numOfItems)
    {
        QString recordName = "";

        saveCurrentRecord();

        ui->tblCoolList->selectRow(row);
        activeRow = row;                                                // save off for future use

        QTableWidgetItem *nameItem = new QTableWidgetItem;              // get the Category name from the clicked row
        nameItem = ui->tblCoolList->item(activeRow, 1);
        recordName = nameItem->text();

        if(recordName.length() == 0)                                    //	Not sure why we were looking for a length of 1. Maybe because of a space char? [CCD]
            viewCOOLRecord();
        else
            readRecordFromRow(row);
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::deleteRecord()
{

    if (activeRow != -1)
    {
        QString recNum = ui->txtRecordNum->text();
        session_->send(COOLDev,COOLDevDeleteCategory, recNum);		//	135, COOLDevDeleteCategory, record number

        ui->tblCoolList->removeRow(activeRow);
        clearTheFields();
        numOfItems--;
        numOfRows = ui->tblCoolList->rowCount();
        activeRow = -1;

        hasChanged = 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::readRecordFromElement()
{
    saveCurrentRecord();
    QString coolNum = ui->txtRecordNum->text();
    readRecord(coolNum);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::updateHasChanged()
{
    QString myName  = ui->txtRecordName->text();
    coolCategory_->name1() = myName;
    updateActiveRowRecordName(myName);
    hasChanged = 1;
    saveCurrentRecord();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperCoolCreateCategories::onScreenChanging()
{
    saveDataOnExit();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCategories::onSessionMessage( const SessionMessage& msg )
{
    if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteAllCategories == msg[1].toInt() ))
    {
            // Received 135,83 - Update COOL Categories, sent 135,81 to get this back
            updateCoolList(msg);
    }
    else if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteCategory == msg[1].toInt() ))
    {
            // Received 135,82 - Update COOL Category Record, sent 135,80
            displayCoolCategory(msg);
    }
    else if (( Error == msg[0].toInt() ) && ( ErrCOOLDevice == msg[1].toInt() ))
    {
            // Received 103,135 - send read category, it doesn't exist so create a new one
            if( 135001 ==  msg[2].toInt() )
                createNewCOOLCategory(msg);
    }
    else
        requestingData = false;
}
