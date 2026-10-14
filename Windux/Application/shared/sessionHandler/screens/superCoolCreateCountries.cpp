/* @file SuperCoolCreateCountries.cpp
 * Allows users to create Cool Countries strings that can later be added to Cool Short Lists 
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
#include "superCoolCreateCountries.h"
#include "ui_superCoolCreateCountries.h"
#include "TokenParser.h"

static const int DEFAULT_COLS = 1;
static const int NUMBER_OF_ROWS = 1000;                               // crank this number up to see if we can have as many as we want

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

SuperCoolCreateCountries::SuperCoolCreateCountries(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperCoolCreateCountries),
    coolCountry_(NULL)
{
    ui->setupUi(this);
    scaleScreen();

    // retrieve config object
    if ( mainWindow_->isSupervisorMode() )
        coolCountry_ = &dynamic_cast<SupervisorMode*>( mode_ )->coolCountry_;

    initialize();
    translate();
}

SuperCoolCreateCountries::~SuperCoolCreateCountries()
{
    saveDataOnExit();
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::initialize()
{
    origRecordName = "";
    requestingData = false;
    hasChanged = false;
    activeRow = -1;
    numOfItems = 0;
	numOfRows = 0;

    connect(ui->tblCoolList,        SIGNAL(cellClicked(int,int)),   this,       SLOT(clickRow(int,int)));       // click cell
    connect(ui->butDeleteButton,    SIGNAL(clicked()),              this,       SLOT(deleteRecord()));          // click delete record
    connect(ui->butCancelButton,    SIGNAL(clicked()),              this,       SLOT(cancelRecord()));          // click cancel record
    connect(ui->butAddCountry,      SIGNAL(clicked()),              this,       SLOT(addCountry()));            // change cool number field
    connect(ui->txtRecordName,      SIGNAL(editingFinished()),      this,       SLOT(editCountryName()));       // change cool country name field

    update();

    QRegExp validRecordNameRegEx("^[À-Ýà-ý,\\w \\&\\-]*$");
    
    QValidator* recordNameValidator;
    recordNameValidator = new QRegExpValidator(validRecordNameRegEx, this);
    ui->txtRecordName->setValidator(recordNameValidator);

    requestingData = true;
    session_->send(COOLDev, COOLDevReadAllCountries);               // 135,21 -> read all Countries
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperCoolCreateCountries::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::translate()
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
void SuperCoolCreateCountries::clearTheFields()
{
    ui->txtRecordName->clear();
    activeRow = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::updateCoolList(const SessionMessage& msg)
{
    ui->tblCoolList->clear();                                       // clear it before we do anything
    activeRow = -1;

    DomCoolCountryList domList( msg );                              // class - 135,23

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
void SuperCoolCreateCountries::displayCoolCountry(const SessionMessage &msg)
{
    DomCoolCountry m( msg );

    if ( coolCountry_ )
        *coolCountry_ = m;                                          // get the coolCountry class to reuse it later

    QString myName  = m.name1();
    ui->txtRecordName->setText( myName );

    requestingData = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::readRecordFromRow(int row)
{
    Q_UNUSED( row );

    QString recordName = "";

    QTableWidgetItem *nameItem = new QTableWidgetItem;              // get the record number from clicked row
    nameItem = ui->tblCoolList->item(activeRow, 0);
    recordName = nameItem->text();

    readRecord(recordName);                                         // now read the record
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::insertNewCoolListRow()
{
    // called by addCountry->updateActiveRowRecordName

    activeRow = numOfItems;                                        // get number of items - and its likely not selected to update activeRow

    if(numOfItems == numOfRows)                                     // need to decide if we need to add a new row
        ui->tblCoolList->insertRow(numOfRows);                      // guess we do

    QString myName = coolCountry_->name1();                         // doing this just so I can see the value before I assign it
    QTableWidgetItem *nameItem = new QTableWidgetItem;              // create a column for cool names
    nameItem->setText( myName );
    ui->tblCoolList->setItem(activeRow, 0, nameItem);               // add it

    numOfItems = numOfItems + 1;
    numOfRows = ui->tblCoolList->rowCount();
    activeRow = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::readRecord(const QString& record)
{
    requestingData = true;
    session_->send(COOLDev,COOLDevReadCountry, "\"" + record + "\""); // 135,20,name
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::saveCurrentRecord()
{
    if(hasChanged)
    {
        QString coolName =  coolCountry_->name1();

        if( coolName.length() > 0 )
        {
            QString msg = "";
            msg +=  "\"" + coolCountry_->name1() + "\"";
            msg += ",\"" + coolCountry_->name1() + "\"";                // now do absolutely nothing with this. Display it...nothing
            session_->send(COOLDev, COOLDevSaveCountry, msg);           // 135,26

            hasChanged = false;
        }
        else if( coolName.length() == 0 )
        {
            deleteRecord();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::saveDataOnExit()
{
    saveCurrentRecord();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::updateActiveRowRecordName(const QString& name)
{
    // call by addCountry()

    if (name.length() > 0)                                          // shouldn't I even update this field even if they clear it?
    {
        QString msg = "";
        msg +=  "\"" + coolCountry_->name1() + "\"";                // build the message to send a save
        msg += ",\"" + coolCountry_->name1() + "\"";
        session_->send(COOLDev,COOLDevCreateNewCountryAndSave, msg);	// 135, 37, Country name

        insertNewCoolListRow();                                     // now insert it into the row
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::createNewCOOLCountry(const SessionMessage &msg)
{
    returnedErrorsCOOLDev m( msg );

    int okPressed( 0 < m.errorData2() );

    if ( okPressed )
    {
        QString nameItem = ui->txtRecordName->text();
        session_->send(COOLDev,COOLDevCreateNewCountry, nameItem);	// 135, 32, Country name
    }
    else
    {
        clearTheFields();
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::clickRow(int row, int col)
{
    Q_UNUSED( col );

    ui->butAddCountry->setEnabled(false);

    if( (row + 1) <= numOfItems)                                    // otherwise not a valid row
    {
        saveCurrentRecord();

        ui->tblCoolList->selectRow(row);
        activeRow = row;                                            // save off for future use

        QTableWidgetItem *nameItem;                                 // get the Country name from the clicked row

        if ( ( nameItem = ui->tblCoolList->item(activeRow, 0) ))
        {
            QString recordName = nameItem->text();
            origRecordName = recordName;

            readRecordFromRow(row);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::deleteRecord()
{
    if (activeRow != -1)
    {
        QString recName = ui->txtRecordName->text();
        recName = "\"" + recName + "\"";
        session_->send(COOLDev,COOLDevDeleteCountry, recName);		// 135, 24, record number

        ui->tblCoolList->removeRow(activeRow);
        clearTheFields();
        origRecordName = "";
        numOfItems--;
        numOfRows = ui->tblCoolList->rowCount();
        activeRow = -1;
        hasChanged = false;
    }

    ui->butAddCountry->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::cancelRecord()
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
        session_->send(COOLDev, COOLDevSaveCountry, msg);           // 135,26

        origRecordName = "";
    }
    else
    {
        clearTheFields();
        activeRow = -1;
        hasChanged = false;
    }

    ui->butAddCountry->setEnabled( true );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::editCountryName()
{
    // we can only edit existing rows so check if existing.

    if( activeRow > -1)
    {
        hasChanged = true;
        QString myName  = ui->txtRecordName->text();                // get the edited name
        coolCountry_->name1() = myName;                             // update the dom data
        coolCountry_->name2() = "";

        ui->tblCoolList->currentItem()->setText( myName );          // update the text in the current cell

        saveCurrentRecord();
        activeRow = -1;
    }

    ui->butAddCountry->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::addCountry()
{
    // Called when a user click Add Country
    QString myName  = ui->txtRecordName->text();
    coolCountry_->name1() = myName;
    coolCountry_->name2() = "";                                     // because we never use it

    updateActiveRowRecordName(myName);

    clearTheFields();

    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperCoolCreateCountries::onScreenChanging()
{
    saveDataOnExit();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolCreateCountries::onSessionMessage( const SessionMessage& msg )
{
    if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteAllCountries == msg[1].toInt() ))
    {
        // Received 135,23 - Update COOL Countries, sent 135,21 to get this back
        updateCoolList(msg);
    }
    else if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteCountry == msg[1].toInt() ))
    {
        // Received 135,22 - Update COOL Country Record, sent 135,20
        displayCoolCountry(msg);
    }
    else if (( Error == msg[0].toInt() ) && ( ErrCOOLDevice == msg[1].toInt() ))
    {
        // Received 103,135 - send read country, it doesn't exist so create a new one
        if( 135002 ==  msg[2].toInt() )                          // see if we have the right error
            createNewCOOLCountry(msg);	// change to country
    }
	else
		requestingData = false;
}
