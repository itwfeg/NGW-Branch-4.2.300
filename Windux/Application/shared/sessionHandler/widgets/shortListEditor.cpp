
/**
 * @file shortListEditor.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 */

#include <QtGui>
#include <QtCore>
#include <QDebug>

#include "shortListEditor.h"
#include "ui_shortListEditor.h"

#include "../sessioninfo.h"

#include "../modes/supervisormode.h"

#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <TokenParser.h>

// Hobart namespace
namespace hobart {

ShortListEditor::ShortListEditor(QWidget *parent, SessionMainWindow *mainWindow ) :
    _Mybase( parent ),
    ui(new Ui::ShortListEditor)
{
    ui->setupUi(this);
	
    mainWindow_ = mainWindow;
    session_ = mainWindow->session();

    currentPLUData = &session_->scaleConfig->currentPLU;             //currentPLUData     = &session_->scaleConfig->currentPLU;

    initialize();


    // FT-656
    translate();
}

ShortListEditor::~ShortListEditor()
{
    delete ui;
}

//-----------------------------------------------
// Only show certain fields on the initial screen
//-----------------------------------------------
void ShortListEditor::initialize()
{
    // initialize vars
    ignoreSignals_ = true;                                          // when certain events occur, don't trigger SIGNALS
    hasChanged_ = false;
    coolText = "";                                                  // internal string used to store the cool text seen on the screen
    currentCountryList = "";                                        // clears the country list that has been being built
    coolRecordNum = 0;
    countrySelectedRow = -1;                                        // no row selected
    stepSelectedRow = -1;

    currentCategoryNumber = currentPLUData->coolCategoryNum();      // what is the current cool category, it forces only specific countries and steps to show

    // Set up SIGNAL and SLOT connections
    connect( session_, SIGNAL( receive(SessionMessage) ), SLOT( onSessionMessageReceiveInternal(SessionMessage) ) );    // send and receive

    connect(ui->butDone,            SIGNAL(clicked()),                   this, SLOT(updateDoneFormating()));            // clicked Done
    connect(ui->butCancel,          SIGNAL(clicked()),                   this, SLOT(updateCancelFormating()));          // clicked Cancel
    connect(ui->butStartOver,       SIGNAL(clicked()),                   this, SLOT(clearCoolText()));                  // clicked Start over

    connect(ui->tblStepList,        SIGNAL(cellClicked(int,int)),       this,   SLOT(addStep(int,int)));                // click production step cell
    connect(ui->tblCountryList,     SIGNAL(cellClicked(int,int)),       this,   SLOT(addCountry(int,int)));             // click country cell

    ui->tblStepList->clear();                                       // in case we are coming back in again - force a clear
    ui->tblCountryList->clear();                                    // in case we are coming back in again - force a clear

    ignoreSignals_ = false;

    session_->send(COOLDev, COOLDevCreateNewCoolText);				//	135, 112 - Request a new COOL text (Moved from init ChangeProductAllFields)

    // Pretty sure this was part of my testing mechanism I didn't remove. This shouldn't be here.
    //QVector<QString> myMsg;
    //countrySetup(myMsg);
    //stepSetup(myMsg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ShortListEditor::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ShortListEditor::translate()
{
    // FT-656
    ui->retranslateUi(this);

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PRIVATE METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
QSize ShortListEditor::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 950) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 575) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ShortListEditor::countrySetup(const SessionMessage& msg)
{
    DomCoolCountryList m( msg );

    countryListSize = m.numOfItems();
    countryList = m.names();

    // build the country list - this was from buildLists() and merged here
    if (countryListSize > 0)
    {
        ignoreSignals_ = true;
        ui->tblCountryList->clear();
        ui->tblCountryList->setRowCount( countryListSize);
        ui->tblCountryList->setColumnCount( 1 );
        ui->tblCountryList->setColumnWidth(0,420);

        QVector<QString> tableList = TokenParser::splitStringTokens( countryList );
        QString myStr = "";

        for( int j = 0; j < countryListSize; j++ )
        {
            QTableWidgetItem *countryItem = new QTableWidgetItem;
            countryItem->setText(tableList.at(j));
            ui->tblCountryList->setItem(j, 0, countryItem);     // add to row j, col 0
            // also set an id here, countryRowj - determine if I need this id
        }

       update();

       ignoreSignals_ = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ShortListEditor::stepSetup(const SessionMessage& msg)
{

    DomCoolProdStepCatList m( msg );

    stepListSize = m.numOfItems();
    stepList = m.names();

    //build the prodution step list - this was from buildLists() and merged here
    if (stepListSize > 0)
    {
        ignoreSignals_ = true;
        ui->tblStepList->clear();
        ui->tblStepList->setRowCount( stepListSize);
        ui->tblStepList->setColumnCount( 1 );
        ui->tblStepList->setColumnWidth(0,420);

        QVector<QString> tableList = TokenParser::splitStringTokens( stepList );

        for( int j = 0; j < stepListSize; j++ )
        {
            QTableWidgetItem *stepItem = new QTableWidgetItem;
            stepItem->setText(tableList.at(j));
            ui->tblStepList->setItem(j, 0, stepItem);     // add to row j, col 0
            // also set an id here, stepRowj - determine if I need this id
        }

       update();

       ignoreSignals_ = false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
void ShortListEditor::addCountry(int row, int col)
{
    Q_UNUSED( col );

    if( !ignoreSignals_ )
    {
        hasChanged_ = true;

        // STEP 1: Before we do anything do some row selecting/deselecting
        if(row >= 0 && row <= countryListSize)
        {
            // means we have a row already selected
            if(row == countrySelectedRow)
            {
                // then we clicked the same row - WHY? I don't know, just clear it anyway
                ui->tblCountryList->clearSelection();
                countrySelectedRow = -1;
            }
            else
            {
                // Let QTTable do its normal behavior - switch rows
                ui->tblCountryList->selectRow(row);
                countrySelectedRow = row;
            }
        }

        // STEP 2: move items around based on click of a new Production Step
        if( countrySelectedRow >= 0)
        {
            QTableWidgetItem *countryItem = new QTableWidgetItem;   // get the Country name from the clicked row
            countryItem = ui->tblCountryList->item(countrySelectedRow, 0);
            QString countryName = countryItem->text();              // text in the row selected

            QString tempStr = countryName.replace("&amp","&");      // just in case its there
            countryName = tempStr;                                  // put it back in


            if( ui->teCoolText->toPlainText().length() == 1 && coolText.length() == 0 )
                coolText = "";
            else
                coolText = ui->teCoolText->toPlainText();

            // we want to find out if the selected country is already in the list
            int length = currentCountryArray.length();                  // current list of countries in the array
            bool isInList = false;                                  // by defaults its not there
            int j = 0;

            while ( (length > 0) && (isInList == false) && (j < length) )
            {
                if ( countryName == currentCountryArray[j] )
                    isInList = true;                                // means we found this country in the short list
                j++;
            }

            if (isInList == false)                                  // means we can add it
            {
                currentCountryArray.append(countryName);            // add country to the end of the array

                currentCountryList = "";                            // Rebuild the country list in alphabetical order
                length = currentCountryArray.length();              // how long is the array?

                if(length > 0)
                {
                    if(length > 1)
                        currentCountryList += ", ";

                    currentCountryList += currentCountryArray[length - 1];   // grab the last item in the array
                }

                tempStr = coolText.append(currentCountryList);      // right now it's going to append the entire array each time
                ui->teCoolText->setText( tempStr );                 //

            }   // if (isInList == false)
        }       // if countrySelectedRow >= 0
    }           // if ignoreSignals_
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ShortListEditor::addStep(int row, int col)
{
    Q_UNUSED( col );

    if( !ignoreSignals_ )
    {
        hasChanged_ = true;

        // STEP 1: Before we do anything do some row selecting/deselecting
        if(row >= 0 && row <= stepListSize)
        {
            // means we have a row already selected
            if(row == stepSelectedRow)
            {
                // then we clicked the same row - WHY? I don't know, just clear it anyway
                ui->tblStepList->clearSelection();
                stepSelectedRow = -1;
            }
            else
            {
                // Let QTTable do its normal behavior - switch rows
                ui->tblStepList->selectRow(row);
                stepSelectedRow = row;
            }
        }

        // STEP 2: move items around based on click of a new Production Step
        if( stepSelectedRow >= 0)
        {
            QTableWidgetItem *stepItem = new QTableWidgetItem;      // get the Country name from the clicked row
            stepItem = ui->tblStepList->item(stepSelectedRow, 0);
            QString stepName = stepItem->text();

            QString tempStr = stepName.replace("&amp", "&");        // just in case it has this
            stepName = tempStr;


            if( ui->teCoolText->toPlainText().length() == 1 && coolText.length() == 0 )
                coolText = "";
            else
                coolText = ui->teCoolText->toPlainText();

            int length = coolText.length();

            if (length > 0)
                coolText += " ";

            coolText += stepName + " ";

            ui->teCoolText->setText( coolText );

            currentCountryArray.clear();
            currentCountryList = "";
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ShortListEditor::onSessionMessageReceiveInternal( const SessionMessage& msg )
{
    onSessionMessageReceive( msg );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void ShortListEditor::updateDoneFormating()
{

    if (hasChanged_ && coolText != "" )
    {
        // FT-550 Create Cool text is duplicating ending country
        // right here is where I am adding the duplicate, no idea why, so I am
        // removing it.  Looking at the HLX this is a direct port, (superEditCOOLText.asp line 61 )
        // but for here I dont see the need.  So I am removing it.
        //coolText += currentCountryList;

        //	Set the COOL text number in the product record.
        currentPLUData->coolDefaultTextNumRaw() = QString::number(coolRecordNum);

        //	Save the new Default COOL text record to the DB.
        QString msg = currentPLUData->coolDefaultTextNumRaw() + ",\"" + coolText + "\"";
        session_->send(COOLDev, COOLDevSaveCoolText, msg);          // 135,106 - saveCoolText

        //	Append the new COOL text record to the end of the Product's shortlist.
        msg  = currentPLUData->itemNumber() + "," +					//	Product number
               currentPLUData->coolDefaultTextNumRaw() + "," +      //	Cool text number
               "0";                                      //	Append to the end
        session_->send(COOLDev,COOLDevShortlistAddText, msg);       //	135,115 - ShortListAddText
    }

    emit shortListEditorDoneClicked( hasChanged_ );                 // trigger a save of PLU record

    setVisible(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void ShortListEditor::updateCancelFormating()
{
    // Don't have to do anything here, just hide the editor and return back to the COOL tab

    emit shortListEditorCancelClicked();

    setVisible(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ShortListEditor::clearCoolText()
{
    coolText = "";
    ui->teCoolText->clear();
    currentCountryArray.clear();
    currentCountryList = "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ShortListEditor::onSessionMessageReceive( const SessionMessage& msg )
{
    if ((COOLDev == msg[0].toInt() ) && ( COOLDevCreateNewCoolText == msg[1].toInt() ))
    {
        //	received 135, 112 -- now request 135,38 -  read category countries
        coolRecordNum = msg[2].toInt();
        session_->send(COOLDev, COOLDevReadByAllAndByCategoryCountries, QString::number(currentCategoryNumber));                // 135,38
    }
    else if ((COOLDev == msg[0].toInt() ) && ( COOLDevWriteCountriesByCategory == msg[1].toInt() ))
    {
        // received 135,34 - now set up the countries list
        countrySetup(msg);

        // request 135,58 - we have the countries, now go request the production steps
        session_->send(COOLDev, COOLDevReadByAllAndByCategoryProductionSteps, QString::number(currentCategoryNumber));             //  COOLDev, COOLDevReadAllProductionStepsByCategory, category number
    }
    else if ((COOLDev == msg[0].toInt() ) && ( COOLDevWriteProductionStepsByCategory == msg[1].toInt() ))
    {
        // received 135,54 - now we got the production steps back - lets go setup and then fill the list
        stepSetup(msg);
    }

}


} // hobart namespace
