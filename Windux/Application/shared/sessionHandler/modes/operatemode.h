/**
 * @file operatemode.h
 * Operate mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef OPERATEMODE_H
#define OPERATEMODE_H

#include "modewidget.h"

#include "../screens/capacitybarwidget.h"
#include "../screens/weightbarwidget.h"
#include "../../sessionHandlerWidgets/softkeytogglemanager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Softkey actions.
enum SoftkeyActions
{
    Action_NoAction = 0,
    Action_PrintLabel,
    Action_ChangeScreen,
    Action_BrowseWeb,
    Action_ChangeLanguage,
    Action_LoginOperator,                           // 5
    Action_ResetPrice,
    Action_SetPlatterTare,
    Action_SelectProduct,
    Action_LoadSoftkeyGroup,
    Action_LoadCategory_First,                      // 10
    Action_LoadCategory_Next,
    Action_LoadCategory_Previous,
    Action_ApplyDiscount_CentsOff,
    Action_ApplyDiscount_PercentOff,
    Action_VoidTransaction,                         // 15
    Action_LoadSpecials_First,
    Action_LoadSpecials_Next,
    Action_LoadSpecials_Previous,
    Action_LoadSearch_First,
    Action_LoadSearch_Next,                         // 20
    Action_LoadSearch_Previous,
    Action_PrintExistingExpirationLabel,
    Action_FPP_URL,
    Action_Application_URL,
    Action_BeginCutTest,                            // 25
    Action_EndCutTest,
    Action_SelectAnyProduct,
    Action_PrintRunTotals,
    Action_SwitchTotalType,
    Action_PrintMultipleLabels,                     // 30
    Action_ToggleProductionEntryMode,
    Action_PrintNewExpirationLabel,
    Action_LoadTareSoftkey,
    Action_ChangeLabelPlacementAndPosition,
    Action_ServeNextCustomer,                       // 35
    Action_NowServingQueueManagement,
    Action_ChangeLabelType,
    Action_CutTestHistory,
    Action_LoadAlphabeticSearch_First,
    Action_LoadAlphabeticSearch_Next,               // 40
    Action_LoadAlphabeticSearch_Previous,
    Action_LoadMostCommon_First,
    Action_LoadMostCommon_Next,
    Action_LoadMostCommon_Previous,
    Action_RedisplayCurrentGrid,                    // 45
    Action_ToggleAccumulateItemsMode,
    Action_AddItemToAccumulatedItems,
    Action_RemoveLastItemFromAccumulatedItems,
    Action_ShowAccumulatedItems,
    Action_RemoveAllAccumulatedItems,               // 50
    Action_PrintAccumulatedItems,
    Action_LogoutOperator,
    Action_PlayVideo,
    Action_Portioning,
    Action_LaunchExternalApp,                       // 55
    Action_AddSideItemToMeal,
    Action_AddSideItemToMeatSpecSheet,
    Action_CutTestSuspend,
    Action_CutTestResume,
    Action_ToggleMode,                              // 60 SAME
    Action_OpenDocumentFolder,						// 61 SAME
    Action_TogglePick5Active,                       // 62 SAME
    Action_Toggle2ndLabelEnable,                    // 63 from 66 2-11-25 - KEEP THESE COSTCO COMPATIABLE WHICH THEY ARE NOW
    Action_OverrideWrapperCameraDimensions,         // 64 from 63 2-11-25 - KEEP THESE COSTCO COMPATIABLE WHICH THEY ARE NOW
    Action_ToggleExtraShelfLifeDay,                 // 65 SAME
    Action_Swipe,                                   // 66 from 64 2-11-25 - CUSTOMERS WILL HAVE TO UPDATE DBs to ACCOMODATE
    Action_ChangeDepartment,						// 67 from 67
    Action_LastAction
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class OperateMode : public ModeWidget
{
    Q_OBJECT

    Q_PROPERTY( CapacityBarArea capacityBarArea READ capacityBarArea WRITE setCapacityBarArea )
    Q_PROPERTY( QMargins capacityBarMargins READ capacityBarMargins )
    Q_PROPERTY( bool isForcedFields READ isForcedFields )
    Q_PROPERTY( bool isPopupNumberPad READ isPopupNumberPad )
    Q_PROPERTY( WeightBarArea weightBarArea READ weightBarArea WRITE setWeightBarArea )
    Q_PROPERTY( QMargins weightBarMargins READ weightBarMargins )
    Q_PROPERTY( bool widgetDragLock READ widgetDragLock WRITE setWidgetDragLock )
    Q_PROPERTY( bool widgetSettingsLock READ widgetSettingsLock WRITE setWidgetSettingsLock NOTIFY widgetSettingsLockChanged )

    typedef OperateMode _Myt;
    typedef ModeWidget _Mybase;

Q_SIGNALS:

    /**
     * Signal for PLU data changed.
     *
     * @param[in] nof  @c true when plu not on file, @c false otherwise
     */
    void currentProductChanged( bool nof );

/**
     * Signal for PLU data cleared after a run on a cut test.
     *
     * @param[in] none
     */
    void clearPluForCutTest();

    /**
     * Signal for PLU totals.
     */
    void currentProductTotalsChanged();

    /**
     * Signal for PLU support.
     *
     * @param[in] type  product support type
     */
    void currentProductSupportChanged( int type );

    /**
     * Signal for updating fields.
     */
    void currentProductUpdateField( const DomUpdateRunModeField& f );

    /**
     * Signal for when widget settings lock changes.
     *
     * @param[in] locked  lock value
     */
    void widgetSettingsLockChanged( bool locked );

public:

    /**
     * Product support record types.
     */
    enum ProductSupportType
    {
        ExpandedText = 1,                           ///< Expanded Text
        SpecialMessage,                             ///< Special Message
        ProductNote,                                ///< Product Note
        Marquee,                                    ///< Marquee
        CookingInstructions,                        ///< Cooking Instructions
        Recipe,                                     ///< Recipe Data
        ExpandedText2,                              ///< Expanded Text 2
        SuggestiveSell,                             ///< Suggestive Sell
        Allergens,                                  ///< Allergens
        Nutrifacts = 20                             ///< Nutrifacts
    };

    /**
     * Product single field write offsets.
     */
    enum ProductWriteSingleFieldOffset
    {
        OffsetUnknown,
        OffsetProductNum = 1,
        OffsetProductType,
        OffsetDescription,
        OffsetBarcodeType,
        OffsetBarcodeFields,            // 5
        OffsetBarcodePrefix,
        OffsetBarcodeProductNum,		//	HA-100
        OffsetBarcodeVendorNum,			//	HA-100
        OffsetBarcodeEANCode,
        OffsetByCount,                  // 10
        OffsetExceptionPrice,
        OffsetUnitPrice,
        OffsetDiscountType,
        OffsetDiscountValue1,
        OffsetDiscountValue2,           // 15
        OffsetDiscountValue3,
        OffsetDiscountLabel1,
        OffsetDiscountLabel2,
        OffsetDiscountLabel3,
        OffsetTare,                     // 20
        OffsetPropTare,                 // proportional tare
        OffsetFixedWeight,
        OffsetShelfLifeDays,            // sell by
        OffsetShelfLifeHours,
        OffsetProductLifeDays,          // 25 best before
        OffsetProductLifeHours,
        OffsetPriceModifier,
        OffsetProductReportingClassNum,
        OffsetPrimaryLabelType,
        OffsetPrimaryLabelRotation,     // 30
        OffsetSecondLabelType,
        OffsetSecondLabelRotation,
        OffsetThirdLabelType,
        OffsetThirdLabelRotation,
        OffsetBackDateDays,             // 35 packed on
        OffsetPoints,
        OffsetGraphic1Name,
        OffsetGraphic2Name,
        OffsetGraphic3Name,
        OffsetGraphic4Name,             // 40
        OffsetPrimaryLabelTypeWidth,
        OffsetPrimaryLabelTypeLength,
        OffsetSecondLabelTypeWidth,
        OffsetSecondLabelTypeLength,
        OffsetThirdLabelTypeWidth,      // 45
        OffsetThirdLabelTypeLength,
        OffsetByCountForced,
        OffsetTareAForced,
        OffsetFractionalPriceUnit,
        OffsetPoundsForAmount,          // 50
        OffsetUnitPriceForced,
        OffsetTareType,
        OffsetApplySecurity,
        OffsetDoNotPrintPackedOn,
        OffsetDoNotPrintSellBy,         // 55
        OffsetDoNotPrintBestBefore,
        OffsetPrimaryLabelPlacement,
        OffsetSecondLabelPlacement,
        OffsetThirdLabelPlacement,
        OffsetFractionalPrice,          // 60
        OffsetForcedFractionalPrice,
        OffsetPoundsForPrice,
        OffsetForcedPoundsFor,
        OffsetSellByDate,
        OffsetSellByTime,               // 65
        OffsetBestBeforeDate,
        OffsetBestBeforeTime,
        OffsetHasNutrifactsFlag,
        OffsetIntakeMode,
        OffsetLabelHasFreqShopper,      // 70
        OffsetProductGraphicName,
        OffsetProductName1,
        OffsetProductName2,
        OffsetProdCategoryNum,
        OffsetProdSpecialNum,           // 75
        OffsetProdCoolCategoryNum,
        OffsetProdCoolPretextNum,
        OffsetProdCoolDefaultTextNum,
        OffsetCoolTextNum,
        OffsetProdCoolShortlistNum,		// 80
        OffsetForcedCool,
        OffsetCoolTrackingText,
        OffsetCoolRequired,
        OffsetKeyboardNetWeight,
        OffsetAcceptKbdNetWeight,       // 85
        OffsetNumOfLabelsForced,
        OffsetNumOfLabels,
        OffsetProductionPlanningType,
        OffsetForcedBackDateDays,
        OffsetExpirationDaysForced,     // 90
        OffsetExpirationDays,
        OffsetEANFormatString,
        OffsetEAN128CFormatString,
        OffsetPackedOnDate,
        OffsetGS1DatabarAI,             // 95
        OffsetBackDateDate,
        OffsetPortions,
        OffsetShowPortions,
        OffsetBuildAMealSideItemsGroupNum,
        OffsetBuildAMealSideItemsAreFree,  // 100
        OffsetBuildAMealNumberOfSidesIncluded,
        OffsetItemIsBuildAMealSideItem,
        OffsetCutTestMeatSpecSheetGroupNum,
        OffsetTareB,
        OffsetQRCodeFormatString,		// 105
        OffsetDataMatrixFormatString,
        OffsetMinWeight,
        OffsetMaxWeight,
        OffsetMinPrice,
        OffsetMaxPrice,                 // 110
        OffsetDefaultLabelTypeNum1,
        OffsetDefaultLabelTypeNum2,
        OffsetDefaultLabelTypeNum3,
        OffsetManualLabelTypeNum1,
        OffsetManualLabelTypeNum2,      // 115
        OffsetManualLabelTypeNum3,
        OffsetPrepackLabelTypeNum1,
        OffsetPrepackLabelTypeNum2,
        OffsetPrepackLabelTypeNum3,
        OffsetProdEntryLabelTypeNum1,   // 120
        OffsetProdEntryLabelTypeNum2,
        OffsetProdEntryLabelTypeNum3,
        OffsetSelfServLabelTypeNum1,
        OffsetSelfServLabelTypeNum2,
        OffsetSelfServLabelTypeNum3,    // 125
        OffsetPick5LabelTypeNum1,
        OffsetPick5LabelTypeNum2,
        OffsetPick5LabelTypeNum3,

        OffsetShelfLifeMinutes,         // sell by
        OffsetProductLifeMinutes,       // 130

        OffsetsMax
    };

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] screen  operate screen to display
     * @param[in] parent  parent @c SessionMainWindow widget
     */
    explicit OperateMode( ScreenWidget *screen, SessionMainWindow *parent = 0 );

    /**
     * Constructor.
     *
     * @param[in] parent  parent @c SessionMainWindow widget
     */
    explicit OperateMode( SessionMainWindow *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~OperateMode();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve capacity bar area.
     *
     * @returns  capacity bar area
     */
    virtual CapacityBarArea capacityBarArea() const;

    /**
     * Retrieve capacity bar margins.
     *
     * @returns  capacity bar margins
     */
    virtual QMargins capacityBarMargins() const;

    /**
     * Retrieve the current product.
     *
     * @returns  pointer to plu
     */
    virtual DomCurrentPLU *currentProduct() const;

    /**
     * Retrieve the current product support.
     *
     * @returns  pointer to product support record
     */
    virtual DomPluSupport *currentProductSupport( ProductSupportType type ) const;

    /**
     * Retrieve the current product totals.
     *
     * @returns  pointer to plu totals
     */
    virtual MadpUpdateCurrentPLUTotals *currentProductTotals() const;

    /**
     * Check for existence of forced fields.
     *
     * @returns  @c true if forced fields, @c false otherwise
     */
    virtual bool isForcedFields() const;

    /**
     * Check for forced tray entry.
     *
     * @returns  @c true if forced tray entry, @c false otherwise
     */
    virtual bool isForcedTrayEntry() const;

    /**
     * Clear the total price and weight in between requesting a new PLU
     *
     * @ returns none
     */
    virtual void clearPriceAndWeightFields();

    /**
     * Clear the plu in between requesting a new PLU
     *
     * @ returns none
     */
    virtual void clearPLU();

    /**
     * Check if the number pad is popup or static.
     *
     * The popup number pad will show and hide depending on the input field focus. When the number
     * pad is not popup it is static, and will always be visible even when no field has focus.
     *
     * @returns  @c true when popup number pad, @c false otherwise
     */
    virtual bool isPopupNumberPad() const;

    /**
     * Check if printing is allowed.
     *
     * @param[in] printKeyPressed  @c true when checking due to print key being pressed, @c false otherwise
     * @returns  @c true if allowed, @c false otherwise
     */
    virtual bool isPrintingAllowed( bool printKeyPressed = false ) const;

    /**
     * Check if printing production entry.
     *
     * @returns  @c true if printing production entry, @c false otherwise
     */
    virtual bool isPrintingProductionEntry() const;

    /**
     * Check if self service mode.
     *
     * @returns  @c true if self service mode, @c false otherwise
     */
    virtual bool isSelfService() const;

    /**
     * Set if best sellers should be refreshed when plu is cleared.
     *
     * @param[in] value  @c true to refresh, @c false otherwise
     */
    virtual void setRefreshBestSellers( bool value ) {refreshBestSellers_ = value;}

    /**
     * Retrieve weight bar area.
     *
     * @returns  weight bar area
     */
    virtual WeightBarArea weightBarArea() const;

    /**
     * Retrieve weight bar margins.
     *
     * @returns  weight bar margins
     */
    virtual QMargins weightBarMargins() const;

    /**
     * Retrieve widget drag lock.
     *
     * @returns  @c true if locked, @c false otherwise
     */
    virtual bool widgetDragLock() const {return widgetDragLock_;}

    /**
     * Retrieve widget settings lock.
     *
     * @returns  @c true if locked, @c false otherwise
     */
    virtual bool widgetSettingsLock() const {return widgetSettingsLock_;}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Activate the mode.
     *
     * This method is called whenever the mode is set active. Most modes will send a
     * @c SnBrowserMenuChanging message when this method is called.
     */
    virtual void activate();

    /**
     * Create cached screens.
     */
    virtual void createCachedScreens();

    /**
     * Create a menu panel object.
     *
     * @param[in] panelEx  extended panel type
     * @param[in] parent  panel parent dialog
     * @returns  pointer to panel
     */
    virtual PanelWidget *createMenuPanel( int panelEx, SessionDialog *parent );

    /**
     * Process forced input fields.
     */
    virtual void processForcedFields();

    /**
     * Process invalid access to field that requires it.
     *
     * @param widget[in]  pointer to widget that required access
     */
    virtual void processInvalidAccessLevel( QWidget *widget );

    /**
     * Toggle current language.
     */
    virtual void toggleCurrentLanguage();

    /**
     * Toggle primary language.
     */
    virtual void togglePrimaryLanguage();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    /**
     * Write product single field.
     *
     * @param[in] off  field offset
     * @param[in] newValue  new value
     * @param[in] oldValue  old value
     */
    virtual void writeSingleField( ProductWriteSingleFieldOffset off, const QString& newValue, const QString& oldValue );

    // second label applier
    int getPrimaryPlatePosition() const;
    int getSecondaryPlatePosition() const;
    int getRequiredPrimaryPlatePosition() const;
    int getRequiredSecondaryPlatePosition() const;

    int getPackageId() const;

public Q_SLOTS:

    /**
     * Handle flashkey action.
     *
     * @param[in] w  pointer to softkeys widget
     * @param[in] f  pointer to flashkey
     * @param[in] action  action
     * @param[in] param  action parameter
     * @param[in] feature  action feature
     */
    virtual void handleFlashkey( QWidget *w, QWidget *f, int action, const QString& param, int feature );

    /**
     * Hide the weight bar.
     */
    virtual void hideWeightBar();

    /**
     * Set capacity bar area.
     *
     * @param[in] value  capacity bar area
     */
    virtual void setCapacityBarArea( CapacityBarArea value );

    /**
     * Move focus to proper widget depending on popup number pad.
     */
    virtual void setFocusForPopupNumberPad();

    /**
     * Set weight bar area.
     *
     * @param[in] value  weight bar area
     */
    virtual void setWeightBarArea( WeightBarArea value );

    /**
     * Set the widget drag lock.
     *
     * @param[in] value  @c true to lock widget from dragging, @c false otherwise
     */
    virtual void setWidgetDragLock( bool value );

    /**
     * Set the widget settings lock.
     *
     * @param[in] value  @c true to lock widget settings, @c false otherwise
     */
    virtual void setWidgetSettingsLock( bool value );

    /**
     * Show the weight bar.
     */
    virtual void showWeightBar( WeightBarWidget::DisplayMode mode = WeightBarWidget::WeightDisplayMode );

    /**
    * Slot to handle hook shutdown keyboards.
     */
    virtual void shutdownKeyboards();

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve overlay margins.
     *
     * Returns margins of the capacity bar and weight bar (if visible).
     *
     * @returns  overlay margins
     */
    virtual QMargins overlayMargins() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Event filter.
     *
     * @param[in] obj  object being filtered
     * @param[in,out] e  event
     * @returns  @c true to filter event, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /**
     * Slot to handle delete of widgets.
     */
    void onDeleteWidgets();

    /**
     * Slot to handle hook field changes.
     */
    void onHookChangeFieldValue( int type, const QString& value );

    /**
     * Slot to handle hook language change.
     */
    void onHookChangeLanguage();

    /**
     * Slot to handle hook language change.
     */
    void onHookResetPrimaryLanguage();

    /**
     * Slot to handle hook zero weigher.
     */
    void onHookZeroWeigher();

    /**
     * Slot to handle operator logged in.
     */
    void onOperatorLoggedIn();

    /**
     * Slot to handle forced field processing.
     */
    void onProcessForcedFields();

    /**
     * Slot to reset waiting for cool flag.
     */
    void onResetWaitingOnCool();

    /**
     * Slot to reset waiting for item message flag.
     */
    void onResetWaitingOnItemMessage();

    /**
     * Bring back the main screen. I had to disable it to toggle the flashkey but just in case something goes sideways, reenable it
     */
    void onTimerToggleFlashkeyEnable();

private:

    enum ProductNoteStyle
    {
        NoButtons = 0,
        OkayButton = 1,
        YesNoButtons = 2
    };

    typedef QMap<ProductSupportType, DomPluSupport> DomPluSupportMap;

    DomCurrentPLU currentPLU_;
    MadpUpdateCurrentPLUTotals currentPLUTotals_;

    DomPluSupportMap currentPLUSupportMap_;

    bool waitingOnCool_;
    bool waitingOnItemMessage_;
    bool refreshBestSellers_;
    bool printingProductionEntry_;

    bool needToShowProductNote_;
    bool needToShowSuggestiveSell_;
    bool needToShowCookingInstructions_;

    int suggestiveSellCounter_;

    CapacityBarWidget *capacityBar_;
    WeightBarWidget *weightBar_;

    CapacityBarWidget *oldCapacityBar_;
    WeightBarWidget *oldWeightBar_;

    bool weightBarVisible_;

    bool isStableNonZeroWeight_;
    double minWeightToPrint_;

    bool widgetDragLock_;
    bool widgetSettingsLock_;

    QTimer *timerDeleteWidgets_;
    QTimer *timerProcessForcedFields_;
    QTimer *timerResetWaitingOnCool_;
    QTimer *timerResetWaitingOnItemMessage_;
    QTimer *timerToggleFlashkeyEnable_;

    QWidget *operatorLoggedInWidget_;

    bool selfServiceOperatorRequest_;
    bool selfServiceForceWriteSingleField_;

    int primaryPlatePosition_;
    int secondaryPlatePosition_;
    int requiredPrimaryPlatePosition_;
    int requiredSecondaryPlatePosition_;
    int packageId_;

    static const bool keepBarsTogether_;
    static const int missingCoolTimeout_;
    static const int missingItemMessageTimeout_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Update field in current plu.
     */
    void updateField( ProductWriteSingleFieldOffset off, const QString& value );

    /**
     * Load (update )softkey widgets.
     */
    void updateSoftkeyGroups();

    /**
     * Determine the flashkey location by Row and Column
     */
    void setToggledCoordinates(QWidget *w, QWidget *f, int feature, int action);

    /**
     * Disabling Add Extra Sell By Rows can only be done by the flashkey that turned it on and vice versa
     */
    bool isExtraDayAlreadyEnabled(int feature);

    /**
     * Show a product note in the dca area.
     */
    void showProductNote( ProductSupportType supportType );

    /**
     * Show printer settings.
     */
    void showPrinterSettings();

    /**
     * Setup capacity and weight bars.
     */
    void setupBars( CapacityBarArea capacityArea, WeightBarArea weightArea = TopWeightBarArea );

    /**
     * Create capacity bar.
     */
    CapacityBarWidget *createCapacityBar( CapacityBarArea area );

    /**
     * Create weight bar.
     */
    WeightBarWidget *createWeightBar( WeightBarArea area );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // OPERATEMODE_H
