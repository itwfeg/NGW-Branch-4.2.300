/**
 * @file superNormalScaleOperation.h
 * 
 * @class SuperNormalScaleOperation
 * @brief Supervisor mode - Configuration - Normal Scale Operations - Tabbed Interface
 *  
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Job E. Crew and Larry Pevoar
 * @addtogroup SuperNormalScaleOperationsCsu
 * @{
 */

#ifndef SUPERNORMALSCALEOPERATION_H
#define SUPERNORMALSCALEOPERATION_H

#include <Messages.h>
#include "screenwidget.h"
#include "../widgets/eanEditor.h"

class QIntValidator;
class timeEntryDialog;
class QDateTimeEdit;
class QLineEdit;
class QCheckBox;
class QSignalMapper;
class QRegExpValidator;

namespace Ui {
    class SuperNormalScaleOperation;
}

//namespace hobart {
//class stackedWebViewer;
//}

class DomNormalScaleOperation;
class QComboBox;

/**
 * @class SuperNormalScaleOperation
 * @brief Supervisor mode - Configuration - Normal Scale Operations - Tabbed Interface
 */
class SuperNormalScaleOperation  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperNormalScaleOperation _Myt;
    typedef ScreenWidget _Mybase;
    
public:

    /**
      * Constructor.
      */
    explicit SuperNormalScaleOperation(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperNormalScaleOperation();

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

	// Sample Slot Methods for various widgets

    /**
    * SLOT method called when any checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChanged(int state);

    /**
    * SLOT method called when any checkbox is selected or deselected that is in the displayOptions message.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedDisplayOptions(int state);

    /**
    * SLOT method called when new UI via Elevator checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedSelfServGenUIs( int state );

    // PJR383
    // void checkboxChangedEnablePlaylists(int state);
    // void checkboxChangedEnableByCounts(int state);
    // void checkboxChangedEnableBestSellers(int state);

    /**
    * SLOT method called when combobox index is changed in the displayOptions message.
    *
    * @param[in] index, pointing to the selected playlist file
    * @returns  none
    */
    void comboboxChangedDisplayOptions(int index);


    /**
    * SLOT method called when Allow Remote Access is selected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedRemoteAccess(int state);

    /**
    * SLOT method called when Remove Address Bar is selected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedRemoveAddressBar(int state);

    /**
    * SLOT method called when DB compress checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    // Removed compress time fields 05-15-2015 during timesys 1.15 release
    //void checkboxChangedCompressFlag(int state);

    /**
    * SLOT method called when Collect Transactions checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedCollectTrans(int state);

    /**
    * SLOT method called when Track Operator Changes checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedTrackOperators(int state);

    /**
    * SLOT method called when Track Supervisor Changes checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedTrackSuperOperators(int state);

    // SOF-43
    /**
    * SLOT method called when a new index is selected in the Days to Keep Records combo box.
    *
    * @param[in] newIndex, newIndex is the new currentIndex in the combo box
    * @returns  none
    */
    void comboboxDaysToKeepTrans( int newIndex );

    /**
    * SLOT method called when the Auto Lookup checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedAutoLookup(int state);

    /**
    * SLOT method called when the Auto Lookup checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void comboboxChangedNumOfDigits(int state);

    /**
    * SLOT method called when the Production Planning checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedProdPlan(int state);

    /**
    * SLOT method called when the user selects a checkbox and the data needs to be updated
    * immediately so the results can add or remove the Printer options in the menu.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedUpdatePrinter(int state);

    /**
    * SLOT method called when the user selects a checkbox and the data needs to be updated
    * immediately so the results can add or remove the Delete Files options in the menu.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedUpdateDelete(int state);

    /**
    * SLOT method called when the user selects a checkbox and the data needs to be updated
    * immediately so the results can add or remove the Search option from the menu.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedUpdateSearch(int state);

    /**
    * SLOT method called when the user selects a checkbox and the data needs to be updated
    * immediately so the results can add a search option to the keypad.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedUpdateSearchOnKeypad(int state);

    /**
    * SLOT method called when the user selects a checkbox and the data needs to be updated
    * immediately so the results can add or remove the Production Entry options in the menu.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedUpdateProdEntry(int state);

    /**
    * SLOT method called when the user selects a checkbox and the data needs to be updated
    * immediately so the results can add or remove the Accumulate Items options in the menu.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedUpdateAccumulate(int state);


    /**
    * SLOT method called when any Accumulate Items checkbox is selected or deselected.
    * The selection can also either add or remove the option from the menu
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedAccumulate(int state);

    // PJR 454 - Slot method called when enable barcode cloud feature checkbox is checked
    void checkboxChangedEnableCloudFeature(int state);

    /**
    * SLOT method called when the Item Strip Mode checkbox for Accumulate Items is selected or deselected.
    * The selection of the strip mode has an inpact on Totals Label Only in Accumulate Items.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedAccumulateStrip(int state);

    /**
    * SLOT method called when the Item Strip Mode checkbox for Accumulate Items is selected or deselected.
    * The selection of the strip mode has an inpact on Totals Label Only in Accumulate Items.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedDisableOpFlashkeyEditor(int state);

    /**
    * SLOT method called when the Item Strip Mode checkbox for Accumulate Items is selected or deselected.
    * The selection of the strip mode has an inpact on Totals Label Only in Accumulate Items.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedDisableLeftPanel(int state);

    /**
    * SLOT method called when the Item Strip Mode checkbox for Accumulate Items is selected or deselected.
    * The selection of the strip mode has an inpact on Totals Label Only in Accumulate Items.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedEnableTransType(int state);

    /**
    * SLOT method called when the we enable or disable the deli vision feature.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedEnableEnhancedCenterScreen(int state);

    /**
    * SLOT method called when user clicks the the network connectivity
    * series of checkboxes. The boxes are tied together in where once
    * one of the checkboxes are checked other actions can occur.
    *
    * @param[in] none
    * @returns  none
    */
    void updateNetworkCheckboxesChanged(QWidget *checkBox);

    /**
    * SLOT method called when the Clear PLU after Printing checkbox needs
    * updated. This checkbox can be affected by the start screen home screen.
    * If the home screen is the left pane, that automatically means the clearPLU
    * checkbox is checked and disabled. If the center screen is the home screen
    * after printing the clearPLU checkbox is enabled and the user can choose to clear.
    *
    * @param[in] startScreenIndex  startScreenIndex is the current value of the start screen index
    * @returns  none
    */
    void updateClearPluOnPrint(int startScreenIndex);

    /**
    * SLOT method called when any spin box is modified.
    *
    * @param[in] numValue, numValue is the new value in the spin box
    * @returns  none
    */
    void inputChanged(int numValue);

    /**
    * SLOT method called when the time is changed in a QTimeEdit spinbox.
    *
    * @param[in] numValue, numValue is the new value in the spin box
    * @returns  none
    */
    void timeboxChanged(QTime newTime);

    /**
    * SLOT method called when the text is changed in the maxTare input box.
    *
    * @param[in] none
    * @returns  none
    */
    void textboxChangedMaxTare();

    /**
    * SLOT method called when the text is changed in the tareSetback input box.
    *
    * @param[in] none
    * @returns  none
    */
    void textboxChangedTareSetback();

    /**

    * SLOT method called when the text is changed in the minSweetheartWeight input box.
    *
    * @param[in] none
    * @returns  none

    */
    void textboxChangedMinSweetheartWeight();

    /**
    * SLOT method called when the text is changed in the production planning fields
    *
    * @param[in] none
    * @returns  newText  newText is the new text in the FPP text field
    */
    void textboxChangedDisplayOptions(QString newText);

    /**
    * SLOT method called when the text is changed in the defaultHomePage input box
    *
    * @param[in] none
    * @returns  none
    */
    void textboxChangedDefaultHomePage();

    /**
    * SLOT method called when the text is changed in an accumulate items input box
    *
    * @param[in] none
    * @returns  none
    */
    void textboxChangedAccumulate();

    ///////////////////////////////////////////////////////////////////
    /**
    * SLOT method called when the text is changed in a QLineEdit.
    *
    * @param[in] none
    * @returns  none
    */
    void textboxChanged();

    /**
    * SLOT method called when a new index is selected in the combo box.
    *
    * @param[in] newIndex, newIndex is the new currentIndex in the combo box
    * @returns  none
    */
    void comboboxChanged(int newIndex);

    /**
    * SLOT method called when a new index is selected in the combo box.
    *
    * @param[in] newIndex, newIndex is the new currentIndex in the combo box
    * @returns  none
    */
    void comboboxLoginMethodChanged(int newIndex);

    /**
    * SLOT method called when a new index is selected in the date combo box.
    *
    * @param[in] newIndex, newIndex is the new currentIndex in the combo box
    * @returns  none
    */
    void comboboxDateChanged(int newIndex);

    /**
    * SLOT method called when a new index is selected in the time combo box.
    *
    * @param[in] newIndex, newIndex is the new currentIndex in the combo box
    * @returns  none
    */
    void comboboxTimeChanged(int newIndex);

    /**
    * SLOT method called when a new index is selected in the primary language combo box.
    *
    * @param[in] newIndex, newIndex is the new currentIndex in the combo box
    * @returns  none
    */
    void comboboxPriLangChanged(int newIndex);

    /**
    * SLOT method called when a new index is selected in the secondary language combo box.
    *
    * @param[in] newIndex, newIndex is the new currentIndex in the combo box
    * @returns  none
    */
    void comboboxSecLangChanged(int newIndex);

    /**
    * SLOT method called when the Access Browser checkbox is selected or deselected.
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
    void comboboxChangedAccessBrowser(int newIndex);

    /**
    * SLOT method called when the Right Panel Content combobox index has changed.
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
    void comboboxRightPanelMode(int newIndex);

    /**
    * SLOT method called when the Bottom Panel Content combo box index has changed.
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
    void comboboxBottomPanelMode(int newIndex);


    /**
    * SLOT method called when the Start Screen after printing is modified.
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
    void comboboxChangedOperateStartScreen(int newIndex);

    /**
    * SLOT method called when the user changes the Screen to Start on
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
    void comboboxChangedSelfServStartScreen(int newIndex);

    /**
    * SLOT method called when the user changes the way to look up PLUs.
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
    void comboboxChangedSelfServItemEntryScreen(int newIndex);

    /**
    * SLOT method called to decide to use Char by Char search or A-Z cateogry search
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
     void comboboxChangedSelfServSearchScreen(int newIndex);

     /**
     * SLOT method called to decide to using what to print combo
     *
     * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
     * @returns  none
     */
      void comboboxChangedSelfServPrintScreen(int newIndex);

      /**
      * SLOT method called to decide to using what to display when printing
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
      void comboboxChangedSelfServAfterPrintScreen(int newIndex);

    /**
    * SLOT method called when the user changes the Now Serving combo box.
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
    void comboboxChangedNowServing(int newIndex);

    /**
    * SLOT method called when the user changes the Cut Test Type Combo Box.
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
    void comboboxChangedCutTest(int newIndex);

	/**
    * SLOT method called when the user changes the Flashkey Type Combo Box.
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
    void comboboxChangedFlashkeyType(int newIndex);

    /**
    * SLOT method called when user clicks the Reset Flashkey Statistics.
    * Sends a message to the scale application to reset the statistics.
    *
    * @param[in] none
    * @returns  none
    */
    void resetStatistics();

    /**
    * SLOT method called when user clicks the EAN Format String.
    *
    * @param[in] none
    * @returns  none
    */
    void launchEanEditor();

    /**
    * SLOT method called when user clicks Screen Size. Used to determine the dimensions of the desktop
    *
    * @param[in] none
    * @returns  none
    */
    void checkScreenSize();

    /**
    * SLOT method called when user selects a new tab. If there is any anything
    * that needs to be done from initializing to saving a previous tab's contents,
    * when changing tabs it can be triggered within this method. Use currentTab
    * to check the previous tab number. The input parameter will update the
    * currentTab value to the current
    *
    * @param[in] tabNum  tabNum is the new tab selected.
    * @returns  none
    */
    void currentChangeTab(int tabNum);

    /**
    * SLOT method called when user selects a new tab in the sub tab under Features.
    * If there is any anything that needs to be done from initializing to saving a
    * previous tab's contents, when changing tabs it can be triggered within this
    * method. Use currentTab to check the previous tab number. The input parameter
    * will update the currentTab value to the current.
    *
    * @param[in] tabNum  tabNum is the new tab selected.
    * @returns  none
    */
    void currentChangeSubTab(int tabNum);

    /**
    * SLOT method called when the text is changed in the number of chars to display
    * as a description on a flashkey. If the number of chars > 0, then the display
    * entire description checkbox is unchecked. This field is part of the
    * operatorDisplayFields message which will have to be saved and updated if the
    * number of chars to display changes.
    *
    * @param[in] none
    * @returns  none
    */
    void numOfCharsOnFlashkeyTextChanged();

    /**
    * SLOT method called when  display entire description checkbox are selected.
    * These fields are part of the operatorDisplayFields message which will have
    * to be saved and updated if the checkboxes or number of chars to display changes.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void numOfCharsOnFlashkeyCheckboxChanged(int state);

    /**
    * SLOT method called when the text is changed in the default flashkey font pointsize.
    *
    * @param[in] none
    * @returns  none
    */
    void defaultFlashkeyFontPointsizeChanged();

    /**
    * SLOT method called when selecting appending PLU number to a flashkey. This field is not
    * part of normal scale operations but as part of Prepack config so we have to save separately.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void appendingFlashkeyCheckboxChanged(int state);

    /**
     * Method to handle flipping the AM and PM compress time strings.
     *
     * @param[in] none
     * @returns  none
     */
    // Removed compress time fields 05-15-2015 during timesys 1.15 release
    //void flipAmPm();

    /**
     * SLOT method called when the DONE button is clicked in the EAN editor.
     *
     * @param[in] none
     * @returns  none
     */
    void onEanEditorDoneClicked(QString eanString);

    /**
     * SLOT method called when the CANCEL button is clicked in the EAN editor.
     *
     * @param[in] none
     * @returns  none
     */
    void onEanEditorCancelClicked(QString eanString);

    // FT-790
    /**
    * SLOT method called when the Run Screen Content combobox on the Features tab is changed.
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
    void comboboxChangedRunScreenBrowser(int newIndex);

    /**
    * SLOT method called when the Default Search Method combobox on the Features->Flashkeys tab is changed.
    *
    * @param[in] newIndex,  newIndex is the new currentIndex of the combo box
    * @returns  none
    */
    void comboboxChangedSearchMethod(int newIndex);

    /**
    * SLOT method called when the user wants to move the barcode lable on the screen to the left.
    *
    * @param[in] none
    * @returns  none
    */
    void editLabelOffsetLeft();

    /**
    * SLOT method called when the user wants to move the barcode lable on the screen to the left.
    *
    * @param[in] none
    * @returns  none
    */
    void editLabelOffsetRight();

    /**
    * SLOT method called when the text is changed in the Enhanced Run Screen input box
    *
    * @param[in] none
    * @returns  none
    */
    void textboxChangedEnhancedCenterHomePage();


    // FT-790
    /**
    * SLOT method called when the text is changed in the Run Screen Home Page input box
    *
    * @param[in] none
    * @returns  none
    */
    void textboxChangedRunScreenHomePage();

    /**
    * SLOT method called when the text is changed in the Reset Statistics input box
    *
    * @param[in] none
    * @returns  none
    */
    void textboxChangedResetStatistics();

    /**
    * SLOT to restrict ping minutes if hours equals zero
    *
    * @param[in] QString
    * returns none
    * ensures ping interval greater than zero hours, zero minutes
    */
    void textPingHoursChanged(QString hours);

    void editOnOffTimes(QWidget *widget);

    // HT-365
    /**
    * SLOT method called when "Enable extra day for Shelf Life" checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedEnableExtraShelfLifeDay( int state );

    // pjr383
    //void checkboxChangedPrintToScreen( int state )
    //void checkboxChangedCombinedOptions( int state );

    // HT-365
    /**
    * SLOT method called when "Shelf Life AM/PM button is clicked to toggle text.
    *
      * @param[in] none
      * @returns  none
    */
    void toggleShelfLifeAmPm();

    // SOF-1001
    /// SLOT method to Calibrate Wrapper (All Mechanisms)
    void calibrateWrapper();
	
    /**
     * Slot to edit playlist file.
     *
     * @param[in] none
     * @returns  none
     */
    void editPlaylistFile();


    // SOF-3846
    /// SLOT method when the “Disable cutting between linerless label printing” checkbox is toggled
    void checkboxChangedDisableCuttingBetweenLabels( int state );

    void onPrimaryLabelChange( int index );
    // PJR 454 - performs a simple request on URL input from user to determine if it is online
    void testServerStatusClicked();

    void onTimeout();

protected:
    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

    /**
     * Event filter.
     *
     * @param[in,out] obj  object event filter was attached to
     * @param[in,out] event  event that is occuring
     * @returns  @c true if event @p event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );


private:

    Ui::SuperNormalScaleOperation *ui;
    hobart::eanEditor *eanConfigEditor_;

    DomNormalScaleOperation *normalScaleOperation_;
    MadpOperatorDisplayFields *operatorDisplayFields_;
    MadpAccumItemsConfig *accumItemsConfig_;
    MadpScaleConfig *scaleConfigData_;
    MadpCurrencySettings *currencyData_;
    MadpPeripheralList *periphSettings_;

    QSignalMapper *signalMapper_;                                   // used so I can map to some of my checkboxes

    QMap<QWidget*, int> offsetMap_;

    // Removed compress time fields 05-15-2015 during timesys 1.15 release
    //QIntValidator *compressHours_;
    //QIntValidator *compressMinutes_;
    QIntValidator *tareSetback_;
    QIntValidator *maxTare_;
    QIntValidator *logoutIdleRemoteTime_;
    QIntValidator *numOfCharsOnFlashkey_;
    QIntValidator *defaultFlashkeyFontPointsize_;
    QIntValidator *resetStatsDays_;
    QIntValidator *pingTarget1_;
    QIntValidator *pingTarget2_;
    QIntValidator *pingTarget3_;
    QIntValidator *pingTarget4_;
    QIntValidator *pingHours_;
    QIntValidator *pingMinutes_;
    QIntValidator *accumItemsScaleId_;
    QIntValidator *accumItemsTransNum_;
    QValidator    *accumItemsEanCode_;
    QIntValidator *accumItemsEanPrefix_;
    QIntValidator *periodicOfflineBeep_;
    QIntValidator *maxSuspendTests_;
    QIntValidator *maxSuspendTestsPerOper_;
    QIntValidator *inactivityLogout_;

    QIntValidator *shelfLifeHoursValidator_;
    QIntValidator *shelfLifeMinutesValidator_;

    QIntValidator *flashkeyNumValidator_;

    QIntValidator * transHoursToView_;

    QIntValidator *minSweetheartWeight_;

	timeEntryDialog *teDialog_;

    int weigherModel_;                                              // the actual weigher model.
    int currentTab;                                                 // used to track the current tab we are on
    int currentSubTab;                                              // Track the sub tab we are on
    int msgCounter;                                                 // have to receive 4 config messages before we are allowed to save data

    double theWeigherMaxWt;                                         // what is the max weight the weigher can display/read

    bool hasChanged;                        						// has any config param been changed?  so we have to save when we leave.
    bool hasChangedDisplayOptions;                                  // if display options has changed
    bool isMetric_;                                                 // is the scale metric?
    bool initNetworkCbos;											// FT-54 Network connectivity cannot be turned off,saved (was supposed to be a bool all along)                                           // This will be true on initialze, and then set to false once run once.

    bool tabAccumHasChanged;
    bool dateTimeHasChanged;                                        // the date time format is saved separate from normal scale operations
    bool langHasChanged;
    bool isUpdating;                                                // used to turn off signals
    bool neverCallThisCodeAgain;                                    // only do this once
    bool maxTareHasBeenUpdated;                                     // indicates we have received a 1,2 message
    bool dataHasBeenInitialized;                                    // data not saved if not received in the first place
    bool wrDisplayFieldsHaveChanged;                                // If checkboxes on the Wrapper tab have changed

    bool isLicensed_FPP_;
    bool isLicensed_FullscreenFlashkeys_;
    bool isLicensed_WebBrowser_;
    bool alreadyReceivedScaleOpWriteMessage;
    bool alreadyReceivedCfpPrepackMessage;
    bool isLicensed_MealBuilder_;
    bool isLicensed_Enhanced_CenterScreen_;

    QString origPrimaryPlacement;
    QString origSecondyPlacement;

    QLineEdit *sundayOnTime_;
    QLineEdit *mondayOnTime_;
    QLineEdit *tuesdayOnTime_;
    QLineEdit *wednesdayOnTime_;
    QLineEdit *thursdayOnTime_;
    QLineEdit *fridayOnTime_;
    QLineEdit *saturdayOnTime_;
    QLineEdit *sundayOffTime_;
    QLineEdit *mondayOffTime_;
    QLineEdit *tuesdayOffTime_;
    QLineEdit *wednesdayOffTime_;
    QLineEdit *thursdayOffTime_;
    QLineEdit *fridayOffTime_;
    QLineEdit *saturdayOffTime_;

    bool isShelfLifeAmSelected;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots.
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to save any and all data when leaving a screen.
     *
     * @param[in] none
     * @returns  none
     */
    void saveDataOnExit();

    /**
     * Method to set up all of the signals and slots for normal scale operations. There are
     * so many widgets with events and the initialize method was getting too large
     *
     * @param[in] none
     * @returns  none
     */
    void setupSlotsAndSignals();

    /**
     * Set up the Self Serve playlists
     *
     * @param[in] none
     * @returns  none
     */
    void setupSelfServePlaylists();

    /**
     * Method to set up any and all controls that need to be initialized with default data.
     * For the most part this pertains to adding items to combo boxes
     *
     * @param[in] none
     * @returns  none
     */
    void setupWidgets();

    /**
     * Method to set up any and all text input fields that need to have a validator
     * attached to them to mask input. Validators such as max digits, max value,
     * min value, precision and separator can be applied.
     *
     * @param[in] none
     * @returns  none
     */
    void setupValidators();

    /**
     * Method to set up the validator for the Number of Hours to View Records text box.
     * Its validator is dependent upon the value in the Days to Keep Records combo box.
     * Its max value allowed will be equal to the value in Days to Keep Records * 24
     *
     * @param[in] none
     * @returns  none
     */
    // SOF-43
    void setupHoursToViewRecordsValidator( int numDays );

     /**
     * Method to validate fields
     *
     * @param[in] digits  digits is current digits in the text input line.
     * @returns digits  where digits is the new number that has been rolled over due to being over length
     */
     int makeValidatorTop(int digits);

    /**
     * Method to enable or disable the compress time fields dependant upon if the
     * enable DB compress is enabled.
     *
     * @param[in] isEnabled  isEnabled enables the fields if true, disables otherwise
     * @returns  none
     */
    // Removed compress time fields 05-15-2015 during timesys 1.15 release
    //void changeCompressFlag(bool isEnabled);

    /**
     * Method to enable or disable the collect transaction time field if the
     * enable collect transactions, cool totals, and operator changes is selected.
     *
     * @param[in] isEnabled  isEnabled enables the fields if true, disables otherwise
     * @returns  none
     */
    void setCollectTransactionsDays(bool isEnabled);

    /**
     * Method to set the start screen for self serve.
     *
     * @param[in] state - screen to start on or return to when nothing is happening
     * @returns  none
     */
    void updateSelfServStartState(bool begin_on_start, bool weight_required, bool enable_playlist, int start_state);

    /**
     * Method to set the item entry screen for self serve.
     *
     * @param[in] state - what method do we find Products
     * @returns  none
     */
    void updateSelfServItemEntryState(bool enable_bestsellers, int start_state);

    /**
     * Method to set the search screen for self serve.
     *
     * @param[in] state - How do we search?
     * @returns  none
     */
    void updateSelfServSearchState(bool enable_chars, int start_state);

    /**
     * Method to set the print screen for self serve.
     *
     * @param[in] state - What do we print?
     * @returns  none
     */
    void updateSelfServPrintState(int ignore_option, bool barcode_option, int start_state);

    /**
     * Method to set the self serve fresh serve options
     *
     * @param[in] state - What do show?
     * @returns  none
     */
    void updateSelfServFreshServOptions(const SessionMessage &msg);

    /**
     * Method to set the when printing do this screen for self serve.
     *
     * @param[in] state - What do we do when printing?
     * @returns  none
     */
    void updateSelfServPrintingState(int start_state);

    /**
     * Method to save the start screen for self serve.
     *
     * @param[in] state - Save off the configs?
     * @returns  none
     */
    void saveSelfServeStartScreen( int state);

    /**
     * Method to save the plu entry screen for self serve.
     *
     * @param[in] state - Save off the configs?
     * @returns  none
     */
    void saveSelfServeItemEntryScreen( int state);

    /**
     * Method to save the search screen for self serve.
     *
     * @param[in] state - Save off the configs?
     * @returns  none
     */
    void saveSelfServeSearchScreen( int state);

    /**
     * Method to save the print screen for self serve.
     *
     * @param[in] state - Save off the configs?
     * @returns  none
     */
    void saveSelfServePrintScreen(int state);

    /**
     * Method to save when printing screen for self serve.
     *
     * @param[in] state - Save off the configs?
     * @returns  none
     */
    void saveSelfServeAfterPrintScreen(int state);

    /**
     * Method to save all of the options when Gen2 Fresh Serve is selected.
     *
     * @param[in] state - Save off the configs?
     * @returns  none
     */
    void saveSelfServeOptionsGen2Screen();

    /**
     * Method to enable or disable the collect transaction time field if the
     * enable collect transactions, cool totals, and operator changes is selected.
     *
     * @param[in] isEnabled  isEnabled enables the fields if true, disables otherwise
     * @returns  none
     */
    void enableDaysToKeepTransactions();

    /**
     * Method to enable or disable the number of digits field if the enable
     * auto-lookup is selected.
     *
     * @param[in] isEnabled  isEnabled enables the fields if true, disables otherwise
     * @returns  none
     */
    void changeAutoLookup(bool isEnabled);

    /**
     * Method to enable or disable the remote restrictions if the enable
     * remote access checkbox is selected.
     *
     * @param[in] isEnabled  isEnabled enables the fields if true, disables otherwise
     * @returns  none
     */
    void changeRemoteAccess(bool isEnabled);

    /**
     * Method to enable or disable tthe user name and password fields if user
     * name and password is selected in the login method combobox
     *
     * @param[in] loginMethod
     * @returns  none
     */
    void changeRemoteAccessLoginMethod(int loginMethod);

    /**
     * Method to enable or disable the Browser Panel in Operate Mode when enable browser
     * mode is selected in Normal Scale Operations. Will also disable or enable the
     * browser URL line given the selected operation
     *
     * @param[in] isEnabled  isEnabled enables the fields if true, disables otherwise
     * @returns  none
     */
    void enableBrowserInPanel(int isEnabled);

    /**
     * Method to add or remove the desktop access option to the menu. DEPRECATED?
     *
     * @param[in] isEnabled  isEnabled enables the fields if true, disables otherwise
     * @returns  none
     */
    void enableDesktopMenuItem(bool isEnabled);

    /**
     * Method to enable or disable the Fresh Production Planning text fields depending
     * on the value of the enable production planning checkbox.
     *
     * @param[in] isEnabled  isEnabled enables the fields if true, disables otherwise
     * @returns  none
     */
    void enableProdPlanSource(bool isEnabled);

    /**
     * Method to add or remove the Printer Settings option to the menu.
     *
     * @param[in] isEnabled  isEnabled enables the fields if true, disables otherwise
     * @returns  none
     */
    void enablePrinterSettingsMenuItem(bool isEnabled);

    /**
     * Method called when Weight Required To Select Item checkbox is selected or deselected.
     *
     * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
     * @returns  none
     */
    void checkboxChangedEnableWeightReq(int state);

    /**
     * Method to traverse a combo box and check the user data against the current dom value to set the selected index
     *
     * @param[in] widget  the QWidget Combo Box to be acted upon
     * @param[in] comboData  the userData we are searching for so we can set the selectedIndex
     * @returns  none
     */
    void setComboBoxIndex(QComboBox *widget, QString comboData);

    /**
     * Method to traverse Film Tension combo box and check the user data against the current dom value to set
     * in the selected index. Also handles if users have databases with a HIGH value we removed [SOF-1332]
     *
     * @param[in] widget  the QWidget Combo Box to be acted upon
     * @param[in] comboData  the userData we are searching for so we can set the selectedIndex
     * @returns  none
     */
    void setFilmTensionComboBoxIndex(QComboBox *widget, QString comboData);
    /**
     * Method to update the weigher model used to set certain configuration fields
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateWeigherModel(const SessionMessage &msg);

    /**
     * Method to enable or disable the IP address fields for network connectivity
     *
     * @param[in] none
     * @returns  none
     */
    void setPingTargetState();

    /**
     * Method to enable or disable ping hours and minutes fields for network connectivity
     *
     * @param[in] none
     * @returns  none
     */
    void setPingIntervalState();

    /**
     * Method to update the hours and minutes for for network connectivity interval
     *
     * @param[in] none
     * @returns  none
     */
    void updatePingIntervalInfo();

    /**
     * Method to enable, disable, and display the compress database hours and minutes fields
     * in addition to selecting/unselecting the compress database checkbox.
     *
     * @param[in] none
     * @returns  none
     */
    // Removed compress time fields 05-15-2015 during timesys 1.15 release
    //void updateCompressTimeInfo();

    /**
     * Method to save to the database when new primary and secondary languages are selected
     * in the combobox. This method will update the language code, country codes for both
     * the primary and secondary langauges. Depending on the languages selected the
     * Change Language option in the menu may be added or removed.
     *
     * @param[in] none
     * @returns  none
     */
    void updateLanguageValues(int languageCode, int newIndex);

    /**
     * Method to reset the flashkeys, if needed, when the secondary languages changes
     * or if set to none.
     *
     * @param[in] none
     * @returns  none
     */
     void updateAndResetFlashkeys();

    /**
     * Method to take the ping time hours and minutes fields on the screen and combine
     * them into one single ping time field and save them to the DOM (Messages.h).
     *
     * @param[in] none
     * @returns  none
     */
    void setDomPingInterval();

    /**
    * Method to take the four ping IP fields and combine the four separate fields
    * into one single IP address and save it to the DOM (Messages.h)
    *
    * @param[in] none
    * @returns  none
    */
    void setDomPingTarget();

    /**
    * Method to take the four ping IP fields and combine the four separate fields
    * into one single IP address and save it to the DOM (Messages.h)
    *
    * @param[in] none
    * @returns  none
    */
    void setDomNowServingServer();

    /**
    * Method to get the two digit country code value
    *
    * @param[in] none
    * @returns  none
    */
    QString getPrimaryCountryCode( int index );

    /**
    * Method to get the two digit language code
    *
    * @param[in] none
    * @returns  none
    */
    QString getPrimaryLanguageCode( int index );

    /**
    * Method to get the two digit country code value
    *
    * @param[in] none
    * @returns  none
    */
    QString getSecondaryCountryCode( int index );

    /**
    * Method to get the two digit language code
    *
    * @param[in] none
    * @returns  none
    */
    QString getSecondaryLanguageCode( int index );

    /**
    * Method to check and set all accumulate items field on the accumulate tab. Most all
    * of the fields are enabled or disabled dependant on the ean string itself and if
    * accumulate items field is set to enabled.
    *
    * @param[in] none
    * @returns none
    */
    void checkEnabledEanFields(bool isEnabled);

    /**
    * Method used to save the Scale Date Time format. The Date Time format is saved
    * directly to the scaleConfiguration class, not the normalScaleOperations class
    *
    * @param[in] none
    * @returns  none
    */
    void saveDateTimeFormat();

    /**
    * Method used to save all of the data on the Features tab. In addition, we have to
    * save 2 fields from tab 3 that deal with product and description on a flashkey.
    * After the data is saved to the database, send a save message because it's a
    * different save message than normal scale operations.
    *
    * @param[in] none
    * @returns  none
    */
    void saveDisplayOptions();

    /**
    * Method used to save accumulate items data if it has changed.
    *
    * @param[in] none
    * @returns  none
    */
    void saveAccumulateItems();

    /**
    * Method used to save the compress time to the database. The hours, minutes and
    * AM/PM string are combined together to form the saved compress time
    *
    * @param[in] none
    * @returns  none
    */
    // Removed compress time fields 05-15-2015 during timesys 1.15 release
    //void saveCompressTime();

    /**
     * Method to set up multiple fields required by the weigher and determins
     * if they should be displayed or disabled dependent if a weigher
     * is attached.  Also removes some weigher fields if metric is enabled.
     * This method uses the existing DomWeigherConfigRead class message
     * to determine the weigher settings.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void setupWeigherFields();

    /**
     * Method to set up multiple fields required by the weigher and determins
     * if they should be displayed or disabled dependent if a weigher
     * is attached.  Also removes some weigher fields if metric is enabled.
     * This method uses the incoming DomWeigherConfigRead message
     * to determine the weigher settings.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateWeigherFields(const SessionMessage &msg);

    /**
     * Method to set up normal scale configuration fields across multiple tabs. This includes
     * the Product, Scale Access, and System Tabs.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateNormalScaleOperations(const SessionMessage &msg);

    /**
     * Method to set up what fields will display in operate mode and some other
     * additional configuration fields
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateOperatorDisplayFields(const SessionMessage &msg);

    /**
     * Method to set up and initialize the accumulate items fields.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateAccumulateItems(const SessionMessage &msg);

    /**
     * Method to re-initialize the Max Tare value during OOBE since setupValidators
     * was moved to sessionMessage()
     * @param[in] incomingMaxTare
     * @returns none
     */
    void initializeMaxTare(QString incomingMaxTare);

    /**
     * Method to updates the Prepack fields on the Wrapper, Sealer Belt, and Prepack related fields
     *
     * @param[in] SessionMessage
     */
    void setupPrepackFields(const SessionMessage &msg);

    /**
     * Return the time in a standard formatted hour:minute string
     *
     * @param[in] current time in QDateTimeFormat 00:00:00:000
     */
    QString getTheTime(QDateTimeEdit *myTime);

    /**
     * Method to discuss any and all added code and its impact to other code modules
     * to identify regression test requirements
     *
     * @param[in] none
     * @returns none
     */
    void doxygenImpactAndRegressionTestingComments();

    void populateSecondaryPositions();
    // PJR 454
    QVector <QWidget*> enableTheseBarcodeBoxes(int param);

    // SOF-3846
    bool cutterAttached_;
    bool disableCuttingBetweenLabelsToggled;
    bool cutterEnabled_;
};

/** @} */

#endif // SUPERNORMALSCALEOPERATION_H
