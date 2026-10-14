/**
 * @file SuperCoolMain.h
 * @brief Super Cool Main allows operators to assign COOL countries, COOL Production Steps, and
 * COOL pretext to Defined COOL Categories.  The countries, steps and pretext can also be
 * added to the ANY category. The asp page that represents this class is the vaguely named
 * and misunderstood web page named superCOOLLinkCategories.asp

 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperCoolMainCsu
 * @{
 */

#ifndef SUPERCOOLMAIN_H
#define SUPERCOOLMAIN_H

#include "screenwidget.h"

namespace Ui {
class SuperCoolMain;
}

/**
* @class SuperCoolMain
* @brief  
*  
*/

class SuperCoolMain  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperCoolMain _Myt;
    typedef ScreenWidget _Mybase;
    
public:
    /**
      * Constructor.
      */
    explicit SuperCoolMain(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperCoolMain();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve screen help text.
     *
     * When help is activated on a screen this property returns the screen title and help text.
     *
     * @param[out] title  screen title
     * @param[out] description  screen help description
     * @returns  @c true if help exists for the screen, @c false otherwise
     */
    virtual bool helpText( QString& title, QString& description ) const;

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

private slots:

    /**
    * SLOT method called when a new index is selected in the Categories Combo Box
    *
    * @param[in] index  index is the new selected index
    * @returns  none
    */
    void getLinksToSelectedCategory(int index);

    /**
    * SLOT method called when the Edit Countries is clicked
    *
    * @param[in] none
    * @returns  none
    */
    void editCountries();

    /**
    * SLOT method called when the Edit Steps is clicked
    *
    * @param[in] none
    * @returns  none
    */
    void editSteps();

    /**
    * SLOT method called when the Edit Pre-text is clicked
    *
    * @param[in] none
    * @returns  none
    */
    void editPretext();

    /**
    * SLOT method called when the New Categories is clicked
    *
    * @param[in] none
    * @returns  none
    */
    void newCategories();

    /**
    * SLOT method called when the New Countries is clicked
    *
    * @param[in] none
    * @returns  none
    */
    void newCountries();

    /**
    * SLOT method called when the New Production Steps is clicked
    *
    * @param[in] none
    * @returns  none
    */
    void newSteps();

    /**
    * SLOT method called when the New Pre-text is clicked
    *
    * @param[in] none
    * @returns  none
    */
    void newPretext();

    /**
    * SLOT method called when the the move to the left button is pressed
    * to move a selected item from the Selected List to the Avaialble List
    *
    * @param[in] none
    * @returns  none
    */
    void moveItemToTheLeftAvailableList();

    /**
    * SLOT method called when the move to the right button is pressed to
    * move a selected item from the Available List to the Selected List
    *
    * @param[in] none
    * @returns  none
    */
    void moveItemToTheRightSelectedList();

    /**
    * SLOT method called when the user selects the DONE button while
    * adding or removing items from the list
    *
    * @param[in] none
    * @returns  none
    */
    void doneEditing();

protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::SuperCoolMain *ui;
    SessionMainWindow *mainWindow_;

    QStringList categoryRecordIds;                                  // keep track of the record IDs to map to a category
    ///// On the dialog
    QStringList availCountryIds;                                    // record IDs to map to an available DIALOG country
    QStringList availProdstepIds;                                   // record IDs to map to an available DIALOG production step
    QStringList availPretextIds;                                    // record IDs to map to an available DIALOG pretext

    ///// On main form - same as the ones on dialog
    QStringList selCountryIds;                                      // record IDs to map to a selected MAIN FORM country
    QStringList selProdstepIds;                                     // record IDs to map to a selected MAIN FORM production step
    QStringList selPretextIds;                                      // record IDs to map to a selected MAIN FORM pretext

    int currentCategoryNumber;                                      // the current category number
    int numOfItems;                                                 // how many are there to process
    int editingMode;                                                // what are we editing, 1=countries, 2=prod steps, 3=pretext
    bool hasChanged;                                                // means we moved something in the list. Must also save
    bool suspendCoolMsgs;                                           // when user clicks move button, disable activities

    enum editing_style
    {
        EditNone,
        EditCountries,
        EditProductionSteps,
        EditPretext
    };

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method used to build and update all of the country information LISTS for COOL Assignment
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void buildAllCountriesList(const SessionMessage &msg);

    /**
     * Method used to build and update all of the Production Steps information LISTS for COOL Assignment
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void buildAllProductionStepsList(const SessionMessage &msg);

    /**
     * Method used to build and update all of the Pretext information LISTS for COOL Assignment
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void buildAllPretextsList(const SessionMessage &msg);

    /**
     * Method used to build and update all of the Category information LISTS for COOL Assignment
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void buildAllCategoriesList(const SessionMessage &msg);

    /**
     * Method used to build and update all of the Countries information LISTS Per Category for COOL Assignment
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void buildCountriesListByCategory(const SessionMessage &msg);

    /**
     * Method used to build and update all of the Countries information LISTS Per Production Steps for COOL Assignment
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void buildProductionStepsListByCategory(const SessionMessage &msg);

    /**
     * Method used to build and update all of the Countries information LISTS Per Pretext for COOL Assignment
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void buildPretextsListByCategory(const SessionMessage &msg);

    /**
     * Method used to update the scale application with a message when the user moves an item from the
     * available list to the selected list.
     *
     * @param[in] theValue  theValue the numerical identifier that identifies a country, pretext, or production step
     * @param[in] theText  theText is the actual text of the item being moved to the Selected list
     * @returns  none
     */
    void recordSelection(int editMode, const QString& theValue, const QString& theText);

    /**
     * Method used to update the scale application with a message when the user moves an item from the
     * selected list to the available list.
     *
     * @param[in] theValue  theValue is the data received from the Scale Application.
     * @param[in] theText  theText is the actual text of item being removed from the Selected list
     * @returns  none
     */
    void recordDeletion(int editMode, const QString& theValue, const QString& theText);

    /**
     * Method used to build and reset the Available Countries, Steps, and Pretext lists in the dialog
     *
     * @param[in] none
     * @returns[out]  none
     */
    void resetSelectAvailLists();

    /**
     * Method used to build the available list content on the dialog for either the countries
     * or production steps, from a list of data
     *
     * @param[in] widget  widget is either the avail prodsteps or countries list
     * @param[in] names  names is the list of items being added to the available list
     * @param[in] numOfItems  numOfItems is the number of items in the list
     * @param[in] doRemoveNoneOption  doRemoveNoneOption indicates if we remove the NONE option
     * @returns  none
     */
    void buildCOOLLists(QWidget *widget, QString names, int numOfItems);

    /**
     * Method used to build the available list content on the dialog for either the countries
     * or production steps, from a list of data. The same as buildCoolLists but with different data.
     *
     * @param[in] widget  widget is either the avail prodsteps or countries list
     * @param[in] names  names is the list of items being added to the available list
     * @param[in] numOfItems  numOfItems is the number of items in the list
     * @param[in] doRemoveNoneOption  doRemoveNoneOption indicates if we remove the NONE option
     * @returns  none
     */
    void buildCOOLListFromList(QWidget *availWidget, QWidget *selWidget, QStringList &availArray, QStringList &selArray, const QString& names, int numOfItems);

    /**
     * Method used to build the available list and selected list content on the dialog when a user
     * selects one of the edit buttons on the COOL page
     *
     * @param[in] availWidget  availWidget is either the avail prodsteps, countries, or pretext avaiable list
     * @param[in] selWidget  selWidget is the list of items in the selected list from countries, prodsteps, or pretext lists
     * @returns  none
     */
    void setupListsToEdit();

    /**
     * Method reset all of the lists once editing is done in the dialog and updates the selected list in the main form area
     *
     * @param[in] none
     * @returns  none
     */
    void doneEditingTheList();

    /**
     * Method sets the None option in the available list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @returns   none
     */
    void resetAvailableListToNone(int editMode);

    /**
     * Method sets the None option in the selected list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @returns   none
     */
    void resetSelectedListToNone(int editMode);

    /**
     * Method clears the available list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @returns   none
     */
    void clearAvailableList(int editMode);

    /**
     * Method clears the selected list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @returns   none
     */
    void clearSelectedList(int editMode);

    /**
     * Method gets the length of the available list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @returns   the length of the list
     */
    int getAvailableListLength(int editMode);

    /**
     * Method gets the length of the selected list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @returns   the length of the list
     */
    int getSelectedListLength(int editMode);

    /**
     * Method gets the length of the selected list on the MAIN FORM
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @returns   the length of the list
     */
    int getMainFormSelectedListLength(int editMode);

    /**
     * Method gets the currently selected row in the  available list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @returns   the current selected index
     */
    int getAvailableCurrentRowIndex(int editMode);

    /**
     * Method gets the currently selected row in the  selected list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @returns   the current selected index
     */
    int getSelectedCurrentRowIndex(int editMode);

    /**
     * Method gets the current text at the index specified in the available list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] index  index indicates the row for which we are inquiring
     * @returns   the text at the  selected index
     */
    QString getAvailableListText(int editMode, int index);

    /**
     * Method gets the current text at the index specified in the  selected list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] index  index indicates the row for which we are inquiring
     * @returns   the text at the  selected index
     */
    QString getSelectedListText(int editMode, int index);


    /**
     * Method gets the current text at the index specified in the available list in the MAIN FORM
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] index  index indicates the row for which we are inquiring
     * @returns   the text at the  selected index
     */
    QString getMainFormSelectedListText(int editMode, int index);

    /**
     * Method gets the record id at the index specified for the available records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] index  index indicates the row for which we are inquiring
     * @returns   the record id at the given index
     */
    int getRecIdFromAvailableList(int editMode, int index);

    /**
     * Method gets the record id at the index specified for the selected records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] index  index indicates the row for which we are inquiring
     * @returns   the record id at the given index
     */
    int getRecIdFromSelectedList(int editMode, int index);

    /**
     * Method sets the current row index specified for specified row in available records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] row  row indicates the row for which we are setting active
     * @returns none
     */
    void setAvailableCurrentRowIndex(int editMode, int row);

    /**
     * Method sets the current row index specified for specified row in selected records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] row  row indicates the row for which we are setting active
     * @returns none
     */
    void setSelectedCurrentRowIndex(int editMode, int row);

    /**
     * Method sets the current row index specified for specified row in selected records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] row  row indicates the row for which we are setting active
     * @returns none
     */
    void setMainFormSelectedCurrentRowIndex(int editMode, int row);

    /**
     * Method adds the current item to the end of the available records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] strToAdd  the text being added to the available list
     * @returns none
     */
    void addItemToAvailableList(int editMode, const QString& strToAdd);

    /**
     * Method adds the current item to the end of the selected records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] strToAdd  the text being added to the selected list
     * @returns none
     */
    void addItemToSelectedList(int editMode, const QString& strToAdd);

    /**
     * Method adds the current item to the end of the available records list in the MAIN FORM
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] strToAdd  the text being added to the available list
     * @returns none
     */
    void addItemToMainFormSelectedList(int editMode, const QString& strToAdd);

    /**
     * Method adds the given record id to the end of the available recordsID list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] recordId  the recordId being added to the available recordList
     * @returns none
     */
    void addRecordIdToAvailableList(int editMode, const QString& recordId);

    /**
     * Method adds the given record id to the end of the selected recordsID list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] recordId  the recordId being added to the selected recordList
     * @returns none
     */
    void addRecordIdToSelectedList(int editMode, const QString& recordId);

    /**
     * Method inserts the current item at the given index of the avvailable list list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] listStr  the text being inserted to the available list
     * @param[in] index     the location to where the new item will be inserted
     * @returns none
     */
    void insertItemIntoAvailableList(int editMode, const QString& listStr, int index);

    /**
     * Method inserts the current item at the given index of the selected list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] listStr  the text being inserted to the selected list
     * @param[in] index     the location to where the new item will be inserted
     * @returns none
     */
    void insertItemIntoSelectedList(int editMode, const QString& listStr, int index);

    /**
     * Method inserts the current record ID at the given index of the available records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] recordId  the id being inserted to the available list
     * @param[in] index     the location to where the new item will be inserted
     * @returns none
     */
    void insertRecordIdToAvailableList(int editMode, const QString& recordId, int index);

    /**
     * Method inserts the current record ID at the given index of the selected records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] recordId  the id being inserted to the selected list
     * @param[in] index     the location to where the new item will be inserted
     * @returns none
     */
    void insertRecordIdToSelectedList(int editMode, const QString& recordId, int index);

    /**
     * Method removes the text/item at a given index of the available records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] index     the location from where the text will be removed from the available list
     * @returns int     the text of the record removed
     */
    QString removeItemFromAvailableList(int editMode, int index);

    /**
     * Method removes the text/item at a given index of the selected records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] index     the location from where the text will be removed from the selected list
     * @returns QString     the text of the item removed
     */
    QString removeItemFromSelectedList(int editMode, int index);

    /**
     * Method removes the text/item at a given index of the selected records list in the MAIN FORM
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] index     the location from where the text will be removed from the selected list
     * @returns QString     the text of the item removed
     */
    QString removeItemFromMainFormSelectedList(int editMode, int index);

    /**
     * Method removes the record ID at a given index of the available records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] index     the location from where the record id will be removed from the available records list
     * @returns int     the id of the record removed
     */
    int removeRecIdFromAvailableList(int editMode, int index);

    /**
     * Method removes the record ID at a given index of the selected records list in the DIALOG
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] index     the location from where the record id will be removed from the available records list
     * @returns int     the id of the record removed
     */
    int removeRecIdFromSelectedList(int editMode, int index);

    /**
     * Method removes the record ID at a given index of the selected records list in the MAIN FORM
     *
     * @param[in] editMode  editMode indicates which list is being edited, 1=countries, 2=prodsteps, 3=pretext
     * @param[in] index     the location from where the record id will be removed from the available records list
     * @returns int     the id of the record removed
     */
    int removeRecIdFromMainFormSelectedList(int editMode, int index);


};

#endif // SUPERCOOLMAIN_H
