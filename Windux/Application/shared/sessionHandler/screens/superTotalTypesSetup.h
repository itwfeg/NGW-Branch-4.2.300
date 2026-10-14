/**
 * @file superTotalTypesSetup.h
 * @brief Total Types Setup allows operators to set up different Total Types to be used in
 * Operate Mode. As PLUs are printed in Operate Mode, each item printed belongs to a Total
 * Type which tracks the items printed (sold). Typically Total Types are Production, which
 * is the hardcoded standard Total Type. Additional Total Types, which management may not
 * want to be counted as part of the production totals include rewraps, and discounted items.
 * SuperTotalTypesSetup replaces the file superTotalTypes.asp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SuperTotalTypesSetupCsu
 * @{
*/

#ifndef SUPERTOTALTYPESSETUP_H
#define SUPERTOTALTYPESSETUP_H

#include "screenwidget.h"

class DomTotalTypeData;

namespace Ui {
class SuperTotalTypesSetup;
}

/**
* @class SuperTotalTypesSetup
* @brief Allows for the setup and configuration of all Total Types in the scale.
*/
class SuperTotalTypesSetup : public ScreenWidget
{
    Q_OBJECT
    
    typedef SuperTotalTypesSetup _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
      * Constructor.
      */
    explicit SuperTotalTypesSetup(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperTotalTypesSetup();

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
    * SLOT method named updatePrimaryTypeName to perform any updates when the text data changes
    *
    * @param[in] none
    * @returns  none
    */
    void updatePrimaryTypeName(QString transName);

    /**
    * SLOT method named updateSecodaryTypeName to perform any updates when the text data changes
    *
    * @param[in] none
    * @returns  none
    */
    void updateSecondaryTypeName(QString transName);

    /**
    * SLOT method named updateLabelText to perform any updates when the text data changes
    *
    * @param[in] none
    * @returns  none
    */
    void updateTextForLabel(QString transName);

    /**
    * SLOT method named updateReturnKey sets the returnKey flag to true
    *
    * @param[in] none
    * @returns  none
    */
    void updateReturnKey();


    /**
    * SLOT method named deleteTotalType when the delete button is selected the currrent transaction type is deleted
    *
    * @param[in] none
    * @returns  none
    */
    void deleteTotalType();

    /**
    * SLOT method named editTotalTypeName when the edit button is selected the currrent transaction type is editable
    *
    * @param[in] none
    * @returns  none
    */
    void editTotalTypeName();

    /**
    * SLOT method named getNewTotalType when the new button is selected a new transaction type can be created and modified
    *
    * @param[in] none
    * @returns  none
    */
    void getNewTotalType();

    /**
    * SLOT method named checkedAllBackdating which indicates back dating is ok for the current production type
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkedAllowBackdating(int state);

    /**
    * SLOT method named checkedAllowManualChange which indicates users can manually change the UPC digot
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkedAllowManualChange(int state);

    /**
    * SLOT method named checkedAutoChanged which indicates the UPC digit is automatically changed
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkedAutoChanged(int state);

    /**
    * SLOT method named checkedChangeBgColor which indicates whether to change background color
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkedChangeBgColor(int state);

    /**
    * SLOT method named getTotalTypeData which allows users to select different transaction types and change\see its properties
    *
    * @param[in] index, where index = the transaction type selected from the combo box
    * @returns  none
    */
    void getTotalTypeData(int index);

    /**
    * SLOT method named updateUpcChangeDigit which allows users to select different leading UPC digits
    *
    * @param[in] index, where index = the leading UPC digit selected from the combo box
    * @returns  none
    */
    void updateUpcChangeDigit(int index);

    /**
    * setTotalTypeImpact is a SLOT method which allows users to select different Prepack Reporting Values
    *
    * @param[in] index, where index = the reporting type selected from the combo box
    * @returns  none
    */
    void setTotalTypeImpact(int index);

    /**
    * setTotalTypeImpact is a SLOT method which allows users to select background color
    *
    * @param[in] index, where index = the reporting type selected from the combo box
    * @returns  none
    */
    void setBackgroundColor(int index);

protected:
    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

private:
    Ui::SuperTotalTypesSetup *ui;

    DomTotalTypeData *totalTypeData_;

    int theSelectedIndex;                                           // the current selectedIndex of the totalType
    int updatedField;                                               // field being edited
    bool hasChanged;                                                // data has changed, any data
    bool newTotalType;                                              // if NEW button has been pressed
    bool canTotalTypesReceiveEvents;                                // indicates if a combo can receive events
    bool canReportsToPrepackReceiveEvents;                          // indicates if a combo can receive events
    bool canChangeUpcDigitReceiveEvents;                            // indicates if a combo can receive events

    bool canAllowBackDatingReceiveEvents;                           // indicates if a checkbox can receive events
    bool canManualChangeReceiveEvents;                              // indicates if a checkbox can receive events
    bool canAutoChangeReceiveEvents;                                // indicates if a checkbox can receive events

    QStringList priNameArray;
    QStringList secNameArray;
    QStringList recordIdsArray;
    QString newPriTotalTypeText;
    QString newSecTotalTypeText;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method used to return the Reporting Value of the different Prepack Reporting Modes, where 1=Adds To Production
     * which is the 0th indexed item in the Reporting ComboBox, -1=Subtracts From Production and is the 1st indexed
     * item, and 0= means no impact which is the 2nd indexed item in the combo box. The selection indicates if a package
     * is added, subtacted, or has no impact on overall transactions.
     *
     * @param[in] currentIndex  which is the currentIndex of the combo box
     * @returns[out]  reportingVal which indicates the reporting value mapped to different selected indexes in the reporting
     * combo box, but which do not match up 1 to 1.
     */
    QString getPrepackReportingValueFromIndex(int currentIndex);

    /**
     * Method used to return the combo box index based upon the PrepackReportingValue which do not map one to one.
     * Index=0 when the reporting value is 1, index=1 when the reporting value is -1, index=2 when the reporting
     * value is 0.
     *
     * @param[in] currentVal  which is the current Prepack Reporting value
     * @returns[out]  the index based upon the current Prepack Reporting value.
     */
    int getIndexFromPrepackReportingValue(int reportingVal);

    /**
     * Method used to update fields after using the keyboard on a input field.
     *
     * @param[in] index of the field modified - page dependent
     * @returns[out]  none
     * @issues: The keyboard functionality is not yet set up to handle this functionality
     */
    void updateHasChanged(QWidget *widget);

    /**
     * Method used to display the editable text fields for primary and secondary name of the non-editable label instead
     *
     * @param[in] flag indicating if the fields displayed are disabled or editable
     * @returns[out]  none
     */
    void enableTextFields(bool doEnable);

    /**
     * Method used to update all of the primary and secondary total type names
     *
     * @param[in] msg  session message from the scale application
     * @returns[out] none
     */
    void updateTotalTypesNameList(const SessionMessage& msg);

    /**
     * Method used to update a single active total type selected on the screen
     *
     * @param[in] msg  session message from the scale application
     * @returns[out] none
     */
    void displayTotalTypeData(const SessionMessage& msg);

    /**
     * Method used when user clicks "New Total Type", and we want to add a new default named item to the end
     * of the total types name combo box. This is the reply from Session indicating a new record has been created,
     * and now add this item to the end of the total types combo box.
     *
     * @param[in] none
     * @returns[out] none
     */
    void addBlankTotalTypeName();

    /**
     * Method used to save any newly created Total Type or the editing of properties of existing ones.
     *
     * @param[in] none
     * @returns[out] none
     */
    void saveTotalType();

    void setComboBoxItemTextFromData( QComboBox *w, const QString& data, const QString& text );
    void setComboBoxItemTextFromData( QComboBox *w, int data, const QString& text );

};

#endif // SUPERTOTALTYPESSETUP_H
