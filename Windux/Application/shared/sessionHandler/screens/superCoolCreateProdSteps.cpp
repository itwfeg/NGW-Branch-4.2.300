/* @file SuperCoolCreateProdSteps.cpp
 * Allows users to create Cool ProdSteps strings that can later be added to Cool Short Lists
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Job E. Crew
 */

#include <QtGui>
#include <QtCore>

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "superCoolCreateProdSteps.h"
#include "ui_superCoolCreateProdSteps.h"
#include "TokenParser.h"

static const int DEFAULT_COLS = 1;
static const int NUMBER_OF_ROWS = 1000;                               // crank this number up to see if we can have as many as we want

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

SuperCoolCreateProdSteps::SuperCoolCreateProdSteps(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperCoolCreateProdSteps),
    coolProductionStep_(NULL)
{
    ui->setupUi(this);
    scaleScreen();

    // retrieve config object
    if ( mainWindow_->isSupervisorMode() )
        coolProductionStep_ = &dynamic_cast<SupervisorMode*>( mode_ )->coolProductionStep_;

    initialize();
    translate();
}

SuperCoolCreateProdSteps::~SuperCoolCreateProdSteps()
{
    saveDataOnExit();
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::initialize()
{
    origRecordName = "";
    requestingData = false;
    hasChanged     = false;
    activeRow  = -1;
    numOfItems = 0;
    numOfRows  = 0;

    connect(ui->tblCoolList,        SIGNAL(cellClicked(int,int)),   this,       SLOT(clickRow(int,int)));        // click cell
    connect(ui->butDeleteButton,    SIGNAL(clicked()),              this,       SLOT(deleteRecord()));           // click delete record
    connect(ui->butCancelButton,    SIGNAL(clicked()),              this,       SLOT(cancelRecord()));           // click cancel record
    connect(ui->butAddProdStep,     SIGNAL(clicked()),              this,       SLOT(addProductionStep()));      // add production step button
    connect(ui->txtRecordName,      SIGNAL(editingFinished()),      this,       SLOT(editProductionStepName())); // change cool production name field

    update();

    QRegExp validRecordNameRegEx("^[À-Ýà-ý,\\w \\&\\-]*$");
    QValidator* recordNameValidator;
    recordNameValidator = new QRegExpValidator(validRecordNameRegEx, this);
    ui->txtRecordName->setValidator(recordNameValidator);

    requestingData = true;
    session_->send(COOLDev, COOLDevReadAllProductionSteps);         // 135,41 -> read all Production Steps
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperCoolCreateProdSteps::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::translate()
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
void SuperCoolCreateProdSteps::clearTheFields()
{
    ui->txtRecordName->clear();
    activeRow = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::updateCoolList(const SessionMessage& msg)
{
    ui->tblCoolList->clear();                                       // clear it before we do anything
    activeRow = -1;

    DomCoolProdStepList domList( msg );                             // class - 135,43

    int myItems = domList.numOfItems();
    numOfItems = myItems;

    ui->tblCoolList->setRowCount(myItems);
    numOfRows = myItems;

    ui->tblCoolList->setColumnCount(DEFAULT_COLS);

    ui->tblCoolList->setColumnWidth( 0, scaleWidthForScreen( 872 ) );                         // set the width of the first column

    if (myItems > 0)
    {
        QVector<QString> nameArray = TokenParser::splitStringTokens( domList.names() );

        for(int j = 0; j < myItems; j++ )
        {
            QTableWidgetItem *nameItem = new QTableWidgetItem;      // create a column for cool numbers
            nameItem->setText( nameArray.at(j) );
            ui->tblCoolList->setItem(j, 0, nameItem);               // add it
        }
    }
    requestingData = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::displayCoolProductionStep(const SessionMessage &msg)
{
    DomCoolProductionStep m( msg );

    if ( coolProductionStep_ )
        *coolProductionStep_ = m;                                   // get the coolProductionStep class to reuse it later

    QString myName  = m.name1();
    ui->txtRecordName->setText( myName );

    requestingData = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::readRecordFromRow(int row)
{
    Q_UNUSED( row );

    QTableWidgetItem *nameItem;              // get the record number from clicked row

    if ( (nameItem = ui->tblCoolList->item(activeRow, 0)) )
    {
        QString recordName = nameItem->text();

        readRecord(recordName);                                         // now read the record
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::insertNewCoolListRow()
{
    // called by addProductionStep->updateActiveRowRecordName

    activeRow = numOfItems;                                         // get number of items - and its likely not selected to update activeRow

    if(numOfItems == numOfRows)                                     // need to decide if we need to add a new row
        ui->tblCoolList->insertRow(numOfRows);                      // guess we do

    QString myName = coolProductionStep_->name1();                  // doing this just so I can see the value before I assign it
    QTableWidgetItem *nameItem = new QTableWidgetItem;              // create a column for cool names
    nameItem->setText( myName );
    ui->tblCoolList->setItem(activeRow, 0, nameItem);               // add it

    numOfItems = numOfItems + 1;
    numOfRows = ui->tblCoolList->rowCount();
    activeRow = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::readRecord(const QString& record)
{
    requestingData = true;
    session_->send(COOLDev,COOLDevReadProductionStep, "\"" + record + "\""); // 135,40,name
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::saveCurrentRecord()
{
    if(hasChanged)
    {
        QString sayMyName =  coolProductionStep_->name1();

        if( sayMyName.length() > 0 )
        {
            QString msg = "";
            msg +=  "\"" + coolProductionStep_->name1() + "\"";
            msg += ",\"" + coolProductionStep_->name1() + "\"";         // now do absolutely nothing with this. Display it...nothing
            session_->send(COOLDev, COOLDevSaveProductionStep, msg);    // 135,46

            hasChanged = false;
        }
        else if( sayMyName.length() == 0 )
        {
            deleteRecord();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::saveDataOnExit()
{
    saveCurrentRecord();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::updateActiveRowRecordName(const QString& name)
{
    // call by addProductionStep()

    if (name.length() > 0)                                          // shouldn't I even update this field even if they clear it?
    {
        QString msg = "";
        msg +=  "\"" + coolProductionStep_->name1() + "\"";         // build the message to send a save
        msg += ",\"" + coolProductionStep_->name1() + "\"";
        session_->send(COOLDev,COOLDevCreateNewProductionStepAndSave, msg);	// 135, 57, ProductionStep name

        // HTA-142 added line below for comma
        hasChanged = false;

        insertNewCoolListRow();                                     // now insert it into the row
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::createNewCOOLProductionStep(const SessionMessage &msg)
{
    returnedErrorsCOOLDev m( msg );

    int okPressed( 0 < m.errorData2() );

    if ( okPressed )
    {
        QString nameItem = "\"" + ui->txtRecordName->text() + "\"";
        session_->send(COOLDev,COOLDevCreateNewProductionStep, nameItem);	// 135, 52, ProductionStep name
    }
    else
    {
        clearTheFields();
    }

    // jump to and select the newly added row
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::clickRow(int row, int col)
{
    Q_UNUSED( col );

    ui->butAddProdStep->setEnabled(false);

    if( (row + 1) <= numOfItems)                                    // otherwise not a valid row
    {
        saveCurrentRecord();

        ui->tblCoolList->selectRow(row);
        activeRow = row;                                            // save off for future use

        QTableWidgetItem *nameItem;          // get the ProductionStep name from the clicked row

        if ( (nameItem = ui->tblCoolList->item(activeRow, 0)) )
        {
            QString recordName = nameItem->text();
            origRecordName = recordName;                            // save off in case we reset it.

            readRecordFromRow(row);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::deleteRecord()
{

    if (activeRow != -1)
    {
        QString recName = ui->txtRecordName->text();
        recName = "\"" + recName + "\"";
        session_->send(COOLDev,COOLDevDeleteProductionStep, recName);		// 135, 44, record name

        ui->tblCoolList->removeRow(activeRow);
        clearTheFields();
        numOfItems--;
        numOfRows = ui->tblCoolList->rowCount();
        origRecordName = "";
        activeRow = -1;
        hasChanged = false;
    }

    ui->butAddProdStep->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::cancelRecord()
{
    if( ui->tblCoolList->currentItem() && origRecordName.length() > 0 )
    {
        ui->tblCoolList->currentItem()->setText( origRecordName );
        clearTheFields();
        activeRow = -1;
        hasChanged = false;

        QString msg = "";
        msg +=  "\"" + origRecordName + "\"";
        msg += ",\"" + origRecordName + "\"";         // now do absolutely nothing with this. Display it...nothing
        session_->send(COOLDev, COOLDevSaveProductionStep, msg);    // 135,46

        origRecordName = "";
    }
    else
    {
        clearTheFields();
        activeRow = -1;
        hasChanged = false;
    }

    ui->butAddProdStep->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::editProductionStepName()
{
    // we can only edit existing rows so check if existing.

    if( activeRow > -1)
    {
        hasChanged = true;
        QString sayMyName  = ui->txtRecordName->text();
        coolProductionStep_->name1() = sayMyName;
        coolProductionStep_->name2() = "";

        ui->tblCoolList->currentItem()->setText( sayMyName );          // update the text in the current cell

        saveCurrentRecord();
        activeRow = -1;
    }

    ui->butAddProdStep->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::addProductionStep()
{
    // Called when a user click add Production Step
    QString sayMyName  = ui->txtRecordName->text();
    coolProductionStep_->name1() = sayMyName;
    coolProductionStep_->name2() = "";                              // because we never use it

    updateActiveRowRecordName(sayMyName);

    clearTheFields();

    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperCoolCreateProdSteps::onScreenChanging()
{
    saveDataOnExit();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateProdSteps::onSessionMessage( const SessionMessage& msg )
{
    if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteAllProductionSteps == msg[1].toInt() ))
    {
        // Received 135,43 - Update COOL Production Step, sent 135,41 to get this back
        updateCoolList(msg);
    }
    else if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteProductionStep == msg[1].toInt() ))
    {
        // Received 135,42 - Update COOL Production Step Record, sent 135,40
        displayCoolProductionStep(msg);
    }
    else if (( Error == msg[0].toInt() ) && ( ErrCOOLDevice == msg[1].toInt() ))
    {
        // Received 103,135 - send read ProductionStep, it doesn't exist so create a new one
        if( 135003 ==  msg[2].toInt() )                          // see if we have the right error
            createNewCOOLProductionStep(msg);	// change to productionstep
    }
    else
        requestingData = false;
}
