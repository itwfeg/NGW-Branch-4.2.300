/**
 * @file flashkeyeditor.cpp
 *
 * @copyright Copyright (c) 2013 Hobart Corporation
 * @author Randy Blankley
 */
//#define USE_QDEBUG

#include <QMessageBox>
#include <filesystem.h>
#include <logmanager.h>
#include <UiUtilities.h>

#include "appsscreen.h"
#include "flashkeyeditor.h"
#include "screenwidget.h"

#include "../numberpadwidget.h"
#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../dialogs/colordialog.h"
#include "../dialogs/dcadialog.h"
#include "../dialogs/messageboxdialog.h"

#include "../modes/operatemode.h"

#include "../screens/plumainscreen.h"

#include "../widgets/dynamiccontentwidget.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <numericlineedit.h>
#include <palette.h>
#include <refusebin.h>
#include <softkeybutton.h>
#include <softkeybuttonlist.h>
#include <softkeyswidget.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <Session.h>

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalMapper>
#include <QSplitter>
#include <QTimer>
#include <QTranslator>
#include <QVBoxLayout>

static const int    NUMBER_OF_FEATURES = 12;    // how many features do we display. Bump up if we add another feature

///////////////////////////////////////////////////////////////////////////////////////////////////
FlashkeyEditor::FlashkeyEditor( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    isNewSoftkey_( false ),
    isMoveSoftkey_( false ),
    isMoveSoftkeySetVisible_( false ),
    isUpdateSoftkey_( false ),
    twoLabelAppliersAttached_( false )
{
    setObjectName( "FlashkeyEditor" );

    // check for two label appliers/printers
    if ( session_->scaleConfig->labelApplierConfig.secondaryStationInstalled() )
        twoLabelAppliersAttached_ = true;
    else if ( session_->scaleConfig->evolLabelApplierConfig.hasSecondPrinter() )
        twoLabelAppliersAttached_ = true;

    // connect slots
    if ( mainWindow_->isOperateMode() )
        connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );

    // initialize screen
    initialize();
    createLayout();

    // populate external applications
    launchExternalAppAction_->addItem( "", "" ); // none

    OperateMode *operateMode( mainWindow_->retrieveMode<OperateMode>() );
    AppsScreen *appsScreen( operateMode ? operateMode->isCached<AppsScreen>() : NULL );

    if ( appsScreen )
    {
        QStringList apps( appsScreen->apps() );

        foreach( const QString &app, apps )
            launchExternalAppAction_->addItem( app, app );
    }

    translate();

    // request list of softkey groups
    session_->send( SoftkeyDev, ReadAllSoftkeyGroups );

    // request list of softkey images
    session_->send( GraphicDev, ReadAllSoftkeyGraphicNames );

    // request list of videos
    session_->send( GraphicDev, ReadAllVideoNames );

    // request list of total types
    session_->send( TransactionTypeDev, TransactionReadNonVoidGroup );

    // request list of available script names
    session_->send( SoftkeyDev, ReadSoftkeyScripts );

    // request list of departments
    session_->send(DeptDev, DeptDevDtReadAll);

    // request best sellers
    bestSellers_->sendResetMessage( true );

    // load product grid
    load( QString::number( hobart::softkeysWidget::ProductGrid ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FlashkeyEditor::~FlashkeyEditor()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString FlashkeyEditor::currentFeature() const
{
    return QString::number( flashkeys_->feature() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString FlashkeyEditor::currentGroup() const
{
    return QString::number( flashkeys_->group() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setCurrentFeature( const QString& value )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::setCurrentFeature(" << value << ")";
#endif
    LOG_CLASS_INFO( "FlashkeyEditor", "setCurrentFeature" );

    // nothing to do
    if ( value.isEmpty() )
        return;
    else if ( currentFeature() == value )
        return;

    // make sure valid feature
    int index = feature_->findData( value );

    if ( index < 0 )
    {
        LOG_WARN( "invalid feature '%s'", qPrintable( value ) );
        return;
    }

    setCurrentFeatureInternal( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setCurrentGroup( const QString& value )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::setCurrentGroup(" << value << ")";
#endif
    // nothing to do
    if ( value.isEmpty() )
        return;
    else if ( currentGroup() == value )
        return;

    // cannot load a group unless we have a feature
    if ( currentFeature().isEmpty() )
        return;

    setCurrentGroupInternal( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::enableOperateFeatures( const QStringList& features )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::enableOperateFeatures(" << features << ")";
#endif
    // must be in operate mode
    if ( !mainWindow_->isOperateMode() )
        return;

    // sanity check
    if ( !features.count() )
        return;

keepRemovingStuff:

    // remove features not in list
    for ( int i = (feature_->count()-1); i >= 0; --i )
    {
        bool found = false;

        foreach ( const QString& f, features )
            found |= (f == feature_->itemData( i ).toString());

        if ( !found )
        {
            feature_->removeItem( i );

            goto keepRemovingStuff;
        }
    }

    // refresh mode
    showMode( GridEditMode );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::load( const QString& feature, const QString& group )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::load(" << feature << ", " << group << ")";
#endif
    LOG_CLASS_INFO( "FlashkeyEditor", "load" );

    // this is the group we are interested in
    actionSoftkeyGroup_ = group;

    // load!
    LOG_DEBUG( "load feature '%s' group '%s'", qPrintable( feature ), qPrintable( group ) );
    setCurrentFeature( feature );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::translate()
{
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    // ========================================================================
    // main pane widgets
    // ========================================================================

    featureLabel_->setText( tr( "Feature" ) );

    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::ProductGrid, tr( "Left panel flashkeys" ) );                             // 999001
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::RunScreenButtons, tr( "Center panel flashkeys" ) );                      // 999003
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::PresetTareGrid, tr( "Tare flashkeys" ) );                                // 999004
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::MostCommonlyUsedItems, tr( "Best Sellers flashkeys" ) );                 // new - 999007
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::SelfservCategoryResultsButtons, tr( "Self-serve category results" ) );   // new - 999017
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::SelfservProductGrid, tr( "Self-serve product grid" ) );                  // 999018
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::FixedRowColumnGrid, tr( "Fixed Flashkeys" ) );                           // 999020
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::RunScreenNumericKeypadButtons, tr( "Numeric keypad buttons" ) );         // 999030
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::BuildAMealScreenButtons, tr( "Meal Builder buttons" ) );                 // 999031
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::MeatSpecSheetScreeenButtons, tr( "Meat Specification Sheet buttons" ) ); // 999033
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::RunScreenCutTestOn, tr( "Default Cutting Test flashkeys" ) );            // 999015
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::RunScreenCutTestOnOptionOne, tr( "Option1 Cutting Test flashkeys" ) );   // 999035
    setComboBoxItemTextFromData( feature_, hobart::softkeysWidget::ForcedCameraOverride, tr( "Wrapper Camera Override Trays" ) );           // 999025

    groupLabel_->setText( tr( "Group Number" ) );

    actionLabel_->setText( tr( "Action" ) );

    // NOTE:
    // anything added here needs to be added to the list also (untranslated); see below
    // This is for the Combo Box on the Edit (create new) Flashkey screen
    setComboBoxItemTextFromData( action_, Action_NoAction,                           tr( "None" ) );
    setComboBoxItemTextFromData( action_, Action_PrintLabel,                         tr( "Print Label" ) );
    setComboBoxItemTextFromData( action_, Action_ChangeScreen,                       tr( "Change screen" ) );
    setComboBoxItemTextFromData( action_, Action_BrowseWeb,                          tr( "Browse Web" ) );
    setComboBoxItemTextFromData( action_, Action_ChangeLanguage,                     tr( "Change Language" ) );
    setComboBoxItemTextFromData( action_, Action_LoginOperator,                      tr( "Operator Login" ) );
    setComboBoxItemTextFromData( action_, Action_ResetPrice,                         tr( "Reset Price" ) );
    setComboBoxItemTextFromData( action_, Action_SetPlatterTare,                     tr( "Platter Tare" ) );
    setComboBoxItemTextFromData( action_, Action_SelectProduct,                      tr( "Select Product" ) );
    setComboBoxItemTextFromData( action_, Action_LoadSoftkeyGroup,                   tr( "Load Softkey Group" ) );
    setComboBoxItemTextFromData( action_, Action_LoadCategory_First,                 tr( "Load Category" ) );
    setComboBoxItemTextFromData( action_, Action_ApplyDiscount_CentsOff,             tr( "Cents Off Disc." ) );
    setComboBoxItemTextFromData( action_, Action_ApplyDiscount_PercentOff,           tr( "Percent Off Disc." ) );
    setComboBoxItemTextFromData( action_, Action_VoidTransaction,                    tr( "Void transaction" ) );
    setComboBoxItemTextFromData( action_, Action_LoadSpecials_First,                 tr( "Load Specials" ) );
    setComboBoxItemTextFromData( action_, Action_LoadSearch_First,                   tr( "Load Search" ) );
    setComboBoxItemTextFromData( action_, Action_PrintExistingExpirationLabel,       tr( "Print Existing Shelf Life Label" ) );
    setComboBoxItemTextFromData( action_, Action_FPP_URL,                            tr( "Production Planning" ) );
    setComboBoxItemTextFromData( action_, Action_Application_URL,                    tr( "Store Applications" ) );
    setComboBoxItemTextFromData( action_, Action_BeginCutTest,                       tr( "Start Cut Test" ) );
    setComboBoxItemTextFromData( action_, Action_CutTestHistory,                     tr( "Cutting Test History" ) );
    setComboBoxItemTextFromData( action_, Action_CutTestSuspend,                     tr( "Put Cutting Test on Hold" ) );
    setComboBoxItemTextFromData( action_, Action_EndCutTest,                         tr( "Stop Cut Test" ) );
    setComboBoxItemTextFromData( action_, Action_SelectAnyProduct,                   tr( "Request a Product" ) );
    setComboBoxItemTextFromData( action_, Action_PrintRunTotals,                     tr( "Print Run totals label" ) );
    setComboBoxItemTextFromData( action_, Action_SwitchTotalType,                    tr( "Switch Total Type" ) );
    setComboBoxItemTextFromData( action_, Action_PrintMultipleLabels,                tr( "Print Multiple Labels" ) );
    setComboBoxItemTextFromData( action_, Action_ToggleProductionEntryMode,          tr( "Toggle Production Entry Mode" ) );
    setComboBoxItemTextFromData( action_, Action_PrintNewExpirationLabel,            tr( "Print New Shelf Life Label" ) );
    setComboBoxItemTextFromData( action_, Action_LoadTareSoftkey,                    tr( "Preset Tare" ) );
    setComboBoxItemTextFromData( action_, Action_ChangeLabelPlacementAndPosition,    tr( "Label placement & rotation" ) );
    setComboBoxItemTextFromData( action_, Action_ServeNextCustomer,                  tr( "Serve Next Customer" ) );
    setComboBoxItemTextFromData( action_, Action_NowServingQueueManagement,          tr( "Queue Management" ) );
    setComboBoxItemTextFromData( action_, Action_ChangeLabelType,                    tr( "Change Label type" ) );
    setComboBoxItemTextFromData( action_, Action_LoadMostCommon_First,               tr( "Best Sellers" ) );
    setComboBoxItemTextFromData( action_, Action_ToggleAccumulateItemsMode,          tr( "Toggle Accumulate Items Mode" ) );
    setComboBoxItemTextFromData( action_, Action_AddItemToAccumulatedItems,          tr( "Add Item To Accumulated Items" ) );
    setComboBoxItemTextFromData( action_, Action_RemoveLastItemFromAccumulatedItems, tr( "Remove Last Item From Accum Items" ) );
    setComboBoxItemTextFromData( action_, Action_ShowAccumulatedItems,               tr( "Show Accumulated Items" ) );
    setComboBoxItemTextFromData( action_, Action_RemoveAllAccumulatedItems,          tr( "Remove All Accumulated Items" ) );
    setComboBoxItemTextFromData( action_, Action_PrintAccumulatedItems,              tr( "Print Accumulated Items" ) );
    setComboBoxItemTextFromData( action_, Action_LogoutOperator,                     tr( "Operator Logout" ) );
    setComboBoxItemTextFromData( action_, Action_PlayVideo,                          tr( "Play Video" ) );
    setComboBoxItemTextFromData( action_, Action_Portioning,                         tr( "Portion Zero" ) );
    setComboBoxItemTextFromData( action_, Action_LaunchExternalApp,                  tr( "Application Shortcut" ) );
    setComboBoxItemTextFromData( action_, Action_ToggleMode,                         tr( "Toggle Weigh Mode" ) );
    setComboBoxItemTextFromData( action_, Action_OpenDocumentFolder,                 tr( "Open Document" ) );
    if( session_->uiUtilities->isFeatureLicensed( SCALE_PICK5 ) )
        setComboBoxItemTextFromData( action_, Action_TogglePick5Active,              tr( "Toggle Pick 5 Active" ) );
    setComboBoxItemTextFromData( action_, Action_Swipe,                              tr( "Swipe Screen" ) );
    setComboBoxItemTextFromData( action_, Action_ToggleExtraShelfLifeDay,            tr( "Toggle Extra Sell By Day" ) );
    setComboBoxItemTextFromData( action_, Action_OverrideWrapperCameraDimensions,    tr( "Override Wrapper Camera Dimensions" ) );
	setComboBoxItemTextFromData( action_, Action_Toggle2ndLabelEnable,               tr( "2nd Label Enable/Disable" ) );
    setComboBoxItemTextFromData( action_, Action_ChangeDepartment,                   tr( "Change Department" ) );
    setComboBoxItemTextFromData( action_, Action_ToggleRfidPrinting,                 tr( "Toggle RFID Printing" ) );

    // NOTE:
    // anything added here needs to be added to the combo also (translated); see above
    actions_[Action_NoAction] = "None";
    actions_[Action_PrintLabel] = "Print Label";
    actions_[Action_ChangeScreen] = "Change screen";
    actions_[Action_BrowseWeb] = "Browse Web";
    actions_[Action_ChangeLanguage] = "Change Language";
    actions_[Action_LoginOperator] = "Operator Login";
    actions_[Action_ResetPrice] = "Reset Price";
    actions_[Action_SetPlatterTare] = "Platter Tare";
    actions_[Action_SelectProduct] = "Select Product";
    actions_[Action_LoadSoftkeyGroup] = "Load Softkey Group";
    actions_[Action_LoadCategory_First] = "Load Category";
    actions_[Action_ApplyDiscount_CentsOff] = "Cents Off Disc.";
    actions_[Action_ApplyDiscount_PercentOff] = "Percent Off Disc.";
    actions_[Action_VoidTransaction] = "Void transaction";
    actions_[Action_LoadSpecials_First] = "Load Specials";
    actions_[Action_LoadSearch_Next] = "Load Search";
    actions_[Action_PrintExistingExpirationLabel] = "Print Existing Shelf Life Label";
    actions_[Action_FPP_URL] = "Production Planning";
    actions_[Action_Application_URL] = "Store Applications";
    actions_[Action_BeginCutTest] = "Start Cut Test";
    actions_[Action_EndCutTest] = "Stop Cut Test";
    actions_[Action_CutTestHistory] = "Cutting Test History";
    actions_[Action_CutTestSuspend] = "Put Cutting Test on Hold";
    actions_[Action_SelectAnyProduct] = "Request a Product";
    actions_[Action_PrintRunTotals] = "Print Run totals label";
    actions_[Action_SwitchTotalType] = "Switch Total Type";
    actions_[Action_PrintMultipleLabels] = "Print Multiple Labels";
    actions_[Action_ToggleProductionEntryMode] = "Toggle Production Entry Mode";
    actions_[Action_PrintNewExpirationLabel] = "Print New Shelf Life Label";
    actions_[Action_LoadTareSoftkey] = "Preset tare";
    actions_[Action_ChangeLabelPlacementAndPosition] = "Label placement & rotation";
    actions_[Action_ServeNextCustomer] = "Serve Next Customer";
    actions_[Action_NowServingQueueManagement] = "Queue Management";
    actions_[Action_ChangeLabelType] = "Change Label type";
    actions_[Action_LoadMostCommon_First] = "Best Sellers";
    actions_[Action_ToggleAccumulateItemsMode] = "Toggle Accumulate Items Mode";
    actions_[Action_AddItemToAccumulatedItems] = "Add Item To Accumulated Items";
    actions_[Action_RemoveLastItemFromAccumulatedItems] = "Remove Last Item From Accumulated Items";
    actions_[Action_ShowAccumulatedItems] = "Show Accumulated Items";
    actions_[Action_RemoveAllAccumulatedItems] = "Remove All Accumulated Items";
    actions_[Action_PrintAccumulatedItems] = "Print Accumulated Items";
    actions_[Action_LogoutOperator] = "Operator Logout";
    actions_[Action_PlayVideo] = "Play Video";
    actions_[Action_Portioning] = "Portion Zero";
    actions_[Action_LaunchExternalApp] = "Application Shortcut";
    actions_[Action_ToggleMode] = "Toggle Mode";
    actions_[Action_OpenDocumentFolder] = "Open Document";
    if( session_->uiUtilities->isFeatureLicensed( SCALE_PICK5 ) )
        actions_[Action_TogglePick5Active] = "Toggle Pick 5 Active";
    actions_[Action_Swipe] = "Swipe Screen";
    actions_[Action_ToggleExtraShelfLifeDay] = "Toggle Extra Sell By Day";
    actions_[Action_OverrideWrapperCameraDimensions] = "Override Wrapper Camera Dimensions";
	actions_[Action_Toggle2ndLabelEnable] = "2nd Label is Enable/Disable";
    actions_[Action_ChangeDepartment] = "Change Department";
    actions_[Action_ToggleRfidPrinting] = "Toggle RFID Printing";

    setActionParamLabelText();

    // NOTE:
    // anything added here needs to be added to the list also (untranslated); see below
    setComboBoxItemTextFromData( screenChangeAction_, 0, tr( "Home Panel" ) );

    bool searchEnabled = scaleConfig->enableSearchMenuButton();
    if( searchEnabled )
        setComboBoxItemTextFromData( screenChangeAction_, 4, tr( "Search" ) );

    setComboBoxItemTextFromData( screenChangeAction_, 5, tr( "Operator Login" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 6, tr( "Label Data" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 7, tr( "Product and Shelf Life" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 8, tr( "Wrapper Settings" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 9, tr( "Printer Settings" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 10, tr( "Label placement & rotation" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 11, tr( "Advanced Wrapper Settings" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 12, tr( "Total Types" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 13, tr( "COOL" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 14, tr( "Expiration History" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 15, tr( "Void Transactions" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 16, tr( "Preset Tares" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 17, tr( "Show Accumulated Items" ) );    
    setComboBoxItemTextFromData( screenChangeAction_, 18, tr( "About Scale" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 19, tr( "Wrapper Tray Override" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 40+OperateMode::ExpandedText, tr( "Show Expanded Text" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 40+OperateMode::ExpandedText2, tr( "Show Expanded Text 2" ) ); // FT-381
    setComboBoxItemTextFromData( screenChangeAction_, 40+OperateMode::Recipe, tr( "Show Recipe" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 40+OperateMode::CookingInstructions, tr( "Show Cooking Instructions" ) ); // FT-379
    setComboBoxItemTextFromData( screenChangeAction_, 40+OperateMode::SuggestiveSell, tr( "Show Suggestive Sell" ) );
    setComboBoxItemTextFromData( screenChangeAction_, 40+OperateMode::Allergens, tr( "Show Allergens" ) );

    // NOTE:
    // anything added here needs to be added to the combo also (translated); see above
    screenChangeActions_[0] = "Home Panel";

    if( searchEnabled )
        screenChangeActions_[4] = "Search";

    screenChangeActions_[5] = "Operator Login";
    screenChangeActions_[6] = "Label Data";
    screenChangeActions_[7] = "Product and Shelf Life";
    screenChangeActions_[8] = "Wrapper Settings";
    screenChangeActions_[9] = "Printer Settings";
    screenChangeActions_[10] = "Label placement & rotation";
    screenChangeActions_[11] = "Advanced Wrapper Settings";
    screenChangeActions_[12] = "Total Types";
    screenChangeActions_[13] = "COOL";
    screenChangeActions_[14] = "Expiration History";
    screenChangeActions_[15] = "Void Transactions";
    screenChangeActions_[16] = "Preset Tares";
    screenChangeActions_[17] = "Show Accumulated Items";
    screenChangeActions_[40+OperateMode::ExpandedText] = "Show Expanded Text";
    screenChangeActions_[40+OperateMode::ExpandedText2] = "Show Expanded Text 2"; // FT-381
    screenChangeActions_[40+OperateMode::Recipe] = "Show Recipe";
    screenChangeActions_[40+OperateMode::CookingInstructions] = "Show Cooking Instructions"; // FT-379
    screenChangeActions_[40+OperateMode::SuggestiveSell] = "Show Suggestive Sell";
    screenChangeActions_[40+OperateMode::Allergens] = "Show Allergens";

    // external applications
    launchExternalAppAction_->setItemText( 0, tr( "None" ) );

    // ========================================================================
    // edit grid widgets
    // ========================================================================

    deleteGroup_->setText( tr( "Delete Group" ) );

    defaultGroupLabel_->setText( tr( "Default Group" ) );

    horzSpacingLabel_->setText( tr( "H. Spacing" ) );
    vertSpacingLabel_->setText( tr( "V. Spacing" ) );

    editCell_->setText( tr( "Edit Cell" ) );

    rowsLabel_->setText( tr( "Rows" ) );
    addRow_->setText( tr( "+" ) );
    removeRow_->setText( tr( "-" ) );

    columnsLabel_->setText( tr( "Columns" ) );
    addColumn_->setText( tr( "+" ) );
    removeColumn_->setText( tr( "-" ) );

    heightLabel_->setText( tr( "Row Span" ) );
    addHeight_->setText( tr( "+" ) );
    removeHeight_->setText( tr( "-" ) );

    widthLabel_->setText( tr( "Column Span" ) );
    addWidth_->setText( tr( "+" ) );
    removeWidth_->setText( tr( "-" ) );

    createNewCell_->setText( tr( "Create New Cell" ) );

    // PJR-383
    fixedRowColumnLabel_->setText( tr( "Fixed:") );
    setComboBoxItemTextFromData( fixedRowColumn_, hobart::softkeysWidget::FRCM_None, tr( "None" ) );
    setComboBoxItemTextFromData( fixedRowColumn_, hobart::softkeysWidget::FRCM_TopRow, tr( "Top Row" ) );
    setComboBoxItemTextFromData( fixedRowColumn_, hobart::softkeysWidget::FRCM_FirstColumn, tr( "First Column" ) );
    setComboBoxItemTextFromData( fixedRowColumn_, hobart::softkeysWidget::FRCM_BottomRow, tr( "Bottom Row" ) );
    setComboBoxItemTextFromData( fixedRowColumn_, hobart::softkeysWidget::FRCM_LastColumn, tr( "Last Column" ) );

    fixedRowColumnDefaultButtonLabel_->setText( tr( "Default Button:") );

    // ========================================================================
    // edit cell widgets
    // ========================================================================

    labelPlacement1Label_->setText( tr( "Primary Placement" ) );

    setComboBoxItemTextFromData( labelPlacement1_, "2", tr( "No Change" ) );

    if ( !twoLabelAppliersAttached_ )
    {
        setComboBoxItemTextFromData( labelPlacement1_, "0", tr( "Right Side" ) );
        setComboBoxItemTextFromData( labelPlacement1_, "1", tr( "Left Side" ) );
    }
    else
    {
        setComboBoxItemTextFromData( labelPlacement1_, "0", tr( "Upper Right Side" ) );
        setComboBoxItemTextFromData( labelPlacement1_, "1", tr( "Upper Left Side" ) );
        setComboBoxItemTextFromData( labelPlacement1_, "3", tr( "Lower Right Side" ) );
        setComboBoxItemTextFromData( labelPlacement1_, "4", tr( "Lower Left Side" ) );
    }

    labelRotation1Label_->setText( tr( "Primary Rotation" ) );

    setComboBoxItemTextFromData( labelRotation1_, "0", tr( "No Change" ) );
    setComboBoxItemTextFromData( labelRotation1_, "1", QStringLiteral( "0°" ) );
    setComboBoxItemTextFromData( labelRotation1_, "2", QStringLiteral( "45°" ) );
    setComboBoxItemTextFromData( labelRotation1_, "3", QStringLiteral( "90°" ) );
    setComboBoxItemTextFromData( labelRotation1_, "4", QStringLiteral( "135°" ) );
    setComboBoxItemTextFromData( labelRotation1_, "5", QStringLiteral( "180°" ) );
    setComboBoxItemTextFromData( labelRotation1_, "6", QStringLiteral( "225°" ) );
    setComboBoxItemTextFromData( labelRotation1_, "7", QStringLiteral( "270°" ) );
    setComboBoxItemTextFromData( labelRotation1_, "8", QStringLiteral( "315°" ) );

    labelPlacement2Label_->setText( tr( "Second Placement" ) );

    setComboBoxItemTextFromData( labelPlacement2_, "2", tr( "No Change" ) );
    setComboBoxItemTextFromData( labelPlacement2_, "0", tr( "Upper Right Side" ) );
    setComboBoxItemTextFromData( labelPlacement2_, "1", tr( "Upper Left Side" ) );
    setComboBoxItemTextFromData( labelPlacement2_, "3", tr( "Lower Right Side" ) );
    setComboBoxItemTextFromData( labelPlacement2_, "4", tr( "Lower Left Side" ) );
    setComboBoxItemTextFromData( labelPlacement2_, "5", tr( "Right of Primary" ) );
    setComboBoxItemTextFromData( labelPlacement2_, "6", tr( "Left of Primary" ) );
    setComboBoxItemTextFromData( labelPlacement2_, "7", tr( "Above Primary" ) );
    setComboBoxItemTextFromData( labelPlacement2_, "8", tr( "Below Primary" ) );

    labelRotation2Label_->setText( tr( "Second Rotation" ) );

    setComboBoxItemTextFromData( labelRotation2_, "0", tr( "No Change" ) );
    setComboBoxItemTextFromData( labelRotation2_, "1", QStringLiteral( "0°" ) );
    setComboBoxItemTextFromData( labelRotation2_, "2", QStringLiteral( "45°" ) );
    setComboBoxItemTextFromData( labelRotation2_, "3", QStringLiteral( "90°" ) );
    setComboBoxItemTextFromData( labelRotation2_, "4", QStringLiteral( "135°" ) );
    setComboBoxItemTextFromData( labelRotation2_, "5", QStringLiteral( "180°" ) );
    setComboBoxItemTextFromData( labelRotation2_, "6", QStringLiteral( "225°" ) );
    setComboBoxItemTextFromData( labelRotation2_, "7", QStringLiteral( "270°" ) );
    setComboBoxItemTextFromData( labelRotation2_, "8", QStringLiteral( "315°" ) );

    printerLabel_->setText( tr( "Printer" ) );

    setComboBoxItemTextFromData( printer_, "0", tr( "Primary" ) );
    setComboBoxItemTextFromData( printer_, "1", tr( "Second" ) );
    setComboBoxItemTextFromData( printer_, "2", tr( "Third" ) );

    webPageParameters_->setTitle( tr( "CGI Parameters" ) );
    webPageUsesStoreNumber_->setText( "store" );
    webPageUsesDepartment_->setText( "dept" );
    webPageUsesPlu_->setText( "plu" );
    webPageUsesQty_->setText( "qty" );
    webPageUsesLang_->setText( "lang" );
    webPageUsesModel_->setText( "model" );
    webPageUsesAll_->setText( tr( "All" ) );
    webPageUsesNone_->setText( tr( "None" ) );

    trayLengthLabel_->setText( tr( "Tray Length (in)" ) );
    trayWidthLabel_->setText( tr( "Tray Width (in)" ) );
    trayLipHeightLabel_->setText( tr( "Tray Lip Height (in)" ) );

    displayLocationLabel_->setText( tr( "Display Results Where?" ) );

    setComboBoxItemTextFromData( displayLocation_, 0, tr( "Default" ) );
    setComboBoxItemTextFromData( displayLocation_, hobart::softkeysWidget::ProductGrid, tr( "Left panel flashkeys" ) );
    setComboBoxItemTextFromData( displayLocation_, hobart::softkeysWidget::RunScreenButtons, tr( "Center panel flashkeys" ) );
    setComboBoxItemTextFromData( displayLocation_, hobart::softkeysWidget::CategoryButtons, tr( "Programmable buttons" ) );
    setComboBoxItemTextFromData( displayLocation_, hobart::softkeysWidget::MostCommonlyUsedItems, tr( "Best Sellers flashkeys" ) );
    setComboBoxItemTextFromData( displayLocation_, hobart::softkeysWidget::SelfservCategoryButtons, tr( "Category buttons" ) );
    setComboBoxItemTextFromData( displayLocation_, hobart::softkeysWidget::SelfservProductGrid, tr( "Self-serve Product Grid" ) );                  // 999018
    setComboBoxItemTextFromData( displayLocation_, hobart::softkeysWidget::FixedRowColumnGrid, tr( "Fixed Flashkeys" ) );                            // 999020
    setComboBoxItemTextFromData( displayLocation_, hobart::softkeysWidget::RunScreenNumericKeypadButtons, tr( "Numeric keypad buttons" ) );
    setComboBoxItemTextFromData( displayLocation_, hobart::softkeysWidget::BuildAMealScreenButtons, tr( "Meal Builder buttons" ) );
    setComboBoxItemTextFromData( displayLocation_, hobart::softkeysWidget::MeatSpecSheetScreeenButtons, tr( "Meat Specification Sheet buttons" ) );  // 999033

    imageLabel_->setText( tr( "Softkey Image" ) );

    pyScriptLabel_->setText( tr( "Python Script Name" ) );

    pyScriptIntervalLabel_->setText( tr( "Python Script Interval (seconds)" ) );

    setComboBoxItemTextFromData( image_, 0, tr( "None" ) );
    setComboBoxItemTextFromData( image_, 1, tr( "Default" ) );

    setComboBoxItemTextFromData( pyScript_, 0, tr( "None" ) );

    // PJR-383
    toggledImageLabel_->setText( tr( "Toggled Softkey Image" ) );
    setComboBoxItemTextFromData( toggledImage_, 0, tr( "None" ) );
    setComboBoxItemTextFromData( toggledImage_, 1, tr( "Default" ) );

    backgroundColorLabel_->setText( tr( "Background Color" ) );

    setColorComboBoxItemText( backgroundColor_ );

    textColorLabel_->setText( tr( "Text Color" ) );

    setColorComboBoxItemText( textColor_ );

    textAlignmentLabel_->setText( tr( "Text Alignment" ) );

    textAlignment_->setItemText( 0, tr( "Top" ) );
    textAlignment_->setItemText( 1, tr( "Center" ) );
    textAlignment_->setItemText( 2, tr( "Bottom" ) );

    primaryTextLabel_->setText( tr( "Primary Text" ) );
    secondaryTextLabel_->setText( tr( "Secondary Text" ) );

    cancelCellEdit_->setText( tr( "Cancel" ) );
    cellEditDone_->setText( tr( "Done" ) );

    // ========================================================================
    // new cell widgets
    // ========================================================================

    for ( int i = 1; i < action_->count(); ++i )
    {
        hobart::softkeyButton *b( actionList_->softkeyButtonItem( i-1 ) );

        if ( !b )
            continue;

        b->setText( action_->itemText( i ) );
    }

    cancelCellCreate_->setText( tr( "Cancel" ) );

    // ========================================================================
    // operate mode widgets
    // ========================================================================

    enableBestSellerList_->setText( tr( "Show Best Sellers" ) );

    operateBackgroundColorLabel_->setText( tr( "Background Color" ) );

    setComboBoxItemTextFromData( operateBackgroundColor_, 0, tr( "Default" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "orangeSolid.png", tr( "Orange Solid" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "orangeGlass.png", tr( "Orange Glass" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "chartreuseSolid.png", tr( "Chartreuse Solid" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "chartreuseGlass.png", tr( "Chartreuse Glass" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "greenSolid.png", tr( "Green Solid" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "greenGlass.png", tr( "Green Glass" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "paleGreenSolid.png", tr( "Pale Green Solid" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "paleGreenGlass.png", tr( "Pale Green Glass" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "skyBlueSolid.png", tr( "Sky Blue Solid" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "skyBlueGlass.png", tr( "Sky Blue Glass" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "blueSolid.png", tr( "Blue Solid" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "blueGlass.png", tr( "Blue Glass" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "purpleSolid.png", tr( "Purple Solid" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "purpleGlass.png", tr( "Purple Glass" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "pinkSolid.png", tr( "Pink Solid" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "pinkGlass.png", tr( "Pink Glass" ) );
    setComboBoxItemTextFromData( operateBackgroundColor_, "custom", tr( "Custom..." ) );

    for ( int i = 1; i < action_->count(); ++i )
    {
        hobart::softkeyButton *b( operateActionList_->softkeyButtonItem( i-1 ) );

        if ( !b )
            continue;

        b->setText( action_->itemText( i ) );
    }

	// FT-298/FT-361 Flashkey text doesn't fit in combos
    QFont newFont = action_->font();
    if("EN.US" == mainWindow_->language() )
        newFont.setPointSize(14);
    else
        newFont.setPointSize(12);

    action_->setFont(newFont);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setCurrentFeatureInternal( const QString& value )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::setCurrentFeatureInternal(" << value << ")";
#endif
    LOG_CLASS_INFO( "FlashkeyEditor", "setCurrentFeatureInternal" );

    // set widget visibility
    showMode( GridEditMode );

    // disable screen until we have data
    setScreenEnabled( false );

    // create filter for feature we are interested in
    flashkeys_->setCreateFilter( (hobart::softkeysWidget::GroupType) value.toInt() );

    LOG_INFO( "reading feature '%s'", qPrintable( value ) );
    readSoftkeyFeature( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setCurrentGroupInternal( const QString& value )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::setCurrentGroupInternal(" << value << ")";
#endif
    LOG_CLASS_INFO( "FlashkeyEditor", "setCurrentGroupInternal" );

    // set widget visibility
    showMode( GridEditMode );

    // disable screen until we have data
    setScreenEnabled( false );

    LOG_INFO( "reading group '%s'", qPrintable( value ) );
    readSoftkeyGroup( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool FlashkeyEditor::onScreenChanging()
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::onScreenChanging()";
#endif
    MadpInternalConfig *internalConfig( &session_->scaleConfig->internalConfig );

    LOG_CLASS_INFO("FlashkeyEditor", "onScreenChanging");
    // reload best seller configuration
    if ( mainWindow_->isOperateMode() )
        session_->send( ConfigurationDev, CfgPrepackRead );

    // reload flashkeys in operate mode
    internalConfig->updateOperateFlashkeyData = true;
    session_->scaleConfig->showToggled = true;

    // redrawing the flashkeys so reset Toogle Extra Day available. Something may have changed.
    session_->scaleConfig->extraShelfLifeDayAvail = false;

    LOG_DEBUG("activate mode (browser)", NULL);
    if ( mainWindow_->isOperateMode() )
        mode_->activate();
    LOG_DEBUG("activate mode (browser) complete", NULL);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "FlashkeyEditor", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();
#ifdef USE_QDEBUG
    if ((device != 100) && (device != 105) && (device != 145) && (device != 170))
        qDebug() << "FlashkeyEditor::onSessionMessage():  " << device << ", " << event;
#endif

    // (136,22) write softkey
    if (( SoftkeyDev == device ) && ( WriteSoftkey == event ))
    {
        DomSoftkey m( msg );

        // ignore softkeys we are not interested in
        if ( currentGroup() == m.groupNumRaw() )
        {
            // find softkey
            hobart::softkeyButton *flashkey( flashkeys_->at( QPoint( m.hPosition(), m.vPosition() ) ) );

            // moving a flashkey
            if ( isMoveSoftkey_ )
            {
#ifdef USE_QDEBUG
        qDebug() << "FlashkeyEditor::onSessionMessage():  handling move of flashkey " << m.hPosition() << ", " << m.vPosition();
#endif
                LOG_INFO( "handling move of flashkey %d %d", m.hPosition(), m.vPosition() );

                MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

                // delete the softkey
                deleteSoftkey( QPoint( m.hPosition(), m.vPosition() ) );

                // create it again at new location
                m.event() = QString::number( CreateNewSoftkey );
                m.setHPosition( moveSoftkeyGridPos_.x() );
                m.setVPosition( moveSoftkeyGridPos_.y() );

                // FT-934 these fields should be quoted
                m.actionStrParamRaw() = "\"" + m.actionStrParam() + "\"";
                m.name1Raw() = "\"" + m.name1() + "\"";
                m.defName1Raw() = "\"" + m.defName1() + "\"";
                m.name2Raw() = "\"" + m.name2() + "\"";
                m.defName2Raw() = "\"" + m.defName2() + "\"";

                // Preset Tare (Avoir)
                if (( Action_LoadTareSoftkey == m.action() ) && ( !scaleConfig->isScaleMetric() ))
                {
                    // special case for 2 digit tare precision, we need to fix the decimal places

                    // HT-193 the number of decimal places will be strictly based on the Scale Tare Precision, not weigher models, etc.
                    int tarePrecision = qMax( 2, scaleConfig->tarePrecision() );

                    if ( 2 == tarePrecision )
                    {
                        double tare( m.actionParam().toDouble() );
                        tare *= 10.0;

                        m.actionParamRaw() = QString::number( tare );
                    }
                }

                isMoveSoftkeySetVisible_ = true;
                session_->send( m );

                // enable screen
                setScreenEnabled( true );

                // reset flag
                isMoveSoftkey_ = false;
            }

            // validate flashkey exists
            else if ( !flashkey )
                LOG_WARN( "flashkey not found %d %d", m.hPosition(), m.vPosition() );

            // show moved flashkey
            else if ( isMoveSoftkeySetVisible_ )
            {
                LOG_DEBUG( "setting flashkey visible", NULL );

                // set visibility
                flashkey->setVisible( true );

                // reset flag
                isMoveSoftkeySetVisible_ = false;
            }

            // all other actions
            else
            {
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::onSessionMessage():  All other actions.";
#endif
                // script interval
                pyScriptInterval_->blockSignals( true );
                pyScriptInterval_->fromInt( m.scriptInterval()/1000 );
                pyScriptInterval_->blockSignals( false );

                // create new softkey from message
                if (( isNewSoftkey_ ) || ( isUpdateSoftkey_ ))
                {
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::onSessionMessage():  Creating flashkey " << m.hPosition() << ", " << m.vPosition() << " from message.";
#endif
                    LOG_INFO( "creating flashkey %d %d from message", m.hPosition(), m.vPosition() );
                    flashkey->create( session_->scaleConfig->currentLanguage, &m, &style_ );
                }

                // select the flashkey
                flashkeys_->setSelected( flashkey );

                setCurrentSoftkey( selected_ = m );

                // enable screen
                setScreenEnabled( true );

                // create new softkey action
                if ( isNewSoftkey_ )
                {
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::onSessionMessage():  Editing new flashkey " << m.hPosition() << ", " << m.vPosition();
#endif
                    LOG_INFO( "editing flashkey %d %d", m.hPosition(), m.vPosition() );

                    // pretend they clicked the edit cell button
                    onClicked( editCell_ );

                    // pretend they selected an action from the list
                    action_->setCurrentIndex( action_->findData( QString::number( newSoftkeyAction_ ) ) );

                    // for operate mode *and* for flashkey with no parameters
                    if ( mainWindow_->isOperateMode() )
                        if (( !actionParam_->isVisible() ) &&
                            ( !actionParamNumeric_->isVisible() ) &&
                            ( !screenChangeAction_->isVisible() ) &&
                            ( !playVideoAction_->isVisible() ) &&
                            ( !swipeAction_->isVisible() ) &&
                            ( !totalTypesAction_->isVisible() ) &&
                            ( !launchExternalAppAction_->isVisible() ))
                        {
                            // there is nothing to do, pretend they saved the key
                            onClicked( cellEditDone_ );
                        }
                }

                // show fields
                else if ( isUpdateSoftkey_ )
                {
                    int action = selected_.action();

#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::onSessionMessage():  Updating flashkey " << m.hPosition() << ", " << m.vPosition();
#endif
                    LOG_INFO( "update of flashkey %d %d", m.hPosition(), m.vPosition() );

                    // show relevent fields
                    showActionFields( action, false );

                    // special case for preset tares... need to set description manually...
                    // should not the service do this?!?
                    if ( Action_LoadTareSoftkey == action )
                        setCurrentSoftkeyDefaultDescription( flashkey );
                }

                // reset flags
                isNewSoftkey_ = false;
                isUpdateSoftkey_ = false;
            }
        }
    }

    // (136,42) write softkey feature
    else if (( SoftkeyDev == device ) && ( WriteSoftkeyFeature == event ))
    {
        DomSoftkeyFeature m( msg );

        // check if our flashkeys
        if ( flashkeys_->createFilter() == m.featureNum() )
        {
            // set default group number
            defaultGroup_->blockSignals( true );
            defaultGroup_->fromInt( m.defaultGroupNum() );
            defaultGroup_->blockSignals( false );

            // save off feature in case group does not exist
            featureDefaultGroupNum_ = QString::number( m.defaultGroupNum() );
            featureGridSize_.setWidth( m.horizGridSize() );
            featureGridSize_.setHeight( m.vertGridSize() );
            featureHorizSpacing_ = m.horizSpacing();
            featureVertSpacing_ = m.vertSpacing();
            featureFixedRowColumn_ = m.fixedRowColMethod();
            featureDefaultButton_ = m.defaultButton();

            // set Fixed Row/Col Method
            fixedRowColumn_->blockSignals( true );
            fixedRowColumn_->setCurrentIndex( m.fixedRowColMethod() );
            fixedRowColumn_->blockSignals( false );

            // set Fixed Row/Col Default Button
            fixedRowColumnDefault_->blockSignals( true );
            fixedRowColumnDefault_->setText(QString::number(m.defaultButton() ) );
            fixedRowColumnDefault_->blockSignals( false );

            int index = feature_->currentIndex();
            int itemData( feature_->itemData( index ).toInt() );

            if( (itemData == hobart::softkeysWidget::MostCommonlyUsedItems) ||
                    (itemData == hobart::softkeysWidget::SelfservCategoryResultsButtons) )
            {
                createNewCell_->setEnabled(false);
            }
            else
            {
                createNewCell_->setEnabled(true);
            }

            // PJR-383
            if( itemData == hobart::softkeysWidget::FixedRowColumnGrid )
            {
                fixedRowColumnLabel_->show();
                fixedRowColumn_->show();

                fixedRowColumnDefaultButtonLabel_->show();
                fixedRowColumnDefault_->show();
            }

            // read group
            readSoftkeyGroup( actionSoftkeyGroup_.isEmpty() ? defaultGroup_->text() : actionSoftkeyGroup_ );
            actionSoftkeyGroup_.clear();
        }
    }

    // (136,62) write softkey style
    else if (( SoftkeyDev == device ) && ( WriteSoftkeyStyle == event ))
    {
        DomSoftkeyStyle m( msg );

        // save for later use
        style_ = m;
    }

    // (136,82) write softkey group
    else if (( SoftkeyDev == device ) && ( WriteSoftkeyGroup == event ))
    {
        DomSoftkeyGroup m( msg );

        // load softkeys
        loadSoftkeyGroup( QString::number( flashkeys_->createFilter() ), QString::number( m.groupNum() ) );
    }

    // (136,83) write all softkey groups
    else if (( SoftkeyDev == device ) && ( WriteAllSoftkeyGroups == event ))
    {
        DomWriteAllSoftkeyGroups m( msg );

        integerList groups( m.groupList() );

        // validate
        if ( m.numGroups() != groups.count() )
            LOG_WARN( "group count mismatch %d %d", m.numGroups(), groups.count() );

        int count = qMin( m.numGroups(), groups.count() );

        // populate combo with groups
        while ( 0 < count-- )
        {
            // TODO
        }
    }

    // (136,93) load softkey group
    else if (( SoftkeyDev == device ) && ( LoadSoftkeyGroup == event ))
    {
        DomLoadSoftkeyGroup m( msg );

        LOG_DEBUG( "loading flashkeys...", NULL );

        dynamicContent_->setFlashkeys( &m );

        // check if our flashkeys
        if ( flashkeys_->createFilter() == m.featureNum() )
        {
            hobart::dynamicContentWidget::WidgetType flashkeyBankIndex = hobart::dynamicContentWidget::Flashkeys;
            int displayFieldType = 0;

            // set feature number
            feature_->blockSignals( true );
            feature_->setCurrentIndex( feature_->findData( currentFeature() ) );
            feature_->blockSignals( false );

            // set group number
            group_->blockSignals( true );
            group_->fromInt( flashkeys_->group() );
            group_->blockSignals( false );

            // set horz spacing
            horzSpacing_->blockSignals( true );
            horzSpacing_->fromInt( flashkeys_->hSpacing() );
            horzSpacing_->blockSignals( false );

            // set vert spacing
            vertSpacing_->blockSignals( true );
            vertSpacing_->fromInt( flashkeys_->vSpacing() );
            vertSpacing_->blockSignals( false );

            // set grid type (flashkeys vs. best seller list)
            if ( hobart::softkeysWidget::ProductGrid == m.featureNum() )
                displayFieldType = session_->scaleConfig->operatorDisplayFields.dcaStartScreenType();
            else if ( hobart::softkeysWidget::RunScreenButtons == m.featureNum() )
                displayFieldType = session_->scaleConfig->operatorDisplayFields.dcaRunScreenType();
            else if ( hobart::softkeysWidget::RunScreenNumericKeypadButtons == m.featureNum() )
                displayFieldType = session_->scaleConfig->operatorDisplayFields.dcaKeypadAreaType();
            else if ( hobart::softkeysWidget::SelfservCategoryButtons == m.featureNum() )
                displayFieldType = session_->scaleConfig->operatorDisplayFields.dcaStartScreenType();

            if ( displayFieldType )
                flashkeyBankIndex = (hobart::dynamicContentWidget::WidgetType) displayFieldType;

            enableBestSellerList_->blockSignals( true );
            enableBestSellerList_->setChecked( hobart::dynamicContentWidget::BestSellerList == flashkeyBankIndex );
            enableBestSellerList_->blockSignals( false );

            if ( mainWindow_->isOperateMode() )
                dynamicContent_->setCurrentIndex( flashkeyBankIndex );

            // set grid size
            if (( hobart::softkeysWidget::RunScreenButtons == m.featureNum() ) ||
                ( hobart::softkeysWidget::PresetTareGrid == m.featureNum() ) ||
                ( hobart::softkeysWidget::RunScreenCutTestOn == m.featureNum() ) ||
                ( hobart::softkeysWidget::RunScreenCutTestOnOptionOne == m.featureNum() ) ||
                ( hobart::softkeysWidget::ForcedCameraOverride == m.featureNum() ))
            {
                ModeWidget *mode( mainWindow_->retrieveMode<OperateMode>() );
                PluMainScreen *pluMainScreen( mode ? mode->isCached<PluMainScreen>() : NULL );
                QSize s;

                if ( pluMainScreen )
                {
                    const QRect geometry( pluMainScreen->dcaGeometry() );
                    s = geometry.size();
                }

                if (( !s.isValid() ) ||
                    ( s.height() < (mainWindow_->height() / 2) ) ||
                    ( s.width() < (mainWindow_->width() / 2) ))
                {
                    dynamicContent_->setMinimumSize( 0, 0 );
                    dynamicContent_->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
                }
                else
                {
                    dynamicContent_->setFixedSize( s );
                }
            }
            else if ( hobart::softkeysWidget::RunScreenNumericKeypadButtons == m.featureNum() )
            {
                dynamicContent_->setFixedSize( mainWindow_->numberPad()->size() );
            }
            else
            {
                dynamicContent_->setMinimumSize( 0, 0 );
                dynamicContent_->setMaximumSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
            }

            // enable screen
            setScreenEnabled( true );

            int index = feature_->currentIndex();
            int itemData( feature_->itemData( index ).toInt() );

            if( (itemData == hobart::softkeysWidget::MostCommonlyUsedItems) ||
                    (itemData == hobart::softkeysWidget::SelfservCategoryResultsButtons) )
            {
                createNewCell_->setEnabled(false);
            }
            else
            {
                createNewCell_->setEnabled(true);
            }

            // read style
            readSoftkeyStyle( currentFeature() );
        }
    }

    // (103,136) sofkey error
    else if (( Error == device ) && ( ErrSoftkeyDevice == event ))
    {
        returnedErrorsSoftkeyDev m( msg );

        // must enter data to continue
        if (( 136012 == m.errorCode() ) ||
            ( 136013 == m.errorCode() ) ||
            ( 136014 == m.errorCode() ))
        {
            // set focus to field required, allowing some time for dialog processing
            setFocusActionParam( 250 );
        }

        // feature not found
        else if ( 136201 == m.errorCode() )
        {
            // enable feature selection so they can choose different one
            featureLabel_->setEnabled( true );
            feature_->setEnabled( true );
        }

        // create new softkey group message
        else if ( 136601 == m.errorCode() )
        {
            // operator said NO to create
            if ( returnedErrorsVersionManagerDev::ActionNo == m.errorData2() )
            {
                // load previous group
                setCurrentGroupInternal( currentGroup() );
            }

            // operator said YES to create
            else if ( returnedErrorsVersionManagerDev::ActionYes == m.errorData2() )
            {
                // create!
                createNewSoftkeyGroup( actionSoftkeyGroup_ );
                actionSoftkeyGroup_.clear();
            }
        }

        // delete softkey group message
        else if ( 136801 == m.errorCode() )
        {
            // operator said YES to delete question
            if ( returnedErrorsVersionManagerDev::ActionYes == m.errorData2() )
            {
                // delete!
                deleteSoftkeyGroup( actionSoftkeyGroup_ );
                actionSoftkeyGroup_.clear();

                setCurrentFeatureInternal( currentFeature() );
            }
        }

        // delete row
        // delete column
        else if (( 136605 == m.errorCode() ) || ( 136607 == m.errorCode() ))
        {
            // operator said YES to delete question
            if ( returnedErrorsVersionManagerDev::ActionYes == m.errorData2() )
            {
                QSize s( flashkeys_->gridSize() );

                if ( 136607 == m.errorCode() )
                    s.rwidth() -= 1;
                else if ( 136605 == m.errorCode() )
                    s.rheight() -= 1;

                LOG_TRACE( "new grid size %d %d", s.width(), s.height() );
                flashkeys_->setGridSize( s );

                // save changes
                saveSoftkeyGroup();
            }
        }
    }

    // (105,155) softkey graphic names
    else if (( GraphicDev == device ) && ( ReadAllSoftkeyGraphicNames == event ))
    {
        DomGraphicNameListSoftkeys m( msg );

        QStringList graphicList( m.field<QStringList>( m.graphicList() ) );

        if ( m.numOfItems() != graphicList.count() )
            LOG_WARN( "mismatch in graphic count %d %d", m.numOfItems(), graphicList.count() );

        foreach ( const QString& graphic, graphicList )
        {
            LOG_DEBUG( "found flashkey image '%s'", qPrintable( graphic ) );
            image_->addItem( graphic, graphic );

            // PJR-383: Also populate the combo box for the "Toggled Softkey Image" (i.e. depressed images)
            toggledImage_->addItem( graphic, graphic );
        }
    }

    else if ((DeptDev == device) && (DeptDevDtWriteAll == event ))
    {
        DomDepartments m (msg);

        const QStringList deptsArray( m.deptsList().split(",") );
        const QStringList name1Array( m.names1().split(",") );
        const QStringList name2Array( m.names2().split(",") );

        const int language = session_->scaleConfig->currentLanguage;

        for (int i = 0; i < deptsArray.size(); i++)
        {
            if(deptsArray.size() == name1Array.size() && deptsArray.size() == name2Array.size())
            {

                if(1 == language)
                {
                    departments_->addItem( name1Array[i], QVariant(deptsArray[i]) );

                }
                else
                {
                     departments_->addItem( name2Array[i], QVariant(deptsArray[i]) );

                }
            }
        }
    }

    // (105,195) video names
    else if (( GraphicDev == device ) && ( ReadAllVideoNames == event ))
    {
        DomGraphicNameListVideos m( msg );

        QStringList graphicList( m.field<QStringList>( m.graphicList() ) );

        if ( m.numOfItems() != graphicList.count() )
            LOG_WARN( "mismatch in video count %d %d", m.numOfItems(), graphicList.count() );

        foreach ( const QString& graphic, graphicList )
        {
            LOG_DEBUG( "found video '%s'", qPrintable( graphic ) );
            playVideoAction_->addItem( graphic, graphic );
        }
    }

    // (152,21) total types
    else if (( TransactionTypeDev == device ) && ( TransactionTypeWriteAllNames == event ))
    {
        DomTotalTypesList m( msg );

        QVector<QString> ids;
        QVector<QString> names;

        // parse total types
        ids = TokenParser::splitStringTokens( m.allRecordIds() );
        names = TokenParser::splitStringTokens( (2 == session_->scaleConfig->currentLanguage) ? m.secNamesList() : m.namesList() );

        int count = qMin( ids.count(), names.count() );

        // clear the list
        totalTypesAction_->clear();

        // process list
        for ( int i = 0; i < count; ++i )
            totalTypesAction_->addItem( names[i], ids[i] );
    }

    // (136,37)
    else if (( SoftkeyDev == device ) && ( ListSoftkeyScripts == event ) )
    {

        ListSoftkeyScriptsMsg m( msg );

        QStringList scriptStrList = m.scriptList().split( "," );
        scriptStrList.removeAll( "" );

        if ( scriptStrList.length() > 0 )
        {
            int tracker = 0;

            foreach ( QString script, scriptStrList )
            {
                ++tracker;
                pyScript_->addItem( script, script );
            }


        }
    }
    _Mybase::onSessionMessage( msg );

}
///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::resizeEvent( QResizeEvent *e )
{
    // set background for operate mode
    if ( mainWindow_->isOperateMode() )
    {
        QLinearGradient g( 0, 0, 0, height() );
        g.setColorAt( 0.0, hobart::gradientColors::lightOperatorField_L0 );
        g.setColorAt( 1.0, hobart::gradientColors::lightOperatorField_L1 );

        QBrush b( g );

        modifyPaletteBackground setBackground( b );

        setAutoFillBackground( true );
        setBackground( this );
    }

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::onClicked( QWidget *widget )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::onClicked()";
#endif
    LOG_CLASS_INFO( "FlashkeyEditor", "onClicked" );

    // ========================================================================
    // edit grid widgets
    // ========================================================================

    if ( deleteGroup_ == widget )
    {
        actionSoftkeyGroup_ = group_->text();

        // prompt user to delete group
        if ( actionSoftkeyGroup_.length() )
            session_->send( Error, ErrSoftkeyDevice, QString::number( 136801 ) + ",1" );
    }

    else if ( editCell_ == widget )
    {
        QString tare = actionParam_->text();
        QString tareNum = actionParamNumeric_->text();
        hobart::softkeyButton *flashkey( flashkeys_->selected() );

        // validate selected
        if ( !flashkey )
        {
            LOG_WARN( "no flashkey to edit", NULL );
            return;
        }

        int index = action_->findData( QString::number( selected_.action() ) );

        // validate action
        if ( index < 0 )
        {
            LOG_WARN( "bad flashkey action %d", selected_.action() );
            return;
        }

        // enter edit mode
        flashkeys_->editSelection();

        // set widget visibility
        showMode( CellEditMode );
    }

    else if (( addRow_ == widget ) || ( removeRow_ == widget ) ||
        ( addColumn_ == widget ) || ( removeColumn_ == widget ))
    {
        QSize s( flashkeys_->gridSize() );

        if ( addRow_ == widget )
            s.rheight() += 1;
        else if ( removeRow_ == widget )
        {
            hobart::softkeysWidget::ActionCode reason;

            if ( s.height() <= 1 )
                return;
            else if ( !flashkeys_->canRemoveRow( reason ) )
            {
                if ( hobart::softkeysWidget::ContainsSoftkey == reason )
                    session_->send( Error, ErrSoftkeyDevice, "136605,1" );
                else if ( hobart::softkeysWidget::ContainsSpannedSoftkey == reason )
                    session_->send( Error, ErrSoftkeyDevice, "136606,5" );

                return;
            }

            s.rheight() -= 1;
        }
        else if ( addColumn_ == widget )
            s.rwidth() += 1;
        else if ( removeColumn_ == widget )
        {
            hobart::softkeysWidget::ActionCode reason;

            if ( s.width() <= 1 )
                return;
            else if ( !flashkeys_->canRemoveColumn( reason ) )
            {
                if ( hobart::softkeysWidget::ContainsSoftkey == reason )
                    session_->send( Error, ErrSoftkeyDevice, "136607,1" );
                else if ( hobart::softkeysWidget::ContainsSpannedSoftkey == reason )
                    session_->send( Error, ErrSoftkeyDevice, "136608,5" );

                return;
            }

            s.rwidth() -= 1;
        }

        LOG_TRACE( "new grid size %d %d", s.width(), s.height() );
        flashkeys_->setGridSize( s );

        // save changes
        saveSoftkeyGroup();
    }

    else if (( addHeight_ == widget ) || ( removeHeight_ == widget ) ||
        ( addWidth_ == widget ) || ( removeWidth_ == widget ))
    {
        hobart::softkeyButton *flashkey( flashkeys_->selected() );

        // validate selected
        if ( !flashkey )
        {
            LOG_WARN( "no flashkey to edit", NULL );
            return;
        }

        QSize s( flashkey->gridSize() );

        if ( addHeight_ == widget )
        {
            hobart::softkeysWidget::ActionCode reason;

            if ( !flashkeys_->canExpandSoftkeyVert( flashkey, reason ) )
            {
                if ( hobart::softkeysWidget::ContainsSoftkey == reason )
                    session_->send( Error, ErrSoftkeyDevice, "136217,5" );
                else if ( hobart::softkeysWidget::GridTooSmall == reason )
                    session_->send( Error, ErrSoftkeyDevice, "136209,5" );

                return;
            }

            s.rheight() += 1;
        }
        else if ( removeHeight_ == widget )
        {
            if ( s.height() <= 1 )
            {
                session_->send( Error, ErrSoftkeyDevice, "136213,5" );
                return;
            }

            s.rheight() -= 1;
        }
        else if ( addWidth_ == widget )
        {
            hobart::softkeysWidget::ActionCode reason;

            if ( !flashkeys_->canExpandSoftkeyHorz( flashkey, reason ) )
            {
                if ( hobart::softkeysWidget::ContainsSoftkey == reason )
                    session_->send( Error, ErrSoftkeyDevice, "136217,5" );
                else if ( hobart::softkeysWidget::GridTooSmall == reason )
                    session_->send( Error, ErrSoftkeyDevice, "136211,5" );

                return;
            }

            s.rwidth() += 1;
        }
        else if ( removeWidth_ == widget )
        {
            if ( s.width() <= 1 )
            {
                session_->send( Error, ErrSoftkeyDevice, "136215,5" );
                return;
            }

            s.rwidth() -= 1;
        }

        LOG_TRACE( "new flashkey size %d %d", s.width(), s.height() );
        flashkey->setGridSize( s );

        // save changes
        saveSoftkey();

        // refresh widget
        flashkeys_->resizeButtons();
    }

    else if ( createNewCell_ == widget )
    {
        // set widget visibility
        showMode( CreateNewCellMode );
    }

    // ========================================================================
    // edit cell widgets
    // ========================================================================

    else if (( webPageUsesAll_ == widget ) || ( webPageUsesNone_ == widget ))
    {
        bool isChecked( webPageUsesAll_ == widget );

        webPageUsesStoreNumber_->setChecked( isChecked );
        webPageUsesDepartment_->setChecked( isChecked );
        webPageUsesPlu_->setChecked( isChecked );
        webPageUsesQty_->setChecked( isChecked );
        webPageUsesLang_->setChecked( isChecked );
        webPageUsesModel_->setChecked( isChecked );
    }

    else if (( cancelCellEdit_ == widget ) || ( cellEditDone_ == widget ))
    {
        hobart::softkeyButton *flashkey( flashkeys_->selected() );

        // validate selected
        if ( !flashkey )
            return;

        if ( cellEditDone_ == widget )
        {
            // for operate mode *and* missing flashkey description
            if ( mainWindow_->isOperateMode() )
            {
                QString text( flashkey->text() );

                // create our own flashkey description
                if ( text.isEmpty() )
                    setCurrentSoftkeyDefaultDescription( flashkey );
            }

            LOG_TRACE( "saving flashkey...", NULL );

            // save flashkey
            saveSoftkey();

            // save style (text alignment)
            saveSoftkeyStyle();
        }
        else
        {
            LOG_TRACE( "cancelling flashkey changes...", NULL );

            // revert flashkey
            flashkey->create( session_->scaleConfig->currentLanguage, &selected_, &style_ );

            // revert text alignment
            flashkeys_->setTextAlignment( style_.textVAlign() );
        }

        // set widget visibility
        showMode( GridEditMode );

        // enable screen
        setScreenEnabled( true );

        // FT-637
        // in operate mode select the flashkey as they probably intend to change the color
        if (( mainWindow_->isOperateMode() ) && ( cellEditDone_ == widget ))
        {
            // request the flashkey
            readSoftkey( flashkey->gridPos() );
        }
    }

    // ========================================================================
    // new cell widgets
    // ========================================================================

    else if ( cancelCellCreate_ == widget )
    {
        // set widget visibility
        showMode( GridEditMode );
    }

    // ========================================================================
    //  operate mode widgets
    // ========================================================================

    else if ( enableBestSellerList_ == widget )
    {
        MadpOperatorDisplayFields operatorDisplayFields( session_->scaleConfig->operatorDisplayFields );

        hobart::dynamicContentWidget::WidgetType newScreenType(
            enableBestSellerList_->isChecked() ?
                hobart::dynamicContentWidget::BestSellerList :
                hobart::dynamicContentWidget::Flashkeys );

        // show grid
        dynamicContent_->setCurrentIndex( newScreenType );

        // set grid type (flashkeys vs. best seller list)
        if ( hobart::softkeysWidget::ProductGrid == currentFeature().toInt() )
            operatorDisplayFields.setDcaStartScreenType( newScreenType );
        else if ( hobart::softkeysWidget::RunScreenButtons == currentFeature().toInt() )
            operatorDisplayFields.setDcaRunScreenType( newScreenType );
        else if ( hobart::softkeysWidget::RunScreenNumericKeypadButtons == currentFeature().toInt() )
            operatorDisplayFields.setDcaKeypadAreaType( newScreenType );
        else if ( hobart::softkeysWidget::SelfservCategoryResultsButtons == currentFeature().toInt() )      // new - did not add MCI since
            operatorDisplayFields.setDcaStartScreenType( newScreenType );                                   // new - thats what we are testing
        else
        {
            // this is bad, the check box should not even be enabled/visible
            return;
        }

        // reset selection
        if ( mainWindow_->isOperateMode() )
        {
            flashkeys_->resetSelection();

            operateBackgroundColor_->setEnabled( false );
        }

        // make sure fields are quoted
        operatorDisplayFields.setProdPlanningFrame( operatorDisplayFields.prodPlanningFrame() );
        operatorDisplayFields.setProdPlanningUpdate( operatorDisplayFields.prodPlanningUpdate() );

        // save!
        session_->send( operatorDisplayFields );
        session_->send( ConfigurationDev, CfgPrepackSave );

        // save local also
        session_->scaleConfig->operatorDisplayFields.setDcaStartScreenType( operatorDisplayFields.dcaStartScreenType() );
        session_->scaleConfig->operatorDisplayFields.setDcaRunScreenType( operatorDisplayFields.dcaRunScreenType() );
        session_->scaleConfig->operatorDisplayFields.setDcaKeypadAreaType( operatorDisplayFields.dcaKeypadAreaType() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::onCurrentIndexChanged( QWidget *widget )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::onCurrentIndexChanged()";
#endif
    QComboBox *comboBox = dynamic_cast<QComboBox*>( widget );

    if ( !comboBox )
        return;

    int index = comboBox->currentIndex();
    QString itemData( comboBox->itemData( index ).toString() );

    // ========================================================================
    // main pane widgets
    // ========================================================================

    if ( feature_ == widget )
    {
        actionSoftkeyGroup_.clear();

        // set feature
        setCurrentFeature( feature_->itemData( index ).toString() );

        // PJR-383
        int index = feature_->currentIndex();
        int itemData( feature_->itemData( index ).toInt() );

        if( itemData == hobart::softkeysWidget::FixedRowColumnGrid )
        {
            fixedRowColumnLabel_->show();
            fixedRowColumn_->show();

            fixedRowColumnDefaultButtonLabel_->show();
            fixedRowColumnDefault_->show();
        }
        else
        {
            fixedRowColumnLabel_->hide();
            fixedRowColumn_->hide();

            fixedRowColumnDefaultButtonLabel_->hide();
            fixedRowColumnDefault_->hide();
        }
    }

    else if ( action_ == widget )
    {
        // update ui
        showActionFields( itemData.toInt() );
    }

    // PJR-383
    else if ( fixedRowColumn_ == widget )
    {
        QString data = fixedRowColumn_->itemData( fixedRowColumn_->currentIndex() ).toString();
        flashkeys_->setFRowColMethod( data.toInt() );

        saveSoftkeyFeature();
    }

    // ========================================================================
    // edit cell widgets
    // ========================================================================

    else if ( image_ == widget )
    {
        hobart::softkeyButton *flashkey( flashkeys_->selected() );

        // validate selected
        if ( !flashkey )
            return;

        // none
        if ( 0 == index )
            flashkey->resetBackgroundImage();

        // default
        else if ( 1 == index )
            flashkey->setBackgroundImage( selected_.defBgImage() );

        else
        {
            MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

            QDir path( scaleConfig->SoftkeyGraphicsDirName() );
            flashkey->setBackgroundImage( path.filePath( itemData ) );
        }
    }

    // PJR-383
    else if ( toggledImage_ == widget )
    {
        hobart::softkeyButton *flashkey( flashkeys_->selected() );

        // validate selected
        if ( !flashkey )
            return;

        // none
        if ( 0 == index )
            flashkey->resetToggledBackgroundImage();

        // default
        else if ( 1 == index )
            flashkey->setToggledBackgroundImage( selected_.defBgImage() );

        else
        {
            MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

            QDir path( scaleConfig->SoftkeyGraphicsDirName() );
            flashkey->setToggledBackgroundImage( path.filePath( itemData ) );
        }
    }

    else if ( backgroundColor_ == widget )
    {
        hobart::softkeyButton *flashkey( flashkeys_->selected() );

        // validate selected
        if ( !flashkey )
            return;

        QString c;

        // default
        if ( 0 == index )
        {
            c = selected_.defBgColor();

            if ( !c.length() )
                c = style_.backgroundColor();
        }

        else if ( "custom" != itemData )
            c = itemData;

        else
        {
            ColorDialog dialog( mainWindow_ );
            dialog.setCurrentColor( flashkey->backgroundColor() );

            if ( QDialog::Accepted == dialog.exec() )
            {
                c = dialog.selectedColor().name().right( 6 );

                // add custom color to dialog
                backgroundColor_->blockSignals( true );
                backgroundColor_->addItem( c.toUpper(), QVariant( c ) );
                backgroundColor_->setCurrentIndex( index = (backgroundColor_->count() - 1) );
                backgroundColor_->blockSignals( false );
            }
            else
            {
                // revert
                backgroundColor_->blockSignals( true );
                backgroundColor_->setCurrentIndex( backgroundColorIndex_ );
                backgroundColor_->blockSignals( false );
            }
        }

        // set the color
        if ( c.length() )
        {
            flashkey->setBackgroundColor( makeColor( c ) );
            backgroundColorIndex_ = index;
        }
    }

    else if ( textColor_ == widget )
    {
        hobart::softkeyButton *flashkey( flashkeys_->selected() );

        // validate selected
        if ( !flashkey )
            return;

        QString c;

        // default
        if ( 0 == index )
        {
            c = selected_.defTextColor();

            if ( !c.length() )
                c = style_.textColor();
        }

        else if ( "custom" != itemData )
            c = itemData;

        else
        {
            ColorDialog dialog( mainWindow_ );
            dialog.setCurrentColor( flashkey->textColor() );

            if ( QDialog::Accepted == dialog.exec() )
            {
                c = dialog.selectedColor().name().right( 6 );

                // add custom color to dialog
                textColor_->blockSignals( true );
                textColor_->addItem( c.toUpper(), QVariant( c ) );
                textColor_->setCurrentIndex( index = (textColor_->count() - 1) );
                textColor_->blockSignals( false );
            }
            else
            {
                // revert
                textColor_->blockSignals( true );
                textColor_->setCurrentIndex( textColorIndex_ );
                textColor_->blockSignals( false );
            }
        }

        // set the color
        if ( c.length() )
        {
            flashkey->setTextColor( c );
            textColorIndex_ = index;
        }
    }

    else if ( textAlignment_ == widget )
    {
        // set text alignment
        flashkeys_->setTextAlignment( itemData.toInt() );
    }

    // ========================================================================
    //  operate mode widgets
    // ========================================================================

    else if ( operateBackgroundColor_ == widget )
    {
        hobart::softkeyButton *flashkey( flashkeys_->selected() );

        // validate selected
        if ( !flashkey )
            return;

        QString c;
        QString img;

        // default
        if ( 0 == index )
        {
            c = selected_.defBgColor();
            img = selected_.defBgImage();

            if ( !c.length() )
                c = style_.backgroundColor();
        }

        else if ( "custom" != itemData )
        {
            // check for a filename
            if (( itemData.contains( "." ) ) || ( 6 != itemData.length() ))
            {
                MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

                QDir path( scaleConfig->SoftkeyGraphicsDirName() );
                img = path.filePath( itemData );
            }

            // color
            else
            {
                c = itemData;
            }
        }

        else
        {
            ColorDialog dialog( mainWindow_ );
            dialog.setCurrentColor( flashkey->backgroundColor() );

            if ( QDialog::Accepted == dialog.exec() )
            {
                c = dialog.selectedColor().name().right( 6 );

                // add custom color to dialog
                operateBackgroundColor_->blockSignals( true );
                operateBackgroundColor_->addItem( c.toUpper(), QVariant( c ) );
                operateBackgroundColor_->setCurrentIndex( index = (operateBackgroundColor_->count() - 1) );
                operateBackgroundColor_->blockSignals( false );
            }
            else
            {
                // revert
                operateBackgroundColor_->blockSignals( true );
                operateBackgroundColor_->setCurrentIndex( operateBackgroundColorIndex_ );
                operateBackgroundColor_->blockSignals( false );
            }
        }

        // set the color and image
        if (( c.length() ) || ( img.length() ))
        {
            flashkey->setBackgroundColor( c.isEmpty() ? QColor() : makeColor( c ) );
            flashkey->setBackgroundImage( img.isEmpty() ? QString() : img );
            operateBackgroundColorIndex_ = index;

            // save flashkey
            isOperateColorChange = true;
            saveSoftkey();
            isOperateColorChange = false;
        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::onEditingFinished( QWidget *widget )
{
    LOG_CLASS_INFO( "FlashkeyEditor", "onEditingFinished" );

    // ========================================================================
    // main pane widgets
    // ========================================================================

    if ( group_ == widget )
    {
        QString value( group_->text() );

        // make sure we are requesting something new
        if ( currentGroup() != value )
        {
            actionSoftkeyGroup_ = value;

            LOG_TRACE( "new group '%s'", qPrintable( value ) );
            setCurrentGroup( value );
        }
    }

    else if (( actionParam_ == widget ) || ( actionParamNumeric_ == widget ))
    {
        LOG_TRACE( "actionParam_ or actionParamNumeric_", NULL );
        QString action( action_->itemData( action_->currentIndex() ).toString() );

        switch ( action.toInt() )
        {
        case Action_SelectProduct:		// Select Product
        case Action_LoadCategory_First: // Load Category
        case Action_LoadTareSoftkey:	// Preset tare
        case Action_Swipe:

            isUpdateSoftkey_ = true;
            writeSoftkey();
            break;

        case Action_ChangeLabelType:	// Change Label type
            isUpdateSoftkey_ = true;
            labelTypeNum_ = actionParam_->text();
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::onEditingFinished():  labelTypeNum_ = actionParam_->text() = " << labelTypeNum_;
#endif
            LOG_TRACE( "labelTypeNum_ = actionParam_->text() = '%s'", qPrintable( labelTypeNum_ ) );
            break;

        default:
            break;
        }
    }

    // ========================================================================
    // edit grid widgets
    // ========================================================================

    else if ( defaultGroup_ == widget || fixedRowColumnDefault_ == widget )
    {
        saveSoftkeyFeature();
    }

    else if ( horzSpacing_ == widget )
    {
        int value( horzSpacing_->text().toInt() );

        LOG_TRACE( "new horz spacing %d", value );
        flashkeys_->setHSpacing( value );

        saveSoftkeyGroup();
    }

    else if ( vertSpacing_ == widget )
    {
        int value( vertSpacing_->text().toInt() );

        LOG_TRACE( "new vert spacing %d", value );
        flashkeys_->setVSpacing( value );

        saveSoftkeyGroup();
    }

    else if ( fixedRowColumnDefault_ == widget )
    {
        int value( fixedRowColumnDefault_->text().toInt() );

        LOG_TRACE( "new fixed row/col default %d", value );
        flashkeys_->setFRowColDefButton( value );

        saveSoftkeyGroup();
    }

    // ========================================================================
    // edit cell widgets
    // ========================================================================

    else if ( primaryText_ == widget )
    {
        hobart::softkeyButton *flashkey( flashkeys_->selected() );

        // validate selected
        if ( !flashkey )
            return;

        if ( 2 != session_->scaleConfig->currentLanguage )
            flashkey->setText( primaryText_->text() );
    }

    else if ( secondaryText_ == widget )
    {
        hobart::softkeyButton *flashkey( flashkeys_->selected() );

        // validate selected
        if ( !flashkey )
            return;

        if ( 2 == session_->scaleConfig->currentLanguage )
            flashkey->setText( secondaryText_->text() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::onFlashkeyClicked( QWidget *w, QWidget *f, int action, const QString& param, int feature )
{
    LOG_CLASS_INFO( "FlashkeyEditor", "onFlashkeyClicked" );

    Q_UNUSED( action );
    Q_UNUSED( param );
    Q_UNUSED( feature );

    // only interested in clicks from the flashkeys widget
    if ( flashkeys_ != w )
        return;

    hobart::softkeyButton *flashkey = dynamic_cast<hobart::softkeyButton*>( f );

    if ( !flashkey )
    {
        LOG_WARN( "bad flashkey click", NULL );
        return;
    }

    // nothing to do
    if ( flashkeys_->selected() == flashkey )
        return;

    LOG_DEBUG( "flashkey '%s' selected 0x%08x", qPrintable( flashkey->text() ), flashkey );

    // request the flashkey
    readSoftkey( flashkey->gridPos() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::onFlashkeyCreateNewCell( QWidget *w, const QPoint& gridPos, int action )
{
    LOG_CLASS_INFO( "FlashkeyEditor", "onFlashkeyCreateNewCell" );

    Q_UNUSED( w );

    // we are creating a new softkey
    isNewSoftkey_ = true;
    newSoftkeyAction_ = action;

    LOG_DEBUG( "create new flashkey action %d pos %d %d", action, gridPos.x(), gridPos.y() );

    // create the flashkey
    createNewSoftkey( gridPos );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::onFlashkeyDeleteCell( QWidget *f )
{
    LOG_CLASS_INFO( "FlashkeyEditor", "onFlashkeyDeleteCell" );

    hobart::softkeyButton *flashkey = dynamic_cast<hobart::softkeyButton*>( f );

    if ( !flashkey )
    {
        LOG_WARN( "bad flashkey click", NULL );
        return;
    }

    QPoint gridPos( flashkey->gridPos() );

    LOG_INFO( "flashkey '%s' at %d %d deleted 0x%08x",
        qPrintable( flashkey->text() ),
        gridPos.x(),
        gridPos.y(),
        flashkey );

    // delete softkey
    deleteSoftkey( gridPos );

    // reset selection
    LOG_TRACE( "resetting selection", NULL );
    flashkeys_->resetSelection();

    LOG_TRACE( "enable screen", NULL );
    setScreenEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::onFlashkeyMoveCell( QWidget *w, QWidget *b, const QPoint& oldGridPos, const QPoint& newGridPos )
{
    LOG_CLASS_INFO( "FlashkeyEditor", "onFlashkeyMoveCell" );

    Q_UNUSED( w );
    Q_UNUSED( b );

    LOG_TRACE( "resetting selection", NULL );

    // reset any selection
    flashkeys_->resetSelection();

    // we are moving a softkey
    isMoveSoftkey_ = true;
    moveSoftkeyGridPos_ = newGridPos;

    LOG_INFO( "moving flashkey pos %d %d to new pos %d %d",
        oldGridPos.x(), oldGridPos.y(),
        newGridPos.x(), newGridPos.y() );

    // request the flashkey
    readSoftkey( oldGridPos );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::onScreenChanged()
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::onScreenChanged()";
#endif
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    OperateMode *operateMode = dynamic_cast<OperateMode*>( mode_ );

    // make sure weight bar is hidden
    if ( operateMode )
        operateMode->hideWeightBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::initialize()
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::initialize()";
#endif
    static const int actionFixedWidth = 356;        // 256
    static const int buttonHeight = 40;
    static const int rightPaneFixedWidth = 256;
    static const int scaleModel = session_->scaleConfig->getScaleModel();

    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );
    session_->scaleConfig->showToggled = false;

    isOperateColorChange = false;
    modifyPaletteForeground setForegroundGray( modifyPaletteForeground::createPalette( hobart::colorPalette::gray ) );

    modifyPaletteBackground setTransparentBackground( Qt::transparent );
    modifyPaletteColorRole<QPalette::Window> setCheckBoxMouseOverColor( Qt::transparent );

    modifyFontPointSize setFontPointSize10( scaleFontPointSizeForScreen( 10 ) );
    modifyFontPointSize setFontPointSize14( scaleFontPointSizeForScreen( 14 ) );

    if ( !mainWindow_->isOperateMode() )
        setPalette( QApplication::palette() );
    else
    {
        setFontPointSize14( this );
        setForegroundGray( this );
    }

    // ========================================================================

    clickedSignal_ = new QSignalMapper( this );
    connect( clickedSignal_, SIGNAL( mapped(QWidget*) ), SLOT( onClicked(QWidget*) ) );

    editingFinishedSignal_ = new QSignalMapper( this );
    connect( editingFinishedSignal_, SIGNAL( mapped(QWidget*) ), SLOT( onEditingFinished(QWidget*) ) );

    currentIndexChangedSignal_ = new QSignalMapper( this );
    connect( currentIndexChangedSignal_, SIGNAL( mapped(QWidget*) ), SLOT( onCurrentIndexChanged(QWidget*) ) );    

    // setup main and right panes with splitter in middle
    splitter_ = new QSplitter( Qt::Horizontal, this );
    splitter_->setChildrenCollapsible( false );
    splitter_->setObjectName( "splitter" );

    mainPane_ = new QWidget( splitter_ );
    mainPane_->setObjectName( "mainPane" );
    splitter_->addWidget( mainPane_ );

    rightPane_ = new QWidget( splitter_ );
    rightPane_->setFixedWidth( rightPaneFixedWidth );
    rightPane_->setObjectName( "rightPane" );
    splitter_->addWidget( rightPane_ );

    // hide the handle
    splitter_->setHandleWidth( 0 );

    QSplitterHandle *handle = splitter_->handle( 1 );
    handle->setFixedWidth( 0 );

    // ========================================================================
    // main pane widgets
    // ========================================================================

    // feature
    featureLabel_ = new QLabel( mainPane_ );
    featureLabel_->setObjectName( "featureLabel" );

    feature_ = new QComboBox( mainPane_ );
    feature_->setObjectName( "feature" );
    //feature_->setMaxVisibleItems( NUMBER_OF_FEATURES );

    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::ProductGrid ) ) );
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::RunScreenButtons ) ) );
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::PresetTareGrid ) ) );
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::MostCommonlyUsedItems ) ) );
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) ) );
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::SelfservProductGrid ) ) );
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::FixedRowColumnGrid ) ) );                 // 999020
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::RunScreenNumericKeypadButtons ) ) );
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::BuildAMealScreenButtons ) ) );
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::MeatSpecSheetScreeenButtons ) ) );        // 999033
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::ForcedCameraOverride ) ) );

    // if ( session_->isPrepack() ) - jhc being cut test is no longer just for prepack
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::RunScreenCutTestOn ) ) );
    feature_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::RunScreenCutTestOnOptionOne ) ) );

    feature_->setMaxVisibleItems( feature_->count() );

    currentIndexChangedSignal_->setMapping( feature_, feature_ );
    connect( feature_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // group
    groupLabel_ = new QLabel( mainPane_ );
    groupLabel_->setObjectName( "groupLabel" );

    group_ = new hobart::numericLineEdit( mainPane_ );
    group_->setInputTruncation( true );
    group_->setInputWrapping( true );
    group_->setObjectName( "group" );
    group_->setValidator( new QIntValidator( 0, 999999, mainPane_ ) );

    editingFinishedSignal_->setMapping( group_, group_ );
    connect( group_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    // action
    actionLabel_ = new QLabel( mainPane_ );
    actionLabel_->setObjectName( "actionLabel" );

    action_ = new QComboBox( mainPane_ );
    action_->setFixedWidth( actionFixedWidth );
    action_->setObjectName( "action" );

    if ( 1 == scaleModel )
    {
        action_->addItem( "", QVariant( QString::number( Action_NoAction ) ) );
        action_->addItem( "", QVariant( QString::number( Action_SelectProduct ) ) );
        action_->addItem( "", QVariant( QString::number( Action_LoadSoftkeyGroup ) ) );
        action_->addItem( "", QVariant( QString::number( Action_LoadCategory_First ) ) );
        action_->addItem( "", QVariant( QString::number( Action_ChangeScreen ) ) );
        action_->addItem( "", QVariant( QString::number( Action_LoadSpecials_First ) ) );
    }
    else
    {
        action_->addItem( "", QVariant( QString::number( Action_NoAction ) ) );
        action_->addItem( "", QVariant( QString::number( Action_SelectProduct ) ) );
        action_->addItem( "", QVariant( QString::number( Action_LoadSoftkeyGroup ) ) );
        action_->addItem( "", QVariant( QString::number( Action_LoadCategory_First ) ) );
        action_->addItem( "", QVariant( QString::number( Action_PrintLabel ) ) );
        action_->addItem( "", QVariant( QString::number( Action_SetPlatterTare ) ) );
        action_->addItem( "", QVariant( QString::number( Action_ApplyDiscount_PercentOff ) ) );
        action_->addItem( "", QVariant( QString::number( Action_ResetPrice ) ) );
        action_->addItem( "", QVariant( QString::number( Action_ChangeScreen ) ) );
        action_->addItem( "", QVariant( QString::number( Action_BrowseWeb ) ) );
        action_->addItem( "", QVariant( QString::number( Action_Application_URL ) ) );

        if ( 2 != scaleModel )
            action_->addItem( "", QVariant( QString::number( Action_FPP_URL ) ) );

        action_->addItem( "", QVariant( QString::number( Action_LoginOperator ) ) );
        action_->addItem( "", QVariant( QString::number( Action_LogoutOperator ) ) );
        action_->addItem( "", QVariant( QString::number( Action_LoadSpecials_First ) ) );

        if ( 2 != scaleModel )
        {
            action_->addItem( "", QVariant( QString::number( Action_PrintNewExpirationLabel ) ) );
            action_->addItem( "", QVariant( QString::number( Action_PrintExistingExpirationLabel ) ) );
        }

        bool allowPrintMultiple = ( 2 != scaleModel ) || scaleConfig->isAllowProductionEntryModeOnWrapper();
        if( allowPrintMultiple )
            action_->addItem( "", QVariant( QString::number( Action_PrintMultipleLabels ) ) );

        // These are no longer just available if we have a Prepack scale
        action_->addItem( "", QVariant( QString::number( Action_BeginCutTest ) ) );
        action_->addItem( "", QVariant( QString::number( Action_EndCutTest ) ) );
        action_->addItem( "", QVariant( QString::number( Action_CutTestHistory ) ) );
        action_->addItem( "", QVariant( QString::number( Action_CutTestSuspend ) ) );
        action_->addItem( "", QVariant( QString::number( Action_SelectAnyProduct ) ) );
        action_->addItem( "", QVariant( QString::number( Action_PrintRunTotals ) ) );
        action_->addItem( "", QVariant( QString::number( Action_ToggleMode ) ) );
        action_->addItem( "", QVariant( QString::number( Action_ToggleRfidPrinting ) ) );

        // SOF-3840
        // This is allowed in scale too
        action_->addItem( "", QVariant( QString::number( Action_SwitchTotalType ) ) );

        if ( 2 == scaleModel )
        {
            action_->addItem( "", QVariant( QString::number( Action_LoadTareSoftkey ) ) );
            action_->addItem( "", QVariant( QString::number( Action_ChangeLabelPlacementAndPosition ) ) );
            action_->addItem( "", QVariant( QString::number( Action_OverrideWrapperCameraDimensions ) ) );
        }
        else
        {
            action_->addItem( "", QVariant( QString::number( Action_ToggleProductionEntryMode ) ) );
            action_->addItem( "", QVariant( QString::number( Action_LoadTareSoftkey ) ) );
            action_->addItem( "", QVariant( QString::number( Action_LoadMostCommon_First ) ) );
            action_->addItem( "", QVariant( QString::number( Action_ToggleAccumulateItemsMode ) ) );
            action_->addItem( "", QVariant( QString::number( Action_AddItemToAccumulatedItems ) ) );
            action_->addItem( "", QVariant( QString::number( Action_RemoveLastItemFromAccumulatedItems ) ) );
            action_->addItem( "", QVariant( QString::number( Action_ShowAccumulatedItems ) ) );
            action_->addItem( "", QVariant( QString::number( Action_RemoveAllAccumulatedItems ) ) );
            action_->addItem( "", QVariant( QString::number( Action_PrintAccumulatedItems ) ) );
            action_->addItem( "", QVariant( QString::number( Action_PlayVideo ) ) );
            action_->addItem( "", QVariant( QString::number( Action_ServeNextCustomer ) ) );
            action_->addItem( "", QVariant( QString::number( Action_NowServingQueueManagement ) ) );
            action_->addItem( "", QVariant( QString::number( Action_Portioning ) ) );
            action_->addItem( "", QVariant( QString::number( Action_LaunchExternalApp ) ) );
        }

        action_->addItem( "", QVariant( QString::number( Action_ChangeLabelType ) ) );
        action_->addItem( "", QVariant( QString::number( Action_ChangeLanguage ) ) );

        action_->addItem( "", QVariant( QString::number( Action_OpenDocumentFolder ) ) );
        if( session_->uiUtilities->isFeatureLicensed( SCALE_PICK5 ) )
            action_->addItem( "", QVariant( QString::number( Action_TogglePick5Active  ) ) );
        action_->addItem( "", QVariant( QString::number( Action_Swipe ) ) );
        action_->addItem( "", QVariant( QString::number( Action_ToggleExtraShelfLifeDay ) ) );
		action_->addItem( "", QVariant( QString::number( Action_Toggle2ndLabelEnable  ) ) );
        action_->addItem("",  QVariant( QString::number( Action_ChangeDepartment  ) ) );
    }

    // cull available actions for operate mode
    if ( mainWindow_->isOperateMode() )
    {
        QStringList doomed;

        // make list of actions to remove
        for ( int i = 0; i < action_->count(); ++i )
        {
            QString data( action_->itemData( i ).toString() );

            switch ( data.toInt() )
            {
            case Action_NoAction:                       // None
            case Action_PrintLabel:                     // Print Label
            case Action_ChangeScreen:                   // Change screen // FT-461
            case Action_ChangeLanguage:                 // Change Language
            case Action_ResetPrice:                     // Reset Price
            case Action_SetPlatterTare:                 // Platter Tare
            case Action_SelectProduct:                  // Select Product
            case Action_ApplyDiscount_CentsOff:         // Cents Off Disc.
            case Action_ApplyDiscount_PercentOff:       // Percent Off Disc.
            case Action_VoidTransaction:                // Void transaction
            case Action_LoadSpecials_First:             // Load Specials
            case Action_LoadSearch_First:               // Load Search
            case Action_PrintExistingExpirationLabel:   // Print Existing Shelf Life Label
            case Action_PrintMultipleLabels:            // Print Multiple Labels
            case Action_PrintNewExpirationLabel:        // Print New Shelf Life Label
            case Action_LoadTareSoftkey:                // Preset tare
            case Action_LogoutOperator:                 // Operator Logout
            case Action_Portioning:                     // Portioning (Portion Zero)
            case Action_OpenDocumentFolder:             // Open Document
            case Action_Swipe:                          // Swipe Screen
            case Action_ChangeDepartment:
                break;

            default:
                doomed.append( data );
                break;
            }
        }

        // remove doomed actions
        foreach ( const QString& s, doomed )
            action_->removeItem( action_->findData( s ) );
    }

    currentIndexChangedSignal_->setMapping( action_, action_ );
    connect( action_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // action parameter
    actionParamLabel_ = new QLabel( mainPane_ );
    actionParamLabel_->setObjectName( "actionParamLabel" );

    actionParam_ = new QLineEdit( mainPane_ );
    actionParam_->setObjectName( "actionParam" );

    editingFinishedSignal_->setMapping( actionParam_, actionParam_ );
    connect( actionParam_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    actionParamNumeric_ = new hobart::numericLineEdit( mainPane_ );
    actionParamNumeric_->setObjectName( "actionParamNumeric" );

    editingFinishedSignal_->setMapping( actionParamNumeric_, actionParamNumeric_ );
    connect( actionParamNumeric_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    // screen change actions
    screenChangeAction_ = new QComboBox( mainPane_ );
    screenChangeAction_->setObjectName( "screenChangeAction" );

    screenChangeAction_->addItem( "", QVariant( QString::number( 0 ) ) );

    bool searchEnabled = scaleConfig->enableSearchMenuButton();
    if( searchEnabled )
        screenChangeAction_->addItem( "", QVariant( QString::number( 4 ) ) );
    screenChangeAction_->addItem( "", QVariant( QString::number( 5 ) ) );
    screenChangeAction_->addItem( "", QVariant( QString::number( 9 ) ) );
    screenChangeAction_->addItem( "", QVariant( QString::number( 12 ) ) );

    if ( !session_->isPrepack() )
        screenChangeAction_->addItem( "", QVariant( QString::number( 14 ) ) );

    screenChangeAction_->addItem( "", QVariant( QString::number( 6 ) ) );
    screenChangeAction_->addItem( "", QVariant( QString::number( 7 ) ) );
    screenChangeAction_->addItem( "", QVariant( QString::number( 13 ) ) );
    screenChangeAction_->addItem( "", QVariant( QString::number( 15 ) ) );

    if ( session_->isPrepack() )
    {
        screenChangeAction_->addItem( "", QVariant( QString::number( 8 ) ) );
        screenChangeAction_->addItem( "", QVariant( QString::number( 10 ) ) );
        screenChangeAction_->addItem( "", QVariant( QString::number( 11 ) ) );
    }

    screenChangeAction_->addItem( "", QVariant( QString::number( 16 ) ) );

    if ( !session_->isPrepack() )
    {
        screenChangeAction_->addItem( "", QVariant( QString::number( 17 ) ) );
        screenChangeAction_->addItem( "", QVariant( QString::number( 40+OperateMode::ExpandedText ) ) );
        screenChangeAction_->addItem( "", QVariant( QString::number( 40+OperateMode::ExpandedText2 ) ) ); // FT-381
        screenChangeAction_->addItem( "", QVariant( QString::number( 40+OperateMode::Recipe ) ) );
        screenChangeAction_->addItem( "", QVariant( QString::number( 40+OperateMode::CookingInstructions ) ) ); // FT-379
        screenChangeAction_->addItem( "", QVariant( QString::number( 40+OperateMode::SuggestiveSell ) ) );
        screenChangeAction_->addItem( "", QVariant( QString::number( 40+OperateMode::Allergens ) ) );
    }

    screenChangeAction_->addItem( "", QVariant( QString::number( 18 ) ) );
    screenChangeAction_->addItem( "", QVariant( QString::number( 19 ) ) );

    currentIndexChangedSignal_->setMapping( screenChangeAction_, screenChangeAction_ );
    connect( screenChangeAction_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // play video actions
    playVideoAction_ = new QComboBox( mainPane_ );
    playVideoAction_->setObjectName( "playVideoAction" );

    currentIndexChangedSignal_->setMapping( playVideoAction_, playVideoAction_ );
    connect( playVideoAction_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );
    
    // swipe action
    swipeAction_ = new QComboBox( mainPane_ );
    swipeAction_->setObjectName( "swipeAction" );
    
    swipeAction_->addItem( tr( "Down" ), QVariant( QString::number( ScreenWidget::Down )));
    swipeAction_->addItem( tr( "Left" ), QVariant( QString::number( ScreenWidget::Left )));
    swipeAction_->addItem( tr( "Right" ), QVariant( QString::number( ScreenWidget::Right )));
    swipeAction_->addItem( tr( "Up" ), QVariant( QString::number( ScreenWidget::Up )));
    
    currentIndexChangedSignal_->setMapping( swipeAction_, swipeAction_ );
    connect( swipeAction_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // change departments action
    departments_ = new QComboBox(mainPane_);
    departments_->setObjectName("changeDepartmentsAction");

    currentIndexChangedSignal_->setMapping(departments_, departments_);
    connect(departments_, SIGNAL (currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // total types actions
    totalTypesAction_ = new QComboBox( mainPane_ );
    totalTypesAction_->setObjectName( "totalTypesAction" );

    currentIndexChangedSignal_->setMapping( totalTypesAction_, totalTypesAction_ );
    connect( totalTypesAction_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // launch external app actions
    launchExternalAppAction_ = new QComboBox( mainPane_ );
    launchExternalAppAction_->setObjectName( "launchExternalAppAction" );

    currentIndexChangedSignal_->setMapping( launchExternalAppAction_, launchExternalAppAction_ );
    connect( launchExternalAppAction_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // flashkeys
    static const int dynamicContentWidgets =
        hobart::dynamicContentWidget::Flashkeys |
        hobart::dynamicContentWidget::BestSellerList;

    dynamicContent_ = new hobart::dynamicContentWidget( (hobart::dynamicContentWidget::WidgetType) dynamicContentWidgets, mainPane_ );
    dynamicContent_->setObjectName( "dynamicContent" );

    // save flashkey objects for later use
    flashkeys_ = dynamic_cast<hobart::softkeysWidget*>( dynamicContent_->widget( hobart::dynamicContentWidget::Flashkeys ) );
    bestSellers_ = dynamic_cast<hobart::softkeysWidget*>( dynamicContent_->widget( hobart::dynamicContentWidget::BestSellerList ) );

    flashkeys_->setEditMode( true );

    connect( flashkeys_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SLOT( onFlashkeyClicked(QWidget*,QWidget*,int,QString,int) ) );
    connect( flashkeys_, SIGNAL( createNewCell(QWidget*,QPoint,int) ), SLOT( onFlashkeyCreateNewCell(QWidget*,QPoint,int) ) );
    connect( flashkeys_, SIGNAL( moveCell(QWidget*,QWidget*,QPoint,QPoint) ), SLOT( onFlashkeyMoveCell(QWidget*,QWidget*,QPoint,QPoint) ) );

    // ========================================================================
    // edit grid widgets
    // ========================================================================

    editGridWidgets_ = new QWidget( rightPane_ );
    editGridWidgets_->setObjectName( "editGridWidgets" );

    // delete group
    deleteGroup_ = new QPushButton( editGridWidgets_ );
    deleteGroup_->setFixedHeight( buttonHeight );
    deleteGroup_->setObjectName( "deleteGroup" );

    clickedSignal_->setMapping( deleteGroup_, deleteGroup_ );
    connect( deleteGroup_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // default group
    defaultGroupLabel_ = new QLabel( editGridWidgets_ );
    defaultGroupLabel_->setObjectName( "defaultGroupLabel" );

    defaultGroup_ = new hobart::numericLineEdit( editGridWidgets_ );
    defaultGroup_->setInputTruncation( true );
    defaultGroup_->setInputWrapping( true );
    defaultGroup_->setObjectName( "defaultGroup" );
    defaultGroup_->setValidator( new QIntValidator( 0, 999999, editGridWidgets_ ) );

    editingFinishedSignal_->setMapping( defaultGroup_, defaultGroup_ );
    connect( defaultGroup_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    // horizontal spacing
    horzSpacingLabel_ = new QLabel( editGridWidgets_ );
    horzSpacingLabel_->setObjectName( "horzSpacingLabel" );

    horzSpacing_ = new hobart::numericLineEdit( editGridWidgets_ );
    horzSpacing_->setInputTruncation( true );
    horzSpacing_->setInputWrapping( true );
    horzSpacing_->setObjectName( "horzSpacing" );
    horzSpacing_->setValidator( new QIntValidator( 0, 99, editGridWidgets_ ) );

    editingFinishedSignal_->setMapping( horzSpacing_, horzSpacing_ );
    connect( horzSpacing_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    // vertical spacing
    vertSpacingLabel_ = new QLabel( editGridWidgets_ );
    vertSpacingLabel_->setObjectName( "vertSpacingLabel" );

    vertSpacing_ = new hobart::numericLineEdit( editGridWidgets_ );
    vertSpacing_->setInputTruncation( true );
    vertSpacing_->setInputWrapping( true );
    vertSpacing_->setObjectName( "vertSpacing" );
    vertSpacing_->setValidator( new QIntValidator( 0, 99, editGridWidgets_ ) );

    editingFinishedSignal_->setMapping( vertSpacing_, vertSpacing_ );
    connect( vertSpacing_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    // edit cell
    editCell_ = new QPushButton( editGridWidgets_ );
    editCell_->setFixedHeight( buttonHeight );
    editCell_->setObjectName( "editCell" );

    clickedSignal_->setMapping( editCell_, editCell_ );
    connect( editCell_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // rows
    rowsLabel_ = new QLabel( editGridWidgets_ );
    rowsLabel_->setObjectName( "rowsLabel" );

    addRow_ = new QPushButton( editGridWidgets_ );
    addRow_->setFixedHeight( buttonHeight );
    addRow_->setFixedWidth( buttonHeight );
    addRow_->setObjectName( "addRow" );

    clickedSignal_->setMapping( addRow_, addRow_ );
    connect( addRow_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    removeRow_ = new QPushButton( editGridWidgets_ );
    removeRow_->setFixedHeight( buttonHeight );
    removeRow_->setFixedWidth( buttonHeight );
    removeRow_->setObjectName( "removeRow" );

    clickedSignal_->setMapping( removeRow_, removeRow_ );
    connect( removeRow_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // columns
    columnsLabel_ = new QLabel( editGridWidgets_ );
    columnsLabel_->setObjectName( "columnsLabel" );

    addColumn_ = new QPushButton( editGridWidgets_ );
    addColumn_->setFixedHeight( buttonHeight );
    addColumn_->setFixedWidth( buttonHeight );
    addColumn_->setObjectName( "addColumn" );

    clickedSignal_->setMapping( addColumn_, addColumn_ );
    connect( addColumn_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    removeColumn_ = new QPushButton( editGridWidgets_ );
    removeColumn_->setFixedHeight( buttonHeight );
    removeColumn_->setFixedWidth( buttonHeight );
    removeColumn_->setObjectName( "removeColumn" );

    clickedSignal_->setMapping( removeColumn_, removeColumn_ );
    connect( removeColumn_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // button row span
    heightLabel_ = new QLabel( editGridWidgets_ );
    heightLabel_->setObjectName( "heightLabel" );

    addHeight_ = new QPushButton( editGridWidgets_ );
    addHeight_->setFixedHeight( buttonHeight );
    addHeight_->setFixedWidth( buttonHeight );
    addHeight_->setObjectName( "addHeight" );

    clickedSignal_->setMapping( addHeight_, addHeight_ );
    connect( addHeight_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    removeHeight_ = new QPushButton( editGridWidgets_ );
    removeHeight_->setFixedHeight( buttonHeight );
    removeHeight_->setFixedWidth( buttonHeight );
    removeHeight_->setObjectName( "removeHeight" );

    clickedSignal_->setMapping( removeHeight_, removeHeight_ );
    connect( removeHeight_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // button column span
    widthLabel_ = new QLabel( editGridWidgets_ );
    widthLabel_->setObjectName( "widthLabel" );

    addWidth_ = new QPushButton( editGridWidgets_ );
    addWidth_->setFixedHeight( buttonHeight );
    addWidth_->setFixedWidth( buttonHeight );
    addWidth_->setObjectName( "addWidth" );

    clickedSignal_->setMapping( addWidth_, addWidth_ );
    connect( addWidth_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    removeWidth_ = new QPushButton( editGridWidgets_ );
    removeWidth_->setFixedHeight( buttonHeight );
    removeWidth_->setFixedWidth( buttonHeight );
    removeWidth_->setObjectName( "removeWidth" );

    clickedSignal_->setMapping( removeWidth_, removeWidth_ );
    connect( removeWidth_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // create new cell
    createNewCell_ = new QPushButton( editGridWidgets_ );
    createNewCell_->setFixedHeight( buttonHeight );
    createNewCell_->setObjectName( "createNewCell" );

    clickedSignal_->setMapping( createNewCell_, createNewCell_ );
    connect( createNewCell_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // PJR-383
    // Fixed Row/Column selection
    fixedRowColumnLabel_ = new QLabel( editGridWidgets_ );
    fixedRowColumnLabel_->setObjectName( "fixedLabel" );

    fixedRowColumn_ = new QComboBox( editGridWidgets_ );
    fixedRowColumn_->setObjectName( "fixedRowColumn" );

    fixedRowColumn_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::FRCM_None ) ) );
    fixedRowColumn_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::FRCM_TopRow ) ) );
    fixedRowColumn_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::FRCM_FirstColumn ) ) );
    fixedRowColumn_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::FRCM_BottomRow ) ) );
    fixedRowColumn_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::FRCM_LastColumn ) ) );

    currentIndexChangedSignal_->setMapping( fixedRowColumn_, fixedRowColumn_ );
    connect( fixedRowColumn_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // Fixed Row/Column - Default button
    fixedRowColumnDefaultButtonLabel_ = new QLabel( editGridWidgets_ );
    fixedRowColumnDefaultButtonLabel_->setObjectName( "fixedDefaultLabel" );

    fixedRowColumnDefault_ = new hobart::numericLineEdit( editGridWidgets_ );
    fixedRowColumnDefault_->setInputTruncation( true );
    fixedRowColumnDefault_->setInputWrapping( true );
    fixedRowColumnDefault_->setObjectName( "fixedRowColDef" );
    fixedRowColumnDefault_->setValidator( new QIntValidator( 0, 99, editGridWidgets_ ) );

    editingFinishedSignal_->setMapping( fixedRowColumnDefault_, fixedRowColumnDefault_ );
    connect( fixedRowColumnDefault_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    // waste bin
    refuseBin_ = new hobart::refuseBinWidget( editGridWidgets_ );
    refuseBin_->setFixedHeight( buttonHeight );
    refuseBin_->setIcon( QPixmap( ":/images/screens/refusebin.png" ) );
    refuseBin_->setObjectName( "refuseBin" );
    refuseBin_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

    {
        QObjectList acceptedSources( refuseBin_->acceptedSources() );
        acceptedSources.append( flashkeys_ );
        refuseBin_->setAcceptedSources( acceptedSources );

        QStringList acceptedMimeTypes( refuseBin_->acceptedMimeTypes() );
        acceptedMimeTypes.append( hobart::softkeyButton::mimeDataType() );
        refuseBin_->setAcceptedMimeTypes( acceptedMimeTypes );
    }

    connect( refuseBin_, SIGNAL( deleteWidget(QWidget*) ), SLOT( onFlashkeyDeleteCell(QWidget*) ) );

    // ========================================================================
    // edit cell widgets
    // ========================================================================

    editCellWidgets_ = new QWidget( rightPane_ );
    editCellWidgets_->setObjectName( "editCellWidgets" );

    // PRIMARY label placement
    labelPlacement1Label_ = new QLabel( editCellWidgets_ );
    labelPlacement1Label_->setObjectName( "labelPlacement1Label" );

    labelPlacement1_ = new QComboBox( editCellWidgets_ );
    labelPlacement1_->setObjectName( "labelPlacement1" );

    labelPlacement1_->addItem( "", QVariant( "2" ) );
    labelPlacement1_->addItem( "", QVariant( "0" ) );
    labelPlacement1_->addItem( "", QVariant( "1" ) );

    if ( twoLabelAppliersAttached_ )
    {
        labelPlacement1_->addItem( "", QVariant( "3" ) );
        labelPlacement1_->addItem( "", QVariant( "4" ) );
    }

    currentIndexChangedSignal_->setMapping( labelPlacement1_, labelPlacement1_ );
    connect( labelPlacement1_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // PRIMARY label rotation
    labelRotation1Label_ = new QLabel( editCellWidgets_ );
    labelRotation1Label_->setObjectName( "labelRotation1Label" );

    labelRotation1_ = new QComboBox( editCellWidgets_ );
    labelRotation1_->setObjectName( "labelRotation1" );

    labelRotation1_->addItem( "", QVariant( "0" ) );
    labelRotation1_->addItem( "", QVariant( "1" ) );
    labelRotation1_->addItem( "", QVariant( "2" ) );
    labelRotation1_->addItem( "", QVariant( "3" ) );
    labelRotation1_->addItem( "", QVariant( "4" ) );
    labelRotation1_->addItem( "", QVariant( "5" ) );
    labelRotation1_->addItem( "", QVariant( "6" ) );
    labelRotation1_->addItem( "", QVariant( "7" ) );
    labelRotation1_->addItem( "", QVariant( "8" ) );

    currentIndexChangedSignal_->setMapping( labelRotation1_, labelRotation1_ );
    connect( labelRotation1_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // SECOND label placement
    labelPlacement2Label_ = new QLabel( editCellWidgets_ );
    labelPlacement2Label_->setObjectName( "labelPlacement2Label" );

    labelPlacement2_ = new QComboBox( editCellWidgets_ );
    labelPlacement2_->setObjectName( "labelPlacement2" );

    labelPlacement2_->addItem( "", QVariant( "2" ) );           // becaase No Change or System Default or None is actually = 2 in emum list so this makes 2 get loaded first.
    labelPlacement2_->addItem( "", QVariant( "0" ) );
    labelPlacement2_->addItem( "", QVariant( "1" ) );
    labelPlacement2_->addItem( "", QVariant( "3" ) );
    labelPlacement2_->addItem( "", QVariant( "4" ) );
    labelPlacement2_->addItem( "", QVariant( "5" ) );
    labelPlacement2_->addItem( "", QVariant( "6" ) );
    labelPlacement2_->addItem( "", QVariant( "7" ) );
    labelPlacement2_->addItem( "", QVariant( "8" ) );

    currentIndexChangedSignal_->setMapping( labelPlacement2_, labelPlacement2_ );
    connect( labelPlacement2_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // SECOND label rotation
    labelRotation2Label_ = new QLabel( editCellWidgets_ );
    labelRotation2Label_->setObjectName( "labelRotation2Label" );

    labelRotation2_ = new QComboBox( editCellWidgets_ );
    labelRotation2_->setObjectName( "labelRotation2" );

    labelRotation2_->addItem( "", QVariant( "0" ) );
    labelRotation2_->addItem( "", QVariant( "1" ) );
    labelRotation2_->addItem( "", QVariant( "2" ) );
    labelRotation2_->addItem( "", QVariant( "3" ) );
    labelRotation2_->addItem( "", QVariant( "4" ) );
    labelRotation2_->addItem( "", QVariant( "5" ) );
    labelRotation2_->addItem( "", QVariant( "6" ) );
    labelRotation2_->addItem( "", QVariant( "7" ) );
    labelRotation2_->addItem( "", QVariant( "8" ) );

    currentIndexChangedSignal_->setMapping( labelRotation2_, labelRotation2_ );
    connect( labelRotation2_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // Printer
    printerLabel_ = new QLabel( editCellWidgets_ );
    printerLabel_->setObjectName( "printerLabel" );

    printer_ = new QComboBox( editCellWidgets_ );
    printer_->setObjectName( "printerSelection" );

    printer_->addItem( "", QVariant( "0" ) );
    printer_->addItem( "", QVariant( "1" ) );
    printer_->addItem( "", QVariant( "2" ) );

    currentIndexChangedSignal_->setMapping( printer_, printer_ );
    connect( printer_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // web page cgi options
    webPageParameters_ = new QGroupBox( editCellWidgets_ );
    webPageParameters_->setObjectName( "webPageParameters" );

    webPageUsesStoreNumber_ = new QCheckBox( webPageParameters_ );
    webPageUsesStoreNumber_->setObjectName( "webPageUsesStoreNumber" );

    clickedSignal_->setMapping( webPageUsesStoreNumber_, webPageUsesStoreNumber_ );
    connect( webPageUsesStoreNumber_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    webPageUsesDepartment_ = new QCheckBox( webPageParameters_ );
    webPageUsesDepartment_->setObjectName( "webPageUsesPlu" );

    clickedSignal_->setMapping( webPageUsesDepartment_, webPageUsesDepartment_ );
    connect( webPageUsesDepartment_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    webPageUsesPlu_ = new QCheckBox( webPageParameters_ );
    webPageUsesPlu_->setObjectName( "webPageUsesPlu" );

    clickedSignal_->setMapping( webPageUsesPlu_, webPageUsesPlu_ );
    connect( webPageUsesPlu_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    webPageUsesQty_ = new QCheckBox( webPageParameters_ );
    webPageUsesQty_->setObjectName( "webPageUsesQty" );

    clickedSignal_->setMapping( webPageUsesQty_, webPageUsesQty_ );
    connect( webPageUsesQty_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    webPageUsesLang_ = new QCheckBox( webPageParameters_ );
    webPageUsesLang_->setObjectName( "webPageUsesLang" );

    clickedSignal_->setMapping( webPageUsesLang_, webPageUsesLang_ );
    connect( webPageUsesLang_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    webPageUsesModel_ = new QCheckBox( webPageParameters_ );
    webPageUsesModel_->setObjectName( "webPageUsesModel" );

    clickedSignal_->setMapping( webPageUsesModel_, webPageUsesModel_ );
    connect( webPageUsesModel_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    webPageUsesAll_ = new QPushButton( webPageParameters_ );
    webPageUsesAll_->setFixedHeight( buttonHeight );
    webPageUsesAll_->setObjectName( "webPageUsesAll" );

    clickedSignal_->setMapping( webPageUsesAll_, webPageUsesAll_ );
    connect( webPageUsesAll_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    webPageUsesNone_ = new QPushButton( webPageParameters_ );
    webPageUsesNone_->setFixedHeight( buttonHeight );
    webPageUsesNone_->setObjectName( "webPageUsesNone" );

    clickedSignal_->setMapping( webPageUsesNone_, webPageUsesNone_ );
    connect( webPageUsesNone_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // camera override tray length, width, and lip height
    QIntValidator *traySizeValidator( new QIntValidator( 0, 999, this ) );

    trayLengthLabel_ = new QLabel( editCellWidgets_ );
    trayLengthLabel_->setObjectName( "trayLengthLabel" );

    trayLength_ = new hobart::numericLineEdit( editCellWidgets_ );
    trayLength_->setDecimalPlaces( 1 );
    trayLength_->setInputTruncation( true );
    trayLength_->setInputWrapping( true );
    trayLength_->setObjectName( "trayLength" );
    trayLength_->setValidator( traySizeValidator );

    trayWidthLabel_ = new QLabel( editCellWidgets_ );
    trayWidthLabel_->setObjectName( "trayWidthLabel" );

    trayWidth_ = new hobart::numericLineEdit( editCellWidgets_ );
    trayWidth_->setDecimalPlaces( 1 );
    trayWidth_->setInputTruncation( true );
    trayWidth_->setInputWrapping( true );
    trayWidth_->setObjectName( "trayWidth" );
    trayWidth_->setValidator( traySizeValidator );

    trayLipHeightLabel_ = new QLabel( editCellWidgets_ );
    trayLipHeightLabel_->setObjectName( "trayLipHeightLabel" );

    trayLipHeight_ = new hobart::numericLineEdit( editCellWidgets_ );
    trayLipHeight_->setDecimalPlaces( 1 );
    trayLipHeight_->setInputTruncation( true );
    trayLipHeight_->setInputWrapping( true );
    trayLipHeight_->setObjectName( "trayLipHeight" );
    trayLipHeight_->setValidator( traySizeValidator );

    // display location
    displayLocationLabel_ = new QLabel( editCellWidgets_ );
    displayLocationLabel_->setObjectName( "displayLocationLabel" );

    displayLocation_ = new QComboBox( editCellWidgets_ );
    displayLocation_->setObjectName( "displayLocation" );

    displayLocation_->addItem( "", QVariant( QString::number( 0 ) ) );
    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::ProductGrid ) ) );
    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::RunScreenButtons ) ) );
    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::RunScreenNumericKeypadButtons ) ) );

    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::ProductGrid ) ) );
    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::RunScreenButtons ) ) );
    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::MostCommonlyUsedItems ) ) );
    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::SelfservCategoryButtons ) ) );
    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::SelfservProductGrid ) ) );
    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::FixedRowColumnGrid ) ) );
    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::RunScreenNumericKeypadButtons ) ) );
    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::BuildAMealScreenButtons ) ) );
    displayLocation_->addItem( "", QVariant( QString::number( hobart::softkeysWidget::MeatSpecSheetScreeenButtons ) ) );

    currentIndexChangedSignal_->setMapping( displayLocation_, displayLocation_ );
    connect( displayLocation_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // flashkey image
    imageLabel_ = new QLabel( editCellWidgets_ );
    imageLabel_->setObjectName( "imageLabel" );

    image_ = new QComboBox( editCellWidgets_ );
    image_->setObjectName( "image" );

    image_->addItem( "", QVariant( QString::number( 0 ) ) );
    image_->addItem( "", QVariant( QString::number( 1 ) ) );

    currentIndexChangedSignal_->setMapping( image_, image_ );
    connect( image_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // python scripts
   pyScriptLabel_ = new QLabel( editCellWidgets_ );
    pyScriptLabel_->setObjectName( "pyScriptLabel" );

    pyScript_ = new QComboBox( editCellWidgets_ );
    pyScript_->setObjectName( "pyScript" );

    pyScript_->addItem( "", QVariant( QString::number( 0 ) ) );

    currentIndexChangedSignal_->setMapping( pyScript_, pyScript_ );
    connect( pyScript_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // python script interval
    pyScriptIntervalLabel_ = new QLabel( editCellWidgets_ );
    pyScriptIntervalLabel_->setObjectName( "pyScriptIntervalLabel" );

    pyScriptInterval_ = new hobart::numericLineEdit( editCellWidgets_ );
    pyScriptInterval_->setObjectName( "pyScriptInterval" );

    pyScriptInterval_->fromInt( 0 );

    editingFinishedSignal_->setMapping( pyScriptInterval_, pyScriptInterval_ );
    connect( pyScriptInterval_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    // PJR-383
    toggledImageLabel_ = new QLabel( editCellWidgets_ );
    toggledImageLabel_->setObjectName( "depressedImageLabel" );
    toggledImage_ = new QComboBox( editCellWidgets_ );
    toggledImage_->setObjectName( "depressedImage" );
    toggledImage_->addItem( "", QVariant( QString::number( 0 ) ) );
    toggledImage_->addItem( "", QVariant( QString::number( 1 ) ) );
    currentIndexChangedSignal_->setMapping( toggledImage_, toggledImage_ );
    connect( toggledImage_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // background color
    backgroundColorLabel_ = new QLabel( editCellWidgets_ );
    backgroundColorLabel_->setObjectName( "backgroundColorLabel" );

    backgroundColor_ = new QComboBox( editCellWidgets_ );
    backgroundColor_->setObjectName( "backgroundColor" );

    backgroundColor_->addItem( "", QVariant( QString::number( 0 ) ) );
    backgroundColor_->addItem( "", QVariant( "000099" ) );
    backgroundColor_->addItem( "", QVariant( "000000" ) );
    backgroundColor_->addItem( "", QVariant( "000080" ) );
    backgroundColor_->addItem( "", QVariant( "0000FF" ) );
    backgroundColor_->addItem( "", QVariant( "00FFFF" ) );
    backgroundColor_->addItem( "", QVariant( "800080" ) );
    backgroundColor_->addItem( "", QVariant( "FF00FF" ) );
    backgroundColor_->addItem( "", QVariant( "800000" ) );
    backgroundColor_->addItem( "", QVariant( "FF0000" ) );
    backgroundColor_->addItem( "", QVariant( "808000" ) );
    backgroundColor_->addItem( "", QVariant( "FFFF00" ) );
    backgroundColor_->addItem( "", QVariant( "008000" ) );
    backgroundColor_->addItem( "", QVariant( "00FF00" ) );
    backgroundColor_->addItem( "", QVariant( "008080" ) );
    backgroundColor_->addItem( "", QVariant( "808080" ) );
    backgroundColor_->addItem( "", QVariant( "C0C0C0" ) );
    backgroundColor_->addItem( "", QVariant( "FFFFFF" ) );
    backgroundColor_->addItem( "", QVariant( "custom" ) );

    currentIndexChangedSignal_->setMapping( backgroundColor_, backgroundColor_ );
    connect( backgroundColor_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // text color
    textColorLabel_ = new QLabel( editCellWidgets_ );
    textColorLabel_->setObjectName( "textColorLabel" );

    textColor_ = new QComboBox( editCellWidgets_ );
    textColor_->setObjectName( "textColor" );

    textColor_->addItem( "", QVariant( QString::number( 0 ) ) );
    textColor_->addItem( "", QVariant( "000099" ) );
    textColor_->addItem( "", QVariant( "000000" ) );
    textColor_->addItem( "", QVariant( "000080" ) );
    textColor_->addItem( "", QVariant( "0000FF" ) );
    textColor_->addItem( "", QVariant( "00FFFF" ) );
    textColor_->addItem( "", QVariant( "800080" ) );
    textColor_->addItem( "", QVariant( "FF00FF" ) );
    textColor_->addItem( "", QVariant( "800000" ) );
    textColor_->addItem( "", QVariant( "FF0000" ) );
    textColor_->addItem( "", QVariant( "808000" ) );
    textColor_->addItem( "", QVariant( "FFFF00" ) );
    textColor_->addItem( "", QVariant( "008000" ) );
    textColor_->addItem( "", QVariant( "00FF00" ) );
    textColor_->addItem( "", QVariant( "008080" ) );
    textColor_->addItem( "", QVariant( "808080" ) );
    textColor_->addItem( "", QVariant( "C0C0C0" ) );
    textColor_->addItem( "", QVariant( "FFFFFF" ) );
    textColor_->addItem( "", QVariant( "custom" ) );

    currentIndexChangedSignal_->setMapping( textColor_, textColor_ );
    connect( textColor_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // text alignment
    textAlignmentLabel_ = new QLabel( editCellWidgets_ );
    textAlignmentLabel_->setObjectName( "textAlignmentLabel" );

    textAlignment_ = new QComboBox( editCellWidgets_ );
    textAlignment_->setObjectName( "textAlignment" );

    textAlignment_->addItem( "", QVariant( QString::number( 0 ) ) );
    textAlignment_->addItem( "", QVariant( QString::number( 1 ) ) );
    textAlignment_->addItem( "", QVariant( QString::number( 2 ) ) );

    currentIndexChangedSignal_->setMapping( textAlignment_, textAlignment_ );
    connect( textAlignment_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // primary flaskey description
    primaryTextLabel_ = new QLabel( editCellWidgets_ );
    primaryTextLabel_->setObjectName( "primaryTextLabel" );

    primaryText_ = new QLineEdit( editCellWidgets_ );
    primaryText_->setObjectName( "primaryText" );

    editingFinishedSignal_->setMapping( primaryText_, primaryText_ );
    connect( primaryText_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    // secondary flaskey description
    secondaryTextLabel_ = new QLabel( editCellWidgets_ );
    secondaryTextLabel_->setObjectName( "secondaryTextLabel" );

    secondaryText_ = new QLineEdit( editCellWidgets_ );
    secondaryText_->setObjectName( "secondaryText" );

    editingFinishedSignal_->setMapping( secondaryText_, secondaryText_ );
    connect( secondaryText_, SIGNAL( editingFinished() ), editingFinishedSignal_, SLOT( map() ) );

    // cancel button
    cancelCellEdit_ = new QPushButton( editCellWidgets_ );
    cancelCellEdit_->setFixedHeight( buttonHeight );
    cancelCellEdit_->setObjectName( "cancelCellEdit" );

    clickedSignal_->setMapping( cancelCellEdit_, cancelCellEdit_ );
    connect( cancelCellEdit_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // done button
    cellEditDone_ = new QPushButton( editCellWidgets_ );
    cellEditDone_->setFixedHeight( buttonHeight );
    cellEditDone_->setObjectName( "cellEditDone" );

    clickedSignal_->setMapping( cellEditDone_, cellEditDone_ );
    connect( cellEditDone_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // ========================================================================
    // new cell widgets
    // ========================================================================

    createNewCellWidgets_ = new QWidget( rightPane_ );
    createNewCellWidgets_->setObjectName( "createNewCellWidgets" );

    // action list
    actionList_ = new hobart::softkeyButtonListWidget( createNewCellWidgets_ );
    actionList_->setDragDropMode( QListWidget::DragOnly ); // allow only dragging (i.e. no dropping)
    actionList_->setRowHeight( buttonHeight );
    actionList_->setObjectName( "actionList_" );

    for ( int i = 1; i < action_->count(); ++i )
    {
        QString action( action_->itemData( i ).toString() );

        // add button to list
        actionList_->addSoftkeyButtonItem( action_->itemText( i ), action.toInt() );
    }

    // cancel cell create
    cancelCellCreate_ = new QPushButton( createNewCellWidgets_ );
    cancelCellCreate_->setFixedHeight( buttonHeight );
    cancelCellCreate_->setObjectName( "cancelCellCreate" );

    clickedSignal_->setMapping( cancelCellCreate_, cancelCellCreate_ );
    connect( cancelCellCreate_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // ========================================================================
    // operate mode widgets
    // ========================================================================

    operateModeWidgets_ = new QWidget( rightPane_ );
    operateModeWidgets_->setObjectName( "operateModeWidgets" );

    // best sellers list
    enableBestSellerList_ = new QCheckBox( operateModeWidgets_ );
    enableBestSellerList_->setAutoFillBackground( true );
    enableBestSellerList_->setFocusPolicy( Qt::NoFocus );
    enableBestSellerList_->setObjectName( "enableBestSellerList" );
    setCheckBoxMouseOverColor( enableBestSellerList_ );
    setTransparentBackground( enableBestSellerList_ );

    clickedSignal_->setMapping( enableBestSellerList_, enableBestSellerList_ );
    connect( enableBestSellerList_, SIGNAL( clicked() ), clickedSignal_, SLOT( map() ) );

    // operate background color
    operateBackgroundColorLabel_ = new QLabel( operateModeWidgets_ );
    operateBackgroundColorLabel_->setObjectName( "operateBackgroundColorLabel" );

    operateBackgroundColor_ = new QComboBox( operateModeWidgets_ );
    operateBackgroundColor_->setObjectName( "operateBackgroundColor" );

    operateBackgroundColor_->addItem( "", QVariant( QString::number( 0 ) ) );
    operateBackgroundColor_->addItem( "", QVariant( "orangeSolid.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "orangeGlass.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "chartreuseSolid.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "chartreuseGlass.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "greenSolid.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "greenGlass.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "paleGreenSolid.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "paleGreenGlass.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "skyBlueSolid.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "skyBlueGlass.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "blueSolid.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "blueGlass.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "purpleSolid.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "purpleGlass.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "pinkSolid.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "pinkGlass.png" ) );
    operateBackgroundColor_->addItem( "", QVariant( "custom" ) );

    currentIndexChangedSignal_->setMapping( operateBackgroundColor_, operateBackgroundColor_ );
    connect( operateBackgroundColor_, SIGNAL( currentIndexChanged(int) ), currentIndexChangedSignal_, SLOT( map() ) );

    // operate action list
    operateActionList_ = new hobart::softkeyButtonListWidget( operateModeWidgets_ );
    operateActionList_->setDragDropMode( QListWidget::DragOnly ); // allow only dragging (i.e. no dropping)
    operateActionList_->setRowHeight( buttonHeight );
    operateActionList_->setObjectName( "operateActionList" );

    for ( int i = 1; i < action_->count(); ++i )
    {
        QString action( action_->itemData( i ).toString() );

        // add button to list
        operateActionList_->addSoftkeyButtonItem( action_->itemText( i ), action.toInt() );
    }

    clickedSignal_->setMapping( operateActionList_, operateActionList_ );
    connect( operateActionList_, SIGNAL( clicked(QModelIndex) ), clickedSignal_, SLOT( map() ) );

    // waste bin
    operateRefuseBin_ = new hobart::refuseBinWidget( operateModeWidgets_ );
    operateRefuseBin_->setFixedHeight( buttonHeight );
    operateRefuseBin_->setIcon( QPixmap( ":/images/screens/refusebin.png" ) );
    operateRefuseBin_->setObjectName( "operateRefuseBin" );
    operateRefuseBin_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

    {
        QObjectList acceptedSources( operateRefuseBin_->acceptedSources() );
        acceptedSources.append( flashkeys_ );
        operateRefuseBin_->setAcceptedSources( acceptedSources );

        QStringList acceptedMimeTypes( operateRefuseBin_->acceptedMimeTypes() );
        acceptedMimeTypes.append( hobart::softkeyButton::mimeDataType() );
        operateRefuseBin_->setAcceptedMimeTypes( acceptedMimeTypes );
    }

    connect( operateRefuseBin_, SIGNAL( deleteWidget(QWidget*) ), SLOT( onFlashkeyDeleteCell(QWidget*) ) );

    // ========================================================================

    QList<QWidget*> allWidgets( findChildren<QWidget*>() );

    // update font for non-user editable fields
    foreach ( QWidget *w, allWidgets )
    {
        if (( dynamic_cast<QCheckBox*>( w ) ) ||
            ( dynamic_cast<QGroupBox*>( w ) ) ||
            ( dynamic_cast<QLabel*>( w ) ) ||
            ( dynamic_cast<QPushButton*>( w ) ))
        {
            setFontPointSize10( w );
        }
    }

    // create timers for focus handling
    setFocusActionParam_ = new QTimer( this );
    connect( setFocusActionParam_, SIGNAL( timeout() ), actionParam_, SLOT( setFocus() ) );

    setFocusActionParamNumeric_ = new QTimer( this );
    connect( setFocusActionParamNumeric_, SIGNAL( timeout() ), actionParamNumeric_, SLOT( setFocus() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::createLayout()
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::createLayout()";
#endif
    // delete existing layout
    if ( layout() )
        delete layout();

    QHBoxLayout *hlayout;
    QVBoxLayout *vlayout;

    QHBoxLayout *screenLayout = new QHBoxLayout( this );
    screenLayout->addWidget( splitter_ );

    // main pane
    QVBoxLayout *mainPaneLayout = new QVBoxLayout( mainPane_ );

    // right pane
    QVBoxLayout *rightPaneLayout = new QVBoxLayout( rightPane_ );
    rightPaneLayout->setContentsMargins( QMargins() );

    rightPaneLayout->addWidget( editGridWidgets_ );
    rightPaneLayout->addWidget( editCellWidgets_ );
    rightPaneLayout->addWidget( createNewCellWidgets_ );
    rightPaneLayout->addWidget( operateModeWidgets_ );

    // ========================================================================
    // main pane widgets
    // ========================================================================

    QHBoxLayout *top;

    // grid editing
    top = new QHBoxLayout();
    mainPaneLayout->addLayout( top );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( featureLabel_ );
    hlayout->addWidget( feature_, 1 );
    top->addLayout( hlayout, 1 );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( groupLabel_ );
    hlayout->addWidget( group_ );
    top->addLayout( hlayout, 1 );

    // cell editing
    top = new QHBoxLayout();
    mainPaneLayout->addLayout( top );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( actionLabel_ );
    hlayout->addWidget( action_, 1 );
    top->addLayout( hlayout );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( actionParamLabel_ );
    hlayout->addWidget( actionParam_, 1 );
    hlayout->addWidget( actionParamNumeric_, 1 );
    top->addLayout( hlayout, 1 );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( screenChangeAction_ );
    top->addLayout( hlayout, 1 );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( playVideoAction_ );
    top->addLayout( hlayout, 1 );
    
    hlayout = new QHBoxLayout();
    hlayout->addWidget( swipeAction_ );
    top->addLayout( hlayout, 1 );

    hlayout = new QHBoxLayout();
    hlayout->addWidget(departments_);
    top->addLayout(hlayout, 1);

    hlayout = new QHBoxLayout();
    hlayout->addWidget( totalTypesAction_ );
    top->addLayout( hlayout, 1 );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( launchExternalAppAction_ );
    top->addLayout( hlayout, 1 );

    top->addStretch();

    // flashkeys
    hlayout = new QHBoxLayout();
    hlayout->setContentsMargins( QMargins() );
    mainPaneLayout->addLayout( hlayout, 1 );

    vlayout = new QVBoxLayout();
    vlayout->setContentsMargins( QMargins() );
    hlayout->addStretch();
    hlayout->addLayout( vlayout, 1 );
    hlayout->addStretch();

    vlayout->addStretch();
    vlayout->addWidget( dynamicContent_, 1 );
    vlayout->addStretch();

    // ========================================================================
    // edit grid widgets
    // ========================================================================

    QVBoxLayout *editGridWidgetsLayout = new QVBoxLayout( editGridWidgets_ );

    editGridWidgetsLayout->addWidget( deleteGroup_ );

    editGridWidgetsLayout->addWidget( defaultGroupLabel_ );
    editGridWidgetsLayout->addWidget( defaultGroup_ );

    hlayout = new QHBoxLayout();
    vlayout = new QVBoxLayout();
    vlayout->addWidget( horzSpacingLabel_ );
    vlayout->addWidget( horzSpacing_ );
    hlayout->addLayout( vlayout );
    vlayout = new QVBoxLayout();
    vlayout->addWidget( vertSpacingLabel_ );
    vlayout->addWidget( vertSpacing_ );
    hlayout->addLayout( vlayout );
    editGridWidgetsLayout->addLayout( hlayout );

    editGridWidgetsLayout->addWidget( editCell_ );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( addRow_ );
    hlayout->addStretch();
    hlayout->addWidget( rowsLabel_ );
    hlayout->addStretch();
    hlayout->addWidget( removeRow_ );
    editGridWidgetsLayout->addLayout( hlayout );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( addColumn_ );
    hlayout->addStretch();
    hlayout->addWidget( columnsLabel_ );
    hlayout->addStretch();
    hlayout->addWidget( removeColumn_ );
    editGridWidgetsLayout->addLayout( hlayout );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( addHeight_ );
    hlayout->addStretch();
    hlayout->addWidget( heightLabel_ );
    hlayout->addStretch();
    hlayout->addWidget( removeHeight_ );
    editGridWidgetsLayout->addLayout( hlayout );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( addWidth_ );
    hlayout->addStretch();
    hlayout->addWidget( widthLabel_ );
    hlayout->addStretch();
    hlayout->addWidget( removeWidth_ );
    editGridWidgetsLayout->addLayout( hlayout );

    editGridWidgetsLayout->addWidget( createNewCell_ );

    // PJR-383
    hlayout = new QHBoxLayout();
    hlayout->addWidget( fixedRowColumnLabel_ );
    hlayout->addStretch();
    hlayout->addWidget( fixedRowColumn_ );
    editGridWidgetsLayout->addLayout( hlayout );

    hlayout = new QHBoxLayout();
    hlayout->addWidget( fixedRowColumnDefaultButtonLabel_ );
    hlayout->addStretch();
    hlayout->addWidget( fixedRowColumnDefault_ );
    editGridWidgetsLayout->addLayout( hlayout );

    fixedRowColumnLabel_->hide();
    fixedRowColumn_->hide();
    fixedRowColumnDefaultButtonLabel_->hide();
    fixedRowColumnDefault_->hide();

    editGridWidgetsLayout->addStretch();

    editGridWidgetsLayout->addWidget( refuseBin_ );

    // ========================================================================
    // edit cell widgets
    // ========================================================================

    QVBoxLayout *editCellWidgetsLayout = new QVBoxLayout( editCellWidgets_ );

    editCellWidgetsLayout->addWidget( labelPlacement1Label_ );
    editCellWidgetsLayout->addWidget( labelPlacement1_ );

    editCellWidgetsLayout->addWidget( labelRotation1Label_ );
    editCellWidgetsLayout->addWidget( labelRotation1_ );

    editCellWidgetsLayout->addWidget( labelPlacement2Label_ );
    editCellWidgetsLayout->addWidget( labelPlacement2_ );

    editCellWidgetsLayout->addWidget( labelRotation2Label_ );
    editCellWidgetsLayout->addWidget( labelRotation2_ );

    editCellWidgetsLayout->addWidget( printerLabel_ );
    editCellWidgetsLayout->addWidget( printer_ );

    editCellWidgetsLayout->addWidget( webPageParameters_ );

    hlayout = new QHBoxLayout( webPageParameters_ );
    vlayout = new QVBoxLayout();
    vlayout->addWidget( webPageUsesStoreNumber_ );
    vlayout->addWidget( webPageUsesPlu_ );
    vlayout->addWidget( webPageUsesQty_ );
    vlayout->addWidget( webPageUsesAll_ );
    hlayout->addLayout( vlayout, 1 );
    vlayout = new QVBoxLayout();
    vlayout->addWidget( webPageUsesDepartment_ );
    vlayout->addWidget( webPageUsesLang_ );
    vlayout->addWidget( webPageUsesModel_ );
    vlayout->addWidget( webPageUsesNone_ );
    hlayout->addLayout( vlayout, 1 );

    editCellWidgetsLayout->addWidget( trayLengthLabel_ );
    editCellWidgetsLayout->addWidget( trayLength_ );

    editCellWidgetsLayout->addWidget( trayWidthLabel_ );
    editCellWidgetsLayout->addWidget( trayWidth_ );

    editCellWidgetsLayout->addWidget( trayLipHeightLabel_ );
    editCellWidgetsLayout->addWidget( trayLipHeight_ );

    editCellWidgetsLayout->addWidget( displayLocationLabel_ );
    editCellWidgetsLayout->addWidget( displayLocation_ );

    editCellWidgetsLayout->addWidget( imageLabel_ );
    editCellWidgetsLayout->addWidget( image_ );

    // PJR-383
    editCellWidgetsLayout->addWidget( toggledImageLabel_ );
    editCellWidgetsLayout->addWidget( toggledImage_ );

    editCellWidgetsLayout->addWidget( backgroundColorLabel_ );
    editCellWidgetsLayout->addWidget( backgroundColor_ );

    editCellWidgetsLayout->addWidget( textColorLabel_ );
    editCellWidgetsLayout->addWidget( textColor_ );

    editCellWidgetsLayout->addWidget( textAlignmentLabel_ );
    editCellWidgetsLayout->addWidget( textAlignment_ );

    editCellWidgetsLayout->addWidget( primaryTextLabel_ );
    editCellWidgetsLayout->addWidget( primaryText_ );

    editCellWidgetsLayout->addWidget( secondaryTextLabel_ );
    editCellWidgetsLayout->addWidget( secondaryText_ );

    editCellWidgetsLayout->addWidget( pyScriptLabel_ );
    editCellWidgetsLayout->addWidget( pyScript_ );

    editCellWidgetsLayout->addWidget( pyScriptIntervalLabel_ );
    editCellWidgetsLayout->addWidget( pyScriptInterval_ );

    editCellWidgetsLayout->addStretch();

    hlayout = new QHBoxLayout();
    hlayout->addWidget( cancelCellEdit_ );
    hlayout->addWidget( cellEditDone_ );
    editCellWidgetsLayout->addLayout( hlayout );

    // ========================================================================
    // new cell widgets
    // ========================================================================

    QVBoxLayout *createNewCellWidgetsLayout = new QVBoxLayout( createNewCellWidgets_ );

    createNewCellWidgetsLayout->addWidget( actionList_, 1 );
    createNewCellWidgetsLayout->addWidget( cancelCellCreate_ );

    // ========================================================================
    // operate mode widgets
    // ========================================================================

    QVBoxLayout *operateModeWidgetsLayout = new QVBoxLayout( operateModeWidgets_ );

    operateModeWidgetsLayout->addWidget( enableBestSellerList_ );

    operateModeWidgetsLayout->addWidget( operateBackgroundColorLabel_ );
    operateModeWidgetsLayout->addWidget( operateBackgroundColor_ );

    operateModeWidgetsLayout->addWidget( operateActionList_, 1 );

    operateModeWidgetsLayout->addWidget( operateRefuseBin_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setCurrentSoftkey(const DomSoftkey& m)
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::setCurrentSoftkey()";
#endif
    LOG_CLASS_INFO( "FlashkeyEditor", "setCurrentSoftkey" );

    int index;

    // action
    action_->blockSignals( true );
    index = action_->findData( QString::number( m.action() ) );
    action_->setCurrentIndex( qMax( 0, index ) );
    action_->blockSignals( false );

    // action parameter
    actionParam_->blockSignals( true );
    actionParam_->setValidator( NULL );
    if (( 0 < (index = m.actionParam().indexOf( "?" )) ) &&
        (( Action_FPP_URL == m.action() ) || ( Action_Application_URL == m.action() ))) // FT-1110 only applies to FPP and Store Applications
        actionParam_->setText( m.actionParam().left( index ) );
    else if ( Action_SelectProduct == m.action() )
        actionParam_->setText( ("-" == m.actionParam().left( 1 )) ? "0" : m.actionParam() );
    else
    {
        if ( Action_LoadTareSoftkey == m.action() )
        {
            QString tare = m.actionParam();

            actionParam_->setText( tare );
        }
        else
        {
            actionParam_->setText( m.actionParam() );           // DID THIS
        }
    }

    actionParam_->blockSignals( false );

    actionParamNumeric_->blockSignals( true );
    actionParamNumeric_->setValidator( NULL );
    actionParamNumeric_->fromSessionField( m.actionParam() );
    actionParamNumeric_->blockSignals( false );

    // screen change action
    screenChangeAction_->blockSignals( true );
    if ( Action_ChangeScreen != m.action() )
        screenChangeAction_->setCurrentIndex( 0 );
    else
    {
        index = screenChangeAction_->findData( m.actionParam() );
        screenChangeAction_->setCurrentIndex( qMax( 0, index ) );
    }
    screenChangeAction_->blockSignals( false );

    // play video action
    playVideoAction_->blockSignals( true );
    if (( Action_PlayVideo != m.action() ) || ( m.actionStrParam().isEmpty() ))
        playVideoAction_->setCurrentIndex( 0 );
    else
    {
        index = playVideoAction_->findData( m.actionStrParam() );
        if ( index < 0 )
        {
            playVideoAction_->addItem( m.actionStrParam(), m.actionStrParam() );
            index = playVideoAction_->count() - 1;
        }
        playVideoAction_->setCurrentIndex( qMax( 0, index ) );
    }
    playVideoAction_->blockSignals( false );

    // swipe action
    swipeAction_->blockSignals( true );
    if (( Action_Swipe != m.action() ) || ( m.actionParam().isEmpty() ))
        swipeAction_->setCurrentIndex( 0 );
    else
    {
        index = swipeAction_->findData( m.actionParam() );
        swipeAction_->setCurrentIndex( qMax( 0, index ) );
    }
    swipeAction_->blockSignals( false );

    // change departments action
    departments_->blockSignals( true );
    if (( Action_ChangeDepartment != m.action() ) || ( m.actionParam().isEmpty() ))
        departments_->setCurrentIndex( 0 );
    else
    {
        index = departments_->findData( m.actionParam() );
        departments_->setCurrentIndex( qMax( 0, index ) );
    }
    departments_->blockSignals( false );


    // total types action
    totalTypesAction_->blockSignals( true );
    if (( 29 != m.action() ) || ( m.actionParam().isEmpty() ))
        totalTypesAction_->setCurrentIndex( 0 );
    else
    {
        index = totalTypesAction_->findData( m.actionParam() );
        if ( index < 0 )
        {
            totalTypesAction_->addItem( m.actionParam(), m.actionParam() );
            index = totalTypesAction_->count() - 1;
        }
        totalTypesAction_->setCurrentIndex( qMax( 0, index ) );
    }
    totalTypesAction_->blockSignals( false );

    // launch external application action
    launchExternalAppAction_->blockSignals( true );
    if (( Action_LaunchExternalApp != m.action() ) || ( m.actionStrParam().isEmpty() ))
        launchExternalAppAction_->setCurrentIndex( 0 );
    else
    {
        index = launchExternalAppAction_->findData( m.actionStrParam() );
        if ( index < 0 )
        {
            launchExternalAppAction_->addItem( m.actionStrParam(),  m.actionStrParam() );
            index = launchExternalAppAction_->count() - 1;
        }
        launchExternalAppAction_->setCurrentIndex( qMax( 0, index ) );
    }
    launchExternalAppAction_->blockSignals( false );

    // label placement
    // label rotation
    int labelPlacement1 = 0;
    int labelRotation1 = 0;
    int labelPlacement2 = 0;
    int labelRotation2 = 0;

    /*
    if (( Action_ChangeLabelPlacementAndPosition == m.action() ) && ( !m.actionStrParam().isEmpty() ))
    {
        // offset 0 = printer
        // offset 1 = label placement
        // offset 2 = label rotation
        QStringList params( m.actionStrParam().split( "," ) );

        if ( "R" == params[1] )
            labelPlacement1 = 1;
        else if ( "L" == params[1] )
            labelPlacement1 = 2;

        QMap<QString, int> rotMapping;
        rotMapping["NC"] = 0;
        rotMapping["0"] = 1;
        rotMapping["45"] = 2;
        rotMapping["90"] = 3;
        rotMapping["135"] = 4;
        rotMapping["180"] = 5;
        rotMapping["225"] = 6;
        rotMapping["270"] = 7;
        rotMapping["315"] = 8;

        if ( rotMapping.contains( params[2] ) )
            labelRotation1 = rotMapping[ params[2] ];
    }
    */
    if (( Action_ChangeLabelPlacementAndPosition == m.action() ) && ( !m.actionStrParam().isEmpty() ))
    {
        const int value = m.actionStrParam().toInt();

        // old way (single printer)
        if ( value < 0x1000000 )
        {
            labelRotation1 = (value % 27) / 3;
            labelPlacement1 = (value % 27) % 3;
        }
        // new way (up to three printers)
        else
        {
            labelRotation1 = value & 0xf;
            labelPlacement1 = (value & 0xf0) >> 4;
            labelRotation2 = (value & 0xf00) >> 8;
            labelPlacement2 = (value & 0xf000) >> 12;
        }
    }

    labelPlacement1_->blockSignals( true );
    index = labelPlacement1_->findData( labelPlacement1 );
    labelPlacement1_->setCurrentIndex( qMax( 0, index ) );
    labelPlacement1_->blockSignals( false );

    labelRotation1_->blockSignals( true );
    labelRotation1_->setCurrentIndex( qMax( 0, labelRotation1 ) );
    labelRotation1_->blockSignals( false );

    labelPlacement2_->blockSignals( true );
    index = labelPlacement2_->findData( labelPlacement2 );
    labelPlacement2_->setCurrentIndex( qMax( 0, index ) );
    labelPlacement2_->blockSignals( false );

    labelRotation2_->blockSignals( true );
    labelRotation2_->setCurrentIndex( qMax( 0, labelRotation2 ) );
    labelRotation2_->blockSignals( false );

    printer_->blockSignals( true );
	if (( Action_ChangeLabelType != m.action() ) || ( m.actionStrParam().isEmpty() ))
    {
        printer_->setCurrentIndex( 0 );
        labelTypeNum_ = "0";

#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::setCurrentSoftkey(Action_ChangeLabelType):  labelTypeNum_ = " << labelTypeNum_;
#endif
        LOG_WARN( "a) labelTypeNum_ = %s", qPrintable( labelTypeNum_ ) );
    }
    else
    {
        // offset 0 = printer
        // offset 1 = label type
        QStringList params( m.actionStrParam().split( "," ) );
        if (params.size() > 0)
        {
            index = printer_->findData( params[0] );
            printer_->setCurrentIndex( qMax( 0, index ) );

            if ( params.size() > 1 )
                labelTypeNum_ = params[1];
            else
                labelTypeNum_ = "0";

#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::setCurrentSoftkey(Action_ChangeLabelType):  printer = " << params[0] << ", labelType = " << labelTypeNum_;
#endif
	        LOG_WARN( "b) labelTypeNum_ = %s", qPrintable( labelTypeNum_ ) );

		    actionParam_->blockSignals( true );
		    actionParam_->setText(labelTypeNum_);
		    actionParam_->blockSignals( false );
        }
	}
    printer_->blockSignals( false );

    // cgi parameters
    bool webPageUsesStoreNumber = false;
    bool webPageUsesDepartment = false;
    bool webPageUsesPlu = false;
    bool webPageUsesQty = false;
    bool webPageUsesLang = false;
    bool webPageUsesModel = false;

    if ( 0 < (index = m.actionParam().indexOf( "?" )) )
        if (( Action_FPP_URL == m.action() ) || ( Action_Application_URL == m.action() )) // FT-1110 only applies to FPP and Store Applications
        {
            webPageUsesStoreNumber = (0 < m.actionParam().indexOf( "store", index ));
            webPageUsesDepartment = (0 < m.actionParam().indexOf( "dept", index ));
            webPageUsesPlu = (0 < m.actionParam().indexOf( "plu", index ));
            webPageUsesQty = (0 < m.actionParam().indexOf( "qty", index ));
            webPageUsesLang = (0 < m.actionParam().indexOf( "lang", index ));
            webPageUsesModel = (0 < m.actionParam().indexOf( "model", index ));
        }

    webPageUsesStoreNumber_->blockSignals( true );
    webPageUsesStoreNumber_->setChecked( webPageUsesStoreNumber );
    webPageUsesStoreNumber_->blockSignals( false );

    webPageUsesDepartment_->blockSignals( true );
    webPageUsesDepartment_->setChecked( webPageUsesDepartment );
    webPageUsesDepartment_->blockSignals( false );

    webPageUsesPlu_->blockSignals( true );
    webPageUsesPlu_->setChecked( webPageUsesPlu );
    webPageUsesPlu_->blockSignals( false );

    webPageUsesQty_->blockSignals( true );
    webPageUsesQty_->setChecked( webPageUsesQty );
    webPageUsesQty_->blockSignals( false );

    webPageUsesLang_->blockSignals( true );
    webPageUsesLang_->setChecked( webPageUsesLang );
    webPageUsesLang_->blockSignals( false );

    webPageUsesModel_->blockSignals( true );
    webPageUsesModel_->setChecked( webPageUsesModel );
    webPageUsesModel_->blockSignals( false );

    // override wrapper tray dimensions
    trayLength_->blockSignals( true );
    trayWidth_->blockSignals( true );
    trayLipHeight_->blockSignals( true );

    trayLength_->fromInt( 0 );
    trayWidth_->fromInt( 0 );
    trayLipHeight_->fromInt( 0 );

    if ( Action_OverrideWrapperCameraDimensions == m.action() )
    {
        const QStringList params( m.actionStrParam().split( "_" ) );

        if (( 3 <= params.size() ) && ( params[2].length() ))
            trayLipHeight_->fromInt( params[2].toInt() );

        if (( 2 <= params.size() ) && ( params[1].length() ))
            trayWidth_->fromInt( params[1].toInt() );

        if (( 1 <= params.size() ) && ( params[0].length() ))
            trayLength_->fromInt( params[0].toInt() );
    }

    trayLength_->blockSignals( false );
    trayWidth_->blockSignals( false );
    trayLipHeight_->blockSignals( false );

    // display location
    displayLocation_->blockSignals( true );
    index = displayLocation_->findData( QString::number( m.actionFeature() ) );
    displayLocation_->setCurrentIndex( qMax( 0, index ) );
    displayLocation_->blockSignals( false );

    // image
    image_->blockSignals( true );
    if ( m.useDefBgImage() )
        image_->setCurrentIndex( 1 );
    else
    {
        QString filename( m.bgImage() );

        if ( 0 < (index = filename.lastIndexOf( "\\" )) )
            filename = filename.mid( index + 1 );
        else if ( 0 < (index = filename.lastIndexOf( "/" )) )
            filename = filename.mid( index + 1 );

        index = image_->findData( filename );
        image_->setCurrentIndex( qMax( 0, index ) );
    }
    image_->blockSignals( false );

    //python script
    pyScript_->blockSignals( true );
    if ( m.scriptName() == "" )
        pyScript_->setCurrentIndex( 0 );
    else
    {
        index = pyScript_->findData( m.scriptName() );
        pyScript_->setCurrentIndex( index );
    }
    pyScript_->blockSignals( false );

    //python script interval
    pyScriptInterval_->blockSignals( true );
    if ( m.scriptInterval() != 0 )
        pyScriptInterval_->fromInt( m.scriptInterval()/1000 );
    else
        pyScriptInterval_->fromInt( 0 );
    pyScriptInterval_->blockSignals( false );

    // PJR-383: Toggled Softkey Image
    toggledImage_->blockSignals( true );
//    if ( m.useDefBgImage() )
//        toggledImage_->setCurrentIndex( 1 );
//    else
    {
        QString filename( m.bgToggledImage() );

        if ( 0 < (index = filename.lastIndexOf( "\\" )) )
            filename = filename.mid( index + 1 );
        else if ( 0 < (index = filename.lastIndexOf( "/" )) )
            filename = filename.mid( index + 1 );

        index = toggledImage_->findData( filename );
        toggledImage_->setCurrentIndex( qMax( 0, index ) );
    }
    toggledImage_->blockSignals( false );

    // background color
    backgroundColor_->blockSignals( true );
    if (( m.useDefBgColor() ) || ( m.bgColor().isEmpty() ))
        backgroundColor_->setCurrentIndex( backgroundColorIndex_ = 0 );
    else
    {
        index = backgroundColor_->findData( QVariant( m.bgColor() ) );
        if ( 0 <= index )
            backgroundColorIndex_ = index;
        else
        {
            backgroundColor_->addItem( m.bgColor().toUpper(), QVariant( m.bgColor() ) );
            backgroundColorIndex_ = backgroundColor_->count() - 1;
        }
        backgroundColor_->setCurrentIndex( backgroundColorIndex_ );
    }
    backgroundColor_->blockSignals( false );

    // background color (for operate mode)
    operateBackgroundColor_->blockSignals( true );

    operateBackgroundColor_->setCurrentIndex( operateBackgroundColorIndex_ = 0 );

    if ( m.bgImage().length() )
    {
        QString filename( m.bgImage() );

        if ( 0 < (index = filename.lastIndexOf( "\\" )) )
            filename = filename.mid( index + 1 );
        else if ( 0 < (index = filename.lastIndexOf( "/" )) )
            filename = filename.mid( index + 1 );

        index = operateBackgroundColor_->findData( QVariant( filename ) );
        if ( 0 <= index )
            operateBackgroundColorIndex_ = index;
        else
        {
            operateBackgroundColor_->addItem( filename, QVariant( filename ) );
            operateBackgroundColorIndex_ = operateBackgroundColor_->count() - 1;
        }
        operateBackgroundColor_->setCurrentIndex( operateBackgroundColorIndex_ );
    }
    else if (( !m.useDefBgColor() ) && ( m.bgColor().length() ))
    {
        index = operateBackgroundColor_->findData( QVariant( m.bgColor() ) );
        if ( 0 <= index )
            operateBackgroundColorIndex_ = index;
        else
        {
            operateBackgroundColor_->addItem( m.bgColor().toUpper(), QVariant( m.bgColor() ) );
            operateBackgroundColorIndex_ = operateBackgroundColor_->count() - 1;
        }
        operateBackgroundColor_->setCurrentIndex( operateBackgroundColorIndex_ );
    }

    operateBackgroundColor_->blockSignals( false );

    // text color
    textColor_->blockSignals( true );
    if (( m.useDefTextColor() ) || ( m.textColor().isEmpty() ))
        textColor_->setCurrentIndex( textColorIndex_ = 0 );
    else
    {
        index = textColor_->findData( QVariant( m.textColor() ) );
        if ( 0 <= index )
            textColorIndex_ = index;
        else
        {
            textColor_->addItem( m.textColor().toUpper(), QVariant( m.textColor() ) );
            textColorIndex_ = textColor_->count() - 1;
        }
        textColor_->setCurrentIndex( textColorIndex_ );
    }
    textColor_->blockSignals( false );

    // text alignment
    textAlignment_->blockSignals( true );
    index = textAlignment_->findData( QString::number( flashkeys_->textAlignment() ) );
    textAlignment_->setCurrentIndex( qMax( 0, index ) );
    textAlignment_->blockSignals( false );

    // primary text
    primaryText_->blockSignals( true );
    primaryText_->setText( m.useDefName1() ? m.defName1() : m.name1() );
    primaryText_->blockSignals( false );

    // secondary text
    secondaryText_->blockSignals( true );
    secondaryText_->setText( m.useDefName2() ? m.defName2() : m.name2() );
    secondaryText_->blockSignals( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setCurrentSoftkeyDefaultDescription( hobart::softkeyButton *flashkey )
{
    LOG_CLASS_INFO( "FlashkeyEditor", "setCurrentSoftkeyDefaultDescription" );

    QString textPri( flashkey->text() );
    QString textSec( flashkey->text() );

    // check for description already set
    if ( !textPri.isEmpty() )
    {
        LOG_DEBUG( "softkey already has description", NULL );
        return;
    }

    QString priLang( session_->scaleConfig->getLanguageCode() + session_->scaleConfig->getCountryCode() );
    QString secLang( session_->scaleConfig->getSecLanguageCode() + session_->scaleConfig->getSecCountryCode() );

    LOG_DEBUG( "primary language is '%s'", qPrintable( priLang ) );
    LOG_DEBUG( "secondary language is '%s'", qPrintable( secLang ) );

    QString action( action_->itemData( action_->currentIndex() ).toString() );
    QString actionParam( actionParam_->text() );

    if ( actionParamNumeric_->isVisible() )
        actionParam = actionParamNumeric_->text();

    QString translateMe;
    int index;

    // text from action
    //
    // NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
    //
    // FT-914
    // Do *NOT* use tr(), use translate()... we should translate text into primary and
    // secondary languages here! Then also set translateMe to the tr() string so it gets picked
    // up by lupdate and Joe can translate it.
    //
    // NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
    switch ( action.toInt() )
    {
    case Action_ChangeScreen: // Change screen
        index = screenChangeAction_->itemData( screenChangeAction_->currentIndex() ).toInt();
        textPri = translate( priLang, screenChangeActions_[index] );
        textSec = translate( secLang, screenChangeActions_[index] );
        break;

    case Action_ApplyDiscount_CentsOff: // Cents Off Disc.
        textPri = actionParam + translate( priLang, " Cents Off" );
        textSec = actionParam + translate( secLang, " Cents Off" );

        // dummy string so string gets picked up by lupdate
        translateMe = tr( " Cents Off" );
        break;

    case Action_ApplyDiscount_PercentOff: // Percent Off Disc.
        textPri = actionParam + translate( priLang, "% Off" );
        textSec = actionParam + translate( secLang, "% Off" );

        // dummy string so string gets picked up by lupdate
        translateMe = tr( "% Off" );
        break;

    case Action_LoadSpecials_First: // Load Specials
        textPri = translate( priLang, "Specials" );
        textSec = translate( secLang, "Specials" );

        // dummy string so string gets picked up by lupdate
        translateMe = tr( "Specials" );
        break;

    case Action_LoadSearch_First: // Load Search
        textPri = translate( priLang, "Search" );
        textSec = translate( secLang, "Search" );

        // dummy string so string gets picked up by lupdate
        translateMe = tr( "Search" );
        break;

    case Action_PrintExistingExpirationLabel: // Print Existing Shelf Life Label
        textPri = translate( priLang, "Existing Shelf Life Label" );
        textSec = translate( secLang, "Existing Shelf Life Label" );

        // dummy string so string gets picked up by lupdate
        translateMe = tr( "Existing Shelf Life Label" );
        break;

    case Action_PrintNewExpirationLabel: // Print New Shelf Life Label
        textPri = translate( priLang, "New Shelf Life Label" );
        textSec = translate( secLang, "New Shelf Life Label" );

        // dummy string so string gets picked up by lupdate
        translateMe = tr( "New Shelf Life Label" );
        break;

    case Action_LoadTareSoftkey: // Preset tare
        if ( QString::number( hobart::softkeysWidget::PresetTareGrid ) == currentFeature() )
            textPri = textSec = "(" + actionParam + ")";
        else
        {
            textPri = actionParam + translate( priLang, " Tare" );
            textSec = actionParam + translate( secLang, " Tare" );

            // dummy string so string gets picked up by lupdate
            translateMe = tr( " Tare" );
        }
        break;

    case Action_PlayVideo: // Play Video
        textPri = translate( priLang, "Play " ) + playVideoAction_->itemText( playVideoAction_->currentIndex() );
        textSec = translate( secLang, "Play " ) + playVideoAction_->itemText( playVideoAction_->currentIndex() );

        // dummy string so string gets picked up by lupdate
        translateMe = tr( "Play " );
        break;

    default:
        index = action_->itemData( action_->currentIndex() ).toInt();
        textPri = translate( priLang, actions_[index] );
        textSec = translate( secLang, actions_[index] );
        break;
    }

    // no text
    if ( textPri.isEmpty() )
    {
        LOG_WARN( "no default text for action %s param '%s'", qPrintable( action ), qPrintable( actionParam ) );
        return;
    }

    // FT-915 flashkey text should be in uppercase (to be consistent)
    textPri = textPri.toUpper();
    textSec = textSec.toUpper();

    if ( 2 == session_->scaleConfig->currentLanguage )
        flashkey->setText( textSec );
    else
        flashkey->setText( textPri );

    primaryText_->blockSignals( true );
    primaryText_->setText( textPri );
    primaryText_->blockSignals( false );

    secondaryText_->blockSignals( true );
    secondaryText_->setText( textSec );
    secondaryText_->blockSignals( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setFocusActionParam( int msec )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::setFocusActionParam(" << msec << ")";
#endif
    if ( actionParam_->isVisible() )
    {
        setFocusActionParam_->setSingleShot( true );
        setFocusActionParam_->start( msec );
    }
    else if ( actionParamNumeric_->isVisible() )
    {
        setFocusActionParamNumeric_->setSingleShot( true );
        setFocusActionParamNumeric_->start( msec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setScreenEnabled( bool enabled )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::setScreenEnabled(" << enabled << ")";
#endif
    QList<QWidget*> allWidgets;

    // only interested in container widgets
    allWidgets.append( mainPane_->findChildren<QWidget*>() );
    allWidgets.append( editGridWidgets_->findChildren<QWidget*>() );
    allWidgets.append( createNewCellWidgets_->findChildren<QWidget*>() );
    allWidgets.append( operateModeWidgets_->findChildren<QWidget*>() );

    // set property
    foreach ( QWidget *w, allWidgets )
    {
        bool isEnabled( enabled );

        // these widgets to not enable unless a flashkey is currently selected
        if (( w == editCell_ ) ||
            ( w == heightLabel_ ) ||
            ( w == addHeight_ ) ||
            ( w == removeHeight_ ) ||
            ( w == widthLabel_ ) ||
            ( w == addWidth_ ) ||
            ( w == removeWidth_ ) ||
            ( w == operateBackgroundColorLabel_ ) ||
            ( w == operateBackgroundColor_ ))
        {
            isEnabled &= (flashkeys_->selected() ? true : false);
        }

        // these widgets disabled for default groups
        else if ( w == deleteGroup_ )
        {
            int group( currentGroup().toInt() );

            if (( hobart::softkeysWidget::ProductGrid == group ) ||
                ( hobart::softkeysWidget::RunScreenButtons == group ) ||
                ( hobart::softkeysWidget::PresetTareGrid == group ) ||
                ( hobart::softkeysWidget::MostCommonlyUsedItems == group ) ||
                ( hobart::softkeysWidget::SelfservCategoryButtons == group ) ||
                ( hobart::softkeysWidget::SelfservProductGrid == group ) ||
                ( hobart::softkeysWidget::FixedRowColumnGrid == group ) ||
                ( hobart::softkeysWidget::RunScreenNumericKeypadButtons == group ) ||
                ( hobart::softkeysWidget::RunScreenCutTestOn == group ) ||
                ( hobart::softkeysWidget::RunScreenCutTestOnOptionOne == group ) ||
                ( hobart::softkeysWidget::ForcedCameraOverride == group ) ||
                ( currentGroup() == featureDefaultGroupNum_ ))
            {
                isEnabled = false;
            }
        }

        // FT-669
        // these widgets disabled for specific features
        else if ( w == enableBestSellerList_ )
        {
            int feature( currentFeature().toInt() );

            if (( hobart::softkeysWidget::ProductGrid != feature ) &
                ( hobart::softkeysWidget::RunScreenButtons != feature ) &
                ( hobart::softkeysWidget::MostCommonlyUsedItems != feature ) &
                ( hobart::softkeysWidget::SelfservCategoryButtons != feature ) &
                ( hobart::softkeysWidget::SelfservProductGrid != feature ) &
                ( hobart::softkeysWidget::FixedRowColumnGrid != feature ) )
            {
                isEnabled = false;
            }
        }

        w->setEnabled( isEnabled );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setActionParamLabelText()
{
    QString actionParamText( tr( "Action Param" ) );
    QString action( action_->itemData( action_->currentIndex() ).toString() );
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::setActionParamLabelText():  action = " << action << ")";
#endif

    // text from action type
    switch ( action.toInt() )
    {
    case Action_BrowseWeb:	// Browse Web
        actionParamText = tr( "Web page" );
        break;

    case Action_LoginOperator: // Operator Login
        actionParamText = tr( "Operator ID" );
        break;

    case Action_SelectProduct: // Select Product
        actionParamText = tr( "PLU number" );
        break;

    case Action_LoadSoftkeyGroup: // Load Softkey Group
        actionParamText = tr( "Group number" );
        break;

    case Action_LoadCategory_First: // Load Category
        actionParamText = tr( "Category number" );
        break;

    case Action_ApplyDiscount_CentsOff: // Cents Off Disc.
        actionParamText = tr( "Cents off" );
        break;

    case Action_ApplyDiscount_PercentOff: // Percent Off Disc.
        actionParamText = tr( "Percent off" );
        break;

    case Action_LoadSearch_First: // Load Search
        actionParamText = tr( "Search string" );
        break;

    case Action_FPP_URL: // Production Planning
        actionParamText = tr( "FPP URL" );
        break;

    case Action_Application_URL: // Store Applications
        actionParamText = tr( "Applications URL" );
        break;

    case Action_LoadTareSoftkey: // Preset tare
        actionParamText = tr( "Tare value" );
        break;

    case Action_ChangeLabelType: // Change Label type
        actionParamText = tr( "Label type" );
        break;
    
    case Action_Swipe:
        actionParamText = tr( "Swipe Direction" );
        break;

    default:
        break;
    }

    // set text
    actionParamLabel_->setText( actionParamText );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setColorComboBoxItemText( QComboBox *w )
{
    setComboBoxItemTextFromData( w, 0, tr( "Default" ) );
    setComboBoxItemTextFromData( w, "000099", tr( "Hobart Blue" ) );
    setComboBoxItemTextFromData( w, "000000", tr( "Black" ) );
    setComboBoxItemTextFromData( w, "000080", tr( "Navy" ) );
    setComboBoxItemTextFromData( w, "0000FF", tr( "Blue" ) );
    setComboBoxItemTextFromData( w, "00FFFF", tr( "Aqua" ) );
    setComboBoxItemTextFromData( w, "800080", tr( "Purple" ) );
    setComboBoxItemTextFromData( w, "FF00FF", tr( "Fuchsia" ) );
    setComboBoxItemTextFromData( w, "800000", tr( "Maroon" ) );
    setComboBoxItemTextFromData( w, "FF0000", tr( "Red" ) );
    setComboBoxItemTextFromData( w, "808000", tr( "Olive" ) );
    setComboBoxItemTextFromData( w, "FFFF00", tr( "Yellow" ) );
    setComboBoxItemTextFromData( w, "008000", tr( "Lime" ) );
    setComboBoxItemTextFromData( w, "00FF00", tr( "Green" ) );
    setComboBoxItemTextFromData( w, "008080", tr( "Teal" ) );
    setComboBoxItemTextFromData( w, "808080", tr( "Gray" ) );
    setComboBoxItemTextFromData( w, "C0C0C0", tr( "Silver" ) );
    setComboBoxItemTextFromData( w, "FFFFFF", tr( "White" ) );
    setComboBoxItemTextFromData( w, "custom", tr( "Custom..." ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::showMode( EditMode mode )
{
    bool isGridEditing = (( GridEditMode == mode ) || ( CreateNewCellMode == mode ));

    if ( mainWindow_->isOperateMode() )
    {
        // only show feature when something to pick from
        featureLabel_->setVisible(( isGridEditing ) && ( 1 < feature_->count() ));
        feature_->setVisible(( isGridEditing ) && ( 1 < feature_->count() ));
        groupLabel_->setVisible( false );
        group_->setVisible( false );
    }
    else
    {
        featureLabel_->setVisible( isGridEditing );
        feature_->setVisible( isGridEditing );
        groupLabel_->setVisible( isGridEditing );
        group_->setVisible( isGridEditing );
    }

    actionLabel_->setVisible( CellEditMode == mode  );
    action_->setVisible( CellEditMode == mode  );
    actionParamLabel_->setVisible( false ); // not visible until action has been chosen
    actionParam_->setVisible( false ); // not visible until action has been chosen
    actionParamNumeric_->setVisible( false ); // not visible until action has been chosen
    screenChangeAction_->setVisible( false ); // not visible until action has been chosen
    playVideoAction_->setVisible( false ); // not visible until action has been chosen
    swipeAction_->setVisible( false ); // not visible until action has been chosen
    departments_->setVisible(false);
    totalTypesAction_->setVisible( false ); // not visible until action has been chosen
    launchExternalAppAction_->setVisible( false ); // not visible until action has been chosen

    if ( mainWindow_->isOperateMode() )
    {
        editGridWidgets_->setVisible( false );
        editCellWidgets_->setVisible( CellEditMode == mode );
        createNewCellWidgets_->setVisible( false );
        operateModeWidgets_->setVisible( CellEditMode != mode );
    }
    else
    {
        editGridWidgets_->setVisible( GridEditMode == mode );
        editCellWidgets_->setVisible( CellEditMode == mode );
        createNewCellWidgets_->setVisible( CreateNewCellMode == mode );
        operateModeWidgets_->setVisible( false );
    }

    // reset any flashkey selection
    if ( isGridEditing )
    {
        flashkeys_->resetSelection();

        isMoveSoftkey_ = isMoveSoftkeySetVisible_ = false;
        isNewSoftkey_ = false;
        isUpdateSoftkey_ = false;
    }

    // show only fields pertaining to the flashkey action
    else if ( CellEditMode == mode )
    {
        hobart::softkeyButton *flashkey( flashkeys_->selected() );
        
        //disables flashkey overlays on flashkey editor, edit cell screen
        flashkey->disableOverlay(true);

        if ( flashkey )
            showActionFields( selected_.action(), false );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::showActionFields( int action, bool isNewAction )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::showActionFields(" << action << ", " << isNewAction << ")";
#endif
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );
    MadpCurrencySettings *currencySettings( &session_->scaleConfig->currencySettings );
    DomNormalScaleOperation *normalScaleOperation( &session_->scaleConfig->normalScaleOperation );

    int currencyPrecision = currencySettings->primaryPrecision();
    int currencyDigits = currencySettings->unitPriceDigits();

    int currencyBottom = 0;
    int currencyTop = 0;

    // HT-193 the number of decimal places will be strictly based on the Scale Tare Precision, not weigher models, etc.
    int tarePrecision = qMax( 2, scaleConfig->tarePrecision() );

    QString tare = actionParam_->text();
    QString tareNum = actionParamNumeric_->text();

    int tareBottom = 0;
    int tareTop = (scaleConfig->isScaleMetric() ? 15 : 30) * (2 == tarePrecision ? 100 : 1000);

    // compute currency top
    for ( int i = 0; i < currencyDigits; ++i )
    {
        currencyTop *= 10;
        currencyTop += 9;
    }

    // compute tare top
    if ( normalScaleOperation->maxTare().length() )
    {
        bool okay = false;
        double maxTare = normalScaleOperation->maxTare().toDouble( &okay );

        if ( okay )
            tareTop = (int)(maxTare * (2 == tarePrecision ? 100.0 : 1000.0));
    }

    // ========================================================================
    // hide/show fields as per action type
    // ========================================================================

    bool showActionParam = false;
    bool showActionParamNumeric = false;
    bool showScreenChangeAction = false;
    bool showPlayVideoAction = false;
    bool showSwipeAction = false;
    bool showTotalTypesAction = false;
    bool showLaunchExternalAppAction = false;

    bool hideEditCellWidgets = false;
    bool showLabelPlacementAndRotation = false;
    bool showPrinterSelection = false;
    bool showWebPageParameters = false;
    bool showTrayDimensions = false;
    bool showDisplayLocation = false;
    bool showChangeDepartmentAction = false;

    QValidator *actionParamValidator = NULL;

    switch ( action )
    {
    case Action_ChangeScreen: // Change screen
        showScreenChangeAction = true;
        break;

    case Action_BrowseWeb: // Browse Web
        showActionParam = true;
        break;

    case Action_LoginOperator: // Operator Login
        showActionParam = true;
        actionParamValidator = new QRegExpValidator( QRegExp( "[\\d]{1,4}" ), this );
        break;

    case Action_SelectProduct: // Select Product
        showActionParam = true;
        actionParamValidator = new QRegExpValidator( QRegExp( "[\\d]{1,6}" ), this );
        hideEditCellWidgets = isNewAction; // user must choose product first
        break;

    case Action_LoadSoftkeyGroup: // Load Softkey Group
        showActionParam = true;
        actionParamValidator = new QRegExpValidator( QRegExp( "[\\d]{1,6}" ), this );
        showDisplayLocation = true;
        break;

    case Action_LoadCategory_First: // Load Category
        showActionParam = true;
        actionParamValidator = new QRegExpValidator( QRegExp( "[\\d]{1,6}" ), this );
        showDisplayLocation = true;
        hideEditCellWidgets = isNewAction; // user must choose category first
        break;

    case Action_ApplyDiscount_CentsOff: // Cents Off Disc.
        showActionParamNumeric = true;
        actionParamNumeric_->setDecimal( currencySettings->unitSeparator() );
        actionParamNumeric_->setDecimalPlaces( currencyPrecision );
        actionParamNumeric_->setValidator( new QIntValidator( currencyBottom, currencyTop, this ) );
        break;

    case Action_ApplyDiscount_PercentOff: // Percent Off Disc.
        showActionParamNumeric = true;
        actionParamNumeric_->setDecimal( currencySettings->unitSeparator() );
        actionParamNumeric_->setDecimalPlaces( 2 );
        actionParamNumeric_->setValidator( new QIntValidator( 0, 10000, this ) );
        break;

    case Action_LoadSpecials_First: // Load Specials
        showDisplayLocation = true;
        break;

    case Action_FPP_URL: // Production Planning
    case Action_Application_URL: // Store Applications
        showActionParam = true;
        showWebPageParameters = true;
        break;

    case Action_SwitchTotalType: // Change Total Type
        showTotalTypesAction = true;
        break;

    case Action_LoadTareSoftkey: // Preset tare
        tare = actionParam_->text();
        tareNum = actionParamNumeric_->text();

        // HT-193 If dual-range weigher and Tare Entry Precision is set to 2,
        // then need to divide the tare by 10.  Otherwise the value was in essence
        // getting multiplied by 10 every time you edited the Tare flashkey
        if( !scaleConfig->isSingleRangeWeigher() && ( 2 == scaleConfig->tarePrecision() ) )
        {
            double dblTare( actionParamNumeric_->text().toDouble() );
            dblTare /= 10.0;
            actionParamNumeric_->setText( QString::number( dblTare ) );
        }

        showActionParamNumeric = true;
        actionParamNumeric_->setDecimal( currencySettings->unitSeparator() );
        actionParamNumeric_->setDecimalPlaces( tarePrecision );
        actionParamNumeric_->setValidator( new QIntValidator( tareBottom, tareTop, this ) );
        hideEditCellWidgets = isNewAction; // user must enter tare first

        tare = actionParam_->text();
        tareNum = actionParamNumeric_->text();

        break;

    case Action_ChangeLabelPlacementAndPosition: // Label placement & rotation
        showLabelPlacementAndRotation = true;
        break;

     case Action_ChangeLabelType: // Change Label type
        showActionParam = true;
        actionParamValidator = new QIntValidator( 0, 999999, this );
        showPrinterSelection = true;
        break;

    case Action_LoadMostCommon_First: // Best Sellers
        showDisplayLocation = true;
        break;

    case Action_PlayVideo: // Play Video
        showPlayVideoAction = true;
        break;

    case Action_LaunchExternalApp: // Launch External App
        showLaunchExternalAppAction = true;
        break;
        
    case Action_Swipe:
        showSwipeAction = true;
        break;

    case Action_OverrideWrapperCameraDimensions: // Override Wrapper Camera Dimensions
        showTrayDimensions = true;
        break;
    case Action_ChangeDepartment:
        showChangeDepartmentAction = true;
        break;

    default:
        break;
    }

    // hide/show parameters
    actionParamLabel_->setVisible( showActionParam );
    actionParam_->setVisible( showActionParam );

    actionParamNumeric_->setVisible( showActionParamNumeric );

    screenChangeAction_->setVisible( showScreenChangeAction );

    playVideoAction_->setVisible( showPlayVideoAction );
    swipeAction_->setVisible( showSwipeAction );
    departments_->setVisible(showChangeDepartmentAction);

    totalTypesAction_->setVisible( showTotalTypesAction );

    launchExternalAppAction_->setVisible( showLaunchExternalAppAction );

    // hide/show fields
    if ( hideEditCellWidgets )
        editCellWidgets_->hide();

    else if ( mainWindow_->isOperateMode() )
    {
        QList<QWidget*> allWidgets( editCellWidgets_->findChildren<QWidget*>() );

        editCellWidgets_->show();

        // hide everything *except* the save button
        foreach ( QWidget *w, allWidgets )
            w->setVisible( cellEditDone_ == w );
    }

    else
    {
        editCellWidgets_->show();

        labelPlacement1Label_->setVisible( showLabelPlacementAndRotation );
        labelPlacement1_->setVisible( showLabelPlacementAndRotation );
        labelRotation1Label_->setVisible( showLabelPlacementAndRotation );
        labelRotation1_->setVisible( showLabelPlacementAndRotation );

        labelPlacement2Label_->setVisible( showLabelPlacementAndRotation && twoLabelAppliersAttached_ );
        labelPlacement2_->setVisible( showLabelPlacementAndRotation && twoLabelAppliersAttached_ );
        labelRotation2Label_->setVisible( showLabelPlacementAndRotation && twoLabelAppliersAttached_ );
        labelRotation2_->setVisible( showLabelPlacementAndRotation && twoLabelAppliersAttached_ );

        printerLabel_->setVisible( showPrinterSelection );
        printer_->setVisible( showPrinterSelection );

        displayLocationLabel_->setVisible( showDisplayLocation );
        displayLocation_->setVisible( showDisplayLocation );

        webPageParameters_->setVisible( showWebPageParameters );

        trayLengthLabel_->setVisible( showTrayDimensions );
        trayLength_->setVisible( showTrayDimensions );
        trayWidthLabel_->setVisible( showTrayDimensions );
        trayWidth_->setVisible( showTrayDimensions );
        trayLipHeightLabel_->setVisible( showTrayDimensions );
        trayLipHeight_->setVisible( showTrayDimensions );
    }

    // set proper label
    setActionParamLabelText();

    // install new validator
    actionParam_->setValidator( actionParamValidator );

    // ========================================================================
    // handle new action
    // ========================================================================

    if ( !isNewAction )
        return;

    bool processSetFocusActionParam = (( actionParam_->isVisible() ) || ( actionParamNumeric_->isVisible() ));

    switch ( action )
    {
    case Action_SelectProduct: // Select Product
        session_->send( Error, ErrSoftkeyDevice, "136012,5" ); // enter plu number
        processSetFocusActionParam = false;
        break;

    case Action_LoadCategory_First: // Load Category
        session_->send( Error, ErrSoftkeyDevice, "136013,5" ); // enter category
        processSetFocusActionParam = false;
        break;

    case Action_LoadTareSoftkey: // Preset tare
        session_->send( Error, ErrSoftkeyDevice, "136014,5" ); // enter tare value
        processSetFocusActionParam = false;
        break;

    default:
        break;
    }

    // clear param
    actionParam_->blockSignals( true );
    actionParam_->clear();
    actionParam_->blockSignals( false );

    actionParamNumeric_->blockSignals( true );
    actionParamNumeric_->clear();
    actionParamNumeric_->blockSignals( false );

    // set focus to param
    if ( processSetFocusActionParam )
        setFocusActionParam();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::readSoftkey( const QPoint& pos ) const
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::readSoftkey()";
#endif
    // (136,20)
    session_->send(
        SoftkeyDev,
        ReadSoftkey,
        currentFeature() + "," +
        currentGroup() + "," +
        QString::number( pos.x() ) + "," +
        QString::number( pos.y() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::writeSoftkey() const
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::writeSoftkey()";
#endif
    hobart::softkeyButton *flashkey( flashkeys_->selected() );

    // validate selected
    if ( !flashkey )
        return;

    QString text( actionParam_->text() );

    if ( actionParamNumeric_->isVisible() )
        text = actionParamNumeric_->text();

    session_->uiUtilities->youGottaResetTheSeparator( text );         // need to pass a decimal instead of a comma if we have one

    // validate entry
    if ( !text.length() )
        return;
    else if ( selected_.actionParam() == text )
        return;

    QPoint gridPos( flashkey->gridPos() );
    QSize gridSize( flashkey->gridSize() );
    QString action( action_->itemData( action_->currentIndex() ).toString() );

    // write softkey (136,22)
    session_->send(
        SoftkeyDev,
        WriteSoftkey,
        "," +
        currentGroup() + "," +
        QString::number( gridPos.x() ) + "," +
        QString::number( gridPos.y() ) + "," +
        QString::number( gridSize.width() ) + "," +
        QString::number( gridSize.height() ) + "," +
        action + "," +
        text );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::deleteSoftkey( const QPoint& pos ) const
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::deleteSoftkey()";
#endif
    // (136,24)
    session_->send(
        SoftkeyDev,
        DeleteSoftkey,
        currentFeature() + "," +
        currentGroup() + "," +
        QString::number( pos.x() ) + "," +
        QString::number( pos.y() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::saveSoftkey() const
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::saveSoftkey()";
#endif
    LOG_CLASS_INFO( "FlashkeyEditor", "saveSoftkey" );

    hobart::softkeyButton *flashkey( flashkeys_->selected() );
    MadpInternalConfig *internalConfig( &session_->scaleConfig->internalConfig );

    // validate selected
    if ( !flashkey )
        return;

    DomSoftkey softkey( selected_ );

    QString value;
    int params = 0;

    // find out if we need to refresh flashkeys for self serv
    if( hobart::softkeysWidget::SelfservProductGrid == currentFeature().toInt() ) /// PJR-383: Need to add FixedRowColumnGrid???
        internalConfig->updateSelfServiceFlashkeys = true;           // reload flashkeys in self service mode


    // save softkey details
    softkey.device() = QString::number( SoftkeyDev );
    softkey.event() = QString::number( SaveSoftkey );
    softkey.setHSize( flashkey->gridSize().width() );
    softkey.setVSize( flashkey->gridSize().height() );

    value = action_->itemData( action_->currentIndex() ).toString();
    softkey.setAction( value.toInt() );

    if( Action_ToggleExtraShelfLifeDay == softkey.action() )
        session_->scaleConfig->extraShelfLifeDayAvail = true;

    if ( Action_ChangeScreen == softkey.action() )
        softkey.actionParamRaw() = screenChangeAction_->itemData( screenChangeAction_->currentIndex() ).toString();
    else if ( Action_SwitchTotalType == softkey.action() )
        softkey.actionParamRaw() = totalTypesAction_->itemData( totalTypesAction_->currentIndex() ).toString();
    else if ( Action_PlayVideo == softkey.action() )
        softkey.actionParamRaw() = playVideoAction_->itemData( playVideoAction_->currentIndex() ).toString();
    else if ( Action_Swipe == softkey.action() )
        softkey.actionParamRaw() = swipeAction_->itemData( swipeAction_->currentIndex() ).toString();
    else if ( Action_LaunchExternalApp == softkey.action() )
        softkey.actionParamRaw() = launchExternalAppAction_->itemData( launchExternalAppAction_->currentIndex() ).toString();
    else if ( Action_ChangeDepartment == softkey.action() )
        softkey.actionParamRaw() = departments_->itemData( departments_->currentIndex() ).toString();
    else
    {
        QString text( actionParam_->text() );

        if ( actionParamNumeric_->isVisible() )
        {
            text = actionParamNumeric_->text();
        }

        softkey.actionParamRaw() = text;
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::saveSoftkey():  softkey.actionParamRaw() = [" << softkey.actionParamRaw() << "]";
#endif
    }

    if ( webPageUsesStoreNumber_->isChecked() )
        softkey.actionParamRaw() += QString( (params++ <= 0) ? "?" : "&" ) + QString( "store=" );
    if ( webPageUsesDepartment_->isChecked() )
        softkey.actionParamRaw() += QString( (params++ <= 0) ? "?" : "&" ) + QString( "dept=" );
    if ( webPageUsesPlu_->isChecked() )
        softkey.actionParamRaw() += QString( (params++ <= 0) ? "?" : "&" ) + QString( "plu=" );
    if ( webPageUsesQty_->isChecked() )
        softkey.actionParamRaw() += QString( (params++ <= 0) ? "?" : "&" ) + QString( "qty=" );
    if ( webPageUsesLang_->isChecked() )
        softkey.actionParamRaw() += QString( (params++ <= 0) ? "?" : "&" ) + QString( "lang=" );
    if ( webPageUsesModel_->isChecked() )
        softkey.actionParamRaw() += QString( (params++ <= 0) ? "?" : "&" ) + QString( "model=" );

    // NGW-26
    if ( Action_ChangeLabelPlacementAndPosition == softkey.action() )
    {
        int value = 0x1000000;
        value |= labelRotation1_->currentData().toString().toInt();
        value |= (labelPlacement1_->currentData().toString().toInt() << 4);
        value |= (labelRotation2_->currentData().toString().toInt() << 8);
        value |= (labelPlacement2_->currentData().toString().toInt() << 12);

        softkey.setActionParam( QString::number( value ) );
        softkey.setActionStrParam( QString::number( value )  );
    }
    else if ( Action_ChangeLabelType == softkey.action() )
    {
        value = printer_->itemData( printer_->currentIndex() ).toString() + "," + labelTypeNum_;

#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::saveSoftkey():  value = " << value;
#endif

        softkey.setActionStrParam( value );
        LOG_WARN( "softkey.setActionStrParam(%s)", qPrintable( value ) );
    }
    else if ( Action_OverrideWrapperCameraDimensions == softkey.action() )
    {
        value = QString::number( trayLength_->toInt() ) + "_" + QString::number( trayWidth_->toInt() ) + "_" + QString::number( trayLipHeight_->toInt() );
        softkey.setActionStrParam( value );
        softkey.actionParamRaw() = value;
    }
    else
    {
        softkey.setActionStrParam( "" );
    }

    // default
    if ( 0 == displayLocation_->currentIndex() )
        softkey.actionFeatureRaw() = "";
    else
    {
        value = displayLocation_->itemData( displayLocation_->currentIndex() ).toString();
        softkey.setActionFeature( value.toInt() );
    }

    if ( mainWindow_->isOperateMode() )
    {
        // default
        if ( 0 == operateBackgroundColor_->currentIndex() )
        {
            softkey.bgColorRaw() = "";
            softkey.setUseDefBgColor( true );

            softkey.setBgImage( "" );
            softkey.setUseDefBgImage( true );
        }
        // color
        else if ( flashkey->backgroundImage().isEmpty() )
        {
            value = flashkey->backgroundColor().name();
            softkey.bgColorRaw() = value.right( 6 ).toUpper();
            softkey.setUseDefBgColor( false );

            softkey.setBgImage( "" );
            softkey.setUseDefBgImage( false );
        }
        // image
        else
        {
            softkey.bgColorRaw() = "";
            softkey.setUseDefBgColor( false );

            softkey.bgImageRaw() = flashkey->backgroundImage();
            softkey.setUseDefBgImage( false );

            // PJR-383
            softkey.bgToggledColorRaw() = "";
            softkey.bgToggledImageRaw() = flashkey->toggledBackgroundImage();
        }
    }
    else
    {
        // default
        if ( 0 == backgroundColor_->currentIndex() )
        {
            softkey.bgColorRaw() = "";
            softkey.setUseDefBgColor( true );
        }
        else
        {
            value = flashkey->backgroundColor().name();
            softkey.bgColorRaw() = value.right( 6 ).toUpper();
            softkey.setUseDefBgColor( false );
        }

        // none
        if ( 0 == image_->currentIndex() )
        {
            softkey.setBgImage( "" );
            softkey.setUseDefBgImage( false );
        }
        // default
        else if ( 1 == image_->currentIndex() )
        {
            softkey.setBgImage( "" );
            softkey.setUseDefBgImage( true );
        }
        else
        {
            softkey.bgImageRaw() = flashkey->backgroundImage();
            softkey.setUseDefBgImage( false );

            // PJR-383
            softkey.bgToggledColorRaw() = "";
            softkey.bgToggledImageRaw() = flashkey->toggledBackgroundImage();
        }

        //none
        if ( 0 == pyScript_->currentIndex() )
            softkey.setScriptName( "" );
        else
            softkey.setScriptName(pyScript_->currentText());

        if ( 0 == pyScriptInterval_ )
            softkey.setScriptInterval( 0 );
        else
            softkey.setScriptInterval(pyScriptInterval_->toInt()*1000);
    }

    // default
    if ( 0 == textColor_->currentIndex() )
    {
        softkey.textColorRaw() = "";
        softkey.setUseDefTextColor( true );
    }
    else
    {
        value = flashkey->textColor().name();
        softkey.textColorRaw() = value.right( 6 ).toUpper();
        softkey.setUseDefTextColor( false );
    }

    if ( softkey.defName1() == primaryText_->text() )
    {
        softkey.setName1( "" );
        softkey.setUseDefName1( true );
    }
    else
    {
        softkey.setName1( primaryText_->text() );
        softkey.setUseDefName1( false );
    }

    if ( softkey.defName2() == secondaryText_->text() )
    {
        softkey.setName2( "" );
        softkey.setUseDefName2( true );
    }
    else
    {
        softkey.setName2( secondaryText_->text() );
        softkey.setUseDefName2( false );
    }

    // FT-1416: quote default softkey names
    QString defName1( softkey.defName1() );
    QString defName2( softkey.defName2() );

    softkey.setDefName1( defName1 );
    softkey.setDefName2( defName2 );

    // (136,26)
    session_->send( softkey );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::createNewSoftkey( const QPoint& pos ) const
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::createNewSoftkey()";
#endif
    // (136,32)
    session_->send(
        SoftkeyDev,
        CreateNewSoftkey,
        currentFeature() + "," +
        currentGroup() + "," +
        QString::number( pos.x() ) + "," +
        QString::number( pos.y() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::readSoftkeyFeature( const QString& feature ) const
{
    // (136,40)
    session_->send( SoftkeyDev, ReadSoftkeyFeature, feature );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::saveSoftkeyFeature() const
{
    QString dataFixedRowCol = fixedRowColumn_->itemData( fixedRowColumn_->currentIndex() ).toString();

    MadpInternalConfig *internalConfig( &session_->scaleConfig->internalConfig );
    if( currentFeature().toInt() == hobart::softkeysWidget::FixedRowColumnGrid )
        internalConfig->fixedFlashkeyDefaultGroup = defaultGroup_->text().toInt();

    // (136,46)
    session_->send( SoftkeyDev, SaveSoftkeyFeature, currentFeature() + "," + defaultGroup_->text() +  ",,,,,," + dataFixedRowCol +  "," + fixedRowColumnDefault_->text() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::readSoftkeyStyle( const QString& feature ) const
{
    // (136,60)
    session_->send( SoftkeyDev, ReadSoftkeyStyle, feature );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::saveSoftkeyStyle() const
{
    DomSoftkeyStyle softkeyStyle;

    // save style details
    softkeyStyle.device() = QString::number( SoftkeyDev );
    softkeyStyle.event() = QString::number( SaveSoftkeyStyle );
    softkeyStyle.setStyleNum( style_.styleNum() );
    softkeyStyle.setTextVAlign( flashkeys_->textAlignment() );

    // (136,66)
    session_->send( softkeyStyle );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::readSoftkeyGroup( const QString& group ) const
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::readSoftkeyGroup(" << group << ")";
#endif
    // (136,80)
    session_->send( SoftkeyDev, ReadSoftkeyGroup, group );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::deleteSoftkeyGroup( const QString& group ) const
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::deleteSoftkeyGroup(" << group << ")";
#endif
    // (136,84)
    if ( group.length() )
        session_->send( SoftkeyDev, DeleteSoftkeyGroup, group );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::saveSoftkeyGroup() const
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::saveSoftkeyGroup()";
#endif
    const QSize gridSize( flashkeys_->gridSize() );

    DomSoftkeyGroup softkeyGroup;

    // save grid details
    softkeyGroup.device() = QString::number( SoftkeyDev );
    softkeyGroup.event() = QString::number( SaveSoftkeyGroup );
    softkeyGroup.setGroupNum( flashkeys_->group() );
    softkeyGroup.setHorizGridSize( gridSize.width() );
    softkeyGroup.setVertGridSize( gridSize.height() );
    softkeyGroup.setHorizSpacing( flashkeys_->hSpacing() );
    softkeyGroup.setVertSpacing( flashkeys_->vSpacing() );

    // (136,86)
    session_->send( softkeyGroup );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::createNewSoftkeyGroup( const QString& group ) const
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::createNewSoftkeyGroup(" << group << ")";
#endif
    DomSoftkeyGroup softkeyGroup;

    // create grid details
    softkeyGroup.device() = QString::number( SoftkeyDev );
    softkeyGroup.event() = QString::number( CreateNewSoftkeyGroup );

	// Note:
    // It is possible (in a round about way) to delete the default group when default group is not
	// one of the 999XXX groups. In this situation we will create invalid groups since no feature
	// has been loaded.
    if ( group.isEmpty() )
    {
        softkeyGroup.groupNumRaw() = featureDefaultGroupNum_;
        softkeyGroup.setHorizGridSize( featureGridSize_.width() );
        softkeyGroup.setVertGridSize( featureGridSize_.height() );
        softkeyGroup.setHorizSpacing( featureHorizSpacing_ );
        softkeyGroup.setVertSpacing( featureVertSpacing_ );
    }
    else
    {
        const QSize gridSize( flashkeys_->gridSize() );

        softkeyGroup.groupNumRaw() = group;
        softkeyGroup.setHorizGridSize( gridSize.width() );
        softkeyGroup.setVertGridSize( gridSize.height() );
        softkeyGroup.setHorizSpacing( flashkeys_->hSpacing() );
        softkeyGroup.setVertSpacing( flashkeys_->vSpacing() );
    }

    // (136,92)
    session_->send( softkeyGroup );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::loadSoftkeyGroup( const QString& feature, const QString& group ) const
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::loadSoftkeyGroup(" << feature << ")" << group << ")";
#endif
    // (136,95)
    session_->send( SoftkeyDev, LoadSoftkeyGroupForEditor, feature + "," + group );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setComboBoxItemTextFromData( QComboBox *w, const QString& data, const QString& text )
{
#ifdef USE_QDEBUG
    qDebug() << "FlashkeyEditor::setComboBoxItemTextFromData(" << data << ")";
#endif
    int index;

    if ( 0 <= (index = w->findData( data )) )
        w->setItemText( index, text );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FlashkeyEditor::setComboBoxItemTextFromData( QComboBox *w, int data, const QString& text )
{
    setComboBoxItemTextFromData( w, QString::number( data ), text );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QColor FlashkeyEditor::makeColor( const QString& c )
{
    // hexadecimal string (i.e. 80FF80)
    if (( 6 <= c.length() ) && ( c.length() <= 7 ))
    {
        if ( "#" != c.left( 1 ) )
            return QColor( "#" + c );

        return QColor( c );
    }

    return QColor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString FlashkeyEditor::translate( const QString& lang, const QString& s )
{
    LOG_CLASS_INFO( "FlashkeyEditor", "translate" );

    QTranslator tran;
    bool result;

    LOG_DEBUG( "language '%s'", qPrintable( lang ) );

    // load proper translator
    if ( "esMX" == lang )
        result = tran.load( ":/languages/lang_es_mx" );
    else if ( "enCA" == lang )
        result = tran.load( ":/languages/lang_en_ca" );
    else if ( "frCA" == lang )
        result = tran.load( ":/languages/lang_fr_ca" );
    else
    {
        // nothing to do
        return s;
    }

    // check load result
    if ( !result )
    {
        LOG_WARN( "error loading translator for language '%s'", qPrintable( lang ) );
        return s;
    }

    // translate!
    return tran.translate( "FlashkeyEditor", qPrintable( s ) );
}
