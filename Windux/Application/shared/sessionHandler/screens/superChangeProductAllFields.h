/**
 * @file superChangeProductAllFields.h
 * @brief Tab-based Widget for changing all fields of a Product
  *
  * @copyright Copyright (c) 2013 Hobart Corporation
  * @author Todd Davis
  * @addtogroup Supervisor and Service Mode CSU
  * @{
*/

#ifndef SUPERCHANGEPRODUCTALLFIELDS_H
#define SUPERCHANGEPRODUCTALLFIELDS_H

#include "screenwidget.h"

#include "../dialogs/texteditor.h"

#include "../widgets/shortListEditor.h"

#include <numericlineedit.h>

#define TEXT_EDITOR_MAX_CHARS 2000
#define ESC_CHAR 0x1B

class DomPluSupport;
class DomCurrentPLU;
class DomNormalScaleOperation;
class MadpScaleConfig;
class MadpCurrencySettings;
class MadpInternalConfig;
class MadpFrequentShopper;
class QIntValidator;
class SupervisorMode;
class QRegExpValidator;
class QSignalMapper;
class MadpPeripheralList;


namespace Ui {
class superChangeProductAllFields;
}

/**
* @class superChangeProductAllFields
* @brief  TBD
*
*/

class superChangeProductAllFields  : public ScreenWidget
{
    Q_OBJECT

    typedef superChangeProductAllFields _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
      * Constructor.
      */
    explicit superChangeProductAllFields(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superChangeProductAllFields();

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

    QString getEncodedEditorContents();
    static QString getEncodedTextOnly(QString incomingString);

private slots:

    void doPluLookup();
    void doReportingClassLookup();

    /**
     * Slot for widget editing finished.
     */
    void onEditingFinished( QWidget *widget );

    void doValidGraphicLookup1(int);
    void doValidGraphicLookup2(int);
    void doValidGraphicLookup3(int);
    void doValidGraphicLookup4(int);
    void newPLUOKButClicked();
    void newPLUCancelButClicked();
    void writeProductType(int);
    void tabChangedSlot(int);
    void displayTheFlashkeyGraphic(int);
    void onUpdateDiscountPricing(int newIndex);
    void productDescriptionEdit();

    //zzz
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void currentCharFormatChanged(const QTextCharFormat &format);
    void textSelect(bool yes);
    void doneEditing();
    void onTextChanged();

    // Used for Tracking Supervisor Changes
    void trackSuperChangeUnitPrice();
    void trackSuperChangeLbsForPrice();
    void trackSuperChangeFractionalPrice();
    void trackSuperChangeTare();
    void trackSuperChangePropTare();
    void trackSuperChangeByCount();
    void trackSuperChangeNetWeight();
    void trackSuperChangeFluidNetWeight();

    void onPrefixTextChanged();
    void onEANTextChanged();
    void onUpcNumTextChanged();
    void onProdNumTextChanged();

    void onUnitPriceTextChanged(QString focusTxt);
    void onLbsForTextChanged(QString focusTxt);
    void onFracPriceTextChanged(QString focusTxt);
    void onTareTextChanged(QString focusTxt);
    void onPropTareTextChanged(QString focusTxt);
    void onByCountTextChanged(QString focusTxt);
    void onNetWtTextChanged(QString focusTxt);
    void onFluidNetWtTextChanged(QString focusTxt);
    void onPriceEditedFinished();
    void onWeightEditedFinished();

    void onForcedTareChanged(int state);
    void onForcedByCountChanged(int state);
    void onForcedUnitPriceChanged(int state);

    void reuseDoneButClicked();
    void reuseCopyEditButClicked();
    void reuseDeleteButClicked();
    void reuseDeleteAllButClicked();
    void reuseNewButClicked();
    void reuseUseButClicked();
    void onPlacementViewClicked();

    void textEditorLimitInputToSize();
    void useSupportDataFor();

    void deleteProductClicked();
    void launchReuseReferencePLUGroupBox();
    void hideReuseReferencePLUGroupBox();

    // COOL Slots
    void deleteCOOL();
    void showFullText();
    void launchShortListEditor();
    void updateCoolShortList(int newIndex);
    void updateCoolCategory(int newIndex);
    void updateCoolPretext(int newIndex);
    void onShortListEditorCancelClicked();
    void onShortListEditorDoneClicked(bool didAddRecord);

    void onPrimaryPlacementChanged(int newIndex);

    void saveEntireProduct();

protected:

    /**
     * Check for specific error message is handled by screen.
     *
     * Session will call this method upon receiving an error message. The default error handling
     * (i.e. popup an error dialog) shall be invoked unless @c true is returned from
     * this property.
     *
     * @returns  @c true to use custom error handler, @c false otherwise
     */
    virtual bool isErrorMessageHandler( const SessionMessage& errorMsg ) const;

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

    virtual bool eventFilter( QObject *obj, QEvent *event );

    //Ui::superChangeProductAllFields *ui;

private:
    // make this protected instead of private so I can start extending this class.
    // It's a monopoly, time to break it up.
    Ui::superChangeProductAllFields *ui;

    SupervisorMode *supervisorMode_;
    int updatingFromIncomingPLUData;

    enum SaveWho
    {
        SAV_UNITPRICE,
        SAV_LBSFOR,
        SAV_FRACPRICE,
        SAV_TARE,
        SAV_PROPTARE,
        SAV_BYCOUNT,
        SAV_NETWT,
        SAV_FLNETWT,
        SAV_NONE
    };

    enum CodeType
    {
        UPCA, 				// 0
        UPCD3,
        UPCE,				// 2            - not supported
        //
        EAN13,				// 3
        EAN128C,
        //
        RSS14E,             // 5    GS1
        RSS14E_GTIN_13,     // 6    GTIN    - limit this to 13 chars?
        RSS14E_GTIN_12,     // 7    GTIN    - limit to 12 chars?
        RSS14E_GTIN_8,      // 8    GTIN    - limit to 8 chars?
        //
        QR_LEVELM,			// 9    QR      - limits?
        QR_LEVELL,          // 10   QR      - limits?
        QR_LEVELQ,          // 11   QR      - limits?
        QR_LEVELH,          // 12   QR      - limits?
        DATAMATRIX			// 13   DM      - limits?
    };

    enum RelevantFields
    {
        UseNoFields                 = 0,
        UseProductNumber            = 1,        // Same as UPC Number/EAN Number
        UseVendorNumber             = 2,
        UseProductAndVendorNumber   = 3,
        UseEanCode                  = 4         // these done really match APP bc over there they are bitwise not enums
    };



    /**
     * Method to initialize all variables (over and over and over)
     *
     * @param[in] none
     * @returns  none
     */
    void initialize();

    /**
     * Method to initialize all variables once and only once and other inique tasks
     *
     * @param[in] none
     * @returns  none
     */
    void realInitialize();

    /**
     * @brief updateProductInformation
     * Method to initialize stuff about the product itself.  Pull stuff out of initialize() that should not be there.
     */
    void updateProductInformation();

    void changeUPCNumberBoxes(int value);
    void loadPricingTab(int productType);
    int loadDiscountInformation(int xPos, int yPos);

    /**
     * Method to set given tab pane components visible or hidden
     *
     * @param[in] none
     * @returns  none
     */
    void governFields(int,bool);

    void saveTabData(int);

    void loadIncomingPLUData(const SessionMessage &msg);
    void loadLabelGraphicNames(const SessionMessage &msg);
    void setGraphicCurrentIndex(QWidget *widget, const QString& grName);
    void loadTextTabs();

//  void saveEntireProduct();
    void hidePriceFields();

    void requestCategories();
    void loadCategoriesCombo(const SessionMessage &msg);

    void requestSpecials();
    void loadSpecialsCombo(const SessionMessage &msg);

    void requestFlashkeyGraphics();
    void loadFlashkeyGraphicsCombo(const SessionMessage &msg);
    void setFlashkeyGraphicsComboIndex();

    // COOL private methods
    void setDefaultCoolTextForProduct();
    void updateRunModeValue(const SessionMessage& msg);
    void buildAllPretextsList(const SessionMessage& msg);
    void buildAllCategoriesList(const SessionMessage& msg);
    void buildAllCoolTextByProduct(const SessionMessage& msg);
    void buildPretextsListByCategory(const SessionMessage& msg);
    void packCombo( QString names, QString recordNums, int numOfItems );
    void buildCoolList(QWidget *widget, QString names, QString recordIds, int numOfItems, int numToSelect);

    void SaveMain();
    void SavePricing();
    void SaveProductLife();
    void SaveLabeling();
    void SaveTexts();
    void SaveFlashkeys();
    void SaveCOOL();

    void blankEverything();
 //   void displayTheFlashkeyGraphic(int);

    void initValidators();

    /**
     * Function to scale sub-widgets that may not have access to screenwidget functions,
     * specifically the shortListEditor. Just call this and pass the current instance.
     *
     * @param    widgetToScale - widget to scale to current screen size (if needed)
     * @returns  none
     */
    void scaleWidgets(QWidget* widgetToScale);

    /**
     * Method to set up the fields for all three Applier rotations and Label placements.
     * Also includes Intake mode.
     *
     * @param[in] none
     * @returns  none
     */
    void setupWrapperAndApplierFields();

    /**
     * Method to discuss any and all added code and its impact to other code modules
     * to identify regression test requirements
     *
     * @param[in] none
     * @returns none
     */
    void doxygenImpactAndRegressionTestingComments();

    void populateSecondaryPlacementComboBox();

    QSignalMapper *editingFinishedSignal_;

    DomCurrentPLU *currentPLUData;
    DomNormalScaleOperation *normalScaleOperation;
    MadpScaleConfig *scaleConfigData;
    MadpCurrencySettings *currencyData;
    MadpInternalConfig *internalConfigData;
    MadpFrequentShopper *frequentShopperData;
    hobart::ShortListEditor *shortListEditor_;                    // COOL private variable

    QVector<QString> shortListNamesList;
    QStringList specialNumbersList;
    QStringList shortListNumList;

    bool showForcedFields;
    bool isPlatterTareOn;
    bool isMetric;
    bool switchToMainTab;
    bool clearedUPCType;
    bool coolTabIsPresent;
    bool isFullMode;                                                // COOL private variable
    bool ignoreSignals_;
    bool ignoreDiscountPriceSignals_;
    bool initializationComplete;                                    // some thins we don't want to redo over
    bool hasLabelBeenUpdated;                                       // when a label changes we have to re-request the PLU info
    bool isTabMainClicked;                                          // has one been clicked?
    bool alreadyAdded;                                              // has items been added to FPP combo box
    bool savefbcTransaction;                                        // if bc was forced, and then changed to not
    bool isTwoPrinterSystem_;

    int tabToSave;
    int pluEmitted;                                                 // what plu are we requesting?
    int labelEmitted_1;                                             // what primary  label are we requesting?
    int labelEmitted_2;                                             // what secondary label are we requesting?
    int labelEmitted_3;                                             // what third label are we requesting?
    int suppTextEmitted;                                            // what text are we going to emit
    int alreadyDidThis;                                             // if we already set the validator.
    int reportingClassEmitted;

    SaveWho whoHasFocus;                                                // who has focus

    QString previousPLUItemNumber;

    ////////////////////////////
    // Product Data variables for a save
    QString Item_Number;
    QString Product_Type;
    QString Description;
    QString Barcode_Type;
    QString Barcode_Fields;
    QString Number_System;
    QString Product_Number;
    QString Venodr_Number;
    QString EAN_Code;
    QString By_Count;
    QString Exception_Price;
    QString Unit_Price;
    QString Discount_Type;
    QString Discount_Value1;
    QString Discount_Value2;
    QString Discount_Value3;
    QString Discount_Label1;
    QString Discount_Label2;
    QString Discount_Label3;
    QString Tare;

    QString prev_unitprice;
    QString prev_lbsforprice;
    QString prev_fracprice;
    QString prev_tareA;
    QString prev_tareB;
    QString prev_proptare;
    QString prev_bycount;
    QString prev_netwt;
    QString prev_fluidnetwt;

    QString Proportional_Tare;
    QString Fixed_Weight;
    QString Shelf_Life_Days;
    QString Shelf_Life_Hours;
    QString Product_Life_Days;
    QString Product_Life_Hours;
    QString Price_Modifier;
    QString Reporting_Class;
    QString Primary_Label_Type;
    QString Primary_Label_Rotation;
    QString Secondary_Label_Type;
    QString Secondary_Label_Rotation;
    QString Third_Label_Type;
    QString Third_Label_Rotation;
    QString Back_Date_Days;
    QString Points;
    QString Graphic_1_Name;
    QString Graphic_2_Name;
    QString Graphic_3_Name;
    QString Graphic_4_Name;
    QString Primary_Label_Type_Width;
    QString Primary_Label_Type_Length;
    QString Secondary_Label_Type_Width;
    QString Secondary_Label_Type_Length;
    QString Third_Label_Type_Width;
    QString Third_Label_Type_Length;
    QString Forced_By_Count;
    QString Forced_Tare;
    QString Fracitonal_Price_Unit;

    QString widgetUsedForProductNumber;
    QString widgetUsedForVendorNumber;
    QString widgetUsedForEANCode;
    QString widgetUsedForNumberSystem;
    QString widgetUsedForPriceModifier;
    QString widgetUsedForDiscountValue1;
    QString widgetUsedForFixedWeight;
    QString widgetUsedForPrice;

    QString Product_Number_Orig;
    QString Vendor_Number_Orig;
    QString EAN_Code_Orig;

    QString unitSeparator;

    bool cameFromCreate;

    int textEditorHeight, textEditorWidth, textEditorFontSize;
    QString textEditorFontName;
    void editText(QString incomingText, QString labelWidth, QString labelHeight, QString fieldType, QString fontName, QString fontSize);
    void loadReuseFrame(const SessionMessage& msg, QString textType);

    QFont textFont;
    QFont font4Backspacing;
    QTextEdit *textEdit;
    QString lastTextEditHtml;
    void updateFontSizeComboBox();
    void setTextEditRegionFont();
    void setTextEditRegionFontSize();
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void setFieldNameWatermark(QString fieldName);
    void fontChanged(const QFont &f);
    void encodeBuffer();
    QString rstrip(const QString& str, const char *skip = " \r\n\t");
    void decodeBuffer(QString contents);
    QString getEncodedMarqueeTextOnly(QString marqueeText);

    bool textSelected;
    bool useFontChanges;

    QString encodedBuffer;
    QString escSeq;
    QString doubleEscSeq;
    QString defaultFontFamily;
    QString defaultFontSize;
    QString defaultLabelWidth;
    QString defaultLabelHeight;
    //MadpInternalConfig *internalConfigData;

    QString fieldName;
    QString initialContents;
    bool editable;

    // FT-581
    QString newCOOLTextAddedID;

    QString formattedDescription; //30
    QString formattedExpandedText; //1-106
    QString formattedSpecialMessage; // 2-107
    QString formattedProductNote; // 3-108
    QString formattedProductMarquee; // 4-137
    QString formattedNutrifacts; //20-?
    QString formattedCookingInstructions; // 5-165
    QString formattedRecipes; // 6-166
    QString formattedExpandedText2; // 7-167
    QString formattedSuggestiveSelling; // 8-168
    QString formattedAllergens; // 9-169

    QString reuseTextType;
    QString restoreReuseTextType;

    QString aspTextType;
    QString aspTextFunction;
    QString aspUsedByMany;
    QString aspSupportPLU;
    QString aspSupportText;
    QString aspSupportText2;
    QString aspSupportText3;
    QString aspSupportText4;
    QString aspSupportText5;

    int messageTextType;
    QString messageTextText;
    void getTextsFromBackend();
    bool popupActive;
    bool cameFromCancel;
    bool wroteTransaction;
    bool ifUpdating;

    SessionMessage reuseMessageVar;

    QString reuseReferencePLUNumber;

    //void reuseUseButClicked();

    int makeValidatorTop(int digits);

    QIntValidator *productNumberValidator_;

    //Tab1
    QIntValidator *le1ReportingClassValidator_;

    QIntValidator *txtUPCVendorNumberValidator_;
    QIntValidator *txtUPCProductNumberValidator_;
    QIntValidator *txtUPCNumberValidator_;
    QIntValidator *txtPrefixValidator_;
    QValidator *txtEANCodeValidator_;

    //Tab2
    QIntValidator *le2Discount1Validator_;
    QIntValidator *le2Discount2Validator_;
    QIntValidator *le2Discount3Validator_;
    QIntValidator *le2UnitPriceValidator_;
    QIntValidator *minPriceValidator_;
    QIntValidator *maxPriceValidator_;
    QIntValidator *le2PointsValidator_;
    QIntValidator *le2ByCountValidator_;
    QIntValidator *le2TareValidator_;
    QIntValidator *le2ProportionalTareValidator_;
    QIntValidator *le2NetWeightValidator_;
    QIntValidator *minWeightValidator_;
    QIntValidator *maxWeightValidator_;
    QIntValidator *le2NetWeightFluidValidator_;
    QIntValidator *le2ExceptionPriceValidator_;
    QIntValidator *le2RegularPriceValidator_;
    QIntValidator *le2FractionalPriceValidator_;
    QIntValidator *le2PoundsForPriceValidator_;
    QIntValidator *le2TarePlatterValidator_;

    //Tab3
    QIntValidator *le3ShelfLifeDaysValidator_;
    QIntValidator *le3ShelfLifeHoursValidator_;
    QIntValidator *le3ShelfLifeMinutesValidator_;
    QIntValidator *le3ProductLifeDaysValidator_;
    QIntValidator *le3ProductLifeHoursValidator_;
    QIntValidator *le3ProductLifeMinutesValidator_;

    QIntValidator *le3PortionsValidator_;
    QIntValidator *le3ProductionPlanningTypeValidator_;
    QIntValidator *le3BuildAMealValidator_;
    QIntValidator *le3NumberSidesValidator_;
    QIntValidator *le3MeatSpecSheetValidator_;

    //Tab4
    QIntValidator *priDefaultLabelTypeValidator_;
    QIntValidator *secDefaultLabelTypeValidator_;
    QIntValidator *triDefaultLabelTypeValidator_;

    QIntValidator *priManualLabelTypeValidator_;
    QIntValidator *secManualLabelTypeValidator_;
    QIntValidator *triManualLabelTypeValidator_;

    QIntValidator *priPrepackLabelTypeValidator_;
    QIntValidator *secPrepackLabelTypeValidator_;
    QIntValidator *triPrepackLabelTypeValidator_;

    QIntValidator *priProdEntryLabelTypeValidator_;
    QIntValidator *secProdEntryLabelTypeValidator_;
    QIntValidator *triProdEntryLabelTypeValidator_;

    QIntValidator *priSelfServLabelTypeValidator_;
    QIntValidator *secSelfServLabelTypeValidator_;
    QIntValidator *triSelfServLabelTypeValidator_;

    QIntValidator *priPick5LabelTypeValidator_;
    QIntValidator *secPick5LabelTypeValidator_;
    QIntValidator *triPick5LabelTypeValidator_;

    QIntValidator *priOnlineOrderingLabelTypeValidator_;
    QIntValidator *secOnlineOrderingLabelTypeValidator_;
    QIntValidator *triOnlineOrderingLabelTypeValidator_;

    DomPluSupport *pluSupport_;
    bool goingToNutrifactsEditorScreen;
    bool goingToMarqueeScreen;
    bool goingToLabelPlacementView;
    bool showAllFieldsReuseMarquee_;
    bool showAllFieldsReuseNutrifacts_;

    QString decodeBufferIncomingFontName;
    QString decodeBufferIncomingFontSize;

    int countChars(QString incomingString, QChar character);

    int chosenFontIndex;
    int indexOfFontName(QString fontName);
    int labelFormSizeIncomingFont;
    int labelFormSizeIncomingFontSize;

    QPushButton *placementView_;
};
/** @} */
#endif // SUPERCHANGEPRODUCTALLFIELDS_H
