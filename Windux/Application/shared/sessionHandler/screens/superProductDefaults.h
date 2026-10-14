/**
 * @file SuperProductDefaults.h
 * @brief Super Product Defaults sets up all of the initial settings for each product type, ByCount,
 * Random Weight, FixedWeight, Fluid Ounces. These can include default setting for fields such as Unit Price
 * EAN Format String, Description, or Tare.
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 * @addtogroup SupervisorProductDefaultsCsu
 * @{
*/


#ifndef SUPERPRODUCTDEFAULTS_H
#define SUPERPRODUCTDEFAULTS_H

#include "screenwidget.h"
#include <Messages.h>
#include "../widgets/eanEditor.h"
#include "../widgets/gs1Editor.h"
#include "../widgets/datamatrixEditor.h"

class SupervisorMode;

class DomDefaultCurrentPLU;
class QIntValidator;
class QPushButton;              // prolly wont need this anymore
class DomEvolWrapperSystemInformation;

namespace Ui {
class SuperProductDefaults;
}

/**
* @class SuperProductDefaults
* @brief Sets up and initializes any default fields for a product.
*
*/

class SuperProductDefaults  : public ScreenWidget
{
    Q_OBJECT

    typedef SuperProductDefaults _Myt;
    typedef ScreenWidget _Mybase;

public:
    /**
      * Constructor.
      */
    explicit SuperProductDefaults(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~SuperProductDefaults();

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
    * SLOT method called the EAN format field is selected
    *
    * @param[in] none
    * @returns  none
    */
    void launchEanEditor();

    /**
    * SLOT method called the EAN format field is selected
    *
    * @param[in] none
    * @returns  none
    */
    void launchEan128Editor();

    /**
    * SLOT method called the GS1 Random Weight for GTIN format field is selected
    *
    * @param[in] none
    * @returns  none
    */
    void launchRwGTIN();

    /**
    * SLOT method called the GS1 Fixed Weight for GTIN format field is selected
    *
    * @param[in] none
    * @returns  none
    */
    void launchFwGTIN();

    /**
    * SLOT method called the GS1 By Count for GTIN format field is selected
    *
    * @param[in] none
    * @returns  none
    */
    void launchBcGTIN();

    /**
    * SLOT method called the GS1 Fluid Ounce for GTIN format field is selected
    *
    * @param[in] none
    * @returns  none
    */
    void launchFlGTIN();

    /**
    * SLOT method called the GS1 Random Weight for AI format field is selected
    *
    * @param[in] none
    * @returns  none
    */
    void launchRwAI();

    /**
    * SLOT method called the GS1 Fixed Weight for AI format field is selected
    *
    * @param[in] none
    * @returns  none
    */
    void launchFwAI();

    /**
    * SLOT method called the GS1 By Count for AI format field is selected
    *
    * @param[in] none
    * @returns  none
    */
    void launchBcAI();

    /**
    * SLOT method called the GS1 Fluid Ounce for AI format field is selected
    *
    * @param[in] none
    * @returns  none
    */
    void launchFlAI();

    /**
    * SLOT method called when launching the QR Code Data Matrix Editor
    *
    * @param[in] none
    * @returns  new format string
    */
    void launchQRCodeEditor();

    /**
    * SLOT method called when launching the Data Matrix Editor
    *
    * @param[in] none
    * @returns  new format string
    */
    void launchDataMatrixEditor();

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

    /**
     * SLOT method called when the DONE button is clicked in the GS1 editor.
     *
     * @param[in] none
     * @returns  none
     */
    void onGS1EditorDoneClicked(const QString &gs1String);

    /**
     * SLOT method called when the CANCEL button is clicked in the GS1 editor.
     *
     * @param[in] none
     * @returns  none
     */
    void onGS1EditorCancelClicked();

    /**
     * SLOT method called when the DONE button is clicked in the Data Matrix editor.
     *
     * @param[in] none
     * @returns  none
     */
    void onDataMatrixEditorDoneClicked(QString datamatrixString);

    /**
     * SLOT method called when the CANCEL button is clicked in the Data Matrix editor.
     *
     * @param[in] none
     * @returns  none
     */
    void onDataMatrixEditorCancelClicked();

    /**
    * SLOT method called when a new index is selected in the combo box.
    *
    * @param[in] newIndex, newIndex is the new currentIndex in the combo box
    * @returns  none
    */
    void comboboxChanged(int newIndex);

    void onPrimaryPlacementComboChanged(int newIndex);
    void onSecondaryPlacementComboChanged(int newIndex);
    void onThirdPlacementComboChanged(int newIndex);

    void onPrimaryRotationComboChanged(int newIndex);
    void onSecondaryRotationComboChanged(int newIndex);
    void onThirdRotationComboChanged(int newIndex);

    /**
    * SLOT method called when text in a text field is modified
    *
    * @param[in] none
    * @returns  none
    */
    void textboxChanged();

    /**
    * SLOT method called when a new product type is selected in the combo box.
    * The current product type information needs to be gathered up and saved
    * off to the application, and then a new product type is requested.
    *
    * @param[in] newIndex, newIndex is the new currentIndex in the combo box
    * @returns  none
    */
    void requestNewProductType(int newProductType);

    /**
    * SLOT method called when any checkbox is selected or deselected.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChanged(int state);

    /**
    * SLOT method called when the user selects a forced Unit Price and clears the unitPrice field.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedForcedUnitPrice(int state);

    /**
    * SLOT method called when the user selects a forced By Count and clears the bycount field.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedForcedByCount(int state);

    /**
    * SLOT method called when the user selects a forced Tare and clears the tare field.
    *
    * @param[in] state, where 0=Unchecked, 1=PartiallyChecked(not used here), and 2=Checked
    * @returns  none
    */
    void checkboxChangedForcedTare(int state);

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

    void onPlacementButtonClicked();


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
     * Handle an event for widgets
     *
     * @param[in] obj  widget that has an event
     * @param[in] event fired for the widget
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

private:
    Ui::SuperProductDefaults *ui;

    DomDefaultCurrentPLU *defaultCurrentPLU_;
    MadpScaleConfig *scaleConfigData_;
    DomEvolWrapperSystemInformation *wrSystemInfo_;
    MadpCurrencySettings *currencyData;
    hobart::eanEditor *eanConfigEditor_;
    hobart::gs1Editor *gs1ConfigEditor_;
    hobart::datamatrixEditor  *datamatrixConfigEditor_;

    SupervisorMode *supervisorMode_;

    QStringList zeros;

    bool hasChanged_;                                               // has data been modified
    bool isScaleMetric_;                                            // is the scale metric?
    bool ignoreSignals_;                                            // when certain events occur, don't trigger SIGNALS
    bool isUpdating_;

    int eanEditorType;                                              // 1=regular editor, 2=ean128 editor
    int gs1EditorType;                                              // 1=rwgtin, 2=fwgtin, 3=bcgtin, 4=rwai, 5=fwai, 6=bcai
    int matrixEditorType;                                           // 1=qrcode, 2=datamatrix
    int currentTab;                                                 // what tab we are currently hanging out on.
    int tareEntryPrecision;                                         // gets the current tare precision


    QString savedProductType;
    QString unitSeparator;                                          // decimal or comma
    QString unitSymbol;                                             // lb or kg

    QIntValidator *eanNumberSystemValidator_,
    *reportingClassValidator_,
    *discountValidator1_,
    *discountValidator2_,
    *discountValidator3_,
    *unitPriceValidator_,
    *byCountValidator_,
    *tareValidator_,
    *proportionalTareValidator_,
    *netWeightValidator_,
    *exceptionPriceValidator_,
    *pointsValidator,
    *shelfLifeDaysValidator_,
    *shelfLifeHoursValidator_,
    *productLifeDaysValidator_,
    *productLifeHoursValidator_,
    *pointsValidator_,
    *priDefaultLabelTypeValidator_,
    *secDefaultLabelTypeValidator_,
    *triDefaultLabelTypeValidator_,
    *priManualLabelTypeValidator_,
    *secManualLabelTypeValidator_,
    *triManualLabelTypeValidator_,
    *priPrepackLabelTypeValidator_,
    *secPrepackLabelTypeValidator_,
    *triPrepackLabelTypeValidator_,
    *priProdEntryLabelTypeValidator_,
    *secProdEntryLabelTypeValidator_,
    *triProdEntryLabelTypeValidator_,
    *priSelfServLabelTypeValidator_,
    *secSelfServLabelTypeValidator_,
    *triSelfServLabelTypeValidator_,
    *priPick5LabelTypeValidator_,
    *secPick5LabelTypeValidator_,
    *triPick5LabelTypeValidator_;

    /**
     * Method to initialize all variables, sets components visible or hidden, and creates all signals and slots
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
     * Method to update the product defaults screen with default data values that
     * all plu numbers of this product type have in common.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateDefaultPluData(const SessionMessage &msg);

    //void populateSecondaryPlacementComboBox();

    /**
     * Method to update the combo boxes on the Labeling tab with the names of the graphics.
     *
     * @param[in] msg  msg is the data received from the Scale Application.
     * @returns  none
     */
    void updateGraphicNames(const SessionMessage &msg);

    /**
     * Method to set up all of the signals and slots for normal scale operations. There are
     * so many widgets with events and the initialize method was getting too large
     *
     * @param[in] none
     * @returns  none
     */
    void setupSlotsAndSignals();

    /**
     * Method to set up any and all controls that need to be initialized with default data.
     * For the most part this pertains to adding items to combo boxes
     *
     * @param[in] none
     * @returns  none
     */
    void setupWidgets();

    /**
     * Function to scale sub-widgets that may not have access to screenwidget functions,
     * specifically EAN/GS1 editor frames. Just call this and pass the current instance.
     *
     * @param    widgetToScale - widget to scale to current screen size (if needed)
     * @returns  none
     */
    void scaleWidgets(QWidget* widgetToScale);

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
     * Method to set up any and all symbols for a specific country and language.
     * Currently these include the currency symbol and the units for weight.
     *
     * @param[in] none
     * @returns  none
     */
    void setupCountrySymbols();

    /**
     * Method to set up the fields for all three Applier rotations and Label placements.
     * Also includes Intake mode.
     *
     * @param[in] none
     * @returns  none
     */
    void setupWrapperAndApplierFields();

    /**
     * Method to set up field visibility dependant on the product type
     *
     * @param[in] none
     * @returns  none
     */
     void setupVisibility(int pluType);

     /**
     * Method to validate fields
     *
     * @param[in] digits  digits is current digits in the text input line.
     * @returns digits  where digits is the new number that has been rolled over due to being over length
     */
     int makeValidatorTop(int digits);

     /**
     * SLOT method called when a new product type is selected in the combo box.
     * Depending on the product type selected, the UPC Number field needs reset.
     *
     * @param[in] newIndex, newIndex is the new currentIndex in the combo box
     * @returns  none
     */
     void repopulateUPCTypeCombo(int);

     /**
     * Method to remove font formatting
     *
     * @param[in] incomingString string to be stripped.
     * @returns a string with formatting removed. Only plain text.
     */
     QString getEncodedTextOnly(QString incomingString);

     /**
     * Method to enable barcode strings
     *
     * @param[in] indicator if fields are enabled.
     * @returns none
     */
     void enableRandomWtBarcodes(bool isEnabled);

     /**
     * Method to enable barcode strings
     *
     * @param[in] indicator if fields are enabled.
     * @returns none
     */
     void enableFixedWtBarcodes(bool isEnabled);

     /**
     * Method to enable barcode strings
     *
     * @param[in] indicator if fields are enabled.
     * @returns none
     */
     void enableByCountBarcodes(bool isEnabled);

     /**
     * Method to enable barcode strings
     *
     * @param[in] indicator if fields are enabled.
     * @returns none
     */
     void enableFluidWtBarcodes(bool isEnabled);

     /**
     * Method set up secondary placement dialog depending of the choice of the primary
     *
     * @param[in] none
     * @returns none
     */
     void populateSecondaryPositions();
};

#endif // SUPERPRODUCTDEFAULTS
