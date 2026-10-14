/* @file SuperCoolMain.cpp
 * Super Cool Main allows operators to assign COOL countries, COOL Production Steps, and
 * COOL pretext to Defined COOL Categories.  The countries, steps and pretext can also be
 * added to the ANY category. The asp page that represents this class is the vaguely named
 * and misunderstood web page named superCOOLLinkCategories.asp 
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Job E. Crew
 */

#include <logmanager.h>

#include <QtGui>
#include <QtCore>

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/supervisormode.h"
#include <ScaleConfiguration.h>
#include <TokenParser.h>
#include <UiUtilities.h>
#include "superCoolMain.h"
#include "superCoolCreateCategories.h"
#include "superCoolCreatePretext.h"
#include "superCoolCreateProdSteps.h"
#include "superCoolCreateCountries.h"
#include "ui_superCoolMain.h"
#include "TokenParser.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

SuperCoolMain::SuperCoolMain(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperCoolMain)
{
    ui->setupUi(this);
    scaleScreen();

    mainWindow_ = mainWindow;

    initialize();
    translate();
}

SuperCoolMain::~SuperCoolMain()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::initialize()
{
    currentCategoryNumber = 0;                                      // the current category number
    numOfItems = 0;                                                 // how many are there to process
    editingMode = EditNone;                                         // what are we editing, 1=countries, 2=prod steps, 3=pretext
    hasChanged = false;                                             // indicates if we have moved anything
    suspendCoolMsgs = false;                                        // when user clicks move button, disable activities

    // editing buttons
    connect(ui->butEditCountries,   SIGNAL(clicked()),                  this,   SLOT(editCountries()));                  // click edit countries
    connect(ui->butEditSteps,       SIGNAL(clicked()),                  this,   SLOT(editSteps()));                      // click edit steps
    connect(ui->butEditPretext,     SIGNAL(clicked()),                  this,   SLOT(editPretext()));                    // click edit pretext

    // Goto buttons
    connect(ui->butGotoCategories,  SIGNAL(clicked()),                  this,   SLOT(newCategories()));                  // click new categories
    connect(ui->butGotoCountries,   SIGNAL(clicked()),                  this,   SLOT(newCountries()));                   // click new countries
    connect(ui->butGotoSteps,       SIGNAL(clicked()),                  this,   SLOT(newSteps()));                       // click new steps
    connect(ui->butGotoPretext,     SIGNAL(clicked()),                  this,   SLOT(newPretext()));                     // click new pre-text

    // moving buttons
    connect(ui->butToTheLeft,       SIGNAL(clicked()),                  this,   SLOT(moveItemToTheLeftAvailableList())); // click <<
    connect(ui->butToTheRight,      SIGNAL(clicked()),                  this,   SLOT(moveItemToTheRightSelectedList())); // click >>
    connect(ui->butDone,            SIGNAL(clicked()),                  this,   SLOT(doneEditing()));                    // click DONE

    // Category combo box
    connect(ui->cboCategoryList,    SIGNAL(currentIndexChanged(int)),   this,   SLOT(getLinksToSelectedCategory(int)));  // select new index

    ui->lstAvailCountries->setVisible(false);
    ui->lstAvailProdSteps->setVisible(false);
    ui->lstAvailPretext->setVisible(false);
    ui->lstDialogSelCountries->setVisible(false);
    ui->lstDialogSelProdSteps->setVisible(false);
    ui->lstDialogSelPretext->setVisible(false);

    ui->Dialog->setVisible(false);

    ui->lstAvailCountries->addItem(tr("None"));
    ui->lstAvailPretext->addItem(tr("None"));
    ui->lstAvailProdSteps->addItem(tr("None"));
    ui->lstCatCountryList->addItem(tr("None"));
    ui->lstCatPretextList->addItem(tr("None"));
    ui->lstCatStepsList->addItem(tr("None"));
    ui->lstDialogSelCountries->addItem(tr("None"));
    ui->lstDialogSelPretext->addItem(tr("None"));
    ui->lstDialogSelProdSteps->addItem(tr("None"));

    update();


    session_->send(COOLDev, COOLDevReadAllCountries);           // 135,21 -> read all COOL Countries
}

//categoryRecordIds;
//countryRecordIds;
//prodstepRecordIds;
//pretextRecordIds;

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperCoolMain::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::translate()
{
	/* ALL Widget Text MUST be in here even if its the default text */

    ui->retranslateUi( this );

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::buildCOOLLists(QWidget *widget, QString names, int numOfItems)
{
    // widget - the widget we are adding names to, the available list on the DIALOG
    // names - the names we are adding to the widget
    // numOfItems - the number of items to make an array of names
    // doRemoveNoneOption - indicates if we leave or add the initial None option
    // 2 widgets are filled up in here: availProdSteps, availCountries

    QListWidget *myList = dynamic_cast<QListWidget*> ( widget );
    QString myNames = names;

    if( numOfItems > 0 )
    {
        QVector<QString> vectorList( TokenParser::splitStringTokens( myNames ) );
        int vecLen = vectorList.size();

        if( myList->count() == 1)
            myList->clear();                                        // remove None from the list
                              // no for loops here to add items, w00t
        for(int j = 0; j < vecLen; j++)
            myList->addItem(vectorList[j]);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::buildCOOLListFromList(QWidget *availWidget, QWidget *selWidget, QStringList &availArray, QStringList &selArray, const QString& names, int numOfItems)
{
    // widget - the widget we are adding names to
    // names - the names we are adding to the widget
    // numOfItems - the number of items to make an array of names
    // doRemoveNoneOption - indicates if we leave or add the initial None option
    // 2 widgets are filled up in here: availProdSteps, availCountries, from the category list

    // Build both the list of Countries and Production Steps by selecting a category

    QListWidget *myAvailableList = dynamic_cast<QListWidget*>( availWidget );   // oAvailableList = lstAvailable(country/prodsteps)
    QListWidget *mySelectedList = dynamic_cast<QListWidget*>( selWidget );      // oSelectedList = lstSelected(country/prodsteps)
    QString myNames = names;

    int totalAvail = myAvailableList->count();

    if( (totalAvail == 1) && myAvailableList->item(0)->text() == tr("None") )
        totalAvail = 0;
    else if(totalAvail > 1 && myAvailableList->item(0)->text() == tr("None"))
    {
        myAvailableList->takeItem(0);                               // get this out of here
        totalAvail--;
    }

    if( totalAvail > 0 )
    {
        if( numOfItems > 0 )
        {
            QVector<QString> myArray( TokenParser::splitStringTokens( myNames ) );

            if(mySelectedList->count() == 1)
                mySelectedList->clear();                            // remove NONE

            QString itemInList = "";
            QString numInList  = "";
            for( int j = 0; j < numOfItems; j++)
            {
                for( int k = 0; k < totalAvail; k++ )
                {
                    itemInList = myAvailableList->item(k)->text();

                    if( itemInList == myArray[j] )
                    {

                        numInList = availArray.at(k);
                        myAvailableList->takeItem(k);               // remove item from available list and array
                        availArray.removeAt(k);

                        --totalAvail;

                        mySelectedList->addItem(itemInList);        // add item to selected list and array
                        selArray.append(numInList);
                        break;
                    }
                }
            }

            if (totalAvail == 0)
            {
                myAvailableList->addItem(tr("None"));            //	Put the none option back in the Available list.
            }

        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::setupListsToEdit()
{
    // Just to let you know (and to remind me because I forgot I did it),
    //  1. all lists in the DIALOG are hidden coming in here
    //  2. all lists have been cleared someplace else
    //  3. all record id arrays representing lists have been cleared
    //  4. The DIALOG is hidden

    int editMode = editingMode;
    QString selText = "";

    if( editMode == EditCountries )
    {
        clearSelectedList( editMode );                              // remove the NONE option so its empty now if that was only thing there

        ui->lstAvailCountries->setVisible(true);                    // show available countries
        ui->lstDialogSelCountries->setVisible(true);                // show the selected dialog country list

        for( int j = 0; j < ui->lstCatCountryList->count(); j++ )
        {
            selText = getMainFormSelectedListText(editMode, j);
            ui->lstDialogSelCountries->addItem( selText );          // populate the dialog selected list with main form selected list
        }
    }
    else if( editMode == EditProductionSteps )
    {
        clearSelectedList( editMode );                              // remove the NONE option so its empty now if that was only thing there

        ui->lstAvailProdSteps->setVisible(true);                    // show available prod steps
        ui->lstDialogSelProdSteps->setVisible(true);                // hide the selected dialog prod steps

        for( int j = 0; j < ui->lstCatStepsList->count(); j++ )
        {
            selText = getMainFormSelectedListText(editMode, j);
            ui->lstDialogSelProdSteps->addItem( selText );          // populate the dialog selected list with main form selected list
        }
    }
    else if( editMode == EditPretext )
    {
        clearSelectedList( editMode );                              // remove the NONE option so its empty now if that was only thing there

        ui->lstAvailPretext->setVisible(true) ;                     // show available pretext
        ui->lstDialogSelPretext->setVisible(true);                  // hide the selected dialog pretext

        for( int j = 0; j < ui->lstCatPretextList->count(); j++ )
        {
            selText = getMainFormSelectedListText(editMode, j);
            ui->lstDialogSelPretext->addItem( selText );             // populate the dialog selected list with main form selected list
        }
    }

    ui->Dialog->setVisible(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::doneEditingTheList()
{
    // we may or may not have new data in the lstDialogs Lists.  Need to transfer
    // that data back to the main form area lists.

    if(hasChanged)
    {
        // Need to populate the selected lists on the main form to what is
        // contained in the selected list on the dialog

        int editMode = editingMode;
        QString selText = "";

        if( editMode == EditCountries )
        {
            ui->lstCatCountryList->clear();
            //selCountryIds.clear();

            for( int j = 0; j < ui->lstDialogSelCountries->count(); j++ )
            {
                selText = getSelectedListText(editMode, j);         // get the text to add
                addItemToMainFormSelectedList(editMode, selText);   // now add it
            }

            ui->lstDialogSelCountries->clear();                     // clear the DIALOG list on leaving
            //selCountryIds.clear();                            // clear the DIALOG record id array
        }
        else if( editMode == EditProductionSteps )
        {
            ui->lstCatStepsList->clear();
            //selProdstepIds.clear();

            for( int j = 0; j < ui->lstDialogSelProdSteps->count(); j++ )
            {
                selText = getSelectedListText(editMode, j);         // get the text to add
                addItemToMainFormSelectedList(editMode, selText);   // now add it
            }

            ui->lstDialogSelProdSteps->clear();                     // clear the DIALOG list on leaving
            //selProdstepIds.clear();                           // clear the DIALOG record id array
        }
        else if( editMode == EditPretext )
        {
            ui->lstCatPretextList->clear();
            //selPretextIds.clear();

            for( int j = 0; j < ui->lstDialogSelPretext->count(); j++ )
            {
                selText = getSelectedListText(editMode, j);         // get the text to add
                addItemToMainFormSelectedList(editMode, selText);   // now add it
            }

            ui->lstDialogSelPretext->clear();                       // clear the DIALOG list on leaving
            //selPretextIds.clear();                            // clear the DIALOG record id array
        }
    }

    ui->Dialog->setVisible(false);                                  // hide the dialog

    ui->lstAvailCountries->setVisible(false);                       // hide available countries
    ui->lstAvailProdSteps->setVisible(false);                       // hide available prod steps
    ui->lstAvailPretext->setVisible(false);                         // hide available pretext

    ui->lstDialogSelCountries->setVisible(false);                   // hide the selected dialog country list
    ui->lstDialogSelProdSteps->setVisible(false);                   // hide the selected dialog prod steps
    ui->lstDialogSelPretext->setVisible(false);                     // hide the selected dialog pretext

    editingMode = EditNone;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::buildAllCountriesList(const SessionMessage& msg)
{
    // 135,23 - Processes the 135,23 message for COOL Countries
    // builds the list of available Countries on the DIALOG
    DomCoolCountryList m( msg );
    numOfItems = m.numOfItems();
    
    if( numOfItems > 0 )
    {
        // updates the country ids
        availCountryIds.clear();

        for( int j = 0; j < numOfItems; j++ )
        {
            availCountryIds.append( QString::number(j) );
        }
    }

    buildCOOLLists(ui->lstAvailCountries, m.names(), numOfItems);

    session_->send(COOLDev, COOLDevReadAllProductionSteps);         // 135,41 -> read all COOL Production Steps
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::buildAllProductionStepsList(const SessionMessage& msg)
{
    // 135,23 - Processes the 135,43 message for COOL Countries
    // builds the list of available Production Steps on the DIALOG

    DomCoolProdStepList m( msg );
    numOfItems = m.numOfItems();

    if( numOfItems > 0 )
    {
        // updates the country ids
        availProdstepIds.clear();
        for( int j = 0; j < numOfItems; j++ )
        {
            availProdstepIds.append( QString::number(j) );
        }
    }

    buildCOOLLists(ui->lstAvailProdSteps, m.names(), numOfItems);

    session_->send(COOLDev, COOLDevReadAllPretexts);                // 135,61 -> read all COOL Pre-text
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::buildAllPretextsList(const SessionMessage& msg)
{
    // Received a 135,63 message
    // Adds items to the available Pretexts List on the DIALOG box, but not selected.

    DomCoolPretextList m( msg );
    int numOfItems = m.numOfItems();

    if(numOfItems > 0)
    {
        QString tmpNames = m.names();
        QString tmpNums = m.recordNums();

        availPretextIds.clear();

        QVector<QString> pretextNameArray( TokenParser::splitStringTokens( tmpNames ) );
        int pretxtLen = pretextNameArray.count();

        availPretextIds = tmpNums.split(",");

        if(ui->lstAvailPretext->count() == 1)
            ui->lstAvailPretext->clear();                           // clear away NONE since that is the only thing in there

        for(int j = 0; j < pretxtLen; j++)
            ui->lstAvailPretext->addItem( pretextNameArray[j] );         // now add the rest of the items

    }

    session_->send(COOLDev, COOLDevReadAllCategories);              // 135,81 -> read all COOL Categories
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::buildAllCategoriesList(const SessionMessage& msg)
{
    // Received 135,83 - Builds the Category Combo box list on the MAIN form

    DomCoolCategoryList m( msg );
    int numOfItems =  m.numOfItems();

    if(numOfItems > 0)
    {
        ui->butEditCountries->setEnabled(true);                     // see the else why I have to do this
        ui->butEditPretext->setEnabled(true);                       // I  may get a blank list of categories
        ui->butEditSteps->setEnabled(true);                         // which disables these 3 buttons

        QString tmpNames = m.names();
        QString tmpNums = m.recordNums();

        categoryRecordIds.clear();                                  // clear the list of category Id #s

        QVector<QString> categoryNameArray( TokenParser::splitStringTokens( tmpNames ) );
        int catLen = categoryNameArray.count();

        categoryRecordIds = tmpNums.split(",");

        if(ui->cboCategoryList->count() == 1)
            ui->cboCategoryList->clear();                           // clear away NONE of that is the only thing in there

        categoryRecordIds.prepend("0");                             // need to insert at [0] the record id for the ANY category
        ui->cboCategoryList->addItem( tr("Any") );                  // add the ANY item to the list first

        for(int j = 0; j < catLen; j++)
            ui->cboCategoryList->addItem( categoryNameArray[j] );         // now add the rest of the categories

        currentCategoryNumber = categoryRecordIds.at(0).toInt();    // right here, the currentCategoryNumber will always be 0

        if(currentCategoryNumber > 0)
            session_->send(COOLDev, COOLDevReadByCategoryOnlyCountries, QString::number(currentCategoryNumber) ); // 135,33,Cat# -> Read Current COOL Category info per country
        else
            session_->send(COOLDev, COOLDevReadByCategoryOnlyCountries);         // 135,33,Cat# -> Read Current COOL Category info per country

    }
    else
    {
        // FT-700 Assign to COOL Categories does not have an "Any" category
        // If there are no categories, we still need to add the ANY category to the list
        // This reported bug is not right. If there are no categories, you need to add "None" per HLX
        categoryRecordIds.prepend("0");                             // need to insert at [0] the record id for the ANY category
        ui->cboCategoryList->addItem( tr("None") );                  // add the None item to the list first if no items
        currentCategoryNumber = categoryRecordIds.at(0).toInt();    // Also the item buttons are disabled at this point
        ui->butEditCountries->setEnabled(false);
        ui->butEditPretext->setEnabled(false);
        ui->butEditSteps->setEnabled(false);
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::buildCountriesListByCategory(const SessionMessage& msg)
{
    // received a 135,34
    // update the available Countries List on the DIALOG box, based upon selected category

    DomCoolCountryCatList m( msg );
    editingMode = EditCountries;
    resetSelectAvailLists();
    editingMode = EditNone;
    numOfItems =  m.numOfItems();

    if( numOfItems > 0 )
    {
        // For this message, the recordIds do not exist like other messages, just the names.  That's confusing to me.

        buildCOOLListFromList(ui->lstAvailCountries, ui->lstCatCountryList, availCountryIds, selCountryIds, m.names(), numOfItems);
    }

    if(currentCategoryNumber > 0)       // where did the currentCategoryNumber get set? In method that SLOT handles changing the index? Better be.
        session_->send(COOLDev, COOLDevReadByCategoryOnlyProductionSteps, QString::number(currentCategoryNumber) ); // 135,53,Cat# -> Read Current COOL Category info per prod steps
    else
        session_->send(COOLDev, COOLDevReadByCategoryOnlyProductionSteps);         // 135,53,Cat# -> Read Current COOL Category info per prod steps
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::buildProductionStepsListByCategory(const SessionMessage& msg)
{
    // received a 135,34
    // updates the available list by removing items in it and adding them to the selected list
    DomCoolProdStepCatList m( msg );
    editingMode = EditProductionSteps;
    resetSelectAvailLists();
    editingMode = EditNone;
    numOfItems =  m.numOfItems();

    if( numOfItems > 0 )
    {
        // For this message, the recordIds do not exist like other messages, just the names.  That's confusing to me.

        buildCOOLListFromList(ui->lstAvailProdSteps, ui->lstCatStepsList, availProdstepIds, selProdstepIds, m.names(), numOfItems);
    }

    if(currentCategoryNumber > 0)
        session_->send(COOLDev, COOLDevReadByCategoryOnlyPretexts, QString::number(currentCategoryNumber)); // 135,73,Cat# -> Read Current COOL Category info per prod steps
    else
        session_->send(COOLDev, COOLDevReadByCategoryOnlyPretexts);         // 135,73,Cat# -> Read Current COOL Category info per prod steps
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::buildPretextsListByCategory(const SessionMessage& msg)
{
    // Received 135,74
    // builds the Available Pretext list on the DIALOG
    DomCoolPretextCatList m( msg );
    editingMode = EditPretext;
    resetSelectAvailLists();
    editingMode = EditNone;
    int totalAvail = ui->lstAvailPretext->count();

    if( (totalAvail == 1) && (ui->lstAvailPretext->item(0)->text() == tr("None") ) )
        totalAvail = 0;

    if( totalAvail > 0 )
    {
        int numOfItems = m.numOfItems();

        if( numOfItems > 0 )
        {
            QString tmpNums = m.recordNums();
            QString tmpNames = m.names();

            selPretextIds.clear();

            QVector<QString> pretextNameArray( TokenParser::splitStringTokens( tmpNames ) );      
            selPretextIds = tmpNums.split(",");

            ui->lstAvailPretext->blockSignals(true);                // makes the repaint faster to remove items
            ui->lstCatPretextList->blockSignals(true);

            if( ui->lstCatPretextList->count() == 1)
                ui->lstCatPretextList->clear();                     // remove NONE from selected list on MAIN form

            QString itemInList = "";

            // Essentially find out if any of the incoming names exist in the available list
            // on the DIALOG.  If they do sit in the available list, remove it and add to selected list.
            for( int j = 0; j < numOfItems; j++ )
            {
                totalAvail = ui->lstAvailPretext->count();          // how many items in available list on DIALOG

                for(int k = 0; k < totalAvail; k++ )
                {
                    itemInList = ui->lstAvailPretext->item(k)->text();
                    if( itemInList == pretextNameArray[j] )
                    {
                        ui->lstAvailPretext->takeItem(k);           // remove from the available list in DIALOG
                        availPretextIds.removeAt(k);                // remove the record id as well
                        ui->lstCatPretextList->addItem(itemInList); // add to the selected list in MAIN

                        break;
                    }
                }
            }

            totalAvail = ui->lstAvailPretext->count();

            if( totalAvail == 0)
            {
                // Put the none option back in
                ui->lstAvailPretext->insertItem(0, tr("None") );

            }

            ui->lstAvailPretext->blockSignals(false);
            ui->lstCatPretextList->blockSignals(false);

        }
    }

    // There is a chunk of javascript code I left out here

    // Enable the buttons
    ui->butEditCountries->setEnabled(true);
    ui->butEditSteps->setEnabled(true);
    ui->butEditPretext->setEnabled(true);
}

// started having weird issues with this, no idea why. Got rid of the references in Strings
void SuperCoolMain::recordSelection(int editMode, const QString &theValue, const QString &theText)
{
    if (editMode == EditCountries)
    {
        session_->send(COOLDev, COOLDevAddCountryToCategory, "\"" + theText + "\",," + QString::number(currentCategoryNumber) );
    }
    else if (editMode == EditProductionSteps)
    {
        session_->send(COOLDev, COOLDevAddProductionStepToCategory, "\"" + theText + "\",," + QString::number(currentCategoryNumber) );
    }
    else if (editMode == EditPretext)
    {
        session_->send(COOLDev, COOLDevAddPretextToCategory, theValue + ",,," + QString::number(currentCategoryNumber) );
    }
}

// started having weird issues with this, no idea why. Got rid of the references in Strings
void SuperCoolMain::recordDeletion(int editMode, const QString &theValue, const QString &theText)
{
    QString category = QString::number( ui->cboCategoryList->currentIndex() );

    if (editMode == EditCountries)
    {
        session_->send(COOLDev, COOLDevDeleteCountryFromCategory, "\"" + theText + "\",," + category);
    }
    else if (editMode == EditProductionSteps)
    {
        session_->send(COOLDev, COOLDevDeleteProductionStepFromCategory, "\"" + theText + "\",," + category);
    }
    else if (editMode == EditPretext)
    {
        session_->send(COOLDev, COOLDevDeletePretextFromCategory,  theValue + ",,," + category);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::editCountries()
{
    // do something if a user clicks a edit countries button
    editingMode = EditCountries;
    ui->lblDialogTitleBlock->setText(tr("Editing Countries"));
    ui->lblTitle1->setText(tr("Available Countries"));
    ui->lblTitle2->setText(tr("Selected Countries"));
    setupListsToEdit();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::editSteps()
{
    // do something if a user clicks edit production steps button
    editingMode = EditProductionSteps;
    ui->lblDialogTitleBlock->setText(tr("Editing Production Steps"));
    ui->lblTitle1->setText(tr("Available Production steps"));
    ui->lblTitle2->setText(tr("Selected Production steps"));
    setupListsToEdit();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::editPretext()
{
    // do something if a user clicks the edit pretext button
    editingMode = EditPretext;
    ui->lblDialogTitleBlock->setText(tr("Editing Pre-text"));
    ui->lblTitle1->setText(tr("Available Pre-text"));
    ui->lblTitle2->setText(tr("Selected Pre-text"));
    setupListsToEdit();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::newCategories()
{
    // do something if a user clicks a button
    mainWindow_->switchToModeScreen<SupervisorMode, SuperCoolCreateCategories>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::newCountries()
{
    // do something if a user clicks a button
    mainWindow_->switchToModeScreen<SupervisorMode, SuperCoolCreateCountries>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::newSteps()
{
    // do something if a user clicks a button
    mainWindow_->switchToModeScreen<SupervisorMode, SuperCoolCreateProdSteps>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::newPretext()
{
    // do something if a user clicks a button
    mainWindow_->switchToModeScreen<SupervisorMode, SuperCoolCreatePretext>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::moveItemToTheLeftAvailableList()
{
    // do something if a user clicks a move button. This method moves something
    // from the selected list on the right to the available list on the left IN THE DIALOG.
    hasChanged = true;

    if(suspendCoolMsgs)
        return;

    suspendCoolMsgs = true;

    int editMode = editingMode;

    int selectedLength = getSelectedListLength( editMode );
    int selectedIndex = getSelectedCurrentRowIndex( editMode );

    if(selectedIndex < 0 || selectedLength < 1 )
    {
        suspendCoolMsgs = false;
        return;
    }
    QString selText = getSelectedListText( editMode,selectedIndex );

    if( (selectedLength > 1) || ((selectedLength == 1) && selText != tr("None")) )
    {
        if(selectedIndex < 0)
        {
            // safety net.  User selected to move something to the right, but nothing
            // on the left is selected. So how is index -1? how this is possible? something is
            // not selected. So set it to 0 and move that item.
            selectedIndex = 0;
            setSelectedCurrentRowIndex(editMode, selectedIndex);    // set a row as active, if nothing active, set 0th element as it
        }

        // remove the item and the recordID from the selected list
        QString selectedText = removeItemFromSelectedList( editMode, selectedIndex ); // remove item from the left selected list
        int selectedRecordId = removeRecIdFromSelectedList( editMode, selectedIndex); // remove record id of item from selected list
        selectedLength = getSelectedListLength( editMode );         // selected list: how long is it now? - test after deleting

        recordDeletion( editMode, QString::number(selectedRecordId), selectedText );           // delete the record

        if(selectedLength == 0)
        {
            resetSelectedListToNone( editMode );                   // if selected list empty, fill it
            setSelectedCurrentRowIndex(editMode, 0);               // set None row as active

        }
        else
        {
            if( selectedIndex == selectedLength )                              // if we still have items, set something in the available list active
                setSelectedCurrentRowIndex(editMode, selectedIndex-1);
            else
                setSelectedCurrentRowIndex(editMode, selectedIndex);
        }

        //	Now move this option back to the Available list (left side).
        int availableLength = getAvailableListLength( editMode );
        QString availableText = getAvailableListText( editMode, 0 );            // get text at 0 element

        if( availableLength == 1 && availableText == tr("None") )
        {
            clearAvailableList( editMode );                         // remove the NONE option so its empty now if that was only thing there
            addItemToAvailableList( editMode, selectedText);        // now add the selected item (right side) to available list (left side) at end of list
            setAvailableCurrentRowIndex(editMode, 0);               // highlight the only item in the list
            addRecordIdToAvailableList(editMode, QString::number(selectedRecordId)); // add the id to the available list
        }
        else
        {
            int recordId = 0;
            int foundIndex = 0;
            bool foundIt = false;

            for( int j = 0; j < availableLength; j++ )              // iterate thru the list so we can find out to where to insert it
            {
                recordId = getRecIdFromAvailableList( editMode, j);

                if(recordId > selectedRecordId )                    // if true, means we found an recordId larger than the recordId we want to insert
                {
                    foundIndex = j;                                 // set the foundIndex to the counter and jump out
                    foundIt = true;
                    break;
                }
            }

            if( foundIt )
            {
                insertRecordIdToAvailableList(editMode, QString::number(selectedRecordId), foundIndex );
                insertItemIntoAvailableList(editMode, selectedText, foundIndex);
                setAvailableCurrentRowIndex(editMode, foundIndex);
            }
            else
            {
                addRecordIdToAvailableList(editMode, QString::number(selectedRecordId));
                addItemToAvailableList(editMode, selectedText );
                setAvailableCurrentRowIndex(editMode, availableLength);
            }
        }
    }

    suspendCoolMsgs = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::moveItemToTheRightSelectedList()
{
    // do something if a user clicks a move button. This method moves something
    // from the available list on the left to the selected list on the right IN THE DIALOG.

    hasChanged = true;

    if(suspendCoolMsgs)
        return;

    suspendCoolMsgs = true;

    int editMode = editingMode;

    int availLength =  getAvailableListLength(editMode);            // available list: how long is it? - test length b4 deleting
    if ( availLength == 0 )
        return;

    int availableIndex = getAvailableCurrentRowIndex( editMode );   // what row is currently selected in the available list?
    if(availableIndex < 0 || availLength < 1 )
    {
        suspendCoolMsgs = false;
        return;                                                     // protect yourself, if nothing selected, bail.
    }

    QString availText = getAvailableListText(editMode, availableIndex);

    if( (availLength > 1) || ((availLength == 1) && availText != tr("None")) )
    {
        // if there is more than one item in the available list,
        // or if there is only one item, it CANNOT be None.

        if(availableIndex < 0)
        {
            // safety net.  User selected to move something to the right, but nothing
            // on the left is selected. So availableIndex is -1?. So something is
            // not selected. So set it to 0 and move that item.
            availableIndex = 0;
            setAvailableCurrentRowIndex(editMode, availableIndex);  // set a row as active, if nothing active, set 0th element as it
        }

        // remove the item and the recordID from the available list
        QString availableText = removeItemFromAvailableList( editMode, availableIndex ); // remove item from the left available list
        int availableRecordId = removeRecIdFromAvailableList( editMode, availableIndex); // remove record id of item from available list
        availLength = getAvailableListLength(editMode);             // avaiable list: how long is it now? - test after deleting

        recordSelection( editMode, QString::number(availableRecordId), availableText);         // save off the record

        if( availLength == 0 )
        {
            resetAvailableListToNone( editMode );                   // if available list empty, fill it
            setAvailableCurrentRowIndex(editMode, 0);               // set None row as active
        }
        else
        {
            if( availableIndex == availLength )                     // if we still have items, set something in the available list active
                setAvailableCurrentRowIndex(editMode, availableIndex - 1);
            else
                setAvailableCurrentRowIndex(editMode, availableIndex);
        }

        // Now add the item from the available list somewhere in the selected list
        int selectedLength = getSelectedListLength( editMode );
        QString selectedText = getSelectedListText( editMode, 0 );

        if( selectedLength == 1 && selectedText == tr("None") )
        {
            clearSelectedList( editMode );                          // remove the NONE option so its empty now if that was only thing there
            addItemToSelectedList( editMode, availableText);        // now add the available item (left side) to selected list (right side) at end of list
            setSelectedCurrentRowIndex(editMode, 0);                // highlight the only item in the list
            addRecordIdToSelectedList(editMode, QString::number(availableRecordId));
        }
        else
        {
            addItemToSelectedList( editMode, availableText);        // now add the available item (left side) to selected list (right side) at end of list
            setSelectedCurrentRowIndex( editMode, selectedLength);
        }

        // Need to add the new item to the record list as well.
        addRecordIdToSelectedList(editMode, QString::number(availableRecordId) );
    }

    update();
    suspendCoolMsgs = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::resetSelectAvailLists()
{
    // Each time we want to build a list, by selecting a category, we have items in the selected list
    // and items in the available list. To start fresh, once we select a given category, we remove each
    // item from the selected list and add it back to the available list. Then we request the category
    // information.  The available list is full of all of the items, and the selected list is blank.
    // Each item in a particular category selected is in the available list. Once we get the category
    // information, we remove each needed item from the available list and add it to the selected list
    // for that given category. This just seems like not being the most efficient way to do this.
    // This method clears the selected list and adds each of the items back to the available list.
    // Another method takes from the available list and adds it to the selected list. I don't know.

    if( suspendCoolMsgs )
        return;

    suspendCoolMsgs = true;

    int editMode = editingMode;
    int selectedLength = getMainFormSelectedListLength(editMode);
    if ( selectedLength == 0 )
    {
        suspendCoolMsgs = false;
        return;
    }

    QString selectedText = getMainFormSelectedListText( editMode, 0);
    QString availableText = getAvailableListText( editMode, 0);
    int selectedValue = 0;
    int availableValue = 0;

    if( (selectedLength > 1) || ( (selectedLength == 1) && (selectedText != tr("None")) ))
    {
        int availableLen = getAvailableListLength( editMode );

        // see if we can clear the available list if just None is sitting there.
        if( (availableLen == 1) && (availableText == tr("None") && (selectedLength >= 1)))
            clearAvailableList(editMode);                           // remove the None by just clearing the whole available list

        // traverse the SELECTED list from the end to the beginning
        int endOfList = selectedLength - 1;

        for(int index = endOfList; index >= 0; index--)
        {
            selectedText = removeItemFromMainFormSelectedList(editMode, index);     // remove the item off of the selected list
            selectedValue = removeRecIdFromMainFormSelectedList(editMode, index);   // get record id of item in the selected list and then delete it

            availableLen = getAvailableListLength(editMode);                // now how long is the available list now?

            if( availableLen == 0)
            {
                // Add the just removed selected list item to the available list
                addItemToAvailableList( editMode, selectedText );                       // add selected item to the end of the available list
                addRecordIdToAvailableList(editMode, QString::number(selectedValue));   // add the record ID
            }
            else
            {
                // since items already exist in the available list, search through the list and see where we can insert this one
                int availableIndex = 0;
                bool foundIt = false;

                for(int j = 0; j < availableLen; j++)
                {
                    availableValue = getRecIdFromAvailableList(editMode, j);
                    if(availableValue > selectedValue)
                    {
                        availableIndex = j;
                        foundIt = true;
                        break;
                    }
                }

                // determine if we found a spot where we can insert the selected item into the available list
                // if we found a higher record id, insert before it. Otherwise attach it to the end
                if( foundIt )
                {
                    insertItemIntoAvailableList(editMode, selectedText, availableIndex);
                    insertRecordIdToAvailableList(editMode, QString::number(selectedValue), availableIndex);
                }
                else
                {
                    addItemToAvailableList(editMode, selectedText);             // add the removed selected item to end of available list
                    addRecordIdToAvailableList(editMode, QString::number(selectedValue));        // add the record id to the array list
                }
            }

            // NOTE: Im not inserting a record id with the NONE option, what is its ID, if any?
            selectedLength = getMainFormSelectedListLength(editMode);
            if(selectedLength == 0)
                addItemToMainFormSelectedList(editMode, tr("None"));
        }
    }

    setAvailableCurrentRowIndex(editMode, -1);
    setMainFormSelectedCurrentRowIndex(editMode, -1);

    update();
    suspendCoolMsgs = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::addItemToAvailableList(int editMode, const QString &strToAdd)
{
    QString myStrToAdd = strToAdd;

    if( editMode == EditCountries )
        ui->lstAvailCountries->addItem(myStrToAdd);
    else if( editMode == EditProductionSteps )
        ui->lstAvailProdSteps->addItem(myStrToAdd);
    else if( editMode == EditPretext )
        ui->lstAvailPretext->addItem(myStrToAdd);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::addItemToMainFormSelectedList(int editMode, const QString &strToAdd)
{
    QString myStr = strToAdd;

    if( EditCountries == editMode )
    {
        ui->lstCatCountryList->addItem(myStr);
    }
    else if( EditProductionSteps == editMode )
    {
        ui->lstCatStepsList->addItem(myStr);
    }
    else if( EditPretext == editMode )
    {
        ui->lstCatPretextList->addItem(myStr);
    }
}

void SuperCoolMain::addItemToSelectedList(int editMode, const QString &strToAdd)
{
    LOG_CLASS_INFO( "SuperCoolMain", "addItemToSelectedList" );

    QString myStrToAdd = strToAdd;      // i know i didnt have to do this, but weird stuff is happening

    if(1 == editMode)
    {
        ui->lstDialogSelCountries->addItem(myStrToAdd);
        LOG_DEBUG( "Countries myStrToAdd: %s", myStrToAdd.toLatin1().constData() );
    }
    else if(2 == editMode)
    {
        ui->lstDialogSelProdSteps->addItem(myStrToAdd);
        LOG_DEBUG( "Prod Steps myStrToAdd: %s", myStrToAdd.toLatin1().constData() );
    }
    else if(3 == editMode)
    {
        ui->lstDialogSelPretext->addItem(myStrToAdd);
        LOG_DEBUG( "Pretext myStrToAdd: %s", myStrToAdd.toLatin1().constData() );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::addRecordIdToAvailableList(int editMode, const QString &recordId)
{
    QString myId = recordId;

    if( 1 == editMode )
    {
        availCountryIds.append(myId);
    }
    else if( 2 == editMode )
    {
        availProdstepIds.append(myId);
    }
    else if( 3 == editMode )
    {
        availPretextIds.append(myId);
    }
}

void SuperCoolMain::addRecordIdToSelectedList(int editMode, const QString &recordId)
{
    LOG_CLASS_INFO( "SuperCoolMain", "addRecordIdToSelectedList" );

    QString myId = recordId;

    if(1 == editMode)
    {
        selCountryIds.append(myId);
        LOG_DEBUG( "Countries myId: %s", myId.toLatin1().constData() );
    }
    else if(2 == editMode)
    {
        selProdstepIds.append(myId);
        LOG_DEBUG( "Prod Steps myId: %s", myId.toLatin1().constData() );
    }
    else if(3 == editMode)
    {
        selPretextIds.append(myId);
        LOG_DEBUG( "Pretext myId: %s", myId.toLatin1().constData() );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::insertRecordIdToAvailableList(int editMode, const QString &recordId, int index)
{
    if( editMode == 1 )
        availCountryIds.insert(index, recordId);                           // add record id to the array
    else if( editMode == 2 )
        availProdstepIds.insert(index, recordId);
    else if( editMode == 3 )
        availPretextIds.insert(index, recordId);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::insertRecordIdToSelectedList(int editMode, const QString &recordId, int index)
{
    if( editMode == 1 )
        selCountryIds.insert(index, recordId);                       // add record id to the array
    else if( editMode == 2 )
        selProdstepIds.insert(index, recordId);
    else if( editMode == 3 )
        selPretextIds.insert(index, recordId);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::insertItemIntoAvailableList(int editMode, const QString &listStr, int index)
{
    if( editMode == 1 )
        ui->lstAvailCountries->insertItem(index, listStr);                           // add record id to the array
    else if( editMode == 2 )
        ui->lstAvailProdSteps->insertItem(index, listStr);
    else if( editMode == 3 )
        ui->lstAvailPretext->insertItem(index, listStr);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::insertItemIntoSelectedList(int editMode, const QString &listStr, int index)
{
    if( editMode == 1 )
        ui->lstDialogSelCountries->insertItem(index, listStr);                       // add record id to the array
    else if( editMode == 2 )
        ui->lstDialogSelProdSteps->insertItem(index, listStr);
    else if( editMode == 3 )
        ui->lstDialogSelPretext->insertItem(index, listStr);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperCoolMain::getAvailableListLength(int editMode)
{
    int availLen = 0;

    if( editMode == 1 )
        availLen = ui->lstAvailCountries->count();                  // how many is it?
    else if( editMode == 2 )
        availLen = ui->lstAvailProdSteps->count();
    else if( editMode == 3 )
        availLen = ui->lstAvailPretext->count();

    return availLen;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperCoolMain::getMainFormSelectedListLength(int editMode)
{
    int selLen = 0;

    if( 1 == editMode )
    {
        selLen = ui->lstCatCountryList->count();
    }
    else if( 2 == editMode )
    {
        selLen = ui->lstCatStepsList->count();
    }
    else if( 3 == editMode )
    {
        selLen = ui->lstCatPretextList->count();
    }

    return selLen;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperCoolMain::getSelectedListLength(int editMode)
{
    int selLen = 0;

    if( editMode == 1 )
    {
        selLen = ui->lstDialogSelCountries->count();                  // how many is it?
    }
    else if( editMode == 2 )
    {
        selLen = ui->lstDialogSelProdSteps->count();
    }
    else if( editMode == 3 )
    {
        selLen = ui->lstDialogSelPretext->count();
    }

    return selLen;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperCoolMain::getAvailableCurrentRowIndex(int editMode)
{
    int curRow = 0;

    if( editMode == 1 )
        curRow = ui->lstAvailCountries->currentRow();               // how long is it?
    else if( editMode == 2 )
        curRow = ui->lstAvailProdSteps->currentRow();
    else if( editMode == 3 )
        curRow = ui->lstAvailPretext->currentRow();

    return curRow;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperCoolMain::getSelectedCurrentRowIndex(int editMode)
{
    int curRow = 0;

    if( editMode == 1 )
        curRow = ui->lstDialogSelCountries->currentRow();               // how long is it?
    else if( editMode == 2 )
        curRow = ui->lstDialogSelProdSteps->currentRow();
    else if( editMode == 3 )
        curRow = ui->lstDialogSelPretext->currentRow();

    return curRow;
}

int SuperCoolMain::getRecIdFromAvailableList(int editMode, int index)
{
    int recId = 0;

    if( editMode == 1 && availCountryIds.count() > 0 )
        recId = availCountryIds.at(index).toInt();                  // how id is it?
    else if( editMode == 2 && availProdstepIds.count() > 0)
        recId = availProdstepIds.at(index).toInt();
    else if( editMode == 3 && availPretextIds.count() > 0)
        recId = availPretextIds.at(index).toInt();

    return recId;
}

int SuperCoolMain::getRecIdFromSelectedList(int editMode, int index)
{
    int recId = 0;

    if( editMode == 1 )
        recId = selCountryIds.at(index).toInt();                  // how id is it?
    else if( editMode == 2 )
        recId = selProdstepIds.at(index).toInt();
    else if( editMode == 3 )
        recId = selPretextIds.at(index).toInt();

    return recId;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperCoolMain::getAvailableListText(int editMode, int index)
{
    QString availText = "";

    if( editMode == 1 )
        availText = ui->lstAvailCountries->item(index)->text();         // what is your name first
    else if( editMode == 2 )
        availText = ui->lstAvailProdSteps->item(index)->text();
    else if( editMode == 3 )
        availText = ui->lstAvailPretext->item(index)->text();

    return availText;
}

QString SuperCoolMain::getSelectedListText(int editMode, int rowIndex)
{
    LOG_CLASS_INFO( "SuperCoolMain", "getSelectedListText" );

    QString selectedText = "";

    if(1 == editMode)
    {
        selectedText = ui->lstDialogSelCountries->item(rowIndex)->text();
        LOG_DEBUG( "Countries selectedText: %s", selectedText.toLatin1().constData() );
    }
    else if(2 == editMode)
    {
        selectedText = ui->lstDialogSelProdSteps->item(rowIndex)->text();
        LOG_DEBUG( "Prod Steps selectedText: %s", selectedText.toLatin1().constData() );
    }
    else if(3 == editMode)
    {
        selectedText = ui->lstDialogSelPretext->item(rowIndex)->text();
        LOG_DEBUG( "Pretext selectedText: %s", selectedText.toLatin1().constData() );
    }

    return selectedText;
}

QString SuperCoolMain::getMainFormSelectedListText(int editMode, int index)
{
    QString selText = "";

    if( editMode == EditCountries )
        selText = ui->lstCatCountryList->item(index)->text();   // what is your name first
    else if( editMode == EditProductionSteps )
        selText = ui->lstCatStepsList->item(index)->text();
    else if( editMode == EditPretext )
        selText = ui->lstCatPretextList->item(index)->text();

    return selText;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::setAvailableCurrentRowIndex(int editMode, int row)
{

    if( editMode == EditCountries )
        ui->lstAvailCountries->setCurrentRow(row);                  // set where it is?
    else if( editMode == EditProductionSteps )
        ui->lstAvailProdSteps->setCurrentRow(row);
    else if( editMode == EditPretext )
        ui->lstAvailPretext->setCurrentRow(row);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::setSelectedCurrentRowIndex(int editMode, int row)
{
    if( editMode == EditCountries )
        ui->lstDialogSelCountries->setCurrentRow(row);              // set where it is?
    else if( editMode == EditProductionSteps )
        ui->lstDialogSelProdSteps->setCurrentRow(row);
    else if( editMode == EditPretext )
        ui->lstDialogSelPretext->setCurrentRow(row);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::setMainFormSelectedCurrentRowIndex(int editMode, int row)
{
    if( editMode == EditCountries )
        ui->lstCatCountryList->setCurrentRow(row);
    else if( editMode == EditProductionSteps )
        ui->lstCatStepsList->setCurrentRow(row);
    else if( editMode == EditPretext )
        ui->lstCatPretextList->setCurrentRow(row);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::clearAvailableList(int editMode)
{
    if( editMode == EditCountries )
    {
        ui->lstAvailCountries->clear();                         // clear the whole list
        availCountryIds.clear();
    }
    else if( editMode == EditProductionSteps )
    {
        ui->lstAvailProdSteps->clear();
        availProdstepIds.clear();
    }
    else if( editMode == EditPretext )
    {
        ui->lstAvailPretext->clear();
        availPretextIds.clear();
    }
}

void SuperCoolMain::clearSelectedList(int editMode)
{
    if( editMode == EditCountries )
    {
        ui->lstDialogSelCountries->clear();                         // clear the whole list
        //selCountryIds.clear();
    }
    else if( editMode == EditProductionSteps )
    {
        ui->lstDialogSelProdSteps->clear();
        //selProdstepIds.clear();
    }
    else if( editMode == EditPretext )
    {
        ui->lstDialogSelPretext->clear();
        //selPretextIds.clear();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperCoolMain::removeItemFromSelectedList(int editMode, int row)
{
    LOG_CLASS_INFO( "SuperCoolMain", "removeItemFromSelectedList" );

    QString itemName = "";
    itemName = getSelectedListText(editMode, row);

    if(EditCountries == editMode)
    {
        ui->lstDialogSelCountries->takeItem(row);
        LOG_DEBUG( "Countries itemName: %s", itemName.toLatin1().constData() );
    }
    else if(EditProductionSteps == editMode)
    {
        ui->lstDialogSelProdSteps->takeItem(row);
        LOG_DEBUG( "Prod Steps itemName: %s", itemName.toLatin1().constData() );
    }
    else if(EditPretext == editMode)
    {
        ui->lstDialogSelPretext->takeItem(row);
        LOG_DEBUG( "Pretext itemName: %s", itemName.toLatin1().constData() );
    }

    return itemName;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperCoolMain::removeItemFromAvailableList( int editMode, int index )
{
    QString itemName = getAvailableListText(editMode, index);

    // In the debugger, when I do if condition 1, after the takeItem, it ALWAYS jumps down to
    // takeitem under editMode 3. So Im adding some things to try and figure out some stuff.

    if( editMode == EditCountries )
    {
        ui->lstAvailCountries->takeItem( index );
    }
    else if( editMode == EditProductionSteps )
    {
        ui->lstAvailProdSteps->takeItem( index );
    }
    else if( editMode == EditPretext )
    {
        ui->lstAvailPretext->takeItem( index );
        ui->lstAvailPretext->count();
    }

    return itemName;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperCoolMain::removeItemFromMainFormSelectedList( int editMode, int index )
{

    QString itemName = getMainFormSelectedListText(editMode, index);

    if( editMode == EditCountries )
    {
        ui->lstCatCountryList->takeItem( index );
    }
    else if( editMode == EditProductionSteps )
    {
        ui->lstCatStepsList->takeItem( index );
    }
    else if( editMode == EditPretext )
    {
        ui->lstCatPretextList->takeItem( index );
    }

    return itemName;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperCoolMain::removeRecIdFromAvailableList( int editMode, int index)
{
    int recordId = 0;

    if( editMode == EditCountries )
    {
        recordId = availCountryIds.at(index).toInt();
        availCountryIds.removeAt(index);
    }
    else if( editMode == EditProductionSteps )
    {
        recordId = availProdstepIds.at(index).toInt();
        availProdstepIds.removeAt(index);
    }
    else if( editMode == EditPretext )
    {
        recordId = availPretextIds.at(index).toInt();
        availPretextIds.removeAt(index);
    }

    return recordId;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperCoolMain::removeRecIdFromSelectedList( int editMode, int index)
{
    int recordId = 0;

    if( editMode == EditCountries )
    {
        recordId = selCountryIds.at(index).toInt();
        selCountryIds.removeAt(index);
    }
    else if( editMode == EditProductionSteps )
    {
        recordId = selProdstepIds.at(index).toInt();
        selProdstepIds.removeAt(index);
    }
    else if( editMode == EditPretext )
    {
        recordId = selPretextIds.at(index).toInt();
        selPretextIds.removeAt(index);
    }

    return recordId;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperCoolMain::removeRecIdFromMainFormSelectedList( int editMode, int index)
{
    int recordId = 0;

    if( editMode == EditCountries )
    {
        recordId = selCountryIds.at(index).toInt();
        selCountryIds.removeAt(index);
    }
    else if( editMode == EditProductionSteps )
    {
        recordId = selProdstepIds.at(index).toInt();
        selProdstepIds.removeAt(index);
    }
    else if( editMode == EditPretext )
    {
        recordId = selPretextIds.at(index).toInt();
        selPretextIds.removeAt(index);
    }

    return recordId;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::resetAvailableListToNone( int editMode )
{
    if( editMode == EditCountries )
        ui->lstAvailCountries->addItem(tr("None"));                 // if available list goes empty, fill with None
    else if( editMode == EditProductionSteps )
        ui->lstAvailProdSteps->addItem(tr("None"));
    else if( editMode == EditPretext )
        ui->lstAvailPretext->addItem(tr("None"));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::resetSelectedListToNone( int editMode )
{
    if( editMode == EditCountries )
        ui->lstDialogSelCountries->addItem(tr("None"));                 // if available list goes empty, fill with None
    else if( editMode == EditProductionSteps )
        ui->lstDialogSelProdSteps->addItem(tr("None"));
    else if( editMode == EditPretext )
        ui->lstDialogSelPretext->addItem(tr("None"));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::getLinksToSelectedCategory( int index )
{
    // select a category item in category combo box

    if(ui->cboCategoryList->count() > 1   )
    {
        // completely incorrect code - this should be pulled from categoryRecordIds
        QString catNum = categoryRecordIds.at(index);
        currentCategoryNumber = catNum.toInt();

        if(currentCategoryNumber > 0)
            session_->send(COOLDev, COOLDevReadByCategoryOnlyCountries, catNum); // 135,33,Cat# -> Read Current COOL Category info per country
        else
            session_->send(COOLDev, COOLDevReadByCategoryOnlyCountries);         // 135,33,Cat# -> Read Current COOL Category info per country
     }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::doneEditing()
{
    // do something if a user clicks the DONE button on the dialog
    doneEditingTheList();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperCoolMain::onSessionMessage( const SessionMessage& msg )
{
    if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteAllCountries == msg[1].toInt() ))
    {
        // Received 135,23 - Update COOL countries, sent 135,21 to get this back
        buildAllCountriesList(msg);
    }
    else if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteAllProductionSteps == msg[1].toInt() ))
    {
        // Received 135,43 - Update COOL Production Steps, sent 135,41 to get this back
        buildAllProductionStepsList(msg);
    }
    else if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteAllPretexts == msg[1].toInt() ))
    {
        // Received 135,63 - Update COOL Pretext, sent 135,61 to get this back
        buildAllPretextsList(msg);
    }
    else if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteAllCategories == msg[1].toInt() ))
    {
        // Received 135,83 - Update COOL Categories, sent 135,61 to get this back
        buildAllCategoriesList(msg);
    }
    else if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteCountriesByCategory == msg[1].toInt() ))
    {
        // Received 135,34 - Update COOL Categories, sent 135,33 to get this back
        buildCountriesListByCategory(msg);
    }
    else if (( COOLDev == msg[0].toInt() ) && ( COOLDevWriteProductionStepsByCategory == msg[1].toInt() ))
    {
        // Received 135,54 - Update COOL Categories, sent 135,53 to get this back
        buildProductionStepsListByCategory(msg);
    }
    else if (( COOLDev == msg[0].toInt() ) && ( COOLDevWritePretextsByCategory == msg[1].toInt() ))
    {
        // Received 135,74 - Update COOL Categories, sent 135,73 to get this back
        buildPretextsListByCategory(msg);
    }
}
