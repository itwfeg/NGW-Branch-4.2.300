/**
 * @file superTotalTypesSetup.h
 * Total Types Setup allows operators to set up different Total Types to be used in
 * Operate Mode. As PLUs are printed in Operate Mode, each item printed belongs to a Total
 * Type which tracks the items printed (sold). Typically Total Types are Production, which
 * is the hardcoded standard Total Type. Additional Total Types, which management may not
 * want to be counted as part of the production totals include rewraps, and discounted items.
 * SuperTotalTypesSetup replaces the file superTotalTypes.asp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @issues:
 * @{
*/

#include <logmanager.h>

#include <QtGui>
#include <QtCore>

#include "../sessionmainwindow.h"
#include "../dialogs/colordialog.h"
#include "../modes/supervisormode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "TokenParser.h"

#include "superTotalTypesSetup.h"
#include "ui_superTotalTypesSetup.h"

static const int MAX_TOTAL_TYPES = 8;
static const int NOT_CHECKED = 0;
static const int CHECKED = 2;

///////////////////////////////////////////////////////////////////////////////////////////////////
SuperTotalTypesSetup::SuperTotalTypesSetup(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::SuperTotalTypesSetup),
    totalTypeData_(NULL)
{
    // saveOnExit_(false),
    ui->setupUi(this);
    scaleScreen();

    // retrieve config object
    if ( mainWindow_->isSupervisorMode() )
        totalTypeData_ = &dynamic_cast<SupervisorMode*>( mode_ )->totalTypeData_;

    initialize();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SuperTotalTypesSetup::~SuperTotalTypesSetup()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::initialize()
{
    // initialize vars
    theSelectedIndex = 0;                                           // Production is the current index
    updatedField = 0;                                               // no field being edited right now
    hasChanged = false;
    newTotalType = false;

    canTotalTypesReceiveEvents = true;                              // indicates if a combo can receive events
    canReportsToPrepackReceiveEvents = true;                        // indicates if a combo can receive events
    canChangeUpcDigitReceiveEvents = true;                          // indicates if a combo can receive events
    canAllowBackDatingReceiveEvents = true;                         // checkbox can receive events
    canManualChangeReceiveEvents = true;                            // ditto
    canAutoChangeReceiveEvents = true;                              // ditto

    newPriTotalTypeText = "";
    newSecTotalTypeText = "";

    // hide or display some element we don't need at first.
    ui->cboReportsToPrepack->setVisible(false);
    ui->cboUpcChangeDigit->setVisible(false);
    ui->lblHowReports->setVisible(false);
    ui->lblUpcDigit->setVisible(false);
    ui->lblPriTotalTypeName->setVisible(false);
    ui->lblSecTotalTypeName->setVisible(false);

    // create all of the signals and slots
    // buttons
    connect(ui->butDeleteType,          SIGNAL(clicked()),                this, SLOT(deleteTotalType()));             // click delete transaction
    connect(ui->butEditName,            SIGNAL(clicked()),                this, SLOT(editTotalTypeName()));           // click edit transaction
    connect(ui->butNewType,             SIGNAL(clicked()),                this, SLOT(getNewTotalType()));             // click new transaction

    // checkboxes
    connect(ui->cbAllowBackDating,      SIGNAL(stateChanged(int)),        this, SLOT(checkedAllowBackdating(int)));   // select transaction type
    connect(ui->cbManualChangeUPCDigit, SIGNAL(stateChanged(int)),        this, SLOT(checkedAllowManualChange(int))); // select transaction type
    connect(ui->cbAutoChangeUPCDigit,   SIGNAL(stateChanged(int)),        this, SLOT(checkedAutoChanged(int)));       // select transaction type
    connect(ui->cbChangeBgColor,        SIGNAL(stateChanged(int)),        this, SLOT(checkedChangeBgColor(int)));     // select transaction type

    // combos
    connect(ui->cboTotalType,           SIGNAL(currentIndexChanged(int)), this, SLOT(getTotalTypeData(int)));         // select transaction type
    connect(ui->cboUpcChangeDigit,      SIGNAL(currentIndexChanged(int)), this, SLOT(updateUpcChangeDigit(int)));     // select upc digit
    connect(ui->cboReportsToPrepack,    SIGNAL(currentIndexChanged(int)), this, SLOT(setTotalTypeImpact(int)));       // select UPC reporting value
    connect(ui->cboBackgroundColor,     SIGNAL(currentIndexChanged(int)), this, SLOT(setBackgroundColor(int)));       // select background color

    // text fields
    connect(ui->txtPriTotalTypeName,    SIGNAL(textEdited(QString)), this, SLOT(updatePrimaryTypeName(QString)));     // set flags indicating a field is being edited
    connect(ui->txtSecTotalTypeName,    SIGNAL(textEdited(QString)), this, SLOT(updateSecondaryTypeName(QString)));   // set flags indicating a field is being edited
    connect(ui->txtTextForLabel,        SIGNAL(textEdited(QString)), this, SLOT(updateTextForLabel(QString)));   // set flags indicating a field is being edited

    connect(ui->txtPriTotalTypeName,    SIGNAL(editingFinished()), this, SLOT(updateReturnKey()));                    // fires on return key
    connect(ui->txtSecTotalTypeName,    SIGNAL(editingFinished()), this, SLOT(updateReturnKey()));                    // fires on return key
    connect(ui->txtTextForLabel,        SIGNAL(editingFinished()), this, SLOT(updateReturnKey()));

    // REFERENCE DEFINITION - Notes for anyone who reads this.
    // textChanged - This signal is emitted whenever the text changes. The text argument is the new text.
    // Unlike textEdited(), this signal is also emitted when the text is changed programmatically,
    // for example, by calling setText().

    // textEdited - This signal is emitted whenever the text is edited. The text argument is the next text.
    // Unlike textChanged(), this signal is not emitted when the text is changed programmatically,
    // for example, by calling setText().

    canChangeUpcDigitReceiveEvents = false;							// turn off because it fires on these
    QStringList digits;
    digits << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9";
    ui->cboUpcChangeDigit->clear();
    ui->cboUpcChangeDigit->addItems(digits);
    canChangeUpcDigitReceiveEvents = true;							// okay turn back on

    canReportsToPrepackReceiveEvents = false;						// could of just turned SIGNALS off instead, duh to myself.
    ui->cboReportsToPrepack->addItem(tr("Adds to Production"), 1);
    ui->cboReportsToPrepack->addItem(tr("Removes from Production"), -1);
    ui->cboReportsToPrepack->addItem(tr("No impact"), 0);
    canReportsToPrepackReceiveEvents = true;

    enableTextFields(0);

    // Populate Colors
    ui->cboBackgroundColor->addItem( "", QVariant( "000099" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "000000" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "000080" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "0000FF" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "00FFFF" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "800080" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "FF00FF" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "800000" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "FF0000" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "808000" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "FFFF00" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "008000" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "00FF00" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "008080" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "808080" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "C0C0C0" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "FFFFFF" ) );
    ui->cboBackgroundColor->addItem( "", QVariant( "custom" ) );


    session_->send(TransactionTypeDev,TransactionReadNonVoidGroup);     // 152,20 - read all transactions
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::translate()
{
	ui->retranslateUi( this );

    ui->cboReportsToPrepack->setItemText(0, tr("Adds to Production"));
    ui->cboReportsToPrepack->setItemText(1, tr("Removes from Production"));
    ui->cboReportsToPrepack->setItemText(2, tr("No impact"));

    setComboBoxItemTextFromData( ui->cboBackgroundColor, "000099", tr( "Hobart Blue" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "000000", tr( "Black" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "000080", tr( "Navy" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "0000FF", tr( "Blue" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "00FFFF", tr( "Aqua" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "800080", tr( "Purple" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "FF00FF", tr( "Fuchsia" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "800000", tr( "Maroon" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "FF0000", tr( "Red" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "808000", tr( "Olive" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "FFFF00", tr( "Yellow" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "008000", tr( "Lime" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "00FF00", tr( "Green" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "008080", tr( "Teal" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "808080", tr( "Gray" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "C0C0C0", tr( "Silver" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "FFFFFF", tr( "White" ) );
    setComboBoxItemTextFromData( ui->cboBackgroundColor, "custom", tr( "Custom..." ) );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperTotalTypesSetup::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperTotalTypesSetup::onScreenChanging()
{
    // save every time b\c we dont know what will happen when
    saveTotalType();

    // request current scale mode to update operate mode
    session_->send( SessionDev, SnReadWeighModeTransactionType );

    // base method
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
//                                    PRIVATE METHODS                                               //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperTotalTypesSetup::getPrepackReportingValueFromIndex(int currentIndex)
{
    if ( currentIndex == 0 )
        return "1";
    else if ( currentIndex == 1 )
        return "-1";
    else if ( currentIndex == 2 )
        return "0";

    return "0";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperTotalTypesSetup::getIndexFromPrepackReportingValue(int reportingVal)
{
    if ( reportingVal == 1 || reportingVal == -2)
        return 0;
    else if ( reportingVal == -1 )
        return 1;
    else if ( reportingVal == 0 )
        return 2;

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::updateHasChanged(QWidget *widget)
{
    // Im kind of faking this right now since it really needs to be called when the keyboard is dismissed
    // and that functionality is not yet in place. So I am faking it with this until it is added
    // to the keyboard.

    QLineEdit *lineEdit = dynamic_cast<QLineEdit*>( widget );

    QString nameId = widget->objectName();
    QString name = lineEdit->text();

    if(nameId == "txtPriTotalTypeName")
    {
        if(name.length() == 0)
        {
            name = totalTypeData_->primaryName();                   //	Restore the previous name entry as a valid name was not entered.
            ui->txtPriTotalTypeName->setText( name );
            ui->lblPriTotalTypeName->setText( name );
        }

        if(name != totalTypeData_->primaryName())
        {
            totalTypeData_->primaryName() = name;
            ui->lblPriTotalTypeName->setText( name );
            hasChanged = true;
        }

        priNameArray[theSelectedIndex] = name;
        ui->txtSecTotalTypeName->setFocus();
        ui->txtSecTotalTypeName->setCursorPosition( ui->txtSecTotalTypeName->text().length() );

    }
    else if(nameId == "txtSecTotalTypeName")
    {
        if(name.length() == 0)
        {
            name = totalTypeData_->secondName();                    //	Restore the previous name entry as a valid name was not entered.
            ui->txtSecTotalTypeName->setText( name );
            ui->lblPriTotalTypeName->setText( name );
        }
        if(name != totalTypeData_->secondName() )
        {
            totalTypeData_->secondName() = name;
            ui->lblSecTotalTypeName->setText( name );
            hasChanged = true;
        }

        secNameArray[theSelectedIndex] = name;
        enableTextFields(0);
    }
    else if( nameId == "txtTextForLabel" )
    {

    }

    if (newTotalType == true)
        newTotalType = false;

    ui->cboTotalType->setEnabled(true);                             //	Enable the name combo.

    if (nameId == "txtPriTotalTypeName" && name != "")
        ui->cboTotalType->setItemText(theSelectedIndex, name);

    saveTotalType();                                                // save every time b\c we dont know what will happen when
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::enableTextFields(bool doEnable)
{
    // Called to display the editable text fields for primary and secondary name of the non-editable label instead

    if(doEnable == 1)
    {
        ui->lblPriTotalTypeName->setVisible(false);
        ui->lblSecTotalTypeName->setVisible(false);
        ui->txtPriTotalTypeName->setVisible(true);
        ui->txtSecTotalTypeName->setVisible(true);
    }
    else
    {
        ui->txtPriTotalTypeName->setVisible(false);
        ui->txtSecTotalTypeName->setVisible(false);
        ui->lblPriTotalTypeName->setVisible(true);
        ui->lblSecTotalTypeName->setVisible(true);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::updateTotalTypesNameList(const SessionMessage &msg)
{
    DomTotalTypesList m( msg );

    int numOfItems = m.numOfRecordIds();                // how many names

    recordIdsArray = m.allRecordIds().split(",");       // Parse the record id list
    priNameArray = m.namesList().split(",");            // Parse the names of the primary totals names and put this into a list
    secNameArray = m.secNamesList().split(",");         // Parse the names of the second  totals names and put this into a list


    // fill the combo box with just the primary names, and we have a list of the secondary names.
    // This method follows the original implementation - BUT, it could be made better if secondLanguage was considered
    if (numOfItems > 0)
    {
        canTotalTypesReceiveEvents = false;                         // Any time we add items we need to deactivate the event SIGNALS

        for(int j = 0; j < numOfItems; ++j)
        {
            ui->cboTotalType->addItem(priNameArray.at(j), recordIdsArray.at(j));
        }

        canTotalTypesReceiveEvents = true;

        if (numOfItems == MAX_TOTAL_TYPES)
            ui->butNewType->setDisabled(true);                      // Can't add more total types if we are at MAX
    }

    session_->send(TransactionTypeDev,TransactionTypeDtRead,"1");   //	152,0,1 - Read the first Total type, TotalTypeDev, DtRead, Total type #1
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::displayTotalTypeData(const SessionMessage& msg)
{
    LOG_CLASS_INFO( "SuperTotalTypesSetup", "displayTotalTypeData" );

    // Upon loading the page, request a "152,0,1" - read the first Total Type, this is the reply to read it
    // Also gets called when a user clicks on "New Total Type" button, when the reply comes back

    DomTotalTypeData m( msg );

    // save the TotalTypeData class to reuse it later
    if ( totalTypeData_ )
        *totalTypeData_ = m;

    canAllowBackDatingReceiveEvents = false;                        // dont want the checkboxes firing events while programatically being selected
    canManualChangeReceiveEvents = false;                           // see above line
    canAutoChangeReceiveEvents = false;                             // see 2 lines ago

    int isChecked = false;
    int currTotalTypeNum = m.number().toInt();                      // The totaltype ID number

    if (newTotalType)												// if we are adding a new total type, do this
    {
        LOG_DEBUG( "adding blank total type...", NULL );
        addBlankTotalTypeName();									// add a new Option to the Combo Box if this is a new item
    }
    else															// else we are reading an existing total type information
    {
        int len = ui->cboTotalType->count();                        // how many items are in the Combo Box?
        int totalVal = -1;

        //	PTR 788 (#3).	If you deleted a TT, the modified list always showed "Production", which did not
        //					always match the Primary name of the now current TT.
        int index;
        for (index = 0; index < len; index++)
        {
            totalVal = ui->cboTotalType->itemData(index).toInt();
            if (totalVal == currTotalTypeNum)
                break;
        }

        if (index < len)
        {
            LOG_DEBUG( "setting index to %d", index );

            theSelectedIndex = index;
            canTotalTypesReceiveEvents = false;
            ui->cboTotalType->setCurrentIndex(index);
            canTotalTypesReceiveEvents = true;
        }
    }

    ui->txtPriTotalTypeName->setText( m.primaryName() );            // add the name to the input field
    ui->lblPriTotalTypeName->setText( m.primaryName() );

    ui->txtSecTotalTypeName->setText( m.secondName() );             // add the name to the input field
    ui->lblSecTotalTypeName->setText( m.secondName() );

    ui->txtTextForLabel->setText( m.textForLabel() );                    // add the ttext if there is some

    if (currTotalTypeNum == 1)										// If this is the MAIN #1 Production Total Type
    {
        enableTextFields(0);

        if ( ui->cboReportsToPrepack->isVisible() )
        {
            ui->cboReportsToPrepack->setVisible(false);
            ui->lblHowReports->setVisible(false);
        }

        // cant allow editing the "Production" Total Type,
        ui->butEditName->setDisabled(true);                         // Can't edit the name for TotalType 1 (Prepack)
        ui->butDeleteType->setDisabled(true);                       // Can't delete TotalType 1 (Prepack)
    }
    else
    {
        if ( ui->cboReportsToPrepack->isVisible() == false )
        {
            ui->cboReportsToPrepack->setVisible(true);
            ui->lblHowReports->setVisible(true);
        }

        ui->butEditName->setDisabled(false);                        // Allow editing
        ui->butDeleteType->setDisabled(false);

        int reportIndex = getIndexFromPrepackReportingValue(m.reportsToPrepack().toInt() );
        ui->cboReportsToPrepack->setCurrentIndex(reportIndex);      // set the prepak reporting value combo box index
    }


    if (newTotalType == true)										// if we are adding a new total type, do this
    {
        priNameArray.append( m.primaryName() );                     // push the name onto the end of the array
        secNameArray.append( m.secondName() );                      // push the name onto the end of the array
        enableTextFields(1);
    }

    isChecked = m.manualChangeUPCDigit().toInt();
    if(isChecked == 0)
        ui->cbManualChangeUPCDigit->setChecked(false);
    else
        ui->cbManualChangeUPCDigit->setChecked(true);

    isChecked = m.autoChangeUPCDigit().toInt();
    if(isChecked == 0)
    {
        ui->cbAutoChangeUPCDigit->setChecked(false);
        ui->cboUpcChangeDigit->setDisabled(true);
        ui->cboUpcChangeDigit->setVisible(false);
    }
    else
    {
        int digit = m.upcChangeDigit().toInt();

        if ( (digit < 0) || (digit > 9) )
        {
            digit = 0;
            hasChanged = true;
        }

        ui->cboUpcChangeDigit->setCurrentIndex(digit);
        ui->cbAutoChangeUPCDigit->setChecked(true);
        ui->cboUpcChangeDigit->setVisible(true);
        ui->cboUpcChangeDigit->setDisabled(false);
    }

    isChecked = m.allowBackDating().toInt();
    if(isChecked == 0)
        ui->cbAllowBackDating->setChecked(false);
    else
        ui->cbAllowBackDating->setChecked(true);

    isChecked = m.useBackgroundColor().toInt();
    if(isChecked == 0)
    {
        ui->cbChangeBgColor->setChecked(false);
        ui->lblBackgroundColor->setEnabled(false);
        ui->cboBackgroundColor->setEnabled(false);
    }
    else
    {
        ui->cbChangeBgColor->setChecked(true);
        ui->lblBackgroundColor->setEnabled(true);
        ui->cboBackgroundColor->setEnabled(true);
    }

    QString color( QString("%1").arg( m.backgroundColor().toInt(), 6, 16, QLatin1Char('0') ) );
    color = color.toUpper();

    if ( ui->cboBackgroundColor->findData( color ) < 0 )
    {
        ui->cboBackgroundColor->blockSignals( true );
        ui->cboBackgroundColor->addItem( color, QVariant( color ) );
        ui->cboBackgroundColor->blockSignals( false );
    }

    int index;

    if ( 0 <= (index = ui->cboBackgroundColor->findData( color )) )
    {
        ui->cboBackgroundColor->blockSignals( true );
        ui->cboBackgroundColor->setCurrentIndex( index );
        ui->cboBackgroundColor->blockSignals( false );
    }

    if (newTotalType == false)
        hasChanged = false;
    else
    {
        hasChanged = true;
        editTotalTypeName();
    }

    canAllowBackDatingReceiveEvents = true;                        // Everything is okay again
    canManualChangeReceiveEvents = true;                           // see above line
    canAutoChangeReceiveEvents = true;                             // see 2 lines ago
}

// Called when user clicks "New Total Type", and we want to add a new default named item to the end of the combo box
// Get a reply from Session indicating a new record has been created, and now add this item to the combo.
void SuperTotalTypesSetup::addBlankTotalTypeName()
{
    LOG_CLASS_INFO( "SuperTotalTypesSetup", "addBlankTotalTypeName" );

    int len = ui->cboTotalType->count();                            // how many?

    if (len < MAX_TOTAL_TYPES)
    {
        // create a new default name slot in the totals combo box for the new total type.  Add it to the end
        // ISSUE: This is implemented as previously implemented, but it does not take into consideration
        canTotalTypesReceiveEvents = false;

        ui->cboTotalType->addItem(newPriTotalTypeText, totalTypeData_->number() );
        theSelectedIndex = ( ui->cboTotalType->count() - 1 );
        ui->cboTotalType->setCurrentIndex(theSelectedIndex);        // update theSelectedIndex and select the new last item in the list

        LOG_DEBUG( "current index is %d", theSelectedIndex );

        canTotalTypesReceiveEvents = true;
    }

    if (ui->cboTotalType->count() == MAX_TOTAL_TYPES )
        ui->butNewType->setDisabled(true);                          // disable the combo if this is the max number of items.
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
//                                   SLOT METHODS                                                   //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::deleteTotalType()
{
    // Delete the currently selected item, send messages, and update markers

    int totalTypeNum = ui->cboTotalType->itemData(theSelectedIndex).toInt(); // get the ID of the currently selected Transaction Type

    if (totalTypeNum > 1)                                           // Never delete the first item, "Production.", ond Productions's id=1
    {
        canTotalTypesReceiveEvents = false;

        session_->send(TransactionTypeDev, TransactionTypeDtDelete, QString::number(totalTypeNum) ); // 152,4, Total type #
        ui->cboTotalType->removeItem(theSelectedIndex);  			// Remove from the Combo Box
        priNameArray.removeAt(theSelectedIndex);                    // remove the transaction types from the vectors
        secNameArray.removeAt(theSelectedIndex);

        theSelectedIndex--;                                         // decrement the index since we are moving up 1
        totalTypeNum = ui->cboTotalType->itemData(theSelectedIndex).toInt();          // get the new ID at the new index

        session_->send(TransactionTypeDev, TransactionTypeDtRead, QString::number(totalTypeNum) );  // 152,0 Total type #

        canTotalTypesReceiveEvents = true;

        if (ui->cboTotalType->count() < MAX_TOTAL_TYPES)
            ui->butNewType->setEnabled(true);                       // Can only have at most 8 trans types
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::editTotalTypeName()
{
    // displays the input text box, and makes it editable.
    // 1: Called when we click the "New Total Type" button, we ask to read a new record (a new one is created in the Session) and we
    //	get a reply with the new "blank" Total Type.  After we set up the new total type to be created in the Combo Box, we then
    //	need to set up the text fields and allow for editing the total type name information
    // 2: Also called directly when "Edit Name" is called.

    QString priName = "";
    QString secName = "";

    priName = priNameArray[theSelectedIndex];                       // Get the total type name from the pulldown.
    secName = secNameArray[theSelectedIndex];						// Get the total type name from the pulldown.

    ui->cboTotalType->setDisabled(true);                            // Disable the name combo.
    enableTextFields(1);											// enable the text field names for editing

    ui->txtPriTotalTypeName->setText( priName );
    ui->txtSecTotalTypeName->setText( secName );
    ui->lblPriTotalTypeName->setText( priName );
    ui->lblSecTotalTypeName->setText( secName );

    ui->txtPriTotalTypeName->setFocus();
    ui->txtPriTotalTypeName->setCursorPosition( ui->txtPriTotalTypeName->text().length() );

    //ui->txtPriTotalTypeName->selectAll();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::getNewTotalType()
{
    LOG_CLASS_INFO( "SuperTotalTypesSetup", "getNewTotalType" );

    LOG_DEBUG( "saving total type...", NULL );
    saveTotalType();

    LOG_DEBUG( "creating new total type...", NULL );
    newTotalType = true;
    session_->send(TransactionTypeDev,TransactionTypeDtCreateNew);  // 152,12 - create new
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::checkedAllowBackdating(int state)
{
    if( canAllowBackDatingReceiveEvents )
    {
        hasChanged = true;
        if(state == CHECKED)
            totalTypeData_->allowBackDating() = "1";
        else if(state == NOT_CHECKED)
            totalTypeData_->allowBackDating() = "0";
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::checkedAllowManualChange(int state)
{
    if( canManualChangeReceiveEvents )
    {
        hasChanged = true;
        if(state == CHECKED)
            totalTypeData_->manualChangeUPCDigit() = "1";
        else if(state == NOT_CHECKED)
            totalTypeData_->manualChangeUPCDigit() = "0";
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::checkedAutoChanged(int state)
{
    if( canAutoChangeReceiveEvents )
    {
        hasChanged = true;
        if(state == CHECKED)
            totalTypeData_->autoChangeUPCDigit() = "1";
        else if(state == NOT_CHECKED)
            totalTypeData_->autoChangeUPCDigit() = "0";

        if( ui->cboUpcChangeDigit->isVisible() )
        {
            ui->cboUpcChangeDigit->setDisabled(true);
            ui->cboUpcChangeDigit->setVisible(false);
            ui->lblUpcDigit->setVisible(false);
        }
        else
        {
            ui->cboUpcChangeDigit->setVisible(true);
            ui->lblUpcDigit->setVisible(true);
            ui->cboUpcChangeDigit->setDisabled(false);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::checkedChangeBgColor(int state)
{
    if( canAutoChangeReceiveEvents )
    {
        hasChanged = true;
        if(state == CHECKED)
            totalTypeData_->useBackgroundColor() = "1";
        else if(state == NOT_CHECKED)
            totalTypeData_->useBackgroundColor() = "0";

        ui->lblBackgroundColor->setEnabled( state == CHECKED );
        ui->cboBackgroundColor->setEnabled( state == CHECKED );
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::getTotalTypeData(int index)
{
    if(canTotalTypesReceiveEvents)
    {
        // Called when the user selects a Total Type in the Combo Box, READ it's data
        saveTotalType();                                                                //	save the current total type

        theSelectedIndex = index;
        QString totalTypeNum = ui->cboTotalType->itemData(theSelectedIndex).toString();   // TODO - test retrieving this variant
        session_->send(TransactionTypeDev,TransactionTypeDtRead, totalTypeNum);         //	TotalTypeDev, DtRead, Total type #
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::updateUpcChangeDigit(int index)
{
    if(canChangeUpcDigitReceiveEvents)
    {
        totalTypeData_->upcChangeDigit() = QString::number(index);
        hasChanged = true;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::setTotalTypeImpact(int index)
{
    if(canReportsToPrepackReceiveEvents)
    {
        totalTypeData_->reportsToPrepack() = getPrepackReportingValueFromIndex(index);
        hasChanged = true;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::setBackgroundColor(int index)
{
    Q_UNUSED( index );

    if ( !canReportsToPrepackReceiveEvents )
        return;

    // for custom color we need to prompt color picker
    if ( "custom" == ui->cboBackgroundColor->currentData().toString() )
    {
        QString oldColor( QString( "%1" ).arg( totalTypeData_->backgroundColor().toInt(), 6, 16, QLatin1Char('0') ) );
        oldColor = oldColor.toUpper();


        ColorDialog dialog( mainWindow_ );
        dialog.setCurrentColor( QColor( "#" + oldColor ) );

        if ( QDialog::Accepted == dialog.exec() )
        {
            QString newColor( dialog.selectedColor().name().right( 6 ) );
            newColor = newColor.toUpper();

            // add custom color to dialog
            ui->cboBackgroundColor->blockSignals( true );
            ui->cboBackgroundColor->addItem( newColor, QVariant( newColor ) );
            ui->cboBackgroundColor->setCurrentIndex( ui->cboBackgroundColor->count() - 1 );
            ui->cboBackgroundColor->blockSignals( false );
        }
        else
        {
            // revert
            ui->cboBackgroundColor->blockSignals( true );
            ui->cboBackgroundColor->setCurrentIndex( ui->cboBackgroundColor->findData( oldColor ) );
            ui->cboBackgroundColor->blockSignals( false );

            return;
        }
    }

    // save off selected color
    QString colorHex( ui->cboBackgroundColor->currentData().toString() );
    int color( colorHex.toInt( NULL, 16 ) );

    totalTypeData_->backgroundColor() = QString::number( color );
    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::updatePrimaryTypeName(QString transName)
{
    Q_UNUSED( transName );

    updatedField = 1;
    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::updateSecondaryTypeName(QString transName)
{
    Q_UNUSED( transName );

    updatedField = 2;
    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::updateTextForLabel(QString id)
{
    Q_UNUSED( id );

    updatedField = 3;
    hasChanged = true;
}

void SuperTotalTypesSetup::updateReturnKey()
{
    if(hasChanged)
    {
        if(updatedField == 1)
            updateHasChanged(ui->txtPriTotalTypeName);
        else if(updatedField == 2)
            updateHasChanged(ui->txtSecTotalTypeName);
        else if( updatedField == 3 )
            totalTypeData_->textForLabel() = ui->txtTextForLabel->text();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::saveTotalType()
{
    LOG_CLASS_INFO( "SuperTotalTypesSetup", "saveTotalType" );

    if ( !hasChanged )
        LOG_DEBUG( "not changed", NULL );
    else
    {
        QString msg = ui->cboTotalType->itemData(theSelectedIndex).toString();
        msg += ",";                                             // total type ID Number from above
        msg += "\"" + priNameArray.at(theSelectedIndex)  + "\""    + ",";	// primaryName
        msg += "\"" + secNameArray.at(theSelectedIndex)  + "\""    + ",";	// add the save of the secondName
        msg += ",";												// grossWeight does not change here
        msg += totalTypeData_->upcChangeDigit()       + ",";	// upcChangeDigit
        msg += totalTypeData_->reportsToPrepack()     + ",";	// reportsToPrepack
        msg += totalTypeData_->manualChangeUPCDigit() + ",";	// manualChangeUPCDigit
        msg += totalTypeData_->autoChangeUPCDigit()   + ",";	// autoChangeUPCDigit
        msg += totalTypeData_->allowBackDating()      + ",";    // allowBackDating
        msg += ",";                                             // flags does not change here
        msg += totalTypeData_->useBackgroundColor()   + ",";
        msg += totalTypeData_->backgroundColor()      + ",";
        msg += "\"" + totalTypeData_->textForLabel()  + "\"";   // this field prints on a label, ids a trans type

        LOG_DEBUG( "saving...", NULL );
        session_->send(TransactionTypeDev,TransactionTypeDtSave, msg);	//	112,6, Total type data info
        hasChanged = false;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "SuperTotalTypesSetup", "onSessionMessage" );

    if(( TransactionTypeDev == msg[0].toInt() ) && ( TransactionTypeWriteAllNames == msg[1].toInt() ))
    {
        LOG_DEBUG( "have all names message", NULL );

        // 152,21 - Transaction Type Names
        updateTotalTypesNameList(msg);
    }
    else if(( TransactionTypeDev == msg[0].toInt() ) && ( TransactionTypeDtWrite == msg[1].toInt() ))
    {
        LOG_DEBUG( "have write total type message", NULL );

        // 152,2 - Transaction Type Data for a single transaction type selected
        displayTotalTypeData(msg);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::setComboBoxItemTextFromData( QComboBox *w, const QString& data, const QString& text )
{
    int index;

    if ( 0 <= (index = w->findData( data )) )
        w->setItemText( index, text );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperTotalTypesSetup::setComboBoxItemTextFromData( QComboBox *w, int data, const QString& text )
{
    setComboBoxItemTextFromData( w, QString::number( data ), text );
}

